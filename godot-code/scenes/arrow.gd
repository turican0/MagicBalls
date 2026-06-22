@tool
extends Sprite2D

## World/screen position where the arrow tip is located
@export var target_position: Vector2 = Vector2(100, 0):
	set(value):
		target_position = value
		_update_transform()

@export var arrow_length: float = 30.0:
	set(value):
		arrow_length = value
		queue_redraw()
@export var arrow_thickness: float = 2.0:
	set(value):
		arrow_thickness = value
		queue_redraw()
@export var arrow_head_size: float = 10.0:
	set(value):
		arrow_head_size = value
		queue_redraw()
@export var arrow_color: Color = Color.WHITE:
	set(value):
		arrow_color = value
		queue_redraw()

func _ready() -> void:
	texture = null
	if not Engine.is_editor_hint():
		get_viewport().size_changed.connect(_update_transform)
	_update_transform()

func _update_transform() -> void:
	# Position the node at the tip position
	global_position = target_position

	# Direction pointing AWAY from the screen center, through the tip position
	var center: Vector2 = get_viewport_rect().size / 2.0
	var direction: Vector2 = (target_position - center)

	if direction.length() > 0.001:
		rotation = direction.angle()
	else:
		rotation = 0.0

	queue_redraw()

func _draw() -> void:
	# Arrow is drawn along the local +X axis (rotation handles the direction);
	# the tip sits at the local origin (0,0) = target_position.
	var tail: Vector2 = Vector2(-arrow_length, 0)
	var tip: Vector2 = Vector2.ZERO

	# Shaft
	draw_line(tail, tip, arrow_color, arrow_thickness)

	# Arrowhead (two diagonal lines back from the tip)
	var head_angle: float = deg_to_rad(25.0)
	var left: Vector2 = tip.rotated(0) + Vector2(-1, 0).rotated(head_angle) * arrow_head_size
	var right: Vector2 = tip.rotated(0) + Vector2(-1, 0).rotated(-head_angle) * arrow_head_size
	draw_line(tip, left, arrow_color, arrow_thickness)
	draw_line(tip, right, arrow_color, arrow_thickness)
