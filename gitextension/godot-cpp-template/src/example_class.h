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

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

#include <godot_cpp/classes/shader_material.hpp>

using namespace godot;

class ExampleClass : public RefCounted {
	GDCLASS(ExampleClass, RefCounted)


private:
	MeshInstance3D *mesh_instance = nullptr;
	Ref<SurfaceTool> surface_tool;

	// Data gridu (doporučeno použít 1D pole pro výkon)
	//std::vector<Vector3> vertices;
	//std::vector<int> texture_indices;

	static const int GRID_SIZE = 256;
	int VERTEX_COUNT = GRID_SIZE + 1;
	float CELL_SCALE = 1.0f;

	PackedByteArray control_data;
	std::vector<float> height_data;
	int texture_indices[GRID_SIZE][GRID_SIZE];

	Ref<Image> height_image;
	Ref<ImageTexture> height_texture;
	Ref<Image> control_image;
	Ref<ImageTexture> control_texture;

	void add_triangle(Vector3 p1, Vector3 p2, Vector3 p3, Vector2 uv1, Vector2 uv2, Vector2 uv3,
			Vector2 grid_p1, Vector2 grid_p2, Vector2 grid_p3, Vector2 main_p);

protected:
	static void _bind_methods();

public:
	ExampleClass() = default;
	~ExampleClass() override = default;

	PackedByteArray deRNC(PackedByteArray bytearray);
	//void MakeTerrain(PackedByteArray bytearray);

	void TerrainMake(PackedByteArray bytearray, String cdPath);
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
	void renew_terrain();
	void update_gpu_heightmap();
	void update_gpu_controlmap();
	void initialize_controlmap();
	void initialize_heightmap();
	Array getActiveSpells();
	Array getSelectedSpells();
	void setPlayerActiveSpell(int spell_index, int button);
	Array getPaletteModifications();
	Ref<Image> getMinimap();
	void convertOriginalData(String path, String path2);
	void convertOriginalDataExtractCD(String path, String path2);
	Array getPendingSoundActions();
	Array getPendingGraphicsActions();
	//void soundQueueClear();
	void updateFreeSoundPlayers(const godot::Array &p_indices);
	void playAnim(int index);
	Array getTexts();
	PackedByteArray getVGABuffer();
	int playAnimStep(int run);
	void mapMenuInit();
	int mapMenuStep(int run);
	};
