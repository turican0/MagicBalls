extends Camera3D

# ─────────────────────────────────────────
#  Settings – tweak in the Inspector
# ─────────────────────────────────────────
@export var move_speed        : float = 10.0   # m/s base movement speed
@export var sprint_multiplier : float = 3.0    # Shift = fast movement
@export var mouse_sensitivity : float = 0.002  # radians per pixel
@export var pitch_limit       : float = 85.0   # max look up/down angle (degrees)

# Raycast settings
@export var ray_length        : float = 1000.0 # ray length in meters
@export var collision_mask    : int   = 1       # terrain collision layer (layer 1 = default)

# ─────────────────────────────────────────
#  Internal state
# ─────────────────────────────────────────
var _yaw   : float = 0.0   # rotation around Y axis (left/right)
var _pitch : float = 0.0   # rotation around X axis (up/down)

# Last recorded terrain hit position
var terrain_hit_position : Vector3 = Vector3.ZERO
var terrain_hit_valid    : bool    = false

# Reference to PhysicsDirectSpaceState (set at runtime)
var _space_state : PhysicsDirectSpaceState3D

# ─────────────────────────────────────────
#  Initialization
# ─────────────────────────────────────────
func _ready() -> void:
	# Lock mouse to window
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)

	# Initialize rotation from current node orientation
	_yaw   = rotation.y
	_pitch = rotation.x

# ─────────────────────────────────────────
#  Input – mouse capture and Escape key
# ─────────────────────────────────────────
func _unhandled_input(event: InputEvent) -> void:
	# Mouse motion → rotate camera
	if event is InputEventMouseMotion:
		_yaw   -= event.relative.x * mouse_sensitivity
		_pitch -= event.relative.y * mouse_sensitivity
		_pitch  = clamp(_pitch, deg_to_rad(-pitch_limit), deg_to_rad(pitch_limit))
		_apply_rotation()

	# Escape → toggle mouse lock
	if event is InputEventKey and event.pressed:
		if event.keycode == KEY_ESCAPE:
			if Input.get_mouse_mode() == Input.MOUSE_MODE_CAPTURED:
				Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
			else:
				Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)

# ─────────────────────────────────────────
#  Game loop
# ─────────────────────────────────────────
func _physics_process(delta: float) -> void:
	_handle_movement(delta)
	_cast_ray_to_terrain()

# ─────────────────────────────────────────
#  Movement (WASD + QE for elevation, Shift = sprint)
# ─────────────────────────────────────────
func _handle_movement(delta: float) -> void:
	var speed := move_speed
	if Input.is_key_pressed(KEY_SHIFT):
		speed *= sprint_multiplier

	# Direction vector in camera local space
	var dir := Vector3.ZERO

	if Input.is_key_pressed(KEY_W) or Input.is_key_pressed(KEY_UP):
		dir -= transform.basis.z          # forward
	if Input.is_key_pressed(KEY_S) or Input.is_key_pressed(KEY_DOWN):
		dir += transform.basis.z          # backward
	if Input.is_key_pressed(KEY_A) or Input.is_key_pressed(KEY_LEFT):
		dir -= transform.basis.x          # strafe left
	if Input.is_key_pressed(KEY_D) or Input.is_key_pressed(KEY_RIGHT):
		dir += transform.basis.x          # strafe right
	if Input.is_key_pressed(KEY_Q):
		dir -= Vector3.UP                 # move down (global)
	if Input.is_key_pressed(KEY_E):
		dir += Vector3.UP                 # move up (global)

	if dir.length_squared() > 0.0:
		position += dir.normalized() * speed * delta

# ─────────────────────────────────────────
#  Apply rotation (yaw on Y, pitch on X)
# ─────────────────────────────────────────
func _apply_rotation() -> void:
	rotation.y = _yaw
	rotation.x = _pitch
	rotation.z = 0.0   # prevent roll

# ─────────────────────────────────────────
#  Raycast → terrain: returns X/Z world position
# ─────────────────────────────────────────
func _cast_ray_to_terrain() -> void:
	_space_state = get_world_3d().direct_space_state

	# Ray from camera through screen center
	var viewport      := get_viewport()
	var screen_center := viewport.get_visible_rect().size * 0.5

	var ray_origin := project_ray_origin(screen_center)
	var ray_dir    := project_ray_normal(screen_center)
	var ray_end    := ray_origin + ray_dir * ray_length

	# Collision query parameters
	var query := PhysicsRayQueryParameters3D.create(ray_origin, ray_end)
	query.collision_mask      = collision_mask
	query.collide_with_bodies = true
	query.collide_with_areas  = false

	var result := _space_state.intersect_ray(query)

	if result.is_empty():
		terrain_hit_valid = false
	else:
		terrain_hit_valid    = true
		terrain_hit_position = result["position"]   # Vector3 – Y = elevation at hit point

# ─────────────────────────────────────────
#  Public API – call from other scripts
# ─────────────────────────────────────────

## Returns Vector2(X, Z) of the terrain point the camera is aimed at.
## Returns Vector2.ZERO if the ray hits nothing.
func get_terrain_xz() -> Vector2:
	if terrain_hit_valid:
		return Vector2(terrain_hit_position.x, terrain_hit_position.z)
	return Vector2.ZERO

## Returns the full Vector3 hit point (Y = terrain elevation at that position).
func get_terrain_hit() -> Vector3:
	return terrain_hit_position if terrain_hit_valid else Vector3.ZERO

## Is the ray currently hitting the terrain collider?
func is_terrain_hit_valid() -> bool:
	return terrain_hit_valid
