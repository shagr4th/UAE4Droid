int kickstart=1;
int oldkickstart=-1;	/* reload KS at startup */

extern char launchDir[300];

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
#include "vkbd.h"
#include "config.h"
#include "uae.h"
#include "options.h"
#include "thread.h"
#include "debug_uae4all.h"
#include "gensound.h"
#include "events.h"
#include "memory-uae.h"
#include "audio.h"
#include "memory-uae.h"
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
#include "menu.h" 
#include "gp2xutil.h"
#include "savestate.h"
#include "menu_config.h"
#ifdef __WINS__
#include "target.h"
#endif
/* PocketUAE */
#include "native2amiga.h"

#ifdef USE_SDL
#include "SDL.h"
#endif
#ifdef DREAMCAST
#include<SDL_dreamcast.h>
#endif
#ifdef GP2X
#include "gp2xutil.h"
#endif
long int version = 256*65536L*UAEMAJOR + 65536L*UAEMINOR + UAESUBREV;

struct uae_prefs currprefs, changed_prefs; 

int no_gui = 0;
int joystickpresent = 0;
int cloanto_rom = 0;

extern int gfxHeight;
extern int hwScaled;

struct gui_info gui_data;

char warning_buffer[256];

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
    produce_sound = 2;
    prefs_gfx_framerate = 2;

    

	/* 1MB */
    prefs_chipmem_size = 0x00100000;
    prefs_bogomem_size = 0;
	changed_prefs.fastmem_size = 0;
}

int quit_program = 0;
int pause_program = 0;
int right_mouse;

int mainMenu_drives = DEFAULT_DRIVES;
int mainMenu_floppyspeed = 100;
int mainMenu_CPU_model = DEFAULT_CPU_MODEL;
int mainMenu_chipset = DEFAULT_CHIPSET_SELECT;
int mainMenu_sound = DEFAULT_SOUND;
int mainMenu_CPU_speed = 0;

int mainMenu_cpuSpeed = 600;

int mainMenu_joyConf = 0;
int mainMenu_autofireRate = 8;
int mainMenu_showStatus = DEFAULT_STATUSLN;
int mainMenu_mouseMultiplier = 1;
int mainMenu_stylusOffset = 0;
int mainMenu_tapDelay = 10;
int mainMenu_customControls = 0;
int mainMenu_custom_dpad = 0;
int mainMenu_custom_up = 0;
int mainMenu_custom_down = 0;
int mainMenu_custom_left = 0;
int mainMenu_custom_right = 0;
int mainMenu_custom_A = 0;
int mainMenu_custom_B = 0;
int mainMenu_custom_X = 0;
int mainMenu_custom_Y = 0;
int mainMenu_custom_L = 0;
int mainMenu_custom_R = 0;

int mainMenu_displayedLines = 240;
int mainMenu_displayHires = 0;
char presetMode[20] = "320x240 upscaled";
int presetModeId = 2;
int mainMenu_cutLeft = 0;
int mainMenu_cutRight = 0;
int mainMenu_ntsc = DEFAULT_NTSC;
int mainMenu_frameskip = 0;
int mainMenu_autofire = DEFAULT_AUTOFIRE;

// The following params in use, but can't be changed with gui
int mainMenu_throttle = 0;
int mainMenu_autosave = DEFAULT_AUTOSAVE;
int mainMenu_button1 = 0;
int mainMenu_button2 = 0;
int mainMenu_autofireButton1 = 0;
int mainMenu_jump = -1;

// The following params not in use, but stored to write them back to the config file
int mainMenu_enableHWscaling = DEFAULT_SCALING;
int gp2xClockSpeed = -1;
int mainMenu_scanlines = 0;
int mainMenu_ham = 1;
int mainMenu_enableScreenshots = DEFAULT_ENABLESCREENSHOTS;
int mainMenu_enableScripts = DEFAULT_ENABLESCRIPTS;

void SetPresetMode(int mode) {}
void update_display() {}
void menu_raise(void) {}
void menu_unraise(void) {}
void init_text(int splash) {}
void quit_text(void) {}
void inputmode_init(void) {}
void inputmode_redraw(void) {}
int run_mainMenu() {}

int saveMenu_n_savestate=0;
int gp2xButtonRemappingOn=0;
int hasGp2xButtonRemapping=0;
int gp2xMouseEmuOn=0;
int switch_autofire=0;
char *statusmessages[] = { "AUTOFIRE ON\0", "AUTOFIRE OFF\0","SCREENSHOT SAVED\0","SCRIPT SAVED\0","SCRIPT AND SCREENSHOT SAVED\0"};
int showmsg=0;

