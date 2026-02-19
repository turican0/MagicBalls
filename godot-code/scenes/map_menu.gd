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
	Main_DecodeLevel.mapMenuBegin($Control/Foreground)
	Main_DecodeLevel.changeLanguage(2)
	Main_DecodeLevel.getLangTexts()

func _process(delta) -> void:
	if(!runned):
		return
	var mapMenuAction = Main_DecodeLevel.mapMenuStep()

	match mapMenuAction:
		1:
			runned=false
			goGame()
		2:
			runned=false
			exitMenu()
		3:
			runned=false
		4:
			runned=false
		5:
			runned=false
		#endSpritesrender()

func endMapMenu():
	#Main_DecodeLevel.mapMenuEnd()
	Global.Main_Sounds.stopAllSounds()
	#endAnim()

func goGame():
	fadeNode = Global.addFadeIn(fadeNode)
	endMapMenu()
	await fadeNode.fade_finished
	Global.last_scene_path = get_tree().current_scene.scene_file_path
	get_tree().change_scene_to_file("res://scenes/CodeGeneratedDemo.tscn")

func exitMenu():
	fadeNode = Global.addFadeIn(fadeNode)
	endMapMenu()
	await fadeNode.fade_finished
	Global.last_scene_path = get_tree().current_scene.scene_file_path
	get_tree().change_scene_to_file("res://scenes/MainMenu.tscn")
