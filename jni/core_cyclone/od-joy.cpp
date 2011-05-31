 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * Joystick emulation for Linux and BSD. They share too much code to
  * split this file.
  * 
  * Copyright 1997 Bernd Schmidt
  * Copyright 1998 Krister Walfridsson
  */
#include "sysconfig.h"
#include "sysdeps.h"
#include "config.h"
#include "uae.h"
#include "options.h"
#include "memorya.h"
#include "custom.h"
#include "joystick.h"
#include "SDL.h"
#include "vkbd.h"
#ifdef GP2X
#include "gp2x.h"
#include "xwin.h"
extern int gp2xMouseEmuOn;
extern int mainMenu_mouseMultiplier;
#endif


#ifdef PSP
#include "psp.h"
#include "xwin.h"
extern int gp2xMouseEmuOn;
// XXX fix this properly
int mainMenu_mouseMultiplier = 1;
#endif

#ifdef GIZMONDO
#include "gizmondo.h"
#include "xwin.h"
extern int gp2xMouseEmuOn;
extern int mainMenu_mouseMultiplier;
#endif

int nr_joysticks;
SDL_Joystick *uae4all_joy0, *uae4all_joy1;

#if !defined (DREAMCAST) && !defined (GP2X) && !defined (GIZMONDO)
struct joy_range
{
    int minx, maxx, miny, maxy;
} range0, range1;
#endif
void read_joystick(int nr, unsigned int *dir, int *button)
{
#ifndef MAX_AUTOEVENTS
    int x_axis, y_axis;
    int left = 0, right = 0, top = 0, bot = 0;
    int len, i, num;
    SDL_Joystick *joy = nr == 0 ? uae4all_joy0 : uae4all_joy1;
    *dir = 0;
    *button = 0;
    nr = (~nr)&0x1;
	SDL_JoystickUpdate ();
#if !defined (DREAMCAST) && !defined (GP2X) && !defined (GIZMONDO) && !defined (PSP) && !defined (ANDROID)
    struct joy_range *r = nr == 0 ? &range0 : &range1;
    x_axis = SDL_JoystickGetAxis (joy, 0);
    y_axis = SDL_JoystickGetAxis (joy, 1);
    if (x_axis < r->minx) r->minx = x_axis;
    if (y_axis < r->miny) r->miny = y_axis;
    if (x_axis > r->maxx) r->maxx = x_axis;
    if (y_axis > r->maxy) r->maxy = y_axis;
    
    if (x_axis < (r->minx + (r->maxx - r->minx)/3))
    	left = 1;
    else if (x_axis > (r->minx + 2*(r->maxx - r->minx)/3))
    	right = 1;
    if (y_axis < (r->miny + (r->maxy - r->miny)/3))
    	top = 1;
    else if (y_axis > (r->miny + 2*(r->maxy - r->miny)/3))
    	bot = 1;

    num = SDL_JoystickNumButtons (joy);
    if (num > 16)
	num = 16;
    for (i = 0; i < num; i++)
	*button |= (SDL_JoystickGetButton (joy, i) & 1) << i;
    if (left) top = !top;
    if (right) bot = !bot;
    *dir = bot | (right << 1) | (top << 8) | (left << 9);
#else
#ifdef DREAMCAST
    int hat=15^(SDL_JoystickGetHat(joy,0));
    if (hat & SDL_HAT_LEFT)
	    left = 1;
    else if (hat & SDL_HAT_RIGHT)
	    right = 1;
    if (hat & SDL_HAT_UP)
	    top = 1;
    else if (hat & SDL_HAT_DOWN)
	    bot = 1;
    if (vkbd_button2==(SDLKey)0)
    	top |= SDL_JoystickGetButton(joy,6) & 1;
    *button = SDL_JoystickGetButton(joy,2) & 1;
    
    if(vkbd_mode && nr)
    {
    	if (left)
		vkbd_move |= VKBD_LEFT;
	else
	{
		vkbd_move &= ~VKBD_LEFT;
		if (right)
			vkbd_move |= VKBD_RIGHT;
		else
			vkbd_move &= ~VKBD_RIGHT;
	}
	if (top)
		vkbd_move |= VKBD_UP;
	else
	{
		vkbd_move &= ~VKBD_UP;
		if (bot)
			vkbd_move |= VKBD_DOWN;
		else
			vkbd_move &= ~VKBD_DOWN;
	}
	if (*button)
	{
		vkbd_move=VKBD_BUTTON;
		*button=0;
	}
	else if (SDL_JoystickGetButton(joy,6)&1)
		vkbd_move=VKBD_BUTTON2;
    }
    else
    {
    	if (left) top = !top;
    	if (right) bot = !bot;
    	*dir = bot | (right << 1) | (top << 8) | (left << 9);
    }
#endif
#ifdef GP2X
	int mouseScale = mainMenu_mouseMultiplier;

	if (SDL_JoystickGetButton(joy, GP2X_BUTTON_Y) && gp2xMouseEmuOn)
	{
		mouseScale = mainMenu_mouseMultiplier * 2;
	}
    

    if (SDL_JoystickGetButton(joy, GP2X_BUTTON_LEFT))
    {
      if (gp2xMouseEmuOn)
      {
         lastmx -= mouseScale;
         newmousecounters=1;
      }
      else
         left = 1;
    }
	if (SDL_JoystickGetButton(joy, GP2X_BUTTON_UPLEFT))
    {
      if (gp2xMouseEmuOn)
      {
         lastmx -= mouseScale;
         lastmy -= mouseScale;
         newmousecounters=1;
      }
      else
	  {
         top = 1;
         left = 1;
      }
    }
	if (SDL_JoystickGetButton(joy, GP2X_BUTTON_DOWNLEFT))
    {
      if (gp2xMouseEmuOn)
      {
          lastmx -= mouseScale;
          lastmy += mouseScale;
          newmousecounters=1;
      }
      else
	  {   
          bot = 1;
          left = 1;
      }
    }
    if (SDL_JoystickGetButton(joy, GP2X_BUTTON_RIGHT))
    {
      if (gp2xMouseEmuOn)
      {
         lastmx += mouseScale;
         newmousecounters=1;
      }
      else
          right = 1;
    }
	if (SDL_JoystickGetButton(joy, GP2X_BUTTON_UPRIGHT))
    {
      if (gp2xMouseEmuOn)
      {
          lastmx += mouseScale;
          lastmy -= mouseScale;
          newmousecounters=1;
      }
      else
      {
	      top = 1;
          right = 1;
      }
    }
	if (SDL_JoystickGetButton(joy, GP2X_BUTTON_DOWNRIGHT))
    {
      if (gp2xMouseEmuOn)
      {
          lastmx += mouseScale;
          lastmy += mouseScale;
          newmousecounters=1;
      }
      else
	  {
          bot = 1;
          right = 1;
      }
    }

    if (SDL_JoystickGetButton(joy, GP2X_BUTTON_UP))
    {
      if (gp2xMouseEmuOn)
      {
          lastmy -= mouseScale;
          newmousecounters=1;
      }
      else
          top = 1;
    }
    if (SDL_JoystickGetButton(joy, GP2X_BUTTON_DOWN))
    {
      if (gp2xMouseEmuOn)
      {
          lastmy += mouseScale;
          newmousecounters=1;
      }
      else
          bot = 1;
    }
    
    *button = SDL_JoystickGetButton(joy, GP2X_BUTTON_B) & 1;
    
    // support second joystick button
    *button |= (SDL_JoystickGetButton (joy, GP2X_BUTTON_X) & 1) << 1;
#else
#ifdef PSP
    struct joy_range *r = nr == 0 ? &range0 : &range1;
    x_axis = SDL_JoystickGetAxis (joy, 0);
    y_axis = SDL_JoystickGetAxis (joy, 1);

    if (x_axis < r->minx) r->minx = x_axis;
    if (y_axis < r->miny) r->miny = y_axis;
    if (x_axis > r->maxx) r->maxx = x_axis;
    if (y_axis > r->maxy) r->maxy = y_axis;
    
    if (x_axis < (r->minx + (r->maxx - r->minx)/3))
    	left = 1;
    else if (x_axis > (r->minx + 2*(r->maxx - r->minx)/3))
    	right = 1;

    if (y_axis < (r->miny + (r->maxy - r->miny)/3))
    	top = 1;
    else if (y_axis > (r->miny + 2*(r->maxy - r->miny)/3))
    	bot = 1;


	int mouseScale = mainMenu_mouseMultiplier;

	if (SDL_JoystickGetButton(joy, 2) && gp2xMouseEmuOn)
	{
		mouseScale = mainMenu_mouseMultiplier * 3;
	}
    

    if (SDL_JoystickGetButton(joy, 7))
    {
      if (gp2xMouseEmuOn)
      {
         lastmx -= mouseScale;
         newmousecounters=1;
      }
      else
         left = 1;
    }

    if (SDL_JoystickGetButton(joy, 9))
    {
      if (gp2xMouseEmuOn)
      {
         lastmx += mouseScale;
         newmousecounters=1;
      }
      else
          right = 1;
    }
    if (SDL_JoystickGetButton(joy, 8))
    {
      if (gp2xMouseEmuOn)
      {
          lastmy -= mouseScale;
          newmousecounters=1;
      }
      else
          top = 1;
    }
    if (SDL_JoystickGetButton(joy, 6))
    {
      if (gp2xMouseEmuOn)
      {
          lastmy += mouseScale;
          newmousecounters=1;
      }
      else
          bot = 1;
    }
    
    *button = SDL_JoystickGetButton(joy, 1) & 1;
    
    // support second joystick button
    *button |= (SDL_JoystickGetButton (joy, 2) & 1) << 1;
#else
#ifdef ANDROID
	x_axis = SDL_JoystickGetAxis (joy, 0);
    y_axis = SDL_JoystickGetAxis (joy, 1);
    
    if (x_axis < 0)
    	left = 1;
    else if (x_axis > 0)
    	right = 1;
    if (y_axis < 0)
    	top = 1;
    else if (y_axis > 0)
    	bot = 1;

    num = SDL_JoystickNumButtons (joy);
    if (num > 16)
	num = 16;
    for (i = 0; i < num; i++)
	*button |= (SDL_JoystickGetButton (joy, i) & 1) << i;
    //if (left) top = !top;
    //if (right) bot = !bot;
    *dir = bot | (right << 1) | (top << 8) | (left << 9);
#else
#ifdef GIZMONDO
    int hat=SDL_JoystickGetHat(joy,0);

	int mouseScale = mainMenu_mouseMultiplier;

	if (SDL_JoystickGetButton(joy, GIZ_PLAY) && gp2xMouseEmuOn)
	{
		mouseScale = mainMenu_mouseMultiplier * 2;
	}
    

    if (hat & SDL_HAT_LEFT)
	{
      if (gp2xMouseEmuOn)
      {
         lastmx -= mouseScale;
         newmousecounters=1;
      }
      else
         left = 1;
    }
	else if (hat & SDL_HAT_RIGHT)
    {
      if (gp2xMouseEmuOn)
      {
         lastmx += mouseScale;
         newmousecounters=1;
      }
      else
          right = 1;
    }

    if (hat & SDL_HAT_UP)
    {
      if (gp2xMouseEmuOn)
      {
          lastmy -= mouseScale;
          newmousecounters=1;
      }
      else
          top = 1;
    }
	else if (hat & SDL_HAT_DOWN)
    {
      if (gp2xMouseEmuOn)
      {
          lastmy += mouseScale;
          newmousecounters=1;
      }
      else
          bot = 1;
    }

	*button = SDL_JoystickGetButton(joy, GIZ_FORWARDS) & 1;
	*button |= (SDL_JoystickGetButton(joy, GIZ_PLAY) & 1) << 1;
#endif
#endif
#endif
#endif
	if (vkbd_mode && nr)
	{
		// move around the virtual keyboard instead
		if (left)
			vkbd_move |= VKBD_LEFT;
		else
		{
			vkbd_move &= ~VKBD_LEFT;
			if (right)
				vkbd_move |= VKBD_RIGHT;
			else
				vkbd_move &= ~VKBD_RIGHT;
		}
		if (top)
			vkbd_move |= VKBD_UP;
		else
		{
			vkbd_move &= ~VKBD_UP;
			if (bot)
				vkbd_move |= VKBD_DOWN;
			else
				vkbd_move &= ~VKBD_DOWN;
		}
		if (*button)
		{
			vkbd_move=VKBD_BUTTON;
			*button=0;
		}
		// TODO: add vkbd_button2 mapped to button2
	}
	else
	{
		// normal joystick movement
	    if (left) top = !top;
		if (right) bot = !bot;
		*dir = bot | (right << 1) | (top << 8) | (left << 9);
	}
#endif
#endif
}
void init_joystick(void)
{
    int i;
    nr_joysticks = SDL_NumJoysticks ();
	if (nr_joysticks > 0)
	uae4all_joy0 = SDL_JoystickOpen (0);
    if (nr_joysticks > 1)
	uae4all_joy1 = SDL_JoystickOpen (1);
    else
	uae4all_joy1 = NULL;
#if !defined (DREAMCAST) && !defined (GP2X) && !defined (GIZMONDO)
    range0.minx = INT_MAX;
    range0.maxx = INT_MIN;
    range0.miny = INT_MAX;
    range0.maxy = INT_MIN;
    range1.minx = INT_MAX;
    range1.maxx = INT_MIN;
    range1.miny = INT_MAX;
    range1.maxy = INT_MIN;
#endif
}
void close_joystick(void)
{
    if (nr_joysticks > 0)
	SDL_JoystickClose (uae4all_joy0);
    if (nr_joysticks > 1)
	SDL_JoystickClose (uae4all_joy1);
}
