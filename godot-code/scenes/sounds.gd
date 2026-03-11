extends Node3D

var MainMusic:MidiPlayer
var MainMusicHi:AudioStreamPlayer

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
	status.resize(Global.sfx_players.size())	
	for i in range(Global.sfx_players.size()):
		status[i] = Global.sfx_players[i].playing
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
					Global.music_map[music_idx] = full_path
					print("Midi track readed: ", music_idx)
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
					Global.music_hi_map[music_idx] = full_path
					print("Ogg track readed: ", music_idx)
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
					if not Global.sounds_map.has(pack_idx):
						Global.sounds_map[pack_idx] = {}
					Global.sounds_map[pack_idx][sound_idx] = stream
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
			"SOUND_end_sample":
				play_sound_end(soundBank, p1)
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
			"SOUND_set_sample_volume_panning":
				set_sound_panning(p1, p2)
				matchok=true
			"SetSamplePosition":
				set_sample_position(p1, p2, p3)
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


const MAX_SIMULTANEOUS_SOUNDS := 30

func init() -> void:
	for i in MAX_SIMULTANEOUS_SOUNDS:
		var player := AudioStreamPlayer2D.new()
		player.name = "SFXPlayer" + str(i)
		player.bus = "SFX"
		add_child(player)
		Global.sfx_players.append(player)
		
func stopAllSounds():
	stop_music()
	for i in MAX_SIMULTANEOUS_SOUNDS:
		stop_sound(i)

func play_sound(pack_idx: int, player_index: int, sound_idx: int):
	if player_index < 0 or player_index >= MAX_SIMULTANEOUS_SOUNDS:
		push_warning("Neplatný channel %d! Musí být 0-%d." % [player_index, MAX_SIMULTANEOUS_SOUNDS - 1])
		return
	if not Global.sounds_map.has(pack_idx):
		push_warning("Pack %d neexistuje!" % pack_idx)
		return
	if not Global.sounds_map[pack_idx].has(sound_idx):
		push_warning("Zvuk index %d v packu %d neexistuje!" % [sound_idx, pack_idx])
		return
	#if(sfx_players[player_index].playing):
		#return
	var stream: AudioStream = Global.sounds_map[pack_idx][sound_idx]
	var player = Global.sfx_players[player_index]
	if player.playing:
		player.stop()
	player.stream = stream
	player.play()
	print("Playing stream:%d:%d" % [player_index, sound_idx])
	
func play_sound_end(pack_idx: int, player_index: int):
	if player_index >= 0 and player_index < MAX_SIMULTANEOUS_SOUNDS:
		var player = Global.sfx_players[player_index]
		if player.playing:
			player.stop()
			print("Stopping stream on channel: %d" % player_index)

func stop_sound(index: int) -> void:
	if index >= 0 and index < MAX_SIMULTANEOUS_SOUNDS:
		Global.sfx_players[index].stop()

func set_sound_volume(index: int, volume_int: int) -> void:
	if index >= 0 and index < MAX_SIMULTANEOUS_SOUNDS:
		Global.sfx_players[index].volume_linear = float(volume_int) / 128.0

func set_sound_panning(index: int, pan_int: int) -> void:
	if index >= 0 and index < Global.sfx_players.size():
		var player = Global.sfx_players[index]
		var pan_x : float = (float(pan_int) - 64.0) * 10.0
		if player is AudioStreamPlayer2D:
			player.position.x = pan_x
			player.attenuation = 0.0001

func set_sample_position(index: int, angle: int, distance: int) -> void:
	if index < 0 or index >= Global.sfx_players.size():
		return
	var player := Global.sfx_players[index]
	if not (player is AudioStreamPlayer2D):
		return
	var rad := deg_to_rad(float(angle) - 90.0)
	var max_dist: float = 400.0
	var norm_dist: float = clamp(float(distance) / 255.0, 0.0, 1.0)
	var dist: float = norm_dist * max_dist
	player.position = Vector2(
		cos(rad) * dist,
		sin(rad) * dist
	)
	player.attenuation = 0.001

func set_music_volume(volume_int: int) -> void:
	if(Global.himusic):
		MainMusicHi.volume_db = linear_to_db(float(volume_int) / 128.0)
	else:
		MainMusic.volume_db = linear_to_db(float(volume_int) / 128.0)
			
func stop_music() -> void:
	if(Global.himusic):
		if MainMusicHi.playing:
			MainMusicHi.stop()
	else:
		if MainMusic.playing:
			MainMusic.stop()

func start_music(index: int) -> void:
	if(Global.himusic):
		if MainMusicHi.playing:
			MainMusicHi.stop()
		MainMusicHi.stream = Global.load_external_audio(Global.music_hi_map[index])
		MainMusicHi.play()
	else:
		if MainMusic.playing:
			MainMusic.stop()
		MainMusic.file = Global.music_map[index]
		MainMusic.play()
