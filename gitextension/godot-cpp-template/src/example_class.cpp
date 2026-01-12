#include "example_class.h"
#include "remc2/utilities/DataFileRNC.h"
#include "remc2/engine/Terrain.h"
#include "remc2/engine/ConvertMapInfo.h"
#include "remc2/engine/EventsFunctions.h"
#include "remc2/engine/LevelInit.h"
#include "remc2/engine/Level.h"
#include "remc2/engine/DatTabIndexes.h"
#include "remc2/sub_main.h"
#include "remc2/engine/ReadAndDecompress.h"
#include "remc2/engine/MenusAndIntros.h"
#include "remc2/engine/Basic.h"
#include "remc2/engine/GameUI.h"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node3d.hpp>

void ExampleClass::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("deRNC", "bytearray"), &ExampleClass::deRNC);
	godot::ClassDB::bind_method(D_METHOD("TerrainMake", "bytearray"), &ExampleClass::TerrainMake);
	godot::ClassDB::bind_method(D_METHOD("TerrainGetMapHeight"), &ExampleClass::TerrainGetMapHeight);
	godot::ClassDB::bind_method(D_METHOD("TerrainGetMapTerrainType"), &ExampleClass::TerrainGetMapTerrainType);
	godot::ClassDB::bind_method(D_METHOD("TerrainGetAngle"), &ExampleClass::TerrainGetAngle);
	godot::ClassDB::bind_method(D_METHOD("RunGameStep", "Dictionary"), &ExampleClass::RunGameStep);
	godot::ClassDB::bind_method(D_METHOD("GetEntites"), &ExampleClass::GetEntites);
	godot::ClassDB::bind_method(D_METHOD("GetPlayerPositionRotation"), &ExampleClass::GetPlayerPositionRotation);
	godot::ClassDB::bind_method(D_METHOD("set_mesh_instance", "Node3D"), &ExampleClass::set_mesh_instance);
	godot::ClassDB::bind_method(D_METHOD("initialize_grid_data"), &ExampleClass::initialize_grid_data);
	godot::ClassDB::bind_method(D_METHOD("recalculate_mesh"), &ExampleClass::recalculate_mesh);
	godot::ClassDB::bind_method(D_METHOD("renew_terrain"), &ExampleClass::renew_terrain);
	godot::ClassDB::bind_method(D_METHOD("getActiveSpells"), &ExampleClass::getActiveSpells);
	godot::ClassDB::bind_method(D_METHOD("getSelectedSpells"), &ExampleClass::getSelectedSpells);
	godot::ClassDB::bind_method(D_METHOD("setPlayerActiveSpell", "Int", "Int"), &ExampleClass::setPlayerActiveSpell);
}

typedef struct {
	int spellIndex;
	bool glow;
	int spellLevel;
	int mana1;
	int mana2;
} drawSpellInfoType;

void ExampleClass::setPlayerActiveSpell(int spell_index, int button) {
	switch (button) {
		case 0: //left
		{
			type_event_0x6E8E *actEvent = ENTITY_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
			actEvent->dword_0xA4_164x->str_611.leftSpellIndex_0x451_1105 = spell_index;
			actEvent->dword_0xA4_164x->str_611.leftSubSpellIndex_0x455_1109 = actEvent->dword_0xA4_164x->str_611.array_0x437_1079x.subSpellIndex[spell_index];
			//actEvent->dword_0xA4_164x->str_611.leftSubSpellIndex_0x455_1109 = D41A0_0.array_0x6E3E[D41A0_0.LevelIndex_0xc].str_0x6E3E_byte2;
			x_D41A0_BYTEARRAY_4_struct.leftSpellPlayerIndex_38400 = 8;
			break;
		}
		case 1: //right
		{
			type_event_0x6E8E *actEvent = ENTITY_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
			actEvent->dword_0xA4_164x->str_611.rightSpellIndex_0x453_1107 = spell_index;
			actEvent->dword_0xA4_164x->str_611.rightSubSpellIndex_0x456_1110 = actEvent->dword_0xA4_164x->str_611.array_0x437_1079x.subSpellIndex[spell_index];
			//actEvent->dword_0xA4_164x->str_611.rightSubSpellIndex_0x456_1110 = D41A0_0.array_0x6E3E[D41A0_0.LevelIndex_0xc].str_0x6E3E_byte2;
			x_D41A0_BYTEARRAY_4_struct.rightSpellPlayerIndex_38401 = 8;
			break;
		}
	}
}

