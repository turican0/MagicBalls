extends Node3D
@export_range(0, 360) var offset_uhlu: float = 75.0
func _process(_delta):
	var camera = get_viewport().get_camera_3d()	
	if camera:
		var target_pos = camera.global_position		
		target_pos.y = global_position.y
		if global_position.distance_to(target_pos) > 0.001:
			look_at(target_pos, Vector3.UP)
			rotate_y(deg_to_rad(offset_uhlu))
