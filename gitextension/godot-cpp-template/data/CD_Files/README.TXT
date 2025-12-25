
*****************************
MAGIC CARPET 2 - NETHERWORLDS
*****************************




*****************************************************************************
THIS SOFTWARE IS PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND, WHETHER EXPRESS
OR IMPLIED, INCLUDING WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
PARTICULAR PURPOSE WHICH ARE HEREBY DISCLAIMED. IN NO EVENT WILL ELECTRONIC 
ARTS BE LIABLE FOR ANY SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES RESULTING 
FROM POSSESSION, USE, OR MALFUNCTION OF THIS ELECTRONIC ARTS SOFTWARE PRODUCT.

ELECTRONIC ARTS RESERVES THE RIGHT TO MAKE IMPROVEMENTS IN THIS FILE AND TO THE
SOFTWARE AT ANY TIME AND WITHOUT NOTICE.

THIS FILE AND THE SOFTWARE HEREWITH IS COPYRIGHTED.  ALL RIGHTS ARE RESERVED.  
NO PART OF THIS FILE OR THE SOFTWARE MAY BE COPIED, REPRODUCED, TRANSLATED, OR 
REDUCED TO ANY ELECTRONIC MEDIUM OR MACHINE-READABLE FORM WITHOUT THE PRIOR 
WRITTEN CONSENT OF ELECTRONIC ARTS, P.O. BOX 835, SLOUGH, BERKS. SL3 8XU, 

MAGIC CARPET 2 (NETHERWORLDS) IS A TRADEMARK OF BULLFROG PRODUCTIONS LTD.
COPYRIGHT 1995 BULLFROG PRODUCTIONS LTD.
ALL RIGHTS RESERVED.
*****************************************************************************



*********************
HARDWARE REQUIREMENTS
*********************

Magic Carpet 2 requires a 486 66MHz or higher with 8Mb RAM, 
and 8Mb hard disk space, plus additional hard drive space
for any saved games.
	
Recommended setup: Pentium, quad-speed CD-ROM drive. Hi-Res mode requires 16Mb
RAM, VESA compatible driver, SVGA video card & monitor.

Supported: Soundblaster and 100% compatibles, AWE32, WaveBlaster, AdLib, Pro
Audio Spectrum, Roland MT32, General MIDI. Joystick, VFX-1 headset, i-Glasses,
Gravis Gamepad.

Multiplayer mode: 2-8 players, requires NETBIOS & 8Mb+ RAM. Hi-performance
systems recommended. 



*********************
SOUND AND MUSIC SETUP
*********************

To configure Magic Carpet 2 for the sound card in your PC, insert the game CD
in your CD-ROM drive, switch to that drive, and type NWSETUP. 
NOTE: If you wish to play the game with sound and speech, the audio output
from the CD ROM must be internally connected to your sound card.



****************
TO PLAY THE GAME
****************

To play Magic Carpet 2, having configured the sound and music if 
required, change to the CD-ROM drive, and type NETHERW. The intro will load;
followed by the main menu screen, known as the Stone Tablet of Janus.
 


***********************
MAGIC CARPET 2 CONTROLS
***********************


IN ALL BUT INPUT MODES:

   With no control keys:

      P                       Pause toggle (not network)
      R                       Change screen resolution
      ESC                     Abort mission
      Return                  Toggle between game screen and mission map
      O                       Show current objective
      D                       Options panel
      M                       Adjust music volume
      V                       Adjust sound volume
      1-8                     Send respective message
      F1                      Toggle in-game help on/off
      F2                      Toggle flight assistance on/off
      F3                      Toggle process speed fast/normal
      F4                      Toggle speech on/off
      F5                      Toggle sound on / off
      F6                      Toggle music on / off
      F7                      Adjust palette
      F8                      Toggle player names on / off
      LeftButton              Fire left selected spell
      RightButton             Fire right selected spell
      Up Cursor               Increase speed
      Down Cursor             Decrease speed
      Left Cursor             Move left
      Right Cursor            Move right
      Left & Right Cursor     Barrel Roll
      Backspace               Stop moving


   With Shift:

      L                       Destroy last castle stage
      F1                      Toggle reflections on / off
      F2                      Toggle sky on / off
      F3                      Toggle shadows on / off
      F4                      Toggle light sources on / off
      F5                      Toggle map and icons on / off
      F6                      Toggle panel transparency on / off
      F7                      Toggle flat shading on / off
      LeftButton              Toggle left selected spell through 
                              left marked spells forwards
      RightButton             Toggle right selected spell through 
                              right marked spells forwards


   With Alt:

      S                       Toggle stereo mode on/off
      Left Mouse Button       Toggle left selected spell through 
                              left marked spells backwards
      Right Mouse Button      Toggle right selected spell through 
                              right marked spells backwards



