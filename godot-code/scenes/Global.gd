extends Node

var last_scene_path: String = ""
var MBEX = null

var levelType: String = ""

var soundInited:bool = false
var sounds_map = {}
var music_map = {}
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

func initSound():
	if(soundInited):
		return
	if(!get_tree().root.get_node_or_null("Sounds")):
		var sounds_scene = preload("res://scenes/Sounds.tscn")
		var new_layer = sounds_scene.instantiate()
		get_tree().root.add_child(new_layer)
		new_layer.name = "Sounds"
		Main_Sounds=get_tree().root.get_node_or_null("Sounds")
	Main_Sounds.MainMusic = Main_Sounds.get_node("MidiPlayer")
	Main_Sounds.MainMusicHi = Main_Sounds.get_node("AudioStreamPlayer")
	
	Main_Sounds.load_sounds_from_dir(Global.convertdata+"sounds/")
	Main_Sounds.load_musics_from_dir(Global.convertdata+"musics/")
	Main_Sounds.load_musics_hi_from_dir(Global.hidata+"musics/")	
	Main_Sounds.init()
	Main_Sounds.setSoundBank(1)#Night
	Global.soundInited = true

var loadScreenNodeRect:TextureRect;
func loadScreenInit():
	if(!get_tree().root.get_node_or_null("LoadScreen")):
		var fade_layer_scene = preload("res://scenes/LoadScreen.tscn")
		var new_layer = fade_layer_scene.instantiate()
		new_layer.name = "LoadScreen"
		get_tree().root.add_child(new_layer)
		loadScreenNodeRect = new_layer.get_node_or_null("TextureRect");
		var fade = get_tree().root.get_node_or_null("FadeInOut")
		if fade:
			get_tree().root.move_child(new_layer, fade.get_index())
	return get_tree().root.get_node_or_null("LoadScreen")

func setLoadingScreen(buffer):
	if buffer == null:
		pass#loadScreenNodeRect.texture = null
	else:
		loadScreenNodeRect.texture = ImageTexture.create_from_image(buffer)		
	print(loadScreenNodeRect)

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
