extends SpinBox

signal state_changed_graphicsType(new_state_name: String, index: int)

var states = ["Day", "Night", "Cave"]

func _ready():
	# Setup range to allow manual wrapping logic
	min_value = -1
	max_value = states.size()
	step = 1
	
	# Access the internal LineEdit
	var line_edit = get_line_edit()
	
	# Disable text editing
	line_edit.editable = false
	line_edit.context_menu_enabled = false # Optional: disable right-click menu
	
	# Optional: make it look like a label (no focus border)
	line_edit.focus_mode = Control.FOCUS_NONE 
	
	value_changed.connect(_on_value_changed)
	update_text()

func _on_value_changed(new_value: float):
	# 1. Handle Cyclicity (Wrapping)
	if new_value >= states.size():
		value = 0
		return
	elif new_value < 0:
		value = states.size() - 1
		return
	
	# 2. Update the label and notify others
	update_text()
	var index = int(value)
	state_changed_graphicsType.emit(states[index], index)

func update_text():
	# We use call_deferred to stay ahead of Godot's internal number formatting
	_force_label_update.call_deferred()

func _force_label_update():
	var line_edit = get_line_edit()
	var safe_index = int(clamp(value, 0, states.size() - 1))
	line_edit.text = states[safe_index]

func _process(_delta):
	# Safety net: keep the text synchronized
	var line_edit = get_line_edit()
	var safe_index = int(clamp(value, 0, states.size() - 1))
	var target_text = states[safe_index]
	
	if line_edit.text != target_text:
		line_edit.text = target_text
