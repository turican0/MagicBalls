extends Control

@export var spell_grid: Node
@export var spell_grid_selected: Node

var player: Node
var visible_mode: int = 1


func _init() -> void:
	RenderingServer.set_debug_generate_wireframes(true)


func _process(_p_delta) -> void:
	$Label.text = "FPS: %d\n" % Engine.get_frames_per_second()
	if(visible_mode == 1):
		$Label.text += "Move Speed: %.1f\n" % player.MOVE_SPEED if player else ""
		$Label.text += "Life: %.1f\n" % player.LIFE if player else ""
		$Label.text += "Mana: %.1f\n" % player.MANA if player else ""
		$Label.text += "Position: %.1v\n" % player.global_position if player else ""
		$Label.text += """
			Player
			Move: WASDEQ,Space,Mouse
			Move speed: Wheel,+/-,Shift
			Camera View: V
			Gravity toggle: G
			Collision toggle: C

			Window
			Quit: F8
			UI toggle: F9
			Render mode: F10
			Full screen: F11
			Mouse toggle: Escape / F12
			"""


func _unhandled_key_input(p_event: InputEvent) -> void:
	if p_event is InputEventKey and p_event.pressed:
		match p_event.keycode:
			KEY_F8:
				get_tree().quit()
			KEY_F9:
				visible_mode = (visible_mode + 1 ) % 3
				$Label/Panel.visible = (visible_mode == 1)
				visible = visible_mode > 0
			KEY_F10:
				var vp = get_viewport()
				vp.debug_draw = (vp.debug_draw + 1 ) % 6
				get_viewport().set_input_as_handled()
			KEY_F11:
				toggle_fullscreen()
				get_viewport().set_input_as_handled()
			KEY_ESCAPE, KEY_F12:
				if Input.get_mouse_mode() == Input.MOUSE_MODE_VISIBLE:
					Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
				else:
					Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
				get_viewport().set_input_as_handled()
		
		
func toggle_fullscreen() -> void:
	if DisplayServer.window_get_mode() == DisplayServer.WINDOW_MODE_EXCLUSIVE_FULLSCREEN or \
		DisplayServer.window_get_mode() == DisplayServer.WINDOW_MODE_FULLSCREEN:
		DisplayServer.window_set_mode(DisplayServer.WINDOW_MODE_WINDOWED)
		DisplayServer.window_set_size(Vector2(1280, 720))
	else:
		DisplayServer.window_set_mode(DisplayServer.WINDOW_MODE_EXCLUSIVE_FULLSCREEN)
		
func updateSelectedSpells(spells:Array):
	for child in spell_grid_selected.get_children():
		child.queue_free()
	for spell in spells:
		var slot = Panel.new()
		slot.custom_minimum_size = Vector2(64,64)
		var frame = TextureRect.new()
		frame.texture = preload("res://MC2FILES/HSPRN0-0-089-00.png")
		frame.anchor_left = 0
		frame.anchor_top = 0
		frame.anchor_right = 1
		frame.anchor_bottom = 1
		slot.add_child(frame)
		var btn = TextureButton.new()
		if(spell.spellIndex>=0):
			btn.texture_normal = load("res://MC2FILES/HSPRN0-0-%03d-00.png" % (spell.spellIndex+123))
		btn.tooltip_text = "fireball"#spell["id"]
		btn.pressed.connect(Callable(self, "_on_spell_pressed").bind("fireball"))
		slot.add_child(btn)		
		var mana_bar = ProgressBar.new()
		mana_bar.show_percentage = false
		mana_bar.custom_minimum_size = Vector2(0, 6) 
		mana_bar.set_anchors_and_offsets_preset(Control.PRESET_BOTTOM_WIDE)
		var style_fill = StyleBoxFlat.new()
		style_fill.bg_color = Color(0, 0.6, 1)
		mana_bar.add_theme_stylebox_override("fill", style_fill)
		var style_bg = StyleBoxFlat.new()
		style_bg.bg_color = Color(0, 0, 0, 1)
		mana_bar.add_theme_stylebox_override("background", style_bg)
		mana_bar.value = spell.mana1
		slot.add_child(mana_bar)		
		spell_grid_selected.add_child(slot)

func updateSpells(spells:Array):
	for child in spell_grid.get_children():
		child.queue_free()
	var index=0
	for spell in spells:
		var slot = Panel.new()
		slot.custom_minimum_size = Vector2(64,64)
		var frame = TextureRect.new()
		frame.texture = preload("res://MC2FILES/HSPRN0-0-089-00.png")
		frame.anchor_left = 0
		frame.anchor_top = 0
		frame.anchor_right = 1
		frame.anchor_bottom = 1
		slot.add_child(frame)
		var btn = TextureButton.new()
		if(spell.spell_state==1):
			btn.texture_normal = load("res://MC2FILES/HSPRN0-0-%03d-00.png" % (index+97))
		btn.tooltip_text = "fireball"#spell["id"]
		btn.pressed.connect(Callable(self, "_on_spell_pressed").bind("fireball"))
		slot.add_child(btn)
		
		var mana_bar = ProgressBar.new()
		mana_bar.show_percentage = false
		mana_bar.custom_minimum_size = Vector2(0, 6) 
		mana_bar.set_anchors_and_offsets_preset(Control.PRESET_BOTTOM_WIDE)
		var style_fill = StyleBoxFlat.new()
		style_fill.bg_color = Color(0, 0.6, 1)
		mana_bar.add_theme_stylebox_override("fill", style_fill)
		var style_bg = StyleBoxFlat.new()
		style_bg.bg_color = Color(0, 0, 0, 1)
		mana_bar.add_theme_stylebox_override("background", style_bg)
		mana_bar.value = spell.spell_mana
		slot.add_child(mana_bar)		
		spell_grid.add_child(slot)
		index+=1

func load_spells():
	for child in spell_grid.get_children():
		child.queue_free()
	var spells = player.get_spells()	
	for spell in spells:
		var slot = Panel.new()
		slot.custom_minimum_size = Vector2(64,64)
		var frame = TextureRect.new()
		frame.texture = preload("res://assets-2d/frame-cut64.png")
		frame.anchor_left = 0
		frame.anchor_top = 0
		frame.anchor_right = 1
		frame.anchor_bottom = 1
		slot.add_child(frame)
		var btn = TextureButton.new()
		btn.texture_normal = spell["icon"]  # icon musí být textura
		btn.tooltip_text = spell["id"]  # název kouzla
		btn.pressed.connect(Callable(self, "_on_spell_pressed").bind(spell["id"]))
		slot.add_child(btn)
		spell_grid.add_child(slot)
		
func _on_spell_pressed(spell_id):
	if player:
		player.select_spell(spell_id)
