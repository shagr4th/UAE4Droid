#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "menu.h"
#include "sysconfig.h"
#include "sysdeps.h"
#include "uae.h"
#include "options.h"
#include "sound.h"

#include "gp2x.h"
static int norelaunchGP2XMenu = 0;

//#include "gp2xutil.h"
#include "cpuspeed/cpuctrl.h"

int init_sound(void);
void gp2x_stop_sound(void);

extern int emulating;
extern void setBatteryLED(int);
extern int flashLED;
extern int gp2xClockSpeed;
extern int sound_rate;
extern int timeslice_mode;
extern int skipintro;

static char *text_str_title=    "----- UAE4ALL GP2X ------";
static char *text_str_load=     "Select Image Disk (L)";
static char *text_str_flash_led="Drive LED";
static char *text_str_throttle= "System clock";
static char *text_str_sync_trsh="Sync threshold";
static char *text_str_cpu_clock="GP2X Overclock";
static char *text_str_frameskip="Frameskip";
static char *text_str_videomode="Video mode";
static char *text_str_0="0";
static char *text_str_1="1";
static char *text_str_2="2";
static char *text_str_3="3";
static char *text_str_4="4";
static char *text_str_5="5";
static char *text_str_mouse_multiplier="Mouse speed";
static char *text_str_1x="1x";
static char *text_str_2x="2x";
static char *text_str_3x="3x";
static char *text_str_4x="4x";
static char *text_str_5x="5x";
static char *text_str_44k="44k";
static char *text_str_32k="32k";
static char *text_str_22k="22k";
static char *text_str_11k="11k";
static char *text_str_8k="8k";
static char *text_str_100p="100";
static char *text_str_83p="83";
static char *text_str_75p="75";
static char *text_str_50p="50";
static char *text_str_25p="25";
static char *text_str_pal="PAL";
static char *text_str_ntsc="NTSC";
static char *text_str_auto="auto";
static char *text_str_eject="Eject DF1 (X)";
//static char *text_str_gp2x_clock="Clock MHz";
static char *text_str_gp2x_clock_none="none";
static char *text_str_gp2x_clock_200="200";
static char *text_str_gp2x_clock_250="250";
static char *text_str_gp2x_clock_275="275";
static char *text_str_sound="Sound";
static char *text_str_on="on";
static char *text_str_off="off";
static char *text_str_faked="faked";
static char *text_str_status_line="Status ln";
static char *text_str_separator="--------------------------------";
static char *text_str_reset="Reset  R";
static char *text_str_run=  "Run";
static char *text_str_exit= "Exit";
static char *text_str_autosave="Autosave";
static char *text_str_sndrate="Sound rate";

int mainMenu_throttle=0;
#if !defined(DEBUG_UAE4ALL) && !defined(PROFILER_UAE4ALL) && !defined(AUTO_RUN) && !defined(AUTO_FRAMERATE)
int mainMenu_frameskip=-1;
#else
#ifdef PROFILER_UAE4ALL
#ifndef AUTO_PROFILER
int mainMenu_frameskip=0;
#else
int mainMenu_frameskip=-1;
#endif
#else
#ifdef DEBUG_FRAMERATE
int mainMenu_frameskip=-1;
#else
int mainMenu_frameskip=0;
#endif
#endif
#endif

int mainMenu_showStatus=0;
int mainMenu_mouseMultiplier = 1;
int mainMenu_sound=1;
int mainMenu_case=-1;
int mainMenu_autosave=1;
int mainMenu_ntsc=0;

