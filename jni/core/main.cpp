#ifdef DREAMCAST
#include <kos.h>
extern uint8 romdisk[];
KOS_INIT_FLAGS(INIT_DEFAULT);
KOS_INIT_ROMDISK(romdisk);
#endif

extern "C" int main( int argc, char *argv[] );

/*
  * UAE - The Un*x Amiga Emulator
  *
  * Main program
  *
  * Copyright 1995 Ed Hanway
  * Copyright 1995, 1996, 1997 Bernd Schmidt
  */
#include "sysconfig.h"
#include "sysdeps.h"
#include <assert.h>

#include "config.h"
#include "uae.h"
#include "options.h"
#include "thread.h"
#include "debug_uae4all.h"
#include "gensound.h"
#include "events.h"
#include "memorya.h"
#include "audio.h"
#include "sound.h"
#include "custom.h"
#include "m68k/m68k_intrf.h"
#include "disk.h"
#include "debug.h"
#include "xwin.h"
#include "joystick.h"
#include "keybuf.h"
#include "gui.h"
#include "zfile.h"
#include "autoconf.h"
#include "osemu.h"
#include "exectasks.h"
#include "compiler.h"
#include "bsdsocket.h"
#include "drawing.h"
#include "savestate.h"

#ifdef USE_SDL
#include "SDL.h"
#endif
#ifdef DREAMCAST
#include<SDL_dreamcast.h>
#endif
#ifdef GP2X
#include "gp2xutil.h"
#endif

#include <jni.h>
long int version = 256*65536L*UAEMAJOR + 65536L*UAEMINOR + UAESUBREV;

int no_gui = 0;
int joystickpresent = 0;
int cloanto_rom = 0;

struct gui_info gui_data;

char warning_buffer[256];

char optionsfile[256];

/* If you want to pipe printer output to a file, put something like
 * "cat >>printerfile.tmp" above.
 * The printer support was only tested with the driver "PostScript" on
 * Amiga side, using apsfilter for linux to print ps-data.
 *
 * Under DOS it ought to be -p LPT1: or -p PRN: but you'll need a
 * PostScript printer or ghostscript -=SR=-
 */

/* Slightly stupid place for this... */
/* ncurses.c might use quite a few of those. */
char *colormodes[] = { "256 colors", "32768 colors", "65536 colors",
    "256 colors dithered", "16 colors dithered", "16 million colors",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
};

void discard_prefs ()
{
}

void default_prefs ()
{
/*#ifdef NO_SOUND
    produce_sound = 0;
#else
    produce_sound = 2;
#endif

    prefs_gfx_framerate = -1;

    strcpy (prefs_df[0], "/sdcard/df0.adf");
    strcpy (prefs_df[1], "/sdcard/df1.adf");

#ifdef DREAMCAST
    strcpy (romfile, ROM_PATH_PREFIX "kick.rom");
#else
//    strcpy (romfile, "/cdrom/kick.rom");
    strcpy (romfile, "/sdcard/kick.rom");
#endif
*/
    prefs_chipmem_size=0x00100000;
}

int quit_program = 0;
int pause_program = 0;

#ifndef SDL_JAVA_PACKAGE_PATH
#error You have to define SDL_JAVA_PACKAGE_PATH to your package path with dots replaced with underscores, for example "com_example_SanAngeles"
#endif
#define JAVA_EXPORT_NAME2(name,package) Java_##package##_##name
#define JAVA_EXPORT_NAME1(name,package) JAVA_EXPORT_NAME2(name,package)
#define JAVA_EXPORT_NAME(name) JAVA_EXPORT_NAME1(name,SDL_JAVA_PACKAGE_PATH)

extern "C" void
JAVA_EXPORT_NAME(DemoRenderer_nativePause) ( JNIEnv*  env, jobject  thiz) {
	pause_program = 1;
}

extern "C" void
JAVA_EXPORT_NAME(DemoRenderer_nativeResume) ( JNIEnv*  env, jobject  thiz) {
	pause_program = 0;
}

