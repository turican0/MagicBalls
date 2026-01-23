extends Node3D

func _ready() -> void:
	#get_tree().reload_current_scene() # Restartuje aktuální scénu - bud

	#OS.set_restart_on_exit(true, ["--argumenty"]) # Volitelné argumenty - a nebo
	#get_tree().quit()
	
	get_tree().paused = true	
	var countdown_label = $Control/Label
	var seconds_left = 5
	while seconds_left > 0:
		countdown_label.text = "Hledám data ve složce:" + str(seconds_left) + "s"
		await get_tree().create_timer(1.0, true).timeout
		seconds_left -= 1
	countdown_label.text = ""
	get_tree().paused = false
	
	get_tree().change_scene_to_file("res://scenes/MainMenu.tscn")
