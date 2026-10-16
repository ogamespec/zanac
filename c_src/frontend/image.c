/* image.c - PPM / WAV writers used by the test harness. */
#include <stdio.h>
#include <stdlib.h>

#include "frontend.h"
#include "neslib/neslib.h"

int frontend_write_ppm(const char *path, const uint32_t *framebuffer,
                       int width, int height)
{
    FILE *fp;
    int x, y;

    if (!framebuffer) return -1;
    fp = fopen(path, "wb");
    if (!fp) {
        fprintf(stderr, "zanac: cannot write %s\n", path);
        return -1;
    }
    fprintf(fp, "P6\n%d %d\n255\n", width, height);
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            uint32_t p = framebuffer[y * width + x];
            unsigned char rgb[3];
            rgb[0] = (unsigned char)((p >> 16) & 0xFF);
            rgb[1] = (unsigned char)((p >> 8) & 0xFF);
            rgb[2] = (unsigned char)(p & 0xFF);
            fwrite(rgb, 1, 3, fp);
        }
    }
    fclose(fp);
    return 0;
}

static void wav_u32(FILE *fp, uint32_t v)
{
    fputc(v & 0xFF, fp); fputc((v >> 8) & 0xFF, fp);
    fputc((v >> 16) & 0xFF, fp); fputc((v >> 24) & 0xFF, fp);
}

static void wav_u16(FILE *fp, uint16_t v)
{
    fputc(v & 0xFF, fp); fputc((v >> 8) & 0xFF, fp);
}

uint32_t frontend_frame_hash(const uint32_t *framebuffer)
{
    uint32_t h = 2166136261u;
    size_t i;
    if (!framebuffer) return 0;
    for (i = 0; i < (size_t)NESLIB_SCREEN_W * NESLIB_SCREEN_H; i++) {
        uint32_t v = framebuffer[i];
        int b;
        for (b = 0; b < 4; b++) {
            h ^= (v >> (b * 8)) & 0xFF;
            h *= 16777619u;
        }
    }
    return h;
}

int frontend_write_wav(const char *path)
{
    FILE *fp;
    float buffer[4096];
    long total = 0;
    long written = 0;
    int n;

    fp = fopen(path, "wb");
    if (!fp) {
        fprintf(stderr, "zanac: cannot write %s\n", path);
        return -1;
    }
    /* Placeholder header, patched after the samples are known. */
    fwrite("RIFF", 1, 4, fp);
    wav_u32(fp, 0);
    fwrite("WAVEfmt ", 1, 8, fp);
    wav_u32(fp, 16);
    wav_u16(fp, 1);                                   /* PCM */
    wav_u16(fp, 1);                                   /* mono */
    wav_u32(fp, (uint32_t)FRONTEND_SAMPLE_RATE);
    wav_u32(fp, (uint32_t)FRONTEND_SAMPLE_RATE * 2);
    wav_u16(fp, 2);
    wav_u16(fp, 16);
    fwrite("data", 1, 4, fp);
    wav_u32(fp, 0);

    while ((n = neslib_audio_read(buffer, 4096)) > 0) {
        int i;
        for (i = 0; i < n; i++) {
            int v = (int)(buffer[i] * 32767.0f);
            if (v > 32767) v = 32767;
            if (v < -32768) v = -32768;
            wav_u16(fp, (uint16_t)(int16_t)v);
        }
        total += n;
        written += (long)n * 2;
    }

    fseek(fp, 4, SEEK_SET);
    wav_u32(fp, (uint32_t)(36 + written));
    fseek(fp, 40, SEEK_SET);
    wav_u32(fp, (uint32_t)written);
    fclose(fp);
    fprintf(stderr, "zanac: %ld audio samples written to %s\n", total, path);
    return 0;
}

/* Diagnostic dump of the PPU memory the ported game produced, for the
 * comparison against the original ROM: the 8 KB pattern tables (CHR), the
 * 2 KB of name-table RAM (CIRAM), the palette and the primary OAM.
 * `prefix` is used as-is; the files get the .chr/.nt/.pal/.oam suffixes. */
#include "neslib/core/nes.h"
#include "neslib/core/ppu.h"
#include "neslib/core/cart.h"

int frontend_dump_nes(Nes *nes, const char *prefix)
{
    char path[512];
    FILE *fp;
    int i;

    if (!nes) return -1;
    snprintf(path, sizeof(path), "%s.chr", prefix);
    fp = fopen(path, "wb");
    if (fp) { fwrite(nes->cart->chr, 1, nes->cart->chr_size, fp); fclose(fp); }
    snprintf(path, sizeof(path), "%s.nt", prefix);
    fp = fopen(path, "wb");
    if (fp) { fwrite(nes->ppu->ciram, 1, sizeof(nes->ppu->ciram), fp); fclose(fp); }
    snprintf(path, sizeof(path), "%s.pal", prefix);
    fp = fopen(path, "wb");
    if (fp) { fwrite(nes->ppu->pal, 1, sizeof(nes->ppu->pal), fp); fclose(fp); }
    snprintf(path, sizeof(path), "%s.oam", prefix);
    fp = fopen(path, "wb");
    if (fp) { fwrite(nes->ppu->oam, 1, sizeof(nes->ppu->oam), fp); fclose(fp); }
    (void)i;
    return 0;
}

int frontend_dump_ram(const uint8_t *ram, const char *path)
{
    FILE *fp = fopen(path, "wb");
    if (!fp || !ram) { if (fp) fclose(fp); return -1; }
    fwrite(ram, 1, 2048, fp);
    fclose(fp);
    return 0;
}

/* The port's engine handle (the reference runner calls frontend_dump_nes()
 * with its own Nes). */
int frontend_dump_ppu(const char *prefix)
{
    return frontend_dump_nes((Nes *)neslib_debug_nes(), prefix);
}
