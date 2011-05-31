#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "config.h"
#include "menu.h"

#include <SDL_mixer.h>

#include "sysconfig.h"
#include "sysdeps.h"
#include "uae.h"
#include "options.h"
#include "zfile.h"
#include "sound.h"

#include "msg.h"
#include "fade.h"

#ifdef DREAMCAST
#define VIDEO_FLAGS_INIT SDL_HWSURFACE|SDL_FULLSCREEN
#endif

#ifdef GP2X
#define VIDEO_FLAGS_INIT SDL_SWSURFACE|SDL_FULLSCREEN
#endif

#if ! defined (DREAMCAST) && ! defined (GP2X)
#define VIDEO_FLAGS_INIT SDL_HWSURFACE
#endif

#ifdef DOUBLEBUFFER
#define VIDEO_FLAGS VIDEO_FLAGS_INIT | SDL_DOUBLEBUF
#else
#define VIDEO_FLAGS VIDEO_FLAGS_INIT
#endif

SDL_Surface *text_screen=NULL, *text_image, *text_background, *text_window_background;

static Uint32 menu_inv_color=0, menu_win0_color=0, menu_win1_color=0;
static Uint32 menu_barra0_color=0, menu_barra1_color=0;
static Uint32 menu_win0_color_base=0, menu_win1_color_base=0;

void write_text_pos(int x, int y, char * str);
void write_num(int x, int y, int v);
int menu_msg_pos=330;
int menu_moving=1;
Uint32 menu_msg_time=0x12345678;
int skipintro=0;

#ifdef DREAMCAST
extern int __sdl_dc_emulate_keyboard;
#endif

static void obten_colores(void)
{
	FILE *f=fopen(DATA_PREFIX "colors.txt", "rt");
	if (f)
	{
		Uint32 r,g,b;
		fscanf(f,"menu_inv_color=0x%X,0x%X,0x%X\n",&r,&g,&b);
		menu_inv_color=SDL_MapRGB(text_screen->format,r,g,b);
		fscanf(f,"menu_win0_color=0x%X,0x%X,0x%X\n",&r,&g,&b);
		menu_win0_color=SDL_MapRGB(text_screen->format,r,g,b);
		fscanf(f,"menu_win1_color=0x%X,0x%X,0x%X\n",&r,&g,&b);
		menu_win1_color=SDL_MapRGB(text_screen->format,r,g,b);
		fscanf(f,"menu_barra0_color=0x%X,0x%X,0x%X\n",&r,&g,&b);
		menu_barra0_color=SDL_MapRGB(text_screen->format,r,g,b);
		fscanf(f,"menu_barra1_color=0x%X,0x%X,0x%X\n",&r,&g,&b);
		menu_barra1_color=SDL_MapRGB(text_screen->format,r,g,b);
		fclose(f);
	}
	else
	{
		menu_inv_color=SDL_MapRGB(text_screen->format, 0x20, 0x20, 0x40);
		menu_win0_color=SDL_MapRGB(text_screen->format, 0x10, 0x08, 0x08);
		menu_win1_color=SDL_MapRGB(text_screen->format, 0x20, 0x10, 0x10);
		menu_barra0_color=SDL_MapRGB(text_screen->format, 0x30, 0x20, 0x20);
		menu_barra1_color=SDL_MapRGB(text_screen->format, 0x50, 0x40, 0x40);
	}
	menu_win0_color_base=menu_win0_color;
	menu_win1_color_base=menu_win1_color;
}

void menu_raise(void)
{
	int i;
	for(i=80;i>=0;i-=16)
	{
#if !defined(NO_SOUND) && defined(MENU_MUSIC)
		Mix_VolumeMusic(MUSIC_VOLUME-(i<<1));
#endif
		text_draw_background();
		fade16(text_screen,i);
		text_flip();
		SDL_Delay(10);
	}
}

void menu_unraise(void)
{
	int i;
	for(i=0;i<=80;i+=16)
	{
#if !defined(NO_SOUND) && defined(MENU_MUSIC)
		Mix_VolumeMusic(MUSIC_VOLUME-(i<<1));
#endif
		text_draw_background();
		fade16(text_screen,i);
		text_flip();
		SDL_Delay(10);
	}
}

static void text_draw_menu_msg()
{
	write_text_pos(menu_msg_pos,0,menu_msg);
	if (menu_msg_pos<MAX_SCROLL_MSG)
		menu_msg_pos=330;
	else
		menu_msg_pos--;
}


