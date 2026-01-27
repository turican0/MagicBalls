extends Node3D

@onready var gamemap = $Control/GameMap
@onready var foreground = $Control/Foreground

@onready var sprAnimations = $Control/Animation

var scroll_speed = 2000.0
var edge_margin = 50.0

var runned = false

#NewGameDialog_77350

const SPRITE_DATA_END = [
	{"pos_x": 518, "pos_y": 17, "index": 285},
	{"pos_x": 583, "pos_y": 17, "index": 286},
	{"pos_x": 657, "pos_y": 17, "index": 287},
	{"pos_x": 696, "pos_y": 17, "index": 288},
	{"pos_x": 518, "pos_y": 88, "index": 289},
	{"pos_x": 574, "pos_y": 88, "index": 290},
	{"pos_x": 657, "pos_y": 88, "index": 291},
	{"pos_x": 706, "pos_y": 88, "index": 292},
	{"pos_x": 518, "pos_y": 156, "index": 293},
	{"pos_x": 582, "pos_y": 156, "index": 294},
	{"pos_x": 657, "pos_y": 156, "index": 295},
	{"pos_x": 703, "pos_y": 156, "index": 296}
]

const MAP_SCREEN_PORTALS_E17CC = [
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 116, "posy": 478, "sizex": 0x0028, "sizey": 0x0028, "targetx": 420, "targety": 820, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 368, "posy": 478, "sizex": 0x0028, "sizey": 0x0028, "targetx": 666, "targety": 805, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 576, "posy": 478, "sizex": 0x0028, "sizey": 0x0028, "targetx": 881, "targety": 734, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 260, "posy": 402, "sizex": 0x0028, "sizey": 0x0028, "targetx": 549, "targety": 626, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 260, "posy": 402, "sizex": 0x0028, "sizey": 0x0028, "targetx": 450, "targety": 652, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 304, "posy": 402, "sizex": 0x0028, "sizey": 0x0028, "targetx": 610, "targety": 666, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 304, "posy": 402, "sizex": 0x0028, "sizey": 0x0028, "targetx": 763, "targety": 652, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 304, "posy": 402, "sizex": 0x0028, "sizey": 0x0028, "targetx": 732, "targety": 558, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 304, "posy": 402, "sizex": 0x0028, "sizey": 0x0028, "targetx": 644, "targety": 554, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 304, "posy": 402, "sizex": 0x0028, "sizey": 0x0028, "targetx": 536, "targety": 540, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 512, "posy": 306, "sizex": 0x0028, "sizey": 0x0028, "targetx": 822, "targety": 450, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 638, "posy": 190, "sizex": 0x0028, "sizey": 0x0028, "targetx": 1009, "targety": 412, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 638, "posy": 92,  "sizex": 0x0028, "sizey": 0x0028, "targetx": 1058, "targety": 268, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 478, "posy": 92,  "sizex": 0x0028, "sizey": 0x0028, "targetx": 901,  "targety": 304, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 478, "posy": 92,  "sizex": 0x0028, "sizey": 0x0028, "targetx": 817,  "targety": 202, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 478, "posy": 92,  "sizex": 0x0028, "sizey": 0x0028, "targetx": 684,  "targety": 262, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 122, "posy": 96,  "sizex": 0x0028, "sizey": 0x0028, "targetx": 530,  "targety": 316, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 122, "posy": 96,  "sizex": 0x0028, "sizey": 0x0028, "targetx": 427,  "targety": 206, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 122, "posy": 96,  "sizex": 0x0028, "sizey": 0x0028, "targetx": 322,  "targety": 254, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 306, "posy": 196, "sizex": 0x0028, "sizey": 0x0028, "targetx": 627,  "targety": 416, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 1,   "posy": 68,  "sizex": 0x0028, "sizey": 0x0028, "targetx": 180,  "targety": 278, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 296, "posy": 68,  "sizex": 0x0028, "sizey": 0x0028, "targetx": 609,  "targety": 218, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 480, "posy": 0,   "sizex": 0x0028, "sizey": 0x0028, "targetx": 838,  "targety": 96,  "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 308, "posy": 0,   "sizex": 0x0028, "sizey": 0x0028, "targetx": 679,  "targety": 126, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 308, "posy": 0,   "sizex": 0x0028, "sizey": 0x0028, "targetx": 605,  "targety": 120, "type": 0x0021, "idx": 2, "flags": 0x00, "extra": 0x0000},
	{"unk1": 0x0000, "unk2": 0x0000, "posx": 0,   "posy": 0,   "sizex": 0x0000, "sizey": 0x0000, "targetx": 0,    "targety": 0,    "type": 0x0000, "idx": 0, "flags": 0x00, "extra": 0x0000}
]

