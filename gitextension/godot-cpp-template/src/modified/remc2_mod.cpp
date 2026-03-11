#include "remc2_mod.h"

//int NewGameDialog_endAction_mod;

void InitLanguage_76A40_mod_only_language() //257A40
{
	
	//FILE *configdatfile;
	FILE *langfile;
	uint32_t filelenght;
	//TypeConfigDat configDat;
	/*
	memset(printbuffer, 0, 80);
	memset(&x_DWORD_17DE38str, 0, sizeof(type_x_DWORD_17DE38str));
	x_DWORD_17DE38str.x_DWORD_17DEE0_filedesc = NULL;
	memset(&configDat, 0, sizeof(TypeConfigDat));
	sprintf(printbuffer, "%s/%s", gameDataPath.c_str(), "CONFIG.DAT");
	configdatfile = DataFileIO::CreateOrOpenFile(printbuffer, 512);
	if (configdatfile == NULL) //config is not found
	{
		LoadAndSetGraphicsAndPalette_7AC00(); //25BC00 //change screen radio
		LanguageSettingDialog_779E0(0); //2589E0 //set language
		sub_7ADE0(1);
	} else {
		DataFileIO::Read(configdatfile, (uint8_t *)&configDat.configDatSign_0, 4); //first dword
		if (configDat.configDatSign_0 == 0xfffffff7) {
			DataFileIO::Read(configdatfile, (uint8_t *)&configDat.langIndex_4, 28);

			x_D41A0_BYTEARRAY_4_struct.configDatSign_0 = configDat.configDatSign_0;
			x_D41A0_BYTEARRAY_4_struct.langIndex_4 = configDat.langIndex_4;
			x_D41A0_BYTEARRAY_4_struct.soundVolume_6 = configDat.soundVolume_6;
			x_D41A0_BYTEARRAY_4_struct.musicVolume_8 = configDat.musicVolume_8;
			x_D41A0_BYTEARRAY_4_struct.showHelp_10 = configDat.showHelp_10;
			x_D41A0_BYTEARRAY_4_struct.brightness_11 = configDat.brightness_11;
			x_D41A0_BYTEARRAY_4_struct.brightness_12 = configDat.brightness_12;
			x_D41A0_BYTEARRAY_4_struct.brightness_13 = configDat.brightness_13;
			x_D41A0_BYTEARRAY_4_struct.wordindex_14 = configDat.wordindex_14;
			x_D41A0_BYTEARRAY_4_struct.dwordindex_16 = configDat.dwordindex_16;
			x_D41A0_BYTEARRAY_4_struct.stubb[0] = configDat.stubb[0];
			x_D41A0_BYTEARRAY_4_struct.stubb[1] = configDat.stubb[1];

			for (int i = 0; i < 10; i++)
				x_BYTE_EB39E_keys[i] = configDat.keys[i];

			DataFileIO::Close(configdatfile);
			sub_8E470_sound_proc17_volume(x_D41A0_BYTEARRAY_4_struct.soundVolume_6);
			sub_8E410_sound_proc16_xmidivolume(x_D41A0_BYTEARRAY_4_struct.musicVolume_8);
			*/

			sprintf(printbuffer, "%s/%s/L%d.TXT", cdDataPath.c_str(), "LANGUAGE", x_D41A0_BYTEARRAY_4_struct.langIndex_4);
			for (int i = 0; i < 2; i++) {
				x_D41A0_BYTEARRAY_4_struct.SelectedLangIndex = x_D41A0_BYTEARRAY_4_struct.langIndex_4 & 0xff;
				if (x_BYTE_E29E0 || x_DWORD_D41BC_langbuffer)
					FreeMem_83E80((uint8_t *)x_DWORD_D41BC_langbuffer);
				langfile = DataFileIO::CreateOrOpenFile(printbuffer, 512);

				if (langfile != NULL) {
					filelenght = DataFileIO::FileLengthBytes(langfile) - 4785;
					x_DWORD_D41BC_langbuffer = (char *)Malloc_83CD0(filelenght);
					if (x_DWORD_D41BC_langbuffer) {
						DataFileIO::Seek(langfile, 4785, 0);
						DataFileIO::Read(langfile, (uint8_t *)x_DWORD_D41BC_langbuffer, filelenght);
					}
					DataFileIO::Close(langfile);
					sub_5B870_copy_sentence(x_DWORD_D41BC_langbuffer, x_DWORD_E9C4C_langindexbuffer, 471); //Exit Game
					break;
				}
				sprintf(printbuffer, "%s/%s/L%d.TXT", cdDataPath.c_str(), "LANGUAGE", x_D41A0_BYTEARRAY_4_struct.SelectedLangIndex);
				
			}/*
		} else {
			DataFileIO::Close(configdatfile);
			LoadAndSetGraphicsAndPalette_7AC00();
			LanguageSettingDialog_779E0(nullptr);
			sub_7ADE0(1);
		}
	}
	if (x_D41A0_BYTEARRAY_4_struct.SelectedLangIndex != 2 || !soundAble_E3798)
		DisplaySubtitles_D41C0 = 1;
	nextMenu_E29D8 = MenuItem::SetToIntro;*/
}

std::string REMC2mod_customLevelPath = "";
bool REMC2mod_isSecretLevel; // al
int16_t REMC2mod_setLevel = -1;
bool REMC2mod_skipMenus = false;

int REMC2_tempa2;
int REMC2_tempa3;

void sub_main_mod_begin(int argc, char **argv,char *real_cdPathch) {
	SetTimeStart();

	begin_plugin();
	preconvert(); //rewrite and remove it later
	*xadataclrd0dat.colorPalette_var28 = (uint8_t *)malloc(4096); //fix it
	signed int v3; // edi
	unsigned __int16 v4; // si
	v3 = 0;
	v4 = 0;
	printf("Reading Ini file\n");
	//skip if (!readini())
	//skip 	exit(1);

	EventDispatcher::I = new EventDispatcher();
	std::function<void(Scene)> sceneChangeCallBack = SetCurrentScene;
	EventDispatcher::I->RegisterEvent(new Event<Scene>(EventType::E_SCENE_CHANGE, sceneChangeCallBack));
	EventDispatcher::I->DispatchEvent(EventType::E_GAME_STATE_CHANGE, GameState::STARTED);

	sprintf(gameFolder, "%sGAME/NETHERW", real_cdPathch); //added
	sprintf(cdFolder, "%sCD_Files", real_cdPathch); //added
	windowResWidth = 640;//added
	windowResHeight = 480;//added
	gameResWidth = 640;//added
	gameResHeight = 480;//added

	if (CommandLineParams.DoDisableGraphicsEnhance()) {
		bigSprites = false;
		bigTextures = false;
		texturepixels = 32;
	}
	//Set Paths for game data
	gameDataPath = GetSubDirectoryPath(gameFolder);
	cdDataPath = GetSubDirectoryPath(cdFolder);
	bigGraphicsPath = GetSubDirectoryPath(bigGraphicsFolder);
	VGA_Init(windowResWidth, windowResHeight, gameResWidth, gameResHeight, maintainAspectRatio, displayIndex);
	gamepad_init(gameResWidth, gameResHeight);
	if (std::string mainfile = GetSubDirectoryFile(gameFolder, "CDATA", "TMAPS0-0.DAT"); !file_exists(mainfile.c_str())) //test original file
	{
	} else {
	}
	initposistruct();
	sub_56210_process_command_line(argc, argv); //236FD4 - 237210
	if (CommandLineParams.ModeTestNetwork()) {
		if (Iam_server || Iam_client)
			InitNetworkInfo();
	}
	if (CommandLineParams.DoCopySkipConfig()) {
		x_BYTE_D41AD_skip_screen = config_skip_screen;
	}
	Initialize();
	//sub_46830_main_loop_mod_before_cycle(v3, v4); //227830


	REMC2_tempa2=v3;
	REMC2_tempa3=v4;
}

void sub_main_mod_end() {
	sub_5BC20(); //23CC20 //remove devices?
	sub_56730_clean_memory(); //237730
	if (CommandLineParams.ModeTestNetwork()) {
		if (Iam_server || Iam_client) {
			EndMyNetLib();
		}
	}
	delete EventDispatcher::I;
}


std::vector<GraphicsAction> graphics_queue;

std::vector<GraphicsAction> graphics_queue_get_pending_actions() {
	return graphics_queue;
}

/*
void graphics_queue_add_action(const std::string &action, int x, int y, int index,int type) {
	GraphicsAction ga;
	ga.action = action;
	ga.x = x;
	ga.y = y;
	ga.index = index;
	ga.type = type;
	graphics_queue.push_back(ga);
}
void graphics_queue_clear() {
	graphics_queue.clear();
}*/

typeStateMenu2 actState = typeStateMenu2::Zero;

bool main_loop_isSecretLevel;
bool main_loop_skipMenus;
int16_t main_loop_setLevel;
std::string main_loop_customLevelPath;

bool MainMenu_onlyBlit;
uint16_t MainMenu_introIndex;

int MainMenu_lastTime;
int16_t MainMenu_tempMousePosX;
int16_t MainMenu_tempMousePosY;
int MainMenu_scanCode;

bool NewGameDialog_result;

int NewGameDialog_endAction;

bool NewGameDialog_77350_modX(type_menuButtons_E1F84 *a1x, typeStateMenu newState) //258350
{
	if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Begin }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Step }))
	{
		NewGameDialog_result = false;
		NewGameDialog_endAction = 0;
		map_not_moving_WORD_E29D6 = false;
		if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 2 && x_D41A0_BYTEARRAY_4_struct.levelnumber_43w == 24) {
			// show credits after finishing the last level
			ShowEndCredits_833C0();
		}
		memset((void *)&unk_17DBA8str, 0, sizeof(unk_17DBA8str));
		unk_17DBA8str.x_BYTE_17DBB6 = 2;
	}
	if (LoadLevelNumber_D419C <= -1) {
		if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Begin }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Step }))
		{
			IsPlayingCDTrack_17E09D = 0;
			x_DWORD_17DE38str.unk_17E078x.lastSpriteIndex_11 = 16;
			x_DWORD_17DE38str.y_17E06E = 480;
			x_DWORD_17DE38str.unk_17E078x.spriteIndex_8 = 13;
			x_DWORD_17DE38str.x_BYTE_17E09C = 0;
			x_DWORD_17DE38str.unk_17E078x.firstSpriteIndex_10 = 13;
			x_DWORD_17DE38str.x_17E06C = 0;
			memset((void *)&x_DWORD_17DB70str, 0, sizeof(type_x_DWORD_17DB70str));
			if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 2)
				x_DWORD_17DB70str.x_BYTE_17DB8F = 4;
			else
				x_DWORD_17DB70str.x_BYTE_17DB8F = 1;
			sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 6); //here init sprites
			SetCursor_8CD27((*filearray_2aa18c[filearrayindex_POINTERSDATTAB].posistruct)[0]); //Set cursor to Null (Don't Draw)

			ResetMouse_7B5A0();
			sub_90B27_VGA_pal_fadein_fadeout(0, 0x10u, 0);
			if (x_WORD_180660_VGA_type_resolution & 1) {
				ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 320, 200, getPaletteIndex_5BE80(x_DWORD_17DE38str.palette_17DE38x, 0, 0, 0));
			} else {
				ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 640, 480, getPaletteIndex_5BE80(x_DWORD_17DE38str.palette_17DE38x, 0, 0, 0));
			}
			if (x_WORD_180660_VGA_type_resolution & 1)
				sub_90478_VGA_Blit320();
			else
				sub_75200_VGA_Blit640(480);
			sub_41A90_VGA_Palette_install(x_DWORD_17DE38str.palette_17DE38x);
			PortalsUpdate_7DD70();
			x_DWORD_17DE38str.x_WORD_17DEEC = 0;
			SetCenterScreenForFlyAssistant_6EDB0();
			SetCursor_8CD27(xy_DWORD_17DED4_spritestr[239]);
			x_DWORD_17DB70str.x_WORD_17DB8A = x_D41A0_BYTEARRAY_4_struct.levelnumber_43w;
		}
		if (newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Step })
		if (!NewGameDialog_endAction) {
			SetFrameStart(std::chrono::system_clock::now());
			if (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode == 59) {
				x_DWORD_17DE38str.showHelp_17DF13 = x_D41A0_BYTEARRAY_4_struct.showHelp_10 != 1;
				x_D41A0_BYTEARRAY_4_struct.showHelp_10 = x_D41A0_BYTEARRAY_4_struct.showHelp_10 != 1;
				x_D41A0_BYTEARRAY_4_struct.setting_38402 = 1;
			}
			NewGameDialog_endAction = NewGameDraw_7EAE0(
					&x_DWORD_17DB70str.unk_17DB76_posx,
					&x_DWORD_17DB70str.unk_17DB78_posy,
					&x_DWORD_17DB70str.unk_17DB7E,
					&x_DWORD_17DB70str.unk_17DB80,
					&x_DWORD_17DB70str.x_BYTE_17DB8F,
					&x_DWORD_17DB70str.unk_17DB90);

			//added code
			if (NewGameDialog_endAction)
			switch (NewGameDialog_endAction) {
				case 1:
					actState = typeStateMenu2::RunGameFromMapMenuSelected;
					break;
				case 2:
					actState = typeStateMenu2::ExitMapMenuSelected;
					break;
			}
			//added code

			if (CommandLineParams.ModeRegressionsTestType() == 0) {
				x_DWORD_17DB70str.x_BYTE_17DB8E = 1;
				x_D41A0_BYTEARRAY_4_struct.levelnumber_43w = test_regression_level;
				if (mapScreenPortals_E17CC[test_regression_level].activated_18 == 1)
					x_D41A0_BYTEARRAY_4_struct.setting_38545 |= 4u;
				Type_SecretMapScreenPortals_E2970 *v46x = GetSecretAndActivedPortal_824B0(x_D41A0_BYTEARRAY_4_struct.levelnumber_43w);
				if (v46x && v46x->activated_12 == 2)
					x_D41A0_BYTEARRAY_4_struct.setting_38545 |= 0x10u;
				if (x_D41A0_BYTEARRAY_4_struct.levelnumber_43w == 24)
					x_D41A0_BYTEARRAY_4_struct.setting_38545 |= 0x20u;
				NewGameDialog_endAction = 1;
			}
			if (x_WORD_180660_VGA_type_resolution & 1)
				sub_90478_VGA_Blit320(menuFps);
			else
				sub_75200_VGA_Blit640(480, menuFps);
			sub_7A060_get_mouse_and_keyboard_events();
		}
		if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::End }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Step }))
		{
			StopCdPlayback_86860(x_WORD_1803EC);
			D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] = 0;
			EndSample_8D8F0();
			map_not_moving_WORD_E29D6 = false;
			if (NewGameDialog_endAction == 1) {
				m_ExitMenuLoop_E29DC = 1;
				if (a1x)
					a1x->dword_4 = 0;
			} else if (NewGameDialog_endAction == 2 && a1x) {
				a1x->dword_4 = 1;
			}
			SetCursor_8CD27((*filearray_2aa18c[filearrayindex_POINTERSDATTAB].posistruct)[0]);
			sub_90B27_VGA_pal_fadein_fadeout(0, 0x10u, 0);
			NewGameDialog_result = true;
		}
	} else {
		if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Begin }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Step }))
		{
			m_ExitMenuLoop_E29DC = 1;
			x_D41A0_BYTEARRAY_4_struct.levelnumber_43w = LoadLevelNumber_D419C;
			if (a1x)
				a1x->dword_4 = 0;
			NewGameDialog_result = true;
		}
	}
	return NewGameDialog_result;
}

