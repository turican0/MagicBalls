# type_str_360d2.gd
class_name TypeStr360D2 extends RefCounted
# Reprezentuje C/C++ strukturu 'type_str_0x360D2' (délka 110 bytů)

# Offset 0x0000 (0)
# uint8_t stuba[3];
@export var stuba: PackedByteArray = PackedByteArray() # Délka 3

# Offset 0x0003 (3)
# int16_t word_0x360D5;
@export var word_360D5: int = 0

# Offset 0x0005 (5)
# uint8_t stubb[2];
@export var stubb: PackedByteArray = PackedByteArray() # Délka 2

# Offset 0x0007 (7)
# int16_t word_0x360D9;
@export var word_360D9: int = 0

# Offset 0x0009 (9)
# uint8_t stubc[2];
@export var stubc: PackedByteArray = PackedByteArray() # Délka 2

# Offset 0x000B (11)
# int16_t word_0x360DD;
@export var word_360DD: int = 0

# Offset 0x000D (13)
# uint8_t stubd[2];
@export var stubd: PackedByteArray = PackedByteArray() # Délka 2

# Offset 0x000F (15)
# uint8_t byte_0x360E1x[26];
@export var array_360E1: PackedByteArray = PackedByteArray() # Délka 26

# Offset 0x0029 (41)
# uint8_t byte_0x360FBx[26];
@export var array_360FB: PackedByteArray = PackedByteArray() # Délka 26

# Offset 0x0043 (67)
# uint8_t byte_0x36115x[26];
@export var array_36115: PackedByteArray = PackedByteArray() # Délka 26

# Původní kód má mezeru (komentované položky) mezi 0x36115x a 0x3612F.
# 67 + 26 = 93. Další pole je na offsetu 103 (0x67).
# Tedy 103 - 93 = 10 bytů jsou nevyužité mezery/stub data.
# 
# Původní struktura:
# ... byte_0x36115x[26]; (do offsetu 93)
# //uint8_t stube[25]; (komentováno, ale naznačuje možnou mezeru)
# int16_t word_0x3612F; (Offset 103)

# Offset 0x005D (93)
# stub data 10 bytů (93 až 103)
@export var stub_e_gap: PackedByteArray = PackedByteArray() # Délka 10

# Offset 0x0
