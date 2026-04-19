@tool
extends GridContainer

@export var spustit_generovani: bool = false:
	set(value):
		if value == true:
			vytvor_panely()
		spustit_generovani = false

const BUTTON_SCRIPT = preload("res://scenes-editor/spellIcon.gd")

func vytvor_panely():
	var scene_root = get_tree().edited_scene_root
	if not scene_root:
		print("Chyba: Nebyl nalezen root scény!")
		return

	# Styl panelu
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
		btn.name = "TextureButton" + str(i).pad_zeros(2)
		
		# 1. Přidání do stromu a nastavení skriptu
		add_child(panel)
		panel.add_child(btn)
		btn.set_script(BUTTON_SCRIPT)
		
		# 2. Nastavení vlastníků (aby to nezmizelo)
		panel.owner = scene_root
		btn.owner = scene_root
		
		# 3. Načtení cesty
		var spell_id = 96 + i
		var fixed_name = "HSPRD0-0.DAT_" + str(spell_id).pad_zeros(3) + ".png"
		var path = "user://convertdata/HSPR/HSPR-day/" + fixed_name
		# Převedeme user:// na reálnou cestu v OS
		var global_path = ProjectSettings.globalize_path(path)
		
		if FileAccess.file_exists(global_path):
			var img = Image.load_from_file(global_path)
			if img:
				var tex = ImageTexture.create_from_image(img)
				
				# TADY JE TA ZMĚNA: Nastavíme to "natvrdo" přes set_deferred
				btn.set_deferred("texture_normal", tex)
				
				btn.ignore_texture_size = true
				btn.stretch_mode = TextureButton.STRETCH_KEEP_ASPECT_CENTERED
				btn.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
				print("ID ", spell_id, ": Načteno z ", global_path)
		else:
			print("ID ", spell_id, ": SOUBOR NENALEZEN na ", global_path)
		
	print("Hotovo! Panely vygenerovány.")
