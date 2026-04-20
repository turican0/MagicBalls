extends TextureButton

func _ready():
	custom_minimum_size = Vector2(40, 40)
	toggle_mode = true # Důležité: aby tlačítko drželo stav
	
	# Signál pro změnu stavu (toggled funguje pro zapnutí i vypnutí)
	if not self.toggled.is_connected(_on_toggled):
		self.toggled.connect(_on_toggled)
	
	_update_visuals(button_pressed)

func _on_toggled(toggled_on: bool):
	_update_visuals(toggled_on)

func _update_visuals(active: bool):
	if active:
		self.modulate = Color(1, 1, 1, 1)
	else:
		self.modulate = Color(0.3, 0.3, 0.3, 1)
