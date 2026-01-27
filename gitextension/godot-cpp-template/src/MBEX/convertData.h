#pragma once

#include "../lib/lib9660.h"

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
#include "../libxmi/xmi2mid.h"

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

void MBEXconvertData(String path, String path2);
void MBEXsoundConverts(String path);
void MBEXmusicConverts(String path);
bool MBEXsoundConvert(int i,String path);
bool MBLoadSound(uint8_t soundIndex);
bool make_dir_godot(const String &path);
void MBEXtextureConverts(String path, int width, int height, String texture, String palette, bool makeBorders);
void MBEXtexturesConverts(String path);
void MBEXgraphicsConverts(String path);
void MBEXwebConverts(String path);
void MBEXsmatsConverts(String path);
void MBEXhtablesConverts(String path);
void MBEXhscreenConverts(String path);
void MBEXhbuttonsConverts(String path);
void MBEXcdrConverts(String path);
void MBEXpointersConverts(String path);
void MBEXgtdConverts(String path);
void MBEXextractLang(String path, String langPath, String cdLangPath);
void MBEXsaveBitmap(String path, char *name, int width, int height, uint8_t *data, TColor *palette, bool alpha, int alphaIndex);
void MBEXsaveBitmapCrop(String path, char *name, int width, int height, uint8_t *data, TColor *palette, int cropXmin);
void MBEXcdExtract(char *pathGOG, char *pathOut);
