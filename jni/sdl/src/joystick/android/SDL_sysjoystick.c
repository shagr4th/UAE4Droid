/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2009 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

/* This is the system specific header for the SDL joystick API */

#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_joystick.h"
#include "../SDL_sysjoystick.h"
#include "../SDL_joystick_c.h"
#include <android/log.h>
#include "../../video/android/SDL_androidvideo.h"

/* Function to scan the system for joysticks.
 * This function should set SDL_numjoysticks to the number of available
 * joysticks.  Joystick 0 should be the system default joystick.
 * It should return 0, or -1 on an unrecoverable fatal error.
 */
int SDL_SYS_JoystickInit(void)
{
	SDL_numjoysticks = 2;
    //keysInit();
	__android_log_print(ANDROID_LOG_INFO, "libSDL", "init2");
	return SDL_numjoysticks;
}

/* Function to get the device-dependent name of a joystick */
const char *SDL_SYS_JoystickName(int index)
{
	if(!index)
		return "NDS builtin joypad";
	SDL_SetError("No joystick available with that index");
	return (NULL);
}

/* Function to open a joystick for use.
   The joystick to open is specified by the index field of the joystick.
   This should fill the nbuttons and naxes fields of the joystick structure.
   It returns 0, or -1 if there is an error.
 */
int SDL_SYS_JoystickOpen(SDL_Joystick *joystick)
{
	__android_log_print(ANDROID_LOG_INFO, "libSDL", "open joystick android");
    joystick->nbuttons=1;
	joystick->nhats=0;
	joystick->nballs=0;
	joystick->naxes=2;
	return 0;
}


/* Function to update the state of a joystick - called as a device poll.
 * This function shouldn't update the joystick structure directly,
 * but instead should call SDL_PrivateJoystick*() to deliver events
 * and update joystick device state.
 */

int prevbutton=0;
int prevkey=0;

int dc=0;int ldc=0;

void SDL_SYS_JoystickUpdate(SDL_Joystick *joystick)
{
    //dc=keysd;
	//if (dc)
	//{
		//fprintf(stderr,"heartbeat= %d\n",REG_VCOUNT); 
		//swiWaitForVBlank();
		//scanKeys();
		//keysd = keysDown(); 
		//keysu = keysUp();
		//ldc=keysd;
		 
	//}
	/*if (prevkey && prevbutton)
	{
		scanKeys();
	}
	*/
		
	//scanKeys();
	int pop = -1;


		if (joystick->index == 0)
			pop = pop_circular_queue_for_joystick1();
		else if (joystick->index == 1)
			pop = pop_circular_queue_for_joystick2();


	if (pop != -1)
	{
	
		unsigned int keysd = 0; 
		unsigned int keysu = 0;
		
		if (pop >= 0)
		{
			keysd = pop;
		} else if (pop < -1)
		{
			keysu = -pop-2;
		}

		//__android_log_print(ANDROID_LOG_INFO, "libSDL", "keysd: %d / keysu: %d", keysd, keysu);
	
	short ax=0,v=0,h=0;
	if((keysd==46 ||keysd==33 ||keysd==48)) {ax=1;v=-10;SDL_PrivateJoystickAxis(joystick,ax,v);}//fprintf(stderr,"KEY_UP\n");}
	if((keysd==31 ||keysd==50 ||keysd==52)) {ax=1;v=10;SDL_PrivateJoystickAxis(joystick,ax,v);}//fprintf(stderr,"KEY_DOWN\n");}
	if((keysd==32 ||keysd==33 ||keysd==50)) {ax=0;h=-10;SDL_PrivateJoystickAxis(joystick,ax,h);}//fprintf(stderr,"KEY_LEFT\n");}
	if((keysd==34 ||keysd==48 ||keysd==52)) {ax=0;h=10;SDL_PrivateJoystickAxis(joystick,ax,h);}//fprintf(stderr,"KEY_RIGHT\n");}

	if((keysu==46 ||keysu==33 ||keysu==48)) {ax=1;v=0;SDL_PrivateJoystickAxis(joystick,ax,v);}//fprintf(stderr,"KEY_UP\n");}
	if((keysu==31 ||keysu==50 ||keysu==52)) {ax=1;v=0;SDL_PrivateJoystickAxis(joystick,ax,v);}//fprintf(stderr,"KEY_DOWN\n");}
	if((keysu==32 ||keysu==33 ||keysu==50)) {ax=0;h=0;SDL_PrivateJoystickAxis(joystick,ax,h);}//fprintf(stderr,"KEY_LEFT\n");}
	if((keysu==34 ||keysu==48 ||keysu==52)) {ax=0;h=0;SDL_PrivateJoystickAxis(joystick,ax,h);}//fprintf(stderr,"KEY_RIGHT\n");}

	if((keysd==44))		{SDL_PrivateJoystickButton(joystick,0,SDL_PRESSED);prevbutton=44;}
	
	if((keysu==44))		{SDL_PrivateJoystickButton(joystick,0,SDL_RELEASED);prevbutton=0;}
	
	}

}

/* Function to close a joystick after use */
void SDL_SYS_JoystickClose(SDL_Joystick *joystick)
{
}

/* Function to perform any system-specific joystick related cleanup */
void SDL_SYS_JoystickQuit(void)
{
}

