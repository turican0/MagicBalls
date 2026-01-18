extends Node3D

var sounds_map = {}

func _load_wav_as_sample(file_path: String) -> AudioStream:
	var file = FileAccess.open(file_path, FileAccess.READ)
	if file == null:
		push_error("Nelze otevřít soubor pro čtení: " + file_path)
		return null
	var bytes = file.get_buffer(file.get_length())
	file.close()
	var stream = AudioStreamWAV.new()
	stream.format = AudioStreamWAV.FORMAT_8_BITS#.FORMAT_16_BITS  # nebo podle tvých WAV
	stream.stereo = false  # nebo false pro mono
	stream.mix_rate = 22050  # uprav podle tvých souborů
	stream.data = bytes

	return stream

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
					var full_path = path + file_name
					var stream = _load_wav_as_sample(full_path)  
					#var stream = ResourceLoader.load(full_path, "AudioStream", ResourceLoader.CACHE_MODE_REUSE)
					#var stream = load(path + file_name)
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

		match action:
			"SOUND_start_sample":
				play_sound(0, 0, 127, 0)

const MAX_SIMULTANEOUS_SOUNDS := 10
var sfx_players: Array[AudioStreamPlayer] = []

func init() -> void:
	for i in MAX_SIMULTANEOUS_SOUNDS:
		var player := AudioStreamPlayer.new()
		player.name = "SFXPlayer" + str(i)
		player.bus = "SFX"
		add_child(player)
		sfx_players.append(player)

func playOrigSound(pack: int, index: int):
	if sounds_map.has(pack) and sounds_map[pack].has(index):
		var stream = sounds_map[pack][index]
		var player = AudioStreamPlayer.new()
		add_child(player)
		player.stream = stream
		player.play()
		player.finished.connect(player.queue_free)
		
func play_sound(pack_idx: int, sound_idx: int, volume_db: float = 0.0, pitch: float = 1.0) -> int:
	if not sounds_map.has(pack_idx):
		push_warning("Pack %d neexistuje!" % pack_idx)
		return -1
	if not sounds_map[pack_idx].has(sound_idx):
		push_warning("Zvuk index %d v packu %d neexistuje!" % [sound_idx, pack_idx])
		return -1
	var stream: AudioStream = sounds_map[pack_idx][sound_idx]
	var free_index := -1
	for i in MAX_SIMULTANEOUS_SOUNDS:
		if not sfx_players[i].playing:
			free_index = i
			break
	if free_index == -1:
		free_index = 0
	var player: AudioStreamPlayer = sfx_players[free_index]
	player.stream = stream
	player.volume_db = volume_db
	player.pitch_scale = pitch
	player.play()
	return free_index

func stop_sound(index: int) -> void:
	if index >= 0 and index < MAX_SIMULTANEOUS_SOUNDS:
		sfx_players[index].stop()

func set_volume(index: int, volume_db: float) -> void:
	if index >= 0 and index < MAX_SIMULTANEOUS_SOUNDS:
		sfx_players[index].volume_db = volume_db

func set_pitch(index: int, pitch: float) -> void:
	if index >= 0 and index < MAX_SIMULTANEOUS_SOUNDS:
		sfx_players[index].pitch_scale = pitch
