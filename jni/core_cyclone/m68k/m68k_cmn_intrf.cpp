#ifdef DREAMCAST
#include<kos.h>
#endif

#define NO_SHORT_EVENTS
#define PROTECT_INFINITE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sysconfig.h"
#include "sysdeps.h"
#include "debug_uae4all.h"
#include "config.h"
#include "uae.h"
#include "options.h"
#include "memorya.h"
#include "custom.h"
#include "autoconf.h"
#include "ersatz.h"
#include "debug.h"
#include "compiler.h"
#include "gui.h"
#include "savestate.h"
#include "blitter.h"
#include "events.h"
#include "sound.h"

#include "m68k/debug_m68k.h"

#ifdef FAME_INTERRUPTS_PATCH
int uae4all_go_interrupt=0;
#endif


#ifdef DEBUG_UAE4ALL
int DEBUG_AHORA=1;
#endif

#ifdef DEBUG_UAE4ALL
void carga(void)
{
	unsigned pc,sr,a0,a1,a2,a3,a4,a5,a6,a7,d0,d1,d2,d3,d4,d5,d6,d7;
	FILE *f=fopen("/tmp/uae4all_guarda","rb");
	if (f)
	{
		fread((void *)&pc,sizeof(unsigned),1,f);
		fread((void *)&sr,sizeof(unsigned),1,f);
		fread((void *)&a0,sizeof(unsigned),1,f);
		fread((void *)&a1,sizeof(unsigned),1,f);
		fread((void *)&a2,sizeof(unsigned),1,f);
		fread((void *)&a3,sizeof(unsigned),1,f);
		fread((void *)&a4,sizeof(unsigned),1,f);
		fread((void *)&a5,sizeof(unsigned),1,f);
		fread((void *)&a6,sizeof(unsigned),1,f);
		fread((void *)&a7,sizeof(unsigned),1,f);
		fread((void *)&d0,sizeof(unsigned),1,f);
		fread((void *)&d1,sizeof(unsigned),1,f);
		fread((void *)&d2,sizeof(unsigned),1,f);
		fread((void *)&d3,sizeof(unsigned),1,f);
		fread((void *)&d4,sizeof(unsigned),1,f);
		fread((void *)&d5,sizeof(unsigned),1,f);
		fread((void *)&d6,sizeof(unsigned),1,f);
		fread((void *)&d7,sizeof(unsigned),1,f);
		fread((void *)chipmemory,1,allocated_chipmem,f);
		fclose(f);
		{
			unsigned char *p=(unsigned char *)chipmemory;
			unsigned i;
			for (i=0;i<allocated_chipmem;i+=2)
			{
				unsigned char t=p[i];
				p[i]=p[i+1];
				p[i+1]=t;
			}
		}
		_68k_areg(0)=a0;
		_68k_areg(1)=a1;
		_68k_areg(2)=a2;
		_68k_areg(3)=a3;
		_68k_areg(4)=a4;
		_68k_areg(5)=a5;
		_68k_areg(6)=a6;
		_68k_areg(7)=a7;
		_68k_dreg(0)=d0;
		_68k_dreg(1)=d1;
		_68k_dreg(2)=d2;
		_68k_dreg(3)=d3;
		_68k_dreg(4)=d4;
		_68k_dreg(5)=d5;
		_68k_dreg(6)=d6;
		_68k_dreg(7)=d7;
		_68k_sreg=sr;
		_68k_setpc(pc);
	}
}
#endif


int m68k_speed=0;
int timeslice_mode=0;
static unsigned cycles_factor=1<<8;
int next_vpos[512];

#ifndef USE_CYCLONE_CORE
extern struct M68K_CONTEXT micontexto;
#endif

unsigned mispcflags=0;

#if !defined(DREAMCAST) || defined(DEBUG_UAE4ALL)
int in_m68k_go = 0;
#endif

