extends Node

var Main_DecodeLevel
var Main_Player
func _ready() -> void:
	Engine.max_fps = 60	
	Main_DecodeLevel = get_node("DecodeLevel") 
	Main_Player = get_node("Player")
	Main_DecodeLevel.Main_Player = Main_Player
	$UI.player = $Player
	#$CanvasUI.init()
