extends Control

@export var spell_grid: Node
@export var sub_spell_grid: Node
#@export var spell_grid_selected: Node

@onready var start_button: Button = $StartButton

var player: Node
var visible_mode: int = 1

var Main_DecodeLevel

# --- Nové proměnné pro logiku CTRL ---
var is_ctrl_active: bool = false
var old_is_ctrl_active: bool = false
var hovered_index: int = -1
var sub_hovered_index: int = -1

func _ready():
	start_button.pressed.connect(_on_start_pressed)
	
	var sb := StyleBoxTexture.new()
	sb.texture = preload("res://tests/9slice.png")
	sb.draw_center = true
	sb.texture_margin_left = 16
	sb.texture_margin_right = 16
	sb.texture_margin_top = 16
	sb.texture_margin_bottom = 16
	sb.axis_stretch_horizontal = StyleBoxTexture.AXIS_STRETCH_MODE_STRETCH
	sb.axis_stretch_vertical = StyleBoxTexture.AXIS_STRETCH_MODE_STRETCH
	#$RefreshLevel.add_theme_stylebox_override("panel", sb)
	#$RefreshLevel.borderless = true
	#$RefreshLevel.popup_centered()
	
	
func _on_start_pressed():
	Main_DecodeLevel.SetRunned(true)
	start_button.hide()

func _init() -> void:
	RenderingServer.set_debug_generate_wireframes(true)
	
func _input(event: InputEvent) -> void:
	if event is InputEventKey and event.keycode == KEY_CTRL:
		if event.pressed:
			if not is_ctrl_active:
				spell_grid.show()
				is_ctrl_active = true
		else:
			hide_panels()
			
func hide_panels():
	is_ctrl_active = false
	hovered_index = -1
	sub_hovered_index = -1
	spell_grid.hide()
	sub_spell_grid.hide()


var saved_mouse_pos: Vector2 = Vector2.ZERO

func update_hover_selection():
	var mouse_pos = get_global_mouse_position()
	for i in range(spell_grid.get_child_count()):
		var slot = spell_grid.get_child(i) as Control
		if is_instance_valid(slot) and slot.get_global_rect().has_point(mouse_pos):
			hovered_index = i
			break

func update_hover_sub_selection():
	var mouse_pos = get_global_mouse_position()
	for i in range(sub_spell_grid.get_child_count()):
		var slot = sub_spell_grid.get_child(i) as Control
		if is_instance_valid(slot) and slot.get_global_rect().has_point(mouse_pos):
			sub_hovered_index = i
			break

func _process(_p_delta) -> void:
	#$Label.text = "FPS: %d\n" % Engine.get_frames_per_second()
	#if(visible_mode == 1):
		#$Label.text += "Move Speed: %.1f\n" % player.MOVE_SPEED if player else ""
		#$Label.text += "Life: %.1f\n" % player.LIFE if player else ""
		#$Label.text += "Mana: %.1f\n" % player.MANA if player else ""
		#$Label.text += "Position: %.1v\n" % player.global_position if player else ""
		#$Label.text += """
			#Player
			#Move: WASDEQ,Space,Mouse
			#Move speed: Wheel,+/-,Shift
			#Camera View: V
			#Gravity toggle: G
			#Collision toggle: C
#
			#Window
			#Quit: F8
			#UI toggle: F9
			#Render mode: F10
			#Full screen: F11
			#Mouse toggle: Escape / F12
			#"""
	if is_ctrl_active:
		update_hover_selection()
		update_hover_sub_selection()


#func _unhandled_key_input(p_event: InputEvent) -> void:
	#if p_event is InputEventKey and p_event.pressed:
		#match p_event.keycode:
			#KEY_F8:
				#get_tree().quit()
			#KEY_F9:
				#visible_mode = (visible_mode + 1 ) % 3
				#$Label/Panel.visible = (visible_mode == 1)
				#visible = visible_mode > 0
			#KEY_F10:
				#var vp = get_viewport()
				#vp.debug_draw = (vp.debug_draw + 1 ) % 6
				#get_viewport().set_input_as_handled()
			#KEY_F11:
				#toggle_fullscreen()
				#get_viewport().set_input_as_handled()
			#KEY_ESCAPE, KEY_F12:
				#if Input.get_mouse_mode() == Input.MOUSE_MODE_VISIBLE:
					#Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
				#else:
					#Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
				#get_viewport().set_input_as_handled()
		
		
