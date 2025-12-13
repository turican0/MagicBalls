extends Node

@export_file("*.tab") var levels_tab_path: String = ""
@export_file("*.dat") var levels_dat_path: String = ""

#class_name TMapDefinition extends RefCounted
# Globální proměnná pro pole definic (MUSÍ být zde)
var tmap_definitions: Array[TMapDefinition] = []
# --- 1. Proměnné a Konstanty (Adaptace C/C++ globálních/statických dat) ---

# Proměnná pro paletu (nahrazuje malloc)
# V Godotu obvykle používáme ImageTexture nebo PoolByteArray pro data palety
var xadataclrd0dat_color_palette: PackedByteArray = PackedByteArray()

# Indexy pro datové tabulky
const FILEARRAYINDEX_POINTERSDATTAB: int = 0
const FILEARRAYINDEX_BUILD00DATTAB: int = 1

# Nastavení terénu (Adaptace struktury D41A0_0.terrain_2FECE)
# Použijeme slovník pro přehlednost
var terrain_settings: Dictionary = {
	"seed": 0,
	"offset": 0,
	"raise": 0,
	"gnarl": 0,
	"source": 0,
	"snLin": 0,
	"bhFlt": 0,
	"rkSte": 0
}

var x_vga_type_resolution: int = 1
var x_word_type_resolution: int = 1

# Cesty (Godot standard)
var game_data_path: String = ""
var cd_data_path: String = ""

var decode_buffer: PackedByteArray = PackedByteArray()


# --- 2. Adaptace Inicializačních funkcí ---

# Funkce, která by v Godotu volala ekvivalenty C-funkcí
func init():
	print("--- Start Godot Initialization Adaptace ---")

	# 1. Alokace paměti pro paletu (Původně malloc)
	# V Godotu alokujeme PoolByteArray o požadované velikosti.
	# Původní kód: *xadataclrd0dat.colorPalette_var28 = (uint8_t*)malloc(4096);
	xadataclrd0dat_color_palette.resize(4096)
	print("Paleta inicializována (4096 bytů).")

	# Původní kód: initposistruct();
	_init_position_structure()

	# 3. Inicializace subsystémů

	# Původní kód: sub_5BCC0_set_any_variables1();
	_set_any_variables_1()

	# Původní kód: if (!sub_5BF50_load_psxdata()) exit(-1);
	if not _load_psx_data():
		push_error("CHYBA: Selhalo načtení PSX dat!")
		get_tree().quit() # Ukončení aplikace
		return

	# Původní kód: sub_5C1B0_set_any_variables2();
	_set_any_variables_2()
	
	# Původní kód: sub_71410_process_tmaps();
	_process_texture_maps()

	# 4. Tvorba indexů pro PAK soubory

	# Původní kód: CreateIndexes_6EB90(...)
	_create_indexes(FILEARRAYINDEX_POINTERSDATTAB)
	_create_indexes(FILEARRAYINDEX_BUILD00DATTAB)
	
	# Původní kód: sub_101C0();
	# Pokud se kód do této části nedostane, jak naznačuje C komentář,
	# můžeme jej zakomentovat nebo nechat jako placeholder.
	# _sub_101c0_placeholder()

	# Původní kód: sub_6EBF0(...)
	_sub_6ebf0_placeholder(FILEARRAYINDEX_POINTERSDATTAB)

	# 5. Nastavení rozlišení
	x_vga_type_resolution = 1
	x_word_type_resolution = 1
	print("Nastaveno rozlišení (Type 1).")

	# 6. Nastavení parametrů terénu
	terrain_settings.seed = 0
	terrain_settings.offset = 0
	terrain_settings.raise = 0
	terrain_settings.gnarl = 0
	terrain_settings.source = 0
	terrain_settings.snLin = 0
	terrain_settings.bhFlt = 0
	terrain_settings.rkSte = 0 # Dvojité nastavení je zachováno
	print("Inicializována nastavení terénu.")

	# 7. Načtení a zpracování terénu

	# Původní kód: clean_tarrain();
	_clean_terrain()
	
	# Původní kód: loadlevel(0);
	loadlevel(0)
	
	# Původní kód: terrain_recalculate();
	_recalculate_terrain()
	
	print("--- Inicializace dokončena. ---")


# --- Placeholdery pro původní C/C++ funkce ---
# Tyto funkce musíte postupně implementovat v Godotu/GDScriptu
# nebo jako GDExtension, které provede původní logiku.

func _init_position_structure():
	# Inicializace herních pozic, kamer, atd.
	print("- initposistruct() voláno.")

func _set_any_variables_1():
	# Placeholder pro sub_5BCC0
	print("- _set_any_variables_1 voláno.")

func _load_psx_data() -> bool:
	# Zde musí proběhnout vlastní načítání souborů
	# Např. načtení palet a datových souborů pomocí File
	print("- _load_psx_data: Načítám PSX data...")
	# TBD: Vraťte true při úspěchu, false při selhání
	return true 

func _set_any_variables_2():
	# Placeholder pro sub_5C1B0
	print("- _set_any_variables_2 voláno.")

