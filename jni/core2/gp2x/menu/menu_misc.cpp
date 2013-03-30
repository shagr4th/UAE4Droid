#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "menu.h"

#include <sys/stat.h>
#include <unistd.h>
#include<dirent.h>

#include "sysconfig.h"
#include "sysdeps.h"
#include "uae.h"
#include "options.h"
#include "sound.h"
#include <SDL.h>
#include "gp2x.h"
#include <SDL_ttf.h>
#include "custom.h"
#include "menu_config.h"

static char *text_str_status_line="Status ln";

char *text_str_misc_separator="----------------------------------";
static char *text_str_misc_title=    "            Miscellanous         -";
static char *text_str_sound="Sound";
static char *text_str_fast="fast";
static char *text_str_accurate="accurate";
static char *text_str_off="off";
static char *text_str_sndrate="Sound rate";
static char *text_str_44k="44k";
static char *text_str_32k="32k";
static char *text_str_22k="22k";
static char *text_str_11k="11k";
static char *text_str_8k="8k";
static char *text_str_stylus_offset="StylusOffset";
static char *text_str_0px="0px";
static char *text_str_1px="1px";
static char *text_str_3px="3px";
static char *text_str_5px="5px";
static char *text_str_8px="8px";
static char *text_str_tap_delay="Tap delay";
static char *text_str_normal="normal";
static char *text_str_short="short";
static char *text_str_none="no";
static char *text_str_mouse_multiplier="Mouse speed";
static char *text_str_025x=".25";
static char *text_str_05x=".5";
static char *text_str_1x="1x";
static char *text_str_2x="2x";
static char *text_str_4x="4x";
int menuMisc = 0;

extern int kickstart;
extern int sound_rate;

