extends Node3D

@onready var particles = $GPUParticles3D

func _ready():
	$GPUParticles3D.emitting = true