static void update_window_color(void)
{
	static int cambio=0;
	static int spin=0;

	Uint8 r,g,b;
	int cambio2=cambio>>3;
	SDL_GetRGB(menu_win0_color_base,text_screen->format,&r,&g,&b);
	if (((int)r)-cambio2>0) r-=cambio2;
	else r=0;
	if (((int)g)-cambio2>0) g-=cambio2;
	else g=0;
	if (((int)b)-cambio2>0) b-=cambio2;
	else b=0;
	menu_win0_color=SDL_MapRGB(text_screen->format,r,g,b);
	SDL_GetRGB(menu_win1_color_base,text_screen->format,&r,&g,&b);
	if (((int)r)-cambio>0) r-=cambio;
	else r=0;
	if (((int)g)-cambio>0) g-=cambio;
	else g=0;
	if (((int)b)-cambio>0) b-=cambio;
	else b=0;
	menu_win1_color=SDL_MapRGB(text_screen->format,r,g,b);
	if (spin)
	{
		if (cambio<=0) spin=0;
		else cambio-=2;

	}
	else
	{
		if (cambio>=24) spin=1;
		else cambio+=2;
	}
}

void text_draw_background()
{
	static int pos_x=12345678;
	static int pos_y=12345678;
	SDL_Rect r;
	int i,j;
	int w=text_screen->w+text_background->w-1;
	int h=text_screen->h+text_background->h-1;

	if (menu_moving)
	{
		if (pos_x>=0) pos_x=-text_screen->w;
		else pos_x++;
		if (pos_y>=0) pos_y=-text_screen->h;
		else pos_y++;
	}

	for(i=pos_x;i<w;i+=text_background->w)
		for(j=pos_y;j<h;j+=text_background->h)
		{
			r.x=i;
			r.y=j;
			r.w=text_background->w;
			r.h=text_background->h;
			SDL_BlitSurface(text_background,NULL,text_screen,&r);
		}
	if (menu_moving)
	{
#ifdef DREAMCAST
		text_draw_menu_msg();
#endif
		update_window_color();
	}
}

void text_flip(void)
{
	SDL_Delay(10);
	SDL_BlitSurface(text_screen,NULL,prSDLScreen,NULL);
	SDL_Flip(prSDLScreen);
}

void init_text(int splash)
{
	SDL_Surface *tmp;

	if (prSDLScreen==NULL)
	{
		SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK);
		prSDLScreen=SDL_SetVideoMode(320,240,16,VIDEO_FLAGS);
    		SDL_ShowCursor(SDL_DISABLE);
 	   	SDL_JoystickEventState(SDL_ENABLE);
    		SDL_JoystickOpen(0);
	}
#ifdef DREAMCAST
        __sdl_dc_emulate_keyboard=1;
