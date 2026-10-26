#include "entity_renderer.h"

#include <godot_cpp/classes/multi_mesh.hpp>
#include <godot_cpp/classes/multi_mesh_instance3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <algorithm>
#include <cmath>

namespace {
const double INV_256 = 1.0 / 256.0;
const double RAD_MULT = Math_PI / 1024.0;
const double CHUNK_SIZE = 32.0; // the MultiMeshes are split into pieces of the map this big

bool is_wizard_model(int model) {
	return model == 203 || model == 211 || model == 219 || model == 227 || model == 235 || model == 243 || model == 251;
}

// the scale of the entity node (and of its MultiMesh instance)
double entity_scale(int act_class, int act_model, int mana, int bitmap_scale) {
	if (act_class == 10 && act_model == 39) // mana sphere
		return std::pow((double)mana, 1.0 / 3.0) * 0.1;
	if (act_class == 5 && act_model == 22) // mana sphere of the mana snake
		return bitmap_scale * INV_256;
	return 1.0;
}

// the coordinate moved next to the camera, the map wraps around every 256 units
double wrap_to_camera(double value, double camera) {
	return camera + Math::fposmod(value - camera + 128.0, 256.0) - 128.0;
}
} // namespace

void EntityRenderer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("register_batch", "parts", "scale_node", "has_scale_node"), &EntityRenderer::register_batch);
	ClassDB::bind_method(D_METHOD("set_kind", "uid", "type", "batch"), &EntityRenderer::set_kind);
	ClassDB::bind_method(D_METHOD("has_kind", "uid"), &EntityRenderer::has_kind);
	ClassDB::bind_method(D_METHOD("forget_kinds"), &EntityRenderer::forget_kinds);
	ClassDB::bind_method(D_METHOD("add_pool_node", "uid", "node", "lights", "particles", "ran_out"), &EntityRenderer::add_pool_node);
	ClassDB::bind_method(D_METHOD("pool_size", "uid"), &EntityRenderer::pool_size);
	ClassDB::bind_method(D_METHOD("unknown_kinds", "data"), &EntityRenderer::unknown_kinds);
	ClassDB::bind_method(D_METHOD("render", "data", "level_inverse", "camera_position", "has_camera", "wizard_labels"), &EntityRenderer::render);
	ClassDB::bind_method(D_METHOD("render_missing", "data", "indices"), &EntityRenderer::render_missing);
	ClassDB::bind_method(D_METHOD("finish_frame"), &EntityRenderer::finish_frame);
	ClassDB::bind_method(D_METHOD("active_node_count"), &EntityRenderer::active_node_count);
	ClassDB::bind_method(D_METHOD("multimesh_instance_count"), &EntityRenderer::multimesh_instance_count);
	ClassDB::bind_method(D_METHOD("clear"), &EntityRenderer::clear);
	BIND_CONSTANT(KIND_NONE);
	BIND_CONSTANT(KIND_NODE);
	BIND_CONSTANT(KIND_BATCH);
}

int64_t EntityRenderer::key_of(int act_class, int model, int lib_type) {
	return ((int64_t)act_class << 32) | ((int64_t)(model & 0xFFFFFF) << 8) | (lib_type & 0xFF);
}

int64_t EntityRenderer::key_of(const Vector3i &uid) {
	return key_of(uid.x, uid.y, uid.z);
}

EntityRenderer::Entity EntityRenderer::read_entity(const float *data, int index) {
	const float *d = data + index * STRIDE;
	Entity e;
	e.x = d[0];
	e.y = d[1];
	e.z = d[2];
	e.act_class = (int)d[7];
	e.act_model = (int)d[8];
	e.speed = (int)d[12];
	e.byte0 = (int)d[17];
	e.byte1 = (int)d[18];
	e.byte2 = (int)d[19];
	e.model_index = (int)d[21];
	e.yaw_raw = d[22];
	e.mana = (int)d[24];
	e.life = (int)d[25];
	e.max_life = (int)d[27];
	e.bitmap_scale_help = (int)d[29];
	e.bitmap_scale = (int)d[30];
	return e;
}

