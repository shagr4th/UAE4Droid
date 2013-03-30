#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <SDL.h>

#include "gp2xutil.h"
#include "menu.h"
#include "menu_config.h"
#include "sysconfig.h"
#include "sysdeps.h"
#include "autoconf.h"
#include "vkbd.h"
#include "options.h"
#include "sound.h"
#include "zfile.h"
#include "gui.h"
#include "gp2x.h"
#include "disk.h"
#include "cpuspeed/cpuctrl.h"
#include "custom.h"

/* PocketUAE config file. Used for parsing PocketUAE-like options. */
#include "cfgfile.h"
#include "savestate.h"

extern int kickstart;
extern int oldkickstart;
extern int bReloadKickstart;
extern unsigned int sound_rate;
extern int skipintro;
extern int screenWidth;
extern int moveX;
extern int moveY;
extern int timeslice_mode;
extern int emulating;
extern int gp2xMouseEmuOn;
extern int gp2xButtonRemappingOn;

extern int init_sound(void);
extern void gp2x_stop_sound(void);

extern char launchDir[300];
extern char currentDir[300];

char *statusmessages[] = { "AUTOFIRE ON\0", "AUTOFIRE OFF\0","SCREENSHOT SAVED\0","SCRIPT SAVED\0","SCRIPT AND SCREENSHOT SAVED\0"};
int showmsg=0;

extern char filename0[256];
extern char filename1[256];
extern char filename2[256];
extern char filename3[256];
static char *text_str_title=    "----- UAE4All Android -----";
static char *text_str_df0=		"DF0:";
static char *text_str_df1=		"DF1:";
static char *text_str_df2=		"DF2:";
static char *text_str_df3=		"DF3:";
static char* text_str_hdnmem="Harddisk and Memory Options (H)";
static char *text_str_display="Display Settings (L-trigger)";
static char *text_str_savestates="Savestates (S)";
static char *text_str_eject="Eject All Drives";
char *text_str_separator="--------------------------------";
static char *text_str_reset="Reset (R-trigger)";
static char *text_str_exit= "Quit (Q)";

int mainMenu_case=-1;
int mainMenu_system=-1;

int nr_drives=DEFAULT_DRIVES;
int current_drive=0;

int lastCpuSpeed=600;
int ntsc=0;

extern SDL_Surface *prSDLScreen;

void reset_hdConf()
{
	/* Reset HD config */
	if (hd_dir_unit_nr >= 0)
	{
		kill_filesys_unit(currprefs.mountinfo, 0);
		hd_dir_unit_nr = -1;
	}
	if (hd_file_unit_nr >= 0)
	{
		kill_filesys_unit(currprefs.mountinfo, 0);
		hd_file_unit_nr = -1;
	}
	mainMenu_filesysUnits = 0;

	switch (mainMenu_bootHD)
	{
		case 0:
			// nothing to do, already killed above
			break;
		case 1:
			if (hd_dir_unit_nr < 0)
			{
				if (uae4all_hard_dir[0] != '\0')
				{
					parse_filesys_spec(0, uae4all_hard_dir);
					hd_dir_unit_nr = mainMenu_filesysUnits++;
				}
			}
			if (hd_file_unit_nr < 0)
			{
				if (uae4all_hard_file[0] != '\0')
				{
					parse_hardfile_spec(uae4all_hard_file);
					hd_file_unit_nr = mainMenu_filesysUnits++;
				}
			}
			break;
		case 2:
			if (hd_file_unit_nr < 0)
			{
				if (uae4all_hard_file[0] != '\0')
				{
					parse_hardfile_spec(uae4all_hard_file);
					hd_file_unit_nr = mainMenu_filesysUnits++;
				}
			}
			if (hd_dir_unit_nr < 0)
			{
				if (uae4all_hard_dir[0] != '\0')
				{
					parse_filesys_spec(0, uae4all_hard_dir);
					hd_dir_unit_nr = mainMenu_filesysUnits++;
				}
			}
			break;
	}
}

static void extractFileName(char * str,char *buffer)
{
	char *p=str+strlen(str)-1;
	while(*p != '/')
		p--;
	p++;
	strcpy(buffer,p);
}

static void adjustToWindow(char *str, char* buffer)
{
	if (strlen(str)<33) return;
	char *p=str+strlen(str)-13;
	for (int i=0;i<15;i++) {buffer[i]=*str;str++;}
	char tt[]={'.','.','.','\0',};
	strcat(buffer,tt);
	strcat(buffer,p);
}

