uae4allgp2x0.7.0
----------------

This is an Amiga emulator for the GP2X.  It runs quite a lot of games at a reasonable speed with sound.

uae4all is Chui's work - I have just changed a few things to make it a bit more convenient to run
on the GP2X.

This port is maintained by critical (criticalhippo@gmail.com).

The wiki at:

http://wiki.gp2x.org/wiki/UAE4all 

is where the compatibility list, keys, etc. are held.

Since 0.7.0 Chui and fox68k's FAME was replaced with Cyclone for some additional speed improvements.

Cyclone was started by fDave, but finished and integrated by notaz.

Some blitter optimizations were submitted by GnoStiC.

The mmu hack included in this release is thanks to Squidge.

The cpuspeed.o included in this release is thanks to Hermes/PS2Reality, Vimacs and god_at_hell.

There is use of ryleh's minlib too, and of course Paeryn's HW SDL libs.


Installation
------------

* Copy uae4all.gpe and mmuhack.o into a directory on your SD card

* Copy the data directory to that directory, so it sits beneath 
  the directory you just copied uae4all.gpe to.

* Put your kick.rom file in the same directory as uae4all.gpe
  (this is kickstart 1.3 image, 512KB overdumped version).

* Create a roms directory in the same directory as uae4all.gpe 
  (but you can also use any other directory).


Command line options
--------------------

Since 0.7.1 these command line options are accepted:

-norelaunchmenu {1,0}    If not 0, uae4all won't relauch gp2xmenu on exit.
-statusln {1,0}          0 disables status line, nonzero enables.
-mousemultiplier <x>     Mouse multiplier: 1 for 1x, 2 for 2x, etc.
-sound {0,1,2}           0 disables sound, 1 enables, 2 fakes it.
-soundrate <x>           Sound rate in Hz, for example 22050.
-autosave {1,0}          Turn autosave on/off.
-systemclock {0,2,4}     System clock. 0 means 100%, 2 means 83%, 4 means 75%.
-syncthreshold {0,1,2,3} Sync threshold. 0,1,2,3 is for 100% 75% 50% and 25%.
-frameskip <x>           Sets frameskip. -1 means auto, >=0 is the frameskip number.
-skipintro {1,0}         Don't show uae4all intro on startup.
-ntsc {1,0}              0 means PAL mode, not 0 NTSC.
-kick <name>             Filename of kickstart ROM image to use.
-df0 <name>              Filename of disk image to insert to first drive on startup.
                         Also skips the menu and boots the disk.
-df1 <name>              Filename of disk image to insert to second drive on startup.


Release history
---------------

20060203 - version 0.1.0	Initial release

20060204 - version 0.2.0 	Added diagonals to directions
				Fixed down-left causing return to menu (select does that now)
				Added left mouse-click by pressing L button (does the job)
				Added right mouse-click by pressing R button (untested)
                         	Tested with Chaos Engine as well now (handles disk swap)

20060204 - version 0.2.1 	Added second joystick button support (Y button).  This lets
                         	you turn into the spinning disc in Turrican, and may well work
                         	with other games too (I'm hoping Chase HQ, Shadow Dancer, and
                         	Wonderboy).

				Made exit on the main menu exit back to the GP2X menu properly.

20060205 - version 0.2.5 	Added mouse support.  The START button toggles it on and off.
                        	This lets you play Lemmings :)  Hopefully UFO might work too.

20060210 - version 0.2.8	Added ability to disable status line.
				Mapped A button to send keystrokes 'Return', then 'Space'.
				Used Iorgy77's new splash screen and background bmps (cheers).

20060212 - version 0.3.0        Added virtual keyboard support (rough around the edges still).

20060213 - version 0.3.1	Stopped sending joystick events to the games when in virtual
				keyboard mode.

20060215 - version 0.3.2        Added initial remapping support and removed awful hack that made
                                users install to /uae4all.  Set up remapped demo key for Pinball
                                Dreams.

