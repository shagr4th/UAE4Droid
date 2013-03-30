 /* 
  * Minimalistic sound.c implementation for gp2x
  * (c) notaz, 2007
  */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>

#include "sysconfig.h"
#include "sysdeps.h"
#include "config.h"
#include "uae.h"
#include "options.h"
#include "memory-uae.h"
#include "debug_uae4all.h"
#include "audio.h"
#include "gensound.h"
#include "sound.h"
#include "custom.h"


extern unsigned long next_sample_evtime;

int produce_sound=0;
int changed_produce_sound=0;

unsigned int sound_rate=DEFAULT_SOUND_FREQ;

static uae_u16 sndbuffer[4][SNDBUFFER_LEN+32] UAE4ALL_ALIGN;
unsigned n_callback_sndbuff, n_render_sndbuff;
uae_u16 *sndbufpt = sndbuffer[0];
uae_u16 *render_sndbuff = sndbuffer[0];

#ifndef PROFILER_UAE4ALL

int tablas_ajuste[8][9]=
{
	{ 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 },	// 0
	{ 3 , 4 , 3 , 4 , 3 , 4 , 3 , 4 , 3 },	// 1 
	{ 1 , 2 , 1 , 2 , 1 , 2 , 1 , 2 , 1 },	// 2 
	{ 1 , 0 , 1 , 0 , 1 , 0 , 1 , 0 , 1 },	// 3 
	{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },	// 4 
	{ 0 ,-1 , 0 ,-1 , 0 ,-1 , 0 ,-1 , 0 },	// 5 
	{-2 ,-1 ,-2 ,-1 ,-2 ,-1 ,-2 ,-1 ,-2 },	// 6 
	{-4 ,-3 ,-4 ,-3 ,-4 ,-3 ,-4 ,-3 ,-4 },	// 7
};

#else

int tablas_ajuste[8][9]=
{
	{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },
	{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },
	{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },
	{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },
	{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },
	{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },
	{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },
	{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },
};

#endif


int *tabla_ajuste=(int *)&tablas_ajuste[4];

#ifdef NO_SOUND


void finish_sound_buffer (void) {  }

int setup_sound (void) { sound_available = 0; return 0; }

void close_sound (void) { }

int init_sound (void) { return 0; }

void pause_sound (void) { }

void resume_sound (void) { }

void uae4all_init_sound(void) { }

void uae4all_play_click(void) { }

void uae4all_pause_music(void) { }

void uae4all_resume_music(void) { }

#else 


static int have_sound = 0;

void sound_default_evtime(void)
{
	int pal = beamcon0 & 0x20;
	switch(m68k_speed)
	{
		case 6:
			scaled_sample_evtime=(unsigned)(MAXHPOS_PAL*MAXVPOS_PAL*VBLANK_HZ_PAL*CYCLE_UNIT/1.86)/sound_rate;
			break;

		case 5:
		case 4: // ~4/3 234
			if (pal)
				scaled_sample_evtime=(MAXHPOS_PAL*244*VBLANK_HZ_PAL*CYCLE_UNIT)/sound_rate; // ???
			else
				scaled_sample_evtime=(MAXHPOS_NTSC*255*VBLANK_HZ_NTSC*CYCLE_UNIT)/sound_rate;
			break;

		case 3:
		case 2: // ~8/7 273
			if (pal)
				scaled_sample_evtime=(MAXHPOS_PAL*270*VBLANK_HZ_PAL*CYCLE_UNIT)/sound_rate;
			else
				scaled_sample_evtime=(MAXHPOS_NTSC*255*VBLANK_HZ_NTSC*CYCLE_UNIT)/sound_rate;
			break;

		case 1:
		default: // MAXVPOS_PAL?
			if (pal)
				scaled_sample_evtime=(MAXHPOS_PAL*313*VBLANK_HZ_PAL*CYCLE_UNIT)/sound_rate;
			else
				scaled_sample_evtime=(MAXHPOS_NTSC*MAXVPOS_NTSC*VBLANK_HZ_NTSC*CYCLE_UNIT)/sound_rate + 1;
			break;
	}

	schedule_audio();
}


static int sounddev = -1, s_oldrate = 0, s_oldbits = 0, s_oldstereo = 0;
static int sound_thread_active = 0, sound_thread_exit = 0;
static sem_t sound_sem;

static void *sound_thread(void *unused)
{
	int cnt = 0, sem_val = 0;
	sound_thread_active = 1;

	printf("sound_thread started\n");
	for (;;)
	{
		sem_getvalue(&sound_sem, &sem_val);
		while (sem_val > 1)
		{
			//printf("skip %i (%i)\n", cnt, sem_val);
			sem_wait(&sound_sem);
			cnt++;
			sem_getvalue(&sound_sem, &sem_val);
		}

		sem_wait(&sound_sem);
		if (sound_thread_exit) break;

		//printf("writing %i\n", cnt);
		write(sounddev, sndbuffer[cnt&3], SNDBUFFER_LEN);
		//printf("wrote %i\n", cnt);
		cnt++;
	}

	printf("leaving sound_thread\n");
	return NULL;
}