// the library key of the entity (class, model, 1 = library / 2 = library2), skip when not drawn
Vector3i EntityRenderer::uid_of(const Entity &e, bool &skip) {
	skip = (e.byte1 & 4) != 0;
	bool is_draw = (e.byte0 & 1) == 0;
	if (e.act_class == 3 && is_wizard_model(e.model_index) && e.life <= 0)
		is_draw = false; // dead sorcerers
	const int c = e.act_class;
	const bool library1 = is_draw && (c == 2 || c == 3 || c == 5 || c == 9 || c == 10 || c == 15);
	return Vector3i(c, e.model_index, library1 ? 1 : 2);
}

EntityRenderer::Bucket *EntityRenderer::bucket_of(const Vector3i &uid) {
	auto it = bucket_index.find(key_of(uid));
	return it == bucket_index.end() ? nullptr : &buckets[it->second];
}

EntityRenderer::Bucket &EntityRenderer::make_bucket(const Vector3i &uid) {
	if (Bucket *b = bucket_of(uid))
		return *b;
	bucket_index[key_of(uid)] = (int)buckets.size();
	buckets.emplace_back();
	buckets.back().uid = uid;
	return buckets.back();
}

int EntityRenderer::register_batch(const Array &parts, const Transform3D &scale_node, bool has_scale_node) {
	Batch batch;
	batch.scale_node = scale_node;
	batch.has_scale_node = has_scale_node;
	for (int i = 0; i < parts.size(); i++) {
		Dictionary p = parts[i];
		Part part;
		part.mesh = p["mesh"];
		part.under_scale = p["under_scale"];
		part.local = p["local"];
		part.settings = p["settings"];
		batch.parts.push_back(part);
	}
	batches.push_back(std::move(batch));
	return (int)batches.size() - 1;
}

void EntityRenderer::set_kind(const Vector3i &uid, int type, int batch) {
	Kind kind;
	kind.type = type;
	kind.batch = batch;
	kinds[key_of(uid)] = kind;
}

bool EntityRenderer::has_kind(const Vector3i &uid) const {
	return kinds.count(key_of(uid)) != 0;
}

// the library changed (updateLibrary) - its scenes are asked for again
void EntityRenderer::forget_kinds() {
	kinds.clear();
}

void EntityRenderer::add_pool_node(const Vector3i &uid, Node3D *node, bool lights, bool particles, bool ran_out) {
	if (!node)
		return;
	Bucket &b = make_bucket(uid);
	b.nodes.push_back(node->get_instance_id());
	b.lights = b.lights || lights;
	b.particles = b.particles || particles;
	b.ran_out = b.ran_out || ran_out;
}

int EntityRenderer::pool_size(const Vector3i &uid) const {
	auto it = bucket_index.find(key_of(uid));
	return it == bucket_index.end() ? 0 : (int)buckets[it->second].nodes.size();
}

// [uid, entity index] of the kinds drawn this frame which GDScript has not told yet
Array EntityRenderer::unknown_kinds(const PackedFloat32Array &data) const {
	Array result;
	std::unordered_map<int64_t, bool> seen;
	const int count = std::min<int>(ENTITY_COUNT, (int)(data.size() / STRIDE));
	const float *d = data.ptr();
	for (int i = 0; i < count; i++) {
		const Entity e = read_entity(d, i);
		bool skip;
		const Vector3i uid = uid_of(e, skip);
		if (skip)
			continue;
		const int64_t key = key_of(uid);
		if (kinds.count(key) || seen.count(key))
			continue;
		seen[key] = true;
		Array item;
		item.push_back(uid);
		item.push_back(i);
		result.push_back(item);
	}
	return result;
}

