extends Node3D

var Main_DecodeLevel
var MBEngine
var Main_TerrainsMB
var Main_Camera

func _ready() -> void:
	await get_tree().process_frame
	Main_Camera = get_node("Camera3D")
	Main_DecodeLevel = get_node("DecodeLevel")
	MBEngine = get_node("MBEngine")
	Main_TerrainsMB = $TerrainsMB
	Main_DecodeLevel.NodeSky3D = $NodeSky3D/Sky3D
	Global.setLevelType("Day")
	gameInit()
	

func gameInit():
	match Global.getLevelType():
		"Day":
			Main_DecodeLevel.setTime(9.0)
			Main_DecodeLevel.setDayEntites()
			Main_DecodeLevel.setSkyExposure(0.3)
			Main_DecodeLevel.sefFogEnd(200)
			Main_DecodeLevel.setFogFall(15)
			Main_DecodeLevel.setFogDensity(0.01)
			Main_DecodeLevel.setAtmDayTint(Color(0.8,0.9,1.0))
			Main_DecodeLevel.setSunMoon(true,true,0.7,1.0)
		"Night":
			Main_DecodeLevel.setTime(2.5)
			Main_DecodeLevel.setNightEntites()
			Main_DecodeLevel.setSkyExposure(1.5)
			Main_DecodeLevel.sefFogEnd(200)
			Main_DecodeLevel.setFogFall(15)
			Main_DecodeLevel.setFogDensity(0.01)
			Main_DecodeLevel.setAtmDayTint(Color(0.8,0.9,1.0))
			Main_DecodeLevel.setSunMoon(true,true,1.0,1.0)
		"Cave":
			Main_DecodeLevel.setTime(0.0)
			Main_DecodeLevel.setFog(0.03)
			Main_DecodeLevel.setFogSky(1.0)
			Main_DecodeLevel.setSkyExposure(0.3)
			Main_DecodeLevel.setCaveEntites()
			Main_DecodeLevel.setAtmDayTint(Color(0,0,0))
			Main_DecodeLevel.setSunMoon(false,true,1.0,0.0)
		"Final":
			Main_DecodeLevel.setTime(2.5)
			Main_DecodeLevel.setNightEntites()
			Main_DecodeLevel.setSkyExposure(1.5)
			Main_DecodeLevel.sefFogEnd(200)
			Main_DecodeLevel.setFogFall(15)
			Main_DecodeLevel.setFogDensity(0.01)
			Main_DecodeLevel.setAtmDayTint(Color(0.8,0.9,1.0))
			Main_DecodeLevel.setSunMoon(true,true,1.0,1.0)
