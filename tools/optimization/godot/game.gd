extends Node
# Test harness (not committed): run a level in the full game and report frame times and the
# entity pool. Env: MB_TEST_LEVEL, MB_TEST_FRAMES, MB_TEST_CLICKS="f1,f2,..." (left clicks).
class Runner extends Node:
	var t := 0.0
	var frame := -1
	var frames := int(OS.get_environment("MB_TEST_FRAMES"))
	var clicks := []
	var times := []
	var t_start := 0
	func _ready():
		for c in OS.get_environment("MB_TEST_CLICKS").split(",", false): clicks.append(int(c))
	func _click(pressed: bool):
		var e = InputEventMouseButton.new()
		e.button_index = MOUSE_BUTTON_LEFT
		e.pressed = pressed
		e.position = Vector2(get_viewport().get_visible_rect().size.x / 2, get_viewport().get_visible_rect().size.y * 0.7)
		Input.parse_input_event(e)
	func _process(delta):
		var scene = get_tree().current_scene
		var in_game = scene and scene.has_node("MBEngine") and scene.get_node("MBEngine").inGameLoop
		if not in_game:
			t += delta
			if t > 1.5:
				t = 0.0
				var e = InputEventKey.new()
				e.keycode = KEY_SPACE
				e.physical_keycode = KEY_SPACE
				e.pressed = true
				Input.parse_input_event(e)
				await get_tree().process_frame
				var r = e.duplicate()
				r.pressed = false
				Input.parse_input_event(r)
			return
		frame += 1
		if frame == 0:
			t_start = Time.get_ticks_msec()
			printerr("HARNESS in game after %d ms" % Time.get_ticks_msec())
		times.append(Performance.get_monitor(Performance.TIME_PROCESS) * 1000.0)
		if frame in clicks: _click(true)
		if frame - 3 in clicks: _click(false)
		if frame % 25 == 0:
			var dl = scene.get_node("DecodeLevel")
			var pooled := 0
			for b in dl.entites_pool.values(): pooled += b["array"].size()
			var mmc := 0
			for c in dl._entity_multimesh.get_children(): if c.visible: mmc += c.multimesh.instance_count
			printerr("HARNESS frame %d pooled %d buckets %d multimesh instances %d" % [frame, pooled, dl.entites_pool.size(), mmc])
		if frame >= frames:
			var s = times.duplicate(); s.sort()
			var sum := 0.0
			for x in s: sum += x
			var slow := 0
			for x in s: if x > 33.0: slow += 1
			printerr("HARNESS done frames %d wall %d ms avg %.2f p50 %.2f p99 %.2f max %.2f slow>33ms %d" % [s.size(), Time.get_ticks_msec() - t_start, sum / s.size(), s[s.size() / 2], s[int(s.size() * 0.99)], s[-1], slow])
			var worst = []
			for i in times.size(): if times[i] > 33.0: worst.append("%d:%.0f" % [i, times[i]])
			printerr("HARNESS slow frames ", worst.slice(0, 40))
			var after := 0.0
			for i in range(40, times.size()): after += times[i]
			printerr("HARNESS avg after frame 40: %.1f" % (after / max(1, times.size() - 40)))
			get_tree().quit(0)

func _ready():
	Global.level_mode = 1
	Global.custom_level = int(OS.get_environment("MB_TEST_LEVEL"))
	Global.max_fps = 30
	Global.master_volume = 0.0
	Global.music_volume = 0.0
	Global.sounds_volume = 0.0
	Global.speech_volume = 0.0
	get_tree().root.add_child.call_deferred(Runner.new())
	await get_tree().process_frame
	get_tree().change_scene_to_file("res://scenes/CodeGeneratedDemo.tscn")