bool DrawAndServe_pre_sub_7B250_modX(uint32_t var, type_menuButtons_E1F84 *var2x) {
	bool callres = true;
	switch (var) {
		case 0x258350: {
			//return NewGameDialog_77350(var2x); //258350 - new game
			actState = typeStateMenu2::MapMenuSelected;
			return true;
			break;
		}
		case 0x258980: {
			bool result=ExitDialog_77980(var2x);
			if (result) {
				if (m_ExitMenuLoop_E29DC) {
					actState = typeStateMenu2::ExitGameSelected;
				}
				return true;
			}
			else
				return false;
			break;
		}
		case 0x2589e0: {
			actState = typeStateMenu2::changeLangSelected;
			return true;
			//return LanguageSettingDialog_779E0(var2x);
			break;
		}
		case 0x2590f0: {
			return LoadGameDialog_780F0(var2x); //2590f0 - load
			break;
		}
		case 0x259730: {
			return SaveGameDialog_78730(var2x); //259730 -save
			break;
		}
		case 0x259e00: {
			return SetPlayerNameDialog_78E00(var2x); //set user name
			break;
		}
		case 0x25a160: { //set joystick
			return SetJoystickDialog_79160(var2x);
			break;
		}
		case 0x25a610: { //set keys
			//return SetKeysDialog_79610();
			actState = typeStateMenu2::setKeysSelected;
			return true;
			break;
		}
		case 0x25dcf0: {
			return sub_7CCF0();
			break;
		}
		case 0x25dd30: {
			return sub_7CD30();
			break;
		}
		case 0x25dca0: {
			return sub_7CCA0();
			break;
		}
		case 0x25dda0: {
			return sub_7CDA0();
			break;
		}
		case 0x25dbf0: {
			return sub_7CBF0();
			break;
		}
		case 0x25dc40: {
			return sub_7CC40();
			break;
		}
		case 0x25ee80: {
			//return MultiplayerMenu_7DE80(var2x);
			actState = typeStateMenu2::multiplayerSelected;
			return true;
			break;
		}
	}
	return callres;
}

typeTextBoxtextBoxStr_E24BCx DrawAndServe_textBoxStr[2];

bool DrawAndServe_7B250_modX(typeStateMenu newState) //25c250
{
	for (int i = 0; str_E1BAC[i].xmin_10; i++) {
		if (str_E1BAC[i].selected_8 && str_E1BAC[i].dword_0) {
			if (DrawAndServe_pre_sub_7B250_modX(str_E1BAC[i].dword_0, &str_E1BAC[i])) {
				str_E1BAC[i].selected_8 = 0;
				ResetMouse_7B5A0();
			}
			if (str_E1BAC[i].dword_4) {
				str_E1BAC[i].selected_8 = 0;
				SetCenterScreenForFlyAssistant_6EDB0();
				sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 4);
				ResetMouse_7B5A0();
				SetCursor_8CD27(xy_DWORD_17DED4_spritestr[39]);
				if (str_E1BAC[i].dword_4 == 2)
					str_E1BAC[i].dword_4 = 0;
				return 1;
			}
			return 0;
		}
	}
	//clear/set off_E1BAC
	int jx;
	for (jx = 0; str_E1BAC[jx].xmin_10; jx++) //clear/set off_E1BAC
	{
		str_E1BAC[jx].selected_8 = 0;
		str_E1BAC[jx].gold_color_24 = 0;
		if (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode == 1 && str_E1BAC[jx].byte_22 == 11)
			str_E1BAC[jx].selected_8 = 1;
	}
	if (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode == 1) {
		x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode = 0;
	} else {
		for (jx = 0; str_E1BAC[jx].xmin_10; jx++) {
			if (InRegion_7B200(&str_E1BAC[jx], x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx, x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony) && str_E1BAC[jx].canSelect_23) {
				if (x_DWORD_17DE38str.x_WORD_17DEEE_mouse_buttons & 1) {
					PlaySample_8F100(0, 14, 127, 64, 0x64u, 0, 3u);
					str_E1BAC[jx].selected_8 = 1;
					ResetMouse_7B5A0();
				} else {
					str_E1BAC[jx].gold_color_24 = 1;
					x_BYTE_17DBC6 = 1;
				}
				break;
			}
		}
	}
	if (x_D41A0_BYTEARRAY_4_struct.showHelp_10 != 1 || !x_DWORD_17DE38str.showHelp_17DF13 || m_ExitMenuLoop_E29DC)
		return 0;
	if (x_BYTE_17DBC6 == 2) {
		times_17DBB8[1] = j___clock();
		if ((times_17DBB8[1] - times_17DBB8[0]) / 0x64u > 1) {
			x_WORD_17DBC4++;
			if (!textBoxStr_E25DC[x_WORD_17DBC4].minx2_2)
				x_WORD_17DBC4 = 0;
			times_17DBB8[0] = times_17DBB8[1];
		}
		memset(DrawAndServe_textBoxStr, 0, 36);
		DrawAndServe_textBoxStr[0] = textBoxStr_E25DC[x_WORD_17DBC4];
		int index = 0;
		sub_7E840_draw_textbox_with_line(DrawAndServe_textBoxStr, 80, 89);
		if (!str_E1BAC[0].xmin_10)
			return 0;
		do {
			if (str_E1BAC[index].byte_22 == str_BYTE_E25ED_0x[x_WORD_17DBC4].byte_0) {
				str_E1BAC[index].gold_color_24 = 1; //turn on gold selection
				return 0;
			}
			index++;
		} while (str_E1BAC[index].xmin_10);
		return false;
	} else {
		if (x_BYTE_17DBC6 == 3) {
			times_17DBB8[1] = j___clock();
			if ((times_17DBB8[1] - times_17DBB8[0]) / 0x64u > 1) {
				x_BYTE_17DBC6 = 2;
				return 0;
			}
			return 0;
		}
		int index2 = 0;
		if (textBoxStr_E25DC[index2].minx2_2) {
			while (textBoxStr_E25DC[index2].byte_17 != str_E1BAC[jx].byte_22) {
				index2++;
				if (!textBoxStr_E25DC[index2].minx2_2)
					return 0;
			}
			memset(DrawAndServe_textBoxStr, 0, 36);
			DrawAndServe_textBoxStr[0] = textBoxStr_E25DC[index2];
			sub_7E840_draw_textbox_with_line(DrawAndServe_textBoxStr, 80, 89);
			int time = j___clock();
			times_17DBB8[1] = time;
			times_17DBB8[0] = time;
			x_BYTE_17DBC6 = 3;
			return false;
		} else {
			return false;
		}
	}
	return false;
}

void MainMenu_76FA0_modX(typeStateMenu newState) //257fa0
{
	if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Begin })||
	    (newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Begin }))
	{
		//fixed
		help_VGA_type_resolution = 0;
		//fixed
		MainMenu_onlyBlit = false;
		MainMenu_introIndex = 1;

		StopCdPlayback_86860(x_WORD_1803EC); //267860
		LoadSounds_84300(0); //265300
		memset(&times_17DBB8, 0, 16);
		x_BYTE_17DBC6 = 2;
		x_DWORD_17DE38str.savedGameIndex_17DF04 = -1;
		x_DWORD_17DE38str.x_DWORD_17DE44 = x_DWORD_E9C38_smalltit;
		SetCenterScreenForFlyAssistant_6EDB0();
		StopMusic_8E020(); //26f020
		StartMusic_8E160(4, 0x7Fu); //26f160
		x_WORD_17DE26 = 0;
		VGA_cleanKeyBuffer();
	}
	if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Begin }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Step }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::End }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Begin }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Step }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::End }))
		if (x_BYTE_E29E1 || x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 & Setting::MULTIPLAYER_MODE || (NewGameDialog_77350_modX(0, newState), !m_ExitMenuLoop_E29DC)) {
			if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Begin })||
				(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Begin }))
			{
				x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 &= 0xEFu;
				sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 4);
				ResetMouse_7B5A0();
				SetCursor_8CD27(xy_DWORD_17DED4_spritestr[39]);
				x_DWORD_17DE38str.showHelp_17DF13 = x_D41A0_BYTEARRAY_4_struct.showHelp_10;
				times_17DBB8[0] = j___clock();
				int lastTime = j___clock();
				int16_t tempMousePosX = x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx;
				int16_t tempMousePosY = x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony;
				int scanCode = x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode;
			}
			if (!m_ExitMenuLoop_E29DC) {
				if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Begin })||
				    (newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Step }))
				{
					SetFrameStart(std::chrono::system_clock::now());
					if ((MainMenu_tempMousePosX == x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx) && (MainMenu_tempMousePosY == x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony) && (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode == MainMenu_scanCode)) {
						if ((j___clock() - MainMenu_lastTime) / 100 > 60) //after 1 min run intro
						{
							//actState = typeStateMenu2::animFlvSelected;//not used at now
							/*
							uint8_t *tempSmalltit = x_DWORD_E9C38_smalltit;
							x_DWORD_E9C38_smalltit = x_DWORD_17DE38str.x_DWORD_17DE44;
							if (newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Begin })
								PlayIntros_83250(MainMenu_introIndex);
							x_DWORD_E9C38_smalltit = tempSmalltit;
							MainMenu_introIndex = (MainMenu_introIndex == 1) + 1; //alternate 1 and 2
							MainMenu_tempMousePosY = x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony;
							MainMenu_tempMousePosX = x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx;
							x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode = 0;
							MainMenu_scanCode = 0;
							MainMenu_lastTime = j___clock();
							StopMusic_8E020();
							MainMenu_onlyBlit = 0;
							StartMusic_8E160(4, 0x7Fu);
							*/
							MainMenu_lastTime = j___clock(); //added code
						}
					} else {
						MainMenu_tempMousePosY = x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony;
						MainMenu_tempMousePosX = x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx;
						MainMenu_scanCode = x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode;
						MainMenu_lastTime = j___clock();
					}
					if (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode == 59) {
						x_D41A0_BYTEARRAY_4_struct.showHelp_10 = x_D41A0_BYTEARRAY_4_struct.showHelp_10 != 1;
						x_DWORD_17DE38str.showHelp_17DF13 = x_D41A0_BYTEARRAY_4_struct.showHelp_10;
						x_D41A0_BYTEARRAY_4_struct.setting_38402 = 1;
					}
					if (x_WORD_180660_VGA_type_resolution & 1)
						CopyScreen(x_DWORD_E9C38_smalltit, pdwScreenBuffer_351628, 320, 200);
					else
						CopyScreen(x_DWORD_E9C38_smalltit, pdwScreenBuffer_351628, 640, 480);

					sub_7C120_draw_bitmap_640(185, 232, xy_DWORD_17DED4_spritestr[66]); //adress 25827a
					if (newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Step })
						DrawMenuAnimations_7AB00(); //25bb00	
				}
				if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Begin }) ||
					(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Step }) ||
					(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::End }) ||
					(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Step }))
				{
					if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Begin }) ||//added code
						(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Step }) ||//added code
						(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::End }))//added code
					{//added code
						if (NewGameDialog_77350_modX(&str_E1BAC[0], newState))//added code
						{//added code
							MainMenu_tempMousePosY = x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony;//added code
							MainMenu_tempMousePosX = x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx;//added code
							MainMenu_onlyBlit = false;//added code
							MainMenu_scanCode = x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode;//added code
							MainMenu_lastTime = j___clock();//added code
						}//added code
					} else//added code
						if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::End }) ||
						    (newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Step }))
						{
							if (DrawAndServe_7B250_modX(newState)) //25c250
							{
								MainMenu_tempMousePosY = x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony;
								MainMenu_tempMousePosX = x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx;
								MainMenu_onlyBlit = false;
								MainMenu_scanCode = x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode;
								MainMenu_lastTime = j___clock();
							}
						}
				}
				if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::End }) ||
					(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Step }))
				{
					if (MainMenu_onlyBlit) {
						if (x_WORD_180660_VGA_type_resolution & 1)
							sub_90478_VGA_Blit320(menuFps);
						else
							sub_75200_VGA_Blit640(480, menuFps);
					} else {
						MainMenu_onlyBlit = true;
						//34ee38 20 0
						sub_90B27_VGA_pal_fadein_fadeout(x_DWORD_17DE38str.palette_17DE38x, 0x20u, 0); //tady
					}
					sub_7A060_get_mouse_and_keyboard_events();
				}
			}
			if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::End }) ||
				(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::End }))
			{
				sub_41BC0();
				D41A0_0.m_GameSettings.m_Display.m_uiScreenSize = 0;
				sub_753D0();
				x_DWORD_E9C38_smalltit = x_DWORD_17DE38str.x_DWORD_17DE44;
			}
		} else {
			if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Begin }) ||
				(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::End }) ||
				(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Begin }) ||
				(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::End }))
			{
				sub_41BC0();
				D41A0_0.m_GameSettings.m_Display.m_uiScreenSize = 0;
				sub_753D0();
			}
		}
}

