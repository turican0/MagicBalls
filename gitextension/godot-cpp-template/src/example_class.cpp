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

void ExampleClass::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("deRNC", "bytearray"), &ExampleClass::deRNC);
	godot::ClassDB::bind_method(D_METHOD("TerrainMake", "bytearray"), &ExampleClass::TerrainMake);
	godot::ClassDB::bind_method(D_METHOD("TerrainGetMapHeight"), &ExampleClass::TerrainGetMapHeight);
	godot::ClassDB::bind_method(D_METHOD("TerrainGetMapTerrainType"), &ExampleClass::TerrainGetMapTerrainType);
	godot::ClassDB::bind_method(D_METHOD("TerrainGetAngle"), &ExampleClass::TerrainGetAngle);
	godot::ClassDB::bind_method(D_METHOD("RunGameStep", "Dictionary"), &ExampleClass::RunGameStep);
	godot::ClassDB::bind_method(D_METHOD("GetEntites"), &ExampleClass::GetEntites);
	godot::ClassDB::bind_method(D_METHOD("GetTerrainChanges"), &ExampleClass::GetTerrainChanges);
	godot::ClassDB::bind_method(D_METHOD("GetPlayerPositionRotation"), &ExampleClass::GetPlayerPositionRotation);
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
	result.resize(count * 17);

	float *write_ptr = result.ptrw();
	int idx = 0;

	for (int i = 0; i < count; i++) {
		type_event_0x6E8E *actEntity = x_DWORD_EA3E4[i];

		write_ptr[idx++] = (float)actEntity->axis_0x4C_76.x;//1
		write_ptr[idx++] = (float)actEntity->axis_0x4C_76.y;//2
		write_ptr[idx++] = (float)actEntity->axis_0x4C_76.z;//3
		write_ptr[idx++] = (float)actEntity->array_0x52_82.yaw;//4
		write_ptr[idx++] = (float)actEntity->array_0x52_82.pitch;//5
		write_ptr[idx++] = (float)actEntity->array_0x52_82.roll;//6
		write_ptr[idx++] = (float)actEntity->class_0x3F_63;//7
		write_ptr[idx++] = (float)actEntity->model_0x40_64;//8
		write_ptr[idx++] = (float)actEntity->state_0x45_69;//9
		write_ptr[idx++] = (float)actEntity->id_0x1A_26;//10
		write_ptr[idx++] = (float)actEntity->struct_byte_0xc_12_15.byte[0]; //11
		write_ptr[idx++] = (float)actEntity->struct_byte_0xc_12_15.byte[1]; //12
		write_ptr[idx++] = (float)actEntity->struct_byte_0xc_12_15.byte[2]; //13
		write_ptr[idx++] = (float)actEntity->struct_byte_0xc_12_15.byte[3]; //14
		write_ptr[idx++] = (float)str_WORD_D951C[actEntity->word_0x5A_90].word_0; //15
		write_ptr[idx++] = (float)actEntity->yaw_0x1C_28; //16
		write_ptr[idx++] = (float)actEntity->pitchAngle_0x1E_30; //17
		int test=str_WORD_D951C[actEntity->word_0x5A_90].word_0;
		switch (test) {
			case 0: //hrac0
				break;
			case 8://strelec
				break;
			case 38: //ohen
				break;
			case 54: //ohen
				break;
			case 57: //kour1(dole)
				break;
			case 59: //schranka s kouzlem
				break;
			case 63: //kour2(na zacatku)
				break;
			case 79: //stonehenge
				break;
			case 96: //oznaceni domu
				break;
			case 152: //koza
				break;
			case 155: //vesnican
				break;
			case 180: //vesnican
				break;
			case 183: //vesnican
				break;
			case 199: //vesnican
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

Dictionary ExampleClass::GetTerrainChanges() {
	Dictionary result;
	return result;
}

void ExampleClass::RunGameStep(Dictionary inputs) {
	Vector2 mouse_pos = inputs["mouse_pos"];
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

	//x_WORD_E3760_mouse.x = 320;
	//x_WORD_E3760_mouse.y = 240;
	//x_WORD_E375C_mouse_position_x = 320;
	//x_WORD_E375E_mouse_position_y = 240;

	//mousex = event.motion.x;
	//mousey = event.motion.y;
	//mouse_pos.x = 320;
	//mouse_pos.y = 240;
	MouseEvents(1, mouse_pos.x, mouse_pos.y);

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