func _process_texture_maps():
	# Zpracování textur a příprava VRAM/Textur pro Godot
	# Zde by se C-textury převáděly na Godot Image nebo Texture2D
	print("- _process_texture_maps: Zpracování T-map.")

func _create_indexes(file_index: int):
	# Logika pro čtení hlaviček a vytvoření indexů v PAK souborech
	print("- _create_indexes: Vytvářím indexy pro tabulku: " + str(file_index))

func _sub_6ebf0_placeholder(file_index: int):
	# Další zpracování datových indexů
	print("- _sub_6ebf0_placeholder voláno.")

const MAX_ENTITIES: int = 1200 # 0x4b0
var temp_entities_inactive: PackedByteArray = PackedByteArray() # Efektivnější pro pole bytů/booleanů
var temp_entities_selected: PackedByteArray = PackedByteArray() # Efektivnější pro pole bytů/booleanů

func _clean_terrain():
	# Vyčištění datových struktur pro terén (geometrie, výška)
	print("- _clean_terrain voláno.")

func loadlevel(levelnumber: int):
	var current_level_data: TypeStr2FECE
	if current_level_data == null:
		current_level_data = TypeStr2FECE.new()
	sub_533B0_decompress_levels(levelnumber, current_level_data)

	#if current_level_data.entities.size() >= MAX_ENTITIES:
	#	temp_entities = current_level_data.entities.slice(0, MAX_ENTITIES)
	#else:
	#	printerr("Warning: Not enough entities in level data.")
	#	temp_entities = current_level_data.entities.duplicate() # Zkopíruje i méně entit
	temp_entities_inactive.resize(MAX_ENTITIES)
	temp_entities_inactive.fill(0) # Vyplní celou PackedByteArray nulami
	temp_entities_selected.resize(MAX_ENTITIES)
	temp_entities_selected.fill(0) # Vyplní celou PackedByteArray nulami

func _recalculate_terrain():
	# Vlastní generování terénu v Godotu (použití NoiseTexture nebo ArrayMesh)
	print("- _recalculate_terrain: Přepočítávám terén s nastavením: " + str(terrain_settings))

func process_tmaps():
	print("--- process_tmaps: Načítání a příprava textur ---")

	# Adaptace: sub_70A60_open_tmaps()
	if not _open_tmaps_file():
		push_error("Nelze otevřít soubor T-Maps.")
		return

	# Adaptace: procházení str_WORD_D951C
	for i in range(tmap_definitions.size()):
		var tmap_def: TMapDefinition = tmap_definitions[i]

		# Původní podmínka: Zpracovávat, jen pokud je definována rychlost/rotace
		if tmap_def.speed == 0 and tmap_def.rot_speed == 0:
			continue

		var v2_tmap_index: int = tmap_def.index
		
		# Adaptace: Zjištění délky textury (z TMAPS00TAB_BEGIN_BUFFER)
		# Předpokládáme existenci globálního pole tmap_size_table
		var start_offset = _get_tmap_start_offset(v2_tmap_index)
		var end_offset = _get_tmap_start_offset(v2_tmap_index + 1)
		var tmap_length = end_offset - start_offset

		# Adaptace: memset((void*)v1, 0, length)
		decode_buffer.resize(tmap_length) # Alokace a vynulování paměti
		
		# Adaptace: sub_70C60_decompress_tmap(v2, v1)
		var decode_result = _decompress_tmap_data(v2_tmap_index, decode_buffer) # Zde je potřeba C++ kód

		# Dekódování metadat textury z bufferu
		var width_from_data: int = 255 # Default
		var height_from_data: int = 255 # Default
		var type_flag_from_data: int = 1 # Default

		if decode_result != -1:
			# Z bufferu (v1) se čtou metadata:
			# Offset 1: type/flag (uint8)
			# Offset 2: width (uint16)
			# Offset 4: height (uint16)
			
			# Tato nízkoúrovňová čtení jsou nejlepší v GDExtension (C++)
			# Pro GDScript by bylo nutné použít FileAccess.get_buffer/get_16/get_8 po přesunu na offset.
			type_flag_from_data = decode_buffer[1]
			# Upozornění: Museli byste implementovat čtení LITTLE-ENDIAN uint16 z indexů 2 a 4.
			# Předpokládáme, že funkce read_uint16_at_offset by to zajistila.
			width_from_data = _read_uint16_at_offset(decode_buffer, 2)
			height_from_data = _read_uint16_at_offset(decode_buffer, 4)
		
		# Původní kontrola rozměrů a chyb
		var needs_default_size = (decode_result == -1) or (width_from_data == 0) or (height_from_data == 0)

		if needs_default_size:
			width_from_data = 255
			height_from_data = 255
			type_flag_from_data = 1

		# 1. Normalizace rychlostí
		tmap_def.width = width_from_data
		tmap_def.height = height_from_data

		if tmap_def.speed != 0:
			if tmap_def.rot_speed == 0:
				# rotSpeed = height * speed / width
				if tmap_def.width != 0:
					tmap_def.rot_speed = tmap_def.height * tmap_def.speed / tmap_def.width
		else: # speed == 0
			# speed = width * rotSpeed / height
			if tmap_def.height != 0:
				tmap_def.speed = tmap_def.width * tmap_def.rot_speed / tmap_def.height

		# 2. Uložení příznaku
		tmap_def.type_flag = type_flag_from_data

		# Důležité: Tady by měla následovat konverze PackedByteArray na Godot Image / ImageTexture
		tmap_def.texture = _convert_buffer_to_texture(decode_buffer, width_from_data, height_from_data)

	# Adaptace: sub_70BF0_close_tmaps()
	_close_tmaps_file()
	print("--- process_tmaps dokončeno. ---")

