extends TextureButton

# Proměnná pro sledování stavu
var is_active: bool = true

func _ready():
	# Velikost a signály
	custom_minimum_size = Vector2(40, 40)
	
	# Propojíme signál stisknutí se funkcí, pokud ještě není připojen
	if not self.pressed.is_connected(_on_pressed):
		self.pressed.connect(_on_pressed)
	
	# Nastavíme výchozí vizuál při startu
	_update_visuals()

func _on_pressed():
	# Přepneme true/false
	is_active = !is_active
	_update_visuals()

func _update_visuals():
	if is_active:
		# Normální barvy
		self.modulate = Color(1, 1, 1, 1)
	else:
		# Ztmavení/zašednutí
		self.modulate = Color(0.3, 0.3, 0.3, 1)