#endif
	if (!text_screen)
	{
		text_screen=SDL_CreateRGBSurface(prSDLScreen->flags,prSDLScreen->w,prSDLScreen->h,prSDLScreen->format->BitsPerPixel,prSDLScreen->format->Rmask,prSDLScreen->format->Gmask,prSDLScreen->format->Bmask,prSDLScreen->format->Amask);
		tmp=SDL_LoadBMP(MENU_FILE_TEXT);
		if (text_screen==NULL || tmp==NULL)
			exit(-1);
		text_image=SDL_DisplayFormat(tmp);
		SDL_FreeSurface(tmp);
		if (text_image==NULL)
			exit(-2);
		SDL_SetColorKey(text_image,(SDL_SRCCOLORKEY | SDL_RLEACCEL),SDL_MapRGB(text_image -> format, 0, 0, 0));
		tmp=SDL_LoadBMP(MENU_FILE_BACKGROUND);
		if (tmp==NULL)
			exit(-3);
		text_background=SDL_DisplayFormat(tmp);
		SDL_FreeSurface(tmp);
		if (text_background==NULL)
			exit(-3);
		tmp=SDL_LoadBMP(MENU_FILE_WINDOW);
		if (tmp==NULL)
			exit(-4);
		text_window_background=SDL_DisplayFormat(tmp);
		SDL_FreeSurface(tmp);
		if (text_window_background==NULL)
			exit(-5);
	}
	if (splash)
	{
		SDL_Surface *sur;
		SDL_Rect r;
		int i,j;

		obten_colores();
		uae4all_init_sound();
		if (skipintro) goto skipintro;
#if !defined(DEBUG_UAE4ALL) && !defined(PROFILER_UAE4ALL) && !defined(AUTO_RUN) && !defined(AUTO_FRAMERATE)
		tmp=SDL_LoadBMP(MENU_FILE_SPLASH);
		if (tmp==NULL)
			exit(-6);
		sur = SDL_DisplayFormat(tmp);
		SDL_FreeSurface(tmp);
		r.x=(text_screen->w - sur->w)/2;
		r.y=(text_screen->h - sur->h)/2;
		r.h=sur->w;
		r.w=sur->h;
		SDL_FillRect(text_screen,NULL,0xFFFFFFFF);
		for (i=128;i>-8;i-=8)
		{
#ifdef DREAMCAST
			vid_waitvbl();
#else
			SDL_Delay(50);
#endif
			SDL_FillRect(text_screen,NULL,0xFFFFFFFF);
			SDL_BlitSurface(sur,NULL,text_screen,&r);
			fade16(text_screen,i);
			text_flip();
		}
		SDL_Delay(3000);
		for(i=0;i<128;i+=16)
		{
#ifdef DREAMCAST
			vid_waitvbl();
#else
			SDL_Delay(50);
#endif
			SDL_FillRect(text_screen,NULL,0xFFFFFFFF);
			SDL_BlitSurface(sur,NULL,text_screen,&r);
			fade16(text_screen,i);
			text_flip();
		}
		for(i=128;i>-8;i-=8)
		{
#ifdef DREAMCAST
			vid_waitvbl();
#else
			SDL_Delay(50);
#endif
			text_draw_background();
			fade16(text_screen,i);
			text_flip();
		}
		SDL_FreeSurface(sur);
#else
#if !defined (DREAMCAST) && !defined (GP2X)
		chdir("example");
#else
#ifdef DREAMCAST
		fs_chdir("/cd");
#endif
#endif
#endif
skipintro:
		for(i=0;i<10;i++)
		{
			SDL_Event ev;
			if (!uae4all_init_rom(romfile))
				break;

			text_draw_background();
			text_draw_window(54,91,250,64,"--- ERROR ---");
			write_text(11,14,"KICK.ROM not found");
			write_text(8,16,"Press any button to retry");
			text_flip();
			SDL_Delay(333);
			while(SDL_PollEvent(&ev))
#ifndef DREAMCAST
				if (ev.type==SDL_QUIT)
					exit(1);
				else
#endif
				SDL_Delay(10);
			while(!SDL_PollEvent(&ev))
				SDL_Delay(10);
			while(SDL_PollEvent(&ev))
				if (ev.type==SDL_QUIT)
					exit(1);
			text_draw_background();
			text_flip();
			SDL_Delay(333);
		}
		if (i>=10)
			exit(1);
	}
	else
	{
		SDL_FillRect(text_screen,NULL,0xFFFFFFFF);
		text_flip();
		uae4all_resume_music();
	}
	menu_msg_time=SDL_GetTicks();
}


void quit_text(void)
{
/*
	SDL_FreeSurface(text_image);
	SDL_FreeSurface(text_background);
	SDL_FreeSurface(text_window_background);
	SDL_FreeSurface(text_screen);
*/
}

void write_text_pos(int x, int y, char * str)
{
  int i, c;
  SDL_Rect src, dest;
  
  for (i = 0; i < strlen(str); i++)
    {
      c = -1;
      
      if (str[i] >= '0' && str[i] <= '9')
	c = str[i] - '0';
      else if (str[i] >= 'A' && str[i] <= 'Z')
	c = str[i] - 'A' + 10;
      else if (str[i] >= 'a' && str[i] <= 'z')
	c = str[i] - 'a' + 36;
      else if (str[i] == '#')
	c = 62;
      else if (str[i] == '=')
	c = 63;
      else if (str[i] == '.')
	c = 64;
      else if (str[i] == '_')
	c = -2;
      else if (str[i] == '-')
	c = -3;
      else if (str[i] == '(')
	c = 65;
      else if (str[i] == ')')
	c = 66;
      
      if (c >= 0)
	{
	  src.x = c * 8;
	  src.y = 0;
	  src.w = 8;
	  src.h = 8;
	  
	  dest.x = x + (i * 8);
	  dest.y = y;
	  dest.w = 8;
	  dest.h = 8;
	  
	  SDL_BlitSurface(text_image, &src,
			  text_screen, &dest);
	}
      else if (c == -2 || c == -3)
	{
	  dest.x = x + (i * 8);
	  
	  if (c == -2)
	    dest.y = y  + 7;
	  else if (c == -3)
	    dest.y = y  + 3;
	  
	  dest.w = 8;
	  dest.h = 1;
	  
	  SDL_FillRect(text_screen, &dest, menu_barra0_color);
	}
    }
}

