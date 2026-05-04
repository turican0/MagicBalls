extends Node

var last_scene_path: String = ""
var MBEX = null

var LevelType: String = ""

var master_volume:float
var music_volume:float
var	sounds_volume:float
var speech_volume:float
var max_fps:int

var inverse_mouseY = true

var level_mode:int = 0
var custom_level:int = 0

var soundInited:bool = false
var sounds_map = {}
var music_map = {}
var speech_map = {}
var music_hi_map = {}
var himusic = true
var sfx_players: Array[AudioStreamPlayer2D] = []

var Main_Sounds: Node
var Main_Music

var cdPath:String="user://CDdata/"
var convertdata:String="user://convertdata/"
var hidata:String="res://hidata/"

var langTexts:Dictionary

var defaultLangIndex = 1
var countLang = 0

var canNotification=true;

var target_level: String = ""

var editorLevel: Dictionary

func getLevelType():
	return LevelType

func setLevelType(levelType):
	LevelType=levelType

func initSound():
	if(soundInited):
		return
	var master_bus_idx = AudioServer.get_bus_index("Master")
	AudioServer.set_bus_volume_db(master_bus_idx, linear_to_db(master_volume))	
	if(!get_tree().root.get_node_or_null("Sounds")):
		var sounds_scene = preload("res://scenes/Sounds.tscn")
		var new_layer = sounds_scene.instantiate()
		get_tree().root.add_child(new_layer)
		new_layer.name = "Sounds"
		Main_Sounds=get_tree().root.get_node_or_null("Sounds")
	Main_Sounds.MainMusic = Main_Sounds.get_node("MidiPlayer")
	Main_Sounds.MainMusicHi = Main_Sounds.get_node("AudioStreamPlayer")
	Main_Sounds.MainMusicHi2 = Main_Sounds.get_node("AudioStreamPlayer2")
	Main_Sounds.MainSpeech = Main_Sounds.get_node("AudioStreamPlayerSpeech")
	
	Main_Sounds.load_sounds_from_dir(Global.convertdata+"sounds/")
	Main_Sounds.load_musics_from_dir(Global.convertdata+"musics/")
	Main_Sounds.load_musics_hi_from_dir(Global.hidata+"musics/")
	Main_Sounds.load_speech_from_dir(Global.convertdata+"speech/")
	Main_Sounds.init()
	Main_Sounds.setSoundBank(1)#Night
	Global.soundInited = true

var loadScreenNodeRect:TextureRect;
#func loadScreenInit():
	#if(!get_tree().root.get_node_or_null("LoadScreen")):
		#var fade_layer_scene = preload("res://scenes/LoadScreen.tscn")
		#var new_layer = fade_layer_scene.instantiate()
		#new_layer.name = "LoadScreen"
		#new_layer.z_index=99
		#get_tree().root.add_child(new_layer)
		#loadScreenNodeRect = new_layer.get_node_or_null("TextureRect");
		#var fade = get_tree().root.get_node_or_null("FadeInOut")
		#if fade:
			#get_tree().root.move_child(new_layer, fade.get_index())
	#return get_tree().root.get_node_or_null("LoadScreen")
	
#func loadScreenInit():
	#if !get_tree().root.get_node_or_null("LoadScreenCanvas"):
		#var fade_layer_scene = preload("res://scenes/LoadScreen.tscn")
		#var new_layer = fade_layer_scene.instantiate()
		#new_layer.name = "LoadScreen"
		#new_layer.mouse_filter = Control.MOUSE_FILTER_IGNORE
		#var canvas = CanvasLayer.new()
		#canvas.layer = 10
		#canvas.name = "LoadScreenCanvas"
		#get_tree().root.add_child(canvas)
		#canvas.add_child(new_layer)
		#loadScreenNodeRect = new_layer.get_node_or_null("TextureRect")
		#loadScreenNodeRect.mouse_filter = Control.MOUSE_FILTER_IGNORE
		#var fade = get_tree().root.get_node_or_null("FadeInOut")
		#if fade:
			#get_tree().root.move_child(canvas, fade.get_index())
	#return get_tree().root.get_node_or_null("LoadScreen")
#
#func setLoadingScreenStr(fileName):
	#if(fileName):
		#setLoadingScreen(null)
	#var path = "user://convertdata/smat/" + fileName
	#if FileAccess.file_exists(path):
		#var img = Image.load_from_file(path)
		#if img:
			#setLoadingScreen(img)
			#return
	#setLoadingScreen(null)

#func setLoadingScreen(buffer):
	#if buffer == null:
		#loadScreenNodeRect.texture = null
	#else:
		#loadScreenNodeRect.texture = ImageTexture.create_from_image(buffer)