static int gp2x_start_sound(int rate, int bits, int stereo)
{
	int frag = 0, buffers, ret;
	unsigned int bsize;

	if (!sound_thread_active)
	{
		// init sem, start sound thread
		printf("starting sound thread..\n");
		pthread_t thr;
		ret = sem_init(&sound_sem, 0, 0);
		if (ret != 0) printf("sem_init() failed: %i, errno=%i\n", ret, errno);
		ret = pthread_create(&thr, NULL, sound_thread, NULL);
		if (ret != 0) printf("pthread_create() failed: %i\n", ret);
		pthread_detach(thr);
	}

	//if (sounddev > 0) close(sounddev);
	if (sounddev <= 0)
	{
		sounddev = open("/dev/dsp", O_WRONLY);
		if (sounddev == -1)
		{
			printf("open(\"/dev/dsp\") failed with %i\n", errno);
			return -1;
		}
	}

	// if no settings change, we don't need to do anything
	if (rate == s_oldrate && s_oldbits == bits && s_oldstereo == stereo) return 0;

	ioctl(sounddev, SNDCTL_DSP_SPEED,  &rate);
	ioctl(sounddev, SNDCTL_DSP_SETFMT, &bits);
	ioctl(sounddev, SNDCTL_DSP_STEREO, &stereo);
	// calculate buffer size
	buffers = 16;
	bsize = rate / 32;
	if (rate > 22050) { bsize*=4; buffers*=2; } // 44k mode seems to be very demanding
	while ((bsize>>=1)) frag++;
	frag |= buffers<<16; // 16 buffers
	ioctl(sounddev, SNDCTL_DSP_SETFRAGMENT, &frag);
	printf("gp2x_set_sound: %i/%ibit/%s, %i buffers of %i bytes\n",
		rate, bits, stereo?"stereo":"mono", frag>>16, 1<<(frag&0xffff));

	s_oldrate = rate; s_oldbits = bits; s_oldstereo = stereo;
	usleep(100000);
	return 0;
}


// this is meant to be called only once on exit
void gp2x_stop_sound(void)
{
	if (sound_thread_exit)
		printf("don't call gp2x_stop_sound more than once!\n");
	if (sound_thread_active)
	{
		printf("stopping sound thread..\n");
		sound_thread_exit = 1;
		sem_post(&sound_sem);
		usleep(100*1000);
	}

	if (sounddev > 0)
		close(sounddev);
	sounddev = -1;
}



void finish_sound_buffer (void)
{
	static int wrcnt = 0;
#ifdef DEBUG_SOUND
	dbg("sound.c : finish_sound_buffer");
#endif

	//printf("finish %i\n", wrcnt);
#if 0
	write(sounddev, sndbuffer[0], SNDBUFFER_LEN);
	sndbufpt = render_sndbuff = sndbuffer[0];
#else
	sem_post(&sound_sem);
	wrcnt++;
	sndbufpt = render_sndbuff = sndbuffer[wrcnt&3];
#endif

#ifdef DEBUG_SOUND
	dbg(" sound.c : ! finish_sound_buffer");
#endif
}


/* Try to determine whether sound is available.  This is only for GUI purposes.  */
int setup_sound (void)
{
#ifdef DEBUG_SOUND
    dbg("sound.c : setup_sound");
#endif

    if (gp2x_start_sound(sound_rate, 16, 0) != 0)
	    return 0;

#ifdef DEBUG_SOUND
    dbg(" sound.c : ! setup_sound");
#endif
    return 1;
}

static int open_sound (void)
{
#ifdef DEBUG_SOUND
    dbg("sound.c : open_sound");
#endif

    if (gp2x_start_sound(sound_rate, 16, 0) != 0)
	    return 0;

    sound_default_evtime();

    have_sound = 1;
    scaled_sample_evtime_ok = 1;
    sound_available = 1;

#ifdef DEBUG_SOUND
    dbg(" sound.c : ! open_sound");
#endif
    return 1;
}

void close_sound (void)
{
#ifdef DEBUG_SOUND
    dbg("sound.c : close_sound");
#endif
    if (!have_sound)
	return;

    // testing shows that reopenning sound device is not a good idea on gp2x (causes random sound driver crashes)
    // we will close it on real exit instead
    //gp2x_stop_sound();
    have_sound = 0;

#ifdef DEBUG_SOUND
    dbg(" sound.c : ! close_sound");
#endif
}

int init_sound (void)
{
#ifdef DEBUG_SOUND
    dbg("sound.c : init_sound");
#endif

    have_sound=open_sound();

#ifdef DEBUG_SOUND
    dbg(" sound.c : ! init_sound");
#endif
    return have_sound;
}

void pause_sound (void)
{
#ifdef DEBUG_SOUND
    dbg("sound.c : pause_sound");
#endif

    /* nothing to do */

#ifdef DEBUG_SOUND
    dbg(" sound.c : ! pause_sound");
#endif
}

void resume_sound (void)
{
#ifdef DEBUG_SOUND
    dbg("sound.c : resume_sound");
#endif

    /* nothing to do */

#ifdef DEBUG_SOUND
    dbg(" sound.c : ! resume_sound");
#endif
}

void uae4all_init_sound(void)
{
#ifdef DEBUG_SOUND
    dbg("sound.c : uae4all_init_sound");
#endif
#ifdef DEBUG_SOUND
    dbg(" sound.c : ! uae4all_init_sound");
#endif
}

void uae4all_pause_music(void)
{
#ifdef DEBUG_SOUND
    dbg("sound.c : pause_music");
#endif
#ifdef DEBUG_SOUND
    dbg(" sound.c : ! pause_music");
#endif
}

void uae4all_resume_music(void)
{
#ifdef DEBUG_SOUND
    dbg("sound.c : resume_music");
#endif
#ifdef DEBUG_SOUND
    dbg(" sound.c : ! resume_music");
#endif
}

void uae4all_play_click(void)
{
}
#endif

