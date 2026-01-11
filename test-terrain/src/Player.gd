extends Node3D

@export var bullet_scene: PackedScene

@export var ROTATION_SPEED: float = 0.0
@export var MOVE_SPEED: float = 0.0
@export var LIFE: float = 0.0
@export var MANA: float = 0.0
@export var JUMP_SPEED: float = 2.0

var min_player_height: float = 0.5;
var max_speed:float = 10
var accel_speed:float = 10
var remove_speed:float = 2
var max_rot:float = 10
var accel_rot:float = 1
var remove_rot:float = 0.5




@export var first_person: bool = false : 
	set(p_value):
		first_person = p_value
		if first_person:
			var tween: Tween = create_tween()
			tween.tween_property($CameraManager/Arm, "spring_length", 0.0, .33)
			tween.tween_callback($Body.set_visible.bind(false))
		else:
			#$Body.visible = true
			create_tween().tween_property($CameraManager/Arm, "spring_length", 6.0, .33)

##var speed: float = 0
#var direction2: Vector3 = Vector3(1,0,0)
#
#const KEY_INDEX := {
	#KEY_A: 0, # Left
	#KEY_D: 1, # Right
	#KEY_W: 2, # Forward
	#KEY_S: 3  # Backward
#}
#
#var input_state: Dictionary = {
	#"keys": {},
	#"mouse_pos": Vector2.ZERO
#}
#
#func _physics_process(p_delta) -> void:
	#for keycode: int in KEY_INDEX:
		#var index: int = KEY_INDEX[keycode]
		#input_state["keys"][index] = Input.is_key_pressed(keycode)
	#input_state["mouse_pos"] = get_viewport().get_mouse_position()
	#decode.runGameStep(input_state)
	
	#get_camera_relative_input(p_delta)
	#update_keys(p_delta)

#func update_keys(p_delta) -> void:
	#if(ROTATION_SPEED>remove_rot*p_delta):
		#ROTATION_SPEED-=remove_rot*p_delta
	#else:
		#if(ROTATION_SPEED<-remove_rot*p_delta):
			#ROTATION_SPEED+=remove_rot*p_delta
		#else:
			#ROTATION_SPEED=0	
	#direction2 = direction2.rotated(Vector3.UP, ROTATION_SPEED*p_delta)
	#look_at(global_position + direction2, Vector3.UP)		
	#if(MOVE_SPEED>remove_speed*p_delta):
		#MOVE_SPEED-=remove_speed*p_delta
	#else:
		#if(MOVE_SPEED<-remove_speed*p_delta):
			#MOVE_SPEED+=remove_speed*p_delta
		#else:
			#MOVE_SPEED=0			
	#self.position+=direction2*MOVE_SPEED*p_delta	
	##var terrain_height_y: float = terrain.data.get_height(global_position)
	#var terrain_height_y=1
	#if(terrain_height_y+min_player_height<global_position.y):
		#global_position.y-=1*p_delta
		#if(global_position.y<min_player_height):
			#global_position.y=min_player_height
	#else:
		#if(terrain_height_y+min_player_height>global_position.y):
			#global_position.y+=1*p_delta

# Returns the input vector relative to the camera. Forward is always the direction the camera is facing
func get_camera_relative_input(p_delta):
	#var input_dir: Vector3 = Vector3.ZERO
	if Input.is_key_pressed(KEY_A): # Left
		ROTATION_SPEED=ROTATION_SPEED+accel_rot*p_delta
		if(ROTATION_SPEED>max_rot):
			ROTATION_SPEED=max_rot
	if Input.is_key_pressed(KEY_D): # Right
		ROTATION_SPEED=ROTATION_SPEED-accel_rot*p_delta
		if(ROTATION_SPEED<-max_rot):
			ROTATION_SPEED=-max_rot
	if Input.is_key_pressed(KEY_W): # Forward
		MOVE_SPEED=MOVE_SPEED+accel_speed*p_delta
		if(MOVE_SPEED>max_speed):
			MOVE_SPEED=max_speed
		#input_dir -= %Camera3D.global_transform.basis.z
	if Input.is_key_pressed(KEY_S): # Backward
		MOVE_SPEED=MOVE_SPEED-accel_speed*p_delta
		if(MOVE_SPEED<-max_speed):
			MOVE_SPEED=-max_speed
			
#func shoot_bullet() -> void:
	#if bullet_scene == null:
		#return
	#var bullet: Node3D = bullet_scene.instantiate()
	#bullet.direction = direction2
	#get_tree().current_scene.add_child(bullet)
	#bullet.global_position = global_position + Vector3(0, 1.5, 0) # 1.5 = výška od podlahy
	#

#func _input(p_event: InputEvent) -> void:
	#if p_event is InputEventMouseButton and p_event.button_index == MOUSE_BUTTON_LEFT and p_event.pressed:
		#shoot_bullet()
	#
	#elif p_event is InputEventKey:
		#if p_event.pressed:
			#if p_event.keycode == KEY_V:
				#first_person = ! first_person
			#elif p_event.keycode == KEY_G:
				#gravity_enabled = ! gravity_enabled
			#elif p_event.keycode == KEY_C:
				#collision_enabled = ! collision_enabled
#
		## Else if up/down released
		#elif p_event.keycode in [ KEY_Q, KEY_E, KEY_SPACE ]:
			#velocity.y = 0
			
var spells = [
	{"id": "fireball", "icon": preload("res://assets-2d/fireball64.png")},
	{"id": "ice", "icon": preload("res://assets-2d/fireball64.png")},
	{"id": "heal", "icon": preload("res://assets-2d/fireball64.png")}
]

var active_spell: String = ""

func get_spells():
	return spells

func select_spell(id: String):
	active_spell = id
	print("Selected spell:", id)
