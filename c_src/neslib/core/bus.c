/* bus.c - CPU address space, OAM DMA and DMC DMA.
 *
 * Reference material:
 *   Specs/01-platform-overview.md section 4 - CPU memory map and mirrors
 *   Specs/02-cpu-dma.md                     - OAM/DMC DMA, RDY halting
 *   Specs/05-boards-and-io.md section 6/7   - controllers, $4014
 */
#include <string.h>

#include "nes.h"
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "cart.h"

void nes_set_open_bus(Nes *nes, uint8_t value) { nes->cpu_open_bus = value; }
void nes_set_ext_bus(Nes *nes, uint8_t value) { nes->ext_bus = value; }

/* --------------------------------------------------------------- reads ---- */

uint8_t nes_cpu_read(Nes *nes, uint16_t addr)
{
    uint8_t v;
    bool driven = true;

    if (addr < 0x2000) {
        v = nes->ram[addr & 0x7FF];           /* A0-A10 only: four mirrors */
    } else if (addr < 0x4000) {
        v = ppu_read_register(nes->ppu, (uint16_t)(addr & 0x2007));
    } else if (addr < 0x4018) {
        if (addr == 0x4015) {
            /* The APU status register drives every bit except bit 5, which the
             * external bus supplies (03-apu-frame-counter.md section 2;
             * AccuracyCoin "Implied Dummy Reads" test 4 reads $4015 with bit 5
             * set on the bus and expects it back). */
            v = (uint8_t)((apu_read(nes->apu, addr) & 0xDF) |
                          (nes->cpu_open_bus & 0x20));
        } else if (addr == 0x4016) {
            v = input_read(&nes->input, 0, nes->ext_bus);
        } else if (addr == 0x4017) {
            v = input_read(&nes->input, 1, nes->ext_bus);
        } else {
            /* $4000-$4014 are write-only and drive nothing
             * (02-cpu-core.md phenomenon 2): the external bus keeps its last
             * value, and that is what a read of the register returns. */
            v = nes->ext_bus;
            driven = false;
        }
    } else if (addr < 0x4020) {
        /* $4018-$401F: 2A03 test registers, open bus when DBG = 0
         * (01-platform-overview.md section 4). */
        v = nes->ext_bus;
        driven = false;
    } else if (addr < 0x6000) {
        /* $4020-$5FFF: the motherboard does not decode this range, so nothing
         * drives the data bus and a read returns whatever was last on it
         * (01-platform-overview.md section 4).  AccuracyCoin's "Open Bus"
         * test 1 and the "APU Register Activation" pre-requisites depend on
         * this range not reading back as zero. */
        v = nes->ext_bus;
        driven = false;
    } else {
        v = cart_cpu_read(nes->cart, addr);
    }

    /* The latch is updated by the CPU side, which is the only layer that
     * knows whether a read was a fetch or real data.  Updating it here as
     * well silently overwrote the value with the operand-fetch bytes. */
    (void)driven;
    return v;
}

/* --------------------------------------------------------------- writes --- */

void nes_cpu_write(Nes *nes, uint16_t addr, uint8_t value)
{
    /* The CPU drives both data buses for the whole write cycle, so the
     * latches follow every store (02-cpu-core.md phenomenon 2). */
    nes->cpu_open_bus = value;
    nes->ext_bus = value;

    if (addr < 0x2000) {
        nes->ram[addr & 0x7FF] = value;
    } else if (addr < 0x4000) {
        ppu_write_register(nes->ppu, (uint16_t)(addr & 0x2007), value);
    } else if (addr < 0x4018) {
        switch (addr) {
        case 0x4014:
            /* Writing OAMDMA latches the source page.  The transfer cannot
             * begin before the next read cycle (02-cpu-dma.md section 2), so
             * the engine only becomes pending here; the halt is decided by the
             * cycle loop.  A second write before the halt reloads the page and
             * clears the counter. */
            nes->oam_dma_page = value;
            if (!nes->oam_dma_active) {
                nes->oam_dma_pending = true;
                nes->oam_dma_pos = 0;
            } else {
                nes->oam_dma_pos = 0;
            }
            break;
        case 0x4015: {
            /* Only the 0 -> 1 transition of bit 4 performs DSLOAD and starts a
             * sample, so only that write schedules a *load* fetch: it halts on
             * a get cycle during the 2nd APU cycle after the write, i.e. on
             * the 3rd or 4th CPU cycle after it (02-cpu-dma.md section 4,
             * NESdev APU_DMC).  A repeat write of 1 while the channel is
             * already enabled leaves the running sample alone
             * (AccuracyCoin "DMA Landing on Write" test 3). */
            bool was_enabled = nes->apu->dmc.enabled;
            apu_write(nes->apu, addr, value);
            if (nes->apu->dmc.enabled && !was_enabled) {
                nes->dmc_load_delay = 3;
                nes->dmc_halt_on_get = true;
            }
            break;
        }
        case 0x4016:
            /* The register bit is latched here; the pad-side strobe follows it
             * at the next ACLK2 edge (see apu_tick). */
            apu_write(nes->apu, addr, value);
            break;
        case 0x4017:
            apu_write(nes->apu, addr, value); /* frame counter, not the pad */
            break;
        default:
            apu_write(nes->apu, addr, value);
            break;
        }
    } else if (addr < 0x4020) {
        apu_write(nes->apu, addr, value); /* $401A test write; ignored otherwise */
    } else {
        cart_cpu_write(nes->cart, addr, value);
    }
}

/* ------------------------------------------------------------- DMA engine -- */

/* The DMA engines are driven cycle-by-cycle from nes_run_cpu_cycle(): the bus
 * arbitration cannot be expressed as a blocking step because a DMC fetch can
 * pause an OAM transfer and every no-op cycle has to repeat the core's read
 * (02-cpu-dma.md sections 3-6).  nes_dma_tick() therefore has nothing left to
 * do; it is kept for the save-state decoder's benefit. */
void nes_dma_tick(Nes *nes)
{
    (void)nes;
}
