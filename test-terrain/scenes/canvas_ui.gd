extends CanvasLayer

@export var spell_grid: Node  # odkaz na HBoxContainer / GridContainer v UI
var player: Node = null

func updateSpells(spells:Array):
	for child in spell_grid.get_children():
		child.queue_free()
	#for item in data:
		#print("A: ", item.spell_state, " B: ", item.spell_mana)
	
	var index=0
	for spell in spells:
		var slot = Panel.new()
		slot.custom_minimum_size = Vector2(64,64)
		#slot.custom_styles["panel"] = get_slot_style()  # nastavíme styl rámečku
		
		var frame = TextureRect.new()
		frame.texture = preload("res://MC2FILES/HSPRN0-0-089-00.png")
		#frame.stretch_mode = TextureRect.STRETCH_SCALE_ON_EXPAND
		frame.anchor_left = 0
		frame.anchor_top = 0
		frame.anchor_right = 1
		frame.anchor_bottom = 1
		slot.add_child(frame)

		# vytvoříme ikonku jako TextureButton
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


func init():
	load_spells()

func load_spells():
	# nejdřív vyčistíme grid
	for child in spell_grid.get_children():
		child.queue_free()

	# získáme seznam kouzel z Playera
	var spells = player.get_spells()  # očekáváme, že Player má funkci get_spells()
	
	for spell in spells:
		var slot = Panel.new()
		slot.custom_minimum_size = Vector2(64,64)
		#slot.custom_styles["panel"] = get_slot_style()  # nastavíme styl rámečku
		
		var frame = TextureRect.new()
		frame.texture = preload("res://assets-2d/frame-cut64.png")
		#frame.stretch_mode = TextureRect.STRETCH_SCALE_ON_EXPAND
		frame.anchor_left = 0
		frame.anchor_top = 0
		frame.anchor_right = 1
		frame.anchor_bottom = 1
		slot.add_child(frame)

		# vytvoříme ikonku jako TextureButton
		var btn = TextureButton.new()
		btn.texture_normal = spell["icon"]  # icon musí být textura
		btn.tooltip_text = spell["id"]  # název kouzla
		btn.pressed.connect(Callable(self, "_on_spell_pressed").bind(spell["id"]))

		#var fixed_size = Vector2(64, 64)
		#btn.custom_minimum_size = fixed_size
		#btn.expand_ui_icon = true
		#btn.stretch_mode = TextureButton.STRETCH_KEEP_ASPECT_CENTERED
		#btn.size_flags_horizontal = Control.SIZE_SHRINK_BEGIN
		#btn.size_flags_vertical = Control.SIZE_SHRINK_BEGIN
		#btn.stretch_mode = TextureButton.STRETCH_KEEP_ASPECT_CENTERED

		slot.add_child(btn)
		spell_grid.add_child(slot)
		
func _on_spell_pressed(spell_id):
	if player:
		player.select_spell(spell_id)