// Draws the entities. Returns "missing" / "missing_uids" (entities of a node kind whose pool ran
// out - GDScript makes their nodes and calls render_missing) and "player" ([speed, life, mana]).
Dictionary EntityRenderer::render(const PackedFloat32Array &data, const Transform3D &p_level_inverse, const Vector3 &p_camera_position, bool p_has_camera, bool p_wizard_labels) {
	level_inverse = p_level_inverse;
	camera_position = p_camera_position;
	has_camera = p_has_camera;
	wizard_labels = p_wizard_labels;
	Dictionary result;
	PackedInt32Array missing;
	Array missing_uids;
	PackedInt32Array player;
	const int count = std::min<int>(ENTITY_COUNT, (int)(data.size() / STRIDE));
	const float *d = data.ptr();
	for (int i = 0; i < count; i++) {
		const Entity e = read_entity(d, i);
		if (e.model_index == 0 && e.act_class == 3) {
			player.resize(3);
			player.set(0, e.speed);
			player.set(1, e.life);
			player.set(2, e.mana);
		}
		bool skip;
		const Vector3i uid = uid_of(e, skip);
		if (skip)
			continue;
		auto it = kinds.find(key_of(uid));
		if (it == kinds.end())
			continue; // its scene is still loading
		if (it->second.type == KIND_BATCH) {
			draw_batch(e, it->second);
			continue;
		}
		if (it->second.type != KIND_NODE)
			continue; // not drawn (no scene in the library)
		Bucket *b = bucket_of(uid);
		if (!b || b->act_index >= (int)b->nodes.size()) {
			missing.push_back(i);
			missing_uids.push_back(uid);
			continue;
		}
		Node3D *node = Object::cast_to<Node3D>(ObjectDB::get_instance(b->nodes[b->act_index]));
		b->act_index++;
		b->active_count++;
		if (node)
			update_node(node, e);
	}
	update_multimeshes();
	result["missing"] = missing;
	result["missing_uids"] = missing_uids;
	result["player"] = player;
	return result;
}

// the entities of render()'s "missing", after GDScript added a node for each of them
void EntityRenderer::render_missing(const PackedFloat32Array &data, const PackedInt32Array &indices) {
	const float *d = data.ptr();
	for (int n = 0; n < indices.size(); n++) {
		const Entity e = read_entity(d, indices[n]);
		bool skip;
		Bucket *b = bucket_of(uid_of(e, skip));
		if (!b || b->act_index >= (int)b->nodes.size())
			continue;
		Node3D *node = Object::cast_to<Node3D>(ObjectDB::get_instance(b->nodes[b->act_index]));
		b->act_index++;
		b->active_count++;
		if (node)
			update_node(node, e);
	}
}

// Only what changes is written: every write goes on to the renderer, and most entities stand still.
void EntityRenderer::update_node(Node3D *node, const Entity &e) {
	if (is_wizard_model(e.model_index))
		node->call("updateHealthBar", e.life, e.max_life, wizard_labels);
	if (e.model_index == 411) { // zombie
		if ((e.byte2 & 0x80) == 0x80)
			node->call("fade_out");
		else
			node->call("fade_in");
	}
	if (e.bitmap_scale_help) {
		if (Node3D *scale_node = Object::cast_to<Node3D>(node->get_node_or_null(NodePath("Scale")))) {
			const double s = e.bitmap_scale * INV_256;
			const Vector3 bitmap_scale(s, s, s);
			if (scale_node->get_scale() != bitmap_scale)
				scale_node->set_scale(bitmap_scale);
		}
	}
	const double es = entity_scale(e.act_class, e.act_model, e.mana, e.bitmap_scale);
	const Vector3 new_scale(es, es, es);
	if (node->get_scale() != new_scale)
		node->set_scale(new_scale);
	const double base_x = e.x * INV_256;
	const double base_y = e.z * INV_256;
	const double base_z = e.y * INV_256;
	if (has_camera) {
		const Vector3 new_global(wrap_to_camera(base_x, camera_position.x), base_y, wrap_to_camera(base_z, camera_position.z));
		if (node->get_global_position() != new_global)
			node->set_global_position(new_global);
	} else {
		const Vector3 new_local(base_x, base_y, base_z);
		if (node->get_position() != new_local)
			node->set_position(new_local);
	}
	const Vector3 new_rotation(0, -(double)e.yaw_raw * RAD_MULT, 0);
	if (node->get_rotation() != new_rotation)
		node->set_rotation(new_rotation);
}

