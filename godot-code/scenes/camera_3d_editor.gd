extends Camera3D

# ═══════════════════════════════════════════════════════════
#  SETTINGS – tweak in the Inspector
# ═══════════════════════════════════════════════════════════
@export_group("Movement")
@export var move_speed        : float = 10.0   # m/s base movement speed
@export var sprint_multiplier : float = 3.0    # Shift = fast movement
@export var mouse_sensitivity : float = 0.002  # radians per pixel
@export var pitch_limit       : float = 85.0   # max look up/down angle (degrees)

@export_group("Ray / Cone")
@export var ray_length          : float = 1000.0  # ray length in meters
@export var collision_mask      : int   = 1        # terrain collision layer
@export var cone_angle_min      : float = 1.0      # minimum cone half-angle (degrees)
@export var cone_angle_max      : float = 45.0     # maximum cone half-angle (degrees)
@export var cone_scroll_speed   : float = 1.5      # degrees changed per scroll tick
@export var cone_rays           : int   = 32        # rays used for cone overlap query
@export var cone_visual_opacity : float = 0.18     # transparency of cone mesh (0–1)

# ═══════════════════════════════════════════════════════════
#  INTERNAL STATE
# ═══════════════════════════════════════════════════════════
var _yaw   : float = 0.0
var _pitch : float = 0.0

# Ray hit result
var terrain_hit_position : Vector3 = Vector3.ZERO
var terrain_hit_valid    : bool    = false

# Cone mode
var cone_mode_active : bool  = false   # toggle with middle mouse button
var cone_angle       : float = 10.0   # current half-angle in degrees
var cone_hits        : Array = []      # Array[Vector3] – all hits inside cone

var _space_state : PhysicsDirectSpaceState3D

# ─── Visual nodes (created at runtime) ───────────────────
var _ray_mesh_instance  : MeshInstance3D   # the center ray line
var _cone_mesh_instance : MeshInstance3D   # translucent cone
var _hit_marker         : MeshInstance3D   # small sphere at ray hit point

# ImmediateMesh objects rebuilt each frame
var _ray_imm_mesh  : ImmediateMesh
var _cone_imm_mesh : ImmediateMesh

# Materials
var _ray_mat  : StandardMaterial3D
var _cone_mat : StandardMaterial3D
var _hit_mat  : StandardMaterial3D

# ═══════════════════════════════════════════════════════════
#  INITIALIZATION
# ═══════════════════════════════════════════════════════════
func _ready() -> void:
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
	_yaw   = rotation.y
	_pitch = rotation.x
	_build_visuals()

func _build_visuals() -> void:
	# ── Ray line material ──────────────────────────────────
	_ray_mat                    = StandardMaterial3D.new()
	_ray_mat.shading_mode       = BaseMaterial3D.SHADING_MODE_UNSHADED
	_ray_mat.albedo_color       = Color(1.0, 0.9, 0.1, 1.0)   # bright yellow
	_ray_mat.vertex_color_use_as_albedo = false

	_ray_imm_mesh               = ImmediateMesh.new()
	_ray_mesh_instance          = MeshInstance3D.new()
	_ray_mesh_instance.mesh     = _ray_imm_mesh
	_ray_mesh_instance.material_override = _ray_mat
	_ray_mesh_instance.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	add_child(_ray_mesh_instance)

	# ── Cone material (translucent) ────────────────────────
	_cone_mat                    = StandardMaterial3D.new()
	_cone_mat.shading_mode       = BaseMaterial3D.SHADING_MODE_UNSHADED
	_cone_mat.transparency       = BaseMaterial3D.TRANSPARENCY_ALPHA
	_cone_mat.albedo_color       = Color(0.2, 0.6, 1.0, cone_visual_opacity)
	_cone_mat.cull_mode          = BaseMaterial3D.CULL_DISABLED   # visible from inside too
	_cone_mat.no_depth_test      = false

	_cone_imm_mesh               = ImmediateMesh.new()
	_cone_mesh_instance          = MeshInstance3D.new()
	_cone_mesh_instance.mesh     = _cone_imm_mesh
	_cone_mesh_instance.material_override = _cone_mat
	_cone_mesh_instance.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	_cone_mesh_instance.visible  = false
	add_child(_cone_mesh_instance)

	# ── Hit marker sphere ──────────────────────────────────
	_hit_mat              = StandardMaterial3D.new()
	_hit_mat.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	_hit_mat.albedo_color = Color(1.0, 0.2, 0.1, 1.0)   # red dot

	var sphere_mesh          := SphereMesh.new()
	sphere_mesh.radius        = 0.15
	sphere_mesh.height        = 0.30
	sphere_mesh.radial_segments = 8
	sphere_mesh.rings         = 4

	_hit_marker              = MeshInstance3D.new()
	_hit_marker.mesh         = sphere_mesh
	_hit_marker.material_override = _hit_mat
	_hit_marker.cast_shadow  = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	_hit_marker.visible      = false
	get_parent().add_child(_hit_marker)   # world-space node (not child of camera)

