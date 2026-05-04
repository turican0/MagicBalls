extends HBoxContainer

# Simplified signal - only returns the String
signal state_changed_graphicsType(state_name: String)

@export var states: Array[String] = ["Night", "Day", "Cave"]
var current_index: int = 0

@onready var label = $StateLabel
@onready var btn_less = $BtnLess
@onready var btn_more = $BtnMore

func _ready():
	# Connect button signals
	btn_less.pressed.connect(_on_btn_less_pressed)
	btn_more.pressed.connect(_on_btn_more_pressed)
	
	# Set initial UI without emitting signal during initialization
	# (prevents potential bugs during scene loading)
	label.text = states[current_index]

func _on_btn_less_pressed():
	current_index = posmod(current_index - 1, states.size())
	#get_parent().getnodeornull("map_type").get_node("SpinBox").value=current_index
	_update_ui()

func _on_btn_more_pressed():
	current_index = posmod(current_index + 1, states.size())
	_update_ui()

func _update_ui():
	var current_state = states[current_index]
	label.text = current_state
	# Emit only the String value
	state_changed_graphicsType.emit(current_state)

func _gui_input(event):
	if event is InputEventMouseButton and event.pressed:
		if event.button_index == MOUSE_BUTTON_WHEEL_UP:
			_on_btn_more_pressed()
		elif event.button_index == MOUSE_BUTTON_WHEEL_DOWN:
			_on_btn_less_pressed()
