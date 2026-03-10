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
	godot::ClassDB::bind_method(D_METHOD("getMinimap"), &MBEXclass::getMinimap);
	godot::ClassDB::bind_method(D_METHOD("convertOriginalData", "text", "text"), &MBEXclass::convertOriginalData);
	godot::ClassDB::bind_method(D_METHOD("convertOriginalDataExtractCD", "text", "text"), &MBEXclass::convertOriginalDataExtractCD);
	//godot::ClassDB::bind_method(D_METHOD("soundQueueClear"), &MBEXclass::soundQueueClear);
	godot::ClassDB::bind_method(D_METHOD("getPendingSoundActions"), &MBEXclass::getPendingSoundActions);
	//godot::ClassDB::bind_method(D_METHOD("getPendingGraphicsActions"), &MBEXclass::getPendingGraphicsActions);
	godot::ClassDB::bind_method(D_METHOD("updateFreeSoundPlayers", "indices"), &MBEXclass::updateFreeSoundPlayers);
	godot::ClassDB::bind_method(D_METHOD("playAnim", "Int"), &MBEXclass::playAnim);
	godot::ClassDB::bind_method(D_METHOD("playAnimStep", "Int"), &MBEXclass::playAnimStep);
	godot::ClassDB::bind_method(D_METHOD("getVGABuffer"), &MBEXclass::getVGABuffer);

	godot::ClassDB::bind_method(D_METHOD("getLangTexts"), &MBEXclass::getLangTexts);
	godot::ClassDB::bind_method(D_METHOD("changeLanguage", "Int"), &MBEXclass::changeLanguage);

	godot::ClassDB::bind_method(D_METHOD("REMC2BeginGame", "text"), &MBEXclass::REMC2BeginGame);
	godot::ClassDB::bind_method(D_METHOD("REMC2EndGame"), &MBEXclass::REMC2EndGame);

	godot::ClassDB::bind_method(D_METHOD("REMC2BeginAnim", "TextureRect", "Int"), &MBEXclass::REMC2BeginAnim);
	godot::ClassDB::bind_method(D_METHOD("REMC2StepAnim", "Dictionary"), &MBEXclass::REMC2StepAnim);
	godot::ClassDB::bind_method(D_METHOD("REMC2BeginMap", "TextureRect"), &MBEXclass::REMC2BeginMap);
	godot::ClassDB::bind_method(D_METHOD("REMC2StepMap", "Dictionary"), &MBEXclass::REMC2StepMap);
	godot::ClassDB::bind_method(D_METHOD("REMC2BeginMain", "TextureRect"), &MBEXclass::REMC2BeginMain);
	godot::ClassDB::bind_method(D_METHOD("REMC2StepMain", "Dictionary"), &MBEXclass::REMC2StepMain);
	godot::ClassDB::bind_method(D_METHOD("REMC2BeginInGame"), &MBEXclass::REMC2BeginInGame);
	godot::ClassDB::bind_method(D_METHOD("REMC2BeginInGameAfterScreen"), &MBEXclass::REMC2BeginInGameAfterScreen);
	godot::ClassDB::bind_method(D_METHOD("REMC2StepInGame", "Dictionary", "Int"), &MBEXclass::REMC2StepInGame);

	godot::ClassDB::bind_method(D_METHOD("REMC2GetLevelType"), &MBEXclass::REMC2GetLevelType);
	godot::ClassDB::bind_method(D_METHOD("REMC2GetWebInfo"), &MBEXclass::REMC2GetWebInfo);

	godot::ClassDB::bind_method(D_METHOD("REMC2IsHiddenLevel"), &MBEXclass::REMC2IsHiddenLevel);
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
	MBEXcdExtract((char *)real_path2.utf8().get_data(), (char *)real_path.utf8().get_data()); //user some path
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

void MBEXclass::changeLanguage(int index) {
	x_D41A0_BYTEARRAY_4_struct.langIndex_4 = index;
	InitLanguage_76A40_mod_only_language();
}

Dictionary MBEXclass::getLangTexts() {
	Dictionary result;

	for (int i = 0; i < 1000; i++) {
		result[i] = String(x_DWORD_E9C4C_langindexbuffer[i]);
	}

	return result;
}

/*
FILE *animTempfile;
Type_SoundEvent_E17CC* tempPSoundEvent;
void PlayInfoFmvBegin(__int16 a1, __int16 a2, Type_SoundEvent_E17CC *pSoundEvent, char *path)
{
	tempPSoundEvent = pSoundEvent;
	x_WORD_E12FC = a2;
	x_WORD_D4004 = 0;
	x_WORD_17DB58 = 0;
	ActualKeyframe_17DB60 = 0;
	x_DWORD_E12F4x = (TColor *)pdwScreenBuffer_351628;
	animTempfile = DataFileIO::CreateOrOpenFile(path, 512);
	x_DWORD_17DB38_intro_file_handle = animTempfile;
	if (animTempfile) {
		DataFileIO::Read(animTempfile, unk_17DB40, 12); //ecx=12
		LastKeyframe_17DB46 = *(int16_t *)&unk_17DB40[6];
		x_WORD_17DB48 = *(int16_t *)&unk_17DB40[8];
		x_WORD_17DB4A = *(int16_t *)&unk_17DB40[10];

		x_WORD_180744_mouse_right_button = 0;
		x_WORD_180746_mouse_left_button = 0;
		x_DWORD_E1300 += 12;
		LastPressedKey_1806E4 = 0;
		x_WORD_17DB5A = 0;
		FlvInitSet_473B0(); //2283b0
		x_WORD_17DB5C = a1;
	}
}
*/
/*
bool endAnim = false;

void PlayInfoFmvStep() {
	if ((LastPressedKey_1806E4 != 1) && (!endAnim) && animTempfile)
	{
		SetFrameStart(std::chrono::system_clock::now());
		if (x_WORD_17DB5A)
			endAnim=true;
		else {
			if (ActualKeyframe_17DB60 >= LastKeyframe_17DB46 - 1) //34eb60 a 34eb46
				endAnim = true;
			else {
				PlayIntoSoundEvents_1B280(tempPSoundEvent);
				sub_75DB0();
				sub_75E70();
				ActualKeyframe_17DB60++;
			}
		}
	} else
		endAnim = true;
}
*/
int oldScreenWidth;
int old_VGA_type_resolution;

