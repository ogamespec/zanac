/* state.c - save-state serialisation over a temporary file handle.
 *
 * The serialiser walks the same field order in both directions, so the format
 * is symmetric and version-checked. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nes.h"
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "cart.h"
#include "state.h"

static FILE *g_fp;

static void w_bytes(const void *p, size_t n) { if (g_fp) fwrite(p, 1, n, g_fp); }
static void r_bytes(void *p, size_t n) { if (g_fp) { if (fread(p, 1, n, g_fp) != n) { /* short read */ } } }

#define W(field) w_bytes(&(field), sizeof(field))
#define R(field) r_bytes(&(field), sizeof(field))

static void ser_cpu(Cpu *c)
{
    W(c->a); W(c->x); W(c->y); W(c->s); W(c->p); W(c->pc);
    W(c->opcode); W(c->cycle); W(c->pages); W(c->base); W(c->ea); W(c->ptr);
    W(c->operand); W(c->data); W(c->rmw_value); W(c->rmw_new);
    W(c->jsr_lo_scratch); W(c->frame_lo); W(c->frame_pc);
    W(c->frame_pushed_p); W(c->frame_vector); W(c->frame_flags);
    W(c->nmi_line); W(c->nmi_pending); W(c->nmi_latched); W(c->irq_line);
    W(c->irq_latched); W(c->irq_inhibit_next);
    W(c->rdy_cycles); W(c->total_cycles); W(c->instructions);
    /* Re-derive the step-table pointer from the opcode.  The frame pointers are
     * not serialised because they alias constant tables chosen by frame_flags. */
    c->steps = NULL;
    c->info = NULL;
}

static void ser_ppu(Ppu *p)
{
    W(p->ctrl); W(p->mask); W(p->mask_pending); W(p->mask_pending_delay);
    W(p->addr_qn);
    for (int qi = 0; qi < 4; qi++) { W(p->addr_q[qi]); W(p->addr_q_delay[qi]); } W(p->status); W(p->oam_addr); W(p->read_buffer);
    W(p->v); W(p->t); W(p->fine_x); W(p->w); W(p->ppu_bus);
    W(p->latch); W(p->nt_latch); W(p->at_latch);
    W(p->line); W(p->dot); W(p->frame_odd); W(p->odd_frame_skip);
    W(p->dot_count); W(p->nmi_out); W(p->nmi_prev);
    W(p->rc_active); W(p->rc_until_cpu_cycle);
    W(p->int_ff); W(p->int_delay);
    W(p->vblank_read_suppress); W(p->vblank_set_in_cycle);
    w_bytes(p->ciram, sizeof(p->ciram));
    w_bytes(p->pal, sizeof(p->pal));
    w_bytes(p->oam, sizeof(p->oam));
    w_bytes(p->oam2, sizeof(p->oam2));
    W(p->bg_sr_lo); W(p->bg_sr_hi); W(p->bg_at_lo); W(p->bg_at_hi);
    W(p->nt_latch); W(p->at_latch); W(p->pat_lo); W(p->fetch_phase);
    w_bytes(p->bus_decay, sizeof(p->bus_decay));
    w_bytes(p->spr_lane_x, sizeof(p->spr_lane_x));
    w_bytes(p->spr_lane_cnt, sizeof(p->spr_lane_cnt));
    w_bytes(p->spr_lane_is0, sizeof(p->spr_lane_is0));
    W(p->oam2_addr); W(p->oam2_seed); W(p->corrupt_seed_delay); W(p->oam_corrupt_pending); W(p->oam_read);
    w_bytes(p->spr_lane_attr, sizeof(p->spr_lane_attr));
    w_bytes(p->spr_lane_pat, sizeof(p->spr_lane_pat));
    w_bytes(p->spr_lane_armed, sizeof(p->spr_lane_armed));
    w_bytes(p->spr_lane_done, sizeof(p->spr_lane_done));
    w_bytes(p->spr_lane_shifted, sizeof(p->spr_lane_shifted));
    w_bytes(p->spr_lane_flip, sizeof(p->spr_lane_flip));
    W(p->spr_count); W(p->sprite0_on_line); W(p->eval_start);
    W(p->spr_eval_n); W(p->spr_eval_b); W(p->spr_eval_m); W(p->spr_eval_found); W(p->spr_eval_state);
    W(p->pixel);
    W(p->rendering); W(p->prev_rendering); W(p->oam_corrupt);
    W(p->bg_frozen);
    W(p->emph_r); W(p->emph_g); W(p->emph_b);
    W(p->greyscale); W(p->frame_dots);
    w_bytes(p->framebuffer, sizeof(p->framebuffer));
}

