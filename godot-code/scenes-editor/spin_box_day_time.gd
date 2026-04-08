extends SpinBox

signal state_changed_graphicsType(new_state_name: String, index: int)

var states = ["Day", "Night", "Cave"]

func _ready():
	# Standard setup
	min_value = -1 # We set range slightly wider to detect overflow
	max_value = states.size()
	step = 1
	
	value_changed.connect(_on_value_changed)
	update_text()

func _on_value_changed(new_value: float):
	# Handle cyclicity (Wrapping)
	if new_value >= states.size():
		value = 0
		return # Return early, value_changed will fire again with 0
	elif new_value < 0:
		value = states.size() - 1
		return # Return early
	
	# Normal state update
	var index = int(value)
	call_deferred("update_text")
	state_changed_graphicsType.emit(states[index], index)

func update_text():
	var line_edit = get_line_edit()
	# Double check index safety
	var index = int(clamp(value, 0, states.size() - 1))
	line_edit.text = states[index]

func _process(_delta):
	var line_edit = get_line_edit()
	# Prevention of showing -1 or out of bounds numbers
	var safe_index = int(clamp(value, 0, states.size() - 1))
	var current_state = states[safe_index]
	
	if line_edit.text != current_state and not line_edit.has_focus():
		line_edit.text = current_state