static void draw_miscMenu(int c)
{
	int leftMargin=3;
	int tabstop1 = 17;
	int tabstop2 = 19;
	int tabstop3 = 21;
	int tabstop4 = 23;
	int tabstop5 = 25;
	int tabstop6 = 27;
	int tabstop7 = 29;
	int tabstop8 = 31;
	int tabstop9 = 33;

	int menuLine = 3;
	static int b=0;
	int bb=(b%6)/3;
	SDL_Rect r;
	extern SDL_Surface *text_screen;
	char cpuSpeed[8];
	r.x=80-64; r.y=0; r.w=110+64+64; r.h=240;

	text_draw_background();
	text_draw_window(2,2,40,30,text_str_misc_title);

	// 1
	if (menuMisc == 0 && bb)
		write_text_inv(3, menuLine, "Return to main menu");
	else
		write_text(3, menuLine, "Return to main menu");

	menuLine++;
	write_text(leftMargin,menuLine,text_str_misc_separator);
	menuLine++;

	// 2
	write_text(leftMargin,menuLine,"CPU");
	if ((mainMenu_CPU_model==0)&&((menuMisc!=1)||(bb)))
	  write_text_inv(tabstop1,menuLine,"68000");
	else
	  write_text(tabstop1,menuLine,"68000");
	
	if ((mainMenu_CPU_model==1)&&((menuMisc!=1)||(bb)))
	  write_text_inv(tabstop4,menuLine,"68020");
	else
	  write_text(tabstop4,menuLine,"68020");

	// 3
	menuLine+=2;
	write_text(leftMargin,menuLine,"Chipset");
	
	if ((mainMenu_chipset==0)&&((menuMisc!=2)||(bb)))
		write_text_inv(tabstop1,menuLine,"OCS");
	else
		write_text(tabstop1,menuLine,"OCS");
	
	if ((mainMenu_chipset==1)&&((menuMisc!=2)||(bb)))
		write_text_inv(tabstop3,menuLine,"ECS");
	else
		write_text(tabstop3,menuLine,"ECS");
	
	if ((mainMenu_chipset==2)&&((menuMisc!=2)||(bb)))
		write_text_inv(tabstop5,menuLine,"AGA");
	else
		write_text(tabstop5,menuLine,"AGA");

	// 4
	menuLine+=2;
	write_text(leftMargin,menuLine,"Kickstart");
	if ((kickstart==0)&&((menuMisc!=3)||(bb)))
		write_text_inv(tabstop1,menuLine,"1.2");
	else
		write_text(tabstop1,menuLine,"1.2");

	if ((kickstart==1)&&((menuMisc!=3)||(bb)))
		write_text_inv(tabstop3,menuLine,"1.3");
	else
		write_text(tabstop3,menuLine,"1.3");

	if ((kickstart==2)&&((menuMisc!=3)||(bb)))
		write_text_inv(tabstop5,menuLine,"2.0");
	else
		write_text(tabstop5,menuLine,"2.0");

	if ((kickstart==3)&&((menuMisc!=3)||(bb)))
		write_text_inv(tabstop7,menuLine,"3.1");
	else
		write_text(tabstop7,menuLine,"3.1");

	// 5
	menuLine+=2;
	write_text(leftMargin,menuLine,"CPU Speed");
	if ((mainMenu_CPU_speed==0)&&((menuMisc!=4)||(bb)))
		write_text_inv(tabstop1,menuLine,"500");
	else
		write_text(tabstop1,menuLine,"500");

	if ((mainMenu_CPU_speed==1)&&((menuMisc!=4)||(bb)))
		write_text_inv(tabstop3,menuLine,"5T");
	else
		write_text(tabstop3,menuLine,"5T");

	if ((mainMenu_CPU_speed==2)&&((menuMisc!=4)||(bb)))
		write_text_inv(tabstop4+1,menuLine,"1200");
	else
		write_text(tabstop4+1,menuLine,"1200");

	if ((mainMenu_CPU_speed==3)&&((menuMisc!=4)||(bb)))
		write_text_inv(tabstop7,menuLine,"12T");
	else
		write_text(tabstop7,menuLine,"12T");

	if ((mainMenu_CPU_speed==4)&&((menuMisc!=4)||(bb)))
		write_text_inv(tabstop9,menuLine,"12T2");
	else
		write_text(tabstop9,menuLine,"12T2");	

	menuLine++;
	write_text(leftMargin,menuLine,text_str_misc_separator);
	menuLine++;

	// 6
	write_text(leftMargin,menuLine,text_str_sound);
	if ((mainMenu_sound==0)&&((menuMisc!=5)||(bb)))
		write_text_inv(tabstop1,menuLine,text_str_off);
	else
		write_text(tabstop1,menuLine,text_str_off);

	if ((mainMenu_sound==1)&&((menuMisc!=5)||(bb)))
		write_text_inv(tabstop3,menuLine,text_str_fast);
	else
		write_text(tabstop3,menuLine,text_str_fast);

	if ((mainMenu_sound==2)&&((menuMisc!=5)||(bb)))
		write_text_inv(tabstop5+1,menuLine,text_str_accurate);
	else
		write_text(tabstop5+1,menuLine,text_str_accurate);

	// 7
	menuLine+=2;
	write_text(leftMargin,menuLine,text_str_sndrate);

	if ((sound_rate==8000)&&((menuMisc!=6)||(bb)))
		write_text_inv(tabstop1,menuLine,text_str_8k);
	else
		write_text(tabstop1,menuLine,text_str_8k);

	if ((sound_rate==11025)&&((menuMisc!=6)||(bb)))
		write_text_inv(tabstop3,menuLine,text_str_11k);
	else
		write_text(tabstop3,menuLine,text_str_11k);

	if ((sound_rate==22050)&&((menuMisc!=6)||(bb)))
		write_text_inv(tabstop5,menuLine,text_str_22k);
	else
		write_text(tabstop5,menuLine,text_str_22k);

	if ((sound_rate==32000)&&((menuMisc!=6)||(bb)))
		write_text_inv(tabstop7,menuLine,text_str_32k);
	else
		write_text(tabstop7,menuLine,text_str_32k);

	if ((sound_rate==44100)&&((menuMisc!=6)||(bb)))
		write_text_inv(tabstop9,menuLine,text_str_44k);
	else
		write_text(tabstop9,menuLine,text_str_44k);

	// 8
	menuLine+=2;
	write_text(leftMargin,menuLine,"Pandora CPU-Speed");
	snprintf((char*)cpuSpeed, 8, "%d",mainMenu_cpuSpeed);
	if ((menuMisc!=7)||(bb))
		write_text_inv(tabstop4-1,menuLine,cpuSpeed);
	else
		write_text(tabstop4-1,menuLine,cpuSpeed);
	write_text(tabstop6-1,menuLine,"MHz");

	menuLine++;
	write_text(leftMargin,menuLine,text_str_misc_separator);
	menuLine++;

	// 9
	write_text(leftMargin,menuLine,"Control config");

	if ((mainMenu_joyConf==0)&&((menuMisc!=8)||(bb)))
		write_text_inv(tabstop5,menuLine,"1");
	else
		write_text(tabstop5,menuLine,"1");

	if ((mainMenu_joyConf==1)&&((menuMisc!=8)||(bb)))
		write_text_inv(tabstop6,menuLine,"2");
	else
		write_text(tabstop6,menuLine,"2");

	if ((mainMenu_joyConf==2)&&((menuMisc!=8)||(bb)))
		write_text_inv(tabstop7,menuLine,"3");
	else
		write_text(tabstop7,menuLine,"3");

	if ((mainMenu_joyConf==3)&&((menuMisc!=8)||(bb)))
		write_text_inv(tabstop8,menuLine,"4");
	else
		write_text(tabstop8,menuLine,"4");

	menuLine+=2;
	if (mainMenu_joyConf==0) write_text(5,menuLine,"A=Autofire X=Fire Y=Space B=2nd");
	else if (mainMenu_joyConf==1) write_text(5,menuLine,"A=Fire X=Autofire Y=Space B=2nd");
	else if (mainMenu_joyConf==2) write_text(5,menuLine,"A=Autofire X=Jump Y=Fire B=2nd");
	else if (mainMenu_joyConf==3) write_text(5,menuLine,"A=Fire X=Jump Y=Autofire B=2nd");

	// 10
	menuLine+=2;
	write_text(leftMargin,menuLine,"Autofire Rate");

	if ((mainMenu_autofireRate==12)&&((menuMisc!=9)||(bb)))
		write_text_inv(tabstop3-2,menuLine,"Light");
	else
		write_text(tabstop3-2,menuLine,"Light");

	if ((mainMenu_autofireRate==8)&&((menuMisc!=9)||(bb)))
		write_text_inv(tabstop6-2,menuLine,"Medium");
	else
		write_text(tabstop6-2,menuLine,"Medium");

	if ((mainMenu_autofireRate==4)&&((menuMisc!=9)||(bb)))
		write_text_inv(tabstop9-1,menuLine,"Heavy");
	else
		write_text(tabstop9-1,menuLine,"Heavy");

	// 11
	menuLine+=2;
	write_text(leftMargin, menuLine,text_str_status_line);
	if ((!mainMenu_showStatus)&&((menuMisc!=10)||(bb)))
		write_text_inv(tabstop1,menuLine, "Off");
	else
		write_text(tabstop1, menuLine, "Off");
	if ((mainMenu_showStatus)&&((menuMisc!=10)||(bb)))
		write_text_inv(tabstop3, menuLine,"On");
	else
		write_text(tabstop3, menuLine,"On");

	menuLine++;
	write_text(leftMargin,menuLine,text_str_misc_separator);
	menuLine++;

	// 12
	write_text(leftMargin,menuLine,text_str_mouse_multiplier);

	if ((mainMenu_mouseMultiplier==25)&&((menuMisc!=11)||(bb)))
		write_text_inv(tabstop1,menuLine,text_str_025x);
	else
		write_text(tabstop1,menuLine,text_str_025x);

	if ((mainMenu_mouseMultiplier==50)&&((menuMisc!=11)||(bb)))
		write_text_inv(tabstop3,menuLine,text_str_05x);
	else
		write_text(tabstop3,menuLine,text_str_05x);

	if ((mainMenu_mouseMultiplier==1)&&((menuMisc!=11)||(bb)))
		write_text_inv(tabstop5,menuLine,text_str_1x);
	else
		write_text(tabstop5,menuLine,text_str_1x);

	if ((mainMenu_mouseMultiplier==2)&&((menuMisc!=11)||(bb)))
		write_text_inv(tabstop7,menuLine,text_str_2x);
	else
		write_text(tabstop7,menuLine,text_str_2x);

	if ((mainMenu_mouseMultiplier==4)&&((menuMisc!=11)||(bb)))
		write_text_inv(tabstop9,menuLine,text_str_4x);
	else
		write_text(tabstop9,menuLine,text_str_4x);

	// 13
	menuLine+=2;
	write_text(leftMargin,menuLine,text_str_stylus_offset);

	if ((mainMenu_stylusOffset==0)&&((menuMisc!=12)||(bb)))
		write_text_inv(tabstop1,menuLine,text_str_0px);
	else
		write_text(tabstop1,menuLine,text_str_0px);

	if ((mainMenu_stylusOffset==2)&&((menuMisc!=12)||(bb)))
		write_text_inv(tabstop3,menuLine,text_str_1px);
	else
		write_text(tabstop3,menuLine,text_str_1px);

	if ((mainMenu_stylusOffset==6)&&((menuMisc!=12)||(bb)))
		write_text_inv(tabstop5,menuLine,text_str_3px);
	else
		write_text(tabstop5,menuLine,text_str_3px);

	if ((mainMenu_stylusOffset==10)&&((menuMisc!=12)||(bb)))
		write_text_inv(tabstop7,menuLine,text_str_5px);
	else
		write_text(tabstop7,menuLine,text_str_5px);

	if ((mainMenu_stylusOffset==16)&&((menuMisc!=12)||(bb)))
		write_text_inv(tabstop9,menuLine,text_str_8px);
	else
		write_text(tabstop9,menuLine,text_str_8px);

	// 14
	menuLine+=2;
	write_text(leftMargin,menuLine,text_str_tap_delay);

	if ((mainMenu_tapDelay==10)&&((menuMisc!=13)||(bb)))
		write_text_inv(tabstop1,menuLine,text_str_normal);
	else
		write_text(tabstop1,menuLine,text_str_normal);

	if ((mainMenu_tapDelay==5)&&((menuMisc!=13)||(bb)))
		write_text_inv(tabstop5,menuLine,text_str_short);
	else
		write_text(tabstop5,menuLine,text_str_short);

	if ((mainMenu_tapDelay==2)&&((menuMisc!=13)||(bb)))
		write_text_inv(tabstop9,menuLine,text_str_none);
	else
		write_text(tabstop9,menuLine,text_str_none);

	menuLine++;
	write_text(leftMargin,menuLine,text_str_misc_separator);
	menuLine++;

	text_flip();
	b++;
}

