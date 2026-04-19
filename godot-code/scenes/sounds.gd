extends Node3D

var MainMusic:MidiPlayer
var MainMusicHi:AudioStreamPlayer
var MainSpeech:AudioStreamPlayer

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

func load_external_audio(file_path: String) -> AudioStream:
	if FileAccess.file_exists(file_path):
		var stream = AudioStreamOggVorbis.load_from_file(file_path)
		if stream:
			return stream
	return null

func load_speech_from_dir(path: String):
	if not path.ends_with("/"):
		path += "/"
	var dir = DirAccess.open(path)
	if dir:
		dir.list_dir_begin()
		var file_name = dir.get_next()
		while file_name != "":
			if !dir.current_is_dir() and file_name.ends_with(".ogg"):
				var base_name = file_name.get_basename()
				var raw_idx = base_name.substr(1).to_int()
				var final_idx = raw_idx - 1
				var full_path = path + file_name
				var audio_resource = Global.load_external_audio(full_path)
				if audio_resource:
					Global.speech_map[final_idx] = audio_resource
					print("Speech loaded: ", file_name, " as index: ", final_idx)
			file_name = dir.get_next()
		dir.list_dir_end()
	else:
		print("Chyba: Adresář nelze otevřít: ", path)
	
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
		dir.list_dir_end()
	else:
		print("DirAccess selhal!")  # toto se vypíše pokud adresář nejde otevřít
	# Fallback – pokud DirAccess selhal v exportu, načti natvrdo
	if Global.music_hi_map.is_empty():
		print("DirAccess selhal, používám fallback seznam")
		var fallback = [
			"000_C2GAME1.ogg",
			"001_C2GAME2.ogg",
			"002_C2GAME3.ogg",
			"003_C2SETUP.ogg",
			"004_C2INTRO.ogg",
			"005_C2CUTS.ogg"
		]
		for file_name2 in fallback:
			var parts = file_name2.get_basename().split("_")
			if parts.size() >= 2:
				var music_idx = parts[0].to_int()
				Global.music_hi_map[music_idx] = path + file_name2
				print("Fallback loaded: ", music_idx, " -> ", path + file_name2)

func load_sounds_from_dir(path: String):
	var dir = DirAccess.open(path)
	if dir:
		dir.list_dir_begin()
		var file_name = dir.get_next()
		while file_name != "":
			if !dir.current_is_dir() and file_name.to_upper().ends_with(".WAV"):
				if "441" in file_name:
					var parts = file_name.split("_")
					if parts.size() >= 2:
						var pack_idx = parts[0].to_int()
						var sound_idx = parts[1].to_int()
						var stream = Global.load_external_audio(path + file_name)
						if stream:
							if not Global.sounds_map.has(pack_idx):
								Global.sounds_map[pack_idx] = {}
							Global.sounds_map[pack_idx][sound_idx] = stream
							print("Loaded file: ", file_name) # UPDATE: Success log
						else:
							print("Not loaded file (parse error): ", file_name) # UPDATE: Fail log
					else:
						print("Not loaded file (bad name format): ", file_name) # UPDATE: Fail log
				else:
					# Skip files without 441 silently or log them as skipped
					print("Not loaded file (missing 441): ", file_name) # UPDATE: Skip log
			
			file_name = dir.get_next()
		dir.list_dir_end()
	else:
		print("Error: Directory not found: ", path)

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
			"PlayCdTrackSegment":
				start_speech(p1, p2, p3)
				matchok=true
			"EndPlayingCdTrackSegment":
				stop_speech()
				matchok=true
			#sound_queue_add_action("PlayCdTrackSegment", trackIdx, startPosMs, lengthMs);
			#sound_queue_add_action("EndPlayingCdTrackSegment", 0, 0, 0);
			#sound_queue_add_action("ClearCdTrackSegment", 0, 0, 0);
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


const MAX_SIMULTANEOUS_SOUNDS := 10

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
		Global.sfx_players[index].volume_db = linear_to_db(float(volume_int) / 128.0)

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

func start_speech(index: int, startPosMs: int, lengthMs: int) -> void:
	MainSpeech.volume_db = linear_to_db(1.0)
	print("=== START SPEECH DEBUG ===")
	print("Index: ", index)	
	if Global.speech_map.has(index):
		var stream = Global.speech_map[index]
		if stream:
			if stream is AudioStreamOggVorbis:
				stream.loop = false
			elif stream is AudioStreamWAV:
				stream.loop_mode = AudioStreamWAV.LOOP_DISABLED
			MainSpeech.stream = stream
			var start_pos_sec : float = startPosMs / 1000.0
			var length_sec : float = lengthMs / 1000.0
			MainSpeech.play(start_pos_sec)
			print("Playing from: ", start_pos_sec, "s")
			if lengthMs > 0:
				await get_tree().create_timer(length_sec).timeout
				if MainSpeech.stream == stream:
					MainSpeech.stop()
					print("Speech stopped after ", length_sec, "s")
		else:
			print("Error: Stream load failed!")
	else:
		print("Error: Index ", index, " not found in speech_map.")

func stop_speech() -> void:
	if MainSpeech.playing:
		MainSpeech.stop()
		print("Speech manually stopped.")
	else:
		print("Stop requested, but no speech was playing.")

func start_music(index: int) -> void:
	if Global.himusic:
		print("=== START MUSIC DEBUG ===")
		print("Index: ", index)
		print("music_hi_map: ", Global.music_hi_map)
		print("Has index: ", Global.music_hi_map.has(index))

		if Global.music_hi_map.has(index):
			var path = Global.music_hi_map[index]
			print("Path: ", path)
			print("ResourceLoader exists: ", ResourceLoader.exists(path))
			var stream = ResourceLoader.load(path, "AudioStream")
			print("Stream loaded: ", stream)
			if stream is AudioStreamOggVorbis:
				stream.loop = true
			elif stream is AudioStreamWAV:
				stream.loop_mode = AudioStreamWAV.LOOP_FORWARD
			MainMusicHi.stream = stream
			print("Stream set, playing...")
			MainMusicHi.play()
			print("Playing: ", MainMusicHi.playing)
	else:
		# --- MIDI SECTION (MainMusic) ---
		print("=== START MIDI MUSIC ===")
		if Global.music_map.has(index):
			var path = Global.music_map[index]
			# Assuming MainMusic is a MidiPlayer node
			MainMusic.file = path
			# Most Godot MIDI players use 'loop' or 'loop_selection'
			MainMusic.loop = true
			MainMusic.play()
			print("Playing MIDI track: ", path, " (Looping enabled)")
		else:
			print("Error: MIDI track index ", index, " not found in music_map.")
			
func play_at_pcm_byte(byte_offset: int):
	var bytes_per_second = 44100 * 2 * 2
	var target_time = float(byte_offset) / bytes_per_second
	$AudioStreamPlayer.play(target_time)

#war channels - SONG 0 - channels  7,8,9(NIGHT)   |   SONG 1 - channels 8,9(DAY)   |   SONG 2 - channels 8,9(CAVE)