FILE *tempfile_PlayInfoFmv;
bool PlayInfoFmv_break=false;

void PlayInfoFmv_modX(__int16 allowSkip, __int16 redrawText, Type_SoundEvent_E17CC *pSoundEvent, char *path, typeStateMenu newState) //sub_76160 - 257160
{
	//FILE *tempfile;
	if (newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Begin })
	{
		redrawTextInVideo_E12FC = redrawText;
		soundEventIndex_D4004 = 0;
		//x_WORD_17DB58 = 0;//not used
		ActualKeyframe_17DB60 = 0;
		framebuffer_E12F4x = (TColor *)pdwScreenBuffer_351628;
		tempfile_PlayInfoFmv = DataFileIO::CreateOrOpenFile(path, 512);
		x_DWORD_17DB38_intro_file_handle = tempfile_PlayInfoFmv;
	}
	if (tempfile_PlayInfoFmv) {
		if (newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Begin })
		{
			DataFileIO::Read(tempfile_PlayInfoFmv, (uint8_t *)&unk_17DB40str, sizeof(Type_17DB40)); //ecx=12
			LastKeyframe_17DB46 = unk_17DB40str.frameCount_6;
			height_17DB48 = unk_17DB40str.height_8;
			width_17DB4A = unk_17DB40str.width_alt_10;
			x_WORD_180744_mouse_right_button = 0;
			x_WORD_180746_mouse_left_button = 0;
			fileOffset_E1300 += 12;
			LastPressedKey_1806E4 = 0;
			stopPlaybackFlag_17DB5A = 0;
			FlvInitSet_473B0(); //2283b0
			allowSkipVideo_17DB5C = allowSkip;
		}
		if (newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Step })
			if ((LastPressedKey_1806E4 != 1) && !PlayInfoFmv_break) {
			SetFrameStart(std::chrono::system_clock::now());
			if (stopPlaybackFlag_17DB5A)
				PlayInfoFmv_break = true; //break;
			if (ActualKeyframe_17DB60 >= LastKeyframe_17DB46 - 1) //34eb60 a 34eb46
				PlayInfoFmv_break = true; //break;
			PlayIntoSoundEvents_1B280(pSoundEvent);
			ReadFrame_75DB0(); //256db0 - read header
			DrawFrame_75E70(); //256e70 - draw intro frame
			ActualKeyframe_17DB60++;
		}// while (LastPressedKey_1806E4 != 1); //while not key pressed
		if (newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::End })
			DataFileIO::Close(x_DWORD_17DB38_intro_file_handle);
	}
}

int globalAnimIndex = -1;

void Intros_76D10_modX(char introType, typeStateMenu newState) //257d10
{
	char introPath[MAX_PATH];
	if (newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Begin })
	{
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
		StopSubtitles_2EB40();
		if (soundAble_E3798 && x_D41A0_BYTEARRAY_4_struct.SelectedLangIndex == 2) {
			DisplaySubtitles_D41C1 = 0;
			DisplaySubtitles_D41C0 = 0;
		} else {
			DisplaySubtitles_D41C0 = 1;
			DisplaySubtitles_D41C1 = 1;
		}
		SetCursor_8CD27((*filearray_2aa18c[filearrayindex_POINTERSDATTAB].posistruct)[0]); //Set cursor to Null (Don't Draw)
	}
	switch (introType) {
		case 0:
			if (newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Begin })
			{
#ifndef debug_hide_graphics
				ShowWelcomeScreen_83850(); //frog logo and wait
#endif
				sprintf(introPath, "%s/%s", cdDataPath.c_str(), "INTRO/INTRO.DAT");
				PlayInfoFmv_modX(1, 1, str_E17CC_0, introPath, newState); //257160 intro .. 2b27cc
				StopSubtitles_2EB40();
				DisplaySubtitles_D41C1 = 0;
				DisplaySubtitles_D41C0 = 0;
				while (sub_9A10A_check_keyboard()) {
					LastPressedKey_1806E4 = 0;
					sub_7A060_get_mouse_and_keyboard_events();
				}
				j___delay(50);
			}
			if ((newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Begin })||
				(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Step })||
				(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::End }))
			{
				sprintf(introPath, "%s/%s", cdDataPath.c_str(), "INTRO/INTRO2.DAT");
				PlayInfoFmv_modX(1, 1, str_E17CC_0x160, introPath, newState); //E192C
			}
			break;
		case 1:
			if ((newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Begin })||
				(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Step })||
				(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::End }))
			{
				sprintf(introPath, "%s/%s", cdDataPath.c_str(), "INTRO/INTRO.DAT");
				PlayInfoFmv_modX(1, 1, str_E17CC_0, introPath, newState);
			}
			break;
		case 2:
			if (newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Begin })
			{
				LastPressedKey_1806E4 = 0;
				DisplaySubtitles_D41C1 = 0;
				DisplaySubtitles_D41C0 = 0;
			}
			if ((newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Begin })||
				(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Step })||
				(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::End }))
			{
				sprintf(introPath, "%s/%s", cdDataPath.c_str(), "INTRO/INTRO2.DAT");
				PlayInfoFmv_modX(1, 1, str_E17CC_0x160, introPath, newState); //E192C
			}
			break;
	}
	if (newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::End }) {
		sub_90B27_VGA_pal_fadein_fadeout(0, 0x10u, 0);
		EndSample_8D8F0();
		StopMusic_8E020();
		FadeClearBlit_7B5D0();
		nextMenu_E29D8 = MenuItem::MainMenu;
		DisplaySubtitles_D41C0 = 0;
		DisplaySubtitles_D41C1 = 0;
		if (sub_9A10A_check_keyboard())
			sub_7A060_get_mouse_and_keyboard_events();
		x_DWORD_17DE38str.x_BYTE_17DF11_last_key_status = 0;
		x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode = 0;
		if (!introType)
			LoadAndSetGraphicsAndPalette_7AC00();
	}
}

void MenusAndIntros_76930_mod(bool skipMenus, typeStateMenu newState) //257930
{
	int animIndex = 0;
	if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Begin }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Begin }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Begin }))
	{
		if (newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Begin })
			nextMenu_E29D8 = MenuItem::InitLanguage;

		//1 -351660
		x_BYTE_E29DF_skip_screen = x_BYTE_D41AD_skip_screen;
		if (skipMenus) {
			x_BYTE_D41AD_skip_screen = 1;
			m_ExitMenuLoop_E29DC = 1;
		} else {
			m_ExitMenuLoop_E29DC = 0;
		}

		if (x_BYTE_D41AD_skip_screen == 1 || (nextMenu_E29D8 != MenuItem::InitLanguage)) {
			PlayInGameFmv_82670();
			LoadAndSetGraphicsAndPalette_7AC00();
		}
		if (x_BYTE_D41AD_skip_screen == 1) {
			InitLanguage_76A40();
			nextMenu_E29D8 = MenuItem::MainMenu;
		}
		memset(&x_DWORD_17DE38str, 0, sizeof(type_x_DWORD_17DE38str));
		x_DWORD_17DE38str.x_DWORD_17DEE0_filedesc = NULL;
		sub_7BEC0(); //25CEC0
		SetCenterScreenForFlyAssistant_6EDB0(); //24FDB0
		WriteConfigDat_81DB0(); //262DB0

		//added code!!!!!!!!!!!!!!!!!!!!!!!!
		if (((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Begin })||
		    (newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Begin }))
				/*
				&& actState == typeStateMenu2::Zero*/)
		{
			nextMenu_E29D8 = MenuItem::MainMenu;
			Intros_76D10(-1);
			//x_WORD_180660_VGA_type_resolution = 1;
			LoadAndSetGraphicsAndPalette_7AC00();
			x_BYTE_E29E1 = 1;
		}
		//added code!!!!!!!!!!!!!!!!!!!!!!!!
	}

	//added code!!!!!!!!!!!!!!!!!!!!!!!!
	if ((newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Begin }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Step }))
	{
		//ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 640, 480, 0); //fix
		//sub_90B27_VGA_pal_fadein_fadeout((TColor *)*xadatapald0dat2.colorPalette_var28, 0x20u, 0);
		nextMenu_E29D8 = MenuItem::Intros;
		animIndex = globalAnimIndex;
	}
	if (newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Step }) {
		//ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 640, 480, 0); //fix
		//sub_90B27_VGA_pal_fadein_fadeout((TColor *)*xadatapald0dat2.colorPalette_var28, 0x20u, 0);
		nextMenu_E29D8 = MenuItem::MainMenu;
	}
	//added code!!!!!!!!!!!!!!!!!!!!!!!!

	if (!m_ExitMenuLoop_E29DC) {
		//2b39d8
		switch (nextMenu_E29D8) {
			case MenuItem::InitLanguage:
				InitLanguage_76A40(); //257A40
				break;
			case MenuItem::SetToIntro:
				SetToIntro_76CF0(); //257cf0 nastavi x_WORD_E29D8 na 3
				break;
			case MenuItem::LeaveX:
				//_wcpp_1_unwind_leave__131(); //257d00 asi konec
				break;
			case MenuItem::Intros:
				Intros_76D10_modX(animIndex, newState); //257d10
				if (newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::End })
					nextMenu_E29D8 = MenuItem::MainMenu;
				break;
			case MenuItem::MainMenu:
				MainMenu_76FA0_modX(newState); //257fa0
				break;
			case MenuItem::Exit:
				m_ExitMenuLoop_E29DC = 1;
				break;
			case MenuItem::LangSettings:
				LanguageSettingDialog_779E0(0); //2589e0
				break;
			default:
				break;
		}
	}// while (!m_ExitMenuLoop_E29DC);
	if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::End }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::End }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::End }))
	{
		sub_7ADE0(x_BYTE_E29DE);
		if (x_BYTE_E29E1)
			x_BYTE_E29E1 = 0;
		WriteConfigDat_81DB0();
	}
}

