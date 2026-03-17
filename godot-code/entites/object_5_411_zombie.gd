@tool
extends Node3D

@export var fade_duration: float = 1.0
@export_range(0.0, 1.0) var preview_alpha: float = 1.0 :
	set(v):
		preview_alpha = v
		if Engine.is_editor_hint():
			_apply_alpha(v)

@export_tool_button("Fade Out") var _btn_out = func(): fade_out()
@export_tool_button("Fade In")  var _btn_in  = func(): fade_in()

var _tween: Tween

@export_range(0.0, 1.0) var min_alpha: float = 0.1

func fade_out() -> void: _fade(min_alpha)
func fade_in()  -> void: _fade(1.0)

func _fade(target_alpha: float) -> void:
	if _tween: _tween.kill()
	_tween = create_tween().set_parallel(true)
	for mesh in _get_meshes():
		for i in mesh.mesh.get_surface_count():
			var mat := _get_unique_mat(mesh, i)
			if mat: _tween.tween_property(mat, "albedo_color:a", target_alpha, fade_duration)

func _apply_alpha(value: float) -> void:
	for mesh in _get_meshes():
		for i in mesh.mesh.get_surface_count():
			var mat := _get_unique_mat(mesh, i)
			if mat: mat.albedo_color.a = value

func _get_meshes() -> Array:
	return find_children("*", "MeshInstance3D", true, false)

func _get_unique_mat(mesh: MeshInstance3D, idx: int) -> StandardMaterial3D:
	var mat := mesh.get_active_material(idx)
	if not mat: return null
	if not mesh.get_surface_override_material(idx):
		var u := mat.duplicate() as StandardMaterial3D
		if not u: return null
		u.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
		mesh.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_ON  # ← tady
		mesh.set_surface_override_material(idx, u)
	return mesh.get_surface_override_material(idx)