const main_menu_animations = [
	{
		"dword_0": 0x00000000,
		"word_10": 0x0025,
		"byte_12": 0x02,
		"word_14": 0x0025,
		"byte_16": 0x03,
		"array_word_18": [0x0000, 0x0000, 0x0180, 0x0000, 0x021F, 0x03C0, 0x008C, 0x0091, 0x008C, 0x008C, 0x0091, 0x0032],
		"byte_42": 0x01,
		"byte_43": 0x00,
		"byte_44": 0x03,
		"array_word_45": [0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000]
	},
	{
		"dword_0": 0x00000000,
		"word_10": 0x0025,
		"byte_12": 0x02,
		"word_14": 0x0025,
		"byte_16": 0x03,
		"array_word_18": [0x0000, 0x0000, 0x018B, 0x000A, 0x0226, 0x03CA, 0x008C, 0x0091, 0x008C, 0x008C, 0x0091, 0x0032],
		"byte_42": 0x01,
		"byte_43": 0x00,
		"byte_44": 0x03,
		"array_word_45": [0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000]
	},
	{
		"dword_0": 0x00000000,
		"word_10": 0x0025,
		"byte_12": 0x02,
		"word_14": 0x0025,
		"byte_16": 0x03,
		"array_word_18": [0x0000, 0x0000, 0x0190, 0x0006, 0x022B, 0x03D4, 0x008C, 0x0091, 0x008C, 0x008C, 0x0091, 0x0032],
		"byte_42": 0x01,
		"byte_43": 0x00,
		"byte_44": 0x03,
		"array_word_45": [0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000]
	},
	{
		"dword_0": 0x00000000,
		"word_10": 0x0027,
		"byte_12": 0x02,
		"word_14": 0x0027,
		"byte_16": 0x03,
		"array_word_18": [0x0000, 0x0000, 0x0001, 0x00BE, 0x04FC, 0x0134, 0x00CE, 0x00D3, 0x00CE, 0x00CE, 0x00D3, 0x0028],
		"byte_42": 0x01,
		"byte_43": 0x00,
		"byte_44": 0x03,
		"array_word_45": [0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000]
	},
	{
		"dword_0": 0x00000000,
		"word_10": 0x0025,
		"byte_12": 0x02,
		"word_14": 0x0025,
		"byte_16": 0x03,
		"array_word_18": [0x0000, 0x0000, 0x01CB, 0x0000, 0x04FE, 0x03C0, 0x0092, 0x0097, 0x0092, 0x0092, 0x0097, 0x0014],
		"byte_42": 0x01,
		"byte_43": 0x00,
		"byte_44": 0x03,
		"array_word_45": [0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000]
	},
	{
		"dword_0": 0x00000000,
		"word_10": 0x0009,
		"byte_12": 0x02,
		"word_14": 0x0003,
		"byte_16": 0x03,
		"array_word_18": [0x0000, 0x0000, 0x0253, 0x0000, 0x023A, 0x027C, 0x0115, 0x011C, 0x0115, 0x005D, 0x0067, 0x0000],
		"byte_42": 0x02,
		"byte_43": 0x00,
		"byte_44": 0x03,
		"array_word_45": [0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000]
	},
	{
		"dword_0": 0x00000000,
		"word_10": 0x0009,
		"byte_12": 0x02,
		"word_14": 0x0003,
		"byte_16": 0x03,
		"array_word_18": [0x0000, 0x0000, 0x0262, 0x0072, 0x0262, 0x0072, 0x0129, 0x0130, 0x0129, 0x0129, 0x0130, 0x0000],
		"byte_42": 0x03,
		"byte_43": 0x00,
		"byte_44": 0x18,
		"array_word_45": [0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000]
	},
	{
		"dword_0": 0x00000000,
		"word_10": 0x0009,
		"byte_12": 0x02,
		"word_14": 0x0003,
		"byte_16": 0x03,
		"array_word_18": [0x0000, 0x0000, 0x0165, 0x0000, 0x011F, 0x0290, 0x006B, 0x0072, 0x006B, 0x005D, 0x0067, 0x000F],
		"byte_42": 0x01,
		"byte_43": 0x00,
		"byte_44": 0x00,
		"array_word_45": [0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000]
	},
	{
		"dword_0": 0x00000000,
		"word_10": 0x0009,
		"byte_12": 0x02,
		"word_14": 0x0003,
		"byte_16": 0x03,
		"array_word_18": [0x0000, 0x0000, 0x03AB, 0x0000, 0x036F, 0x0266, 0x006B, 0x0072, 0x006B, 0x005D, 0x0067, 0x000F],
		"byte_42": 0x01,
		"byte_43": 0x00,
		"byte_44": 0x00,
		"array_word_45": [0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000]
	},
	{
		"dword_0": 0x00000000,
		"word_10": 0x0009,
		"byte_12": 0x02,
		"word_14": 0x0003,
		"byte_16": 0x03,
		"array_word_18": [0x0000, 0x0000, 0x01AF, 0x0000, 0x0169, 0x0200, 0x006B, 0x0072, 0x006B, 0x005D, 0x0067, 0x000B],
		"byte_42": 0x01,
		"byte_43": 0x00,
		"byte_44": 0x00,
		"array_word_45": [0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000]
	},
	{
		"dword_0": 0x00000000,
		"word_10": 0x0009,
		"byte_12": 0x02,
		"word_14": 0x0003,
		"byte_16": 0x03,
		"array_word_18": [0x0000, 0x0000, 0x01C7, 0x0000, 0x018B, 0x0072, 0x006B, 0x0072, 0x006B, 0x005D, 0x0067, 0x0012],
		"byte_42": 0x01,
		"byte_43": 0x00,
		"byte_44": 0x00,
		"array_word_45": [0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000]
	},
	{
		"dword_0": 0x00000000,
		"word_10": 0x0000,
		"byte_12": 0x00,
		"word_14": 0x0000,
		"byte_16": 0x00,
		"array_word_18": [0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000],
		"byte_42": 0x00,
		"byte_43": 0x00,
		"byte_44": 0x00,
		"array_word_45": [0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000]
	}
]

