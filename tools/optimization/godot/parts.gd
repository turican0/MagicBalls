extends Node3D
# Test (not committed): left = original fire scenes, right = the same scenes through EntityParticles.
const EntityParticles = preload("res://scenes/entity_particles.gd")
var shared
var taken := []
func _ready():
	var env = WorldEnvironment.new(); env.environment = Environment.new()
	env.environment.ambient_light_source = Environment.AMBIENT_SOURCE_COLOR
	env.environment.ambient_light_color = Color(0.3, 0.3, 0.3)
	env.environment.background_mode = Environment.BG_COLOR
	env.environment.background_color = Color(0.02, 0.02, 0.05)
	add_child(env)
	shared = EntityParticles.new(); add_child(shared)
	for path in ["res://entites/object_10_8_fair.tscn", "res://entites/object_10_77_fire.tscn", "res://entites/object_9_64_meteor.tscn"]:
		if path.ends_with("meteor.tscn"): continue
		var ps: PackedScene = load(path)
		var z = [0.0, -5.0, -12.0][["res://entites/object_10_8_fair.tscn", "res://entites/object_10_77_fire.tscn", "res://entites/object_9_64_meteor.tscn"].find(path)]
		var a = ps.instantiate(); a.position = Vector3(-3, 0, z); add_child(a)
		var b = ps.instantiate(); b.position = Vector3(3, 0, z); add_child(b)
		taken.append_array(shared.register(b))
	print("TAKEN ", taken.size())
	var cam = Camera3D.new(); add_child(cam); cam.current = true
	cam.position = Vector3(0, 3, 7); cam.look_at(Vector3(0, 1.5, -4))
	for i in 90:
		await get_tree().process_frame
		shared.emit(taken, get_process_delta_time())
		if i in [45, 89]:
			await RenderingServer.frame_post_draw
			get_viewport().get_texture().get_image().save_png(OS.get_environment("OUT") + "_%d.png" % i)
	get_tree().quit()
