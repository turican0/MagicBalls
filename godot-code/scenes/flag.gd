extends SoftBody3D

@export var wind_direction : Vector3 = Vector3(0, 0, 1)
@export var wind_strength : float = 20.0
@export var noise_speed : float = 3.0
@export var noise_scale : float = 3.0
@export var direction_variation : float = 0.4
# Gust settings
@export var gust_min : float = 0.5
@export var gust_max : float = 2.0
@export var gust_speed : float = 0.3

var noise : FastNoiseLite
var time_passed : float = 0.0

func _ready() -> void:
	noise = FastNoiseLite.new()
	noise.noise_type = FastNoiseLite.TYPE_PERLIN
	noise.frequency = 0.5

func _physics_process(delta: float) -> void:
	time_passed += delta * noise_speed

	# Smoothly vary wind strength over time
	var noise_strength = noise.get_noise_1d(time_passed)
	var current_strength = wind_strength + noise_scale * (noise_strength - 0.5)

	# Gust multiplier — slow independent noise channel, remapped to [gust_min, gust_max]
	var gust_noise = noise.get_noise_1d(time_passed * gust_speed + 300.0)
	var gust_multiplier = remap(gust_noise, -1.0, 1.0, gust_min, gust_max)

	# Slowly rotate wind direction
	var noise_yaw = noise.get_noise_1d(time_passed + 100.0) * direction_variation
	var noise_pitch = noise.get_noise_1d(time_passed + 200.0) * direction_variation * 0.5

	var rotated = wind_direction.normalized()
	rotated = rotated.rotated(Vector3.UP, noise_yaw)
	rotated = rotated.rotated(Vector3.RIGHT, noise_pitch)

	apply_central_force(rotated * current_strength * gust_multiplier)
