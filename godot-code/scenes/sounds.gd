extends Node3D

var MainMusic:MidiPlayer
var MainMusicHi:AudioStreamPlayer

var sounds_map = {}
var music_map = {}
var music_hi_map = {}

var himusic = true

#func _load_wav_as_sample(file_path: String) -> AudioStream:
	#var file = FileAccess.open(file_path, FileAccess.READ)
	#if file == null:
		#push_error("Nelze otevřít soubor pro čtení: " + file_path)
		#return null
	#var bytes = file.get_buffer(file.get_length())
	#file.close()
	#var stream = AudioStreamWAV.new()
	#stream.format = AudioStreamWAV.FORMAT_8_BITS#.FORMAT_16_BITS  # nebo podle tvých WAV
	#stream.stereo = false  # nebo false pro mono
	#stream.mix_rate = 22050  # uprav podle tvých souborů
	#stream.data = bytes
	#return stream
	
func get_free_player_indices() -> Array:
	var status = []
	status.resize(sfx_players.size())	
	for i in range(sfx_players.size()):
		status[i] = sfx_players[i].playing
	return status
	
func load_musics_from_dir(path: String):
	var dir = DirAccess.open(path)
	if dir:
		dir.list_dir_begin()
		var file_name = dir.get_next()
		
		while file_name != "":
			if !dir.current_is_dir() and file_name.ends_with(".mid"):
				var parts = file_name.split("_")
				if parts.size() >= 2:
					var music_idx = parts[0].to_int()
					var full_path = path + file_name
					music_map[music_idx] = full_path
					print("Hudba nacetna: ", music_idx)
			file_name = dir.get_next()
	else:
		print("Chyba: Adresář nebyl nalezen.")
		
func load_musics_hi_from_dir(path: String):
	var dir = DirAccess.open(path)
	if dir:
		dir.list_dir_begin()
		var file_name = dir.get_next()
		
		while file_name != "":
			if !dir.current_is_dir() and file_name.ends_with(".ogg"):
				var parts = file_name.split("_")
				if parts.size() >= 2:
					var music_idx = parts[0].to_int()
					var full_path = path + file_name
					music_hi_map[music_idx] = full_path
					print("Hudba nacetna: ", music_idx)
			file_name = dir.get_next()
	else:
		print("Chyba: Adresář nebyl nalezen.")

func load_sounds_from_dir(path: String):
	var dir = DirAccess.open(path)
	if dir:
		dir.list_dir_begin()
		var file_name = dir.get_next()
		
		while file_name != "":
			if !dir.current_is_dir() and file_name.ends_with(".WAV"):
				var parts = file_name.split("_")
				if parts.size() >= 2:
					var pack_idx = parts[0].to_int()
					var sound_idx = parts[1].to_int()
					#var full_path = path + file_name
					#var stream = _load_wav_as_sample(full_path)  
					#var stream = ResourceLoader.load(full_path, "AudioStream", ResourceLoader.CACHE_MODE_REUSE)
					var stream = Global.load_external_audio(path + file_name)
					if not sounds_map.has(pack_idx):
						sounds_map[pack_idx] = {}
					sounds_map[pack_idx][sound_idx] = stream
					print("Načten zvuk: Pack ", pack_idx, " Index ", sound_idx)			
			file_name = dir.get_next()
	else:
		print("Chyba: Adresář nebyl nalezen.")	

func updateSounds(soundActions:Array):
	for action_dict in soundActions:
		if not action_dict is Dictionary:
			continue
		var action: String = action_dict.get("action", "")
		var p1: int = action_dict.get("p1", 0)
		var p2: int = action_dict.get("p2", 0)
		var p3: int = action_dict.get("p3", 0)
		var matchok=false
		match action:
			"SOUND_start_sample":
				play_sound(soundBank, p1, p2)
				set_sound_volume(p1, p3)
				matchok=true
			"SOUND_set_sample_volume":
				set_sound_volume(p1, p2)
				matchok=true
			"SOUND_stop_sequence":
				stop_music()
				matchok=true
			"SOUND_start_sequence":
				start_music(p1)
				matchok=true
			"SOUND_set_sequence_volume":
				set_music_volume(p1)
				matchok=true
		if(!matchok):
			matchok=true

var soundBank=0
func setSoundBank(bank):
	soundBank=bank
#	0-DAY
#	1-NIGHT
#	2-CAVE
#	3-VIDEO0
#	4-VIDEO1
#	5-VIDEO2
#	6-VIDEO3
#	7-VIDEO4
#	8-VIDEO5
#	9-VIDEO6


const MAX_SIMULTANEOUS_SOUNDS := 1
var sfx_players: Array[AudioStreamPlayer] = []

func init() -> void:
	for i in MAX_SIMULTANEOUS_SOUNDS:
		var player := AudioStreamPlayer.new()
		player.name = "SFXPlayer" + str(i)
		player.bus = "SFX"
		add_child(player)
		sfx_players.append(player)
		
func stopAllSounds():
	stop_music()
	for i in MAX_SIMULTANEOUS_SOUNDS:
		stop_sound(i)

func play_sound(pack_idx: int, player_index: int, sound_idx: int):
	if player_index < 0 or player_index >= MAX_SIMULTANEOUS_SOUNDS:
		push_warning("Neplatný channel %d! Musí být 0-%d." % [player_index, MAX_SIMULTANEOUS_SOUNDS - 1])
		return
	if not sounds_map.has(pack_idx):
		push_warning("Pack %d neexistuje!" % pack_idx)
		return
	if not sounds_map[pack_idx].has(sound_idx):
		push_warning("Zvuk index %d v packu %d neexistuje!" % [sound_idx, pack_idx])
		return
	#if(sfx_players[player_index].playing):
		#return
	var stream: AudioStream = sounds_map[pack_idx][sound_idx]
	var player = sfx_players[player_index]
	if player.playing:
		player.stop()
	player.stream = stream
	player.play()
	print("Playing stream:%d:%d" % [player_index, sound_idx])

func stop_sound(index: int) -> void:
	if index >= 0 and index < MAX_SIMULTANEOUS_SOUNDS:
		sfx_players[index].stop()

func set_sound_volume(index: int, volume_int: int) -> void:
	if index >= 0 and index < MAX_SIMULTANEOUS_SOUNDS:
		sfx_players[index].volume_linear = float(volume_int) / 128.0

func set_music_volume(volume_int: int) -> void:
	if(himusic):
		MainMusicHi.volume_db = linear_to_db(float(volume_int) / 128.0)
	else:
		MainMusic.volume_db = linear_to_db(float(volume_int) / 128.0)
			
func stop_music() -> void:
	if(himusic):
		if MainMusicHi.playing:
			MainMusicHi.stop()
	else:
		if MainMusic.playing:
			MainMusic.stop()

func start_music(index: int) -> void:
	if(himusic):
		if MainMusicHi.playing:
			MainMusicHi.stop()
		MainMusicHi.stream = Global.load_external_audio(music_hi_map[index])
		MainMusicHi.play()
	else:
		if MainMusic.playing:
			MainMusic.stop()
		MainMusic.file = music_map[index]
		MainMusic.play()
