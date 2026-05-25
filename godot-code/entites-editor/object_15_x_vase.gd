extends Node3D

@onready var sprite_3d2 = $Sprite3D2

func _process(delta: float) -> void:
	if has_meta("subtype_0x30311"):
		var subtype_value: int = get_meta("subtype_0x30311")
		if subtype_value >= 0 and subtype_value <= 25:
			var texture_filename: String = "../../HSPR/HSPR-day/HSPRD0-0.DAT_%03d.png" % (subtype_value + 97)
			if sprite_3d2.texture_filename != texture_filename:
				sprite_3d2.texture_filename = texture_filename
