extends AnimationPlayer

func _ready() -> void:
	await get_tree().process_frame
	
	if current_animation_length > 0:
		var random_time = randf() * current_animation_length
		seek(random_time, true)
