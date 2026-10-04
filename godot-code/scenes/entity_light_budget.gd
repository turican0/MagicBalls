extends Node3D
# Keeps the number of entity lights low when there are a lot of them (Meteor, Volcano,
# Lightning make hundreds of fires/flashes, each with its own OmniLight3D). Up to MAX_LIGHTS
# nothing changes. Above that, lights of the same kind which are close to each other are
# replaced by one light in their middle with their summed energy - from a distance it lights
# the same. Near the camera the groups are small, further away they are bigger.

const MAX_LIGHTS := 64
const BASE_CELL := 2.0 # size of a group near the camera
const NEAR_DISTANCE := 16.0 # groups double in size with every doubling of the distance past this
const MAX_PASSES := 6
# how much of the group size is added to the range of a merged light
const RANGE_SPREAD := 0.5

var _merged_lights: Array = [] # our lights, one per group of entity lights
var _hidden: Dictionary = {} # entity lights we switched off -> true
var _used := 0

# lights: the OmniLight3D nodes of the entities drawn this frame
func update_lights(lights: Array, camera_position: Vector3) -> void:
	_used = 0
	if lights.size() <= MAX_LIGHTS:
		if not _hidden.is_empty():
			_restore_all()
		_hide_unused()
		return
	var groups := {}
	var cell := BASE_CELL
	for pass_index in MAX_PASSES:
		groups = _group(lights, camera_position, cell)
		if groups.size() <= MAX_LIGHTS:
			break
		cell *= 2.0
	var still_hidden := {}
	for key in groups:
		var group: Array = groups[key]
		if group.size() == 1:
			var light: OmniLight3D = group[0]
			if not light.visible:
				light.visible = true
			continue
		var center := Vector3.ZERO
		var energy := 0.0
		var indirect := 0.0
		var weight := 0.0
		for light in group:
			var w: float = max(light.light_energy, 0.001)
			center += light.global_position * w
			weight += w
			energy += light.light_energy
			indirect += light.light_indirect_energy
			if light.visible:
				light.visible = false
			still_hidden[light] = true
		center /= weight
		var spread := 0.0
		for light in group:
			spread = max(spread, center.distance_to(light.global_position))
		_set_merged(group[0], center, energy, indirect, spread * RANGE_SPREAD)
	# lights which were merged last frame but are on their own now
	for light in _hidden:
		if not still_hidden.has(light) and is_instance_valid(light) and not light.visible:
			light.visible = true
	_hidden = still_hidden
	_hide_unused()

func _group(lights: Array, camera_position: Vector3, cell: float) -> Dictionary:
	var groups := {}
	for light in lights:
		var p: Vector3 = light.global_position
		var distance := p.distance_to(camera_position)
		var size := cell
		if distance > NEAR_DISTANCE:
			size *= pow(2.0, floorf(log(distance / NEAR_DISTANCE) / log(2.0)) + 1.0)
		# only lights of the same kind are merged
		var key = [floori(p.x / size), floori(p.y / size), floori(p.z / size), size,
			light.light_color.to_rgba32(), light.omni_range, light.omni_attenuation]
		if groups.has(key):
			groups[key].append(light)
		else:
			groups[key] = [light]
	return groups

func _set_merged(model: OmniLight3D, center: Vector3, energy: float, indirect: float, spread: float) -> void:
	var light: OmniLight3D
	if _used < _merged_lights.size():
		light = _merged_lights[_used]
	else:
		light = OmniLight3D.new()
		light.top_level = true
		add_child(light)
		_merged_lights.append(light)
	_used += 1
	light.global_position = center
	light.light_color = model.light_color
	light.light_energy = energy
	light.light_indirect_energy = indirect
	light.light_specular = model.light_specular
	light.light_volumetric_fog_energy = model.light_volumetric_fog_energy
	light.light_size = model.light_size
	light.light_cull_mask = model.light_cull_mask
	light.omni_attenuation = model.omni_attenuation
	light.omni_range = model.omni_range + spread
	if not light.visible:
		light.visible = true

func _hide_unused() -> void:
	for i in range(_used, _merged_lights.size()):
		if _merged_lights[i].visible:
			_merged_lights[i].visible = false

func _restore_all() -> void:
	for light in _hidden:
		if is_instance_valid(light) and not light.visible:
			light.visible = true
	_hidden.clear()

func clear() -> void:
	_hidden.clear()
	_used = 0
	_hide_unused()
