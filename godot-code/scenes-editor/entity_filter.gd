extends Control

@onready var SpinBoxType: SpinBox = $PreContainer/PanelContainer/MarginContainer/VBoxContainer/type_0x30311/SpinBoxType
@onready var CheckBoxType: CheckBox = $PreContainer/PanelContainer/MarginContainer/VBoxContainer/type_0x30311/CheckBoxType
@onready var SpinBoxSubType: SpinBox = $PreContainer/PanelContainer/MarginContainer/VBoxContainer/subtype_0x30311/SpinBoxSubType
@onready var CheckBoxSubType: CheckBox = $PreContainer/PanelContainer/MarginContainer/VBoxContainer/subtype_0x30311/CheckBoxSubType
@onready var SpinBoxDisId: SpinBox = $PreContainer/PanelContainer/MarginContainer/VBoxContainer/DisId/SpinBoxDisId
@onready var CheckBoxDisId: CheckBox = $PreContainer/PanelContainer/MarginContainer/VBoxContainer/DisId/CheckBoxDisId
@onready var SpinBoxWord: SpinBox = $PreContainer/PanelContainer/MarginContainer/VBoxContainer/word_10/SpinBoxWord
@onready var CheckBoxWord: CheckBox = $PreContainer/PanelContainer/MarginContainer/VBoxContainer/word_10/CheckBoxWord
@onready var SpinBoxTag: SpinBox = $PreContainer/PanelContainer/MarginContainer/VBoxContainer/stageTag_12/SpinBoxTag
@onready var CheckBoxTag: CheckBox = $PreContainer/PanelContainer/MarginContainer/VBoxContainer/stageTag_12/CheckBoxTag
@onready var SpinBoxPar1: SpinBox = $PreContainer/PanelContainer/MarginContainer/VBoxContainer/par1_14/SpinBoxPar1
@onready var CheckBoxPar1: CheckBox = $PreContainer/PanelContainer/MarginContainer/VBoxContainer/par1_14/CheckBoxPar1
@onready var SpinBoxPar2: SpinBox = $PreContainer/PanelContainer/MarginContainer/VBoxContainer/par2_16/SpinBoxPar2
@onready var CheckBoxPar2: CheckBox = $PreContainer/PanelContainer/MarginContainer/VBoxContainer/par2_16/CheckBoxPar2
@onready var SpinBoxPar3: SpinBox = $PreContainer/PanelContainer/MarginContainer/VBoxContainer/par3_18/SpinBoxPar3
@onready var CheckBoxPar3: CheckBox = $PreContainer/PanelContainer/MarginContainer/VBoxContainer/par3_18/CheckBoxPar3

func _ready() -> void:
	Global.editorFilteres.clear()

func _on_check_box_type_toggled(toggled_on: bool) -> void:
	if toggled_on:
		Global.add_editor_filter("type_0x30311", int(SpinBoxType.value))
	else:
		Global.remove_editor_filter("type_0x30311")

func _on_check_box_sub_type_toggled(toggled_on: bool) -> void:
	if toggled_on:
		Global.add_editor_filter("subtype_0x30311", int(SpinBoxSubType.value))
	else:
		Global.remove_editor_filter("subtype_0x30311")

func _on_check_box_dis_id_toggled(toggled_on: bool) -> void:
	if toggled_on:
		Global.add_editor_filter("DisId", int(SpinBoxDisId.value))
	else:
		Global.remove_editor_filter("DisId")

func _on_check_box_word_toggled(toggled_on: bool) -> void:
	if toggled_on:
		Global.add_editor_filter("word_10", int(SpinBoxWord.value))
	else:
		Global.remove_editor_filter("word_10")

func _on_check_box_tag_toggled(toggled_on: bool) -> void:
	if toggled_on:
		Global.add_editor_filter("stageTag_12", int(SpinBoxTag.value))
	else:
		Global.remove_editor_filter("stageTag_12")

func _on_check_box_par_1_toggled(toggled_on: bool) -> void:
	if toggled_on:
		Global.add_editor_filter("par1_14", int(SpinBoxPar1.value))
	else:
		Global.remove_editor_filter("par1_14")

func _on_check_box_par_2_toggled(toggled_on: bool) -> void:
	if toggled_on:
		Global.add_editor_filter("par2_16", int(SpinBoxPar2.value))
	else:
		Global.remove_editor_filter("par2_16")

func _on_check_box_par_3_toggled(toggled_on: bool) -> void:
	if toggled_on:
		Global.add_editor_filter("par3_18", int(SpinBoxPar3.value))
	else:
		Global.remove_editor_filter("par3_18")

func _on_spin_box_type_value_changed(value: float) -> void:
	if CheckBoxType.button_pressed:
		Global.remove_editor_filter("type_0x30311")
		Global.add_editor_filter("type_0x30311", int(value))

func _on_spin_box_sub_type_value_changed(value: float) -> void:
	if CheckBoxSubType.button_pressed:
		Global.remove_editor_filter("subtype_0x30311")
		Global.add_editor_filter("subtype_0x30311", int(value))

func _on_spin_box_dis_id_value_changed(value: float) -> void:
	if CheckBoxDisId.button_pressed:
		Global.remove_editor_filter("DisId")
		Global.add_editor_filter("DisId", int(value))

func _on_spin_box_word_value_changed(value: float) -> void:
	if CheckBoxWord.button_pressed:
		Global.remove_editor_filter("word_10")
		Global.add_editor_filter("word_10", int(value))

func _on_spin_box_tag_value_changed(value: float) -> void:
	if CheckBoxTag.button_pressed:
		Global.remove_editor_filter("stageTag_12")
		Global.add_editor_filter("stageTag_12", int(value))

func _on_spin_box_par_1_value_changed(value: float) -> void:
	if CheckBoxPar1.button_pressed:
		Global.remove_editor_filter("par1_14")
		Global.add_editor_filter("par1_14", int(value))

func _on_spin_box_par_2_value_changed(value: float) -> void:
	if CheckBoxPar2.button_pressed:
		Global.remove_editor_filter("par2_16")
		Global.add_editor_filter("par2_16", int(value))

func _on_spin_box_par_3_value_changed(value: float) -> void:
	if CheckBoxPar3.button_pressed:
		Global.remove_editor_filter("par3_18")
		Global.add_editor_filter("par3_18", int(value))
