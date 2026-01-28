extends Node3D

@onready var file_dialog = $Control/FileDialog

var fadeNode: Node3D

var Main_DecodeLevel
func _ready() -> void:
	Main_DecodeLevel = get_node("DecodeLevel")
	await get_tree().process_frame
	fadeNode=Global.addFadeOut(fadeNode)
	if !check_existing_data():
		file_dialog.dir_selected.connect(_on_file_dialog_dir_selected)
		show_default_dialog()
	else:
		goFirstMenu()

func goFirstMenu():
	fadeNode=Global.addFadeIn(fadeNode)
	await fadeNode.fade_finished
	Global.last_scene_path = "res://scenes/MainMenu.tscn"
	get_tree().change_scene_to_file("res://scenes/PlayAnim.tscn")
	
func check_existing_data() -> bool:
	var music_path = "res://convertdata/musics/"
	if DirAccess.dir_exists_absolute(music_path):
		return true
	else:
		return false

func show_default_dialog():
	var dialog = AcceptDialog.new()
	dialog.dialog_text = "Find the installation path for the GOG version of Magic Carpet 2"
	dialog.ok_button_text = "GO"	
	dialog.confirmed.connect(_on_select_manual)	
	add_child(dialog)
	dialog.popup_centered()

func _on_select_manual():
	file_dialog.access = FileDialog.ACCESS_FILESYSTEM
	file_dialog.file_mode = FileDialog.FILE_MODE_OPEN_DIR	
	var starting_path = ""
	if OS.get_name() == "Windows":
		starting_path = "C:/"
	else:
		starting_path = "/"
	var paths_to_try = [
		"C:/GAMES/Magic Carpet 2",
		"C:/HRY/Magic Carpet 2"
	]
	for path in paths_to_try:
		if DirAccess.dir_exists_absolute(path):
			starting_path = path
			break
	file_dialog.current_dir = starting_path
	file_dialog.title = "Open Original Installation Path for the GOG version of Magic Carpet 2"
	var screen_size = get_viewport().get_visible_rect().size
	file_dialog.size = screen_size * 0.8
	file_dialog.popup_centered()

func _on_file_dialog_dir_selected(dir):
	Main_DecodeLevel.MBEXinit()
	Main_DecodeLevel.MBEXextractCD("c:/prenos/godot-zyllan/MagicBalls/gitextension/godot-cpp-template/data/", dir)
	Main_DecodeLevel.init()
	Main_DecodeLevel.MBEXconvert("res://convertdata",dir)
	goFirstMenu()