static void showInfo()
{
	text_draw_background();
	char buffer[128];
	char buffertext[128];

	text_draw_window(2,2,35,20,"Info");
	SDL_Rect r;
	r.x=80-64; r.y=0; r.w=35*7; r.h=140;
	extern SDL_Surface *text_screen;
	SDL_SetClipRect(text_screen,&r);
	write_text(4,2,"DF0");

	extractFileName(uae4all_image_file0,buffer);
	adjustToWindow(buffer,buffertext);
	write_text(10,2,buffertext);

	write_text(4,4,"DF1");
	if (!uae4all_image_file1[0]) write_text(10,4,"Empty");
	else 
	{
		extractFileName(uae4all_image_file1,buffer);
		adjustToWindow(buffer,buffertext);
		write_text(10,4,buffer);
	}

	write_text(4,6,"DF2");
	if (!uae4all_image_file2[0]) write_text(10,4,"Empty");
	else 
	{
		extractFileName(uae4all_image_file2,buffer);
		adjustToWindow(buffer,buffertext);
		write_text(10,6,buffer);
	}
	
	write_text(4,8,"DF3");
	if (!uae4all_image_file3[0]) write_text(10,4,"Empty");
	else 
	{
		extractFileName(uae4all_image_file3,buffer);
		adjustToWindow(buffer,buffertext);
		write_text(10,8,buffer);
	}

		text_flip();
		SDL_Event ev;
		SDL_Delay(333);
		while(SDL_PollEvent(&ev))
		SDL_Delay(10);
		while(!SDL_PollEvent(&ev))
				SDL_Delay(10);
		while(SDL_PollEvent(&ev))
				if (ev.type==SDL_QUIT)
					exit(1);
		SDL_Delay(200);
		SDL_SetClipRect(text_screen,NULL);
}

void set_joyConf()
{
	if(mainMenu_joyConf==0)
	{
		mainMenu_button1=GP2X_BUTTON_X;
		mainMenu_button2=GP2X_BUTTON_A;
		mainMenu_jump=-1;
		mainMenu_autofireButton1=GP2X_BUTTON_B;
	}
	else if(mainMenu_joyConf==1)
	{
		mainMenu_button1=GP2X_BUTTON_B;
		mainMenu_button2=GP2X_BUTTON_A;
		mainMenu_jump=-1;
		mainMenu_autofireButton1=GP2X_BUTTON_X;
	}	
	else if(mainMenu_joyConf==2)
	{
		mainMenu_button1=GP2X_BUTTON_Y;
		mainMenu_button2=GP2X_BUTTON_A;
		mainMenu_jump=GP2X_BUTTON_X;
		mainMenu_autofireButton1=GP2X_BUTTON_B;
	}
	else if(mainMenu_joyConf==3)
	{
		mainMenu_button1=GP2X_BUTTON_B;
		mainMenu_button2=GP2X_BUTTON_A;
		mainMenu_jump=GP2X_BUTTON_X;
		mainMenu_autofireButton1=GP2X_BUTTON_Y;
	}
}