extern "C" void
JAVA_EXPORT_NAME(DemoActivity_nativeReset) ( JNIEnv*  env, jobject  thiz) {
	uae_reset();
}

extern "C" void
JAVA_EXPORT_NAME(DemoActivity_nativeQuit) ( JNIEnv*  env, jobject  thiz) {
	uae_quit();
	exit(0);
}

int mainMenu_showStatus;
int mainMenu_ntsc;
int right_mouse;

extern "C" void
JAVA_EXPORT_NAME(DemoActivity_setRightMouse) ( JNIEnv*  env, jobject  thiz, jint right) {
	right_mouse = right;
}

extern "C" void
JAVA_EXPORT_NAME(DemoActivity_setPrefs) ( JNIEnv*  env, jobject  thiz, jstring rom, jstring floppy1, jstring floppy2, jstring floppy3, jstring floppy4, jint frameskip, jint m68k, jint times, jint change_sound, jint sound, jint change_disk, jint reset, jint drive_status, jint ntsc ) {
	if (rom)
	{
		const char *srom = (env)->GetStringUTFChars(rom, 0);
		strcpy(romfile, srom);
		(env)->ReleaseStringUTFChars(rom, srom);
	}

	if (change_disk)
	{
		savestate_state = 0;
	}
	if (floppy1)
	{
		const char *sfloppy1 = (env)->GetStringUTFChars(floppy1, 0);
		
		if (change_disk) {
			strcpy(changed_df[0], sfloppy1);
			real_changed_df[0]=1;
			__android_log_print(ANDROID_LOG_INFO, "UAE", "change floppy1: %s", changed_df[0]);
		} else
			strcpy(prefs_df[0], sfloppy1);
		(env)->ReleaseStringUTFChars(floppy1, sfloppy1);
		
	} else
		strcpy (prefs_df[0], "/sdcard/df0.adf");
	if (floppy2)
	{
		const char *sfloppy2 = (env)->GetStringUTFChars(floppy2, 0);
		
		if (change_disk) {
			strcpy(changed_df[1], sfloppy2);
			real_changed_df[1]=1;
		} else
			strcpy(prefs_df[1], sfloppy2);
		(env)->ReleaseStringUTFChars(floppy2, sfloppy2);
		//__android_log_print(ANDROID_LOG_INFO, "UAE", "prefs_df[1]: %s", prefs_df[1]);
	} else
		strcpy (prefs_df[1], "/sdcard/df1.adf");

	if (floppy3)
	{
		const char *sfloppy3 = (env)->GetStringUTFChars(floppy3, 0);
		
		if (change_disk) {
			strcpy(changed_df[2], sfloppy3);
			real_changed_df[2]=1;
		} else
			strcpy(prefs_df[2], sfloppy3);
		(env)->ReleaseStringUTFChars(floppy3, sfloppy3);
		//__android_log_print(ANDROID_LOG_INFO, "UAE", "prefs_df[1]: %s", prefs_df[1]);
	} else
		strcpy (prefs_df[2], "/sdcard/df2.adf");

	if (floppy4)
	{
		const char *sfloppy4 = (env)->GetStringUTFChars(floppy4, 0);
		
		if (change_disk) {
			strcpy(changed_df[3], sfloppy4);
			real_changed_df[3]=1;
		} else
			strcpy(prefs_df[3], sfloppy4);
		(env)->ReleaseStringUTFChars(floppy4, sfloppy4);
		//__android_log_print(ANDROID_LOG_INFO, "UAE", "prefs_df[1]: %s", prefs_df[1]);
	} else
		strcpy (prefs_df[3], "/sdcard/df3.adf");

	mainMenu_showStatus = drive_status;
	mainMenu_ntsc = ntsc;

	if (change_sound)
		changed_produce_sound = sound;
	else {
		produce_sound = sound;
		changed_produce_sound = sound;
	}

	if (frameskip >= 100)	
		prefs_gfx_framerate = -1;
	else
		prefs_gfx_framerate = frameskip;
	m68k_speed = m68k;
	//timeslice_mode = times;
	check_prefs_changed_cpu();
	check_prefs_changed_audio();
	//DISK_init();



	//__android_log_print(ANDROID_LOG_INFO, "UAE", "prefs_df[0]: %s", prefs_df[0]);
	//__android_log_print(ANDROID_LOG_INFO, "UAE", "prefs_df[1]: %s", prefs_df[1]);
	//__android_log_print(ANDROID_LOG_INFO, "UAE", "m68k_speed: %d / timeslice_mode: %d", m68k_speed, timeslice_mode);

	if (reset) {

		uae_reset();
	}
}

