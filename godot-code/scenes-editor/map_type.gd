extends HBoxContainer

signal state_changed_graphicsType(state_name: String)

@export var states: Array[String] = ["Day", "Night", "Cave"]

func _on_spin_box_value_changed(value: float) -> void:
	get_node("Label2").text=states[value]
	state_changed_graphicsType.emit(states[value])
