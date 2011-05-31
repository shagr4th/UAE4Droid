 /* 
  * Minimalistic sound.c implementation for gp2x
  * (c) notaz, 2007
  */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include <jni.h>

#include "sysconfig.h"
#include "sysdeps.h"
#include "config.h"
#include "uae.h"
#include "options.h"
#include "memorya.h"
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

static jshortArray audioarray = NULL;
/*static JNIEnv * jniEnv = NULL;

static JNIEnv *sandroid_env;
static jclass sandroid_caller;
static jobject sandroid_callback;*/
static jmethodID android_sendaudio;
static jmethodID android_initaudio;
static jmethodID android_playSound;
static jmethodID android_initSound;
//static JavaVM *jvm;
int sinit;

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

/*static jbyte * soundBuffer = NULL;
static jbyte * currentSoundBuffer = NULL;
static int currentBlock;*/

#ifndef SDL_JAVA_PACKAGE_PATH
#error You have to define SDL_JAVA_PACKAGE_PATH to your package path with dots replaced with underscores, for example "com_example_SanAngeles"
#endif
#define JAVA_EXPORT_NAME2(name,package) Java_##package##_##name
#define JAVA_EXPORT_NAME1(name,package) JAVA_EXPORT_NAME2(name,package)
#define JAVA_EXPORT_NAME(name) JAVA_EXPORT_NAME1(name,SDL_JAVA_PACKAGE_PATH)
/*
extern "C" void JAVA_EXPORT_NAME(DemoActivity_nativeAudioInit) (JNIEnv * env,jobject  thiz, jobject callback)
{
	__android_log_print(ANDROID_LOG_INFO, "libSDL", "startAudioRegister");
	sandroid_callback = callback;
	sandroid_env = env;
	sandroid_caller = (env)->GetObjectClass( callback);
	android_sendaudio = (sandroid_env)->GetMethodID(sandroid_caller, "sendAudio", "([BI)I");
	android_initaudio = (sandroid_env)->GetMethodID(sandroid_caller, "initAudio", "(II)V");
	//(env)->GetJavaVM( &jvm);
	__android_log_print(ANDROID_LOG_INFO, "libSDL", "endAudioRegister");
};
*/

void finish_sound_buffer (void)
{
	static int wrcnt = 0;
	//static short* buffers = new short[SNDBUFFER_LEN];
#ifdef DEBUG_SOUND
	dbg("sound.c : finish_sound_buffer");
#endif



if (android_env && !sinit) {
	android_sendaudio = (android_env)->GetMethodID(android_caller, "sendAudio", "([SI)I");
	android_initaudio = (android_env)->GetMethodID(android_caller, "initAudio", "(II)V");

	/*android_initSound = (android_env)->GetMethodID(android_caller, "initSound", "(IIII)Ljava/nio/ByteBuffer;");
	android_playSound = (android_env)->GetMethodID(android_caller, "playSound", "()V");
	jobject buffer = (android_env)->CallObjectMethod(android_callback, android_initSound, DEFAULT_SOUND_FREQ, 16, SNDBUFFER_LEN, 32);
	soundBuffer = (jbyte*) (android_env)->GetDirectBufferAddress(buffer);
	currentSoundBuffer = soundBuffer;*/

	(android_env)->CallVoidMethod(android_callback, android_initaudio, DEFAULT_SOUND_FREQ, 16);
	audioarray = (android_env)->NewShortArray(SNDBUFFER_LEN);
	sinit = 1;
}

if (android_env)
{

	(android_env)->SetShortArrayRegion(audioarray, 0, SNDBUFFER_LEN/2, (jshort*) sndbuffer[(wrcnt&3)]);
	jint result = (android_env)->CallIntMethod(android_callback, android_sendaudio, audioarray, SNDBUFFER_LEN/2);

	/*memcpy(currentSoundBuffer, sndbuffer[(wrcnt&3)], SNDBUFFER_LEN);
	(android_env)->CallVoidMethod(android_callback, android_playSound);
	currentSoundBuffer += SNDBUFFER_LEN;
	currentBlock ++;
	if (currentBlock==32)
	{
		currentBlock = 0;
		currentSoundBuffer = soundBuffer;
	}
*/
}

wrcnt++;
sndbufpt = render_sndbuff = sndbuffer[wrcnt&3];

//__android_log_print(ANDROID_LOG_INFO, "libSDL", "buffer");

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

