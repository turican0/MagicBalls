extends CanvasLayer

@onready var content: Control = get_child(0) 

var fade_tween: Tween

func start_fade_out() -> void:
	if fade_tween:
		fade_tween.kill()
	content.modulate.a = 1.0
	self.show()
	fade_tween = create_tween()
	fade_tween.tween_interval(3.0)
	fade_tween.tween_property(content, "modulate:a", 0.0, 2.0)
	fade_tween.tween_callback(self.hide)