drawSpellInfoType getDraWSpellInfo(type_event_0x6E8E *playerEvent) //20f260
{
	drawSpellInfoType result;
	result.spellIndex = -1;
	if (playerEvent > ENTITY_EA3E4[0]) {
		GetFont_6FC50(FontType_D419D);
		type_event_0x6E8E *parentEvent = ENTITY_EA3E4[playerEvent->parentId_0x28_40];
		if (parentEvent > ENTITY_EA3E4[0]) {
			uint8_t color0 = playersColors_E88E0x[GetTrueWizardNumber_61790(parentEvent->dword_0xA4_164x->playerColorIndex_0x38_56)][0];
			uint8_t color1 = playersColors_E88E0x[GetTrueWizardNumber_61790(parentEvent->dword_0xA4_164x->playerColorIndex_0x38_56)][1];
			if (!(SPELLS_BEGIN_BUFFER_str[playerEvent->model_0x40_64].isEnabled_1 & 4) || playerEvent->word_0x2E_46 <= 0 || playerEvent->word_0x2E_46 >= 32 || !x_D41A0_BYTEARRAY_4_struct.colorIndex_121[1]) {
				if (playerEvent->word_0x2E_46)
					result.glow = true;
				else
					result.glow = false;
				result.spellIndex = playerEvent->model_0x40_64;
				result.spellLevel = playerEvent->byte_0x46_70;
				result.mana1 = 0;
				result.mana2 = 0;

				if (playerEvent->maxMana_0x8C_140) {
					result.mana1 = (parentEvent->mana_0x90_144 % playerEvent->maxMana_0x8C_140) / playerEvent->maxMana_0x8C_140;
					result.mana2 = parentEvent->mana_0x90_144 / playerEvent->maxMana_0x8C_140;
				}
				/*
				if (playerEvent->manaRegen_0x88_136)
					if (!parentEvent->dword_0xA4_164x->word_0x3A_58 || playerEvent->manaRegen_0x88_136 > ENTITY_EA3E4[parentEvent->dword_0xA4_164x->word_0x3A_58]->mana_0x90_144)
						if (D41A0_0.terrain_2FECE.MapType != MapType_t::Day)
							DrawSquareByColor_2E850(posX, posY, (*filearray_2aa18c[filearrayindex_MSPRD00DATTAB].posistruct)[SPELL_TOPTILE_BAR].width_4 * scale, (*filearray_2aa18c[filearrayindex_MSPRD00DATTAB].posistruct)[SPELL_TOPTILE_BAR].height_5 * scale, 16);
						else
							DrawSquareByColor_2E850(posX, posY, (*filearray_2aa18c[filearrayindex_MSPRD00DATTAB].posistruct)[SPELL_TOPTILE_BAR].width_4 * scale, (*filearray_2aa18c[filearrayindex_MSPRD00DATTAB].posistruct)[SPELL_TOPTILE_BAR].height_5 * scale, 48);
							*/
			}
		}
	}
	return result;
}

Array ExampleClass::getSelectedSpells() {
	Array result;
	drawSpellInfoType spellIndex;
	type_event_0x6E8E* playerEntity = ENTITY_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
	//if (x_D41A0_BYTEARRAY_4_struct.leftSpellPlayerIndex_38400) {
	spellIndex = getDraWSpellInfo(ENTITY_EA3E4[playerEntity->dword_0xA4_164x->str_611.array_0x333_819x.word[playerEntity->dword_0xA4_164x->str_611.leftSpellIndex_0x451_1105]]);
	Dictionary dl;
	dl["spellIndex"] = spellIndex.spellIndex;
	dl["glow"] = spellIndex.glow;
	dl["spellLevel"] = spellIndex.spellLevel;
	dl["glow"] = spellIndex.glow;
	dl["mana1"] = spellIndex.mana1;
	dl["mana2"] = spellIndex.mana2;
		result.append(dl);
	//}
	//if (x_D41A0_BYTEARRAY_4_struct.rightSpellPlayerIndex_38401) {
		spellIndex = getDraWSpellInfo(ENTITY_EA3E4[playerEntity->dword_0xA4_164x->str_611.array_0x333_819x.word[playerEntity->dword_0xA4_164x->str_611.rightSpellIndex_0x453_1107]]);
		Dictionary dr;
		dr["spellIndex"] = spellIndex.spellIndex;
		dr["glow"] = spellIndex.glow;
		dr["spellLevel"] = spellIndex.spellLevel;
		dr["glow"] = spellIndex.glow;
		dr["mana1"] = spellIndex.mana1;
		dr["mana2"] = spellIndex.mana2;
		result.append(dr);
	//}
	return result;
}

