@tool # Tento řádek zajistí, že skript běží i v editoru
extends GridContainer

# Přidáme tlačítko do Inspektoru, na které stačí kliknout
@export var spustit_generovani: bool = false:
	set(value):
		if value == true:
			vytvor_panely()
		spustit_generovani = false # Hned se to vrátí do vypnutého stavu

const BUTTON_SCRIPT = preload("res://scenes-editor/spellIcon.gd")

func vytvor_panely():
	# Vyčistíme staré panely, pokud tam nějaké jsou (volitelné)
	# for child in get_children():
	# 	child.free()

	var scene_root = get_tree().edited_scene_root if Engine.is_editor_hint() else get_tree().current_scene
	
	var style_box = StyleBoxFlat.new()
	style_box.bg_color = Color(0.1, 0.1, 0.1, 0.5)
	style_box.border_width_left = 1
	style_box.border_width_top = 1
	style_box.border_width_right = 1
	style_box.border_width_bottom = 1
	style_box.border_color = Color(0.8, 0.8, 0.8, 1.0)
	style_box.corner_radius_top_left = 3
	style_box.corner_radius_top_right = 3
	style_box.corner_radius_bottom_right = 3
	style_box.corner_radius_bottom_left = 3

	for i in range(1, 27):
		var panel = Panel.new()
		panel.name = "Panel" + str(i).pad_zeros(2)
		panel.custom_minimum_size = Vector2(40, 40)
		panel.add_theme_stylebox_override("panel", style_box)
		
		var btn = TextureButton.new()
		btn.name = "TextureButton"
		btn.set_script(BUTTON_SCRIPT)
		
		var spell_id = 96 + i
		var fixed_name = "HSPRD0-0.DAT." + str(spell_id).pad_zeros(3) + ".png"
		var image_path = "user://convertdata/HSPR/HSPR-day/" + fixed_name
		
		if FileAccess.file_exists(image_path):
			var image = Image.load_from_file(image_path)
			var texture = ImageTexture.create_from_image(image)
			btn.texture_normal = texture
			btn.ignore_texture_size = true
			btn.stretch_mode = TextureButton.STRETCH_KEEP_ASPECT_CENTERED
			btn.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
		
		add_child(panel)
		panel.owner = scene_root
		btn.owner = scene_root
		
	print("Panely vygenerovány přes Inspektora!")