static int do_specialties (int cycles)
{
    if (mispcflags & SPCFLAG_COPPER)
    {
#ifdef DEBUG_M68K
	dbg("do_specialties -> do_copper");    
#endif
        do_copper ();
    }

    /*n_spcinsns++;*/
    while ((mispcflags & SPCFLAG_BLTNASTY) && cycles > 0) {
        int c = blitnasty();
        if (!c) {
            cycles -= 2 * CYCLE_UNIT;
            if (cycles < CYCLE_UNIT)
                cycles = 0;
            c = 1;
        }
#ifdef DEBUG_M68K
	dbgf("do_specialties -> do_cycles BLTNASTY %i\n",c);
#endif
        do_cycles(c * CYCLE_UNIT);
        if (mispcflags & SPCFLAG_COPPER)
	{
#ifdef DEBUG_M68K
	    dbg("do_specialties -> do_copper BLTNASTY");
#endif
            do_copper ();
	}
    }

#ifdef DEBUG_M68K
    while (M68KCONTEXT.execinfo & 0x0080) {
	if (mispcflags & SPCFLAG_BRK)
		break;
	else
		if (M68KCONTEXT.execinfo & 0x0080)
		{
			int intr = intlev ();
			if (intr != -1 && intr > _68k_intmask)
			{
				M68KCONTEXT.execinfo &= 0xFF7F;
				break;
			}
		}
	dbg("CPU STOPPED !");
        do_cycles(4 * CYCLE_UNIT);
        if (mispcflags & SPCFLAG_COPPER)
	{
	    dbg("do_specialties -> do_copper STOPPED");
            do_copper ();
	}
    }
    unset_special (SPCFLAG_STOP);
#endif

#if 0 // !defined(FAME_INTERRUPTS_SECURE_PATCH) && defined(FAME_INTERRUPTS_PATCH)
    if (uae4all_go_interrupt)
    {
	/*if (verb)*/ printf("uae4all_go_interrupt (s): %02x\n", uae4all_go_interrupt);
	M68KCONTEXT.interrupts[0]=uae4all_go_interrupt;
	M68KCONTEXT.execinfo&=0xFF67;
	uae4all_go_interrupt=0;
	m68k_irq_update(0);
    }
#endif

#ifdef SPECIAL_DEBUG_INTERRUPTS
    if ((mispcflags & SPCFLAG_DOINT)&&(!(mispcflags & SPCFLAG_INT))) {
	int intr = intlev ();
#ifdef DEBUG_INTERRUPTS_EXTRA
	dbgf("DOINT : intr = %i, intmask=%i\n", intr, _68k_intmask);
#endif
	unset_special (SPCFLAG_DOINT);
	if (intr != -1 && intr > _68k_intmask) {
		M68KCONTEXT.execinfo&=0xFF6F;
		m68k_raise_irq(intr,M68K_AUTOVECTORED_IRQ);
//		m68k_emulate(0);
	}
    }
    if (mispcflags & SPCFLAG_INT) {
#ifdef DEBUG_INTERRUPTS_EXTRA
	dbg("ESTAMOS EN INT -> PASAMOS A DOINT");
#endif
	unset_special (SPCFLAG_INT);
	set_special (SPCFLAG_DOINT);
    }
#endif

    if (mispcflags & SPCFLAG_BRK) {
        unset_special (SPCFLAG_BRK);
        return 1;
    }
    return 0;
}

static void uae4all_reset(void)
{
    int i;
#ifndef USE_CYCLONE_CORE
    m68k_set_context(&micontexto);
#endif
    m68k_reset();
    for(i=1;i<8;i++)
#if defined(DEBUG_INTERRUPTS)
    	M68KCONTEXT.interrupts[i]=0xFF;
#else
    	M68KCONTEXT.interrupts[i]=0x18+i;
#endif
    M68KCONTEXT.interrupts[0]=0;
    m68k_irq_update(0);
    mispcflags=0;
    _68k_areg(7) = get_long (0x00f80000);
    _68k_setpc(get_long (0x00f80004));
    //_68k_sreg = 0x2700; // already done by m68k_reset()
    mispcflags=0;
#ifdef DEBUG_FRAMERATE
    uae4all_update_time();
#endif
}

