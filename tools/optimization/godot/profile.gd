extends Node
# Profiling harness (copied to godot-code/zz_tools, not committed): plays a remc2 recording in a
# level of the full game and writes the time of the parts of every frame to a CSV.
# Env:
#   MB_TEST_LEVEL   level index for --set_level (level 5 = 4)
#   MB_PLAY_FILE    remc2 recording (InputRecorder, e.g. record003/Level5-mine.dem)
#   MB_TEST_FRAMES  frames in the game to record
#   MB_OUT          CSV path
#   MB_FPS          frame limit (0 = none, default 0); vsync is off
#   MB_SCALE        3D render scale (default 1.0), FSR below 1.0 as in the game (MB_SCALE_MODE=bilinear)
#   MB_VARIANT      settings to try, comma separated: noshadows, nofog, nossao, noglow, msaa0, fxaa0
#
#   MB_GPU          1 = measure the GPU time of the viewport (makes the renderer sync every frame)
#   MB_DUMP_FRAME   frame to print the processing scripts and the heaviest geometry
#   MB_QUALITY      quality settings "shadows,reflections" (0..3, 0..1; default 3,1 = Ultra)
#   MB_SHOT_EVERY   save a screenshot every N frames to MB_SHOT_OUT-<frame>.png (e.g. for the README)
#   MB_WINDOW       window size, e.g. 1920x1080
#
# Columns (ms): frame_ms = wall time of the frame; process = all _process callbacks (first node to
# last node); render = RenderingServer draw on the main thread (frame_pre_draw..frame_post_draw);
# between = the rest (physics, input, OS); render_gpu = GPU time (MB_GPU).
# run = REMC2Run = engine (engine thread, e_* are its parts) + screen_image;
# gd_* / entities_get / ent_* / terrain / sounds = parts of decode_level.MBrun (ent_show_hide
# includes ent_lights and ent_particles); mbrun = all of MBrun; other_process = process - mbrun.

const KEYS := ["frame_ms", "process", "render", "between", "render_gpu", "mbrun", "other_process",
	"run", "engine", "screen_image", "e_palette", "e_input", "e_entities", "e_lighting",
	"e_objectives", "e_draw_2d", "e_minimap", "e_top_bar", "gd_fps_print", "gd_engine_post",
	"gd_input", "gd_camera", "gd_player", "entities_get", "ent_loop", "ent_multimesh",
	"ent_show_hide", "ent_lights", "ent_particles", "ent_fill_pool", "terrain", "gd_misc", "sounds",
	"draw_calls", "primitives", "objects", "nodes", "pooled_active", "multimesh_inst", "lights",
	"k_meteor", "k_fair", "k_fire", "k_smoke",
	"pipe_canvas", "pipe_mesh", "pipe_surface", "pipe_draw", "pipe_special", "vram_mb"]

# pipeline compilations in the frame, by where they were asked for (draw = while drawing: a stall)
const PIPE := {"pipe_canvas": RenderingServer.RENDERING_INFO_PIPELINE_COMPILATIONS_CANVAS,
	"pipe_mesh": RenderingServer.RENDERING_INFO_PIPELINE_COMPILATIONS_MESH,
	"pipe_surface": RenderingServer.RENDERING_INFO_PIPELINE_COMPILATIONS_SURFACE,
	"pipe_draw": RenderingServer.RENDERING_INFO_PIPELINE_COMPILATIONS_DRAW,
	"pipe_special": RenderingServer.RENDERING_INFO_PIPELINE_COMPILATIONS_SPECIALIZATION}

# kinds counted in every frame (k_*), to find the parts of a replay with a spell
const WATCH := {"k_meteor": Vector3i(9, 64, 1), "k_fair": Vector3i(10, 38, 1), "k_fire": Vector3i(10, 77, 1), "k_smoke": Vector3i(10, 63, 1)}

const MBRUN_KEYS := ["gd_input", "run", "gd_camera", "gd_player", "entities_get", "ent_loop",
	"ent_multimesh", "ent_show_hide", "ent_fill_pool", "terrain", "gd_misc", "sounds"]

# the first node to process: when the process part of the frame starts
class First extends Node:
	var start_usec := 0
	func _ready():
		process_priority = -100000
	func _process(_delta):
		start_usec = Time.get_ticks_usec()

