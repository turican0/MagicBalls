extends Node

var Main_DecodeLevel
var Main_Player
var Main_UI
var Main_Filter
var Main_Sounds
var Main_TerrainMB
var MainMusic
func _ready() -> void:
	Engine.max_fps = 60
	Main_DecodeLevel = get_node("DecodeLevel")
	Main_Player = get_node("Player")
	Main_UI = get_node("UI")
	Main_Sounds = $Sounds
	Main_Sounds.MainMusic = get_node("Sounds").get_node("MidiPlayer")
	Main_Sounds.MainMusicHi = get_node("Sounds").get_node("AudioStreamPlayer")
	Main_Filter = get_node("Filter").get_node("ColorRect")
	Main_TerrainMB = $TerrainMB
	Main_DecodeLevel.Main_Player = Main_Player
	Main_DecodeLevel.Main_UI = Main_UI
	Main_UI.Main_DecodeLevel = Main_DecodeLevel
	Main_DecodeLevel.Main_Filter = Main_Filter
	Main_DecodeLevel.Main_Sounds = Main_Sounds
	$UI.player = $Player
	Main_TerrainMB.init()
	#$CanvasUI.init()
