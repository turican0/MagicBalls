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

bool NewGameDialog_77350_mod(type_menuButtons_E1F84 *a1x, typeStateMenu newState) //258350
{
	if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPreAnim }) ||
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
		if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPreAnim }) ||
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
		if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPreAnim }) ||
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

bool DrawAndServe_pre_sub_7B250_mod(uint32_t var, type_menuButtons_E1F84 *var2x) {
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

bool DrawAndServe_7B250_mod(typeStateMenu newState) //25c250
{
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

void MainMenu_76FA0_mod(typeStateMenu newState) //257fa0
{
	if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPostAnim }) ||
	    (newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::MainMenuBegin }))
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
	if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPostAnim }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Step }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::End }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::MainMenuBegin }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Step }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::End }))
		if (x_BYTE_E29E1 || x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 & Setting::MULTIPLAYER_MODE || (NewGameDialog_77350_mod(0, newState), !m_ExitMenuLoop_E29DC)) {
			if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPostAnim }) ||
				(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::MainMenuBegin }))
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
				if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPostAnim }) ||
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
							if (newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::AnimFlvBegin })
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
				if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPostAnim }) ||
					(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Step }) ||
					(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::End }) ||
					(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Step }))
				{
					if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPostAnim }) || //added code
						(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Step }) ||//added code
						(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::End }))//added code
					{//added code
						if (NewGameDialog_77350_mod(&str_E1BAC[0], newState))//added code
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
							if (DrawAndServe_7B250_mod(newState)) //25c250
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
			if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPostAnim }) ||
				(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::End }) ||
				(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::MainMenuBegin }) ||
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

void PlayInfoFmv_mod(__int16 allowSkip, __int16 redrawText, Type_SoundEvent_E17CC *pSoundEvent, char *path, typeStateMenu newState) //sub_76160 - 257160
{
	//FILE *tempfile;
	if ((newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::AnimFlvBegin }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::MapMenuBeginPreAnim }))
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
		if ((newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::AnimFlvBegin }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::MapMenuBeginPreAnim }))
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
		if ((newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Step }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::MapMenuBeginStepAnim }))
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
		if ((newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::End }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::MapMenuBeginPostAnim }))
				DataFileIO::Close(x_DWORD_17DB38_intro_file_handle);
	}
}

int globalAnimIndex = -1;