Array ExampleClass::getActiveSpells() {
	unsigned __int16 entityIndex;
	type_event_0x6E8E *playerEntity = ENTITY_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
	uint8 color0 = playersColors_E88E0x[GetTrueWizardNumber_61790(playerEntity->dword_0xA4_164x->playerColorIndex_0x38_56)][0];
	uint8 color1 = playersColors_E88E0x[GetTrueWizardNumber_61790(playerEntity->dword_0xA4_164x->playerColorIndex_0x38_56)][1];
	int spellIconIndex = 0;
	Array result;
	while (spellIconIndex < 26) {
		uint8_t spell_state=0;
		uint32_t spell_mana=0;
		int spellIndex2 = spellIndex_D94FF[spellIconIndex];
		if (SPELLS_BEGIN_BUFFER_str[spellIndex_D94FF[spellIconIndex]].byte_0 && !(!isCaveLevel_D41B6 && spellIndex2 == 25)) {
			type_event_0x6E8E *spellEntity = ENTITY_EA3E4[playerEntity->dword_0xA4_164x->str_611.array_0x333_819x.word[spellIndex_D94FF[spellIconIndex]]];
			if (spellEntity > ENTITY_EA3E4[0])
			{
				int subSpellIndex;
				if (spellIconIndex == playerEntity->dword_0xA4_164x->str_611.spellIndex_0x458_1112) {
					x_D41A0_BYTEARRAY_4_struct.spellOnCursor_50 = spellIconIndex;
					subSpellIndex = playerEntity->dword_0xA4_164x->str_611.subSpellIndex_0x459_1113;
				} else {
					subSpellIndex = playerEntity->dword_0xA4_164x->str_611.array_0x437_1079x.subSpellIndex[spellIndex_D94FF[spellIconIndex]];
				}
				bool skipToLabel43 = false;
				if (SPELLS_BEGIN_BUFFER_str[spellEntity->model_0x40_64].isEnabled_1 & 4) {
					if (spellEntity->word_0x2E_46 > 0 && spellEntity->word_0x2E_46 < 32 && x_D41A0_BYTEARRAY_4_struct.colorIndex_121[1]) {
						skipToLabel43 = true;
					}
				}
				if (!skipToLabel43) {
					bool canSummon = false;
					if (!SPELLS_BEGIN_BUFFER_str[spellIndex2].subspell[subSpellIndex].maxManaLimit_A || ((entityIndex = playerEntity->dword_0xA4_164x->word_0x3A_58) != 0 && SPELLS_BEGIN_BUFFER_str[spellIndex2].subspell[subSpellIndex].maxManaLimit_A <= ENTITY_EA3E4[entityIndex]->mana_0x90_144)) {
						canSummon = true;
					}
					if (canSummon /* && x_D41A0_BYTEARRAY_4_struct.byteindex_50 == spellIconIndex*/) {
						int manaCost = GetSpellManaCost_6D710(playerEntity, spellIndex2, subSpellIndex);
						if (manaCost > 0) {
							//DrawBitmap_2BB40(posX + posIconsX, posIconsY, (*filearray_2aa18c[filearrayindex_MSPRD00DATTAB].posistruct)[SPELL_TILE_BAR], scale);
							//DrawLine_2BC80(posX + posIconsX + (6 * scale), posIconsY + (28 * scale), (36 * scale) * (playerEntity->mana_0x90_144 % manaCost) / manaCost, (4 * scale), color1);
							//drawline color1-!!!!
							int manaPosX = playerEntity->mana_0x90_144 / manaCost;
							spell_mana = manaPosX;
							//draw manaPosX color0-!!!!
							/*
							for (int x = 0; x < 36 && manaPosX > 0; x += 2) {
								int y = 0;
								while (y < 4 && manaPosX > 0) {
									DrawLine_2BC80(x + posX + posIconsX + (6 * scale), y + posIconsY + (28 * scale), (2 * scale), (2 * scale), color0);
									//drawline color0-!!!!
									y += 2;
									manaPosX--;
								}
							}
							*/
						}
					}
					if (canSummon)
						spell_state=1; //draw standart - fireball
					else
						spell_state=2; //draw transaprent - fireball
				}
			} else {
				spell_state = 3;
				//draw spell icon colorized
			}
		}
		Dictionary d;
		d["spell_state"] = spell_state;
		d["spell_mana"] = spell_mana;
		result.append(d);
		spellIconIndex++;
	}
	return result;
}