static void draw_mainMenu(int c)
{
	/* New Menu
	0 = DF0:
	1 = DF1:
	2 = DF2:
	3 = DF3:
	4 = eject all drives
	5 = number of drives
	6 = preset system setup
	7 = harddisk and memory options
	8 = display settings
	9 = savestates
	10 = custom controls
	11 = more options
	12 = reset
	13 = save config current game
	14 = save general config
	15 = quit
	*/
	static int b=0;
	int bb=(b%6)/3;
	int menuLine = 3;
	int leftMargin = 8;
	int tabstop1 = 17+4;
	int tabstop2 = 19+4;
	int tabstop3 = 21+4;
	int tabstop4 = 23+4;
	int tabstop5 = 25+4;
	int tabstop6 = 27+4;
	int tabstop7 = 29+4;
	int tabstop8 = 31+4;
	int tabstop9 = 33+4;

	text_draw_background();
	text_draw_window(leftMargin-1,menuLine-1,34,40,text_str_title);

	// 1
	if ((c==0)&&(bb))
		write_text_inv(leftMargin,menuLine,text_str_df0);
	else
		write_text(leftMargin,menuLine,text_str_df0);
	if(strcmp(uae4all_image_file0, "")==0)
		write_text_inv(13,menuLine,"insert disk image");
	else
		write_text_inv(13,menuLine,filename0);

	// 2
	menuLine+=2;
	if(nr_drives > 1)
	{
		if((c==1)&&(bb))
			write_text_inv(leftMargin,menuLine,text_str_df1);
		else
			write_text(leftMargin,menuLine,text_str_df1);
		if(strcmp(uae4all_image_file1, "")==0)
			write_text_inv(13,menuLine,"insert disk image");
		else
			write_text_inv(13,menuLine,filename1);
	}

	// 3
	menuLine+=2;
	if(nr_drives > 2)
	{
		if ((c==2)&&(bb))
			write_text_inv(leftMargin,menuLine,text_str_df2);
		else
			write_text(leftMargin,menuLine,text_str_df2);
		if(strcmp(uae4all_image_file2, "")==0)
			write_text_inv(13,menuLine,"insert disk image");
		else
			write_text_inv(13,menuLine,filename2);
	}

	// 4
	menuLine+=2;
	if(nr_drives > 3)
	{
		if ((c==3)&&(bb))
			write_text_inv(leftMargin,menuLine,text_str_df3);
		else
			write_text(leftMargin,menuLine,text_str_df3);
		if(strcmp(uae4all_image_file3, "")==0)
			write_text_inv(13,menuLine,"insert disk image");
		else
			write_text_inv(13,menuLine,filename3);
	}

	menuLine++;
	write_text(leftMargin,menuLine,text_str_separator);
	menuLine++;

	// 5
	if ((c==4)&&(bb))
		write_text_inv(leftMargin,menuLine,text_str_eject);
	else
		write_text(leftMargin, menuLine,text_str_eject);

	// 6
	menuLine+=2;
	write_text(leftMargin,menuLine,"Number of drives:");
	
	if ((nr_drives==1)&&((c!=5)||(bb)))
		write_text_inv(tabstop3,menuLine,"1");
	else
		write_text(tabstop3,menuLine,"1");

	if ((nr_drives==2)&&((c!=5)||(bb)))
		write_text_inv(tabstop4,menuLine,"2");
	else
		write_text(tabstop4,menuLine,"2");

	if ((nr_drives==3)&&((c!=5)||(bb)))
		write_text_inv(tabstop5,menuLine,"3");
	else
		write_text(tabstop5,menuLine,"3");

	if ((nr_drives==4)&&((c!=5)||(bb)))
		write_text_inv(tabstop6,menuLine,"4");
	else
		write_text(tabstop6,menuLine,"4");

	menuLine++;
	write_text(leftMargin,menuLine,text_str_separator);
	menuLine++;

	// 7
	write_text(leftMargin,menuLine,"Preset System Setup:");

	if ((mainMenu_system!=1)&&((c!=6)||(bb)))
		write_text_inv(tabstop5,menuLine,"A500");
	else
		write_text(tabstop5,menuLine,"A500");

	if ((mainMenu_system!=0)&&((c!=6)||(bb)))
		write_text_inv(tabstop8-1,menuLine,"A1200");
	else
		write_text(tabstop8-1,menuLine,"A1200");

	// 8
	menuLine+=2;
	if ((c==7)&&(bb))
		write_text_inv(leftMargin,menuLine,text_str_hdnmem);
	else
		write_text(leftMargin,menuLine,text_str_hdnmem);

	menuLine++;
	write_text(leftMargin,menuLine,text_str_separator);
	menuLine++;

	// 9
	if ((c==8)&&(bb))
		write_text_inv(leftMargin,menuLine,text_str_display);
	else
		write_text(leftMargin,menuLine,text_str_display);

	// 10
	menuLine+=2;
	if ((c==9)&&(bb))
		write_text_inv(leftMargin,menuLine,text_str_savestates);
	else
		write_text(leftMargin,menuLine,text_str_savestates);

	// 11
	menuLine+=2;
	if ((c==10)&&(bb))
		write_text_inv(leftMargin,menuLine,"Custom Control Config (Y)");
	else
		write_text(leftMargin,menuLine,"Custom Control Config (Y)");

	// 12
	menuLine+=2;
	if ((c==11)&&(bb))
		write_text_inv(leftMargin,menuLine,"More Options (B)");
	else
		write_text(leftMargin,menuLine,"More Options (B)");

	menuLine++;
	write_text(leftMargin,menuLine,text_str_separator);

	// 13
	menuLine++;
	if ((c==12)&&(bb))
		write_text_inv(leftMargin,menuLine,text_str_reset);
	else
		write_text(leftMargin,menuLine,text_str_reset);

	menuLine++;
	write_text(leftMargin,menuLine,text_str_separator);

	// 14
	menuLine++;
	if ((c==13)&&(bb))
		write_text_inv(leftMargin,menuLine,"Save Config for current game");
	else
		write_text(leftMargin,menuLine,"Save Config for current game");

	// 15
	menuLine+=2;
	if ((c==14)&&(bb))
		write_text_inv(leftMargin,menuLine,"Save General Config");
	else
		write_text(leftMargin,menuLine,"Save General Config");

	menuLine++;
	write_text(leftMargin,menuLine,text_str_separator);

	// 16
	menuLine++;
	if ((c==15)&&(bb))
		write_text_inv(leftMargin,menuLine,text_str_exit);
	else
		write_text(leftMargin,menuLine,text_str_exit);

	menuLine++;
	write_text(leftMargin,menuLine,text_str_separator);

	text_flip();
	
	b++;
}

void showWarning(char *msg)
{
	text_draw_window(54/7,91/8,255/7,64/8,"--- Config ---");
	write_text(12,14,msg);
	write_text(11,16,"Press any button to continue");
	text_flip();
	SDL_Event ev;
	SDL_Delay(333);
	while(SDL_PollEvent(&ev))
	{
		if (ev.type==SDL_QUIT)
			exit(1);
		SDL_Delay(10);
	}
}

void update_display()
{
	char layersize[20];
	snprintf(layersize, 20, "%dx480", screenWidth);
	setenv("SDL_OMAP_LAYER_SIZE",layersize,1);
	char bordercut[20];
	snprintf(bordercut, 20, "%d,%d,0,%d", mainMenu_cutLeft, mainMenu_cutRight, 270-mainMenu_displayedLines);
	setenv("SDL_OMAP_BORDER_CUT",bordercut,1);
	if(mainMenu_case == MAIN_MENU_CASE_DISPLAY)
		prSDLScreen = SDL_SetVideoMode(320, 240, 16, SDL_SWSURFACE|SDL_FULLSCREEN|SDL_DOUBLEBUF);
	else {
	  if (mainMenu_displayHires)
		prSDLScreen = SDL_SetVideoMode(640, mainMenu_displayedLines, 16, SDL_SWSURFACE|SDL_FULLSCREEN|SDL_DOUBLEBUF);
	  else
		prSDLScreen = SDL_SetVideoMode(320, mainMenu_displayedLines, 16, SDL_SWSURFACE|SDL_FULLSCREEN|SDL_DOUBLEBUF);
	}
}

