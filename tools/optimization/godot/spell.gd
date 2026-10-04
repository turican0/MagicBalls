extends Node
# Analysis only (not committed): cast MB_TEST_SPELL and record, per entity type,
# how many exist before the cast and at most after it.
var rect: TextureRect
var steps := 0
var game_steps := 0
var in_game := false
var max_count := {}
var first_count := {}
func _ready():
	rect = TextureRect.new()
	add_child(rect)
	Global.MBEX = MBEXclass.new()
	Global.MBEX.REMC2SetScrBuffer(rect)
	Global.MBEX.REMC2BeginGame(Global.cdPath, Global.hidata, int(OS.get_environment("AN_LEVEL")), "")
func _process(_d):
	for i in 25:
		var kc = []
		if not in_game:
			if steps % 30 == 0: kc.append({"key_index": 0x3920, "action": "pressed"})
			if steps % 30 == 3: kc.append({"key_index": 0x3920, "action": "released"})
		var free = []
		for j in 10: free.append(0)
		Global.MBEX.updateFreeSoundPlayers(free)
		var mb = []
		if in_game and game_steps in [150, 210, 270]: mb.append({"button_index": 0, "action": "pressed"})
		if in_game and game_steps in [154, 214, 274]: mb.append({"button_index": 0, "action": "released"})
		var r = Global.MBEX.REMC2Run({"key_changes": kc, "mouse_button_changes": mb, "mouse_pos2": Vector2(320, int(OS.get_environment("AN_AIM_Y")) if OS.get_environment("AN_AIM_Y") != "" else 100)}, 0)
		Global.MBEX.getPendingSoundActions()
		steps += 1
		if r == 5: in_game = true
		if in_game:
			game_steps += 1
			var d: PackedFloat32Array = Global.MBEX.GetEntites()
			var cnt := {}
			var n := d.size() / 31
			for e in n:
				var o = e * 31
				var cls = int(d[o + 7]); var model = int(d[o + 21])
				var byte0 = int(d[o + 17]); var byte1 = int(d[o + 18])
				if (byte1 & 4) != 0: continue
				if cls == 0 and model == 0: continue
				var key = "%d,%d,%d" % [cls, model, 1 if (byte0 & 1) == 0 else 0]
				cnt[key] = cnt.get(key, 0) + 1
			if game_steps % 20 == 0:
				var h := 0.0
				for e in n: h += d[e*31] + d[e*31+1]
				var tot := 0
				for k in cnt: tot += cnt[k]
				print("DBG step ", game_steps, " tot ", tot, " keys ", cnt.size())
			if game_steps == 149: first_count = cnt.duplicate()
			if game_steps >= 150:
				for k in cnt: max_count[k] = max(max_count.get(k, 0), cnt[k])
			if game_steps >= int(OS.get_environment("AN_STEPS")):
				var f = FileAccess.open(OS.get_environment("AN_OUT"), FileAccess.WRITE)
				f.store_string(JSON.stringify({"level": int(OS.get_environment("AN_LEVEL")), "spell": OS.get_environment("MB_TEST_SPELL"), "first": first_count, "max": max_count}))
				f.close()
				get_tree().quit(0)
				return
		if steps > 20000:
			get_tree().quit(2)
			return