# --- Nízkoúrovňové GDScript/GDExtension Placeholdery ---

# Tuto funkci je ideální implementovat jako C++/GDExtension modul
# kde lze použít původní dekompresní algoritmus.
func _decompress_tmap_data(index: int, buffer: PackedByteArray) -> int:
	# return -1 při chybě, 0 při úspěchu
	return 0 # Placeholder pro úspěch
	
func _open_tmaps_file() -> bool:
	# Zde bude implementace pro otevření souboru
	print("- Otevírám soubor T-Maps.")
	# Příklad použití Godot FileAccess:
	# tmaps_file = FileAccess.open("res://data/cd/TMAPS00TAB.DAT", FileAccess.READ)
	# return tmaps_file != null
	return true # Prozatímní placeholder
	
func _get_tmap_start_offset(index: int) -> int:
	# Získání offsetu z TMAPS00TAB_BEGIN_BUFFER (adaptace původní C struktury)
	# TBD: Zde bude čtení z pole offsetů
	return 0 # Prozatímní placeholder, aby nedošlo k chybě
	
func _read_uint16_at_offset(buffer: PackedByteArray, offset: int) -> int:
	# TBD: Čtení 16bitového čísla z bufferu (např. little-endian)
	return 255 # Placeholder, aby nedošlo k chybě
	
func _convert_buffer_to_texture(buffer: PackedByteArray, width: int, height: int) -> Texture2D:
	# TBD: Konverze surových dat textury na Godot Image/Texture
	return null # Placeholder, aby nedošlo k chybě
	
func _close_tmaps_file():
	# Uzavření souboru T-MAPS (adaptace sub_70BF0)
	print("- Uzavírám soubor T-Maps.")
	# Implementace by zde měla volat: tmaps_file.close()
	pass
	
# Třída představující datovou strukturu úrovně (náhrada za type_str_2FECE)
# Musí být definována jinde (např. ve vlastním skriptu)
#class TypeStr2FECE extends RefCounted:
	# Definujte zde všechna pole ze struktury type_str_2FECE

# Zde předpokládáme, že existuje globální třída/Singleton 'GameData' pro přístup k cestám a globálním proměnným.
# const X_DWORD_E9C38_SMALLTIT_SIZE = ... # Skutečná velikost dat (např. 0x6604)

func sub_533B0_decompress_levels(level_id: int, level_data: TypeStr2FECE) -> bool:
	# V původním kódu je návratová hodnota char (bool). Použijeme Godot bool.

	if level_id >= 1000:
		# V původním kódu vrací 1/true, pokud je level_id >= 1000.
		# Možná se jedná o kód pro editor nebo speciální úrovně.
		# Pokračujeme v logice, kde je návrat 1 (úspěch).
		return true

	var level_dat_file: FileAccess = null
	var level_tab_file: FileAccess = null

	# --- 1. Hledání LEVELS.DAT ---

	# Hledání v gameFolder/CLEVELS
	level_dat_file = FileAccess.open(levels_dat_path, FileAccess.READ)

	if level_dat_file == null:
		# Hledání v cdFolder/LEVELS
		levels_dat_path = ProjectSettings.get_setting("global_paths/cd_folder") + "/LEVELS/LEVELS.DAT"
		level_dat_file = FileAccess.open(levels_dat_path, FileAccess.READ)

	if level_dat_file == null:
		# Godot nemá návratovou hodnotu 0 pro NULL, ale vrací null.
		# Vracíme false pro neúspěch.
		return false

	# --- 2. Hledání LEVELS.TAB ---

	# Hledání v gameFolder/CLEVELS
	level_tab_file = FileAccess.open(levels_tab_path, FileAccess.READ)
	level_dat_file = FileAccess.open(levels_dat_path, FileAccess.READ)
	
	var level_tab_file_arr: Array[int] = []

	while not level_tab_file.eof_reached():
		level_tab_file_arr.append(level_tab_file.get_32())   # načte 4 byty jako int32
	level_tab_file.close()
	
	# Získání offsetu a velikosti (4-bajtová celá čísla, little-endian)
	# level_id * 4 (index do pole 4-bajtových intů)
	var level_start: int = level_tab_file_arr[level_id]
	var compressed_size: int = level_tab_file_arr[level_id+1]-level_tab_file_arr[level_id]
	
	level_dat_file.seek(level_start)
	var level_tab_data: PackedByteArray = level_dat_file.get_buffer(compressed_size)
	level_dat_file.close()
	
	var src := FileAccess.get_file_as_bytes("res://compressed.dat")
	var output := PackedByteArray()
	output.resize(1024 * 1024)
	var level_tab_data_unpacked = decompress_rnc1(level_tab_data)
	
	var level_struct:TypeStr2FECE = TypeStr2FECE.new()
	level_struct.decode_from(level_tab_data_unpacked)

	sub_56C00_sound_proc2(level_struct) # <--- NUTNÁ IMPLEMENTACE!
	sub_53590(level_struct) # <--- NUTNÁ IMPLEMENTACE!
	generate_level_map_43830(level_struct)
	return true

