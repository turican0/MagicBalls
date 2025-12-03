extends CharacterBody3D

@export var speed: float = 50.0
@export var lifetime: float = 3.0   # kolik sekund střela existuje

var direction: Vector3 = Vector3.ZERO
var timer: float = 0.0


func _ready() -> void:
	# Pro jistotu normalizujeme směr
	direction = direction.normalized()


func _physics_process(delta: float) -> void:
	# Pohyb projektilu
	velocity = direction * speed
	move_and_slide()

	# Konec životnosti
	timer += delta
	if timer >= lifetime:
		queue_free()
