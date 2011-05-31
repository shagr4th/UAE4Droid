#include "sysconfig.h"
#include "sysdeps.h"
#include "memorya.h"
#include "custom.h"
#include "autoconf.h"
#include "uae.h"
#include "ersatz.h"
#include "m68k/m68k_intrf.h"

#include <stdio.h>

// note: Shadow of the beast intentionally causes address errors
// Project-X uses trace mode.

#define dprintf_al(f,...) printf("%05i|%03i: %s: " f "\n",M68KCONTEXT.cycles_counter,m68k_context.cycles,__FUNCTION__,##__VA_ARGS__)
#define dprintf(...)
//#define dprintf(f,...) \
if (verb) \
printf("%05i| %s: " f "\n",M68KCONTEXT.cycles_counter,__FUNCTION__,##__VA_ARGS__)
#define mdprintf(...)
//#define mdprintf(f,...) \
if (verb) \
printf("%05i|%03i: %06x: %s: " f "\n",M68KCONTEXT.cycles_counter,m68k_context.cycles,m68k_context.pc-m68k_context.membase,__FUNCTION__,##__VA_ARGS__)
#if defined(DEBUG_UAE4ALL) || defined(UAE_CONSOLE)
#define dprintfu dprintf_al
#else
#define dprintfu(...)
#endif

// for easier sync with FAMEC
//#define SPLIT_32_2_16

struct Cyclone m68k_context;
M68KCONTEXT_t M68KCONTEXT;

#ifdef USE_CYCLONE_MEMHANDLERS
#define MH_STATIC extern "C"
#else
#define MH_STATIC static
#endif

MH_STATIC unsigned int cyclone_read8 (unsigned int a);
MH_STATIC unsigned int cyclone_read16(unsigned int a);
MH_STATIC unsigned int cyclone_read32(unsigned int a);
MH_STATIC void cyclone_write8 (unsigned int a,unsigned char  d);
MH_STATIC void cyclone_write16(unsigned int a,unsigned short d);
MH_STATIC void cyclone_write32(unsigned int a,unsigned int   d);

static unsigned int check_pc(unsigned int pc);
static int irq_ack(int level);
static int unrecognized_callback(void);


void init_m68k(void)
{
	CycloneInit();
	memset(&m68k_context, 0, sizeof(m68k_context));
	m68k_context.checkpc = check_pc;
	m68k_context.read8   = m68k_context.fetch8  = cyclone_read8;
	m68k_context.read16  = m68k_context.fetch16 = cyclone_read16;
	m68k_context.read32  = m68k_context.fetch32 = cyclone_read32;
	m68k_context.write8  = cyclone_write8;
	m68k_context.write16 = cyclone_write16;
	m68k_context.write32 = cyclone_write32;
	m68k_context.IrqCallback = irq_ack;
	m68k_context.UnrecognizedCallback = unrecognized_callback;
}


/* FAME interface */
void m68k_release_timeslice(void)
{
	m68k_context.cycles = -1;
	dprintf(" ");
}


int m68k_raise_irq(int level, int vector)
{
	M68KCONTEXT.interrupts[0] |= 1 << level;
	m68k_irq_update(0);
	dprintf("lvl: %i, line: %i, irqs: %02x", level, m68k_context.irq, M68KCONTEXT.interrupts[0]);

	return 0;
}


int m68k_lower_irq(int level)
{
	M68KCONTEXT.interrupts[0] &= ~(1 << level);
	m68k_irq_update(0);
	dprintf("lvl: %i, line: %i, irqs: %02x", level, m68k_context.irq, M68KCONTEXT.interrupts[0]);

	return 0;
}


int m68k_reset(void)
{
	dprintfu("");
	m68k_context.state_flags = 0;
	m68k_context.osp = 0;
	m68k_context.srh = 0x27; // Supervisor mode
	m68k_context.flags = 0;
//	m68k_context.irq = 0;
//	m68k_context.a[7] = m68k_context.read32(0); // Stack Pointer, set by caller
//	m68k_context.membase=0;
//	m68k_context.pc = m68k_context.checkpc(PicoCpu.read32(4)); // Program Counter, set by caller

	return 0;
}


#if 0 // inlined
int m68k_emulate(int cycles)
{
	cycles -= 1;
	m68k_context.cycles = cycles;
	//dprintf("c = %i, stoped: %i", cycles, m68k_context.stopped);

	//if (m68k_context.stopped)
	//	dprintf("c = %i, stopped: %i, irq: %02x", cycles, m68k_context.stopped, M68KCONTEXT.interrupts[0]);

	CycloneRun(&m68k_context);

	M68KCONTEXT.cycles_counter += cycles - m68k_context.cycles;

	// Cyclone doesn't check for irq changes while it executes, so try to take it now
#if 0
	if (m68k_context.irq > (m68k_context.srh&7))
	{
		int oirq = m68k_context.irq, c = CycloneFlushIrq(&m68k_context);
		printf("CycEnd irq %i %i -> %i\n", c, oirq, m68k_context.irq);
		M68KCONTEXT.cycles_counter += c; // CycloneFlushIrq(&m68k_context);
	}
#endif

#ifdef SPLIT_32_2_16
	if (m68k_context.state_flags & 1)
	{
	     M68KCONTEXT.cycles_counter++; // adjust for sync with FAME
	}
#endif

	return 0;
}
#endif


/* helper funcs */

// must call this whenever M68KCONTEXT.interrupts[0] gets changed
#if 0 // inlined
void m68k_irq_update(int end_timeslice)
{
	int level, ints = M68KCONTEXT.interrupts[0];

	for (level = 7; level && !(ints & (1 << level)); level--);

	m68k_context.irq = level;

	if (end_timeslice && m68k_context.cycles >= 0 && !(m68k_context.state_flags & 1))
	{
		// cause timeslice end after 24 cycles
		M68KCONTEXT.cycles_counter += 24 - 1 - m68k_context.cycles;
		m68k_context.cycles = 24 - 1;
	}
}
#endif


static unsigned int check_pc(unsigned int pc)
{
	static int loopcode = 0x60fe60fe;
	pc -= m68k_context.membase;
	// pc &= ~1; // leave it for address error emulation

	// do we really need to do this? I've seen some cores never clearing the upper bits
	// and pushing them, etc. But in that case kickstart 1.3 hangs because cmp @ fc090e fails.
	pc &= ~0xff000000;

	uae_u8 *p = baseaddr[pc >> 16];

	if ((int)p & 1)
	{
		printf("Cyclone problem: branched to unknown memory location: %06x\n", pc);
		p = (uae_u8 *)&loopcode - pc;
	}

	//dprintf("newpc=%06x, base=%p, result=%08x, oldpc=%06x", pc, p, (unsigned)p + pc, m68k_context.pc - m68k_context.membase);
	m68k_context.membase = (unsigned)p;
	return (unsigned)p + pc;
}


static int irq_ack(int level)
{
	M68KCONTEXT.interrupts[0] &= ~(1 << level);
	m68k_irq_update(0);

	dprintf("level: %i, irqs: %02x", level, M68KCONTEXT.interrupts[0]);
	return CYCLONE_INT_ACK_AUTOVECTOR;
}


static int unrecognized_callback(void)
{
	unsigned pc = m68k_context.pc - m68k_context.membase;
	unsigned opcode = *(uae_u16 *)m68k_context.pc;

	dprintfu("op 0x%04x @ 0x%06x", opcode, pc);

	if (cloanto_rom && (opcode & 0xF100) == 0x7100) {
		dprintfu("  cloanto"); // whatever it is
		_68k_dreg((opcode >> 9) & 7) = (uae_s8)(opcode & 0xFF);
		m68k_context.pc += 2;
		return 1;
	}
	if (opcode == 0x4E7B && get_long(0x10) == 0 && (pc & 0xF80000) == 0xF80000) {
		dprintfu("  68020");
		write_log("Your Kickstart requires a 68020 CPU. Giving up.\n");
		set_special (SPCFLAG_BRK);
		quit_program = 1;
		m68k_context.cycles = 0;
		return 1;
	}
	if (opcode == 0xFF0D) {
		if ((pc & 0xF80000) == 0xF80000) {
			dprintfu("  dummy");
			// This is from the dummy Kickstart replacement
			uae_u16 arg = *(uae_u16 *)(pc+2);
			m68k_context.pc += 4;
			ersatz_perform(arg);
		} else if ((pc & 0xFFFF0000) == RTAREA_BASE) {
			dprintfu("  stop");
			// User-mode STOP replacement 
			mispcflags|=SPCFLAG_STOP;
			m68k_context.state_flags |= 1;
			m68k_context.cycles = 0;
		}
		return 1;
	}
	if ((opcode & 0xF000) == 0xA000 && (pc & 0xFFFF0000) == RTAREA_BASE) {
		dprintfu("  call");
		// Calltrap.
		m68k_context.pc += 2;
#ifdef USE_AUTOCONFIG
		call_calltrap (opcode & 0xFFF);
#endif
		return 1;
	}
	if ((opcode & 0xF000) == 0xF000) {
		dprintfu("  exp8");
		return 0;
	}
	if ((opcode & 0xF000) == 0xA000) {
		dprintfu("  expA");
		return 0;
	}
	dprintfu("  real illegal");
#if 0
	if (opcode==0x4e7b && pc==0x00080ab6)
	{
		FILE *f;
		uae_u8 *p;
		f = fopen("80000.bin", "wb");
		p = (uae_u8 *) baseaddr[0x80000>>16];
		fwrite(p+0x80000, 1, 0x20000, f);
		fclose(f);
		f = fopen("00000.bin", "wb");
		p = (uae_u8 *) baseaddr[0x00000>>16];
		fwrite(p+0x00000, 1, 0x20000, f);
		fclose(f);
		sync();
	}
#endif

	write_log ("Illegal instruction: %04x at %08x\n", opcode, pc);
	return 0;
}


/* memory handlers */
#ifndef USE_CYCLONE_MEMHANDLERS
static unsigned char cyclone_read8(unsigned int a)
{
	a &= ~0xff000000;
	uae_u8 *p = baseaddr[a>>16];
	if ((int)p & 1)
	{
		addrbank *ab = (addrbank *) ((unsigned)p & ~1);
		uae_u32 ret = ab->bget(a);
		mdprintf("@ %06x, handler, =%02x", a, ret);
		return ret;
	}
	else
	{
		mdprintf("@ %06x, =%02x", a, p[a^1]);
		return p[a^1];
	}
}

static unsigned short cyclone_read16(unsigned int a)
{
	a &= ~0xff000000;
	uae_u16 *p = (uae_u16 *) baseaddr[a>>16];
	if ((int)p & 1)
	{
		addrbank *ab = (addrbank *) ((unsigned)p & ~1);
		uae_u32 ret = ab->wget(a);
		mdprintf("@ %06x, handler, =%04x", a, ret);
		return ret;
	}
	else
	{
		mdprintf("@ %06x, =%04x", a, p[a>>1]);
		return p[a>>1];
	}
}

static unsigned int cyclone_read32(unsigned int a)
{
#ifdef SPLIT_32_2_16
	return (read16(a)<<16) | read16(a+2);
#else
	a &= ~0xff000000;
	uae_u16 *p = (uae_u16 *) baseaddr[a>>16];
	if ((int)p & 1)
	{
		addrbank *ab = (addrbank *) ((unsigned)p & ~1);
		uae_u32 ret = ab->lget(a);
		mdprintf("@ %06x, handler, =%08x", a, ret);
		return ret;
	}
	else
	{
		mdprintf("@ %06x, =%08x", a, (p[a>>1]<<16)|p[(a>>1)+1]);
		a >>= 1;
		return (p[a]<<16)|p[a+1];
	}
#endif
}

static void cyclone_write8(unsigned int a, unsigned char d)
{
	a &= ~0xff000000;
	uae_u8 *p = baseaddr[a>>16];
	if ((int)p & 1)
	{
		addrbank *ab = (addrbank *) ((unsigned)p & ~1);
		mdprintf("@ %06x, handler, =%02x", a, d);
		ab->bput(a, d&0xff);
	}
	else
	{
		mdprintf("@ %06x, =%02x", a, d);
		p[a^1] = d;
	}
}

static void cyclone_write16(unsigned int a,unsigned short d)
{
	a &= ~0xff000000;
	uae_u16 *p = (uae_u16 *) baseaddr[a>>16];
	if ((int)p & 1)
	{
		addrbank *ab = (addrbank *) ((unsigned)p & ~1);
		mdprintf("@ %06x, handler, =%04x", a, d);
		ab->wput(a, d&0xffff);
	}
	else
	{
		mdprintf("@ %06x, =%04x", a, d);
		p[a>>1] = d;
	}
}

static void cyclone_write32(unsigned int a,unsigned int d)
{
#ifdef SPLIT_32_2_16
	write16(a, d>>16);
	write16(a+2, d);
#else
	a &= ~0xff000000;
	uae_u16 *p = (uae_u16 *) baseaddr[a>>16];
	if ((int)p & 1)
	{
		addrbank *ab = (addrbank *) ((unsigned)p & ~1);
		mdprintf("@ %06x, handler, =%08x", a, d);
		ab->lput(a, d);
	}
	else
	{
		mdprintf("@ %06x, =%08x", a, d);
		a >>= 1;
		p[a] = d >> 16;
		p[a+1] = d;
	}
#endif
}
#endif


/* nothing to do? */
void init_memmaps(addrbank* banco)
{
}

void map_zone(unsigned addr, addrbank* banco, unsigned realstart)
{
}

void clear_fame_mem_dummy(void)
{
}