void ExampleClass::set_mesh_instance(Node *p_node) {
	if (!p_node) {
		mesh_instance = nullptr;
		return;
	}

	// Bezpečné přetypování (ekvivalent "as MeshInstance3D" v GDScriptu)
	mesh_instance = Object::cast_to<MeshInstance3D>(p_node);

	if (!mesh_instance) {
		UtilityFunctions::printerr("Chyba: Předaný uzel není typu MeshInstance3D!");
	}
}

void ExampleClass::initialize_grid_data() {
}

void ExampleClass::recalculate_mesh() {
	surface_tool.instantiate();	
	surface_tool->begin(Mesh::PRIMITIVE_TRIANGLES);
	surface_tool->set_custom_format(0, SurfaceTool::CUSTOM_RGBA_FLOAT);

	for (int x = 0; x < GRID_SIZE; x++) {
		for (int y = 0; y < GRID_SIZE; y++) {
			//int angle_idx = (y % GRID_SIZE) * GRID_SIZE + (x % GRID_SIZE);
			//int textUV_42 = (mapAngle_13B4E0[angle_idx] >> 2) & 0x1C;

			Vector2 rPoint1 = Vector2(0, 0);
			Vector2 rPoint2 = Vector2(1, 0);
			Vector2 rPoint3 = Vector2(1, 1);
			Vector2 rPoint4 = Vector2(0, 1);

			Vector3 v1 = Vector3(x * CELL_SCALE, 0, y * CELL_SCALE);
			Vector3 v2 = Vector3((x + 1) * CELL_SCALE, 0, y * CELL_SCALE);
			Vector3 v3 = Vector3((x + 1) * CELL_SCALE, 0, (y + 1) * CELL_SCALE);
			Vector3 v4 = Vector3(x * CELL_SCALE, 0, (y + 1) * CELL_SCALE);

			Vector2 g1 = Vector2(x, y);
			Vector2 g2 = Vector2(x + 1, y);
			Vector2 g3 = Vector2(x + 1, y + 1);
			Vector2 g4 = Vector2(x, y + 1);

			if ((x + y + 1) & 1) {
				add_triangle(v1, v2, v3, rPoint1, rPoint2, rPoint3, g1, g2, g3, g1);
				add_triangle(v1, v3, v4, rPoint1, rPoint3, rPoint4, g1, g3, g4, g1);
			} else {
				add_triangle(v2, v3, v4, rPoint2, rPoint3, rPoint4, g2, g3, g4, g1);
				add_triangle(v2, v4, v1, rPoint2, rPoint4, rPoint1, g2, g4, g1, g1);
			}
		}
	}

	surface_tool->generate_normals();
	surface_tool->index();
	mesh_instance->set_mesh(surface_tool->commit());
	//renew_terrain();
}

void ExampleClass::renew_terrain() {
	if (height_image.is_null())
		initialize_heightmap();
	if (control_image.is_null())
		initialize_controlmap();

	if (control_data.size() != GRID_SIZE * GRID_SIZE * 4) {
		control_data.resize(GRID_SIZE * GRID_SIZE * 4);
	}
	uint8_t *cd_ptr = control_data.ptrw();

	for (int y = 0; y < GRID_SIZE; ++y) {
		for (int x = 0; x < GRID_SIZE; ++x) {
			int idx = (y % GRID_SIZE) * GRID_SIZE + (x % GRID_SIZE);
			int final_c = mapTerrainType_10B4E0[idx];
			int textUV_42 = (mapAngle_13B4E0[idx] >> 2) & 0x1C;
			int write_idx = (y * GRID_SIZE + x) * 4;
			cd_ptr[write_idx + 0] = (uint8_t)final_c;
			cd_ptr[write_idx + 1] = (uint8_t)textUV_42;
			cd_ptr[write_idx + 2] = 0;
			cd_ptr[write_idx + 3] = 0;
		}
	}

	// 3. Aktualizace Height dat pro GPU
	height_data.resize(GRID_SIZE * GRID_SIZE);
	for (int y = 0; y < GRID_SIZE; ++y) {
		for (int x = 0; x < GRID_SIZE; ++x) {
			int idx = (y % GRID_SIZE) * GRID_SIZE + (x % GRID_SIZE);
			height_data[y * GRID_SIZE + x] = (float)mapHeightmap_11B4E0[idx] * 0.125f;
		}
	}

	update_gpu_heightmap();
	update_gpu_controlmap();
}

