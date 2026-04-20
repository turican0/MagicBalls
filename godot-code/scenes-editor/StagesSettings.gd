extends VBoxContainer

# Index hráče, kterého chceme zobrazit (0-7)
@export var player_index: int = 0

func display_stages_data(idx: int):
	# Kontrola, zda pole obsahuje data pro daný index
	if idx >= Global.stages_settings.size():
		push_error("Stage s indexem %d není v Global.stages_settings načten!" % idx)
		return

	# Přímý přístup k objektu ve tvém poli
	var data = Global.stages_settings[idx]

	# 1. Číselné hodnoty (SpinBoxy)
	$StageIndex/SpinBox.value = data.index
	$StageStage/SpinBox.value = data.stage
	$StageX1/SpinBox.value = data.x1
	$StageY1/SpinBox.value = data.y1
	$StageX2/SpinBox.value = data.x2
	$StageY2/SpinBox.value = data.y2
	
func _on_spin_box_value_changed(value: float) -> void:
	display_stages_data(value-1)
