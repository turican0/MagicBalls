#pragma once

#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>

#include <cstdint>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace godot;

// The entities of the engine (MBEXclass::GetEntites, 31 floats each) drawn every frame (called
// from decode_level.gd renderEntites): which scene draws an entity, its transform,
// the pooled nodes it takes, the MultiMeshes of mesh-only scenes, and hiding what is not used.
// GDScript still loads the scenes, makes the pool nodes and tells which scenes are batched
// (entity_multimesh.gd); it is asked only for what it has not told yet.
class EntityRenderer : public Node3D {
	GDCLASS(EntityRenderer, Node3D)

public:
	// constexpr: inline, so they have a definition (std::min takes them by reference - MSVC links
	// without one, the Android/Linux linker does not)
	static constexpr int STRIDE = 31;
	static constexpr int ENTITY_COUNT = 1000;

	enum KindType {
		KIND_NONE = 0, // not drawn
		KIND_NODE = 1, // one pooled node per entity
		KIND_BATCH = 2, // drawn by the MultiMeshes of a batch (entity_multimesh.gd can_batch)
	};

private:
	struct Kind {
		int type = 0;
		int batch = -1;
	};

	struct Bucket {
		Vector3i uid;
		std::vector<uint64_t> nodes;
		int active_count = 0;
		int act_index = 0;
		bool lights = false;
		bool particles = false;
		bool ran_out = false;
	};

	struct Part {
		Ref<Mesh> mesh;
		bool under_scale = false;
		Transform3D local;
		Dictionary settings;
	};

	struct Chunk {
		std::vector<uint64_t> instances; // MultiMeshInstance3D, one per part
		std::vector<PackedFloat32Array> buffers; // filled this frame
		std::vector<PackedFloat32Array> shown;
	};

	struct Batch {
		std::vector<Part> parts;
		Transform3D scale_node;
		bool has_scale_node = false;
		std::map<std::pair<int, int>, Chunk> chunks;
	};

	// what is used of one entity of the data array
	struct Entity {
		float x, y, z;
		int act_class, act_model, model_index;
		int byte0, byte1, byte2;
		int speed, mana, life, max_life;
		int bitmap_scale_help, bitmap_scale;
		float yaw_raw;
	};

	std::unordered_map<int64_t, Kind> kinds;
	std::vector<Bucket> buckets; // in the order they were made, like the Dictionary they replace
	std::unordered_map<int64_t, int> bucket_index;
	std::vector<Batch> batches;

	// the frame being drawn
	Transform3D level_inverse;
	Vector3 camera_position;
	bool has_camera = false;
	bool wizard_labels = false;
	int multimesh_instances = 0;

	// profiling (tools/optimization/godot/profile.gd), per kind of entity
	struct KindStats {
		Vector3i uid;
		int frames = 0; // frames it was drawn in
		int64_t drawn_sum = 0; // entities drawn (nodes or MultiMesh instances), summed over the frames
		int drawn_max = 0;
		double update_ms = 0; // update_node()
		double show_ms = 0; // finish_frame() for its pool
		double show_max_ms = 0;
		int show_spikes = 0; // frames its finish_frame() took over 1 ms
		int first_frame = -1; // profiled frame it was first drawn in
	};
	int profile_frame = 0; // frames since profiling was switched on
	bool profiling = false;
	std::unordered_map<int64_t, KindStats> kind_stats;
	std::unordered_map<int64_t, int> drawn_this_frame;
	std::unordered_map<int64_t, int> drawn_last_frame;
	std::unordered_map<int64_t, bool> hidden_kinds; // not drawn - to measure what a kind costs
	KindStats &stats_of(const Vector3i &uid);

	static int64_t key_of(int act_class, int model, int lib_type);
	static int64_t key_of(const Vector3i &uid);
	static Entity read_entity(const float *data, int index);
	static Vector3i uid_of(const Entity &e, bool &skip);
	Bucket *bucket_of(const Vector3i &uid);
	Bucket &make_bucket(const Vector3i &uid);
	void draw_batch(const Entity &e, const Kind &kind);
	void update_node(Node3D *node, const Entity &e);
	void update_multimeshes();
	Chunk &chunk_of(Batch &batch, const std::pair<int, int> &key);

protected:
	static void _bind_methods();

public:
	int register_batch(const Array &parts, const Transform3D &scale_node, bool has_scale_node);
	void set_kind(const Vector3i &uid, int type, int batch);
	bool has_kind(const Vector3i &uid) const;
	void forget_kinds();
	void add_pool_node(const Vector3i &uid, Node3D *node, bool lights, bool particles, bool ran_out);
	int pool_size(const Vector3i &uid) const;
	Array unknown_kinds(const PackedFloat32Array &data) const;
	Dictionary render(const PackedFloat32Array &data, const Transform3D &level_inverse, const Vector3 &camera_position, bool has_camera, bool wizard_labels);
	void render_missing(const PackedFloat32Array &data, const PackedInt32Array &indices);
	Dictionary finish_frame();
	int active_node_count() const;
	int multimesh_instance_count() const { return multimesh_instances; }
	void clear();
	void set_profiling(bool enabled) { profiling = enabled; }
	Array get_kind_stats() const;
	int drawn_last_frame_of(const Vector3i &uid) const;
	void set_hidden_kinds(const Array &uids);
};
