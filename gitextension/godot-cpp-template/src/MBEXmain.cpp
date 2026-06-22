#include "MBEXmain.h"
#include "MBEX/convertData.h"
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
#include "remc2/portability/port_sdl_sound.h"
#include "remc2/engine/Sound.h"

#include "modified/remc2_mod.h"

#include "MBEX/editor.h"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node3d.hpp>

// ── Android: res:// → user:// helper ────────────────────────────────────────
// Na Androidu nelze res:// globalizovat na filesystem cestu použitelnou nativním
// C kódem. Proto při prvním volání zkopírujeme celý strom res://hidata/ do
// user://hidata/ a dále používáme user:// cestu.
// Na ostatních platformách funkce vrátí prázdný String a volající použije
// původní logiku (globalize_path).
#if defined(__ANDROID__)

static void android_copy_dir_recursive(const String &src_dir, const String &dst_dir) {
	Ref<DirAccess> src = DirAccess::open(src_dir);
	if (src.is_null())
		return;

	// Zajistíme existenci cílové složky
	String global_dst = ProjectSettings::get_singleton()->globalize_path(dst_dir);
	if (!DirAccess::dir_exists_absolute(global_dst)) {
		DirAccess::make_dir_recursive_absolute(global_dst);
	}

	src->list_dir_begin();
	String name = src->get_next();
	while (name != "") {
		if (name != "." && name != "..") {
			String src_path = src_dir.path_join(name);
			String dst_path = dst_dir.path_join(name);

			if (src->current_is_dir()) {
				android_copy_dir_recursive(src_path, dst_path);
			} else {
				if (!FileAccess::file_exists(dst_path)) {
					// Ignorujeme .import soubory, které způsobují chyby v logu
					if (!name.ends_with(".import")) {
						Ref<FileAccess> f_src = FileAccess::open(src_path, FileAccess::READ);
						if (f_src.is_valid()) {
							Ref<FileAccess> f_dst = FileAccess::open(dst_path, FileAccess::WRITE);
							if (f_dst.is_valid()) {
								f_dst->store_buffer(f_src->get_buffer(f_src->get_length()));
							}
						}
					}
				}
			}
		}
		name = src->get_next();
	}
	src->list_dir_end();
}

// Vrátí globalizovanou user:// cestu odpovídající res:// cestě.
// Při prvním volání zkopíruje celý strom (pokud ještě nebyl zkopírován).
// Kopíruje pouze soubory, které v cíli ještě neexistují.
static String android_resolve_res_path(const String &path) {
	if (!path.begins_with("res://"))
		return String();

	String rel = path.substr(6);
	String user_path = String("user://") + rel;
	bool is_dir = (path.get_extension() == "");

	if (is_dir) {
		// ZMĚNA: Kontrola existence adresáře přímo v user://
		if (!DirAccess::dir_exists_absolute(ProjectSettings::get_singleton()->globalize_path(user_path))) {
			UtilityFunctions::print("android_resolve_res_path: copying tree: ", path);
			android_copy_dir_recursive(path, user_path);

			// Marker klidně necháme pro informaci, ale už ho nekontrolujeme
			String marker = user_path.trim_suffix("/") + "/.android_copied";
			Ref<FileAccess> mf = FileAccess::open(marker, FileAccess::WRITE);
			if (mf.is_valid())
				mf->store_string("ok");
		}
	} else {
		// Kopírování jednotlivého souboru (stejná logika přes buffer)
		if (!FileAccess::file_exists(user_path)) {
			String parent = user_path.get_base_dir();
			DirAccess::make_dir_recursive_absolute(ProjectSettings::get_singleton()->globalize_path(parent));

			Ref<FileAccess> f_src = FileAccess::open(path, FileAccess::READ);
			if (f_src.is_valid()) {
				Ref<FileAccess> f_dst = FileAccess::open(user_path, FileAccess::WRITE);
				if (f_dst.is_valid())
					f_dst->store_buffer(f_src->get_buffer(f_src->get_length()));
			}
		}
	}

	return ProjectSettings::get_singleton()->globalize_path(user_path);
}

// Hlavní helper: rozhodne, co použít.
// - res:// na Androidu → zkopíruje do user://, vrátí globalizovanou user:// cestu
// - user:// → globalizuje přímo
// - cokoliv jiného → vrátí beze změny
static String android_globalize_path(const String &path) {
	if (path.begins_with("res://")) {
		return android_resolve_res_path(path);
	}
	if (path.begins_with("user://")) {
		return ProjectSettings::get_singleton()->globalize_path(path);
	}
	return path;
}

#define PLATFORM_GLOBALIZE_PATH(p) android_globalize_path(p)

#else // ! __ANDROID__

static String default_globalize_path(const String &path) {
	if (path.begins_with("res://") || path.begins_with("user://"))
		return ProjectSettings::get_singleton()->globalize_path(path);
	return path;
}

#define PLATFORM_GLOBALIZE_PATH(p) default_globalize_path(p)

#endif // __ANDROID__
// ── konec Android helperu ────────────────────────────────────────────────────

void MBEXclass::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("deRNC", "bytearray"), &MBEXclass::deRNC);
	//godot::ClassDB::bind_method(D_METHOD("TerrainMake", "bytearray", "text"), &MBEXclass::TerrainMake);
	godot::ClassDB::bind_method(D_METHOD("TerrainGetMapHeight", "int"), &MBEXclass::TerrainGetMapHeight);
	godot::ClassDB::bind_method(D_METHOD("TerrainGetMapTerrainType"), &MBEXclass::TerrainGetMapTerrainType);
	godot::ClassDB::bind_method(D_METHOD("TerrainGetAngle"), &MBEXclass::TerrainGetAngle);
	godot::ClassDB::bind_method(D_METHOD("RunGameStep", "Dictionary"), &MBEXclass::RunGameStep);
	godot::ClassDB::bind_method(D_METHOD("GetEntites"), &MBEXclass::GetEntites);
	godot::ClassDB::bind_method(D_METHOD("GetPlayerPositionRotation"), &MBEXclass::GetPlayerPositionRotation);
	godot::ClassDB::bind_method(D_METHOD("set_mesh_instances", "Node3D", "Node3D", "bool"), &MBEXclass::set_mesh_instances);
	godot::ClassDB::bind_method(D_METHOD("recalculate_mesh", "bool"), &MBEXclass::recalculate_mesh);
	godot::ClassDB::bind_method(D_METHOD("renew_terrain", "bool"), &MBEXclass::renew_terrain);
	godot::ClassDB::bind_method(D_METHOD("getActiveSpells"), &MBEXclass::getActiveSpells);
	//godot::ClassDB::bind_method(D_METHOD("getActiveSubSpells"), &MBEXclass::getActiveSubSpells);
	godot::ClassDB::bind_method(D_METHOD("getSelectedSpells"), &MBEXclass::getSelectedSpells);
	godot::ClassDB::bind_method(D_METHOD("setPlayerActiveSpell", "Int", "Int"), &MBEXclass::setPlayerActiveSpell);
	godot::ClassDB::bind_method(D_METHOD("setPlayerActiveSubSpell", "Int", "Int", "Int"), &MBEXclass::setPlayerActiveSubSpell);
	godot::ClassDB::bind_method(D_METHOD("getPaletteModifications"), &MBEXclass::getPaletteModifications);
	godot::ClassDB::bind_method(D_METHOD("convertOriginalData", "text", "text"), &MBEXclass::convertOriginalData);
	godot::ClassDB::bind_method(D_METHOD("convertOriginalDataExtractCD", "text", "text"), &MBEXclass::convertOriginalDataExtractCD);
	//godot::ClassDB::bind_method(D_METHOD("soundQueueClear"), &MBEXclass::soundQueueClear);
	godot::ClassDB::bind_method(D_METHOD("getPendingSoundActions"), &MBEXclass::getPendingSoundActions);
	//godot::ClassDB::bind_method(D_METHOD("getPendingGraphicsActions"), &MBEXclass::getPendingGraphicsActions);
	godot::ClassDB::bind_method(D_METHOD("updateFreeSoundPlayers", "indices"), &MBEXclass::updateFreeSoundPlayers);
	godot::ClassDB::bind_method(D_METHOD("getVGABuffer"), &MBEXclass::getVGABuffer);

	godot::ClassDB::bind_method(D_METHOD("getLangTexts"), &MBEXclass::getLangTexts);
	godot::ClassDB::bind_method(D_METHOD("changeLanguage", "Int"), &MBEXclass::changeLanguage);
	godot::ClassDB::bind_method(D_METHOD("initLanguage", "Int"), &MBEXclass::initLanguage);

	godot::ClassDB::bind_method(D_METHOD("REMC2BeginGame", "text", "text", "int"), &MBEXclass::REMC2BeginGame);
	godot::ClassDB::bind_method(D_METHOD("REMC2EndGame"), &MBEXclass::REMC2EndGame);

	godot::ClassDB::bind_method(D_METHOD("REMC2GetLevelType"), &MBEXclass::REMC2GetLevelType);
	godot::ClassDB::bind_method(D_METHOD("REMC2SetLevelType", "text"), &MBEXclass::REMC2SetLevelType);
	godot::ClassDB::bind_method(D_METHOD("REMC2GetWebInfo"), &MBEXclass::REMC2GetWebInfo);

	godot::ClassDB::bind_method(D_METHOD("REMC2IsHiddenLevel"), &MBEXclass::REMC2IsHiddenLevel);

	//godot::ClassDB::bind_method(D_METHOD("REMC2SetCDPath", "text"), &MBEXclass::REMC2SetCDPath);
	godot::ClassDB::bind_method(D_METHOD("REMC2SetScrBuffer", "TextureRect"), &MBEXclass::REMC2SetScrBuffer);
	godot::ClassDB::bind_method(D_METHOD("REMC2Run", "Dictionary", "Int"), &MBEXclass::REMC2Run);

	godot::ClassDB::bind_method(D_METHOD("REMC2GetGraphicsEenhance"), &MBEXclass::REMC2GetGraphicsEenhance);
	godot::ClassDB::bind_method(D_METHOD("REMC2getWarpMouse"), &MBEXclass::REMC2getWarpMouse);

	godot::ClassDB::bind_method(D_METHOD("REMC2SetInverseMouse", "Bool"), &MBEXclass::REMC2SetInverseMouse);

	godot::ClassDB::bind_method(D_METHOD("REMC2GetMapMode"), &MBEXclass::REMC2GetMapMode);
	godot::ClassDB::bind_method(D_METHOD("REMC2GetWizardsLabelsVisible"), &MBEXclass::REMC2GetWizardsLabelsVisible);
	godot::ClassDB::bind_method(D_METHOD("REMC2GetTerrainAlt", "int", "int"), &MBEXclass::REMC2GetTerrainAlt);

	godot::ClassDB::bind_method(D_METHOD("REMC2EditorBegin", "text"), &MBEXclass::REMC2EditorBegin);
	godot::ClassDB::bind_method(D_METHOD("REMC2EditorEnd"), &MBEXclass::REMC2EditorEnd);
	godot::ClassDB::bind_method(D_METHOD("REMC2EditorLoop"), &MBEXclass::REMC2EditorLoop);

	//godot::ClassDB::bind_method(D_METHOD("REMC2EditorGetTerrainValue", "Int"), &MBEXclass::REMC2EditorGetTerrainValue);
	//godot::ClassDB::bind_method(D_METHOD("REMC2EditorSetTerrainValue", "Int", "Int"), &MBEXclass::REMC2EditorSetTerrainValue);
	//godot::ClassDB::bind_method(D_METHOD("REMC2EditorGetTerrainEntites"), &MBEXclass::REMC2EditorGetTerrainEntites);
	//godot::ClassDB::bind_method(D_METHOD("REMC2EditorGetTerrainPlayers"), &MBEXclass::REMC2EditorGetTerrainPlayers);
	//godot::ClassDB::bind_method(D_METHOD("REMC2EditorGetTerrainStages"), &MBEXclass::REMC2EditorGetTerrainStages);
	godot::ClassDB::bind_method(D_METHOD("REMC2EditorIsGroupType", "Int", "Int"), &MBEXclass::REMC2EditorIsGroupType);
	godot::ClassDB::bind_method(D_METHOD("REMC2EditorIsParentType", "Int", "Int"), &MBEXclass::REMC2EditorIsParentType);
	godot::ClassDB::bind_method(D_METHOD("REMC2EditorAddEntity", "Dictionary"), &MBEXclass::REMC2EditorAddEntity);
	godot::ClassDB::bind_method(D_METHOD("REMC2EditorDeleteEntites", "Array"), &MBEXclass::REMC2EditorDeleteEntites);

	godot::ClassDB::bind_method(D_METHOD("REMC2EditorGetLevelData"), &MBEXclass::REMC2EditorGetLevelData);
	godot::ClassDB::bind_method(D_METHOD("REMC2EditorSetLevelData", "Dictionary"), &MBEXclass::REMC2EditorSetLevelData);
	godot::ClassDB::bind_method(D_METHOD("REMC2EditorExportToCSV"), &MBEXclass::REMC2EditorExportToCSV);

	godot::ClassDB::bind_method(D_METHOD("REMC2EditorUndo"), &MBEXclass::REMC2EditorUndo);
	godot::ClassDB::bind_method(D_METHOD("REMC2EditorRedo"), &MBEXclass::REMC2EditorRedo);
	godot::ClassDB::bind_method(D_METHOD("REMC2EditorSaveState"), &MBEXclass::REMC2EditorSaveState);
	godot::ClassDB::bind_method(D_METHOD("REMC2EditorTimedSaveState", "Float"), &MBEXclass::REMC2EditorTimedSaveState);

	godot::ClassDB::bind_method(D_METHOD("REMC2EditorLoadLevel", "text"), &MBEXclass::REMC2EditorLoadLevel);
	godot::ClassDB::bind_method(D_METHOD("REMC2EditorSaveLevel", "text"), &MBEXclass::REMC2EditorSaveLevel);

	godot::ClassDB::bind_method(D_METHOD("REMC2EditorCleanLevel"), &MBEXclass::REMC2EditorCleanLevel);
	godot::ClassDB::bind_method(D_METHOD("REMC2EditorExampleLevel", "int"), &MBEXclass::REMC2EditorExampleLevel);
	godot::ClassDB::bind_method(D_METHOD("REMC2EditorLoadInGameLevel", "Int"), &MBEXclass::REMC2EditorLoadInGameLevel);
}

//PlayIntoSoundEvents_1B280
//StopMusic_8E020();
//InitMusicBank_8EAD0(pSoundEvent[x_WORD_D4004].index);
//StartMusic_8E160(pSoundEvent[x_WORD_D4004].index, 0x64u);

//SetMusicVolume_98790
//StopMusic_8E020
//StartMusic_8E160

//DrawAndSoundDragonAndFire_81EE0-clean
//NewGameSubdraw_81760 - portals

int MBEXclass::convertOriginalDataExtractCD(String path, String path2) {
	String real_path, real_path2;
	if (path.begins_with("res://") || path.begins_with("user://")) {
		real_path = PLATFORM_GLOBALIZE_PATH(path);
	} else {
		real_path = path;
	}
	if (path2.begins_with("res://") || path2.begins_with("user://")) {
		real_path2 = PLATFORM_GLOBALIZE_PATH(path2);
	} else {
		real_path2 = path2;
	}
	int result = MBEXcdExtract((char *)real_path2.utf8().get_data(), (char *)real_path.utf8().get_data()); //user some path
	if (result < 0)
		return result;
	MBEXfixLang(real_path, 2);
	return result;
}

String MBEXclass::REMC2GetLevelType() {
	if (x_D41A0_BYTEARRAY_4_struct.levelnumber_43w == 24)
		return "Final";
	else if (D41A0_0.terrain_2FECE.MapType == MapType_t::Day) {
		return String("Day");
	} else if (D41A0_0.terrain_2FECE.MapType == MapType_t::Night) {
		return String("Night");
	} else
		return String("Cave");
}

void MBEXclass::REMC2SetLevelType(String level) {
	if (level == "Day") {
		D41A0_0.terrain_2FECE.MapType = MapType_t::Day;
	} else if (level == "Night") {
		D41A0_0.terrain_2FECE.MapType = MapType_t::Night;
	} else
		D41A0_0.terrain_2FECE.MapType = MapType_t::Cave;
}