int mainMenu_chipMemory = DEFAULT_CHIPMEM_SELECT;
int mainMenu_slowMemory = 0;    /* off */
int mainMenu_fastMemory = 0;    /* off */

int mainMenu_bootHD = DEFAULT_ENABLE_HD;
int mainMenu_filesysUnits = 0;
int hd_dir_unit_nr = -1;
int hd_file_unit_nr = -1;

void UpdateCPUModelSettings(struct uae_prefs *p)
{
    switch (mainMenu_CPU_model)
    {
        case 1: p->cpu_level = M68020; break;
        default: p->cpu_level = M68000; break;
    }
}


void UpdateMemorySettings(struct uae_prefs *p)
{
    prefs_chipmem_size = 0x000080000 << mainMenu_chipMemory;

    /* >2MB chip memory => 0 fast memory */
    if ((mainMenu_chipMemory > 2) && (mainMenu_fastMemory > 0))
    {
        mainMenu_fastMemory = 0;
        p->fastmem_size = 0;
    }

    switch (mainMenu_slowMemory) 
    {
        case 1: case 2:
            prefs_bogomem_size = 0x00080000 << (mainMenu_slowMemory - 1);
            break;
        case 3:
            prefs_bogomem_size = 0x00180000;    /* 1.5M */
            break;
        default:
            prefs_bogomem_size = 0;
    }

    switch (mainMenu_fastMemory) 
    {
        case 0:
            p->fastmem_size = 0;
            break;
        default:
            p->fastmem_size = 0x00080000 << mainMenu_fastMemory;
    }

}


void UpdateChipsetSettings(struct uae_prefs *p)
{
    switch (mainMenu_chipset) 
    {
        case 1: p->chipset_mask = CSMASK_ECS_AGNUS | CSMASK_ECS_DENISE; break;
        case 2: p->chipset_mask = CSMASK_ECS_AGNUS | CSMASK_ECS_DENISE | CSMASK_AGA; break;
        default: p->chipset_mask = CSMASK_ECS_AGNUS; break;
    }
}


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

extern "C" void
JAVA_EXPORT_NAME(DemoActivity_setRightMouse) ( JNIEnv*  env, jobject  thiz, jint right) {
    right_mouse = right;
}

