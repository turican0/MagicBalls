extends Node

var is_edit_mode: bool = false

func _ready():
	_parse_command_line_args()
	if is_edit_mode:
		print("Starting in EDITOR mode")
		get_tree().change_scene_to_file("res://scenes-editor/Editor.tscn")
	else:
		print("Starting in GAME mode")
		if !check_existing_data():
			get_tree().change_scene_to_file("res://scenes/Importer.tscn")
		else:
			await get_tree().process_frame	
			_go_to_main_menu()

func _go_to_main_menu():
	Global.last_scene_path = "res://scenes/Settings.tscn"
	get_tree().change_scene_to_file("res://scenes/Settings.tscn")
			
func check_existing_data() -> bool:
	var music_path = Global.convertdata + "musics/"
	return DirAccess.dir_exists_absolute(music_path)

func _parse_command_line_args():
	var args = OS.get_cmdline_args()
	for i in range(args.size()):
		var argument = args[i]
		match argument:
			"--edit":
				is_edit_mode = true
			"--custom_level":
				if i + 1 < args.size():
					Global.target_level = args[i + 1]
