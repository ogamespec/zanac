/* savestate.c - save states of the Zanac C port.
 *
 * Why this lives in the game directory
 * ------------------------------------
 * A complete state is not only the engine's (PPU/APU/cartridge/RAM) but also
 * the ported 6502 programmer's model that lives in zanac_ram.c (A/X/Y/SP, the
 * flags, the UxROM bank latch) and the game's resume point.  Keeping the whole
 * snapshot here lets one function write and read both halves through the same
 * versioned container, instead of teaching neslib about the game.  The engine
 * itself is reached through neslib_debug_nes(), which is the handle the
 * diagnostic dumps already use.
 *
 * File format (all integers little-endian, so a save can move between the gcc
 * and MSVC builds of the port):
 *
 *   offset  size  field
 *   0       8     magic "ZANACST\0"
 *   8       4     container version (ZANAC_STATE_FILE_VERSION)
 *   12      4     payload size in bytes
 *   16      4     CRC-32 (IEEE) of the payload
 *   20      ...   payload:
 *                   u32 payload version
 *                   u32 engine version
 *                   u64 host frame number (the frontend's s_frames)
 *                   u64 neslib PPU frame counter
 *                   u8  resume kind (ZANAC_RESUME_*)
 *                   u8  game_mode
 *                   u8  current_level
 *                   u8  bank latch
 *                   u8  mapper, u8 submapper, u8 chr_is_ram, u8 mirroring
 *                   u32 chr size
 *                   ... engine state (Nes, Ppu, Apu, Cart + CHR RAM)
 *                   ... ported-game state (registers, flags, bank, ...)
 *
 * The format is deliberately explicit (one write per field) rather than a
 * memcpy of the C structs: save files then survive a different compiler,
 * padding or pointer layout, and the layout is readable in this file.  A
 * change to any field list must bump ZANAC_STATE_FILE_VERSION (or the payload /
 * engine version), so an old file is rejected instead of being misread.
 *
 * What is deliberately not in the file: the C call stack (impossible to
 * serialise portably), the SDL window/audio device, and the APU's decimated
 * audio ring (samples from the abandoned run must not be played).  The game's
 * per-frame loops are restartable because the port keeps all game state in RAM
 * and in the registers above, so the loader resumes at the recorded loop: the
 * level loop (loc_E880), the pause loop (loc_D3B9), the title/demo menu loop
 * (loc_E080) or the GAME START/CONTINUE selector (loc_E190).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zanac.h"
#include "savestate.h"

#include "core/nes.h"
#include "core/ppu.h"
#include "core/apu.h"
#include "core/cart.h"

#define ZANAC_STATE_MAGIC          "ZANACST"
#define ZANAC_STATE_MAGIC_SIZE     8u
#define ZANAC_STATE_HEADER_SIZE    20u
#define ZANAC_STATE_FILE_VERSION   1u
#define ZANAC_STATE_PAYLOAD_VERSION 2u
#define ZANAC_STATE_ENGINE_VERSION  1u

static ZanacStateCheckpointHook s_checkpoint_hook;
static int s_loaded_canonical = 1;

/* ------------------------------------------------------------------------ *
 *  Little-endian buffer writer / reader
 * ------------------------------------------------------------------------ */
typedef struct {
    uint8_t *data;
    size_t   len;
    size_t   cap;
    int      err;
} Writer;

typedef struct {
    const uint8_t *data;
    size_t         len;
    size_t         pos;
    int            err;
} Reader;

static void w_reserve(Writer *w, size_t n)
{
    size_t need, cap;
    uint8_t *p;

    if (w->err) return;
    if (n > SIZE_MAX - w->len) { w->err = 1; return; }
    need = w->len + n;
    if (need <= w->cap) return;

    cap = w->cap ? w->cap : 4096;
    while (cap < need) {
        if (cap > SIZE_MAX / 2) { cap = need; break; }
        cap *= 2;
    }
    p = (uint8_t *)realloc(w->data, cap);
    if (!p) { w->err = 1; return; }
    w->data = p;
    w->cap = cap;
}

static void w_bytes(Writer *w, const void *p, size_t n)
{
    w_reserve(w, n);
    if (w->err) return;
    memcpy(w->data + w->len, p, n);
    w->len += n;
}

static void w_u8(Writer *w, uint8_t v) { w_bytes(w, &v, 1); }

static void w_u16(Writer *w, uint16_t v)
{
    uint8_t b[2];
    b[0] = (uint8_t)(v & 0xFF);
    b[1] = (uint8_t)(v >> 8);
    w_bytes(w, b, sizeof(b));
}

static void w_u32(Writer *w, uint32_t v)
{
    uint8_t b[4];
    b[0] = (uint8_t)(v & 0xFF);
    b[1] = (uint8_t)((v >> 8) & 0xFF);
    b[2] = (uint8_t)((v >> 16) & 0xFF);
    b[3] = (uint8_t)((v >> 24) & 0xFF);
    w_bytes(w, b, sizeof(b));
}

static void w_u64(Writer *w, uint64_t v)
{
    uint8_t b[8];
    int i;
    for (i = 0; i < 8; i++)
        b[i] = (uint8_t)((v >> (8 * i)) & 0xFF);
    w_bytes(w, b, sizeof(b));
}

static void w_bool(Writer *w, bool v) { w_u8(w, v ? 1u : 0u); }
static void w_i32(Writer *w, int v) { w_u32(w, (uint32_t)(int32_t)v); }

static void r_bytes(Reader *r, void *p, size_t n)
{
    if (r->err) return;
    if (n > r->len - r->pos) { r->err = 1; memset(p, 0, n); return; }
    memcpy(p, r->data + r->pos, n);
    r->pos += n;
}

static uint8_t r_u8(Reader *r)
{
    uint8_t v = 0;
    r_bytes(r, &v, 1);
    return v;
}

static uint16_t r_u16(Reader *r)
{
    uint8_t b[2] = { 0, 0 };
    r_bytes(r, b, sizeof(b));
    return (uint16_t)(b[0] | ((uint16_t)b[1] << 8));
}

