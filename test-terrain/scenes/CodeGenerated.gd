extends Node

var Main_DecodeLevel
var Main_Player
var Main_UI
var Main_Filter
func _ready() -> void:
	Engine.max_fps = 60	
	Main_DecodeLevel = get_node("DecodeLevel") 
	Main_Player = get_node("Player")
	Main_UI = get_node("UI")
	Main_Filter = get_node("CanvasLayer").get_node("ColorFilter")
	Main_DecodeLevel.Main_Player = Main_Player
	Main_DecodeLevel.Main_UI = Main_UI
	Main_UI.Main_DecodeLevel = Main_DecodeLevel
	Main_DecodeLevel.Main_Filter = Main_Filter
	$UI.player = $Player
	#$CanvasUI.init()
