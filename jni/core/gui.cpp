 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Interface to the Tcl/Tk GUI
  *
  * Copyright 1996 Bernd Schmidt
  */

#include "sysconfig.h"
#include "sysdeps.h"
#include "config.h"
#include "uae.h"
#include "options.h"
#include "gui.h"
#ifndef ANDROID
#include "menu.h"
#endif
#include "vkbd.h"
#include "debug_uae4all.h"
#include "custom.h"
#include "memorya.h"
#include "xwin.h"
#include "drawing.h"
#include "sound.h"
#include "audio.h"
#include "keybuf.h"
#include "disk.h"

#include <SDL.h>

#ifdef PROFILER_UAE4ALL
unsigned long long uae4all_prof_initial[UAE4ALL_PROFILER_MAX];
unsigned long long uae4all_prof_sum[UAE4ALL_PROFILER_MAX];
unsigned long long uae4all_prof_executed[UAE4ALL_PROFILER_MAX];
#endif

#ifdef DREAMCAST
#include <SDL_dreamcast.h>
#define VIDEO_FLAGS_INIT SDL_HWSURFACE|SDL_FULLSCREEN
#else
#ifdef PSP
#define VIDEO_FLAGS_INIT SDL_SWSURFACE
#else
#ifdef GIZMONDO
#define VIDEO_FLAGS_INIT SDL_HWSURFACE|SDL_FULLSCREEN
#else
#ifdef GP2X
#define VIDEO_FLAGS_INIT SDL_SWSURFACE|SDL_FULLSCREEN
#else
#define VIDEO_FLAGS_INIT SDL_HWSURFACE
#endif
#endif
#endif
#endif

#ifdef GP2X
#include "gp2x.h"
#include "gp2xutil.h"
extern int gp2xMouseEmuOn, gp2xButtonRemappingOn;
#endif

#ifdef PSP
#include "psp.h"
#include "psputil.h"
extern int gp2xMouseEmuOn, gp2xButtonRemappingOn;
#endif

#ifdef GIZMONDO
#include "gizmondo.h"
#include "gizmondoutil.h"
extern int gp2xMouseEmuOn, gp2xButtonRemappingOn;
#endif

#ifdef DOUBLEBUFFER
#define VIDEO_FLAGS VIDEO_FLAGS_INIT | SDL_DOUBLEBUF
#else
#define VIDEO_FLAGS VIDEO_FLAGS_INIT
#endif


extern SDL_Surface *prSDLScreen;
extern struct uae_prefs changed_prefs;
extern struct uae_prefs currprefs;
extern SDL_Joystick *uae4all_joy0, *uae4all_joy1;

extern int keycode2amiga(SDL_keysym *prKeySym);
extern int uae4all_keystate[];

int emulating=0;
char uae4all_image_file0[128]  = { 0, };
char uae4all_image_file1[128] = { 0, };
char uae4all_image_file2[128]  = { 0, };
char uae4all_image_file3[128] = { 0, };

int drawfinished=0;

extern int mainMenu_throttle, mainMenu_frameskip, mainMenu_sound, mainMenu_case, mainMenu_autosave, saveMenu_n_savestate;
int mainMenu_throttle, mainMenu_frameskip, mainMenu_sound, mainMenu_case, mainMenu_autosave;
int mainMenu_drives = 4;
int saveMenu_n_savestate = 0;

static void getChanges(void)
{
    m68k_speed=mainMenu_throttle;
#ifndef NO_SOUND
    if (mainMenu_sound)
    {
#if defined (GP2X) || defined (PSP) || defined (GIZMONDO)
		if (mainMenu_sound == 1)
		{
			changed_produce_sound=2;
		}
		else
		{
			// fake it
			changed_produce_sound=1;
		}
	    sound_default_evtime();
#else
	    changed_produce_sound=2;
	    sound_default_evtime();
#endif
    }
    else
#endif
	    changed_produce_sound=0;
    changed_gfx_framerate=mainMenu_frameskip;
}

