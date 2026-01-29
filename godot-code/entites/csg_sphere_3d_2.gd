@tool
extends CSGSphere3D
var angular_velocity = Vector3.ZERO
var friction = 0.98
var change_timer = 0.0
var time_passed = 0.0
@export_group("Pulsing Settings")
@export var pulse_speed: float = 2.0
@export var scale_amount: float = 0.2
@export var base_scale: float = 1.0
@export var glow_pulse: bool = true
func _process(delta):
	time_passed += delta
	change_timer -= delta
	if change_timer <= 0:
		var impulse = Vector3(
		randf_range(-1, 1),
		randf_range(-1, 1),
		randf_range(-1, 1)
		).normalized() * 1.5
		angular_velocity += impulse
		change_timer = randf_range(0.0, 0.5)
	rotate_x(angular_velocity.x * delta*5)
	rotate_y(angular_velocity.y * delta*5)
	rotate_z(angular_velocity.z * delta*5)
	angular_velocity *= pow(friction, delta * 60)	
	var pulse_wave = (sin(time_passed * pulse_speed) + 1.0) / 2.0	
	var s = base_scale + (pulse_wave * scale_amount)
	scale = Vector3(2*s, 2*s, 2*s)
	if glow_pulse and material:
		var emission_strength = 1.0 + (pulse_wave * 4.0)
		material.set_shader_parameter("emission_energy_multiplier", emission_strength)
