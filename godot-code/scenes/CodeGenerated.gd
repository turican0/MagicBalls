extends Node3D

var fadeNode: Node3D

var Main_DecodeLevel
var Main_Player
var Main_UI
var Main_Filter
var MBEngine
#var Main_Sounds
var Main_TerrainsMB
#var MainMusic
func _ready() -> void:
	await get_tree().process_frame
	Engine.max_fps = 60
	Main_DecodeLevel = get_node("DecodeLevel")
	MBEngine = get_node("MBEngine")
	Main_Player = get_node("Player")
	Main_UI = get_node("UILayer/UI")
	Main_Filter = get_node("Filter").get_node("ColorRect")
	Main_TerrainsMB = $TerrainsMB
	Main_DecodeLevel.Main_Player = Main_Player
	Main_DecodeLevel.Main_UI = Main_UI
	Main_UI.Main_DecodeLevel = Main_DecodeLevel
	Main_DecodeLevel.Main_Filter = Main_Filter
	Main_DecodeLevel.NodeSky3D = $NodeSky3D/Sky3D
	$UILayer/UI.player = $Player
	#Main_TerrainsMB.init()
	MBEngine.Main_DecodeLevel=Main_DecodeLevel
	MBEngine.init()
	
	#$CanvasUI.init()
