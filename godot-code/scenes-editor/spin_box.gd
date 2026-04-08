extends SpinBox

func _ready():
	min_value = 0
	max_value = 65535
	allow_greater = false
	allow_lesser = false
	value_changed.connect(_on_value_changed)

func _on_value_changed(new_value):
	if new_value > 65535:
		value = 0
	elif new_value < 0:
		value = 65535
