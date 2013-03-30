#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu.h"
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "sysconfig.h"
#include "sysdeps.h"
#include "uae.h"
#include "options.h"
#include "sound.h"
#include <SDL.h>
#include "gp2x.h"
#include <SDL_ttf.h>
#include "menu_config.h"

char *text_str_display_separator="----------------------------------";
char *text_str_display_title=    "        Display Settings        -";

#define MAX_CUSTOM_ID 96
#define MIN_CUSTOM_ID -5

int menuDisplay = 0;

extern int moveY;
extern int screenWidth;

enum { 
	MENUDISPLAY_RETURNMAIN = 0,
	MENUDISPLAY_PRESETWIDTH,
	MENUDISPLAY_PRESETHEIGHT,
	MENUDISPLAY_DISPLINES,
	MENUDISPLAY_SCREENWIDTH,
	MENUDISPLAY_VERTPOS,
	MENUDISPLAY_CUTLEFT,
	MENUDISPLAY_CUTRIGHT,
	MENUDISPLAY_FRAMESKIP,
	MENUDISPLAY_REFRESHRATE,
	MENUDISPLAY_END
};



static void draw_displayMenu(int c)
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
	char value[20]="";
	r.x=80-64; r.y=0; r.w=110+64+64; r.h=240;

	text_draw_background();
	text_draw_window(2,2,40,30,text_str_display_title);

	// 0
	if (menuDisplay == MENUDISPLAY_RETURNMAIN && bb)
		write_text_inv(3, menuLine, "Return to main menu");
	else
		write_text(3, menuLine, "Return to main menu");

	menuLine++;
	write_text(leftMargin,menuLine,text_str_display_separator);
	menuLine++;

	// 1
	write_text(leftMargin,menuLine,"Preset Width");
	snprintf(value, 20, "%d", (presetModeId < 10) ? 320 : 640);
	if ((menuDisplay!=MENUDISPLAY_PRESETWIDTH)||(bb))
		write_text(tabstop3,menuLine,value);
	else
		write_text_inv(tabstop3,menuLine,value);

	// 2
	menuLine+=2;
	write_text(leftMargin,menuLine,"Preset Height");
	if ((menuDisplay!=MENUDISPLAY_PRESETHEIGHT)||(bb))
		write_text(tabstop3,menuLine,presetMode);
	else
		write_text_inv(tabstop3,menuLine,presetMode);

	menuLine++;
	write_text(leftMargin,menuLine,text_str_display_separator);
	menuLine+=2;
	write_text(leftMargin,menuLine,"Custom Settings");
	menuLine++;
	write_text(leftMargin,menuLine,"---------------");
	menuLine++;

	// 3
	write_text(leftMargin,menuLine,"Displayed Lines");
	sprintf(value, "%d", mainMenu_displayedLines);
	if ((menuDisplay!=MENUDISPLAY_DISPLINES)||(bb))
		write_text(tabstop3,menuLine,value);
	else
		write_text_inv(tabstop3,menuLine,value);

	// 4
	menuLine+=2;
	write_text(leftMargin,menuLine,"Screen Width");
	sprintf(value, "%d", screenWidth);
	if ((menuDisplay!=MENUDISPLAY_SCREENWIDTH)||(bb))
		write_text(tabstop3,menuLine,value);
	else
		write_text_inv(tabstop3,menuLine,value);

	// 5
	menuLine+=2;
	write_text(leftMargin,menuLine,"Vertical Position");
	sprintf(value, "%d", moveY);
	if ((menuDisplay!=MENUDISPLAY_VERTPOS)||(bb))
		write_text(tabstop3,menuLine,value);
	else
		write_text_inv(tabstop3,menuLine,value);

	// 6
	menuLine+=2;
	write_text(leftMargin,menuLine,"Cut Left");
	sprintf(value, "%d", mainMenu_cutLeft);
	if ((menuDisplay!=MENUDISPLAY_CUTLEFT)||(bb))
		write_text(tabstop3,menuLine,value);
	else
		write_text_inv(tabstop3,menuLine,value);

	// 7
	menuLine+=2;
	write_text(leftMargin,menuLine,"Cut Right");
	sprintf(value, "%d", mainMenu_cutRight);
	if ((menuDisplay!=MENUDISPLAY_CUTRIGHT)||(bb))
		write_text(tabstop3,menuLine,value);
	else
		write_text_inv(tabstop3,menuLine,value);

	// 8
	menuLine++;
	write_text(leftMargin,menuLine,text_str_display_separator);
	menuLine++;
	write_text(leftMargin,menuLine,"Frameskip");
	if ((mainMenu_frameskip==0)&&((menuDisplay!=MENUDISPLAY_FRAMESKIP)||(bb)))
		write_text_inv(tabstop1,menuLine,"0");
	else
		write_text(tabstop1,menuLine,"0");
	if ((mainMenu_frameskip==1)&&((menuDisplay!=MENUDISPLAY_FRAMESKIP)||(bb)))
		write_text_inv(tabstop3,menuLine,"1");
	else
		write_text(tabstop3,menuLine,"1");
	if ((mainMenu_frameskip==2)&&((menuDisplay!=MENUDISPLAY_FRAMESKIP)||(bb)))
		write_text_inv(tabstop5,menuLine,"2");
	else
		write_text(tabstop5,menuLine,"2");
	if ((mainMenu_frameskip==3)&&((menuDisplay!=MENUDISPLAY_FRAMESKIP)||(bb)))
		write_text_inv(tabstop7,menuLine,"3");
	else
		write_text(tabstop7,menuLine,"3");
	if ((mainMenu_frameskip==4)&&((menuDisplay!=MENUDISPLAY_FRAMESKIP)||(bb)))
		write_text_inv(tabstop9,menuLine,"4");
	else
		write_text(tabstop9,menuLine,"4");

	// 9
	menuLine+=2;
	write_text(leftMargin,menuLine,"Refresh Rate");
	if ((!mainMenu_ntsc)&&((menuDisplay!=MENUDISPLAY_REFRESHRATE)||(bb)))
		write_text_inv(tabstop1,menuLine,"50Hz");
	else
		write_text(tabstop1,menuLine,"50Hz");

	if ((mainMenu_ntsc)&&((menuDisplay!=MENUDISPLAY_REFRESHRATE)||(bb)))
		write_text_inv(tabstop3+1,menuLine,"60Hz");
	else
		write_text(tabstop3+1,menuLine,"60Hz");

	text_flip();
	b++;
}