IN GAME MODE:

   With no control keys:

      [                       Increase screen size
      ]                       Decrease screen size
      I                       Start text input
      C                       Calibrate head set
      Control                 Enter spell menu mode



IN SPELL MENU MODE:

   With Shift:

      Left Mouse Button       Toggle left marked spell on/off
      Right Mouse Button      Toggle right marked spell on/off

   With no control keys

      Left Mouse Button       Selected left selected spell, hold 
                              button down to select spell level.
      Right Mouse Button      Selected right selected spell, hold 
                              button down to select spell level.
      Left & Right Buttons    Fire spell without selecting.
     



IN MAP SCREEN MODE:

   	  Alt                     Display player information.



IN TEXT ENTRY MODE:

      Shift F1                Use message store 1
      Shift F2                Use message store 2
      Shift F3                Use message store 3
      Shift F4                Use message store 4
      Shift F5                Use message store 5
      Shift F6                Use message store 6
      Shift F7                Use message store 7
      Shift F8                Use message store 8
      F9                      Toggle message target mode
      Return                  Accept message
      ESC                     Abort message
      Other keys typed as text



IN OPTION PANEL MODE:

      Left Mouse Button       Toggle option forwards
      Right Mouse Button      Toggle option backwards


IN FRONT END SCREENS:

      F1                      Toggle help mode


IN FRONT END MAP SCREEN:

      O                       Show level text




********************
COMMAND LINE OPTIONS
********************

From the command line, type NETHERW plus one of the following options for
advanced options:

    -harddrive [drive_letter]      Allows user to define the drive name on
 	                               which to store all game local drive data.
    -music2                        Use alternative (Magic Carpet 1) music.
    -vio [baudrate] [com. port]    Defines parameters for use with VIO headset.
	-vfx1                          Enables use of VFX headset.
	-detectoff                     Disable auto-detection of sound hardware.




************************************
MAGIC CARPET 2 ON NON C: LOCAL DRIVE
************************************

If your hard drive is anything other than C:, Magic Carpet 2 must be run
with the following command:

        NETHERW -HARDDRIVE [drive_letter]

where [drive_letter] represents the single character defining the local
hard drive. If you wish to play the game with sound and music, make a 
copy of the batch file NWSETUP.BAT, and edit it such that the sound
files are copied from the Magic Carpet 2 CD SOUND directory, to your
local drive \NETHERW\SOUND directory, and that the SETSOUND.EXE is
called with the -HARDDRIVE parameter, eg:

        SETSOUND -HARDDRIVE D 




*************************
CALIBRATING YOUR JOYSTICK
*************************

When configuring the game for use with an analogue joystick, having clicked on
the joystick icon in the main menu screen, and selected the analogue joystick
option, you will be prompted to circle the joystick. When prompted, you must
rotate the joystick once, and then click button 1. This will calibrate the
joystick.



***********
JOYPAD PLAY
***********

If you intend to play Magic Carpet 2 with a joypad such as the Gravis Gamepad,
select the Joystick option from the Stone Tablet, and when the scroll opens
left-click the digital option. Confirm your choice by left-clicking the tick
icon.

The Gamepad's control buttons work as follows:

D-Pad: Control the movement of the carpet

Top button (blue): Fire right selected spell

Left button (red): Fire left selected spell

Right button (green): Open/close Spells Menu

Bottom button (yellow): Press and hold, and use in conjunction with the D-Pad
to move your carpet forwards or backwards, and to dodge incoming spells.



*****************************
CONFIGURING KEYBOARD CONTROLS
*****************************

At the Stone Tablet (see the Playguide) left-click the Configure Keyboard icon
for the Keyboard Configuration screen. Here you can customise carpet control,
menu access and spell casting controls. 