void write_text(int x, int y, char * str)
{
  int i, c;
  SDL_Rect src, dest;
  
  for (i = 0; i < strlen(str); i++)
    {
      c = -1;
      
      if (str[i] >= '0' && str[i] <= '9')
	c = str[i] - '0';
      else if (str[i] >= 'A' && str[i] <= 'Z')
	c = str[i] - 'A' + 10;
      else if (str[i] >= 'a' && str[i] <= 'z')
	c = str[i] - 'a' + 36;
      else if (str[i] == '#')
	c = 62;
      else if (str[i] == '=')
	c = 63;
      else if (str[i] == '.')
	c = 64;
      else if (str[i] == '_')
	c = -2;
      else if (str[i] == '-')
	c = -3;
      else if (str[i] == '(')
	c = 65;
      else if (str[i] == ')')
	c = 66;
      
      if (c >= 0)
	{
	  src.x = c * 8;
	  src.y = 0;
	  src.w = 8;
	  src.h = 8;
	  
	  dest.x = (x + i) * 8;
	  dest.y = y * 7; //10;
	  dest.w = 8;
	  dest.h = 8;
	  
	  SDL_BlitSurface(text_image, &src,
			  text_screen, &dest);
	}
      else if (c == -2 || c == -3)
	{
	  dest.x = (x + i) * 8;
	  
	  if (c == -2)
	    dest.y = y * 7 /*10*/ + 7;
	  else if (c == -3)
	    dest.y = y * 7 /*10*/ + 3;
	  
	  dest.w = 8;
	  dest.h = 1;
	  
	  SDL_FillRect(text_screen, &dest, menu_barra0_color);
	}
    }
}


/* Write text, inverted: */

void write_text_inv(int x, int y, char * str)
{
  SDL_Rect dest;
  
  
  dest.x = (x * 8) -2 ;
  dest.y = (y * 7) /*10*/ - 2;
  dest.w = (strlen(str) * 8) + 4;
  dest.h = 12;

  SDL_FillRect(text_screen, &dest, menu_inv_color);

  write_text(x, y, str);
}


/* Write text, horizontally centered... */

void write_centered_text(int y, char * str)
{
  write_text(20 - (strlen(str) / 2), y/2, str);
}


/* Write numbers on the option prSDLScreen: */

void write_num(int x, int y, int v)
{
  char str[24];
  
  sprintf(str, "%d", v);
  write_text(x, y, str);
}

void write_num_inv(int x, int y, int v)
{
  SDL_Rect dest;
  int i,l=1;

  for(i=10;i<1000000;i*=10)
	if (v/i)
		l++;
  	else
		break;
  	
  dest.x = (x * 8) -2 ;
  dest.y = (y * 8) /*10*/ - 2;
  dest.w = (l * 8) + 4;
  dest.h = 12;

  SDL_FillRect(text_screen, &dest, menu_inv_color);

  write_num(x, y, v);
}


void text_draw_window(int x, int y, int w, int h, char *title)
{
	int i,j;
	int r8x = x / 8;
	int r8y = y / 7;
	int rx = r8x * 8;
	int ry = r8y * 7;
	int r32w =  w / 32;
	int r24h =  h / 24;
	int rw = r32w * 32;
	int rh = r24h * 24;
	int r8w = rw / 8;

	SDL_Rect dest;

	dest.x = rx + 6;
	dest.y = ry - 4;
	dest.w = rw + 6;
	dest.h = rh + 18;
	SDL_FillRect(text_screen, &dest, menu_win0_color);

	dest.x = rx - 2;
	dest.y = ry - 10; //12;
	dest.w = rw + 4;
	dest.h = rh + 14; //16;
	SDL_FillRect(text_screen, &dest, menu_win1_color);

	for(i=0;i<r32w;i++)
		for(j=0;j<r24h;j++)
		{
			dest.x=rx+i*32;
			dest.y=ry+j*24;
			dest.w=32;
			dest.h=24;
			SDL_BlitSurface(text_window_background,NULL,text_screen,&dest);

		}

	write_text(r8x, r8y - 1, "OOO");
	write_text(r8x + ((r8w-strlen(title)) / 2), r8y - 1, title);

}
