extends Node3D

@export_file("*.tab") var levels_tab_path: String = ""
@export_file("*.dat") var levels_dat_path: String = ""

var MBEX

# Cesty (Godot standard)
var game_data_path: String = ""
var cd_data_path: String = ""

#var speed: float = 0
var direction2: Vector3 = Vector3(1,0,0)

var Main_Player: Node

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
var library = {
	#0: "res://components/gold_sphere.tscn",
	#1: "res://components/gold_sphere.tscn",
	#2: "res://components/gold_sphere.tscn",
	Vector3i(10,8,0): "res://entites/object_8_10b.tscn",#fair
	Vector3i(9,144,0): "res://entites/object_54_9b.tscn",#select
	Vector3i(9,55,0): "res://entites/object_55_9b.tscn",#fairball
	#38: "res://entites/object_38b.tscn",
	#54: "res://entites/object_38b.tscn",
	Vector3i(10,57,0): "res://entites/object_63b.tscn",#smoke
	Vector3i(10,58,0): "res://components/gold_sphere.tscn",
	Vector3i(10,67,0): "res://components/white_sphere.tscn",
	Vector3i(0,59,0): "res://entites/object_59b.tscn",
	Vector3i(10,63,0): "res://entites/object_63b.tscn",#smoke
	Vector3i(2,75,0): "res://entites/object_75b.tscn",#tree
	Vector3i(2,79,0): "res://entites/object_79b.tscn",#dolmen
	Vector3i(2,87,0): "res://entites/object_75b.tscn",#tree
	Vector3i(10,96,0): "res://entites/object_96b.tscn",#building
	Vector3i(0,121,0): "res://entites/object_8b.tscn",#bowman
	Vector3i(5,152,0): "res://entites/object_152b.tscn",#goat
	#153: "res://entites/object_152b.tscn",
	Vector3i(5,155,0): "res://entites/object_155b.tscn",#people
	Vector3i(5,180,0): "res://entites/object_155b.tscn",#people
	Vector3i(5,183,0): "res://entites/object_155b.tscn",#people
	#186: "res://entites/object_8b.tscn",
	Vector3i(5,199,0): "res://entites/object_155b.tscn",#people
	Vector3i(0,999,0): "res://entites/object_text.tscn"
}

func _ready():
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
	
	# Inicializujeme pole s nulami (prázdná místa)
	node_pool.resize(pool_size)
	for i in range(pool_size):
		node_pool[i] = null

func _input(event):
	if event is InputEventMouseMotion:
		total_mouse_delta += event.relative

func _physics_process(p_delta) -> void:
	getInputs()
	runGameStep(input_state)
	var playerPosRot=getPlayerPosRot()
	var newEntites: PackedFloat32Array=getEntites()
	Main_Player.position=playerPosRot.position/256
	var yaw = PI*playerPosRot.rotation.yaw/(256*4)   # Rotace kolem osy Y
	var pitch = PI*playerPosRot.rotation.pitch/(256*4) # Rotace kolem osy X
	var roll = PI*playerPosRot.rotation.roll/(256*4)  # Rotace kolem osy Z
	Main_Player.rotation=Vector3(-pitch, -yaw, -roll)
	renderEntites(newEntites)
	#get_parent().get_node("Node3D-testTerrain").renew_terrain()
	#var terrainChanges=getTerrainChanges()