void DrawAndEventsInGame_47560_modX(int16_t turn) //228560
{
	SetFrameStart(std::chrono::system_clock::now());
	PaletteChanges_47760();
	if (!(x_D41A0_BYTEARRAY_4_struct.OptionsSettingFlag_24 & 1)) {
		sub_715B0(); //nothing draw //animate sprites
	}
	ReadGameUserInputs_89D10(); //get keys
	if (CommandLineParams.DoAutoChangeRes()) {
		if ((gameResWidth >= 640) && (gameResHeight >= 480)) {
			if (resindex_begin == 1) {
				//Auto press "r" key
				LastPressedKey_1806E4 = 0x13;
				resindex_begin++;
			} else {
				if (resindex_begin == 2) {
					LastPressedKey_1806E4 = 0;
					resindex_begin++;
				} else {
					if (resindex_begin < 1) {
						resindex_begin++;
					}
				}
			}
		}
	}
	MouseAndKeysEvents_17A00(turn);
	if (CommandLineParams.DoIntervalSave()) {
		//save in interval
		int interval = 1;
		if (save_debugcounter % interval == 0)
			intervalsave(save_debugcounter / interval);
		//save in interval
		save_debugcounter++;
	}
	PlayerEvents_51BB0(); //nothing draw
	sub_848A0(); //nothing draw
	uint8_t speed = x_D41A0_BYTEARRAY_4_struct.speedIndex;
	if (speed == 0) {
		if (!speed)
			UpdateEntities_57730();
	} else if (speed == 1) {
		for (int i = 0; i < 4; i++)
			UpdateEntities_57730();
	} else if (speed == 2) {
		for (int j = 0; j < 8; j++)
			UpdateEntities_57730();
	}
	sub_84B80(); //prepare lightting
	sub_58F00_game_objectives(); //nothing draw
	PresentObjective_59820(); //nothing draw
	if (!(x_D41A0_BYTEARRAY_4_struct.OptionsSettingFlag_24 & 1))
		sub_57570(); //nothing draw
	sub_575C0(); //nothing draw
	PlayEntitySounds_6E150(); //nothing draw
	//DrawGameFrame_2BE30();//ONLY DRAW GET DRAW MENU FROM IT MAYBE
	//if (x_D41A0_BYTEARRAY_4_struct.showHelp_10)
	//	DrawHelpPopUps_871F0();//ONLY DRAW GET DRAW MENU FROM IT MAYBE
	x_D41A0_BYTEARRAY_4_struct.byteindex_196 = GameTimerTurn_17DB54 - x_D41A0_BYTEARRAY_4_struct.byteindex_196;
	//DrawGameDebugText_6FEC0();//ONLY DRAW GET DRAW MENU FROM IT MAYBE
	x_D41A0_BYTEARRAY_4_struct.byteindex_196 = GameTimerTurn_17DB54;
	//if (x_D41A0_BYTEARRAY_4_struct.paletteMod_51 >= 3u)
	//	sub_40F80();//ONLY DRAW GET DRAW MENU FROM IT MAYBE
}

bool InGameLoop_47320_break = false;
uint32_t InGameLoop_47320_mod_gameTurn = 0;

void InGameLoop_47320_modX(typeStateMenu newState) //228320
{
	if ((newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::BeginAfterScreen }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::BeginAfterSecret }))
	{
		x_D41A0_BYTEARRAY_4_struct.paletteMod_51 = 0;
		InGameLoop_47320_mod_gameTurn = 0;
		D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.word[1] = 0;

		//fix res on begin level for hidden levels-neoriginal code
		//if (((gameResWidth != 320) && (gameResHeight != 200)) && ((gameResWidth != 640) && (gameResHeight != 480)))
		{
			VGA_Resize(320, 200);
			screenWidth_18062C = 320;
			screenHeight_180624 = 200;
			sub_A0D50_set_viewport(0, 0, 320, 200);
			x_WORD_180660_VGA_type_resolution = 1;
			resindex_begin = 0;
		}
		//fix res on begin level for hidden levels-neoriginal code
	}
	if (newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::Step })
		if (!InGameLoop_47320_break) {
			if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].byte_0x004_2BE0_11234 || D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 8)
				InGameLoop_47320_break = true;
			DrawAndEventsInGame_47560_modX(GameTimerTurn_17DB54);
			if (InGameLoop_47320_mod_gameTurn < 2) {
				StopMusic_8E020();
				if (InGameLoop_47320_mod_gameTurn == 1)
					StartMusic_8E160(D41A0_0.maptypeMusic_0x235, 0x7Fu);
				InGameLoop_47320_mod_gameTurn++;
			}
		}
	if (newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::End })
	sub_90E07_VGA_set_video_mode_640x480_and_Palette((TColor *)*xadatapald0dat2.colorPalette_var28);
}

bool setLoadScreen = false;

void sub_47FC0_load_screen_mod(bool isSecretLevel) //228fc0
{
	char dataPath[MAX_PATH];
	sub_90B27_VGA_pal_fadein_fadeout(0, 0x10u, 0);
	//debug
	sub_90B27_VGA_pal_fadein_fadeout((TColor *)*xadatapald0dat2.colorPalette_var28, 0x20u, 0);
	//debug
	if (isSecretLevel) {
		sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/SMATITL2.DAT");
		DataFileIO::ReadFileAndDecompress(dataPath, &x_DWORD_E9C38_smalltit);
	} else {
		sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/SMATITLE.DAT");
		DataFileIO::ReadFileAndDecompress(dataPath, &x_DWORD_E9C38_smalltit);
	}
	CopyScreen_85B20(x_DWORD_E9C38_smalltit, pdwScreenBuffer_351628, 0x190u);
	if (x_WORD_180660_VGA_type_resolution & 1)
		sub_90478_VGA_Blit320();
	else
		sub_75200_VGA_Blit640(480);
	if (isSecretLevel) {
		sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/SMATITL2.PAL");
		DataFileIO::ReadFileAndDecompress(dataPath, xadatapald0dat2.colorPalette_var28);
	} else {
		sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/SMATITLE.PAL");
		DataFileIO::ReadFileAndDecompress(dataPath, xadatapald0dat2.colorPalette_var28);
	}
	sub_90B27_VGA_pal_fadein_fadeout((TColor *)*xadatapald0dat2.colorPalette_var28, 0x20u, 0);
	D41A0_0.dword_0x23a = 1;
	D41A0_0.dword_0x23e = 0;
	D41A0_0.dword_0x242 = 0;
	setLoadScreen = true;
}

Type_SecretMapScreenPortals_E2970* secretsModPortals;

bool sub_46830_main_loop_break = false;

unsigned __int16 tempActLevel=-1;

void sub_46830_main_loop_modX(unsigned __int16 actLevel, typeStateMenu newState) //227830
{ //graphics already inited
	/*
	bool isSecretLevel;
	bool skipMenus;
	int16_t setLevel;
	std::string customLevelPath;
	*/
	if ((newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::BeginAfterSecret }) ||		
		(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::EndPostSecretScreen }))
		actLevel=tempActLevel;

	if (((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Begin }) ||		
		(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Begin }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::Begin }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Begin }))
			/*	&& actState == typeStateMenu2::Zero*/
		)
	{
		main_loop_skipMenus = false;
		main_loop_setLevel = -1;
		main_loop_customLevelPath = "";
		x_D41A0_BYTEARRAY_4_struct.setting_30 = 0; //2a51a4
		D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].byte_0x004_2BE0_11234 = 0;
		main_loop_setLevel = CommandLineParams.GetSetLevel();
		main_loop_customLevelPath = CommandLineParams.GetCustomLevelPath();
		if (main_loop_setLevel > -1 || main_loop_customLevelPath.length() > 0)
			main_loop_skipMenus = true;
		//return;
	}
	//while (1)
	if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Begin }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Step }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::End }) ||		
		(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Begin }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Step }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::End }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::Begin }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::BeginAfterScreen }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::BeginAfterSecret }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::Step }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::End }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::EndPostSecretScreen }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Begin }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Step }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::End }))
	{
		if (((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Begin }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Begin }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::Begin }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Begin }))
				/*
				&& actState == typeStateMenu2::Zero*/)
		{
			if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].byte_0x004_2BE0_11234) {
				return;
			}
			sub_48350(); //fix it //229350
		}
		if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Begin }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Step }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::End }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Begin }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Step }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::End }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Begin }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Step }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::End }))
				MenusAndIntros_76930_mod(main_loop_skipMenus, newState); //set language, intro, menu, atd. //257930
		if ((newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::Begin }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::BeginAfterScreen }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::BeginAfterSecret }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::Step }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::End }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::EndPostSecretScreen }))
		if (!D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].byte_0x004_2BE0_11234) {
			if (newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::Begin })
			{
				main_loop_isSecretLevel = x_D41A0_BYTEARRAY_4_struct.levelnumber_43w > 24 && x_D41A0_BYTEARRAY_4_struct.levelnumber_43w < 50;
				sub_47FC0_load_screen_mod(main_loop_isSecretLevel); //vga smaltitle
			}
			if (newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::BeginAfterScreen })
			{
				LevelInitGame_56A30(main_loop_setLevel, main_loop_customLevelPath);
				if (CommandLineParams.DoAutoChangeRes()) {
					resindex_begin = 0;
				}
				sub_47160();
			}
			if ((newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::BeginAfterScreen }) ||
				(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::BeginAfterSecret }) ||
				(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::Step }) ||
				(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::End }) ||
				(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::EndPostSecretScreen }))
				if ((!D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].byte_0x004_2BE0_11234) && !sub_46830_main_loop_break) {
					if ((newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::BeginAfterScreen }) ||
					    (newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::BeginAfterSecret }))
					{
						if (musicAble_E37FC && musicActive_E37FD && m_iNumberOfTracks) {
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
						SetCenterScreenForFlyAssistant_6EDB0();
						/*
						if (m_ptrGameRender == nullptr) {
							if (!strcmp(forceRender, "NG"))
								m_ptrGameRender = (GameRenderInterface *)new GameRenderNG();
							else if (!strcmp(forceRender, "Original"))
								m_ptrGameRender = (GameRenderInterface *)new GameRenderOriginal();
							else if (!strcmp(forceRender, "HD"))
								m_ptrGameRender = (GameRenderInterface *)new GameRenderHD(pdwScreenBuffer_351628, *xadatapald0dat2.colorPalette_var28, (multiThreadedRender ? numberOfRenderThreads : 0), assignToSpecificCores);
							else {
								if ((gameResWidth <= 640) && (gameResHeight <= 480)) {
									m_ptrGameRender = (GameRenderInterface *)new GameRenderOriginal();
								} else {
									m_ptrGameRender = (GameRenderInterface *)new GameRenderHD(pdwScreenBuffer_351628, *xadatapald0dat2.colorPalette_var28, (multiThreadedRender ? numberOfRenderThreads : 0), assignToSpecificCores);
								}
							}
						}
						*/
					}
					if ((newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::Step }) ||
							(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::BeginAfterScreen }) ||
							(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::BeginAfterSecret }) ||
							(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::End }))
					{
						//added code
						VGA_Set_Palette(xadatapald0dat2.colorPalette_var28[0], true);
						//added code

						InGameLoop_47320_modX(newState);
					}
					/*
					if (m_ptrGameRender != nullptr) {
						delete m_ptrGameRender;
						m_ptrGameRender = nullptr;
					}*/
					if ((newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::End }) ||
						(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::EndPostSecretScreen }))
					{
						if (newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::End })
						{
							sub_53CC0_close_movie();
							EndSample_8D8F0();
							StopMusic_8E020();
							StopCdPlayback_86860(x_WORD_1803EC);
							RestoreSoundVolume_59BF0();
							sub_90B27_VGA_pal_fadein_fadeout(0, 0x10u, 0);
							if (x_WORD_180660_VGA_type_resolution & 1)
								ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 320, 200, getPaletteIndex_5BE80((TColor *)*xadatapald0dat2.colorPalette_var28, 0, 0, 0));
							else
								ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 640, 480, getPaletteIndex_5BE80((TColor *)*xadatapald0dat2.colorPalette_var28, 0, 0, 0));
							if (x_WORD_180660_VGA_type_resolution & 1)
								sub_90478_VGA_Blit320(maxGameFps);
							else
								sub_75200_VGA_Blit640(480, maxGameFps);
							if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 2 && !(x_D41A0_BYTEARRAY_4_struct.setting_38545 & 4)) {
								sub_6DB50(1, 0);
							}
							sub_713A0();
							if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 4) {
								sub_56D60(actLevel, 0);
								D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] = 4;
							} else if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 2) {
								CollectLevelStats_5C530();
								if (x_D41A0_BYTEARRAY_4_struct.setting_38545 & 0x20)
									sub_6E0D0();
							} else {
								D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] |= 8;
							}
						}
						if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 0x10)
						{
							if (newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::End })
							{
								actLevel = x_D41A0_BYTEARRAY_4_struct.levelnumber_43w;
								if (actLevel >= 0x18u) {
									if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 2)
										CollectLevelStats_5C530();
									sub_46830_main_loop_break = true;
									//break;
								}
							}
							if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 2)
							{
								if (newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::End })
									secretsModPortals = GetSecretAndActivedPortal_824B0(actLevel);
								if (secretsModPortals)
								{
									if (newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::End })
									{
										x_D41A0_BYTEARRAY_4_struct.levelnumber_43w = secretsModPortals->levelNumber_6;

										//fix level number-neoriginal code
										actLevel = x_D41A0_BYTEARRAY_4_struct.levelnumber_43w;
										//fix level number-neoriginal code

										//fix res before secret level-neoriginal code
										VGA_Resize(320, 200);
										screenWidth_18062C = 320;
										screenHeight_180624 = 200;
										sub_A0D50_set_viewport(0, 0, 320, 200);
										x_WORD_180660_VGA_type_resolution = 1;
										tempActLevel = actLevel;
										//fix res before secret level-neoriginal code

										sub_47FC0_load_screen_mod(true);
									}
									if (newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::EndPostSecretScreen })
									{
										LevelInitGame_56A30(actLevel);
										sub_47160();
									}
								}
							}
						} else if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 0xA) {
							sub_46830_main_loop_break = true;
							//break; //must be here
						}
					}
				}
			if (newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::EndPostSecretScreen }) {
				nextMenu_E29D8 = MenuItem::MainMenu;
				main_loop_skipMenus = false;
				main_loop_setLevel = -1;
				main_loop_customLevelPath = "";

				sub_46830_main_loop_break = false;
			}
		}
	}
}

std::mutex main_mutex;
std::condition_variable main_cv;
bool thread1_turn = false;

