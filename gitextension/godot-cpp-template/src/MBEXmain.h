#pragma once
#include <thread>

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

#include <godot_cpp/classes/texture_rect.hpp>

using namespace godot;

class MBEXclass : public RefCounted {
	GDCLASS(MBEXclass, RefCounted)


private:
	std::thread t2;
	String saved_real_cdPath;
	String saved_real_gamePath;
	int saved_argc;
	char *saved_argv[5];

	MeshInstance3D *mesh_instance_bottom = nullptr;
	MeshInstance3D *mesh_instance_top = nullptr;
	Ref<SurfaceTool> surface_tool;

	// Data gridu (doporučeno použít 1D pole pro výkon)
	//std::vector<Vector3> vertices;
	//std::vector<int> texture_indices;

	static const int GRID_SIZE = 256;
	int VERTEX_COUNT = GRID_SIZE + 1;
	float CELL_SCALE = 1.0f;

	PackedByteArray control_data;
	std::vector<float> height_data_bottom;
	std::vector<float> height_data_top;
	int texture_indices[GRID_SIZE][GRID_SIZE];

	Ref<Image> height_image_bottom;
	Ref<ImageTexture> height_texture_bottom;
	Ref<Image> height_image_top;
	Ref<ImageTexture> height_texture_top;
	Ref<Image> control_image;
	Ref<ImageTexture> control_texture;

	void add_triangle(Vector3 p1, Vector3 p2, Vector3 p3, Vector2 uv1, Vector2 uv2, Vector2 uv3,
			Vector2 grid_p1, Vector2 grid_p2, Vector2 grid_p3, Vector2 main_p);

protected:
	static void _bind_methods();

public:
	MBEXclass() = default;
	~MBEXclass() override = default;

	PackedByteArray deRNC(PackedByteArray bytearray);
	//void MakeTerrain(PackedByteArray bytearray);

	//void TerrainMake(PackedByteArray bytearray, String cdPath);
	void RunGameStep(Dictionary inputs);
	Dictionary GetPlayerPositionRotation();
	PackedFloat32Array GetEntites();
	uint8_t TerrainGetTileTerrainType(int index);
	void TerrainSetTileTerrainType(int index, uint8_t value);
	PackedByteArray TerrainGetMapTerrainType();
	PackedByteArray TerrainGetMapHeight(int index);
	PackedByteArray TerrainGetAngle();

	void set_mesh_instances(Node *p_node_bottom, Node *p_node_top, bool isCave);
	//void initialize_grid_data();
	void recalculate_mesh(bool isCave);
	void renew_terrain(bool isCave);
	void update_gpu_heightmap(bool isCave);
	void update_gpu_controlmap();
	void initialize_controlmap(bool isCave);
	void initialize_heightmap(int index);
	Array getActiveSpells();
	//Array getActiveSubSpells();
	Array getSelectedSpells();
	void setPlayerActiveSpell(int spell_index, int button);
	void setPlayerActiveSubSpell(int spell_index, int sub_spell_index, int button);
	Array getPaletteModifications();
	void convertOriginalData(String path, String path2);
	bool convertOriginalDataExtractCD(String path, String path2);
	Array getPendingSoundActions();
		//void soundQueueClear();
	void updateFreeSoundPlayers(const godot::Array &p_indices);
	PackedByteArray getVGABuffer();
	//void mapMenuInit();
	//int mapMenuStep(int run);
	Dictionary getLangTexts();
	int initLanguage(int index);
	void changeLanguage(int index);

	void REMC2BeginGame(String cdPath, String gamePath, int CustomLevel = -1, String CustomLevelPath = "");
	void REMC2EndGame();
	//void REMC2BeginItem();
	//void REMC2EndItem();
	String REMC2GetLevelType();
	void REMC2SetLevelType(String level);
	bool REMC2GetWebInfo();

	bool REMC2IsHiddenLevel();


	void REMC2SetScrBuffer(TextureRect *scrBufferRect);
	//void REMC2SetCDPath(String cdPath);
	int REMC2Run(Dictionary inputs, int stage);

	int REMC2GetGraphicsEenhance();

	Dictionary REMC2getWarpMouse();
	void REMC2SetInverseMouse(bool locInverse_mouseY);
	void MBEXfixLang(char *path, int index);

	int REMC2GetTerrainAlt(int x, int y);

	void REMC2EditorBegin(String cdPath);
	void REMC2EditorEnd();
	void REMC2EditorLoop();

	//int REMC2EditorGetTerrainValue(int type);
	//void REMC2EditorSetTerrainValue(int type, int value);

	//PackedFloat32Array REMC2EditorGetTerrainEntites();
	//PackedFloat32Array REMC2EditorGetTerrainPlayers();
	//PackedFloat32Array REMC2EditorGetTerrainStages();
	bool REMC2EditorIsGroupType(int type, int subtype);
	bool REMC2EditorIsParentType(int type, int subtype);
	
	int REMC2EditorDeleteEntites(Array p_indices);

	Dictionary REMC2EditorGetLevelData();
	void REMC2EditorSetLevelData(Dictionary d);

	void REMC2EditorExportToCSV();

	void REMC2EditorUndo();
	void REMC2EditorRedo();
	void REMC2EditorSaveState();
	void REMC2EditorTimedSaveState(float time);

	bool REMC2EditorLoadLevel(String path);
	void REMC2EditorSaveLevel(String path);

	void REMC2EditorCleanLevel();
	void REMC2EditorLoadInGameLevel(int levelIndex);

	bool REMC2GetMapMode();
};

void MBEXaudioExtract(String path);