#FadeInOut
func fadeInit(fadeNode):
	if(!get_tree().root.get_node_or_null("FadeInOut")):
		var fade_layer_scene = preload("res://scenes/FadeInOut.tscn")
		var new_layer = fade_layer_scene.instantiate()
		fadeNode=new_layer
		get_tree().root.add_child(new_layer)
		new_layer.name = "FadeInOut"
	return get_tree().root.get_node_or_null("FadeInOut")
func addFadeIn(fadeNode):
	fadeNode=fadeInit(fadeNode)
	fadeNode.start_fade(0.3, Color(0, 0, 0, 0),Color(0, 0, 0, 1))
	return fadeNode
func addFadeOut(fadeNode):
	fadeNode=fadeInit(fadeNode)
	fadeNode.start_fade(0.3, Color(0, 0, 0, 1),Color(0, 0, 0, 0))
	return fadeNode

#Imports
func load_custom_texture(path: String) -> ImageTexture:
	if(path=='user://convertdata/HSCREEN/4/114.png'):
		pass
	var img = Image.load_from_file(path)
	if img == null:
		return null
	var tex = ImageTexture.create_from_image(img)
	return tex

func load_external_audio(file_path: String) -> AudioStream:
	if not FileAccess.file_exists(file_path):
		print("Error: File not found at ", file_path)
		return null
	var file = FileAccess.open(file_path, FileAccess.READ)
	var buffer = file.get_buffer(file.get_length())
	var ext = file_path.get_extension().to_lower()
	match ext:
		"mp3":
			var stream = AudioStreamMP3.new()
			stream.data = buffer
			return stream			
		"ogg":
			return AudioStreamOggVorbis.load_from_buffer(buffer)			
		"wav":
			return _parse_wav(buffer)
		_:
			print("Error: Unsupported audio format: ", ext)
			return null

func _parse_wav(buffer: PackedByteArray) -> AudioStreamWAV:
	if buffer.size() < 44:
		print("Error: Buffer too small for WAV")
		return null
	if buffer.slice(0, 4).get_string_from_ascii() != "RIFF" or buffer.slice(8, 12).get_string_from_ascii() != "WAVE":
		print("Error: Not a valid WAV file")
		return null	
	var stream = AudioStreamWAV.new()
	var channels = buffer.decode_u16(22)
	stream.stereo = (channels == 2)
	var sample_rate = buffer.decode_u32(24)
	stream.mix_rate = sample_rate
	var bits_per_sample = buffer.decode_u16(34)
	if bits_per_sample == 8:
		stream.format = AudioStreamWAV.FORMAT_8_BITS
	elif bits_per_sample == 16:
		stream.format = AudioStreamWAV.FORMAT_16_BITS
	else:
		print("Error: Unsupported WAV bit depth: ", bits_per_sample)
		return null
	var pos = 12
	var data_found = false	
	while pos < buffer.size() - 8:
		var chunk_id = buffer.slice(pos, pos + 4).get_string_from_ascii()
		var chunk_size = buffer.decode_u32(pos + 4)
		if chunk_id == "data":
			var raw_data = buffer.slice(pos + 8, pos + 8 + chunk_size)
			if bits_per_sample == 8:
				for i in range(raw_data.size()):
					var val = raw_data[i]
					raw_data[i] = (val + 128) % 256
			stream.data = raw_data
			data_found = true
			break
		pos += 8 + chunk_size
	if not data_found:
		print("Error: Could not find 'data' chunk in WAV")
		return null
	return stream
	
func _input(event):
	if event is InputEventMouseButton and event.pressed:
		var c = get_viewport().gui_get_hovered_control()
		while c:
			print("GUI chain: ", c.get_path())
			c = c.get_parent()
			
class WizardSettings:
	var aggression: int
	var reflexes: int
	var perception: int
	var starting_spells: Array = []
	var unknown_bytes: Array = []
	var blocked_spells: Array = []
	var life: int
	
	var stageIndex: int
	var stageStage: int
	var stageX: int
	var stageY: int

	func _init():
		starting_spells.resize(26)
		unknown_bytes.resize(26)
		blocked_spells.resize(26)
		
class StagesSettings:
	var index: int
	var stage: int
	var x1: int
	var y1: int
	var x2: int
	var y2: int
		
var editorFilteres: Array[Dictionary] = []

func add_editor_filter(name: String, key: Variant) -> void:
	for i in range(editorFilteres.size()):
		if editorFilteres[i]["filterName"] == name:
			editorFilteres[i]["filterKey"] = key
			return
	editorFilteres.append({
		"filterName": name,
		"filterKey": key
	})

func remove_editor_filter(name: String) -> void:
	for i in range(editorFilteres.size()):
		if editorFilteres[i]["filterName"] == name:
			editorFilteres.remove_at(i)
			return

func clear_editor_filters() -> void:
	editorFilteres.clear()
