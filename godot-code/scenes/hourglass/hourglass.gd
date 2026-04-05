extends AnimatedSprite2D

func _ready():
	position = Vector2.ZERO
	centered = true
	if sprite_frames.has_animation("default"):
		play("default")
	else:
		play(sprite_frames.get_animation_names()[0])
	speed_scale = 1.0
