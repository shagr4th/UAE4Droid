#ifdef DREAMCAST
#include <kos.h>
#endif

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

#include "m68k/debug_m68k.h"

#undef dprintf
#if defined(DEBUG_UAE4ALL) || defined(UAE_CONSOLE)
#define dprintf(f,...) printf("%05i: %s: " f "\n",M68KCONTEXT.cycles_counter,__FUNCTION__,##__VA_ARGS__)
#else
#define dprintf(...)
#endif

static unsigned short mimemoriadummy[65536/2];

void clear_fame_mem_dummy(void)
{
	memset((void *)&mimemoriadummy[0],0,65536);
}

struct M68K_CONTEXT micontexto;
struct M68K_PROGRAM miprograma[257];
struct M68K_DATA midato_read_8[257];
struct M68K_DATA midato_read_16[257];
struct M68K_DATA midato_write_8[257];
struct M68K_DATA midato_write_16[257];
static unsigned micontexto_fpa[256];


void init_m68k(void)
{
	m68k_init();
}

static void m68k_exception(unsigned n)
{
	unsigned pc=m68k_get_pc();
	unsigned sr=m68k_get_register(M68K_REG_SR);
	unsigned sp=m68k_get_register(M68K_REG_A7);

	if (!(sr&0x2000))
	{
		unsigned asp=m68k_get_register(M68K_REG_ASP);
		m68k_set_register(M68K_REG_SR,(sr&0x7FF)|0x2000);
		m68k_set_register(M68K_REG_ASP,sp);
		m68k_set_register(M68K_REG_A7,asp-6);
		sp=asp;
	}
	else
		m68k_set_register(M68K_REG_A7,sp-6);

	put_long(sp-4,pc);
	put_word(sp-6,sr);

	m68k_set_register(M68K_REG_PC,m68k_fetch((n*4)+2,0)+(m68k_fetch(n*4,0)<<16));

	//m68k_release_timeslice();
	if (n==4) IO_CYCLE+=4; // adjust for cyclone
	M68KCONTEXT.execinfo&=0x65;
}


void uae_chk_handler(unsigned vector)
{
	unsigned opcode=m68k_fetch(m68k_get_pc(),0);
	unsigned pc=m68k_get_pc();

#ifdef DEBUG_UAE4ALL
	dprintf("INVALID OPCODE 0x%X at PC=0x%X -> ",opcode,pc);
#endif
	if (cloanto_rom && (opcode & 0xF100) == 0x7100) {
#ifdef DEBUG_UAE4ALL
		dprintf("cloanto");
#endif
		_68k_dreg((opcode >> 9) & 7) = (uae_s8)(opcode & 0xFF);
		m68k_set_register(M68K_REG_PC,pc+2);
		return;
	}

	if (opcode == 0x4E7B && get_long (0x10) == 0 && (pc & 0xF80000) == 0xF80000) {
#ifdef DEBUG_UAE4ALL
		dprintf("68020");
#endif
		write_log ("Your Kickstart requires a 68020 CPU. Giving up.\n");
		set_special (SPCFLAG_BRK);
		quit_program = 1;
		return;
	}

	if (opcode == 0xFF0D) {
		if ((pc & 0xF80000) == 0xF80000) {
#ifdef DEBUG_UAE4ALL
			dprintf("dummy");
#endif
			// This is from the dummy Kickstart replacement
			uae_u16 arg = m68k_fetch(pc+2,0);
			m68k_set_register(M68K_REG_PC,pc+4);
			ersatz_perform (arg);
			return;
		}
		else
		if ((pc & 0xFFFF0000) == RTAREA_BASE) {
#ifdef DEBUG_UAE4ALL
			dprintf("stop");
#endif
			// User-mode STOP replacement
			M68KCONTEXT.execinfo|=0x0080;
#ifdef DEBUG_M68K			
			mispcflags|=SPCFLAG_STOP;
#endif
			m68k_set_register(M68K_REG_PC,pc+2);
			return;
		}
	}

	if ((opcode & 0xF000) == 0xA000 && (pc & 0xFFFF0000) == RTAREA_BASE) {
#ifdef DEBUG_UAE4ALL
		dprintf("call");
#endif
		// Calltrap.
#ifdef USE_AUTOCONFIG
		m68k_set_register(M68K_REG_PC,pc+2);
		call_calltrap (opcode & 0xFFF);
#endif
		return;
	}

	if ((opcode & 0xF000) == 0xF000) {
#ifdef DEBUG_UAE4ALL
		dprintf("exp8");
#endif
		// Exception 0xB
		m68k_exception(0xB);
		return;
	}

	if ((opcode & 0xF000) == 0xA000) {
		if ((pc & 0xFFFF0000) == RTAREA_BASE) {
#ifdef DEBUG_UAE4ALL
			dprintf("call +");
#endif
			// Calltrap.
#ifdef USE_AUTOCONFIG
			call_calltrap (opcode & 0xFFF);
#endif
		}
#ifdef DEBUG_UAE4ALL
		dprintf("expA");
#endif
		m68k_exception(0xA);
		return;
	}

#ifdef DEBUG_UAE4ALL
	dprintf("Real invalid");
#endif
	write_log ("Illegal instruction: %04x at %08lx\n", opcode, pc);
	m68k_exception(0x4);
}