int gui_init (void)
{
//Se ejecuta justo despues del MAIN

	mainMenu_autosave = 1;

    if (prSDLScreen==NULL)
#ifdef DREAMCAST
	prSDLScreen=SDL_SetVideoMode(320,240,16,VIDEO_FLAGS);
#else
	prSDLScreen=SDL_SetVideoMode(320,240,16,VIDEO_FLAGS);
#endif
    SDL_ShowCursor(SDL_DISABLE);
    SDL_JoystickEventState(SDL_ENABLE);
    SDL_JoystickOpen(0);
    if (prSDLScreen!=NULL)
    {
	emulating=0;
#if !defined(DEBUG_UAE4ALL) && !defined(PROFILER_UAE4ALL) && !defined(AUTO_RUN) && !defined(AUTO_FRAMERATE)
	//uae4all_image_file[0]=0;
	//uae4all_image_file2[0]=0;
#else
	//strcpy(uae4all_image_file,"prueba.adz");
	//strcpy(uae4all_image_file2,"prueba2.adz");
#endif
#ifndef ANDROID
	init_text(1);
	if (!uae4all_image_file[0])
		run_mainMenu();
	quit_text();
	vkbd_init();
#endif
#ifdef ANDROID
	uae4all_init_sound();
#endif
	 //strcpy (uae4all_image_file, "/sdcard/df0.adf");
    //strcpy (uae4all_image_file2, "/sdcard/df1.adf");

#ifdef GP2X
	inputmode_init();
	volumecontrol_init();
#endif

#if defined (PSP) || defined (GIZMONDO)
	inputmode_init();
#endif

	uae4all_pause_music();
	emulating=1;
	//getChanges();
	check_all_prefs();
	reset_frameskip();
#ifdef DEBUG_FRAMERATE
	uae4all_update_time();
#endif
#ifdef PROFILER_UAE4ALL
	uae4all_prof_init();
	uae4all_prof_add("M68K");			// 0
	uae4all_prof_add("EVENTS");			// 1
	uae4all_prof_add("HSync");			// 2
	uae4all_prof_add("Copper");			// 3
	uae4all_prof_add("Audio");			// 4
	uae4all_prof_add("CIA");			// 5
	uae4all_prof_add("Blitter");			// 6
	uae4all_prof_add("Vsync");			// 7
	uae4all_prof_add("update_fetch");		// 8
	uae4all_prof_add("linetoscr");			// 9
	uae4all_prof_add("do_long_fetch");		// 10
	uae4all_prof_add("pfield_doline");		// 11
	uae4all_prof_add("draw_sprites_ecs");		// 12
	uae4all_prof_add("flush_block");		// 13
	uae4all_prof_add("SET_INTERRUPT");		// 14
/*
	uae4all_prof_add("15");		// 15
	uae4all_prof_add("16");		// 16
	uae4all_prof_add("17");		// 17
	uae4all_prof_add("18");		// 18
	uae4all_prof_add("19");		// 19
	uae4all_prof_add("20");		// 20
	uae4all_prof_add("21");		// 21
	uae4all_prof_add("22");		// 22
*/
#endif
#ifdef DREAMCAST
	SDL_DC_EmulateKeyboard(SDL_FALSE);
#endif
	return 0;
    }
    return -1;
}

int gui_update (void)
{
	extern char *savestate_filename;
    extern int saveMenu_n_savestate;
// SE EJECUTA DESPUES DE INICIAR EL CORE 68k
    strcpy(changed_df[0],uae4all_image_file0);
    strcpy(changed_df[1],uae4all_image_file1);
    strcpy(changed_df[2],uae4all_image_file2);
    strcpy(changed_df[3],uae4all_image_file3);
    strcpy(savestate_filename,uae4all_image_file0);
	switch(saveMenu_n_savestate)
    {
	    case 1:
    		strcat(savestate_filename,"-1.asf");
	    case 2:
    		strcat(savestate_filename,"-2.asf");
	    case 3:
    		strcat(savestate_filename,"-3.asf");
	    default: 
    	   	strcat(savestate_filename,".asf");
    }
    real_changed_df[0]=1;
    real_changed_df[1]=1;
	real_changed_df[2]=1;
	real_changed_df[3]=1;
    return 0;
}