void Intros_76D10_mod(char introType, typeStateMenu newState) //257d10
{
	char introPath[MAX_PATH];
	if (newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::AnimFlvBegin })
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
			if (newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::AnimFlvBegin })
			{
#ifndef debug_hide_graphics
				ShowWelcomeScreen_83850(); //frog logo and wait
#endif
				sprintf(introPath, "%s/%s", cdDataPath.c_str(), "INTRO/INTRO.DAT");
				PlayInfoFmv_mod(1, 1, str_E17CC_0, introPath, newState); //257160 intro .. 2b27cc
				StopSubtitles_2EB40();
				DisplaySubtitles_D41C1 = 0;
				DisplaySubtitles_D41C0 = 0;
				while (sub_9A10A_check_keyboard()) {
					LastPressedKey_1806E4 = 0;
					sub_7A060_get_mouse_and_keyboard_events();
				}
				j___delay(50);
			}
			if ((newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::AnimFlvBegin })||
				(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Step })||
				(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::End }))
			{
				sprintf(introPath, "%s/%s", cdDataPath.c_str(), "INTRO/INTRO2.DAT");
				PlayInfoFmv_mod(1, 1, str_E17CC_0x160, introPath, newState); //E192C
			}
			break;
		case 1:
			if ((newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::AnimFlvBegin })||
				(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Step })||
				(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::End }))
			{
				sprintf(introPath, "%s/%s", cdDataPath.c_str(), "INTRO/INTRO.DAT");
				PlayInfoFmv_mod(1, 1, str_E17CC_0, introPath, newState);
			}
			break;
		case 2:
			if (newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::AnimFlvBegin })
			{
				LastPressedKey_1806E4 = 0;
				DisplaySubtitles_D41C1 = 0;
				DisplaySubtitles_D41C0 = 0;
			}
			if ((newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::AnimFlvBegin })||
				(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Step })||
				(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::End }))
			{
				sprintf(introPath, "%s/%s", cdDataPath.c_str(), "INTRO/INTRO2.DAT");
				PlayInfoFmv_mod(1, 1, str_E17CC_0x160, introPath, newState); //E192C
			}
			break;
	}
	if (newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::End })
	{
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

uint8_t tempTypeResolution_pomMod;
int16_t tempFinalCutSceneIndex = 0;
int tempCutSceneIndex = 0;
char tempCutScenePath[MAX_PATH];

void PlayInGameFmv_82670_mod(typeStateMenu newState) //263670
{
	char dataPath[MAX_PATH];
	if (newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPreAnim })
	{
		tempTypeResolution_pomMod = x_WORD_180660_VGA_type_resolution;
		sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/SCREENS/HSCREEN0.DAT");
		LastPressedKey_1806E4 = 0;
	}
	if (!x_BYTE_E29E1) {
		if (!(x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 & Setting::MULTIPLAYER_MODE)) {
			if (newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPreAnim })
			{
				DisplaySubtitles_D41C1 = 0;
				x_DWORD_17DE38str.x_DWORD_17DE48c = &x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0];
				x_DWORD_17DE38str.x_DWORD_17DE54 = &x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[301787];
				x_DWORD_17DE38str.x_DWORD_17DEC0 = (bitmap_pos_struct2_t *)(&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[308527]);
				x_DWORD_17DE38str.x_DWORD_17DEC4 = (bitmap_pos_struct2_t *)(&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[310159]);
				sub_7AA70_load_and_decompres_dat_file(dataPath, &x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[301787], 0x164FCD, 860);
				sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.x_DWORD_17DEC0, 0x165329, 548);
			}
			if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 2 || x_D41A0_BYTEARRAY_4_struct.levelnumber_43w > 0x18u) {
				if (newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPreAnim })
				{
					tempFinalCutSceneIndex = 0;
					tempCutSceneIndex = 0;
					while (cutScene_E16E0[tempCutSceneIndex].levelNumber_4) {
						if (x_D41A0_BYTEARRAY_4_struct.levelnumber_43w + 1 == cutScene_E16E0[tempCutSceneIndex].levelNumber_4) {
							if (!cutScene_E16E0[tempCutSceneIndex].overplayed_5) {
								tempFinalCutSceneIndex = tempCutSceneIndex + 1;
								cutScene_E16E0[tempCutSceneIndex].overplayed_5 = 1;
							}
							break;
						}
						tempCutSceneIndex++;
					}
					if (!tempFinalCutSceneIndex) {
						uint16_t levelNumber = x_D41A0_BYTEARRAY_4_struct.levelnumber_43w;
						if (levelNumber > 0x18u) {
							Type_SecretMapScreenPortals_E2970 *secretLevel = GetSecretAndActivedPortal2_824E0(levelNumber);
							if (secretLevel) {
								tempCutSceneIndex = 0;
								while (cutScene_E16E0[tempCutSceneIndex].levelNumber_4) {
									if (secretLevel->index_4 + 1 == cutScene_E16E0[tempCutSceneIndex].levelNumber_4) {
										if (!cutScene_E16E0[tempCutSceneIndex].overplayed_5) {
											tempFinalCutSceneIndex = tempCutSceneIndex + 1;
											cutScene_E16E0[tempCutSceneIndex].overplayed_5 = 1;
										}
										break;
									}
									tempCutSceneIndex++;
								}
							}
						}
					}
				}
				if (tempFinalCutSceneIndex) {
					if (newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPreAnim })
					{
						if (x_D41A0_BYTEARRAY_4_struct.SelectedLangIndex == 2 && soundAble_E3798 || tempFinalCutSceneIndex >= 6) {
							DisplaySubtitles_D41C1 = 0;
							DisplaySubtitles_D41C0 = 0;
						} else {
							StopSubtitles_2EB40();
							DisplaySubtitles_D41C1 = 1;
							DisplaySubtitles_D41C0 = 1;
						}
						SetCursor_8CD27((*filearray_2aa18c[filearrayindex_POINTERSDATTAB].posistruct)[0]); //Set cursor to Null (Don't Draw)
						memset((void *)pdwScreenBuffer_351628, 0, 640 * 480);
						if (x_WORD_180660_VGA_type_resolution != 1) {
							sub_54600_mouse_reset();
							memset((void *)*xadatapald0dat2.colorPalette_var28, 0, 768);
							if (x_WORD_180660_VGA_type_resolution & 1)
								ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 320, 200, 0);
							else
								ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 640, 480, 0);
							sub_41A90_VGA_Palette_install((TColor *)*xadatapald0dat2.colorPalette_var28);
							x_WORD_180660_VGA_type_resolution = 1;
							sub_90D6E_VGA_set_video_mode_320x200_and_Palette((TColor *)*xadatapald0dat2.colorPalette_var28);
							sub_8CEDF_install_mouse();
							SetCursor_8CD27((*filearray_2aa18c[filearrayindex_POINTERSDATTAB].posistruct)[0]); //Set cursor to Null (Don't Draw)
						}
						if (x_WORD_180660_VGA_type_resolution & 1)
							sub_98709_create_index_dattab_power(x_DWORD_17DE38str.x_DWORD_17DEC0, x_DWORD_17DE38str.x_DWORD_17DEC4, x_DWORD_17DE38str.x_DWORD_17DE54, xy_DWORD_17DEC0_spritestr);
						else
							sub_9874D_create_index_dattab(x_DWORD_17DE38str.x_DWORD_17DEC0, x_DWORD_17DE38str.x_DWORD_17DEC4, x_DWORD_17DE38str.x_DWORD_17DE54, xy_DWORD_17DEC0_spritestr);
						sprintf(tempCutScenePath, "%s/INTRO/CUT%d.DAT", cdDataPath.c_str(), cutScene_E16E0[tempCutSceneIndex].fileIndex_6);
						sprintf(printbuffer, "%s", tempCutScenePath);
					}

					if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPreAnim }) ||
						(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginStepAnim }) ||
						(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPostAnim }))
						PlayInfoFmv_mod(0, 1, cutScene_E16E0[tempCutSceneIndex].pSoundEvent_0, tempCutScenePath, newState);

					if (newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPostAnim }) {
						sub_90B27_VGA_pal_fadein_fadeout(0, 0x10u, 0);
						EndSample_8D8F0();
						StopMusic_8E020();
						FadeClearBlit_7B5D0();
						if (x_WORD_180660_VGA_type_resolution & 1)
							ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 320, 200, 0);
						else
							ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 640, 480, 0);

						if (x_WORD_180660_VGA_type_resolution & 1)
							sub_90478_VGA_Blit320();
						else
							sub_75200_VGA_Blit640(480);
						if (tempTypeResolution_pomMod != x_WORD_180660_VGA_type_resolution) {
							sub_54600_mouse_reset();
							memset((void *)*xadatapald0dat2.colorPalette_var28, 0, 768);
							if (x_WORD_180660_VGA_type_resolution & 1)
								ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 320, 200, 0);
							else
								ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 640, 480, 0);
							sub_41A90_VGA_Palette_install((TColor *)*xadatapald0dat2.colorPalette_var28);
							x_WORD_180660_VGA_type_resolution = tempTypeResolution_pomMod;
							if (x_WORD_180660_VGA_type_resolution & 1)
								sub_90D6E_VGA_set_video_mode_320x200_and_Palette((TColor *)*xadatapald0dat2.colorPalette_var28);
							else
								sub_90E07_VGA_set_video_mode_640x480_and_Palette((TColor *)*xadatapald0dat2.colorPalette_var28);
							sub_8CEDF_install_mouse();
							SetCursor_8CD27((*filearray_2aa18c[filearrayindex_POINTERSDATTAB].posistruct)[0]); //Set cursor to Null (Don't Draw)
						}
					}
				}
			}
		}
	}
	if (newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPostAnim })
		DisplaySubtitles_D41C1 = 0;
}

