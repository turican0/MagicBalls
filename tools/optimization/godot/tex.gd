extends SceneTree
# Analysis only: list textures used by library scenes with their size.
var seen := {}
func _walk_res(r, owner_path: String):
	if r == null: return
	if r is Texture2D:
		var key = r.resource_path
		if not seen.has(key):
			seen[key] = [r.get_width(), r.get_height(), owner_path]
		return
	if r is Material:
		for p in r.get_property_list():
			if p.type == TYPE_OBJECT:
				var v = r.get(p.name)
				if v is Texture2D or v is Material: _walk_res(v, owner_path)
func _walk(n: Node, owner_path: String):
	if n is GeometryInstance3D:
		_walk_res(n.material_override, owner_path)
		if n is MeshInstance3D and n.mesh:
			for i in n.mesh.get_surface_count():
				_walk_res(n.mesh.surface_get_material(i), owner_path)
				_walk_res(n.get_surface_override_material(i), owner_path)
		if n is GPUParticles3D:
			_walk_res(n.process_material, owner_path)
			if n.draw_pass_1: for i in n.draw_pass_1.get_surface_count(): _walk_res(n.draw_pass_1.surface_get_material(i), owner_path)
	for c in n.get_children(): _walk(c, owner_path)
func _init():
	for path in FileAccess.get_file_as_string(OS.get_environment("AN_LIST")).split("\n", false):
		var ps = load(path)
		if ps == null: continue
		var n = ps.instantiate()
		_walk(n, path)
		n.free()
	var out := ""
	for k in seen: out += "%s;%d;%d;%s\n" % [k, seen[k][0], seen[k][1], seen[k][2]]
	var f = FileAccess.open(OS.get_environment("AN_OUT"), FileAccess.WRITE)
	f.store_string(out)
	quit()
