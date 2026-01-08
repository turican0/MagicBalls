# MagicBalls
Rewrite game Magic Carpet 2 with Engine [GODOT](https://godotengine.org/) and REMC2 engine [REMC2](https://github.com/turican0/remc2)

---

## What needs to be done to complete the game:
-Create 3D models of all entities.<br>
-Add sounds/music.<br>
-Add menus/levels, etc.<br>
-Add proper water.<br>
-Add particle effects.<br>
-Add gamepad controls, etc.<br>
-Add first-person/third-person camera options<br>
-Integrate data copier from Magic Carpet 2<br>
-Better terrain abilites(water/lava)<br>
-Add terrain changes<br>
-Add using mouse buttons<br>
-Add select spells<br>

---

## Next version:
-update UI

## Version 260108:
A lot has changed in this version. I found that copying terrain in every frame is extremely demanding.<br>
The first tests showed less than 1 FPS.<br>
I had to put most things into the shader. After a series of struggles, I created a shader that:<br>
-handles reflections and water waves<br>
-I now send the height, index, and rotation maps to the shader as textures<br>
Thanks to this, I got up to 3-4 FPS.<br>
Then all I had to do was rewrite the key parts of the code from GDSCRIPT to C++, and I got up to 60 FPS.<br>
### Changes:
-terrain rendering and updating moved to shader<br>
-terrain work moved to C++<br>
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
