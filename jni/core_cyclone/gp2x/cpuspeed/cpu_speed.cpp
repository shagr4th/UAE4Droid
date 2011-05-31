
/*  CPU_SPEED for GP2X
    Copyright (C) 2005  Hermes/PS2Reality 
	Modified by Vimacs and god_at_hell(LCD-part)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include "cpuctrl.h"


/*  cpuctrl for GP2X
    Copyright (C) 2005  Hermes/PS2Reality 
	Modified by Vimacs using Robster's Code and god_at_hell(LCD)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include <fcntl.h>


/****************************************************************************************************************************************/
// CPU CONTROL
/****************************************************************************************************************************************/
#include "gp2x.h"
#include <sys/mman.h>
#define SYS_CLK_FREQ 7372800

// system registers
static struct 
{
	unsigned short SYSCLKENREG,SYSCSETREG,FPLLVSETREG,DUALINT920,DUALINT940,DUALCTRL940,DISPCSETREG;
}
system_reg;

extern volatile unsigned short *MEM_REG;
unsigned MDIV,PDIV,SCALE;
volatile unsigned *arm940code;

void save_system_regs()
{
	system_reg.SYSCSETREG=MEM_REG[0x91c>>1];
	system_reg.FPLLVSETREG=MEM_REG[0x912>>1];
	system_reg.SYSCLKENREG=MEM_REG[0x904>>1];
	system_reg.DUALINT920=MEM_REG[0x3B40>>1];	
	system_reg.DUALINT940=MEM_REG[0x3B42>>1];
	system_reg.DUALCTRL940=MEM_REG[0x3B48>>1];
	system_reg.DISPCSETREG=MEM_REG[0x924>>1];
}

void load_system_regs()
{
	MEM_REG[0x91c>>1]=system_reg.SYSCSETREG;
	MEM_REG[0x910>>1]=system_reg.FPLLVSETREG;
	MEM_REG[0x3B40>>1]=system_reg.DUALINT920;
	MEM_REG[0x3B42>>1]=system_reg.DUALINT940;
	MEM_REG[0x3B48>>1]=system_reg.DUALCTRL940;
	MEM_REG[0x904>>1]=system_reg.SYSCLKENREG;
	/* Set UPLLSETVREG to 0x4F02, which gives 80MHz */
	MEM_REG[0x0914>>1] = 0x4F02;
	/* Wait for clock change to start */
	while (MEM_REG[0x0902>>1] & 2);
	/* Wait for clock change to be verified */
	while (MEM_REG[0x0916>>1] != 0x4F02);
}

void set_FCLK(unsigned MHZ)
{
	unsigned v;
	unsigned mdiv,pdiv=3,scale=0;
	MHZ*=1000000;
	mdiv=(MHZ*pdiv)/SYS_CLK_FREQ;
	//printf ("Old value = %04X\r",MEM_REG[0x924>>1]," ");
	//printf ("APLL = %04X\r",MEM_REG[0x91A>>1]," ");
	mdiv=((mdiv-8)<<8) & 0xff00;
	pdiv=((pdiv-2)<<2) & 0xfc;
	scale&=3;
	v=mdiv | pdiv | scale;
	MEM_REG[0x910>>1]=v;
}

unsigned get_FCLK()
{
	return MEM_REG[0x910>>1];
}

void set_add_FLCDCLK(int addclock)
{
	//Set LCD controller to use FPLL
	MEM_REG[0x924>>1]= 0x5A00 + ((addclock)<<8); 
	//If you change the initial timing, don't forget to shift your intervall-borders in "cpu_speed.c"
}

void set_add_ULCDCLK(int addclock)
{
	//Set LCD controller to use UPLL
	MEM_REG[0x0924>>1] = 0x8900 + ((addclock)<<8);
	//If you change the initial timing, don't forget to shift your intervall-borders in "cpu_speed.c"
}

unsigned get_LCDClk()
{
	return(MEM_REG[0x0924>>1]);
}

unsigned get_freq_UCLK()
{
	unsigned i;
	unsigned reg,mdiv,pdiv,scale;
	i = MEM_REG[0x900>>1];
	i = ((i >> 7) & 1) ;
	if(i) return 0;
	reg=MEM_REG[0x916>>1];
	mdiv = ((reg & 0xff00) >> 8) + 8;
	pdiv = ((reg & 0xfc) >> 2) + 2;
	scale = reg & 3;	
	return ((SYS_CLK_FREQ * mdiv) / (pdiv << scale));
}

unsigned get_freq_ACLK()
{
	unsigned i;
	unsigned reg,mdiv,pdiv,scale;
	i = MEM_REG[0x900>>1];
	i = ((i >> 8) & 1) ;
	if(i) return 0;
	reg=MEM_REG[0x918>>1];
	mdiv = ((reg & 0xff00) >> 8) + 8;
	pdiv = ((reg & 0xfc) >> 2) + 2;
	scale = reg & 3;
	return ((SYS_CLK_FREQ * mdiv)/(pdiv << scale));
}

