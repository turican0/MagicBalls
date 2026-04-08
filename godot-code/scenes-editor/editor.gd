extends Node3D

@onready var Main_DecodeLevel: Node3D = $DecodeLevel
@onready var MBEngine: Node3D = $MBEngine
@onready var Main_TerrainsMB: Node3D = $TerrainsMB
@onready var Main_Player: Node3D = $PlayerEditor
@onready var Main_Camera: Camera3D = $PlayerEditor/Camera3D
@onready var Ray_Cylinder: MeshInstance3D = $RayCylinder

@onready var Terrain_Edit_Panel: CanvasLayer = $TerrainEdit

var editor_runned=false
var is_ui_visible = false

func _ready() -> void:
	await get_tree().process_frame
	Global.MBEX = MBEXclass.new()
	Main_DecodeLevel.NodeSky3D = $NodeSky3D/Sky3D
	Main_Camera.Ray_Cylinder = Ray_Cylinder
	Main_Camera.terrain_node = Main_TerrainsMB
	var Ray_Cylinder: MeshInstance3D = null
	Global.setLevelType("Night")
	Global.MBEX.REMC2SetLevelType("Night")
	Main_TerrainsMB.init(false)
	Main_DecodeLevel.gameInit(false)
	gameInit()
	EditorInit(Global.cdPath)
	Input.set_mouse_mode(Input.MOUSE_MODE_CONFINED_HIDDEN)
	#Main_TerrainsMB.updateMeshes(false)
	editor_runned=true
	
func _input(event: InputEvent) -> void:
	# M pressed
	if (event is InputEventKey and event.keycode == KEY_M and event.pressed):
		toggle_terrain_editor()

func toggle_terrain_editor():
	is_ui_visible = !is_ui_visible
	if is_ui_visible:
		Terrain_Edit_Panel.show()
		Input.set_mouse_mode(Input.MOUSE_MODE_CONFINED)
	else:
		Terrain_Edit_Panel.hide()
		Input.set_mouse_mode(Input.MOUSE_MODE_CONFINED_HIDDEN)
		var center = get_viewport().get_visible_rect().size / 2.0
		get_viewport().warp_mouse(center)

func _process(delta: float) -> void:
	if editor_runned:
		EditorStep()

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

func EditorInit(cdPath):
	Global.MBEX.REMC2EditorBegin(cdPath)

func EditorStep():
	Global.MBEX.REMC2EditorLoop()
	var isCave:bool = false
	Global.MBEX.renew_terrain(isCave)

func EditorEnd():
	Global.MBEX.REMC2EditorEnd()

func _on_terrain_type_state_changed_graphics_type(state_name: String) -> void:
	if(!Main_TerrainsMB):
		return
	Global.setLevelType(state_name)
	Global.MBEX.REMC2SetLevelType(state_name)
	Main_DecodeLevel.gameInit(false)
	Main_TerrainsMB.updateMeshes(false)
	