static uint32_t r_u32(Reader *r)
{
    uint8_t b[4] = { 0, 0, 0, 0 };
    r_bytes(r, b, sizeof(b));
    return (uint32_t)b[0] | ((uint32_t)b[1] << 8) |
           ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
}

static uint64_t r_u64(Reader *r)
{
    uint8_t b[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    uint64_t v = 0;
    int i;
    r_bytes(r, b, sizeof(b));
    for (i = 7; i >= 0; i--)
        v = (v << 8) | b[i];
    return v;
}

static bool r_bool(Reader *r) { return r_u8(r) != 0; }
static int r_i32(Reader *r) { return (int)(int32_t)r_u32(r); }

/* ------------------------------------------------------------------------ *
 *  Small helpers
 * ------------------------------------------------------------------------ */
static uint32_t crc32_ieee(const uint8_t *p, size_t n)
{
    uint32_t crc = 0xFFFFFFFFu;
    size_t i;
    int k;
    for (i = 0; i < n; i++) {
        crc ^= p[i];
        for (k = 0; k < 8; k++)
            crc = (crc >> 1) ^ (0xEDB88320u & (uint32_t)-(int32_t)(crc & 1u));
    }
    return ~crc;
}

static void put_u32le(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v & 0xFF);
    p[1] = (uint8_t)((v >> 8) & 0xFF);
    p[2] = (uint8_t)((v >> 16) & 0xFF);
    p[3] = (uint8_t)((v >> 24) & 0xFF);
}

