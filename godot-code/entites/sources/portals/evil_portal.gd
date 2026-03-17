extends Node3D
func _process(_delta):
	var camera = get_viewport().get_camera_3d()	
	if camera:
		var target_pos = camera.global_position		
		target_pos.y = global_position.y
		if global_position.distance_to(target_pos) > 0.001:
			look_at(target_pos, Vector3.UP)
			rotate_y(PI)