func toggle_fullscreen() -> void:
	if DisplayServer.window_get_mode() == DisplayServer.WINDOW_MODE_EXCLUSIVE_FULLSCREEN or \
		DisplayServer.window_get_mode() == DisplayServer.WINDOW_MODE_FULLSCREEN:
		DisplayServer.window_set_mode(DisplayServer.WINDOW_MODE_WINDOWED)
		DisplayServer.window_set_size(Vector2(1280, 720))
	else:
		DisplayServer.window_set_mode(DisplayServer.WINDOW_MODE_EXCLUSIVE_FULLSCREEN)
		
func updateSelectedSpells(spells:Array):
	pass
	#for child in spell_grid_selected.get_children():
		#child.queue_free()
	#for spell in spells:
		#var slot = Panel.new()
		#slot.custom_minimum_size = Vector2(64,64)
		#var frame = TextureRect.new()
		#frame.texture = Global.load_custom_texture(Global.convertdata+"HSPR/HSPR-night/HSPRN0-0.DAT_089.png")
		#frame.anchor_left = 0
		#frame.anchor_top = 0
		#frame.anchor_right = 1
		#frame.anchor_bottom = 1
		#slot.add_child(frame)
		#var btn = TextureButton.new()
		#if(spell.spellIndex>=0):
			#btn.texture_normal = Global.load_custom_texture(Global.convertdata+"HSPR/HSPR-night/HSPRN0-0.DAT_%03d.png" % (spell.spellIndex+123))
		#btn.tooltip_text = "fireball"#spell["id"]
		#slot.add_child(btn)
		#var mana_bar = ProgressBar.new()
		#mana_bar.show_percentage = false
		#mana_bar.custom_minimum_size = Vector2(0, 6) 
		#mana_bar.set_anchors_and_offsets_preset(Control.PRESET_BOTTOM_WIDE)
		#var style_fill = StyleBoxFlat.new()
		#style_fill.bg_color = Color(0, 0.6, 1)
		#mana_bar.add_theme_stylebox_override("fill", style_fill)
		#var style_bg = StyleBoxFlat.new()
		#style_bg.bg_color = Color(0, 0, 0, 1)
		#mana_bar.add_theme_stylebox_override("background", style_bg)
		#mana_bar.value = spell.mana1
		#slot.add_child(mana_bar)
		#spell_grid_selected.add_child(slot)
		
func updateMinimap(image:Image):
	if image:
		var tex = ImageTexture.create_from_image(image)
		$Minimap.texture = tex

func updateSpells(spells:Array):
	for child in spell_grid.get_children():
		child.queue_free()
	var index=0
	var someSpellSelected:bool=false
	for spell in spells:
		var slot = Panel.new()
		slot.mouse_filter = Control.MOUSE_FILTER_PASS
		slot.custom_minimum_size = Vector2(64,64)
		var frame = TextureRect.new()
		frame.mouse_filter = Control.MOUSE_FILTER_IGNORE
		frame.texture = Global.load_custom_texture(Global.convertdata+"HSPR/HSPR-night/HSPRN0-0.DAT_089.png")
		frame.anchor_left = 0
		frame.anchor_top = 0
		frame.anchor_right = 1
		frame.anchor_bottom = 1
		slot.add_child(frame)
		var btn = TextureButton.new()
		if(spell.spell_state==1):
			btn.texture_normal = Global.load_custom_texture(Global.convertdata+"HSPR/HSPR-night/HSPRN0-0.DAT_%03d.png" % (index+97))
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
		if(hovered_index!=index) or (spell.spell_state!=1):
			selection_rect.hide()
		else:
			someSpellSelected=true
			updateSubSpells(index,spell)
			sub_spell_grid.show()
		slot.add_child(selection_rect)
	
		spell_grid.add_child(slot)
		index+=1
	if(!someSpellSelected):
		sub_spell_grid.hide()