#const TEXTURE_PATH = "res://convertdata/HSCREEN/6/%03d.png"
const SPRITE_DIR6 = "res://convertdata/HSCREEN/6/"
const SPRITE_DIR = "res://convertdata/HSCREEN/"

func load_custom_texture(path: String) -> ImageTexture:
	var img = Image.load_from_file(path)
	if img == null:
		return null
	var tex = ImageTexture.create_from_image(img)
	return tex

var fadeNode: Node3D
func fadeInit():
	if(!get_tree().root.get_node_or_null("FadeInOut")):
		var fade_layer_scene = preload("res://scenes/FadeInOut.tscn")
		var new_layer = fade_layer_scene.instantiate()
		get_tree().root.add_child(new_layer)
		new_layer.name = "FadeInOut"
	if(!fadeNode):
		fadeNode=get_tree().root.get_node_or_null("FadeInOut")
func addFadeIn():
	fadeInit()
	fadeNode.start_fade(1.0, Color(0, 0, 0, 0),Color(0, 0, 0, 1))
func addFadeOut():
	fadeInit()
	fadeNode.start_fade(1.0, Color(0, 0, 0, 1),Color(0, 0, 0, 0))

var Main_DecodeLevel
var Main_Sounds
var MainMusic
func _ready():
	await get_tree().process_frame
	var screen_size = get_viewport().get_visible_rect().size
	#foreground.position = screen_size / 2
	gamemap.position = screen_size / 2
	setup_sprites()
	setup_animations()
	#Engine.max_fps = 60
	Main_DecodeLevel = get_node("DecodeLevel")
	Main_Sounds = $Sounds
	Main_Sounds.MainMusic = get_node("Sounds").get_node("MidiPlayer")
	Main_Sounds.MainMusicHi = get_node("Sounds").get_node("AudioStreamPlayer")
	Main_DecodeLevel.Main_Sounds = Main_Sounds	
	addFadeOut()
	startMenuLoop()

