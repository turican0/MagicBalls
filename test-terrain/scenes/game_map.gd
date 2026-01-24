extends Node3D

@onready var gamemap = $Control/GameMap
@onready var foreground = $Control/Foreground

var scroll_speed = 500.0
var edge_margin = 50.0

#NewGameDialog_77350

func _ready():
	var screen_size = get_viewport().get_visible_rect().size
	foreground.position = screen_size / 2
	gamemap.position = screen_size / 2

func _process(delta):
	var mouse_pos = get_viewport().get_mouse_position()
	var screen_size = get_viewport().get_visible_rect().size
	var move_vector = Vector2.ZERO
	if mouse_pos.x < edge_margin:
		move_vector.x = 1
	elif mouse_pos.x > screen_size.x - edge_margin:
		move_vector.x = -1
	if mouse_pos.y < edge_margin:
		move_vector.y = 1
	elif mouse_pos.y > screen_size.y - edge_margin:
		move_vector.y = -1
	if move_vector != Vector2.ZERO:
		gamemap.position += move_vector.normalized() * scroll_speed * delta
		_check_map_limits(screen_size)

func _check_map_limits(screen_size):
	# Zde můžeš omezit, aby mapa neodjela úplně pryč
	# Např. pokud je mapa velká 2000x2000:
	# gamemap.position.x = clamp(gamemap.position.x, -500, 1500)
	pass