To configure keyboard controls, simply highlight the box on the left of the
control you wish to alter and left-click. Now press your preferred key; the
current key on the right side of the control option changes to reflect this. 

When your keyboard configuration changes are complete, left-click the tick icon
at the bottom of the screen to return to the Stone tablet. 



*****************
MULTIPLAYER GAMES
*****************


In multiplayer games, all the competitors must run their own copy of Magic
Carpet 2 and choose the network option independently from their own PC. The
network should support NETBIOS, which you must run from the command line before
you enter the game.
 
Note: For information on configuring your system for network play, consult your
NETBIOS¸ documentation or network manager. 

At the Stone Tablet, left-click the Multiplayer Game icon. Enter the selected
network session number (0-9); each player in your network game must enter the
same session number. Now left-click the tick icon; the Network Configuration
screen appears.



SELECTING A WIZARD

In the centre of the Network Configuration screen is your currently selected
wizard. To change the wizard, use the mouse to click on the obelisks either
side of the screen. As you scroll through the available wizards, the colour
above the door changes. Other colours appear in the seven slots above the
doorway, representing the other players in your network game.

The wizard colours are as follows:
You - White
Nyphur - Red
Rahn - Green
Jark - Purple
Belix - Blue
Elyssia - Pink
Yragore - Black
Prish - Orange



SELECTING A LEVEL

Network games take place on single levels, rather than following the Magic
Carpet 2 narrative and progressing through the levels. To select the
particular Netherworld you wish to play, left-click the switches at the top of
the screen and then left-click the gargoyle icon on the right to confirm
selection and enter the level. 

To cancel the network session at any time, left-click the gargoyle icon on the
left of the Network Configuration screen; you're returned to the Stone Tablet.
 


SENDING MESSAGES DURING NETWORK PLAY

I        - Start Text Entry mode
Shift-F1 - Use message store 1
Shift-F2 - Use message store 2
Shift-F3 - Use message store 3
Shift-F4 - Use message store 4
Shift-F5 - Use message store 5
Shift-F6 - Use message store 6
Shift-F7 - Use message store 7
Shift-F8 - Use message store 8
F9       - Toggle message target mode
Control  - Accept message
Esc      - Abort message

All other keys enter text



******************
THE ALL-SEEING EYE
******************

Here's the definitive key to the All-Seeing Eye:

Marker stone                   - Flashing blue dot
Creature (night/cavern levels) - White dot
Creature (day levels only)     - Black dot
Teleport                       - Large pink square
Fly to point/a sign            - Flashing yellow square
Unpossessed mana               - Yellow dot
Your possessed mana            - Flashing white dot
Exit point                     - Red X
Spell                          - Red dot
Objective creature (Night and Cavern levels) - White dot in flashing red square
Objective creature (Day levels only)         - Black dot in flashing red square
Experience spell               - Flashing red and white dot
Tree (night/day levels)        - Green dot
Mushroom (cavern levels only)  - Green dot
Unpossessed building           - Pink dot
Possessed building             - Coloured flashing dot
Civilian                       - Blue dot
White balloon                  - Your balloon
Coloured balloon               - Enemy balloon
Direction to current objective - Flashing red arrow



**********
PAUSE MENU
**********

Press P during a mission for the Pause menu. This has five options; to activate
any of them, left-click the appropriate icon. The Pause menu options are: 

Save level: This option allows you to preserve an incomplete mission, and can
be very useful if you're about to fly into real trouble. Left-click the tick or
X icon to confirm your decision yes or no; any previous level save is
immediately overwritten.

Load level: Okay, so the ghastly minions of Vissuluth creamed you. But if you
had the good sense to save the level before a terminal encounter, you can
resume it immediately using this option. Left-click the tick or X icon to
confirm your decision yes or no. 

Sound volume: With this option active, press and hold the left mouse button and
move the mouse left or right to adjust sound volume up or down.

Music volume: With this option active, press and hold the left mouse button and
move the mouse left or right to adjust music volume up or down.

Options: Left-click this icon for the Game Configuration menu. Use the mouse to
highlight the desired option(s) and left-click to activate an option or increase
its value. Right-click to de-activate or decrease the setting. When you alter a
Game Configuration option, the new setting appears above the menu. To return to
the Pause Menu at any time, left-click OK. 




