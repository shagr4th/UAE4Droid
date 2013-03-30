Patched this to be more lightweight so I can call it from atari800
in a timely fashion

-------------------
CPU & LCD Tuner V.1.1b
======================

I highly uses the code of Hermes/PS2Reality's clockchanger (V.1.01), so about 95% of the CPU-part is his work.

This little program lets you change the CPU-clock and LCD-timing to get higher framerates and reduce the flickering.


Installation:
=============

Just copy the "cpu_speed.gpe" and the "test.bmp" to a folder on your sd-card. The test.bmp has to be in the root-directory of your SD-card. 
Then run from the programm with the game-selector.


Explernation:
=============

CPU:
----
Use the R- and the L-button to higher or lower the CPU-frequency in 25MHz Steps. Use the A- and Y-button for fine-tuning. 
When you press select the program will write the settings and exits. If your GP2X won't respond anymore, just turn off 
and turn on again. You can set the frequency from 33Mhz to 320Mhz in 1MHz Steps.
The specified max-frequency is 266mhz, going there should be safe. 
 
LCD:
----
With the initial settings, the LCD utilizes the same clockgenerator like the cpu-clock. If you change the cpu-clock, you
alter the lcd-timings too, furthermore GPH used a bad value. So this program sets a better value and lets you change this using 
 the volume-buttons.
In FPLL-mode the LCD still utilizes the same clockgenerator like the cpu-clock, but you can alter the prescaler to reduce the 
flickering. Just play around with it a little bit. When your screen becomes corrupted (e.g. when you use a low cpu-clock
a high timing-setting), just press the opposite volume-button several times ... it should get to normal.
In UPLL-mode the LCD gets its own clockgenerator and so the LCD-timing should be constant. You can use the volume-buttons
to finetune the timing. 
The UPLL-mode is the former 1.0.1 mode, so it should only work on units with a 1.0.1 firmware, but people with 1.0.0 firmware can
try this mode ... if it works, I'm wrong :D.

Test-Picture:
-------------
You now have the option to display a testpicture. Press X to display it and any other button but X to return to the menue. 
The test.bmp has to be in the root-directory of your SD-card. Set the resulution to 320x240 and 8 or 16bit (i use 8bit).

Commandline-options:
--------------------
You now can use commandline-parameters. When you set 2 or 3 parameters, the program will run in commandline-mode. In this mode,
no graphical output is given, so you can run it with a GPU-script. A proper GPU looks like this:
---
#/bin/sh
/mnt/sd/cpu_speed.gpe 1 5 230 > /mnt/sd/output.txt
cd /usr/gp2x
exec /usr/gp2x/gp2xmenu
---
The first parameter sets the clock-generator. Choose 0 for FPLL and 1 for UPLL.
The second parameter sets the prescaler for the LCD-timing. Get your prefered values with the graphical-menue. Choose values between 
-20 and 36 for FPLL-mode and -5 and 10 for UPLL-mode.
The third parameter sets the CPU-frequency in MHz. Choose a value between 33MHz and 320MHz.
If you use values which are too high or too low, the program exits without changing anything.
With > you can pipe the output to a file. This only works in commandline-mode. You'll get something like this:
---
set Timing-Prescaler = 5
..set to UPLL-Clockgen...
set LCD-Timing = 0x8E00
Sys.-Freq. = 228556800Hz M85 P1 S0
UCLK-Freq. = 95846400Hz
ACLK-Freq. = 147456000Hz
---

Buttons:
========
X		= display test-picture ... press any other key to return to the menue
Vol+ and Vol-	= Alter the prescaler for the LCD-timing
B		= Toggle clockgenerator for the LCD-display 
L and R		= Change CPU-frequency by +-25MHz
A and Y		= Finetune CPU-frequency
start		= exit program without setting new CPU-values (should be more stable) 
select		= set CPU and exit program


NOTE: Because the "940T-disable-code" from hermes didn't work proberbly, i removed it from the program. Robster also pointed out, that
      this code isn't necessary, because the second CPU allways is held in reset and so it doesn't have a big effect on the power-consumption.

NOTE2: I removed the div-setting, because it caused some problems ... the finetuning-option should be a good replacement. 

NOTE3: You can run 640x480 XviD-Movies with MP3-Sound (48khz) @ 133mhz o_o ... this should save much power.


I take no responsibility for any damage caused by the program.


What's new:
===========

CPU & LCD Tuner V.1.2
---------------------
- added pictureviewer to use test-pictures
- removed div-setting, because it caused some instabilities
- removed hardcoded array for CPU-Clock Settings
- added CPU-clock finetune as replacement for div
- busted some bugs ... but..  	for sure i made new ones :)

CPU & LCD Tuner V.1.1c
----------------------
I messed some things up with the 1.1b version. It seems it was a very unstable one. This should be better.
- commandline and menue don't use the same exit-function any more.

CPU & LCD Tuner V.1.1b
----------------------
- fixed a little issue, where you could not run anything after the tool, when you run a GPU-File.

CPU & LCD Tuner V.1.1
---------------------
- merged to one version
- using commandline-options
- cleaning up the code (indend was really necessary)
- fixed some issues
- added output
- removed "disable_940t()", because it doesn't do anything.

CPU & LCD Tuner v1.0.0 & v1.0.1:
--------------------------------
- other glockgenerator for the LCD (1.0.1 only)
- finetuning for the LCD
- new background-color
- new initial-value and so ... no more flickering

CPU-Speed 1.01c:
----------------
- added more frequenz-settings, no you can go up to 320mhz. From 266mhz on you can go up in 5mhz steps (bzw 4mhz from 266 to 270)

CPU-Speed 1.01b:
----------------
- Correct display and function by using robsters he posted at www.gp32x.com

CPU-Speed 1.01:
---------------
- programmed by Hermes/PS2Reality


To do
======
- remove CPU-dependency in FPLL-mode
- fixe some bugs


Thanks goes to Hermes/PS2Reality for the original-code, robster for great help with the LCD-settings, 
Guyfawkes for his demo-code which helped me with the picture viewer, Vimacs for the cpu-code-editing,
all those guys who tested the program at gp2xdev and all i have forgotten. :)


enjoy 

god_at_hell

PS: Please excuse my maybe bad english ^^.
