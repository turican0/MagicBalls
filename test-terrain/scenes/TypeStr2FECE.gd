# type_shadow_str_2fece.gd
class_name TypeStr2FECE extends RefCounted

# Velikost: 0x6604u (26116 bytů) - Důležité pro čtení ze souboru

# --- POČÁTEČNÍ HLAVIČKA (Offset 0x0000) ---
# Upozornění: Godot automaticky spravuje paměť, ale offsety jsou užitečné pro mapování I/O.

# Offset 0x0000 (0)
# uint16_t word_2FECE;
@export var word_2FECE: int = 0

# Offset 0x0002 (2)
# uint16_t word_2FED0;
@export var word_2FED0: int = 0

# Offset 0x0004 (4)
# uint8_t byte_0x2FED2; // type of level graphics
@export var byte_0x2FED2: int = 0

# Offset 0x0005 (5)
# uint8_t byte_0x2FED3;
@export var byte_0x2FED3: int = 0

# Offset 0x0006 (6)
# uint8_t MapType; // GraphicsType
@export var MapType: int = 0

# Offset 0x0007 (7)
# int16_t word_0x2FED5;
@export var word_0x2FED5: int = 0

# Offset 0x0009 (9)
# int16_t word_0x2FED7;
@export var word_0x2FED7: int = 0

# Offset 0x000B (11)
# int8_t array_0x2FED9[8];
@export var array_0x2FED9: PackedByteArray = PackedByteArray() # Měla by mít délku 8

# Offset 0x0013 (19)
# uint8_t stubb[4];
#@export var stubb: PackedByteArray = PackedByteArray() # Délka 4

# Offset 0x0017 (23)
# uint16_t word_0x2FEE5;
@export var word_0x2FEE5: int = 0

# Offset 0x0019 (25)
# uint8_t stubb2[2];
#@export var stubb2: PackedByteArray = PackedByteArray() # Délka 2

# Offset 0x001B (27)
# uint16_t word_0x2FEE9; // first seed position
@export var word_0x2FEE9: Vector2i = Vector2i(0,0)

# Offset 0x001D (29)
# uint8_t stubc[2];
#@export var stubc: PackedByteArray = PackedByteArray() # Délka 2

# Offset 0x001F (31)
# uint16_t word_0x2FEED; // first seed height
@export var word_0x2FEED: int = 0

# Offset 0x0021 (33)
# uint8_t stubc2[2];
#@export var stubc2: PackedByteArray = PackedByteArray() # Délka 2

# Offset 0x0023 (35)
# uint16_t word_0x2FEF1; // random seed
@export var word_0x2FEF1: int = 0

# Offset 0x0025 (37)
# uint8_t stubc3[2];
#@export var stubc3: PackedByteArray = PackedByteArray() # Délka 2

# Offset 0x0027 (39)
# uint32_t word_0x2FEF5;
@export var word_0x2FEF5: int = 0

# Offset 0x002B (43)
# uint16_t word_0x2FEF9;
@export var word_0x2FEF9: int = 0

# Offset 0x002D (45)
# uint8_t stube[2];
#@export var stube: PackedByteArray = PackedByteArray() # Délka 2

# Offset 0x002F (47)
# uint16_t word_0x2FEFD;
@export var word_0x2FEFD: int = 0

# Offset 0x0031 (49)
# uint8_t stubf[2];
#@export var stubf: PackedByteArray = PackedByteArray() # Délka 2

# Offset 0x0033 (51)
# uint16_t word_0x2FF01;
@export var word_0x2FF01: int = 0

# Offset 0x0035 (53)
# uint8_t stubg[2];
#@export var stubg: PackedByteArray = PackedByteArray() # Délka 2

# Offset 0x0037 (55)
# uint16_t word_0x2FF05;
@export var word_0x2FF05: int = 0

# Offset 0x0039 (57)
# uint8_t stubh[2];
#@export var stubh: PackedByteArray = PackedByteArray() # Délka 2

# Offset 0x003B (59)
# uint16_t word_0x2FF09;
@export var word_0x2FF09: int = 0

# Offset 0x003D (61)
# uint8_t stubi[2];
#@export var stubi: PackedByteArray = PackedByteArray() # Délka 2

# Offset 0x003F (63)
# uint16_t word_0x2FF0D;
@export var word_0x2FF0D: int = 0

# Offset 0x0041 (65)
# uint8_t stubj[2];
#@export var stubj: PackedByteArray = PackedByteArray() # Délka 2

# Offset 0x0043 (67)
# uint16_t word_0x2FF11;
@export var word_0x2FF11: int = 0

