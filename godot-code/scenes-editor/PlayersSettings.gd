extends VBoxContainer

# Index hráče, kterého chceme zobrazit (0-7)
@export var player_index: int = 0

func display_player_data(idx: int):
	# Kontrola, zda pole obsahuje data pro daný index
	if idx >= 8:
		push_error("Hráč s indexem %d není v Global.players_settings načten!" % idx)
		return

	$Aggression/SpinBox.value = Global.editorLevel["wizards"][idx]["aggression"]
	$Reflexes/SpinBox.value   = Global.editorLevel["wizards"][idx]["reflexes"]
	$Perception/SpinBox.value = Global.editorLevel["wizards"][idx]["perception"]
	$Life/SpinBox.value       = Global.editorLevel["wizards"][idx]["life"]
	
	$StageIndex/SpinBox.value = Global.editorLevel["stages"][idx]["index"]
	$StageStage/SpinBox.value = Global.editorLevel["stages"][idx]["stage"]
	$StageX/SpinBox.value     = Global.editorLevel["stages"][idx]["axis_x"]
	$StageY/SpinBox.value     = Global.editorLevel["stages"][idx]["axis_y"]

	# 2. Kouzla (TextureButtony v panelech)
	# Použijeme tvé proměnné ze třídy WizardSettings
	_update_spell_buttons($StartingSpells, Global.editorLevel["wizards"][idx]["starting_spells"])
	_update_spell_buttons($AvailableSpells, Global.editorLevel["wizards"][idx]["byte_array"])
	_update_spell_buttons($BlockedSpells, Global.editorLevel["wizards"][idx]["blocked_spells"])

# Pomocná funkce pro iteraci přes kontejnery
func _update_spell_buttons(container: Control, spell_array: Array):
	if not container: return	
	var panels = container.get_children()	
	for i in range(min(panels.size(), spell_array.size())):
		# Předpokládáme strukturu: Panel -> TextureButton
		var btn = panels[i].get_child(0) as TextureButton		
		if btn:
			btn.toggle_mode = true
			# Pokud je hodnota v poli > 0, tlačítko se zapne
			btn.button_pressed = (spell_array[i] > 0)


func _on_spin_box_value_changed(value: float) -> void:
	display_player_data(value-1)
