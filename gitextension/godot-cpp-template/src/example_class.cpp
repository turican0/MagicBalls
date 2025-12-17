#include "example_class.h"
#include "utilites/DataFileRNC.h"
#include "engine/Terrain.h"
#include "engine/ConvertMapInfo.h"

void ExampleClass::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("print_type", "variant"), &ExampleClass::print_type);
	godot::ClassDB::bind_method(D_METHOD("my_add", "a", "b"), &ExampleClass::my_add);
	godot::ClassDB::bind_method(D_METHOD("deRNC", "bytearray"), &ExampleClass::deRNC);
	godot::ClassDB::bind_method(D_METHOD("TerrainMake", "bytearray"), &ExampleClass::TerrainMake);
	godot::ClassDB::bind_method(D_METHOD("TerrainGetMapHeight"), &ExampleClass::TerrainGetMapHeight);
	godot::ClassDB::bind_method(D_METHOD("TerrainGetMapTerrainType"), &ExampleClass::TerrainGetMapTerrainType);
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
void ExampleClass::TerrainMake(PackedByteArray bytearray) {
	const uint8_t *src = bytearray.ptr();
	if (bytearray.size() < sizeof(type_shadow_str_2FECE)) {
		return;
	}
	type_shadow_str_2FECE shadow_a2x;
	qmemcpy(&shadow_a2x, (type_shadow_str_2FECE *)(const void *)src, sizeof(type_shadow_str_2FECE)); //0x6604
	Convert_from_shadow_str_2FECE(&shadow_a2x, &D41A0_0.terrain_2FECE);

	uint8_t buffer[1000000];
	pdwScreenBuffer_351628 = buffer;
	x_BYTE_14B4E0_second_heightmap = new uint8_t[65536];
	GenerateLevelMap_43830(&D41A0_0.terrain_2FECE);
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
