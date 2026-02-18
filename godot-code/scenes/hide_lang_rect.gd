extends TextureRect
@export var fade_speed: float = 0.2
func _process(delta):
	var current_alpha = material.get_shader_parameter("alpha")
	if current_alpha > 0.0:
		var new_alpha = current_alpha - (fade_speed * delta)
		new_alpha = max(new_alpha, 0.0)
		material.set_shader_parameter("alpha", new_alpha)