// The instance of every part of the scene, in the piece of the map the entity is in.
void EntityRenderer::draw_batch(const Entity &e, const Kind &kind) {
	if (kind.batch < 0 || kind.batch >= (int)batches.size())
		return;
	Batch &batch = batches[kind.batch];
	const double mm_scale = entity_scale(e.act_class, e.act_model, e.mana, e.bitmap_scale);
	Vector3 position(e.x * INV_256, e.z * INV_256, e.y * INV_256);
	if (has_camera) {
		position.x = wrap_to_camera(position.x, camera_position.x);
		position.z = wrap_to_camera(position.z, camera_position.z);
		position = level_inverse.xform(position);
	}
	const Basis basis = Basis::from_euler(Vector3(0, -(double)e.yaw_raw * RAD_MULT, 0)).scaled(Vector3(mm_scale, mm_scale, mm_scale));
	const Transform3D root(basis, position);
	Transform3D scale_xform = batch.scale_node;
	if (batch.has_scale_node && e.bitmap_scale_help) {
		const double bs = e.bitmap_scale * INV_256;
		scale_xform = Transform3D(Basis(batch.scale_node.basis.get_rotation_quaternion()).scaled(Vector3(bs, bs, bs)), batch.scale_node.origin);
	}
	const std::pair<int, int> key((int)std::floor(root.origin.x / CHUNK_SIZE), (int)std::floor(root.origin.z / CHUNK_SIZE));
	Chunk &chunk = chunk_of(batch, key);
	for (size_t i = 0; i < batch.parts.size(); i++) {
		const Part &part = batch.parts[i];
		const Transform3D t = part.under_scale ? root * scale_xform * part.local : root * part.local;
		const Basis &b = t.basis;
		PackedFloat32Array &buffer = chunk.buffers[i];
		const float values[12] = { (float)b.rows[0][0], (float)b.rows[0][1], (float)b.rows[0][2], (float)t.origin.x,
			(float)b.rows[1][0], (float)b.rows[1][1], (float)b.rows[1][2], (float)t.origin.y,
			(float)b.rows[2][0], (float)b.rows[2][1], (float)b.rows[2][2], (float)t.origin.z };
		const int64_t at = buffer.size();
		buffer.resize(at + 12);
		float *w = buffer.ptrw() + at;
		for (int v = 0; v < 12; v++)
			w[v] = values[v];
	}
}

EntityRenderer::Chunk &EntityRenderer::chunk_of(Batch &batch, const std::pair<int, int> &key) {
	auto it = batch.chunks.find(key);
	if (it != batch.chunks.end())
		return it->second;
	Chunk &chunk = batch.chunks[key];
	for (const Part &part : batch.parts) {
		MultiMeshInstance3D *mmi = memnew(MultiMeshInstance3D);
		Ref<MultiMesh> mm;
		mm.instantiate();
		mm->set_transform_format(MultiMesh::TRANSFORM_3D);
		mm->set_mesh(part.mesh);
		mmi->set_multimesh(mm);
		const Array keys = part.settings.keys();
		for (int k = 0; k < keys.size(); k++)
			mmi->set(keys[k], part.settings[keys[k]]);
		add_child(mmi);
		chunk.instances.push_back(mmi->get_instance_id());
		chunk.buffers.push_back(PackedFloat32Array());
		chunk.shown.push_back(PackedFloat32Array());
	}
	return chunk;
}

// after all entities of the frame were added
void EntityRenderer::update_multimeshes() {
	multimesh_instances = 0;
	for (Batch &batch : batches) {
		for (auto &entry : batch.chunks) {
			Chunk &chunk = entry.second;
			for (size_t i = 0; i < chunk.instances.size(); i++) {
				PackedFloat32Array &buffer = chunk.buffers[i];
				const int count = (int)(buffer.size() / 12);
				multimesh_instances += count;
				if (buffer != chunk.shown[i]) { // most of them stand still
					if (MultiMeshInstance3D *mmi = Object::cast_to<MultiMeshInstance3D>(ObjectDB::get_instance(chunk.instances[i]))) {
						Ref<MultiMesh> mm = mmi->get_multimesh();
						if (mm->get_instance_count() != count)
							mm->set_instance_count(count);
						if (count > 0)
							mm->set_buffer(buffer);
						mmi->set_visible(count > 0);
					}
					chunk.shown[i] = buffer;
				}
				chunk.buffers[i] = PackedFloat32Array();
			}
		}
	}
}

