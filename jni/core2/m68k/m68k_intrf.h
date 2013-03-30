#ifndef M68KIntrfH
#define M68KIntrfH
#if defined(USE_FAME_CORE)

/* defined in m68k_cmn_intrf.cpp */
extern int uae4all_go_interrupt;
extern unsigned mispcflags;

void init_memmaps(addrbank* banco);
void map_zone(unsigned addr, addrbank* banco, unsigned realstart);
void m68k_go(int may_quit);
void init_m68k(void);

#define _68k_spcflags mispcflags
#define fill_prefetch_0()
#define dump_counts()
#define flush_icache(X)

#endif


#if defined(USE_FAME_CORE) && !defined(USE_CYCLONE_CORE)

#if defined(DREAMCAST) || defined(USE_FAME_CORE_C)
#define M68KCONTEXT m68kcontext
#else
#define M68KCONTEXT _m68kcontext
#endif
#if defined(DREAMCAST) || defined(USE_FAME_CORE_C)
#define IO_CYCLE io_cycle_counter
#else
#define IO_CYCLE __io_cycle_counter
#endif
extern int IO_CYCLE;
#define m68k_irq_update(end_timeslice) \
	if ((end_timeslice) && IO_CYCLE > 0) { \
		M68KCONTEXT.cycles_counter += 24 - IO_CYCLE; \
		IO_CYCLE = 24; \
	}


#include "m68k/fame/fame.h"
#include "memory-uae.h"

extern struct M68K_CONTEXT M68KCONTEXT;

#define _68k_dreg(num) (M68KCONTEXT.dreg[(num)])
#define _68k_areg(num) (M68KCONTEXT.areg[(num)])
#define _68k_sreg 	M68KCONTEXT.sr
#define _68k_ispreg 	M68KCONTEXT.isp
#define _68k_mspreg 	M68KCONTEXT.msp
#define _68k_uspreg 	M68KCONTEXT.usp
#define _68k_intmask   ((M68KCONTEXT.sr >> 8) & 7)
#define _68k_incpc(o)  (M68KCONTEXT.pc += (o))

static __inline__ unsigned _68k_getpc(void)
{
//	return M68KCONTEXT.pc;
	return m68k_get_pc();
}
static __inline__ void _68k_setpc(unsigned mipc)
{
	M68KCONTEXT.pc=mipc;
	m68k_set_register(M68K_REG_PC, mipc);
}

static __inline__ void set_special (unsigned x)
{
    _68k_spcflags |= x;
}

static __inline__ void unset_special (uae_u32 x)
{
    _68k_spcflags &= ~x;
}

#elif defined(USE_CYCLONE_CORE)

#include "m68k/cyclone/cyclone.h"

extern struct Cyclone m68k_context;

// FAME interface
void m68k_release_timeslice(void);
int  m68k_raise_irq(int level, int vector);
int  m68k_lower_irq(int level);
int  m68k_reset(void);
//int  m68k_emulate(int cycles);
//void m68k_irq_update(int end_timeslice);
#define M68K_AUTOVECTORED_IRQ 0 // not going to use vector number


typedef struct
{
	uae_u8  interrupts[8];	// only interrupts[0] is used
	uae_u32 cycles_counter;
	uae_u16 execinfo;	// will really only use bit7 (HALTED)
} M68KCONTEXT_t;

extern M68KCONTEXT_t M68KCONTEXT;

#define m68k_emulate(cycles) \
{ \
	m68k_context.cycles = (cycles) - 1;  \
	CycloneRun(&m68k_context); \
	M68KCONTEXT.cycles_counter += (cycles) - 1 - m68k_context.cycles; \
}

#define m68k_irq_update(end_timeslice) \
{ \
	int level, ints = M68KCONTEXT.interrupts[0]; \
	for (level = 7; level && !(ints & (1 << level)); level--); \
	m68k_context.irq = level; \
\
	if ((end_timeslice) && m68k_context.cycles >= 0 && !(m68k_context.state_flags & 1)) \
	{ \
		M68KCONTEXT.cycles_counter += 24 - 1 - m68k_context.cycles; \
		m68k_context.cycles = 24 - 1; \
	} \
}


#define IO_CYCLE        m68k_context.cycles

// this is only set in one place, .srh is ok in that situation
#define _68k_sreg 	m68k_context.srh

#define _68k_dreg(num)  m68k_context.d[num]
#define _68k_areg(num)  m68k_context.a[num]
#define _68k_ispreg 	m68k_context.a[7]
#define _68k_mspreg 	m68k_context.osp
#define _68k_uspreg 	m68k_context.osp
#define _68k_intmask   (m68k_context.srh & 7)

#define _68k_getpc()       (m68k_context.pc - m68k_context.membase)
#define set_special(x)      _68k_spcflags |= x
#define unset_special(x)    _68k_spcflags &= ~x

static __inline__ void _68k_setpc(unsigned mipc)
{
	// this will only work if we are not in CycloneRun
	// (which always appears to be the case as far as I checked
	//  (except exception handlers, which should properly reload PC))
	m68k_context.membase = 0;
	m68k_context.pc = m68k_context.checkpc(mipc);
}

#else
#ifndef __SYMBIAN32__
#define _68k_getpc	m68k_getpc
#define _68k_setpc	m68k_setpc
#define _68k_areg	m68k_areg
#define _68k_dreg	m68k_dreg
#define _68k_incp	m68k_incp
#define _68k_sreg 	uae_regs.s
#define _68k_ispreg 	uae_regs.isp
#define _68k_mspreg 	uae_regs.msp
#define _68k_uspreg 	uae_regs.usp
#define _68k_intmask	uae_regs.intmask
#define _68k_incpc(o)   m68k_incpc(o)
#define _68k_spcflags	uae_regs.spcflags

#include "custom.h"
#include "m68k/uae/newcpu.h"
#endif
#endif

/* PocketUAE */
#ifdef USE_CYCLONE_CORE 
	/* UAE redefine */
	#define m68k_dreg(r,num)  m68k_context.d[num]
	#define m68k_areg(r,num)  m68k_context.a[num]
#else
	/* UAE redefine */
	#define m68k_dreg(r,num) (m68kcontext.dreg[num])
	#define m68k_areg(r,num) (m68kcontext.areg[num])
	
	typedef char flagtype;
	
	/* You can set this to long double to be more accurate. However, the
	   resulting alignment issues will cost a lot of performance in some
	   apps */
	#define USE_LONG_DOUBLE 0
	
	#if USE_LONG_DOUBLE
	typedef long double fptype;
	#else
	typedef double fptype;
	#endif
	
	extern struct regstruct
	{
		uae_u32 regs[16];
		uaecptr  usp,isp,msp;
		uae_u16 sr;
		flagtype t1;
		flagtype t0;
		flagtype s;
		flagtype m;
		flagtype x;
		flagtype stopped;
		int intmask;
	
		uae_u32 pc;
		uae_u8 *pc_p;
		uae_u8 *pc_oldp;
	
		uae_u32 vbr,sfc,dfc;
	
		fptype fp[8];
		fptype fp_result;
	
		uae_u32 fpcr,fpsr,fpiar;
		uae_u32 fpsr_highbyte;
	
		uae_u32 spcflags;
		uae_u32 kick_mask;
	
		uae_u32 prefetch_pc;
		uae_u32 prefetch;
	} regs, lastint_regs;
#endif
/***************/

#endif