void setSystem()
{
	if(mainMenu_system > 0)
	{
		mainMenu_chipMemory=2;
		mainMenu_slowMemory=0;
		mainMenu_fastMemory=4;
		kickstart=3;
		mainMenu_CPU_model=1;
		mainMenu_chipset=2;
	}
	else
	{
		mainMenu_chipMemory=1;
		mainMenu_slowMemory=0;
		mainMenu_fastMemory=0;
		kickstart=1;
		mainMenu_CPU_model=0;
		mainMenu_chipset=0;
	}
	UpdateMemorySettings();
	UpdateCPUModelSettings();
	UpdateChipsetSettings();
}

static int key_mainMenu(int *cp)
{
	int back_c=-1;
	int c=(*cp);
	int end=0;
	int left=0, right=0, up=0, down=0, hit0=0, hit1=0, hit2=0, hit3=0, hit4=0, hit5=0, hit6=0, hitH=0, hitS=0, hitQ=0, hitN1=0, hitN2=0, hitN3=0, hitN4=0;
	SDL_Event event;
	int info=0;

	while (SDL_PollEvent(&event) > 0)
	{
		if (event.type == SDL_QUIT)
		{
			mainMenu_case=MAIN_MENU_CASE_REBOOT;
			end=-1;
		}
		else if (event.type == SDL_KEYDOWN)
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
				case SDLK_LCTRL: hit2=1; break;
				case SDLK_RSHIFT: hit3=1; break;
				case SDLK_RCTRL: hit4=1; break;
				case SDLK_END: hit5=1; break;
				case SDLK_PAGEUP: hit6=1; break;
				case SDLK_i: info=1; break;
				case SDLK_h: hitH=1; break;
				case SDLK_s: hitS=1; break;
				case SDLK_q: hitQ=1; break;
				case SDLK_1: hitN1=1; break;
				case SDLK_2: hitN2=1; break;
				case SDLK_3: hitN3=1; break;
				case SDLK_4: hitN4=1;
			}
		}

		if (info)
			showInfo();
		else if (hit1)
		{
			mainMenu_case=MAIN_MENU_CASE_RUN;
			end=1;
		}
		else if (hit2)
		{
			mainMenu_case=MAIN_MENU_CASE_CANCEL;
			end=1;
		}
		else if (hit3)
		{
			mainMenu_case=MAIN_MENU_CASE_DISPLAY;
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
			// more options
			back_c = c;
			hit0 = 1;
			c = 11;
		}
		else if (hit6)
		{
			// custom controls
			back_c = c;
			hit0 = 1;
			c = 10;
		}
		else if (hitH)
		{
			mainMenu_case=MAIN_MENU_CASE_MEMDISK;
			end=1;
		}
		else if (hitS)
		{
			mainMenu_case=MAIN_MENU_CASE_SAVESTATES;
			end=1;
		}
		else if (hitQ)
		{
			mainMenu_case=MAIN_MENU_CASE_REBOOT;
			end=1;
		}
		else if(hitN1)
		{
			current_drive=0;
			mainMenu_case=MAIN_MENU_CASE_LOAD;
			end=1;
		}
		else if(hitN2)
		{
			current_drive=1;
			mainMenu_case=MAIN_MENU_CASE_LOAD;
			end=1;
		}
		else if(hitN3)
		{
			current_drive=2;
			mainMenu_case=MAIN_MENU_CASE_LOAD;
			end=1;
		}
		else if(hitN4)
		{
			current_drive=3;
			mainMenu_case=MAIN_MENU_CASE_LOAD;
			end=1;
		}
		else if (up)
		{
			if(nr_drives<2 && c==4)
				c=0;
			else if(nr_drives<3 && c==4)
				c=1;
			else if(nr_drives<4 && c==4)
				c=2;
			else
				c--;
			if (c < 0) c = 15;
		}
		else if (down)
		{
			if(nr_drives<4 && c==2)
				c=4;
			else if(nr_drives<3 && c==1)
				c=4;
			else if(nr_drives<2 && c==0)
				c=4;
			else
				c=(c+1)%16;
		}

	/* New Menu
	0 = DF0:
	1 = DF1:
	2 = DF2:
	3 = DF3:
	4 = eject all drives
	5 = number of drives
	6 = preset system setup
	7 = display settings
	8 = sound
	9 = savestates
	10 = custom controls
	11 = more options
	12 = reset
	13 = save config current game
	14 = save general config
	15 = exit
	*/
		switch(c)
		{
			case 0:
				if (hit0)
				{
					current_drive=0;
					mainMenu_case=MAIN_MENU_CASE_LOAD;
					end=1;
				}
				break;
			case 1:
				if (hit0)
				{
					current_drive=1;
					mainMenu_case=MAIN_MENU_CASE_LOAD;
					end=1;
				}
				break;
			case 2:
				if (hit0)
				{
					current_drive=2;
					mainMenu_case=MAIN_MENU_CASE_LOAD;
					end=1;
				}
				break;
			case 3:
				if (hit0)
				{
					current_drive=3;
					mainMenu_case=MAIN_MENU_CASE_LOAD;
					end=1;
				}
				break;
			case 4:
				if (hit0)
				{
					strcpy(uae4all_image_file0, "");
					strcpy(uae4all_image_file1, "");
					strcpy(uae4all_image_file2, "");
					strcpy(uae4all_image_file3, "");
				}
				break;
			case 5:
				if (left)
				{
					if (nr_drives>1)
						nr_drives--;
					else
						nr_drives=4;
				}
				else if (right)
				{
					if (nr_drives<4)
						nr_drives++;
					else
						nr_drives=1;
				}	
				break;
			case 6:
				if (left)
				{
					if (mainMenu_system==0)
						mainMenu_system=1;
					else
						mainMenu_system=0;
					setSystem();
				}
				else if (right)
				{
					if (mainMenu_system<1)
						mainMenu_system=1;
					else
						mainMenu_system=0;
					setSystem();
				}
				break;
			case 7:
				if (hit0)
				{
					mainMenu_case=MAIN_MENU_CASE_MEMDISK;
					end=1;
				}
				break;
			case 8:
				if (hit0)
				{
					mainMenu_case=MAIN_MENU_CASE_DISPLAY;
					end=1;
				}
				break;
			case 9:
				if (hit0)
				{
					mainMenu_case=MAIN_MENU_CASE_SAVESTATES;
					end=1;
				}
				break;
			case 10:
				if (hit0)
				{
					mainMenu_case=MAIN_MENU_CASE_CONTROLS;
					end=1;
				}
				break;
			case 11:
				if (hit0)
				{
					mainMenu_case=MAIN_MENU_CASE_MISC;
					printf("Launch main menu MISC\n");
					end=1;
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
					mainMenu_case=MAIN_MENU_CASE_SAVE;
					if (saveconfig())
						showWarning("Config saved for this game");
				}
				break;
			case 14:
				if (hit0)
				{
					mainMenu_case=MAIN_MENU_CASE_SAVE;
					saveconfig(1);
					showWarning("General config file saved");
				}
				break;
			case 15:
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
	setenv("SDL_OMAP_LAYER_SIZE","640x480",1);
	setenv("SDL_OMAP_BORDER_CUT","0,0,0,30",1);
	prSDLScreen = SDL_SetVideoMode(320, 240, 16, SDL_SWSURFACE|SDL_FULLSCREEN|SDL_DOUBLEBUF);

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

int saveAdfDir()
{
	char path[300];
	snprintf(path, 300, "%s/conf/adfdir.conf", launchDir);
	FILE *f=fopen(path,"w");
	if (!f) return 0;
	char buffer[310];
	snprintf((char*)buffer, 310, "path=%s\n",currentDir);
	fputs(buffer,f);
	fclose(f);
	return 1;
}

void setCpuSpeed()
{
	char speedCmd[128];

	if(mainMenu_cpuSpeed!=lastCpuSpeed)
	{
		snprintf((char*)speedCmd, 128, "unset DISPLAY; echo y | sudo -n /usr/pandora/scripts/op_cpuspeed.sh %d", mainMenu_cpuSpeed);
		system(speedCmd);
		lastCpuSpeed = mainMenu_cpuSpeed;
	}
	if(mainMenu_ntsc!=ntsc)
	{
		ntsc=mainMenu_ntsc;
		if(ntsc)
			system("sudo /usr/pandora/scripts/op_lcdrate.sh 60");
		else
			system("sudo /usr/pandora/scripts/op_lcdrate.sh 50");
	}
	update_display();
}

int run_mainMenu()
{
	static int c=0;
	int end;
	int old_sound_rate = sound_rate;
	mainMenu_case=-1;
	init_text(0);

	while(mainMenu_case<0)
	{
		raise_mainMenu();
		end=0;
		draw_mainMenu(c);
		while(!end)
		{
			draw_mainMenu(c);
			end=key_mainMenu(&c);
		}
		unraise_mainMenu();
		switch(mainMenu_case)
		{
		case MAIN_MENU_CASE_LOAD:
			if(run_menuLoad(currentDir, MENU_LOAD_FLOPPY) && current_drive==0) loadconfig();
			mainMenu_case=-1;
			break;
		case MAIN_MENU_CASE_MEMDISK:
			run_menuMemDisk();
			mainMenu_case=-1;
			break;
		case MAIN_MENU_CASE_SAVESTATES:
			run_menuSavestates();
			if(savestate_state == STATE_DORESTORE || savestate_state == STATE_DOSAVE)
			{
				setCpuSpeed();
				mainMenu_case=1;
			}
			else
				mainMenu_case=-1;
			break;
		case MAIN_MENU_CASE_EJECT:
			mainMenu_case=3;
			break;
		case MAIN_MENU_CASE_CANCEL:
			if (emulating)
			{
				setCpuSpeed();
				mainMenu_case=1;
			}
			else
				mainMenu_case=-1;
			break;
		case MAIN_MENU_CASE_RESET:
			setCpuSpeed();
			gp2xMouseEmuOn=0;
			gp2xButtonRemappingOn=0;
			mainMenu_drives=nr_drives;
			if (kickstart!=oldkickstart) 
			{
				oldkickstart=kickstart;
				snprintf(romfile, 256, "%s/kickstarts/%s",launchDir,kickstarts_rom_names[kickstart]);
				bReloadKickstart=1;
				uae4all_init_rom(romfile);
			}
			reset_hdConf();
			if (emulating)
			{
				mainMenu_case=2;	
				break;
			}
		case MAIN_MENU_CASE_RUN:
			setCpuSpeed();
			mainMenu_case=1;
			break;
		case MAIN_MENU_CASE_CONTROLS:
			{
				run_menuControls();
				mainMenu_case=-1;
			}
			break;
		case MAIN_MENU_CASE_DISPLAY:
			{
				run_menuDisplay();
				mainMenu_case=-1;
			}
			break;
		case MAIN_MENU_CASE_MISC:
			{
				run_menuMisc();
				mainMenu_case=-1;
			}
			break;
		case MAIN_MENU_CASE_REBOOT:
#ifndef USE_SDLSOUND
			gp2x_stop_sound();
#endif
			saveAdfDir();
			SDL_Quit();
			sync();
			exit(0);
			break;
		default:
			mainMenu_case=-1;
		}
	}

	if (sound_rate != old_sound_rate)
		init_sound();

	return mainMenu_case;
}

#ifndef PANDORA
typedef struct _cmdline_opt
{
	char *optname;
	int len;
	void *opt;
} cmdline_opt;

static cmdline_opt cmdl_opts[] =
{
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
	{ "-joyconf",            0, &mainMenu_joyConf },
	{ "-use1mbchip",            0, &mainMenu_chipMemory },
	{ "-autofire",            0, &mainMenu_autofire },
	{ "-drives",            0, &mainMenu_drives },
	{ "-script",            0, &mainMenu_enableScripts},
	{ "-screenshot",            0, &mainMenu_enableScreenshots},
	{ "-kick",            sizeof(romfile), romfile },
	{ "-df0",             sizeof(uae4all_image_file0), uae4all_image_file0 },
	{ "-df1",             sizeof(uae4all_image_file1), uae4all_image_file1 },
	{ "-df2",             sizeof(uae4all_image_file2), uae4all_image_file2 },
	{ "-df3",             sizeof(uae4all_image_file2), uae4all_image_file3 },
};
#endif

static void replace(char * str,char replace, char toreplace)
{
	while(*str)
	{	
		if (*str==toreplace) *str=replace;
		str++;
	}
}

int saveconfig(int general)
{
	char path[300];
	char *p; 
	if (general) snprintf(path, 300, "%s/conf/uaeconfig.conf", launchDir);
	else{
		if (!uae4all_image_file0[0]) return 0;
	p=uae4all_image_file0+strlen(uae4all_image_file0)-1;
	while (*p != '/')p--;
	p++;
	int len=strlen(p) + 1;
	char gamename[len];
	strcpy(gamename,p);
	
	char * pch;
	pch = strstr (gamename,".adf.gz");
	if (pch) strcpy(pch,"\0");
		else{
				pch = strstr (gamename,".adf");
				if (pch) strcpy(pch,"\0");
		}
	if (uae4all_image_file0[0])	snprintf(path, 300, "%s/conf/%s.conf", launchDir,gamename);
	else return 0;
	}

	FILE *f=fopen(path,"w");
	if (!f) return 0;
	char buffer[255];

	snprintf((char*)buffer, 255, "kickstart=%d\n",kickstart);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "scaling=%d\n",mainMenu_enableHWscaling);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "showstatus=%d\n",mainMenu_showStatus);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "mousemultiplier=%d\n",mainMenu_mouseMultiplier);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "systemclock=%d\n",mainMenu_throttle);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "syncthreshold=%d\n",timeslice_mode);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "frameskip=%d\n",mainMenu_frameskip);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "sound=%d\n",mainMenu_sound);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "soundrate=%d\n",sound_rate);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "autosave=%d\n",mainMenu_autosave);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "gp2xclock=%d\n",gp2xClockSpeed);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "joyconf=%d\n",mainMenu_joyConf);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "autofireRate=%d\n",mainMenu_autofireRate);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "autofire=%d\n",mainMenu_autofire);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "stylusOffset=%d\n",mainMenu_stylusOffset);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "tapDelay=%d\n",mainMenu_tapDelay);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "scanlines=%d\n",mainMenu_scanlines);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "ham=%d\n",mainMenu_ham);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "enableScreenshots=%d\n",mainMenu_enableScreenshots);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "floppyspeed=%d\n",mainMenu_floppyspeed);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "drives=%d\n",nr_drives);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "videomode=%d\n",mainMenu_ntsc);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "mainMenu_cpuSpeed=%d\n",mainMenu_cpuSpeed);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "presetModeId=%d\n",presetModeId);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "moveX=%d\n",moveX);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "moveY=%d\n",moveY);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "displayedLines=%d\n",mainMenu_displayedLines);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "screenWidth=%d\n",screenWidth);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "cutLeft=%d\n",mainMenu_cutLeft);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "cutRight=%d\n",mainMenu_cutRight);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "customControls=%d\n",mainMenu_customControls);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "custom_dpad=%d\n",mainMenu_custom_dpad);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "custom_up=%d\n",mainMenu_custom_up);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "custom_down=%d\n",mainMenu_custom_down);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "custom_left=%d\n",mainMenu_custom_left);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "custom_right=%d\n",mainMenu_custom_right);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "custom_A=%d\n",mainMenu_custom_A);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "custom_B=%d\n",mainMenu_custom_B);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "custom_X=%d\n",mainMenu_custom_X);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "custom_Y=%d\n",mainMenu_custom_Y);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "custom_L=%d\n",mainMenu_custom_L);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "custom_R=%d\n",mainMenu_custom_R);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "cpu=%d\n",mainMenu_CPU_model);
  fputs(buffer,f);
	snprintf((char*)buffer, 255, "chipset=%d\n",mainMenu_chipset);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "cpu=%d\n",mainMenu_CPU_speed);
 	fputs(buffer,f);

	if(!general)
	{
		char namebuffer[256];
		strcpy(namebuffer,uae4all_image_file0);
		replace (namebuffer,'|',' ');
		snprintf((char*)buffer, 255, "df0=%s\n",namebuffer);
		fputs(buffer,f);
		if (uae4all_image_file1[0])
		{
			strcpy(namebuffer,uae4all_image_file1);
			replace (namebuffer,'|',' ');
			snprintf((char*)buffer, 255, "df1=%s\n",namebuffer);
			fputs(buffer,f);
		}
		if (uae4all_image_file2[0])
		{
			strcpy(namebuffer,uae4all_image_file2);
			replace (namebuffer,'|',' ');	
			snprintf((char*)buffer, 255, "df2=%s\n",namebuffer);
			fputs(buffer,f);
		}
		if (uae4all_image_file3[0])
		{
			strcpy(namebuffer,uae4all_image_file3);
			replace (namebuffer,'|',' ');
			snprintf((char*)buffer, 255, "df3=%s\n",namebuffer);
			fputs(buffer,f);
		}
	}
	else
	{
		snprintf((char*)buffer, 255, "script=%d\n",mainMenu_enableScripts);
		fputs(buffer,f);
		snprintf((char*)buffer, 255, "screenshot=%d\n",mainMenu_enableScreenshots);
		fputs(buffer,f);
		snprintf((char*)buffer, 255, "skipintro=%d\n",skipintro);
		fputs(buffer,f);
		snprintf((char*)buffer, 255, "boot_hd=%d\n",mainMenu_bootHD);
		fputs(buffer,f);
		if (uae4all_hard_dir[0] == '\0')
			snprintf((char*)buffer, 255, "hard_disk_dir=%s\n","*");
		else
			snprintf((char*)buffer, 255, "hard_disk_dir=%s\n",uae4all_hard_dir);
		fputs(buffer,f);
		if (uae4all_hard_file[0] == '\0')
			snprintf((char*)buffer, 255, "hard_disk_file=%s\n","*");
		else
			snprintf((char*)buffer, 255, "hard_disk_file=%s\n",uae4all_hard_file);
		fputs(buffer,f);
	}
	snprintf((char*)buffer, 255, "chipmemory=%d\n",mainMenu_chipMemory);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "slowmemory=%d\n",mainMenu_slowMemory);
	fputs(buffer,f);
	snprintf((char*)buffer, 255, "fastmemory=%d\n",mainMenu_fastMemory);
	fputs(buffer,f);
	fclose(f);
	return 1;
}