void PlayInfoFmvEnd() {
	/*
	if (animTempfile) {
		DataFileIO::Close(x_DWORD_17DB38_intro_file_handle);
	}
	screenWidth_18062C = oldScreenWidth;
	x_WORD_180660_VGA_type_resolution = old_VGA_type_resolution;
	*/
}

void MBEXclass::playAnim(int index) {
	/*
	memset(pdwScreenBuffer_351628, 0, 640 * 480); //clear screen buffer

	old_VGA_type_resolution = x_WORD_180660_VGA_type_resolution;
	oldScreenWidth = screenWidth_18062C;
	screenWidth_18062C = 320;
	x_WORD_180660_VGA_type_resolution = 1;
	InitLanguage_76A40();

	char dataPath[MAX_PATH];
	x_DWORD_17DE38str.x_DWORD_17DE54 = &x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[301787];
	x_DWORD_17DE38str.x_DWORD_17DEC0 = (bitmap_pos_struct2_t *)&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[308527];
	x_DWORD_17DE38str.x_DWORD_17DEC4 = (bitmap_pos_struct2_t *)&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[310159];
	sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/SCREENS/HSCREEN0.DAT");
	sub_7AA70_load_and_decompres_dat_file(dataPath, &x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[301787], 0x164FCD, 0x35C);
	sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.x_DWORD_17DEC0, 0x165329, 0x224);
	sub_7AA70_load_and_decompres_dat_file(0, 0, 0, 0);
	if (x_WORD_180660_VGA_type_resolution & 1)
		sub_98709_create_index_dattab_power(x_DWORD_17DE38str.x_DWORD_17DEC0, x_DWORD_17DE38str.x_DWORD_17DEC4, x_DWORD_17DE38str.x_DWORD_17DE54, xy_DWORD_17DEC0_spritestr);
	else
		sub_9874D_create_index_dattab(x_DWORD_17DE38str.x_DWORD_17DEC0, x_DWORD_17DE38str.x_DWORD_17DEC4, x_DWORD_17DE38str.x_DWORD_17DE54, xy_DWORD_17DEC0_spritestr);

	sub_2EB40();
	if (soundAble_E3798 && x_D41A0_BYTEARRAY_4_struct.SelectedLangIndex == 2) {
		x_BYTE_D41C1 = 0;
		x_BYTE_D41C0 = 0;
	} else {
		x_BYTE_D41C0 = 1;
		x_BYTE_D41C1 = 1;
	}


	//char dataPath[MAX_PATH];
	switch(index)
		{
		case 0:
			sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "INTRO/INTEL.DAT");
			PlayInfoFmvBegin(1, 1, str_E17CC_0, dataPath);
			break;
		case 1:
			sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "INTRO/INTRO.DAT");
			PlayInfoFmvBegin(1, 1, str_E17CC_0, dataPath);
			break;
		case 2:
			sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "INTRO/INTRO2.DAT");
			PlayInfoFmvBegin(1, 1, str_E17CC_0x160, dataPath);
			break;
	}
	*/
}

