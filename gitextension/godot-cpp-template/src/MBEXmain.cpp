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

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node3d.hpp>

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
	godot::ClassDB::bind_method(D_METHOD("initialize_grid_data"), &MBEXclass::initialize_grid_data);
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

	godot::ClassDB::bind_method(D_METHOD("REMC2BeginGame", "text", "text"), &MBEXclass::REMC2BeginGame);
	godot::ClassDB::bind_method(D_METHOD("REMC2EndGame"), &MBEXclass::REMC2EndGame);

	godot::ClassDB::bind_method(D_METHOD("REMC2GetLevelType"), &MBEXclass::REMC2GetLevelType);
	godot::ClassDB::bind_method(D_METHOD("REMC2GetWebInfo"), &MBEXclass::REMC2GetWebInfo);

	godot::ClassDB::bind_method(D_METHOD("REMC2IsHiddenLevel"), &MBEXclass::REMC2IsHiddenLevel);

	//godot::ClassDB::bind_method(D_METHOD("REMC2SetCDPath", "text"), &MBEXclass::REMC2SetCDPath);
	godot::ClassDB::bind_method(D_METHOD("REMC2SetScrBuffer", "TextureRect"), &MBEXclass::REMC2SetScrBuffer);
	godot::ClassDB::bind_method(D_METHOD("REMC2Run", "Dictionary", "Int"), &MBEXclass::REMC2Run);

	godot::ClassDB::bind_method(D_METHOD("REMC2GetGraphicsEenhance"), &MBEXclass::REMC2GetGraphicsEenhance);
	godot::ClassDB::bind_method(D_METHOD("REMC2getWarpMouse"), &MBEXclass::REMC2getWarpMouse);

	godot::ClassDB::bind_method(D_METHOD("REMC2EditorBegin"), &MBEXclass::REMC2EditorBegin);
	godot::ClassDB::bind_method(D_METHOD("REMC2EditorEnd"), &MBEXclass::REMC2EditorEnd);
	godot::ClassDB::bind_method(D_METHOD("REMC2EditorLoop"), &MBEXclass::REMC2EditorLoop);
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

void MBEXclass::convertOriginalDataExtractCD(String path, String path2) {
	String real_path = ProjectSettings::get_singleton()->globalize_path(path);
	String real_path2 = ProjectSettings::get_singleton()->globalize_path(path2);
	MBEXcdExtract((char*)real_path2.utf8().get_data(), (char *)real_path.utf8().get_data()); //user some path
	MBEXfixLang((char*)real_path.utf8().get_data(), 2);
}

