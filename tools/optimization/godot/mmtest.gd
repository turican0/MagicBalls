extends Node3D
# Test (not committed): the same entities as nodes (MM=0) or through EntityMultiMesh (MM=1).
const EntityMultiMesh = preload("res://scenes/entity_multimesh.gd")
func _ready():
	var env = WorldEnvironment.new(); env.environment = Environment.new()
	env.environment.background_mode = Environment.BG_COLOR
	env.environment.background_color = Color(0.35, 0.4, 0.5)
	env.environment.ambient_light_source = Environment.AMBIENT_SOURCE_COLOR
	env.environment.ambient_light_color = Color(0.4, 0.4, 0.4)
	add_child(env)
	var sun = DirectionalLight3D.new(); add_child(sun); sun.rotation_degrees = Vector3(-50, 30, 0)
	var mm = EntityMultiMesh.new(); add_child(mm)
	var paths = ["res://entites/object_5_66_centipedeBody.tscn", "res://entites/object_2_422_barell.tscn", "res://entites/object_2_423_basket.tscn", "res://entites/object_2_79_dolmen.tscn", "res://entites/object_5_271_stoneHead.tscn", "res://entites/object_9_105_arrow.tscn", "res://entites/object_5_410_bigDragonNeck.tscn"]
	var rng = RandomNumberGenerator.new(); rng.seed = 3
	var batched := 0
	for k in paths.size():
		var ps: PackedScene = load(paths[k])
		print("CAN ", paths[k].get_file(), " ", mm.can_batch(ps))
		for i in 6:
			var pos = Vector3(k * 6 - 18, 0, i * 6 - 40)
			var yaw = rng.randf() * TAU
			var s = rng.randf_range(0.7, 1.3)
			var bitmap = rng.randf_range(0.5, 1.5) if i % 2 == 0 else null
			if OS.get_environment("MM") == "1" and mm.can_batch(ps):
				var b = Basis.from_euler(Vector3(0, yaw, 0)).scaled(Vector3(1, 1, 1))
				mm.add(ps, Transform3D(b, pos), Vector3(bitmap, bitmap, bitmap) if bitmap != null else null)
				batched += 1
			else:
				var n = ps.instantiate(); add_child(n)
				n.position = pos; n.rotation = Vector3(0, yaw, 0)
				var sn = n.get_node_or_null("Scale")
				if sn and bitmap != null: sn.scale = Vector3(bitmap, bitmap, bitmap)
	mm.update()
	print("BATCHED ", batched)
	var cam = Camera3D.new(); add_child(cam); cam.current = true
	cam.position = Vector3(0, 30, 12); cam.look_at(Vector3(0, 0, -25))
	for i in 20: await get_tree().process_frame
	await RenderingServer.frame_post_draw
	get_viewport().get_texture().get_image().save_png(OS.get_environment("OUT"))
	get_tree().quit()