var x_WORD_17B4E0:int
var D41A0_0:type_D41A0_BYTESTR_0
var mapTerrainType_10B4E0: PackedByteArray = PackedByteArray()
var mapHeightmap_11B4E0: PackedByteArray = PackedByteArray()
var mapShading_12B4E0: PackedByteArray = PackedByteArray()
var mapAngle_13B4E0: PackedByteArray = PackedByteArray()
var mapEntityIndex_15B4E0: PackedInt32Array = PackedInt32Array()

func generate_level_map_43830(level_struct: TypeStr2FECE) -> void:
	D41A0_0 = type_D41A0_BYTESTR_0.new()
	mapTerrainType_10B4E0.resize(256 * 256)
	mapHeightmap_11B4E0.resize(256 * 256)
	mapShading_12B4E0.resize(256 * 256)
	mapAngle_13B4E0.resize(256 * 256)
	mapEntityIndex_15B4E0.resize(256 * 256)
	# ekvivalent:
	# x_WORD_17B4E0 = a2x->seed_0x2FEE5;
	x_WORD_17B4E0 = level_struct.word_0x2FEE5
	D41A0_0.rand_0x8 = level_struct.word_0x2FEE5
	# memset(mapEntityIndex_15B4E0, 0, 0x20000);
	mapEntityIndex_15B4E0.fill(0)
	sub_B5E70_decompress_terrain_map_level(
		x_WORD_17B4E0,
		level_struct.word_0x2FEE9,
		level_struct.word_0x2FEED,
		level_struct.word_0x2FEF1
	)
	# trunc + create heightmap
	#sub_44DB0_truncTerrainHeight(
		#mapEntityIndex_15B4E0,
		#mapHeightmap_11B4E0
	#)
	## memset(mapEntityIndex_15B4E0, 0, 0x20000);
	#mapEntityIndex_15B4E0.fill(0)
	#sub_44E40(
		#level_struct.word_0x2FEF5,
		#level_struct.word_0x2FEF9
	#)
	#sub_45AA0_setMax4Tiles()
	#sub_440D0(level_struct.word_0x2FF01)
	#sub_45060(
		#level_struct.word_0x2FF05,
		#level_struct.word_0x2FF09
	#)
	#sub_44320()
	#sub_45210(
		#level_struct.word_0x2FF05,
		#level_struct.word_0x2FF09
	#)
	#sub_454F0(
		#level_struct.word_0x2FEFD,
		#level_struct.word_0x2FF11
	#)
	#sub_45600(level_struct.word_0x2FF0D)
	#sub_43FC0()
	## memset(mapTerrainType_10B4E0, 0, 0x10000);
	#mapTerrainType_10B4E0.fill(0)
	#sub_43970() # smooth terrain
	#sub_43EE0() # add rivers
	#sub_44580() # set angle of terrain
	#if isCaveLevel_D41B6:
		#sub_43B40()
	#else:
		#sub_43D50()
	#sub_44D00()

var mapW: int = 256
var mapH: int = 256

func sub_B5E70_decompress_terrain_map_level(
	a1: int,   # int16
	a2: Vector2i,   # uint16
	a3: int,   # int16
	a4: int    # int32
) -> void:
	var sumEnt:Vector2i = Vector2i(0,0)
	mapEntityIndex_15B4E0[a2.y%mapH * mapW + a2.x%mapW] = a3  # first seed
	for i in range(7, -1, -1):
		sumEnt = a2
		var count :int = 1 << (7 - i)
		for j in range(count):
			for k in range(count):
				sub_B5EFA(1 << i, sumEnt, a4, a1)
			sumEnt.y += (2 * (1 << i))
		for j in range(count):
			for k in range(count):
				sub_B5F8F(1 << i, sumEnt, a4, a1)
			sumEnt.y += (2 * (1 << i))

func sub_B5EFA(a1: int, indexx: Vector2i, a3: int, nextRand: int):
	var sumEnt: int = mapEntityIndex_15B4E0[indexx.y%mapH * mapW + indexx.x%mapW]
	indexx += Vector2i(2 * a1,0)
	sumEnt += mapEntityIndex_15B4E0[indexx.y%mapH * mapW + indexx.x%mapW]
	indexx += Vector2i(0,2 * a1)
	sumEnt += mapEntityIndex_15B4E0[indexx.y%mapH * mapW + indexx.x%mapW]
	indexx += Vector2i(-2 * a1,0)
	sumEnt += mapEntityIndex_15B4E0[indexx.y%mapH * mapW + indexx.x%mapW]
	indexx += Vector2i(a1,-a1)
	nextRand = (9377 * nextRand + 9439) % 0x10000
	if mapEntityIndex_15B4E0[indexx.y%mapH * mapW + indexx.x%mapW] == 0:
		mapEntityIndex_15B4E0[indexx.y%mapH * mapW + indexx.x%mapW] = (
			(nextRand % (2 * a3 + 1)) +
			(nextRand % ((a1 << 6) + 1)) +
			(sumEnt >> 2) -
			32 * a1 -
			a3
		)
	indexx += Vector2i(a1,-a1)
	