int MBEXclass::playAnimStep(int run) {
	/*
	if (run)
		LastPressedKey_1806E4 = 20;
	PlayInfoFmvStep();
	if (endAnim) {
		PlayInfoFmvEnd();
		return 1;
	}
	*/
	return 0;
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
			SetSpell_6D5E0(Entities_EA3E4[actEvent->dword_0xA4_164x->str_611.array_0x333_819x.SpellEnabled[spell_index]], sub_spell_index);
			break;
		}
		case 1: //right
		{
			type_entity_0x6E8E *actEvent = Entities_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
			actEvent->dword_0xA4_164x->str_611.SpellIndexRight_0x453_1107 = spell_index;
			actEvent->dword_0xA4_164x->str_611.SubSpellIndexRight_1110 = sub_spell_index;
			//actEvent->dword_0xA4_164x->str_611.SubSpellIndexRight_1110 = D41A0_0.array_0x6E3E[D41A0_0.LevelIndex_0xc].str_0x6E3E_byte2;
			x_D41A0_BYTEARRAY_4_struct.rightSpellPlayerIndex_38401 = 8;
			SetSpell_6D5E0(Entities_EA3E4[actEvent->dword_0xA4_164x->str_611.array_0x333_819x.SpellEnabled[spell_index]], sub_spell_index);
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
	spellIndex = getDraWSpellInfo(Entities_EA3E4[playerEntity->dword_0xA4_164x->str_611.array_0x333_819x.SpellEnabled[playerEntity->dword_0xA4_164x->str_611.SpellIndexLeft_0x451_1105]]);
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
		spellIndex = getDraWSpellInfo(Entities_EA3E4[playerEntity->dword_0xA4_164x->str_611.array_0x333_819x.SpellEnabled[playerEntity->dword_0xA4_164x->str_611.SpellIndexRight_0x453_1107]]);
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
			type_entity_0x6E8E *spellEntity = Entities_EA3E4[playerEntity->dword_0xA4_164x->str_611.array_0x333_819x.SpellEnabled[spellIndex_D94FF[spellIconIndex]]];
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
			signed __int16 spellIndex3 = playerEntity->dword_0xA4_164x->str_611.array_0x41D_1053z.SpellIndex[spellIconIndex];
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
				type_entity_0x6E8E *spellEntity = Entities_EA3E4[playerEntity->dword_0xA4_164x->str_611.array_0x333_819x.SpellEnabled[spellIndex_D94FF[spellIconIndex]]];
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

uint MyUiBackGroundColorIdx = 200;

void DrawGameFrame_2BE30_mod() //20CE30
{
	int16_t spellLeftPosX = 510;
	int16_t spellRightPosX = 574;
	uint8_t scale = 1;

	type_entity_0x6E8E *playerEntity;
	void (*drawBitmapFunction)(int16_t, int16_t, bitmap_pos_struct_t, uint8_t scale); // eax

	//added code
	ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, screenWidth_18062C, screenHeight_180624, MyUiBackGroundColorIdx /* uiBackGroundColorIdx_EB3A8*/);
	//added code

	SetTextBoxMinMaxForSetResolution();

	if (x_WORD_180660_VGA_type_resolution != 1) {
		if (!DefaultResolutions()) {
			scale = gameUiScale;
			spellLeftPosX = screenWidth_18062C - (130 * scale);
			spellRightPosX = screenWidth_18062C - (66 * scale);
		}
	}

	x_DWORD_D41C8 = 0;
	if (D41A0_0.m_GameSettings.str_0x2196.transparency_0x2198) {
		if (x_WORD_180660_VGA_type_resolution & 1)
			drawBitmapFunction = drawBitmap320_8F8B0;
		else
			drawBitmapFunction = drawBitmap640_8F8E8;
		ptrDrawBitmap_F01E8 = drawBitmapFunction;
	} else {
		ptrDrawBitmap_F01E8 = GameBitmap::DrawTransparentBitmap_2DE80;
	}
	ptrDrawBitmap_F01EC = ptrDrawBitmap_F01E8;
	ptrDrawBitmap_F01E8 = sub_2BBB0;
	if (x_D41A0_BYTEARRAY_4_struct.showHelp_10)
		sub_88580();
	int actPlayerIndex = D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].ActPlayerIndex_0x00e_2BDE_11244;

	/*
	uint8_t *help_ScreenBuffer = nullptr;
	if (CommandLineParams.DoTestRenderers()) {
		help_ScreenBuffer = (uint8_t *)malloc(screenWidth_18062C * screenHeight_180624);
		renderer_tests_frame_count++;
	}*/
	switch (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].MenuState_0x3DF_2BE4_12221) {
		case 0:
		case 3:
		case 5:
		case 9:
		case 0xA:
		case 0xD:
			playerEntity = Entities_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
			if (x_BYTE_D41C4 || D41A0_0.m_GameSettings.m_Graphics.m_wViewPortSize != x_BYTE_D41C6_old_graphics_mode) {
				x_BYTE_D41C4 = 0;
				x_BYTE_D41C6_old_graphics_mode = D41A0_0.m_GameSettings.m_Graphics.m_wViewPortSize;
				if (D41A0_0.m_GameSettings.m_Graphics.m_wViewPortSize < 40) {
					if (x_WORD_180660_VGA_type_resolution & 1)
						ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 320, 200, uiBackGroundColorIdx_EB3A8 /* uiBackGroundColorIdx_EB3A8*/);
					else
						ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, screenWidth_18062C, screenHeight_180624, uiBackGroundColorIdx_EB3A8 /* uiBackGroundColorIdx_EB3A8*/);
				}
			}
			if (D41A0_0.m_GameSettings.m_Graphics.m_wViewPortSize < 40) {
				if (x_WORD_180660_VGA_type_resolution & 1)
					ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 320, 200, uiBackGroundColorIdx_EB3A8);
				else
					ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, screenWidth_18062C, screenHeight_180624, uiBackGroundColorIdx_EB3A8);
			}

			viewPort.SetRenderViewPortSize_40C50(D41A0_0.m_GameSettings.m_Graphics.m_wViewPortSize);
			/*
			m_ptrGameRender->DrawWorld_411A0( //draw terrain and particles
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].axis_2BDE_11695.x, //position of player
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].axis_2BDE_11695.y, //position of player
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].rotation__2BDE_11701.yaw, //rotation of player z
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].axis_2BDE_11695.z + 128,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].rotation__2BDE_11701.pitch,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].rotation__2BDE_11701.roll,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].rotation__2BDE_11701.fov);

			if (CommandLineParams.DoTestRenderers()) {
				memcpy(help_ScreenBuffer, pdwScreenBuffer_351628, screenWidth_18062C * screenHeight_180624);

				std::string help_buffer_name;
				std::string screenbuffer_buffer_name;
				if (typeid(*m_ptrGameRender) == typeid(GameRenderHD)) {
					delete m_ptrGameRender;
					m_ptrGameRender = nullptr;
					m_ptrGameRender = (GameRenderInterface *)new GameRenderOriginal();
					help_buffer_name = "ScreenBuffer_HD.bmp";
					screenbuffer_buffer_name = "ScreenBuffer_Original.bmp";
				} else {
					delete m_ptrGameRender;
					m_ptrGameRender = nullptr;
					m_ptrGameRender = (GameRenderInterface *)new GameRenderHD(pdwScreenBuffer_351628, *xadatapald0dat2.colorPalette_var28, (multiThreadedRender ? numberOfRenderThreads : 0), assignToSpecificCores);
					help_buffer_name = "ScreenBuffer_Original.bmp";
					screenbuffer_buffer_name = "ScreenBuffer_HD.bmp";
				}
				
				m_ptrGameRender->DrawWorld_411A0(
						D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].axis_2BDE_11695.x, //position of player
						D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].axis_2BDE_11695.y, //position of player
						D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].rotation__2BDE_11701.yaw, //rotation of player z
						D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].axis_2BDE_11695.z + 128,
						D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].rotation__2BDE_11701.pitch,
						D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].rotation__2BDE_11701.roll,
						D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].rotation__2BDE_11701.fov);

				WriteBufferToBMP(screenWidth_18062C, screenHeight_180624, *xadatapald0dat2.colorPalette_var28, pdwScreenBuffer_351628, screenbuffer_buffer_name.c_str());
				WriteBufferToBMP(screenWidth_18062C, screenHeight_180624, *xadatapald0dat2.colorPalette_var28, help_ScreenBuffer, help_buffer_name.c_str());

				int difference = 0;
				for (int test_compi = 0; test_compi < screenWidth_18062C * screenHeight_180624; test_compi++) {
					if (pdwScreenBuffer_351628[test_compi] != help_ScreenBuffer[test_compi]) {
						difference++;
					}
				}

				if (difference > 0) {
					std::ostringstream screenBufferName;
					screenBufferName << "Level-" << CommandLineParams.GetSetLevel() << "-Frame-" << renderer_tests_frame_count << "-" << screenbuffer_buffer_name;
					std::ostringstream helpScreenBufferName;
					helpScreenBufferName << "Level-" << CommandLineParams.GetSetLevel() << "-Frame-" << renderer_tests_frame_count << "-" << help_buffer_name;

					renderer_tests[CommandLineParams.GetSetLevel()].differences += difference;
					Logger->error("Differences between HD and Original renderer in frame {0}: {1}", renderer_tests_frame_count, difference);
					WriteBufferToBMP(screenWidth_18062C, screenHeight_180624, *xadatapald0dat2.colorPalette_var28, pdwScreenBuffer_351628, screenBufferName.str().c_str());
					WriteBufferToBMP(screenWidth_18062C, screenHeight_180624, *xadatapald0dat2.colorPalette_var28, help_ScreenBuffer, helpScreenBufferName.str().c_str());
				}

				if (typeid(*m_ptrGameRender) == typeid(GameRenderHD)) {
					delete m_ptrGameRender;
					m_ptrGameRender = nullptr;
					m_ptrGameRender = (GameRenderInterface *)new GameRenderOriginal();
				} else {
					delete m_ptrGameRender;
					m_ptrGameRender = nullptr;
					m_ptrGameRender = (GameRenderInterface *)new GameRenderHD(pdwScreenBuffer_351628, *xadatapald0dat2.colorPalette_var28, (multiThreadedRender ? numberOfRenderThreads : 0), assignToSpecificCores);
				}
			}*/

			if (playerEntity->life_0x8 < 0) {
				int menuIndex = D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].MenuState_0x3DF_2BE4_12221;
				if (menuIndex == (int)MenuState::SHOW_IN_GAME_OPTIONS) {
					DrawInGameOptionsMenu_30050(scale);
					break;
				} else if (menuIndex == 10) {
					DrawVolumeSettings_303D0();
				} else if (menuIndex == (int)MenuState::SHOW_OK_CANCEL_OPTIONS) {
					DrawOkCancelMenu_30A60(6, 6, scale);
					break;
				}
				DrawPauseMenu_2FD90(scale);
			} else {
				if (x_DWORD_D4188 && playerEntity->dword_0xA4_164x->mobilizeCounter_0x14E_334) {
					/*
					//Draw Spiders Web
					int16_t offSetX = 0;
					int16_t offSetY = 0;
					int maxCountX;
					int maxCountY;
					if (x_WORD_180660_VGA_type_resolution & 1) {
						maxCountY = 2;
						maxCountX = 4;
					} else {
						maxCountY = 4;
						maxCountX = 6;
					}
					if (x_WORD_180660_VGA_type_resolution != 1)
						if (!DefaultResolutions()) {
							offSetX = (screenWidth_18062C - 640) / 2;
							offSetY = (screenHeight_180624 - 480) / 2;
						}
					actPlayerIndex = 1;
					int countY = 0;
					int y = 0;
					int yAdd = x_DWORD_D418C[1].height_5;
					while (countY < maxCountY) {
						int countX = 0;
						int x = 0;
						while (countX < maxCountX) {
							DrawBitmap_2BB40(offSetX + x, offSetY + y, x_DWORD_D4188t_spritestr[actPlayerIndex]);
							countX++;
							actPlayerIndex++;
							x += x_DWORD_D4188t_spritestr[actPlayerIndex].width_4;
							yAdd = x_DWORD_D4188t_spritestr[1].height_5;
						}
						countY++;
						y += yAdd;
					}
					*/
				}
				if (D41A0_0.m_GameSettings.m_Display.m_wMiniMap) {
					DrawMinimap_63600( //draw minimap
							0,
							0,
							D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].ActPlayerIndex_0x00e_2BDE_11244 + 1].axis_2BDE_11695.x,
							D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].ActPlayerIndex_0x00e_2BDE_11244 + 1].axis_2BDE_11695.y,
							128 * scale,
							128 * scale,
							D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].ActPlayerIndex_0x00e_2BDE_11244 + 1].rotation__2BDE_11701.yaw,
							256 / scale,
							0);
					DrawMinimapEntites_61880( //draw entites in minimap
							0,
							0,
							D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].ActPlayerIndex_0x00e_2BDE_11244 + 1].axis_2BDE_11695.x,
							D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].ActPlayerIndex_0x00e_2BDE_11244 + 1].axis_2BDE_11695.y,
							128 * scale,
							128 * scale,
							D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].ActPlayerIndex_0x00e_2BDE_11244 + 1].rotation__2BDE_11701.yaw,
							256 / scale,
							scale);
					DrawMinimapMarks_644F0(
							0,
							0,
							D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].ActPlayerIndex_0x00e_2BDE_11244 + 1].axis_2BDE_11695.x,
							D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].ActPlayerIndex_0x00e_2BDE_11244 + 1].axis_2BDE_11695.y,
							128 * scale,
							128 * scale,
							D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].ActPlayerIndex_0x00e_2BDE_11244 + 1].rotation__2BDE_11701.yaw,
							256 / scale,
							scale);
				}

				GetFont_6FC50(FontType_D419D);
				if (D41A0_0.m_GameSettings.m_Display.m_wTopBar) {
					//Left
					DrawSpellIcon_2E260(
							spellLeftPosX,
							2 * scale,
							Entities_EA3E4[playerEntity->dword_0xA4_164x->str_611.array_0x333_819x.SpellEnabled[playerEntity->dword_0xA4_164x->str_611.SpellIndexLeft_0x451_1105]],
							false,
							scale);

					//Right
					DrawSpellIcon_2E260(
							spellRightPosX,
							2 * scale,
							Entities_EA3E4[playerEntity->dword_0xA4_164x->str_611.array_0x333_819x.SpellEnabled[playerEntity->dword_0xA4_164x->str_611.SpellIndexRight_0x453_1107]],
							false,
							scale);

					DrawTopStatusBar_2D710(playerEntity, scale);
				}
				switch (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].MenuState_0x3DF_2BE4_12221) {
					case (int)MenuState::SHOW_CHAT_MENU:
						DrawChatMenu_2F6B0();
						DrawPauseMenu_2FD90(scale);
						break;
					case (int)MenuState::SHOW_IN_GAME_OPTIONS:
						DrawInGameOptionsMenu_30050(scale);
						break;
					case (int)MenuState::SHOW_VOLUME_OPTIONS:
						DrawVolumeSettings_303D0(scale);
						DrawPauseMenu_2FD90(scale);
						break;
					case (int)MenuState::SHOW_OK_CANCEL_OPTIONS:
						DrawOkCancelMenu_30A60(132, 50, scale);
						break;
					default:
						DrawPauseMenu_2FD90(scale);
						break;
				}
				DrawTextPauseEndOfLevel_2CE30(132 * scale, 50 * scale, scale);
				if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].MenuState_0x3DF_2BE4_12221 == (int)MenuState::SHOW_BOTTOM_MENU)
					DrawBottomSpellsMenu_2ECC0();
			}
			break;
		case 6:
		case 7:
		case 8:
		case 0xB:
		case 0xC:
		case 0xE:
			playerEntity = Entities_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
			if (x_WORD_180660_VGA_type_resolution & 1)
				ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 320, 200, MyUiBackGroundColorIdx /* uiBackGroundColorIdx_EB3A8*/);
			else
				ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, screenWidth_18062C, screenHeight_180624, MyUiBackGroundColorIdx /* uiBackGroundColorIdx_EB3A8 */);

			int locViewportPosx;
			int locViewportWidth;
			int locViewportHeight;
			int locMinimapHeight;

			if (x_WORD_180660_VGA_type_resolution == 1) {
				locViewportPosx = 384; //320x200
				locViewportWidth = 256;
				locViewportHeight = 400;
				locMinimapHeight = 400;
			} else {
				locViewportPosx = 0.6 * screenWidth_18062C; //bigger than 320x200
				if (locViewportPosx > 384)
					locViewportPosx = 384;
				locViewportWidth = screenWidth_18062C - locViewportPosx;
				locViewportHeight = screenHeight_180624;
				locMinimapHeight = screenHeight_180624;
				if (locMinimapHeight > 400)
					locMinimapHeight = 400;

				if (scale > 1) {
					locViewportPosx *= scale;
					locMinimapHeight *= scale;
					locViewportWidth = screenWidth_18062C - locViewportPosx;

					if (locMinimapHeight > screenHeight_180624) {
						locMinimapHeight = screenHeight_180624;
					}
				}
			}

			DrawMinimap_63600(
					0,
					0,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].axis_2BDE_11695.x,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].axis_2BDE_11695.y,
					locViewportPosx - 2,
					locMinimapHeight,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].rotation__2BDE_11701.yaw,
					204 / scale,
					1);

			DrawMinimapEntites_61880(
					0,
					0,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].axis_2BDE_11695.x,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].axis_2BDE_11695.y,
					locViewportPosx - 2,
					locMinimapHeight,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].rotation__2BDE_11701.yaw,
					204 / scale,
					scale);

			viewPort.SetRenderViewPortSize_40BF0(locViewportPosx, 0, locViewportWidth, locViewportHeight);

			m_ptrGameRender->DrawWorld_411A0(
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].axis_2BDE_11695.x,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].axis_2BDE_11695.y,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].rotation__2BDE_11701.yaw,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].axis_2BDE_11695.z + 128,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].rotation__2BDE_11701.pitch,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].rotation__2BDE_11701.roll,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].rotation__2BDE_11701.fov);
			/*
			if (CommandLineParams.DoTestRenderers()) {
				memcpy(help_ScreenBuffer, pdwScreenBuffer_351628, screenWidth_18062C * screenHeight_180624);

				std::string help_buffer_name;
				std::string screenbuffer_buffer_name;
				if (typeid(*m_ptrGameRender) == typeid(GameRenderHD)) {
					delete m_ptrGameRender;
					m_ptrGameRender = nullptr;
					m_ptrGameRender = (GameRenderInterface *)new GameRenderOriginal();
					help_buffer_name = "ScreenBuffer_HD.bmp";
					screenbuffer_buffer_name = "ScreenBuffer_Original.bmp";
				} else {
					delete m_ptrGameRender;
					m_ptrGameRender = nullptr;
					m_ptrGameRender = (GameRenderInterface *)new GameRenderHD(pdwScreenBuffer_351628, *xadatapald0dat2.colorPalette_var28, (multiThreadedRender ? numberOfRenderThreads : 0), assignToSpecificCores);
					help_buffer_name = "ScreenBuffer_Original.bmp";
					screenbuffer_buffer_name = "ScreenBuffer_HD.bmp";
				}

				m_ptrGameRender->DrawWorld_411A0(
						D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].axis_2BDE_11695.x, //position of player
						D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].axis_2BDE_11695.y, //position of player
						D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].rotation__2BDE_11701.yaw, //rotation of player z
						D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].axis_2BDE_11695.z + 128,
						D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].rotation__2BDE_11701.pitch,
						D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].rotation__2BDE_11701.roll,
						D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].rotation__2BDE_11701.fov);

				WriteBufferToBMP(screenWidth_18062C, screenHeight_180624, *xadatapald0dat2.colorPalette_var28, pdwScreenBuffer_351628, screenbuffer_buffer_name.c_str());
				WriteBufferToBMP(screenWidth_18062C, screenHeight_180624, *xadatapald0dat2.colorPalette_var28, help_ScreenBuffer, help_buffer_name.c_str());

				int difference = 0;
				for (int test_compi = 0; test_compi < screenWidth_18062C * screenHeight_180624; test_compi++) {
					if (pdwScreenBuffer_351628[test_compi] != help_ScreenBuffer[test_compi]) {
						difference++;
					}
				}

				if (difference > 0) {
					renderer_tests[CommandLineParams.GetSetLevel()].differences += difference;
					Logger->error("Differences between HD and Original renderer in frame {0}: {1}", renderer_tests_frame_count, difference);
				}

				if (typeid(*m_ptrGameRender) == typeid(GameRenderHD)) {
					delete m_ptrGameRender;
					m_ptrGameRender = nullptr;
					m_ptrGameRender = (GameRenderInterface *)new GameRenderOriginal();
				} else {
					delete m_ptrGameRender;
					m_ptrGameRender = nullptr;
					m_ptrGameRender = (GameRenderInterface *)new GameRenderHD(pdwScreenBuffer_351628, *xadatapald0dat2.colorPalette_var28, (multiThreadedRender ? numberOfRenderThreads : 0), assignToSpecificCores);
				}
			}
			*/
			if (x_WORD_180660_VGA_type_resolution & 1)
				sub_9025C(locViewportPosx, 0, locViewportPosx, locViewportHeight, (unsigned short)(*xadataclrd0dat.colorPalette_var28)[0], 0);
			else
				sub_90374(locViewportPosx, 0, locViewportPosx, locViewportHeight, (unsigned short)(*xadataclrd0dat.colorPalette_var28)[0], 0);
			if (x_WORD_180660_VGA_type_resolution & 1)
				sub_9025C(locViewportPosx - 2, 0, locViewportPosx - 2, locMinimapHeight, (unsigned short)(*xadataclrd0dat.colorPalette_var28)[0], 0);
			else
				sub_90374(locViewportPosx - 2, 0, locViewportPosx - 2, locMinimapHeight, (unsigned short)(*xadataclrd0dat.colorPalette_var28)[0], 0);
			viewPort.SetRenderViewPortSize_40C50(D41A0_0.m_GameSettings.m_Graphics.m_wViewPortSize);
			DrawMinimapMarks_644F0(
					0,
					0,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].axis_2BDE_11695.x,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].axis_2BDE_11695.y,
					locViewportPosx - 2,
					locMinimapHeight,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].rotation__2BDE_11701.yaw,
					204 / scale,
					scale);
			switch (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].MenuState_0x3DF_2BE4_12221) {
				case (int)MenuState::SHOW_MAP_SORCERER_SCORES:
					DrawSorcererScores_2D1D0(scale);
					break;
				case (int)MenuState::SHOW_MAP_BOTTOM_MENU:
					DrawPauseMenu_2FD90(scale);
					DrawBottomSpellsMenu_2ECC0();
					break;
				case (int)MenuState::SHOW_MAP_GAME_OPTIONS:
					DrawInGameOptionsMenu_30050(scale);
					break;
				case (int)MenuState::SHOW_MAP_VOLUME_OPTIONS:
					DrawVolumeSettings_303D0();
					DrawPauseMenu_2FD90(scale);
					break;
				case (int)MenuState::SHOW_MAP_OK_CANCEL_OPTIONS:
					DrawOkCancelMenu_30A60(6, 6, scale);
					break;
				default:
					DrawPauseMenu_2FD90(scale);
					break;
			}
			DrawTextPauseEndOfLevel_2CE30(6, 6);
			if (x_D41A0_BYTEARRAY_4_struct.leftSpellPlayerIndex_38400)
				DrawSpellIcon_2E260(
						spellLeftPosX,
						2,
						Entities_EA3E4[playerEntity->dword_0xA4_164x->str_611.array_0x333_819x.SpellEnabled[playerEntity->dword_0xA4_164x->str_611.SpellIndexLeft_0x451_1105]],
						false,
						scale);
			if (x_D41A0_BYTEARRAY_4_struct.rightSpellPlayerIndex_38401)
				DrawSpellIcon_2E260(
						spellRightPosX,
						2,
						Entities_EA3E4[playerEntity->dword_0xA4_164x->str_611.array_0x333_819x.SpellEnabled[playerEntity->dword_0xA4_164x->str_611.SpellIndexRight_0x453_1107]],
						false,
						scale);
			break;
		default:
			break;
	}
	/*
	if (CommandLineParams.DoTestRenderers()) {
		if (help_ScreenBuffer) {
			free(help_ScreenBuffer);
			help_ScreenBuffer = nullptr;
		}
	}*/
	if (D41A0_0.byte_counter_current_objective_box_0x36E04)
		DrawCurrentObjectiveTextbox_30630(scale);
	GetFont_6FC50(FontType_D419D);
	if (!(x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 & 4))
		return;

	if (x_D41A0_BYTEARRAY_4_struct.byteindex_204 == 3) {
		if (x_D41A0_BYTEARRAY_4_struct.byteindex_210d <= 0)
			x_D41A0_BYTEARRAY_4_struct.byteindex_204 = 2;
		else
			x_D41A0_BYTEARRAY_4_struct.byteindex_210d--; // = v26 - 1;
	}
	if (x_D41A0_BYTEARRAY_4_struct.byteindex_204 == 2) {
		x_D41A0_BYTEARRAY_4_struct.byteindex_204 = 1;
		x_D41A0_BYTEARRAY_4_struct.byteindex_214w = 0;
		x_D41A0_BYTEARRAY_4_struct.byteindex_210d = 50;
	}
	if (x_D41A0_BYTEARRAY_4_struct.byteindex_204 != 1)
		return;

	if (x_D41A0_BYTEARRAY_4_struct.byteindex_210d <= 0) {
		while (1) {
			if (*off_DB558[x_D41A0_BYTEARRAY_4_struct.byteindex_214w] == 33)
				break;
			x_D41A0_BYTEARRAY_4_struct.byteindex_214w++;
		}
		x_D41A0_BYTEARRAY_4_struct.byteindex_214w++;
		if (*off_DB558[x_D41A0_BYTEARRAY_4_struct.byteindex_214w] == 35) {
			x_D41A0_BYTEARRAY_4_struct.byteindex_204 = 3;
			x_D41A0_BYTEARRAY_4_struct.byteindex_210d = 200;
			return;
		}
		x_D41A0_BYTEARRAY_4_struct.byteindex_210d = 50;
		return;
	}
	int height = 380;
	int heightIndex = 0;
	while (*off_DB558[heightIndex + x_D41A0_BYTEARRAY_4_struct.byteindex_214w] != 33) {
		heightIndex++;
		height -= GetLetterHeight_6FC30();
	}
	int index = 0;
	int actHeight = 8;
	while (heightIndex > 0) {
		if (index)
			DrawText_2BC10(off_DB558[index + x_D41A0_BYTEARRAY_4_struct.byteindex_214w], 8, actHeight, (*xadataclrd0dat.colorPalette_var28)[0]);
		else
			DrawText_2BC10(off_DB558[x_D41A0_BYTEARRAY_4_struct.byteindex_214w], 8, actHeight, (*xadataclrd0dat.colorPalette_var28)[4095]);
		heightIndex--;
		index++;
		actHeight += GetLetterHeight_6FC30();
	}
	x_D41A0_BYTEARRAY_4_struct.byteindex_210d--;
}