String MBEXclass::REMC2GetLevelType() {
	if (x_D41A0_BYTEARRAY_4_struct.levelnumber_43w==24)
		return "Final";
	else
		if (D41A0_0.terrain_2FECE.MapType == MapType_t::Day) {
			return String("Day");
		} else if (D41A0_0.terrain_2FECE.MapType == MapType_t::Night) {
			return String("Night");
		} else
			return String("Cave");
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

void MBEXclass::MBEXfixLang(char* path,int index) {

	char pathBuffer[512];
	sprintf(pathBuffer, "%s%s", path, "GAME/NETHERW");
	FILE *configFile2;
	char configFilePath[MAX_PATH];
	TypeConfigDat configDat;
	sprintf(configFilePath, "%s/%s", pathBuffer, "CONFIG.DAT");
	configFile2 = DataFileIO::CreateOrOpenFile(configFilePath, 512);
	if (configFile2 != nullptr) {
		DataFileIO::Read(configFile2, (uint8_t *)&configDat, sizeof(TypeConfigDat));
		DataFileIO::Close(configFile2);
		if (configDat.configDatSign_0 == 0xfffffff7) {
			configDat.langIndex_4 = index;
			configFile2 = DataFileIO::CreateOrOpenFile(configFilePath, 546);
			if (configFile2 != nullptr) {
				DataFileIO::WriteFile_98CAA(configFile2, (uint8_t *)&configDat, sizeof(TypeConfigDat));
				DataFileIO::Close(configFile2);
			}
		}
	}
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
	convertPre();
	MBEXconvertData(path, path2);
	convertPost();
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
		result.append(d);
	}
	sound_queue_clear();
	return result;	
}
/*
Array MBEXclass::getPendingGraphicsActions() {
	Array result;
	std::vector<GraphicsAction> pending = graphics_queue_get_pending_actions();
	for (size_t i = 0; i < pending.size(); i++) {
		const GraphicsAction &ga = pending[i];
		Dictionary d;
		d["action"] = String(ga.action.c_str());
		d["posx"] = ga.x;
		d["posy"] = ga.y;
		d["index"] = ga.index;
		d["type"] = ga.type;
		result.append(d);
	}
	graphics_queue_clear();
	return result;
}*/

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
	type_entity_0x6E8E* playerEntity = Entities_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
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
		uint8_t spell_state=0;
		uint32_t spell_mana=0;
		uint8_t sub_spell_state[3] = { 0, 0, 0 };
		uint32_t sub_spell_mana[3] = { 0, 0, 0 };
		int spellIndex2 = spellIndex_D94FF[spellIconIndex];
		if (SPELLS_BEGIN_BUFFER_str[spellIndex_D94FF[spellIconIndex]].byte_0 && !(!isCaveLevel_D41B6 && spellIndex2 == 25)) {
			type_entity_0x6E8E *spellEntity = Entities_EA3E4[playerEntity->dword_0xA4_164x->str_611.SpellsEnabled_0x333_819x.SpellEnabled[spellIndex_D94FF[spellIconIndex]]];
			if (spellEntity > Entities_EA3E4[0])
			{
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
						spell_state=1; //draw standart - fireball
					else
						spell_state=2; //draw transaprent - fireball
				}
			} else {
				spell_state = 3;
				//draw spell icon colorized
			}


			//int selectedSpellIndex = spellIconIndex;
			//if (selectedSpellIndex != -1)
			//spellIndex = spellIndex_D94FF[spellIconIndex];
			signed __int16 spellIndex3 = playerEntity->dword_0xA4_164x->str_611.SpellLevels_0x41D_1053z.SpellIndex[spellIconIndex];
			for (int subSpellIconIndex = 0; subSpellIconIndex < 3; subSpellIconIndex++)
			{
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
					if (canSubSummon)
					{
						if (manaPart)
							sub_spell_state[subSpellIconIndex] = 2;
						else
							sub_spell_state[subSpellIconIndex] = 3;
					}
					else
					{
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

void MBEXclass::initialize_grid_data() {
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
			continue;//not show mount before end level
		if (!(D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 0x20u) && (particlesParameters_D951C[actEntity->word_0x5A_90].word_0 == 462))
			continue; //not show mount before end level

		write_ptr[idx++] = (float)actEntity->position_0x4C_76.x; //1
		write_ptr[idx++] = (float)actEntity->position_0x4C_76.y; //2
		write_ptr[idx++] = (float)actEntity->position_0x4C_76.z;//3
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
			case 8://strelec
				break;
			case 38: //ohen-ok
				break;
			case 54: //ohen-ok
				break;
			case 57: //kour1(dole)-ok
				break;
			case 58: //mana
				if (actEntity->class_0x3F_63 == 5)//mana snake
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
				break;//12
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

Ref<Image> GetFrameBuffer(int width,int height) {
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

int game_paused = 0;
bool oneFrameRun = false;

void handleInputs(Dictionary inputs,int type) {
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
			mainSetPress(is_pressed, key_index);
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
			case 0x1d00:
				//mainSetPress(is_pressed, 0x1d00); //CTRL
				mainSetPress(is_pressed, /*inputMapping.SpellMenu*/0x1de0); //CTRL
				//0xe0 - LEFT CTRL//inputMapping.SpellMenu
				//0xe1 - SHIFT LEFT
				//0xe2 - ALT LEFT
				//0xe3 - LEFT WIN / META
				//0xe4 - RIGHT CTRL
				//0xe5 - RIGHT SHIFT
				//0xe6 - RIGHT ALT
				//0xe7 - RIGHT WIN / META
				break;
			case 0x5300: //DELETE
				if (is_pressed) {
					HandleButtonClick_191B0(29, 0);
					HandleButtonClick_191B0(27, 0);
				}
				break;
			case 0x1769: //I - objective
				mainSetPress(is_pressed, 0x186f); //O
				break;
			case 0x2368: //H - change graphics type
				if (is_pressed) {
					graphics_enhance = 1 - graphics_enhance;

					if (graphics_enhance) {
						D41A0_0.m_GameSettings.str_0x2196.transparency_0x2198 = 1;
					} else
						D41A0_0.m_GameSettings.str_0x2196.transparency_0x2198 = 0;
					oneFrameRun = true;
				}
				break;
			case 0x1970: //P - pause
				if (is_pressed)
					game_paused = 1 - game_paused;
				break;
			case 0x186f: //O - one step in pause mode
				if (game_paused)
					oneFrameRun = true;
				break;
			case 0x1c0d: //Enter - change map type
				if (is_pressed)
					mainSetPress(is_pressed, key_index);
				break;
			case 0x266c: //L - destroy castle
				if (is_pressed) {
					type_entity_0x6E8E* event = Entities_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
					if (event->dword_0xA4_164x->CastleEntityIndex_0x3A_58)
						HandleButtonClick_191B0(42, 0);
				}
				break;
			case 0x256b: //K - kill all creatures - cheat
				KillAllCreatures_1B5F0();
				break;
			case 0x3f00://F5
				if (type != 0)
					break;
				if (is_pressed) {
					SaveLevel_55080(0, x_D41A0_BYTEARRAY_4_struct.levelnumber_43w, (char *)""); //SAVE
					x_D41A0_BYTEARRAY_4_struct.byteindex_208 = DataFileIO::sub_55C00_TestSaveFile2(x_D41A0_BYTEARRAY_4_struct.levelnumber_43w);
					x_D41A0_BYTEARRAY_4_struct.SelectedMenuItem_38546 = 0;
					HandleButtonClick_191B0(20, x_D41A0_BYTEARRAY_4_struct.byte_38544);
				}
				break;
			case 0x4300://F9
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

	Vector2 mouse_pos;
	if (type == 0)
	{
		//mouse_pos = inputs["mouse_pos"];
		mouse_pos = inputs["mouse_pos2"];
		if (x_WORD_18072C_cursor_sizex == 0)
			MouseEvents(buttonresult, mouse_pos.x, 480 - mouse_pos.y);
		else
			MouseEvents(buttonresult, mouse_pos.x, mouse_pos.y);
	}
	if (type != 0)
	{
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
	String real_cdPath = ProjectSettings::get_singleton()->globalize_path(cdPath);

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

	x_DWORD_180648_map_resolution2_x = 640;//fake resolution
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
	VGA_Set_Palette(xadatapald0dat2.colorPalette_var28[0],true);

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

void MBEXclass::REMC2BeginGame(String cdPath, String gamePath) { //OK!!
	saved_real_cdPath = ProjectSettings::get_singleton()->globalize_path(cdPath);
	saved_real_gamePath = ProjectSettings::get_singleton()->globalize_path(gamePath);
	saved_argc = 3;
	saved_argv[0] = (char *)"game.exe";
	saved_argv[1] = (char *)"";
	saved_argv[2] = (char *)"--auto_change_res";

	CommandLineParams.Init(saved_argc, saved_argv);

	fixedMenuGraphics = true;

	support_begin();

	{
		std::lock_guard<std::mutex> lock(main_mutex);
		thread2_state = Thread2_State::BEGIN;
		thread1_state = Thread1_State::BEGIN;
		thread2_waiting = false;
		thread1_waiting = true;
	}

    printf("REMC2BeginGame: spoustim vlakno 2\n");
	t2 = std::thread([this]() {
		printf("Vlakno 2: startuje\n");
		sub_main_mod(saved_argc, saved_argv, (char *)saved_real_cdPath.utf8().get_data(), (char *)saved_real_gamePath.utf8().get_data());
		printf("Vlakno 2: skoncilo\n");
	});
	t2.detach();

	printf("REMC2BeginGame: cekam na vlakno 2\n");
	{
		std::unique_lock<std::mutex> lock(main_mutex);
		main_cv.wait(lock, [] {
			printf("REMC2BeginGame: kontrolujem thread2_waiting=%d\n", thread2_waiting);
			return thread2_waiting == true; // ← čekej až vlákno 2 resetuje
		});
	}
	printf("REMC2BeginGame: probудил se, konec\n");
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

void MBEXclass::REMC2EndGame() {//OK!!
	sub_main_mod_end();
	support_end();
	//MBEXstate = 6;
}

std::set<uint32_t> used_colors; //test used colors in palette

Ref<Image> getScrBufferImg(uint8_t transparentColor=255) {
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
			if (color_idx == transparentColor && transparentColor!=255) {
				int dest_pos = (r * crop_w + c) * 4;
				dest[dest_pos + 0] = 0;
				dest[dest_pos + 1] = 0;
				dest[dest_pos + 2] = 0;
				dest[dest_pos + 3] = 0;
			}  else {
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
			if (!game_paused || oneFrameRun) {
				thread1_wait_for_continue(Thread1_State::CONTINUE);
				oneFrameRun = false;
			}
				Ref<Image> img;
				if (inGameBeginSteps > 1 && graphics_enhance)
					img = getScrBufferImg(MyUiBackGroundColorIdx);//NIGHT 254 or 10, cave 254 or 10, day 254 or 28
				else
					img = getScrBufferImg();
				if (img.is_null())
					return 0;
				if (mainTexture.is_null() || mainTexture->get_width() != img->get_width() || mainTexture->get_height() != img->get_height())
				{
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
					switch (numberOfIntroVideos)
					{
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

void MBEXclass::REMC2EditorBegin()
{
};

void MBEXclass::REMC2EditorEnd()
{
};

void MBEXclass::REMC2EditorLoop()
{
};