func sub_B5F8F(a1: int, indexx: Vector2i, a3: int, nextRand: int):
	var sumEnt: int = mapEntityIndex_15B4E0[indexx.y%mapH * mapW + indexx.x%mapW]
	var sumEnt2: int = sumEnt
	indexx += Vector2i(a1,-a1)
	sumEnt += mapEntityIndex_15B4E0[indexx.y%mapH * mapW + indexx.x%mapW]
	indexx += Vector2i(a1,a1)
	sumEnt += mapEntityIndex_15B4E0[indexx.y%mapH * mapW + indexx.x%mapW]
	indexx += Vector2i(-a1,a1)
	sumEnt += mapEntityIndex_15B4E0[indexx.y%mapH * mapW + indexx.x%mapW]
	nextRand = (9377 * nextRand + 9439) % 0x10000
	sumEnt2 += mapEntityIndex_15B4E0[indexx.y%mapH * mapW + indexx.x%mapW]
	indexx += Vector2i(0,-a1)
	if mapEntityIndex_15B4E0[indexx.y%mapH * mapW + indexx.x%mapW] == 0:
		mapEntityIndex_15B4E0[indexx.y%mapH * mapW + indexx.x%mapW] = (
			(nextRand % (2 * a3 + 1)) +
			(nextRand % ((a1 << 6) + 1)) +
			(sumEnt >> 2) -
			32 * a1 -
			a3
		)
	indexx += Vector2i(-2 * a1,a1)
	sumEnt2 += mapEntityIndex_15B4E0[indexx.y%mapH * mapW + indexx.x%mapW]
	indexx += Vector2i(a1,a1)
	sumEnt2 += mapEntityIndex_15B4E0[indexx.y%mapH * mapW + indexx.x%mapW]
	indexx += Vector2i(0,-a1)
	nextRand = (9377 * nextRand + 9439) % 0x10000
	if mapEntityIndex_15B4E0[indexx.y%mapH * mapW + indexx.x%mapW] == 0:
		mapEntityIndex_15B4E0[indexx.y%mapH * mapW + indexx.x%mapW] = (
			(nextRand % (2 * a3 + 1)) +
			(nextRand % ((a1 << 6) + 1)) +
			(sumEnt2 >> 2) -
			32 * a1 -
			a3
		)
	indexx += Vector2i(2 * a1,-a1)


const RNC_SIGNATURE = 0x524E4301  # "RNC\x01"

const CRC_TABLE: Array[int] = [
		0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
		0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
		0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
		0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
		0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
		0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
		0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
		0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
		0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
		0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
		0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
		0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
		0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
		0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
		0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
		0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
		0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
		0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
		0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
		0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
		0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
		0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
		0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
		0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
		0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
		0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
		0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
		0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
		0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
		0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
		0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
		0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
]

# ────────────────────── PŘESNÁ CRC16 FUNKCE PODLE TVÉHO KÓDU (MSB-first, table-driven) ──────────────────────
func crc_block(buf: PackedByteArray, offset: int, size: int) -> int:
	var crc: int = 0
	var pos: int = offset
	while size > 0:
		var byte: int = buf[pos]
		pos += 1
		crc ^= byte
		crc = (crc >> 8) ^ CRC_TABLE[crc & 0xFF]
		size -= 1
	return crc
	
#var pack_block_start_0:int = 0
#var pack_block_start_1:int = 0
#var pack_block_start_2:int = 0
#var bit_count:int = 0
#var bit_buffer:int = 0
#var match_offset:int = 0
#var match_count:int = 0
#var window:int = 0

#func input_bits_m1(reader: BitReader,count: int) -> int:
	#var bits: int = 0
	##var prev_bits: int = 1
	#while count > 0:
		#if reader.bit_count == 0:
			#var b1: int = reader.read_bits(8)
			#var b2: int = reader.read_bits(8)
			##bit_buffer = (pack_block_start_1 << 24) | \
						 ##(pack_block_start_0 << 16) | \
						 ##(b2 << 8) | \
						 ##b1
			#reader.bit_count = 16
		##if (bit_buffer & 1) == 1:
			##bits |= prev_bits
		##bit_buffer >>= 1
		##prev_bits <<= 1
		#reader.bit_count -= 1
		#count -= 1
	#return bits
	
func inverse_bits(value: int, count: int) -> int:
	var reversed_val: int = 0
	for i in range(count):
		if (value & (1 << i)):
			reversed_val |= (1 << (count - 1 - i))
	return reversed_val

func proc_20(data_array: Array, count: int) -> void:
	var val: int = 0
	var div: int = 0x80000000 
	var bits_count: int = 1
	while bits_count <= 16:
		var i: int = 0
		while true:
			if i >= count:
				bits_count += 1
				div >>= 1
				break			
			var entry: HuffEntry = data_array[i]
			if entry.bit_depth == bits_count:
				entry.l3 = inverse_bits(int(val / div), bits_count)
				val += div	
			i += 1