Ref<Image> MBEXclass::getMinimap() {
	int locViewportPosx;
	int locViewportWidth;
	int locViewportHeight;
	int locMinimapHeight;
	//uint8_t scale = 1;

	int bufferWidth = 640;
	int bufferHeight = 480;
	/*
	if (x_WORD_180660_VGA_type_resolution == 1) {
		locViewportPosx = 384; //320x200
		locViewportWidth = 256;
		locViewportHeight = 400;
		locMinimapHeight = 400;
	} else {
		locViewportPosx = 0.6 * screenWidth_18062C; //bigger than 320x200
		if (locViewportPosx > 384)
			locViewportPosx = 384;
		locViewportWidth = screenWidth_18062C - locViewportPosx;
		locViewportHeight = screenHeight_180624;
		locMinimapHeight = screenHeight_180624;
		if (locMinimapHeight > 400)
			locMinimapHeight = 400;

		if (scale > 1) {
			locViewportPosx *= scale;
			locMinimapHeight *= scale;
			locViewportWidth = screenWidth_18062C - locViewportPosx;

			if (locMinimapHeight > screenHeight_180624) {
				locMinimapHeight = screenHeight_180624;
			}
		}
	}*/

	/*
	memset(pdwScreenBuffer_351628, 0, 640 * 480);
	//x_DWORD_180644_map_resolution2_y?
	
				DrawMinimap_63600(//draw minimap
					0,
					0,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].ActPlayerIndex_0x00e_2BDE_11244 + 1].axis_2BDE_11695.x,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].ActPlayerIndex_0x00e_2BDE_11244 + 1].axis_2BDE_11695.y,
					128 * scale,
					128 * scale,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].ActPlayerIndex_0x00e_2BDE_11244 + 1].rotation__2BDE_11701.yaw,
					256 / scale,
					0);
				DrawMinimapEntites_61880(//draw entites in minimap
					0,
					0,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].ActPlayerIndex_0x00e_2BDE_11244 + 1].axis_2BDE_11695.x,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].ActPlayerIndex_0x00e_2BDE_11244 + 1].axis_2BDE_11695.y,
					128 * scale,
					128 * scale,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].ActPlayerIndex_0x00e_2BDE_11244 + 1].rotation__2BDE_11701.yaw,
					256 / scale,
					scale);
				DrawMinimapMarks_644F0(
					0,
					0,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].ActPlayerIndex_0x00e_2BDE_11244 + 1].axis_2BDE_11695.x,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].ActPlayerIndex_0x00e_2BDE_11244 + 1].axis_2BDE_11695.y,
					128 * scale,
					128 * scale,
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].ActPlayerIndex_0x00e_2BDE_11244 + 1].rotation__2BDE_11701.yaw,
					256 / scale,
					scale);
	*/
	DrawGameFrame_2BE30_mod();

	uint8_t *palette = VGA_Get_Palette(true);
	int crop_x = 0;
	int crop_y = 0;
	int crop_w = bufferWidth;
	int crop_h = bufferHeight;
	PackedByteArray rgba_data;
	rgba_data.resize(crop_w * crop_h * 4);
	uint8_t *dest = rgba_data.ptrw();
	int circle_size = 256;
	float center = circle_size / 2.0;
	float radius = circle_size / 2.0;
	for (int r = 0; r < crop_h; ++r) {
		int row_offset = (crop_y + r) * screenWidth_18062C;
		for (int c = 0; c < crop_w; ++c) {
			//float dx = c - center;
			//float dy = r - center;
			//float distanceSquared = dx * dx + dy * dy;
			uint32_t color_idx = pdwScreenBuffer_351628[row_offset + (crop_x + c)];
			int pal_pos = color_idx * 3;
			//if (distanceSquared > radius * radius) {
			if (color_idx == MyUiBackGroundColorIdx) {
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
	cmax = max(cmax, col.y);
	cmax = max(cmax, col.z);
	float cmin = col.x;
	cmin = min(cmin, col.y);
	cmin = min(cmin, col.z);
	float chroma = cmax - cmin;
	return (cmax > 0.001f) ? chroma / cmax : 0.0f;
}

Array MBEXclass::getPaletteModifications() {
	Vector3 out_gain;
	Vector3 out_offset;
	float out_sat_multiplier;
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

	Array result;
	result.push_back(out_gain);
	result.push_back(out_offset);
	result.push_back(out_sat_multiplier);
	return result;
}

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
				mainSetPress(is_pressed, 0x4800); //UP
				break;
			case 0x1f73:
				mainSetPress(is_pressed, 0x5000); //DOWN
				break;
			case 0x1e61:
				mainSetPress(is_pressed, 0x4b00); //LEFT
				break;
			case 0x2064:
				mainSetPress(is_pressed, 0x4d00); //RIGHT
				break;
			case 0x3920:
				mainSetPress(is_pressed, 0x3920); //SPACE
				break;
			case 0x011B:
				mainSetPress(is_pressed, 0x011B); //ESC
				break;
			case 0x5300:
				HandleButtonClick_191B0(29, 0);
				HandleButtonClick_191B0(27, 0);
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
	if (type == 0) {
		mouse_pos = inputs["mouse_pos"];
		MouseEvents(buttonresult, mouse_pos.x, 480 - mouse_pos.y);
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

	sprintf(gameFolder, "%sGAME/NETHERW", real_cdPath.utf8().get_data());
	sprintf(cdFolder, "%sCD_Files", real_cdPath.utf8().get_data());
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
	sub_47160();
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

void MBEXclass::REMC2BeginGame_old(String cdPath) {//OK!!
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

	sub_main_mod_begin(argc, argv,(char *) real_cdPath.utf8().get_data());
	//MBEXstate = 1;

	//changeLanguage(2);//added code
}

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
}


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

void MBEXclass::REMC2BeginAnim(TextureRect *scrBufferRect,int animIndex) {
	PlayInfoFmv_break = false;
	mainScrBufferRect = scrBufferRect;
	globalAnimIndex = animIndex;
	sub_46830_main_loop_mod(0, typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Begin });
}

Ref<Image> getScrBufferImg(int crop_w = 640, int crop_h = 480) {
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
			/* if (color_idx == 0) {
				int dest_pos = (r * crop_w + c) * 4;
				dest[dest_pos + 0] = 0;
				dest[dest_pos + 1] = 0;
				dest[dest_pos + 2] = 0;
				dest[dest_pos + 3] = 0;
			}  else {*/
				uint8_t red = palette[pal_pos + 0] * 4;
				uint8_t green = palette[pal_pos + 1] * 4;
				uint8_t blue = palette[pal_pos + 2] * 4;
				int dest_pos = (r * crop_w + c) * 4;
				dest[dest_pos + 0] = red;
				dest[dest_pos + 1] = green;
				dest[dest_pos + 2] = blue;
				dest[dest_pos + 3] = 255;
			//}
		}
	}
	Ref<Image> img = Image::create_from_data(crop_w, crop_h, false, Image::FORMAT_RGBA8, rgba_data);
	return img;
}

