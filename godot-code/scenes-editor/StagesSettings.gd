extends VBoxContainer

# Index hráče, kterého chceme zobrazit (0-7)
@export var player_index: int = 0

func display_stages_data(idx: int):
	# Kontrola, zda pole obsahuje data pro daný index
	if idx >= 11:
		push_error("Stage s indexem %d není v Global.stages_settings načten!" % idx)
		return

	# Přímý přístup k objektu ve tvém poli
	#var data = Global.stages_settings[idx]

	# 1. Číselné hodnoty (SpinBoxy)
	$StageIndex/SpinBox.value = Global.editorLevel["stage_vars"][idx]["index"]
	$StageStage/SpinBox.value = Global.editorLevel["stage_vars"][idx]["stage"]
	$StageX1/SpinBox.value = Global.editorLevel["stage_vars"][idx]["union_axis_2d_x"]
	$StageY1/SpinBox.value = Global.editorLevel["stage_vars"][idx]["union_axis_2d_y"]
	$StageX2/SpinBox.value = Global.editorLevel["stage_vars"][idx]["union_dword_axis_x"]
	$StageY2/SpinBox.value = Global.editorLevel["stage_vars"][idx]["union_dword_axis_y"]
	
func _on_spin_box_value_changed(value: float) -> void:
	display_stages_data(value-1)