Thread1_State thread1_state = Thread1_State::BEGIN;
Thread2_State thread2_state = Thread2_State::BEGIN;

bool thread1_waiting = false;
bool thread2_waiting = false;

void thread2_wait_for_continue(Thread2_State sendstate) {
	{
		std::lock_guard<std::mutex> lock(main_mutex);
		thread2_state = sendstate;
		thread2_waiting = true;
		thread1_waiting = false;
	}
	main_cv.notify_all();

	std::unique_lock<std::mutex> lock(main_mutex);
	main_cv.wait(lock, [] { return !thread2_waiting; });
}

void thread1_wait_for_continue(Thread1_State sendstate) {
	{
		std::lock_guard<std::mutex> lock(main_mutex);
		thread1_state = sendstate;
		thread1_waiting = true;
		thread2_waiting = false;
	}
	main_cv.notify_all();

	std::unique_lock<std::mutex> lock(main_mutex);
	main_cv.wait(lock, [] { return !thread1_waiting; });
}

void thread1_continue(Thread2_State sendstate) {	
	{
		std::lock_guard<std::mutex> lock(main_mutex);
		thread2_state = sendstate;
		thread1_waiting = false;
	}
	main_cv.notify_one();
}

void thread2_continue(Thread1_State sendstate) {	
	{
		std::lock_guard<std::mutex> lock(main_mutex);
		thread1_state = sendstate;
		thread2_waiting = false;
	}
	main_cv.notify_one();
}

void ShowWelcomeScreen_83850_mod() //264850
{
	char dataPath[MAX_PATH];
	sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/SCREENS/HSCREEN0.DAT");
	sub_7AA70_load_and_decompres_dat_file(dataPath, x_DWORD_E9C38_smalltit, 0x178E5F, 0x32B9);
	sub_7AA70_load_and_decompres_dat_file(dataPath, *xadatapald0dat2.colorPalette_var28, 0x17C118, 0x300);
	sub_7AA70_load_and_decompres_dat_file(cdDataPath.c_str(), nullptr, 0, 0);
	bool afterFade = false;
	int time = j___clock();
	LastPressedKey_1806E4 = 0;
	while (!LastPressedKey_1806E4 && !x_WORD_180746_mouse_left_button && !x_WORD_180744_mouse_right_button && (j___clock() - time) / 100 <= 20) {
		SetFrameStart(std::chrono::system_clock::now());
		if (x_WORD_180660_VGA_type_resolution & 1)
			CopyScreen(x_DWORD_E9C38_smalltit, pdwScreenBuffer_351628, 320, 200);
		else
			CopyScreen(x_DWORD_E9C38_smalltit, pdwScreenBuffer_351628, 640, 480);
		if (afterFade) {
			if (x_WORD_180660_VGA_type_resolution & 1)
				sub_90478_VGA_Blit320(menuFps);
			else
				sub_75200_VGA_Blit640(480, menuFps);
			thread2_wait_for_continue(Thread2_State::SHOW_WELCOME_SCREEN_LOOP);
		} else {
			afterFade = true;
			ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 640, 480, 0);
			sub_75200_VGA_Blit640(480, menuFps);
			thread2_wait_for_continue(Thread2_State::SHOW_WELCOME_SCREEN_LOOP);
			sub_90B27_VGA_pal_fadein_fadeout((TColor *)*xadatapald0dat2.colorPalette_var28, 0x20u, 0);
		}
	}
	sub_90B27_VGA_pal_fadein_fadeout(0, 0x10u, 0);
	if (x_WORD_180660_VGA_type_resolution & 1)
		ClearGraphicsBuffer_72883(pdwScreenBuffer_351628, 320, 200, 0);
	else
		ClearGraphicsBuffer_72883(pdwScreenBuffer_351628, 640, 480, 0);
	if (x_WORD_180660_VGA_type_resolution & 1)
		sub_90478_VGA_Blit320(menuFps);
	else
		sub_75200_VGA_Blit640(480, menuFps);
	thread2_wait_for_continue(Thread2_State::SHOW_WELCOME_SCREEN_LOOP);
	while (sub_9A10A_check_keyboard()) {
		LastPressedKey_1806E4 = 0;
		sub_7A060_get_mouse_and_keyboard_events();
	}
	j___delay(50);
}

void PlayInfoFmv_mod(__int16 allowSkip, __int16 redrawText, Type_SoundEvent_E17CC *pSoundEvent, char *path) //sub_76160 - 257160
{
	FILE *tempfile;
	redrawTextInVideo_E12FC = redrawText;
	soundEventIndex_D4004 = 0;
	//x_WORD_17DB58 = 0;//not used
	ActualKeyframe_17DB60 = 0;
	framebuffer_E12F4x = (TColor *)pdwScreenBuffer_351628;
	tempfile = DataFileIO::CreateOrOpenFile(path, 512);
	x_DWORD_17DB38_intro_file_handle = tempfile;
	if (tempfile) {
		DataFileIO::Read(tempfile, (uint8_t *)&unk_17DB40str, sizeof(Type_17DB40)); //ecx=12
		LastKeyframe_17DB46 = unk_17DB40str.frameCount_6;
		height_17DB48 = unk_17DB40str.height_8;
		width_17DB4A = unk_17DB40str.width_alt_10;
		x_WORD_180744_mouse_right_button = 0;
		x_WORD_180746_mouse_left_button = 0;
		fileOffset_E1300 += 12;
		LastPressedKey_1806E4 = 0;
		stopPlaybackFlag_17DB5A = 0;
		FlvInitSet_473B0(); //2283b0
		allowSkipVideo_17DB5C = allowSkip;
		do {
			SetFrameStart(std::chrono::system_clock::now());
			if (stopPlaybackFlag_17DB5A)
				break;
			if (ActualKeyframe_17DB60 >= LastKeyframe_17DB46 - 1) //34eb60 a 34eb46
				break;
			PlayIntoSoundEvents_1B280(pSoundEvent);
			ReadFrame_75DB0(); //256db0 - read header
			DrawFrame_75E70(); //256e70 - draw intro frame
			thread2_wait_for_continue(Thread2_State::PLAY_INFO_FLV_LOOP);
			ActualKeyframe_17DB60++;
		} while (LastPressedKey_1806E4 != 1); //while not key pressed
		DataFileIO::Close(x_DWORD_17DB38_intro_file_handle);
	}
}

void Intros_76D10_mod(char introType) //257d10
{
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

	StopSubtitles_2EB40();
	if (soundAble_E3798 && x_D41A0_BYTEARRAY_4_struct.SelectedLangIndex == 2) {
		DisplaySubtitles_D41C1 = 0;
		DisplaySubtitles_D41C0 = 0;
	} else {
		DisplaySubtitles_D41C0 = 1;
		DisplaySubtitles_D41C1 = 1;
	}
	SetCursor_8CD27((*filearray_2aa18c[filearrayindex_POINTERSDATTAB].posistruct)[0]); //Set cursor to Null (Don't Draw)
	char introPath[MAX_PATH];
	switch (introType) {
		case 0:
#ifndef debug_hide_graphics
			ShowWelcomeScreen_83850_mod(); //frog logo and wait
#endif
			sprintf(introPath, "%s/%s", cdDataPath.c_str(), "INTRO/INTRO.DAT");
			PlayInfoFmv_mod(1, 1, str_E17CC_0, introPath); //257160 intro .. 2b27cc
			StopSubtitles_2EB40();
			DisplaySubtitles_D41C1 = 0;
			DisplaySubtitles_D41C0 = 0;
			while (sub_9A10A_check_keyboard()) {
				LastPressedKey_1806E4 = 0;
				sub_7A060_get_mouse_and_keyboard_events();
			}
			j___delay(50);
			sprintf(introPath, "%s/%s", cdDataPath.c_str(), "INTRO/INTRO2.DAT");
			PlayInfoFmv_mod(1, 1, str_E17CC_0x160, introPath); //E192C
			break;
		case 1:
			sprintf(introPath, "%s/%s", cdDataPath.c_str(), "INTRO/INTRO.DAT");
			PlayInfoFmv_mod(1, 1, str_E17CC_0, introPath);
			break;
		case 2:
			LastPressedKey_1806E4 = 0;
			DisplaySubtitles_D41C1 = 0;
			DisplaySubtitles_D41C0 = 0;
			sprintf(introPath, "%s/%s", cdDataPath.c_str(), "INTRO/INTRO2.DAT");
			PlayInfoFmv_mod(1, 1, str_E17CC_0x160, introPath); //E192C
			break;
	}
	sub_90B27_VGA_pal_fadein_fadeout(0, 0x10u, 0);
	EndSample_8D8F0();
	StopMusic_8E020();
	FadeClearBlit_7B5D0();
	nextMenu_E29D8 = MenuItem::MainMenu;
	DisplaySubtitles_D41C0 = 0;
	DisplaySubtitles_D41C1 = 0;
	if (sub_9A10A_check_keyboard())
		sub_7A060_get_mouse_and_keyboard_events();
	x_DWORD_17DE38str.x_BYTE_17DF11_last_key_status = 0;
	x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode = 0;
	if (!introType)
		LoadAndSetGraphicsAndPalette_7AC00();
}

bool NewGameDialog_77350_mod(type_menuButtons_E1F84 *a1x) //258350
{
	bool result = false;

	int endAction = 0;
	map_not_moving_WORD_E29D6 = false;
	if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 2 && x_D41A0_BYTEARRAY_4_struct.levelnumber_43w == 24) {
		// show credits after finishing the last level
		ShowEndCredits_833C0();
	}
	memset((void *)&unk_17DBA8str, 0, sizeof(unk_17DBA8str));
	unk_17DBA8str.x_BYTE_17DBB6 = 2;
	if (LoadLevelNumber_D419C <= -1) {
		IsPlayingCDTrack_17E09D = 0;
		x_DWORD_17DE38str.unk_17E078x.lastSpriteIndex_11 = 16;
		x_DWORD_17DE38str.y_17E06E = 480;
		x_DWORD_17DE38str.unk_17E078x.spriteIndex_8 = 13;
		x_DWORD_17DE38str.x_BYTE_17E09C = 0;
		x_DWORD_17DE38str.unk_17E078x.firstSpriteIndex_10 = 13;
		x_DWORD_17DE38str.x_17E06C = 0;
		memset((void *)&x_DWORD_17DB70str, 0, sizeof(type_x_DWORD_17DB70str));
		if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 2)
			x_DWORD_17DB70str.x_BYTE_17DB8F = 4;
		else
			x_DWORD_17DB70str.x_BYTE_17DB8F = 1;
		sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 6); //here init sprites
		SetCursor_8CD27((*filearray_2aa18c[filearrayindex_POINTERSDATTAB].posistruct)[0]); //Set cursor to Null (Don't Draw)

		ResetMouse_7B5A0();
		sub_90B27_VGA_pal_fadein_fadeout(0, 0x10u, 0);
		if (x_WORD_180660_VGA_type_resolution & 1) {
			ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 320, 200, getPaletteIndex_5BE80(x_DWORD_17DE38str.palette_17DE38x, 0, 0, 0));
		} else {
			ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 640, 480, getPaletteIndex_5BE80(x_DWORD_17DE38str.palette_17DE38x, 0, 0, 0));
		}
		if (x_WORD_180660_VGA_type_resolution & 1)
			sub_90478_VGA_Blit320();
		else
			sub_75200_VGA_Blit640(480);
		sub_41A90_VGA_Palette_install(x_DWORD_17DE38str.palette_17DE38x);
		PortalsUpdate_7DD70();
		x_DWORD_17DE38str.x_WORD_17DEEC = 0;
		SetCenterScreenForFlyAssistant_6EDB0();
		SetCursor_8CD27(xy_DWORD_17DED4_spritestr[239]);
		x_DWORD_17DB70str.x_WORD_17DB8A = x_D41A0_BYTEARRAY_4_struct.levelnumber_43w;
		while (!endAction) {
			SetFrameStart(std::chrono::system_clock::now());
			if (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode == 59) {
				x_DWORD_17DE38str.showHelp_17DF13 = x_D41A0_BYTEARRAY_4_struct.showHelp_10 != 1;
				x_D41A0_BYTEARRAY_4_struct.showHelp_10 = x_D41A0_BYTEARRAY_4_struct.showHelp_10 != 1;
				x_D41A0_BYTEARRAY_4_struct.setting_38402 = 1;
			}
			endAction = NewGameDraw_7EAE0(
					&x_DWORD_17DB70str.unk_17DB76_posx,
					&x_DWORD_17DB70str.unk_17DB78_posy,
					&x_DWORD_17DB70str.unk_17DB7E,
					&x_DWORD_17DB70str.unk_17DB80,
					&x_DWORD_17DB70str.x_BYTE_17DB8F,
					&x_DWORD_17DB70str.unk_17DB90);
			if (CommandLineParams.ModeRegressionsTestType() != -1) {
				endAction = 1;
			}
			if (CommandLineParams.ModeRegressionsTestType() == 0) {
				x_DWORD_17DB70str.x_BYTE_17DB8E = 1;
				x_D41A0_BYTEARRAY_4_struct.levelnumber_43w = test_regression_level;
				if (mapScreenPortals_E17CC[test_regression_level].activated_18 == 1)
					x_D41A0_BYTEARRAY_4_struct.setting_38545 |= 4u;
				Type_SecretMapScreenPortals_E2970 *v46x = GetSecretAndActivedPortal_824B0(x_D41A0_BYTEARRAY_4_struct.levelnumber_43w);
				if (v46x && v46x->activated_12 == 2)
					x_D41A0_BYTEARRAY_4_struct.setting_38545 |= 0x10u;
				if (x_D41A0_BYTEARRAY_4_struct.levelnumber_43w == 24)
					x_D41A0_BYTEARRAY_4_struct.setting_38545 |= 0x20u;
				endAction = 1;
			}
			if (x_WORD_180660_VGA_type_resolution & 1)
				sub_90478_VGA_Blit320(menuFps);
			else
				sub_75200_VGA_Blit640(480, menuFps);
			thread2_wait_for_continue(Thread2_State::MAP_MENU_LOOP);
			sub_7A060_get_mouse_and_keyboard_events();
		}
		StopCdPlayback_86860(x_WORD_1803EC);
		D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] = 0;
		EndSample_8D8F0();
		map_not_moving_WORD_E29D6 = false;
		if (endAction == 1) {
			m_ExitMenuLoop_E29DC = 1;
			if (a1x)
				a1x->dword_4 = 0;
		} else if (endAction == 2 && a1x) {
			a1x->dword_4 = 1;
		}
		SetCursor_8CD27((*filearray_2aa18c[filearrayindex_POINTERSDATTAB].posistruct)[0]);
		sub_90B27_VGA_pal_fadein_fadeout(0, 0x10u, 0);
		result = true;
	} else {
		m_ExitMenuLoop_E29DC = 1;
		x_D41A0_BYTEARRAY_4_struct.levelnumber_43w = LoadLevelNumber_D419C;
		if (a1x)
			a1x->dword_4 = 0;
		result = true;
	}
	return result;
}

