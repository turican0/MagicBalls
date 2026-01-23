extends Node3D

signal fade_finished

var fade_layer: CanvasLayer = null
var fade_rect: ColorRect = null

func start_fade(duration: float,from:Color,to:Color):
	if fade_layer == null:
		fade_layer = CanvasLayer.new()
		fade_layer.layer = 128
		fade_layer.name = "GlobalFadeLayer" 
		add_child(fade_layer)
		fade_rect = ColorRect.new()
		fade_rect.name = "FadeRect"
		fade_rect.mouse_filter = Control.MOUSE_FILTER_IGNORE
		fade_rect.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
		fade_layer.add_child(fade_rect)	
	fade_rect.color = from
	fade_rect.show()
	var tween = get_tree().create_tween()
	# Animujeme vlastnost "color" (barvu)
	# Z Color(0,0,0,1) -> černá neprůhledná
	# Do Color(0,0,0,0) -> černá úplně průhledná
	# Doba trvání: 2.0 sekundy
	tween.tween_property(fade_rect, "color", to, duration)
	tween.finished.connect(func():
		fade_finished.emit()
		#queue_free()
	)