void init_memmaps(addrbank* banco)
{
	unsigned i;

	memset(&micontexto,0,sizeof(struct M68K_CONTEXT));

	memset(&micontexto_fpa,0,sizeof(unsigned)*256);

	micontexto_fpa[0x04]=(unsigned)&uae_chk_handler;
//	micontexto_fpa[0x10]=(unsigned)&uae_chk_handler; // FAME BUG !!!
	micontexto.icust_handler = (unsigned int*)&micontexto_fpa;

	micontexto.fetch=(struct M68K_PROGRAM *)&miprograma;
	micontexto.read_byte=(struct M68K_DATA *)&midato_read_8;
	micontexto.read_word=(struct M68K_DATA *)&midato_read_16;
	micontexto.write_byte=(struct M68K_DATA *)&midato_write_8;
	micontexto.write_word=(struct M68K_DATA *)&midato_write_16;

	micontexto.sv_fetch=(struct M68K_PROGRAM *)&miprograma;
	micontexto.sv_read_byte=(struct M68K_DATA *)&midato_read_8;
	micontexto.sv_read_word=(struct M68K_DATA *)&midato_read_16;
	micontexto.sv_write_byte=(struct M68K_DATA *)&midato_write_8;
	micontexto.sv_write_word=(struct M68K_DATA *)&midato_write_16;
	
	micontexto.user_fetch=(struct M68K_PROGRAM *)&miprograma;
	micontexto.user_read_byte=(struct M68K_DATA *)&midato_read_8;
	micontexto.user_read_word=(struct M68K_DATA *)&midato_read_16;
	micontexto.user_write_byte=(struct M68K_DATA *)&midato_write_8;
	micontexto.user_write_word=(struct M68K_DATA *)&midato_write_16;

	micontexto.reset_handler=NULL;
	micontexto.iack_handler=NULL;
	
	for(i=0;i<256;i++)
	{
		unsigned offset=(unsigned)banco->baseaddr;
		unsigned low_addr=(i<<16);
		unsigned high_addr=((i+1)<<16)-1;
		void *data=NULL;
		void *mem_handler_r8=NULL;
		void *mem_handler_r16=NULL;
		void *mem_handler_w8=NULL;
		void *mem_handler_w16=NULL;

		if (offset)
			data=(void *)(offset-low_addr);
		else
		{
			mem_handler_r8=(void *)banco->bget;
			mem_handler_r16=(void *)banco->wget;
			mem_handler_w8=(void *)banco->bput;
			mem_handler_w16=(void *)banco->wput;
		}

		miprograma[i].low_addr=low_addr;
		miprograma[i].high_addr=high_addr;
		miprograma[i].offset=((unsigned)&mimemoriadummy)-low_addr;
		midato_read_8[i].low_addr=low_addr;
		midato_read_8[i].high_addr=high_addr;
		midato_read_8[i].mem_handler=mem_handler_r8;
		midato_read_8[i].data=data;
		midato_read_16[i].low_addr=low_addr;
		midato_read_16[i].high_addr=high_addr;
		midato_read_16[i].mem_handler=mem_handler_r16;
		midato_read_16[i].data=data;
		midato_write_8[i].low_addr=low_addr;
		midato_write_8[i].high_addr=high_addr;
		midato_write_8[i].mem_handler=mem_handler_w8;
		midato_write_8[i].data=data;
		midato_write_16[i].low_addr=low_addr;
		midato_write_16[i].high_addr=high_addr;
		midato_write_16[i].mem_handler=mem_handler_w16;
		midato_write_16[i].data=data;
	}
	miprograma[256].low_addr=(unsigned)-1;
	miprograma[256].high_addr=(unsigned)-1;
	miprograma[256].offset=(unsigned)NULL;
	midato_read_8[256].low_addr=(unsigned)-1;
	midato_read_8[256].high_addr=(unsigned)-1;
	midato_read_8[256].mem_handler=NULL;
	midato_read_8[256].data=NULL;
	midato_read_16[256].low_addr=(unsigned)-1;
	midato_read_16[256].high_addr=(unsigned)-1;
	midato_read_16[256].mem_handler=NULL;
	midato_read_16[256].data=NULL;
	midato_write_8[256].low_addr=(unsigned)-1;
	midato_write_8[256].high_addr=(unsigned)-1;
	midato_write_8[256].mem_handler=NULL;
	midato_write_8[256].data=NULL;
	midato_write_16[256].low_addr=(unsigned)-1;
	midato_write_16[256].high_addr=(unsigned)-1;
	midato_write_16[256].mem_handler=NULL;
	midato_write_16[256].data=NULL;

	m68k_set_context(&micontexto);
}

