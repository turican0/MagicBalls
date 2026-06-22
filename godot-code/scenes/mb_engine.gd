extends Node3D

var Main_DecodeLevel
var Main_TerrainsMB
var Main_UI
var Main_Filter

var runned:bool = false

var LoadScrRect:TextureRect
var FlagRect:TextureRect

func init():
	Engine.max_fps = 30
	Input.mouse_mode = Input.MOUSE_MODE_CONFINED_HIDDEN
	#Global.loadScreenInit()
	Main_DecodeLevel.MBEXinit()
	Global.MBEX.REMC2SetInverseMouse(Global.inverse_mouseY)
	Global.initSound()
	#Main_DecodeLevel.anim1Begin($Control/Foreground)
	Main_DecodeLevel.setLoadScreenBuffer(LoadScrRect)
	Global.defaultLangIndex=Main_DecodeLevel.initLanguage(2)-1
	
	#Global.Main_Sounds.setSoundBank(3)
	
	runned=true
	
func beginMainMenu():
	Global.Main_Sounds.setSoundBank(0)
	Global.countLang=getLangCount()
	Global.defaultLangIndex=Main_DecodeLevel.initLanguage(Global.defaultLangIndex+1)-1
	Main_DecodeLevel.getLangTexts()
	set_language_texture(Global.defaultLangIndex+1)
	FlagRect.material.set_shader_parameter("alpha", 1.0)

func beginAnimation(soundIndex):
	Global.Main_Sounds.setSoundBank(soundIndex)

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

func beginInGame():
	Main_TerrainsMB.init(true)
	Main_DecodeLevel.gameInit(true)

var inGameLoop=false

func _process(_p_delta) -> void:
	if(!runned):
		return

	print("FPS:", Engine.get_frames_per_second(), "Limit:", Engine.max_fps)

	var endRunVar = Main_DecodeLevel.MBrun(inGameLoop)
	var warpMouse = Main_DecodeLevel.MBgetWarpMouse()
	if(warpMouse["is"]):
		if get_window().has_focus():
			var screen_size = get_viewport().get_visible_rect().size
			if(warpMouse["yRevert"]):
				if(Global.inverse_mouseY):
					warpMouse["y"]=warpMouse["y"]
				else:
					warpMouse["y"]=Main_DecodeLevel.SCREEN_HEIGHT-warpMouse["y"]
			var target_x = (warpMouse["x"] / float(Main_DecodeLevel.SCREEN_WIDTH)) * screen_size.x
			var target_y = (warpMouse["y"] / float(Main_DecodeLevel.SCREEN_HEIGHT)) * screen_size.y
			Input.warp_mouse(Vector2(target_x, target_y))
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
			inGameLoop=false
			Engine.max_fps = 30
		4:
			beginAnimation(4)
			inGameLoop=false
			Engine.max_fps = 30
		5:
			beginInGame()
			inGameLoop=true
			Engine.max_fps = Global.max_fps
		6:
			beginAnimation(0)
			inGameLoop=false
			Engine.max_fps = 30
		15:
			beginAnimation(5)
			inGameLoop=false
			Engine.max_fps = 30
		16:
			beginAnimation(6)
			inGameLoop=false
			Engine.max_fps = 30
		17:
			beginAnimation(7)
			inGameLoop=false
			Engine.max_fps = 30
		18:
			beginAnimation(8)
			inGameLoop=false
			Engine.max_fps = 30
		19:
			beginAnimation(9)
			inGameLoop=false
			Engine.max_fps = 30
	if Global.show_navigation && inGameLoop && warpMouse["yRevert"]:
		Main_DecodeLevel.showNavigation(true)
	else:
		Main_DecodeLevel.showNavigation(false)
	#if(inGameLoop):
		#Main_Filter.show()
	#else:
		#Main_Filter.hide()
	
	#else:
		#animImage.set_data(animWidth, animHeight, false, Image.FORMAT_RGB8, Main_DecodeLevel.getVGABuffer())
		#animTextureRect.update(animImage)
	
