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

#ifndef _SDL_androidvideo_h
#define _SDL_androidvideo_h

#include "../SDL_sysvideo.h"

/* Hidden "this" pointer for the video functions */
#define _THIS	SDL_VideoDevice *this

extern void ANDROID_InitOSKeymap(_THIS);
extern void ANDROID_PumpEvents(_THIS);

extern int pop_circular_queue();
extern int android_numJoysticks;

// Keycodes ripped from Java SDK
enum KEYCODES_ANDROID 
{
KEYCODE_UNKNOWN         = 0,
KEYCODE_SOFT_LEFT       = 1,
KEYCODE_SOFT_RIGHT      = 2,
KEYCODE_HOME            = 3,
KEYCODE_BACK            = 4,
KEYCODE_CALL            = 5,
KEYCODE_ENDCALL         = 6,
KEYCODE_0               = 7,
KEYCODE_1               = 8,
KEYCODE_2               = 9,
KEYCODE_3               = 10,
KEYCODE_4               = 11,
KEYCODE_5               = 12,
KEYCODE_6               = 13,
KEYCODE_7               = 14,
KEYCODE_8               = 15,
KEYCODE_9               = 16,
KEYCODE_STAR            = 17,
KEYCODE_POUND           = 18,
KEYCODE_DPAD_UP         = 19,
KEYCODE_DPAD_DOWN       = 20,
KEYCODE_DPAD_LEFT       = 21,
KEYCODE_DPAD_RIGHT      = 22,
KEYCODE_DPAD_CENTER     = 23,
KEYCODE_VOLUME_UP       = 24,
KEYCODE_VOLUME_DOWN     = 25,
KEYCODE_POWER           = 26,
KEYCODE_CAMERA          = 27,
KEYCODE_CLEAR           = 28,
KEYCODE_A               = 29,
KEYCODE_B               = 30,
KEYCODE_C               = 31,
KEYCODE_D               = 32,
KEYCODE_E               = 33,
KEYCODE_F               = 34,
KEYCODE_G               = 35,
KEYCODE_H               = 36,
KEYCODE_I               = 37,
KEYCODE_J               = 38,
KEYCODE_K               = 39,
KEYCODE_L               = 40,
KEYCODE_M               = 41,
KEYCODE_N               = 42,
KEYCODE_O               = 43,
KEYCODE_P               = 44,
KEYCODE_Q               = 45,
KEYCODE_R               = 46,
KEYCODE_S               = 47,
KEYCODE_T               = 48,
KEYCODE_U               = 49,
KEYCODE_V               = 50,
KEYCODE_W               = 51,
KEYCODE_X               = 52,
KEYCODE_Y               = 53,
KEYCODE_Z               = 54,
KEYCODE_COMMA           = 55,
KEYCODE_PERIOD          = 56,
KEYCODE_ALT_LEFT        = 57,
KEYCODE_ALT_RIGHT       = 58,
KEYCODE_SHIFT_LEFT      = 59,
KEYCODE_SHIFT_RIGHT     = 60,
KEYCODE_TAB             = 61,
KEYCODE_SPACE           = 62,
KEYCODE_SYM             = 63,
KEYCODE_EXPLORER        = 64,
KEYCODE_ENVELOPE        = 65,
KEYCODE_ENTER           = 66,
KEYCODE_DEL             = 67,
KEYCODE_GRAVE           = 68,
KEYCODE_MINUS           = 69,
KEYCODE_EQUALS          = 70,
KEYCODE_LEFT_BRACKET    = 71,
KEYCODE_RIGHT_BRACKET   = 72,
KEYCODE_BACKSLASH       = 73,
KEYCODE_SEMICOLON       = 74,
KEYCODE_APOSTROPHE      = 75,
KEYCODE_SLASH           = 76,
KEYCODE_AT              = 77,
KEYCODE_NUM             = 78,
KEYCODE_HEADSETHOOK     = 79,
KEYCODE_FOCUS           = 80,
KEYCODE_PLUS            = 81,
KEYCODE_MENU            = 82,
KEYCODE_NOTIFICATION    = 83,
KEYCODE_SEARCH          = 84,
KEYCODE_MEDIA_PLAY_PAUSE= 85,
KEYCODE_MEDIA_STOP      = 86,
KEYCODE_MEDIA_NEXT      = 87,
KEYCODE_MEDIA_PREVIOUS  = 88,
KEYCODE_MEDIA_REWIND    = 89,
KEYCODE_MEDIA_FAST_FORWARD = 90,
KEYCODE_MUTE            = 91,
KEYCODE_ESCAPE			= 92,
KEYCODE_F1				= 93,
KEYCODE_F2				= 94,
KEYCODE_F3				= 95,
KEYCODE_F4				= 96,
KEYCODE_F5				= 97,
KEYCODE_F6				= 98,
KEYCODE_F7				= 99,
KEYCODE_F8				= 100,


KEYCODE_LAST            = KEYCODE_F8
};

#endif /* _SDL_androidvideo_h */
