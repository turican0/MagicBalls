extends CanvasLayer

@onready var content: Control = get_child(0) 

var fade_tween: Tween
func _ready() -> void:
	visibility_changed.connect(_on_visibility_changed)

func _on_visibility_changed() -> void:
	if visible:
		start_fade_out()

func start_fade_out() -> void:
	if fade_tween:
		fade_tween.kill()
	content.modulate.a = 1.0	
	fade_tween = create_tween()
	fade_tween.tween_interval(5.0)
	fade_tween.tween_property(content, "modulate:a", 0.0, 2.0)
	fade_tween.tween_callback(self.hide)
