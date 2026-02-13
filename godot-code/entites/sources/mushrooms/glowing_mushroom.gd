extends Node3D

func _ready():
	randomize() 
	var nahodny_uhel = randf_range(0, 360)
	rotation_degrees.y = nahodny_uhel
