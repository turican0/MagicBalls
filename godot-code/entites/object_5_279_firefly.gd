extends Node3D
# Firefly: plays the flight loop and flashes the lantern like a real firefly (Photinus) - a quick
# rise, a slower fade and dark pauses of random length, short and long, sometimes a short burst.
# The emission of the lantern (mesh "Lantern", material M_Lantern) and the light in the abdomen
# follow the same curve.

@export var light: NodePath # OmniLight3D, put at the lantern
@export var glow_min: float = 0.05 # emission energy between flashes (the organ is pale, not lit)
@export var glow_max: float = 2.5
@export var light_min: float = 0.35 # light energy between flashes (the old firefly's light had 1.0)
@export var light_max: float = 1.6
@export var rise_time: float = 0.15
@export var fade_time: float = 0.4
@export var pause_min: float = 1.0 # dark pause between flashes (or bursts)
@export var pause_max: float = 9.0

var _mat: StandardMaterial3D
var _light: OmniLight3D
var _rng := RandomNumberGenerator.new()
var _t := 0.0
var _rise := 0.15
var _hold := 0.05
var _fade := 0.4
var _peak := 1.0
var _wait := 0.0 # dark time before the next flash
var _burst := 0 # flashes left in the current burst
var _level := -1.0

func _ready() -> void:
	_rng.randomize()
	_light = get_node_or_null(light) as OmniLight3D
	var mesh := find_child("Lantern", true, false) as MeshInstance3D
	if mesh:
		var m := mesh.get_active_material(0)
		if m is StandardMaterial3D:
			_mat = m.duplicate() # every firefly flashes on its own
			_mat.emission_enabled = true
			mesh.set_surface_override_material(0, _mat)
		if _light:
			_light.global_position = mesh.global_transform * mesh.get_aabb().get_center()
	var anim := find_child("AnimationPlayer", true, false) as AnimationPlayer
	# the flight loop of the model ("Fly-loop" in the glb, "Fly" after the import of the .blend)
	var fly := "Fly" if anim and anim.has_animation("Fly") else "Fly-loop"
	if anim and anim.has_animation(fly):
		anim.get_animation(fly).loop_mode = Animation.LOOP_LINEAR
		anim.play(fly)
		anim.seek(_rng.randf_range(0.0, anim.current_animation_length)) # not all in step
	_wait = _rng.randf_range(0.0, pause_max)
	_apply(0.0)

func _next_flash() -> void:
	_rise = rise_time * _rng.randf_range(0.7, 1.4)
	_hold = _rng.randf_range(0.02, 0.15)
	_fade = fade_time * _rng.randf_range(0.7, 1.8)
	_peak = _rng.randf_range(0.7, 1.0)
	if _burst > 0:
		_burst -= 1
		_wait = _rng.randf_range(0.35, 0.8) # flashes of a burst follow each other closely
	elif _rng.randf() < 0.25:
		_burst = _rng.randi_range(1, 4)
		_wait = _rng.randf_range(0.35, 0.8)
	else:
		# more short pauses than long ones
		_wait = lerpf(pause_min, pause_max, pow(_rng.randf(), 1.6))

func _process(delta: float) -> void:
	_t += delta
	var level := 0.0
	if _t >= _wait:
		var f := _t - _wait
		if f < _rise:
			level = smoothstep(0.0, 1.0, f / _rise)
		elif f < _rise + _hold:
			level = 1.0
		elif f < _rise + _hold + _fade:
			var k := (f - _rise - _hold) / _fade
			level = (1.0 - k) * (1.0 - k) # the fade slows down at the end
		else:
			_t = 0.0
			_next_flash()
		level *= _peak
	if absf(level - _level) > 0.002:
		_apply(level)

func _apply(level: float) -> void:
	_level = level
	if _mat:
		_mat.emission_energy_multiplier = lerpf(glow_min, glow_max, level)
	if _light:
		_light.light_energy = lerpf(light_min, light_max, level)
