extends Control

@export var spell_grid: Node
@export var spell_grid_selected: Node

var player: Node
var visible_mode: int = 1

var Main_DecodeLevel

# --- Nové proměnné pro logiku CTRL ---
var is_ctrl_active: bool = false
var old_is_ctrl_active: bool = false
var hovered_index: int = -1

func _init() -> void:
	RenderingServer.set_debug_generate_wireframes(true)
	
func _input(event: InputEvent) -> void:
	if event is InputEventKey and event.keycode == KEY_CTRL:
		if event.pressed:
			if not is_ctrl_active:
				spell_grid.show()
				is_ctrl_active = true
		else:
			is_ctrl_active = false
			hovered_index = -1
			spell_grid.hide()

var saved_mouse_pos: Vector2 = Vector2.ZERO

func update_hover_selection():
	var mouse_pos = get_global_mouse_position()
	for i in range(spell_grid.get_child_count()):
		var slot = spell_grid.get_child(i) as Control
		if is_instance_valid(slot) and slot.get_global_rect().has_point(mouse_pos):
			hovered_index = i
			break

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
	if is_ctrl_active:
		update_hover_selection()


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
		
func updateMinimap(image:Image):
	if image:
		var tex = ImageTexture.create_from_image(image)
		$Minimap.texture = tex

func updateSpells(spells:Array):
	for child in spell_grid.get_children():
		child.queue_free()
	var index=0
	for spell in spells:
		var slot = Panel.new()
		slot.mouse_filter = Control.MOUSE_FILTER_PASS
		slot.custom_minimum_size = Vector2(64,64)
		var frame = TextureRect.new()
		frame.mouse_filter = Control.MOUSE_FILTER_IGNORE
		frame.texture = preload("res://MC2FILES/HSPRN0-0-089-00.png")
		frame.anchor_left = 0
		frame.anchor_top = 0
		frame.anchor_right = 1
		frame.anchor_bottom = 1
		slot.add_child(frame)
		var btn = TextureButton.new()
		if(spell.spell_state==1):
			btn.texture_normal = load("res://MC2FILES/HSPRN0-0-%03d-00.png" % (index+97))
		btn.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
		btn.tooltip_text = "fireball"#spell["id"]
		#btn.pressed.connect(Callable(self, "on_spell_pressed").bind(index))
		btn.gui_input.connect(on_slot_gui_input.bind(index))
		slot.add_child(btn)
		
		var mana_bar = ProgressBar.new()
		mana_bar.mouse_filter = Control.MOUSE_FILTER_IGNORE
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
		
		var selection_rect = Panel.new()
		selection_rect.name = "Highlight"
		selection_rect.mouse_filter = Control.MOUSE_FILTER_IGNORE
		selection_rect.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
		var style = StyleBoxFlat.new()
		style.draw_center = false           # <--- Tímto vypneš výplň (bude průhledná)
		style.border_width_left = 3         # Tloušťka okraje v pixelech
		style.border_width_top = 3
		style.border_width_right = 3
		style.border_width_bottom = 3
		style.border_color = Color(1.0, 0.84, 0.0)
		#style.set_corner_radius_all(2)
		selection_rect.add_theme_stylebox_override("panel", style)
		if(hovered_index!=index):
			selection_rect.hide()
		slot.add_child(selection_rect)
	
		spell_grid.add_child(slot)
		index+=1

func on_slot_gui_input(event: InputEvent, index: int):
	if event is InputEventMouseButton and event.pressed:
		if event.button_index == MOUSE_BUTTON_LEFT:
			Main_DecodeLevel.setPlayerActiveSpell(index,0)
			get_viewport().set_input_as_handled()
		else:
			if event.button_index == MOUSE_BUTTON_RIGHT:
				Main_DecodeLevel.setPlayerActiveSpell(index,1)
				get_viewport().set_input_as_handled()