static void ser_apu(Apu *a)
{
    int i;
    for (i = 0; i < 2; i++) {
        ApuPulse *p = &a->pulse[i];
        W(p->enabled); W(p->duty); W(p->halt); W(p->constant_volume);
        W(p->volume); W(p->timer); W(p->timer_counter); W(p->duty_counter);
        W(p->length); W(p->env_start); W(p->env_divider); W(p->env_decay);
        W(p->sweep_enable); W(p->sweep_period); W(p->sweep_negate);
        W(p->sweep_shift); W(p->sweep_divider); W(p->sweep_reload); W(p->output);
    }
    W(a->triangle.enabled); W(a->triangle.control); W(a->triangle.linear_reload);
    W(a->triangle.reload_flag); W(a->triangle.linear_counter);
    W(a->triangle.timer); W(a->triangle.timer_counter); W(a->triangle.length);
    W(a->triangle.seq); W(a->triangle.output);
    W(a->frame_reset_count);
    for (i = 0; i < (int)(sizeof(a->frame_reset) / sizeof(a->frame_reset[0])); i++) {
        W(a->frame_reset[i].delay); W(a->frame_reset[i].mode);
    }
    W(a->dmc_stopped); W(a->dmc_stop_cycle); W(a->dmc_stop_eta);

    W(a->noise.enabled); W(a->noise.halt); W(a->noise.constant_volume);
    W(a->noise.volume); W(a->noise.period_index); W(a->noise.mode);
    W(a->noise.timer_counter); W(a->noise.lfsr); W(a->noise.length);
    W(a->noise.env_start); W(a->noise.env_divider); W(a->noise.env_decay);
    W(a->noise.output);

    W(a->dmc.enabled); W(a->dmc.irq_enable); W(a->dmc.loop); W(a->dmc.rate_index);
    W(a->dmc.rate_counter); W(a->dmc.sample_addr); W(a->dmc.sample_len);
    W(a->dmc.sample_buffer); W(a->dmc.buffer_full); W(a->dmc.bits_remaining);
    W(a->dmc.shift_reg); W(a->dmc.output); W(a->dmc.irq_flag);
    W(a->dmc.start_pending); W(a->dmc.silence);
    W(a->dmc_sample_base); W(a->dmc_sample_len);

    W(a->status); W(a->frame_irq); W(a->frame_irq_clear_pending);
    W(a->frame_irq_inhibit); W(a->frame_mode);
    W(a->frame_event); W(a->frame_cycles); W(a->apu_cycles); W(a->phase);
    W(a->out_latch); W(a->pad_out); W(a->lock);
}

static void ser_cart(Cart *c)
{
    W(c->mapper); W(c->submapper); W(c->prg_banks_16k); W(c->chr_banks_8k);
    W(c->battery); W(c->four_screen); W(c->nes2); W(c->mirroring);
    W(c->prg_bank_lo); W(c->prg_bank_hi);
    w_bytes(c->chr_bank, sizeof(c->chr_bank));
    w_bytes(c->reg, sizeof(c->reg));
    W(c->shift); W(c->shift_count);
    W(c->irq_latch); W(c->irq_counter); W(c->irq_reload);
    W(c->irq_enable); W(c->irq_pending);
    W(c->mmc3_bank_select); W(c->mmc3_prg_mode); W(c->mmc3_chr_mode);
    w_bytes(c->prg_ram, sizeof(c->prg_ram));
    /* CHR-RAM is part of the cartridge state. */
    if (c->chr_ram && c->chr)
        w_bytes(c->chr, c->chr_size);
}

static bool state_serialize(Nes *nes)
{
    uint32_t magic = STATE_MAGIC;
    uint32_t version = STATE_VERSION;
    uint8_t region = (uint8_t)nes->region;
    uint32_t chr_is_ram = nes->cart->chr_ram ? 1u : 0u;

    w_bytes(&magic, sizeof(magic));
    w_bytes(&version, sizeof(version));
    w_bytes(&region, sizeof(region));
    w_bytes(&chr_is_ram, sizeof(chr_is_ram));

    w_bytes(nes->ram, sizeof(nes->ram));
    W(nes->cpu_cycles); W(nes->frame_count); W(nes->cpu_open_bus); W(nes->ext_bus);
    W(nes->oam_dma_active); W(nes->oam_dma_page); W(nes->oam_dma_pos);
    W(nes->dma_get_parity); W(nes->cpu_halted); W(nes->cpu_halt_addr);
    W(nes->oam_dma_pending); W(nes->oam_dma_want); W(nes->oam_dma_buffer);
    W(nes->dmc_phase); W(nes->dmc_attempt); W(nes->dmc_halt_on_get); W(nes->dmc_load_delay);
    W(nes->dmc_dma_addr); W(nes->dmc_dma_value);
    W(nes->dmc_target_cycle); W(nes->dmc_abort_cycle); W(nes->dmc_abort_hold);
    w_bytes(&nes->input, sizeof(nes->input));
    w_bytes(&nes->ppu_rev, sizeof(nes->ppu_rev));

    ser_cpu(nes->cpu);
    ser_ppu(nes->ppu);
    ser_apu(nes->apu);
    ser_cart(nes->cart);
    return true;
}

