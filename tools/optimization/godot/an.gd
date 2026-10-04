extends Node
# Analysis only (not committed): load and instantiate every library scene and describe it.
func _ready():
	var out = FileAccess.open(OS.get_environment("AN_OUT"), FileAccess.WRITE)
	out.store_line("path;load_ms;inst1_ms;inst2_ms;nodes;meshes;tris;skinned;materials;textures;tex_mpix;max_tex;particles;particle_amount;lights;shadow_lights;anim_players;scripts")
	for p in FileAccess.get_file_as_string(OS.get_environment("AN_LIST")).split("\n", false):
		var t0 = Time.get_ticks_usec()
		var ps: PackedScene = load(p)
		var t1 = Time.get_ticks_usec()
		if ps == null:
			out.store_line("%s;LOADFAIL" % p); continue
		var n1 = ps.instantiate()
		var t2 = Time.get_ticks_usec()
		var n2 = ps.instantiate()
		var t3 = Time.get_ticks_usec()
		var st = {"nodes":0,"meshes":0,"tris":0,"skinned":0,"particles":0,"pamount":0,"lights":0,"shadow":0,"anim":0,"scripts":0}
		var mats = {}
		var texs = {}
		_walk(n1, st, mats, texs)
		var mpix = 0.0
		var maxt = 0
		for t in texs.keys():
			mpix += t.get_width() * t.get_height() / 1000000.0
			maxt = max(maxt, max(t.get_width(), t.get_height()))
		out.store_line("%s;%.1f;%.2f;%.2f;%d;%d;%d;%d;%d;%d;%.1f;%d;%d;%d;%d;%d;%d;%d" % [p, (t1-t0)/1000.0, (t2-t1)/1000.0, (t3-t2)/1000.0, st.nodes, st.meshes, st.tris, st.skinned, mats.size(), texs.size(), mpix, maxt, st.particles, st.pamount, st.lights, st.shadow, st.anim, st.scripts])
		n1.free(); n2.free()
	out.close()
	get_tree().quit(0)

func _mat_textures(m: Material, texs: Dictionary):
	if m == null: return
	if m is BaseMaterial3D:
		for prop in m.get_property_list():
			if prop.name.ends_with("_texture") or prop.name == "albedo_texture":
				var v = m.get(prop.name)
				if v is Texture2D: texs[v] = true
	elif m is ShaderMaterial:
		for prop in m.get_property_list():
			if prop.name.begins_with("shader_parameter/"):
				var v = m.get(prop.name)
				if v is Texture2D: texs[v] = true
	if m.next_pass: _mat_textures(m.next_pass, texs)

func _walk(n: Node, st: Dictionary, mats: Dictionary, texs: Dictionary):
	st.nodes += 1
	if n.get_script() != null: st.scripts += 1
	if n is MeshInstance3D and n.mesh:
		st.meshes += 1
		if n.skin != null or n.skeleton != NodePath(""): st.skinned += 1
		var m: Mesh = n.mesh
		for s in m.get_surface_count():
			var idx = m.surface_get_array_index_len(s) if m is ArrayMesh else 0
			st.tris += (idx / 3) if idx > 0 else (m.surface_get_array_len(s) / 3 if m is ArrayMesh else 0)
			var mat = n.get_surface_override_material(s)
			if mat == null: mat = m.surface_get_material(s)
			if mat: mats[mat] = true; _mat_textures(mat, texs)
		if n.material_override: mats[n.material_override] = true; _mat_textures(n.material_override, texs)
	if n is GPUParticles3D or n is CPUParticles3D:
		st.particles += 1; st.pamount += n.amount
		if n is GPUParticles3D and n.draw_pass_1 and n.draw_pass_1.surface_get_material(0):
			_mat_textures(n.draw_pass_1.surface_get_material(0), texs)
	if n is Light3D:
		st.lights += 1
		if n.shadow_enabled: st.shadow += 1
	if n is AnimationPlayer: st.anim += 1
	for c in n.get_children():
		_walk(c, st, mats, texs)
