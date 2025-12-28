#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"

using namespace godot;

class ExampleClass : public RefCounted {
	GDCLASS(ExampleClass, RefCounted)

protected:
	static void _bind_methods();

public:
	ExampleClass() = default;
	~ExampleClass() override = default;

	PackedByteArray deRNC(PackedByteArray bytearray);
	void MakeTerrain(PackedByteArray bytearray);

	void TerrainMake(PackedByteArray bytearray);
	void RunGameStep();
	Dictionary GetPlayerPositionRotation();
	uint8_t TerrainGetTileTerrainType(int index);
	void TerrainSetTileTerrainType(int index, uint8_t value);
	PackedByteArray TerrainGetMapTerrainType();
	PackedByteArray TerrainGetMapHeight();
	PackedByteArray TerrainGetAngle();
};