static bool state_deserialize(Nes *nes)
{
    uint32_t magic = 0, version = 0, chr_is_ram = 0;
    uint8_t region = 0;
    r_bytes(&magic, sizeof(magic));
    r_bytes(&version, sizeof(version));
    if (magic != STATE_MAGIC || version != STATE_VERSION)
        return false;
    r_bytes(&region, sizeof(region));
    r_bytes(&chr_is_ram, sizeof(chr_is_ram));
    if (chr_is_ram != (nes->cart->chr_ram ? 1u : 0u))
        return false;
    nes->region = (NesRegion)region;

    r_bytes(nes->ram, sizeof(nes->ram));
    R(nes->cpu_cycles); R(nes->frame_count); R(nes->cpu_open_bus); R(nes->ext_bus);
    R(nes->oam_dma_active); R(nes->oam_dma_page); R(nes->oam_dma_pos);
    R(nes->dma_get_parity); R(nes->cpu_halted); R(nes->cpu_halt_addr);
    R(nes->oam_dma_pending); R(nes->oam_dma_want); R(nes->oam_dma_buffer);
    R(nes->dmc_phase); R(nes->dmc_attempt); R(nes->dmc_halt_on_get); R(nes->dmc_load_delay);
    R(nes->dmc_dma_addr); R(nes->dmc_dma_value);
    r_bytes(&nes->input, sizeof(nes->input));
    r_bytes(&nes->ppu_rev, sizeof(nes->ppu_rev));

    ser_cpu(nes->cpu);
    ser_ppu(nes->ppu);
    ser_apu(nes->apu);
    ser_cart(nes->cart);

    /* Re-derive the instruction step pointer from the saved opcode, and treat
     * a mid-instruction restore as an instruction boundary. */
    nes->cpu->steps = NULL;
    nes->cpu->info = NULL;
    return true;
}

static FILE *open_state(const char *path, const char *mode, const char *container)
{
    FILE *tmp = tmpfile();
    FILE *out;
    if (!tmp) return NULL;
    (void)container;
    g_fp = tmp;
    if (strcmp(mode, "rb") == 0) {
        out = fopen(path, "rb");
        if (!out) { fclose(tmp); g_fp = NULL; return NULL; }
        {
            char buf[65536];
            size_t n;
            while ((n = fread(buf, 1, sizeof(buf), out)) > 0)
                fwrite(buf, 1, n, tmp);
        }
        fclose(out);
        fseek(tmp, 0, SEEK_SET);
    }
    return tmp;
}

bool state_save(Nes *nes, const char *path)
{
    FILE *fp = tmpfile();
    bool ok;
    if (!fp) return false;
    g_fp = fp;
    ok = state_serialize(nes);
    if (ok) {
        FILE *out = fopen(path, "wb");
        char buf[65536];
        size_t n;
        if (!out) {
            fclose(fp);
            g_fp = NULL;
            return false;
        }
        fseek(fp, 0, SEEK_SET);
        while ((n = fread(buf, 1, sizeof(buf), fp)) > 0)
            fwrite(buf, 1, n, out);
        fclose(out);
    }
    fclose(fp);
    g_fp = NULL;
    return ok;
}

bool state_load(Nes *nes, const char *path)
{
    FILE *in = fopen(path, "rb");
    FILE *fp;
    bool ok;
    if (!in) return false;
    fp = tmpfile();
    if (!fp) { fclose(in); return false; }
    {
        char buf[65536];
        size_t n;
        while ((n = fread(buf, 1, sizeof(buf), in)) > 0)
            fwrite(buf, 1, n, fp);
    }
    fclose(in);
    fseek(fp, 0, SEEK_SET);
    g_fp = fp;
    ok = state_deserialize(nes);
    fclose(fp);
    g_fp = NULL;
    return ok;
}

/* ------------------------------------------------------------------ slots -- */

const char *state_slot_path(int slot)
{
    static char buf[512];
#ifdef _WIN32
    const char *base = getenv("APPDATA");
    if (!base) base = ".";
    snprintf(buf, sizeof(buf), "%s\\Nespeculator\\slot%d.nst", base, slot);
#else
    const char *base = getenv("HOME");
    if (!base) base = ".";
    snprintf(buf, sizeof(buf), "%s/.nespeculator/slot%d.nst", base, slot);
#endif
    return buf;
}

bool state_slot_exists(int slot)
{
    FILE *f = fopen(state_slot_path(slot), "rb");
    if (!f) return false;
    fclose(f);
    return true;
}

bool state_save_slot(Nes *nes, int slot) { return state_save(nes, state_slot_path(slot)); }
bool state_load_slot(Nes *nes, int slot) { return state_load(nes, state_slot_path(slot)); }
