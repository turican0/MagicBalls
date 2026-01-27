extends Node3D

@export_file("*.tab") var levels_tab_path: String = ""
@export_file("*.dat") var levels_dat_path: String = ""

var game_data_path: String = ""
var cd_data_path: String = ""

#var speed: float = 0
var direction2: Vector3 = Vector3(1,0,0)

var Main_Player: Node
var Main_UI: Node
var Main_Filter: Node
var Main_Sounds: Node

const KEY_INDEX := {
	KEY_W: 0, # Forward
	KEY_S: 1,  # Backward
	KEY_A: 2, # Left
	KEY_D: 3 # Right
}

const MOUSE_BUTTON_INDEX := {
	MOUSE_BUTTON_LEFT: 0,
	MOUSE_BUTTON_RIGHT: 1,
	MOUSE_BUTTON_MIDDLE: 2,
	MOUSE_BUTTON_WHEEL_UP: 3,
	MOUSE_BUTTON_WHEEL_DOWN: 4
}

var input_state: Dictionary = {
	"keys": {},
	"mouse_pos": Vector2.ZERO
}

# Akumulovaný posun myši od startu
var total_mouse_delta := Vector2.ZERO

var node_pool = []
var pool_size = 1000
# Katalog cest k tscn souborům podle ID typu
var library_scenes = {}
var library2_scenes = {}
var library = {
	Vector3i(0,999,0): "res://entites/object_text.tscn",
	Vector3i(2,75,0): "res://entites/object_2_75_tree.tscn",#tree
	Vector3i(2,78,0): "res://entites/object_2_78_statue.tscn",#statue
	Vector3i(2,79,0): "res://entites/object_2_79_dolmen.tscn",#dolmen
	Vector3i(2,87,0): "res://entites/object_2_87_tree.tscn",#tree2 - doplnit
	Vector3i(3,0,0): "",#player1
	Vector3i(3,88,0): "res://entites/object_3_88_ballon.tscn",#ballon
	Vector3i(3,96,0): "res://entites/object_10_96_posses_building.tscn",#castle
	Vector3i(5,8,0): "res://entites/object_5_8_bowman.tscn",#bowman
	Vector3i(5,9,0): "res://entites/object_5_8_bowman.tscn",#bowman
	Vector3i(5,10,0): "res://entites/object_5_8_bowman.tscn",#bowman
	Vector3i(5,11,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	Vector3i(5,12,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	Vector3i(5,13,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	Vector3i(5,14,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	Vector3i(5,15,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	Vector3i(5,16,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	Vector3i(5,17,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	Vector3i(5,18,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	Vector3i(5,19,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	Vector3i(5,20,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	Vector3i(5,21,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	Vector3i(5,30,0): "res://entites/object_5_30_centipedeHead.tscn",#centipede-head
	Vector3i(5,66,0): "res://entites/object_5_66_centipedeBody.tscn",#centipede-body
	Vector3i(5,121,0): "res://entites/object_5_121_bowman.tscn",#bowman-crouch
	Vector3i(5,122,0): "res://entites/object_5_121_bowman.tscn",#bowman-crouch
	Vector3i(5,123,0): "res://entites/object_5_121_bowman.tscn",#bowman-crouch
	Vector3i(5,132,0): "res://entites/object_5_132_people5.tscn",#people5 132 133 134
	Vector3i(5,135,0): "res://entites/object_5_135_people6.tscn",#people6 135 136 137
	Vector3i(5,138,0): "res://entites/object_5_138_people7.tscn",#people7 138 139 140
	Vector3i(5,141,0): "res://entites/object_5_141_people8.tscn",#people8 141 142 143s
	Vector3i(5,152,0): "res://entites/object_5_152_goat.tscn",#goat
	Vector3i(5,155,0): "res://entites/object_5_155_people1.tscn",#people1 155 156 157
	Vector3i(5,180,0): "res://entites/object_5_180_people2.tscn",#people2 180 181 182
	Vector3i(5,183,0): "res://entites/object_5_183_people3.tscn",#people3 183 184 185
	Vector3i(5,199,0): "res://entites/object_5_199_people4.tscn",#people4 199 200 201
	Vector3i(5,279,0): "res://entites/object_5_279_beetle.tscn",#beetle
	Vector3i(9,55,0): "res://entites/object_9_55_fireball.tscn",#fireball
	Vector3i(9,61,0): "res://entites/object_9_61_castleball.tscn",#castleball
	Vector3i(9,105,0): "res://entites/object_9_105_arrow.tscn",#arrow
	Vector3i(9,144,0): "res://entites/object_9_144_posses.tscn",#posses
	Vector3i(9,463,0): "res://entites/object_9_463_sunball.tscn",#sunball
	Vector3i(10,38,0): "res://entites/object_10_8_fair.tscn",#end-of explosion
	Vector3i(10,54,0): "res://entites/object_10_54_explosion.tscn",#explosion
	Vector3i(10,57,0): "res://entites/object_10_57_smoke.tscn",#smoke1
	Vector3i(10,58,0): "res://entites/object_10_58_goldSphere.tscn",
	Vector3i(10,63,0): "res://entites/object_10_63_smoke.tscn",#smoke2
	Vector3i(10,67,0): "res://entites/object_10_67_whiteSphere.tscn",
	Vector3i(10,96,0): "res://entites/object_10_96_posses_building.tscn",#building
	Vector3i(10,186,0): "res://entites/object_10_186_splash.tscn",#splash
	Vector3i(14,259,0): "res://entites/object_14_259_scroll.tscn",#scroll
	Vector3i(14,461,0): "res://entites/object_14_461_mouth.tscn",#mouth-gate
	Vector3i(15,59,0): "res://entites/object_15_59_jar.tscn",#jar
}

var library2 = {
	Vector3i(0,999,0): "res://entites/object_text.tscn",
	Vector3i(3,0,0): "",#unknown
	Vector3i(0,8,0): "",#unknown
	Vector3i(0,58,0): "",#unknown
	Vector3i(0,67,0): "",#unknown
	Vector3i(10,8,0): "res://entites/object_10_8_fair.tscn",#fair
	#Vector3i(10,8,0): "res://entites/object_10_8_fair.tscn",#fair
	Vector3i(10,54,0): "",#fair
	#Vector3i(10,96,0): "res://entites/object_10_96_posses_building.tscn",#building
	Vector3i(10,96,0): "",#building
	Vector3i(11,8,0): "",#unknown
	Vector3i(14,259,0): "res://entites/object_14_259_scroll.tscn",#scroll
	Vector3i(14,461,0): "res://entites/object_14_461_mouth.tscn",#mouth-gate
	Vector3i(15,59,0): "",#unknown-jar?
	#
	#,
	#Vector3i(2,75,0): "res://entites/object_2_75_tree.tscn",#tree
	#Vector3i(2,79,0): "res://entites/object_2_79_dolmen.tscn",#dolmen
	#Vector3i(2,87,0): "res://entites/object_2_75_tree.tscn",#tree2 - doplnit
	#Vector3i(3,0,0): "",#player
	#Vector3i(3,88,0): "res://entites/object_3_88_ballon.tscn",#ballon
	#Vector3i(5,8,0): "res://entites/object_5_8_bowman.tscn",#bowman
	#Vector3i(5,9,0): "res://entites/object_5_8_bowman.tscn",#bowman
	#Vector3i(5,10,0): "res://entites/object_5_8_bowman.tscn",#bowman
	#Vector3i(5,11,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,12,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,13,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,14,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,15,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,16,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,17,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,18,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,19,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,20,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,21,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow	
	#Vector3i(5,121,0): "res://entites/object_5_121_bowman.tscn",#bowman-crouch
	#Vector3i(5,122,0): "res://entites/object_5_121_bowman.tscn",#bowman-crouch
	#Vector3i(5,123,0): "res://entites/object_5_121_bowman.tscn",#bowman-crouch
	#Vector3i(5,152,0): "res://entites/object_5_152_goat.tscn",#goat
	#Vector3i(5,155,0): "res://entites/object_5_155_people1.tscn",#people1 155 156 157
	#Vector3i(5,180,0): "res://entites/object_5_180_people2.tscn",#people2 180 181 182
	#Vector3i(5,183,0): "res://entites/object_5_183_people3.tscn",#people3 183 184 185
	#Vector3i(5,199,0): "res://entites/object_5_199_people4.tscn",#people4 199 200 201
	#Vector3i(5,279,0): "res://entites/object_5_279_beetle.tscn",#beetle
	#Vector3i(9,55,0): "res://entites/object_9_55_fireball.tscn",#fireball
	#Vector3i(9,105,0): "res://entites/object_9_105_arrow.tscn",#arrow
	#Vector3i(9,144,0): "res://entites/object_9_144_posses.tscn",#posses
	#Vector3i(10,8,0): "res://entites/object_10_8_fair.tscn",#fair
	#Vector3i(10,57,0): "res://entites/object_10_57_smoke.tscn",#smoke
	#Vector3i(10,58,0): "res://entites/object_10_58_goldSphere.tscn",
	#Vector3i(10,63,0): "res://entites/object_10_57_smoke.tscn",#smoke
	#Vector3i(10,67,0): "res://entites/object_10_67_whiteSphere.tscn",
	#Vector3i(10,96,0): "res://entites/object_10_96_posses_building.tscn"#building
}

var filter_material: ShaderMaterial
var data_img: Image
var data_tex: ImageTexture

func _preload_library(source_dict: Dictionary, target_dict: Dictionary):
	for key in source_dict:
		var path = source_dict[key]
		if path != "":
			target_dict[key] = load(path)

func _ready():	
	_preload_library(library, library_scenes)
	_preload_library(library2, library2_scenes)	
	node_pool.resize(pool_size)
	for i in range(pool_size):
		node_pool[i] = null

func _input(event):
	if event is InputEventMouseMotion:
		total_mouse_delta += event.relative

var last_spell_index:int =-1
var last_button:int =-1

func setPlayerActiveSpell(spell_index: int,button:int):
	last_spell_index = spell_index
	last_button = button
	Global.MBEX.setPlayerActiveSpell(spell_index,button)


func updatePlayer(playerPosRot) -> void:
	Main_Player.position=playerPosRot.position/256
	var yaw = PI*playerPosRot.rotation.yaw/(256*4)
	var pitch = PI*playerPosRot.rotation.pitch/(256*4)
	var roll = PI*playerPosRot.rotation.roll/(256*4)
	Main_Player.rotation=Vector3(-pitch, -yaw, -roll)
	
var last_gain: Vector3
var last_offset: Vector3

var runned: bool

func SetRunned(sendRunned) -> void:
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
	runned = sendRunned
	
func mapMenuInit():
	Global.MBEX.updateFreeSoundPlayers(Main_Sounds.get_free_player_indices())
	Global.MBEX.mapMenuInit()

func mapMenuStep(endMapMenu:int):
	Global.MBEX.updateFreeSoundPlayers(Main_Sounds.get_free_player_indices())	
	var mapMenuOut=Global.MBEX.mapMenuStep(endMapMenu)
	Main_Sounds.updateSounds(Global.MBEX.getPendingSoundActions())
	return mapMenuOut
	
func playAnim(index:int):
	Global.MBEX.updateFreeSoundPlayers(Main_Sounds.get_free_player_indices())
	Global.MBEX.playAnim(index)
	
func playAnimStep(endAnimIn:int) -> int:
	Global.MBEX.updateFreeSoundPlayers(Main_Sounds.get_free_player_indices())	
	var endAnimOut=Global.MBEX.playAnimStep(endAnimIn)	
	Main_Sounds.updateSounds(Global.MBEX.getPendingSoundActions())
	return endAnimOut

func _process(_p_delta) -> void:
	if(!runned):
		return
	getInputs()
	#MBEX.soundQueueClear()
	Global.MBEX.updateFreeSoundPlayers(Main_Sounds.get_free_player_indices())
	if(Main_UI.old_is_ctrl_active!=Main_UI.is_ctrl_active):
		if Main_UI.is_ctrl_active:
			Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
			Main_UI.saved_mouse_pos = get_viewport().get_mouse_position()
			var grid_rect = Main_UI.spell_grid.get_global_rect()
			var center_pos = grid_rect.position + (grid_rect.size / 2.0)
			center_pos.x=50
			get_viewport().warp_mouse(center_pos)
		else:
			get_viewport().warp_mouse(Main_UI.saved_mouse_pos)
			Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
		Main_UI.old_is_ctrl_active=Main_UI.is_ctrl_active
	if(last_spell_index!=-1):
		Global.MBEX.setPlayerActiveSpell(last_spell_index,last_button)
		last_spell_index = -1
		last_button = -1
	
	Global.MBEX.RunGameStep(input_state)
	Global.MBEX.renew_terrain()
	var mods = Global.MBEX.getPaletteModifications()
	var current_gain = mods[0]
	var current_offset = mods[1]
	var curreny_saturation = mods[2]
	if current_gain != last_gain or current_offset != last_offset:
		if(!filter_material):
			filter_material = Main_Filter.material as ShaderMaterial
		filter_material.set_shader_parameter("MyGain", current_gain)
		filter_material.set_shader_parameter("MyOffset", current_offset)
		filter_material.set_shader_parameter("MySatMultiplier", curreny_saturation)
		last_gain = current_gain
		last_offset = current_offset
	#var gain_vec = Vector3(gain_rgb.r, gain_rgb.g, gain_rgb.b)
	#var offset_vec = Vector3(offset_rgb.r, offset_rgb.g, offset_rgb.b)	
	#Main_Filter.material_override.set_shader_parameter("MyGain", gain_vec)
	#Main_Filter.material_override.set_shader_parameter("MyOffset", offset_vec)
	updatePlayer(getPlayerPosRot())
	renderEntites(getEntites())
	get_parent().get_node("UI").updateSpells(Global.MBEX.getActiveSpells())
	get_parent().get_node("UI").updateSelectedSpells(Global.MBEX.getSelectedSpells())
	get_parent().get_node("UI").updateMinimap(Global.MBEX.getMinimap())
	Main_Sounds.updateSounds(Global.MBEX.getPendingSoundActions())


func renderEntites(data_array: PackedFloat32Array) -> void:
	var stride = 31
	for i in range(pool_size):
		var offset = i * stride
		var pos = Vector3(data_array[offset], data_array[offset+2], data_array[offset+1])
		var rot = Vector3(data_array[offset+3], data_array[offset+4], data_array[offset+5])
		var actState = int(data_array[offset+6])
		var actClass = int(data_array[offset+7])
		var actModel = int(data_array[offset+8])
		var actXtype = int(data_array[offset+9])
		var actXsubtype = int(data_array[offset+10])
		var actWord2C = int(data_array[offset+11])
		var actSpeed = int(data_array[offset+12])
		var actByte38 = int(data_array[offset+13])
		var actByte39 = int(data_array[offset+14])
		var actByte3A = int(data_array[offset+15])
		var actId = int(data_array[offset+16])
		var actByte0 = int(data_array[offset+17])
		var actByte1 = int(data_array[offset+18])
		var actByte2 = int(data_array[offset+19])
		var actByte3 = int(data_array[offset+20])
		var modelIndex = int(data_array[offset+21])
		var rot2 = Vector3(data_array[offset+22], 0, 0)
		#if((actClass==10)&&(modelIndex==58)||(modelIndex==67)):
			#rot2 = Vector3(data_array[offset+22], data_array[offset+23], data_array[offset+24])
		var actMana = int(data_array[offset+24])
		var actLife = int(data_array[offset+25])
		var actMaxMana = int(data_array[offset+26])
		var actMaxLife = int(data_array[offset+27])
		var actOwnerObject = int(data_array[offset+28])
		var actBitmapScaleHelp = int(data_array[offset+29])
		var actBitmapScale = int(data_array[offset+30])
		
		if(modelIndex==0)&&(actClass==3):
			Main_Player.MOVE_SPEED=actSpeed
			Main_Player.LIFE=actLife
			Main_Player.MANA=actMana
		
		var current_node = node_pool[i]
		
		if current_node == null or current_node.get_meta("id") != modelIndex*1024*1024+actId*1024+actByte0:
			if current_node != null:
				current_node.queue_free()			
			if !(actByte1 & 4):
				var isDraw = true
				if(actByte0&1):
					isDraw = false
				var fromlib=false
				var scene_to_instance = null
				var default_key = Vector3i(0, 999, 0)
				if isDraw and actClass in [2, 3, 5, 9, 10, 15]:
					var key = Vector3i(actClass, modelIndex,0)
					if library_scenes.has(key):
						scene_to_instance = library_scenes[key]
						fromlib=true
					else:
						if !library.has(key):
							scene_to_instance = library_scenes.get(default_key)
				else:
					#if((actClass==3)&&(modelIndex==0))||((actClass==11)&&(modelIndex==8)):#player/spell
					var key = Vector3i(actClass, modelIndex,0)
					if library2_scenes.has(key):
						scene_to_instance = library2_scenes[key]
						fromlib=true
					else:
						if !library2.has(key):
							scene_to_instance = library2_scenes.get(default_key)
				if(actClass==9):
					actState+=0
				if scene_to_instance != null:
					var new_node = scene_to_instance.instantiate()
					if !fromlib:
						new_node.get_node("Label3D").text="M:" + str(modelIndex)+"_C:" +str(actClass)+"_M:" +str(actModel)+"_S:" +str(actState)+"_B0:"+str(actByte0)					
					#if((actClass==10)&&(modelIndex==57)||(modelIndex==63)):
						#var scale_scene_node = new_node.get_node("Sketchfab_Scene")
						#var s = randf_range(0.6, 1.8)
						#scale_scene_node.scale *= s
					add_child(new_node)
					new_node.set_meta("id", modelIndex*1024*1024+actId*1024+actByte0) # Uložíme ID pro budoucí kontrolu
					node_pool[i] = new_node
					current_node = new_node
		else:
			if (actByte1 & 4):
				current_node.queue_free()
		if current_node != null:
			if(actBitmapScaleHelp):
				var scale_scene_node = current_node.get_node_or_null("Scale")
				var s = actBitmapScale / 256.0
				if(scale_scene_node):
					scale_scene_node.scale = Vector3(s,s,s)
						
			var base_pos = pos / 256.0
			var camera = get_viewport().get_camera_3d()
			var entityScale=1
			if(actState==0x29)&&(actClass==0xA)&&(actModel==0x27):#manSphere
				entityScale=pow(actMana, 1.0 / 3.0)*0.2
			current_node.scale = Vector3(entityScale, entityScale, entityScale)
			if camera:
				var cam_pos = camera.global_position
				var grid_size = 256.0
				var half_grid = grid_size / 2.0
				var new_x = cam_pos.x + fposmod(base_pos.x - cam_pos.x + half_grid, grid_size) - half_grid
				var new_z = cam_pos.z + fposmod(base_pos.z - cam_pos.z + half_grid, grid_size) - half_grid
				current_node.global_position = Vector3(new_x, base_pos.y, new_z)
			else:
				current_node.position = base_pos
			var yaw = PI*rot2.x/(256*4)
			var pitch = PI*rot2.y/(256*4)
			var roll = PI*rot2.z/(256*4)
			current_node.rotation=Vector3(-pitch, -yaw, -roll)

var last_keys_state: Dictionary = {}
var last_mouse_buttons_state: Dictionary = {}
var mouse_640: Vector2
const SCREEN_WIDTH := 640
const SCREEN_HEIGHT := 480

func getInputs():
	var changes = []
	if(Main_UI.is_ctrl_active):
		return
	for keycode: int in KEY_INDEX:
		var index: int = KEY_INDEX[keycode]
		var is_pressed: bool = Input.is_key_pressed(keycode)
		var previous_state = last_keys_state.get(index, false)
		if is_pressed != previous_state:
			var status = "pressed" if is_pressed else "released"
			changes.append({
				"key_index": index,
				"action": status
			})
			last_keys_state[index] = is_pressed
	input_state["key_changes"] = changes
	
	var mouse_changes := []

	for button: int in MOUSE_BUTTON_INDEX:
		var index: int = MOUSE_BUTTON_INDEX[button]
		var is_pressed: bool = Input.is_mouse_button_pressed(button)
		var previous_state = last_mouse_buttons_state.get(index, false)
		if is_pressed != previous_state:
			var status = "pressed" if is_pressed else "released"
			mouse_changes.append({
				"button_index": index,
				"action": status
			})
			last_mouse_buttons_state[index] = is_pressed
	input_state["mouse_button_changes"] = mouse_changes

	mouse_640 = Vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2)
	mouse_640 += total_mouse_delta*0.2
	mouse_640.x = clamp(mouse_640.x, 0, SCREEN_WIDTH)
	mouse_640.y = clamp(mouse_640.y, 0, SCREEN_HEIGHT)		
	input_state["mouse_pos"] = mouse_640

func init():
	if(Global.terrainInited):
		return
	Global.terrainInited = true
	loadlevel(0)
	
func setMesh():
	Global.MBEX.set_mesh_instance(get_parent().get_node("TerrainMB").mesh_instance)
	Global.MBEX.initialize_grid_data()
	Global.MBEX.recalculate_mesh()

func loadlevel(levelnumber: int):
	sub_533B0_decompress_levels(levelnumber)

func getVGABuffer():
	return Global.MBEX.getVGABuffer()

func MBEXinit():
	if !Global.MBEX:
		Global.MBEX = ExampleClass.new()

func MBEXconvert(path, path2):
	Global.MBEX.convertOriginalData(path,path2)

func MBEXextractCD(path, path2):
	Global.MBEX.convertOriginalDataExtractCD(path,path2)

func sub_533B0_decompress_levels(level_id: int):
	if level_id >= 1000:
		return true
	var level_dat_file: FileAccess = null
	var level_tab_file: FileAccess = null
	level_dat_file = FileAccess.open(levels_dat_path, FileAccess.READ)

	if level_dat_file == null:
		levels_dat_path = ProjectSettings.get_setting("global_paths/cd_folder") + "/LEVELS/LEVELS.DAT"
		level_dat_file = FileAccess.open(levels_dat_path, FileAccess.READ)

	if level_dat_file == null:
		return false
	level_tab_file = FileAccess.open(levels_tab_path, FileAccess.READ)
	level_dat_file = FileAccess.open(levels_dat_path, FileAccess.READ)
	
	var level_tab_file_arr: Array[int] = []
	while not level_tab_file.eof_reached():
		level_tab_file_arr.append(level_tab_file.get_32())   # načte 4 byty jako int32
	level_tab_file.close()
	var level_start: int = level_tab_file_arr[level_id]
	var compressed_size: int = level_tab_file_arr[level_id+1]-level_tab_file_arr[level_id]
	
	level_dat_file.seek(level_start)
	var level_tab_data: PackedByteArray = level_dat_file.get_buffer(compressed_size)
	level_dat_file.close()
	
	MBEXinit()
	var level_tab_data_unpacked:PackedByteArray = Global.MBEX.deRNC(level_tab_data)
	Global.MBEX.TerrainMake(level_tab_data_unpacked)
	
	#if not DirAccess.dir_exists_absolute("res://convertdata/musics/"):
		#MBEXconvert("res://convertdata",null)
		
func initSound():
	if(Global.soundInited):
		return
	Global.soundInited = true
	Main_Sounds.load_sounds_from_dir("res://convertdata/sounds/")
	Main_Sounds.load_musics_from_dir("res://convertdata/musics/")
	Main_Sounds.load_musics_hi_from_dir("res://hidata/musics/")
	
	Main_Sounds.init()
	Main_Sounds.setSoundBank(1)#Night

func initTerrainObsolette():
	mapHeightmap_11B4E0 = Global.MBEX.TerrainGetMapHeight()
	mapTerrainType_10B4E0 = Global.MBEX.TerrainGetMapTerrainType()
	mapAngle_13B4E0 = Global.MBEX.TerrainGetAngle()
	

func getPlayerPosRot() -> Dictionary:
	var playerPosRot: Dictionary = Global.MBEX.GetPlayerPositionRotation()
	return playerPosRot
	
func getEntites() -> Array:
	var result: Array = Global.MBEX.GetEntites()
	return result
	
func getTerrainChanges() -> Dictionary:
	var result: Dictionary = Global.MBEX.GetTerrainChanges()
	return result

var mapTerrainType_10B4E0: PackedByteArray = PackedByteArray()
var mapHeightmap_11B4E0: PackedByteArray = PackedByteArray()
var mapShading_12B4E0: PackedByteArray = PackedByteArray()
var mapAngle_13B4E0: PackedByteArray = PackedByteArray()
var mapEntityIndex_15B4E0: PackedInt32Array = PackedInt32Array()

func generate_level_map_43830() -> void:
	mapTerrainType_10B4E0.resize(256 * 256)
	mapHeightmap_11B4E0.resize(256 * 256)
	mapShading_12B4E0.resize(256 * 256)
	mapAngle_13B4E0.resize(256 * 256)
	mapEntityIndex_15B4E0.resize(256 * 256)
	mapTerrainType_10B4E0.fill(0)
	mapHeightmap_11B4E0.fill(0)
	mapShading_12B4E0.fill(0)
	mapAngle_13B4E0.fill(0)
	mapEntityIndex_15B4E0.fill(0)