static void draw_mainMenu(int c)
{
	/*
	0 = load
	1 = throttle
	2 = sync threshold
	3 = frameskip
	4 = cpu speed
	5 = mouse cursor speed
	6 = sound
	7 = show status line
	8 = eject df1
	9 = autosave
	10 = sound rate
	11 = video mode
	12 = reset
	13 = run
	14 = exit
	*/
	static int b=0;
	int bb=(b%6)/3;
	int menuLine = 3;
	int leftMargin = 4;
	int tabstop1 = 17;
	int tabstop2 = 19;
	int tabstop3 = 21;
	int tabstop4 = 23;
	int tabstop5 = 25;
	int tabstop6 = 27;
	int tabstop7 = 29;
	int tabstop8 = 31;
	int tabstop9 = 33;

	text_draw_background();
	text_draw_window(20,14,300,220,text_str_title);
	//write_text(leftMargin,menuLine,text_str_separator);
	//menuLine++;

	if ((c==0)&&(bb))
		write_text_inv(leftMargin,menuLine,text_str_load);
	else
		write_text(leftMargin,menuLine,text_str_load);

	menuLine++;
	write_text(leftMargin,menuLine,text_str_separator);
	menuLine++;

	// 1
	write_text(leftMargin,menuLine,text_str_throttle);

	if ((mainMenu_throttle==0)&&((c!=1)||(bb)))
		write_text_inv(tabstop3,menuLine,text_str_100p);
	else
		write_text(tabstop3,menuLine,text_str_100p);

	if ((mainMenu_throttle==2)&&((c!=1)||(bb)))
		write_text_inv(tabstop5,menuLine,text_str_83p);
	else
		write_text(tabstop5,menuLine,text_str_83p);

	if ((mainMenu_throttle==4)&&((c!=1)||(bb)))
		write_text_inv(tabstop7,menuLine,text_str_75p);
	else
		write_text(tabstop7,menuLine,text_str_75p);

	// 2
	menuLine+=2;
	write_text(leftMargin,menuLine,text_str_sync_trsh);

	if ((timeslice_mode==0)&&((c!=2)||(bb)))
		write_text_inv(tabstop3,menuLine,text_str_100p);
	else
		write_text(tabstop3,menuLine,text_str_100p);

	if ((timeslice_mode==1)&&((c!=2)||(bb)))
		write_text_inv(tabstop5,menuLine,text_str_75p);
	else
		write_text(tabstop5,menuLine,text_str_75p);

	if ((timeslice_mode==2)&&((c!=2)||(bb)))
		write_text_inv(tabstop7,menuLine,text_str_50p);
	else
		write_text(tabstop7,menuLine,text_str_50p);

	if ((timeslice_mode==3)&&((c!=2)||(bb)))
		write_text_inv(tabstop9,menuLine,text_str_25p);
	else
		write_text(tabstop9,menuLine,text_str_25p);

	// 3
	menuLine+=2;
	write_text(leftMargin,menuLine,text_str_frameskip);

	if ((mainMenu_frameskip==0)&&((c!=3)||(bb)))
		write_text_inv(tabstop1,menuLine,text_str_0);
	else
		write_text(tabstop1,menuLine,text_str_0);
	if ((mainMenu_frameskip==1)&&((c!=3)||(bb)))
		write_text_inv(tabstop2,menuLine,text_str_1);
	else
		write_text(tabstop2,menuLine,text_str_1);
	if ((mainMenu_frameskip==2)&&((c!=3)||(bb)))
		write_text_inv(tabstop3,menuLine,text_str_2);
	else
		write_text(tabstop3,menuLine,text_str_2);
	if ((mainMenu_frameskip==3)&&((c!=3)||(bb)))
		write_text_inv(tabstop4,menuLine,text_str_3);
	else
		write_text(tabstop4,menuLine,text_str_3);
	if ((mainMenu_frameskip==4)&&((c!=3)||(bb)))
		write_text_inv(tabstop5,menuLine,text_str_4);
	else
		write_text(tabstop5,menuLine,text_str_4);
	if ((mainMenu_frameskip==5)&&((c!=3)||(bb)))
		write_text_inv(tabstop6,menuLine,text_str_5);
	else
		write_text(tabstop6,menuLine,text_str_5);
	if ((mainMenu_frameskip==-1)&&((c!=3)||(bb)))
		write_text_inv(tabstop7,menuLine,text_str_auto);
	else
		write_text(tabstop7,menuLine,text_str_auto);

	// 4
	menuLine+=2;
	if ((c==4)&&(bb))
		write_text_inv(leftMargin,menuLine,text_str_cpu_clock);
	else
		write_text(leftMargin,menuLine,text_str_cpu_clock);

	// 5
	menuLine+=2;
	write_text(leftMargin,menuLine,text_str_mouse_multiplier);

	if ((mainMenu_mouseMultiplier==1)&&((c!=5)||(bb)))
		write_text_inv(tabstop1,menuLine,text_str_1x);
	else
		write_text(tabstop1,menuLine,text_str_1x);

	if ((mainMenu_mouseMultiplier==2)&&((c!=5)||(bb)))
		write_text_inv(tabstop3,menuLine,text_str_2x);
	else
		write_text(tabstop3,menuLine,text_str_2x);

	if ((mainMenu_mouseMultiplier==3)&&((c!=5)||(bb)))
		write_text_inv(tabstop5,menuLine,text_str_3x);
	else
		write_text(tabstop5,menuLine,text_str_3x);

	if ((mainMenu_mouseMultiplier==4)&&((c!=5)||(bb)))
		write_text_inv(tabstop7,menuLine,text_str_4x);
	else
		write_text(tabstop7,menuLine,text_str_4x);

	if ((mainMenu_mouseMultiplier==5)&&((c!=5)||(bb)))
		write_text_inv(tabstop9,menuLine,text_str_5x);
	else
		write_text(tabstop9,menuLine,text_str_5x);

	// 6
	menuLine+=2;
	write_text(leftMargin,menuLine,text_str_sound);
	if ((mainMenu_sound==0)&&((c!=6)||(bb)))
		write_text_inv(tabstop1,menuLine,text_str_off);
	else
		write_text(tabstop1,menuLine,text_str_off);

	if ((mainMenu_sound==1)&&((c!=6)||(bb)))
		write_text_inv(tabstop3,menuLine,text_str_on);
	else
		write_text(tabstop3,menuLine,text_str_on);

	if ((mainMenu_sound==2)&&((c!=6)||(bb)))
		write_text_inv(tabstop5,menuLine,text_str_faked);
	else
		write_text(tabstop5,menuLine,text_str_faked);

	// 7
	menuLine+=2;
	write_text(leftMargin, menuLine,text_str_status_line);
	if ((!mainMenu_showStatus)&&((c!=7)||(bb)))
		write_text_inv(tabstop1,menuLine, text_str_off);
	else
		write_text(tabstop1, menuLine, text_str_off);
	if ((mainMenu_showStatus)&&((c!=7)||(bb)))
		write_text_inv(tabstop3, menuLine,text_str_on);
	else
		write_text(tabstop3, menuLine,text_str_on);

	/*menuLine+=2;
	write_text(leftMargin, menuLine,text_str_flash_led);
	if ((!flashLED)&&((c!=6)||(bb)))
		write_text_inv(tabstop1,menuLine, text_str_off);
	else
		write_text(tabstop1, menuLine, text_str_off);
	if ((flashLED)&&((c!=6)||(bb)))
		write_text_inv(tabstop3, menuLine,text_str_on);
	else
		write_text(tabstop3, menuLine,text_str_on);
	*/

	// 8
	menuLine+=2;
	if ((c==8)&&(bb))
		write_text_inv(leftMargin,menuLine,text_str_eject);
	else
		write_text(leftMargin, menuLine,text_str_eject);
	
	// 9
	menuLine+=2;
	write_text(leftMargin,menuLine,text_str_autosave);
	if ((!mainMenu_autosave)&&((c!=9)||(bb)))
		write_text_inv(tabstop1,menuLine,text_str_off);
	else
		write_text(tabstop1,menuLine,text_str_off);

	if ((mainMenu_autosave)&&((c!=9)||(bb)))
		write_text_inv(tabstop3,menuLine,text_str_on);
	else
		write_text(tabstop3,menuLine,text_str_on);

	// 10
	menuLine+=2;
	write_text(leftMargin,menuLine,text_str_sndrate);

	if ((sound_rate==8000)&&((c!=10)||(bb)))
		write_text_inv(tabstop1,menuLine,text_str_8k);
	else
		write_text(tabstop1,menuLine,text_str_8k);

	if ((sound_rate==11025)&&((c!=10)||(bb)))
		write_text_inv(tabstop3,menuLine,text_str_11k);
	else
		write_text(tabstop3,menuLine,text_str_11k);

	if ((sound_rate==22050)&&((c!=10)||(bb)))
		write_text_inv(tabstop5,menuLine,text_str_22k);
	else
		write_text(tabstop5,menuLine,text_str_22k);

	if ((sound_rate==32000)&&((c!=10)||(bb)))
		write_text_inv(tabstop7,menuLine,text_str_32k);
	else
		write_text(tabstop7,menuLine,text_str_32k);

	if ((sound_rate==44100)&&((c!=10)||(bb)))
		write_text_inv(tabstop9,menuLine,text_str_44k);
	else
		write_text(tabstop9,menuLine,text_str_44k);

	// 11
	menuLine+=2;
	write_text(leftMargin,menuLine,text_str_videomode);
	if ((!mainMenu_ntsc)&&((c!=11)||(bb)))
		write_text_inv(tabstop1,menuLine,text_str_pal);
	else
		write_text(tabstop1,menuLine,text_str_pal);

	if ((mainMenu_ntsc)&&((c!=11)||(bb)))
		write_text_inv(tabstop3,menuLine,text_str_ntsc);
	else
		write_text(tabstop3,menuLine,text_str_ntsc);


	menuLine++;
	write_text(leftMargin,menuLine,text_str_separator);

	// 12
	menuLine++;
	if ((c==12)&&(bb))
		write_text_inv(leftMargin,menuLine,text_str_reset);
	else
		write_text(leftMargin,menuLine,text_str_reset);

	menuLine++;
	write_text(leftMargin,menuLine,text_str_separator);

	// 13
	menuLine++;
	//write_text(leftMargin,20,text_str_separator);
	if ((c==13)&&(bb))
		write_text_inv(leftMargin,menuLine,text_str_run);
	else
		write_text(leftMargin,menuLine,text_str_run);

	menuLine++;
	write_text(leftMargin,menuLine,text_str_separator);

	// 14
	menuLine++;
	//write_text(leftMargin,menuLine,text_str_separator);////////////
	if ((c==14)&&(bb))
		write_text_inv(leftMargin,menuLine,text_str_exit);
	else
		write_text(leftMargin,menuLine,text_str_exit);

	menuLine++;
	write_text(leftMargin,menuLine,text_str_separator);

	text_flip();
	b++;
}