int MBEXclass::REMC2StepAnim(Dictionary inputs) {
	handleInputs(inputs, 2);
	sub_46830_main_loop_mod(0, typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Step });
	Ref<Image> img = getScrBufferImg(320, 200);
	if (img.is_null())
		return 0;
	if (mainTexture.is_null()) {
		mainTexture = ImageTexture::create_from_image(img);
		mainScrBufferRect->set_texture(mainTexture);
	} else {
		mainTexture->update(img);
	}

	if (PlayInfoFmv_break) {
		sub_46830_main_loop_mod(0, typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::End });
		mainTexture.unref();
	}

	return PlayInfoFmv_break;
}

void MBEXclass::REMC2BeginMap(TextureRect* scrBufferRect) {
	mainScrBufferRect = scrBufferRect;
	sub_46830_main_loop_mod(0, typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Begin });
}

int MBEXclass::REMC2StepMap(Dictionary inputs) {
	handleInputs(inputs, 1);
	sub_46830_main_loop_mod(0, typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Step });
	Ref<Image> img = getScrBufferImg();
	if (img.is_null())
		return 0;
	if (mainTexture.is_null()) {
		mainTexture = ImageTexture::create_from_image(img);
		mainScrBufferRect->set_texture(mainTexture);
	} else {
		mainTexture->update(img);
	}

	//if ((actState == typeStateMenu2::MapMenuSelected) || (actState == typeStateMenu2::ExitGameSelected))
	if (NewGameDialog_endAction)
	{
		sub_46830_main_loop_mod(0, typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::End });
		mainTexture.unref();
	}

	int result = 0;
	switch (actState) {
		case typeStateMenu2::RunGameFromMapMenuSelected:
			result = 1;
			break;
		case typeStateMenu2::ExitMapMenuSelected:
			result = 2;
			break;
		default:
			break;
	}

	actState = typeStateMenu2::AfterMenu;

	return NewGameDialog_endAction;
}

