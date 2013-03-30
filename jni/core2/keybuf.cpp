 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Keyboard buffer. Not really needed for X, but for SVGAlib and possibly
  * Mac and DOS ports.
  *
  * Note: it's possible to have two threads in UAE, one reading keystrokes
  * and the other one writing them. Despite this, no synchronization effort
  * is needed. This code should be perfectly thread safe. At least if you
  * assume that integer store instructions are atomic.
  *
  * Copyright 1995, 1997 Bernd Schmidt
  */

#include "sysconfig.h"
#include "sysdeps.h"
#include <assert.h>

#include "config.h"
#include "uae.h"
#include "options.h"
#include "keybuf.h"
#include "keyboard.h"
#include "joystick.h"
#include "custom.h"

static int kpb_first, kpb_last;

static int keybuf[256];

int keys_available (void)
{
    int val;
    val = kpb_first != kpb_last;
    return val;
}

int get_next_key (void)
{
    int key;
    assert (kpb_first != kpb_last);

    key = keybuf[kpb_last];
    if (++kpb_last == 256)
	kpb_last = 0;
    return key;
}

void record_key (int kc)
{
    int kpb_next = kpb_first + 1;

    if (kpb_next == 256)
	kpb_next = 0;
    if (kpb_next == kpb_last) {
	write_log ("Keyboard buffer overrun. Congratulations.\n");
	return;
    }
    if ((kc >> 1) == AK_RCTRL) {
	kc ^= AK_RCTRL << 1;
	kc ^= AK_CTRL << 1;
    }
    keybuf[kpb_first] = kc;
    kpb_first = kpb_next;
}

void keybuf_init (void)
{
    kpb_first = kpb_last = 0;
}