void ExampleClass::update_gpu_heightmap() {
	if (height_image.is_null())
		initialize_heightmap();
	PackedByteArray byte_array;
	byte_array.resize(height_data.size() * sizeof(float));
	memcpy(byte_array.ptrw(), height_data.data(), byte_array.size());

	height_image->set_data(GRID_SIZE, GRID_SIZE, false, Image::FORMAT_RF, byte_array);
	height_texture->update(height_image);
}

void ExampleClass::update_gpu_controlmap() {
	control_image->set_data(GRID_SIZE, GRID_SIZE, false, Image::FORMAT_RGBA8, control_data);
	control_texture->update(control_image);
}

void ExampleClass::initialize_controlmap() {
	control_image = Image::create(GRID_SIZE, GRID_SIZE, false, Image::FORMAT_RGBA8);
	control_texture = ImageTexture::create_from_image(control_image);
	if (mesh_instance) {
		Ref<ShaderMaterial> mat = mesh_instance->get_material_override();
		if (mat.is_valid()) {
			mat->set_shader_parameter("control_map", control_texture);
		}
	}
}

void ExampleClass::initialize_heightmap() {
	height_data.assign(GRID_SIZE * GRID_SIZE, 0.0f);
	height_image = Image::create(GRID_SIZE, GRID_SIZE, false, Image::FORMAT_RF);
	height_texture = ImageTexture::create_from_image(height_image);
	if (mesh_instance) {
		Ref<ShaderMaterial> mat = mesh_instance->get_material_override();
		if (mat.is_valid()) {
			mat->set_shader_parameter("height_map", height_texture);
		}
	}
}

void ExampleClass::add_triangle(Vector3 p1, Vector3 p2, Vector3 p3, Vector2 uv1, Vector2 uv2, Vector2 uv3,
	Vector2 grid_p1, Vector2 grid_p2, Vector2 grid_p3, Vector2 main_p)
{
	Vector3 verts[] = { p1, p2, p3 };
	Vector2 uvs[] = { uv1, uv2, uv3 };

	Vector2 global_uvs[] = {
		grid_p1 / float(GRID_SIZE),
		grid_p2 / float(GRID_SIZE),
		grid_p3 / float(GRID_SIZE)
	};

	Vector2 main_uvs = main_p / float(GRID_SIZE);

	for (int i = 0; i < 3; i++) {
		surface_tool->set_uv(uvs[i]);
		surface_tool->set_custom(0, Color(global_uvs[i].x, global_uvs[i].y, main_uvs.x, main_uvs.y));
		surface_tool->add_vertex(verts[i]);
	}
}

PackedByteArray ExampleClass::deRNC(PackedByteArray bytearray) {
	int input_size = bytearray.size();
	std::vector<uint8_t> src(input_size);
	for (int i = 0; i < input_size; ++i)
		src[i] = bytearray[i];
	int output_size = input_size * 20;
	std::vector<uint8_t> dst(output_size);
	int decompressed_size = DataFileRNC::Decompress(src.data(), dst.data());
	PackedByteArray result;
	result.resize(decompressed_size);
	for (int i = 0; i < decompressed_size; ++i)
		result[i] = dst[i];
	return result;
}

uint8_t ExampleClass::TerrainGetTileTerrainType(int index) {
	if (index < 0 || index >= 65536)
		return 0;
	return mapTerrainType_10B4E0[index];
}

void ExampleClass::TerrainSetTileTerrainType(int index, uint8_t value) {
	if (index < 0 || index >= 65536)
		return;
	mapTerrainType_10B4E0[index] = value;
}

PackedByteArray ExampleClass::TerrainGetMapTerrainType() {
	PackedByteArray arr;
	arr.resize(65536);
	memcpy(arr.ptrw(), mapTerrainType_10B4E0, 65536);
	return arr;
}

PackedByteArray ExampleClass::TerrainGetMapHeight() {
	PackedByteArray arr;
	arr.resize(65536);
	memcpy(arr.ptrw(), mapHeightmap_11B4E0, 65536);
	return arr;
}

