@tool
extends Node3D

@export var radius := 1.5        # poloměr kruhu
@export var angular_speed := 1.0 # rychlost rotace
@export var y_amplitude := 0.5   # výška vlnění
@export var y_speed := 1.2       # rychlost vlnění
@export var noise_strength := 0.15
@export var speed := 1.0

var angle := 0.0
var time := 0.0
var noise := FastNoiseLite.new()

func _ready():
	noise.seed = randi()
	noise.noise_type = FastNoiseLite.TYPE_SIMPLEX

func _process(delta):
	rotate_y(speed * delta)
	angle += angular_speed * delta
	angle += noise.get_noise_1d(time) * noise_strength
	time += delta
	var x = cos(angle) * radius
	var z = sin(angle) * radius
	#var y = sin(time * y_speed) * y_amplitude
	#y += noise.get_noise_1d(time) * noise_strength
	position = Vector3(x, position.y, z)
