extends CanvasLayer

@export var spell_grid: Node  # odkaz na HBoxContainer / GridContainer v UI
var player: Node = null

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
