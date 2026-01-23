extends Node3D

var tooltip_label: Label

var main_menu_animations = [
	{"name": "FireLeft","pos_x": 17,  "pos_y": 159, "first_sprite": 1,  "last_sprite": 8}, # left fire
	{"name": "FireRight","pos_x": 531, "pos_y": 156, "first_sprite": 9,  "last_sprite": 16}, # right fire
	{"name": "IncenseLeft","pos_x": 154, "pos_y": 308, "first_sprite": 17, "last_sprite": 25}, # left incense
	{"name": "IncenseRight","pos_x": 482, "pos_y": 308, "first_sprite": 26, "last_sprite": 34}  # right incense
]

var main_menu_selection: Array[Dictionary] = [
	{"help": "New Game", "dword_0": 0x00258350, "dword_4": 0, "selected_8": 0, "xmin_10": 0x00CE, "ymin_12": 0x0043, "sizex_14": 0x0050, "sizey_16": 0x0050, "word_18": 0x019A, "byte_20": 0x3B, "byte_21": 0x33, "byte_22": 0x06, "canSelect_23": 0x01, "gold_color_24": 0, "byte_25": 0, "str_26": [0x004E, 0x004E, 0, 0, 0x0050, 0, 0, 0, 0x0003]},
	{"help": "Set Name", "dword_0": 0x00259E00, "dword_4": 0, "selected_8": 0, "xmin_10": 0x0119, "ymin_12": 0x0041, "sizex_14": 0x0050, "sizey_16": 0x0050, "word_18": 0x0194, "byte_20": 0x3C, "byte_21": 0x34, "byte_22": 0x0A, "canSelect_23": 0x01, "gold_color_24": 0, "byte_25": 0, "str_26": [0x0164, 0x0070, 0, 0, 0x0050, 0, 0x01A4, 0, 0x0003]},
	{"help": "Multiplayer", "dword_0": 0x0025EE80, "dword_4": 0, "selected_8": 0, "xmin_10": 0x016A, "ymin_12": 0x0048, "sizex_14": 0x0050, "sizey_16": 0x0050, "word_18": 0x0199, "byte_20": 0x3D, "byte_21": 0x35, "byte_22": 0x07, "canSelect_23": 0x01, "gold_color_24": 0, "byte_25": 0, "str_26": [0x01AE, 0x004E, 0, 0, 0x0050, 0, 0x019C, 0, 0x0003]},
	{"help": "Load Game", "dword_0": 0x00259730, "dword_4": 0, "selected_8": 0, "xmin_10": 0x00C8, "ymin_12": 0x009D, "sizex_14": 0x0050, "sizey_16": 0x0050, "word_18": 0x0196, "byte_20": 0x3E, "byte_21": 0x36, "byte_22": 0x09, "canSelect_23": 0x01, "gold_color_24": 0, "byte_25": 0, "str_26": [0x004E, 0x00A0, 0, 0, 0x00C8, 0, 0x01A6, 0, 0x0003]},
	{"help": "Set Control", "dword_0": 0x0025A610, "dword_4": 0, "selected_8": 0, "xmin_10": 0x0195, "ymin_12": 0x00E7, "sizex_14": 0x003C, "sizey_16": 0x002C, "word_18": 0x01CF, "byte_20": 0x6A, "byte_21": 0x6A, "byte_22": 0x0F, "canSelect_23": 0x01, "gold_color_24": 0, "byte_25": 0, "str_26": [0x0160, 0x00AE, 0, 0, 0x0050, 0, 0, 0, 0x0003]},
	{"help": "Save Game", "dword_0": 0x002590F0, "dword_4": 0, "selected_8": 0, "xmin_10": 0x0187, "ymin_12": 0x009E, "sizex_14": 0x0050, "sizey_16": 0x0050, "word_18": 0x0195, "byte_20": 0x3F, "byte_21": 0x37, "byte_22": 0x08, "canSelect_23": 0x01, "gold_color_24": 0, "byte_25": 0, "str_26": [0x01C0, 0x00A0, 0, 0, 0x00C8, 0, 0x01A5, 0, 0x0003]},
	{"help": "Exit", "dword_0": 0x00258980, "dword_4": 0, "selected_8": 0, "xmin_10": 0x0126, "ymin_12": 0x0019, "sizex_14": 0x0034, "sizey_16": 0x002C, "word_18": 0x0197, "byte_20": 0x40, "byte_21": 0x38, "byte_22": 0x0B, "canSelect_23": 0x01, "gold_color_24": 0, "byte_25": 0, "str_26": [0x0160, 0x001A, 0, 0, 0x0050, 0, 0x0197, 0, 0x0003]},
	{"help": "Set Language", "dword_0": 0x002589E0, "dword_4": 0, "selected_8": 0, "xmin_10": 0x0121, "ymin_12": 0x009B, "sizex_14": 0x003C, "sizey_16": 0x002C, "word_18": 0x0198, "byte_20": 0x41, "byte_21": 0x39, "byte_22": 0x0C, "canSelect_23": 0x01, "gold_color_24": 0, "byte_25": 0, "str_26": [0x0160, 0x00AE, 0, 0, 0x0050, 0, 0, 0, 0x0003]},
	{"help": "Set Joystick", "dword_0": 0x0025A160, "dword_4": 0, "selected_8": 0, "xmin_10": 0x00B9, "ymin_12": 0x00E8, "sizex_14": 0x003C, "sizey_16": 0x002C, "word_18": 0x019B, "byte_20": 0x42, "byte_21": 0x3A, "byte_22": 0x0D, "canSelect_23": 0x01, "gold_color_24": 0, "byte_25": 0, "str_26": [0x004A, 0x00F6, 0, 0, 0x0064, 0, 0x019B, 0, 0x0003]}
]

