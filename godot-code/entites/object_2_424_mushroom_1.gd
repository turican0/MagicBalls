extends Node3D

func _ready():
	randomize()
	# Initial check and setupwwwwwwwwwwwwwwwwwwwwwww
	ensure_single_variant_visible()

func _process(_delta):
	# Continuous check to ensure logic is always met
	ensure_single_variant_visible()

func ensure_single_variant_visible():
	var children = get_children()
	var mushroom_variants = []
	
	# 1. Filter only relevant mushroom nodes (starting with "obj")
	for child in children:
		if child is Node3D and child.name.begins_with("obj"):
			mushroom_variants.append(child)
	
	if mushroom_variants.size() == 0:
		return

	# 2. Identify which mushrooms are currently visible
	var currently_visible = []
	for m in mushroom_variants:
		if m.visible:
			currently_visible.append(m)
			
	# 3. FIXING LOGIC:
	
	# IF MORE THAN ONE: Keep one random, hide the rest
	if currently_visible.size() > 1:
		var keep_index = randi() % currently_visible.size()
		for i in range(currently_visible.size()):
			if i != keep_index:
				currently_visible[i].visible = false
				
	# IF NONE: Pick one at random and show it with random rotation
	elif currently_visible.size() == 0:
		var random_mushroom = mushroom_variants[randi() % mushroom_variants.size()]
		random_mushroom.visible = true
		random_mushroom.rotation_degrees.y = randf_range(0, 360)
