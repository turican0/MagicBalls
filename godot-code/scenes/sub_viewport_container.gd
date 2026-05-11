@tool
extends SubViewportContainer

var screen_size: Vector2

func _ready():
	get_tree().root.size_changed.connect(_update_layout)
	_update_layout()

func _update_layout():
	var screen_size = get_parent().size
	var extra_ratio = 0.4
	var newWcoef=(1-extra_ratio+extra_ratio/2)*2
	size = Vector2(screen_size.x * newWcoef, screen_size.y)
	position = Vector2(0, 0)
	#$SubViewport.size = Vector2i(int(screen_size.x + extra_width), int(screen_size.y))


func _process(delta):
	##print("screen_size: ", screen_size)
	#_on_size_changed()
	_update_layout()
	
	#print("screen_get_usable_rect: ", DisplayServer.screen_get_usable_rect().size)
	#var screen_size = Vector2(get_node("../Camera3D").get_viewport().get_visible_rect().size)
	#var extra_ratio = 0.4
	##if mapMode:
	#var extra_width = screen_size.x * extra_ratio
	##$SubViewport.size = Vector2i(int(screen_size.x + extra_width), int(screen_size.y))
#
	## Container zobrazuje jen pravou část - ořez přes size a position
	##size = screen_size #Vector2i(int(screen_size.x + extra_width), int(screen_size.y))
	#position = Vector2.ZERO
	##else:
		##size = screen_size
		##position = Vector2.ZERO
		##$SubViewport.size = Vector2i(screen_size)