# ═══════════════════════════════════════════════════════════
#  INPUT
# ═══════════════════════════════════════════════════════════
func _unhandled_input(event: InputEvent) -> void:
	# ── Mouse look ────────────────────────────────────────
	if event is InputEventMouseMotion:
		_yaw   -= event.relative.x * mouse_sensitivity
		_pitch -= event.relative.y * mouse_sensitivity
		_pitch  = clamp(_pitch, deg_to_rad(-pitch_limit), deg_to_rad(pitch_limit))
		_apply_rotation()

	# ── Mouse wheel → resize cone (only while cone mode is active) ──
	if event is InputEventMouseButton:
		match event.button_index:
			MOUSE_BUTTON_MIDDLE:
				# Middle click toggles cone mode
				if event.pressed:
					cone_mode_active = !cone_mode_active
					_cone_mesh_instance.visible = cone_mode_active

			MOUSE_BUTTON_WHEEL_UP:
				if cone_mode_active and event.pressed:
					cone_angle = clamp(cone_angle + cone_scroll_speed,
									   cone_angle_min, cone_angle_max)

			MOUSE_BUTTON_WHEEL_DOWN:
				if cone_mode_active and event.pressed:
					cone_angle = clamp(cone_angle - cone_scroll_speed,
									   cone_angle_min, cone_angle_max)

	# ── Escape ────────────────────────────────────────────
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

	if cone_mode_active:
		_cast_cone()
		_draw_cone_mesh()
	else:
		cone_hits = []

	_draw_ray_mesh()
	_update_hit_marker()

# ═══════════════════════════════════════════════════════════
#  MOVEMENT
# ═══════════════════════════════════════════════════════════
func _handle_movement(delta: float) -> void:
	var speed := move_speed
	if Input.is_key_pressed(KEY_SHIFT):
		speed *= sprint_multiplier

	var dir := Vector3.ZERO
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
	var viewport      := get_viewport()
	var screen_center := viewport.get_visible_rect().size * 0.5

	var ray_origin := project_ray_origin(screen_center)
	var ray_dir    := project_ray_normal(screen_center)
	var ray_end    := ray_origin + ray_dir * ray_length

	var query := PhysicsRayQueryParameters3D.create(ray_origin, ray_end)
	query.collision_mask      = collision_mask
	query.collide_with_bodies = true
	query.collide_with_areas  = false

	var result := _space_state.intersect_ray(query)

	if result.is_empty():
		terrain_hit_valid    = false
		terrain_hit_position = ray_end   # draw ray to max length even if no hit
	else:
		terrain_hit_valid    = true
		terrain_hit_position = result["position"]

# ═══════════════════════════════════════════════════════════
#  CONE MULTI-HIT
# ═══════════════════════════════════════════════════════════
func _cast_cone() -> void:
	cone_hits.clear()
	var seen_bodies : Dictionary = {}

	var viewport      := get_viewport()
	var screen_center := viewport.get_visible_rect().size * 0.5
	var ray_origin    := project_ray_origin(screen_center)
	var forward       := project_ray_normal(screen_center)

	# Build a perpendicular basis for scattering rays inside the cone
	var right := forward.cross(Vector3.UP)
	if right.length_squared() < 0.001:
		right = forward.cross(Vector3.RIGHT)
	right = right.normalized()
	var up_local := forward.cross(right).normalized()

	var half_rad := deg_to_rad(cone_angle)

	for i in cone_rays:
		var angle_around := (float(i) / float(cone_rays)) * TAU
		# Random-ish radial offset using a stratified ring
		var t : float = fmod(float(i) * 0.618033988, 1.0)   # golden ratio spread
		var r : float = sqrt(t) * half_rad                    # uniform disk distribution

		var offset := (cos(angle_around) * right + sin(angle_around) * up_local) * r
		var dir    := (forward + offset).normalized()
		var end    := ray_origin + dir * ray_length

		var query := PhysicsRayQueryParameters3D.create(ray_origin, end)
		query.collision_mask      = collision_mask
		query.collide_with_bodies = true
		query.collide_with_areas  = false

		var result := _space_state.intersect_ray(query)
		if not result.is_empty():
			var body : Object = result["collider"]
			if not seen_bodies.has(body):
				seen_bodies[body] = true
				cone_hits.append({
					"position" : result["position"],
					"collider" : body,
					"normal"   : result["normal"]
				})

