extends CharacterBody3D

@export var bullet_scene: PackedScene

@export var ROTATION_SPEED: float = 1.0

@export var MOVE_SPEED: float = 50.0
@export var JUMP_SPEED: float = 2.0

@export var terrain: Terrain3D

@export var first_person: bool = false : 
	set(p_value):
		first_person = p_value
		if first_person:
			var tween: Tween = create_tween()
			tween.tween_property($CameraManager/Arm, "spring_length", 0.0, .33)
			tween.tween_callback($Body.set_visible.bind(false))
		else:
			$Body.visible = true
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

var speed: float = 0
var direction2: Vector3 = Vector3(0,0,-1)

func _physics_process(p_delta) -> void:
	var direction: Vector3 = get_camera_relative_input()
	#var h_input_dir: Vector2 = Vector2(direction.x, direction.z)
	var h_veloc: Vector2 = Vector2(direction.x, direction.z).normalized() * MOVE_SPEED
	if Input.is_key_pressed(KEY_SHIFT):
		h_veloc *= 2
	velocity.x = h_veloc.x
	velocity.z = h_veloc.y
	
	# --- KÓD PRO ROTACI HRÁČE ---
	#if h_input_dir.length_squared() > 0.0:
		#var target_direction: Vector3 = Vector3(h_input_dir.x, 0, h_input_dir.y).normalized()
		#var target_rotation: float = atan2(target_direction.x, target_direction.z)
		#rotation.y = lerp_angle(rotation.y, target_rotation, p_delta * ROTATION_SPEED)
		
	if gravity_enabled:
		velocity.y -= 40 * p_delta
	self.position+=direction2*speed*p_delta
	
	var terrain_height_y: float = terrain.data.get_height(global_position)
	if(terrain_height_y+2<global_position.y):
		global_position.y-=0.1
	else:
		if(terrain_height_y+2>global_position.y):
			global_position.y+=0.1
	#move_and_slide()

# Returns the input vector relative to the camera. Forward is always the direction the camera is facing
func get_camera_relative_input() -> Vector3:
	var input_dir: Vector3 = Vector3.ZERO
	if Input.is_key_pressed(KEY_A): # Left
		direction2 = direction2.rotated(Vector3.UP, 0.01)
		look_at(global_position + direction2, Vector3.UP)
		#input_dir -= %Camera3D.global_transform.basis.x
	if Input.is_key_pressed(KEY_D): # Right
		direction2 = direction2.rotated(Vector3.UP, -0.01)
		look_at(global_position + direction2, Vector3.UP)
		#input_dir += %Camera3D.global_transform.basis.x
	if Input.is_key_pressed(KEY_W): # Forward
		speed=speed+0.1
		#input_dir -= %Camera3D.global_transform.basis.z
	if Input.is_key_pressed(KEY_S): # Backward
		speed=speed-0.1
		#input_dir += %Camera3D.global_transform.basis.z
	if Input.is_key_pressed(KEY_E) or Input.is_key_pressed(KEY_SPACE): # Up
		velocity.y += JUMP_SPEED + MOVE_SPEED*.016
	if Input.is_key_pressed(KEY_Q): # Down
		velocity.y -= JUMP_SPEED + MOVE_SPEED*.016
	if Input.is_key_pressed(KEY_KP_ADD) or Input.is_key_pressed(KEY_EQUAL):
		MOVE_SPEED = clamp(MOVE_SPEED + .5, 5, 9999)
	if Input.is_key_pressed(KEY_KP_SUBTRACT) or Input.is_key_pressed(KEY_MINUS):
		MOVE_SPEED = clamp(MOVE_SPEED - .5, 5, 9999)
	return input_dir

func shoot_bullet() -> void:
	if bullet_scene == null:
		return
	var bullet: Node3D = bullet_scene.instantiate()
	var dir: Vector3 = -%Camera3D.global_transform.basis.z
	
	bullet.direction = dir.normalized()
	bullet.direction.y = 0.0
	bullet.direction = bullet.direction.normalized()
	#bullet.global_position = global_position + Vector3(0, 1.5, 0) # 1.5 = výška od podlahy
	get_tree().current_scene.add_child(bullet)
	bullet.global_position = global_position + Vector3(0, 1.5, 0) # 1.5 = výška od podlahy
	

func _input(p_event: InputEvent) -> void:
	if p_event is InputEventMouseButton and p_event.button_index == MOUSE_BUTTON_LEFT and p_event.pressed:
		shoot_bullet()
	if p_event is InputEventMouseButton and p_event.pressed:
		if p_event.button_index == MOUSE_BUTTON_WHEEL_UP:
			MOVE_SPEED = clamp(MOVE_SPEED + 5, 5, 9999)
		elif p_event.button_index == MOUSE_BUTTON_WHEEL_DOWN:
			MOVE_SPEED = clamp(MOVE_SPEED - 5, 5, 9999)
	
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
