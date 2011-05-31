#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <stdlib.h>
#include <linux/soundcard.h>
#include "SDL.h"
#include "gp2x.h"
#include "gp2xutil.h"
#include "menu/menu.h"

extern int uae4all_keystate[256];
extern void record_key(int);


int vol = 100;
unsigned long memDev;
volatile unsigned short* MEM_REG;
int gp2xClockSpeed;

static int mixerdev;
int soundVolume = 50;
int flashLED;

int gp2xMouseEmuOn;
int gp2xButtonRemappingOn;
int hasGp2xButtonRemapping;
char launchDir[300];

extern int graphics_init (void);

//#define SOUND_MIXER_WRITE_PCM 0xc0044d04

#define GPIOHOUT 0x106E
#define GPIOHPINLVL 0x118E

static int mmuhack(int first_time)
{
	int mmufd;
	char cmdLine[256];

	if (first_time)
	{
		// unload possibly wrong mmuhack if it's loaded
		system("/sbin/rmmod mmuhack");
		snprintf(cmdLine, sizeof(cmdLine), "/sbin/insmod -f %s/mmuhack.o", launchDir);
		system(cmdLine);
	}

	mmufd = open("/dev/mmuhack", O_RDWR);
	if (mmufd < 0) 
	{
		return 1;
	}

	close(mmufd);

	return 0;
}


void gp2x_init(int argc, char **argv)
{
	parse_cmdline(argc, argv);

	//getcwd(launchDir, 256);
	gp2xClockSpeed = -1;

	memDev = open("/dev/mem", O_RDWR);
	MEM_REG=(unsigned short *)mmap(0, 0x10000, PROT_READ|PROT_WRITE, MAP_SHARED,memDev, 0xc0000000);

	mixerdev = open("/dev/mixer", O_RDWR);

	soundVolume=50;
	gp2x_set_volume(soundVolume);
	flashLED = 0;

	SDL_ShowCursor(SDL_DISABLE);
	getcwd(launchDir, 250);
}

int is_overridden_button(int button)
{
	// TODO: load from file
	return button == GP2X_BUTTON_L || button == GP2X_BUTTON_R || 
		   button == GP2X_BUTTON_A || button == GP2X_BUTTON_B ||
		   button == GP2X_BUTTON_X || button == GP2X_BUTTON_Y;
}

int get_key_for_button(int button)
{
	// TODO: load from file
	if (button == GP2X_BUTTON_L)
	{
		return 0x66; // left amiga
	}
	else if (button == GP2X_BUTTON_R)
	{
		return 0x67; // right amiga
	}
	else if (button == GP2X_BUTTON_A)
	{
		return 0x50; // f1
	}
	else if (button == GP2X_BUTTON_B)
	{
		return 0x4d; // down arrow
	}
	else if (button == GP2X_BUTTON_X)
	{
		return 0x40; // space
	}
	else if (button == GP2X_BUTTON_Y)
	{
		return 0x40; // space
	}
	return 0;
}

// apply the remapped button keystroke
void handle_remapped_button_down(int button)
{
	int key = get_key_for_button(button);
	if (!uae4all_keystate[key])
	{
		uae4all_keystate[key] = 1;
		record_key(key << 1);
	}
}

void handle_remapped_button_up(int button)
{
	int key = get_key_for_button(button);

	if (uae4all_keystate[key])
	{
		uae4all_keystate[key] = 0;
		record_key((key << 1) | 1);
	}
}


void gp2x_set_volume(int volume)
{
	int vol = (((volume*0x50)/100)<<8)|((volume*0x50)/100);
	ioctl(mixerdev, SOUND_MIXER_WRITE_PCM, &vol);
}

void setBatteryLED(int state)
{
	/*if (flashLED)
	{
		int i;

		// On/Off battery LED
		if(MEM_REG[GPIOHPINLVL >> 1] & 16)
			i = 0;
		else
			i = 1;

		if (i!=state)
		{
			MEM_REG[GPIOHOUT >> 1] ^= 16;
		}
	}*/
}

void switch_to_hw_sdl(int first_time)
{
	// unmap memory, because we don't wat it to be mmuhacked
	munmap((void *)MEM_REG, 0x10000);

	printf("switching to SDL_HWSURFACE... "); fflush(stdout);
	prSDLScreen = SDL_SetVideoMode(320,240,16,SDL_HWSURFACE|SDL_FULLSCREEN);
	SDL_ShowCursor(SDL_DISABLE);
	printf(prSDLScreen ? "done\n" : "failed\n");
	usleep(100*1000);
	mmuhack(first_time);

	// map again
	MEM_REG=(unsigned short *)mmap(0, 0x10000, PROT_READ|PROT_WRITE, MAP_SHARED,memDev, 0xc0000000);

	// reinit video
	graphics_init();
}

void switch_to_sw_sdl(void)
{
	printf("switching to SDL_SWSURFACE... "); fflush(stdout);
	prSDLScreen = SDL_SetVideoMode(320,240,16,SDL_SWSURFACE|SDL_FULLSCREEN);
	SDL_ShowCursor(SDL_DISABLE);
	printf(prSDLScreen ? "done\n" : "failed\n");
}

