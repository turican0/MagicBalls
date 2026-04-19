@tool
extends TextureButton

func _ready():
	custom_minimum_size = Vector2(40, 40)
	
	var fixed_name = self.name.replace("_DAT", ".DAT")
	fixed_name = fixed_name.replace("_png", ".png")
	var image_path = "user://convertdata/HSPR/HSPR-day/" + fixed_name
	
	if FileAccess.file_exists(image_path):
		var image = Image.load_from_file(image_path)
		var texture = ImageTexture.create_from_image(image)
		texture_normal = texture
		
		# DŮLEŽITÉ: Aby se textura neroztáhla nebo nesmrštila divně
		ignore_texture_size = true
		stretch_mode = TextureButton.STRETCH_KEEP_ASPECT_CENTERED
