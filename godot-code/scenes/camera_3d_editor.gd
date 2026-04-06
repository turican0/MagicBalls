extends Camera3D

var Ray_Cylinder: MeshInstance3D = null

# ═══════════════════════════════════════════════════════════
#  SETTINGS – tweak in the Inspector
# ═══════════════════════════════════════════════════════════
@export_group("Movement")
@export var move_speed        : float = 10.0
@export var sprint_multiplier : float = 3.0
@export var mouse_sensitivity : float = 0.002
@export var pitch_limit       : float = 85.0

@export_group("Ray / Cone")
@export var ray_length     : float = 500.0
@export var collision_mask : int   = 1

@export_group("Cylinder")
@export var cylinder_radius_min  : float = 0.5
@export var cylinder_radius_max  : float = 50.0
@export var cylinder_scroll_step : float = 0.5

# ═══════════════════════════════════════════════════════════
#  INTERNAL STATE
# ═══════════════════════════════════════════════════════════
var _yaw   : float = 0.0
var _pitch : float = 0.0

var terrain_hit_position : Vector3 = Vector3.ZERO
var terrain_hit_valid    : bool    = false

var _cylinder_radius : float = 5.0

var _space_state : PhysicsDirectSpaceState3D

# ═══════════════════════════════════════════════════════════
#  INITIALIZATION
# ═══════════════════════════════════════════════════════════
func _ready() -> void:
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
	_yaw   = rotation.y
	_pitch = rotation.x
	# Ray_Cylinder is assigned externally (e.g. from editor.gd) after _ready

# ═══════════════════════════════════════════════════════════
#  INPUT
# ═══════════════════════════════════════════════════════════
func _unhandled_input(event: InputEvent) -> void:
	if event is InputEventMouseMotion:
		_yaw   -= event.relative.x * mouse_sensitivity
		_pitch -= event.relative.y * mouse_sensitivity
		_pitch  = clamp(_pitch, deg_to_rad(-pitch_limit), deg_to_rad(pitch_limit))
		_apply_rotation()

	if event is InputEventMouseButton:
		match event.button_index:
			MOUSE_BUTTON_WHEEL_UP:
				_cylinder_radius = clamp(_cylinder_radius + cylinder_scroll_step,
										 cylinder_radius_min, cylinder_radius_max)
				get_viewport().set_input_as_handled()
			MOUSE_BUTTON_WHEEL_DOWN:
				_cylinder_radius = clamp(_cylinder_radius - cylinder_scroll_step,
										 cylinder_radius_min, cylinder_radius_max)
				get_viewport().set_input_as_handled()

	if event is InputEventKey and event.pressed:
		if event.keycode == KEY_ESCAPE:
			if Input.get_mouse_mode() == Input.MOUSE_MODE_CAPTURED:
				Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
			else:
				Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)

# ═══════════════════════════════════════════════════════════
#  GAME LOOP
# ═══════════════════════════════════════════════════════════
func _physics_process(delta: float) -> void:
	_handle_movement(delta)
	_space_state = get_world_3d().direct_space_state
	_cast_center_ray()
	_update_ray_position()

# ═══════════════════════════════════════════════════════════
#  MOVEMENT
# ═══════════════════════════════════════════════════════════
func _handle_movement(delta: float) -> void:
	var speed := move_speed * (sprint_multiplier if Input.is_key_pressed(KEY_SHIFT) else 1.0)
	var dir   := Vector3.ZERO
	if Input.is_key_pressed(KEY_W) or Input.is_key_pressed(KEY_UP):    dir -= transform.basis.z
	if Input.is_key_pressed(KEY_S) or Input.is_key_pressed(KEY_DOWN):  dir += transform.basis.z
	if Input.is_key_pressed(KEY_A) or Input.is_key_pressed(KEY_LEFT):  dir -= transform.basis.x
	if Input.is_key_pressed(KEY_D) or Input.is_key_pressed(KEY_RIGHT): dir += transform.basis.x
	if Input.is_key_pressed(KEY_Q): dir -= Vector3.UP
	if Input.is_key_pressed(KEY_E): dir += Vector3.UP
	if dir.length_squared() > 0.0:
		position += dir.normalized() * speed * delta

func _apply_rotation() -> void:
	rotation.y = _yaw
	rotation.x = _pitch
	rotation.z = 0.0

# ═══════════════════════════════════════════════════════════
#  CENTER RAY
# ═══════════════════════════════════════════════════════════
func _cast_center_ray() -> void:
	var sc     := get_viewport().get_visible_rect().size * 0.5
	var origin := project_ray_origin(sc)
	var dir    := project_ray_normal(sc)
	var end    := origin + dir * ray_length

	var q := PhysicsRayQueryParameters3D.create(origin, end)
	q.collision_mask      = collision_mask
	q.collide_with_bodies = true
	q.collide_with_areas  = false

	var res := _space_state.intersect_ray(q)
	if res.is_empty():
		terrain_hit_valid    = false
		terrain_hit_position = end
	else:
		terrain_hit_valid    = true
		terrain_hit_position = res["position"]

# ═══════════════════════════════════════════════════════════
#  RAY POSITION UPDATE
# ═══════════════════════════════════════════════════════════
func _update_ray_position() -> void:
	if Ray_Cylinder == null:
		return
	if not terrain_hit_valid:
		Ray_Cylinder.visible = false
		return

	Ray_Cylinder.visible = true

	# Move cylinder to hit point (Y stays at terrain level)
	Ray_Cylinder.global_position = terrain_hit_position

	# Resize the CylinderMesh radius via the mesh resource
	var cyl := Ray_Cylinder.mesh as CylinderMesh
	if cyl:
		cyl.top_radius    = _cylinder_radius
		cyl.bottom_radius = _cylinder_radius

# ═══════════════════════════════════════════════════════════
#  PUBLIC API
# ═══════════════════════════════════════════════════════════

## Returns Vector2(X, Z) of center ray terrain hit. Vector2.ZERO if no hit.
func get_terrain_xz() -> Vector2:
	return Vector2(terrain_hit_position.x, terrain_hit_position.z) if terrain_hit_valid else Vector2.ZERO

## Returns full Vector3 of center ray hit (Y = elevation).
func get_terrain_hit() -> Vector3:
	return terrain_hit_position if terrain_hit_valid else Vector3.ZERO

## Is the center ray currently hitting the terrain?
func is_terrain_hit_valid() -> bool:
	return terrain_hit_valid

## Current cylinder radius in world units.
func get_cylinder_radius() -> float:
	return _cylinder_radius
