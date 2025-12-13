class_name TMapDefinition extends RefCounted # Dědění z RefCounted je dobré pro datové objekty

# Vlastnosti (Properties) definované v C struct:
var index: int = 0
var width: int = 0
var height: int = 0
var speed: int = 0      # speed_6
var rot_speed: int = 0  # rotSpeed_8
var type_flag: int = 0  # byte_12
var texture: Texture2D = null # Konvertovaná Godot textura

# Volitelná inicializační funkce
func _init(idx: int = 0):
	index = idx