static void goMenu(void)
{
   int exitmode=0;
   int autosave=mainMenu_autosave;
   if (quit_program != 0)
	    return;
#ifdef PROFILER_UAE4ALL
   uae4all_prof_show();
#endif
#ifdef DEBUG_FRAMERATE
   uae4all_show_time();
#endif
   emulating=1;
#ifndef ANDROID
   vkbd_quit();
   init_text(0);
   pause_sound();

   menu_raise();
   exitmode=run_mainMenu();
   notice_screen_contents_lost();
   resume_sound();
   if ((!(strcmp(prefs_df[0],uae4all_image_file))) || ((!(strcmp(prefs_df[1],uae4all_image_file2)))))
	   menu_unraise();
   quit_text();
   vkbd_init();
#endif
#ifdef DREAMCAST
   SDL_DC_EmulateKeyboard(SDL_FALSE);
#endif
    getChanges();
    vkbd_init_button2();
	if (exitmode==1 || exitmode==2)
    {
            extern char *savestate_filename;
    	    extern int saveMenu_n_savestate;
	    for(int i=0;i<mainMenu_drives;i++)
	    {
		if (i==0 && strcmp(changed_df[0],uae4all_image_file0)) {
		    strcpy(changed_df[0],uae4all_image_file0);
		    real_changed_df[0]=1;
		}
		else if (i==1 && strcmp(changed_df[1],uae4all_image_file1)) {
		    strcpy(changed_df[1],uae4all_image_file1);
		    real_changed_df[1]=1;
		}
		else if (i==2 && strcmp(changed_df[2],uae4all_image_file2)) {
		    strcpy(changed_df[2],uae4all_image_file2);
		    real_changed_df[2]=1;
		}
		else if (i==3 && strcmp(changed_df[3],uae4all_image_file3)) {
		    strcpy(changed_df[3],uae4all_image_file3);
		    real_changed_df[3]=1;
		}
	    }
    	    strcpy(savestate_filename,uae4all_image_file0);
	    switch(saveMenu_n_savestate)
    	    {
	    	case 1:
    			strcat(savestate_filename,"-1.asf");
	    	case 2:
    			strcat(savestate_filename,"-2.asf");
	    	case 3:
    			strcat(savestate_filename,"-3.asf");
	    	default: 
    	   		strcat(savestate_filename,".asf");
    	    }
    }
    if (exitmode==3)
    {
	     extern char *savestate_filename;
    	    extern int saveMenu_n_savestate;
	    for(int i=0;i<mainMenu_drives;i++)
	    {
		changed_df[i][0]=0;
		if (i==0) {
		    uae4all_image_file0[0]=0;
		    if (strcmp(changed_df[0],uae4all_image_file0))
		    { 
			strcpy(changed_df[0],uae4all_image_file0);
			real_changed_df[0]=1;
		    }
		}
		else if (i==1) {
		    uae4all_image_file1[0]=0;
		    if (strcmp(changed_df[1],uae4all_image_file1))
		    { 
			strcpy(changed_df[1],uae4all_image_file1);
			real_changed_df[1]=1;
		    }
		}
		else if (i==2) {
		    uae4all_image_file2[0]=0;
		    if (strcmp(changed_df[2],uae4all_image_file2))
		    { 
			strcpy(changed_df[2],uae4all_image_file2);
			real_changed_df[2]=1;
		    }
		}
		else if (i==3) {
		    uae4all_image_file3[0]=0;
		    if (strcmp(changed_df[3],uae4all_image_file3))
		    { 
			strcpy(changed_df[3],uae4all_image_file3);
			real_changed_df[3]=1;
		    }
		}
		disk_eject(i);
	    }
    	    strcpy(savestate_filename,uae4all_image_file0);
    	    switch(saveMenu_n_savestate)
    	    {
	   	case 1:
    			strcat(savestate_filename,"-1.asf");
	    	case 2:
    			strcat(savestate_filename,"-2.asf");
	    	case 3:
    			strcat(savestate_filename,"-3.asf");
	    	default: 
    	  	 	strcat(savestate_filename,".asf");
    	    }
    }
    if (exitmode==2)
    {
	    if (autosave!=mainMenu_autosave)
	    {
	    	prefs_df[0][0]=0;
	   	prefs_df[1][0]=0;
	    	prefs_df[2][0]=0;
	   	prefs_df[3][0]=0;
	    }
	    black_screen_now();
	    uae_reset ();
    }
    check_all_prefs();
    gui_purge_events();
    notice_screen_contents_lost();
#ifdef DEBUG_FRAMERATE
    uae4all_update_time();
#endif
#ifdef PROFILER_UAE4ALL
    uae4all_prof_init();
#endif
}