bool DrawAndServe_pre_sub_7B250_mod(uint32_t var, type_menuButtons_E1F84 *var2x) {
	bool callres = true;
	switch (var) {
		case 0x258350: {
			return NewGameDialog_77350_mod(var2x); //258350 - new game
			break;
		}
		case 0x258980: {
			return ExitDialog_77980(var2x); //exit dialog
			break;
		}
		case 0x2589e0: {
			return LanguageSettingDialog_779E0(var2x);
			break;
		}
		case 0x2590f0: {
			return LoadGameDialog_780F0(var2x); //2590f0 - load
			break;
		}
		case 0x259730: {
			return SaveGameDialog_78730(var2x); //259730 -save
			break;
		}
		case 0x259e00: {
			return SetPlayerNameDialog_78E00(var2x); //set user name
			break;
		}
		case 0x25a160: { //set joystick
			return SetJoystickDialog_79160(var2x);
			break;
		}
		case 0x25a610: { //set keys
			return SetKeysDialog_79610();
			break;
		}
		case 0x25dcf0: {
			return sub_7CCF0(/*var*/);
			break;
		}
		case 0x25dd30: {
			return sub_7CD30(/*var*/);
			break;
		}
		case 0x25dca0: {
			return sub_7CCA0(/*var*/);
			break;
		}
		case 0x25dda0: {
			return sub_7CDA0(/*var*/);
			break;
		}
		case 0x25dbf0: {
			return sub_7CBF0(/*var*/);
			break;
		}
		case 0x25dc40: {
			return sub_7CC40(/*var*/);
			break;
		}
		case 0x25ee80: {
			return MultiplayerMenu_7DE80(var2x);
			break;
		}
	}
	return callres;
}

bool DrawAndServe_7B250_mod() //25c250
{
	typeTextBoxtextBoxStr_E24BCx textBoxStr[2];

	if (CommandLineParams.ModeRegressionsTestType() == 0) {
		str_E1BAC[0].dword_0 = 0x258350;
		str_E1BAC[0].selected_8 = 1;
	}

	if (CommandLineParams.ModeTestNetwork()) {
		if (first_enter) {
			str_E1BAC[2].selected_8 = 1;
		}
	}

	for (int i = 0; str_E1BAC[i].xmin_10; i++) {
		if (str_E1BAC[i].selected_8 && str_E1BAC[i].dword_0) {
			if (DrawAndServe_pre_sub_7B250_mod(str_E1BAC[i].dword_0, &str_E1BAC[i])) {
				str_E1BAC[i].selected_8 = 0;
				ResetMouse_7B5A0();
			}
			if (str_E1BAC[i].dword_4) {
				str_E1BAC[i].selected_8 = 0;
				SetCenterScreenForFlyAssistant_6EDB0();
				sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 4);
				ResetMouse_7B5A0();
				SetCursor_8CD27(xy_DWORD_17DED4_spritestr[39]);
				if (str_E1BAC[i].dword_4 == 2)
					str_E1BAC[i].dword_4 = 0;
				return 1;
			}
			return 0;
		}
	}
	//clear/set off_E1BAC
	int jx;
	for (jx = 0; str_E1BAC[jx].xmin_10; jx++) //clear/set off_E1BAC
	{
		str_E1BAC[jx].selected_8 = 0;
		str_E1BAC[jx].gold_color_24 = 0;
		if (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode == 1 && str_E1BAC[jx].byte_22 == 11)
			str_E1BAC[jx].selected_8 = 1;
	}
	if (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode == 1) {
		x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode = 0;
	} else {
		for (jx = 0; str_E1BAC[jx].xmin_10; jx++) {
			if (InRegion_7B200(&str_E1BAC[jx], x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx, x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony) && str_E1BAC[jx].canSelect_23) {
				if (x_DWORD_17DE38str.x_WORD_17DEEE_mouse_buttons & 1) {
					PlaySample_8F100(0, 14, 127, 64, 0x64u, 0, 3u);
					str_E1BAC[jx].selected_8 = 1;
					ResetMouse_7B5A0();
				} else {
					str_E1BAC[jx].gold_color_24 = 1;
					x_BYTE_17DBC6 = 1;
				}
				break;
			}
		}
	}
	if (x_D41A0_BYTEARRAY_4_struct.showHelp_10 != 1 || !x_DWORD_17DE38str.showHelp_17DF13 || m_ExitMenuLoop_E29DC)
		return 0;
	if (x_BYTE_17DBC6 == 2) {
		times_17DBB8[1] = j___clock();
		if ((times_17DBB8[1] - times_17DBB8[0]) / 0x64u > 1) {
			x_WORD_17DBC4++;
			if (!textBoxStr_E25DC[x_WORD_17DBC4].minx2_2)
				x_WORD_17DBC4 = 0;
			times_17DBB8[0] = times_17DBB8[1];
		}
		memset(textBoxStr, 0, 36);
		textBoxStr[0] = textBoxStr_E25DC[x_WORD_17DBC4];
		int index = 0;
		sub_7E840_draw_textbox_with_line(textBoxStr, 80, 89);
		if (!str_E1BAC[0].xmin_10)
			return 0;
		do {
			if (str_E1BAC[index].byte_22 == str_BYTE_E25ED_0x[x_WORD_17DBC4].byte_0) {
				str_E1BAC[index].gold_color_24 = 1; //turn on gold selection
				return 0;
			}
			index++;
		} while (str_E1BAC[index].xmin_10);
		return false;
	} else {
		if (x_BYTE_17DBC6 == 3) {
			times_17DBB8[1] = j___clock();
			if ((times_17DBB8[1] - times_17DBB8[0]) / 0x64u > 1) {
				x_BYTE_17DBC6 = 2;
				return 0;
			}
			return 0;
		}
		int index2 = 0;
		if (textBoxStr_E25DC[index2].minx2_2) {
			while (textBoxStr_E25DC[index2].byte_17 != str_E1BAC[jx].byte_22) {
				index2++;
				if (!textBoxStr_E25DC[index2].minx2_2)
					return 0;
			}
			memset(textBoxStr, 0, 36);
			textBoxStr[0] = textBoxStr_E25DC[index2];
			sub_7E840_draw_textbox_with_line(textBoxStr, 80, 89);
			int time = j___clock();
			times_17DBB8[1] = time;
			times_17DBB8[0] = time;
			x_BYTE_17DBC6 = 3;
			return false;
		} else {
			return false;
		}
	}
	return false;
}

//----- (00076FA0) --------------------------------------------------------
void MainMenu_76FA0_mod() //257fa0
{
	//fixed
	help_VGA_type_resolution = 0;
	//fixed
	bool onlyBlit = false;
	uint16_t introIndex = 1;

	StopCdPlayback_86860(x_WORD_1803EC); //267860
	LoadSounds_84300(0); //265300
	memset(&times_17DBB8, 0, 16);
	x_BYTE_17DBC6 = 2;
	x_DWORD_17DE38str.savedGameIndex_17DF04 = -1;
	x_DWORD_17DE38str.x_DWORD_17DE44 = x_DWORD_E9C38_smalltit;
	SetCenterScreenForFlyAssistant_6EDB0();
	StopMusic_8E020(); //26f020
	StartMusic_8E160(4, 0x7Fu); //26f160
	x_WORD_17DE26 = 0;
	VGA_cleanKeyBuffer();
	if (x_BYTE_E29E1 || x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 & Setting::MULTIPLAYER_MODE || (NewGameDialog_77350_mod(0), !m_ExitMenuLoop_E29DC)) {
		x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 &= 0xEFu;
		sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 4);
		ResetMouse_7B5A0();

		//fix
		//write_pngs();//only for export
		//fix

		SetCursor_8CD27(xy_DWORD_17DED4_spritestr[39]);
		x_DWORD_17DE38str.showHelp_17DF13 = x_D41A0_BYTEARRAY_4_struct.showHelp_10;
		times_17DBB8[0] = j___clock();
		int lastTime = j___clock();
		int16_t tempMousePosX = x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx;
		int16_t tempMousePosY = x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony;
		int scanCode = x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode;
		while (!m_ExitMenuLoop_E29DC) {
			//g_state_monitor.Update();

			SetFrameStart(std::chrono::system_clock::now());
			if ((tempMousePosX == x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx) && (tempMousePosY == x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony) && (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode == scanCode)) {
				if ((j___clock() - lastTime) / 100 > 60) //after 1 min run intro
				{
					uint8_t *tempSmalltit = x_DWORD_E9C38_smalltit;
					x_DWORD_E9C38_smalltit = x_DWORD_17DE38str.x_DWORD_17DE44;
					PlayIntros_83250(introIndex);
					x_DWORD_E9C38_smalltit = tempSmalltit;
					introIndex = (introIndex == 1) + 1; //alternate 1 and 2
					tempMousePosY = x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony;
					tempMousePosX = x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx;
					x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode = 0;
					scanCode = 0;
					lastTime = j___clock();
					StopMusic_8E020();
					onlyBlit = 0;
					StartMusic_8E160(4, 0x7Fu);
				}
			} else {
				tempMousePosY = x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony;
				tempMousePosX = x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx;
				scanCode = x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode;
				lastTime = j___clock();
			}
			if (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode == 59) {
				x_D41A0_BYTEARRAY_4_struct.showHelp_10 = x_D41A0_BYTEARRAY_4_struct.showHelp_10 != 1;
				x_DWORD_17DE38str.showHelp_17DF13 = x_D41A0_BYTEARRAY_4_struct.showHelp_10;
				x_D41A0_BYTEARRAY_4_struct.setting_38402 = 1;
			}
			if (x_WORD_180660_VGA_type_resolution & 1)
				CopyScreen(x_DWORD_E9C38_smalltit, pdwScreenBuffer_351628, 320, 200);
			else
				CopyScreen(x_DWORD_E9C38_smalltit, pdwScreenBuffer_351628, 640, 480);

			sub_7C120_draw_bitmap_640(185, 232, xy_DWORD_17DED4_spritestr[66]); //adress 25827a
			DrawMenuAnimations_7AB00(); //25bb00
			if (DrawAndServe_7B250_mod()) //25c250
			{
				tempMousePosY = x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony;
				tempMousePosX = x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx;
				onlyBlit = false;
				scanCode = x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode;
				lastTime = j___clock();
			}
			if (onlyBlit) {
				if (x_WORD_180660_VGA_type_resolution & 1)
					sub_90478_VGA_Blit320(menuFps);
				else
					sub_75200_VGA_Blit640(480, menuFps);
				thread2_wait_for_continue(Thread2_State::MAIN_MENU_LOOP);
			} else {
				onlyBlit = true;
				//34ee38 20 0
				sub_90B27_VGA_pal_fadein_fadeout(x_DWORD_17DE38str.palette_17DE38x, 0x20u, 0); //tady
			}
			sub_7A060_get_mouse_and_keyboard_events();
		}
		sub_41BC0();
		D41A0_0.m_GameSettings.m_Display.m_uiScreenSize = 0;
		sub_753D0();
		x_DWORD_E9C38_smalltit = x_DWORD_17DE38str.x_DWORD_17DE44;
	} else {
		sub_41BC0();
		D41A0_0.m_GameSettings.m_Display.m_uiScreenSize = 0;
		sub_753D0();
	}
}

