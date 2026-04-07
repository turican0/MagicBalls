extends Node3D

func _ready():
	var args = OS.get_cmdline_args()
	if "--edit" in args:
		get_tree().change_scene_to_file("res://scenes-editor/Editor.tscn")
	else:
		get_tree().change_scene_to_file("res://scenes/Importer.tscn")
