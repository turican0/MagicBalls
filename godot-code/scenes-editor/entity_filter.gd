extends Control

@onready var SpinBoxType: SpinBox = $PreContainer/PanelContainer/MarginContainer/VBoxContainer/type_0x30311/SpinBoxType

func _ready() -> void:
	Global.editorFilteres.clear()

func _on_spin_box_x_value_changed(value: float) -> void:
	pass # Replace with function body.


			#current_node.set_meta("type_0x30311",int(type_0x30311))
			#current_node.set_meta("subtype_0x30311",int(subtype_0x30311))
			#current_node.set_meta("DisId",int(DisId))
			#current_node.set_meta("word_10",int(word_10))
			#current_node.set_meta("stageTag_12",int(stageTag_12))
			#current_node.set_meta("par1_14",int(par1_14))
			#current_node.set_meta("par2_16",int(par2_16))
			#current_node.set_meta("par3_18",int(par3_18))

func _on_spin_box_type_value_changed(value: float) -> void:
	pass # Replace with function body.


func _on_check_box_type_toggled(toggled_on: bool) -> void:
	if(toggled_on):
		Global.add_editor_filter("type_0x30311",int(SpinBoxType.value))
	else:
		Global.remove_editor_filter("type_0x30311")


func _on_spin_box_sub_type_value_changed(value: float) -> void:
	pass # Replace with function body.


func _on_check_box_sub_type_toggled(toggled_on: bool) -> void:
	pass # Replace with function body.


func _on_spin_box_dis_id_value_changed(value: float) -> void:
	pass # Replace with function body.


func _on_check_box_dis_id_toggled(toggled_on: bool) -> void:
	pass # Replace with function body.


func _on_spin_box_word_value_changed(value: float) -> void:
	pass # Replace with function body.


func _on_check_box_word_toggled(toggled_on: bool) -> void:
	pass # Replace with function body.


func _on_spin_box_tag_value_changed(value: float) -> void:
	pass # Replace with function body.


func _on_check_box_tag_toggled(toggled_on: bool) -> void:
	pass # Replace with function body.


func _on_spin_box_par_1_value_changed(value: float) -> void:
	pass # Replace with function body.


func _on_check_box_par_1_toggled(toggled_on: bool) -> void:
	pass # Replace with function body.


func _on_spin_box_par_2_value_changed(value: float) -> void:
	pass # Replace with function body.


func _on_check_box_par_2_toggled(toggled_on: bool) -> void:
	pass # Replace with function body.


func _on_spin_box_par_3_value_changed(value: float) -> void:
	pass # Replace with function body.


func _on_check_box_par_3_toggled(toggled_on: bool) -> void:
	pass # Replace with function body.