void MenusAndIntros_76930_mod(bool skipMenus) //257930
{
	//1 -351660
	x_BYTE_E29DF_skip_screen = x_BYTE_D41AD_skip_screen;
	if (skipMenus) {
		x_BYTE_D41AD_skip_screen = 1;
		m_ExitMenuLoop_E29DC = 1;
	} else {
		m_ExitMenuLoop_E29DC = 0;
	}

	if (x_BYTE_D41AD_skip_screen == 1 || (nextMenu_E29D8 != MenuItem::InitLanguage)) {
		PlayInGameFmv_82670();
		LoadAndSetGraphicsAndPalette_7AC00();
	}
	if (x_BYTE_D41AD_skip_screen == 1) {
		InitLanguage_76A40();
		nextMenu_E29D8 = MenuItem::MainMenu;
	}
	memset(&x_DWORD_17DE38str, 0, sizeof(type_x_DWORD_17DE38str));
	x_DWORD_17DE38str.x_DWORD_17DEE0_filedesc = NULL;
	sub_7BEC0(); //25CEC0
	SetCenterScreenForFlyAssistant_6EDB0(); //24FDB0
	WriteConfigDat_81DB0(); //262DB0

	//added code!!!!!!!!!!!!!!!!!!!!!!!!
	nextMenu_E29D8 = MenuItem::Intros;
	//Intros_76D10(-1);
	//x_WORD_180660_VGA_type_resolution = 1;
	//LoadAndSetGraphicsAndPalette_7AC00();
	//x_BYTE_E29E1 = 1;
	//added code!!!!!!!!!!!!!!!!!!!!!!!!

	do {
		//2b39d8
		switch (nextMenu_E29D8) {
			case MenuItem::InitLanguage:
				InitLanguage_76A40(); //257A40
				break;
			case MenuItem::SetToIntro:
				SetToIntro_76CF0(); //257cf0 nastavi x_WORD_E29D8 na 3
				break;
			case MenuItem::LeaveX:
				//_wcpp_1_unwind_leave__131(); //257d00 asi konec
				break;
			case MenuItem::Intros:
				Intros_76D10_mod(0); //257d10
				nextMenu_E29D8 = MenuItem::MainMenu;
				break;
			case MenuItem::MainMenu:
				MainMenu_76FA0_mod(); //257fa0
				break;
			case MenuItem::Exit:
				m_ExitMenuLoop_E29DC = 1;
				break;
			case MenuItem::LangSettings:
				LanguageSettingDialog_779E0(0); //2589e0
				break;
			default:
				break;
		}
	} while (!m_ExitMenuLoop_E29DC);
	sub_7ADE0(x_BYTE_E29DE);
	if (x_BYTE_E29E1)
		x_BYTE_E29E1 = 0;
	WriteConfigDat_81DB0();
}

void DrawGameFrame_2BE30_mod() //20CE30
{
	int16_t spellLeftPosX = 510;
	int16_t spellRightPosX = 574;
	uint8_t scale = 1;

	type_entity_0x6E8E *playerEntity;
	void (*drawBitmapFunction)(int16_t, int16_t, bitmap_pos_struct_t, uint8_t scale); // eax

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
						ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 320, 200, uiBackGroundColorIdx_EB3A8);
					else
						ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, screenWidth_18062C, screenHeight_180624, uiBackGroundColorIdx_EB3A8);
				}
			}
			if (D41A0_0.m_GameSettings.m_Graphics.m_wViewPortSize < 40) {
				if (x_WORD_180660_VGA_type_resolution & 1)
					ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 320, 200, uiBackGroundColorIdx_EB3A8);
				else
					ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, screenWidth_18062C, screenHeight_180624, uiBackGroundColorIdx_EB3A8);
			}

			viewPort.SetRenderViewPortSize_40C50(D41A0_0.m_GameSettings.m_Graphics.m_wViewPortSize);
			m_ptrGameRender->DrawWorld_411A0( //draw terrain and particles
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].axis_2BDE_11695.x, //position of player
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].axis_2BDE_11695.y, //position of player
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].struct_0x1d1_2BDE_11695[actPlayerIndex + 1].rotation__2BDE_11701.yaw, //rotation of player z
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
			}
			*/
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
				ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 320, 200, uiBackGroundColorIdx_EB3A8);
			else
				ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, screenWidth_18062C, screenHeight_180624, uiBackGroundColorIdx_EB3A8);

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
			DrawTextPauseEndOfLevel_2CE30(6, 6, scale);
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
	}
	*/
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

void BlendAndBlit_40F80_mod() //221f80
{
	// ── Half-size blending
	if (D41A0_0.m_GameSettings.m_Display.m_uiScreenSize == 2 && !x_BYTE_D478C) {
		const int stride = (uint16_t)iScreenWidth_DE560;
		const int width_dwords = (uint16_t)viewPort.Width_DE564 >> 2;
		const int half_height = (uint16_t)viewPort.Height_DE568 / 2;
		uint8_t *scan = x_DWORD_E9C3C;
		uint8_t *vp = ViewPortRenderBufferStart_DE558;
		for (int row = half_height; row; row--) {
			uint8_t *s = scan;
			uint8_t *d = vp;
			for (int col = width_dwords; col; col--, s += 4, d += 4) {
				*(uint32_t *)d =
						((uint32_t)(x_BYTE_F0520[d[2]] + x_BYTE_F0620[s[2]])) |
						((uint32_t)(x_BYTE_F0920[d[3]] + x_BYTE_F0220[s[3]]) << 8) |
						((uint32_t)(x_BYTE_F0520[d[0]] + x_BYTE_F0620[s[0]]) << 16) |
						((uint32_t)(x_BYTE_F0920[d[1]] + x_BYTE_F0220[s[1]]) << 24);
			}
			s = scan + stride;
			d = vp + stride;
			for (int col = width_dwords; col; col--, s += 4, d += 4) {
				*(uint32_t *)d =
						((uint32_t)(x_BYTE_F0820[d[2]] + x_BYTE_F0320[s[2]])) |
						((uint32_t)(x_BYTE_F0720[d[3]] + x_BYTE_F0420[s[3]]) << 8) |
						((uint32_t)(x_BYTE_F0820[d[0]] + x_BYTE_F0320[s[0]]) << 16) |
						((uint32_t)(x_BYTE_F0720[d[1]] + x_BYTE_F0420[s[1]]) << 24);
			}
			scan += 2 * stride;
			vp += 2 * stride;
		}
	}
	if (D41A0_0.m_GameSettings.m_Display.m_uiScreenSize == 1)
		sub_40D10();
	else if (x_WORD_180660_VGA_type_resolution & 1) {
		if (x_BYTE_D478C)
			sub_BD2CB(unk_F0A20x); //maybe for virtual head set
		else if ((!DefaultResolutions()) && (x_WORD_180660_VGA_type_resolution != 1))
			VGA_BlitAny(maxGameFps);
		else if (x_WORD_180660_VGA_type_resolution & 1)
			sub_90478_VGA_Blit320(maxGameFps);
		else
			sub_75200_VGA_Blit640(480, maxGameFps);
	} else if (D41A0_0.m_GameSettings.str_0x2192.xxxx_0x2193 && D41A0_0.m_GameSettings.m_Display.m_uiScreenSize)
		sub_BD3DD();
	else if (x_BYTE_D478C)
		sub_BD1B6(unk_F0A20x);
	else if ((!DefaultResolutions()) && (x_WORD_180660_VGA_type_resolution != 1))
		VGA_BlitAny(maxGameFps);
	else if (x_WORD_180660_VGA_type_resolution & 1)
		sub_90478_VGA_Blit320(maxGameFps);
	else
		sub_75200_VGA_Blit640(480, maxGameFps);
	thread2_wait_for_continue(Thread2_State::BLEND_AND_BLIT_LOOP);
}

int loc_debug_first_run = 0;
void DrawAndEventsInGame_47560_mod(int16_t turn) //228560
{
	SetFrameStart(std::chrono::system_clock::now());
	/*
	if ((CommandLineParams.ModeRegressionsTestType() != -1) && (count_begin == 1))
		debugcounter_47560++;
	*/
	PaletteChanges_47760();
	if (!(x_D41A0_BYTEARRAY_4_struct.OptionsSettingFlag_24 & 1)) {
		sub_715B0(); //nothing draw //animate sprites
	}
	ReadGameUserInputs_89D10(); //get keys

	if (CommandLineParams.DoAutoChangeRes()) {
		if ((gameResWidth >= 640) && (gameResHeight >= 480)) {
			if (resindex_begin == 1) {
				//Auto press "r" key
				LastPressedKey_1806E4 = 0x13;
				resindex_begin++;
			} else {
				if (resindex_begin == 2) {
					LastPressedKey_1806E4 = 0;
					resindex_begin++;
				} else {
					if (resindex_begin < 1) {
						resindex_begin++;
					}
				}
			}
		}
	}
	MouseAndKeysEvents_17A00(turn);
	//debug
	if (CommandLineParams.DoDebugafterload()) {
		if (loc_debug_first_run == 5) {
			//loadD41A0();
			//x_D41A0_BYTEARRAY_4_struct.langIndex_4 = 1;
			//InitLanguage_76A40_mod_only_language();
			if (LoadLevel_555D0(0u, x_D41A0_BYTEARRAY_4_struct.levelnumber_43w, true))
				sprintf(printbuffer, "%s:%s.", x_DWORD_E9C4C_langindexbuffer[423], "OK"); //Load Level
			else
				sprintf(printbuffer, "%s:%s.", x_DWORD_E9C4C_langindexbuffer[423], x_DWORD_E9C4C_langindexbuffer[429]); //429 - Failed
			ShowMessage_52D70(0, printbuffer);
			x_D41A0_BYTEARRAY_4_struct.OptionsSettingFlag_24 ^= 1;
		}
		if (loc_debug_first_run == 6)
			x_D41A0_BYTEARRAY_4_struct.OptionsSettingFlag_24 ^= 1;
		if (!CommandLineParams.DoRightButton())
			loc_debug_first_run++;
	}
	//debug
	if (CommandLineParams.DoIntervalSave()) {
		//save in interval
		int interval = 1;
		if (save_debugcounter % interval == 0)
			intervalsave(save_debugcounter / interval);
		//save in interval
		save_debugcounter++;
	}
	//adress 228583
	if (CommandLineParams.DoDebugSequences2()) {
		//add_compare(0x228583, CommandLineParams.DoDebugafterload());
	}
	PlayerEvents_51BB0(); //nothing draw
	//adress 228588
	sub_848A0(); //nothing draw
	//adress 22858d
	uint8_t speed = x_D41A0_BYTEARRAY_4_struct.speedIndex;
	if (speed == 0) {
		if (!speed)
			UpdateEntities_57730();
	} else if (speed == 1) {
		for (int i = 0; i < 4; i++)
			UpdateEntities_57730();
	} else if (speed == 2) {
		for (int j = 0; j < 8; j++)
			UpdateEntities_57730();
	}
	sub_84B80(); //prepare lightting
	sub_58F00_game_objectives(); //nothing draw
	PresentObjective_59820(); //nothing draw
	if (!(x_D41A0_BYTEARRAY_4_struct.OptionsSettingFlag_24 & 1))
		sub_57570(); //nothing draw
	sub_575C0(); //nothing draw
	PlayEntitySounds_6E150(); //nothing draw
	DrawGameFrame_2BE30_mod();
	//adress 2285ff
	//add_compare(0x002285FF, CommandLineParams.DoDebugafterload());
	if (CommandLineParams.DoTestRegression()) {
		add_compare(0x002285FF, IsAfterLoad, -1, false, CommandLineParams.GetMaxRegressionsSteps());
		//add_compare(0x002285FF, CommandLineParams.DoDebugafterload(), 6);
	}
	if (CommandLineParams.ModeRegressionsTestType() > 0) {
		add_compare(0x002285FF, IsAfterLoad, -1, false, CommandLineParams.GetMaxRegressionsSteps());
		//add_compare(0x002285FF, IsAfterLoad || (CommandLineParams.GetPlaybackPath().length() > 0), -1, false, CommandLineParams.GetMaxRegressionsSteps(),2140);
		//add_compare(0x002285FF, CommandLineParams.DoDebugafterload(), 6);
	}

	if (CommandLineParams.DoDebugSequences2()) {
		//add_compare(0x002285FF, CommandLineParams.DoDebugafterload());
	}
	if (CommandLineParams.DoDebugSequences()) {
		//add_compare(0x002285FF, CommandLineParams.DoDebugafterload());
	}
	/*if (debugcounter_47560_2>=0x8f)
	{
		debugcounter_47560_2++;
		debugcounter_47560_2--;
	}
	debugcounter_47560_2++;*/
	/*
	if (CommandLineParams.DoAnalyzeEntity()) {
		analyzeEntites();
	}*/
	if (x_D41A0_BYTEARRAY_4_struct.showHelp_10)
		DrawHelpPopUps_871F0();
	//adress 22860f
	if (CommandLineParams.DoDetectDwordA()) {
		for (int iii = 1; iii < 0x3E9; iii++)
			if (Entities_EA3E4[iii]->oldMapEntity_0x16_22 == iii)
				allert_error();
		for (int jx = 1; jx < 50; jx++) {
			if (D41A0_0.str_0x3664C[jx].byte_0)
				if (D41A0_0.str_0x3664C[jx].event_A.pointer_0x6E8E < D41A0_0.struct_0x6E8E || D41A0_0.str_0x3664C[jx].event_A.pointer_0x6E8E >= &D41A0_0.struct_0x6E8E[1000])
					allert_error();
		}
	}
	x_D41A0_BYTEARRAY_4_struct.byteindex_196 = GameTimerTurn_17DB54 - x_D41A0_BYTEARRAY_4_struct.byteindex_196;
	DrawGameDebugText_6FEC0();
	x_D41A0_BYTEARRAY_4_struct.byteindex_196 = GameTimerTurn_17DB54;
	if (x_D41A0_BYTEARRAY_4_struct.paletteMod_51 >= 3u)
		BlendAndBlit_40F80_mod();
}

