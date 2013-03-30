#define _MENU_CONFIG_CPP

#include "menu.h"
#include "menu_config.h"
#include "sysconfig.h"
#include "sysdeps.h"
#include "autoconf.h"
#include "options.h"
#include "gui.h"
#include "sound.h"
#include "custom.h"
#include "gp2x.h"

extern int kickstart;
extern int sound_rate;
extern int skipintro;
extern int screenWidth;
extern int moveX;
extern int moveY;
extern int timeslice_mode;

int mainMenu_chipMemory = DEFAULT_CHIPMEM_SELECT;
int mainMenu_slowMemory = 0;	/* off */
int mainMenu_fastMemory = 0;	/* off */

int mainMenu_bootHD = DEFAULT_ENABLE_HD;
int mainMenu_filesysUnits = 0;
int hd_dir_unit_nr = -1;
int hd_file_unit_nr = -1;

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
int mainMenu_mouseMultiplier = DEFAULT_MOUSEMULTIPLIER;
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
int mainMenu_button1 = GP2X_BUTTON_X;
int mainMenu_button2 = GP2X_BUTTON_A;
int mainMenu_autofireButton1 = GP2X_BUTTON_B;
int mainMenu_jump = -1;

// The following params not in use, but stored to write them back to the config file
int mainMenu_enableHWscaling = DEFAULT_SCALING;
int gp2xClockSpeed = -1;
int mainMenu_scanlines = 0;
int mainMenu_ham = 1;
int mainMenu_enableScreenshots = DEFAULT_ENABLESCREENSHOTS;
int mainMenu_enableScripts = DEFAULT_ENABLESCRIPTS;


void SetDefaultMenuSettings(int clearDisks)
{
	mainMenu_chipMemory = DEFAULT_CHIPMEM_SELECT;
	mainMenu_slowMemory = 0;	/* off */
	mainMenu_fastMemory = 0;	/* off */
	UpdateMemorySettings();

	mainMenu_bootHD=DEFAULT_ENABLE_HD;
	if (hd_dir_unit_nr >= 0) 
	{
		kill_filesys_unit(currprefs.mountinfo, 0);
		hd_dir_unit_nr = -1;
	}
	if (hd_file_unit_nr >= 0) 
	{
		kill_filesys_unit(currprefs.mountinfo, 0);
		hd_file_unit_nr = -1;
	}
	mainMenu_filesysUnits = 0;

	if(clearDisks)
	{
		uae4all_image_file0[0] = '\0';
		uae4all_image_file1[0] = '\0';
		uae4all_image_file2[0] = '\0';
		uae4all_image_file3[0] = '\0';
		mainMenu_drives = DEFAULT_DRIVES;
	}	
	mainMenu_floppyspeed = 100;

	mainMenu_CPU_model = DEFAULT_CPU_MODEL;
	mainMenu_chipset = DEFAULT_CHIPSET_SELECT;
	UpdateChipsetSettings();
	timeslice_mode = 2;
	kickstart = DEFAULT_KICKSTART;
	mainMenu_sound = DEFAULT_SOUND;
	sound_rate = DEFAULT_SOUND_FREQ;
	mainMenu_CPU_speed = 0;
	
	mainMenu_cpuSpeed = 600;
	
	mainMenu_joyConf = 0;
	mainMenu_autofireRate = 8;
	mainMenu_showStatus = DEFAULT_STATUSLN;
	mainMenu_mouseMultiplier = DEFAULT_MOUSEMULTIPLIER;
	mainMenu_stylusOffset = 0;
	mainMenu_tapDelay = 10;
	mainMenu_customControls = 0;
	mainMenu_custom_dpad = 0;
	mainMenu_custom_up = 0;
	mainMenu_custom_down = 0;
	mainMenu_custom_left = 0;
	mainMenu_custom_right = 0;
	mainMenu_custom_A = 0;
	mainMenu_custom_B = 0;
	mainMenu_custom_X = 0;
	mainMenu_custom_Y = 0;
	mainMenu_custom_L = 0;
	mainMenu_custom_R = 0;
	
	SetPresetMode(2);
	moveX = 0;
	moveY = 0;
	mainMenu_cutLeft = 0;
	mainMenu_cutRight = 0;
	mainMenu_ntsc = DEFAULT_NTSC;
	mainMenu_frameskip = 0;
	mainMenu_autofire = DEFAULT_AUTOFIRE;
	
	// The following params can't be changed in gui
	skipintro = DEFAULT_SKIPINTRO;
	mainMenu_throttle = 0;
	mainMenu_autosave = DEFAULT_AUTOSAVE;
	mainMenu_button1 = GP2X_BUTTON_X;
	mainMenu_button2 = GP2X_BUTTON_A;
	mainMenu_autofireButton1 = GP2X_BUTTON_B;
	mainMenu_jump = -1;
	
	mainMenu_enableHWscaling = DEFAULT_SCALING;
	gp2xClockSpeed = -1;
	mainMenu_scanlines = 0;
	mainMenu_ham = 1;
	mainMenu_enableScreenshots = DEFAULT_ENABLESCREENSHOTS;
	mainMenu_enableScripts = DEFAULT_ENABLESCRIPTS;
}