func renderEntites(data_array: PackedFloat32Array) -> void:
	var stride = 29
	var count = data_array.size() / stride
	for i in range(pool_size):
		var offset = i * stride
		var pos = Vector3(data_array[offset], data_array[offset+2], data_array[offset+1])
		var rot = Vector3(data_array[offset+3], data_array[offset+4], data_array[offset+5])
		
		#write_ptr[idx++] = (float)actEntity->state_0x45_69; //7 = 0x29;
		#write_ptr[idx++] = (float)actEntity->class_0x3F_63; //8 = 0xA;
		#write_ptr[idx++] = (float)actEntity->model_0x40_64; //9 = 0x27;
		#write_ptr[idx++] = (float)actEntity->xtype_0x41_65; //10 = 10;
		#write_ptr[idx++] = (float)actEntity->xsubtype_0x42_66; //11 = 39;
		#write_ptr[idx++] = (float)actEntity->word_0x2C_44; //12 = 128;
		#write_ptr[idx++] = (float)actEntity->actSpeed_0x82_130; //13 = 32;
		#write_ptr[idx++] = (float)actEntity->byte_0x38_56; //14 = 3;
		#write_ptr[idx++] = (float)actEntity->byte_0x39_57; //15 = 128;
		#write_ptr[idx++] = (float)actEntity->byte_0x3A_58; //16 = 0;
		
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
		var actMana = int(data_array[offset+24])
		var actLife = int(data_array[offset+25])
		var actMaxMana = int(data_array[offset+26])
		var actMaxLife = int(data_array[offset+27])
		var actOwnerObject = int(data_array[offset+28])
		
		if(modelIndex==0)&&(actClass==3):
			Main_Player.MOVE_SPEED=actSpeed
			Main_Player.LIFE=actLife
			Main_Player.MANA=actMana
		
		var current_node = node_pool[i]
		
		#if(modelIndex==67):
			#actModel+=1
			#actModel-=1
			#if!(actByte0&1):
				#if current_node != null:
					#current_node.queue_free()
			#if(actOwnerObject!=0):
				#current_node.queue_free()
		
		if current_node == null or current_node.get_meta("id") != modelIndex*1024*1024+actId*1024+actByte0:
			if current_node != null:
				current_node.queue_free()
			#if (1):
			#if (actClass && actByte1 & 4):
			#if (!(v3x->struct_byte_0xc_12_15.byte[0] & 0x21))
			var fromlib=false
			if !(actByte1 & 4):
				var isDraw = true
				if(actByte0&1):
					isDraw = false
				var tempModel
				if(isDraw)&&((actClass==2)||(actClass==5)||(actClass==9)||(actClass==10)||(actClass==15)):
					var key = Vector3i(actClass, modelIndex,0)
					if library.has(key):
						tempModel=library[key]
						fromlib=true
					else:
						tempModel=library[Vector3i(0, 999, 0)]
				else:
					tempModel=library[Vector3i(0, 999, 0)]
				var new_node = load(tempModel).instantiate()
				
				if !fromlib:
					new_node.get_node("Label3D").text="M:" + str(modelIndex)+"_C:" +str(actClass)+"_M:" +str(actModel)+"_S:" +str(actState)+"_B0:"+str(actByte0)
					
				add_child(new_node)
				new_node.set_meta("id", modelIndex*1024*1024+actId*1024+actByte0) # Uložíme ID pro budoucí kontrolu
				node_pool[i] = new_node
				current_node = new_node
		else:
			if (actByte1 & 4):
				current_node.queue_free()
		if current_node != null:
			var base_pos = pos / 256.0
			var camera = get_viewport().get_camera_3d()
			var scale=1
			if(actState==0x29)&&(actClass==0xA)&&(actModel==0x27):#manSphere
				scale=pow(actMana, 1.0 / 3.0)*0.2
			current_node.scale = Vector3(scale, scale, scale)
			if camera:
				var cam_pos = camera.global_position
				var grid_size = 256.0
				var half_grid = grid_size / 2.0
				var new_x = cam_pos.x + fposmod(base_pos.x - cam_pos.x + half_grid, grid_size) - half_grid
				var new_z = cam_pos.z + fposmod(base_pos.z - cam_pos.z + half_grid, grid_size) - half_grid
				current_node.global_position = Vector3(new_x, base_pos.y, new_z)
			else:
				current_node.position = base_pos
			var yaw = PI*rot2.x/(256*4)   # Rotace kolem osy Y
			var pitch = PI*rot2.y/(256*4) # Rotace kolem osy X
			var roll = PI*rot2.z/(256*4)  # Rotace kolem osy Z
			current_node.rotation=Vector3(-pitch, -yaw, -roll)
			
			
			#current_node.position=pos/256
			#var yaw = PI*rot2.x/(256*4)   # Rotace kolem osy Y
			#var pitch = PI*rot2.y/(256*4) # Rotace kolem osy X
			#var roll = PI*rot2.z/(256*4)  # Rotace kolem osy Z
			#current_node.rotation=Vector3(-pitch, -yaw, -roll)

var last_keys_state: Dictionary = {}
var last_mouse_buttons_state: Dictionary = {}
var mouse_640: Vector2
const SCREEN_WIDTH := 640
const SCREEN_HEIGHT := 480
	
func getInputs():
	var changes = []	
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

# Funkce, která by v Godotu volala ekvivalenty C-funkcí
func init():
	print("--- Start Godot Initialization Adaptace ---")
	loadlevel(0)
	
	print("--- Inicializace dokončena. ---")

func loadlevel(levelnumber: int):
	sub_533B0_decompress_levels(levelnumber)
func sub_533B0_decompress_levels(level_id: int) -> bool:
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
	
	MBEX = ExampleClass.new()
	var level_tab_data_unpacked:PackedByteArray = MBEX.deRNC(level_tab_data)
	MBEX.TerrainMake(level_tab_data_unpacked)
	mapHeightmap_11B4E0 = MBEX.TerrainGetMapHeight()
	mapTerrainType_10B4E0 = MBEX.TerrainGetMapTerrainType()
	mapAngle_13B4E0 = MBEX.TerrainGetAngle()
	
	#MBEX.set_mesh_instance(get_parent().get_node("Node3D-testTerrain").mesh_instance)
	#MBEX.initialize_grid_data()
	#MBEX.recalculate_mesh()
	
	return true
	
func getPlayerPosRot() -> Dictionary:
	var playerPosRot: Dictionary = MBEX.GetPlayerPositionRotation()
	return playerPosRot
	
func getEntites() -> Array:
	var result: Array = MBEX.GetEntites()
	return result
	
func getTerrainChanges() -> Dictionary:
	var result: Dictionary = MBEX.GetTerrainChanges()
	return result

func runGameStep0():
	runGameStep(input_state)

func runGameStep(inputs:Dictionary):
	MBEX.RunGameStep(input_state)
	#get_parent().get_node("Node3D-testTerrain").renew_terrain()
	#MBEX.initialize_grid_data()
	#MBEX.recalculate_mesh()

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
