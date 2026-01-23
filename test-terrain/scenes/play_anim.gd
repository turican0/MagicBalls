extends Node3D

func load_custom_texture(path: String) -> ImageTexture:#move to imports
	var img = Image.load_from_file(path)
	if img == null:
		return null        
	var tex = ImageTexture.create_from_image(img)    
	return tex


var fadeNode: Node3D
func fadeInit():
	if(!get_tree().root.get_node_or_null("FadeInOut")):
		var fade_layer_scene = preload("res://scenes/FadeInOut.tscn")  # uprav cestu podle svého projektu
		var new_layer = fade_layer_scene.instantiate()
		get_tree().root.add_child(new_layer)
		new_layer.name = "FadeInOut"
	if(!fadeNode):
		fadeNode=get_tree().root.get_node_or_null("FadeInOut")
func addFadeIn():
	fadeInit()
	fadeNode.start_fade(1.0, Color(0, 0, 0, 0),Color(0, 0, 0, 1))
func addFadeOut():
	fadeInit()
	fadeNode.start_fade(1.0, Color(0, 0, 0, 1),Color(0, 0, 0, 0))

func _ready():
	addFadeOut()
	showMyImg(0)
	
func showMyImg(index):
	const SPRITE_DIR = "res://convertdata/HSCREEN/"
	var file_name_spr
	if(index==0):
		file_name_spr="x_DWORD_E9C38_smalltit.png"	
	var sprback = $Control/Sprite2D
	sprback.name = "fullscrimg"
	var file_path_spr = SPRITE_DIR + file_name_spr
	var tex2 = load_custom_texture(file_path_spr)
	sprback.texture=tex2
	sprback.centered = true
	var screen_size = get_viewport().get_visible_rect().size
	sprback.position = screen_size / 2
	
	var tex_size = tex2.get_size()
	var scale_factor = Vector2(screen_size.x / tex_size.x, screen_size.y / tex_size.y)	
	# Pokud chceš zachovat poměr stran a mít černé pruhy (fit):
	# var final_scale = min(scale_factor.x, scale_factor.y)
	# Pokud chceš roztáhnout bez ohledu na deformaci:
	sprback.scale = scale_factor
	
	$Control.add_child(sprback)
	sprback.show()
