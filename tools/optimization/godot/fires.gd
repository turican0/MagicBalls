extends Node3D
# Test (not committed): 300 fires like Meteor III - light budget off/on (env BUDGET), image + frame time.
const EntityLightBudget = preload("res://scenes/entity_light_budget.gd")
const EntityParticles = preload("res://scenes/entity_particles.gd")
var shared
var taken := []
var budget
var lights := []
var cam: Camera3D
func _ready():
	var env = WorldEnvironment.new(); env.environment = Environment.new()
	env.environment.ambient_light_source = Environment.AMBIENT_SOURCE_COLOR
	env.environment.ambient_light_color = Color(0.05, 0.05, 0.05)
	add_child(env)
	var plane = MeshInstance3D.new(); var pm = PlaneMesh.new(); pm.size = Vector2(200, 200); plane.mesh = pm
	var mat = StandardMaterial3D.new(); mat.albedo_color = Color(0.5, 0.45, 0.4); plane.material_override = mat
	add_child(plane)
	var ps: PackedScene = load("res://entites/object_10_8_fair.tscn")
	var rng = RandomNumberGenerator.new(); rng.seed = 7
	for i in int(OS.get_environment("N")):
		var n = ps.instantiate()
		# meteor-like: a few impact areas with fires around them
		var c = Vector3((i % 6) * 12 - 30, 0, (i / 6 % 4) * 12 - 20)
		n.position = c + Vector3(rng.randf_range(-4, 4), rng.randf_range(0, 1.5), rng.randf_range(-4, 4))
		add_child(n)
		if OS.get_environment("NOPART") == "1": n.get_node("GPUParticles3D").draw_pass_1 = null
		lights.append_array(n.find_children("*", "OmniLight3D", true, false))
		if OS.get_environment("NOLIGHT") == "1":
			for l in n.find_children("*", "OmniLight3D", true, false): l.visible = false
	cam = Camera3D.new(); add_child(cam); cam.current = true
	cam.position = Vector3(0, 22, 40); cam.look_at(Vector3(0, 0, -5))
	if OS.get_environment("SHARED") == "1":
		shared = EntityParticles.new(); add_child(shared)
		for c in get_children(): if c.has_node("GPUParticles3D"): taken.append_array(shared.register(c))
	if OS.get_environment("BUDGET") == "1":
		budget = EntityLightBudget.new(); add_child(budget)
	for i in 30:
		if shared: shared.emit(taken, get_process_delta_time())
		await get_tree().process_frame
	var t0 = Time.get_ticks_usec()
	var bt := 0
	for i in 40:
		if budget:
			var b0 = Time.get_ticks_usec(); budget.update_lights(lights, cam.global_position); bt += Time.get_ticks_usec() - b0
		if shared: shared.emit(taken, get_process_delta_time())
		await get_tree().process_frame
	print("RESULT avg frame %.1f ms, budget update %.2f ms" % [(Time.get_ticks_usec() - t0) / 40000.0, bt / 40000.0])
	await RenderingServer.frame_post_draw
	get_viewport().get_texture().get_image().save_png(OS.get_environment("OUT"))
	get_tree().quit()
