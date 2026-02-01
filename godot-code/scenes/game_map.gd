extends Node3D

@export var sprite_pool_size := 200
var sprite_pool: Array[Sprite2D] = []
var used_count: int = 0

@onready var gamemap = $Control/GameMap
@onready var foreground = $Control/Foreground

var tooltip_label: Label

var scroll_speed = 2000.0
var edge_margin = 50.0

var runned = false

var fadeNode: Node3D

#NewGameDialog_77350

var SPRITE_DIR6 = Global.convertdata+"HSCREEN/6/"
var SPRITE_DIR = Global.convertdata+"HSCREEN/"

var map_menu_selection: Array[Dictionary] = [
	{"textIndex_0":0x01D2, "help": "Exit", "dword_0": 0x0025F620, "dword_4": 0x00000000, "selected_8": 0x0000, "xmin_10": 0x0245, "ymin_12": 0x01AB, "sizex_14": 0x0028, "sizey_16": 0x0046, "word_18": 0x0000, "byte_20": 0xF7, "byte_21": 0xF6, "byte_22": 0x0B, "canSelect_23": 0x01, "gold_color_24": 0x00, "byte_25": 0x01, "str_26": [0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0003]},
	{"textIndex_0":0x01D3, "help": "Restart Game", "dword_0": 0x0025F640, "dword_4": 0x00000000, "selected_8": 0x0000, "xmin_10": 0x0000, "ymin_12": 0x01AB, "sizex_14": 0x0028, "sizey_16": 0x0046, "word_18": 0x0000, "byte_20": 0xF9, "byte_21": 0xF8, "byte_22": 0x10, "canSelect_23": 0x01, "gold_color_24": 0x00, "byte_25": 0x01, "str_26": [0x0025, 0x015C, 0x0000, 0x0000, 0x003C, 0x0000, 0x01D3, 0x0000, 0x0003]},
	{"textIndex_0":0x0196, "help": "Load Game", "dword_0": 0x0025F800, "dword_4": 0x00000000, "selected_8": 0x0000, "xmin_10": 0x0000, "ymin_12": 0x0000, "sizex_14": 0x0028, "sizey_16": 0x0046, "word_18": 0x0196, "byte_20": 0xFB, "byte_21": 0xFA, "byte_22": 0x09, "canSelect_23": 0x01, "gold_color_24": 0x00, "byte_25": 0x01, "str_26": [0x001D, 0x003C, 0x0000, 0x0000, 0x00C8, 0x0000, 0x01A6, 0x0000, 0x0003]},
	{"textIndex_0":0x0195, "help": "Save Game", "dword_0": 0x0025F820, "dword_4": 0x00000000, "selected_8": 0x0000, "xmin_10": 0x0245, "ymin_12": 0x0000, "sizex_14": 0x0028, "sizey_16": 0x0046, "word_18": 0x0195, "byte_20": 0xFD, "byte_21": 0xFC, "byte_22": 0x08, "canSelect_23": 0x01, "gold_color_24": 0x00, "byte_25": 0x01, "str_26": [0x01FE, 0x003C, 0x0000, 0x0000, 0x00C8, 0x0000, 0x01A5, 0x0000, 0x0003]}
]

#type_E24BCx str_E2516[5] = {
#{0x01D2,0x012F,0x0169,0x01DC,0x016A,0x0000,0x023C,0x01B8,0x02,0x0B},
#{0x01D3,0x007F,0x0168,0x007B,0x0176,0x0000,0x0034,0x01B0,0x08,0x10},
#{0x0196,0x007E,0x0041,0x007B,0x0052,0x0000,0x0037,0x001C,0x08,0x09},
#{0x0195,0x015E,0x0041,0x0211,0x0052,0x0000,0x0254,0x001C,0x08,0x08},
#{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x00,0x00} };

