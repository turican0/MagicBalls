extends Node

var is_edit_mode: bool = false

func _ready():
	_parse_command_line_args()
	if is_edit_mode:
		print("Starting in EDITOR mode")
		get_tree().change_scene_to_file("res://scenes-editor/Editor.tscn")
	else:
		print("Starting in GAME mode")
		get_tree().change_scene_to_file("res://scenes/Importer.tscn")

func _parse_command_line_args():
	var args = OS.get_cmdline_args()
	for i in range(args.size()):
		var argument = args[i]
		match argument:
			"--edit-mode":
				is_edit_mode = true
			"--custom-level":
				if i + 1 < args.size():
					Global.target_level = args[i + 1]
