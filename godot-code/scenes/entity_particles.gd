extends Node3D
# Draws the particles of many entities of the same kind (fires of Meteor, Volcano, ...) with
# one GPUParticles3D instead of one per entity. Every entity particle node which can be
# shared stops emitting itself and this emits the same particles, from its place, through the
# shared one - same material, mesh, speed and lifetime, so they look the same.

class Shared:
	var particles: GPUParticles3D
	var rate: float # particles per second of one entity
	var material: ParticleProcessMaterial

var _shared: Dictionary = {} # key -> Shared
var _emit_carry: Dictionary = {} # entity particle node -> fraction of a particle not emitted yet

# the start position and velocity are made here as the material would make them, because a
# particle emitted from a script does not get them from the material
const EMIT_FLAGS := GPUParticles3D.EMIT_FLAG_POSITION | GPUParticles3D.EMIT_FLAG_ROTATION_SCALE | GPUParticles3D.EMIT_FLAG_VELOCITY
const SHAPES := [ParticleProcessMaterial.EMISSION_SHAPE_POINT, ParticleProcessMaterial.EMISSION_SHAPE_SPHERE,
	ParticleProcessMaterial.EMISSION_SHAPE_SPHERE_SURFACE, ParticleProcessMaterial.EMISSION_SHAPE_BOX]
const MAX_PARTICLES := 20000 # per kind of particles - the oldest ones are reused above it

# Takes over the particles of an entity node when it can, returns the nodes it took over.
func register(node: Node) -> Array:
	var taken := []
	for p in node.find_children("*", "GPUParticles3D", true, false):
		if not _can_share(p):
			continue
		var key = _key_of(p)
		if not _shared.has(key):
			_shared[key] = _make_shared(p)
		p.emitting = false
		p.set_meta("shared_particles", key)
		taken.append(p)
	return taken

func _can_share(p: GPUParticles3D) -> bool:
	return p.emitting and not p.one_shot and p.explosiveness == 0.0 and p.randomness == 0.0 \
		and not p.local_coords and p.sub_emitter.is_empty() and not p.trail_enabled \
		and p.draw_passes == 1 and p.draw_pass_1 != null and p.get_script() == null \
		and p.process_material is ParticleProcessMaterial and p.lifetime > 0.0 \
		and p.preprocess == 0.0 and p.amount_ratio == 1.0 and _can_share_material(p.process_material)

# only what does not depend on where the emitter is (orbit / radial velocity go around it)
func _can_share_material(m: ParticleProcessMaterial) -> bool:
	return m.emission_shape in SHAPES and m.orbit_velocity_min == 0.0 and m.orbit_velocity_max == 0.0 \
		and m.radial_velocity_min == 0.0 and m.radial_velocity_max == 0.0 \
		and m.inherit_velocity_ratio == 0.0 \
		and not m.collision_mode and m.sub_emitter_mode == ParticleProcessMaterial.SUB_EMITTER_DISABLED \
		and m.velocity_limit_curve == null and m.directional_velocity_curve == null

func _key_of(p: GPUParticles3D) -> Array:
	return [p.process_material, p.draw_pass_1, p.material_override, p.lifetime, p.amount,
		p.speed_scale, p.fixed_fps, p.interpolate, p.fract_delta, p.draw_order, p.transform_align,
		p.cast_shadow, p.layers, p.sorting_offset]

func _make_shared(p: GPUParticles3D) -> Shared:
	var s := Shared.new()
	var g := GPUParticles3D.new()
	g.top_level = true
	g.local_coords = false
	g.process_material = p.process_material
	g.draw_pass_1 = p.draw_pass_1
	g.material_override = p.material_override
	g.lifetime = p.lifetime
	g.speed_scale = p.speed_scale
	g.fixed_fps = p.fixed_fps
	g.interpolate = p.interpolate
	g.fract_delta = p.fract_delta
	g.draw_order = p.draw_order
	g.transform_align = p.transform_align
	g.cast_shadow = p.cast_shadow
	g.layers = p.layers
	g.sorting_offset = p.sorting_offset
	g.amount = MAX_PARTICLES
	# emitted only through emit_particle(), from wherever the entities are
	g.emitting = false
	g.custom_aabb = AABB(Vector3(-100000, -100000, -100000), Vector3(200000, 200000, 200000))
	add_child(g)
	s.particles = g
	s.rate = p.amount / p.lifetime
	s.material = p.process_material
	return s

# particle_nodes: the taken over particle nodes of the entities drawn this frame
func emit(particle_nodes: Array, delta: float) -> void:
	var still := {}
	for p in particle_nodes:
		var s: Shared = _shared[p.get_meta("shared_particles")]
		var count: float = _emit_carry.get(p, 0.0) + s.rate * delta * s.particles.speed_scale
		var n := int(count)
		still[p] = count - n
		if n > 0:
			var xform: Transform3D = p.global_transform
			for i in n:
				var m := s.material
				var t := xform
				t.origin = xform * (_start_position(m) * m.emission_shape_scale + m.emission_shape_offset)
				var velocity := xform.basis * (_start_direction(m) * randf_range(m.initial_velocity_min, m.initial_velocity_max))
				s.particles.emit_particle(t, velocity, Color.WHITE, Color.BLACK, EMIT_FLAGS)
	# an entity which was not drawn starts again from nothing, as its own particles did
	_emit_carry = still

func _start_position(m: ParticleProcessMaterial) -> Vector3:
	match m.emission_shape:
		ParticleProcessMaterial.EMISSION_SHAPE_BOX:
			return Vector3(randf_range(-1, 1), randf_range(-1, 1), randf_range(-1, 1)) * m.emission_box_extents
		ParticleProcessMaterial.EMISSION_SHAPE_SPHERE:
			return _random_unit() * m.emission_sphere_radius * pow(randf(), 1.0 / 3.0)
		ParticleProcessMaterial.EMISSION_SHAPE_SPHERE_SURFACE:
			return _random_unit() * m.emission_sphere_radius
	return Vector3.ZERO

func _random_unit() -> Vector3:
	var z := randf_range(-1.0, 1.0)
	var a := randf() * TAU
	var r := sqrt(1.0 - z * z)
	return Vector3(r * cos(a), r * sin(a), z)

# a direction in the spread cone around the material's direction
func _start_direction(m: ParticleProcessMaterial) -> Vector3:
	var dir := m.direction.normalized() if m.direction != Vector3.ZERO else Vector3.RIGHT
	var spread := deg_to_rad(m.spread)
	if spread <= 0.0:
		return dir
	var cos_angle := randf_range(cos(spread), 1.0)
	var sin_angle := sqrt(1.0 - cos_angle * cos_angle)
	var around := randf() * TAU
	var side := dir.cross(Vector3.UP if abs(dir.y) < 0.99 else Vector3.RIGHT).normalized()
	var up := side.cross(dir)
	return dir * cos_angle + (side * cos(around) + up * sin(around)) * sin_angle

func clear() -> void:
	_emit_carry.clear()
	for s in _shared.values():
		s.particles.restart()