int nowSuperThrottle=0, goingSuperThrottle=0, goingVkbd=0;

static void goSuperThrottle(void)
{
	if (!nowSuperThrottle)
	{
		nowSuperThrottle=1;
		m68k_speed=6;
		changed_produce_sound=0;
		changed_gfx_framerate=80;
		check_prefs_changed_cpu();
		check_prefs_changed_audio();
		check_prefs_changed_custom();
	}
}

static void leftSuperThrottle(void)
{
	if (nowSuperThrottle)
	{
		nowSuperThrottle=0;
		getChanges();
		check_prefs_changed_cpu();
		check_prefs_changed_audio();
		check_prefs_changed_custom();
	}
}

void gui_handle_events (void)
{
#if ! defined (DREAMCAST) && ! defined (GP2X) && ! defined (PSP)  && ! defined (GIZMONDO)
	Uint8 *keystate = SDL_GetKeyState(NULL);
	if ( keystate[SDLK_F12] )
		SDL_WM_ToggleFullScreen(prSDLScreen);
	else
	if ( keystate[SDLK_F11] )
#else
#ifdef DREAMCAST
	if (SDL_JoystickGetButton(uae4all_joy0,3) )
#endif
#ifdef GP2X
    if (SDL_JoystickGetButton(uae4all_joy0, GP2X_BUTTON_SELECT))
#endif
#ifdef PSP
	if (SDL_JoystickGetButton(uae4all_joy0, PSP_BUTTON_SELECT))
#endif
#ifdef GIZMONDO
	Uint8 *keystate = SDL_GetKeyState(NULL);
	if ( keystate[SDLK_F1] )
#endif
#endif
#ifdef GP2X
	{
		// hack: always use SDL_SWSURFACE in menus
		switch_to_sw_sdl();
		goMenu();
		switch_to_hw_sdl(0);
	}
#else
		goMenu();
#endif

#if defined (DREAMCAST) || defined (GP2X) || defined (PSP) || defined (GIZMONDO)
#ifdef DREAMCAST
	if (SDL_JoystickGetAxis (uae4all_joy0, 2))
		if (vkbd_mode)
		{
			vkbd_mode=0;
   			notice_screen_contents_lost();
		}
	if (SDL_JoystickGetAxis (uae4all_joy0, 3))
		vkbd_mode=1;
#else
	
	if (!gp2xMouseEmuOn && !gp2xButtonRemappingOn)
	{
#ifdef GP2X
		if (SDL_JoystickGetButton (uae4all_joy0, GP2X_BUTTON_L))
#else
#ifdef GIZMONDO
		if (SDL_JoystickGetButton (uae4all_joy0, GIZ_LTRIG))
#else
		if (SDL_JoystickGetButton (uae4all_joy0, PSP_BUTTON_L))
#endif
#endif
		{
			if (vkbd_mode)
			{
				vkbd_mode = 0;
				goingVkbd=0;
				notice_screen_contents_lost();
			}
			else
				goingSuperThrottle=1;
		}
		else
		{
			if (!nowSuperThrottle)
				goingSuperThrottle=0;
			else
				goingVkbd=0;
		}

#ifdef GP2X
		if (SDL_JoystickGetButton (uae4all_joy0, GP2X_BUTTON_R))
#else
#ifdef GIZMONDO
		if (SDL_JoystickGetButton (uae4all_joy0, GIZ_RTRIG))
#else
		if (SDL_JoystickGetButton (uae4all_joy0, PSP_BUTTON_R))
#endif
#endif
		{
			if (goingSuperThrottle)
				goSuperThrottle();
			else
			{
				if (goingVkbd>4)
					vkbd_mode=1;
				else
					goingVkbd++;
			}
		}
		else
		{
			if (nowSuperThrottle)
				leftSuperThrottle();
			else
				goingVkbd=0;
		}
	}
	
#endif

	if (vkbd_key)
	{
		if (vkbd_keysave==-1234567)
		{
			SDL_keysym ks;
			ks.sym=vkbd_key;
			vkbd_keysave=keycode2amiga(&ks);
			if (vkbd_keysave >= 0)
			{
				if (!uae4all_keystate[vkbd_keysave])
				{
					uae4all_keystate[vkbd_keysave]=1;
					record_key(vkbd_keysave<<1);
				}
			}
		}
	}
	else
		if (vkbd_keysave!=-1234567)
		{
			if (vkbd_keysave >= 0)
			{
				uae4all_keystate[vkbd_keysave]=0;
				record_key((vkbd_keysave << 1) | 1);
			}
			vkbd_keysave=-1234567;
		}
#endif
}

