extends Sprite2D
## World/screen position where the arrow tip is located
@export var target_position: Vector2 = Vector2(100, 0):
	set(value):
		target_position = value
		_update_transform()
@export var arrow_length: float = 30.0:
	set(value):
		arrow_length = value
		_update_transform()
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
@export var arrow_alpha: float = 0.4:
	set(value):
		arrow_alpha = value
		queue_redraw()

var _effective_length: float = 30.0
var _effective_head_size: float = 10.0
var _mat: CanvasItemMaterial

func _ready() -> void:
	texture = null
	_mat = CanvasItemMaterial.new()
	_mat.blend_mode = CanvasItemMaterial.BLEND_MODE_ADD
	material = _mat
	if not Engine.is_editor_hint():
		get_viewport().size_changed.connect(_update_transform)
	_update_transform()

func _update_transform() -> void:
	# Position the node at the tip position
	global_position = target_position
	# Direction pointing AWAY from the screen center, through the tip position
	var center: Vector2 = get_viewport_rect().size / 2.0
	var direction: Vector2 = (target_position - center)
	var distance: float = direction.length()
	if distance > 0.001:
		rotation = direction.angle()
	else:
		rotation = 0.0
	# Pokud je šipka blíž ke středu, než je její délka, zmenšíme ji tak,
	# aby šahala přesně mezi střed a pozici (tail nepřesáhne střed).
	_effective_length = min(arrow_length, distance)
	# Hlavička se zmenšuje proporčně se zkracující se šipkou
	var ratio: float = 1.0
	if arrow_length > 0.001:
		ratio = _effective_length / arrow_length
	_effective_head_size = arrow_head_size * ratio
	queue_redraw()

func _draw() -> void:
	# Arrow is drawn along the local +X axis (rotation handles the direction);
	# the tip sits at the local origin (0,0) = target_position.
	var tail: Vector2 = Vector2(-_effective_length, 0)
	var tip: Vector2 = Vector2.ZERO
	var c: Color = arrow_color
	c.a = arrow_alpha
	# Shaft
	draw_line(tail, tip, c, arrow_thickness)
	# Arrowhead (two diagonal lines back from the tip)
	var head_angle: float = deg_to_rad(25.0)
	var left: Vector2 = tip.rotated(0) + Vector2(-1, 0).rotated(head_angle) * _effective_head_size
	var right: Vector2 = tip.rotated(0) + Vector2(-1, 0).rotated(-head_angle) * _effective_head_size
	draw_line(tip, left, c, arrow_thickness)
	draw_line(tip, right, c, arrow_thickness)
