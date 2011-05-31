#include<stdio.h>
#include<stdlib.h>
#include<SDL.h>
#include "gp2x.h"
#include "volumecontrol.h"


extern SDL_Surface *prSDLScreen;

static SDL_Surface *ksur;

extern int soundVolume;

void volumecontrol_init(void)
{
	// don't know if we'll ever need anything here.
}
	

void volumecontrol_redraw(void)
{
	SDL_Rect r;
	SDL_Surface* surface;
	int i;

	Uint32 green = SDL_MapRGB(prSDLScreen->format, 0,255,0);

	r.x=110;
	r.y=prSDLScreen->h-80;
	r.w=soundVolume;
	r.h=15;

	// draw the blocks now
	SDL_FillRect(prSDLScreen, &r, green);
	SDL_Delay(100);
}

