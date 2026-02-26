extends Node3D
@export var rot_speed: float = 90.0 # základní rychlost z editoru

var axis: Vector3
var speed: float

func _ready():
	randomize()
	axis = Vector3(randf_range(-1,1), randf_range(-1,1), randf_range(-1,1)).normalized()
	speed = rot_speed * randf_range(-1.0, 1.0)

func _process(delta):
	rotate(axis, deg_to_rad(speed) * delta)
