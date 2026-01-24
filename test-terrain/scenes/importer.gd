extends Node3D

var fadeNode: Node3D
func fadeInit():
	if(!get_tree().root.get_node_or_null("FadeInOut")):
		var fade_layer_scene = preload("res://scenes/FadeInOut.tscn")
		var new_layer = fade_layer_scene.instantiate()
		fadeNode=new_layer
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

func _ready() -> void:
	await get_tree().process_frame
	addFadeOut()
	#get_tree().reload_current_scene() # Restartuje aktuální scénu - bud

	#OS.set_restart_on_exit(true, ["--argumenty"]) # Volitelné argumenty - a nebo
	#get_tree().quit()
	
	#get_tree().paused = true
	var countdown_label = $Control/Label
	var seconds_left = 5
	while seconds_left > 0:
		countdown_label.text = "Hledám data ve složce:" + str(seconds_left) + "s"
		await get_tree().create_timer(1.0, true).timeout
		seconds_left -= 1
	countdown_label.text = ""
	#get_tree().paused = false
	
	addFadeIn()
	await fadeNode.fade_finished
	get_tree().change_scene_to_file("res://scenes/MainMenu.tscn")