PackedByteArray ExampleClass::TerrainGetAngle() {
	PackedByteArray arr;
	arr.resize(65536);
	memcpy(arr.ptrw(), mapAngle_13B4E0, 65536);
	return arr;
}

PackedFloat32Array ExampleClass::GetEntites() {
	PackedFloat32Array result;
	int count = 1000;
	result.resize(count * 29);

	float *write_ptr = result.ptrw();
	int idx = 0;

	for (int i = 0; i < count; i++) {
		type_event_0x6E8E *actEntity = ENTITY_EA3E4[i];

		write_ptr[idx++] = (float)actEntity->axis_0x4C_76.x;//1
		write_ptr[idx++] = (float)actEntity->axis_0x4C_76.y;//2
		write_ptr[idx++] = (float)actEntity->axis_0x4C_76.z;//3
		write_ptr[idx++] = (float)actEntity->array_0x52_82.yaw;//4
		write_ptr[idx++] = (float)actEntity->array_0x52_82.pitch;//5
		write_ptr[idx++] = (float)actEntity->array_0x52_82.roll;//6

		write_ptr[idx++] = (float)actEntity->actionIndex_0x45_69; //7 = 0x29;
		write_ptr[idx++] = (float)actEntity->class_0x3F_63; //8 = 0xA;
		write_ptr[idx++] = (float)actEntity->model_0x40_64; //9 = 0x27;
		write_ptr[idx++] = (float)actEntity->xtype_0x41_65; //10 = 10;
		write_ptr[idx++] = (float)actEntity->xsubtype_0x42_66; //11 = 39;
		write_ptr[idx++] = (float)actEntity->word_0x2C_44; //12 = 128;
		write_ptr[idx++] = (float)actEntity->actSpeed_0x82_130; //13 = 32;
		write_ptr[idx++] = (float)actEntity->byte_0x38_56; //14 = 3;
		write_ptr[idx++] = (float)actEntity->byte_0x39_57; //15 = 128;
		write_ptr[idx++] = (float)actEntity->byte_0x3A_58; //16 = 0;

		write_ptr[idx++] = (float)actEntity->id_0x1A_26;//17
		write_ptr[idx++] = (float)actEntity->struct_byte_0xc_12_15.byte[0]; //18
		write_ptr[idx++] = (float)actEntity->struct_byte_0xc_12_15.byte[1]; //19
		write_ptr[idx++] = (float)actEntity->struct_byte_0xc_12_15.byte[2]; //20
		write_ptr[idx++] = (float)actEntity->struct_byte_0xc_12_15.byte[3]; //21
		write_ptr[idx++] = (float)str_WORD_D951C[actEntity->word_0x5A_90].word_0; //22
		write_ptr[idx++] = (float)actEntity->yaw_0x1C_28; //23
		write_ptr[idx++] = (float)actEntity->pitch_0x1E_30; //24
		write_ptr[idx++] = (float)actEntity->mana_0x90_144; //25
		write_ptr[idx++] = (float)actEntity->life_0x8; //26
		write_ptr[idx++] = (float)actEntity->maxMana_0x8C_140; //27
		write_ptr[idx++] = (float)actEntity->maxLife_0x4; //28
		write_ptr[idx++] = (float)actEntity->playerEntityIndex_0x94_148; //29

		//v7x = &str_WORD_D951C[str_F2C20ar.dword0x14x->word_0x5A_90];
		//67

		//v5x->str_0x5E_94.word_0x68_104 = a1x->id_0x1A_26;

		int test=str_WORD_D951C[actEntity->word_0x5A_90].word_0;
		switch (test) {
			case 0: //hrac0-ok
				break;
			case 8://strelec
				break;
			case 38: //ohen-ok
				break;
			case 54: //ohen-ok
				break;
			case 57: //kour1(dole)-ok
				break;
			case 59: //schranka s kouzlem-ok
				break;
			case 63: //kour2(na zacatku)-ok
				break;
			case 67: //white sphere
				break;
			case 79: //dolmen-ok
				break;
			case 96: //oznaceni domu
				break;
			case 152: //koza-ok
				break;
			case 155: //vesnican-ok
				break;
			case 180: //vesnican-ok
				break;
			case 183: //vesnican-ok
				break;
			case 199: //vesnican-ok
				break;
			default:
				test = 200;
		}
		/*
		word_0x5A_90
		v9x = &str_WORD_D951C[v41x->word_0x5A_90];
		str_TMAPS00TAB_BEGIN_BUFFER[v9x->word_0]
		str_TMAPS00TAB_BEGIN_BUFFER[v26 + v9x->word_0] v26 0..7
		*/
	}
	return result;
}