class Runner extends Node:
	var t := 0.0
	var frame := -1
	var frames := int(OS.get_environment("MB_TEST_FRAMES"))
	var out_path := OS.get_environment("MB_OUT")
	var measure_gpu := OS.get_environment("MB_GPU") == "1"
	var rows: Array[PackedStringArray] = []
	var vp: RID
	var first: First
	var pre_draw_usec := 0
	var post_draw_usec := 0
	var pending := {} # the row of the previous frame, its render time is known one frame later
	var _pipe_last := {} # RENDERING_INFO_PIPELINE_COMPILATIONS_* in the previous frame
	var _last_scene_name := ""

	func _ready():
		process_priority = 100000 # after all other nodes, so prof has the whole frame
		first = First.new()
		get_parent().add_child(first)
		vp = get_viewport().get_viewport_rid()
		if measure_gpu:
			RenderingServer.viewport_set_measure_render_time(vp, true)
		RenderingServer.frame_pre_draw.connect(func(): pre_draw_usec = Time.get_ticks_usec())
		RenderingServer.frame_post_draw.connect(func(): post_draw_usec = Time.get_ticks_usec())

	func _press_space():
		var e = InputEventKey.new()
		e.keycode = KEY_SPACE
		e.physical_keycode = KEY_SPACE
		e.pressed = true
		Input.parse_input_event(e)
		await get_tree().process_frame
		var r = e.duplicate()
		r.pressed = false
		Input.parse_input_event(r)

	func _process(delta):
		var scene = get_tree().current_scene
		var in_game = scene and scene.has_node("MBEngine") and scene.get_node("MBEngine").inGameLoop
		if not in_game and frame > 0 and not rows.is_empty():
			# the level ended (the replay is over) before MB_TEST_FRAMES
			_write()
			get_tree().quit(0)
			return
		if not in_game:
			Global.prof.clear()
			# where the start waits: every long frame before the game, and when the scene changes
			var scene_name: String = scene.name if scene else "-"
			if scene_name != _last_scene_name:
				printerr("PROFILE start %6d ms scene %s" % [Time.get_ticks_msec(), scene_name])
				_last_scene_name = scene_name
			if delta > 0.2:
				var engine_state = scene.get_node("MBEngine").get("runned") if scene and scene.has_node("MBEngine") else "-"
				printerr("PROFILE start %6d ms long frame %d ms (scene %s, engine running %s)" % [Time.get_ticks_msec(), delta * 1000.0, scene_name, engine_state])
			t += delta
			if t > 1.5:
				t = 0.0
				_press_space()
			return
		frame += 1
		var now := Time.get_ticks_usec()
		if frame == 0:
			printerr("PROFILE in game after %d ms" % Time.get_ticks_msec())
			_apply_variant(scene)
			var renderer = scene.get_node("DecodeLevel")._entity_renderer
			renderer.set_profiling(true)
			var hidden := []
			for h in OS.get_environment("MB_HIDE_KINDS").split(";", false): # "5,11,1;10,63,2" - to measure what they cost
				var v := h.split(",")
				hidden.append(Vector3i(int(v[0]), int(v[1]), int(v[2])))
			renderer.set_hidden_kinds(hidden)
			return
		# the previous frame is complete now: its draw and the time up to this frame's start
		if not pending.is_empty():
			pending["render"] = (post_draw_usec - pre_draw_usec) / 1000.0
			pending["frame_ms"] = (first.start_usec - pending["start"]) / 1000.0
			pending["between"] = (first.start_usec - post_draw_usec) / 1000.0
			if measure_gpu:
				pending["render_gpu"] = RenderingServer.viewport_get_measured_render_time_gpu(vp)
			var line := PackedStringArray([str(frame - 1)])
			for k in KEYS:
				line.append("%.3f" % float(pending.get(k, 0.0)))
			rows.append(line)
		var row := {}
		row["start"] = first.start_usec
		row["process"] = (now - first.start_usec) / 1000.0
		row.merge(Global.prof)
		row.merge(Global.MBEX.getFrameProfile())
		var mbrun := 0.0
		for k in MBRUN_KEYS:
			mbrun += float(row.get(k, 0.0))
		row["mbrun"] = mbrun
		row["other_process"] = row["process"] - mbrun
		row["draw_calls"] = Performance.get_monitor(Performance.RENDER_TOTAL_DRAW_CALLS_IN_FRAME)
		row["primitives"] = Performance.get_monitor(Performance.RENDER_TOTAL_PRIMITIVES_IN_FRAME)
		row["objects"] = Performance.get_monitor(Performance.RENDER_TOTAL_OBJECTS_IN_FRAME)
		row["nodes"] = Performance.get_monitor(Performance.OBJECT_NODE_COUNT)
		var dl = scene.get_node("DecodeLevel")
		row["pooled_active"] = dl._entity_renderer.active_node_count()
		row["multimesh_inst"] = dl._entity_renderer.multimesh_instance_count()
		for k in WATCH:
			row[k] = dl._entity_renderer.drawn_last_frame_of(WATCH[k])
		for k in PIPE:
			var total := RenderingServer.get_rendering_info(PIPE[k])
			row[k] = total - _pipe_last.get(k, total)
			_pipe_last[k] = total
		row["vram_mb"] = RenderingServer.get_rendering_info(RenderingServer.RENDERING_INFO_VIDEO_MEM_USED) / 1048576.0
		row["lights"] = dl._frame_lights.size()
		Global.prof.clear()
		if frame % 300 == 0 and not pending.is_empty():
			printerr("PROFILE frame %d frame_ms %.1f process %.1f render %.1f" % [frame - 1, pending["frame_ms"], pending["process"], pending["render"]])
		pending = row
		if frame == int(OS.get_environment("MB_DUMP_FRAME")):
			_dump(scene)
		var shot_every := int(OS.get_environment("MB_SHOT_EVERY"))
		if shot_every > 0 and frame % shot_every == 0:
			get_viewport().get_texture().get_image().save_png(OS.get_environment("MB_SHOT_OUT") + "-%05d.png" % frame)
		if str(frame) in OS.get_environment("MB_STATE_FRAMES").split(",", false):
			_dump_state(scene, OS.get_environment("MB_STATE_OUT") + "-%d.txt" % frame)
		if low_particles and frame % 30 == 1:
			_lower_particle_meshes(scene)
		if no_tornado and frame % 30 == 1:
			for n in scene.find_children("*", "", true, false):
				if n.get_script() and n.get_script().resource_path.ends_with("lowpoly_tornado.gd"):
					n.set_process(false)
		if frame >= frames:
			_write()
			get_tree().quit(0)

	func _apply_variant(scene):
		var variant := OS.get_environment("MB_VARIANT").split(",", false)
		var scale := OS.get_environment("MB_SCALE")
		if scale != "":
			get_viewport().scaling_3d_scale = float(scale)
			# as the settings do it: FSR below 100 % (MB_SCALE_MODE=bilinear to compare)
			var fsr := float(scale) < 1.0 and OS.get_environment("MB_SCALE_MODE") != "bilinear"
			get_viewport().scaling_3d_mode = Viewport.SCALING_3D_MODE_FSR if fsr else Viewport.SCALING_3D_MODE_BILINEAR
		var env: Environment = null
		var we = scene.find_children("*", "WorldEnvironment", true, false)
		if not we.is_empty():
			env = we[0].environment
		for v in variant:
			match v:
				"noshadows":
					for l in scene.find_children("*", "DirectionalLight3D", true, false):
						l.shadow_enabled = false
				"nofog":
					if env:
						env.fog_enabled = false
						env.volumetric_fog_enabled = false
				"nossao":
					if env:
						env.ssao_enabled = false
						env.ssil_enabled = false
				"noglow":
					if env:
						env.glow_enabled = false
				"msaa0":
					get_viewport().msaa_3d = Viewport.MSAA_DISABLED
				"fxaa0":
					get_viewport().screen_space_aa = Viewport.SCREEN_SPACE_AA_DISABLED
				"nossr":
					if env:
						env.ssr_enabled = false
				"lowpart":
					low_particles = true
				"nomidi": # ablation: what the MIDI players cost while the ogg music plays
					for n in get_tree().root.find_children("*", "", true, false):
						if n.get_script() and n.get_script().resource_path.ends_with("MidiPlayer.gd"):
							n.set_process(false)
				"notornado": # ablation (stops them moving): the tornado scripts
					no_tornado = true
		printerr("PROFILE variant %s scale %s env %s" % [variant, scale, env != null])
		if env:
			printerr("PROFILE env fog %s vfog %s ssao %s ssil %s ssr %s glow %s sdfgi %s" % [env.fog_enabled,
				env.volumetric_fog_enabled, env.ssao_enabled, env.ssil_enabled, env.ssr_enabled, env.glow_enabled, env.sdfgi_enabled])
		printerr("PROFILE viewport msaa %d ssaa %d scale %.2f mode %d size %s" % [get_viewport().msaa_3d,
			get_viewport().screen_space_aa, get_viewport().scaling_3d_scale, get_viewport().scaling_3d_mode, get_viewport().get_visible_rect().size])

	# lowpart: particles drawn as a SphereMesh of the default 64x32 segments get a 12x6 one
	var low_particles := false
	var no_tornado := false
	var low_meshes := {}
	func _lower_particle_meshes(scene):
		for p in scene.find_children("*", "GPUParticles3D", true, false):
			var m = p.draw_pass_1
			if m is SphereMesh and m.radial_segments > 12:
				if not low_meshes.has(m):
					var low: SphereMesh = m.duplicate()
					low.radial_segments = 12
					low.rings = 6
					low_meshes[m] = low
				p.draw_pass_1 = low_meshes[m]

	# What the entities draw now, sorted, to compare two implementations at the same frame of the
	# replay: every visible pool node (uid, transform, its "Scale" node) and every MultiMesh instance.
	func _dump_state(scene, path: String):
		var lines := PackedStringArray()
		var dl = scene.get_node("DecodeLevel")
		for n in dl.get_children():
			if n is Node3D and n.has_meta("uid") and n.visible:
				var t: Transform3D = n.global_transform
				var s = n.get_node_or_null("Scale")
				lines.append("N %s %s %s" % [n.get_meta("uid"), _fmt_xform(t), _fmt_vec(s.scale) if s is Node3D else "-"])
		for mmi in dl.find_children("*", "MultiMeshInstance3D", true, false):
			if not mmi.visible or mmi.multimesh == null or mmi.multimesh.instance_count == 0:
				continue
			var mesh: Mesh = mmi.multimesh.mesh
			var key := "%s|%d|%s" % [mesh.resource_path, mesh.get_surface_count(), _fmt_vec(mesh.get_aabb().size)]
			var buf: PackedFloat32Array = mmi.multimesh.buffer
			for i in mmi.multimesh.instance_count:
				var v := PackedStringArray()
				for k in 12:
					v.append("%.3f" % buf[i * 12 + k])
				lines.append("M %s %s" % [key, " ".join(v)])
		lines.sort()
		var f := FileAccess.open(path, FileAccess.WRITE)
		f.store_string("\n".join(lines))
		f.close()

	func _fmt_vec(v: Vector3) -> String:
		return "%.3f %.3f %.3f" % [v.x, v.y, v.z]

	func _fmt_xform(t: Transform3D) -> String:
		return "%s %s %s %s" % [_fmt_vec(t.basis.x), _fmt_vec(t.basis.y), _fmt_vec(t.basis.z), _fmt_vec(t.origin)]

	# Which scripts process every frame (how many nodes) and which geometry draws the most primitives.
	func _dump(scene):
		var scripts := {}
		var geo := []
		for n in get_tree().root.find_children("*", "", true, false):
			if n.is_processing() or n.is_physics_processing():
				var s = n.get_script()
				var key: String = (s.resource_path if s else "<" + n.get_class() + ">") + (" P" if n.is_processing() else "") + (" F" if n.is_physics_processing() else "")
				scripts[key] = scripts.get(key, 0) + 1
			if n is GeometryInstance3D and n.is_visible_in_tree():
				var tris := 0
				var count := 1
				var mesh: Mesh = null
				if n is MeshInstance3D:
					mesh = n.mesh
				elif n is MultiMeshInstance3D and n.multimesh:
					mesh = n.multimesh.mesh
					count = n.multimesh.visible_instance_count if n.multimesh.visible_instance_count >= 0 else n.multimesh.instance_count
				elif n is GPUParticles3D:
					mesh = n.draw_pass_1
					count = n.amount
				if mesh:
					for si in mesh.get_surface_count():
						var arr = mesh.surface_get_arrays(si)
						var idx = arr[Mesh.ARRAY_INDEX]
						tris += (idx.size() if idx else arr[Mesh.ARRAY_VERTEX].size()) / 3
				geo.append([tris * count, str(n.get_path()), n.get_class(), count, tris])
		var keys := scripts.keys()
		keys.sort_custom(func(a, b): return scripts[a] > scripts[b])
		for k in keys:
			printerr("PROFILE script %5d %s" % [scripts[k], k])
		geo.sort_custom(func(a, b): return a[0] > b[0])
		for g in geo.slice(0, 25):
			printerr("PROFILE geo %12d %s %s x%d tris %d" % g)

	func _write():
		var f := FileAccess.open(out_path, FileAccess.WRITE)
		f.store_line(",".join(PackedStringArray(["frame"] + KEYS)))
		for r in rows:
			f.store_line(",".join(r))
		f.close()
		printerr("PROFILE wrote %d frames to %s" % [rows.size(), out_path])
		for key in Global.prof_entities:
			if key is String and key.begins_with("sound:"):
				var v: Array = Global.prof_entities[key]["call"]
				printerr("PROFILE %s count %d total %.1f ms max %.1f ms" % [key, v[0], v[1], v[2]])
		for what in Global.prof_entities.get("level", {}):
			var v: Array = Global.prof_entities["level"][what]
			printerr("PROFILE level %s count %d total %.1f ms" % [what, v[0], v[1]])
		_write_entity_report(get_tree().current_scene)

	# One line per kind of entity: how many are drawn, what making their nodes and loading their
	# scene cost (Global.prof_entities), the time of EntityRenderer for them, and what the scene
	# draws (triangles, surfaces = draw calls, shadows, transparency, lights, particles).
	func _write_entity_report(scene):
		var dl = scene.get_node("DecodeLevel")
		var path_out := out_path.get_basename() + "-entities.csv"
		var f := FileAccess.open(path_out, FileAccess.WRITE)
		f.store_line("uid,scene,kind,first_frame,frames,drawn_avg,drawn_max,update_ms,show_ms,show_max_ms,show_spikes," +
			"prefill_n,prefill_ms,prefill_max,fill_n,fill_ms,fill_max,ranout_n,ranout_ms,ranout_max,load_wait_ms," +
			"tris,surfaces,shadow_surfaces,transparent_surfaces,lights,shadow_lights,particle_nodes,particle_amount,particle_tris,script_nodes,anim_players,csg,viewports")
		var stats: Array = dl._entity_renderer.get_kind_stats()
		var seen := {}
		for s in stats:
			seen[s["uid"]] = s
		for uid in Global.prof_entities:
			if uid is Vector3i and not seen.has(uid):
				seen[uid] = {"uid": uid, "frames": 0, "drawn_avg": 0, "drawn_max": 0, "update_ms": 0, "show_ms": 0, "show_max_ms": 0, "show_spikes": 0, "first_frame": -1}
		for uid in seen:
			var s: Dictionary = seen[uid]
			var key := Vector3i(uid.x, uid.y, 0)
			var source: Dictionary = dl.library if uid.z == 1 else dl.library2
			var path: String = source.get(key, "<default>")
			var packed: PackedScene = dl._loaded_scenes.get(path) if path != "<default>" else null
			var kind := "batch" if packed and dl._entity_multimesh.can_batch(packed) else "node"
			var p: Dictionary = Global.prof_entities.get(uid, {})
			var l: Dictionary = Global.prof_entities.get(path, {})
			var a := _analyze_scene(packed)
			var cols := [str(uid).replace(",", " "), path.get_file(), kind, s["first_frame"], s["frames"], "%.1f" % s["drawn_avg"], s["drawn_max"],
				"%.2f" % s["update_ms"], "%.2f" % s["show_ms"], "%.2f" % s["show_max_ms"], s["show_spikes"]]
			for what in ["prefill", "fill", "ran_out"]:
				var v: Array = p.get(what, [0, 0.0, 0.0])
				cols.append_array([v[0], "%.2f" % v[1], "%.2f" % v[2]])
			cols.append("%.1f" % l.get("load_wait", [0, 0.0, 0.0])[1])
			for k in ["tris", "surfaces", "shadow_surfaces", "transparent_surfaces", "lights", "shadow_lights",
					"particle_nodes", "particle_amount", "particle_tris", "script_nodes", "anim_players", "csg", "viewports"]:
				cols.append(a.get(k, 0))
			f.store_line(",".join(PackedStringArray(cols.map(func(c): return str(c)))))
		f.close()
		printerr("PROFILE wrote %d entity kinds to %s" % [seen.size(), path_out])

	func _analyze_scene(packed: PackedScene) -> Dictionary:
		var a := {}
		if packed == null:
			return a
		var root := packed.instantiate()
		for n in [root] + root.find_children("*", "", true, false):
			if n.get_script() != null:
				a["script_nodes"] = a.get("script_nodes", 0) + 1
			if n is AnimationPlayer:
				a["anim_players"] = a.get("anim_players", 0) + 1
			if n is CSGShape3D:
				a["csg"] = a.get("csg", 0) + 1
			if n is SubViewport:
				a["viewports"] = a.get("viewports", 0) + 1
			if n is OmniLight3D or n is SpotLight3D:
				a["lights"] = a.get("lights", 0) + 1
				if n.shadow_enabled:
					a["shadow_lights"] = a.get("shadow_lights", 0) + 1
			if n is MeshInstance3D and n.mesh:
				for si in n.mesh.get_surface_count():
					a["tris"] = a.get("tris", 0) + _surface_tris(n.mesh, si)
					a["surfaces"] = a.get("surfaces", 0) + 1
					if n.cast_shadow != GeometryInstance3D.SHADOW_CASTING_SETTING_OFF:
						a["shadow_surfaces"] = a.get("shadow_surfaces", 0) + 1
					var m = n.get_active_material(si)
					if m is BaseMaterial3D and m.transparency != BaseMaterial3D.TRANSPARENCY_DISABLED:
						a["transparent_surfaces"] = a.get("transparent_surfaces", 0) + 1
			if n is GPUParticles3D:
				a["particle_nodes"] = a.get("particle_nodes", 0) + 1
				a["particle_amount"] = a.get("particle_amount", 0) + n.amount
				if n.draw_pass_1:
					var t := 0
					for si in n.draw_pass_1.get_surface_count():
						t += _surface_tris(n.draw_pass_1, si)
					a["particle_tris"] = a.get("particle_tris", 0) + t * n.amount
		root.free()
		return a

	func _surface_tris(mesh: Mesh, si: int) -> int:
		var arr = mesh.surface_get_arrays(si)
		var idx = arr[Mesh.ARRAY_INDEX]
		return (idx.size() if idx else arr[Mesh.ARRAY_VERTEX].size()) / 3

func _ready():
	DisplayServer.window_set_vsync_mode(DisplayServer.VSYNC_DISABLED)
	var size := OS.get_environment("MB_WINDOW").split("x")
	if size.size() == 2:
		DisplayServer.window_set_size(Vector2i(int(size[0]), int(size[1])))
	Global.profiling = true # from the start: the pool is filled while the level loads
	Global.level_mode = 1
	Global.custom_level = int(OS.get_environment("MB_TEST_LEVEL"))
	Global.play_file = OS.get_environment("MB_PLAY_FILE")
	var quality := OS.get_environment("MB_QUALITY").split(",")
	if quality.size() == 2:
		Global.shadow_quality = int(quality[0])
		Global.reflections = quality[1] == "1"
	var fps := OS.get_environment("MB_FPS")
	Global.max_fps = int(fps) if fps != "" else 0
	Global.master_volume = 0.0
	Global.music_volume = 0.0
	Global.sounds_volume = 0.0
	Global.speech_volume = 0.0
	get_tree().root.add_child.call_deferred(Runner.new())
	await get_tree().process_frame
	get_tree().change_scene_to_file("res://scenes/CodeGeneratedDemo.tscn")
