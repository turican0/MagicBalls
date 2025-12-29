extends Node

@export_file("*.tab") var levels_tab_path: String = ""
@export_file("*.dat") var levels_dat_path: String = ""

var MBEX

# Cesty (Godot standard)
var game_data_path: String = ""
var cd_data_path: String = ""

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

func runGameStep():
	MBEX.RunGameStep()

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
