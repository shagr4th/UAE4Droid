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
#include "memory-uae.h"
#include "custom.h"
#include "joystick.h"
#include "SDL.h"
#include "menu.h"
#include "menu_config.h"

#include "vkbd.h"

#ifdef GP2X
#include "gp2x.h"
#include "xwin.h"
extern int gp2xMouseEmuOn;
extern int gp2xButtonRemappingOn;
#ifdef PANDORA
extern int dpadUp;
extern int dpadDown;
extern int dpadLeft;
extern int dpadRight;
extern int buttonA;
extern int buttonB;
extern int buttonX;
extern int buttonY;
extern int triggerL;
extern int triggerR;
extern int buttonSelect;
extern int buttonStart;
#endif
extern char launchDir[300];
extern int showmsg;
bool switch_autofire=false;
int delay=0;
int delay1=0;
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
extern SDL_Surface *prSDLScreen;

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
    int left = 0, right = 0, top = 0, bot = 0, upRight=0, downRight=0, upLeft=0, downLeft=0, x=0, y=0, a=0, b=0;
    int len, i, num;
    SDL_Joystick *joy = nr == 0 ? uae4all_joy0 : uae4all_joy1;

    *dir = 0;
    *button = 0;

    nr = (~nr)&0x1;

    SDL_JoystickUpdate ();
#if !defined (DREAMCAST) && !defined (GP2X) && !defined (GIZMONDO) && !defined (PSP)
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
	int mouseScale = mainMenu_mouseMultiplier * 4;
	if (mouseScale > 99)
		mouseScale /= 100;

	if (!vkbd_mode && ((mainMenu_customControls && mainMenu_custom_dpad==2) || gp2xMouseEmuOn || (triggerL && !triggerR && !gp2xButtonRemappingOn)))
	{
		if (buttonY)
			mouseScale = mainMenu_mouseMultiplier;
#ifdef PANDORA
		if (dpadLeft)
#else
		if (SDL_JoystickGetButton(joy, GP2X_BUTTON_LEFT))
#endif
		{
			lastmx -= mouseScale;
			newmousecounters=1;
		}
#ifndef PANDORA		
		if (SDL_JoystickGetButton(joy, GP2X_BUTTON_UPLEFT))
		{
			lastmx -= mouseScale;
			lastmy -= mouseScale;
			newmousecounters=1;
		}
		if (SDL_JoystickGetButton(joy, GP2X_BUTTON_DOWNLEFT))
		{
			lastmx -= mouseScale;
			lastmy += mouseScale;
			newmousecounters=1;
		}
#endif
#ifdef PANDORA
		if (dpadRight)
#else
		if (SDL_JoystickGetButton(joy, GP2X_BUTTON_RIGHT))
#endif
		{
			lastmx += mouseScale;
			newmousecounters=1;
		}
#ifndef PANDORA
		if (SDL_JoystickGetButton(joy, GP2X_BUTTON_UPRIGHT))
		{
			lastmx += mouseScale;
			lastmy -= mouseScale;
			newmousecounters=1;
		}
		if (SDL_JoystickGetButton(joy, GP2X_BUTTON_DOWNRIGHT))
		{
			lastmx += mouseScale;
			lastmy += mouseScale;
			newmousecounters=1;
		}
#endif
#ifdef PANDORA
		if (dpadUp)
#else
		if (SDL_JoystickGetButton(joy, GP2X_BUTTON_UP))
#endif
		{    
			lastmy -= mouseScale;
			newmousecounters=1;
		}
#ifdef PANDORA
		if (dpadDown)
#else
		if (SDL_JoystickGetButton(joy, GP2X_BUTTON_DOWN))
#endif
		{
			lastmy += mouseScale;
			newmousecounters=1;
		}
	}
	else if (!triggerR /*R+dpad = arrow keys*/ && !(mainMenu_customControls && !mainMenu_custom_dpad))
	{
#ifdef PANDORA
		if (dpadRight || SDL_JoystickGetAxis(joy, 0) > 0) right=1;
		if (dpadLeft || SDL_JoystickGetAxis(joy, 0) < 0) left=1;
		if (dpadUp || SDL_JoystickGetAxis(joy, 1) < 0) top=1;
		if (dpadDown || SDL_JoystickGetAxis(joy, 1) > 0) bot=1;
#else
		if (SDL_JoystickGetButton(joy, GP2X_BUTTON_LEFT))left = 1;
		if (SDL_JoystickGetButton(joy, GP2X_BUTTON_RIGHT)) right = 1;
		if (SDL_JoystickGetButton(joy, GP2X_BUTTON_UP))top = 1;
		if (SDL_JoystickGetButton(joy, GP2X_BUTTON_DOWN))bot = 1;
#endif
#ifndef PANDORA
		if (SDL_JoystickGetButton(joy, GP2X_BUTTON_DOWNLEFT))
		{
			bot = 1;
			left = 1;
		}
		if (SDL_JoystickGetButton(joy, GP2X_BUTTON_DOWNRIGHT))
		{
			bot = 1;
			right = 1;
		}
#endif
		if (!mainMenu_joyConf)
		{
#ifndef PANDORA
			if (SDL_JoystickGetButton(joy, GP2X_BUTTON_UPLEFT))
			{
				top = 1;
				left = 1;
			}
			if (SDL_JoystickGetButton(joy, GP2X_BUTTON_UPRIGHT))
			{
				top = 1;
				right = 1;
			}
#endif
		}
		else
		{
			if (((buttonX && mainMenu_jump > -1) || SDL_JoystickGetButton(joy, mainMenu_jump)) && !vkbd_mode)
				top = 1;
		}
	}

	if(mainMenu_customControls && !vkbd_mode)
	{
		if((mainMenu_custom_A==-5 && buttonA) || (mainMenu_custom_B==-5 && buttonB) || (mainMenu_custom_X==-5 && buttonX) || (mainMenu_custom_Y==-5 && buttonY) || (mainMenu_custom_L==-5 && triggerL) || (mainMenu_custom_R==-5 && triggerR))
			top = 1;
		else if(mainMenu_custom_dpad == 0)
		{
			if((mainMenu_custom_up==-5 && dpadUp) || (mainMenu_custom_down==-5 && dpadDown) || (mainMenu_custom_left==-5 && dpadLeft) || (mainMenu_custom_right==-5 && dpadRight))
				top = 1;
		}
	}

