<img src="https://github.com/turican0/MagicBalls/blob/wiki/MB-theme.%20test.gif">

# MagicBalls
Rewrite game Magic Carpet 2 with Engine [GODOT](https://godotengine.org/) and REMC2 engine [REMC2](https://github.com/turican0/remc2)

# Usefull links:
[Diskord of projects MDmod/remc2 and MagicBalls](https://discord.com/channels/1005849681995255878/1005849682464997378)
[REMC2](https://github.com/turican0/remc2)
[MC2 HDmod](https://github.com/thobbsinteractive/magic-carpet-2-hd)

# Information for testers:
1) Please be patient, but don’t hesitate to report bugs. The game’s optimization is still poor; I’ll continue to tune some models’ triangle counts and see if I can optimize the lighting and fire effects (especially in Meteor III), but don’t expect miracles :)
2) Document everything in an issue on GitHub. For specific bugs, note the level and location; for particularly random bugs, save the game using F5 before the crash and then attach the zipped folder "user\AppData\Roaming\Godot\app_userdata\MagicBalls\CDdata\GAME\NETHERW\SAVE\"
3) Report everything: bad models, incorrect sizes, wrong music, etc.
4) If you see text with numbers instead of an object, try to take a screenshot
5) Some features from the original game are missing, and the keyboard shortcuts listed in the original help menu don’t work either. Press F1 to see which keys are currently in use.
6) Please also add suggestions for improvements or additions to the original features in the issues.
7) If you’d like to contribute to development—whether you’re a graphic designer or a programmer—please reach out on Discord; any help is welcome.
8) Expect that I’ll be a bit clueless at times and will need to ask for help :)

# How to install the game:
1) The game requires data from the original Magic Carpet™ 2: The Netherworlds. The installation now supports the GOG version https://www.gog.com/en/game/magic_carpet_2_the_netherworlds.
Please install that version. If you have a different version and can’t get it to install, create an issue, take a screenshot of the game folder, and I’ll add more options to the engine.
2) Download and extract the game from [the releases)](https://github.com/turican0/MagicBalls/releases) (currently only the Windows version):
4) Run the .exe; the first time, the game will ask you for the installation path for MC2 (from GOG). Find it and confirm.
5) The first time, you’ll need to wait for the game to extract the files; subsequent launches will be faster.
6) The game saves user files to this folder, which also contains save files, etc. If you want to transfer them between the original game and the remake, or if you no longer need the game, you can delete this folder:
user\AppData\Roaming\Godot\app_userdata\MagicBalls\

---

## What needs to be done to complete the game:
-Add gamepad controls, etc.<br>
-Add first-person/third-person camera options<br>
-Clean UP code and unused models<br>
-Optimalizatins<br>
-Some better 3D models<br>

---

## Version 260405-alfa version - first release:
-There are so many improvements that it doesn't make sense to list them all<br>
-You can play through the game from start to finish<br>

## Version 260302:
-Many fixes and updates<br>
-Draw Spider Web<br>
-Draw Terrain modification by fire<br>
-Add models to level 1-5<br>
-Integrate data copier from Magic Carpet 2<br>
-Better terrain abilites(water/lava)<br>
-Add sounds/music.<br>
-Add menus/levels, etc.<br>
-Add proper water.<br>
-Add particle effects.<br>
[Second youtube video](https://www.youtube.com/watch?v=gSrJQ-UL85s)

## Version 260121:
-Add sounds<br>
-Add music<br>
-Add basic code to copy some data from original CD<br>
[First youtube video](https://youtu.be/SaIr8dyr_Rc)

## Version 260115:
-Draw minimap<br>
-Update UI-particaly<br>
-Add Ballon 3d model<br>
-Implement palette modifications (red or grayscale)<br>
-Add using mouse buttons<br>
-Add select spells<br>
-Add terrain changes<br>
<img src="https://github.com/turican0/MagicBalls/blob/wiki/MB-8.%20test.gif" width="400">

## Version 260108:
A lot has changed in this version. I found that copying terrain in every frame is extremely demanding.<br>
The first tests showed less than 1 FPS.<br>
I had to put most things into the shader. After a series of struggles, I created a shader that:<br>
-handles reflections and water waves<br>
-I now send the height, index, and rotation maps to the shader as textures<br>
Thanks to this, I got up to 3-4 FPS.<br>
Then all I had to do was rewrite the key parts of the code from GDSCRIPT to C++, and I got up to 60 FPS.<br>
### Changes:
-Terrain rendering and updating moved to shader<br>
-Terrain work moved to C++<br>
-Create true model for building possession<br>
-Add multimesh for objects(for corect visibility of objects)<br>
-Fix water reflections<br>
-Fix water waves<br>
-Copy terrain modification<br>

<img src="https://github.com/turican0/MagicBalls/blob/wiki/MB-7.%20test.gif" width="400">

## Version 260104:
### Changes:
-Check spell disappearance-maybe-ok<br>
-Test mouse button<br>
-Add teplate spells fireball and possesion<br>
-Add mana spheres for gold and white<br>
-Test kill and possesion mana from goats<br>

<img src="https://github.com/turican0/MagicBalls/blob/wiki/MB-6.%20test.gif" width="400">

## Version 260102:
### Changes:
-Add additional temporary models<br>
-Fix object display so they are always in front of the camera<br>
-Extend Multimesh terrain to 5x5<br>

<img src="https://github.com/turican0/MagicBalls/blob/wiki/MB-5.%20test.gif" width="400">

## Version 260101:
### Changes:
-Test using first billboards and models(smoke and goat)<br>

<img src="https://github.com/turican0/MagicBalls/blob/wiki/MB-4.%20test.gif" width="400">

## Version 251231:
### Changes:
-display entities (all as universal objects for now)<br>
-synchronize speed with engine (limit frame rate)<br>
-add rotation to REMC2 engine<br>

<img src="https://github.com/turican0/MagicBalls/blob/wiki/MB-3.%20test.gif" width="400">

## Version 251230:
### Changes:
-fix terrain rendering triangles<br>
-add compute position of player by REMC2<br>
-add arrows input by REMC2<br>
-many cleanings in REMC2(copy new engine version)<br>

<img src="https://github.com/turican0/MagicBalls/blob/wiki/MB-second%20test.gif" width="400">

## Version 0:
### Changes:
-first render of terrain<br>

<img src="https://github.com/turican0/MagicBalls/blob/wiki/MB-firste%20test.gif" width="400">
