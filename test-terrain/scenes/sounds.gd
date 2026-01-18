extends Node3D

var sounds_map = {}

func _ready():
	load_sounds_from_dir("res://convertdata/sounds/")

func load_sounds_from_dir(path: String):
	var dir = DirAccess.open(path)
	if dir:
		dir.list_dir_begin()
		var file_name = dir.get_next()
		
		while file_name != "":
			if !dir.current_is_dir() and file_name.ends_with(".wav"):
				var parts = file_name.split("_")
				if parts.size() >= 2:
					var pack_idx = parts[0].to_int()
					var sound_idx = parts[1].to_int()
					var stream = load(path + file_name)
					if not sounds_map.has(pack_idx):
						sounds_map[pack_idx] = {}					
					sounds_map[pack_idx][sound_idx] = stream
					print("Načten zvuk: Pack ", pack_idx, " Index ", sound_idx)			
			file_name = dir.get_next()
	else:
		print("Chyba: Adresář nebyl nalezen.")

func playOrigSound(pack: int, index: int):
	if sounds_map.has(pack) and sounds_map[pack].has(index):
		var stream = sounds_map[pack][index]
		var player = AudioStreamPlayer.new()
		add_child(player)
		player.stream = stream
		player.play()
		player.finished.connect(player.queue_free)
	else:
		print("Zvuk s indexy ", pack, ":", index, " neexistuje.")