void MenusAndIntros_76930_mod(bool skipMenus, typeStateMenu newState) //257930
{
	int animIndex = 0;
	if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPreAnim }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginStepAnim }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPostAnim }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::MainMenuBegin }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::AnimFlvBegin }))
	{
		if (newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::AnimFlvBegin })
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
			PlayInGameFmv_82670_mod(newState);
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
		if (((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPostAnim }) ||
		    (newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::MainMenuBegin }))
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
	if ((newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::AnimFlvBegin }) ||
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
				Intros_76D10_mod(animIndex, newState); //257d10
				if (newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::End })
					nextMenu_E29D8 = MenuItem::MainMenu;
				break;
			case MenuItem::MainMenu:
				MainMenu_76FA0_mod(newState); //257fa0
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

void DrawAndEventsInGame_47560_mod(int16_t turn) //228560
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

void InGameLoop_47320_mod(typeStateMenu newState) //228320
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
			DrawAndEventsInGame_47560_mod(GameTimerTurn_17DB54);
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

void sub_46830_main_loop_mod(unsigned __int16 actLevel, typeStateMenu newState) //227830
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

	if (((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPreAnim }) ||		
		(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::MainMenuBegin }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::InGameBegin }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::AnimFlvBegin }))
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
	if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPreAnim }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Step }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::End }) ||		
		(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::MainMenuBegin }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Step }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::End }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::InGameBegin }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::BeginAfterScreen }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::BeginAfterSecret }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::Step }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::End }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::EndPostSecretScreen }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::AnimFlvBegin }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Step }) ||
		(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::End }))
	{
		if (((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPreAnim }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::MainMenuBegin }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::InGameBegin }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::AnimFlvBegin }))
				/*
				&& actState == typeStateMenu2::Zero*/)
		{
			if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].byte_0x004_2BE0_11234) {
				return;
			}
			sub_48350(); //fix it //229350
		}
		if ((newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::MapMenuBeginPreAnim }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::Step }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::MapMenu, typeStateMenu::State::End }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::MainMenuBegin }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::Step }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::MainMenu, typeStateMenu::State::End }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::AnimFlvBegin }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::Step }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::AnimFlv, typeStateMenu::State::End }))
				MenusAndIntros_76930_mod(main_loop_skipMenus, newState); //set language, intro, menu, atd. //257930
		if ((newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::InGameBegin }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::BeginAfterScreen }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::BeginAfterSecret }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::Step }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::End }) ||
			(newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::EndPostSecretScreen }))
		if (!D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].byte_0x004_2BE0_11234) {
			if (newState == typeStateMenu{ typeStateMenu::Name::InGame, typeStateMenu::State::InGameBegin })
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

						InGameLoop_47320_mod(newState);
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