int MBEXclass::initLanguage(int index) {
	FILE *configFile2;
	char configFilePath[MAX_PATH];
	TypeConfigDat configDat;
	sprintf(configFilePath, "%s/%s", gameDataPath.c_str(), "CONFIG.DAT");

	configFile2 = DataFileIO::Open(configFilePath, 0x222, 0x40);
	if (configFile2 != nullptr) {
		DataFileIO::Close(configFile2);
		configFile2 = DataFileIO::CreateOrOpenFile(configFilePath, 512);
		if (configFile2 != nullptr) {
			DataFileIO::Read(configFile2, (uint8_t *)&configDat.configDatSign_0, 4);
			if (configDat.configDatSign_0 == 0xfffffff7) {
				DataFileIO::Read(configFile2, (uint8_t *)&configDat.langIndex_4, 2);
				index = configDat.langIndex_4;
				//DataFileIO::Read(configFile2, (uint8_t *)&configDat.langIndex_4, 2);
				//sprintf(printbuffer, "L%d.TXT", configDat.langIndex_4);
				//x_D41A0_BYTEARRAY_4_struct.SelectedLangIndex = sub_7F960(tabBuffer, tabBufferEnd, langBuffer, printbuffer, langDatTab); //tady se pak zmeni v1 za v1_langdattab
			}
			DataFileIO::Close(configFile2);
		}
	} else
		x_D41A0_BYTEARRAY_4_struct.setting_38402 = 1;

	x_D41A0_BYTEARRAY_4_struct.langIndex_4 = index;
	InitLanguage_76A40_mod_only_language();
	return index;
}

void MBEXclass::MBEXfixLang(String path, int index) {
	String pathBuffer = path.trim_suffix("/") + "/GAME/NETHERW";
	String configFilePath = pathBuffer + "/CONFIG.DAT";
	String versionFilePath = pathBuffer + "/CDATA/VERSION.DAT";

	UtilityFunctions::print("MBEXfixLang path: ", path);
	UtilityFunctions::print("MBEXfixLang pathBuffer: ", pathBuffer);
	UtilityFunctions::print("MBEXfixLang configFilePath: ", configFilePath);
	UtilityFunctions::print("MBEXfixLang versionFilePath: ", versionFilePath);

	// copy CONFIG.DAT from res:// if not exists
	UtilityFunctions::print("CONFIG.DAT exists: ", FileAccess::file_exists(configFilePath));
	if (!FileAccess::file_exists(configFilePath)) {
		UtilityFunctions::print("CONFIG.DAT res source exists: ", FileAccess::file_exists("res://hidata/config/CONFIG.DAT"));
		if (FileAccess::file_exists("res://hidata/config/CONFIG.DAT")) {
			Error err = DirAccess::copy_absolute("res://hidata/config/CONFIG.DAT", configFilePath);
			UtilityFunctions::print("CONFIG.DAT copy result: ", (int)err);
		}
	}

	// copy VERSION.DAT from res:// if not exists
	UtilityFunctions::print("VERSION.DAT exists: ", FileAccess::file_exists(versionFilePath));
	if (!FileAccess::file_exists(versionFilePath)) {
		UtilityFunctions::print("VERSION.DAT res source exists: ", FileAccess::file_exists("res://hidata/version/VERSION.DAT"));
		if (FileAccess::file_exists("res://hidata/version/VERSION.DAT")) {
			Error err = DirAccess::copy_absolute("res://hidata/version/VERSION.DAT", versionFilePath);
			UtilityFunctions::print("VERSION.DAT copy result: ", (int)err);
		}
	}

	// read and write CONFIG.DAT
	FILE *configFile2;
	TypeConfigDat configDat;
	UtilityFunctions::print("Opening CONFIG.DAT for reading (512)");
	configFile2 = DataFileIO::CreateOrOpenFile(configFilePath.utf8().get_data(), 512);
	if (configFile2 != nullptr) {
		UtilityFunctions::print("CONFIG.DAT opened, reading data");
		DataFileIO::Read(configFile2, (uint8_t *)&configDat, sizeof(TypeConfigDat));
		DataFileIO::Close(configFile2);
		UtilityFunctions::print("configDatSign_0: ", (int)configDat.configDatSign_0);
		UtilityFunctions::print("configDat.langIndex_4 before write: ", (int)configDat.langIndex_4);
		if (configDat.configDatSign_0 == 0xfffffff7) {
			configDat.langIndex_4 = index;
			UtilityFunctions::print("Writing langIndex: ", index);
			configFile2 = DataFileIO::CreateOrOpenFile(configFilePath.utf8().get_data(), 546);
			if (configFile2 != nullptr) {
				DataFileIO::WriteFile_98CAA(configFile2, (uint8_t *)&configDat, sizeof(TypeConfigDat));
				DataFileIO::Close(configFile2);
				UtilityFunctions::print("CONFIG.DAT write OK");
			} else {
				UtilityFunctions::printerr("Failed to open CONFIG.DAT for writing (546)");
			}
		} else {
			UtilityFunctions::printerr("configDatSign_0 does not match 0xfffffff7, skipping write");
		}
	} else {
		UtilityFunctions::printerr("Failed to open CONFIG.DAT for reading (512)");
	}

	UtilityFunctions::print("MBEXfixLang done");
}

void MBEXclass::changeLanguage(int index) {
	x_D41A0_BYTEARRAY_4_struct.langIndex_4 = index;
	InitLanguage_76A40_mod_only_language();
	x_D41A0_BYTEARRAY_4_struct.setting_38402 = 1;
	WriteConfigDat_81DB0();
}

Dictionary MBEXclass::getLangTexts() {
	Dictionary result;

	for (int i = 0; i < 1000; i++) {
		result[i] = String(x_DWORD_E9C4C_langindexbuffer[i]);
	}

	return result;
}

PackedByteArray MBEXclass::getVGABuffer() {
	PackedByteArray result;
	result.resize(320 * 200 * 3);
	uint8_t *write_ptr = result.ptrw();
	TColor *ref_palette = (TColor *)VGA_Get_Palette(false);
	for (int i = 0; i < 320 * 200; ++i) {
		uint8_t index_value = tempVGABuffer[i];
		write_ptr[i * 3 + 0] = ref_palette[index_value].red * 4; // R
		write_ptr[i * 3 + 1] = ref_palette[index_value].green * 4; // G
		write_ptr[i * 3 + 2] = ref_palette[index_value].blue * 4; // B
	}
	return result;
}

void MBEXclass::updateFreeSoundPlayers(const godot::Array &p_indices) {
	std::vector<int> standard_vector;
	standard_vector.reserve(p_indices.size());
	for (int i = 0; i < p_indices.size(); i++) {
		standard_vector.push_back(static_cast<int>(p_indices[i]));
	}
	sound_update_playing(standard_vector);
}

int tempX_WORD_180660_VGA_type_resolution;
int tempX_DWORD_180648_map_resolution2_x;
int tempX_DWORD_180644_map_resolution2_y;
void convertPre() {
	tempX_WORD_180660_VGA_type_resolution = x_WORD_180660_VGA_type_resolution;
	tempX_DWORD_180648_map_resolution2_x = x_DWORD_180648_map_resolution2_x;
	tempX_DWORD_180644_map_resolution2_y = x_DWORD_180644_map_resolution2_y;
	x_WORD_180660_VGA_type_resolution = 8;
	sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 4);
	sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 6);
	x_DWORD_180648_map_resolution2_x = 640; //fake resolution
	x_DWORD_180644_map_resolution2_y = 480;
	//CreateIndexes_6EB90(&filearray_2aa18c[filearrayindex_BUILD00DATTAB]); //24fb90 adress 0x23ca2e
}
void convertPost() {
	x_WORD_180660_VGA_type_resolution = tempX_WORD_180660_VGA_type_resolution;
	x_DWORD_180648_map_resolution2_x = tempX_DWORD_180648_map_resolution2_x;
	x_DWORD_180644_map_resolution2_y = tempX_DWORD_180644_map_resolution2_y;
}

void MBEXclass::convertOriginalData(String path, String path2) {
	UtilityFunctions::print("convertOriginalData START");
	UtilityFunctions::print("convertOriginalData path: ", path);
	UtilityFunctions::print("convertOriginalData path2: ", path2);
	UtilityFunctions::print("convertOriginalData calling convertPre...");
	convertPre();
	UtilityFunctions::print("convertOriginalData convertPre done");
	UtilityFunctions::print("convertOriginalData calling MBEXconvertData...");
	MBEXconvertData(path, path2);
	UtilityFunctions::print("convertOriginalData MBEXconvertData done");
	UtilityFunctions::print("convertOriginalData calling MBEXaudioExtract...");
	MBEXaudioExtract(path);
	UtilityFunctions::print("convertOriginalData MBEXaudioExtract done");
	UtilityFunctions::print("convertOriginalData calling convertPost...");
	convertPost();
	UtilityFunctions::print("convertOriginalData convertPost done");
	UtilityFunctions::print("convertOriginalData END");
}

/*
void ExampleClass::soundQueueClear() {
	sound_queue_clear();
}
*/

Array MBEXclass::getPendingSoundActions() {
	Array result;
	std::vector<SoundAction> pending = sound_queue_get_pending_actions();
	for (size_t i = 0; i < pending.size(); i++) {
		const SoundAction &sa = pending[i];
		Dictionary d;
		d["action"] = String(sa.action.c_str());
		d["p1"] = sa.p1;
		d["p2"] = sa.p2;
		d["p3"] = sa.p3;
		d["p4"] = sa.p4;
		result.append(d);
	}
	sound_queue_clear();
	return result;
}

typedef struct {
	int spellIndex;
	bool glow;
	int spellLevel;
	int mana1;
	int mana2;
} drawSpellInfoType;

void MBEXclass::setPlayerActiveSpell(int spell_index, int button) {
	switch (button) {
		case 0: //left
		{
			type_entity_0x6E8E *actEvent = Entities_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
			actEvent->dword_0xA4_164x->str_611.SpellIndexLeft_0x451_1105 = spell_index;
			actEvent->dword_0xA4_164x->str_611.SubSpellIndexLeft_1109 = actEvent->dword_0xA4_164x->str_611.array_0x437_1079x.SpellIndex[spell_index];
			//actEvent->dword_0xA4_164x->str_611.leftSubSpellIndex_0x455_1109 = D41A0_0.array_0x6E3E[D41A0_0.LevelIndex_0xc].str_0x6E3E_byte2;
			x_D41A0_BYTEARRAY_4_struct.leftSpellPlayerIndex_38400 = 8;
			break;
		}
		case 1: //right
		{
			type_entity_0x6E8E *actEvent = Entities_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
			actEvent->dword_0xA4_164x->str_611.SpellIndexRight_0x453_1107 = spell_index;
			actEvent->dword_0xA4_164x->str_611.SubSpellIndexRight_1110 = actEvent->dword_0xA4_164x->str_611.array_0x437_1079x.SpellIndex[spell_index];
			//actEvent->dword_0xA4_164x->str_611.SubSpellIndexRight_1110 = D41A0_0.array_0x6E3E[D41A0_0.LevelIndex_0xc].str_0x6E3E_byte2;
			x_D41A0_BYTEARRAY_4_struct.rightSpellPlayerIndex_38401 = 8;
			break;
		}
	}
}

void MBEXclass::setPlayerActiveSubSpell(int spell_index, int sub_spell_index, int button) {
	switch (button) {
		case 0: //left
		{
			type_entity_0x6E8E *actEvent = Entities_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
			actEvent->dword_0xA4_164x->str_611.SpellIndexLeft_0x451_1105 = spell_index;
			actEvent->dword_0xA4_164x->str_611.SubSpellIndexLeft_1109 = sub_spell_index;
			//actEvent->dword_0xA4_164x->str_611.leftSubSpellIndex_0x455_1109 = D41A0_0.array_0x6E3E[D41A0_0.LevelIndex_0xc].str_0x6E3E_byte2;
			x_D41A0_BYTEARRAY_4_struct.leftSpellPlayerIndex_38400 = 8;
			SetSpell_6D5E0(Entities_EA3E4[actEvent->dword_0xA4_164x->str_611.SpellsEnabled_0x333_819x.SpellEnabled[spell_index]], sub_spell_index);
			break;
		}
		case 1: //right
		{
			type_entity_0x6E8E *actEvent = Entities_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
			actEvent->dword_0xA4_164x->str_611.SpellIndexRight_0x453_1107 = spell_index;
			actEvent->dword_0xA4_164x->str_611.SubSpellIndexRight_1110 = sub_spell_index;
			//actEvent->dword_0xA4_164x->str_611.SubSpellIndexRight_1110 = D41A0_0.array_0x6E3E[D41A0_0.LevelIndex_0xc].str_0x6E3E_byte2;
			x_D41A0_BYTEARRAY_4_struct.rightSpellPlayerIndex_38401 = 8;
			SetSpell_6D5E0(Entities_EA3E4[actEvent->dword_0xA4_164x->str_611.SpellsEnabled_0x333_819x.SpellEnabled[spell_index]], sub_spell_index);
			break;
		}
	}
}