void MBEXclass::REMC2BeginMain(TextureRect *scrBufferRect) {
	mainScrBufferRect = scrBufferRect;
	sub_46830_main_loop_mod(0, typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Begin });
}

int MBEXclass::REMC2StepMain(Dictionary inputs) {
	handleInputs(inputs, 1);
	sub_46830_main_loop_mod(0, typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Step });
	Ref<Image> img = getScrBufferImg();
	if (img.is_null())
		return 0;
	if (mainTexture.is_null()) {
		mainTexture = ImageTexture::create_from_image(img);
		mainScrBufferRect->set_texture(mainTexture);
	} else {
		mainTexture->update(img);
	}

	if ((actState == typeStateMenu2::MapMenuSelected) || (actState == typeStateMenu2::ExitGameSelected)) {
		sub_46830_main_loop_mod(0, typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::End });
		mainTexture.unref();
	}

	int result = 0;
	switch (actState) {
		case typeStateMenu2::MapMenuSelected:
			result = 1;
			break;
		case typeStateMenu2::ExitGameSelected:
			result = 2;
			break;
		case typeStateMenu2::changeLangSelected:
			result = 4;
			break;
		case typeStateMenu2::animFlvSelected:
			actState = typeStateMenu2::AfterMenu;
			result = 0;//no used at now
			break;
		default:
			break;
	}

	actState = typeStateMenu2::AfterMenu;

	return result;
}


