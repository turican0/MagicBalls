extends Node3D

@onready var foreground = $Control/Foreground

var runned = false

var fadeNode: Node3D

var Main_DecodeLevel
func _ready():
	await get_tree().process_frame
	Engine.max_fps = 25
	Main_DecodeLevel = get_node("DecodeLevel")
	fadeNode = Global.addFadeOut(fadeNode)
	startMenuLoop()

func startMenuLoop():
	runned = true
	menuInit()

func menuInit():
	Main_DecodeLevel.init()
	Global.initSound()
	Global.Main_Sounds.setSoundBank(0)
	Input.set_mouse_mode(Input.MOUSE_MODE_HIDDEN)
	Main_DecodeLevel.mainMenuBegin($Control/Foreground)
	Global.countLang=getLangCount()
	Main_DecodeLevel.changeLanguage(Global.defaultLangIndex+1)
	Main_DecodeLevel.getLangTexts()
	set_language_texture(Global.defaultLangIndex+1)

func _process(delta) -> void:
	if(!runned):
		return
	var mapMenuAction = Main_DecodeLevel.mainMenuStep()

	match mapMenuAction:
		1:
			runned=false
			goMapMenu()
		2:
			runned=false
			exitGame()
		3:
			runned=false
		4:
			Global.defaultLangIndex=(Global.defaultLangIndex+1)%Global.countLang;
			set_language_texture(Global.defaultLangIndex+1)
			Main_DecodeLevel.changeLanguage(Global.defaultLangIndex+1)
			$Control/LangRect.material.set_shader_parameter("alpha", 1.0)
		5:
			runned=false
		#endSpritesrender()

func endMapMenu():
	Global.Main_Sounds.stopAllSounds()
	#endAnim()

func goMapMenu():
	fadeNode = Global.addFadeIn(fadeNode)
	endMapMenu()
	await fadeNode.fade_finished
	Global.last_scene_path = get_tree().current_scene.scene_file_path
	get_tree().change_scene_to_file("res://scenes/MapMenu.tscn")

func exitGame():#fix this - add correct end game
	fadeNode = Global.addFadeIn(fadeNode)
	endMapMenu()
	await fadeNode.fade_finished
	get_tree().quit()

	
func set_language_texture(lang_id: int) -> bool:
	var base_path := "user://convertdata/language/"
	var file_name := "L%d.TXT.png" % lang_id
	var full_path := base_path + file_name	
	if not FileAccess.file_exists(full_path):
		push_error("File not found: " + full_path)
		return false
	var image := Image.new()
	var err := image.load(full_path)	
	if err != OK:
		push_error("Can not read image: " + full_path)
		return false	
	var texture := ImageTexture.create_from_image(image)
	$Control/LangRect.texture = texture	
	return true

func getLangCount() -> int:
	var path := "user://convertdata/language/"
	var count := 0
	
	var dir := DirAccess.open(path)
	if dir == null:
		push_error("Nelze otevřít složku: " + path)
		return 0	
	dir.list_dir_begin()
	var file_name = dir.get_next()	
	while file_name != "":
		if not dir.current_is_dir():
			if file_name.begins_with("L") and file_name.ends_with(".TXT.png"):
				count += 1		
		file_name = dir.get_next()	
	dir.list_dir_end()	
	return count
