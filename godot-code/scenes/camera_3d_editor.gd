extends Camera3D

var Ray_Cylinder: MeshInstance3D = null
var terrain_node: Node3D = null

# ═══════════════════════════════════════════════════════════
# SETTINGS
# ═══════════════════════════════════════════════════════════
@export_group("Movement")
@export var move_speed : float = 10.0
@export var sprint_multiplier : float = 3.0
@export var mouse_sensitivity : float = 0.002
@export var pitch_limit : float = 89.0 # 89 brání artefaktům při úplném pohledu nahoru/dolů

@export_group("Ray")
@export var ray_length : float = 200.0
@export var collision_mask : int = 1

@export_group("Cylinder")
@export var cylinder_radius_min : float = 0.1
@export var cylinder_radius_max : float = 10.0
@export var cylinder_scroll_step : float = 0.3

# ═══════════════════════════════════════════════════════════
# REFERENCES
# ═══════════════════════════════════════════════════════════
@onready var yaw_pivot: Node3D = get_parent()

# ═══════════════════════════════════════════════════════════
# INTERNAL STATE
# ═══════════════════════════════════════════════════════════
var _yaw : float = 0.0
var _pitch : float = 0.0
var terrain_hit_position : Vector3 = Vector3.ZERO
var terrain_hit_valid : bool = false
var _cylinder_radius : float = 0.1
var _space_state : PhysicsDirectSpaceState3D

# ═══════════════════════════════════════════════════════════
# INITIALIZATION
# ═══════════════════════════════════════════════════════════
func _ready() -> void:
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
	
	# Inicializace úhlů z aktuálního stavu v editoru
	if yaw_pivot:
		_yaw = yaw_pivot.rotation.y
	_pitch = rotation.x
	
	# ← KLÍČOVÁ OPRAVA
	_pitch = clamp(_pitch, deg_to_rad(-pitch_limit), deg_to_rad(pitch_limit))
	_apply_rotation()  # Vynutí čistou rotaci (odstraní případnou roll nebo nečistotu z editoru)

# ═══════════════════════════════════════════════════════════
# INPUT
# ═══════════════════════════════════════════════════════════
func _unhandled_input(event: InputEvent) -> void:
	if event is InputEventMouseMotion:
		_yaw -= event.relative.x * mouse_sensitivity
		_pitch -= event.relative.y * mouse_sensitivity
		
		# Omezení vertikálního pohledu
		_pitch = clamp(_pitch, deg_to_rad(-pitch_limit), deg_to_rad(pitch_limit))
		_apply_rotation()
	
	if event is InputEventMouseButton:
		match event.button_index:
			MOUSE_BUTTON_WHEEL_UP:
				_cylinder_radius = clamp(_cylinder_radius + cylinder_scroll_step,
										 cylinder_radius_min, cylinder_radius_max)
			MOUSE_BUTTON_WHEEL_DOWN:
				_cylinder_radius = clamp(_cylinder_radius - cylinder_scroll_step,
										 cylinder_radius_min, cylinder_radius_max)
	
	if event is InputEventKey and event.pressed and event.keycode == KEY_ESCAPE:
		if Input.get_mouse_mode() == Input.MOUSE_MODE_CAPTURED:
			Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
		else:
			Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)

# ═══════════════════════════════════════════════════════════
# GAME LOOP
# ═══════════════════════════════════════════════════════════
func _process(delta: float) -> void:
	_handle_movement(delta)
	_space_state = get_world_3d().direct_space_state
	_cast_center_ray()
	_update_ray_position()

# ═══════════════════════════════════════════════════════════
# MOVEMENT & ROTATION
# ═══════════════════════════════════════════════════════════
func _apply_rotation() -> void:
	# KLÍČOVÁ OPRAVA: Oddělení rotací (yaw na parentu, pitch na kameře)
	# Tímto způsobem je yaw vždy kolem světové Y osy → žádný gimbal lock
	yaw_pivot.rotation = Vector3(0, _yaw, 0)
	# Kamera rotuje pouze lokálně kolem X
	rotation = Vector3(_pitch, 0, 0)

func _handle_movement(delta: float) -> void:
	var speed := move_speed * (sprint_multiplier if Input.is_key_pressed(KEY_SHIFT) else 1.0)
	var dir := Vector3.ZERO
	
	# Směry bereme z globální transformace kamery (letíme kam koukáme)
	var forward = -global_transform.basis.z
	var right   =  global_transform.basis.x
	
	if Input.is_key_pressed(KEY_W) or Input.is_key_pressed(KEY_UP):    dir += forward
	if Input.is_key_pressed(KEY_S) or Input.is_key_pressed(KEY_DOWN):  dir -= forward
	if Input.is_key_pressed(KEY_A) or Input.is_key_pressed(KEY_LEFT):  dir -= right
	if Input.is_key_pressed(KEY_D) or Input.is_key_pressed(KEY_RIGHT): dir += right
	
	if Input.is_key_pressed(KEY_Q): dir.y -= 1.0
	if Input.is_key_pressed(KEY_E): dir.y += 1.0
	
	if dir.length_squared() > 0.0:
		yaw_pivot.global_position += dir.normalized() * speed * delta

# ═══════════════════════════════════════════════════════════
# RAYCASTING & UTILS
# ═══════════════════════════════════════════════════════════
func _cast_center_ray() -> void:
	if _space_state == null: return
	
	var sc := get_viewport().get_visible_rect().size * 0.5
	var origin := project_ray_origin(sc)
	var dir := project_ray_normal(sc)
	var end := origin + dir * ray_length
	
	var q := PhysicsRayQueryParameters3D.create(origin, end)
	q.collision_mask = collision_mask
	
	var res := _space_state.intersect_ray(q)
	
	if res.is_empty():
		terrain_hit_valid = false
		terrain_hit_position = end
	else:
		terrain_hit_valid = true
		terrain_hit_position = res["position"]

func _update_ray_position() -> void:
	if Ray_Cylinder == null: return
	
	Ray_Cylinder.visible = true
	if terrain_hit_valid:
		Ray_Cylinder.global_position = terrain_hit_position + Vector3(0, 0.05, 0)
	else:
		Ray_Cylinder.global_position = global_position + (-global_transform.basis.z * 10.0)
	
	Ray_Cylinder.global_rotation = Vector3.ZERO # Válec stojí vždy rovně
	
	var cyl := Ray_Cylinder.mesh as CylinderMesh
	if cyl:
		cyl.top_radius = _cylinder_radius
		cyl.bottom_radius = _cylinder_radius

# ═══════════════════════════════════════════════════════════
# TERRAIN DATA (Helpery zůstávají stejné)
# ═══════════════════════════════════════════════════════════
func get_terrain_uv(world_pos: Vector3) -> Vector2:
	if terrain_node == null: return Vector2(-1, -1)
	var local_pos = terrain_node.to_local(world_pos)
	var aabb_size = terrain_node.mesh.get_aabb().size
	return Vector2((local_pos.x / aabb_size.x) + 0.5, (local_pos.z / aabb_size.z) + 0.5)

func get_terrain_height(world_pos: Vector3) -> float:
	if terrain_node == null: return 0.0
	var uv = get_terrain_uv(world_pos)
	var material = terrain_node.get_surface_override_material(0) as ShaderMaterial
	if material and material.has_shader_parameter("heightmap"):
		var tex: Texture2D = material.get_shader_parameter("heightmap")
		if tex:
			var img = tex.get_image()
			var color = img.get_pixelv(uv * Vector2(img.get_size()))
			var h_scale = material.get_shader_parameter("height_scale") or 10.0
			return color.r * h_scale
	return 0.0