***************************************
CREATING A BOOT DISK FOR MAGIC CARPET 2
***************************************

If you are having trouble installing or running this software, experiencing
lockups, or other problems, we suggest you try starting up your system with a
DOS boot disk. This will ensure that you have the required amount of memory
available and creates the system environment in which the game was designed to
run.

Here are the steps for creating a DOS boot disk. Please follow these exactly.
IMPORTANT: To create a DOS boot disk you need a blank disk the same size as
your A: drive.

1. Type C: and press Enter
2. Insert a blank disk into drive A:.
3. Type FORMAT A:/S and press Enter
4. You are prompted to insert a blank disk into drive A:. DO so if you havenåt
already, and press Enter.
5. Once the disk has finished being formatted, you are asked to label (name)
the disk. Type in a label or press Enter for no label.
6. You are now asked whether you wish to format another disk. Type N and press
Enter.
7. You must now create a config.sys file on your boot disk by typing the
following at the C: prompt:
EDIT A:\CONFIG.SYS and press Enter

When the blue edit screen appears, type in:

DEVICE=C:\DOS\HIMEM.SYS
DOS=HIGH
FILES=50
BUFFERS=30
DEVICE=C:\(PATH TO YOUR CD-ROM DEVICE DRIVER)

Add your cd-rom device driver to the previous line just as it appears in the
config.sys file on the root directory of your hard-drive. To view this file
type the following from the Dos prompt:

TYPE C:\CONFIG.SYS and press ENTER. 

When the file is displayed, look for the line that relates to your CD-Rom
drive, and write it down on a piece of paper. For example, the usual line
for a Panasonic CD-drive connected to a SoundBlaster 16 would be;
device=c:\sb16\drv\sbcd.sys /d:mscd001 /p:220

8. Exit and save this file by pressing

ALT-F
X
Y

9. You will also need an AUTOEXEC.BAT file on your boot disk. To create one,
type:

EDIT A:\AUTOEXEC.BAT and press Enter.

When the new screen appears, type:

PROMPT $P$G
PATH=C:\DOS
C:\(path)\MSCDEX (all parameters as they appear in the AUTOEXEC.BAT) on drive
c: 

Add your MSCDEX to the previous line just as it appears in the autoexec.bat
file on the root directory of your hard-drive. To view this file type the
following from the Dos prompt;

TYPE C:\AUTOEXEC.BAT and press Enter.

When the file is displayed, look for the line that contains MSCDEX, and write
it down on a piece of paper. For example, the usual line for a Panasonic
CD-drive connected to a SoundBlaster 16 would be;

device=c:\dos\MSCDEX /d:mscd001 /v /m:15

NOTE: If your system has MS-DOS version 6.2 or higher and 8Mb of RAM, you can
use the Smartdrive disk caching program to increase the performance of your
CD drive, and help Magic Carpet 2 run as smoothly as possible. Add the
following line under the MSCDEX line in the AUTOEXEC.BAT.

C:\DOS\SMARTDRV.EXE 2048

or

C:\MOUSE\MOUSE.COM (or the path to your mouse driver as described in the
AUTOEXEC.BAT on the C: drive)

10. Exit and save this file by pressing:

ALT-F
X
Y

IMPORTANT: Remember to boot your PC from the DOS boot disk whenever you install
the game, and every time you play Magic Carpet 2 (see below).

To start your machine using the DOS boot disk:
1. Insert the DOS boot disk into drive A: then restart your machine. The
computer boots up to the A:> prompt. 
2. Type C: and press Enter to return to your hard drive.
For more information consult your DOS users manual. 
Video Card Compatibility (16Mb of RAM only)
Magic Carpet 2's high resolution graphics require both a VESA compatible SVGA
video card and VESA Super VGA BIOS Extension installed before you start the
game. VESA stands for Video Electronics Standards Association. This standard
allows the graphics to be displayed on any video card that is VESA compatible
without the need for the game's programmers to know precisely how each
individual card works. 
If when attempting to run the game in high-res mode you get either a blank
screen or garbled graphics, your video card probably needs a VESA driver
loaded prior to playing. Consult the documentation and software that came wit
your video card on the loading of the driver, or contact your video card
manufacturer.