void UpdateCPUModelSettings()
{
	switch (mainMenu_CPU_model)
	{
	    case 1: changed_prefs.cpu_level = M68020; break;
	    default: changed_prefs.cpu_level = M68000; break;
	}
}


void UpdateMemorySettings()
{
	prefs_chipmem_size = 0x000080000 << mainMenu_chipMemory;

	/* >2MB chip memory => 0 fast memory */
	if ((mainMenu_chipMemory > 2) && (mainMenu_fastMemory > 0))
	{
		mainMenu_fastMemory = 0;
		changed_prefs.fastmem_size = 0;
	}

	switch (mainMenu_slowMemory) 
	{
		case 1: case 2:
			prefs_bogomem_size = 0x00080000 << (mainMenu_slowMemory - 1);
			break;
		case 3:
			prefs_bogomem_size = 0x00180000;	/* 1.5M */
			break;
		default:
			prefs_bogomem_size = 0;
	}

	switch (mainMenu_fastMemory) 
	{
		case 0:
			changed_prefs.fastmem_size = 0;
			break;
		default:
			changed_prefs.fastmem_size = 0x00080000 << mainMenu_fastMemory;
	}

}


void UpdateChipsetSettings()
{
	switch (mainMenu_chipset) 
	{
		case 1: changed_prefs.chipset_mask = CSMASK_ECS_AGNUS | CSMASK_ECS_DENISE; break;
		case 2: changed_prefs.chipset_mask = CSMASK_ECS_AGNUS | CSMASK_ECS_DENISE | CSMASK_AGA; break;
		default: changed_prefs.chipset_mask = CSMASK_ECS_AGNUS; break;
	}
}


void SetPresetMode(int mode)
{
	presetModeId = mode;
	
	switch(mode)
	{
		case 0:
			mainMenu_displayedLines = 200;
			screenWidth = 768;
			strcpy(presetMode, "320x200 upscaled");
			break;
			
		case 1:
			mainMenu_displayedLines = 216;
			screenWidth = 716;
			strcpy(presetMode, "320x216 upscaled");
			break;
			
		case 2:
			mainMenu_displayedLines = 240;
			screenWidth = 640;
			strcpy(presetMode, "320x240 upscaled");
			break;

		case 3:
			mainMenu_displayedLines = 256;
			screenWidth = 600;
			strcpy(presetMode, "320x256 upscaled");
			break;
						
		case 4:
			mainMenu_displayedLines = 262;
			screenWidth = 588;
			strcpy(presetMode, "320x262 upscaled");
			break;
						
		case 5:
			mainMenu_displayedLines = 270;
			screenWidth = 570;
			strcpy(presetMode, "320x270 upscaled");
			break;
						
		case 6:
			mainMenu_displayedLines = 200;
			screenWidth = 640;
			strcpy(presetMode, "320x200 NTSC");
			break;
						
		case 7:
			mainMenu_displayedLines = 200;
			screenWidth = 800;
			strcpy(presetMode, "320x200 fullscreen");
			break;
						
		case 10:
			mainMenu_displayedLines = 200;
			screenWidth = 768;
			strcpy(presetMode, "640x200 upscaled");
			break;
			
		case 11:
			mainMenu_displayedLines = 216;
			screenWidth = 716;
			strcpy(presetMode, "640x216 upscaled");
			break;
			
		case 12:
			mainMenu_displayedLines = 240;
			screenWidth = 640;
			strcpy(presetMode, "640x240 upscaled");
			break;

		case 13:
			mainMenu_displayedLines = 256;
			screenWidth = 600;
			strcpy(presetMode, "640x256 upscaled");
			break;
						
		case 14:
			mainMenu_displayedLines = 262;
			screenWidth = 588;
			strcpy(presetMode, "640x262 upscaled");
			break;
						
		case 15:
			mainMenu_displayedLines = 270;
			screenWidth = 570;
			strcpy(presetMode, "640x270 upscaled");
			break;
						
		case 16:
			mainMenu_displayedLines = 200;
			screenWidth = 640;
			strcpy(presetMode, "640x200 NTSC");
			break;
						
		case 17:
			mainMenu_displayedLines = 200;
			screenWidth = 800;
			strcpy(presetMode, "640x200 fullscreen");
			break;

		default:
			mainMenu_displayedLines = 240;
			screenWidth = 640;
			strcpy(presetMode, "320x240 upscaled");
			presetModeId = 2;
			break;
	}
	
	if(presetModeId < 10)
		mainMenu_displayHires = 0;
	else
		mainMenu_displayHires = 1;
}