void ExampleClass::RunGameStep(Dictionary inputs) {

	Array key_changes = inputs["key_changes"];
	for (int i = 0; i < key_changes.size(); i++) {
		Dictionary change = key_changes[i];
		int key_index = change["key_index"];
		String action = change["action"];
		bool is_pressed = (action == "pressed");
		switch (key_index) {
			case 0:
				setPress(is_pressed, 0x4800);
				break;
			case 1:
				setPress(is_pressed, 0x5000);
				break;
			case 2:
				setPress(is_pressed, 0x4b00);
				break;
			case 3:
				setPress(is_pressed, 0x4d00);
				break;
		}
	}

	int buttonresult = 1;
	Array mouse_button_changes = inputs["mouse_button_changes"];
	for (int i = 0; i < mouse_button_changes.size(); i++) {
		Dictionary change = mouse_button_changes[i];
		int button_index = change["button_index"];
		String action = change["action"];
		bool is_pressed = (action == "pressed");
		switch (button_index) {
			case 0://MOUSE_BUTTON_LEFT
				if (is_pressed)
					buttonresult |= 0x2;
				else
					buttonresult |= 0x4;
				break;
			case 1://MOUSE_BUTTON_RIGHT
				if (is_pressed)
					buttonresult |= 0x8;
				else
					buttonresult |= 0x10;
				break;
			case 2://MOUSE_BUTTON_MIDDLE
				if (is_pressed)
					buttonresult |= 0x20;
				else
					buttonresult |= 0x40;
				break;
			case 3://MOUSE_BUTTON_WHEEL_UP
				/* if (is_pressed)
					buttonresult |= 2;
				else
					buttonresult |= 4;*/
				break;
			case 4: //MOUSE_BUTTON_WHEEL_DOWN
				/* if (is_pressed)
					buttonresult |= 2;
				else
					buttonresult |= 4;*/
				break;
		}
	}

	Vector2 mouse_pos = inputs["mouse_pos"];
	MouseEvents(buttonresult, mouse_pos.x, 480-mouse_pos.y);

	ReadGameUserInputs_89D10(); //get keys
	MouseAndKeysEvents_17A00(0, x_DWORD_17DB54_game_turn2);
	GameEvents_51BB0();
	UpdateEntities_57730();
	x_DWORD_17DB54_game_turn2++;
}

Dictionary ExampleClass::GetPlayerPositionRotation() {
	type_struct_0x1d1_2BDE_11695 raw_data = D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].ActPlayerIndex_0x00e_2BDE_11244 + 1];
	Dictionary res;

	Vector3 pos = Vector3(
			(float)raw_data.axis_2BDE_11695.x,
			(float)raw_data.axis_2BDE_11695.z,
			(float)raw_data.axis_2BDE_11695.y);

	Dictionary rot;
	rot["yaw"] = raw_data.rotation__2BDE_11701.yaw;
	rot["pitch"] = raw_data.rotation__2BDE_11701.pitch;
	rot["roll"] = raw_data.rotation__2BDE_11701.roll;
	rot["fov"] = raw_data.rotation__2BDE_11701.fov;

	res["position"] = pos;
	res["rotation"] = rot;

	return res;
}