static uint32_t get_u32le(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static uint8_t *read_whole_file(const char *path, size_t *size)
{
    FILE *fp;
    long end;
    uint8_t *buf;
    size_t n;

    *size = 0;
    fp = fopen(path, "rb");
    if (!fp) return NULL;
    if (fseek(fp, 0, SEEK_END) != 0) { fclose(fp); return NULL; }
    end = ftell(fp);
    if (end < 0) { fclose(fp); return NULL; }
    if (fseek(fp, 0, SEEK_SET) != 0) { fclose(fp); return NULL; }
    n = (size_t)end;
    buf = (uint8_t *)malloc(n ? n : 1);
    if (!buf) { fclose(fp); return NULL; }
    if (n && fread(buf, 1, n, fp) != n) {
        free(buf);
        fclose(fp);
        return NULL;
    }
    fclose(fp);
    *size = n;
    return buf;
}

/* ------------------------------------------------------------------------ *
 *  Engine serialisation
 *
 *  The field lists below mirror the structs in the neslib/core headers one for one.
 *  When a field is added there it must be added here and the file version
 *  bumped - a save that silently skipped state would reproduce a different
 *  frame after loading, which is exactly what this feature is for.
 * ------------------------------------------------------------------------ */
static void ser_nes(Writer *w, const Nes *nes)
{
    w_u8(w, (uint8_t)nes->region);
    w_bool(w, nes->ppu_rev.oam_readable);
    w_bool(w, nes->ppu_rev.palette_readable);
    w_bool(w, nes->ppu_rev.oamaddr_corruption);

    w_u64(w, nes->cpu_cycles);
    w_u64(w, nes->frame_count);
    w_bool(w, nes->reset_pending);

    w_u8(w, nes->cpu_open_bus);
    w_u8(w, nes->ext_bus);
    w_u64(w, nes->buslog_from);
    w_u64(w, nes->buslog_to);
    w_u32(w, nes->power_on_seed);

    w_bytes(w, nes->ram, sizeof(nes->ram));

    w_bool(w, nes->dma_get_parity);
    w_bool(w, nes->cpu_halted);
    w_u16(w, nes->cpu_halt_addr);

    w_bool(w, nes->oam_dma_pending);
    w_bool(w, nes->oam_dma_active);
    w_u8(w, nes->oam_dma_page);
    w_u16(w, nes->oam_dma_pos);
    w_u8(w, nes->oam_dma_want);
    w_u8(w, nes->oam_dma_buffer);

    w_i32(w, nes->dmc_phase);
    w_bool(w, nes->dmc_attempt);
    w_bool(w, nes->dmc_halt_on_get);
    w_i32(w, nes->dmc_load_delay);
    w_u16(w, nes->dmc_dma_addr);
    w_u8(w, nes->dmc_dma_value);
    w_u64(w, nes->dmc_last_cycle);
    w_u64(w, nes->dmc_target_cycle);
    w_u64(w, nes->dmc_abort_cycle);
    w_bool(w, nes->dmc_abort_hold);
    w_i32(w, nes->dmc_refetch_guard);

    w_bytes(w, nes->input.buttons, sizeof(nes->input.buttons));
    w_bytes(w, nes->input.shift, sizeof(nes->input.shift));
    w_u8(w, nes->input.strobe);
    w_bytes(w, nes->input.read_count, sizeof(nes->input.read_count));
}

static bool deser_nes(Reader *r, Nes *nes)
{
    nes->region = (NesRegion)r_u8(r);
    nes->ppu_rev.oam_readable = r_bool(r);
    nes->ppu_rev.palette_readable = r_bool(r);
    nes->ppu_rev.oamaddr_corruption = r_bool(r);

    nes->cpu_cycles = r_u64(r);
    nes->frame_count = r_u64(r);
    nes->reset_pending = r_bool(r);

    nes->cpu_open_bus = r_u8(r);
    nes->ext_bus = r_u8(r);
    nes->buslog_from = r_u64(r);
    nes->buslog_to = r_u64(r);
    nes->power_on_seed = r_u32(r);

    r_bytes(r, nes->ram, sizeof(nes->ram));

    nes->dma_get_parity = r_bool(r);
    nes->cpu_halted = r_bool(r);
    nes->cpu_halt_addr = r_u16(r);

    nes->oam_dma_pending = r_bool(r);
    nes->oam_dma_active = r_bool(r);
    nes->oam_dma_page = r_u8(r);
    nes->oam_dma_pos = r_u16(r);
    nes->oam_dma_want = r_u8(r);
    nes->oam_dma_buffer = r_u8(r);

    nes->dmc_phase = r_i32(r);
    nes->dmc_attempt = r_bool(r);
    nes->dmc_halt_on_get = r_bool(r);
    nes->dmc_load_delay = r_i32(r);
    nes->dmc_dma_addr = r_u16(r);
    nes->dmc_dma_value = r_u8(r);
    nes->dmc_last_cycle = r_u64(r);
    nes->dmc_target_cycle = r_u64(r);
    nes->dmc_abort_cycle = r_u64(r);
    nes->dmc_abort_hold = r_bool(r);
    nes->dmc_refetch_guard = r_i32(r);

    r_bytes(r, nes->input.buttons, sizeof(nes->input.buttons));
    r_bytes(r, nes->input.shift, sizeof(nes->input.shift));
    nes->input.strobe = r_u8(r);
    r_bytes(r, nes->input.read_count, sizeof(nes->input.read_count));

    return r->err == 0;
}

static void ser_ppu(Writer *w, const Ppu *p)
{
    int i, y, x;

    w_u8(w, p->ctrl);
    w_u8(w, p->mask);
    w_u8(w, p->mask_pending);
    w_bytes(w, p->addr_q, sizeof(p->addr_q));
    for (i = 0; i < 4; i++) w_i32(w, p->addr_q_delay[i]);
    w_i32(w, p->addr_qn);
    w_i32(w, p->mask_pending_delay);
    w_u8(w, p->status);
    w_bool(w, p->vblank_read_suppress);
    w_bool(w, p->vblank_set_in_cycle);
    w_bool(w, p->int_ff);
    w_i32(w, p->int_delay);
    w_u8(w, p->oam_addr);
    w_u8(w, p->read_buffer);
    w_u16(w, p->v);
    w_u16(w, p->t);
    w_u8(w, p->fine_x);
    w_u8(w, p->w);
    w_u8(w, p->ppu_bus);
    w_bytes(w, p->bus_decay, sizeof(p->bus_decay));
    w_i32(w, p->line);
    w_i32(w, p->dot);
    w_bool(w, p->frame_odd);
    w_bool(w, p->odd_frame_skip);
    w_u64(w, p->dot_count);
    w_bool(w, p->nmi_out);
    w_bool(w, p->nmi_prev);
    w_bool(w, p->rc_active);
    w_u64(w, p->rc_until_cpu_cycle);

    w_bytes(w, p->ciram, sizeof(p->ciram));
    w_bytes(w, p->pal, sizeof(p->pal));
    w_bytes(w, p->oam, sizeof(p->oam));
    w_bytes(w, p->oam2, sizeof(p->oam2));
    w_u8(w, p->oam2_addr);
    w_u8(w, p->oam2_seed);
    w_u8(w, p->oam2_data);
    w_u8(w, p->oam_read);

    w_u16(w, p->bg_sr_lo);
    w_u16(w, p->bg_sr_hi);
    w_u16(w, p->bg_at_lo);
    w_u16(w, p->bg_at_hi);
    w_u8(w, p->nt_latch);
    w_u8(w, p->latch);
    w_u8(w, p->addr_low);
    w_u8(w, p->spr_latch);
    w_u8(w, p->spr_latch0);
    w_i32(w, p->cpu_read_pending);
    w_i32(w, p->inc_pending);
    w_i32(w, p->cpu_read_active);
    w_u8(w, p->at_latch);
    w_u8(w, p->at_quadrant);
    w_u8(w, p->pat_lo);
    w_bool(w, p->bg_frozen);
    w_u8(w, p->fetch_phase);

    w_u8(w, p->spr_count);
    w_u8(w, p->spr_eval_n);
    w_u8(w, p->spr_eval_b);
    w_u8(w, p->spr_eval_m);
    w_u8(w, p->spr_eval_found);
    w_u8(w, p->spr_eval_state);
    w_bool(w, p->sprite0_on_line);
    w_u8(w, p->eval_start);
    w_u8(w, p->oam_readbuf);
    w_u8(w, p->oam2_reg);
    w_bool(w, p->oam2_inc_frozen);
    w_bool(w, p->spr_shift_hpos);

    w_bytes(w, p->spr_lane_x, sizeof(p->spr_lane_x));
    w_bytes(w, p->spr_lane_cnt, sizeof(p->spr_lane_cnt));
    w_bytes(w, p->spr_lane_attr, sizeof(p->spr_lane_attr));
    w_bytes(w, p->spr_lane_pat, sizeof(p->spr_lane_pat));
    w_bytes(w, p->spr_lane_armed, sizeof(p->spr_lane_armed));
    w_bytes(w, p->spr_lane_done, sizeof(p->spr_lane_done));
    w_bytes(w, p->spr_lane_shifted, sizeof(p->spr_lane_shifted));
    w_bytes(w, p->spr_lane_flip, sizeof(p->spr_lane_flip));
    w_bytes(w, p->spr_lane_is0, sizeof(p->spr_lane_is0));

    w_u8(w, p->pixel);
    for (y = 0; y < NES_SCREEN_H; y++)
        for (x = 0; x < NES_SCREEN_W; x++)
            w_u32(w, p->framebuffer[y][x]);

    w_bool(w, p->rendering);
    w_bool(w, p->prev_rendering);
    w_bool(w, p->oam_corrupt);
    w_i32(w, p->corrupt_seed_delay);
    w_bool(w, p->oam_corrupt_pending);
    w_bool(w, p->emph_r);
    w_bool(w, p->emph_g);
    w_bool(w, p->emph_b);
    w_bool(w, p->greyscale);
    w_u64(w, p->frame_dots);
}

static bool deser_ppu(Reader *r, Ppu *p)
{
    int i, y, x;

    p->ctrl = r_u8(r);
    p->mask = r_u8(r);
    p->mask_pending = r_u8(r);
    r_bytes(r, p->addr_q, sizeof(p->addr_q));
    for (i = 0; i < 4; i++) p->addr_q_delay[i] = r_i32(r);
    p->addr_qn = r_i32(r);
    p->mask_pending_delay = r_i32(r);
    p->status = r_u8(r);
    p->vblank_read_suppress = r_bool(r);
    p->vblank_set_in_cycle = r_bool(r);
    p->int_ff = r_bool(r);
    p->int_delay = r_i32(r);
    p->oam_addr = r_u8(r);
    p->read_buffer = r_u8(r);
    p->v = r_u16(r);
    p->t = r_u16(r);
    p->fine_x = r_u8(r);
    p->w = r_u8(r);
    p->ppu_bus = r_u8(r);
    r_bytes(r, p->bus_decay, sizeof(p->bus_decay));
    p->line = r_i32(r);
    p->dot = r_i32(r);
    p->frame_odd = r_bool(r);
    p->odd_frame_skip = r_bool(r);
    p->dot_count = r_u64(r);
    p->nmi_out = r_bool(r);
    p->nmi_prev = r_bool(r);
    p->rc_active = r_bool(r);
    p->rc_until_cpu_cycle = r_u64(r);

    r_bytes(r, p->ciram, sizeof(p->ciram));
    r_bytes(r, p->pal, sizeof(p->pal));
    r_bytes(r, p->oam, sizeof(p->oam));
    r_bytes(r, p->oam2, sizeof(p->oam2));
    p->oam2_addr = r_u8(r);
    p->oam2_seed = r_u8(r);
    p->oam2_data = r_u8(r);
    p->oam_read = r_u8(r);

    p->bg_sr_lo = r_u16(r);
    p->bg_sr_hi = r_u16(r);
    p->bg_at_lo = r_u16(r);
    p->bg_at_hi = r_u16(r);
    p->nt_latch = r_u8(r);
    p->latch = r_u8(r);
    p->addr_low = r_u8(r);
    p->spr_latch = r_u8(r);
    p->spr_latch0 = r_u8(r);
    p->cpu_read_pending = r_i32(r);
    p->inc_pending = r_i32(r);
    p->cpu_read_active = r_i32(r);
    p->at_latch = r_u8(r);
    p->at_quadrant = r_u8(r);
    p->pat_lo = r_u8(r);
    p->bg_frozen = r_bool(r);
    p->fetch_phase = r_u8(r);

    p->spr_count = r_u8(r);
    p->spr_eval_n = r_u8(r);
    p->spr_eval_b = r_u8(r);
    p->spr_eval_m = r_u8(r);
    p->spr_eval_found = r_u8(r);
    p->spr_eval_state = r_u8(r);
    p->sprite0_on_line = r_bool(r);
    p->eval_start = r_u8(r);
    p->oam_readbuf = r_u8(r);
    p->oam2_reg = r_u8(r);
    p->oam2_inc_frozen = r_bool(r);
    p->spr_shift_hpos = r_bool(r);

    r_bytes(r, p->spr_lane_x, sizeof(p->spr_lane_x));
    r_bytes(r, p->spr_lane_cnt, sizeof(p->spr_lane_cnt));
    r_bytes(r, p->spr_lane_attr, sizeof(p->spr_lane_attr));
    r_bytes(r, p->spr_lane_pat, sizeof(p->spr_lane_pat));
    r_bytes(r, p->spr_lane_armed, sizeof(p->spr_lane_armed));
    r_bytes(r, p->spr_lane_done, sizeof(p->spr_lane_done));
    r_bytes(r, p->spr_lane_shifted, sizeof(p->spr_lane_shifted));
    r_bytes(r, p->spr_lane_flip, sizeof(p->spr_lane_flip));
    r_bytes(r, p->spr_lane_is0, sizeof(p->spr_lane_is0));

    p->pixel = r_u8(r);
    for (y = 0; y < NES_SCREEN_H; y++)
        for (x = 0; x < NES_SCREEN_W; x++)
            p->framebuffer[y][x] = r_u32(r);

    p->rendering = r_bool(r);
    p->prev_rendering = r_bool(r);
    p->oam_corrupt = r_bool(r);
    p->corrupt_seed_delay = r_i32(r);
    p->oam_corrupt_pending = r_bool(r);
    p->emph_r = r_bool(r);
    p->emph_g = r_bool(r);
    p->emph_b = r_bool(r);
    p->greyscale = r_bool(r);
    p->frame_dots = r_u64(r);

    return r->err == 0;
}

static void ser_apu(Writer *w, const Apu *a)
{
    int i;

    for (i = 0; i < 2; i++) {
        const ApuPulse *p = &a->pulse[i];
        w_bool(w, p->enabled);
        w_u8(w, p->duty);
        w_bool(w, p->halt);
        w_bool(w, p->constant_volume);
        w_u8(w, p->volume);
        w_u16(w, p->timer);
        w_u16(w, p->timer_counter);
        w_u8(w, p->duty_counter);
        w_u8(w, p->length);
        w_bool(w, p->env_start);
        w_u8(w, p->env_divider);
        w_u8(w, p->env_decay);
        w_bool(w, p->sweep_enable);
        w_u8(w, p->sweep_period);
        w_bool(w, p->sweep_negate);
        w_u8(w, p->sweep_shift);
        w_u8(w, p->sweep_divider);
        w_bool(w, p->sweep_reload);
        w_u8(w, p->output);
    }

    w_bool(w, a->triangle.enabled);
    w_bool(w, a->triangle.control);
    w_u8(w, a->triangle.linear_reload);
    w_bool(w, a->triangle.reload_flag);
    w_u8(w, a->triangle.linear_counter);
    w_u16(w, a->triangle.timer);
    w_u16(w, a->triangle.timer_counter);
    w_u8(w, a->triangle.length);
    w_u8(w, a->triangle.seq);
    w_u8(w, a->triangle.output);

    w_bool(w, a->noise.enabled);
    w_bool(w, a->noise.halt);
    w_bool(w, a->noise.constant_volume);
    w_u8(w, a->noise.volume);
    w_u8(w, a->noise.period_index);
    w_bool(w, a->noise.mode);
    w_u16(w, a->noise.timer_counter);
    w_u16(w, a->noise.lfsr);
    w_u8(w, a->noise.length);
    w_bool(w, a->noise.env_start);
    w_u8(w, a->noise.env_divider);
    w_u8(w, a->noise.env_decay);
    w_u8(w, a->noise.output);

    w_bool(w, a->dmc.enabled);
    w_bool(w, a->dmc.irq_enable);
    w_bool(w, a->dmc.loop);
    w_u8(w, a->dmc.rate_index);
    w_u16(w, a->dmc.rate_counter);
    w_u16(w, a->dmc.sample_addr);
    w_u16(w, a->dmc.sample_len);
    w_u8(w, a->dmc.sample_buffer);
    w_bool(w, a->dmc.buffer_full);
    w_u8(w, a->dmc.bits_remaining);
    w_u8(w, a->dmc.shift_reg);
    w_u8(w, a->dmc.output);
    w_bool(w, a->dmc.irq_flag);
    w_bool(w, a->dmc.start_pending);
    w_bool(w, a->dmc.silence);

    w_u8(w, a->status);
    w_bool(w, a->frame_irq);
    w_bool(w, a->frame_irq_line);
    w_bool(w, a->frame_irq_clear_pending);
    w_bool(w, a->frame_irq_inhibit);
    w_u8(w, a->frame_mode);
    w_i32(w, a->frame_event);
    w_i32(w, a->frame_cycles);
    w_i32(w, a->frame_reset_count);
    for (i = 0; i < (int)(sizeof(a->frame_reset) / sizeof(a->frame_reset[0])); i++) {
        w_i32(w, a->frame_reset[i].delay);
        w_u8(w, a->frame_reset[i].mode);
    }
    w_bool(w, a->dmc_stopped);
    w_u64(w, a->dmc_stop_cycle);
    w_i32(w, a->dmc_stop_eta);
    w_u64(w, a->apu_cycles);
    w_bool(w, a->phase);
    w_u8(w, a->out_latch);
    w_bool(w, a->pad_out);
    w_bool(w, a->lock);
    w_u16(w, a->dmc_sample_base);
    w_u16(w, a->dmc_sample_len);
}

static bool deser_apu(Reader *r, Apu *a)
{
    int i;

    for (i = 0; i < 2; i++) {
        ApuPulse *p = &a->pulse[i];
        p->enabled = r_bool(r);
        p->duty = r_u8(r);
        p->halt = r_bool(r);
        p->constant_volume = r_bool(r);
        p->volume = r_u8(r);
        p->timer = r_u16(r);
        p->timer_counter = r_u16(r);
        p->duty_counter = r_u8(r);
        p->length = r_u8(r);
        p->env_start = r_bool(r);
        p->env_divider = r_u8(r);
        p->env_decay = r_u8(r);
        p->sweep_enable = r_bool(r);
        p->sweep_period = r_u8(r);
        p->sweep_negate = r_bool(r);
        p->sweep_shift = r_u8(r);
        p->sweep_divider = r_u8(r);
        p->sweep_reload = r_bool(r);
        p->output = r_u8(r);
    }

    a->triangle.enabled = r_bool(r);
    a->triangle.control = r_bool(r);
    a->triangle.linear_reload = r_u8(r);
    a->triangle.reload_flag = r_bool(r);
    a->triangle.linear_counter = r_u8(r);
    a->triangle.timer = r_u16(r);
    a->triangle.timer_counter = r_u16(r);
    a->triangle.length = r_u8(r);
    a->triangle.seq = r_u8(r);
    a->triangle.output = r_u8(r);

    a->noise.enabled = r_bool(r);
    a->noise.halt = r_bool(r);
    a->noise.constant_volume = r_bool(r);
    a->noise.volume = r_u8(r);
    a->noise.period_index = r_u8(r);
    a->noise.mode = r_bool(r);
    a->noise.timer_counter = r_u16(r);
    a->noise.lfsr = r_u16(r);
    a->noise.length = r_u8(r);
    a->noise.env_start = r_bool(r);
    a->noise.env_divider = r_u8(r);
    a->noise.env_decay = r_u8(r);
    a->noise.output = r_u8(r);

    a->dmc.enabled = r_bool(r);
    a->dmc.irq_enable = r_bool(r);
    a->dmc.loop = r_bool(r);
    a->dmc.rate_index = r_u8(r);
    a->dmc.rate_counter = r_u16(r);
    a->dmc.sample_addr = r_u16(r);
    a->dmc.sample_len = r_u16(r);
    a->dmc.sample_buffer = r_u8(r);
    a->dmc.buffer_full = r_bool(r);
    a->dmc.bits_remaining = r_u8(r);
    a->dmc.shift_reg = r_u8(r);
    a->dmc.output = r_u8(r);
    a->dmc.irq_flag = r_bool(r);
    a->dmc.start_pending = r_bool(r);
    a->dmc.silence = r_bool(r);

    a->status = r_u8(r);
    a->frame_irq = r_bool(r);
    a->frame_irq_line = r_bool(r);
    a->frame_irq_clear_pending = r_bool(r);
    a->frame_irq_inhibit = r_bool(r);
    a->frame_mode = r_u8(r);
    a->frame_event = r_i32(r);
    a->frame_cycles = r_i32(r);
    a->frame_reset_count = r_i32(r);
    for (i = 0; i < (int)(sizeof(a->frame_reset) / sizeof(a->frame_reset[0])); i++) {
        a->frame_reset[i].delay = r_i32(r);
        a->frame_reset[i].mode = r_u8(r);
    }
    a->dmc_stopped = r_bool(r);
    a->dmc_stop_cycle = r_u64(r);
    a->dmc_stop_eta = r_i32(r);
    a->apu_cycles = r_u64(r);
    a->phase = r_bool(r);
    a->out_latch = r_u8(r);
    a->pad_out = r_bool(r);
    a->lock = r_bool(r);
    a->dmc_sample_base = r_u16(r);
    a->dmc_sample_len = r_u16(r);

    if (r->err) return false;
    if (a->frame_reset_count < 0 ||
        a->frame_reset_count > (int)(sizeof(a->frame_reset) / sizeof(a->frame_reset[0])))
        return false;
    return true;
}

static void ser_cart(Writer *w, const Cart *c)
{
    int i;

    w_u8(w, c->mapper);
    w_u8(w, c->submapper);
    w_u8(w, c->prg_banks_16k);
    w_u8(w, c->chr_banks_8k);
    w_bool(w, c->battery);
    w_bool(w, c->four_screen);
    w_bool(w, c->nes2);
    w_u8(w, (uint8_t)c->mirroring);
    w_u32(w, c->prg_size);
    w_u32(w, c->chr_size);
    w_bool(w, c->chr_ram);
    w_bytes(w, c->prg_ram, sizeof(c->prg_ram));
    w_u32(w, c->prg_bank_lo);
    w_u32(w, c->prg_bank_hi);
    for (i = 0; i < 8; i++) w_u32(w, c->chr_bank[i]);
    w_bytes(w, c->reg, sizeof(c->reg));
    w_u8(w, c->shift);
    w_u8(w, c->shift_count);
    w_u8(w, c->irq_latch);
    w_u8(w, c->irq_counter);
    w_u8(w, c->irq_reload);
    w_bool(w, c->irq_enable);
    w_bool(w, c->irq_pending);
    w_bool(w, c->a12_high);
    w_u8(w, c->a12_low_dots);
    w_u8(w, c->a12_sprite_edges);
    w_u8(w, c->mmc3_bank_select);
    w_bool(w, c->mmc3_prg_mode);
    w_bool(w, c->mmc3_chr_mode);
    w_bool(w, c->bus_conflict);

    /* CHR RAM (the 8 KB of pattern tables the game uploads) is board state,
     * not ROM data: it changes while the game runs. */
    if (c->chr_ram && c->chr && c->chr_size)
        w_bytes(w, c->chr, c->chr_size);
}

static bool deser_cart(Reader *r, Cart *c)
{
    int i;
    uint8_t mapper, submapper, prg_banks, chr_banks, mirroring;
    uint32_t prg_size, chr_size;
    bool chr_ram, battery, four_screen, nes2;

    mapper = r_u8(r);
    submapper = r_u8(r);
    prg_banks = r_u8(r);
    chr_banks = r_u8(r);
    battery = r_bool(r);
    four_screen = r_bool(r);
    nes2 = r_bool(r);
    mirroring = r_u8(r);
    prg_size = r_u32(r);
    chr_size = r_u32(r);
    chr_ram = r_bool(r);

    /* The board is not reconstructed by a state file: the engine was created
     * with the cartridge it has.  A file from another board must not be
     * applied half-way. */
    if (mapper != c->mapper || submapper != c->submapper ||
        prg_banks != c->prg_banks_16k || chr_banks != c->chr_banks_8k ||
        mirroring != (uint8_t)c->mirroring ||
        chr_ram != c->chr_ram || battery != c->battery ||
        four_screen != c->four_screen || nes2 != c->nes2 ||
        prg_size != c->prg_size || chr_size != c->chr_size)
        return false;
    c->battery = battery;
    c->four_screen = four_screen;
    c->nes2 = nes2;

    r_bytes(r, c->prg_ram, sizeof(c->prg_ram));
    c->prg_bank_lo = r_u32(r);
    c->prg_bank_hi = r_u32(r);
    for (i = 0; i < 8; i++) c->chr_bank[i] = r_u32(r);
    r_bytes(r, c->reg, sizeof(c->reg));
    c->shift = r_u8(r);
    c->shift_count = r_u8(r);
    c->irq_latch = r_u8(r);
    c->irq_counter = r_u8(r);
    c->irq_reload = r_u8(r);
    c->irq_enable = r_bool(r);
    c->irq_pending = r_bool(r);
    c->a12_high = r_bool(r);
    c->a12_low_dots = r_u8(r);
    c->a12_sprite_edges = r_u8(r);
    c->mmc3_bank_select = r_u8(r);
    c->mmc3_prg_mode = r_bool(r);
    c->mmc3_chr_mode = r_bool(r);
    c->bus_conflict = r_bool(r);

    if (chr_ram && c->chr && chr_size) {
        if (chr_size != c->chr_size) return false;
        r_bytes(r, c->chr, chr_size);
    }
    return r->err == 0;
}

/* ------------------------------------------------------------------------ *
 *  The ported game's own state
 * ------------------------------------------------------------------------ */
static void ser_game(Writer *w)
{
    w_u8(w, A);
    w_u8(w, X);
    w_u8(w, Y);
    w_u8(w, SP);
    w_u8(w, F_C);
    w_u8(w, F_Z);
    w_u8(w, F_N);
    w_u8(w, F_V);
    w_u8(w, F_I);
    w_u8(w, F_D);

    w_u8(w, zanac_bank());
    w_u16(w, zanac_inline_block);
    w_u16(w, zanac_inline_table);
    w_u8(w, (uint8_t)zanac_unwind_armed);
    w_u8(w, (uint8_t)zanac_unwind_pending);

    w_bytes(w, zanac_bank_history, sizeof(zanac_bank_history));
    w_i32(w, zanac_bank_history_pos);
}

static bool deser_game(Reader *r)
{
    A = r_u8(r);
    X = r_u8(r);
    Y = r_u8(r);
    SP = r_u8(r);
    F_C = r_u8(r);
    F_Z = r_u8(r);
    F_N = r_u8(r);
    F_V = r_u8(r);
    F_I = r_u8(r);
    F_D = r_u8(r);

    zanac_bank_restore(r_u8(r));
    zanac_inline_block = r_u16(r);
    zanac_inline_table = r_u16(r);
    (void)r_u8(r);              /* saved unwind_armed */
    (void)r_u8(r);              /* saved unwind_pending */

    r_bytes(r, zanac_bank_history, sizeof(zanac_bank_history));
    zanac_bank_history_pos = r_i32(r);

    /* A jmp_buf cannot be serialised.  At a frame boundary the unwind is not
     * armed (it only covers one dispatcher call), so clearing it is exact; if a
     * future code path ever armed it across a wait, the loaded run must not
     * jump into the abandoned process's stack. */
    zanac_unwind_armed = 0;
    zanac_unwind_pending = 0;

    return r->err == 0;
}

/* ------------------------------------------------------------------------ *
 *  Save / load
 * ------------------------------------------------------------------------ */
static bool write_file(const char *path, const uint8_t *header,
                       const Writer *payload)
{
    FILE *fp = fopen(path, "wb");
    int ok = 1;

    if (!fp) return false;
    if (fwrite(header, 1, ZANAC_STATE_HEADER_SIZE, fp) != ZANAC_STATE_HEADER_SIZE)
        ok = 0;
    if (ok && fwrite(payload->data, 1, payload->len, fp) != payload->len)
        ok = 0;
    if (fclose(fp) != 0) ok = 0;
    return ok != 0;
}

static bool state_save_impl(const char *path, uint64_t host_frame,
                            bool checkpoint)
{
    Nes *nes = (Nes *)neslib_debug_nes();
    Writer w = { NULL, 0, 0, 0 };
    uint8_t header[ZANAC_STATE_HEADER_SIZE];
    uint32_t crc;
    bool ok;

    if (!nes || !nes->ppu || !nes->apu || !nes->cart) return false;

    w_u32(&w, ZANAC_STATE_PAYLOAD_VERSION);
    w_u32(&w, ZANAC_STATE_ENGINE_VERSION);
    w_u64(&w, host_frame);
    w_u64(&w, nes->frame_count);
    w_u8(&w, zanac_resume_kind);
    w_u8(&w, MEM8(game_mode));
    w_u8(&w, MEM8(current_level));
    w_u8(&w, zanac_bank());
    w_u8(&w, checkpoint ? 1u : 0u);

    /* Board descriptor, checked before anything is applied on load. */
    w_u8(&w, nes->cart->mapper);
    w_u8(&w, nes->cart->submapper);
    w_u8(&w, nes->cart->chr_ram ? 1u : 0u);
    w_u8(&w, (uint8_t)nes->cart->mirroring);
    w_u32(&w, nes->cart->chr_size);

    ser_nes(&w, nes);
    ser_ppu(&w, nes->ppu);
    ser_apu(&w, nes->apu);
    ser_cart(&w, nes->cart);
    ser_game(&w);

    if (w.err) {
        free(w.data);
        return false;
    }

    crc = crc32_ieee(w.data, w.len);
    memset(header, 0, sizeof(header));
    memcpy(header, ZANAC_STATE_MAGIC, ZANAC_STATE_MAGIC_SIZE);
    put_u32le(header + 8, ZANAC_STATE_FILE_VERSION);
    put_u32le(header + 12, (uint32_t)w.len);
    put_u32le(header + 16, crc);

    ok = write_file(path, header, &w);
    free(w.data);
    return ok;
}

bool zanac_state_save(const char *path, uint64_t host_frame)
{
    return state_save_impl(path, host_frame, false);
}

bool zanac_state_save_checkpoint(const char *path, uint64_t host_frame)
{
    return state_save_impl(path, host_frame, true);
}

/* Parse the file header and return a reader positioned at the payload. */
static bool open_state(const uint8_t *buf, size_t size, Reader *r,
                       uint32_t *payload_size, uint32_t *payload_crc)
{
    uint32_t version, size_field, crc;

    if (size < ZANAC_STATE_HEADER_SIZE) return false;
    if (memcmp(buf, ZANAC_STATE_MAGIC, ZANAC_STATE_MAGIC_SIZE) != 0) return false;
    version = get_u32le(buf + 8);
    size_field = get_u32le(buf + 12);
    crc = get_u32le(buf + 16);
    if (version != ZANAC_STATE_FILE_VERSION) return false;
    if ((uint64_t)size_field + ZANAC_STATE_HEADER_SIZE != (uint64_t)size)
        return false;

    r->data = buf + ZANAC_STATE_HEADER_SIZE;
    r->len = size_field;
    r->pos = 0;
    r->err = 0;
    if (payload_size) *payload_size = size_field;
    if (payload_crc) *payload_crc = crc;
    return true;
}

bool zanac_state_load(const char *path, uint64_t *host_frame)
{
    uint8_t *buf;
    size_t size;
    Reader r;
    uint32_t payload_size, payload_crc, payload_version, engine_version;
    uint64_t saved_host, saved_nes_frame;
    uint8_t resume, saved_mode, saved_level, saved_bank, canonical;
    uint8_t mapper, submapper, chr_ram, mirroring;
    uint32_t chr_size;
    Nes *nes = (Nes *)neslib_debug_nes();
    bool ok;

    if (!nes || !nes->ppu || !nes->apu || !nes->cart) return false;
    buf = read_whole_file(path, &size);
    if (!buf) return false;
    if (!open_state(buf, size, &r, &payload_size, &payload_crc)) {
        free(buf);
        return false;
    }
    if (crc32_ieee(r.data, r.len) != payload_crc) {
        free(buf);
        return false;
    }

    payload_version = r_u32(&r);
    engine_version = r_u32(&r);
    saved_host = r_u64(&r);
    saved_nes_frame = r_u64(&r);
    resume = r_u8(&r);
    saved_mode = r_u8(&r);
    saved_level = r_u8(&r);
    saved_bank = r_u8(&r);
    canonical = r_u8(&r);
    mapper = r_u8(&r);
    submapper = r_u8(&r);
    chr_ram = r_u8(&r);
    mirroring = r_u8(&r);
    chr_size = r_u32(&r);

    if (r.err || payload_version != ZANAC_STATE_PAYLOAD_VERSION ||
        engine_version != ZANAC_STATE_ENGINE_VERSION) {
        free(buf);
        return false;
    }

    /* Refuse a file from a different board before touching the live state. */
    if (mapper != nes->cart->mapper || submapper != nes->cart->submapper ||
        chr_ram != (nes->cart->chr_ram ? 1u : 0u) ||
        mirroring != (uint8_t)nes->cart->mirroring ||
        chr_size != nes->cart->chr_size) {
        free(buf);
        return false;
    }

    ok = deser_nes(&r, nes);
    ok = ok && deser_ppu(&r, nes->ppu);
    ok = ok && deser_apu(&r, nes->apu);
    ok = ok && deser_cart(&r, nes->cart);
    ok = ok && deser_game(&r);
    if (!ok || r.err || r.pos != r.len) {
        free(buf);
        return false;
    }

    zanac_resume_kind = resume;
    s_loaded_canonical = canonical != 0;
    /* Nothing from the run being replaced may leak into the resume chain. */
    zanac_resume_skip_setup = 0;
    zanac_resume_pause = 0;
    zanac_resume_ea0b_after_pause = 0;
    zanac_resume_level_after_ea0b = 0;
    zanac_resume_menu = 0;
    zanac_resume_menu_select = 0;
    neslib_state_after_load();

    if (host_frame) *host_frame = saved_host;
    (void)saved_nes_frame;
    (void)saved_mode;
    (void)saved_level;
    (void)saved_bank;
    free(buf);
    return true;
}

const char *zanac_state_path(void)
{
    const char *path = getenv("ZANAC_STATE_FILE");
    if (path && *path)
        return path;
    return ZANAC_QUICKSAVE_PATH;
}

bool zanac_state_exists(const char *path)
{
    FILE *fp = fopen(path, "rb");
    if (!fp) return false;
    fclose(fp);
    return true;
}

static const char *resume_name(uint8_t kind)
{
    switch (kind) {
    case ZANAC_RESUME_LEVEL:       return "level";
    case ZANAC_RESUME_LEVEL_SETUP: return "level-setup";
    case ZANAC_RESUME_PAUSE:       return "pause";
    case ZANAC_RESUME_MENU_SELECT: return "menu-select";
    case ZANAC_RESUME_MENU:
    default:                       return "menu";
    }
}

bool zanac_state_info(const char *path, char *out, size_t out_size)
{
    uint8_t *buf;
    size_t size;
    Reader r;
    uint32_t payload_size = 0, payload_crc = 0;
    uint32_t payload_version, engine_version;
    uint64_t host, nes_frame;
    uint8_t resume, saved_mode, saved_level, saved_bank, canonical;
    bool ok;

    if (!out || out_size == 0) return false;
    buf = read_whole_file(path, &size);
    if (!buf) return false;
    if (!open_state(buf, size, &r, &payload_size, &payload_crc)) {
        free(buf);
        return false;
    }
    payload_version = r_u32(&r);
    engine_version = r_u32(&r);
    host = r_u64(&r);
    nes_frame = r_u64(&r);
    resume = r_u8(&r);
    saved_mode = r_u8(&r);
    saved_level = r_u8(&r);
    saved_bank = r_u8(&r);
    canonical = r_u8(&r);
    ok = !r.err && payload_version == ZANAC_STATE_PAYLOAD_VERSION &&
         engine_version == ZANAC_STATE_ENGINE_VERSION &&
         crc32_ieee(r.data, r.len) == payload_crc;
    if (ok) {
        snprintf(out, out_size,
                 "Zanac save state v%u: host frame %llu, neslib frame %llu, "
                 "resume %s, game_mode $%02X, level %u, bank %u, %s, "
                 "%u payload bytes, CRC %08X",
                 ZANAC_STATE_FILE_VERSION,
                 (unsigned long long)host,
                 (unsigned long long)nes_frame,
                 resume_name(resume), saved_mode, saved_level, saved_bank,
                 canonical ? "canonical frame boundary" : "immediate snapshot",
                 payload_size, payload_crc);
    }
    free(buf);
    return ok;
}

void zanac_state_set_checkpoint_hook(ZanacStateCheckpointHook hook)
{
    s_checkpoint_hook = hook;
}

void zanac_state_checkpoint(void)
{
    /* Called from the level/pause/menu loops at loc_E880, loc_D3B9, loc_E080
     * and loc_E190, i.e. at points a state can be resumed from without a C call
     * stack.  The host decides whether it wants to write a pending save here
     * (and may leave with longjmp). */
    if (s_checkpoint_hook)
        s_checkpoint_hook();
}

void zanac_state_resume(void)
{
    zanac_resume_skip_setup = 0;
    switch (zanac_resume_kind) {
    case ZANAC_RESUME_LEVEL:
        /* The level setup already ran in the saved state: enter the per-frame
         * loop at loc_E880. */
        zanac_resume_skip_setup = 1;
        sub_E81E();
        break;
    case ZANAC_RESUME_LEVEL_SETUP:
        /* Saved during the level setup: re-run it from the restored RAM. */
        sub_E81E();
        break;
    case ZANAC_RESUME_PAUSE:
        if (!s_loaded_canonical) {
            /* An immediate snapshot from a loop that could not reach a
             * checkpoint: the level loop is the safest outer loop to enter. */
            zanac_resume_skip_setup = 1;
            sub_E81E();
            break;
        }
        /* Rebuild the three C frames the pause loop sits in: sub_D3A8 at its
         * loop label, then sub_EA0B after its sub_D3A8 call, then sub_E81E
         * after its sub_EA0B call.  The emulated 6502 stack (and the pushed
         * byte_83/byte_17A/Y/X) is part of the restored state. */
        zanac_resume_pause = 1;
        sub_D3A8();
        zanac_resume_ea0b_after_pause = 1;
        sub_EA0B();
        zanac_resume_kind = ZANAC_RESUME_LEVEL;
        zanac_resume_level_after_ea0b = 1;
        sub_E81E();
        break;
    case ZANAC_RESUME_MENU:
    case ZANAC_RESUME_MENU_SELECT:
        if (!s_loaded_canonical) {
            /* The file was written outside a menu loop (e.g. a boot/attract
             * snapshot that timed out); redraw the menu from its entry. */
            zanac_resume_kind = ZANAC_RESUME_MENU;
            main_menu_loop();
            break;
        }
        /* Resume inside show_main_menu's menu loop, then run the part of
         * main_menu_loop that follows it. */
        if (zanac_resume_kind == ZANAC_RESUME_MENU_SELECT)
            zanac_resume_menu_select = 1;
        else
            zanac_resume_menu = 1;
        show_main_menu();
        switch_bank_6();
        apu_stop();
        ppu_disable_all();
        start_game_check();
        sub_E81E();
        break;
    default:
        zanac_resume_kind = ZANAC_RESUME_MENU;
        main_menu_loop();
        break;
    }

    /* The original's outer loops never return.  Keep the process alive if a
     * resume path somehow does (same fallback as the menu loop). */
    for (;;) {
        zanac_resume_kind = ZANAC_RESUME_MENU;
        main_menu_loop();
    }
}