drawSpellInfoType getDraWSpellInfo(type_entity_0x6E8E *playerEvent) //20f260
{
	drawSpellInfoType result;
	result.spellIndex = -1;
	if (playerEvent > Entities_EA3E4[0]) {
		GetFont_6FC50(FontType_D419D);
		type_entity_0x6E8E *parentEvent = Entities_EA3E4[playerEvent->parentId_0x28_40];
		if (parentEvent > Entities_EA3E4[0]) {
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
					if (!parentEvent->dword_0xA4_164x->word_0x3A_58 || playerEvent->manaRegen_0x88_136 > Entities_EA3E4[parentEvent->dword_0xA4_164x->word_0x3A_58]->mana_0x90_144)
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

Array MBEXclass::getSelectedSpells() {
	Array result;
	drawSpellInfoType spellIndex;
	type_entity_0x6E8E *playerEntity = Entities_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
	//if (x_D41A0_BYTEARRAY_4_struct.leftSpellPlayerIndex_38400) {
	spellIndex = getDraWSpellInfo(Entities_EA3E4[playerEntity->dword_0xA4_164x->str_611.SpellsEnabled_0x333_819x.SpellEnabled[playerEntity->dword_0xA4_164x->str_611.SpellIndexLeft_0x451_1105]]);
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
	spellIndex = getDraWSpellInfo(Entities_EA3E4[playerEntity->dword_0xA4_164x->str_611.SpellsEnabled_0x333_819x.SpellEnabled[playerEntity->dword_0xA4_164x->str_611.SpellIndexRight_0x453_1107]]);
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

Array MBEXclass::getActiveSpells() {
	unsigned __int16 entityIndex;
	type_entity_0x6E8E *playerEntity = Entities_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
	uint8 color0 = playersColors_E88E0x[GetTrueWizardNumber_61790(playerEntity->dword_0xA4_164x->playerColorIndex_0x38_56)][0];
	uint8 color1 = playersColors_E88E0x[GetTrueWizardNumber_61790(playerEntity->dword_0xA4_164x->playerColorIndex_0x38_56)][1];
	int spellIconIndex = 0;
	Array result;
	while (spellIconIndex < 26) {
		uint8_t spell_state = 0;
		uint32_t spell_mana = 0;
		uint8_t sub_spell_state[3] = { 0, 0, 0 };
		uint32_t sub_spell_mana[3] = { 0, 0, 0 };
		int spellIndex2 = spellIndex_D94FF[spellIconIndex];
		if (SPELLS_BEGIN_BUFFER_str[spellIndex_D94FF[spellIconIndex]].byte_0 && !(!isCaveLevel_D41B6 && spellIndex2 == 25)) {
			type_entity_0x6E8E *spellEntity = Entities_EA3E4[playerEntity->dword_0xA4_164x->str_611.SpellsEnabled_0x333_819x.SpellEnabled[spellIndex_D94FF[spellIconIndex]]];
			if (spellEntity > Entities_EA3E4[0]) {
				int subSpellIndex;
				if (spellIconIndex == playerEntity->dword_0xA4_164x->str_611.spellIndex_0x458_1112) {
					x_D41A0_BYTEARRAY_4_struct.spellOnCursor_50 = spellIconIndex;
					subSpellIndex = playerEntity->dword_0xA4_164x->str_611.subSpellIndex_0x459_1113;
				} else {
					subSpellIndex = playerEntity->dword_0xA4_164x->str_611.array_0x437_1079x.SpellIndex[spellIndex_D94FF[spellIconIndex]];
				}
				bool skipToLabel43 = false;
				if (SPELLS_BEGIN_BUFFER_str[spellEntity->model_0x40_64].isEnabled_1 & 4) {
					if (spellEntity->word_0x2E_46 > 0 && spellEntity->word_0x2E_46 < 32 && x_D41A0_BYTEARRAY_4_struct.colorIndex_121[1]) {
						skipToLabel43 = true;
					}
				}
				if (!skipToLabel43) {
					bool canSummon = false;
					if (!SPELLS_BEGIN_BUFFER_str[spellIndex2].subspell[subSpellIndex].maxManaLimit_A || ((entityIndex = playerEntity->dword_0xA4_164x->CastleEntityIndex_0x3A_58) != 0 && SPELLS_BEGIN_BUFFER_str[spellIndex2].subspell[subSpellIndex].maxManaLimit_A <= Entities_EA3E4[entityIndex]->mana_0x90_144)) {
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
						spell_state = 1; //draw standart - fireball
					else
						spell_state = 2; //draw transaprent - fireball
				}
			} else {
				spell_state = 3;
				//draw spell icon colorized
			}

			//int selectedSpellIndex = spellIconIndex;
			//if (selectedSpellIndex != -1)
			//spellIndex = spellIndex_D94FF[spellIconIndex];
			signed __int16 spellIndex3 = playerEntity->dword_0xA4_164x->str_611.SpellLevels_0x41D_1053z.SpellIndex[spellIconIndex];
			for (int subSpellIconIndex = 0; subSpellIconIndex < 3; subSpellIconIndex++) {
				int manaPart = 0;
				bool canSubSummon = false;
				if (!SPELLS_BEGIN_BUFFER_str[spellIconIndex].subspell[subSpellIconIndex].maxManaLimit_A || (entityIndex = playerEntity->dword_0xA4_164x->CastleEntityIndex_0x3A_58) != 0 && SPELLS_BEGIN_BUFFER_str[spellIconIndex].subspell[subSpellIconIndex].maxManaLimit_A <= Entities_EA3E4[entityIndex]->mana_0x90_144) {
					canSubSummon = true;
					manaPart = playerEntity->mana_0x90_144 / GetSpellManaCost_6D710(playerEntity, spellIconIndex, subSpellIconIndex);
				}
				if (subSpellIconIndex > spellIndex3) {
					sub_spell_state[subSpellIconIndex] = 1;
				} else {
					int bitmapIndex;
					if (canSubSummon) {
						if (manaPart)
							sub_spell_state[subSpellIconIndex] = 2;
						else
							sub_spell_state[subSpellIconIndex] = 3;
					} else {
						if (manaPart)
							sub_spell_state[subSpellIconIndex] = 4;
						else
							sub_spell_state[subSpellIconIndex] = 5;
					}
				}
			}

			/*
		for (int subSpellIconIndex = 0; subSpellIconIndex < 3; subSpellIconIndex++)
		{
			type_entity_0x6E8E *spellEntity = Entities_EA3E4[playerEntity->dword_0xA4_164x->str_611.SpellsEnabled_0x333_819x.SpellEnabled[spellIndex_D94FF[spellIconIndex]]];
			if (spellEntity > Entities_EA3E4[0])
			{
				bool skipToLabel43 = false;
				if (SPELLS_BEGIN_BUFFER_str[spellEntity->model_0x40_64].isEnabled_1 & 4) {
					if (spellEntity->word_0x2E_46 > 0 && spellEntity->word_0x2E_46 < 32 && x_D41A0_BYTEARRAY_4_struct.colorIndex_121[1]) {
						skipToLabel43 = true;
					}
				}
				if (!skipToLabel43) {
					bool canSummon = false;
					if (!SPELLS_BEGIN_BUFFER_str[spellIndex2].subspell[subSpellIconIndex].maxManaLimit_A || ((entityIndex = playerEntity->dword_0xA4_164x->CastleEntityIndex_0x3A_58) != 0 && SPELLS_BEGIN_BUFFER_str[spellIndex2].subspell[subSpellIconIndex].maxManaLimit_A <= Entities_EA3E4[entityIndex]->mana_0x90_144)) {
						canSummon = true;
					}
					if (canSummon ) {
						int manaCost = GetSpellManaCost_6D710(playerEntity, spellIndex2, subSpellIconIndex);
						if (manaCost > 0) {
							//DrawBitmap_2BB40(posX + posIconsX, posIconsY, (*filearray_2aa18c[filearrayindex_MSPRD00DATTAB].posistruct)[SPELL_TILE_BAR], scale);
							//DrawLine_2BC80(posX + posIconsX + (6 * scale), posIconsY + (28 * scale), (36 * scale) * (playerEntity->mana_0x90_144 % manaCost) / manaCost, (4 * scale), color1);
							//drawline color1-!!!!
							int manaPosX = playerEntity->mana_0x90_144 / manaCost;
							sub_spell_mana[subSpellIconIndex] = manaPosX;
							//draw manaPosX color0-!!!!
						}
					}
					if (canSummon)
						sub_spell_state[subSpellIconIndex] = 1; //draw standart - fireball
					else
						sub_spell_state[subSpellIconIndex] = 2; //draw transaprent - fireball
				}
			} else {
				sub_spell_state[subSpellIconIndex] = 3;
				//draw spell icon colorized
			}
		}		*/
		}
		Dictionary d;
		d["spell_state"] = spell_state;
		d["spell_mana"] = spell_mana;
		Array arr_sub_spell_state;
		for (int i = 0; i < 3; i++)
			arr_sub_spell_state.append(sub_spell_state[i]);
		Array arr_sub_spell_mana;
		for (int i = 0; i < 3; i++)
			arr_sub_spell_mana.append(sub_spell_mana[i]);
		d["sub_spell_state"] = arr_sub_spell_state;
		d["sub_spell_mana"] = arr_sub_spell_mana;
		result.append(d);
		spellIconIndex++;
	}
	return result;
}

void MBEXclass::set_mesh_instances(Node *p_node_bottom, Node *p_node_top, bool isCave) {
	if (mesh_instance_bottom == p_node_bottom)
		return;
	if (mesh_instance_bottom != nullptr) {
		//mesh_instance_bottom->queue_free();
		height_image_bottom.unref();
		height_texture_bottom.unref();
		control_image.unref();
		control_texture.unref();
		mesh_instance_bottom = nullptr;
	}
	if (!p_node_bottom)
		return;
	mesh_instance_bottom = Object::cast_to<MeshInstance3D>(p_node_bottom);
	if (!mesh_instance_bottom) {
		UtilityFunctions::printerr("Error: The provided node is not a MeshInstance3D!");
	}
	if (isCave) {
		if (mesh_instance_top == p_node_top)
			return;
		if (mesh_instance_top != nullptr) {
			//mesh_instance_top->queue_free();
			height_image_top.unref();
			height_texture_top.unref();
			mesh_instance_top = nullptr;
		}
		if (!p_node_top)
			return;
		mesh_instance_top = Object::cast_to<MeshInstance3D>(p_node_top);
		if (!mesh_instance_top) {
			UtilityFunctions::printerr("Error: The provided node is not a MeshInstance3D!");
		}
	}
}

void MBEXclass::recalculate_mesh(bool isCave) {
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
	mesh_instance_bottom->set_mesh(surface_tool->commit());
	if (isCave)
		mesh_instance_top->set_mesh(surface_tool->commit());
	//renew_terrain();
}

void MBEXclass::renew_terrain(bool isCave) {
	if (control_image.is_null())
		initialize_controlmap(isCave);
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
	if (height_image_bottom.is_null())
		initialize_heightmap(0);
	height_data_bottom.resize(GRID_SIZE * GRID_SIZE);
	for (int y = 0; y < GRID_SIZE; ++y) {
		for (int x = 0; x < GRID_SIZE; ++x) {
			int idx = (y % GRID_SIZE) * GRID_SIZE + (x % GRID_SIZE);
			height_data_bottom[y * GRID_SIZE + x] = (float)mapHeightmap_11B4E0[idx] * 0.125f;
		}
	}
	if (isCave) {
		if (height_image_top.is_null())
			initialize_heightmap(1);
		height_data_top.resize(GRID_SIZE * GRID_SIZE);
		for (int y = 0; y < GRID_SIZE; ++y) {
			for (int x = 0; x < GRID_SIZE; ++x) {
				int idx = (y % GRID_SIZE) * GRID_SIZE + (x % GRID_SIZE);
				height_data_top[y * GRID_SIZE + x] = (float)x_BYTE_14B4E0_second_heightmap[idx] * 0.125f;
			}
		}
	}
	update_gpu_heightmap(isCave);
	update_gpu_controlmap();
}

void MBEXclass::update_gpu_heightmap(bool isCave) {
	if (height_image_bottom.is_null())
		initialize_heightmap(0);
	PackedByteArray byte_array;
	byte_array.resize(height_data_bottom.size() * sizeof(float));
	memcpy(byte_array.ptrw(), height_data_bottom.data(), byte_array.size());
	height_image_bottom->set_data(GRID_SIZE, GRID_SIZE, false, Image::FORMAT_RF, byte_array);
	height_texture_bottom->update(height_image_bottom);
	if (isCave) {
		if (height_image_top.is_null())
			initialize_heightmap(1);
		byte_array.resize(height_data_top.size() * sizeof(float));
		memcpy(byte_array.ptrw(), height_data_top.data(), byte_array.size());
		height_image_top->set_data(GRID_SIZE, GRID_SIZE, false, Image::FORMAT_RF, byte_array);
		height_texture_top->update(height_image_top);
	}
}

void MBEXclass::update_gpu_controlmap() {
	control_image->set_data(GRID_SIZE, GRID_SIZE, false, Image::FORMAT_RGBA8, control_data);
	control_texture->update(control_image);
}

void MBEXclass::initialize_controlmap(bool isCave) {
	control_image = Image::create(GRID_SIZE, GRID_SIZE, false, Image::FORMAT_RGBA8);
	control_texture = ImageTexture::create_from_image(control_image);
	Ref<ShaderMaterial> mat = mesh_instance_bottom->get_material_override();
	if (mat.is_valid()) {
		mat->set_shader_parameter("control_map", control_texture);
	}
	if (isCave) {
		Ref<ShaderMaterial> mat = mesh_instance_top->get_material_override();
		if (mat.is_valid()) {
			mat->set_shader_parameter("control_map", control_texture);
		}
	}
}

void MBEXclass::initialize_heightmap(int index) {
	if (index == 0) {
		height_data_bottom.assign(GRID_SIZE * GRID_SIZE, 0.0f);
		height_image_bottom = Image::create(GRID_SIZE, GRID_SIZE, false, Image::FORMAT_RF);
		height_texture_bottom = ImageTexture::create_from_image(height_image_bottom);
		if (mesh_instance_bottom) {
			Ref<ShaderMaterial> mat = mesh_instance_bottom->get_material_override();
			if (mat.is_valid()) {
				mat->set_shader_parameter("height_map", height_texture_bottom);
			}
		}
	} else {
		height_data_top.assign(GRID_SIZE * GRID_SIZE, 0.0f);
		height_image_top = Image::create(GRID_SIZE, GRID_SIZE, false, Image::FORMAT_RF);
		height_texture_top = ImageTexture::create_from_image(height_image_top);
		if (mesh_instance_top) {
			Ref<ShaderMaterial> mat = mesh_instance_top->get_material_override();
			if (mat.is_valid()) {
				mat->set_shader_parameter("height_map", height_texture_top);
			}
		}
	}
}

void MBEXclass::add_triangle(Vector3 p1, Vector3 p2, Vector3 p3, Vector2 uv1, Vector2 uv2, Vector2 uv3,
		Vector2 grid_p1, Vector2 grid_p2, Vector2 grid_p3, Vector2 main_p) {
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

PackedByteArray MBEXclass::deRNC(PackedByteArray bytearray) {
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

uint8_t MBEXclass::TerrainGetTileTerrainType(int index) {
	if (index < 0 || index >= 65536)
		return 0;
	return mapTerrainType_10B4E0[index];
}

void MBEXclass::TerrainSetTileTerrainType(int index, uint8_t value) {
	if (index < 0 || index >= 65536)
		return;
	mapTerrainType_10B4E0[index] = value;
}

PackedByteArray MBEXclass::TerrainGetMapTerrainType() {
	PackedByteArray arr;
	arr.resize(65536);
	memcpy(arr.ptrw(), mapTerrainType_10B4E0, 65536);
	return arr;
}

PackedByteArray MBEXclass::TerrainGetMapHeight(int index) {
	PackedByteArray arr;
	arr.resize(65536);
	if (index == 0)
		memcpy(arr.ptrw(), mapHeightmap_11B4E0, 65536);
	else
		memcpy(arr.ptrw(), x_BYTE_14B4E0_second_heightmap, 65536);
	return arr;
}

PackedByteArray MBEXclass::TerrainGetAngle() {
	PackedByteArray arr;
	arr.resize(65536);
	memcpy(arr.ptrw(), mapAngle_13B4E0, 65536);
	return arr;
}

bool MBEXclass::REMC2GetWebInfo() {
	type_entity_0x6E8E *playerEntity = Entities_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
	if (playerEntity->dword_0xA4_164x->mobilizeCounter_0x14E_334)
		return true;
	return false;
}

PackedFloat32Array MBEXclass::GetEntites() {
	PackedFloat32Array result;
	int count = 1000;
	result.resize(count * 31);

	float *write_ptr = result.ptrw();
	int idx = 0;

	for (int i = 0; i < count; i++) {
		type_entity_0x6E8E *actEntity = Entities_EA3E4[i];

		if (!(D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 0x20u) && (particlesParameters_D951C[actEntity->word_0x5A_90].word_0 == 461))
			continue; //not show mount before end level
		if (!(D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 0x20u) && (particlesParameters_D951C[actEntity->word_0x5A_90].word_0 == 462))
			continue; //not show mount before end level

		write_ptr[idx++] = (float)actEntity->position_0x4C_76.x; //1
		write_ptr[idx++] = (float)actEntity->position_0x4C_76.y; //2
		write_ptr[idx++] = (float)actEntity->position_0x4C_76.z; //3
		write_ptr[idx++] = (float)actEntity->array_0x52_82.yaw; //4
		write_ptr[idx++] = (float)actEntity->array_0x52_82.pitch; //5
		write_ptr[idx++] = (float)actEntity->array_0x52_82.roll; //6

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

		write_ptr[idx++] = (float)actEntity->id_0x1A_26; //17
		write_ptr[idx++] = (float)actEntity->struct_byte_0xc_12_15.byte[0]; //18
		write_ptr[idx++] = (float)actEntity->struct_byte_0xc_12_15.byte[1]; //19
		write_ptr[idx++] = (float)actEntity->struct_byte_0xc_12_15.byte[2]; //20
		write_ptr[idx++] = (float)actEntity->struct_byte_0xc_12_15.byte[3]; //21
		write_ptr[idx++] = (float)particlesParameters_D951C[actEntity->word_0x5A_90].word_0; //22
		write_ptr[idx++] = (float)actEntity->yaw_0x1C_28; //23
		write_ptr[idx++] = (float)actEntity->pitch_0x1E_30; //24
		write_ptr[idx++] = (float)actEntity->mana_0x90_144; //25
		write_ptr[idx++] = (float)actEntity->life_0x8; //26
		write_ptr[idx++] = (float)actEntity->maxMana_0x8C_140; //27
		write_ptr[idx++] = (float)actEntity->maxLife_0x4; //28
		write_ptr[idx++] = (float)actEntity->playerEntityIndex_0x94_148; //29
		write_ptr[idx++] = (float)particlesParameters_D951C[actEntity->word_0x5A_90].speed_6; //30
		write_ptr[idx++] = (float)particlesParameters_D951C[actEntity->word_0x5A_90].rotSpeed_8; //31

		//playerEntity->dword_0xA4_164x->mobilizeCounter_0x14E_334

		//v7x = &str_WORD_D951C[str_F2C20ar.dword0x14x->word_0x5A_90];
		//67

		//v5x->str_0x5E_94.word_0x68_104 = a1x->id_0x1A_26;

		type_WORD_D951C part = particlesParameters_D951C[actEntity->word_0x5A_90];

		int test = particlesParameters_D951C[actEntity->word_0x5A_90].word_0;
		switch (test) {
			case 0: //hrac0-ok
				break;
			case 8: //strelec
				break;
			case 38: //ohen-ok
				break;
			case 54: //ohen-ok
				break;
			case 57: //kour1(dole)-ok
				break;
			case 58: //mana
				if (actEntity->class_0x3F_63 == 5) //mana snake
					break;
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
			case 311: //vesnican-ok
				break;
			case 411: //mummy-ok
				break;
			case 424: //muschroom1
				break; //12
			case 425: //muschroom2
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

Ref<Image> GetFrameBuffer(int width, int height) {
	uint8_t *palette = VGA_Get_Palette();
	int crop_x = 0;
	int crop_y = 0;
	int crop_w = width;
	int crop_h = height;
	PackedByteArray rgba_data;
	rgba_data.resize(crop_w * crop_h * 4);
	uint8_t *dest = rgba_data.ptrw();
	for (int r = 0; r < crop_h; ++r) {
		int row_offset = (crop_y + r) * screenWidth_18062C;
		for (int c = 0; c < crop_w; ++c) {
			uint32_t color_idx = pdwScreenBuffer_351628[row_offset + (crop_x + c)];
			int pal_pos = color_idx * 3;
			uint8_t red = palette[pal_pos + 0] * 4;
			uint8_t green = palette[pal_pos + 1] * 4;
			uint8_t blue = palette[pal_pos + 2] * 4;
			int dest_pos = (r * crop_w + c) * 4;
			dest[dest_pos + 0] = red;
			dest[dest_pos + 1] = green;
			dest[dest_pos + 2] = blue;
			dest[dest_pos + 3] = 255;
		}
	}
	Ref<Image> img = Image::create_from_data(crop_w, crop_h, false, Image::FORMAT_RGBA8, rgba_data);
	//img->save_png("user://debug.png");
	return img;
}

Vector3 get_color(const uint8_t *pal, int index) {
	return {
		pal[index * 3 + 0] / 63.0f,
		pal[index * 3 + 1] / 63.0f,
		pal[index * 3 + 2] / 63.0f
	};
}

float get_saturation(const Vector3 &col) {
	float cmax = col.x;
	cmax = std::max(cmax, col.y);
	cmax = std::max(cmax, col.z);
	float cmin = col.x;
	cmin = std::min(cmin, col.y);
	cmin = std::min(cmin, col.z);
	float chroma = cmax - cmin;
	return (cmax > 0.001f) ? chroma / cmax : 0.0f;
}

Array MBEXclass::getPaletteModifications() {
	Vector3 out_gain = Vector3(MB_Palette_gain[0], MB_Palette_gain[1], MB_Palette_gain[2]);
	Vector3 out_offset = Vector3(MB_Paletteoffset[0], MB_Paletteoffset[1], MB_Paletteoffset[2]);
	float out_sat_multiplier = MB_Palettesat_multiplier;

	/*
	float mod_max_sat_after_correction = 0.0f;
	uint8_t *mod_palette = VGA_Get_Palette();
	uint8_t *ref_palette = VGA_Get_Palette(true);
	float r = mod_palette[0xE0 * 3 + 0] / 63.0f;
	float g = mod_palette[0xE0 * 3 + 1] / 63.0f;
	float b = mod_palette[0xE0 * 3 + 2] / 63.0f;
	Color target_white = Color(r, g, b); // 63,63,63
	float rb = mod_palette[0x00 * 3 + 0] / 63.0f;
	float gb = mod_palette[0x00 * 3 + 1] / 63.0f;
	float bb = mod_palette[0x00 * 3 + 2] / 63.0f;
	if ((ref_palette[0x40 * 3 + 0] == 0) &&
			(ref_palette[0x40 * 3 + 1] == 0) &&
			(ref_palette[0x40 * 3 + 2] == 0))
	{
		rb = mod_palette[0x40 * 3 + 0] / 63.0f;
		gb = mod_palette[0x40 * 3 + 1] / 63.0f;
		bb = mod_palette[0x40 * 3 + 2] / 63.0f;
	}
	Color target_black = Color(rb, gb, bb); // 0,0,0
	out_gain = Vector3(target_white.r - target_black.r, target_white.g - target_black.g, target_white.b - target_black.b);
	out_offset = Vector3(target_black.r, target_black.g, target_black.b);
	float ref_max_sat = 0.0f;
	for (int i = 0; i < 256; i++) {
		if (i == MyUiBackGroundColorIdx)
			continue;
		Vector3 ref_col = get_color(ref_palette, i);
		Vector3 mod_col = get_color(mod_palette, i);
		ref_max_sat = max(ref_max_sat, get_saturation(ref_col));
		Vector3 corrected = (mod_col + out_offset) * out_gain;
		corrected.x = (corrected.x < 0.0f) ? 0.0f : (corrected.x > 1.0f) ? 1.0f : corrected.x;
		corrected.y = (corrected.y < 0.0f) ? 0.0f : (corrected.y > 1.0f) ? 1.0f : corrected.y;
		corrected.z = (corrected.z < 0.0f) ? 0.0f : (corrected.z > 1.0f) ? 1.0f : corrected.z;
		float sat = get_saturation(corrected);
		mod_max_sat_after_correction = max(mod_max_sat_after_correction, sat);
	}
	float intensity = 1.5f;
	out_gain.x = 1.0f + (out_gain.x - 1.0f) * intensity;
	out_gain.y = 1.0f + (out_gain.y - 1.0f) * intensity;
	out_gain.z = 1.0f + (out_gain.z - 1.0f) * intensity;
	out_offset *= intensity;
	float sat_ratio = (ref_max_sat > 0.0001f) ? mod_max_sat_after_correction / ref_max_sat : 1.0f;
	out_sat_multiplier = 1.0f + (sat_ratio - 1.0f) * intensity;
	//out_sat_multiplier = (ref_max_sat > 0.0001f) ? mod_max_sat_after_correction / ref_max_sat : 1.0f;
	*/
	Array result;
	result.push_back(out_gain);
	result.push_back(out_offset);
	result.push_back(out_sat_multiplier);
	return result;
}

bool inverse_mouseY;
bool shift_pressed = false;
void handleInputs(Dictionary inputs, int type) {
	LastPressedKey_1806E4 = 0;
	//type==0 game
	//type==1 mapmenu
	Array key_changes = inputs["key_changes"];
	for (int i = 0; i < key_changes.size(); i++) {
		Dictionary change = key_changes[i];
		int key_index = change["key_index"];
		String action = change["action"];
		bool is_pressed = (action == "pressed");
		if (type != 0) {
			switch (key_index) {
				case 0x3c00: //F2
					mainSetPress(is_pressed, 0x3b00); //F1 - help on/off
					break;
				default:
					mainSetPress(is_pressed, key_index);
			}
		} else
			switch (key_index) {
				case 0x1177:
					mainSetPress(is_pressed, /*inputMapping.Forward*/ 0x4800); //UP
					break;
				case 0x1f73:
					mainSetPress(is_pressed, /*inputMapping.Backwards*/ 0x5000); //DOWN
					break;
				case 0x1e61:
					mainSetPress(is_pressed, /*inputMapping.Left*/ 0x4b00); //LEFT
					break;
				case 0x2064:
					mainSetPress(is_pressed, /*inputMapping.Right*/ 0x4d00); //RIGHT
					break;
				case 0x3920:
					mainSetPress(is_pressed, 0x3920); //SPACE
					break;
				case 0x011B:
					mainSetPress(is_pressed, 0x011B); //ESC
					break;
				case 0x3c00: //F2
					mainSetPress(is_pressed, 0x3b00); //F1 - help on/off
					break;
				case 0x3d00: //F3
					if (x_D41A0_BYTEARRAY_4_struct.speedIndex < 2)
						mainSetPress(is_pressed, 0x3d00); //F3 - change speed
					break;
				case 0x3e00: //F4
					if (x_D41A0_BYTEARRAY_4_struct.speedIndex > 0) {
						if (is_pressed)
							x_D41A0_BYTEARRAY_4_struct.speedIndex = (x_D41A0_BYTEARRAY_4_struct.speedIndex + 1) % 3;
						mainSetPress(is_pressed, 0x3d00); //F3 - change speed
					}
					break;
				case 0x4200: //F8
					mainSetPress(is_pressed, 0x4200); //F8 - hide/show wizard names
					break;
				case 0x2d78: //X
					mainSetPress(is_pressed, 0x0E08); //BackSpace - stop move
					break;
				case 0x1de0:
					//mainSetPress(is_pressed, 0x1d00); //CTRL
					mainSetPress(is_pressed, 0x1de0); //CTRL
					//0xe0 - LEFT CTRL//inputMapping.SpellMenu
					//0xe1 - SHIFT LEFT
					//0xe2 - ALT LEFT
					//0xe3 - LEFT WIN / META
					//0xe4 - RIGHT CTRL
					//0xe5 - RIGHT SHIFT
					//0xe6 - RIGHT ALT
					//0xe7 - RIGHT WIN / META
					break;
				case 0x2ae1: //SHIFT
					shift_pressed = is_pressed;
					mainSetPress(is_pressed, 0x2ae1); //SHIFT
					break;
				case 0x38e2: //ALT
					mainSetPress(is_pressed, 0x38e2); //ALT
					break;
				/*
				case 0x5300: //DELETE
					if (is_pressed) {
						HandleButtonClick_191B0(29, 0);
						HandleButtonClick_191B0(27, 0);
					}
					break;*/
				case 0x2368: //H - change graphics type
					if (is_pressed) {
						graphics_enhance = 1 - graphics_enhance;

						if (graphics_enhance) {
							D41A0_0.m_GameSettings.str_0x2196.transparency_0x2198 = 1;
						} else
							D41A0_0.m_GameSettings.str_0x2196.transparency_0x2198 = 0;
					}
					break;
				case 0x1970: //P - pause
					if (is_pressed)
						game_paused = 1 - game_paused;
					break;
				case 0x2e63: //C - pause + spell menu
					game_paused = 1 - game_paused;
					mainSetPress(is_pressed, 0x1de0); //CTRL
					//mainSetPress(is_pressed, 0x186f);
					break;
				case 0x186f: //O - objective
					mainSetPress(is_pressed, 0x186f); //O
					break;
				case 0x1769: //I - one step in pause mode
					if (game_paused)
						oneFrameRun = true;
					break;
				case 0x1c0d: //Enter - change map type
					if (is_pressed)
						mainSetPress(is_pressed, key_index);
					break;
				case 0x266c: //L - destroy castle
					if (is_pressed) {
						type_entity_0x6E8E *event = Entities_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
						if (event->dword_0xA4_164x->CastleEntityIndex_0x3A_58)
							HandleButtonClick_191B0(42, 0);
					}
					break;
				case 0x256b: //K - kill all creatures - cheat
					KillAllCreatures_1B5F0();
					break;
				case 0x3f00: //F5
					if (type != 0)
						break;
					if (is_pressed) {
						SaveLevel_55080(0, x_D41A0_BYTEARRAY_4_struct.levelnumber_43w, (char *)""); //SAVE
						x_D41A0_BYTEARRAY_4_struct.byteindex_208 = DataFileIO::sub_55C00_TestSaveFile2(x_D41A0_BYTEARRAY_4_struct.levelnumber_43w);
						x_D41A0_BYTEARRAY_4_struct.SelectedMenuItem_38546 = 0;
						HandleButtonClick_191B0(20, x_D41A0_BYTEARRAY_4_struct.byte_38544);
					}
					break;
				case 0x4300: //F9
					if (type != 0)
						break;
					if (is_pressed) {
						LoadLevel_555D0(0, x_D41A0_BYTEARRAY_4_struct.levelnumber_43w); //LOAD
						x_D41A0_BYTEARRAY_4_struct.SelectedMenuItem_38546 = 0;
						HandleButtonClick_191B0(20, x_D41A0_BYTEARRAY_4_struct.byte_38544);
					}
					break;
				default:
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
			case 0: //MOUSE_BUTTON_LEFT
				if (is_pressed)
					buttonresult |= 0x2;
				else
					buttonresult |= 0x4;
				break;
			case 1: //MOUSE_BUTTON_RIGHT
				if (is_pressed)
					buttonresult |= 0x8;
				else
					buttonresult |= 0x10;
				break;
			case 2: //MOUSE_BUTTON_MIDDLE
				if (is_pressed)
					buttonresult |= 0x20;
				else
					buttonresult |= 0x40;
				break;
			case 3: //MOUSE_BUTTON_WHEEL_UP
				if (type != 0)
					break;
				if (is_pressed) {
					//type_entity_0x6E8E *v8x = Entities_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
					if (shift_pressed)
						specialAction = 0; //sub_18DA0(v8x, 2, 0);
					else
						specialAction = 1; //sub_18DA0(v8x, 1, 0);
				}
				break;
			case 4: //MOUSE_BUTTON_WHEEL_DOWN
				if (type != 0)
					break;
				if (is_pressed) {
					//type_entity_0x6E8E *v8x = Entities_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
					if (shift_pressed)
						specialAction = 2; //sub_18DA0(v8x, 2, 1);
					else
						specialAction = 3; //sub_18DA0(v8x, 1, 1);
				}
				break;
		}
	}

	Vector2 mouse_pos;
	if (type == 0) {
		//mouse_pos = inputs["mouse_pos"];
		mouse_pos = inputs["mouse_pos2"];
		if (mouse_pos.x < 1)
			mouse_pos.x = 1;
		if (mouse_pos.y < 1)
			mouse_pos.y = 1;
		if (mouse_pos.x > 640 - 2)
			mouse_pos.x = 640 - 2;
		if (mouse_pos.y > 480 - 2)
			mouse_pos.y = 480 - 2;
		if (x_WORD_18072C_cursor_sizex == 0 && inverse_mouseY)
			MouseEvents(buttonresult, mouse_pos.x, 480 - mouse_pos.y - 1);
		else
			MouseEvents(buttonresult, mouse_pos.x, mouse_pos.y);
	}
	if (type != 0) {
		mouse_pos = inputs["mouse_pos2"];
		MouseEvents(buttonresult, mouse_pos.x, mouse_pos.y);
	}

	if (type != 0) {
		x_DWORD_17DE38str.x_WORD_17DEEE_mouse_buttons = 0;
		if (x_WORD_180746_mouse_left_button)
			x_DWORD_17DE38str.x_WORD_17DEEE_mouse_buttons |= 1;
		if (x_WORD_180744_mouse_right_button)
			x_DWORD_17DE38str.x_WORD_17DEEE_mouse_buttons |= 2;
		x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx = x_WORD_E3760_mouse.x; //2b4760
		x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony = x_WORD_E3760_mouse.y; //2b4762
		ReadKeyboardKeysInMenu_7C050();
		if (type != 2) {
			x_WORD_180744_mouse_right_button = 0;
			x_WORD_180746_mouse_left_button = 0;
			x_WORD_18074A_mouse_right2_button = 0;
			x_WORD_18074C_mouse_left2_button = 0;
		}
	}
}

uint32_t gameTurn = 0;
void MBEXclass::RunGameStep(Dictionary inputs) {
	handleInputs(inputs, 0);
	SetFrameStart(std::chrono::system_clock::now());
	PaletteChanges_47760();
	if (!(x_D41A0_BYTEARRAY_4_struct.OptionsSettingFlag_24 & 1)) {
		sub_715B0(); //nothing draw //animate sprites
	}
	ReadGameUserInputs_89D10(); //get keys
	MouseAndKeysEvents_17A00(GameTimerTurn_17DB54);
	PlayerEvents_51BB0();
	UpdateEntities_57730();
	sub_84B80(); //prepare lightting
	sub_58F00_game_objectives(); //nothing draw
	//sub_59820(); //nothing draw-sounds
	if (!(x_D41A0_BYTEARRAY_4_struct.OptionsSettingFlag_24 & 1))
		sub_57570(); //nothing draw
	sub_575C0(); //nothing draw-load level
	PlayEntitySounds_6E150(); //nothing draw-sounds

	if (gameTurn < 2) {
		StopMusic_8E020();
		if (gameTurn == 1)
			StartMusic_8E160(D41A0_0.maptypeMusic_0x235, 0x7Fu);
		gameTurn++;
	}

	GameTimerTurn_17DB54++;
}

Dictionary MBEXclass::GetPlayerPositionRotation() {
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

void TerrainMake(PackedByteArray bytearray, String cdPath) {
	String real_cdPath = PLATFORM_GLOBALIZE_PATH(cdPath);

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

	GameTimerTurn_17DB54 = 0x40;
	x_BYTE_E36D1 = 0x7;
	unk_18058Cstr.x_WORD_1805C2_joystick = 0x7;

	x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx = 0x140;
	x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony = 0xf0;

	screenWidth_18062C = 640;

	//begin - code from sub_main
	initposistruct();
	//end - code from sub_main

	gameFolder = std::string(real_cdPath.utf8().get_data()) + "GAME/NETHERW";
	cdFolder = std::string(real_cdPath.utf8().get_data()) + "CD_Files";

	gameDataPath = GetSubDirectoryPath(gameFolder.c_str());
	cdDataPath = GetSubDirectoryPath(cdFolder.c_str());

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

	sub_46DD0_init_sound_and_music();

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
	//SetCenterScreenForFlyAssistant_6EDB0();
	//StopMusic_8E020(); //26f020
	//StartMusic_8E160(4, 0x7Fu); //26f160 //menu music
	x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 &= 0xEFu;
	x_WORD_180660_VGA_type_resolution = 8;
	sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 4);
	sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 6);

	x_DWORD_180648_map_resolution2_x = 640; //fake resolution
	x_DWORD_180644_map_resolution2_y = 480;
	//end - code from MainMenu

	x_D41A0_BYTEARRAY_4_struct.langIndex_4 = 1;

	InitLanguage_76A40();

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
		D41A0_0.NumberOfPlayers_0xe = D41A0_0.terrain_2FECE.word_0x2FED7;
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
	LoadSpr_47160();
	//end - sub_46830_main_loop

	char dataPath[MAX_PATH];
	sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/PALN-0.DAT");
	DataFileIO::ReadFileAndDecompress(dataPath, xadatapald0dat2.colorPalette_var28);
	VGA_Set_Palette(xadatapald0dat2.colorPalette_var28[0], true);

	soundActive_E3799 = true;
	soundAble_E3798 = true;
	InitSoundAndMusic_90FD0();
	//if ((x_D41A0_BYTEARRAY_4_struct.setting_byte4_25) & 0x40) InitMusicBank_8EAD0(1);

	//x_DWORD_E9C4C_langindexbuffer[374]
	if (musicAble_E37FC && musicActive_E37FD && m_iNumberOfTracks) {
		//v8 = x_D41A0_BYTEARRAY_0[196308];
		switch (D41A0_0.terrain_2FECE.MapType) {
			case MapType_t::Day:
				D41A0_0.maptypeMusic_0x235 = 2;
				break;
			case MapType_t::Night:
				D41A0_0.maptypeMusic_0x235 = 1;
				break;
			case MapType_t::Cave:
				D41A0_0.maptypeMusic_0x235 = 3;
				break;
		}
	}
}

godot::TextureRect *mainScrBufferRect = nullptr;
Ref<ImageTexture> mainTexture;

void MBEXclass::REMC2BeginGame(String cdPath, String gamePath, int customLevel, String CustomLevelPath) {
	UtilityFunctions::print("REMC2BeginGame START");
	UtilityFunctions::print("REMC2BeginGame cdPath: ", cdPath);
	UtilityFunctions::print("REMC2BeginGame gamePath: ", gamePath);
	UtilityFunctions::print("REMC2BeginGame customLevel: ", customLevel);
	UtilityFunctions::print("REMC2BeginGame CustomLevelPath: ", CustomLevelPath);
	saved_real_cdPath = PLATFORM_GLOBALIZE_PATH(cdPath);
	// ── Android: res:// gamePath → zkopíruj do user://, použij user:// cestu ──
	saved_real_gamePath = PLATFORM_GLOBALIZE_PATH(gamePath);
	// ── konec Android výjimky ─────────────────────────────────────────────────
	UtilityFunctions::print("REMC2BeginGame saved_real_cdPath: ", saved_real_cdPath);
	UtilityFunctions::print("REMC2BeginGame saved_real_gamePath: ", saved_real_gamePath);
	for (int i = 0; i < 5; ++i)
		saved_argv[i] = nullptr;
	saved_argv[0] = (char *)"game.exe";
	saved_argv[1] = (char *)"";
	saved_argv[2] = (char *)"--auto_change_res";
	if (customLevel == -1) {
		if (CustomLevelPath == "") {
			saved_argc = 3;
			UtilityFunctions::print("REMC2BeginGame mode: default, argc=3");
		} else {
			saved_argc = 5;
			saved_argv[3] = (char *)"--custom_level";
			String globalCLPath = PLATFORM_GLOBALIZE_PATH(CustomLevelPath);
			static std::string persistentPath;
			persistentPath = globalCLPath.utf8().get_data();
			saved_argv[4] = (char *)persistentPath.c_str();
			UtilityFunctions::print("REMC2BeginGame mode: custom_level path: ", globalCLPath);
		}
	} else {
		saved_argc = 5;
		saved_argv[3] = (char *)"--set_level";
		static char levelBuffer[16];
		snprintf(levelBuffer, sizeof(levelBuffer), "%d", customLevel);
		saved_argv[4] = levelBuffer;
		UtilityFunctions::print("REMC2BeginGame mode: set_level index: ", customLevel);
	}
	UtilityFunctions::print("REMC2BeginGame calling CommandLineParams.Init...");
	CommandLineParams.Init(saved_argc, saved_argv);
	UtilityFunctions::print("REMC2BeginGame CommandLineParams.Init done");
	fixedMenuGraphics = true;
	UtilityFunctions::print("REMC2BeginGame calling support_begin...");
	support_begin();
	UtilityFunctions::print("REMC2BeginGame support_begin done");
	{
		std::lock_guard<std::mutex> lock(main_mutex);
		thread2_state = Thread2_State::BEGIN;
		thread1_state = Thread1_State::BEGIN;
		thread2_waiting = false;
		thread1_waiting = true;
	}
	UtilityFunctions::print("REMC2BeginGame mutex state initialized");
	static std::string persistentCdPath;
	static std::string persistentGamePath;
	persistentCdPath = saved_real_cdPath.utf8().get_data();
	persistentGamePath = saved_real_gamePath.utf8().get_data();
	UtilityFunctions::print("REMC2BeginGame persistentCdPath: ", String(persistentCdPath.c_str()));
	UtilityFunctions::print("REMC2BeginGame persistentGamePath: ", String(persistentGamePath.c_str()));
	UtilityFunctions::print("REMC2BeginGame starting thread 2...");
	t2 = std::thread([this]() {
		UtilityFunctions::print("Thread 2: started");
		sub_main_mod(saved_argc, saved_argv,
				(char *)persistentCdPath.c_str(),
				(char *)persistentGamePath.c_str());
		UtilityFunctions::print("Thread 2: finished");
	});
	//t2.detach();
	UtilityFunctions::print("REMC2BeginGame thread 2 detached, waiting...");
	{
		std::unique_lock<std::mutex> lock(main_mutex);
		main_cv.wait(lock, [] {
			UtilityFunctions::print("REMC2BeginGame checking thread2_waiting: ", thread2_waiting);
			return thread2_waiting == true;
		});
	}
	UtilityFunctions::print("REMC2BeginGame thread2_waiting received, END");
}

/*
void MBEXclass::REMC2BeginGame(String cdPath) { //OK!!
	String real_cdPath = ProjectSettings::get_singleton()->globalize_path(cdPath);

	int argc = 3;
	char *argv[3];
	char arg1[] = "game.exe";
	char arg2[] = "";
	char arg3[] = "--auto_change_res";
	argv[0] = arg1;
	argv[1] = arg2;
	argv[2] = arg3;

	CommandLineParams.Init(argc, argv);

	support_begin();

	// Reset před čekáním !
	{
		std::lock_guard<std::mutex> lock(main_mutex);
		thread1_turn = false;
		thread2_state = Thread2_State::BEGIN;
		thread1_state = Thread1_State::BEGIN;
	}

	std::thread t2([&]() {
		sub_main_mod(argc, argv, (char *)real_cdPath.utf8().get_data());
	});

	// Main čeká na signál od vlákna 2 - POUZE JEDNOU
	{
		std::unique_lock<std::mutex> lock(main_mutex);
		main_cv.wait(lock, [] { return thread1_turn; });
		thread1_state = Thread1_State::RUNNING;
	}
	//sub_main_mod_begin(argc, argv,(char *) real_cdPath.utf8().get_data());
	//MBEXstate = 1;

	//changeLanguage(2);//added code
}*/

void REMC2Continue() {
	// Tato funkce odblokuje vlákno 2
	{
		std::lock_guard<std::mutex> lock(main_mutex);
		thread1_turn = false; // reset pro další čekání
		thread2_state = Thread2_State::RUNNING;
		// dej vláknu 2 signál ať pokračuje
	}
	main_cv.notify_one();
}

void MBEXclass::request_thread2_quit() {
	if (!t2.joinable())
		return;
	{
		std::lock_guard<std::mutex> lock(main_mutex);
		thread2_quit_requested.store(true);
		thread2_waiting = false;
		thread1_waiting = false;
	}
	main_cv.notify_all();
	t2.join();
	thread2_quit_requested.store(false);
}

void MBEXclass::REMC2EndGame() { //OK!!
	request_thread2_quit();
	sub_main_mod_end();
	support_end();
	//MBEXstate = 6;
}

std::set<uint32_t> used_colors; //test used colors in palette

Ref<Image> getScrBufferImg(uint8_t transparentColor = 255) {
	uint8_t locTransparentColor = transparentColor;

	POSITION tempRes = VGA_GetResolution();
	int crop_w = tempRes.x;
	int crop_h = tempRes.y;
	uint8_t *palette = VGA_Get_Palette();
	int crop_x = 0;
	int crop_y = 0;
	PackedByteArray rgba_data;
	rgba_data.resize(crop_w * crop_h * 4);
	uint8_t *dest = rgba_data.ptrw();
	for (int r = 0; r < crop_h; ++r) {
		int row_offset = (crop_y + r) * screenWidth_18062C;
		for (int c = 0; c < crop_w; ++c) {
			//uint32_t color_idx = pdwScreenBuffer_351628[row_offset + (crop_x + c)];
			uint32_t color_idx = tempVGABuffer[row_offset + (crop_x + c)];
			int pal_pos = color_idx * 3;
			//if(transparentColor!=255)
			if (transparentColor != 255)
				used_colors.insert(color_idx);
			if (color_idx == transparentColor && transparentColor != 255) {
				int dest_pos = (r * crop_w + c) * 4;
				dest[dest_pos + 0] = 0;
				dest[dest_pos + 1] = 0;
				dest[dest_pos + 2] = 0;
				dest[dest_pos + 3] = 0;
			} else {
				uint8_t red = palette[pal_pos + 0] * 4;
				uint8_t green = palette[pal_pos + 1] * 4;
				uint8_t blue = palette[pal_pos + 2] * 4;
				int dest_pos = (r * crop_w + c) * 4;
				dest[dest_pos + 0] = red;
				dest[dest_pos + 1] = green;
				dest[dest_pos + 2] = blue;
				dest[dest_pos + 3] = 255;
			}
		}
	}
	Ref<Image> img = Image::create_from_data(crop_w, crop_h, false, Image::FORMAT_RGBA8, rgba_data);
	return img;
}

int MBEXclass::REMC2GetGraphicsEenhance() {
	return graphics_enhance;
}

bool MBEXclass::REMC2IsHiddenLevel() {
	if (x_D41A0_BYTEARRAY_4_struct.levelnumber_43w > 24 && x_D41A0_BYTEARRAY_4_struct.levelnumber_43w < 50)
		return true;
	return false;
};

void MBEXclass::REMC2SetScrBuffer(TextureRect *scrBufferRect) {
	//PlayInfoFmv_break = false;
	mainScrBufferRect = scrBufferRect;
	//globalAnimIndex = animIndex;
	//sub_46830_main_loop_mod(0, typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Begin });
};

/*
void MBEXclass::REMC2SetCDPath(String cdPath) {
	saved_real_cdPath = ProjectSettings::get_singleton()->globalize_path(cdPath);
}*/

Dictionary MBEXclass::REMC2getWarpMouse() {
	Dictionary result;
	result["x"] = warpMouseX;
	result["y"] = warpMouseY;
	result["is"] = warpMouseIs;
	result["yRevert"] = (x_WORD_18072C_cursor_sizex == 0);
	warpMouseIs = false;
	return result;
}

void MBEXclass::REMC2SetInverseMouse(bool locInverse_mouseY) {
	if (locInverse_mouseY)
		inverse_mouseY = 0;
	else
		inverse_mouseY = 1;
}

int inGameBeginSteps = 0;

int MBEXclass::REMC2Run(Dictionary inputs, int stage) {
	switch (stage) {
		case 0: {
			if (thread2_state == Thread2_State::IN_GAME_LOOP) {
				if (inGameBeginSteps < 10) {
					if (inGameBeginSteps == 0)
						graphics_enhance = 1;
					inGameBeginSteps++;
				}
			}
			if (inGameBeginSteps > 0) {
				SetMouseWarp(true);
				handleInputs(inputs, 0);
			} else {
				SetMouseWarp(false);
				handleInputs(inputs, 2);
			}

			//thread2_continue(Thread1_State::CONTINUE);
			//if (!game_paused || oneFrameRun)
			//{
			thread1_wait_for_continue(Thread1_State::CONTINUE);
			oneFrameRun = false;
			//}
			Ref<Image> img;
			if (inGameBeginSteps > 1 && graphics_enhance)
				img = getScrBufferImg(MyUiBackGroundColorIdx); //NIGHT 254 or 10, cave 254 or 10, day 254 or 28
			else
				img = getScrBufferImg();
			if (img.is_null())
				return 0;
			if (mainTexture.is_null() || mainTexture->get_width() != img->get_width() || mainTexture->get_height() != img->get_height()) {
				mainTexture = ImageTexture::create_from_image(img);
				mainScrBufferRect->set_texture(mainTexture);
			} else {
				mainTexture->update(img);
			}
			/*
			if (PlayInfoFmv_break) {
				sub_46830_main_loop_mod(0, typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::End });
				mainTexture.unref();
			}*/
		}
			switch (thread2_state) {
				case Thread2_State::SUB_MAIN_END_FUNCTION:
					return 1;
				case Thread2_State::LANGUAGE_SETTING_CLICKED:
					return 2;
				case Thread2_State::MAIN_MENU_BEGIN:
					return 3;
				case Thread2_State::INTRO_BEGIN:
					switch (numberOfIntroVideos) {
						case 0:
							return 15;
						case 1:
							return 16;
						case 2:
							return 17;
						case 3:
							return 18;
						case 4:
							return 19;
					}
					return 4;
				case Thread2_State::IN_GAME_BEGIN:
					MBChangePalette(0);
					inGameBeginSteps = 0;
					graphics_enhance = 0;
					return 5;
				case Thread2_State::MAP_MENU_BEGIN:
					return 6;
				case Thread2_State::IN_GAME_END:
					MBChangePalette(0);
					inGameBeginSteps = 0;
					break;
			}
			return 0;
		default:
			break;
	}
	return -1;
}

bool MBEXclass::REMC2GetMapMode() {
	switch (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].MenuState_0x3DF_2BE4_12221) {
		case 6:
		case 7:
		case 8:
		case 0xB:
		case 0xC:
		case 0xE:
			return true;
			break;
	}
	return false;
}

bool MBEXclass::REMC2GetWizardsLabelsVisible() {
	return !x_D41A0_BYTEARRAY_4_struct.byteindex_207;
}

int MBEXclass::REMC2GetTerrainAlt(int x, int y) {
	axis_3d position;
	position.x = x * 256;
	position.y = y * 256;
	position.z = 0;
	return (int)(getTerrainAlt_10C40(&position) / 256);
}

void MBEXclass::REMC2EditorBegin(String cdPath) {
	String real_cdPath = PLATFORM_GLOBALIZE_PATH(cdPath);
	gameFolder = std::string(real_cdPath.utf8().get_data()) + "GAME/NETHERW";
	cdFolder = std::string(real_cdPath.utf8().get_data()) + "CD_Files";
	support_begin();
	editor_run();
};

void MBEXclass::REMC2EditorEnd() {
	support_end();
};

void MBEXclass::REMC2EditorLoop() {
	main_x();
};

/*
int MBEXclass::REMC2EditorGetTerrainValue(int type) {
	switch (type) {
		case 0:
			return tempTerrain.seed_0x2FEE5;
		case 1:
			return tempTerrain.offset_0x2FEE9;
		case 2:
			return tempTerrain.raise_0x2FEED;
		case 3:
			return tempTerrain.gnarl_0x2FEF1;
		case 4:
			return tempTerrain.river_0x2FEF5;
		case 5:
			return tempTerrain.lriver_0x2FEF9;
		case 6:
			return tempTerrain.source_0x2FEFD;
		case 7:
			return tempTerrain.snLin_0x2FF01;
		case 8:
			return tempTerrain.snFlt_0x2FF05;
		case 9:
			return tempTerrain.bhLin_0x2FF09;
		case 10:
			return tempTerrain.bhFlt_0x2FF0D;
		case 11:
			return tempTerrain.rkSte_0x2FF11;
		default:
			return 0;
	}
}

void MBEXclass::REMC2EditorSetTerrainValue(int type, int value) {
	switch (type) {
		case 0:
			tempTerrain.seed_0x2FEE5 = (uint16_t)value;
			break;
		case 1:
			tempTerrain.offset_0x2FEE9 = (uint16_t)value;
			break;
		case 2:
			tempTerrain.raise_0x2FEED = (uint16_t)value;
			break;
		case 3:
			tempTerrain.gnarl_0x2FEF1 = (uint16_t)value;
			break;
		case 4:
			tempTerrain.river_0x2FEF5 = (uint32_t)value;
			break;
		case 5:
			tempTerrain.lriver_0x2FEF9 = (uint16_t)value;
			break;
		case 6:
			tempTerrain.source_0x2FEFD = (uint16_t)value;
			break;
		case 7:
			tempTerrain.snLin_0x2FF01 = (uint16_t)value;
			break;
		case 8:
			tempTerrain.snFlt_0x2FF05 = (uint16_t)value;
			break;
		case 9:
			tempTerrain.bhLin_0x2FF09 = (uint16_t)value;
			break;
		case 10:
			tempTerrain.bhFlt_0x2FF0D = (uint16_t)value;
			break;
		case 11:
			tempTerrain.rkSte_0x2FF11 = (uint16_t)value;
			break;
	}
}
*/
/*
PackedFloat32Array MBEXclass::REMC2EditorGetTerrainEntites() {
	PackedFloat32Array result;
	int count = 1200;
	result.resize(count * 11);
	float *write_ptr = result.ptrw();
	int idx = 0;
	for (int i = 0; i < count; i++) {
		type_entity_0x30311 *actEntity = &tempTerrain.entity_0x30311[i];
		write_ptr[idx++] = (float)actEntity->type_0x30311; //1
		write_ptr[idx++] = (float)actEntity->subtype_0x30311; //2
		write_ptr[idx++] = (float)actEntity->axis2d_4.x;//3
		write_ptr[idx++] = (float)actEntity->axis2d_4.y;//4
		axis_3d position;
		position.x = actEntity->axis2d_4.x * 256;
		position.y = actEntity->axis2d_4.y * 256;
		position.z = 0;
		int positionZ = getTerrainAlt_10C40(&position) / 256;
		write_ptr[idx++] = (float)positionZ;//5
		write_ptr[idx++] = (float)actEntity->DisId;//6
		write_ptr[idx++] = (float)actEntity->word_10;//7
		write_ptr[idx++] = (float)actEntity->stageTag_12;//8
		write_ptr[idx++] = (float)actEntity->par1_14;//9
		write_ptr[idx++] = (float)actEntity->par2_16;//10
		write_ptr[idx++] = (float)actEntity->par3_18;//11
	}
	return result;
}
*/
/*
PackedFloat32Array MBEXclass::REMC2EditorGetTerrainPlayers() {
	PackedFloat32Array data;

	int players_count = 8;
	int floats_per_player = 86; // 3 (vlastnosti) + 26 + 26 + 26 (spelly) + 1 (life) + 4 (stages)

	data.resize(players_count * floats_per_player); // Alokace přesné velikosti (656 prvků)

	int idx = 0;
	for (int i = 0; i < players_count; ++i) {
		Type_WizardMapSettings_0x360D2 &w = tempTerrain.WizardMapSettings_0x360D2[i];
		type_str_0x36442 &s = tempTerrain.stages_0x36442[i];

		// Přímý zápis na indexy
		data.set(idx++, (float)w.Aggression_0x360D5);
		data.set(idx++, (float)w.Reflexes_0x360D9);
		data.set(idx++, (float)w.Perception_0x360DD);

		for (int s = 0; s < 26; ++s)
			data.set(idx++, (float)w.StartingSpells_0x360E1x[s]);

		for (int s = 0; s < 26; ++s)
			data.set(idx++, (float)w.byte_0x360FBx[s]);

		for (int s = 0; s < 26; ++s)
			data.set(idx++, (float)w.BlockedSpells_0x36115x[s]);

		data.set(idx++, (float)w.Life_0x3612F);

		data.set(idx++, (float)s.index_0);
		data.set(idx++, (float)s.stage_1);
		data.set(idx++, (float)s._axis_2d.x);
		data.set(idx++, (float)s._axis_2d.y);
	}

	return data;
}
*/
/*
PackedFloat32Array MBEXclass::REMC2EditorGetTerrainStages() {
	PackedFloat32Array data;

	int stages_count = 11;
	int floats_per_stage = 6; // 3 (vlastnosti) + 26 + 26 + 26 (spelly) + 1 (life) + 4 (stages)

	data.resize(stages_count * floats_per_stage); // Alokace přesné velikosti (656 prvků)

	int idx = 0;
	for (int i = 0; i < stages_count; ++i) {
		type_str_0x3647Ac &s = tempTerrain.StageVars_0x3647A[i];

		data.set(idx++, (float)(uint8)s.index_0x3647A_0);
		data.set(idx++, (float)s.stage_0x3647A_1);
		data.set(idx++, (float)s.str_0x3647A_2._axis_2d.x);
		data.set(idx++, (float)s.str_0x3647A_2._axis_2d.y);
		data.set(idx++, (float)s.str_0x3647C_4.axis.x);
		data.set(idx++, (float)s.str_0x3647C_4.axis.y);
	}

	return data;
}
*/
bool MBEXclass::REMC2EditorIsGroupType(int type, int subtype) {
	if ((type == 2) && (subtype == 0))
		return false;
	return true;
}

bool MBEXclass::REMC2EditorIsParentType(int type, int subtype) {
	if (type == 2)
		return false;
	if (type == 3)
		return false;
	if ((type == 5) && (subtype == 19))
		return false;
	if ((type == 10) && (subtype == 5))
		return false;
	if ((type == 10) && (subtype == 45))
		return false;
	if ((type == 11) && (subtype == 0))
		return false;
	if ((type == 11) && (subtype == 1))
		return false;
	if ((type == 11) && (subtype == 4))
		return false;
	if ((type == 11) && (subtype == 32))
		return false;
	if ((type == 14) && (subtype == 5))
		return false;
	return true;
}

void MBEXclass::REMC2EditorAddEntity(Dictionary entity) {
	int countEntities = 1200; // adjust to actual field name
	int lastFreeIndex = -1;
	for (int idx = countEntities - 1; idx > 0; idx--) {
		if (tempTerrain.entity_0x30311[idx].type_0x30311 != 0)
			break;
		lastFreeIndex = idx;
	}
	if (lastFreeIndex > -1) {
		tempTerrain.entity_0x30311[lastFreeIndex].type_0x30311 = 2;
		if (!entity.is_empty()) {
			tempTerrain.entity_0x30311[lastFreeIndex].type_0x30311 = entity["type"];
			tempTerrain.entity_0x30311[lastFreeIndex].subtype_0x30311 = entity["subtype"];
			tempTerrain.entity_0x30311[lastFreeIndex].axis2d_4.x = entity["axis_x"];
			tempTerrain.entity_0x30311[lastFreeIndex].axis2d_4.y = entity["axis_y"];
			tempTerrain.entity_0x30311[lastFreeIndex].DisId = entity["dis_id"];
			tempTerrain.entity_0x30311[lastFreeIndex].word_10 = entity["word10"];
			tempTerrain.entity_0x30311[lastFreeIndex].stageTag_12 = entity["stage_tag"];
			tempTerrain.entity_0x30311[lastFreeIndex].par1_14 = entity["par1"];
			tempTerrain.entity_0x30311[lastFreeIndex].par2_16 = entity["par2"];
			tempTerrain.entity_0x30311[lastFreeIndex].par3_18 = entity["par3"];
		}
	}
}

int MBEXclass::REMC2EditorDeleteEntites(Array p_indices) {
	if (p_indices.is_empty())
		return -1;
	int result = 1;

	std::set<int> selected_set;
	for (int i = 0; i < p_indices.size(); i++) {
		selected_set.insert((int)p_indices[i]);
	}

	int current_count = 1200;

	// Add children of selected entities (par1_14)
	for (int i = 0; i < current_count; i++) {
		if (selected_set.count(tempTerrain.entity_0x30311[i].par1_14) > 0) {
			result |= 2;
			selected_set.insert(i);
		}
	}

	// If entity has a parent NOT in the delete list -> remove from set
	std::set<int> final_set;
	for (int idx : selected_set) {
		int par = tempTerrain.entity_0x30311[idx].par1_14;
		int type = tempTerrain.entity_0x30311[idx].type_0x30311;
		int subtype = tempTerrain.entity_0x30311[idx].subtype_0x30311;
		if (REMC2EditorIsParentType(type, subtype) && par != 0 && selected_set.count(par) == 0) {
			result |= 4;
			continue;
		}
		final_set.insert(idx);
	}

	if (final_set.empty()) {
		result |= 8;
		return result;
	}

	// Sort descending
	std::vector<int> to_delete(final_set.begin(), final_set.end());
	std::sort(to_delete.begin(), to_delete.end(), std::greater<int>());

	for (int index_to_remove : to_delete) {
		if (index_to_remove < 0 || index_to_remove >= current_count)
			continue;

		for (int k = 0; k < current_count; k++) {
			auto &ent = tempTerrain.entity_0x30311[k];

			// --- par1_14 (parent) ---
			if (REMC2EditorIsParentType(ent.type_0x30311, ent.subtype_0x30311)) {
				if (ent.par1_14 == index_to_remove) {
					ent.par1_14 = 0;
				} else if (ent.par1_14 > index_to_remove) {
					ent.par1_14--;
				}
			}

			// --- par2_16 (path) ---
			if (ent.par2_16 == index_to_remove) {
				ent.par2_16 = 0;
			} else if (ent.par2_16 > index_to_remove) {
				ent.par2_16--;
			}

			// --- stageTag_12 ---
			// stageTag_12 is a group identifier, not a direct entity index -> leave unchanged

			// --- DisId ---
			// DisId is a group identifier, not a direct entity index -> leave unchanged
		}

		// --- stage_vars: union_axis_2d_x (direct entity index) ---
		int sv_count = 11; // adjust to actual field name
		for (int s = 0; s < sv_count; s++) {
			auto &sv = tempTerrain.StageVars_0x3647A[s];
			if (sv.str_0x3647A_2._axis_2d.x == index_to_remove) {
				sv.str_0x3647A_2._axis_2d.x = 0;
			} else if (sv.str_0x3647A_2._axis_2d.x > index_to_remove) {
				sv.str_0x3647A_2._axis_2d.x--;
			}
		}

		// --- stages: stage where index==7 (direct entity index) ---
		int st_count = 8; // adjust to actual field name
		for (int s = 0; s < st_count; s++) {
			auto &st = tempTerrain.stages_0x36442[s];
			if (st.index_0 == 7) {
				if (st.stage_1 == index_to_remove) {
					st.stage_1 = 0;
				} else if (st.stage_1 > index_to_remove) {
					st.stage_1--;
				}
			}
		}

		// Remove entity by shifting array
		for (int j = index_to_remove; j < current_count - 1; j++) {
			tempTerrain.entity_0x30311[j] = tempTerrain.entity_0x30311[j + 1];
		}
		current_count--;
	}

	return result;
}

#include <godot_cpp/classes/zip_reader.hpp>
//#include <godot_cpp/classes/dir_access.hpp>
//#include <godot_cpp/classes/file_access.hpp>
//#include <godot_cpp/variant/utility_functions.hpp>

void MBEXaudioExtract(String path) {
	UtilityFunctions::print("MBEXaudioExtract START");
	UtilityFunctions::print("MBEXaudioExtract path: ", path);
	String source_path = "res://hidata/speech/";
	String target_path = path.path_join("speech");
	UtilityFunctions::print("MBEXaudioExtract source_path: ", source_path);
	UtilityFunctions::print("MBEXaudioExtract target_path: ", target_path);
	Ref<DirAccess> da_target = DirAccess::open(path);
	if (da_target.is_valid()) {
		UtilityFunctions::print("MBEXaudioExtract base path opened OK");
		if (!da_target->dir_exists("speech")) {
			UtilityFunctions::print("MBEXaudioExtract speech dir not exists, creating...");
			da_target->make_dir("speech");
			UtilityFunctions::print("MBEXaudioExtract speech dir created: ", target_path);
		} else {
			UtilityFunctions::print("MBEXaudioExtract speech dir already exists");
		}
	} else {
		UtilityFunctions::printerr("MBEXaudioExtract failed to open base path: ", path);
		return;
	}
	Ref<DirAccess> dir = DirAccess::open(source_path);
	if (dir.is_null()) {
		UtilityFunctions::printerr("MBEXaudioExtract failed to open source path: ", source_path);
		return;
	}
	UtilityFunctions::print("MBEXaudioExtract source path opened OK");
	dir->list_dir_begin();
	String file_name = dir->get_next();
	while (file_name != "") {
		if (!dir->current_is_dir() && file_name.ends_with(".zip")) {
			String full_zip_path = source_path + file_name;
			UtilityFunctions::print("MBEXaudioExtract processing ZIP: ", full_zip_path);
			Ref<ZIPReader> zip_reader;
			zip_reader.instantiate();
			if (zip_reader->open(full_zip_path) == OK) {
				UtilityFunctions::print("MBEXaudioExtract ZIP opened OK");
				PackedStringArray files = zip_reader->get_files();
				UtilityFunctions::print("MBEXaudioExtract files in ZIP: ", files.size());
				for (int i = 0; i < files.size(); i++) {
					String internal_file = files[i];
					UtilityFunctions::print("MBEXaudioExtract extracting: ", internal_file);
					PackedByteArray data = zip_reader->read_file(internal_file);
					UtilityFunctions::print("MBEXaudioExtract data size: ", data.size());
					if (data.size() > 0) {
						String out_path = target_path.path_join(internal_file);
						UtilityFunctions::print("MBEXaudioExtract writing to: ", out_path);
						Ref<FileAccess> f_out = FileAccess::open(out_path, FileAccess::WRITE);
						if (f_out.is_valid()) {
							f_out->store_buffer(data);
							UtilityFunctions::print("MBEXaudioExtract extracted OK: ", out_path);
						} else {
							UtilityFunctions::printerr("MBEXaudioExtract failed to open for write: ", out_path);
						}
					} else {
						UtilityFunctions::printerr("MBEXaudioExtract empty data for: ", internal_file);
					}
				}
				zip_reader->close();
				UtilityFunctions::print("MBEXaudioExtract ZIP closed");
			} else {
				UtilityFunctions::printerr("MBEXaudioExtract failed to open ZIP: ", file_name);
			}
		}
		file_name = dir->get_next();
	}
	UtilityFunctions::print("MBEXaudioExtract END");
}

Dictionary MBEXclass::REMC2EditorGetLevelData() {
	Dictionary d;

	// ── Hlavička ────────────────────────────────────────────────
	d["word_2FECE"] = (int)tempTerrain.word_2FECE;
	d["levelID"] = (int)tempTerrain.levelID_2FED0;
	d["byte_2FED2"] = (int)tempTerrain.byte_0x2FED2;
	d["byte_2FED3"] = (int)tempTerrain.byte_0x2FED3;
	d["map_type"] = (int)tempTerrain.MapType;
	d["word_2FED5"] = (int)tempTerrain.word_0x2FED5;
	d["word_2FED7"] = (int)tempTerrain.word_0x2FED7;
	d["seed"] = (int)tempTerrain.seed_0x2FEE5;
	d["offset"] = (int)tempTerrain.offset_0x2FEE9;
	d["raise"] = (int)tempTerrain.raise_0x2FEED;
	d["gnarl"] = (int)tempTerrain.gnarl_0x2FEF1;
	d["river"] = (int64_t)tempTerrain.river_0x2FEF5;
	d["lriver"] = (int)tempTerrain.lriver_0x2FEF9;
	d["source"] = (int)tempTerrain.source_0x2FEFD;
	d["snLin"] = (int)tempTerrain.snLin_0x2FF01;
	d["snFlt"] = (int)tempTerrain.snFlt_0x2FF05;
	d["bhLin"] = (int)tempTerrain.bhLin_0x2FF09;
	d["bhFlt"] = (int)tempTerrain.bhFlt_0x2FF0D;
	d["rkSte"] = (int)tempTerrain.rkSte_0x2FF11;
	d["next"] = (int)tempTerrain.next_0x360D1;

	// ── Hráči ───────────────────────────────────────────────────
	Array players;
	for (int i = 0; i < 8; i++)
		players.push_back((int)tempTerrain.player_0x2FED9[i]);
	d["players"] = players;

	// ── Entity (1200) ───────────────────────────────────────────
	Array entities;
	for (int i = 0; i < 1200; i++) {
		auto &e = tempTerrain.entity_0x30311[i];
		Dictionary ed;
		ed["type"] = (int)e.type_0x30311;
		ed["subtype"] = (int)e.subtype_0x30311;
		ed["axis_x"] = (int)e.axis2d_4.x;
		ed["axis_y"] = (int)e.axis2d_4.y;
		ed["dis_id"] = (int)e.DisId;
		ed["word10"] = (int)e.word_10;
		ed["stage_tag"] = (int)e.stageTag_12;
		ed["par1"] = (int)e.par1_14;
		ed["par2"] = (int)e.par2_16;
		ed["par3"] = (int)e.par3_18;
		axis_3d position;
		position.x = e.axis2d_4.x * 256;
		position.y = e.axis2d_4.y * 256;
		position.z = 0;
		ed["axis_z"] = (float)(getTerrainAlt_10C40(&position) / 256);

		entities.push_back(ed);
	}
	d["entities"] = entities;

	// ── WizardMapSettings (8) ───────────────────────────────────
	Array wizards;
	for (int i = 0; i < 8; i++) {
		auto &w = tempTerrain.WizardMapSettings_0x360D2[i];
		Dictionary wd;
		wd["aggression"] = (int)w.Aggression_0x360D5;
		wd["reflexes"] = (int)w.Reflexes_0x360D9;
		wd["perception"] = (int)w.Perception_0x360DD;
		wd["life"] = (int)w.Life_0x3612F;

		PackedByteArray starting, byte_arr, blocked;
		starting.resize(26);
		byte_arr.resize(26);
		blocked.resize(26);
		memcpy(starting.ptrw(), w.StartingSpells_0x360E1x, 26);
		memcpy(byte_arr.ptrw(), w.byte_0x360FBx, 26);
		memcpy(blocked.ptrw(), w.BlockedSpells_0x36115x, 26);
		wd["starting_spells"] = starting;
		wd["byte_array"] = byte_arr;
		wd["blocked_spells"] = blocked;
		wizards.push_back(wd);
	}
	d["wizards"] = wizards;

	// ── Stages (8) ──────────────────────────────────────────────
	Array stages;
	for (int i = 0; i < 8; i++) {
		auto &s = tempTerrain.stages_0x36442[i];
		Dictionary sd;
		sd["index"] = (int)s.index_0;
		sd["stage"] = (int)s.stage_1;
		sd["axis_x"] = (int)s._axis_2d.x;
		sd["axis_y"] = (int)s._axis_2d.y;
		stages.push_back(sd);
	}
	d["stages"] = stages;

	// ── StageVars (11) ──────────────────────────────────────────
	Array stage_vars;
	for (int i = 0; i < 11; i++) {
		auto &v = tempTerrain.StageVars_0x3647A[i];
		Dictionary vd;
		vd["index"] = (int)v.index_0x3647A_0;
		vd["stage"] = (int)v.stage_0x3647A_1;
		vd["union_axis_2d_x"] = (int)v.str_0x3647A_2._axis_2d.x;
		vd["union_axis_2d_y"] = (int)v.str_0x3647A_2._axis_2d.y;
		vd["union_dword_axis_x"] = (int)v.str_0x3647C_4.axis.x;
		vd["union_dword_axis_y"] = (int)v.str_0x3647C_4.axis.y;
		stage_vars.push_back(vd);
	}
	d["stage_vars"] = stage_vars;

	return d;
}

void MBEXclass::REMC2EditorSetLevelData(Dictionary d) {
	// ── Hlavička ────────────────────────────────────────────────
	if (d.has("word_2FECE"))
		tempTerrain.word_2FECE = (uint16_t)(int)d["word_2FECE"];
	if (d.has("levelID"))
		tempTerrain.levelID_2FED0 = (uint16_t)(int)d["levelID"];
	if (d.has("byte_2FED2"))
		tempTerrain.byte_0x2FED2 = (uint8_t)(int)d["byte_2FED2"];
	if (d.has("byte_2FED3"))
		tempTerrain.byte_0x2FED3 = (uint8_t)(int)d["byte_2FED3"];
	if (d.has("map_type"))
		tempTerrain.MapType = (MapType_t)(int)d["map_type"];
	if (d.has("word_2FED5"))
		tempTerrain.word_0x2FED5 = (int16_t)(int)d["word_2FED5"];
	if (d.has("word_2FED7"))
		tempTerrain.word_0x2FED7 = (int16_t)(int)d["word_2FED7"];
	if (d.has("seed"))
		tempTerrain.seed_0x2FEE5 = (uint16_t)(int)d["seed"];
	if (d.has("offset"))
		tempTerrain.offset_0x2FEE9 = (uint16_t)(int)d["offset"];
	if (d.has("raise"))
		tempTerrain.raise_0x2FEED = (uint16_t)(int)d["raise"];
	if (d.has("gnarl"))
		tempTerrain.gnarl_0x2FEF1 = (uint16_t)(int)d["gnarl"];
	if (d.has("river"))
		tempTerrain.river_0x2FEF5 = (uint32_t)(int64_t)d["river"];
	if (d.has("lriver"))
		tempTerrain.lriver_0x2FEF9 = (uint16_t)(int)d["lriver"];
	if (d.has("source"))
		tempTerrain.source_0x2FEFD = (uint16_t)(int)d["source"];
	if (d.has("snLin"))
		tempTerrain.snLin_0x2FF01 = (uint16_t)(int)d["snLin"];
	if (d.has("snFlt"))
		tempTerrain.snFlt_0x2FF05 = (uint16_t)(int)d["snFlt"];
	if (d.has("bhLin"))
		tempTerrain.bhLin_0x2FF09 = (uint16_t)(int)d["bhLin"];
	if (d.has("bhFlt"))
		tempTerrain.bhFlt_0x2FF0D = (uint16_t)(int)d["bhFlt"];
	if (d.has("rkSte"))
		tempTerrain.rkSte_0x2FF11 = (uint16_t)(int)d["rkSte"];
	if (d.has("next"))
		tempTerrain.next_0x360D1 = (uint8_t)(int)d["next"];

	// ── Hráči ───────────────────────────────────────────────────
	if (d.has("players")) {
		Array players = d["players"];
		for (int i = 0; i < 8 && i < players.size(); i++)
			tempTerrain.player_0x2FED9[i] = (int8_t)(int)players[i];
	}

	// ── Entity ──────────────────────────────────────────────────
	if (d.has("entities")) {
		Array entities = d["entities"];
		for (int i = 0; i < 1200 && i < entities.size(); i++) {
			Dictionary ed = entities[i];
			auto &e = tempTerrain.entity_0x30311[i];
			if (ed.has("type"))
				e.type_0x30311 = (uint16_t)(int)ed["type"];
			if (ed.has("subtype"))
				e.subtype_0x30311 = (int16_t)(int)ed["subtype"];
			if (ed.has("axis_x"))
				e.axis2d_4.x = (int16_t)(int)ed["axis_x"];
			if (ed.has("axis_y"))
				e.axis2d_4.y = (int16_t)(int)ed["axis_y"];
			if (ed.has("dis_id"))
				e.DisId = (int16_t)(int)ed["dis_id"];
			if (ed.has("word10"))
				e.word_10 = (uint16_t)(int)ed["word10"];
			if (ed.has("stage_tag"))
				e.stageTag_12 = (int16_t)(int)ed["stage_tag"];
			if (ed.has("par1"))
				e.par1_14 = (uint16_t)(int)ed["par1"];
			if (ed.has("par2"))
				e.par2_16 = (uint16_t)(int)ed["par2"];
			if (ed.has("par3"))
				e.par3_18 = (uint16_t)(int)ed["par3"];
		}
	}

	// ── Wizards ─────────────────────────────────────────────────
	if (d.has("wizards")) {
		Array wizards = d["wizards"];
		for (int i = 0; i < 8 && i < wizards.size(); i++) {
			Dictionary wd = wizards[i];
			auto &w = tempTerrain.WizardMapSettings_0x360D2[i];
			if (wd.has("aggression"))
				w.Aggression_0x360D5 = (int16_t)(int)wd["aggression"];
			if (wd.has("reflexes"))
				w.Reflexes_0x360D9 = (int16_t)(int)wd["reflexes"];
			if (wd.has("perception"))
				w.Perception_0x360DD = (int16_t)(int)wd["perception"];
			if (wd.has("life"))
				w.Life_0x3612F = (int16_t)(int)wd["life"];

			auto copySpells = [](PackedByteArray &src, uint8_t *dst) {
				memcpy(dst, src.ptr(), MIN(src.size(), 26));
			};
			if (wd.has("starting_spells")) {
				PackedByteArray v = wd["starting_spells"];
				copySpells(v, w.StartingSpells_0x360E1x);
			}
			if (wd.has("byte_array")) {
				PackedByteArray v = wd["byte_array"];
				copySpells(v, w.byte_0x360FBx);
			}
			if (wd.has("blocked_spells")) {
				PackedByteArray v = wd["blocked_spells"];
				copySpells(v, w.BlockedSpells_0x36115x);
			}
		}
	}

	// ── Stages ──────────────────────────────────────────────────
	if (d.has("stages")) {
		Array stages = d["stages"];
		for (int i = 0; i < 8 && i < stages.size(); i++) {
			Dictionary sd = stages[i];
			auto &s = tempTerrain.stages_0x36442[i];
			if (sd.has("index"))
				s.index_0 = (int8_t)(int)sd["index"];
			if (sd.has("stage"))
				s.stage_1 = (int16_t)(int)sd["stage"];
			if (sd.has("axis_x"))
				s._axis_2d.x = (int16_t)(int)sd["axis_x"];
			if (sd.has("axis_y"))
				s._axis_2d.y = (int16_t)(int)sd["axis_y"];
		}
	}

	// ── StageVars ───────────────────────────────────────────────
	if (d.has("stage_vars")) {
		Array stage_vars = d["stage_vars"];
		for (int i = 0; i < 11 && i < stage_vars.size(); i++) {
			Dictionary vd = stage_vars[i];
			auto &v = tempTerrain.StageVars_0x3647A[i];
			if (vd.has("index"))
				v.index_0x3647A_0 = (int8_t)(int)vd["index"];
			if (vd.has("stage"))
				v.stage_0x3647A_1 = (int8_t)(int)vd["stage"];
			if (vd.has("union_axis_2d_x"))
				v.str_0x3647A_2._axis_2d.x = (uint8_t)(int)vd["union_axis_2d_x"];
			if (vd.has("union_axis_2d_y"))
				v.str_0x3647A_2._axis_2d.y = (uint8_t)(int)vd["union_axis_2d_y"];
			if (vd.has("union_dword_axis_x"))
				v.str_0x3647C_4.axis.x = (uint16_t)(int)vd["union_dword_axis_x"];
			if (vd.has("union_dword_axis_y"))
				v.str_0x3647C_4.axis.y = (uint16_t)(int)vd["union_dword_axis_y"];
		}
	}
}

#include <vector>

void MBEXclass::REMC2EditorExportToCSV() {
	// Definice cesty v rámci uživatelského adresáře Godotu
	String p_path = "user://level.csv";

	Ref<FileAccess> f = FileAccess::open(p_path, FileAccess::WRITE);
	if (f.is_null()) {
		return;
	}

	// Získání dat ze stávající funkce (vrací Dictionary)
	Dictionary d = REMC2EditorGetLevelData();

	// ── HLAVIČKA ────────────────────────────────────────────────
	f->store_line("SECTION;Header");
	f->store_line("Key;Value");

	std::vector<String> header_keys = {
		"word_2FECE", "levelID", "byte_2FED2", "byte_2FED3", "map_type",
		"word_2FED5", "word_2FED7", "seed", "offset", "raise", "gnarl",
		"river", "lriver", "source", "snLin", "snFlt", "bhLin", "bhFlt",
		"rkSte", "next_360D1"
	};

	for (const String &key : header_keys) {
		if (d.has(key)) {
			f->store_line(key + String(";") + itos(d[key]));
		}
	}
	f->store_line("");

	// ── HRÁČI ───────────────────────────────────────────────────
	f->store_line("SECTION;Players");
	if (d.has("players")) {
		Array players = d["players"];
		String p_line = "Player Indices;";
		for (int i = 0; i < players.size(); i++) {
			p_line += itos(players[i]) + (i < players.size() - 1 ? "," : "");
		}
		f->store_line(p_line);
	}
	f->store_line("");

	// ── HELPER: popis entity dle type+subtype ───────────────────
	auto get_entity_description = [](int type, int subtype) -> String {
		switch (type) {
			case 2: { // Scenery
				switch (subtype) {
					case 0:
						return "Scenery-Tree";
					case 1:
						return "Scenery-Standing Stone/Statue";
					case 2:
						return "Scenery-Dolmen";
					case 3:
						return "Scenery-Statue";
					/*
					case 4:
						return "Scenery-Blue Dome";
					case 5:
						return "Scenery-Blue Dome (dup)";					
					default:
						return "Scenery-Unknown";
					*/
				}
			}
			case 3: { // Player Spawn
				switch (subtype) {
					case 4:
						return "Spawn-Flyer1";
					case 5:
						return "Spawn-Flyer2";
					case 6:
						return "Spawn-Flyer3";
					case 7:
						return "Spawn-Flyer4";
					case 8:
						return "Spawn-Flyer5";
					case 9:
						return "Spawn-Flyer6";
					case 10:
						return "Spawn-Flyer7";
					case 11:
						return "Spawn-Flyer8";
					/*
					default:
						return "Spawn-Unknown";
					*/
				}
			}
			case 5: { // Creatures
				switch (subtype) {
					/*
					case 0:
						return "Creatures-Dragon";
					*/
					case 1:
						return "Creatures-Goat";
					case 2:
						return "Creatures-Bee";
					case 3:
						return "Creatures-Worm";
					case 4:
						return "Creatures-Archer";
					/*
					case 5:
						return "Creatures-Crab";
					case 6:
						return "Creatures-Kraken";
					case 7:
						return "Creatures-Troll/Ape";
					case 8:
						return "Creatures-Griffin";
					case 9:
						return "Creatures-Skeleton";
					case 10:
						return "Creatures-Emu";
					case 11:
						return "Creatures-Genie";
					case 12:
						return "Creatures-Builder";
					*/
					case 13:
						return "Creatures-Townie";
					case 14:
						return "Creatures-Trader";
					case 16:
						return "Creatures-Wyvern";
					case 19:
						return "Creatures-FireFly";
					/*
					default:
						return "Creatures-Unknown";
					*/
				}
			}
			/*
			case 7: { // Weather
				switch (subtype) {
					case 0:
						return "Weather-Tornado";
					case 1:
						return "Weather-Rain Cloud";
					case 2:
						return "Weather-Thunder Cloud";
					case 3:
						return "Weather-Thermals";
					case 4:
						return "Weather-Wind";
					default:
						return "Weather-Unknown";
				}
			}
			case 9: { // Spells
				switch (subtype) {
					case 0:
						return "Spell-Fireball";
					case 1:
						return "Spell-Possession";
					case 2:
						return "Spell-Castle";
					case 3:
						return "Spell-Speed Up";
					case 4:
						return "Spell-Morph";
					case 5:
						return "Spell-Heal";
					case 6:
						return "Spell-Shield";
					case 7:
						return "Spell-Lightning";
					case 8:
						return "Spell-Rebound";
					case 9:
						return "Spell-Meteor";
					case 10:
						return "Spell-Teleport";
					case 11:
						return "Spell-Invisible";
					case 12:
						return "Spell-Steal Mana";
					case 13:
						return "Spell-Beyond Sight";
					case 14:
						return "Spell-Duel";
					case 15:
						return "Spell-Tremor";
					case 16:
						return "Spell-Crater";
					case 17:
						return "Spell-Earthquake";
					case 18:
						return "Spell-Volcano";
					case 19:
						return "Spell-Summon Army";
					case 20:
						return "Spell-Gravity Well";
					case 21:
						return "Spell-Whirlwind";
					case 22:
						return "Spell-Foll's Mana";
					case 23:
						return "Spell-Magic Mine";
					case 24:
						return "Spell-Alliance";
					case 25:
						return "Spell-Cave In";
					default:
						return "Spell-Unknown";
				}
			}
			*/
			case 10: { // Effects
				switch (subtype) {
					case 0:
						return "Effect-Explosion";
					case 1:
						return "Effect-Big Explosion";
					/*
					case 2:
						return "Effect-Dust";
					case 3:
						return "Effect-Blood";
					*/
					case 4:
						return "Effect-Wizard";
					case 5:
						return "Effect-Splash";
					case 6:
						return "Effect-Fire";
					/*
					case 7:
						return "Effect-Freeze";
					case 8:
						return "Effect-Mini Volcano";
					*/
					case 9:
						return "Effect-Volcano";
					/*
					case 10:
						return "Effect-Mini Crater";					
					case 11:
						return "Effect-Crater";
					case 12:
						return "Effect-Possession";
					*/
					case 13:
						return "Effect-White Smoke";
					case 14:
						return "Effect-Black Smoke";
					/*
					case 15:
						return "Effect-Earthquake";
					*/
					case 17:
						return "Effect-Meteor";
					/*
					case 21:
						return "Effect-Steal Mana Trap";
					*/
					case 22:
						return "Effect-Wind";
					/*
					case 23:
						return "Effect-Lightning";
					case 24:
						return "Effect-Rain of Fire";
					case 25:
						return "Effect-Unknown";
					*/
					case 27:
						return "Effect-Wall2";
					case 28:
						return "Effect-Wall";					
					case 29:
						return "Effect-Path";
					/*
					case 31:
						return "Effect-Canyon";
					case 34:
						return "Effect-Teleport";
					*/
					case 39:
						return "Effect-Mana Ball 512";
					case 45:
						return "Effect-Villager Building";
					/*
					case 49:
						return "Effect-Unknown";
					case 50:
						return "Effect-Ridge Node";
					case 52:
						return "Effect-Crab Egg";
					*/
					case 54:
						return "Effect-Auxiliary Entity";
					case 58:
						return "Effect-Mana Ball 2560";
					case 59:
						return "Effect-Smoke";
					case 60:
						return "Effect-Smoke2";
					case 63:
						return "Effect-Auxiliary entity2";
					case 64:
						return "Effect-Nothing";
					case 76:
						return "Effect-Fire Spheres";
					case 80:
						return "Effect-Terrain Tunnel";
					case 82:
						return "Effect-Terrain Hall";
					case 83:
						return "Effect-Terrain Fat Stalagmit";
					case 84:
						return "Effect-Terrain Stalaktit";
					case 85:
						return "Effect-Terrain Stalagmit";
					case 86:
						return "Effect-Bubble";
					/*
					default:
						return "Effect-Unknown";
					*/
				}
			}
			case 11: { // Switches
				switch (subtype) {
					case 0:
						return "Switch-Hidden Inside";
					case 1:
						return "Switch-Hidden Outside";
					case 2:
						return "Switch-Hidden Inside Re";
					case 3:
						return "Switch-Hidden Outside Re";
					case 4:
						return "Switch-On Victory";
					case 5:
						return "Switch-Death Inside";
					case 6:
						return "Switch-Death Outside";
					case 7:
						return "Switch-Death Inside Re";
					case 8:
						return "Switch-Death Outside Re";
					case 9:
						return "Switch-Obvious Inside";
					case 10:
						return "Switch-Obvious Outside";
					case 11:
						return "Switch-Obvious Inside Re";
					case 12:
						return "Switch-Obvious Outside Re";
					/*
					case 13:
						return "Switch-Dragon";
					case 14:
						return "Switch-Vulture";
					case 15:
						return "Switch-Bee";
					case 16:
						return "Switch-Worm";
					case 17:
						return "Switch-Archer";
					case 18:
						return "Switch-Crab";
					case 19:
						return "Switch-Kraken";
					case 20:
						return "Switch-Troll";
					case 21:
						return "Switch-Griffon";
					case 24:
						return "Switch-Genie";
					case 29:
						return "Switch-Wyvern";
					case 30:
						return "Switch-Creature All";
					case 31:
						return "Switch-Exit Level";
					*/
					case 32:
						return "Switch-Stage Complete";
					/*
					default:
						return "Switch-Unknown";
					*/
				}
			}
			/*
			case 12: { // Spells (vases)
				switch (subtype) {
					case 0:
						return "Vase-Fireball";
					case 1:
						return "Vase-Possession";
					case 2:
						return "Vase-Castle";
					case 3:
						return "Vase-Speed Up";
					case 4:
						return "Vase-Morph";
					case 5:
						return "Vase-Heal";
					case 6:
						return "Vase-Shield";
					case 7:
						return "Vase-Lightning";
					case 8:
						return "Vase-Rebound";
					case 9:
						return "Vase-Meteor";
					case 10:
						return "Vase-Teleport";
					case 11:
						return "Vase-Invisible";
					case 12:
						return "Vase-Steal Mana";
					case 13:
						return "Vase-Beyond Sight";
					case 14:
						return "Vase-Duel";
					case 15:
						return "Vase-Tremor";
					case 16:
						return "Vase-Crater";
					case 17:
						return "Vase-Earthquake";
					case 18:
						return "Vase-Volcano";
					case 19:
						return "Vase-Summon Army";
					case 20:
						return "Vase-Gravity Well";
					case 21:
						return "Vase-Whirlwind";
					case 22:
						return "Vase-Foll's Mana";
					case 23:
						return "Vase-Magic Mine";
					case 24:
						return "Vase-Alliance";
					case 25:
						return "Vase-Cave In";
					default:
						return "Vase-Unknown";
				}
			}
			*/
			case 14: { // SubTypes 14
				switch (subtype) {
					case 3:
						return "Special-Exit Gateway";
					case 5:
						return "Special-Scroll";
					/*
					default:
						return "Type14-Unknown";
					*/
				}
			}
			case 15: { // Spell vases (alt type)
				switch (subtype) {
					case 0:
						return "Vase-Fireball";
					case 1:
						return "Vase-Possession";
					case 2:
						return "Vase-Castle";
					case 3:
						return "Vase-Speed Up";
					case 4:
						return "Vase-Morph";
					case 5:
						return "Vase-Heal";
					case 6:
						return "Vase-Shield";
					case 7:
						return "Vase-Lightning";
					case 8:
						return "Vase-Rebound";
					case 9:
						return "Vase-Meteor";
					case 10:
						return "Vase-Teleport";
					case 11:
						return "Vase-Invisible";
					case 12:
						return "Vase-Steal Mana";
					case 13:
						return "Vase-Beyond Sight";
					case 14:
						return "Vase-Duel";
					case 15:
						return "Vase-Tremor";
					case 16:
						return "Vase-Crater";
					case 17:
						return "Vase-Earthquake";
					case 18:
						return "Vase-Volcano";
					case 19:
						return "Vase-Summon Army";
					case 20:
						return "Vase-Gravity Well";
					case 21:
						return "Vase-Whirlwind";
					case 22:
						return "Vase-Foll's Mana";
					case 23:
						return "Vase-Magic Mine";
					case 24:
						return "Vase-Alliance";
					case 25:
						return "Vase-Cave In";
					/*
					default:
						return "Vase-Unknown";
					*/
				}
			}
			default:
				return "";
		}
	};

	// ── ENTITY (Filtrace type 0 && subtype 0) ───────────────────
	f->store_line("SECTION;Entities");
	f->store_line("ID;Type;Subtype;Axis_X;Axis_Y;Axis_Z;Dis_ID;Word10;Stage_Tag;Par1;Par2;Par3;;Description");

	if (d.has("entities")) {
		Array entities = d["entities"];
		for (int i = 0; i < entities.size(); i++) {
			Dictionary ed = entities[i];
			int type = ed["type"];
			int subtype = ed["subtype"];

			if (type == 0 && subtype == 0) {
				continue;
			}

			String row = itos(i) + ";";
			row += itos(type) + ";";
			row += itos(subtype) + ";";
			row += itos(ed["axis_x"]) + ";";
			row += itos(ed["axis_y"]) + ";";
			row += String::num(ed["axis_z"], 2) + ";";
			row += itos(ed["dis_id"]) + ";";
			row += itos(ed["word10"]) + ";";
			row += itos(ed["stage_tag"]) + ";";
			row += itos(ed["par1"]) + ";";
			row += itos(ed["par2"]) + ";";
			row += itos(ed["par3"]) + ";;";
			row += get_entity_description(type, subtype);
			f->store_line(row);
		}
	}
	f->store_line("");

	// ── WIZARDS (Kouzelníci) ────────────────────────────────────
	f->store_line("SECTION;Wizards");
	f->store_line("Index;Aggression;Reflexes;Perception;Life");
	if (d.has("wizards")) {
		Array wizards = d["wizards"];
		for (int i = 0; i < wizards.size(); i++) {
			Dictionary wd = wizards[i];
			String row = itos(i) + ";";
			row += itos(wd["aggression"]) + ";";
			row += itos(wd["reflexes"]) + ";";
			row += itos(wd["perception"]) + ";";
			row += itos(wd["life"]);
			f->store_line(row);
		}
	}
	f->store_line("");

	// ── STAGES ──────────────────────────────────────────────────
	f->store_line("SECTION;Stages");
	f->store_line("ID;Index;Stage;Axis_X;Axis_Y");
	if (d.has("stages")) {
		Array stages = d["stages"];
		for (int i = 0; i < stages.size(); i++) {
			Dictionary sd = stages[i];
			f->store_line(itos(i) + ";" + itos(sd["index"]) + ";" + itos(sd["stage"]) + ";" + itos(sd["axis_x"]) + ";" + itos(sd["axis_y"]));
		}
	}
	f->store_line("");

	// ── STAGE VARS ──────────────────────────────────────────────
	f->store_line("SECTION;StageVars");
	f->store_line("ID;Index;Stage;U_Axis2D_X;U_Axis2D_Y;U_Dword_X;U_Dword_Y");
	if (d.has("stage_vars")) {
		Array svars = d["stage_vars"];
		for (int i = 0; i < svars.size(); i++) {
			Dictionary vd = svars[i];
			String row = itos(i) + ";";
			row += itos(vd["index"]) + ";";
			row += itos(vd["stage"]) + ";";
			row += itos(vd["union_axis_2d_x"]) + ";";
			row += itos(vd["union_axis_2d_y"]) + ";";
			row += itos(vd["union_dword_axis_x"]) + ";";
			row += itos(vd["union_dword_axis_y"]);
			f->store_line(row);
		}
	}

	f->close();
}

void MBEXclass::REMC2EditorUndo() {
	if (urManager->canUndo()) {
		urManager->undo(tempTerrain);
	}
}

void MBEXclass::REMC2EditorRedo() {
	if (urManager->canRedo()) {
		urManager->redo(tempTerrain);
	}
}

std::chrono::steady_clock::time_point lastSaveTime;

void MBEXclass::REMC2EditorSaveState() {
	urManager->saveState(tempTerrain);
	lastSaveTime = std::chrono::steady_clock::now();
}

void MBEXclass::REMC2EditorTimedSaveState(float seconds) {
	if (!urManager) {
		return;
	}
	auto now = std::chrono::steady_clock::now();
	std::chrono::duration<float> elapsed = now - lastSaveTime;
	if (elapsed.count() >= seconds) {
		REMC2EditorSaveState();
	}
}

bool MBEXclass::REMC2EditorLoadLevel(String path) {
	String fullPath;
	if (path.is_empty()) {
		String levelName = "quickSaved";
		fullPath = PLATFORM_GLOBALIZE_PATH("user://user-levels/") + levelName + ".mc2";
	} else {
		fullPath = path;
	}
	Ref<FileAccess> file = FileAccess::open(fullPath, FileAccess::READ);
	if (file.is_null()) {
		return false;
	}
	if (file->get_length() < sizeof(tempTerrain)) {
		return false;
	}
	file->get_buffer((uint8_t *)&tempTerrain, sizeof(tempTerrain));
	return true;
}

void MBEXclass::REMC2EditorSaveLevel(String path) {
	String fullPath;
	if (path.is_empty()) {
		String levelName = "quickSaved";
		fullPath = PLATFORM_GLOBALIZE_PATH("user://user-levels/") + levelName + ".mc2";
	} else {
		fullPath = path;
	}
	/*String dirPath = "user://user-levels/";
	String levelName = "level0";
	Ref<DirAccess> dir = DirAccess::open("user://");
	if (!dir->dir_exists(dirPath)) {
		dir->make_dir_recursive(dirPath);
	}
	String fullPath = dirPath + levelName + ".mc2";*/
	Ref<FileAccess> file = FileAccess::open(fullPath, FileAccess::WRITE);
	if (file.is_valid()) {
		file->store_buffer((const uint8_t *)&tempTerrain, sizeof(tempTerrain));
	}
}

void MBEXclass::REMC2EditorCleanLevel() {
	memset(&tempTerrain, 0, sizeof(tempTerrain));
	tempTerrain.word_0x2FED7 = 1;
	for (int i = 0; i < 8; i++)
		tempTerrain.stages_0x36442[i].index_0 = -1;
	tempTerrain.entity_0x30311[1].type_0x30311 = 3; // player start
	tempTerrain.entity_0x30311[1].subtype_0x30311 = 4; // player start
	//tempTerrain.WizardMapSettings_0x360D2[0].Aggression_0x360D5= 128;
	//tempTerrain.WizardMapSettings_0x360D2[0].Reflexes_0x360D9 = 128;
	//tempTerrain.WizardMapSettings_0x360D2[0].Perception_0x360DD = 128;
	//tempTerrain.WizardMapSettings_0x360D2[0].Life_0x3612F = 0;
	for (int i = 0; i < 26; i++)
		tempTerrain.WizardMapSettings_0x360D2[0].StartingSpells_0x360E1x[i] = 1; //activate spells
}

void MBEXclass::REMC2EditorExampleLevel(int type) {
	REMC2EditorCleanLevel();
	int lastFreeIndex = 1;
	switch (type) {
		case 0:
			for (int i = 0; i < 100; i++) {
				tempTerrain.entity_0x30311[lastFreeIndex + i].type_0x30311 = 2;
				tempTerrain.entity_0x30311[lastFreeIndex + i].subtype_0x30311 = i;
				tempTerrain.entity_0x30311[lastFreeIndex + i].axis2d_4.x = 10 * (i % 25);
				tempTerrain.entity_0x30311[lastFreeIndex + i].axis2d_4.y = 10 * (int)(i / 25);
			}
			break;
		case 1:
			for (int i = 0; i < 13; i++) {
				tempTerrain.entity_0x30311[lastFreeIndex + i].type_0x30311 = 3;
				tempTerrain.entity_0x30311[lastFreeIndex + i].subtype_0x30311 = i;
				tempTerrain.entity_0x30311[lastFreeIndex + i].axis2d_4.x = 10 * (i % 25);
				tempTerrain.entity_0x30311[lastFreeIndex + i].axis2d_4.y = 10 * (int)(i / 25);
			}
			break;
		case 2:
			for (int i = 0; i < 600; i++) {
				tempTerrain.entity_0x30311[lastFreeIndex + i].type_0x30311 = 5;
				tempTerrain.entity_0x30311[lastFreeIndex + i].subtype_0x30311 = i;
				tempTerrain.entity_0x30311[lastFreeIndex + i].axis2d_4.x = 10 * (i % 25);
				tempTerrain.entity_0x30311[lastFreeIndex + i].axis2d_4.y = 10 * (int)(i / 25);
			}
			break;
		case 3:
			for (int i = 0; i < 100; i++) {
				tempTerrain.entity_0x30311[lastFreeIndex + i].type_0x30311 = 9;
				tempTerrain.entity_0x30311[lastFreeIndex + i].subtype_0x30311 = i;
				tempTerrain.entity_0x30311[lastFreeIndex + i].axis2d_4.x = 10 * (i % 25);
				tempTerrain.entity_0x30311[lastFreeIndex + i].axis2d_4.y = 10 * (int)(i / 25);
			}
			break;
		case 4:
			for (int i = 0; i < 100; i++) {
				int subtype = 64;
				if(i%2==0)
					subtype = 63;
				tempTerrain.entity_0x30311[lastFreeIndex].type_0x30311 = 10;
				tempTerrain.entity_0x30311[lastFreeIndex].subtype_0x30311 = subtype;
				tempTerrain.entity_0x30311[lastFreeIndex].DisId = 5 + i;
				tempTerrain.entity_0x30311[lastFreeIndex].stageTag_12 = 5 + i;
				tempTerrain.entity_0x30311[lastFreeIndex].axis2d_4.x = 10 * (i % 25);
				tempTerrain.entity_0x30311[lastFreeIndex++].axis2d_4.y = 10 * (int)(i / 25);

				tempTerrain.entity_0x30311[lastFreeIndex].type_0x30311 = 11;
				tempTerrain.entity_0x30311[lastFreeIndex].subtype_0x30311 = 0;
				tempTerrain.entity_0x30311[lastFreeIndex].word_10 = 5;
				tempTerrain.entity_0x30311[lastFreeIndex].stageTag_12 = 5+i;
				tempTerrain.entity_0x30311[lastFreeIndex].axis2d_4.x = 10 * (i % 25);
				tempTerrain.entity_0x30311[lastFreeIndex++].axis2d_4.y = 10 * (int)(i / 25) + 1;
				
				tempTerrain.entity_0x30311[lastFreeIndex].type_0x30311 = 2;
				tempTerrain.entity_0x30311[lastFreeIndex].subtype_0x30311 = 1;
				tempTerrain.entity_0x30311[lastFreeIndex].axis2d_4.x = 10 * (i % 25);
				tempTerrain.entity_0x30311[lastFreeIndex++].axis2d_4.y = 10 * (int)(i / 25) + 2;

				if (i % 10 == 0) {
					tempTerrain.entity_0x30311[lastFreeIndex].type_0x30311 = 2;
					tempTerrain.entity_0x30311[lastFreeIndex].subtype_0x30311 = 1;
					tempTerrain.entity_0x30311[lastFreeIndex].axis2d_4.x = 10 * (i % 25);
					tempTerrain.entity_0x30311[lastFreeIndex++].axis2d_4.y = 10 * (int)(i / 25) + 3;
				}
			}
			break;
		case 5:
			for (int i = 0; i < 169; i++) {
				tempTerrain.entity_0x30311[lastFreeIndex + i].type_0x30311 = 14;
				tempTerrain.entity_0x30311[lastFreeIndex + i].subtype_0x30311 = i;
				tempTerrain.entity_0x30311[lastFreeIndex + i].axis2d_4.x = 10 * (i % 25);
				tempTerrain.entity_0x30311[lastFreeIndex + i].axis2d_4.y = 10 * (int)(i / 25);
			}
			break;
		case 6:
			for (int i = 0; i < 77; i++) {
				tempTerrain.entity_0x30311[lastFreeIndex + i].type_0x30311 = 15;
				tempTerrain.entity_0x30311[lastFreeIndex + i].subtype_0x30311 = i;
				tempTerrain.entity_0x30311[lastFreeIndex + i].axis2d_4.x = 10 * (i % 25);
				tempTerrain.entity_0x30311[lastFreeIndex + i].axis2d_4.y = 10 * (int)(i / 25);
			}
			break;
		case 7:
			for (int i = 1; i < 101; i++) {
				tempTerrain.entity_0x30311[lastFreeIndex + i].type_0x30311 = 14;
				tempTerrain.entity_0x30311[lastFreeIndex + i].subtype_0x30311 = 5;
				tempTerrain.entity_0x30311[lastFreeIndex + i].axis2d_4.x = 10 * (i % 25);
				tempTerrain.entity_0x30311[lastFreeIndex + i].axis2d_4.y = 10 * (int)(i / 25);
			}
			break;
		default:
			break;
	}

}

void MBEXclass::REMC2EditorLoadInGameLevel(int levelIndex) {
	loadlevel(levelIndex);
}

/*
debug in android : adb logcat -s godot

in code:
UtilityFunctions::print("REMC2BeginGame: spoustim vlakno 2");
UtilityFunctions::print("thread2_waiting=", thread2_waiting);
UtilityFunctions::printerr("Neco se pokazilo!");
*/