extern "C" void
JAVA_EXPORT_NAME(DemoActivity_saveState) ( JNIEnv*  env, jobject  thiz,  jstring filename, jint num) {

	savestate_state = STATE_DOSAVE;
	const char *srom = (env)->GetStringUTFChars(filename, 0);
	strcpy(savestate_filename, srom);

	switch(num)
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

	(env)->ReleaseStringUTFChars(filename, srom);
		
	__android_log_print(ANDROID_LOG_INFO, "UAE", "Saved %s", savestate_filename);

}

extern "C" void
JAVA_EXPORT_NAME(DemoActivity_loadState) ( JNIEnv*  env, jobject  thiz,  jstring filename, jint num) {

	// shagrath : don't ask
	int hackEnableSound = 0;
	if (!produce_sound)
	{
		changed_produce_sound = 1;
		check_prefs_changed_audio();
		hackEnableSound = 1;
	}

	//

	savestate_state = STATE_DORESTORE;
	const char *srom = (env)->GetStringUTFChars(filename, 0);
	strcpy(savestate_filename, srom);

	switch(num)
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

	(env)->ReleaseStringUTFChars(filename, srom);
		
	__android_log_print(ANDROID_LOG_INFO, "UAE", "Loaded %s", savestate_filename);

	if (hackEnableSound)
	{
		changed_produce_sound = 0;
		check_prefs_changed_audio();
	}

}

void uae_reset (void)
{
    gui_purge_events();
/*
    if (quit_program == 0)
	quit_program = -2;
*/  
    //black_screen_now();
    quit_program = 2;
    set_special (SPCFLAG_BRK);
}

void uae_quit (void)
{
    if (quit_program != -1)
	quit_program = -1;
}

void reset_all_systems (void)
{
    init_eventtab ();

    memory_reset ();
}

/* Okay, this stuff looks strange, but it is here to encourage people who
 * port UAE to re-use as much of this code as possible. Functions that you
 * should be using are do_start_program() and do_leave_program(), as well
 * as real_main(). Some OSes don't call main() (which is braindamaged IMHO,
 * but unfortunately very common), so you need to call real_main() from
 * whatever entry point you have. You may want to write your own versions
 * of start_program() and leave_program() if you need to do anything special.
 * Add #ifdefs around these as appropriate.
 */

void do_start_program (void)
{
    /* Do a reset on startup. Whether this is elegant is debatable. */
#if defined(DREAMCAST) && !defined(DEBUG_UAE4ALL)
	while(1)
#endif
	{
		quit_program = 2;
		reset_frameskip();
		m68k_go (1);
	}
}

void do_leave_program (void)
{
    graphics_leave ();
    close_joystick ();
    close_sound ();
    dump_counts ();
    zfile_exit ();
#ifdef USE_SDL
    SDL_Quit ();
#endif
    memory_cleanup ();

__android_log_print(ANDROID_LOG_INFO, "UAE", "do_leave_program");
}

#if defined(DREAMCAST) && !defined(DEBUG_UAE4ALL)
static uint32 uae4all_dc_args[4]={ 0, 0, 0, 0};
static void uae4all_dreamcast_handler(irq_t source, irq_context_t *context)
{
	irq_create_context(context,context->r[15], (uint32)&do_start_program, (uint32 *)&uae4all_dc_args[0],0);
}
#endif