# ═══════════════════════════════════════════════════════════
#  VISUALS – RAY LINE
# ═══════════════════════════════════════════════════════════
func _draw_ray_mesh() -> void:
	_ray_imm_mesh.clear_surfaces()

	# Draw in local camera space: origin = Vector3.ZERO, end = forward * distance
	var local_end := to_local(terrain_hit_position)

	_ray_imm_mesh.surface_begin(Mesh.PRIMITIVE_LINES)
	_ray_imm_mesh.surface_add_vertex(Vector3.ZERO)
	_ray_imm_mesh.surface_add_vertex(local_end)
	_ray_imm_mesh.surface_end()

# ═══════════════════════════════════════════════════════════
#  VISUALS – CONE MESH (translucent triangle fan)
# ═══════════════════════════════════════════════════════════
func _draw_cone_mesh() -> void:
	_cone_imm_mesh.clear_surfaces()

	var segments  : int   = 48           # smoothness of cone rim
	var half_rad  : float = deg_to_rad(cone_angle)
	var cone_len  : float = ray_length   # visual length matches ray

	# Forward in local space = -Z
	var forward   := Vector3(0, 0, -1)
	var right     := Vector3(1, 0,  0)
	var up_local  := Vector3(0, 1,  0)

	# Precompute rim points (in local camera space)
	var rim : Array[Vector3] = []
	for i in segments:
		var a := (float(i) / float(segments)) * TAU
		var offset := (cos(a) * right + sin(a) * up_local) * sin(half_rad)
		rim.append((forward + offset).normalized() * cone_len)

	# Side faces: triangle fan from apex (origin) to each rim edge
	_cone_imm_mesh.surface_begin(Mesh.PRIMITIVE_TRIANGLES)
	for i in segments:
		var a := rim[i]
		var b := rim[(i + 1) % segments]
		_cone_imm_mesh.surface_add_vertex(Vector3.ZERO)
		_cone_imm_mesh.surface_add_vertex(a)
		_cone_imm_mesh.surface_add_vertex(b)
	_cone_imm_mesh.surface_end()

	# Cap: filled circle at the far end
	var center_far := forward * cone_len
	_cone_imm_mesh.surface_begin(Mesh.PRIMITIVE_TRIANGLES)
	for i in segments:
		var a := rim[i]
		var b := rim[(i + 1) % segments]
		_cone_imm_mesh.surface_add_vertex(center_far)
		_cone_imm_mesh.surface_add_vertex(b)
		_cone_imm_mesh.surface_add_vertex(a)
	_cone_imm_mesh.surface_end()

# ═══════════════════════════════════════════════════════════
#  VISUALS – HIT MARKER
# ═══════════════════════════════════════════════════════════
func _update_hit_marker() -> void:
	if terrain_hit_valid:
		_hit_marker.visible  = true
		_hit_marker.position = terrain_hit_position
	else:
		_hit_marker.visible  = false

# ═══════════════════════════════════════════════════════════
#  PUBLIC API
# ═══════════════════════════════════════════════════════════

## Returns Vector2(X, Z) of center ray terrain hit. Vector2.ZERO if no hit.
func get_terrain_xz() -> Vector2:
	if terrain_hit_valid:
		return Vector2(terrain_hit_position.x, terrain_hit_position.z)
	return Vector2.ZERO

## Returns full Vector3 of center ray hit (Y = elevation).
func get_terrain_hit() -> Vector3:
	return terrain_hit_position if terrain_hit_valid else Vector3.ZERO

## Is the center ray hitting the terrain?
func is_terrain_hit_valid() -> bool:
	return terrain_hit_valid

## Returns all unique colliders hit inside the cone this frame.
## Each entry: { "position": Vector3, "collider": Object, "normal": Vector3 }
func get_cone_hits() -> Array:
	return cone_hits

## Current cone half-angle in degrees.
func get_cone_angle() -> float:
	return cone_angle
