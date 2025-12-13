# type_entity_30311.gd
class_name TypeEntity30311 extends RefCounted
# Reprezentuje C/C++ strukturu 'type_entity_0x30311' (délka 20 bytů)

# Offset 0x0000 (0)
# uint16_t type_0x30311;
@export var entity_type: int = 0

# Offset 0x0002 (2)
# int16_t subtype_0x30311;
@export var entity_subtype: int = 0

# Offset 0x0004 (4)
# axis_2du axis2d_4;
# POZNÁMKA: Type Axis2DU musí být definována pomocí 'class_name' a dostupná.
#@export var position_2d: Axis2DU = Axis2DU.new()

# Offset 0x0008 (8)
# int16_t DisId; // must be signed
@export var display_id: int = 0

# Offset 0x000A (10)
# uint16_t word_10;
@export var word_10: int = 0

# Offset 0x000C (12)
# int16_t stageTag_12;
@export var stage_tag: int = 0

# Offset 0x000E (14)
# uint16_t par1_14; // parameter 1
@export var parameter_1: int = 0

# Offset 0x0010 (16)
# uint16_t par2_16; // parameter 2
@export var parameter_2: int = 0

# Offset 0x0012 (18)
# uint16_t par3_18; // parameter 3
@export var parameter_3: int = 0

# -- Funkce pro čtení 20 bytů z binárního datového proudu --
func load_from_stream(stream: StreamPeerBuffer) -> bool:
	if stream.get_available_bytes() < 20:
		printerr("Chyba: Stream je příliš krátký pro TypeEntity30311 (20 bytů)")
		return false
	
	# Předpokládáme, že endianita (little-endian) je již nastavena na streamu
	
	entity_type = stream.get_16()
	entity_subtype = stream.get_16()
	
	# Načtení vnořené struktury Axis2DU (4 byty)
	#position_2d.load_from_stream(stream)
	
	display_id = stream.get_16_signed() # Signed int16
	word_10 = stream.get_16()
	stage_tag = stream.get_16_signed() # Signed int16
	parameter_1 = stream.get_16()
	parameter_2 = stream.get_16()
	parameter_3 = stream.get_16()
	
	return true