static int key_miscMenu(int *c)
{
	int end=0;
	int left=0, right=0, up=0, down=0, hit0=0, hit1=0;
	SDL_Event event;

	while (SDL_PollEvent(&event) > 0)
	{
		if (event.type == SDL_KEYDOWN)
		{
			uae4all_play_click();
			switch(event.key.keysym.sym)
			{
			case SDLK_RIGHT: right=1; break;
			case SDLK_LEFT: left=1; break;
			case SDLK_UP: up=1; break;
			case SDLK_DOWN: down=1; break;
			case SDLK_PAGEDOWN: hit0=1; break;
			case SDLK_HOME: hit0=1; break;
			case SDLK_LALT: hit1=1; break;
			case SDLK_END: hit0=1; break;
			case SDLK_PAGEUP: hit0=1;
			}
		}

		if (hit0)
		{
			end = -1;
		}
		else if (hit1)
		{
			end = -1;
		}
		else if (up)
		{
			if (menuMisc==0) menuMisc=13;
			else menuMisc--;
		}
		else if (down)
		{
			if (menuMisc==13) menuMisc=0;
			else menuMisc++;
		}
		switch (menuMisc)
		{
			case 1:
				if (left)
				{
				    if (mainMenu_CPU_model > 0)
				       mainMenu_CPU_model--;
				    else
				       mainMenu_CPU_model=1;
				}
				else if (right)
				{
				    if (mainMenu_CPU_model < 1)
				       mainMenu_CPU_model++;
				    else
				       mainMenu_CPU_model=0;
				}
				UpdateCPUModelSettings();
				break;
			case 2:
				if (left)
				{
					if (mainMenu_chipset > 0)
						mainMenu_chipset--;
					else
						mainMenu_chipset=2;
				}
				else if (right)
				{
					if (mainMenu_chipset < 2)
						mainMenu_chipset++;
					else
						mainMenu_chipset=0;
				}
				UpdateChipsetSettings();
				break;
			case 3:
				if (left)
				{
					if (kickstart>0)
						kickstart--;
					else
						kickstart=3;
				}
				else if (right)
				{
					if (kickstart<3)
						kickstart++;
					else
						kickstart=0;
				}
				break;
			case 4:
				if (left)
				{
					if (mainMenu_CPU_speed>0)
						mainMenu_CPU_speed--;
					else
						mainMenu_CPU_speed=4;
				}
				else if (right)
				{
					if (mainMenu_CPU_speed<4)
						mainMenu_CPU_speed++;
					else
						mainMenu_CPU_speed=0;
				}
				break;
			case 5:
				if (left)
				{
					if (mainMenu_sound == 1)
						mainMenu_sound = 0;
					else if (mainMenu_sound == 2)
						mainMenu_sound = 1;
					else if (mainMenu_sound == 0)
						mainMenu_sound = 2;
				}
				else if (right)
				{
					if (mainMenu_sound == 2)
						mainMenu_sound = 0;
					else if (mainMenu_sound == 0)
						mainMenu_sound = 1;
					else if (mainMenu_sound == 1)
						mainMenu_sound = 2;
				}
				break;
			case 6:
				if ((left)||(right))
				{
					static int rates[] = { 8000, 11025, 22050, 32000, 44100 };
					int sel;
					for (sel = 0; sel < sizeof(rates) / sizeof(rates[0]); sel++)
						if (rates[sel] == sound_rate) break;
					sel += left ? -1 : 1;
					if (sel < 0) sel = 4;
					if (sel > 4) sel = 0;
					sound_rate = rates[sel];
				}
				break;
			case 7:
				if(left)
					mainMenu_cpuSpeed-=10;
				else if(right)
					mainMenu_cpuSpeed+=10;
				break;
			case 8:
				if (left)
				{
					if (mainMenu_joyConf>0)
						mainMenu_joyConf--;
					else
						mainMenu_joyConf=3;
				}
				else if (right)
				{
					if (mainMenu_joyConf<3)
						mainMenu_joyConf++;
					else
						mainMenu_joyConf=0;
				}
 				break;
			case 9:
				if(left)
				{
					if(mainMenu_autofireRate==4)
						mainMenu_autofireRate=8;
					else if(mainMenu_autofireRate==8)
						mainMenu_autofireRate=12;
					else
						mainMenu_autofireRate=4;
				}
				else if (right)
				{
					if(mainMenu_autofireRate==4)
						mainMenu_autofireRate=12;
					else if(mainMenu_autofireRate==8)
						mainMenu_autofireRate=4;
					else
						mainMenu_autofireRate=8;
				}
 				break;
			case 10:
				if ((left)||(right))
					mainMenu_showStatus=!mainMenu_showStatus;
				break;
			case 11:
				if (left)
				{
					if (mainMenu_mouseMultiplier == 4)
						mainMenu_mouseMultiplier = 2;
					else if (mainMenu_mouseMultiplier == 2)
						mainMenu_mouseMultiplier = 1;
					else if (mainMenu_mouseMultiplier == 1)
						mainMenu_mouseMultiplier = 50;
					else if (mainMenu_mouseMultiplier == 50)
						mainMenu_mouseMultiplier = 25;
					else
						mainMenu_mouseMultiplier = 4;
				}
				else if (right)
				{
					if (mainMenu_mouseMultiplier == 4)
						mainMenu_mouseMultiplier = 25;
					else if (mainMenu_mouseMultiplier == 2)
						mainMenu_mouseMultiplier = 4;
					else if (mainMenu_mouseMultiplier == 1)
						mainMenu_mouseMultiplier = 2;
					else if (mainMenu_mouseMultiplier == 50)
						mainMenu_mouseMultiplier = 1;
					else
						mainMenu_mouseMultiplier = 50;
				}
				break;
			case 12:
				if (left)
				{
					if (mainMenu_stylusOffset == 0)
						mainMenu_stylusOffset = 16;
					else if (mainMenu_stylusOffset == 2)
						mainMenu_stylusOffset = 0;
					else if (mainMenu_stylusOffset == 6)
						mainMenu_stylusOffset = 2;
					else if (mainMenu_stylusOffset == 10)
						mainMenu_stylusOffset = 6;
					else
						mainMenu_stylusOffset = 10;
				}
				else if (right)
				{
					if (mainMenu_stylusOffset == 0)
						mainMenu_stylusOffset = 2;
					else if (mainMenu_stylusOffset == 2)
						mainMenu_stylusOffset = 6;
					else if (mainMenu_stylusOffset == 6)
						mainMenu_stylusOffset = 10;
					else if (mainMenu_stylusOffset == 10)
						mainMenu_stylusOffset = 16;
					else
						mainMenu_stylusOffset = 0;
				}
				break;
			case 13:
				if (left)
				{
					if (mainMenu_tapDelay == 10)
						mainMenu_tapDelay = 2;
					else if (mainMenu_tapDelay == 5)
						mainMenu_tapDelay = 10;
					else
						mainMenu_tapDelay = 5;
				}
				else if (right)
				{
					if (mainMenu_tapDelay == 10)
						mainMenu_tapDelay = 5;
					else if (mainMenu_tapDelay == 5)
						mainMenu_tapDelay = 2;
					else
						mainMenu_tapDelay = 10;
				}
				break;
		}
	}

	return end;
}

static void raise_miscMenu()
{
	int i;

	text_draw_background();
	text_flip();
	for(i=0;i<10;i++)
	{
		text_draw_background();
		text_draw_window(80-64,(10-i)*24,160+64+64,220,text_str_misc_title);
		text_flip();
	}
}

static void unraise_miscMenu()
{
	int i;

	for(i=9;i>=0;i--)
	{
		text_draw_background();
		text_draw_window(80-64,(10-i)*24,160+64+64,220,text_str_misc_title);
		text_flip();
	}
	text_draw_background();
	text_flip();
}

int run_menuMisc()
{
	SDL_Event event;
	SDL_Delay(150);
	while(SDL_PollEvent(&event))
		SDL_Delay(10);
	int end=0, c=0;
	raise_miscMenu();
	while(!end)
	{
		draw_miscMenu(c);
		end=key_miscMenu(&c);
	}
	set_joyConf();
	unraise_miscMenu();
	return end;
}
