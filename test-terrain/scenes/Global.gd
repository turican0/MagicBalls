extends Node

var last_scene_path: String = ""
var MBEX = null
var terrainInited:bool = false
var soundInited:bool = false

func fadeInit(fadeNode):
	if(!get_tree().root.get_node_or_null("FadeInOut")):
		var fade_layer_scene = preload("res://scenes/FadeInOut.tscn")
		var new_layer = fade_layer_scene.instantiate()
		fadeNode=new_layer
		get_tree().root.add_child(new_layer)
		new_layer.name = "FadeInOut"
	return get_tree().root.get_node_or_null("FadeInOut")
	
func addFadeIn(fadeNode):
	fadeNode=fadeInit(fadeNode)
	fadeNode.start_fade(1.0, Color(0, 0, 0, 0),Color(0, 0, 0, 1))
	return fadeNode
func addFadeOut(fadeNode):
	fadeNode=fadeInit(fadeNode)
	fadeNode.start_fade(1.0, Color(0, 0, 0, 1),Color(0, 0, 0, 0))
	return fadeNode

#Imports
func load_custom_texture(path: String) -> ImageTexture:
	var img = Image.load_from_file(path)
	if img == null:
		return null
	var tex = ImageTexture.create_from_image(img)
	return tex
