extends Node

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

var input_state: Dictionary = {
	"keys": {},
	"mouse_pos": Vector2.ZERO
}

# Akumulovaný posun myši od startu
var total_mouse_delta := Vector2.ZERO

func _ready():
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)

func _input(event):
	if event is InputEventMouseMotion:
		total_mouse_delta += event.relative

func _physics_process(p_delta) -> void:
	getInputs()
	runGameStep(input_state)
	var playerPosRot=getPlayerPosRot()
	Main_Player.position=playerPosRot.position/256
	var yaw = PI*playerPosRot.rotation.yaw/(256*4)   # Rotace kolem osy Y
	var pitch = PI*playerPosRot.rotation.pitch/(256*4) # Rotace kolem osy X
	var roll = PI*playerPosRot.rotation.roll/(256*4)  # Rotace kolem osy Z
	Main_Player.rotation=Vector3(-pitch, -yaw, -roll)

var last_keys_state: Dictionary = {}
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
	#var mouse := get_viewport().get_mouse_position()
	#var vp_size := get_viewport().get_visible_rect().size
	#var mouse_640 := Vector2(
		#mouse.x / vp_size.x * 640.0,
		#mouse.y / vp_size.y * 480.0
	#)
	mouse_640 = Vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2)
	mouse_640 += total_mouse_delta*0.2

	# Omez na velikost obrazovky
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
	return true
	
func getPlayerPosRot() -> Dictionary:
	var playerPosRot: Dictionary = MBEX.GetPlayerPositionRotation()
	return playerPosRot
	
func runGameStep0():
	runGameStep(input_state)

func runGameStep(inputs:Dictionary):
	MBEX.RunGameStep(input_state)

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
