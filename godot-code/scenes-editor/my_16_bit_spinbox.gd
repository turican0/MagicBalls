extends HBoxContainer

@onready var slider = $HSlider
@onready var spin_box = $SpinBox

func _ready():
	# Nastavení stejných limitů
	for node in [slider, spin_box]:
		node.min_value = 0
		node.max_value = 65535
	
	# Propojení signálů
	slider.value_changed.connect(_on_slider_changed)
	spin_box.value_changed.connect(_on_spin_box_changed)

func _on_slider_changed(value):
	if spin_box.value != value:
		spin_box.value = value

func _on_spin_box_changed(value):
	if slider.value != value:
		slider.value = value