# ────────────────────── BitReader (LSB-first) ──────────────────────
class BitReader extends RefCounted:
	var buffer: StreamPeerBuffer
	var bits: int = 0
	var bit_count: int = 0
	var bit_buffer: int = 0
	var vars: Node
	var output: PackedByteArray
	var window: int = 0
	#var outpos: int = 0
	var unpacked_crc_real: int = 0
	
	func _init(data: PackedByteArray, functions:Node):
		buffer = StreamPeerBuffer.new()
		buffer.data_array = data
		buffer.big_endian = false
		self.vars=functions
		
	
	
	func read_bitsx(count: int) -> int:
		while bit_count < count:
			if buffer.get_available_bytes() == 0:
				return 0
			var byte = buffer.get_8()
			bits |= (byte << bit_count)
			bit_count += 8
		var result = bits & ((1 << count) - 1)
		bits >>= count
		bit_count -= count
		return result
		
	func read_bits(count: int) -> int:
		var bits: int = 0
		var prev_bits: int = 1
		while count > 0:
			if bit_count == 0:
				var b1: int = buffer.get_u8()
				var b2: int = buffer.get_u8()
				var pos := buffer.get_position()
				var b3: int = buffer.get_u8()
				var b4: int = buffer.get_u8()
				buffer.seek(pos)
				bit_buffer = (b4 << 24) | (b3 << 16) | (b2 << 8) | b1
				bit_count = 16
			if (bit_buffer & 1) == 1:
				bits |= prev_bits
			bit_buffer >>= 1
			prev_bits <<= 1
			bit_count -= 1
			count -= 1
		return bits
		
	func write_decoded_byte1(decoded_byte: int):
		#if (&v->decoded[65535] == v->window):
			#write_buf(v->output, &v->output_offset, &v->decoded[v->dict_size], 65535 - v->dict_size);
			#memmove(v->decoded, &v->window[-v->dict_size], v->dict_size);
			#v->window = &v->decoded[v->dict_size];
		#*v->window++ = b;
		#v->unpacked_crc_real = crc_table[(v->unpacked_crc_real ^ b) & 0xFF] ^ (v->unpacked_crc_real >> 8);
		unpacked_crc_real = CRC_TABLE[(unpacked_crc_real ^ decoded_byte) & 0xFF] ^ (unpacked_crc_real >> 8)
		output[window]=decoded_byte
		window+=1
		
	func write_decoded_byte2(pos: int):
		write_decoded_byte1(output[window - pos])
	
	func read_byte() -> int:
		return buffer.get_u8()  # přímé čtení (používá se pro raw byty)

# ────────────────────── HuffmanTable ──────────────────────
class HuffmanTable extends RefCounted:
	#var lengths: Array[int] = []
	#var codes: Array[int] = []
	var data: Array[HuffEntry] = []
	var vars: Node
	
	func build(reader: BitReader, count: int, functions: Node):
		vars=functions
		for i in range(count):
			var entry: HuffEntry = HuffEntry.new()
			entry.l1 = 0
			entry.l2 = 0xFFFF 
			entry.l3 = 0
			entry.bit_depth = 0
			data.append(entry)
		var leaf_nodes = reader.read_bits(5)
		if(leaf_nodes):
			if leaf_nodes > 16:
				leaf_nodes = 16
			for i in range(leaf_nodes):
				data[i].bit_depth = reader.read_bits(4)
			vars.proc_20(data, leaf_nodes)
	
	func decode(reader: BitReader) -> int:
		var i:int = 0;
		while (true):
			if (data[i].bit_depth && (data[i].l3 == (reader.bit_buffer & ((1 << data[i].bit_depth) - 1)))):
				reader.read_bits(data[i].bit_depth);
				if (i < 2):
					return i
				return reader.read_bits(i - 1) | (1 << (i - 1))
			i+=1
		return -1