// Shows the nodes used this frame and hides the others.
// Returns the used nodes of the kinds with particles / lights (their metas are read by GDScript)
// and [uid, active count] of the kinds which ran out of nodes.
Dictionary EntityRenderer::finish_frame() {
	Array particle_nodes;
	Array light_nodes;
	Array ran_out;
	bool erased = false;
	for (size_t bi = 0; bi < buckets.size();) {
		Bucket &b = buckets[bi];
		const int active_count = b.active_count;
		if (b.particles) {
			const int n = std::min<int>(active_count, (int)b.nodes.size());
			for (int i = 0; i < n; i++)
				if (Object *node = ObjectDB::get_instance(b.nodes[i]))
					particle_nodes.push_back(node);
		}
		// entites with a start script are freed instead of pooled, so they start again next time
		const bool free_inactive = b.uid.x == 14 && (b.uid.y == 461 || b.uid.y == 462);
		if (b.ran_out) {
			b.ran_out = false;
			Array item;
			item.push_back(b.uid);
			item.push_back(active_count);
			ran_out.push_back(item);
		}
		for (int i = (int)b.nodes.size() - 1; i >= 0; i--) {
			Node3D *node = Object::cast_to<Node3D>(ObjectDB::get_instance(b.nodes[i]));
			if (!node) {
				b.nodes.erase(b.nodes.begin() + i);
				continue;
			}
			const bool active = i < active_count;
			if (!active && free_inactive) {
				node->queue_free();
				b.nodes.erase(b.nodes.begin() + i);
				continue;
			}
			if (node->is_visible() != active)
				node->set_visible(active);
			if (node->is_processing() != active)
				node->set_process(active);
			if (node->is_physics_processing() != active)
				node->set_physics_process(active);
		}
		if (b.nodes.empty()) {
			buckets.erase(buckets.begin() + bi);
			erased = true;
			continue;
		}
		if (b.lights) {
			const int n = std::min<int>(active_count, (int)b.nodes.size());
			for (int i = 0; i < n; i++)
				if (Object *node = ObjectDB::get_instance(b.nodes[i]))
					light_nodes.push_back(node);
		}
		b.act_index = 0;
		b.active_count = 0;
		bi++;
	}
	if (erased) {
		bucket_index.clear();
		for (size_t i = 0; i < buckets.size(); i++)
			bucket_index[key_of(buckets[i].uid)] = (int)i;
	}
	Dictionary result;
	result["particle_nodes"] = particle_nodes;
	result["light_nodes"] = light_nodes;
	result["ran_out"] = ran_out;
	return result;
}

int EntityRenderer::active_node_count() const {
	int count = 0;
	for (const Bucket &b : buckets)
		for (uint64_t id : b.nodes)
			if (Node3D *node = Object::cast_to<Node3D>(ObjectDB::get_instance(id)))
				if (node->is_visible())
					count++;
	return count;
}

// all nodes and MultiMeshes go (decode_level.gd clear_entites_pool, a new level)
void EntityRenderer::clear() {
	for (Bucket &b : buckets)
		for (uint64_t id : b.nodes)
			if (Node *node = Object::cast_to<Node>(ObjectDB::get_instance(id)))
				node->queue_free();
	buckets.clear();
	bucket_index.clear();
	for (Batch &batch : batches) {
		for (auto &entry : batch.chunks)
			for (uint64_t id : entry.second.instances)
				if (Node *node = Object::cast_to<Node>(ObjectDB::get_instance(id)))
					node->queue_free();
		batch.chunks.clear();
	}
	multimesh_instances = 0;
}
