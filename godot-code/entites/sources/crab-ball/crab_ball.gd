extends Node3D

@export var max_dist: float = 30.0
@export var full_dist: float = 20.0

var meshes: Array[MeshInstance3D] = []

func _ready() -> void:
	_collect_meshes(self)
	for m in meshes:
		for i in m.get_surface_override_material_count():
			var mat = m.get_active_material(i)
			if mat:
				var new_mat = mat.duplicate()
				new_mat.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
				m.set_surface_override_material(i, new_mat)

func _collect_meshes(node: Node) -> void:
	if node is MeshInstance3D:
		meshes.append(node)
	for child in node.get_children():
		_collect_meshes(child)

func _process(_delta: float) -> void:
	var cam = get_viewport().get_camera_3d()
	if not cam: return

	var dist = global_position.distance_to(cam.global_position)
	var alpha = clamp(remap(dist, max_dist, full_dist, 0.0, 1.0), 0.0, 1.0)
	
	visible = alpha > 0.0
	if visible:
		for m in meshes:
			for i in m.get_surface_override_material_count():
				m.get_surface_override_material(i).albedo_color.a = alpha
