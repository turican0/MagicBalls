@tool
extends Node3D


@export var start_pos: Vector3 = Vector3.ZERO:
	set(value):
		start_pos = value
		_apply()

@export var end_pos: Vector3 = Vector3(0, 5, 0):
	set(value):
		end_pos = value
		_apply()

@export var shaft_radius: float = 0.05:
	set(value):
		shaft_radius = value
		_apply()

@export var head_height: float = 0.4:
	set(value):
		head_height = value
		_apply()

@export var head_radius: float = 0.12:
	set(value):
		head_radius = value
		_apply()

@export var arrow_color: Color = Color(1.0, 0.5, 0.0):
	set(value):
		arrow_color = value
		_apply()

var _cylinder: MeshInstance3D:
	get:
		return get_node_or_null("Shaft") as MeshInstance3D

var _cone: MeshInstance3D:
	get:
		return get_node_or_null("Head") as MeshInstance3D

func _ready() -> void:
	_apply()

func update_arrow(from: Vector3, to: Vector3) -> void:
	start_pos = from
	end_pos = to

func _apply() -> void:
	# @onready vars are not set until _ready() runs
	if not is_node_ready():
		return
	if not is_instance_valid(_cylinder) or not is_instance_valid(_cone):
		return

	var dir := end_pos - start_pos
	var dist := dir.length()

	if dist < 0.001:
		hide()
		return
	show()

	global_position = (start_pos + end_pos) * 0.5

	var up := Vector3.UP
	var dir_n := dir.normalized()
	var dot := up.dot(dir_n)

	if abs(dot) > 0.9999:
		global_transform.basis = Basis.IDENTITY if dot > 0 else Basis(Vector3.RIGHT, PI)
	else:
		var axis := up.cross(dir_n).normalized()
		var angle := up.angle_to(dir_n)
		global_transform.basis = Basis(axis, angle)

	var shaft_len: float = max(dist - head_height, 0.01)

	# Shaft
	var cyl_mesh := _cylinder.mesh as CylinderMesh
	if cyl_mesh:
		cyl_mesh.top_radius = shaft_radius
		cyl_mesh.bottom_radius = shaft_radius
	_cylinder.scale = Vector3(1.0, shaft_len, 1.0)
	_cylinder.position = Vector3(0.0, -head_height * 0.5, 0.0)

	# Head
	var cone_mesh := _cone.mesh as CylinderMesh
	if cone_mesh:
		cone_mesh.bottom_radius = head_radius
		cone_mesh.height = head_height
	_cone.position = Vector3(0.0, shaft_len * 0.5, 0.0)

	# Material
	var mat := _cylinder.get_surface_override_material(0) as StandardMaterial3D
	if not mat:
		mat = StandardMaterial3D.new()
		mat.emission_enabled = true
		_cylinder.set_surface_override_material(0, mat)
		_cone.set_surface_override_material(0, mat)
	mat.albedo_color = arrow_color
	mat.emission = arrow_color * 0.4