void InGameLoop_47320_mod() //228320
{
	x_D41A0_BYTEARRAY_4_struct.paletteMod_51 = 0;
	uint32_t gameTurn = 0;
	D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.word[1] = 0;

	//fix res on begin level for hidden levels-neoriginal code
	if (!IsDefaultResolution(gameResWidth, gameResHeight)) {
		VGA_Resize(320, 200);
		screenWidth_18062C = 320;
		screenHeight_180624 = 200;
		sub_A0D50_set_viewport(0, 0, 320, 200);
		x_WORD_180660_VGA_type_resolution = 1;
		resindex_begin = 0;
	}
	//fix res on begin level for hidden levels-neoriginal code

	EventDispatcher::I->DispatchEvent(EventType::E_GAME_STATE_CHANGE, GameState::STARTED);

	while (1) {

		if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].byte_0x004_2BE0_11234 || D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 8)
			break; //end level
		DrawAndEventsInGame_47560_mod(GameTimerTurn_17DB54);
		if (gameTurn < 2) {
			StopMusic_8E020();
			if (gameTurn == 1)
				StartMusic_8E160(D41A0_0.maptypeMusic_0x235, 0x7Fu);
			gameTurn++;
		}

		/*
		// force special settings for renderer tests
		if (CommandLineParams.DoTestRenderers()) {
			// force player turn
			SetMousePositionInMemory_5BDC0(
					renderer_tests[CommandLineParams.GetSetLevel()].set_mouse_x,
					renderer_tests[CommandLineParams.GetSetLevel()].set_mouse_y);

			if (renderer_tests[CommandLineParams.GetSetLevel()].set_flatshader) {
				D41A0_0.m_GameSettings.str_0x2196.flat_0x2199 = 1;
			}

			// force up key pressed
			LastPressedKey_1806E4 = 0x48;
			pressedKeys_180664[x_BYTE_EB39E_keys[0]] = 1;

			if (stop_renderer_tests()) {
				break;
			}
		}
		*/
	}
	EventDispatcher::I->DispatchEvent(EventType::E_GAME_STATE_CHANGE, GameState::GAMEPLAY_ENDED);

	sub_90E07_VGA_set_video_mode_640x480_and_Palette((TColor *)*xadatapald0dat2.colorPalette_var28);
}

void sub_46830_main_loop_mod(unsigned __int16 actLevel) //227830
{ //graphics already inited
	bool isSecretLevel;
	bool skipMenus = false;
	int16_t setLevel = -1;
	std::string customLevelPath = "";

	x_D41A0_BYTEARRAY_4_struct.setting_30 = 0; //2a51a4
	D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].byte_0x004_2BE0_11234 = 0;

	setLevel = CommandLineParams.GetSetLevel();
	customLevelPath = CommandLineParams.GetCustomLevelPath();
	if ((setLevel > -1 || customLevelPath.length() > 0) || CommandLineParams.ModeRegressionsTestType() != -1) {
		if (CommandLineParams.ModeRegressionsTestType() == -1)
			x_D41A0_BYTEARRAY_4_struct.OptionsSettingFlag_24 |= LEVEL_LOADED_FROM_ARG;
		skipMenus = true;
	}

	while (1) {
		if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].byte_0x004_2BE0_11234) {
			return;
		}
		sub_48350(); //fix it //229350

		MenusAndIntros_76930_mod(skipMenus); //set language, intro, menu, atd. //257930

		if (!D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].byte_0x004_2BE0_11234) {
			isSecretLevel = x_D41A0_BYTEARRAY_4_struct.levelnumber_43w > 24 && x_D41A0_BYTEARRAY_4_struct.levelnumber_43w < 50;
			sub_47FC0_load_screen(isSecretLevel); //vga smaltitle
			LevelInitGame_56A30(setLevel, customLevelPath);
			if (CommandLineParams.DoAutoChangeRes()) {
				resindex_begin = 0;
			}

			sub_47160();

			while (!D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].byte_0x004_2BE0_11234) {
				if (musicAble_E37FC && musicActive_E37FD && m_iNumberOfTracks) {
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
				SetCenterScreenForFlyAssistant_6EDB0();
				if (m_ptrGameRender == nullptr) {
					if (!strcmp(forceRender, "NG"))
						m_ptrGameRender = (GameRenderInterface *)new GameRenderNG();
					else if (!strcmp(forceRender, "Original"))
						m_ptrGameRender = (GameRenderInterface *)new GameRenderOriginal();
					else if (!strcmp(forceRender, "HD"))
						m_ptrGameRender = (GameRenderInterface *)new GameRenderHD(pdwScreenBuffer_351628, *xadatapald0dat2.colorPalette_var28, (multiThreadedRender ? numberOfRenderThreads : 0), assignToSpecificCores);
					else {
						if ((gameResWidth <= 640) && (gameResHeight <= 480)) {
							m_ptrGameRender = (GameRenderInterface *)new GameRenderOriginal();
						} else {
							m_ptrGameRender = (GameRenderInterface *)new GameRenderHD(pdwScreenBuffer_351628, *xadatapald0dat2.colorPalette_var28, (multiThreadedRender ? numberOfRenderThreads : 0), assignToSpecificCores);
						}
					}
				}
				InGameLoop_47320_mod();
				if (m_ptrGameRender != nullptr) {
					delete m_ptrGameRender;
					m_ptrGameRender = nullptr;
				}
				sub_53CC0_close_movie();
				EndSample_8D8F0();
				StopMusic_8E020();
				StopCdPlayback_86860(x_WORD_1803EC);
				RestoreSoundVolume_59BF0();
				sub_90B27_VGA_pal_fadein_fadeout(0, 0x10u, 0);
				if (x_WORD_180660_VGA_type_resolution & 1)
					ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 320, 200, getPaletteIndex_5BE80((TColor *)*xadatapald0dat2.colorPalette_var28, 0, 0, 0));
				else
					ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 640, 480, getPaletteIndex_5BE80((TColor *)*xadatapald0dat2.colorPalette_var28, 0, 0, 0));
				if (x_WORD_180660_VGA_type_resolution & 1)
					sub_90478_VGA_Blit320(maxGameFps);
				else
					sub_75200_VGA_Blit640(480, maxGameFps);
				if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 2 && !(x_D41A0_BYTEARRAY_4_struct.setting_38545 & 4)) {
					sub_6DB50(1, 0);
				}
				sub_713A0();
				if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 4) {
					sub_56D60(actLevel, 0);
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] = 4;
				} else if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 2) {
					CollectLevelStats_5C530();
					if (x_D41A0_BYTEARRAY_4_struct.setting_38545 & 0x20)
						sub_6E0D0();
				} else
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] |= 8;
				if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 0x10) {
					actLevel = x_D41A0_BYTEARRAY_4_struct.levelnumber_43w;
					if (actLevel >= 0x18u) {
						if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 2)
							CollectLevelStats_5C530();
						break;
					}
					if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 2) {
						Type_SecretMapScreenPortals_E2970 *secretsPortals = GetSecretAndActivedPortal_824B0(actLevel);
						if (secretsPortals) {
							x_D41A0_BYTEARRAY_4_struct.levelnumber_43w = secretsPortals->levelNumber_6;

							//fix level number-neoriginal code
							actLevel = x_D41A0_BYTEARRAY_4_struct.levelnumber_43w;
							//fix level number-neoriginal code

							//fix res before secret level-neoriginal code
							VGA_Resize(320, 200);
							screenWidth_18062C = 320;
							screenHeight_180624 = 200;
							sub_A0D50_set_viewport(0, 0, 320, 200);
							x_WORD_180660_VGA_type_resolution = 1;
							//fix res before secret level-neoriginal code

							sub_47FC0_load_screen(true);
							LevelInitGame_56A30(actLevel);
							sub_47160();
						}
					}
				} else if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 0xA) {
					break; //must be here
				}
			}
			nextMenu_E29D8 = MenuItem::MainMenu;
			skipMenus = false;
			setLevel = -1;
			customLevelPath = "";
		}
	}
}


int sub_main_mod(int argc, char **argv, char *real_cdPathch) {
	std::function<void(Scene)> sceneChangeCallBack = SetCurrentScene;
	int exitCode = 0;
	SetTimeStart();
	try {
		begin_plugin();
		preconvert(); //rewrite and remove it later
		*xadataclrd0dat.colorPalette_var28 = (uint8_t *)malloc(4096); //fix it
		signed int v3; // edi
		unsigned __int16 v4; // si
		v3 = 0;
		v4 = 0;
		//std::cout << "Initializing logger...\n";
		//spdlog::level::level_enum level = GetLoggingLevelFromString(CommandLineParams.GetLogLevelStr().c_str());
		//InitializeLogging(level);
		EventDispatcher::I = new EventDispatcher();
		EventDispatcher::I->RegisterEvent(new Event<Scene>(EventType::E_SCENE_CHANGE, sceneChangeCallBack));
		EventDispatcher::I->DispatchEvent(EventType::E_GAME_STATE_CHANGE, GameState::STARTED);

		sprintf(gameFolder, "%sGAME/NETHERW", real_cdPathch); //added
		sprintf(cdFolder, "%sCD_Files", real_cdPathch); //added
		windowResWidth = 640; //added
		windowResHeight = 480; //added
		gameResWidth = 640; //added
		gameResHeight = 480; //added

		if (CommandLineParams.DoDisableGraphicsEnhance()) {
			Logger->debug("Disabling enhanced graphics");
			bigSprites = false;
			bigTextures = false;
			texturepixels = 32;
		}
		//Set Paths for game data
		gameDataPath = GetSubDirectoryPath(gameFolder);
		cdDataPath = GetSubDirectoryPath(cdFolder);
		bigGraphicsPath = GetSubDirectoryPath(bigGraphicsFolder);
		VGA_Init(windowResWidth, windowResHeight, gameResWidth, gameResHeight, maintainAspectRatio, displayIndex);
		gamepad_init(gameResWidth, gameResHeight);
		if (std::string mainfile = GetSubDirectoryFile(gameFolder, "CDATA", "TMAPS0-0.DAT"); !file_exists(mainfile.c_str())) //test original file
		{
			/*
			if (std::filesystem::is_directory(gameDataPath)) {
				Logger->info("Original game not found in {0} sub folder ", gameFolder);
				Logger->info("Installing game data from CD_Files...");
			} else {
				Logger->error("Sub folder {0} does not exist!", gameFolder);
				mydelay(5000);
				exit(1); //iso not found
			}*/
		} else {
			//Logger->info("Original Game Data Found!");
		}
		initposistruct();
		sub_56210_process_command_line(argc, argv); //236FD4 - 237210
		if (CommandLineParams.ModeTestNetwork()) {
			if (Iam_server || Iam_client)
				InitNetworkInfo();
		}
		if (CommandLineParams.DoCopySkipConfig()) {
			x_BYTE_D41AD_skip_screen = config_skip_screen;
		}
		if (CommandLineParams.GetPlaybackPath().length() > 0 &&
				std::filesystem::exists(CommandLineParams.GetPlaybackPath().c_str())) {
			StartPlayback(CommandLineParams.GetPlaybackPath().c_str());
		} else if (CommandLineParams.GetRecordingPath().length() > 0) {
			StartRecording(CommandLineParams.GetRecordingPath().c_str());
		}
		Initialize(); //236FDC - 23C8D0//rozdil 1E1000

		thread2_wait_for_continue(Thread2_State::SUB_MAIN_BEFORE_LOOP);

		sub_46830_main_loop_mod(v4); //227830

		if (CommandLineParams.GetPlaybackPath().length() > 0 &&
				std::filesystem::exists(CommandLineParams.GetPlaybackPath().c_str())) {
			StopPlayback();
		} else if (CommandLineParams.GetRecordingPath().length() > 0) {
			StopRecording();
		}

		sub_5BC20(); //23CC20 //remove devices?
		sub_56730_clean_memory(); //237730
		if (CommandLineParams.ModeTestNetwork()) {
			if (Iam_server || Iam_client) {
				EndMyNetLib();
				/*EndLibNetClient();
				if (Iam_server)
					EndLibNetServer();*/
			}
		}
		delete EventDispatcher::I;
	} catch (const thread_exit_exception &e) {
		//Logger->info("Immediate Exit called");
	} catch (const std::exception &e) {
		//Logger->critical("Critical Error: {}", e.what());
		exitCode = -1;
	}
	//Logger->info("Exited Game");

	thread2_wait_for_continue(Thread2_State::SUB_MAIN_END_FUNCTION);

	return exitCode;
}


//fix sub_90B27_VGA_pal_fadein_fadeout(0, 0x10u, 0);
//fix changeLanguage