void map_zone(unsigned addr, addrbank* banco, unsigned realstart)
{
	unsigned offset=(unsigned)banco->baseaddr;
	if (addr>255)
		return;

	unsigned low_addr=(addr<<16);
	unsigned high_addr=((addr+1)<<16)-1;
	m68k_get_context(&micontexto);

#ifdef DEBUG_MAPPINGS
	dprintf("map_zone: 0x%.8X (0x%.8X 0x%.8X)",addr<<16,low_addr,high_addr);
#endif

	if (offset)
	{
#ifdef DEBUG_MAPPINGS
		dprintf(" offset");
#endif
		offset+=((addr-realstart)<<16);
		miprograma[addr].low_addr=low_addr;
		miprograma[addr].high_addr=high_addr;
		miprograma[addr].offset=offset-low_addr;
		midato_read_8[addr].low_addr=low_addr;
		midato_read_8[addr].high_addr=high_addr;
		midato_read_8[addr].mem_handler=NULL;
		midato_read_8[addr].data=(void *)(offset-low_addr);
		midato_read_16[addr].low_addr=low_addr;
		midato_read_16[addr].high_addr=high_addr;
		midato_read_16[addr].mem_handler=NULL;
		midato_read_16[addr].data=(void *)(offset-low_addr);
		midato_write_8[addr].low_addr=low_addr;
		midato_write_8[addr].high_addr=high_addr;
		midato_write_8[addr].mem_handler=NULL;
		midato_write_8[addr].data=(void *)(offset-low_addr);
		midato_write_16[addr].low_addr=low_addr;
		midato_write_16[addr].high_addr=high_addr;
		midato_write_16[addr].mem_handler=NULL;
		midato_write_16[addr].data=(void *)(offset-low_addr);
	}
	else
	{
#ifdef DEBUG_MAPPINGS
		dprintf(" handler");
#endif
		miprograma[addr].low_addr=low_addr;
		miprograma[addr].high_addr=high_addr;
		miprograma[addr].offset=((unsigned)&mimemoriadummy)-low_addr;
		midato_read_8[addr].low_addr=low_addr;
		midato_read_8[addr].high_addr=high_addr;
		midato_read_8[addr].mem_handler=(void*)banco->bget;
		midato_read_8[addr].data=NULL;
		midato_read_16[addr].low_addr=low_addr;
		midato_read_16[addr].high_addr=high_addr;
		midato_read_16[addr].mem_handler=(void*)banco->wget;
		midato_read_16[addr].data=NULL;
		midato_write_8[addr].low_addr=low_addr;
		midato_write_8[addr].high_addr=high_addr;
		midato_write_8[addr].mem_handler=(void*)banco->bput;
		midato_write_8[addr].data=NULL;
		midato_write_16[addr].low_addr=low_addr;
		midato_write_16[addr].high_addr=high_addr;
		midato_write_16[addr].mem_handler=(void*)banco->wput;
		midato_write_16[addr].data=NULL;
	}
	m68k_set_context(&micontexto);
}