func startMenuLoop():
	gamemap.texture=load_custom_texture(SPRITE_DIR + "gameWorldMap.png")
	foreground.texture=load_custom_texture(SPRITE_DIR + "gameWorldMapForeground.png")
	runned = true
	menuInit()
	
func menuInit():
	Main_DecodeLevel.init()
	Main_DecodeLevel.initSound()
	Main_Sounds.setSoundBank(0)
	Main_DecodeLevel.mapMenuInit()
	
func endAnim():
	runned=false
	addFadeIn()
	await fadeNode.fade_finished
	get_tree().change_scene_to_file(Global.last_scene_path)

func _process(delta) -> void:
	mouseEvents(delta)
	if(!runned):
		return
	var is_skipping = false
	#var is_skipping = Input.is_anything_pressed() or \
					  #Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT) or \
					  #Input.is_mouse_button_pressed(MOUSE_BUTTON_RIGHT)
	if(is_skipping):
		Main_DecodeLevel.mapMenuStep(1)
		Main_Sounds.stopAllSounds()
		endAnim()
	else:
		var mapMenuStruct = Main_DecodeLevel.mapMenuStep(0)

func setup_animations():
	for cfg in main_menu_animations:
		var ani = AnimatedSprite2D.new()
		#ani.name = cfg["name"]
		ani.centered = false
		ani.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
		ani.texture_repeat = CanvasItem.TEXTURE_REPEAT_DISABLED
		ani.position = Vector2(cfg["array_word_18"][0]+100, cfg["array_word_18"][1]+100)
		var frames = SpriteFrames.new()
		frames.remove_animation("default")
		frames.add_animation("loop")
		for i in range(cfg["array_word_18"][8], cfg["array_word_18"][8]):
			var file_name = "%03d.png" % i
			var file_path = SPRITE_DIR6 + file_name
			var tex2 = load_custom_texture(file_path)
			if tex2:
				frames.add_frame("loop", tex2)
			else:
				push_error("Chybí soubor: " + file_path)
		frames.set_animation_speed("loop", 15.0)
		frames.set_animation_loop("loop", true)
		ani.sprite_frames = frames
		$Control.add_child(ani)
		ani.play("loop")
	
func setup_sprites():
	for data in SPRITE_DATA_END:#for end game vulcan if (mapScreenPortals_E17CC[24].activated_18 == 1)
		var pos_x = data["pos_x"]
		var pos_y = data["pos_y"]
		var sprite_idx = data["index"]
		var new_sprite = Sprite2D.new()
		var file_name = "%03d.png" % sprite_idx
		var file_path = SPRITE_DIR6 + file_name
		var tex = load_custom_texture(file_path)
		new_sprite.centered = false
		new_sprite.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
		new_sprite.texture_repeat = CanvasItem.TEXTURE_REPEAT_DISABLED
		new_sprite.texture = tex
		new_sprite.position = Vector2(pos_x-640, pos_y-480)
		new_sprite.centered = false
		$Control/GameMap.add_child(new_sprite)
		new_sprite.name = "Sprite_" + str(sprite_idx)

func mouseEvents(delta):
	var mouse_pos = get_viewport().get_mouse_position()
	var screen_size = get_viewport().get_visible_rect().size
	var move_vector = Vector2.ZERO
	if mouse_pos.x < edge_margin:
		move_vector.x = 1
	elif mouse_pos.x > screen_size.x - edge_margin:
		move_vector.x = -1
	if mouse_pos.y < edge_margin:
		move_vector.y = 1
	elif mouse_pos.y > screen_size.y - edge_margin:
		move_vector.y = -1
	if move_vector != Vector2.ZERO:
		gamemap.position += move_vector.normalized() * scroll_speed * delta
		_check_map_limits(screen_size)

func _check_map_limits(screen_size):
	var visible_map_width = screen_size.x / $Control.scale.x
	var visible_map_height = screen_size.y / $Control.scale.y
	var max_x = 640.0 * 2
	var min_x = visible_map_width - 640.0 * 2
	var max_y = 480.0 * 2
	var min_y = visible_map_height - 480.0 * 2
	gamemap.position.x = clamp(gamemap.position.x, min_x, max_x)
	gamemap.position.y = clamp(gamemap.position.y, min_y, max_y)
