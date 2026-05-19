extends Node3D

func _process(delta: float) -> void:
	var word10=get_meta("word_10")
	if(word10==0):
		$Cylinder.hide()
	else:
		$Cylinder.mesh.top_radius=word10/2
		$Cylinder.mesh.bottom_radius=word10/2
		$Cylinder.show()
