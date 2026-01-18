#pragma once
#include "../remc2/utilities/DataFileRNC.h"
#include "../remc2/engine/Terrain.h"
#include "../remc2/engine/ConvertMapInfo.h"
#include "../remc2/engine/EventsFunctions.h"
#include "../remc2/engine/LevelInit.h"
#include "../remc2/engine/Level.h"
#include "../remc2/engine/DatTabIndexes.h"
#include "../remc2/sub_main.h"
#include "../remc2/engine/ReadAndDecompress.h"
#include "../remc2/engine/MenusAndIntros.h"
#include "../remc2/engine/Basic.h"
#include "../remc2/engine/GameUI.h"
#include "../remc2/engine/Sound.h"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node3d.hpp>


#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"

#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/surface_tool.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/vector2.hpp>

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

#include <godot_cpp/classes/shader_material.hpp>

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/variant/string.hpp>

#include <godot_cpp/classes/dir_access.hpp>

#include <godot_cpp/classes/file_access.hpp>

using namespace godot;

void MBEXconvertData();
void MBEXsoundConverts();
bool MBEXsoundConvert(int i);
bool MBLoadSound(uint8_t soundIndex);