Ref<Image> MBEXclass::REMC2BeginInGame() {
	setLoadScreen = false;
	sub_46830_main_loop_mod(0, typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::Begin });
	/*
	char dataPath[MAX_PATH];
	switch (D41A0_0.terrain_2FECE.MapType) {
		case MapType_t::Day:
			sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/PALD-0.DAT");
			break;
		case MapType_t::Night:
			sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/PALN-0.DAT");
			break;
		case MapType_t::Cave:
			sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/PALC-0.DAT");
			break;
	}
	uint8_t temp_palette[256 * 3];
	uint8_t *temp_ptr[1];
	temp_ptr[0] = temp_palette;
	DataFileIO::ReadFileAndDecompress(dataPath, temp_ptr);
	VGA_Set_Palette(temp_ptr[0], true);
	*/
	if (setLoadScreen)
		return GetFrameBuffer(320,200);
	return nullptr;
}

void MBEXclass::REMC2BeginInGameAfterScreen() {
	sub_46830_main_loop_mod(0, typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::BeginAfterScreen });

	char dataPath[MAX_PATH];
	switch (D41A0_0.terrain_2FECE.MapType) {
		case MapType_t::Day:
			sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/PALD-0.DAT");
			break;
		case MapType_t::Night:
			sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/PALN-0.DAT");
			break;
		case MapType_t::Cave:
			sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/PALC-0.DAT");
			break;
	}
	uint8_t temp_palette[256 * 3];
	uint8_t *temp_ptr[1];
	temp_ptr[0] = temp_palette;
	DataFileIO::ReadFileAndDecompress(dataPath, temp_ptr);
	VGA_Set_Palette(temp_ptr[0], true);
}

