extends VBoxContainer

func display_stages_data(idx: int):
	$IDX/SpinBox.value = idx
	if idx >= 11:
		push_error("Stage s indexem %d není v Global.stages_settings načten!" % idx)
		return
	$StageIndex/SpinBox.value = Global.editorLevel["stages"][idx]["index"]
	$StageStage/SpinBox.value = Global.editorLevel["stages"][idx]["stage"]
	$StageX1/SpinBox.value = Global.editorLevel["stages"][idx]["axis_x"]
	$StageY1/SpinBox.value = Global.editorLevel["stages"][idx]["axis_y"]
