@tool
extends Node3D

@export var beginY: float = -2.0  # Výška, kde objekt začne (pod zemí)
@export var endY: float = 0.0    # Cílová výška (na zemi)
@export var duration: float = 1.5 # Jak dlouho to potrvá

@export var spustit_animaci: bool = false:
	set(value):
		if value == true: # Když v editoru klikneš na checkbox
			run_animation()
		spustit_animaci = false # Hned se to zase odškrtne

func _ready():
	if not Engine.is_editor_hint():
		run_animation()

func run_animation():
	var moving_node = get_node_or_null("Moving")
	if not moving_node:
		return
		
	moving_node.position.y = beginY
	
	var tween = create_tween()
	tween.set_trans(Tween.TRANS_CUBIC).set_ease(Tween.EASE_OUT)
	tween.tween_property(moving_node, "position:y", endY, duration)
