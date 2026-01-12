extends Node

var Main_DecodeLevel
var Main_Player
func _ready() -> void:
	Engine.max_fps = 60
	
	Main_DecodeLevel = get_node("DecodeLevel") 
	Main_Player = get_node("Player")
	Main_DecodeLevel.Main_Player = Main_Player
	$UI.player = $Player
	$CanvasUI.player = $Player
	$CanvasUI.init()
		
	if has_node("RunThisSceneLabel3D"):
		$RunThisSceneLabel3D.queue_free()

	#terrain = await create_terrain()
	#terrain = load_terrain("level1")
	
	Main_DecodeLevel.runGameStep0()
	var playerPosRot=Main_DecodeLevel.getPlayerPosRot()
	Main_Player.position=playerPosRot.position/256