void start_program (void)
{
#if defined(DREAMCAST) && !defined(DEBUG_UAE4ALL)
    irq_set_handler(EXC_USER_BREAK_PRE,&uae4all_dreamcast_handler);
    irq_set_handler(EXC_INSTR_ADDRESS,&uae4all_dreamcast_handler);
    irq_set_handler(EXC_ILLEGAL_INSTR,&uae4all_dreamcast_handler);
    irq_set_handler(EXC_SLOT_ILLEGAL_INSTR,&uae4all_dreamcast_handler);
    irq_set_handler(EXC_GENERAL_FPU,&uae4all_dreamcast_handler);
    irq_set_handler(EXC_SLOT_FPU,&uae4all_dreamcast_handler);
    irq_set_handler(EXC_DATA_ADDRESS_WRITE,&uae4all_dreamcast_handler);
    irq_set_handler(EXC_DTLB_MISS_WRITE,&uae4all_dreamcast_handler);
    irq_set_handler(EXC_OFFSET_000,&uae4all_dreamcast_handler);
    irq_set_handler(EXC_OFFSET_100,&uae4all_dreamcast_handler);
    irq_set_handler(EXC_OFFSET_400,&uae4all_dreamcast_handler);
    irq_set_handler(EXC_OFFSET_600,&uae4all_dreamcast_handler);
    irq_set_handler(EXC_FPU,&uae4all_dreamcast_handler);
    irq_set_handler(EXC_TRAPA,&uae4all_dreamcast_handler);
    irq_set_handler(EXC_RESET_UDI,&uae4all_dreamcast_handler);
    irq_set_handler(EXC_UNHANDLED_EXC,&uae4all_dreamcast_handler);
#endif
    do_start_program ();
}

void leave_program (void)
{
    do_leave_program ();
}

void real_main (int argc, char **argv)
{
#ifdef USE_SDL
//    SDL_Init (SDL_INIT_EVERYTHING | SDL_INIT_NOPARACHUTE);
    SDL_Init (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK 
#if !defined(NO_SOUND) && !defined(GP2X)
 			| SDL_INIT_AUDIO
#endif
	);
#endif

    default_prefs ();
#ifdef GP2X
    gp2x_init(argc, argv);
#endif
    
    if (! graphics_setup ()) {
	exit (1);
    }

    rtarea_init ();

    machdep_init ();

    if (! setup_sound ()) {
	write_log ("Sound driver unavailable: Sound output disabled\n");
	produce_sound = 0;
    }
    init_joystick ();

	int err = gui_init ();
	if (err == -1) {
	    write_log ("Failed to initialize the GUI\n");
	} else if (err == -2) {
	    exit (0);
	}
    if (sound_available && produce_sound > 1 && ! init_audio ()) {
	write_log ("Sound driver unavailable: Sound output disabled\n");
	produce_sound = 0;
    }

    /* Install resident module to get 8MB chipmem, if requested */
    rtarea_setup ();

    keybuf_init (); /* Must come after init_joystick */

    memory_init ();

    custom_init (); /* Must come after memory_init */
    DISK_init ();

    init_m68k();
#ifndef USE_FAME_CORE
    compiler_init ();
#endif
   // gui_update ();

#ifdef GP2X
    switch_to_hw_sdl(1);
#endif
    if (graphics_init ())
	{
		start_program ();
	}
    leave_program ();
}

#ifndef NO_MAIN_IN_MAIN_C
int main (int argc, char *argv[])
{
#ifdef DREAMCAST
#if defined(DEBUG_UAE4ALL) || defined(DEBUG_FRAMERATE) || defined(PROFILER_UAE4ALL) || defined(AUTO_RUN)
	{
		SDL_DC_ShowAskHz(SDL_FALSE);
    		puts("MAIN !!!!");
	}
#endif
#endif
    real_main (argc, argv);
    return 0;
}

void lanada(void)
{
}

#endif
