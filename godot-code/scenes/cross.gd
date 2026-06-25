extends Sprite2D
@export var cross_size: float = 10.0
@export var cross_thickness: float = 1.0
@export var cross_color: Color = Color.WHITE
@export var cross_alpha: float = 0.4  # síla efektu

var mat: CanvasItemMaterial

func _ready() -> void:
	texture = null
	mat = CanvasItemMaterial.new()
	mat.blend_mode = CanvasItemMaterial.BLEND_MODE_ADD
	material = mat
	get_viewport().size_changed.connect(_update_position)
	_update_position()

func _update_position() -> void:
	global_position = get_viewport_rect().size / 2.0
	queue_redraw()

func _draw() -> void:
	var c := cross_color
	c.a = cross_alpha
	draw_line(Vector2(-cross_size, 0), Vector2(cross_size, 0), c, cross_thickness)
	draw_line(Vector2(0, -cross_size), Vector2(0, cross_size), c, cross_thickness)
