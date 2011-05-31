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

#include "gp2x.h"
#include "cpuspeed/cpuctrl.h"

char *text_str_clock_separator="----------------------------------";
char *text_str_clock_title=    "            Overclocking           -";
int menuClockRate = -1;
extern int gp2xClockSpeed;


static void draw_overclockMenu(int c)
{
	//int i,j;
	int menuLine = 0;
	static int b=0;
	int bb=(b%6)/3;
	SDL_Rect r;
	extern SDL_Surface *text_screen;
	r.x=80-64; r.y=0; r.w=110+64+64; r.h=240;

	text_draw_background();
	text_draw_window(80-64,12,160+64+64,220,text_str_clock_title);

	menuLine = 2;
	write_text(3, menuLine, "GP2X clock speed");
	menuLine+=2;
	write_text(3, menuLine, text_str_clock_separator);
	
	menuLine+=2;
	if (menuClockRate == -1 && bb)
		write_text_inv(3, menuLine, "Cancel and return to main menu");
	else
		write_text(3, menuLine, "Cancel and return to main menu");

	menuLine+=2;

	if (menuClockRate == 200 && bb)
		write_text_inv(3, menuLine, "200 MHz");
	else
		write_text(3, menuLine, "200 MHz");

	menuLine+=2;

	if (menuClockRate == 225 && bb)
		write_text_inv(3, menuLine, "225 MHz");
	else
		write_text(3, menuLine, "225 MHz");

	menuLine+=2;

	if (menuClockRate == 240 && bb)
		write_text_inv(3, menuLine, "240 MHz");
	else
		write_text(3, menuLine, "240 MHz");

	menuLine+=2;

	if (menuClockRate == 250 && bb)
		write_text_inv(3, menuLine, "250 MHz");
	else
		write_text(3, menuLine, "250 MHz");

	menuLine+=2;

	if (menuClockRate == 266 && bb)
		write_text_inv(3, menuLine, "266 MHz");
	else
		write_text(3, menuLine, "266 MHz");

	menuLine+=2;

	if (menuClockRate == 275 && bb)
		write_text_inv(3, menuLine, "275 MHz");
	else
		write_text(3, menuLine, "275 MHz");

	text_flip();
	b++;
}

static int key_overclockMenu(int *c)
{
	int end=0;
	int left=0, right=0, up=0, down=0, hit0=0, hit1=0;
	SDL_Event event;

	while (SDL_PollEvent(&event) > 0)
	{
		if (event.type == SDL_QUIT)
			end=-1;
		else if (event.type == SDL_JOYBUTTONDOWN)
		{
             switch (event.jbutton.button)
             {
                 case GP2X_BUTTON_UP: up=1; break;
                 case GP2X_BUTTON_DOWN: down=1; break;
                 case GP2X_BUTTON_B: hit0=1; break;
				 case GP2X_BUTTON_START: hit1=1; break;
             }
        }

		if (hit0)
		{
			if (menuClockRate != -1)
			{
				// set the clock rate and close
				gp2xClockSpeed = menuClockRate;
				//printf("gp2xClockSpeed = %d\n", gp2xClockSpeed);
				setGP2XClock(gp2xClockSpeed);
			}
			end =-1;
		}
		else if (hit1)
		{
			// cancel
			end=-1;
		}
		else if (up)
		{
			if (menuClockRate == -1)
			{
				menuClockRate = 275;
			}
			else if (menuClockRate == 200)
			{
				menuClockRate = -1;
			}
			else if (menuClockRate == 225)
			{
				menuClockRate = 200;
			}

			else if (menuClockRate == 240)
			{
				menuClockRate = 225;
			}
			else if (menuClockRate == 250)
			{
				menuClockRate = 240;
			}

			else if (menuClockRate == 266)
			{
				menuClockRate = 250;
			}
			else if (menuClockRate == 275)
			{
				menuClockRate = 266;
			}

		}
		else if (down)
		{
			if (menuClockRate == -1)
			{
				menuClockRate = 200;
			}
			else if (menuClockRate == 200)
			{
				menuClockRate = 225;
			}
			else if (menuClockRate == 225)
			{
				menuClockRate = 240;
			}
			else if (menuClockRate == 240)
			{
				menuClockRate = 250;
			}
			else if (menuClockRate == 250)
			{
				menuClockRate = 266;
			}
			else if (menuClockRate == 266)
			{
				menuClockRate = 275;
			}
			else if (menuClockRate == 275)
			{
				menuClockRate = -1;
			}
		}
	}

	return end;
}

static void raise_overclockMenu()
{
	int i;

	text_draw_background();
	text_flip();
	for(i=0;i<10;i++)
	{
		text_draw_background();
		text_draw_window(80-64,(10-i)*24,160+64+64,220,text_str_clock_title);
		text_flip();
	}
}

static void unraise_overclockMenu()
{
	int i;

	for(i=9;i>=0;i--)
	{
		text_draw_background();
		text_draw_window(80-64,(10-i)*24,160+64+64,220,text_str_clock_title);
		text_flip();
	}
	text_draw_background();
	text_flip();
}

int run_menuOverclock()
{
	int end=0,c=0;
	
	//menuClockRate = gp2xClockSpeed;

	raise_overclockMenu();
	while(!end)
	{
		draw_overclockMenu(c);
		end=key_overclockMenu(&c);
	}
	unraise_overclockMenu();

	return end;
}