void ExampleClass::TerrainMake(PackedByteArray bytearray) {
	const uint8_t *src = bytearray.ptr();
	if (bytearray.size() < sizeof(Type_CompressedLevel_2FECE)) {
		return;
	}

	support_begin();

	Type_CompressedLevel_2FECE shadow_a2x;
	qmemcpy(&shadow_a2x, (Type_CompressedLevel_2FECE *)(const void *)src, sizeof(Type_CompressedLevel_2FECE)); //0x6604
	DecompressLevel_2FECE(&shadow_a2x, &D41A0_0.terrain_2FECE);

	uint8_t buffer[1000000];
	pdwScreenBuffer_351628 = buffer;
	x_BYTE_14B4E0_second_heightmap = new uint8_t[65536];
	*xadataclrd0dat.colorPalette_var28 = (uint8_t *)malloc(4096); //fix it 3x256 ?

	x_DWORD_17DB54_game_turn2 = 0x40;
	x_BYTE_E36D1 = 0x7;
	unk_18058Cstr.x_WORD_1805C2_joystick = 0x7;

	x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx = 0x140;
	x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony = 0xf0;

	//begin - code from sub_main
	initposistruct();
	//end - code from sub_main

	sprintf(gameFolder, "c:/prenos/godot-zyllan/MagicBalls/gitextension/godot-cpp-template/data/GAME/NETHERW");
	sprintf(cdFolder, "c:/prenos/godot-zyllan/MagicBalls/gitextension/godot-cpp-template/data/CD_Files");
	gameDataPath = GetSubDirectoryPath(gameFolder);
	cdDataPath = GetSubDirectoryPath(cdFolder);

	//begin - code from Initialize
	DataFileIO::SetCDFilePaths(cdDataPath.c_str(), pstr);
	//---------------------
	sub_5BCC0_set_any_variables1(); //23C9F2 - 23CCC0
	if (!sub_5BF50_load_psxdata()) //23C9F7 - 23CF50 //something with files about their loading, or just a set of Palettes
		exit(-1);
	sub_5C1B0_set_any_variables2(); //23CA05 - 23D1B0
	sub_71410_process_tmaps(); //252410
	CreateIndexes_6EB90(&filearray_2aa18c[filearrayindex_POINTERSDATTAB]); //24fb90
	CreateIndexes_6EB90(&filearray_2aa18c[filearrayindex_BUILD00DATTAB]); //24fb90 adress 0x23ca2e
	sub_101C0();
	sub_8CEDF_install_mouse();
	//end - code from Initialize

	//x_BYTE_F5538[str_TMAPS00TAB_BEGIN_BUFFER[str_WORD_D951C[a1].word_0].word_8]
	/*
	fix this: !!!!!!
	sub_712F0
	void sub_712F0()//2522f0 - x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226
	sub_7A110_load_hscreen -zkontroluj

	x_DWORD_E9C28_str->str_8_data->word_8
	//x_DWORD_E9C28_str = sub_71B40

	x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226 --toto neni pripraveno
	*/

	//begin - code from MainMenu
	// sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 4);
	x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 &= 0xEFu;
	x_WORD_180660_VGA_type_resolution = 8;
	sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 4);
	sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 6);
	//end - code from MainMenu

	x_D41A0_BYTEARRAY_4_struct.langIndex_4 = 1;

	//begin - code from LevelDecompress_533B0
	//LevelInitGame_56A30(-1, "");
	LevelInit_56C00(&D41A0_0.terrain_2FECE);
	SetLevelId_53590(&D41A0_0.terrain_2FECE);
	//end - code from LevelDecompress_533B0

	//begin - code from LevelInitGame_56A30
	CreateIndexes_6EB90(&filearray_2aa18c[filearrayindex_BUILD00DATTAB]); //24fb90 adress 0x23ca2e
	char temp_x_BYTE_E3799_sound_card = soundActive_E3799;
	soundActive_E3799 = false;
	ClearSettings_567C0();
	if (!(x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 & 8)) {
		LevelDecompress_533B0(x_D41A0_BYTEARRAY_4_struct.levelnumber_43w, &D41A0_0.terrain_2FECE, "");
	}
	sub_54660_read_and_decompress_sky_and_blocks(D41A0_0.terrain_2FECE.MapType, x_BYTE_D41B5_texture_size); //235660
	sub_54800_read_and_decompress_tables(D41A0_0.terrain_2FECE.MapType); //235800
	//237ab3
	if (!(x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 & 0x10))
		D41A0_0.word_0xe = D41A0_0.terrain_2FECE.word_0x2FED7;
	if (!(x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 & 4))
		GenerateLevelMap_43830(&D41A0_0.terrain_2FECE);
	sub_49F30(); //prepare events pointers
	if (!(x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 & 4))
		sub_49270_generate_level_features(&D41A0_0.terrain_2FECE);
	memset(&predictedAxis_EB398ar, 0, 6);
	sub_49F90();
	D41A0_0.dword_0x11e6 = -1;
	sub_71A70_setTmaps(D41A0_0.terrain_2FECE.MapType);
	if (!(x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 & 4)) {
		InitStages_58940();
		InitStageVars_11EE0();
		Init0x3664C_84790();
	}
	sub_4A1E0(0, 1);
	sub_53160();
	sub_60F00();
	//end - code from LevelInitGame_56A30

	//begin - sub_46830_main_loop
	sub_47160();
	//end - sub_46830_main_loop
}
