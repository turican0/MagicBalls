# type_str_36442.gd
class_name TypeStr36442 extends RefCounted
# Reprezentuje C/C++ strukturu 'type_str_0x36442' (délka 15 bytů, 8 prvků)

# POZNÁMKA: Předpokládáme, že 'axis_2d' je stejná jako 'Axis2DU' (4 byty)

# Offset 0x0000 (0)
# int8_t index_0;
@export var index: int = 0

# Offset 0x0001 (1)
# int16_t stage_1;
@export var stage_id: int = 0

# Offset 0x0003 (3)
# axis_2d _axis_2d;
var position_2d: Axis2DU = Axis2DU.new() # Předpokládáme existenci Axis2DU

# Offset 0x0007 (7)
# Zde se nachází mezera/stub, aby se dosáhlo délky 15 bytů.
# 15 (celková délka) - 7 (dosud přečteno) = 8 bytů stub dat.
@export var stub_data: PackedByteArray = PackedByteArray() # Délka 8

# -- Funkce pro čtení binárních dat (Klíčová pro deserializaci) --
func load_from_stream(stream: StreamPeerBuffer) -> bool:
	const STRUCT_SIZE = 15
	if stream.get_available_bytes() < STRUCT_SIZE:
		printerr("Chyba: Stream je příliš krátký pro TypeStr36442 (15 bytů)")
		return false
	
	# Nastavte endianitu (předpokládáme little-endian)
	stream.set_big_endian(false) 
	
	# Offset 0: int8_t (Signed)
	index = stream.get_8_signed()
	
	# Offset 1: int16_t (Signed)
	stage_id = stream.get_16_signed()
	
	# Offset 3: axis_2d (4 byty)
	position_2d.load_from_stream(stream)
	
	# Offset 7: Stub data (8 bytů)
	stub_data = stream.get_data(8)[1]
	
	return true