func updateSubSpells(index,spell:Dictionary):
	for child in sub_spell_grid.get_children():
		child.queue_free()

	#spell_grid_sub.global_position = Vector2(slot_pos.x, slot_pos.y - spell_grid_sub.size.y)

	var sub_index=0
	for subspell in 3:
		var slot = Panel.new()
		slot.mouse_filter = Control.MOUSE_FILTER_PASS
		slot.custom_minimum_size = Vector2(80,24)
		
		var frame = TextureRect.new()
		frame.mouse_filter = Control.MOUSE_FILTER_IGNORE
		frame.texture = Global.load_custom_texture(Global.convertdata+"HSPR/HSPR-night/HSPRN0-0.DAT_089.png")
		frame.expand_mode = TextureRect.EXPAND_IGNORE_SIZE
		frame.stretch_mode = TextureRect.STRETCH_SCALE
		frame.anchor_left = 0
		frame.anchor_top = 0
		frame.anchor_right = 1
		frame.anchor_bottom = 1
		slot.add_child(frame)
		
		if(spell.sub_spell_state[subspell]==2):
			var label = Label.new()
			label.text = "Subspell %d" % sub_index
			label.mouse_filter = Control.MOUSE_FILTER_IGNORE
			label.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
			slot.add_child(label)
		else:
			if(spell.sub_spell_state[subspell]==3):
				var label = Label.new()
				label.text = "Subspell %d" % sub_index
				label.modulate = Color.GRAY
				label.mouse_filter = Control.MOUSE_FILTER_IGNORE
				label.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
				slot.add_child(label)
		
		var btn = TextureButton.new()
		#btn.flat = true
		btn.mouse_filter = Control.MOUSE_FILTER_PASS
		#if(true):#if(spell.spell_state==1):
			#btn.texture_normal = Global.load_custom_texture(Global.convertdata+"HSPR/HSPR-night/HSPRN0-0.DAT_%03d.png" % (index+97))
		btn.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
		btn.tooltip_text = "fireball"#spell["id"]
		#btn.pressed.connect(Callable(self, "on_spell_pressed").bind(index))
		btn.gui_input.connect(on_slot_gui_input_sub.bind(index,sub_index))
		slot.add_child(btn)
		
		#var btn = Button.new()
		#btn.flat = true
		#btn.mouse_filter = Control.MOUSE_FILTER_PASS
		#btn.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
		#btn.gui_input.connect(on_slot_gui_input.bind(index))
		#slot.add_child(btn)
		
		#var mana_bar = ProgressBar.new()
		#mana_bar.mouse_filter = Control.MOUSE_FILTER_IGNORE
		#mana_bar.show_percentage = false
		#mana_bar.custom_minimum_size = Vector2(0, 6) 
		#mana_bar.set_anchors_and_offsets_preset(Control.PRESET_BOTTOM_WIDE)
		#var style_fill = StyleBoxFlat.new()
		#style_fill.bg_color = Color(0, 0.6, 1)
		#mana_bar.add_theme_stylebox_override("fill", style_fill)
		#var style_bg = StyleBoxFlat.new()
		#style_bg.bg_color = Color(0, 0, 0, 1)
		#mana_bar.add_theme_stylebox_override("background", style_bg)
		#mana_bar.value = spell.spell_mana
		#slot.add_child(mana_bar)
		
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
		if(sub_hovered_index!=sub_index) or (spell.sub_spell_state[subspell]!=2):
			selection_rect.hide()
		slot.add_child(selection_rect)
	
		sub_spell_grid.add_child(slot)
		sub_index+=1
		
	await get_tree().process_frame
	var hovered_slot = spell_grid.get_child(index)
	var slot_pos = hovered_slot.global_position
	sub_spell_grid.global_position = Vector2(slot_pos.x, slot_pos.y - sub_spell_grid.size.y)

func on_slot_gui_input(event: InputEvent, index: int):
	if event is InputEventMouseButton and event.pressed:
		if event.button_index == MOUSE_BUTTON_LEFT:
			Main_DecodeLevel.setPlayerActiveSpell(index,0)
			#hide_panels()
			get_viewport().set_input_as_handled()
		else:
			if event.button_index == MOUSE_BUTTON_RIGHT:
				Main_DecodeLevel.setPlayerActiveSpell(index,1)
				#hide_panels()
				get_viewport().set_input_as_handled()

func on_slot_gui_input_sub(event: InputEvent, index: int, sub_index: int):
	if event is InputEventMouseButton and event.pressed:
		if event.button_index == MOUSE_BUTTON_LEFT:
			Main_DecodeLevel.setPlayerActiveSubSpell(index,sub_index,0)
			#hide_panels()
			get_viewport().set_input_as_handled()
		else:
			if event.button_index == MOUSE_BUTTON_RIGHT:
				Main_DecodeLevel.setPlayerActiveSubSpell(index,sub_index,1)
				#hide_panels()
				get_viewport().set_input_as_handled()
