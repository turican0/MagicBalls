extends Node3D

var main_menu_animations = [
	{"name": "FireLeft","pos_x": 17,  "pos_y": 159, "first_sprite": 1,  "last_sprite": 8}, # left fire
	{"name": "FireRight","pos_x": 531, "pos_y": 156, "first_sprite": 9,  "last_sprite": 16}, # right fire
	{"name": "IncenseLeft","pos_x": 154, "pos_y": 308, "first_sprite": 17, "last_sprite": 25}, # left incense
	{"name": "IncenseRight","pos_x": 482, "pos_y": 308, "first_sprite": 26, "last_sprite": 34}  # right incense
]

const SPRITE_DIR = "res://convertdata/HSCREEN/"

func _ready():
	for cfg in main_menu_animations:
		var ani = AnimatedSprite2D.new()
		ani.name = cfg["name"]
		ani.centered = false
		ani.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
		ani.position = Vector2(cfg["pos_x"]-320-1, cfg["pos_y"]-240+1)
		var frames = SpriteFrames.new()
		frames.remove_animation("default")
		frames.add_animation("loop")
		for i in range(cfg["first_sprite"], cfg["last_sprite"] + 1):
			var file_name = "%03d.png" % i
			var file_path = SPRITE_DIR + file_name
			var tex = load(file_path)
			if tex:
				frames.add_frame("loop", tex)
			else:
				push_error("Chybí soubor: " + file_path)
		frames.set_animation_speed("loop", 15.0)
		frames.set_animation_loop("loop", true)		
		ani.sprite_frames = frames
		
		var tex = ani.sprite_frames.get_frame_texture("loop", 0)
		print("Velikost obrázku v pixelech: ", tex.get_size())

		$Control.add_child(ani)
		ani.play("loop")
	print("Všechny animace byly načteny a spuštěny.")
	print("Window size: ", get_window().size)
	print("Content scale: ", get_window().content_scale_factor)