int MBEXclass::REMC2StepInGame(Dictionary inputs, int state) {
	if (state == 1) {
		sub_46830_main_loop_mod(0, typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::EndPostSecretScreen });
		sub_46830_main_loop_mod(0, typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::BeginAfterSecret });
		return 3;
	}
	if (state == 0) {
		handleInputs(inputs, 0);
		sub_46830_main_loop_mod(0, typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::Step });
		if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].byte_0x004_2BE0_11234 || D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 8) {
			sub_46830_main_loop_mod(0, typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::End });
			if (secretsModPortals) {
				switch (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2])
				{
					case 0xa://0xa-correct end of hidden level 0x2 0x8
						sub_46830_main_loop_mod(0, typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::EndPostSecretScreen });
						return 0;
					case 0x18://0x18-end of hidden level by escape 0x8 0x10
						sub_46830_main_loop_mod(0, typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::EndPostSecretScreen });
						return 0;
					case 0x1a://0x1a-go to hidden level - show load screen 0x2 0x8 0x10
						return 2;
					case 0x4://0x4-dead 0x4
						return 4;
					default:
						break;
				}
			}
			sub_46830_main_loop_mod(0, typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::EndPostSecretScreen });
			return 0;
		}
	}
	return 1;
}

bool MBEXclass::REMC2IsHiddenLevel() {
	if (x_D41A0_BYTEARRAY_4_struct.levelnumber_43w > 24 && x_D41A0_BYTEARRAY_4_struct.levelnumber_43w < 50)
		return true;
	return false;
};



