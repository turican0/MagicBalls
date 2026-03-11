extends Node3D

var Main_DecodeLevel

var runned:bool = false

var LoadScrRect:TextureRect
var FlagRect:TextureRect

func init():
	#Global.loadScreenInit()
	Main_DecodeLevel.MBEXinit()
	Global.initSound()
	#Main_DecodeLevel.anim1Begin($Control/Foreground)
	Main_DecodeLevel.setLoadScreenBuffer(LoadScrRect)
	Global.defaultLangIndex=Main_DecodeLevel.initLanguage(2)-1
	
	#Global.Main_Sounds.setSoundBank(3)
	
	runned=true
	
func beginMainMenu():
	Global.Main_Sounds.setSoundBank(3)
	Global.countLang=getLangCount()
	Global.defaultLangIndex=Main_DecodeLevel.initLanguage(Global.defaultLangIndex+1)-1
	Main_DecodeLevel.getLangTexts()
	set_language_texture(Global.defaultLangIndex+1)
	FlagRect.material.set_shader_parameter("alpha", 1.0)

func beginAnimation():
	Global.Main_Sounds.setSoundBank(4)

func set_language_texture(lang_id: int) -> bool:
	var base_path := "user://convertdata/language/"
	var file_name := "L%d.TXT.png" % lang_id
	var full_path := base_path + file_name	
	if not FileAccess.file_exists(full_path):
		push_error("File not found: " + full_path)
		return false
	var image := Image.new()
	var err := image.load(full_path)	
	if err != OK:
		push_error("Can not read image: " + full_path)
		return false	
	var texture := ImageTexture.create_from_image(image)
	FlagRect.texture = texture	
	return true

func getLangCount() -> int:
	var path := "user://convertdata/language/"
	var count := 0
	
	var dir := DirAccess.open(path)
	if dir == null:
		push_error("Nelze otevřít složku: " + path)
		return 0	
	dir.list_dir_begin()
	var file_name = dir.get_next()	
	while file_name != "":
		if not dir.current_is_dir():
			if file_name.begins_with("L") and file_name.ends_with(".TXT.png"):
				count += 1
		file_name = dir.get_next()
	dir.list_dir_end()
	return count

func _process(_p_delta) -> void:
	if(!runned):
		return
	var endRunVar = Main_DecodeLevel.MBrun()
	match(endRunVar):
		1:
			Global.Main_Sounds.stopAllSounds()
			get_tree().quit()
		2:
			Global.defaultLangIndex=(Global.defaultLangIndex+1)%Global.countLang;
			set_language_texture(Global.defaultLangIndex+1)
			Main_DecodeLevel.changeLanguage(Global.defaultLangIndex+1)
			FlagRect.material.set_shader_parameter("alpha", 1.0)
		3:
			beginMainMenu()
		4:
			beginAnimation()
	#else:
		#animImage.set_data(animWidth, animHeight, false, Image.FORMAT_RGB8, Main_DecodeLevel.getVGABuffer())
		#animTextureRect.update(animImage)
	
