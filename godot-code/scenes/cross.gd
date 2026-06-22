@tool
extends Sprite2D

@export var cross_size: float = 20.0
@export var cross_thickness: float = 2.0
@export var cross_color: Color = Color.WHITE

func _ready() -> void:
	# Sprite2D has no texture here, we draw directly via _draw()
	texture = null
	get_viewport().size_changed.connect(_update_position)
	_update_position()

func _update_position() -> void:
	# Center on screen (in viewport coordinates)
	global_position = get_viewport_rect().size / 2.0
	queue_redraw()

func _draw() -> void:
	# Horizontal line
	draw_line(
		Vector2(-cross_size, 0),
		Vector2(cross_size, 0),
		cross_color,
		cross_thickness
	)
	# Vertical line
	draw_line(
		Vector2(0, -cross_size),
		Vector2(0, cross_size),
		cross_color,
		cross_thickness
	)
