extends VBoxContainer

func display_stages_data(idx: int):
	$IDX/SpinBox.value = idx
	if idx >= 7:
		push_error("Stage s indexem %d není v Global.stages_settings načten!" % idx)
		return

	$StageIndex/SpinBox.value = Global.editorLevel["stage_vars"][idx]["index"]
	$StageStage/SpinBox.value = Global.editorLevel["stage_vars"][idx]["stage"]
	$StageX1/SpinBox.value = Global.editorLevel["stage_vars"][idx]["union_axis_2d_x"]
	$StageY1/SpinBox.value = Global.editorLevel["stage_vars"][idx]["union_axis_2d_y"]
	$StageX2/SpinBox.value = Global.editorLevel["stage_vars"][idx]["union_dword_axis_x"]
	$StageY2/SpinBox.value = Global.editorLevel["stage_vars"][idx]["union_dword_axis_y"]