void loadconfig(int general)
{
	// Set everthing to default and clear HD settings
//	SetDefaultMenuSettings(general);
	
	if(general)
	{
		char path1[300];
		snprintf(path1, 300, "%s/conf/adfdir.conf", launchDir);
		FILE *f1=fopen(path1,"rt");
		if(!f1)
		{
			printf ("No config file %s!\n",path1);
			strcpy(currentDir, launchDir);
			strcat(currentDir, "/roms/");
		}
		else
		{
			fscanf(f1,"path=%s\n",&currentDir);
			fclose(f1);
			printf("adfdir loaded. currentDir=%s\n", currentDir);
		}
	}

	char path[300];
	char *p; 

	if (general)
		snprintf(path, 300, "%s/conf/uaeconfig.conf", launchDir);
	else
	{
		p=uae4all_image_file0+strlen(uae4all_image_file0)-1;
		while (*p != '/') p--;
		p++;
		int len=strlen(p) + 1;
		char gamename[len];
		strcpy(gamename,p);
		char * pch;
		pch = strstr (gamename,".adf");
		if (pch) *pch='\0';
		snprintf(path, 300, "%s/conf/%s.conf", launchDir,gamename);
	}
	FILE *f=fopen(path,"rt");
	if (!f){
		printf ("No config file %s!\n",path);
	}
	else
	{
		char filebuffer[256];
		fscanf(f,"kickstart=%d\n",&kickstart);
		fscanf(f,"scaling=%d\n",&mainMenu_enableHWscaling);
		fscanf(f,"showstatus=%d\n",&mainMenu_showStatus);
		fscanf(f,"mousemultiplier=%d\n",&mainMenu_mouseMultiplier );
		fscanf(f,"systemclock=%d\n",&mainMenu_throttle);
		fscanf(f,"syncthreshold=%d\n", &timeslice_mode);
		fscanf(f,"frameskip=%d\n",&mainMenu_frameskip);
		fscanf(f,"sound=%d\n",&mainMenu_sound );
		fscanf(f,"soundrate=%d\n",&sound_rate);
		fscanf(f,"autosave=%d\n",&mainMenu_autosave);
		fscanf(f,"gp2xclock=%d\n", &gp2xClockSpeed);
		fscanf(f,"joyconf=%d\n",&mainMenu_joyConf);
		fscanf(f,"autofireRate=%d\n",&mainMenu_autofireRate);
		fscanf(f,"autofire=%d\n",&mainMenu_autofire);
		fscanf(f,"stylusOffset=%d\n",&mainMenu_stylusOffset);
		fscanf(f,"tapDelay=%d\n",&mainMenu_tapDelay);
		fscanf(f,"scanlines=%d\n",&mainMenu_scanlines);
		fscanf(f,"ham=%d\n",&mainMenu_ham);
		fscanf(f,"enableScreenshots=%d\n",&mainMenu_enableScreenshots);
		fscanf(f,"floppyspeed=%d\n",&mainMenu_floppyspeed);
		fscanf(f,"drives=%d\n",&nr_drives);
		fscanf(f,"videomode=%d\n",&mainMenu_ntsc);
		fscanf(f,"mainMenu_cpuSpeed=%d\n",&mainMenu_cpuSpeed);
		fscanf(f,"presetModeId=%d\n",&presetModeId);
		fscanf(f,"moveX=%d\n",&moveX);
		fscanf(f,"moveY=%d\n",&moveY);
		fscanf(f,"displayedLines=%d\n",&mainMenu_displayedLines);
		fscanf(f,"screenWidth=%d\n",&screenWidth);
		fscanf(f,"cutLeft=%d\n",&mainMenu_cutLeft);
		fscanf(f,"cutRight=%d\n",&mainMenu_cutRight);
		fscanf(f,"customControls=%d\n",&mainMenu_customControls);
		fscanf(f,"custom_dpad=%d\n",&mainMenu_custom_dpad);
		fscanf(f,"custom_up=%d\n",&mainMenu_custom_up);
		fscanf(f,"custom_down=%d\n",&mainMenu_custom_down);
		fscanf(f,"custom_left=%d\n",&mainMenu_custom_left);
		fscanf(f,"custom_right=%d\n",&mainMenu_custom_right);
		fscanf(f,"custom_A=%d\n",&mainMenu_custom_A);
		fscanf(f,"custom_B=%d\n",&mainMenu_custom_B);
		fscanf(f,"custom_X=%d\n",&mainMenu_custom_X);
		fscanf(f,"custom_Y=%d\n",&mainMenu_custom_Y);
		fscanf(f,"custom_L=%d\n",&mainMenu_custom_L);
		fscanf(f,"custom_R=%d\n",&mainMenu_custom_R);
		fscanf(f,"cpu=%d\n",&mainMenu_CPU_model);
		fscanf(f,"chipset=%d\n",&mainMenu_chipset);
		fscanf(f,"cpu=%d\n",&mainMenu_CPU_speed);
	
		if(!general)
		{
			fscanf(f,"df0=%s\n",&filebuffer);
			replace(filebuffer,' ','|');
			strcpy(uae4all_image_file0,filebuffer);	
			if(nr_drives > 1)
			{
				fscanf(f,"df1=%s\n",&filebuffer);
				replace(filebuffer,' ','|');
				strcpy(uae4all_image_file1,filebuffer);
				extractFileName(uae4all_image_file1,filename1);
			}
			if(nr_drives > 2)
			{
				fscanf(f,"df2=%s\n",&filebuffer);
				replace(filebuffer,' ','|');
				strcpy(uae4all_image_file2,filebuffer);
				extractFileName(uae4all_image_file2,filename2);
			}
			if(nr_drives > 3)
			{
				fscanf(f,"df3=%s\n",&filebuffer);
				replace(filebuffer,' ','|');
				strcpy(uae4all_image_file3,filebuffer);
				extractFileName(uae4all_image_file3,filename3);
			}
		}
		else
		{
			fscanf(f,"script=%d\n",&mainMenu_enableScripts);
			fscanf(f,"screenshot=%d\n", &mainMenu_enableScreenshots);
			fscanf(f,"skipintro=%d\n", &skipintro);
			fscanf(f,"boot_hd=%d\n",&mainMenu_bootHD);
			
			fscanf(f,"hard_disk_dir=",uae4all_hard_dir);
			uae4all_hard_dir[0] = '\0';
			{
				char c[2] = {0, 0};
				*c = fgetc(f);
				while (*c && (*c != '\n')) {
					strcat(uae4all_hard_dir, c);
					*c = fgetc(f);
				}
			}
			if (uae4all_hard_dir[0] == '*')
				uae4all_hard_dir[0] = '\0';
			
			fscanf(f,"hard_disk_file=",uae4all_hard_file);
			uae4all_hard_file[0] = '\0';
			{
				char c[2] = {0, 0};
				*c = fgetc(f);
				while (*c && (*c != '\n')) {
					strcat(uae4all_hard_file, c);
					*c = fgetc(f);
				}
			}
			if (uae4all_hard_file[0] == '*')
				uae4all_hard_file[0] = '\0';
		}
		mainMenu_drives=nr_drives;
	
		fscanf(f,"chipmemory=%d\n",&mainMenu_chipMemory);
		fscanf(f,"slowmemory=%d\n",&mainMenu_slowMemory);
		fscanf(f,"fastmemory=%d\n",&mainMenu_fastMemory);
	
		fclose(f);
	}

	SetPresetMode(presetModeId);
	UpdateCPUModelSettings();
	UpdateChipsetSettings();
	update_display();
	UpdateMemorySettings();
	set_joyConf();
	reset_hdConf();
}

void parse_cmdline(int argc, char **argv)
{
#ifndef PANDORA
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
#endif
}