extern "C" void
JAVA_EXPORT_NAME(DemoActivity_setPrefs) ( JNIEnv*  env, jobject  thiz, jstring rom, jstring romkey, jstring hddir, jstring hdfile, jstring floppy1, jstring floppy2, jstring floppy3, jstring floppy4, jint frameskip, jint floppyspeed, jint cpu_model, jint chip_mem, jint slow_mem, jint fast_mem, jint chipset, jint cpu_speed, jint change_sound, jint sound, jint change_disk, jint reset, jint drive_status, jint ntsc ) {
    if (rom)
    {
        const char *srom = (env)->GetStringUTFChars(rom, 0);
        strcpy(romfile, srom);
        (env)->ReleaseStringUTFChars(rom, srom);
    }
    
    if (romkey)
    {
        const char *sromkey = (env)->GetStringUTFChars(romkey, 0);
        strcpy(romkeyfile, sromkey);
        (env)->ReleaseStringUTFChars(romkey, sromkey);
    }

    if (change_disk)
    {
        savestate_state = 0;
    }
    default_prefs_uae (&currprefs);
    default_prefs();
    
    mainMenu_floppyspeed = floppyspeed;
    mainMenu_CPU_model = cpu_model; // m68020
    mainMenu_chipMemory = chip_mem; // 2MB
    mainMenu_slowMemory = slow_mem;
    mainMenu_fastMemory = fast_mem;
    mainMenu_chipset = chipset; // aga
    mainMenu_CPU_speed = cpu_speed; // 500/5T/a1200/12T/12T2

    __android_log_print(ANDROID_LOG_INFO, "UAE", "floppyspeed= %d, cpu_model= %d, chip_mem= %d, slow_mem= %d, fast_mem= %d, chipset= %d, cpu_speed= %d", floppyspeed, cpu_model, chip_mem, slow_mem, fast_mem, chipset, cpu_speed);

    UpdateCPUModelSettings(&changed_prefs);
    UpdateMemorySettings(&changed_prefs);
    UpdateChipsetSettings(&changed_prefs);
    if (change_disk && uae4all_hard_dir[0] != '\0' && currprefs.mountinfo) {
        __android_log_print(ANDROID_LOG_INFO, "UAE", "kill_filesys_unit hd dir: %s", uae4all_hard_dir);
        kill_filesys_unit(currprefs.mountinfo, 0);
        mainMenu_filesysUnits--;
        hd_dir_unit_nr = -1;
        uae4all_hard_dir[0] = '\0';
    }
    if (hddir && currprefs.mountinfo)
    {            
        const char *hddir1 = (env)->GetStringUTFChars(hddir, 0);
        strcpy(uae4all_hard_dir, hddir1);
        (env)->ReleaseStringUTFChars(hddir, hddir1);

        __android_log_print(ANDROID_LOG_INFO, "UAE", "add_filesys_unit hd dir: %s", uae4all_hard_dir);
        char *s2 = add_filesys_unit (currprefs.mountinfo, "HD0", uae4all_hard_dir, 1, 0, 0, 0, 0);
        if (s2)
            __android_log_print(ANDROID_LOG_ERROR, "UAE", "%s\n", s2);
        hd_dir_unit_nr = mainMenu_filesysUnits++;
    }
    if (change_disk && uae4all_hard_file[0] != '\0' && currprefs.mountinfo) {
        __android_log_print(ANDROID_LOG_INFO, "UAE", "kill_filesys_unit hd file: %s", uae4all_hard_file);
        kill_filesys_unit(currprefs.mountinfo, 0);
        mainMenu_filesysUnits--;
        hd_file_unit_nr = -1;
        uae4all_hard_file[0] = '\0';
    } 
    if (hdfile && currprefs.mountinfo)
    {
        const char *hdfile1 = (env)->GetStringUTFChars(hdfile, 0);
        strcpy(uae4all_hard_file, hdfile1);
        (env)->ReleaseStringUTFChars(hdfile, hdfile1);
        __android_log_print(ANDROID_LOG_INFO, "UAE", "add_filesys_unit hd file: %s", uae4all_hard_file);
        char *s2 = add_filesys_unit (currprefs.mountinfo, 0, uae4all_hard_file, 0, 32, 1, 2, 512);
        if (s2)
            __android_log_print(ANDROID_LOG_ERROR, "UAE", "%s\n", s2);
        hd_file_unit_nr = mainMenu_filesysUnits++;
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
    changed_gfx_framerate = prefs_gfx_framerate;
    __android_log_print(ANDROID_LOG_INFO, "UAE", "prefs_gfx_framerate: %d", prefs_gfx_framerate);
    m68k_speed = 0;
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
        
    savestate_state = STATE_DOSAVE;
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
        
    savestate_state = STATE_DORESTORE;
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
    black_screen_now();
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
    filesys_reset ();
    filesys_start_threads ();
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
    __android_log_print(ANDROID_LOG_INFO, "UAE", "do_start_program");
	quit_program = 2;
	reset_frameskip();
	m68k_go (1);
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
}

void start_program (void)
{
    do_start_program ();
}

void leave_program (void)
{
    do_leave_program ();
}

void real_main (int argc, char **argv)
{
#ifdef USE_SDL
    SDL_Init (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK 
#if !defined(NO_SOUND) && !defined(GP2X)
 			| SDL_INIT_AUDIO
#endif
	);
#endif
	
    /* PocketUAE prefs */
   
#ifdef GP2X
    gp2x_init(argc, argv);
#endif
    //loadconfig (1);

    if (! graphics_setup ()) {
		exit (1);
    }

    rtarea_init ();

	hardfile_install();

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

#ifdef USE_AUTOCONFIG
    expansion_init ();
#endif

    memory_init ();

    filesys_install (); 
    native2amiga_install ();

    custom_init (); /* Must come after memory_init */
    DISK_init ();

    init_m68k();
#ifndef USE_FAME_CORE
    compiler_init ();
#endif
    //gui_update ();

#ifdef GP2X
    switch_to_hw_sdl(1);
#endif
    if (graphics_init())
	{
		start_program ();
	}
    leave_program ();
}

#ifndef NO_MAIN_IN_MAIN_C
int main (int argc, char *argv[])
{
	gfxHeight = 240;
	hwScaled = 1;

    real_main (argc, argv);
    return 0;
}

void lanada(void)
{
}

void default_prefs_uae (struct uae_prefs *p)
{
    p->chipset_mask = CSMASK_ECS_AGNUS;
    
    p->cpu_level = M68000;
    
    p->fastmem_size = 0x00000000;
    p->z3fastmem_size = 0x00000000;
    p->gfxmem_size = 0x00000000;

    p->mountinfo = alloc_mountinfo ();
}

void discard_prefs_uae (struct uae_prefs *p)
{
    free_mountinfo (p->mountinfo);
}
#endif
