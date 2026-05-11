extends Node3D

func updateHealthBar(actLife: float, actMaxLife: float, visible:bool) -> void:
	if(visible):
		$SubViewport/VBoxContainer.show()
	else:
		$SubViewport/VBoxContainer.hide()
	var value=0
	if(actMaxLife>0):
		value = actLife/actMaxLife*100.0
	$SubViewport/VBoxContainer/HealthBar.value = value
