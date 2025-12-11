extends CharacterBody3D

@export var bullet_scene: PackedScene

@export var ROTATION_SPEED: float = 0.0
@export var MOVE_SPEED: float = 0.0
@export var JUMP_SPEED: float = 2.0

@export var terrain: Terrain3D

var min_player_height: float = 2;
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

@export var gravity_enabled: bool = true :
	set(p_value):
		gravity_enabled = p_value
		if not gravity_enabled:
			velocity.y = 0
			
@export var collision_enabled: bool = true :
	set(p_value):
		collision_enabled = p_value
		$CollisionShapeBody.disabled = ! collision_enabled
		$CollisionShapeRay.disabled = ! collision_enabled

#var speed: float = 0
var direction2: Vector3 = Vector3(0,0,-1)

func _physics_process(p_delta) -> void:
	get_camera_relative_input(p_delta)
	#var h_input_dir: Vector2 = Vector2(direction.x, direction.z)
	#var h_veloc: Vector2 = Vector2(direction.x, direction.z).normalized() * MOVE_SPEED
	#if Input.is_key_pressed(KEY_SHIFT):
	#	h_veloc *= 2
	#velocity.x = h_veloc.x
	#velocity.z = h_veloc.y
	
	# --- KÓD PRO ROTACI HRÁČE ---
	#if h_input_dir.length_squared() > 0.0:
		#var target_direction: Vector3 = Vector3(h_input_dir.x, 0, h_input_dir.y).normalized()
		#var target_rotation: float = atan2(target_direction.x, target_direction.z)
		#rotation.y = lerp_angle(rotation.y, target_rotation, p_delta * ROTATION_SPEED)
		
	#if gravity_enabled:
	#	velocity.y -= 40 * p_delta
	
	if(ROTATION_SPEED>remove_rot*p_delta):
		ROTATION_SPEED-=remove_rot*p_delta
	else:
		if(ROTATION_SPEED<-remove_rot*p_delta):
			ROTATION_SPEED+=remove_rot*p_delta
		else:
			ROTATION_SPEED=0
	
	direction2 = direction2.rotated(Vector3.UP, ROTATION_SPEED*p_delta)
	look_at(global_position + direction2, Vector3.UP)
		
	if(MOVE_SPEED>remove_speed*p_delta):
		MOVE_SPEED-=remove_speed*p_delta
	else:
		if(MOVE_SPEED<-remove_speed*p_delta):
			MOVE_SPEED+=remove_speed*p_delta
		else:
			MOVE_SPEED=0
			
	self.position+=direction2*MOVE_SPEED*p_delta
	
	var terrain_height_y: float = terrain.data.get_height(global_position)
	if(terrain_height_y+min_player_height<global_position.y):
		global_position.y-=0.1
		if(global_position.y<min_player_height):
			global_position.y=min_player_height
	else:
		if(terrain_height_y+min_player_height>global_position.y):
			global_position.y+=0.1
	#move_and_slide()

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
		#input_dir += %Camera3D.global_transform.basis.z
	#if Input.is_key_pressed(KEY_E) or Input.is_key_pressed(KEY_SPACE): # Up
	#	velocity.y += JUMP_SPEED + MOVE_SPEED*.016
	#if Input.is_key_pressed(KEY_Q): # Down
	#	velocity.y -= JUMP_SPEED + MOVE_SPEED*.016
	#if Input.is_key_pressed(KEY_KP_ADD) or Input.is_key_pressed(KEY_EQUAL):
	#	MOVE_SPEED = clamp(MOVE_SPEED + .5, 5, 9999)
	#if Input.is_key_pressed(KEY_KP_SUBTRACT) or Input.is_key_pressed(KEY_MINUS):
	#	MOVE_SPEED = clamp(MOVE_SPEED - .5, 5, 9999)
	#return input_dir

func shoot_bullet() -> void:
	if bullet_scene == null:
		return
	var bullet: Node3D = bullet_scene.instantiate()
	#var dir: Vector3 = -%Camera3D.global_transform.basis.z
	
	#bullet.direction = dir.normalized()
	#bullet.direction.y = 0.0
	#bullet.direction = bullet.direction.normalized()
	bullet.direction = direction2
	#bullet.global_position = global_position + Vector3(0, 1.5, 0) # 1.5 = výška od podlahy
	get_tree().current_scene.add_child(bullet)
	bullet.global_position = global_position + Vector3(0, 1.5, 0) # 1.5 = výška od podlahy
	

func _input(p_event: InputEvent) -> void:
	if p_event is InputEventMouseButton and p_event.button_index == MOUSE_BUTTON_LEFT and p_event.pressed:
		shoot_bullet()
	#if p_event is InputEventMouseButton and p_event.pressed:
	#	if p_event.button_index == MOUSE_BUTTON_WHEEL_UP:
	#		MOVE_SPEED = clamp(MOVE_SPEED + 5, 5, 9999)
	#	elif p_event.button_index == MOUSE_BUTTON_WHEEL_DOWN:
	#		MOVE_SPEED = clamp(MOVE_SPEED - 5, 5, 9999)
	
	elif p_event is InputEventKey:
		if p_event.pressed:
			if p_event.keycode == KEY_V:
				first_person = ! first_person
			elif p_event.keycode == KEY_G:
				gravity_enabled = ! gravity_enabled
			elif p_event.keycode == KEY_C:
				collision_enabled = ! collision_enabled

		# Else if up/down released
		elif p_event.keycode in [ KEY_Q, KEY_E, KEY_SPACE ]:
			velocity.y = 0