20060218 - version 0.4.0	Initial stab at sound support.  Also, added bitmaps for joystick,
				mouse, and remapping modes so they show which mode you're 
				switching to when you press the Start button.

20060221 - version 0.4.1        Had a go at providing overclocking options.  Also added volume
				control (use vol-/+, provides on-screen display).  Changed 
				throttle settings to be a bit more sensible (hopefully).

20060223 - version 0.4.2	Changed the overclocking support to leave existing settings
				alone initially.  Fixed volume control in mouse emu mode.

20060227 - version 0.4.3	Added option to use battery LED to show disk access (thanks
				to Waninkoko's sdl2x code).

20060303 - version 0.4.5	Added support for HAM and EHB.  Added 'Fake' option to sound,
				which does enough to get stubborn games to run, but is intended
				to allow the game to still run faster than it would if real
				sound was being output.

20060313 - version 0.5.0	Merged in Chui's changes for rc-1 of his dreamcast release.
				Improved compatibility (Three Stooges now works, for example),
				and two disk drive support (tested with Lemmings).  Also
				super throttle and adz.  Fake sound temporarily doesn't work.  

20060314 - version 0.5.1	Stopped switching input mode when virtual keyboard visible.
				Fixed annoying "have to select directories twice sometimes" bug
				Sorted both directories and files, rather than just files
				Implemented turbo mouse (hold down X for triple speed mouse)
				Added 266MHz to the overclocking options
				Added png icon

20061029 - version 0.6.0	Used Chui's new FAME core.  Used squidge's mmu hack.

20061030 - version 0.6.1	Fixed crashes on overclock.  Fixed flickering on input mode change.

20061031 - version 0.6.2	Fixed crashes on overclock (hopefully).  Added SELECT to show full
				filename in file browser.  Added -norelaunchmenu command line param
				to stop it relaunching the gp2x menu app on exit.

20061111 - version 0.6.3	Really fixed crashes on overclock.  It seems to just crash when
				using HW accelerated SDL on the main menu.  Added mouse acceleration
				option to main menu.  Various other cosmetic things (no more flickering
				virtual keyboard).

20061113 - version 0.6.4	Added disk writing.  Auto save in the main menu defaults to 'on', which
				will cause any writes to disk to be written into the saves directory
				under the dir in which uae4all is stored.  Defaulted sound to 'on'.

20070718 - version 0.7.0        (notaz's release, but still official)
                                Integrated Cyclone core. Added fps counter (enable status line to see).
                                GnoStiC submitted some blitter optimizations. SDL_Mixer removed,
                                executable size reduced. Now using SDL_HWSURFACE in game and
                                SDL_SWSURFACE in menus (so overclocking doesn't crash, but
                                flickering keyboard returns, unfortunately). Sound rate
                                selector added.

20070729 - version 0.7.1        (notaz's release)
                                "Emu Bias" option has been split into two separate options:
                                  "System clock", which allows you to underclock the whole system to
                                    improve performance, but breaks sound pitch.
                                  "Sync threshold", which allows to reduce amount of synchronization
                                    between 68k and other chips to also improve performance.
                                Set both to 100 for best compatibility.
                                Added command line option support (see above).
                                PAL/NTSC switch added (must reset game to take effect).
                                Sound and frameskipping code adjusted to work in NTSC mode.
                                Faked sound option fixed to emulate sound without outputting it.
                                Sound breaking problem was hopefully fixed. Framelimiter fixed.
                                Added trace mode support to Cyclone. Some timing hacks added to make
                                Project-X work.
                                GnoStiC submitted a few more optimizations.


The rest
--------

The code I ported it from (a Dreamcast port) is available here:

http://chui.dcemu.co.uk/uae4all.html

0.7.1 source is available here:

http://notaz.gp2x.de/releases/uae4all_gp2x_0.7.1.tar.bz2

Email: criticalhippo@gmail.com

This is GPL'ed software... respect the license.
