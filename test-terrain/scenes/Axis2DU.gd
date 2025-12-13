# axis_2du.gd
class_name Axis2DU extends RefCounted
# Reprezentuje C/C++ strukturu 'axis_2du' (délka 4 byty: 2x uint16_t)

# Offset 0x0000 (0)
# uint16_t x_axis;
@export var x: int = 0

# Offset 0x0002 (2)
# uint16_t y_axis;
@export var y: int = 0

# Funkce pro načtení 4 bytů z binárního datového proudu
func load_from_stream(stream: StreamPeerBuffer):
	# Předpokládáme, že endianita (např. little-endian) je již nastavena
	x = stream.get_16()
	y = stream.get_16()