if(!gp2xMouseEmuOn && !gp2xButtonRemappingOn)
{
	if(!mainMenu_customControls && ((mainMenu_autofire & switch_autofire & delay>mainMenu_autofireRate) || (((mainMenu_autofireButton1==GP2X_BUTTON_B && buttonA) || (mainMenu_autofireButton1==GP2X_BUTTON_X && buttonX) || (mainMenu_autofireButton1==GP2X_BUTTON_Y && buttonY)) & delay>mainMenu_autofireRate)))
	{
		if(!buttonB)
			*button=1;
		delay=0;
		*button |= (buttonB & 1) << 1;
	}
	else if(mainMenu_customControls && !vkbd_mode)
	{
		if((mainMenu_custom_A==-3 && buttonA) || (mainMenu_custom_B==-3 && buttonB) || (mainMenu_custom_X==-3 && buttonX) || (mainMenu_custom_Y==-3 && buttonY) || (mainMenu_custom_L==-3 && triggerL) || (mainMenu_custom_R==-3 && triggerR))
			*button = 1;
		else if(mainMenu_custom_dpad == 0)
		{
			if((mainMenu_custom_up==-3 && dpadUp) || (mainMenu_custom_down==-3 && dpadDown) || (mainMenu_custom_left==-3 && dpadLeft) || (mainMenu_custom_right==-3 && dpadRight))
				*button = 1;
		}

		if((mainMenu_custom_A==-4 && buttonA) || (mainMenu_custom_B==-4 && buttonB) || (mainMenu_custom_X==-4 && buttonX) || (mainMenu_custom_Y==-4 && buttonY) || (mainMenu_custom_L==-4 && triggerL) || (mainMenu_custom_R==-4 && triggerR))
			*button |= 1 << 1;
		else if(mainMenu_custom_dpad == 0)
		{
			if((mainMenu_custom_up==-4 && dpadUp) || (mainMenu_custom_down==-4 && dpadDown) || (mainMenu_custom_left==-4 && dpadLeft) || (mainMenu_custom_right==-4 && dpadRight))
				*button |= 1 << 1;
		}
		delay++;
	}
	else
	{
		*button = ((mainMenu_button1==GP2X_BUTTON_B && buttonA) || (mainMenu_button1==GP2X_BUTTON_X && buttonX) || (mainMenu_button1==GP2X_BUTTON_Y && buttonY) || SDL_JoystickGetButton(joy, mainMenu_button1)) & 1;
		delay++;
		*button |= ((buttonB || SDL_JoystickGetButton(joy, mainMenu_button2)) & 1) << 1;
	}
}
	//if (delay1) {delay1++;if (delay1>100) delay1=0;}

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
		mouseScale = mainMenu_mouseMultiplier * 3;

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
