extends Camera3D

# References assigned from the main script
var Ray_Cylinder: MeshInstance3D = null
var terrain_node: Node3D = null
var editor: Node3D = null

# ═══════════════════════════════════════════════════════════
# SETTINGS
# ═══════════════════════════════════════════════════════════
@export_group("Movement")
@export var move_speed : float = 20.0
@export var sprint_multiplier : float = 5.0
@export var mouse_sensitivity : float = 0.002
@export var pitch_limit : float = 89.0

@export_group("Ray")
@export var ray_length : float = 300.0
@export var collision_mask : int = 1

@export_group("Cylinder")
@export var cylinder_radius_min : float = 0.1
@export var cylinder_radius_max : float = 10.0
@export var cylinder_scroll_step : float = 0.3

# ═══════════════════════════════════════════════════════════
# INTERNAL STATE
# ═══════════════════════════════════════════════════════════
var _yaw : float = 0.0
var _pitch : float = 0.0
var terrain_hit_position : Vector3 = Vector3.ZERO
var terrain_hit_valid : bool = false
var _cylinder_radius : float = 1.0

# ═══════════════════════════════════════════════════════════
# INITIALIZATION
# ═══════════════════════════════════════════════════════════
func _ready() -> void:
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
	_yaw = rotation.y
	_pitch = rotation.x
	_apply_absolute_rotation()

# ═══════════════════════════════════════════════════════════
# MAIN LOOP
# ═══════════════════════════════════════════════════════════
func _process(delta: float) -> void:
	#if Input.get_mouse_mode() == Input.MOUSE_MODE_CONFINED_HIDDEN:
		#_handle_mouse_rotation()
	
	_handle_movement(delta)
	_cast_center_ray()
	_update_ray_position()
	
func _input(event: InputEvent) -> void:
	# Reagujeme pouze na pohyb myši a jen když je okno aktivní
	if event is InputEventMouseMotion and Input.get_mouse_mode() == Input.MOUSE_MODE_CAPTURED:
		# event.relative nám dává přesný posun od posledního snímku
		_yaw -= event.relative.x * mouse_sensitivity
		_pitch -= event.relative.y * mouse_sensitivity
		
		_pitch = clamp(_pitch, deg_to_rad(-pitch_limit), deg_to_rad(pitch_limit))
		_yaw = wrapf(_yaw, -PI, PI)
		
		_apply_absolute_rotation()

# ═══════════════════════════════════════════════════════════
# ROTATION LOGIC (Absolute Mouse Position)
# ═══════════════════════════════════════════════════════════
func _handle_mouse_rotation() -> void:
	var viewport = get_viewport()
	var center = viewport.get_visible_rect().size * 0.5
	var mouse_pos = viewport.get_mouse_position()
	
	var diff = mouse_pos - center
	
	if diff.length() > 0.1:
		_yaw -= diff.x * mouse_sensitivity
		_pitch -= diff.y * mouse_sensitivity
		
		_yaw = wrapf(_yaw, -PI, PI)
		_pitch = clamp(_pitch, deg_to_rad(-pitch_limit), deg_to_rad(pitch_limit))
		
		_apply_absolute_rotation()
		viewport.warp_mouse(center)

func _apply_absolute_rotation() -> void:
	# Build rotation from scratch to prevent Gimbal Lock and Roll
	var yaw_b = Basis(Vector3.UP, _yaw)
	var pitch_b = Basis(Vector3.RIGHT, _pitch)
	self.basis = yaw_b * pitch_b

# ═══════════════════════════════════════════════════════════
# MOVEMENT & INPUT
# ═══════════════════════════════════════════════════════════
func _handle_movement(delta: float) -> void:
	if editor.is_ui_visible:
		return
	var speed := move_speed * (sprint_multiplier if Input.is_key_pressed(KEY_SHIFT) else 1.0)
	var dir := Vector3.ZERO
	
	var forward = -global_transform.basis.z
	var right = global_transform.basis.x
	
	if Input.is_key_pressed(KEY_W): dir += forward
	if Input.is_key_pressed(KEY_S): dir -= forward
	if Input.is_key_pressed(KEY_A): dir -= right
	if Input.is_key_pressed(KEY_D): dir += right
	if Input.is_key_pressed(KEY_Q): dir.y -= 1.0
	if Input.is_key_pressed(KEY_E): dir.y += 1.0
	
	if dir.length_squared() > 0.0:
		global_position += dir.normalized() * speed * delta

# ═══════════════════════════════════════════════════════════
# RAYCASTING & CYLINDER (THE MISSING STUFF)
# ═══════════════════════════════════════════════════════════
func _cast_center_ray() -> void:
	var space_state = get_world_3d().direct_space_state
	if not space_state: return
	
	var sc := get_viewport().get_visible_rect().size * 0.5
	var origin := project_ray_origin(sc)
	var dir := project_ray_normal(sc)
	var end := origin + dir * ray_length
	
	var q := PhysicsRayQueryParameters3D.create(origin, end)
	q.collision_mask = collision_mask
	var res := space_state.intersect_ray(q)
	
	if res.is_empty():
		terrain_hit_valid = false
		terrain_hit_position = end
	else:
		terrain_hit_valid = true
		terrain_hit_position = res["position"]

func _update_ray_position() -> void:
	if Ray_Cylinder == null: return # Failsafe if not assigned
	
	Ray_Cylinder.visible = true
	
	# Update position
	if terrain_hit_valid:
		Ray_Cylinder.global_position = terrain_hit_position + Vector3(0, 0.1, 0)
	else:
		Ray_Cylinder.global_position = global_position - global_transform.basis.z * 10.0
		
	# Update cylinder visual scale (radius)
	var mesh = Ray_Cylinder.mesh as CylinderMesh
	if mesh:
		mesh.top_radius = _cylinder_radius
		mesh.bottom_radius = _cylinder_radius
	
	# Keep it straight
	Ray_Cylinder.global_rotation = Vector3.ZERO
	
	editor.select_entities_in_radius_2D(Ray_Cylinder.global_position, _cylinder_radius)
