extends Node3D
# Test (not committed): render each scene of SCENES (comma separated) from close and from afar.
func _aabb(n: Node, acc: Array):
	if n is VisualInstance3D and n.is_visible_in_tree() and not (n is Light3D):
		var a = n.global_transform * n.get_aabb()
		acc[0] = a if acc[0] == null else acc[0].merge(a)
	for c in n.get_children(): _aabb(c, acc)
func _ready():
	var env = WorldEnvironment.new(); env.environment = Environment.new()
	env.environment.background_mode = Environment.BG_COLOR
	env.environment.background_color = Color(0.35, 0.4, 0.5)
	env.environment.ambient_light_source = Environment.AMBIENT_SOURCE_COLOR
	env.environment.ambient_light_color = Color(0.4, 0.4, 0.4)
	add_child(env)
	var sun = DirectionalLight3D.new(); add_child(sun); sun.rotation_degrees = Vector3(-50, 30, 0)
	var cam = Camera3D.new(); add_child(cam); cam.current = true
	for path in OS.get_environment("SCENES").split(",", false):
		var n = load(path).instantiate(); add_child(n)
		await get_tree().process_frame
		for ap in n.find_children("*", "AnimationPlayer", true, false):
			if ap.current_animation != "": ap.seek(0.0, true); ap.pause()
		var acc = [null]; _aabb(n, acc)
		var box: AABB = acc[0] if acc[0] != null else AABB(Vector3(-1, -1, -1), Vector3(2, 2, 2))
		var r = box.size.length() * 0.5
		for d in [["near", 1.6], ["far", 6.0]]:
			cam.position = box.get_center() + Vector3(0.6, 0.35, 1.0).normalized() * r * d[1] * 1.6
			cam.look_at(box.get_center())
			for i in 8: await get_tree().process_frame
			await RenderingServer.frame_post_draw
			get_viewport().get_texture().get_image().save_png("%s/%s_%s.png" % [OS.get_environment("OUT"), path.get_file().get_basename(), d[0]])
		n.queue_free()
		await get_tree().process_frame
	get_tree().quit()
