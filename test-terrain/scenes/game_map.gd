extends Node3D

@export var sprite_pool_size := 200
var sprite_pool: Array[Sprite2D] = []
var used_count: int = 0

@onready var gamemap = $Control/GameMap
@onready var foreground = $Control/Foreground

@onready var sprAnimations = $Control/Animation

var scroll_speed = 2000.0
var edge_margin = 50.0

var runned = false

var fadeNode: Node3D

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

var SPRITE_DIR6 = Global.convertdata+"HSCREEN/6/"
var SPRITE_DIR = Global.convertdata+"HSCREEN/"

var Main_DecodeLevel
var Main_Sounds
var MainMusic
func _ready():
	await get_tree().process_frame
	Engine.max_fps = 60
	var screen_size = get_viewport().get_visible_rect().size
	#foreground.position = screen_size / 2
	gamemap.position = screen_size / 2
	#setup_sprites()#remove it
	_init_sprite_pool()
	Main_DecodeLevel = get_node("DecodeLevel")
	Main_Sounds = $Sounds
	Main_Sounds.MainMusic = get_node("Sounds").get_node("MidiPlayer")
	Main_Sounds.MainMusicHi = get_node("Sounds").get_node("AudioStreamPlayer")
	Main_DecodeLevel.Main_Sounds = Main_Sounds	
	fadeNode = Global.addFadeOut(fadeNode)
	startMenuLoop()
	
func _init_sprite_pool():
	for i in sprite_pool_size:
		var new_sprite = Sprite2D.new()
		new_sprite.centered = false
		new_sprite.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
		new_sprite.texture_repeat = CanvasItem.TEXTURE_REPEAT_DISABLED
		new_sprite.centered = false
		new_sprite.visible = false
		$Control/GameMap.add_child(new_sprite)
		sprite_pool.append(new_sprite)

func startMenuLoop():
	gamemap.texture=Global.load_custom_texture(SPRITE_DIR + "gameWorldMap.png")
	foreground.texture=Global.load_custom_texture(SPRITE_DIR + "gameWorldMapForeground.png")
	runned = true
	menuInit()
	
func menuInit():
	Main_DecodeLevel.init()
	Main_DecodeLevel.initSound()
	Main_Sounds.setSoundBank(0)
	Main_DecodeLevel.mapMenuInit()
	
func endAnim():
	runned=false
	fadeNode = Global.addFadeIn(fadeNode)
	await Global.fadeNode.fade_finished
	get_tree().change_scene_to_file(Global.last_scene_path)

func render_spriteB(posx: int, posy: int, index: int):
	var file_name = "%03d.png" % index
	var file_path = SPRITE_DIR6 + file_name
	var tex = Global.load_custom_texture(file_path)
	render_sprite(tex, Vector2(posx-640, posy-480))

func render_sprite(texture: Texture2D,pos: Vector2) -> void:
	if used_count >= sprite_pool.size():
		return
	var s := sprite_pool[used_count]
	used_count += 1
	s.texture = texture
	s.position = pos
	s.visible = true

func endSpritesrender():
	for i in range(used_count, sprite_pool.size()):
		sprite_pool[i].visible = false

func updateSprites(graphicsActions:Array):
	used_count = 0
	for action_dict in graphicsActions:
		if not action_dict is Dictionary:
			continue
		var action: String = action_dict.get("action", "")
		var posx: int = action_dict.get("posx", 0)
		var posy: int = action_dict.get("posy", 0)
		var index: int = action_dict.get("index", 0)
		var matchok=false
		match action:
			"drawBitmap":
				render_spriteB(posx, posy, index)
				matchok=true
			"drawTextBox":
				matchok=true
		if(!matchok):
			matchok=true

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
		updateSprites(Main_DecodeLevel.getSpritesActions())
		endSpritesrender()

func setup_sprites():
	for data in SPRITE_DATA_END:#for end game vulcan if (mapScreenPortals_E17CC[24].activated_18 == 1)
		var pos_x = data["pos_x"]
		var pos_y = data["pos_y"]
		var sprite_idx = data["index"]
		var new_sprite = Sprite2D.new()
		var file_name = "%03d.png" % sprite_idx
		var file_path = SPRITE_DIR6 + file_name
		var tex = Global.load_custom_texture(file_path)
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