static int key_mainMenu(int *cp)
{
	int back_c = -1;
	int c=(*cp);
	int end=0;
	int left=0, right=0, up=0, down=0, hit0=0, hit1=0, hit2=0, hit3=0, hit4=0, hit5=0;
	SDL_Event event;

	while (SDL_PollEvent(&event) > 0)
	{
		if (event.type == SDL_QUIT)
		{
			mainMenu_case=MAIN_MENU_CASE_REBOOT;
			end=-1;
		}
		else if (event.type == SDL_JOYBUTTONDOWN)
		{
			switch (event.jbutton.button)
			{
			case GP2X_BUTTON_RIGHT: right=1; break;
			case GP2X_BUTTON_LEFT: left=1; break;
			case GP2X_BUTTON_UP: up=1; break;
			case GP2X_BUTTON_DOWN: down=1; break;
			case GP2X_BUTTON_B: hit0=1; break;
			case GP2X_BUTTON_START: hit1=1; break;
			case GP2X_BUTTON_L: hit3=1; break;
			case GP2X_BUTTON_R: hit4=1; break;
			case GP2X_BUTTON_X: hit5=1; break;
			}
		}
		else
			if (event.type == SDL_KEYDOWN)
			{
				uae4all_play_click();
				switch(event.key.keysym.sym)
				{
				case SDLK_RIGHT: right=1; break;
				case SDLK_LEFT: left=1; break;
				case SDLK_UP: up=1; break;
				case SDLK_DOWN: down=1; break;
				case SDLK_RETURN:
				case SDLK_LCTRL: hit0=1; break;
				case SDLK_LALT: hit1=1; break;
				}
			}

			if (hit1)
			{
				mainMenu_case=MAIN_MENU_CASE_CANCEL;
				end=1;
			}
			else if (hit2)
			{
				// ???
			}
			else if (hit3)
			{
				mainMenu_case=MAIN_MENU_CASE_LOAD;
				end=1;
			}
			else if (hit4)
			{
				// reset
				back_c = c;
				hit0 = 1;
				c = 12;
			}
			else if (hit5)
			{
				// eject
				back_c = c;
				hit0 = 1;
				c = 8;
			}
			else if (up)
			{
				c--;
				if (c < 0) c = 14;
			}
			else if (down)
				c=(c+1)%15;
			
			switch(c)
			{
				case 0:
					if (hit0)
					{
						mainMenu_case=MAIN_MENU_CASE_LOAD;
						end=1;
					}
					break;
				case 1:
					if (left)
					{
						if (mainMenu_throttle==0)
							mainMenu_throttle=4;
						else if (mainMenu_throttle==4)
							mainMenu_throttle=2;
						else if (mainMenu_throttle==2)
							mainMenu_throttle=0;
					}
					else if (right)
					{
						if (mainMenu_throttle==0)
							mainMenu_throttle=2;
						else if (mainMenu_throttle==2)
							mainMenu_throttle=4;
						else if (mainMenu_throttle==4)
							mainMenu_throttle=0;
					}
					break;
				case 2:
					if (left)
					{
						if (timeslice_mode>0)
							timeslice_mode--;
						else
							timeslice_mode=3;
					}
					else if (right)
					{
						if (timeslice_mode<3)
							timeslice_mode++;
						else
							timeslice_mode=0;
					}
					break;
				case 3:
					if (left)
					{
						if (mainMenu_frameskip>-1)
							mainMenu_frameskip--;
						else
							mainMenu_frameskip=5;
					}
					else if (right)
					{
						if (mainMenu_frameskip<5)
							mainMenu_frameskip++;
						else
							mainMenu_frameskip=-1;
					}
					break;
				case 4:
					if (hit0)
					{
						mainMenu_case=MAIN_MENU_CASE_OVERCLOCK;
						end=1;
					}
					break;
				case 5:
					if (left)
					{
						if (mainMenu_mouseMultiplier > 1)
							mainMenu_mouseMultiplier--;
					}
					else if (right)
					{
						if (mainMenu_mouseMultiplier < 5)
							mainMenu_mouseMultiplier++;
					}
					break;
				case 6:
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
				case 7:
					if ((left)||(right))
						mainMenu_showStatus=!mainMenu_showStatus;
					break;
				/*case 7:
					if ((left)||(right))
					{
						if (flashLED)
							flashLED=0;
						else
							flashLED=1;
					}
					break;*/
				case 8:
					// eject
					if (hit0)
					{
						mainMenu_case=MAIN_MENU_CASE_EJECT;
						end=1;
					}
					break;
				case 9:
					if ((left)||(right))
					{
						mainMenu_autosave = (mainMenu_autosave == 0) ? 1 : 0;
					}
					break;
				case 10:
					if ((left)||(right))
					{
						static int rates[] = { 8000, 11025, 22050, 32000, 44100 };
						int sel;
						for (sel = 0; sel < sizeof(rates) / sizeof(rates[0]); sel++)
							if (rates[sel] == sound_rate) break;
						sel += left ? -1 : 1;
						if (sel < 0) sel = 0;
						if (sel > 4) sel = 4;
						sound_rate = rates[sel];
					}
					break;
				case 11:
					if ((left)||(right))
					{
						mainMenu_ntsc = !mainMenu_ntsc;
					}
					break;
				case 12:
					if (hit0)
					{
						mainMenu_case=MAIN_MENU_CASE_RESET;
						end=1;
					}
					break;
				case 13:
					if (hit0)
					{
						mainMenu_case=MAIN_MENU_CASE_RUN;
						end=1;
					}
					break;
				case 14:
					if (hit0)
					{
						mainMenu_case=MAIN_MENU_CASE_REBOOT;
						end=1;
					}
					break;
			}
			if (back_c>=0)
			{
				c=back_c;
				back_c=-1;
			}
			
	}


	(*cp)=c;
	return end;
}

