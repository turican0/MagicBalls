extends Node3D

func _ready():
	randomize()	
	var children = get_children()	
	if children.size() > 0:
		for child in children:
			if child is Node3D:
				child.visible = false
		var random_index = randi() % children.size()
		children[random_index].visible = true
		children[random_index].rotation_degrees.y = randf_range(0, 360)
