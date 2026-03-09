@tool
extends Node3D

@export var speed: Vector3 = Vector3(0, 45, 0)
func _process(delta: float) -> void:
	rotation_degrees += speed * delta
