extends SceneTree
# Analysis only: which library scenes could be drawn by a MultiMesh without any visible change.
func _mat_ok(m) -> String:
	if m == null: return ""
	if m is BaseMaterial3D:
		if m.transparency != BaseMaterial3D.TRANSPARENCY_DISABLED: return "transparent"
		if m.billboard_mode != BaseMaterial3D.BILLBOARD_DISABLED: return "billboard"
		return ""
	if m is ShaderMaterial: return "shader"
	return "other"
func _check(n: Node, root: Node) -> String:
	if n.get_script() != null: return "script " + n.name
	if n is MeshInstance3D:
		if n.skin != null or n.skeleton != NodePath(""): 
			if n.skin != null: return "skin"
		if n.mesh == null: return ""
		if n.material_override: return "material_override"
		for i in n.mesh.get_surface_count():
			var r = _mat_ok(n.get_active_material(i))
			if r != "": return r
	elif n.get_class() != "Node3D":
		return "node " + n.get_class()
	for c in n.get_children():
		var r = _check(c, root)
		if r != "": return r
	return ""
func _init():
	for path in FileAccess.get_file_as_string(OS.get_environment("AN_LIST")).split("\n", false):
		var ps = load(path)
		if ps == null: continue
		var n = ps.instantiate()
		var r = _check(n, n)
		var meshes = n.find_children("*", "MeshInstance3D", true, false).size()
		print("MM;%s;%s;%d" % [path, "OK" if r == "" else r, meshes])
		n.free()
	quit()
