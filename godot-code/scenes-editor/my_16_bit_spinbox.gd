extends HBoxContainer

# Signal for the rest of your app
signal value_changed(new_value: int)

@onready var label = $Label
@onready var slider = $HSlider
@onready var spin_box = $SpinBox

var entity_index: int = 0
var entity_name: String = ""

# This variable holds the state. 
# Using a setter allows you to update the UI by simply calling: 
# your_component.current_value = 500
@export var current_value: int = 0:
	set(v):
		current_value = clampi(v, 0, 65535)
		_update_ui()

func _ready():
	# Parse name
	var full_name = str(name)
	var parts = full_name.split("_")	
	if parts.size() >= 2:
		entity_name = parts[0]
		entity_index = parts[1].to_int()
	else:
		entity_name = full_name
		entity_index = 0	
	# Set label text
	label.text = entity_name	
	# Configure limits
	for node in [slider, spin_box]:
		node.min_value = 0
		node.max_value = 65535
		node.step = 1	
	# Connect internal signals
	slider.value_changed.connect(_on_ui_value_changed)
	spin_box.value_changed.connect(_on_ui_value_changed)	
	# Set initial state
	_update_ui()

# One function to handle both UI elements
func _on_ui_value_changed(value: float):
	var new_val = int(value)
	if current_value != new_val:
		current_value = new_val
		value_changed.emit(entity_index, current_value)

# Keeps UI elements in sync with the current_value variable
func _update_ui():
	# Basic check to prevent errors before @onready nodes are ready
	if not is_inside_tree(): return
	
	if slider.value != current_value:
		slider.value = current_value
	if spin_box.value != current_value:
		spin_box.value = current_value
