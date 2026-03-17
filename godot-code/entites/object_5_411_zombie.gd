@tool
extends Node3D

@export var fade_duration: float = 1.0
@export_range(0.0, 1.0) var min_alpha: float = 0.1

@export_tool_button("Fade Out") var _btn_out = Callable(self, "_apply_fade_out")
@export_tool_button("Fade In")  var _btn_in  = Callable(self, "_apply_fade_in")

var _tween: Tween
var _current_alpha: float = 1.0

enum FadeState { VISIBLE, HIDDEN, FADING_IN, FADING_OUT }
var _fade_state: FadeState = FadeState.VISIBLE

func fade_out() -> void: _fade(min_alpha, FadeState.FADING_OUT, FadeState.HIDDEN)
func fade_in()  -> void: _fade(1.0,       FadeState.FADING_IN,  FadeState.VISIBLE)

func get_fade_state() -> FadeState: return _fade_state

func _apply_fade_out() -> void: fade_out()
func _apply_fade_in()  -> void: fade_in()

func _fade(target_alpha: float, during: FadeState, after: FadeState) -> void:
	if _tween:
		_tween.kill()
		_current_alpha = _read_current_alpha()

	var alpha_range: float = abs(target_alpha - _current_alpha)
	var duration: float = fade_duration * alpha_range

	_fade_state = during
	_tween = create_tween().set_parallel(true)
	for mesh in _get_meshes():
		for i in mesh.mesh.get_surface_count():
			var mat := _get_unique_mat(mesh, i)
			if mat:
				mat.albedo_color.a = _current_alpha
				_tween.tween_property(mat, "albedo_color:a", target_alpha, duration)

	_tween.finished.connect(func():
		_current_alpha = target_alpha
		_fade_state = after
	, CONNECT_ONE_SHOT)

func _read_current_alpha() -> float:
	for mesh in _get_meshes():
		for i in mesh.mesh.get_surface_count():
			var mat := mesh.get_surface_override_material(i) as StandardMaterial3D
			if mat:
				return mat.albedo_color.a
	return _current_alpha

func _apply_alpha(value: float) -> void:
	_current_alpha = value
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
		mesh.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_ON
		mesh.set_surface_override_material(idx, u)
	return mesh.get_surface_override_material(idx)