static void raise_mainMenu()
{
	int i;

	text_draw_background();
	text_flip();
	for(i=0;i<10;i++)
	{
		text_draw_background();
		text_draw_window(40,(10-i)*24,260,200,text_str_title);
		text_flip();
	}
}

static void unraise_mainMenu()
{
	int i;

	for(i=9;i>=0;i--)
	{
		text_draw_background();
		text_draw_window(40,(10-i)*24,260,200,text_str_title);
		text_flip();
	}
	text_draw_background();
	text_flip();
}

int run_mainMenu()
{
#if defined(AUTO_RUN) || defined(AUTO_FRAMERATE) || defined(AUTO_PROFILER)
	return 1;
#else
#if !defined(DEBUG_UAE4ALL) && !defined(PROFILER_UAE4ALL) 
	static int c=0;
#else
	static int c=1;
#endif
	int end;
	int old_sound_rate = sound_rate;
	mainMenu_case=-1;
	setBatteryLED(0);

	// reinit text
	init_text(0);

	while(mainMenu_case<0)
	{
		raise_mainMenu();
		end=0;
		while(!end)
		{
			draw_mainMenu(c);
			end=key_mainMenu(&c);
		}
		unraise_mainMenu();
		switch(mainMenu_case)
		{
		case MAIN_MENU_CASE_LOAD:
			run_menuLoad();	
			mainMenu_case=-1;
			break;
		case MAIN_MENU_CASE_OVERCLOCK:
			run_menuOverclock();
			mainMenu_case=-1;
			break;
		case MAIN_MENU_CASE_EJECT:
			mainMenu_case=3;
			break;
		case MAIN_MENU_CASE_CANCEL:
			if (emulating)
				mainMenu_case=1;
			else
				mainMenu_case=-1;
			break;
		case MAIN_MENU_CASE_RESET:
			if (emulating)
			{
				mainMenu_case=2;
				break;
			}
		case MAIN_MENU_CASE_RUN:
			mainMenu_case=1;
			break;
		case MAIN_MENU_CASE_REBOOT:
			gp2x_stop_sound();
			SDL_Quit();

			if (gp2xClockSpeed != -1)
			{
				// try to set it back to something vaguely sane
				setGP2XClock(200);
			}
			sync();

			if (!norelaunchGP2XMenu)
			{
				chdir("/usr/gp2x");
				execl("/usr/gp2x/gp2xmenu", "/usr/gp2x/gp2xmenu", NULL);
			}
			exit(0);
			break;
		default:
			mainMenu_case=-1;
		}
	}

	if (sound_rate != old_sound_rate)
		init_sound();

	return mainMenu_case;
#endif
}

