extends Node

var Main_DecodeLevel
var Main_Player
var Main_UI
func _ready() -> void:
	Engine.max_fps = 60	
	Main_DecodeLevel = get_node("DecodeLevel") 
	Main_Player = get_node("Player")
	Main_UI = get_node("UI")
	Main_DecodeLevel.Main_Player = Main_Player
	Main_DecodeLevel.Main_UI = Main_UI
	$UI.player = $Player
	#$CanvasUI.init()
