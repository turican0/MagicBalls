@tool
extends Sprite3D

@export var texture_filename: String = "":
	set(value):
		texture_filename = value
		_load_texture()

func _load_texture():
	if texture_filename == "":
		return
	
	var path = OS.get_user_data_dir() + "/convertdata/TMAPS/TMAPS-day/" + texture_filename
	
	if not FileAccess.file_exists(path):
		push_warning("Texture not found: " + path)
		return
	
	var img = Image.load_from_file(path)
	if img == null:
		push_error("Can not read image: " + path)
		return
	
	texture = ImageTexture.create_from_image(img)