var Main_DecodeLevel
#var Main_Sounds
#var Main_Music
func _ready():
#	545 54 116 478
#	999 -312
	await get_tree().process_frame
	Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
	Engine.max_fps = 60
	
	tooltip_label = Label.new()
	tooltip_label.z_index = 100 # Aby byla nápověda vždy nahoře
	tooltip_label.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
	tooltip_label.add_theme_color_override("font_outline_color", Color.BLACK)
	tooltip_label.add_theme_constant_override("outline_size", 4)	
	$Control.add_child(tooltip_label)
	tooltip_label.hide()
	
	var screen_size = get_viewport().get_visible_rect().size
	gamemap.position = screen_size / 2
	var centerPos=Vector2((640/2-116)*-1,(480/2-478)*-1)
	gamemap.position-=centerPos*$Control/GameMap.scale
	_init_sprite_pool()
	Main_DecodeLevel = get_node("DecodeLevel")
	Global.Main_Sounds = $Sounds
	Global.Main_Sounds.MainMusic = get_node("Sounds").get_node("MidiPlayer")
	Global.Main_Sounds.MainMusicHi = get_node("Sounds").get_node("AudioStreamPlayer")
	fadeNode = Global.addFadeOut(fadeNode)
	
	for cfgback in map_menu_selection:
		var sprback = Sprite2D.new()
		var file_name_spr = "%03d.png" % (cfgback["byte_21"])
		sprback.name = file_name_spr
		sprback.centered = false
		sprback.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
		sprback.texture_repeat = CanvasItem.TEXTURE_REPEAT_DISABLED
		sprback.scale=$Control/Foreground.size/Vector2(640,480)
		sprback.position = Vector2(cfgback["xmin_10"]*sprback.scale.x, cfgback["ymin_12"]*sprback.scale.y)
		var file_path_spr = SPRITE_DIR6 + file_name_spr
		var tex2 = Global.load_custom_texture(file_path_spr)
		sprback.texture=tex2
		$Control.add_child(sprback)
		
	for cfg2 in map_menu_selection:
		var spr = Sprite2D.new()
		var file_name_spr = "%03d.png" % (cfg2["byte_21"]+1)
		spr.name = file_name_spr
		spr.centered = false
		spr.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
		spr.texture_repeat = CanvasItem.TEXTURE_REPEAT_DISABLED
		spr.scale=$Control/Foreground.size/Vector2(640,480)
		spr.position = Vector2(cfg2["xmin_10"]*spr.scale.x, cfg2["ymin_12"]*spr.scale.y)
		var file_path_spr = SPRITE_DIR6 + file_name_spr
		var tex2 = Global.load_custom_texture(file_path_spr)
		spr.texture=tex2
		spr.hide()
		$Control.add_child(spr)
		cfg2["node_ref"] = spr
	
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
	Global.initSound()
	Global.Main_Sounds.setSoundBank(0)
	Main_DecodeLevel.mapMenuBegin()
	Main_DecodeLevel.changeLanguage(2)
	Main_DecodeLevel.getLangTexts()
	
#func endAnim():
	#runned=false
	#fadeNode = Global.addFadeIn(fadeNode)
	#await Global.fadeNode.fade_finished
	#get_tree().change_scene_to_file(Global.last_scene_path)

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

#func endSpritesrender():
	#for i in range(used_count, sprite_pool.size()):
		#sprite_pool[i].visible = false

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
	#print("Map.Pos:%d:%d" % [gamemap.position.x, gamemap.position.y])
	mouseEvents(delta)
	if(!runned):
		return
	#var is_skipping = false
	##var is_skipping = Input.is_anything_pressed() or \
					  ##Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT) or \
					  ##Input.is_mouse_button_pressed(MOUSE_BUTTON_RIGHT)
	#if(is_skipping):
		#Main_DecodeLevel.mapMenuEnd()
		#Main_Sounds.stopAllSounds()
		#endAnim()
	#else:
	var mapMenuStruct = Main_DecodeLevel.mapMenuStep()
	updateSprites(Main_DecodeLevel.getSpritesActions())
		#endSpritesrender()
		
func endMapMenu():
	Main_DecodeLevel.mapMenuEnd()
	Global.Main_Sounds.stopAllSounds()
	#endAnim()

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

func _input(event):
	await get_tree().process_frame	
	var mouse_pos = $Control.get_local_mouse_position()
	var any_hovered = false
	for cfg2 in map_menu_selection:
		var spr = cfg2["node_ref"]
		if not spr: continue
		var size = spr.texture.get_size() * spr.scale
		var rect = Rect2(spr.position, size)
		if rect.has_point(mouse_pos):
			any_hovered = true
			spr.show()
			#tooltip_label.text = "ID: 0x%X" % cfg2["dword_0"]
			tooltip_label.text = cfg2["help"]
			tooltip_label.text = Global.langTexts[cfg2["textIndex_0"]]
			tooltip_label.show()
			tooltip_label.position = Vector2(floor(mouse_pos.x + 10), floor(mouse_pos.y + 10))
			
			if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT and event.pressed:
				var target_scene = ""
				if cfg2["dword_0"] == 0x0025F640:
					target_scene = "res://scenes/GameMap.tscn"
				elif cfg2["dword_0"] == 0x00259E00:
					target_scene = "res://scenes/PlayAnim.tscn"
				if target_scene != "":
					fadeNode = Global.addFadeIn(fadeNode)
					endMapMenu()
					await fadeNode.fade_finished
					Global.last_scene_path = get_tree().current_scene.scene_file_path
					get_tree().change_scene_to_file(target_scene)
				else:
					print("Kliknuto na: 0x%X" % cfg2["dword_0"])
		else:
			spr.hide()
	if not any_hovered:
		tooltip_label.hide()

func _check_map_limits(screen_size):
	var visible_map_width = screen_size.x / $Control.scale.x
	var visible_map_height = screen_size.y / $Control.scale.y
	var max_x = 640.0 * $Control/GameMap.scale.x
	var min_x = visible_map_width - 640.0 * $Control/GameMap.scale.x
	var max_y = 480.0 * $Control/GameMap.scale.y
	var min_y = visible_map_height - 480.0 * $Control/GameMap.scale.y
	gamemap.position.x = clamp(gamemap.position.x, min_x, max_x)
	gamemap.position.y = clamp(gamemap.position.y, min_y, max_y)
