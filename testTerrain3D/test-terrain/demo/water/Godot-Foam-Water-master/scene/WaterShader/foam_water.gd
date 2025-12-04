extends Node3D

var timer: float
var distance: float = 7
var cameraLookAt: Vector3 = Vector3(0,0,0)
@onready var sphere: MeshInstance3D = $Sphere

@onready var camera_3d: Camera3D = $Camera3D
@onready var water: MeshInstance3D = $WaterFoam

func _ready():
	camera_3d.position = Vector3(6,10,6)
	camera_3d.look_at(cameraLookAt)

func _input(event: InputEvent):
	if event.is_action("ui_cancel") and OS.get_name() != "Web":
		get_tree().quit()

func _physics_process(delta):
	timer += delta
	
	sphere.position.y =  -1.5 + sin(timer)*1.5
	
	#camera update
	distance = 8 + 5 * sin(timer * 0.25)
	camera_3d.position = setFromSpherical(80/PI, timer * 0.25) * distance
	camera_3d.position.y += 8 + delta * sin(timer/5.0)
	camera_3d.look_at(sphere.position)
	
	
func setFromSpherical(azimuthalAngle: float, polarAngle: float) -> Vector3:
	var cosPolar: float = cos(polarAngle)
	var sinPolar: float = sin(polarAngle)
	var cosAzim: float = cos(azimuthalAngle)
	var sinAzim: float = sin(azimuthalAngle)
	return Vector3(cosAzim * sinPolar, sinAzim * sinPolar, cosPolar)


func _on_check_box_toggled(toggled_on: bool) -> void:
	var water_mat: Material = water.get_surface_override_material(0)
	water_mat.set_shader_parameter("pixelate", toggled_on)
