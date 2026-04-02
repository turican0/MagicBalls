extends Node3D

@export_file("*.tab") var levels_tab_path: String = ""
@export_file("*.dat") var levels_dat_path: String = ""

var game_data_path: String = ""
var cd_data_path: String = ""

#var speed: float = 0
var direction2: Vector3 = Vector3(1,0,0)

var Main_Player: Node
var Main_UI: Node
var Main_Filter: Node

var fadeNode: Node3D

var NodeSky3D: WorldEnvironment

const KEY_INDEX := {
	# Systémové a funkční
	KEY_ESCAPE: 0x011b,
	KEY_BACKSPACE: 0x0E08,
	KEY_TAB: 0x0F09,
	KEY_ENTER: 0x1c0d,
	KEY_CTRL: 0x1d00,
	KEY_SHIFT: 0x2a00, # Namapováno na Left Shift z tvého seznamu
	KEY_ALT: 0x3800,
	KEY_SPACE: 0x3920,

	# Čísla (horní řada)
	KEY_1: 0x0231,
	KEY_2: 0x0332,
	KEY_3: 0x0433,
	KEY_4: 0x0534,
	KEY_5: 0x0635,
	KEY_6: 0x0736,
	KEY_7: 0x0837,
	KEY_8: 0x0938,
	KEY_9: 0x0a39,
	KEY_0: 0x0b30,

	# Písmena - Horní řada
	KEY_Q: 0x1071,
	KEY_W: 0x1177,
	KEY_E: 0x1265,
	KEY_R: 0x1372,
	KEY_T: 0x1474,
	KEY_Y: 0x1579,
	KEY_U: 0x1675,
	KEY_I: 0x1769,
	KEY_O: 0x186f,
	KEY_P: 0x1970,

	# Písmena - Prostřední řada
	KEY_A: 0x1e61,
	KEY_S: 0x1f73,
	KEY_D: 0x2064,
	KEY_F: 0x2166,
	KEY_G: 0x2267,
	KEY_H: 0x2368,
	KEY_J: 0x246a,
	KEY_K: 0x256b,
	KEY_L: 0x266c,

	# Písmena - Dolní řada
	KEY_Z: 0x2c7a,
	KEY_X: 0x2d78,
	KEY_C: 0x2e63,
	KEY_V: 0x2f76,
	KEY_B: 0x3062,
	KEY_N: 0x316e,
	KEY_M: 0x326d,

	# Speciální znaky
	KEY_MINUS: 0x0c2d,
	KEY_EQUAL: 0x0d3d,
	KEY_BRACELEFT: 0x1a5b,   # [
	KEY_BRACERIGHT: 0x1b5d,  # ]
	KEY_SEMICOLON: 0x273b,
	KEY_APOSTROPHE: 0x2827,
	KEY_QUOTELEFT: 0x2960,   # `
	KEY_BACKSLASH: 0x2b5c,
	KEY_COMMA: 0x332c,
	KEY_PERIOD: 0x342e,
	KEY_SLASH: 0x352f,

	# Funkční klávesy
	KEY_F1: 0x3b00,
	KEY_F2: 0x3c00,
	KEY_F3: 0x3d00,
	KEY_F4: 0x3e00,
	KEY_F5: 0x3f00,
	KEY_F6: 0x4000,
	KEY_F7: 0x4100,
	KEY_F8: 0x4200,
	KEY_F9: 0x4300,
	KEY_F10: 0x4400,

	# Pohyb a navigace
	KEY_HOME: 0x4700,
	KEY_UP: 0x4800,
	KEY_PAGEUP: 0x4900,
	KEY_LEFT: 0x4b00,
	KEY_RIGHT: 0x4d00,
	KEY_END: 0x4f00,
	KEY_DOWN: 0x5000,
	KEY_PAGEDOWN: 0x5000, # V tvém kódu má DOWN i PAGEDOWN stejné 0x5000
	KEY_INSERT: 0x5200,
	KEY_DELETE: 0x5300
}

const MOUSE_BUTTON_INDEX := {
	MOUSE_BUTTON_LEFT: 0,
	MOUSE_BUTTON_RIGHT: 1,
	MOUSE_BUTTON_MIDDLE: 2,
	MOUSE_BUTTON_WHEEL_UP: 3,
	MOUSE_BUTTON_WHEEL_DOWN: 4
}

var input_state: Dictionary = {
	"keys": {},
	"mouse_pos": Vector2.ZERO
}

# Akumulovaný posun myši od startu
#var total_mouse_delta := Vector2.ZERO

#var node_pool = []
var pool_size = 1000
# Katalog cest k tscn souborům podle ID typu
var library_scenes = {}
var library2_scenes = {}