static void m68k_run (void)
{
	uae4all_reset ();
	unsigned cycles, cycles_actual=M68KCONTEXT.cycles_counter;
	for (;;) {
		while (pause_program > 0)
		{
			usleep(200000);
		}
        
#ifdef DEBUG_M68K
		dbg_cycle(m68k_fetch(m68k_get_pc(),0));
		m68k_emulate(1);

#else
		uae4all_prof_start(0);
		cycles = nextevent - currcycle;
		switch (timeslice_mode) {
			case 3:  cycles>>=6; break;
			case 2:  cycles>>=7; break;
			case 1:  cycles=(cycles>>8)+(cycles>>9); break;
			default: cycles>>=8; break;
		}
#ifdef DEBUG_TIMESLICE
		unsigned ts=cycles;
#endif
#if defined(FAME_INTERRUPTS_SECURE_PATCH) && defined(FAME_INTERRUPTS_PATCH)
		if (uae4all_go_interrupt)
			m68k_emulate(FAME_INTERRUPTS_PATCH);
		else
#endif
			m68k_emulate(cycles);
		uae4all_prof_end(0);
#endif
#if 0 // def FAME_INTERRUPTS_PATCH
		if (uae4all_go_interrupt)
		{
			/*if (verb)*/ printf("uae4all_go_interrupt: %02x\n", uae4all_go_interrupt);
			M68KCONTEXT.interrupts[0]=uae4all_go_interrupt;
			M68KCONTEXT.execinfo&=0xFF67;
			uae4all_go_interrupt=0;
			m68k_irq_update(0);
		}
#endif
#ifdef DEBUG_M68K

		if (M68KCONTEXT.execinfo & 0x0080)
			mispcflags|=SPCFLAG_STOP;
#endif
                uae4all_prof_start(1);

		//cycles=((unsigned)(((double)(M68KCONTEXT.cycles_counter-cycles_actual))*cycles_factor))<<8;
		cycles=(M68KCONTEXT.cycles_counter-cycles_actual) * cycles_factor;

#ifdef DEBUG_INTERRUPTS
		dbgf("cycles=%i (%i) -> PC=%.6x\n",cycles>>8,nextevent - currcycle, _68k_getpc());
#endif


#ifdef NO_SHORT_EVENTS
#ifdef PROTECT_INFINITE
		unsigned cuentalo=0;
#endif
		do{
#endif
			do_cycles(cycles);
			if (mispcflags)
				if (do_specialties (cycles))
					return;
#ifndef DEBUG_M68K
#ifdef NO_SHORT_EVENTS
			cycles=2048;
#ifdef PROTECT_INFINITE
			cuentalo++;
			if (cuentalo>1024)
			{
				quit_program=2;
				return;
			}
#endif
		}while((nextevent - currcycle)<=2048);
#endif
		cycles_actual=M68KCONTEXT.cycles_counter;
#endif
                uae4all_prof_end(1);
	}
}


void m68k_go (int may_quit)
{
    gui_purge_events();
#if !defined(DREAMCAST) || defined(DEBUG_UAE4ALL)
    if (in_m68k_go || !may_quit) {
#ifdef DEBUG_UAE4ALL
        puts("Bug! m68k_go is not reentrant.\n");
#endif
        return;
    }

    in_m68k_go++;
#endif
    quit_program = 2;
    for (;;) {
		if (quit_program > 0) {
            if (quit_program == 1)
                break;
            quit_program = 0;
            reset_all_systems ();
            customreset ();
	    check_prefs_changed_cpu ();
	    sound_default_evtime ();
            /* We may have been restoring state, but we're done now.  */
            handle_active_events ();
            if (mispcflags)
                do_specialties (0);
        }

        m68k_run();
    }
#if !defined(DREAMCAST) || defined(DEBUG_UAE4ALL)
    in_m68k_go--;
#endif
#ifdef DEBUG_UAE4ALL
    puts("BYE?");
#endif
}

void check_prefs_changed_cpu (void)
{
	int i;

	for(i=0;i<512;i++)
		next_vpos[i]=i+1;

	switch(m68k_speed)
	{
		case 6:
			cycles_factor=(unsigned)(1.86*256);
			if (!(beamcon0 & 0x20)) break;
			for(i=0;i<4;i++)
				next_vpos[i]=4;
			next_vpos[4]=5;
			for(i=5;i<20;i++)
				next_vpos[i]=20;
			for(i=20;i<280;i++)
				next_vpos[i]=i+1;
			for(i=280;i<306;i++)
				next_vpos[i]=306;
			for(i=306;i<311;i++)
				next_vpos[i]=i+1;
			for(i=311;i<512;i++)
				next_vpos[i]=510;
			break;
		case 5:
		case 4:
			cycles_factor=(unsigned)(4.0/3.0*256);
			if (!(beamcon0 & 0x20)) break;
			for(i=0;i<4;i++)
				next_vpos[i]=4;
			next_vpos[4]=5;
			for(i=5;i<40;i++)
				next_vpos[i]=40;
			for(i=40;i<280;i++)
				next_vpos[i]=i+1;
			for(i=280;i<312;i++)
				next_vpos[i]=510;
			break;
		case 3:
		case 2:
			cycles_factor=(unsigned)(7.0/6.0*256);
			if (!(beamcon0 & 0x20)) break;
			for(i=0;i<4;i++)
				next_vpos[i]=4;
			next_vpos[4]=5;
			for(i=5;i<20;i++)
				next_vpos[i]=20;
			for(i=20;i<280;i++)
				next_vpos[i]=i+1;
			for(i=280;i<306;i++)
				next_vpos[i]=306;
			for(i=306;i<311;i++)
				next_vpos[i]=i+1;
			for(i=311;i<512;i++)
				next_vpos[i]=510;
			break;
		case 1:
		default:
			cycles_factor=1*256;
			break;

	}
	next_vpos[511]=0;
}
