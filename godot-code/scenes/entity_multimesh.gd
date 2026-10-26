extends Node3D
# Entities whose scene is only meshes (no script, animation, light, particles, skin,
# transparency - nothing whose look or behaviour depends on being its own node) are drawn with
# MultiMeshes instead of one node per entity: one MultiMeshInstance3D per mesh of the scene and
# per piece of the map, so they are still culled and get their LOD by where they are.
# This finds out which scenes can be drawn so and what their meshes are; EntityRenderer (C++)
# draws them.

const DRAW_SETTINGS := ["cast_shadow", "gi_mode", "layers", "lod_bias", "extra_cull_margin",
	"visibility_range_begin", "visibility_range_end", "visibility_range_begin_margin",
	"visibility_range_end_margin", "visibility_range_fade_mode", "ignore_occlusion_culling", "transparency"]

class Part:
	var mesh: Mesh
	var under_scale: bool # below the scene's "Scale" node, which the entity can rescale
	var local: Transform3D # to the root, or to the "Scale" node when under_scale
	var settings: Dictionary # how the mesh node draws, copied to its MultiMeshInstance3D

class Batch:
	var parts: Array = [] # of Part
	var scale_node: Transform3D # the "Scale" node as it is in the scene
	var has_scale_node := false

var _info: Dictionary = {} # PackedScene -> Batch, or null when it can not be batched
var _batch_ids: Dictionary = {} # PackedScene -> batch of EntityRenderer
var renderer: EntityRenderer # draws the MultiMeshes (decode_level.gd renderEntites)

func can_batch(scene: PackedScene) -> bool:
	if not _info.has(scene):
		_info[scene] = _make_batch(scene)
	return _info[scene] != null

# the batch of a scene which can_batch() in EntityRenderer, made the first time it is asked for
func batch_id(scene: PackedScene) -> int:
	if not _batch_ids.has(scene):
		var batch: Batch = _info[scene]
		var parts := []
		for part in batch.parts:
			parts.append({"mesh": part.mesh, "under_scale": part.under_scale, "local": part.local, "settings": part.settings})
		_batch_ids[scene] = renderer.register_batch(parts, batch.scale_node, batch.has_scale_node)
	return _batch_ids[scene]

func _make_batch(scene: PackedScene) -> Batch:
	var root: Node = scene.instantiate()
	var batch := Batch.new()
	var ok := root is Node3D and root.get_script() == null and _collect(root, root, batch)
	if ok and batch.parts.is_empty():
		ok = false
	root.free()
	return batch if ok else null

func _collect(node: Node, root: Node3D, batch: Batch) -> bool:
	for child in node.get_children():
		if child.get_script() != null:
			return false
		if child is MeshInstance3D:
			if not _add_part(child, root, batch):
				return false
		elif child.get_class() != "Node3D":
			return false
		if not _collect(child, root, batch):
			return false
	return true

func _add_part(mi: MeshInstance3D, root: Node3D, batch: Batch) -> bool:
	if mi.skin != null or mi.material_override != null or mi.mesh == null:
		return false
	var mesh: Mesh = mi.mesh
	var overridden := false
	for i in mesh.get_surface_count():
		var material = mi.get_active_material(i)
		if material != null and not _material_ok(material):
			return false
		if mi.get_surface_override_material(i) != null:
			overridden = true
	if overridden:
		# a MultiMesh draws the mesh's own materials - give it a copy with the overrides in it
		mesh = mesh.duplicate()
		for i in mesh.get_surface_count():
			if mi.get_surface_override_material(i) != null:
				mesh.surface_set_material(i, mi.get_surface_override_material(i))
	var part := Part.new()
	part.mesh = mesh
	for property in DRAW_SETTINGS:
		part.settings[property] = mi.get(property)
	var scale_node = root.get_node_or_null("Scale")
	if scale_node and scale_node.is_ancestor_of(mi):
		part.under_scale = true
		part.local = _relative(mi, scale_node)
		batch.has_scale_node = true
		batch.scale_node = scale_node.transform
	else:
		part.local = _relative(mi, root)
	batch.parts.append(part)
	return true

func _material_ok(material: Material) -> bool:
	if material is BaseMaterial3D:
		# drawn in a different order in a MultiMesh, and billboards turn per instance differently
		return material.transparency == BaseMaterial3D.TRANSPARENCY_DISABLED \
			and material.billboard_mode == BaseMaterial3D.BILLBOARD_DISABLED
	return false

func _relative(node: Node3D, ancestor: Node3D) -> Transform3D:
	var t := Transform3D.IDENTITY
	var n: Node = node
	while n != ancestor:
		t = n.transform * t
		n = n.get_parent()
	return t