unsigned get_freq_920_CLK()
{
	unsigned i;
	unsigned reg,mdiv,pdiv,scale;
	reg=MEM_REG[0x912>>1];
	mdiv = ((reg & 0xff00) >> 8) + 8;
	pdiv = ((reg & 0xfc) >> 2) + 2;
	scale = reg & 3;
	MDIV=mdiv;
	PDIV=pdiv;
	SCALE=scale;
	i = (MEM_REG[0x91c>>1] & 7)+1;
	return ((SYS_CLK_FREQ * mdiv)/(pdiv << scale))/i;
}

unsigned get_freq_940_CLK()
{
	unsigned i;
	unsigned reg,mdiv,pdiv,scale;
	reg=MEM_REG[0x912>>1];
	mdiv = ((reg & 0xff00) >> 8) + 8;
	pdiv = ((reg & 0xfc) >> 2) + 2;
	scale = reg & 3;
	i = ((MEM_REG[0x91c>>1]>>3) & 7)+1;
	return ((SYS_CLK_FREQ * mdiv) / (pdiv << scale))/i;
}

unsigned get_freq_DCLK()
{
	unsigned i;
	unsigned reg,mdiv,pdiv,scale;
	reg=MEM_REG[0x912>>1];
	mdiv = ((reg & 0xff00) >> 8) + 8;
	pdiv = ((reg & 0xfc) >> 2) + 2;
	scale = reg & 3;
	i = ((MEM_REG[0x91c>>1]>>6) & 7)+1;
	return ((SYS_CLK_FREQ * mdiv) / (pdiv << scale))/i;
}

void set_920_Div(unsigned short div)
{
	unsigned short v;
	v = MEM_REG[0x91c>>1] & (~0x3);
	MEM_REG[0x91c>>1] = (div & 0x7) | v; 
}

unsigned short get_920_Div()
{
	return (MEM_REG[0x91c>>1] & 0x7); 
}

void set_940_Div(unsigned short div)
{	
	unsigned short v;
	v = (unsigned short)( MEM_REG[0x91c>>1] & (~(0x7 << 3)));
	MEM_REG[0x91c>>1] = ((div & 0x7) << 3) | v; 
}

unsigned short get_940_Div()
{
	return ((MEM_REG[0x91c>>1] >> 3) & 0x7); 
}

void set_DCLK_Div( unsigned short div )
{
	unsigned short v;
	v = (unsigned short)( MEM_REG[0x91c>>1] & (~(0x7 << 6)));
	MEM_REG[0x91c>>1] = ((div & 0x7) << 6) | v; 
}

unsigned short get_DCLK_Div()
{
	return ((MEM_REG[0x91c>>1] >> 6) & 0x7); 
}

unsigned short Disable_Int_920()
{
	unsigned short ret;
	ret=MEM_REG[0x3B40>>1];
	MEM_REG[0x3B40>>1]=0;
	MEM_REG[0x3B44>>1]=0xffff;	
	return ret;	
}

unsigned short Disable_Int_940()
{
	unsigned short ret;
	ret=MEM_REG[0x3B42>>1];
	MEM_REG[0x3B42>>1]=0;
	MEM_REG[0x3B46>>1]=0xffff;	
	return ret;	
}

unsigned get_state940()
{
	return MEM_REG[0x904>>1];	
}


void Enable_Int_920(unsigned short flag)
{
	MEM_REG[0x3B40>>1]=flag;
}

void Enable_Int_940(unsigned short flag)
{
	MEM_REG[0x3B42>>1]=flag;
}

void Disable_940()
{
	Disable_Int_940();
	MEM_REG[0x3B48>>1]|= (1 << 7);
	MEM_REG[0x904>>1]&=0xfffe;
}

void clock_940_off()
{
	MEM_REG[0x904>>1]&=0xfffe;
}

void clock_940_on()
{
	MEM_REG[0x904>>1]|=1;
}

/****************************************************************************************************************************************/
//  MAIN
/****************************************************************************************************************************************/

unsigned gp2x_nKeys=0;
int oldtime=0;
unsigned char cad[256];
extern unsigned MDIV,PDIV,SCALE;
int oldClockSpeed = -1;

int setGP2XClock(int clockSpeed)
{
	//int cpuMemDev = open("/dev/mem", O_RDWR);
	//MEM_REG=(unsigned short *)mmap(0, 0x10000, PROT_READ|PROT_WRITE, MAP_SHARED,cpuMemDev, 0xc0000000);

	if (oldClockSpeed != clockSpeed && clockSpeed>0)
	{
		int failed = cpu_main(clockSpeed);
		if (!failed)
		{
			oldClockSpeed = clockSpeed;
		}
	}
	
	//if (cpuMemDev)
	//{
	//	close(cpuMemDev);
	//}

	return 0;
}

#include <SDL.h>
#include "menu/menu.h"

int cpu_main(int speed)
{
	//int argv2 = -2;

	if (speed > 320) {return 1;}
	if (speed < 33) {return 1;}

	// notaz: don't have to touch LCD controller, it should use UPLL already
	//set_add_ULCDCLK(argv2);

	printf("changing clock to %iMHz... ", speed); fflush(stdout);
	set_FCLK(speed);
	printf("done\n");

	// sleep a bit more (just in case)
	usleep(100*1000);

	return(0);
}
