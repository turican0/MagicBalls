extends Sprite3D
@onready var animated_sprite: AnimatedSprite2D = $AnimatedSprite2D

func _ready():
	# Nejdřív spusť animaci normálně
	animated_sprite.play("tvůj_animace_název")   # např. "idle" nebo "walk"
	
	# Pak okamžitě skoč na náhodný frame
	var pocet_framu = animated_sprite.sprite_frames.get_frame_count(animated_sprite.animation)
	if pocet_framu > 0:
		animated_sprite.frame = randi() % pocet_framu