# Offset 0x0045 (69)
# uint8_t stubk[1022];
#@export var stubk: PackedByteArray = PackedByteArray() # Délka 1022

# --- SEZNAM ENTIT (Offset 0x0447 / 1095) ---
# type_entity_0x30311 array_0x30311[1200];
# Předpokládáme, že type_entity_0x30311 je definována jinde
var entities: Array[TypeEntity30311] = [] # 1200 prvků

# --- NÁSLEDUJÍCÍ DATA ---

# Offset 0x5D87 (23943 - konec pole entities)
# uint8_t next_0x360D1;
@export var next_360D1: int = 0

# Offset 0x5D88 (23944)
# type_str_0x360D2 next_0x360D2[8]; // Délka 110 / 8 prvků
var spells: Array[TypeStr360D2] = [] # 8 prvků

# Offset 0x60DC (24796)
# type_str_0x36442 str_0x36442[8]; // stages (checkpoints)
var stages: Array[TypeStr36442] = [] # 8 prvků

# Offset 0x61B4 (25012)
# type_shadow_str_0x3647Ac array_0x3647A[11]; // 8x11
var shadow_array: Array[TypeShadowStr3647AC] = [] # 11 prvků

func decode_from(buffer: PackedByteArray):
	var sp := StreamPeerBuffer.new()
	sp.data_array = buffer
	sp.big_endian = false # nebo true podle formátu dat
	word_2FECE = sp.get_u16()
	word_2FED0 = sp.get_u16()
	byte_0x2FED2 = sp.get_u8()
	byte_0x2FED3 = sp.get_u8()
	MapType = sp.get_u8()
	word_0x2FED5 = sp.get_u16()
	word_0x2FED7 = sp.get_u16()
	array_0x2FED9.resize(8)
	for i in 8:
		array_0x2FED9[i] = sp.get_u8()  # 0–255
	for i in 4:
		sp.get_u8()
	word_0x2FEE5 = sp.get_u16()
	for i in 2:
		sp.get_u8()
	word_0x2FEE9.x = sp.get_u8()
	word_0x2FEE9.y = sp.get_u8()
	for i in 2:
		sp.get_u8()
	word_0x2FEED = sp.get_u16()
	for i in 2:
		sp.get_u8()
	word_0x2FEF1 = sp.get_u16()
	for i in 2:
		sp.get_u8()
	word_0x2FEF5 = sp.get_u32()
	word_0x2FEF9 = sp.get_u16()
	for i in 2:
		sp.get_u8()
	word_0x2FEFD = sp.get_u16()
	for i in 2:
		sp.get_u8()
	word_0x2FF01 = sp.get_u16()
	for i in 2:
		sp.get_u8()
	word_0x2FF05 = sp.get_u16()
	for i in 2:
		sp.get_u8()
	word_0x2FF09 = sp.get_u16()
	for i in 2:
		sp.get_u8()
	word_0x2FF0D = sp.get_u16()
	for i in 2:
		sp.get_u8()
	word_0x2FF11 = sp.get_u16()
	for i in 1022:
		sp.get_u8()

# Důležitá metoda pro načítání dat z PackedByteArray
# Tato funkce by se používala po dekompresi
func load_from_buffer(buffer: PackedByteArray) -> bool:
	if buffer.size() < 0x6604:
		return false # Nedostatečná velikost bufferu

	var stream = StreamPeerBuffer.new()
	stream.set_data(buffer)
	stream.set_big_endian(false) # Typicky little-endian pro Windows/x86

	# Implementujte zde sekvenční čtení všech polí pomocí stream.get_8(), stream.get_16(), stream.get_32(), atd.
	# U polí (PackedByteArray) použijte stream.get_data(size)[1]

	# Příklad čtení prvních polí
	word_2FECE = stream.get_16() # 2 bajty
	word_2FED0 = stream.get_16() # 2 bajty
	#level_graphics_type = stream.get_8() # 1 bajt
	#byte_2FED3 = stream.get_8() # 1 bajt
	#map_type = stream.get_8() # 1 bajt
	
	# ... (dále pokračujte pro všechna pole)
	
	# Příklad pro pole:
	# stubk = stream.get_data(1022)[1] # Délka 1022
	
	# Příklad pro vnořené struktury:
	# for i in 1200:
	#     var entity_data = stream.get_data(ENTITY_SIZE)[1]
	#     var entity = TypeEntity30311.new()
	#     entity.load_from_buffer(entity_data)
	#     entities.append(entity)

	return true