# ────────────────────── Dekomprese jednoho RNC1 bloku + CRC kontrola ──────────────────────
func decompress_rnc1(data: PackedByteArray) -> PackedByteArray:
	var header = StreamPeerBuffer.new()
	header.data_array = data
	header.big_endian = true
	
	if header.get_u32() != RNC_SIGNATURE:
		push_error("Není RNC1 soubor!")
		return PackedByteArray()
	
	var unpacked_size = header.get_u32()
	var packed_size = header.get_u32()
	var unpacked_crc = header.get_u16()
	var packed_crc  = header.get_u16()
	header.get_16()  # method = 1
	
	# Kontrola packed CRC
	var header_plus_data = data.slice(0, 18 + packed_size)
	var packed_crc_calc = crc_block(data, 18, data.size() - 18)
	if packed_crc_calc != packed_crc:
		push_warning("Poškozený RNC blok – packed CRC nesedí!")
	
	var compressed = data.slice(18)
	var reader = BitReader.new(compressed,self)
	#reader.read_bits(2)  # skip 2 bits
	
	var enc_key = 0
	var pus_mode = 0;
	
	var bit = reader.read_bits(1)
	
	reader.output = PackedByteArray()
	reader.output.resize(unpacked_size)

	#int error_code = 0;
	if (bit && !pus_mode):
		push_warning("RNC error")
	else:
		bit = reader.read_bits(1)
		if (bit && !enc_key):
			push_warning("RNC error 2")
		else:
			var processed_size = 0;
			while processed_size < unpacked_size:
				var raw_table = HuffmanTable.new()
				raw_table.build(reader,16,self)
				var len_table = HuffmanTable.new()
				len_table.build(reader,16,self)
				var pos_table = HuffmanTable.new()
				pos_table.build(reader,16,self)

				var subchunks: int = reader.read_bits(16)
				
				#var outpos = 0
				while subchunks > 0:
					var data_length: int = raw_table.decode(reader)
					processed_size += data_length

					if data_length > 0:
						for i in range(data_length):
							var decoded_byte: int = (enc_key ^ reader.read_byte()) & 0xFF
							reader.write_decoded_byte1(decoded_byte)
							#output[outpos]=decoded_byte
							#outpos+=1
						ror_w(enc_key)
						var pos:int = reader.buffer.get_position()
						var b1: int = reader.buffer.get_u8()
						var b2: int = reader.buffer.get_u8()
						var b3: int = reader.buffer.get_u8()
						reader.buffer.seek(pos)
						var fix_overflow: int = ((b3 << 16) | (b2 << 8) | b1);
						reader.bit_buffer = (fix_overflow << reader.bit_count) | (reader.bit_buffer & ((1 << reader.bit_count) - 1));
						#var fix_overflow: int = (pack_block_start_2 << 16) | (pack_block_start_1 << 8) | pack_block_start_0
						#var mask: int = (1 << reader.bit_count) - 1
						#bit_buffer = (fix_overflow << reader.bit_count) | (bit_buffer & mask)
					subchunks -= 1
					if subchunks > 0:
						var prev_pos:int = reader.buffer.get_position()
						var match_offset = len_table.decode(reader) + 1
						var match_count = pos_table.decode(reader) + 2
						processed_size += match_count
						for i in range(match_count):
							reader.write_decoded_byte2(match_offset)
							#var read_index: int = window - match_offset
							#var pos:int = reader.buffer.get_position()
							#reader.buffer.seek(prev_pos-match_offset)
							#var byte_to_write: int = reader.buffer.get_u8()
							#reader.buffer.seek(pos)
							#output[outpos]=byte_to_write
							#outpos+=1
							#write_decoded_byte(byte_to_write)
	
	
#
	#if (!error_code)
	#{
	#switch (v->method)
	#{
	#case 1: error_code = unpack_data_m1(v); break;
	#case 2: error_code = unpack_data_m2(v); break;
	#}
	#}
#
	#v->enc_key = specified_key;
#
	#free(v->mem1);
	#free(v->decoded);
#
	#v->input_offset = start_pos + v->packed_size + RNC_HEADER_SIZE;
#
	#if (error_code)
	#return error_code;
#
	#if (v->unpacked_crc != v->unpacked_crc_real)
	#return 5;
	
	
	
	#var raw_table = HuffmanTable.new()
	#var dist_table = HuffmanTable.new()
	#var len_table = HuffmanTable.new()
	#
	#if !raw_table.build(reader) || !dist_table.build(reader) || !len_table.build(reader):
		#push_error("Chyba při čtení Huffman tabulek")
		#return PackedByteArray()
	#
	#var output = PackedByteArray()
	#output.resize(unpacked_size)
	#var pos = 0
	#var blocks = reader.read_bits(16)
	#
	#while blocks > 0 and pos < unpacked_size:
		#blocks -= 1
		#
		## Raw bytes
		#var raw_count = raw_table.decode(reader)
		#for _i in raw_count:
			#if pos >= unpacked_size: break
			#output[pos] = reader.read_byte()
			#pos += 1
		#
		#if blocks == 0:
			#break
		#
		## Kopírovací blok
		#var dist = dist_table.decode(reader) + 1
		#var len = len_table.decode(reader) + 2
		#
		#for _i in len:
			#if pos >= unpacked_size: break
			#var src = pos - dist
			#output[pos] = output[src] if src >= 0 else 0
			#pos += 1
	#
	## Konečná kontrola unpacked CRC
	#var unpacked_crc_calc = crc_block(output, 0, output.size())
	#if unpacked_crc_calc != unpacked_crc:
		#push_error("UNPACKED CRC CHYBA! Očekáváno 0x%04X, vypočítáno 0x%04X" % [unpacked_crc, unpacked_crc_calc])
		#return PackedByteArray()
	if (unpacked_crc != reader.unpacked_crc_real):
		push_error("UNPACKED CRC CHYBA! Očekáváno 0x%04X, vypočítáno 0x%04X" % [unpacked_crc, reader.unpacked_crc_calc])
	return reader.output
	
## Provádí bitovou rotaci doprava (ROR) o 1 bit na 16bitovém 'enc_key'.
## Používá se pro posun šifrovacího klíče.
func ror_w(x: int) -> int:
	x = x & 0xFFFF
	var result: int
	if (x & 1) == 1:
		result = 0x8000 | (x >> 1)
	else:
		result = x >> 1
	return result & 0xFFFF
	
