extends VBoxContainer

# Index hráče, kterého chceme zobrazit (0-7)
@export var player_index: int = 0

func _ready():
	# Zavoláme zobrazení dat hned při startu
	display_player_data(player_index)

func display_player_data(idx: int):
	# Kontrola, zda pole obsahuje data pro daný index
	if idx >= Global.players_settings.size():
		push_error("Hráč s indexem %d není v Global.players_settings načten!" % idx)
		return

	# Přímý přístup k objektu ve tvém poli
	var data = Global.players_settings[idx]

	# 1. Číselné hodnoty (SpinBoxy)
	$Aggression/SpinBox.value = data.aggression
	$Reflexes/SpinBox.value   = data.reflexes
	$Perception/SpinBox.value = data.perception
	$Life/SpinBox.value       = data.life

	# 2. Kouzla (TextureButtony v panelech)
	# Použijeme tvé proměnné ze třídy WizardSettings
	_update_spell_buttons($StartingSpells, data.starting_spells)
	_update_spell_buttons($AvailableSpells, data.unknown_bytes)
	_update_spell_buttons($BlockedSpells, data.blocked_spells)

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