var library = {
	Vector3i(0,999,0): "res://entites/object_text.tscn",
	Vector3i(2,75,0): "res://entites/object_2_75_tree.tscn",#tree -difColors!!!
	Vector3i(2,78,0): "res://entites/object_2_78_statue.tscn",#statue -difmodels!!!
	Vector3i(2,79,0): "res://entites/object_2_79_dolmen.tscn",#dolmen
	Vector3i(2,87,0): "res://entites/object_2_87_tree.tscn",#tree2
	Vector3i(2,178,0): "res://entites/object_2_178_burned_tree.tscn",#burned tree
	Vector3i(2,179,0): "res://entites/object_2_179_burned_tree.tscn",#burned tree
	Vector3i(2,198,0): "res://entites/object_2_78_statue.tscn",#statue2 - level2 - same models
	Vector3i(2,422,0): "res://entites/object_2_422_barell.tscn",#barell
	Vector3i(2,423,0): "res://entites/object_2_423_basket.tscn",#basket - yyyyyyyyyyyyyyyyyyyyyyyyy
	Vector3i(2,424,0): "res://entites/object_2_424_mushroom1.tscn",#mushroom1 - zzzzzzzzzzzzzzzzzzzzzzzzzz
	Vector3i(2,425,0): "res://entites/object_2_425_mushroom2.tscn",#mushroom1 - zzzzzzzzzzzzzzzzzzzzzzzzzz
	Vector3i(3,0,0): "",#player1-Zanzamar
	Vector3i(3,88,0): "res://entites/object_3_88_ballon.tscn",#ballon -difColors!!!
	Vector3i(3,89,0): "res://entites/object_3_89_ballon.tscn",#ballon -difColors!!!
	Vector3i(3,90,0): "res://entites/object_3_90_ballon.tscn",#ballon -difColors!!!
	Vector3i(3,91,0): "res://entites/object_3_91_ballon.tscn",#ballon -difColors!!!
	Vector3i(3,92,0): "res://entites/object_3_92_ballon.tscn",#ballon -difColors!!!
	Vector3i(3,93,0): "res://entites/object_3_93_ballon.tscn",#ballon -difColors!!!
	Vector3i(3,94,0): "res://entites/object_3_94_ballon.tscn",#ballon -difColors!!!
	Vector3i(3,95,0): "res://entites/object_3_95_ballon.tscn",#ballon -difColors!!!
	Vector3i(3,96,0): "res://entites/object_3_96_posses_building-whiteD.tscn",#castle -difmodels!!!
	Vector3i(3,97,0): "res://entites/object_3_97_posses_building-redD.tscn",#castle -difmodels!!!
	Vector3i(3,98,0): "res://entites/object_3_98_posses_building-violetD.tscn",#castle -difmodels!!!
	Vector3i(3,99,0): "res://entites/object_3_99_posses_building-blueD.tscn",#castle -difmodels!!!
	Vector3i(3,100,0): "res://entites/object_3_100_posses_building-greenD.tscn",#castle -difmodels!!!
	Vector3i(3,101,0): "res://entites/object_3_101_posses_building-pinkD.tscn",#castle -difmodels!!!
	Vector3i(3,102,0): "res://entites/object_3_102_posses_building-orangeD.tscn",#castle -difmodels!!!
	Vector3i(3,103,0): "res://entites/object_3_103_posses_building-blackD.tscn",#castle -difmodels!!!
	Vector3i(3,203,0): "res://entites/object_3_203_sorcerer-blue.tscn",
	Vector3i(3,211,0): "res://entites/object_3_211_sorcerer.tscn",#sorcerer-red xxxxxxxxxxxxxxxxxxxx
	Vector3i(3,219,0): "res://entites/object_3_219_sorcerer-black.tscn",#sorcerer-black xxxxxxxxxxxxxxxxxxxx
	Vector3i(3,235,0): "res://entites/object_3_235_sorcerer-pink.tscn",#sorcerer-pink xxxxxxxxxxxxxxxxxxxx
	Vector3i(3,243,0): "res://entites/object_3_243_sorcerer-violet.tscn",#sorcerer-violet xxxxxxxxxxxxxxxxxxxx
	Vector3i(3,251,0): "res://entites/object_3_251_sorcerer-green.tscn",#sorcerer-green xxxxxxxxxxxxxxxxxxxx
	Vector3i(5,8,0): "res://entites/object_5_8_bowman.tscn",#bowman
	Vector3i(5,9,0): "res://entites/object_5_8_bowman.tscn",#bowman
	Vector3i(5,10,0): "res://entites/object_5_8_bowman.tscn",#bowman
	Vector3i(5,11,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	Vector3i(5,12,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	Vector3i(5,13,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	Vector3i(5,14,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow -difmodels!!!
	Vector3i(5,15,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow -difmodels!!!
	Vector3i(5,16,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow -difmodels!!!
	Vector3i(5,17,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow -difmodels!!!
	Vector3i(5,18,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow -difmodels!!!
	Vector3i(5,19,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow -difmodels!!!
	Vector3i(5,20,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow -difmodels!!!
	Vector3i(5,21,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow -difmodels!!!
	Vector3i(5,22,0): "res://entites/object_5_26_flyingCentipedeHead.tscn",#xxxxxxxxxxxxxxxxxxx
	Vector3i(5,23,0): "res://entites/object_5_26_flyingCentipedeHead.tscn",#xxxxxxxxxxxxxxxxxxx
	Vector3i(5,24,0): "res://entites/object_5_26_flyingCentipedeHead.tscn",#xxxxxxxxxxxxxxxxxxx
	Vector3i(5,25,0): "res://entites/object_5_26_flyingCentipedeHead.tscn",#xxxxxxxxxxxxxxxxxxx
	Vector3i(5,26,0): "res://entites/object_5_26_flyingCentipedeHead.tscn",#xxxxxxxxxxxxxxxxxxx
	Vector3i(5,27,0): "res://entites/object_5_26_flyingCentipedeHead.tscn",#xxxxxxxxxxxxxxxxxxx
	Vector3i(5,28,0): "res://entites/object_5_26_flyingCentipedeHead.tscn",#xxxxxxxxxxxxxxxxxxx
	Vector3i(5,29,0): "res://entites/object_5_26_flyingCentipedeHead.tscn",#xxxxxxxxxxxxxxxxxxx
	Vector3i(5,30,0): "res://entites/object_5_30_centipedeHead.tscn",#centipede-head-OK
	Vector3i(5,56,0): "res://entites/object_5_56_flyingCentipedeBody.tscn",#xxxxxxxxxxxxxxxxxxxxxx
	Vector3i(5,55,0): "res://entites/object_9_55_fireball.tscn",#fireball-OK-make as star-special
	Vector3i(5,58,0): "res://entites/object_10_58_goldSphere.tscn",#fliing goldMana -OK
	Vector3i(5,66,0): "res://entites/object_5_66_centipedeBody.tscn",#centipede-body-OK
	Vector3i(5,67,0): "res://entites/object_10_67_whiteSphere.tscn",
	Vector3i(5,68,0): "res://entites/object_10_68_redSphere.tscn",
	Vector3i(5,69,0): "res://entites/object_10_69_violedSphere.tscn",#violet sphere xxxxxxxxxxxx
	Vector3i(5,70,0): "res://entites/object_10_70_blueSphere.tscn",#blue sphere xxxxxxxxxxxx
	Vector3i(5,71,0): "res://entites/object_10_71_greenSphere.tscn",#green sphere xxxxxxxxxxxx
	Vector3i(5,72,0): "res://entites/object_10_72_pinkSphere.tscn",#pink sphere xxxxxxxxxxxx
	Vector3i(5,73,0): "res://entites/object_10_73_orangeSphere.tscn",#orange sphere xxxxxxxxxxxx
	Vector3i(5,74,0): "res://entites/object_10_74_blackSphere.tscn",#black sphere xxxxxxxxxxxx
	Vector3i(5,110,0): "res://entites/object_5_110_vampire_bowman.tscn",#vampire archer
	Vector3i(5,111,0): "res://entites/object_5_110_vampire_bowman.tscn",#vampire archer
	Vector3i(5,112,0): "res://entites/object_5_110_vampire_bowman.tscn",#vampire archer
	Vector3i(5,113,0): "res://entites/object_5_110_vampire_bowman.tscn",#vampire archer
	Vector3i(5,114,0): "res://entites/object_5_110_vampire_bowman.tscn",#vampire archer
	Vector3i(5,121,0): "res://entites/object_5_121_bowman.tscn",#bowman-crouch-OK
	Vector3i(5,122,0): "res://entites/object_5_121_bowman.tscn",#bowman-crouch-OK
	Vector3i(5,123,0): "res://entites/object_5_121_bowman.tscn",#bowman-crouch-OK
	Vector3i(5,124,0): "res://entites/object_5_124_dragon.tscn",#dragon
	Vector3i(5,132,0): "res://entites/object_5_132_people5.tscn",#people5 132 133 134-OK
	Vector3i(5,135,0): "res://entites/object_5_135_people6.tscn",#people6 135 136 137-OK
	Vector3i(5,138,0): "res://entites/object_5_138_people7.tscn",#people7 138 139 140-OK
	Vector3i(5,141,0): "res://entites/object_5_141_people8.tscn",#people8 141 142 143-OK
	Vector3i(5,152,0): "res://entites/object_5_152_goat.tscn",#goat-OK
	Vector3i(5,155,0): "res://entites/object_5_155_people1.tscn",#people1 155 156 157-OK
	Vector3i(5,158,0): "res://entites/object_5_158_puerla.tscn",#puerla xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	Vector3i(5,180,0): "res://entites/object_5_180_people2.tscn",#people2 180 181 182-OK
	Vector3i(5,183,0): "res://entites/object_5_183_people3.tscn",#people3 183 184 185-OK
	
	Vector3i(5,271,0): "res://entites/object_5_271_stoneHead.tscn",#stone head
	Vector3i(5,295,0): "res://entites/object_5_295_crabBaloon.tscn",#crab baloon
	Vector3i(5,303,0): "res://entites/object_5_303_manticore.tscn",# 5-303-gryf/manticore
	
	Vector3i(5,328,0): "res://entites/object_5_328_hopper.tscn",#328-335	levá noha pokrčena v koleni, nahoře	ctvrta
	Vector3i(5,336,0): "res://entites/object_5_336_hopper.tscn",#336-343	pravá noha mírně za levou, obě na zemi	pata
	Vector3i(5,344,0): "res://entites/object_5_344_hopper.tscn",#344-351	pravá noha mírně za levou, obě na zemi, tělo mírně přikrčené	sesta
	Vector3i(5,352,0): "res://entites/object_5_352_hopper.tscn",#352-359	pravá noha mírně za levou, obě na zemi, tělo více přikrčené	první
	Vector3i(5,360,0): "res://entites/object_5_360_hopper.tscn",#360-367	levá noha před pravou v nákroku, obě na zemi	druha
	Vector3i(5,368,0): "res://entites/object_5_368_hopper.tscn",#368-375	levá noha pokrčena v koleni, nahoře, pravá téměř natažena	treti
	Vector3i(5,376,0): "res://entites/object_5_376_hopper.tscn",#376-383	nohy kousek od sebe stojací postoj	
	Vector3i(5,384,0): "res://entites/object_5_384_hopper.tscn",#376-383	torso
	Vector3i(5,392,0): "res://entites/object_5_392_minibasket.tscn",#mini
	Vector3i(5,393,0): "res://entites/object_5_393_minibasket.tscn",#mini-torso
	Vector3i(5,199,0): "res://entites/object_5_199_people4.tscn",#people4 199 200 201-OK
	Vector3i(5,263,0): "res://entites/object_5_263_darklion.tscn",#lion zzzzzzzzzzzz
	Vector3i(5,279,0): "res://entites/object_5_279_beetle.tscn",#beetle-OK
	Vector3i(5,287,0): "res://entites/object_5_287_spider.tscn",#spider zzzzzzzzzzzz
	
	Vector3i(5,311,0): "res://entites/object_5_311_water_beast.tscn",#water beast
	Vector3i(5,319,0): "res://entites/object_5_319_water_beast.tscn",#water beast torso
	
	Vector3i(5,411,0): "res://entites/object_5_411_zombie.tscn",#zombie
	
	Vector3i(5,437,0): "res://entites/object_5_437_mummy.tscn",#mummy-walk
	Vector3i(5,445,0): "res://entites/object_5_445_mummy.tscn",#mummy-stay
	
	Vector3i(9,55,0): "res://entites/object_9_55_fireball.tscn",#fireball-OK-make as star
	Vector3i(9,60,0): "res://entites/object_9_60_green-spell.tscn",#green-spell
	Vector3i(9,61,0): "res://entites/object_9_61_castleball.tscn",#castleball-OK
	Vector3i(9,64,0): "res://entites/object_9_64_meteor.tscn",#meteor-OK
	Vector3i(9,80,0): "res://entites/object_10_80_castle_orb.tscn",#fire xxxxxxxxxxxxxxxxxxxxxxxxxx
	Vector3i(9,105,0): "res://entites/object_9_105_arrow.tscn",#arrow-OK
	Vector3i(9,116,0): "res://entites/object_9_116_firearrow.tscn",#arrow xxxxxxxxxxxxxxxxx
	Vector3i(9,144,0): "res://entites/object_9_144_posses.tscn",#posses-OK - more star
	Vector3i(9,151,0): "res://entites/object_9_151_lighting.tscn",#fix this to better
	Vector3i(9,419,0): "res://entites/object_9_419_spider_web.tscn",#spide web zzzzzzzzzzzzzzOK
	Vector3i(9,420,0): "res://entites/object_9_420_walnut.tscn",#walnul ball zzzzzzzzzzzzzzOK
	Vector3i(9,145,0): "res://entites/object_9_146_lump.tscn",#tremor ball zzzzzzzzzzzzzzOK
	Vector3i(9,146,0): "res://entites/object_9_146_lump.tscn",#crater ball zzzzzzzzzzzzzzOK
	Vector3i(9,463,0): "res://entites/object_9_463_sunball.tscn",#sunball-OK
	Vector3i(10,8,0): "res://entites/object_10_8_fair.tscn",#fair-fake number 8 not true index of model xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	Vector3i(10,38,0): "res://entites/object_10_8_fair.tscn",#end-of explosion-OK
	Vector3i(10,54,0): "res://entites/object_10_54_explosion.tscn",#explosion-final-OK
	Vector3i(10,55,0): "res://entites/object_9_55_fireball.tscn",#fireball-OK-make as star-special
	Vector3i(10,57,0): "res://entites/object_10_57_smoke.tscn",#smoke1-OK
	Vector3i(10,58,0): "res://entites/object_10_58_goldSphere.tscn",#goldMana -OK-standartMana
	Vector3i(10,63,0): "res://entites/object_10_63_smoke.tscn",#smoke2 -OK
	Vector3i(10,67,0): "res://entites/object_10_67_whiteSphere.tscn",#whiteMana -OK
	Vector3i(10,68,0): "res://entites/object_10_68_redSphere.tscn",#red sphere xxxxxxxxxxxx
	Vector3i(10,69,0): "res://entites/object_10_69_violedSphere.tscn",#violet sphere xxxxxxxxxxxx
	Vector3i(10,70,0): "res://entites/object_10_70_blueSphere.tscn",#blue sphere xxxxxxxxxxxx
	Vector3i(10,71,0): "res://entites/object_10_71_greenSphere.tscn",#green sphere xxxxxxxxxxxx
	Vector3i(10,72,0): "res://entites/object_10_72_pinkSphere.tscn",#pink sphere xxxxxxxxxxxx
	Vector3i(10,73,0): "res://entites/object_10_73_orangeSphere.tscn",#orange sphere xxxxxxxxxxxx
	Vector3i(10,74,0): "res://entites/object_10_74_blackSphere.tscn",#black sphere xxxxxxxxxxxx	
	Vector3i(10,77,0): "res://entites/object_10_77_fire.tscn",#fire xxxxxxxxxxxxxxxxxxxxxxxxxx
	Vector3i(10,80,0): "res://entites/object_10_80_castle_orb.tscn",#fire xxxxxxxxxxxxxxxxxxxxxxxxxx
	Vector3i(10,81,0): "res://entites/object_10_81_remains.tscn",#remains xxxxxxxxxxxxxxxxxxxxxxx
	Vector3i(10,96,0): "res://entites/object_3_96_posses_building-whiteD.tscn",#building -difmodels!!!
	Vector3i(10,97,0): "res://entites/object_3_97_posses_building-redD.tscn",#building -difmodels!!!
	Vector3i(10,98,0): "res://entites/object_3_98_posses_building-violetD.tscn",#building -difmodels!!!
	Vector3i(10,99,0): "res://entites/object_3_99_posses_building-blueD.tscn",#building -difmodels!!!
	Vector3i(10,100,0): "res://entites/object_3_100_posses_building-greenD.tscn",#building -difmodels!!!
	Vector3i(10,101,0): "res://entites/object_3_101_posses_building-pinkD.tscn",#building -difmodels!!!
	Vector3i(10,102,0): "res://entites/object_3_102_posses_building-orangeD.tscn",#building -difmodels!!!
	Vector3i(10,103,0): "res://entites/object_3_103_posses_building-blackD.tscn",#building -difmodels!!!
	Vector3i(10,145,0): "res://entites/object_9_64_meteor.tscn",#meteor
	Vector3i(10,168,0): "res://entites/object_10_168_FacePortal.tscn",#fire xxxxxxxxxxxxxxxxxxxxxxxxxx
	Vector3i(10,186,0): "res://entites/object_10_186_splash.tscn",#splash -difmodels!!! - in cave buble
	Vector3i(10,202,0): "res://entites/object_10_202_blue_orb.tscn",#blue orb
	Vector3i(10,327,0): "res://entites/object_10_327_tornado.tscn",#tornado xxxxxxxxxxxxxxxxxxxxxxxx
	Vector3i(10,426,0): "res://entites/object_10_426_bubble.tscn",#bubble - zzzzzzzzzzzzzzzzzzzzzzzzzz
	Vector3i(10,463,0): "res://entites/object_10_463_fireball-object.tscn",#fireball-object xxxxxxxxxxxxxxxxxxxxxxxxxx
	Vector3i(14,259,0): "res://entites/object_14_259_scroll.tscn",#scroll -OK
	Vector3i(14,461,0): "res://entites/object_14_461_mouth.tscn",#mouth-gate -OK
	Vector3i(14,462,0): "res://entites/object_14_462_portal.tscn",#day portal
	Vector3i(15,59,0): "res://entites/object_15_59_jar.tscn",#jar -OK
}
#2 424 - mushroom1
#5 287 - spider
#9 419 - spider miniweb - 
func updateLibrary(a:int,b:int,c:int,path:String):
	library[Vector3i(a, b, c)] = path
	library_scenes[Vector3i(a, b, c)] = load(path)

var library2 = {
	Vector3i(0,999,0): "res://entites/object_text.tscn",
	Vector3i(0,0,0): "",#unknown
	Vector3i(3,0,0): "",#unknown
	Vector3i(3,88,0): "",#unknown
	Vector3i(3,203,0): "",#sorcerer-blue xxxxxxxxxxxxxxxxxxxx
	Vector3i(3,211,0): "",#sorcerer-red xxxxxxxxxxxxxxxxxxxx
	Vector3i(3,219,0): "",#sorcerer-red xxxxxxxxxxxxxxxxxxxx
	Vector3i(3,235,0): "",#sorcerer-red xxxxxxxxxxxxxxxxxxxx
	Vector3i(3,243,0): "",#sorcerer-violet xxxxxxxxxxxxxxxxxxxx
	Vector3i(3,251,0): "",#sorcerer-green xxxxxxxxxxxxxxxxxxxx
	Vector3i(0,8,0): "",#unknown
	Vector3i(0,58,0): "",#unknown
	Vector3i(0,67,0): "",#unknowns
	Vector3i(0,68,0): "",#unknown
	Vector3i(0,69,0): "",#unknown
	Vector3i(0,70,0): "",#unknown
	Vector3i(0,71,0): "",#unknown
	Vector3i(0,72,0): "",#unknown
	Vector3i(0,73,0): "",#unknown
	Vector3i(0,74,0): "",#unknown
	Vector3i(0,80,0): "",#unknown
	Vector3i(2,422,0): "",#unknown
	Vector3i(5,14,0): "",#unknown
	Vector3i(5,55,0): "",#unknown
	Vector3i(5,71,0): "",#unknown
	Vector3i(5,279,0): "",#unknown
	Vector3i(5,303,0): "",#unknown
	
	Vector3i(5,311,0): "res://entites/object_5_311_water_beast.tscn",#water beast
	Vector3i(5,319,0): "res://entites/object_5_319_water_beast.tscn",#water beast torso
	
	Vector3i(9,64,0): "",#unknown
	Vector3i(10,8,0): "res://entites/object_10_8_fair.tscn",#fair
	#Vector3i(10,8,0): "res://entites/object_10_8_fair.tscn",#fair
	Vector3i(10,38,0): "",#unknown
	Vector3i(10,54,0): "",#fair
	Vector3i(10,77,0): "",#fair
	#Vector3i(10,96,0): "res://entites/object_10_96_posses_building.tscn",#building
	Vector3i(10,96,0): "",#building
	Vector3i(10,145,0): "",#unknown
	Vector3i(11,8,0): "",#unknown
	Vector3i(14,8,0): "",#unknown-switch?
	Vector3i(14,259,0): "res://entites/object_14_259_scroll.tscn",#scroll
	Vector3i(14,461,0): "res://entites/object_14_461_mouth.tscn",#mouth-gate
	Vector3i(14,462,0): "res://entites/object_14_462_portal.tscn",#day portal
	Vector3i(15,59,0): "",#unknown-jar?
	#
	#,
	#Vector3i(2,75,0): "res://entites/object_2_75_tree.tscn",#tree
	#Vector3i(2,79,0): "res://entites/object_2_79_dolmen.tscn",#dolmen
	#Vector3i(2,87,0): "res://entites/object_2_75_tree.tscn",#tree2 - doplnit
	#Vector3i(3,0,0): "",#player
	#Vector3i(3,88,0): "res://entites/object_3_88_ballon.tscn",#ballon
	#Vector3i(5,8,0): "res://entites/object_5_8_bowman.tscn",#bowman
	#Vector3i(5,9,0): "res://entites/object_5_8_bowman.tscn",#bowman
	#Vector3i(5,10,0): "res://entites/object_5_8_bowman.tscn",#bowman
	#Vector3i(5,11,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,12,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,13,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,14,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,15,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,16,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,17,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,18,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,19,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,20,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,21,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow	
	#Vector3i(5,121,0): "res://entites/object_5_121_bowman.tscn",#bowman-crouch
	#Vector3i(5,122,0): "res://entites/object_5_121_bowman.tscn",#bowman-crouch
	#Vector3i(5,123,0): "res://entites/object_5_121_bowman.tscn",#bowman-crouch
	#Vector3i(5,152,0): "res://entites/object_5_152_goat.tscn",#goat
	#Vector3i(5,155,0): "res://entites/object_5_155_people1.tscn",#people1 155 156 157
	#Vector3i(5,180,0): "res://entites/object_5_180_people2.tscn",#people2 180 181 182
	#Vector3i(5,183,0): "res://entites/object_5_183_people3.tscn",#people3 183 184 185
	#Vector3i(5,199,0): "res://entites/object_5_199_people4.tscn",#people4 199 200 201
	#Vector3i(5,279,0): "res://entites/object_5_279_beetle.tscn",#beetle
	#Vector3i(9,55,0): "res://entites/object_9_55_fireball.tscn",#fireball
	#Vector3i(9,105,0): "res://entites/object_9_105_arrow.tscn",#arrow
	#Vector3i(9,144,0): "res://entites/object_9_144_posses.tscn",#posses
	#Vector3i(10,8,0): "res://entites/object_10_8_fair.tscn",#fair
	#Vector3i(10,57,0): "res://entites/object_10_57_smoke.tscn",#smoke
	#Vector3i(10,58,0): "res://entites/object_10_58_goldSphere.tscn",
	#Vector3i(10,63,0): "res://entites/object_10_57_smoke.tscn",#smoke
	#Vector3i(10,67,0): "res://entites/object_10_67_whiteSphere.tscn",
	#Vector3i(10,96,0): "res://entites/object_10_96_posses_building.tscn"#building
}

#3-97 red flag---ok
#3-251 - wizard-ok
#3-100 - green flag---ok
#3-89 - red ballon-ok
#3-92 - green ballon-ok
#10-71 - green mana--ok
#10-98 - violet flag-must make!!!!!!!!!!!!!
#9-64 - meteor-ok
#14-462 - day portal
#10-77 - ohen-ok
#10-145(koule-metor?)-ok
#10-34
#- fix dead sorcerer

#3-251 5-411-mummy
#Vector3i(10,34,0): "",#fair - portal
#10 80 - catle bold

#2-422-sud-ok
#3-203-osmy kouzelnik-blue
#3-99-modra vlajka hradu-ok
#10-70-blue mana-ok
#10-55-hvezdicka-zbran-ok
#5-58-gold mana-ok
#5-71-ok
#5-67-ok
#5-68-ok
#0-70-ok
#5-67-ok
#9-151
#5-352-skokan---------------
#328	335	levá noha pokrčena v koleni, nahoře	ctvrta
#336	343	pravá noha mírně za levou, obě na zemi	pata
#344	351	pravá noha mírně za levou, obě na zemi, tělo mírně přikrčené	sesta
#352	359	pravá noha mírně za levou, obě na zemi, tělo více přikrčené	první
#360	367	levá noha před pravou v nákroku, obě na zemi	druha
#368	375	levá noha pokrčena v koleni, nahoře, pravá téměř natažena	treti
#376	383	nohy kousek od sebe stojací postoj	
			#
#384	391	samotná horní část těla z předchozího kroku

#5-384-hopper down part-ok
#5-263-jakoby lev-ok
#3-243-predposledni kouzelnik-violet
#9-60-zelena strela
#10-69-fialova mana
#5-70-tmave modra mana-ok

#PLAYER-WHITE- e0e0e8 a4b0f0
#RED-NYMPHUR-OK fc1414 c40808
#VIOLET-JARK 985c84 542848
#BLUE-BELIX-OK 486490 304860
#GREEN-RAHN-OK 305828 24401c
#PINK-ELYSSIA fcccc4 c87474
#ORANGE-PRISH e8982c a4581c
#GRAY-YRAGORE 241814 0c0804

# 5-303-gryf 5-393-ty male mrchy 5-392 5-319-spicka vodni nestvury 5-311-mensi spicka
#9-146 10-38 10-202-blue orb   3-243-sorceler 5-303 5-295
#5-437 5-445 mummy 9-80
#3-235-pink-sorcerer-ok
#3-101-ping flag-must add!!!!!
#10-72-pink sphere-ok
#2-422-sud
#0-72-ok
#3-219-next sorcerer-black-ok
#3-103-black flag
#3-101-ping flag 0-74 5-279 5-124


var filter_material: ShaderMaterial
var data_img: Image
var data_tex: ImageTexture

func _preload_library(source_dict: Dictionary, target_dict: Dictionary):
	for key in source_dict:
		var path = source_dict[key]
		if path != "":
			target_dict[key] = load(path)

func _ready():	
	_preload_library(library, library_scenes)
	_preload_library(library2, library2_scenes)
	#node_pool.resize(pool_size)
	#for i in range(pool_size):
		#node_pool[i] = null

var last_spell_index:int =-1
var last_button:int =-1

var last_spell_sub_index:int =-1
var last_sub_button:int =-1

func setPlayerActiveSpell(spell_index: int,button:int):
	last_spell_index = spell_index
	last_button = button
	Global.MBEX.setPlayerActiveSpell(spell_index,button)

func setPlayerActiveSubSpell(spell_index: int,sub_spell_index: int,button:int):
	last_spell_sub_index = spell_index
	last_sub_button = button
	Global.MBEX.setPlayerActiveSubSpell(spell_index,sub_spell_index,button)

func updatePlayer(playerPosRot) -> void:
	Main_Player.position=playerPosRot.position/256
	var yaw = PI*playerPosRot.rotation.yaw/(256*4)
	var pitch = PI*playerPosRot.rotation.pitch/(256*4)
	var roll = PI*playerPosRot.rotation.roll/(256*4)
	Main_Player.rotation=Vector3(-pitch, -yaw, -roll)
	
var last_gain: Vector3
var last_offset: Vector3
var last_saturation: float

var runned: bool

func SetRunned(sendRunned) -> void:
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
	gameInit()
	runned = sendRunned

#func getSpritesActions():
	#return Global.MBEX.getPendingGraphicsActions()

func initLanguage(langIndex):
	return Global.MBEX.initLanguage(langIndex)

func changeLanguage(langIndex):
	Global.MBEX.changeLanguage(langIndex)

func getLangTexts():
	Global.langTexts=Global.MBEX.getLangTexts()

var nextState=0
func _process(_p_delta) -> void:
	if(!runned):
		return
	getInputs()
	#MBEX.soundQueueClear()
	Global.MBEX.updateFreeSoundPlayers(Global.Main_Sounds.get_free_player_indices())
	#if(Main_UI.old_is_ctrl_active!=Main_UI.is_ctrl_active):
		#if Main_UI.is_ctrl_active:
			#Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
			#Main_UI.saved_mouse_pos = get_viewport().get_mouse_position()
			#var grid_rect = Main_UI.spell_grid.get_global_rect()
			#var center_pos = grid_rect.position + (grid_rect.size / 2.0)
			#center_pos.x=50
			#get_viewport().warp_mouse(center_pos)
		#else:
			#get_viewport().warp_mouse(Main_UI.saved_mouse_pos)
			#Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
		#Main_UI.old_is_ctrl_active=Main_UI.is_ctrl_active
	#if(last_spell_index!=-1):
		#Global.MBEX.setPlayerActiveSpell(last_spell_index,last_button)
		#last_spell_index = -1
		#last_button = -1
	
	if(Global.MBEX.REMC2GetWebInfo()):
		get_parent().get_node("SpiderWeb").show()
	else:
		get_parent().get_node("SpiderWeb").hide()
	
	var gameState=Global.MBEX.REMC2StepInGame(input_state,nextState)
	match gameState:
		1:
			Global.MBEX.renew_terrain((Global.getLevelType()=="Cave"))
			var mods = Global.MBEX.getPaletteModifications()
			var current_gain = mods[0]
			var current_offset = mods[1]
			var current_saturation = mods[2]
			if current_gain != last_gain or current_offset != last_offset or current_saturation != last_saturation:
				if(!filter_material):
					filter_material = Main_Filter.material as ShaderMaterial
				filter_material.set_shader_parameter("MyGain", current_gain)
				filter_material.set_shader_parameter("MyOffset", current_offset)
				filter_material.set_shader_parameter("MySatMultiplier", current_saturation)
				last_gain = current_gain
				last_offset = current_offset
				last_saturation = current_saturation
			#var gain_vec = Vector3(gain_rgb.r, gain_rgb.g, gain_rgb.b)
			#var offset_vec = Vector3(offset_rgb.r, offset_rgb.g, offset_rgb.b)	
			#Main_Filter.material_override.set_shader_parameter("MyGain", gain_vec)
			#Main_Filter.material_override.set_shader_parameter("MyOffset", offset_vec)
			updatePlayer(getPlayerPosRot())
			renderEntites(getEntites())
			get_parent().get_node("UILayer/UI").updateSpells(Global.MBEX.getActiveSpells())
			get_parent().get_node("UILayer/UI").updateSelectedSpells(Global.MBEX.getSelectedSpells())
			get_parent().get_node("UILayer/UI").updateMinimap(Global.MBEX.getMinimap())
			Global.Main_Sounds.updateSounds(Global.MBEX.getPendingSoundActions())
		2:
			Global.setLoadingScreenStr("SMATITL2.DAT.png")
			get_parent().get_node("SpiderWeb").hide()
			filter_material.set_shader_parameter("MyGain", Vector3(1.0,1.0,1.0))
			filter_material.set_shader_parameter("MyOffset", Vector3(0.0,0.0,0.0))
			filter_material.set_shader_parameter("MySatMultiplier", 1.0)
			nextState=1
		4:
			nextState=1
		3:#shadow level
			runned=false
			Global.setLevelType(Global.MBEX.REMC2GetLevelType())
			Global.Main_Sounds.stopAllSounds()
			get_parent().get_node("TerrainsMB").updateMeshes()
			gameInit()
			fadeNode = Global.addFadeIn(fadeNode)
			await fadeNode.fade_finished
			Global.setLoadingScreenStr("")
			fadeNode = Global.addFadeOut(fadeNode)
			nextState=0
			runned=true
		_:
			runned=false
			call_deferred("_do_change_scene")

func _do_change_scene():
	Global.Main_Sounds.stopAllSounds()
	fadeNode = Global.addFadeIn(fadeNode)
	await fadeNode.fade_finished
	Global.last_scene_path = get_tree().current_scene.scene_file_path
	get_tree().change_scene_to_file("res://scenes/MapMenu.tscn")


var entites_pool:Dictionary

func add_to_entites_pool(uid: Vector3i, sendNode: Node) -> void:
	if not entites_pool.has(uid):
		entites_pool[uid] = {
			"array": [],
			"active_count": 0,
			"act_index": 0
		}
	entites_pool[uid]["array"].append(sendNode)
	entites_pool[uid]["active_count"] += 1

func get_first_entity_with_uid(uid: Vector3i) -> Node:
	if entites_pool.has(uid) and not entites_pool[uid]["array"].is_empty():
		if(entites_pool[uid]["act_index"]<entites_pool[uid]["array"].size()):
			var result = entites_pool[uid]["array"][entites_pool[uid]["act_index"]]
			return result
		else:
			return null;
	return null

func add_pool_index(uid: Vector3i):
	if entites_pool.has(uid) and not entites_pool[uid]["array"].is_empty():
		entites_pool[uid]["act_index"]+=1
		entites_pool[uid]["active_count"] += 1

func show_hide_entites() -> void:
	for bucket in entites_pool.values():
		var arr = bucket["array"]
		var active_count = bucket["active_count"]
		for i in range(arr.size()):
			var node: Node = arr[i]
			if i < active_count:
				node.show()
				node.set_process(true)
				node.set_physics_process(true)
			else:
				node.hide()
				node.set_process(false)
				node.set_physics_process(false)
				if(node.get_meta("uid")==Vector3i(14,461,0) or node.get_meta("uid")==Vector3i(14,462,0)):#remove entites with start script
					node.queue_free()
					arr.remove_at(i)
		if arr.is_empty():
			entites_pool.erase(bucket)
		bucket["act_index"] = 0
		bucket["active_count"] = 0

func clear_entites_pool() -> void:
	for bucket in entites_pool.values():
		var arr = bucket["array"]
		for node in arr:
			if is_instance_valid(node):
				node.queue_free()
	entites_pool.clear()

static var _next_uid: int = 0
func generate_unique_id() -> int:
	_next_uid += 1
	return _next_uid


func renderEntites(data_array: PackedFloat32Array) -> void:
	var entites_per_frame=0;
	var stride = 31
	var default_key = Vector3i(0, 999, 0)
	var inv_256 = 1.0 / 256.0
	var camera = get_viewport().get_camera_3d()
	var has_camera = camera != null
	var cam_pos = camera.global_position if has_camera else Vector3.ZERO
	var rad_mult = PI / 1024.0 # Zjednodušeno z PI / (256 * 4)
	#entites_pool = entites_pool_used
	#entites_pool_used = {}
	for i in range(pool_size):
		var offset = i * stride
		var pos = Vector3(data_array[offset], data_array[offset+2], data_array[offset+1])
		var rot = Vector3(data_array[offset+3], data_array[offset+4], data_array[offset+5])
		var actState = int(data_array[offset+6])
		var actClass = int(data_array[offset+7])
		var actModel = int(data_array[offset+8])
		var actXtype = int(data_array[offset+9])
		var actXsubtype = int(data_array[offset+10])
		var actWord2C = int(data_array[offset+11])
		var actSpeed = int(data_array[offset+12])
		var actByte38 = int(data_array[offset+13])
		var actByte39 = int(data_array[offset+14])
		var actByte3A = int(data_array[offset+15])
		var actId = int(data_array[offset+16])
		var actByte0 = int(data_array[offset+17])
		var actByte1 = int(data_array[offset+18])
		var actByte2 = int(data_array[offset+19])
		var actByte3 = int(data_array[offset+20])
		var modelIndex = int(data_array[offset+21])
		var rot2 = Vector3(data_array[offset+22], 0, 0)
		#if((actClass==10)&&(modelIndex==58)||(modelIndex==67)):
			#rot2 = Vector3(data_array[offset+22], data_array[offset+23], data_array[offset+24])
		var actMana = int(data_array[offset+24])
		var actLife = int(data_array[offset+25])
		var actMaxMana = int(data_array[offset+26])
		var actMaxLife = int(data_array[offset+27])
		var actOwnerObject = int(data_array[offset+28])
		var actBitmapScaleHelp = int(data_array[offset+29])
		var actBitmapScale = int(data_array[offset+30])
		
		#if(modelIndex!=144):
			#continue;
		
		if modelIndex == 0 and actClass == 3:
			Main_Player.MOVE_SPEED = actSpeed
			Main_Player.LIFE = actLife
			Main_Player.MANA = actMana
		
		var updateObject=false
		var current_node = null
		if not (actByte1 & 4):
			var isDraw = (actByte0 & 1) == 0
			if actClass == 3 and (modelIndex == 203 or modelIndex == 211 or modelIndex == 219 or modelIndex == 235 or modelIndex == 243 or modelIndex == 251) and actLife <= 0:
				isDraw = false#noDraw dead sorcerers
			var fromlib = false
			var uid2 = Vector3i(actClass,modelIndex,0)
			var scene_to_instance = null
			var libType:int = 0
			if isDraw and actClass in [2, 3, 5, 9, 10, 15]:
				libType = 1
				if library_scenes.has(uid2):
					scene_to_instance = library_scenes[uid2]
					fromlib = true
				elif not library.has(uid2):
					scene_to_instance = library_scenes.get(default_key)
			else:
				libType = 2
				if library2_scenes.has(uid2):
					scene_to_instance = library2_scenes[uid2]
					fromlib = true
				elif not library2.has(uid2):
					scene_to_instance = library2_scenes.get(default_key)
			var uid = Vector3i(actClass,modelIndex,libType)
			if scene_to_instance != null:
				current_node = get_first_entity_with_uid(uid)
				if current_node == null:
					var new_node = scene_to_instance.instantiate()
					if not fromlib:
						new_node.get_node("Label3D").text = "M:%d_C:%d_M:%d_S:%d_B0:%d" % [modelIndex, actClass, actModel, actState, actByte0]
					new_node.set_meta("uid",uid)
					add_child(new_node)
					current_node = new_node
					add_to_entites_pool(uid,new_node)
					updateObject=true
				else:
					add_pool_index(uid)
					updateObject=true
		if (current_node&&updateObject):
			if(modelIndex == 411):#zobmie
				#var nodeStateVisibility=current_node.get_fade_state()
				var entityStateHidden=actByte2 & 0x80
				if(entityStateHidden==0x80):
					current_node.fade_out()
				else:
					current_node.fade_in()
			if actBitmapScaleHelp:
				var scale_scene_node = current_node.get_node_or_null("Scale")
				if scale_scene_node:
					var s = actBitmapScale * inv_256
					scale_scene_node.scale = Vector3(s, s, s)
			var entityScale = 1.0
			if actClass == 10 and actModel == 39: # manSphere
				entityScale = pow(actMana, 1.0 / 3.0) * 0.1 #alternative mana size computing
				#entityScale = actBitmapScale * inv_256  #original sice mana computing
			if actClass == 5 and actModel == 22: # manSphere from mana snake
				entityScale = actBitmapScale * inv_256
			current_node.scale = Vector3(entityScale, entityScale, entityScale)
			var base_pos_x = data_array[offset] * inv_256
			var base_pos_y = data_array[offset + 2] * inv_256
			var base_pos_z = data_array[offset + 1] * inv_256
			if has_camera:
				var new_x = cam_pos.x + fposmod(base_pos_x - cam_pos.x + 128.0, 256.0) - 128.0
				var new_z = cam_pos.z + fposmod(base_pos_z - cam_pos.z + 128.0, 256.0) - 128.0
				current_node.global_position = Vector3(new_x, base_pos_y, new_z)
			else:
				current_node.position = Vector3(base_pos_x, base_pos_y, base_pos_z)
			var yaw = -rot2.x * rad_mult
			current_node.rotation = Vector3(0, yaw, 0)
	show_hide_entites()

var last_keys_state: Dictionary = {}
var last_mouse_buttons_state: Dictionary = {}
#var mouse_640: Vector2
const SCREEN_WIDTH := 640
const SCREEN_HEIGHT := 480

var _pending_key_changes: Array = []
var _pending_mouse_changes: Array = []
#var last_keys_state: Dictionary = {}
#var last_mouse_buttons_state: Dictionary = {}
#var input_state: Dictionary = {}
#var total_mouse_delta: Vector2 = Vector2.ZERO
#var mouse_640: Vector2 = Vector2.ZERO

var fog_presets = [
	{"begin": 10.0, "end": 100.0, "density": 0.01},
	{"begin": 5.0,  "end": 50.0,  "density": 0.03},
	{"begin": 2.0,  "end": 30.0,  "density": 0.08},
	{"begin": 1.0,  "end": 15.0,  "density": 0.2},
	{"begin": 0.0,  "end": 8.0,   "density": 0.5}
]
var current_fog_index: int = 0
func changeFog():
	current_fog_index += 1
	if current_fog_index >= fog_presets.size():
		current_fog_index = 0
	var target_values = fog_presets[current_fog_index]
	sefFogEnd(target_values["end"])
	setFogFall(target_values["begin"])
	setFogDensity(target_values["density"])
	

func _input(event):
	if event is InputEventKey and event.pressed and not event.echo:
		match event.keycode:
			KEY_F7:
				changeFog()
		
	if event is InputEventKey and not event.echo:
		if event.keycode in KEY_INDEX:
			var index = KEY_INDEX[event.keycode]
			var is_pressed: bool = event.pressed
			if last_keys_state.get(index, false) != is_pressed:
				last_keys_state[index] = is_pressed
				_pending_key_changes.append({
					"key_index": index,
					"action": "pressed" if is_pressed else "released"
			})
	if event is InputEventMouseButton:
		if event.button_index in MOUSE_BUTTON_INDEX:
			var index = MOUSE_BUTTON_INDEX[event.button_index]
			var is_pressed: bool = event.pressed
			if last_mouse_buttons_state.get(index, false) != is_pressed:
				last_mouse_buttons_state[index] = is_pressed
				_pending_mouse_changes.append({
					"button_index": index,
					"action": "pressed" if is_pressed else "released"
				})

func _release_all_inputs():
	for index in last_keys_state:
		if last_keys_state[index] == true:
			_pending_key_changes.append({
				"key_index": index,
				"action": "released"
			})
	last_keys_state.clear()
	for index in last_mouse_buttons_state:
		if last_mouse_buttons_state[index] == true:
			_pending_mouse_changes.append({
				"button_index": index,
				"action": "released"
			})
	last_mouse_buttons_state.clear()

func getInputs():
	for keycode in KEY_INDEX:
		var index = KEY_INDEX[keycode]
		var is_actually_pressed = Input.is_key_pressed(keycode)
		var last_state = last_keys_state.get(index, false)
		if last_state == true and not is_actually_pressed:
			_pending_key_changes.append({
				"key_index": index,
				"action": "released"
			})
			last_keys_state[index] = false
	var key_changes = _pending_key_changes
	var mouse_changes = _pending_mouse_changes
	_pending_key_changes = []
	_pending_mouse_changes = []
	var real_mouse_pos = get_viewport().get_mouse_position()
	var screen_size = get_viewport().get_visible_rect().size
	var m_x = clamp((real_mouse_pos.x / screen_size.x) * SCREEN_WIDTH, 0, SCREEN_WIDTH)
	var m_y = clamp((real_mouse_pos.y / screen_size.y) * SCREEN_HEIGHT, 0, SCREEN_HEIGHT)
	input_state = {
		"key_changes": key_changes,
		"mouse_button_changes": mouse_changes,
		"mouse_pos2": Vector2(m_x, m_y)
	}
	
	if Input.is_key_pressed(KEY_F1):
		Main_UI.get_node("CanvasLayerHelp").start_fade_out()

func _notification(what):
	if DL_inGame:
		match what:
			NOTIFICATION_APPLICATION_FOCUS_OUT:
				Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
			NOTIFICATION_APPLICATION_FOCUS_IN:
				_release_all_inputs()
				Input.set_mouse_mode(Input.MOUSE_MODE_CONFINED_HIDDEN)
	else:
		match what:
			NOTIFICATION_APPLICATION_FOCUS_OUT:
				Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
			NOTIFICATION_WM_MOUSE_EXIT:
				Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
			NOTIFICATION_APPLICATION_FOCUS_IN, NOTIFICATION_WM_MOUSE_ENTER:
				Input.set_mouse_mode(Input.MOUSE_MODE_CONFINED_HIDDEN)
	#if what == NOTIFICATION_APPLICATION_FOCUS_OUT:
		#_release_all_inputs()
		#Input.mouse_mode = Input.MOUSE_MODE_VISIBLE
	#elif what == NOTIFICATION_APPLICATION_FOCUS_IN:
		#Input.mouse_mode = Input.MOUSE_MODE_CONFINED_HIDDEN

#func getInputsX():
	#var changes = []
	##if Main_UI && Main_UI.is_ctrl_active:
		##return
	#for keycode: int in KEY_INDEX:
		#var index: int = KEY_INDEX[keycode]
		#var is_pressed: bool = Input.is_key_pressed(keycode)
		#var previous_state = last_keys_state.get(index, false)
		#if is_pressed != previous_state:
			#var status = "pressed" if is_pressed else "released"
			#changes.append({
				#"key_index": index,
				#"action": status
			#})
			#last_keys_state[index] = is_pressed
	#input_state["key_changes"] = changes
	#
	#var mouse_changes := []
#
	#for button: int in MOUSE_BUTTON_INDEX:
		#var index: int = MOUSE_BUTTON_INDEX[button]
		#var is_pressed: bool = Input.is_mouse_button_pressed(button)
		#var previous_state = last_mouse_buttons_state.get(index, false)
		#if is_pressed != previous_state:
			#var status = "pressed" if is_pressed else "released"
			#mouse_changes.append({
				#"button_index": index,
				#"action": status
			#})
			#last_mouse_buttons_state[index] = is_pressed
	#input_state["mouse_button_changes"] = mouse_changes
#
	#mouse_640 = Vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2)
	#mouse_640 += total_mouse_delta*0.2
	#mouse_640.x = clamp(mouse_640.x, 0, SCREEN_WIDTH)
	#mouse_640.y = clamp(mouse_640.y, 0, SCREEN_HEIGHT)
	#
	#var real_mouse_pos = get_viewport().get_mouse_position()
	#var screen_size = get_viewport().get_visible_rect().size
	#var m_x = (real_mouse_pos.x / screen_size.x) * SCREEN_WIDTH
	#var m_y = (real_mouse_pos.y / screen_size.y) * SCREEN_HEIGHT
	#m_x = clamp(m_x, 0, SCREEN_WIDTH)
	#m_y = clamp(m_y, 0, SCREEN_HEIGHT)
	#input_state["mouse_pos"] = mouse_640
	#input_state["mouse_pos2"] = Vector2(m_x, m_y)

func init():
	#Global.loadScreenInit()
	MBEXinit()
	Global.initSound()

func gameInit():
	match Global.getLevelType():
		"Day":
			Global.Main_Sounds.setSoundBank(0)
			setTime(9.0)
			setDayEntites()
			setSkyExposure(0.3)
			sefFogEnd(200)
			setFogFall(15)
			setFogDensity(0.01)
			setAtmDayTint(Color(0.8,0.9,1.0))
			setSunMoon(true,true,0.7,1.0)
		"Night":
			Global.Main_Sounds.setSoundBank(1)
			setTime(2.5)
			setNightEntites()
			setSkyExposure(1.5)
			sefFogEnd(200)
			setFogFall(15)
			setFogDensity(0.01)
			setAtmDayTint(Color(0.8,0.9,1.0))
			setSunMoon(true,true,1.0,1.0)
		"Cave":
			Global.Main_Sounds.setSoundBank(2)
			setTime(0.0)
			setFog(0.03)
			setFogSky(1.0)
			setSkyExposure(0.3)
			setCaveEntites()
			setAtmDayTint(Color(0,0,0))
			setSunMoon(false,true,1.0,0.0)
		"Final":
			Global.Main_Sounds.setSoundBank(0)
			setTime(11.0)
			setDayEntites()
			setSkyExposure(0.3)
			sefFogEnd(200)
			setFogFall(15)
			setFogDensity(0.01)
			setAtmDayTint(Color(0.8,0.9,1.0))
			setSunMoon(true,true,0.7,1.0)
	if(Global.getLevelType()=="Cave"):
		get_parent().get_node("TerrainsMB").mesh_instance_top.show()
		get_parent().get_node("MultiMeshtop").show()
	else:
		get_parent().get_node("TerrainsMB").mesh_instance_top.hide()
		get_parent().get_node("MultiMeshtop").hide()
	clear_entites_pool()

func setDayEntites():
	updateLibrary(5,14,0,"res://entites/object_5_11D_hornet.tscn")
	updateLibrary(5,15,0,"res://entites/object_5_11D_hornet.tscn")
	updateLibrary(5,16,0,"res://entites/object_5_11D_hornet.tscn")
	updateLibrary(5,17,0,"res://entites/object_5_11D_hornet.tscn")
	updateLibrary(5,18,0,"res://entites/object_5_11D_hornet.tscn")
	updateLibrary(5,19,0,"res://entites/object_5_11D_hornet.tscn")
	updateLibrary(5,20,0,"res://entites/object_5_11D_hornet.tscn")
	updateLibrary(5,21,0,"res://entites/object_5_11D_hornet.tscn")
	
	updateLibrary(10,57,0,"res://entites/object_10_57_smoke.tscn")
	updateLibrary(10,63,0,"res://entites/object_10_63_smoke.tscn")
	
	updateLibrary(3,96,0,"res://entites/object_3_96_posses_building-whiteD.tscn")
	updateLibrary(3,97,0,"res://entites/object_3_97_posses_building-redD.tscn")#castle -difmodels!!!
	updateLibrary(3,98,0,"res://entites/object_3_98_posses_buildingD-violet.tscn")#building -difmodels!!!
	updateLibrary(3,99,0,"res://entites/object_3_99_posses_building-blueD.tscn")#castle -difmodels!!!
	updateLibrary(3,100,0,"res://entites/object_3_100_posses_building-greenD.tscn")#castle -difmodels!!!
	updateLibrary(3,101,0,"res://entites/object_3_101_posses_building-pinkD.tscn")#castle -difmodels!!!
	updateLibrary(3,102,0,"res://entites/object_3_102_posses_building-orangeD.tscn")#castle -difmodels!!!
	updateLibrary(3,103,0,"res://entites/object_3_103_posses_building-blackD.tscn")#castle -difmodels!!!
	
	updateLibrary(10,96,0,"res://entites/object_3_96_posses_building-whiteD.tscn")
	updateLibrary(10,97,0,"res://entites/object_3_97_posses_building-redD.tscn")#building -difmodels!!!
	updateLibrary(10,98,0,"res://entites/object_10_98_posses_building-violetD.tscn")#building -difmodels!!!
	updateLibrary(10,99,0,"res://entites/object_3_99_posses_building-blueD.tscn")#castle -difmodels!!!
	updateLibrary(10,100,0,"res://entites/object_3_100_posses_building-greenD.tscn")#castle -difmodels!!!
	updateLibrary(10,101,0,"res://entites/object_3_101_posses_building-pinkD.tscn")#castle -difmodels!!!
	updateLibrary(10,102,0,"res://entites/object_3_102_posses_building-orangeD.tscn")#castle -difmodels!!!
	updateLibrary(10,103,0,"res://entites/object_3_103_posses_building-blackD.tscn")#castle -difmodels!!!
	
	
func setNightEntites():
	updateLibrary(5,14,0,"res://entites/object_5_11_bowman.tscn")
	updateLibrary(5,15,0,"res://entites/object_5_11_bowman.tscn")
	updateLibrary(5,16,0,"res://entites/object_5_11_bowman.tscn")
	updateLibrary(5,17,0,"res://entites/object_5_11_bowman.tscn")
	updateLibrary(5,18,0,"res://entites/object_5_11_bowman.tscn")
	updateLibrary(5,19,0,"res://entites/object_5_11_bowman.tscn")
	updateLibrary(5,20,0,"res://entites/object_5_11_bowman.tscn")
	updateLibrary(5,21,0,"res://entites/object_5_11_bowman.tscn")

	updateLibrary(10,57,0,"res://entites/object_10_57_smoke.tscn")
	updateLibrary(10,63,0,"res://entites/object_10_63_smoke.tscn")
	
	updateLibrary(3,96,0,"res://entites/object_3_96_posses_building-white.tscn")	
	updateLibrary(3,97,0,"res://entites/object_3_97_posses_building-red.tscn")#castle -difmodels!!!
	updateLibrary(3,98,0,"res://entites/object_3_98_posses_building-violet.tscn")#building -difmodels!!!
	updateLibrary(3,99,0,"res://entites/object_3_99_posses_building-blue.tscn")#castle -difmodels!!!
	updateLibrary(3,100,0,"res://entites/object_3_100_posses_building-green.tscn")#castle -difmodels!!!
	updateLibrary(3,101,0,"res://entites/object_3_101_posses_building-pink.tscn")#castle -difmodels!!!
	updateLibrary(3,102,0,"res://entites/object_3_102_posses_building-orange.tscn")#castle -difmodels!!!
	updateLibrary(3,103,0,"res://entites/object_3_103_posses_building-black.tscn")#castle -difmodels!!!
	
	
	updateLibrary(10,96,0,"res://entites/object_3_96_posses_building-white.tscn")	
	updateLibrary(10,97,0,"res://entites/object_3_97_posses_building-red.tscn")#building -difmodels!!!
	updateLibrary(10,98,0,"res://entites/object_3_98_posses_building-violet.tscn")#building -difmodels!!!
	updateLibrary(10,99,0,"res://entites/object_3_99_posses_building-blue.tscn")#castle -difmodels!!!
	updateLibrary(10,100,0,"res://entites/object_3_100_posses_building-green.tscn")#castle -difmodels!!!
	updateLibrary(10,101,0,"res://entites/object_3_101_posses_building-pink.tscn")#castle -difmodels!!!
	updateLibrary(10,102,0,"res://entites/object_3_102_posses_building-orange.tscn")#castle -difmodels!!!
	updateLibrary(10,103,0,"res://entites/object_3_103_posses_building-black.tscn")#castle -difmodels!!!

func setCaveEntites():
	updateLibrary(5,14,0,"res://entites/object_5_11_bowman.tscn")
	updateLibrary(5,15,0,"res://entites/object_5_11_bowman.tscn")
	updateLibrary(5,16,0,"res://entites/object_5_11_bowman.tscn")
	updateLibrary(5,17,0,"res://entites/object_5_11_bowman.tscn")
	updateLibrary(5,18,0,"res://entites/object_5_11_bowman.tscn")
	updateLibrary(5,19,0,"res://entites/object_5_11_bowman.tscn")
	updateLibrary(5,20,0,"res://entites/object_5_11_bowman.tscn")
	updateLibrary(5,21,0,"res://entites/object_5_11_bowman.tscn")
	updateLibrary(3,96,0,"res://entites/object_3_96_posses_building-white.tscn")
	updateLibrary(10,57,0,"res://entites/object_10_57_smokeC.tscn")
	updateLibrary(10,63,0,"res://entites/object_10_63_smokeC.tscn")
	
	updateLibrary(3,96,0,"res://entites/object_3_96_posses_building-white.tscn")	
	updateLibrary(3,97,0,"res://entites/object_3_97_posses_building-red.tscn")#castle -difmodels!!!
	updateLibrary(3,98,0,"res://entites/object_3_98_posses_building-violet.tscn")#building -difmodels!!!
	updateLibrary(3,99,0,"res://entites/object_3_99_posses_building-blue.tscn")#castle -difmodels!!!
	updateLibrary(3,100,0,"res://entites/object_3_100_posses_building-green.tscn")#castle -difmodels!!!
	updateLibrary(3,101,0,"res://entites/object_3_101_posses_building-pink.tscn")#castle -difmodels!!!
	updateLibrary(3,102,0,"res://entites/object_3_102_posses_building-orange.tscn")#castle -difmodels!!!
	updateLibrary(3,103,0,"res://entites/object_3_103_posses_building-black.tscn")#castle -difmodels!!!
	
	
	updateLibrary(10,96,0,"res://entites/object_3_96_posses_building-white.tscn")	
	updateLibrary(10,97,0,"res://entites/object_3_97_posses_building-red.tscn")#building -difmodels!!!
	updateLibrary(10,98,0,"res://entites/object_3_98_posses_building-violet.tscn")#building -difmodels!!!
	updateLibrary(10,99,0,"res://entites/object_3_99_posses_building-blue.tscn")#castle -difmodels!!!
	updateLibrary(10,100,0,"res://entites/object_3_100_posses_building-green.tscn")#castle -difmodels!!!
	updateLibrary(10,101,0,"res://entites/object_3_101_posses_building-pink.tscn")#castle -difmodels!!!
	updateLibrary(10,102,0,"res://entites/object_3_102_posses_building-orange.tscn")#castle -difmodels!!!
	updateLibrary(10,103,0,"res://entites/object_3_103_posses_building-black.tscn")#castle -difmodels!!!

func setTime(time:float):
	time = fposmod(time, 24.0)  # zajistí 0–24 hodin, žádný crash
	var tod = NodeSky3D.get_node_or_null(^"TimeOfDay")
	var moon = NodeSky3D.get_node_or_null(^"MoonLight")
	if tod:
		if "current_time" in tod:
			tod.current_time = time
	if "current_time" in NodeSky3D:
		NodeSky3D.current_time = time
	if(time>=0)&&(time<=5):
		NodeSky3D.clouds_enabled=false
		moon.light_energy=1.5
	else:
		NodeSky3D.clouds_enabled=true
		moon.light_energy=1

func setSunMoon(sun:bool,moon:bool,sunSpec:float,moonSpec:float):
	if(sun):
		NodeSky3D.get_node_or_null(^"SunLight").show()
	else:
		NodeSky3D.get_node_or_null(^"SunLight").hide()
	if(moon):
		NodeSky3D.get_node_or_null(^"MoonLight").show()
	else:
		NodeSky3D.get_node_or_null(^"MoonLight").hide()
	NodeSky3D.get_node_or_null(^"SunLight").light_specular=sunSpec
	NodeSky3D.get_node_or_null(^"MoonLight").light_specular=moonSpec

func setAtmDayTint(value:Color):
	var skydome:SkyDome = NodeSky3D.get_node_or_null(^"SkyDome")
	skydome.atm_day_tint=value

func setSkyExposure(value:float):
	var skydome:SkyDome = NodeSky3D.get_node_or_null(^"SkyDome")
	skydome.exposure=value
	
func sefFogEnd(value:float):
	var skydome:SkyDome = NodeSky3D.get_node_or_null(^"SkyDome")
	skydome.fog_end=value
	
func setFogFall(value:float):
	var skydome:SkyDome = NodeSky3D.get_node_or_null(^"SkyDome")
	skydome.fog_falloff=value
	
func setFogDensity(value:float):
	var skydome:SkyDome = NodeSky3D.get_node_or_null(^"SkyDome")
	skydome.fog_density=value

func setFog(density:float):
	NodeSky3D.environment.fog_density=density

func setFogSky(sky_affect:float):
	NodeSky3D.environment.fog_sky_affect=sky_affect

func anim1Begin(ScrBufferRect:TextureRect,index:int):
	#Global.MBEX.updateFreeSoundPlayers(Main_Sounds.get_free_player_indices())
	#Global.MBEX.playAnim(index)
	#Global.MBEX.REMC2BeginItem()
	#Global.MBEX.REMC2BeginAnim(ScrBufferRect,index)
	Global.MBEX.REMC2SetScrBuffer(ScrBufferRect)
	changeLanguage(2)#only temporary fix

func setLoadScreenBuffer(locTextureRect):
	Global.MBEX.REMC2SetScrBuffer(locTextureRect)

var DL_inGame=false;
func MBrun(inGame):
	DL_inGame=inGame
	var locGraphicsEnhance = Global.MBEX.REMC2GetGraphicsEenhance()
	getInputs()
	Global.MBEX.updateFreeSoundPlayers(Global.Main_Sounds.get_free_player_indices())
	#if(inGame):
		#if(Main_UI.old_is_ctrl_active!=Main_UI.is_ctrl_active):
			#if Main_UI.is_ctrl_active:
				#Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
				#Main_UI.saved_mouse_pos = get_viewport().get_mouse_position()
				#var grid_rect = Main_UI.spell_grid.get_global_rect()
				#var center_pos = grid_rect.position + (grid_rect.size / 2.0)
				#center_pos.x=50
				#get_viewport().warp_mouse(center_pos)
			#else:
				#get_viewport().warp_mouse(Main_UI.saved_mouse_pos)
				#Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
			#Main_UI.old_is_ctrl_active=Main_UI.is_ctrl_active
		#if(last_spell_index!=-1):
			#Global.MBEX.setPlayerActiveSpell(last_spell_index,last_button)
			#last_spell_index = -1
			#last_button = -1
			
	var result=Global.MBEX.REMC2Run(input_state,0)
	if(inGame):
		if(locGraphicsEnhance):
			Main_Filter.show()
			if(Global.MBEX.REMC2GetWebInfo()):
				get_parent().get_node("SpiderWeb").show()
			else:
				get_parent().get_node("SpiderWeb").hide()
			Global.MBEX.renew_terrain((Global.getLevelType()=="Cave"))
			var mods = Global.MBEX.getPaletteModifications()
			var current_gain = mods[0]
			var current_offset = mods[1]
			var current_saturation = mods[2]
			if current_gain != last_gain or current_offset != last_offset or current_saturation != last_saturation:
				if(!filter_material):
					filter_material = Main_Filter.material as ShaderMaterial
				filter_material.set_shader_parameter("MyGain", current_gain)
				filter_material.set_shader_parameter("MyOffset", current_offset)
				filter_material.set_shader_parameter("MySatMultiplier", current_saturation)
				last_gain = current_gain
				last_offset = current_offset
				last_saturation = current_saturation
			updatePlayer(getPlayerPosRot())
			renderEntites(getEntites())
		else:
			Main_Filter.hide()
	else:
		Main_Filter.hide()
		#get_parent().get_node("UILayer/UI").updateSpells(Global.MBEX.getActiveSpells())
		#get_parent().get_node("UILayer/UI").updateSelectedSpells(Global.MBEX.getSelectedSpells())
		#get_parent().get_node("UILayer/UI").updateMinimap(Global.MBEX.getMinimap())
	Global.Main_Sounds.updateSounds(Global.MBEX.getPendingSoundActions())	
	return result

func anim1Step() -> int:
	getInputs()
	Global.MBEX.updateFreeSoundPlayers(Global.Main_Sounds.get_free_player_indices())	
	#var endAnimOut=Global.MBEX.playAnimStep(endAnimIn)
	
	#var endAnimOut=Global.MBEX.REMC2StepAnim(input_state)
	
	var result=Global.MBEX.REMC2Run(input_state,0)
	var endAnimOut=false;
	
	Global.Main_Sounds.updateSounds(Global.MBEX.getPendingSoundActions())
	return endAnimOut

#func anim1End():
	#Global.MBEX.REMC2EndAnim()
	##Global.MBEX.REMC2EndItem()

func mainMenuBegin(ScrBufferRect:TextureRect):
	Global.MBEX.REMC2BeginMain(ScrBufferRect)

func mainMenuStep() -> int:
	getInputs()
	Global.MBEX.updateFreeSoundPlayers(Global.Main_Sounds.get_free_player_indices())	
	var mapMenuOut=Global.MBEX.REMC2StepMain(input_state)
	Global.Main_Sounds.updateSounds(Global.MBEX.getPendingSoundActions())
	return mapMenuOut

func exitGame():
	Global.MBEX.REMC2EndGame()

#func mainenuEnd():
	#Global.MBEX.REMC2EndMain()

func mapMenuBegin(ScrBufferRect:TextureRect):
	#Global.MBEX.updateFreeSoundPlayers(Main_Sounds.get_free_player_indices())
	Global.MBEX.REMC2BeginMap(ScrBufferRect)

func mapMenuStep() -> int:
	getInputs()
	Global.MBEX.updateFreeSoundPlayers(Global.Main_Sounds.get_free_player_indices())	
	var mapMenuOut=Global.MBEX.REMC2StepMap(input_state)
	Global.Main_Sounds.updateSounds(Global.MBEX.getPendingSoundActions())
	return mapMenuOut

#func mapMenuEnd():
	#Global.MBEX.REMC2EndMap()
	#Global.MBEX.updateFreeSoundPlayers(Main_Sounds.get_free_player_indices())	
	#var mapMenuOut=Global.MBEX.REMC2StepMap(endMapMenu)
	#Main_Sounds.updateSounds(Global.MBEX.getPendingSoundActions())
	#return mapMenuOut
	
func inGameBegin():
	#Global.MBEX.updateFreeSoundPlayers(Main_Sounds.get_free_player_indices())
	#var buffer=Global.MBEX.REMC2BeginInGame()
	#Global.setLoadingScreen(buffer)
	#Global.MBEX.REMC2BeginInGameAfterScreen()
	#fadeNode = Global.addFadeIn(fadeNode)
	#await fadeNode.fade_finished
	#Global.setLoadingScreenStr("")
	#fadeNode = Global.addFadeOut(fadeNode)
	Global.setLevelType(Global.MBEX.REMC2GetLevelType())
	nextState=0

func setMesh():
	Global.MBEX.set_mesh_instances(get_parent().get_node("TerrainsMB").mesh_instance_bottom,get_parent().get_node("TerrainsMB").mesh_instance_top,Global.getLevelType()=="Cave")
	Global.MBEX.initialize_grid_data()
	Global.MBEX.recalculate_mesh(Global.getLevelType()=="Cave")

#func loadlevel(levelnumber: int):
	#sub_533B0_decompress_levels(levelnumber)

func getVGABuffer():
	return Global.MBEX.getVGABuffer()

func MBEXinit():
	if !Global.MBEX:
		Global.MBEX = MBEXclass.new()
		Global.MBEX.REMC2BeginGame(Global.cdPath,Global.hidata)
		#Global.MBEX.REMC2SetCDPath(Global.cdPath)

func MBEXconvert(path, path2):
	Global.MBEX.convertOriginalData(path,path2)

func MBEXextractCD(path, path2):
	Global.MBEX.convertOriginalDataExtractCD(path,path2)

#func sub_533B0_decompress_levels(level_id: int):
	#if level_id >= 1000:
		#return true
	#var level_dat_file: FileAccess = null
	#var level_tab_file: FileAccess = null
	#level_dat_file = FileAccess.open(levels_dat_path, FileAccess.READ)
#
	#if level_dat_file == null:
		#levels_dat_path = ProjectSettings.get_setting("global_paths/cd_folder") + "/LEVELS/LEVELS.DAT"
		#level_dat_file = FileAccess.open(levels_dat_path, FileAccess.READ)
#
	#if level_dat_file == null:
		#return false
	#level_tab_file = FileAccess.open(levels_tab_path, FileAccess.READ)
	#level_dat_file = FileAccess.open(levels_dat_path, FileAccess.READ)
	#
	#var level_tab_file_arr: Array[int] = []
	#while not level_tab_file.eof_reached():
		#level_tab_file_arr.append(level_tab_file.get_32())   # načte 4 byty jako int32
	#level_tab_file.close()
	#var level_start: int = level_tab_file_arr[level_id]
	#var compressed_size: int = level_tab_file_arr[level_id+1]-level_tab_file_arr[level_id]
	#
	#level_dat_file.seek(level_start)
	#var level_tab_data: PackedByteArray = level_dat_file.get_buffer(compressed_size)
	#level_dat_file.close()
	#
	#MBEXinit()
	#var level_tab_data_unpacked:PackedByteArray = Global.MBEX.deRNC(level_tab_data)
	#Global.MBEX.TerrainMake(level_tab_data_unpacked,Global.cdPath)
	

#func initTerrainObsolette():
	#mapHeightmap_11B4E0 = Global.MBEX.TerrainGetMapHeight()
	#mapTerrainType_10B4E0 = Global.MBEX.TerrainGetMapTerrainType()
	#mapAngle_13B4E0 = Global.MBEX.TerrainGetAngle()
	

func getPlayerPosRot() -> Dictionary:
	var playerPosRot: Dictionary = Global.MBEX.GetPlayerPositionRotation()
	return playerPosRot
	
func getEntites() -> Array:
	var result: Array = Global.MBEX.GetEntites()
	return result
	
#func getTerrainChanges() -> Dictionary:
	#var result: Dictionary = Global.MBEX.GetTerrainChanges()
	#return result

var mapTerrainType_10B4E0: PackedByteArray = PackedByteArray()
var mapHeightmap_11B4E0: PackedByteArray = PackedByteArray()
var mapShading_12B4E0: PackedByteArray = PackedByteArray()
var mapAngle_13B4E0: PackedByteArray = PackedByteArray()
var mapEntityIndex_15B4E0: PackedInt32Array = PackedInt32Array()

func generate_level_map_43830() -> void:
	mapTerrainType_10B4E0.resize(256 * 256)
	mapHeightmap_11B4E0.resize(256 * 256)
	mapShading_12B4E0.resize(256 * 256)
	mapAngle_13B4E0.resize(256 * 256)
	mapEntityIndex_15B4E0.resize(256 * 256)
	mapTerrainType_10B4E0.fill(0)
	mapHeightmap_11B4E0.fill(0)
	mapShading_12B4E0.fill(0)
	mapAngle_13B4E0.fill(0)
	mapEntityIndex_15B4E0.fill(0)
	
func MBgetWarpMouse():
	return Global.MBEX.REMC2getWarpMouse()