void gui_set_message(char *msg, int t) {

}

void gui_show_window_bar (int per, int max, int case_title) {

}

void gui_changesettings (void)
{
	dbg("GUI: gui_changesettings");
}

void gui_update_gfx (void)
{
// ANTES DE LA ENTRADA EN VIDEO
//	dbg("GUI: gui_update_gfx");
}

/*
int run_menuGame() {}
int run_menuControl() {}
*/


#ifdef PROFILER_UAE4ALL

static unsigned uae4all_prof_total_initial=0;
unsigned uae4all_prof_total=0;
static char *uae4all_prof_msg[UAE4ALL_PROFILER_MAX];

void uae4all_prof_init(void)
{
	unsigned i;
#ifndef DREAMCAST
	unsigned long long s=SDL_GetTicks();
#else
	unsigned long long s=timer_us_gettime64();
#endif
	for(i=0;i<UAE4ALL_PROFILER_MAX;i++)
	{
		uae4all_prof_initial[i]=s;
		uae4all_prof_sum[i]=0;
		uae4all_prof_executed[i]=0;
		if (!uae4all_prof_total)
			uae4all_prof_msg[i]=NULL;
	}
	uae4all_prof_total_initial=s;
}

void uae4all_prof_add(char *msg)
{
	if (uae4all_prof_total<UAE4ALL_PROFILER_MAX)
	{
		uae4all_prof_msg[uae4all_prof_total]=msg;	
		uae4all_prof_total++;
	}
}

void uae4all_prof_show(void)
{
	unsigned i;
	double toper=0;
#ifndef DREAMCAST
	unsigned long long to=SDL_GetTicks()-uae4all_prof_total_initial;
#else
	unsigned long long to=uae4all_prof_sum[0]+uae4all_prof_sum[1];
	for(i=0;i<uae4all_prof_total;i++)
		if (uae4all_prof_sum[i]>to)
			uae4all_prof_sum[i]=0;
#endif

	puts("\n\n\n\n");
	puts("--------------------------------------------");
	for(i=0;i<uae4all_prof_total;i++)
	{
		unsigned long long t0=uae4all_prof_sum[i];
		double percent=(double)t0;
		percent*=100.0;
		percent/=(double)to;
		toper+=percent;
#ifdef DREAMCAST
		t0/=1000;
#endif
		printf("%s: %.2f%% -> Ticks=%i -> %iK veces\n",uae4all_prof_msg[i],percent,((unsigned)t0),(unsigned)(uae4all_prof_executed[i]>>10));
	}
	printf("TOTAL: %.2f%% -> Ticks=%i\n",toper,to);
	puts("--------------------------------------------"); fflush(stdout);
}
#endif
