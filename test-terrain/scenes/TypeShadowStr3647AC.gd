# type_shadow_str_3647ac.gd
class_name TypeShadowStr3647AC extends RefCounted
# Reprezentuje C/C++ strukturu 'type_str_0x3647Ac' (délka 8 bytů)

# Offset 0x0000 (0)
# int8_t index_0x3647A_0;
@export var array_index: int = 0

# Offset 0x0001 (1)
# int8_t stage_0x3647A_1;
@export var stage_index: int = 0

# Offset 0x0002 (2)
# uaxis_2d str_0x3647A_2;
#@export var position_2d: UAxis2D = UAxis2D.new()

# Offset 0x0006 (6)
# un1 str_0x3647C_4;
#@export var unknown_word: UN1 = UN1.new()

# -- Funkce pro čtení 8 bytů z binárního datového proudu --
func load_from_stream(stream: StreamPeerBuffer) -> bool:
	const STRUCT_SIZE = 8
	if stream.get_available_bytes() < STRUCT_SIZE:
		printerr("Chyba: Stream je příliš krátký pro TypeShadowStr3647AC (8 bytů)")
		return false
	
	# Nastavte endianitu (předpokládáme little-endian)
	stream.set_big_endian(false) 
	
	# Offset 0: int8_t (Signed)
	array_index = stream.get_8_signed()
	
	# Offset 1: int8_t (Signed)
	stage_index = stream.get_8_signed()
	
	# Offset 2: uaxis_2d (4 byty)
	#position_2d.load_from_stream(stream)
	
	# Offset 6: un1 (2 byty)
	#unknown_word.load_from_stream(stream)
	
	# Posunuto celkem o 8 bytů.
	return true
