extends CharacterBody3D

@export var speed: float = 50.0
@export var life_distance: float = 10.0  # kolik sekund střela existuje
@export var tick_distance: float = 1.0  # vzdálenost mezi deformacemi
@export var damage_radius: float = 2.0  # Radius deformace pro každou tick
@export var damage_strength: float = -1.0  # Síla snížení výšky (negativní pro damage)

var direction: Vector3 = Vector3.ZERO
var timer: float = 0.0
var last_position: Vector3
var tick_queue: Array = []
var terrain_ref: Terrain3D

var begin_position: Vector3

var first_run: bool = true

func _ready() -> void:
	# Pro jistotu normalizujeme směr
	#direction = direction.normalized()
	floor_snap_length = 0.0
	floor_stop_on_slope = false
	floor_max_angle = 0.0  # Nebere ohled na úhly podlahy
	
	terrain_ref = get_node("/root/Demo/NavigationRegion3D/Terrain3D")
	
func _physics_process(delta: float) -> void:
	if first_run:
		begin_position = global_position
		first_run = false
	# Pohyb projektilu
	global_position += direction * speed * delta
	#velocity = direction * speed
	#var collision = move_and_slide()
	#if collision:
		## Např. if collision.get_collider().has_method("take_damage"): ...
		#queue_free()  # Znič při jakékoliv kolizi
	# pokud jsme uletěli dostatečnou vzdálenost, přidej body do fronty
	var distance = last_position.distance_to(global_transform.origin)
	if distance >= tick_distance:
		var steps = int(distance / tick_distance)
		for i in range(1, steps + 1):
			var t = i / float(steps)
			var point = last_position.lerp(global_transform.origin, t)
			tick_queue.append(point)
		last_position = global_transform.origin

	# zpracování všech ticků ve frontě
	while tick_queue.size() > 0:
		var point = tick_queue.pop_front()
		if terrain_ref:
			terrain_ref._apply_terrain_damage(point, damage_radius, damage_strength)
		else:
			push_warning("Terrain3D reference not set! Skipping damage.")
			
		# Konec životnosti
	var total_distance = global_position.distance_to(begin_position)
	if total_distance >= life_distance:
		queue_free()