const SPRITE_DIR = "res://convertdata/HSCREEN/"

func load_custom_texture(path: String) -> ImageTexture:
	var img = Image.load_from_file(path)
	if img == null:
		return null        
	var tex = ImageTexture.create_from_image(img)    
	return tex

func _ready():
	var file_name_cur = "%03d.png" % 39
	var file_path_cur = SPRITE_DIR + file_name_cur
	var cursor_png = load_custom_texture(file_path_cur)
	if cursor_png:
		Input.set_custom_mouse_cursor(cursor_png, Input.CURSOR_ARROW, Vector2(0, 0))
	
	tooltip_label = Label.new()
	tooltip_label.z_index = 100 # Aby byla nápověda vždy nahoře
	tooltip_label.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
	tooltip_label.add_theme_color_override("font_outline_color", Color.BLACK)
	tooltip_label.add_theme_constant_override("outline_size", 4)	
	$Control.add_child(tooltip_label)
	tooltip_label.hide()
	
	for cfg in main_menu_animations:
		var ani = AnimatedSprite2D.new()
		ani.name = cfg["name"]
		ani.centered = false
		ani.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
		ani.texture_repeat = CanvasItem.TEXTURE_REPEAT_DISABLED
		ani.position = Vector2(cfg["pos_x"]-320, cfg["pos_y"]-240)
		var frames = SpriteFrames.new()
		frames.remove_animation("default")
		frames.add_animation("loop")
		for i in range(cfg["first_sprite"], cfg["last_sprite"] + 1):
			var file_name = "%03d.png" % i
			var file_path = SPRITE_DIR + file_name
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
		
		for cfgback in main_menu_selection:
			var sprback = Sprite2D.new()
			var file_name_spr = "%03d.png" % (cfgback["byte_21"]+8)
			sprback.name = file_name_spr
			sprback.centered = false
			sprback.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
			sprback.texture_repeat = CanvasItem.TEXTURE_REPEAT_DISABLED
			sprback.position = Vector2(cfgback["xmin_10"]-320, cfgback["ymin_12"]-240)
			var file_path_spr = SPRITE_DIR + file_name_spr
			var tex2 = load_custom_texture(file_path_spr)
			sprback.texture=tex2
			$Control.add_child(sprback)
		
		for cfg2 in main_menu_selection:
			var spr = Sprite2D.new()
			var file_name_spr = "%03d.png" % cfg2["byte_21"]
			spr.name = file_name_spr
			spr.centered = false
			spr.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
			spr.texture_repeat = CanvasItem.TEXTURE_REPEAT_DISABLED
			spr.position = Vector2(cfg2["xmin_10"]-320, cfg2["ymin_12"]-240)
			var file_path_spr = SPRITE_DIR + file_name_spr
			var tex2 = load_custom_texture(file_path_spr)
			spr.texture=tex2
			spr.hide()
			$Control.add_child(spr)
			cfg2["node_ref"] = spr
			
func _input(event):
	var mouse_pos = $Control.get_local_mouse_position()
	var any_hovered = false
	for cfg2 in main_menu_selection:
		var spr = cfg2["node_ref"]
		if not spr: continue
		var rect = Rect2(spr.position, Vector2(cfg2["sizex_14"], cfg2["sizey_16"]))
		if rect.has_point(mouse_pos):
			any_hovered = true
			spr.show()
			#tooltip_label.text = "ID: 0x%X" % cfg2["dword_0"]
			tooltip_label.text = cfg2["help"]
			tooltip_label.show()
			tooltip_label.position = Vector2(floor(mouse_pos.x + 10), floor(mouse_pos.y + 10))
			
			if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT and event.pressed:
				if(cfg2["dword_0"]==0x00258350):
					get_tree().change_scene_to_file("res://scenes/CodeGeneratedDemo.tscn")
				else:
					print("Kliknuto na: ", cfg2["dword_0"])
		else:
			spr.hide()
	if not any_hovered:
		tooltip_label.hide()
