#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/surface_tool.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/vector2.hpp>

using namespace godot;

class ExampleClass : public RefCounted {
	GDCLASS(ExampleClass, RefCounted)


private:
	MeshInstance3D *mesh_instance = nullptr;
	Ref<SurfaceTool> surface_tool;

	// Data gridu (doporučeno použít 1D pole pro výkon)
	std::vector<Vector3> vertices;
	std::vector<int> texture_indices;

	int GRID_SIZE = 256;
	int VERTEX_COUNT = GRID_SIZE + 1;
	float CELL_SCALE = 1.0f;

	void add_triangle(Vector3 p1, Vector3 p2, Vector3 p3, int idx1, int idx2, float weight,
			Vector2 uv1, Vector2 uv2, Vector2 uv3,
			float w1_1, float w1_2, float w1_3,
			float w2_1, float w2_2, float w2_3);

protected:
	static void _bind_methods();

public:
	ExampleClass() = default;
	~ExampleClass() override = default;

	PackedByteArray deRNC(PackedByteArray bytearray);
	void MakeTerrain(PackedByteArray bytearray);

	void TerrainMake(PackedByteArray bytearray);
	void RunGameStep(Dictionary inputs);
	Dictionary GetPlayerPositionRotation();
	PackedFloat32Array GetEntites();
	uint8_t TerrainGetTileTerrainType(int index);
	void TerrainSetTileTerrainType(int index, uint8_t value);
	PackedByteArray TerrainGetMapTerrainType();
	PackedByteArray TerrainGetMapHeight();
	PackedByteArray TerrainGetAngle();

	void set_mesh_instance(Node *p_node);
	void initialize_grid_data();
	void recalculate_mesh();
};
