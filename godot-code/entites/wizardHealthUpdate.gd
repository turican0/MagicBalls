extends Node3D

func updateHealthBar(actLife: float, actMaxLife: float) -> void:
	var value=0
	if(actMaxLife>0):
		value = actLife/actMaxLife*100.0
	$SubViewport/VBoxContainer/HealthBar.value = value
