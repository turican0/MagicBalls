extends SceneTree
# Tool (not committed): simplify the ArrayMesh sub-resources of a scene to RATIO of their
# triangles and save each as <scene>_<id>.res next to it. Prints "MESH <id> <res path> <tris>".
func _simplify(mesh: ArrayMesh, ratio: float) -> ArrayMesh:
	var im := ImporterMesh.new()
	for b in mesh.get_blend_shape_count(): im.add_blend_shape(mesh.get_blend_shape_name(b))
	for i in mesh.get_surface_count():
		im.add_surface(mesh.surface_get_primitive_type(i), mesh.surface_get_arrays(i), mesh.surface_get_blend_shape_arrays(i), {}, mesh.surface_get_material(i), mesh.surface_get_name(i), mesh.surface_get_format(i) & ~Mesh.ARRAY_FORMAT_VERTEX & ~Mesh.ARRAY_FORMAT_NORMAL)
	im.generate_lods(25.0, 60.0, [])
	var out := ArrayMesh.new()
	for b in mesh.get_blend_shape_count(): out.add_blend_shape(mesh.get_blend_shape_name(b))
	out.blend_shape_mode = mesh.blend_shape_mode
	for i in im.get_surface_count():
		var arrays = im.get_surface_arrays(i)
		var full: PackedInt32Array = arrays[Mesh.ARRAY_INDEX]
		var best: PackedInt32Array = full
		var want := int(full.size() * ratio)
		for l in im.get_surface_lod_count(i):
			var idx := im.get_surface_lod_indices(i, l)
			if abs(idx.size() - want) < abs(best.size() - want): best = idx
		arrays[Mesh.ARRAY_INDEX] = best
		var shapes := []
		for b in im.get_blend_shape_count(): shapes.append(im.get_surface_blend_shape_arrays(i, b))
		out.add_surface_from_arrays(im.get_surface_primitive_type(i), arrays, shapes, {}, mesh.surface_get_format(i) & ~Mesh.ARRAY_FORMAT_VERTEX & ~Mesh.ARRAY_FORMAT_NORMAL & ~Mesh.ARRAY_FORMAT_INDEX)
		out.surface_set_material(i, im.get_surface_material(i))
		out.surface_set_name(i, im.get_surface_name(i))
		print("  surface %d: %d -> %d tris" % [i, full.size() / 3, best.size() / 3])
	return out
func _init():
	var src := OS.get_environment("SRC")
	var ratio := float(OS.get_environment("RATIO"))
	var root = load(src).instantiate()
	var done := {}
	for mi in root.find_children("*", "MeshInstance3D", true, false):
		var mesh = mi.mesh
		if not (mesh is ArrayMesh) or not mesh.resource_path.contains("::") or done.has(mesh.resource_path):
			continue
		var id = mesh.resource_path.get_slice("::", 1)
		var out := _simplify(mesh, ratio)
		var path = src.get_basename() + "_" + id + ".res"
		ResourceSaver.save(out, path, ResourceSaver.FLAG_COMPRESS)
		done[mesh.resource_path] = true
		print("MESH %s %s" % [id, path])
	root.free()
	quit()
