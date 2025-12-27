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

void ExampleClass::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("print_type", "variant"), &ExampleClass::print_type);
	godot::ClassDB::bind_method(D_METHOD("my_add", "a", "b"), &ExampleClass::my_add);
	godot::ClassDB::bind_method(D_METHOD("deRNC", "bytearray"), &ExampleClass::deRNC);
	godot::ClassDB::bind_method(D_METHOD("TerrainMake", "bytearray"), &ExampleClass::TerrainMake);
	godot::ClassDB::bind_method(D_METHOD("TerrainGetMapHeight"), &ExampleClass::TerrainGetMapHeight);
	godot::ClassDB::bind_method(D_METHOD("TerrainGetMapTerrainType"), &ExampleClass::TerrainGetMapTerrainType);
	godot::ClassDB::bind_method(D_METHOD("TerrainGetAngle"), &ExampleClass::TerrainGetAngle);
}

void ExampleClass::print_type(const Variant &p_variant) const {
	print_line(vformat("Type: %d", p_variant.get_type()));
}
int ExampleClass::my_add(int a, int b) {
	return a + b;
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
	memset(&x_WORD_EB398ar, 0, 6);
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
}