static int key_displayMenu(int *c)
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
		if (menuDisplay==0) menuDisplay = MENUDISPLAY_END - 1;
		else menuDisplay--;
	}
	else if (down)
	{
		if (menuDisplay == MENUDISPLAY_END - 1) menuDisplay=0;
		else menuDisplay++;
	}
	switch (menuDisplay)
	{
		case MENUDISPLAY_PRESETWIDTH:
			if (left || right)
			{
				if(presetModeId < 10)
					SetPresetMode(presetModeId + 10);
				else
					SetPresetMode(presetModeId - 10);
				update_display();
			}
			break;
		case MENUDISPLAY_PRESETHEIGHT:
			if (left)
			{
				switch(presetModeId)
				{
					case 0:
					case 10:
						SetPresetMode(presetModeId + 7);
						break;
					default:
						SetPresetMode(presetModeId - 1);
				}
				update_display();
			}
			else if (right)
			{
				switch(presetModeId)
				{
					case 7:
					case 17:
						SetPresetMode(presetModeId - 7);
						break;
					default:
						SetPresetMode(presetModeId + 1);
				}
				update_display();
			}
			break;
		case MENUDISPLAY_DISPLINES:
			if (left)
			{
				if (mainMenu_displayedLines>100)
					mainMenu_displayedLines--;
				update_display();
			}
			else if (right)
			{
				if (mainMenu_displayedLines<270)
					mainMenu_displayedLines++;
				update_display();
			}
			break;
		case MENUDISPLAY_SCREENWIDTH:
			if (left)
			{
				screenWidth-=10;
				if (screenWidth<200)
					screenWidth=200;
				update_display();
			}
			else if (right)
			{
				screenWidth+=10;
				if (screenWidth>800)
					screenWidth=800;
				update_display();
			}
			break;
		case MENUDISPLAY_VERTPOS:
			if (left)
			{
				if (moveY>-42)
					moveY--;
			}
			else if (right)
			{
				if (moveY<50)
					moveY++;
			}
			break;
		case MENUDISPLAY_CUTLEFT:
			if (left)
			{
				if (mainMenu_cutLeft>0)
					mainMenu_cutLeft--;
				update_display();
			}
			else if (right)
			{
				if (mainMenu_cutLeft<100)
					mainMenu_cutLeft++;
				update_display();
			}
			break;
		case MENUDISPLAY_CUTRIGHT:
			if (left)
			{
				if (mainMenu_cutRight>0)
					mainMenu_cutRight--;
				update_display();
			}
			else if (right)
			{
				if (mainMenu_cutRight<100)
					mainMenu_cutRight++;
				update_display();
			}
			break;
		case MENUDISPLAY_FRAMESKIP:
			if (left)
			{
				if (mainMenu_frameskip>0)
					mainMenu_frameskip--;
				else
					mainMenu_frameskip=4;
			}
			else if (right)
			{
				if (mainMenu_frameskip<4)
					mainMenu_frameskip++;
				else
					mainMenu_frameskip=0;
			}
			break;
		case MENUDISPLAY_REFRESHRATE:
			if ((left)||(right))
					mainMenu_ntsc = !mainMenu_ntsc;
			break;
	}

	return end;
}

static void raise_displayMenu()
{
	update_display();
	int i;

	text_draw_background();
	text_flip();
	for(i=0;i<10;i++)
	{
		text_draw_background();
		text_draw_window(80-64,(10-i)*24,160+64+64,220,text_str_display_title);
		text_flip();
	}
}

static void unraise_displayMenu()
{
	int i;

	for(i=9;i>=0;i--)
	{
		text_draw_background();
		text_draw_window(80-64,(10-i)*24,160+64+64,220,text_str_display_title);
		text_flip();
	}
	text_draw_background();
	text_flip();
}

int run_menuDisplay()
{
	SDL_Event event;
	SDL_Delay(150);
	while(SDL_PollEvent(&event))
		SDL_Delay(10);
	int end=0, c=0;
	raise_displayMenu();
	while(!end)
	{
		draw_displayMenu(c);
		end=key_displayMenu(&c);
	}
	set_joyConf();
	unraise_displayMenu();
	return end;
}