extern char uae4all_image_file[128];
extern char uae4all_image_file2[128];

typedef struct _cmdline_opt
{
	char *optname;
	int  len; // if 0, it means int
	void *opt;
} cmdline_opt;

static cmdline_opt cmdl_opts[] =
{
	{ "-norelaunchmenu",  0, &norelaunchGP2XMenu },
	{ "-statusln",        0, &mainMenu_showStatus },
	{ "-mousemultiplier", 0, &mainMenu_mouseMultiplier },
	{ "-sound",           0, &mainMenu_sound },
	{ "-soundrate",       0, &sound_rate },
	{ "-autosave",        0, &mainMenu_autosave },
	{ "-systemclock",     0, &mainMenu_throttle },
	{ "-syncthreshold",   0, &timeslice_mode },
	{ "-frameskip",       0, &mainMenu_frameskip },
	{ "-skipintro",       0, &skipintro },
	{ "-ntsc",            0, &mainMenu_ntsc },
	{ "-kick",            sizeof(romfile), romfile },
	{ "-df0",             sizeof(uae4all_image_file), uae4all_image_file },
	{ "-df1",             sizeof(uae4all_image_file2), uae4all_image_file2 },
};


void parse_cmdline(int argc, char **argv)
{
	int arg, i, found;

	for (arg = 1; arg < argc-1; arg++)
	{
		for (i = found = 0; i < sizeof(cmdl_opts) / sizeof(cmdl_opts[0]); i++)
		{
			if (strcmp(argv[arg], cmdl_opts[i].optname) == 0)
			{
				arg++;
				if (cmdl_opts[i].len == 0)
					*(int *)(cmdl_opts[i].opt) = atoi(argv[arg]);
				else
				{
					strncpy((char *)cmdl_opts[i].opt, argv[arg], cmdl_opts[i].len);
					((char *)cmdl_opts[i].opt)[cmdl_opts[i].len-1] = 0;
				}
				found = 1;
				break;
			}
		}
		if (!found) printf("skipping unknown option: \"%s\"\n", argv[arg]);
	}
}