var bit_depth: int = 0
var l3: int = 0 # Dekódovaný kód

# --- Implementace metody v DecompressionContext.gd ---

## Dekóduje datovou položku pomocí Huffmanovy tabulky.
## @param data: Pole objektů (HuffEntry), reprezentující Huffmanovu tabulku.
## @return: Dekódovaná celočíselná hodnota (Length/Distance).
#func decode_table_data(data: Array) -> int:
	#var i: int = 0
	#while true:
		#if i >= data.size():
			#push_error("Chyba dekódování Huffmanovy tabulky: Shoda nebyla nalezena.")
			#return -1 
		#var entry: HuffEntry = data[i]
		#if entry.bit_depth > 0:
			#var depth: int = entry.bit_depth
			#var mask: int = (1 << depth) - 1
			#if entry.l3 == (bit_buffer & mask):
				#input_bits_m1(depth) 
				#if i < 2:
					#return i
				#else:
					#var extra_bits_count: int = i - 1
					#var extra_bits: int = input_bits_m1(extra_bits_count)
					#var base_value: int = (1 << extra_bits_count)					
					#return base_value | extra_bits
		#i += 1
		#
#func decode_table_data2(data: Array) -> int:
	#var i: int = 0
	#while true:
		#if i >= data.size():
			#return -1
		#var entry: HuffEntry = data[i]
		#if entry.bit_depth > 0:
			#var mask: int = (1 << entry.bit_depth) - 1
			#if entry.l3 == (bit_buffer & mask):
				#input_bits_m1(entry.bit_depth)
				#if i < 2:
					#return i
				#else:
					#var extra_bits: int = input_bits_m1(i - 1)
					#var base_value: int = (1 << (i - 1))					
					#return base_value | extra_bits
		#i += 1
	#return -1

# Převedení bajtů na "shadow" strukturu
func convert_to_shadow_struct(decompressed_data: PackedByteArray):
	# Převedení PackedByteArray na datovou strukturu/slovník pro stínovou strukturu
	return {}

# Převedení "shadow" struktury na finální strukturu
func convert_from_shadow(shadow_data, target_struct: TypeStr2FECE):
	# Zde implementujte logiku Convert_from_shadow_str_2FECE
	pass

# Funkce pro zpracování zvuku/prostředí
func sub_56C00_sound_proc2(data: TypeStr2FECE):
	pass

# Další funkce pro zpracování dat úrovně
func sub_53590(data: TypeStr2FECE):
	pass

# ... ostatní placeholdery (_open_tmaps_file, _close_tmaps_file, _read_uint16_at_offset, _convert_buffer_to_texture)

	#orig code
	#*xadataclrd0dat.colorPalette_var28 = (uint8_t*)malloc(4096);//fix it 3x256 ?
#
	#initposistruct();
#
	#//x_DWORD_EA3E4[D41A0_BYTESTR_0.array_0x2BDE[D41A0_BYTESTR_0.word_0xc].word_0x00a_2BE4_11240]->dword_0xA4_164x->str_611.word_0x451_1105
#
	#gameDataPath = GetSubDirectoryPath(gameFolder);
	#cdDataPath = GetSubDirectoryPath(cdFolder);
	#SetCDFilePaths(cdDataPath.c_str(), pstr);
	#//init
	#sub_5BCC0_set_any_variables1();//23C9F2 - 23CCC0
	#if (!sub_5BF50_load_psxdata())//23C9F7 - 23CF50 //something with files about their loading, or just a set of palettes
	#exit(-1);
	#sub_5C1B0_set_any_variables2();
#
	#sub_71410_process_tmaps();//252410
#
	#CreateIndexes_6EB90(&filearray_2aa18c[filearrayindex_POINTERSDATTAB]);//24fb90
	#CreateIndexes_6EB90(&filearray_2aa18c[filearrayindex_BUILD00DATTAB]);//24fb90 adress 0x23ca2e
	#sub_101C0();//23CA36 - 1f11c0 //sem se to vubec nedostane
#
	#//init
	#sub_6EBF0(&filearray_2aa18c[filearrayindex_POINTERSDATTAB]);
	#x_WORD_180660_VGA_type_resolution = 1;
	#x_WORD_E29DA_type_resolution = 1;
#
	#//sub_6FC50(1);//only for fonts?
#
	#//save D41A0_BYTESTR_0
	#D41A0_0.terrain_2FECE.seed_0x2FEE5 = 0;
	#D41A0_0.terrain_2FECE.offset_0x2FEE9 = 0;
	#D41A0_0.terrain_2FECE.raise_0x2FEED = 0;
	#D41A0_0.terrain_2FECE.gnarl_0x2FEF1 = 0;
	#D41A0_0.terrain_2FECE.source_0x2FEFD = 0;
	#D41A0_0.terrain_2FECE.snLin_0x2FF01 = 0;
	#D41A0_0.terrain_2FECE.bhFlt_0x2FF0D = 0;
	#D41A0_0.terrain_2FECE.rkSte_0x2FF11 = 0;
	#D41A0_0.terrain_2FECE.rkSte_0x2FF11 = 0;
	#//init_pal();
	#clean_tarrain();
	#loadlevel(0);
	#terrain_recalculate();
