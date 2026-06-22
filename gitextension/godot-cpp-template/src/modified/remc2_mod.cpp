#include "remc2_mod.h"

#include <godot_cpp/variant/utility_functions.hpp>

//int NewGameDialog_endAction_mod;

int graphics_enhance = 0;

int game_paused = 0;
bool oneFrameRun = false;

void InitLanguage_76A40_mod_only_language() //257A40
{	
	FILE *langfile;
	uint32_t filelenght;
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
				
	}
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

//typeStateMenu2 actState = typeStateMenu2::Zero;

//bool main_loop_isSecretLevel;
//bool main_loop_skipMenus;
//int16_t main_loop_setLevel;
//std::string main_loop_customLevelPath;

//bool MainMenu_onlyBlit;
//uint16_t MainMenu_introIndex;

//int MainMenu_lastTime;
//int16_t MainMenu_tempMousePosX;
//int16_t MainMenu_tempMousePosY;
//int MainMenu_scanCode;

//bool NewGameDialog_result;

//int NewGameDialog_endAction;

//typeTextBoxtextBoxStr_E24BCx DrawAndServe_textBoxStr[2];
//FILE *tempfile_PlayInfoFmv;
//bool PlayInfoFmv_break=false;
//int globalAnimIndex = -1;
//bool InGameLoop_47320_break = false;
//uint32_t InGameLoop_47320_mod_gameTurn = 0;

//bool setLoadScreen = false;

//Type_SecretMapScreenPortals_E2970* secretsModPortals;

//bool sub_46830_main_loop_break = false;

//unsigned __int16 tempActLevel=-1;

std::mutex main_mutex;
std::condition_variable main_cv;
bool thread1_turn = false;

Thread1_State thread1_state = Thread1_State::BEGIN;
Thread2_State thread2_state = Thread2_State::BEGIN;

bool thread1_waiting = false;
bool thread2_waiting = false;

std::atomic<bool> thread2_quit_requested{ false };

void thread2_wait_for_continue(Thread2_State sendstate) {
	{
		std::lock_guard<std::mutex> lock(main_mutex);
		thread2_state = sendstate;
		thread2_waiting = true;
		thread1_waiting = false;
	}
	main_cv.notify_all();

	std::unique_lock<std::mutex> lock(main_mutex);
	main_cv.wait(lock, [] {
		return !thread2_waiting || thread2_quit_requested.load();
	});

	// If we were woken by a quit signal, throw so sub_main_mod unwinds.
	if (thread2_quit_requested.load()) {
		throw thread_exit_exception();
	}
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

int16_t sub_90B27_VGA_pal_fadein_fadeout_mod(TColor *newpalbufferx, uint8_t shadow_levels, bool singlestep, int32_t frameDelay=10) //271B27 init and nightfall
{
	TColor outbufferx[256]; // [esp+0h] [ebp-30Ch]
	uint16_t i; // [esp+300h] [ebp-Ch]
	TColor zero_bufferx[256];
	VGA_Init(windowResWidth, windowResHeight, gameResWidth, gameResHeight, maintainAspectRatio, displayIndex);
	bool fadeout = false;
	bool samePal = false;
	if (singlestep) {
		if (x_BYTE_E390C_VGA_pal_not_begin) {
			x_WORD_181B44++;
			if (shadow_levels == x_WORD_181B44)
				x_BYTE_E390C_VGA_pal_not_begin = 0;
		} else {
			x_WORD_181B44 = 0;
			x_BYTE_E390C_VGA_pal_not_begin = 1;
			sub_A0D2C_VGA_get_Palette(x_BYTE_181544_oldpalbufferx);
			if (!newpalbufferx)
				memset(zero_bufferx, 0, 0x300);
		}
		if (!newpalbufferx)
		{
			newpalbufferx = zero_bufferx;
			//fadeout = true;
		}
		for (i = 0; i < 0x100; i++) {
			outbufferx[i].red = x_BYTE_181544_oldpalbufferx[i].red + ((x_WORD_181B44) * (newpalbufferx[i].red - x_BYTE_181544_oldpalbufferx[i].red) / shadow_levels);
			outbufferx[i].green = x_BYTE_181544_oldpalbufferx[i].green + ((x_WORD_181B44) * (newpalbufferx[i].green - x_BYTE_181544_oldpalbufferx[i].green) / shadow_levels);
			outbufferx[i].blue = x_BYTE_181544_oldpalbufferx[i].blue + ((x_WORD_181B44) * (newpalbufferx[i].blue - x_BYTE_181544_oldpalbufferx[i].blue) / shadow_levels);
		}
		sub_41A90_VGA_Palette_install(outbufferx);
		fix_sub_9A0FC_wait_to_screen_beam(frameDelay);

		//compute darker palette
		int old_sum = 0;
		int new_sum = 0;
		for (int i = 0; i < 0x100; i++) {
			old_sum += x_BYTE_181544_oldpalbufferx[i].red;
			old_sum += x_BYTE_181544_oldpalbufferx[i].green;
			old_sum += x_BYTE_181544_oldpalbufferx[i].blue;
			new_sum += newpalbufferx[i].red;
			new_sum += newpalbufferx[i].green;
			new_sum += newpalbufferx[i].blue;
		}
		if (old_sum < 0x400)
			old_sum = 0;
		if (new_sum < 0x400)
			new_sum = 0;
		if (old_sum > 0x5000)
			old_sum = 0x5000;
		if (new_sum < 0x5000)
			new_sum = 0x5000;
		fadeout = (new_sum < old_sum);
		samePal = new_sum == old_sum;
		//compute darker palette

		if (samePal) {
			if (new_sum == 0)
				MBChangePalette(10);
			 else
				MBChangePalette(0);
		} else {
			if (fadeout)
				MBChangePalette(12, shadow_levels);
			else
				MBChangePalette(11, shadow_levels);
		}

		thread2_wait_for_continue(Thread2_State::FADEIN_FADEOUT_LOOP);
	} else {
		sub_A0D2C_VGA_get_Palette(x_BYTE_181544_oldpalbufferx);
		if (!newpalbufferx) //ebp+14 - 355204
		{
			newpalbufferx = zero_bufferx;
			memset(zero_bufferx, 0, 768);
		}

		//compute darker palette
		int old_sum = 0;
		int new_sum = 0;
		for (int i = 0; i < 0x100; i++) {
			old_sum += x_BYTE_181544_oldpalbufferx[i].red;
			old_sum += x_BYTE_181544_oldpalbufferx[i].green;
			old_sum += x_BYTE_181544_oldpalbufferx[i].blue;
			new_sum += newpalbufferx[i].red;
			new_sum += newpalbufferx[i].green;
			new_sum += newpalbufferx[i].blue;
		}
		if (old_sum < 0x400)
			old_sum = 0;
		if (new_sum < 0x400)
			new_sum = 0;
		fadeout = (new_sum < old_sum);
		samePal = new_sum == old_sum;
		//compute darker palette

		for (x_WORD_181B44 = 0; x_WORD_181B44 < shadow_levels; x_WORD_181B44++) {
			for (i = 0; i < 0x100; i++) {
				outbufferx[i].red = x_BYTE_181544_oldpalbufferx[i].red + ((x_WORD_181B44) * (newpalbufferx[i].red - x_BYTE_181544_oldpalbufferx[i].red) / shadow_levels); //352b42 352544
				outbufferx[i].green = x_BYTE_181544_oldpalbufferx[i].green + ((x_WORD_181B44) * (newpalbufferx[i].green - x_BYTE_181544_oldpalbufferx[i].green) / shadow_levels); //352b42 352544
				outbufferx[i].blue = x_BYTE_181544_oldpalbufferx[i].blue + ((x_WORD_181B44) * (newpalbufferx[i].blue - x_BYTE_181544_oldpalbufferx[i].blue) / shadow_levels); //352b42 352544
			}
			sub_41A90_VGA_Palette_install(outbufferx);
			fix_sub_9A0FC_wait_to_screen_beam(frameDelay);

			if (samePal) {
				if (new_sum == 0)
					MBChangePalette(10);
				else
					MBChangePalette(0);
			} else {
				if (fadeout)
					MBChangePalette(9, shadow_levels);
				else
					MBChangePalette(1, shadow_levels);
			}

			thread2_wait_for_continue(Thread2_State::FADEIN_FADEOUT_LOOP);
		}
		x_BYTE_E390C_VGA_pal_not_begin = 0;
	}
	return x_WORD_181B44;
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
		} else {
			afterFade = true;
			ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 640, 480, 0);
			sub_75200_VGA_Blit640(480, menuFps);
			sub_90B27_VGA_pal_fadein_fadeout_mod((TColor *)*xadatapald0dat2.colorPalette_var28, 0x20u, 0);
		}
		thread2_wait_for_continue(Thread2_State::SHOW_WELCOME_SCREEN_LOOP);
	}
	sub_90B27_VGA_pal_fadein_fadeout_mod(0, 0x10u, 0);
	if (x_WORD_180660_VGA_type_resolution & 1)
		ClearGraphicsBuffer_72883(pdwScreenBuffer_351628, 320, 200, 0);
	else
		ClearGraphicsBuffer_72883(pdwScreenBuffer_351628, 640, 480, 0);
	if (x_WORD_180660_VGA_type_resolution & 1)
		sub_90478_VGA_Blit320(menuFps);
	else
		sub_75200_VGA_Blit640(480, menuFps);
	while (sub_9A10A_check_keyboard()) {
		LastPressedKey_1806E4 = 0;
		sub_7A060_get_mouse_and_keyboard_events();
		thread2_wait_for_continue(Thread2_State::SHOW_WELCOME_SCREEN_LOOP);
	}
	j___delay(50);
}

int numberOfIntroVideos = 3;

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
		thread2_wait_for_continue(Thread2_State::INTRO_BEGIN);
		do {
			SetFrameStart(std::chrono::system_clock::now());
			if (stopPlaybackFlag_17DB5A)
				break;
			if (ActualKeyframe_17DB60 >= LastKeyframe_17DB46 - 1) //34eb60 a 34eb46
				break;
			PlayIntoSoundEvents_1B280(pSoundEvent);
			ReadFrame_75DB0(); //256db0 - read header
			DrawFrame_75E70(); //256e70 - draw intro frame			
			ActualKeyframe_17DB60++;
			thread2_wait_for_continue(Thread2_State::PLAY_INFO_FLV_LOOP);
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
			numberOfIntroVideos = -1;
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
			numberOfIntroVideos = -1;
			PlayInfoFmv_mod(1, 1, str_E17CC_0x160, introPath); //E192C
			break;
		case 1:
			sprintf(introPath, "%s/%s", cdDataPath.c_str(), "INTRO/INTRO.DAT");
			numberOfIntroVideos = -1;
			PlayInfoFmv_mod(1, 1, str_E17CC_0, introPath);
			break;
		case 2:
			LastPressedKey_1806E4 = 0;
			DisplaySubtitles_D41C1 = 0;
			DisplaySubtitles_D41C0 = 0;
			sprintf(introPath, "%s/%s", cdDataPath.c_str(), "INTRO/INTRO2.DAT");
			numberOfIntroVideos = -1;
			PlayInfoFmv_mod(1, 1, str_E17CC_0x160, introPath); //E192C
			break;
	}
	sub_90B27_VGA_pal_fadein_fadeout_mod(0, 0x10u, 0);
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
		sub_90B27_VGA_pal_fadein_fadeout_mod(0, 0x10u, 0);
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
		MBChangePalette(0);
		PortalsUpdate_7DD70();
		x_DWORD_17DE38str.x_WORD_17DEEC = 0;
		SetCenterScreenForFlyAssistant_6EDB0();
		SetCursor_8CD27(xy_DWORD_17DED4_spritestr[239]);
		x_DWORD_17DB70str.x_WORD_17DB8A = x_D41A0_BYTEARRAY_4_struct.levelnumber_43w;
		thread2_wait_for_continue(Thread2_State::MAP_MENU_BEGIN);
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
			sub_7A060_get_mouse_and_keyboard_events();
			thread2_wait_for_continue(Thread2_State::MAP_MENU_LOOP);
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

#ifdef DEBUG_PALETTE
		//debug palette log
		writePalLog("NewGameDialog_77350_mod/sub_90B27_VGA_pal_fadein_fadeout_mod(0, 0x10u, 0)");
		//debug palette log
#endif //DEBUG_PALETTE

		sub_90B27_VGA_pal_fadein_fadeout_mod(0, 0x10u, 0);
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

char LanguageSettingDialog_779E0_mod(type_menuButtons_E1F84 *a1y) {
	thread2_wait_for_continue(Thread2_State::LANGUAGE_SETTING_CLICKED);
	return true;
}

bool LoadGameDialog_780F0_mod(type_menuButtons_E1F84 *menuButtons) //0x2590f0
{
	char *save_name;
	FILE *SEARCH_FILE;
	FILE *FILE;
	type_menuButtons_E1F84 menuButtons2;
	int numLevelsCompleted = 0;
	uint32_t signature;
	int32_t some_var = 0;
	bool result = false;
	int drawScrollResult = 0;

	uint8_t pal_selected_text = getPaletteIndex_5BE80(x_DWORD_17DE38str.palette_17DE38x, 0x3Fu, 0x3Fu, 0x3Fu);
	uint8_t pal_text = getPaletteIndex_5BE80(x_DWORD_17DE38str.palette_17DE38x, 0x16u, 0x10u, 9u);
	if (x_DWORD_17DE38str.savedGameIndex_17DF04 == -1) {
		for (int index = 0; index < 8; index++) {
			save_name = &x_DWORD_17DE38str.xx_BYTE_17DF14[index][0];
			strcpy(save_name, x_DWORD_E9C4C_langindexbuffer[414]);
			x_DWORD_17DE38str.xx_BYTE_17DF14[index][41] = 0;
			x_DWORD_17DE38str.xx_BYTE_17DF14[index][42] = 0;
			std::string saveGameFilePath = GetSaveGameFile(gameFolder.c_str(), index + 1);
			SEARCH_FILE = DataFileIO::CreateOrOpenFile(saveGameFilePath.c_str(), 512);
			if (SEARCH_FILE != NULL) {
				DataFileIO::Read(SEARCH_FILE, (uint8_t *)&signature, 4);
				if (signature == 0xFFFFFFF7u) {
					DataFileIO::Read(SEARCH_FILE, (uint8_t *)save_name, 20);
					x_DWORD_17DE38str.xx_BYTE_17DF14[index][41] = 1;
				}
				DataFileIO::Close(SEARCH_FILE);
			}
		}
		x_DWORD_17DE38str.savedGameIndex_17DF04 = 0;
	}
	drawScrollResult = DrawScrollDialog_7BF20(&menuButtons->str_26);
	if (drawScrollResult) {
		ClearScrollDialogVars_7C020(&menuButtons->str_26);
		if (drawScrollResult == 1 && x_DWORD_17DE38str.savedGameIndex_17DF04 > 0) {
			//Load Saved Game File
			std::string loadFilePath = GetSaveGameFile(gameFolder.c_str(), x_DWORD_17DE38str.savedGameIndex_17DF04);
			FILE = DataFileIO::CreateOrOpenFile(loadFilePath.c_str(), 512);
			if (FILE != NULL) {
				DataFileIO::Read(FILE, (uint8_t *)&signature, 4);
				if (signature == 0xFFFFFFF7u) {
					if (menuButtons->byte_25)
						sub_7E640(0);
					DataFileIO::Read(FILE, (uint8_t *)&x_DWORD_17DE38str.xx_BYTE_17DF14[(x_DWORD_17DE38str.savedGameIndex_17DF04 - 1)][0], 20);
					DataFileIO::Read(FILE, (uint8_t *)x_D41A0_BYTEARRAY_4_struct.player_name_57ar, 32);
					DataFileIO::Read(FILE, (uint8_t *)x_D41A0_BYTEARRAY_4_struct.savestring_89, 32);

					//Load completed Secret Portals
					for (int ii = 0; ii < 6; ii++) {
						DataFileIO::Read(FILE, readbuffer, 17);
						secretMapScreenPortals_E2970[ii].activated_12 = *(uint16_t *)(readbuffer + 12);
						if (secretMapScreenPortals_E2970[ii].activated_12 == 1)
							secretMapScreenPortals_E2970[ii].spriteIndex_14 = 305;
						else
							secretMapScreenPortals_E2970[ii].spriteIndex_14 = 270;
					}
					DataFileIO::Read(FILE, (uint8_t *)&D41A0_0.m_GameSettings, 16);
					DataFileIO::Read(FILE, (uint8_t *)&numLevelsCompleted, 4);
					DataFileIO::Read(FILE, (uint8_t *)&some_var, 4);
					DataFileIO::Read(FILE, (uint8_t *)&D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dword_0x3E6_2BE4_12228.str_611, 505);
					DataFileIO::Read(FILE, (uint8_t *)x_DWORD_17DBC8x, 500);
					DataFileIO::Read(FILE, (uint8_t *)x_DWORD_17DDBCx, 100);
					DataFileIO::Close(FILE);
					D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.word[1] = 0;

					int i = 0;
					//Reset all Portals to inactive
					while (mapScreenPortals_E17CC[i].viewPortPosX_4) {
						mapScreenPortals_E17CC[i].activated_18 = 2;
						i++;
					}

					i = 0;
					//Load completed Portals
					while (i < numLevelsCompleted && mapScreenPortals_E17CC[i].viewPortPosX_4) {
						mapScreenPortals_E17CC[i].activated_18 = 1;
						i++;
					}

					i = 0;
					//Set current level number
					while (mapScreenPortals_E17CC[i].viewPortPosX_4) {
						if (mapScreenPortals_E17CC[i].activated_18 == 1)
							x_D41A0_BYTEARRAY_4_struct.levelnumber_43w = i;
						i++;
					}
					x_DWORD_17DB70str.x_BYTE_17DB8F = 1;
					memset(&x_DWORD_17DE28str, 0, 13);
					x_DWORD_17DB70str.x_WORD_17DB8A = -1;
					if (menuButtons->byte_25) {
						MapMenuPortalsDraw_81760();
					} else {
						x_DWORD_17DE38str.savedGameIndex_17DF04 = -1;
						NewGameDialog_77350_mod(menuButtons);
						menuButtons->dword_4 = 2;
					}
				}
			}
		}
		x_DWORD_17DE38str.savedGameIndex_17DF04 = -1;
		result = true;
	}

	for (int jm = 0; jm < 8; jm++) {
		int j = jm + 1;
		GetFont_6FC50(1);
		if (menuButtons->str_26.word_36_5 > 16 * (signed __int16)j + 3 * GetLetterHeight_6FC30()) {
			std::string savegame = std::to_string(j) + ". " + std::string(&x_DWORD_17DE38str.xx_BYTE_17DF14[(j - 1)][0]);
			int16_t savegame_y_pos = menuButtons->str_26.y1_28_1 + 16 * (j + 1);
			int16_t savegame_x_pos = menuButtons->str_26.x1_26_0 + 20;
			uint8_t pal_text_color = (j == x_DWORD_17DE38str.savedGameIndex_17DF04) ? pal_selected_text : pal_text;
			DrawText_2BC10(savegame.c_str(), savegame_x_pos, savegame_y_pos, pal_text_color);
		}
	}
	if (menuButtons->str_26.word_36_5 >= menuButtons->str_26.word_34_4) {
		for (int km = 0; km < 8; km++) {
			int k = km + 1;
			menuButtons2.xmin_10 = menuButtons->str_26.x1_26_0 + 20;
			menuButtons2.ymin_12 = 16 * k + menuButtons->str_26.y1_28_1 + 16;
			menuButtons2.sizex_14 = 100;
			menuButtons2.sizey_16 = 16;
			if (x_DWORD_17DE38str.xx_BYTE_17DF14[(k - 1)][41] && InRegion_7B200(&menuButtons2, x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx, x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony)) {
				if (x_DWORD_17DE38str.x_WORD_17DEEE_mouse_buttons & 1) {
					x_DWORD_17DE38str.savedGameIndex_17DF04 = k;
				} else {
					std::string savegame = std::to_string(k) + ". " + std::string(&x_DWORD_17DE38str.xx_BYTE_17DF14[(k - 1)][0]);
					int16_t savegame_y_pos = menuButtons->str_26.y1_28_1 + 16 * (k + 1);
					int16_t savegame_x_pos = menuButtons->str_26.x1_26_0 + 20;
					DrawText_2BC10(savegame.c_str(), savegame_x_pos, savegame_y_pos, pal_selected_text);
				}
			}
		}
	}
	return result;
}

char SetKeysDialog_79610_mod() //25a610
{
	uint8_t *temp_screen_buffer;
	char textBuff[60];
	int allKeysDone = 0;
	int v39 = 359;
	int keyNameX = 372;
	int buttonAreaWidth = 410;
	int exitFlag = 0;
	int fadeInDone = 0;
	int leftEdge = 191;
	int spriteHeightCopy = xy_DWORD_17DED4_spritestr[107].height_5;
	int clock3 = j___clock();
	int clock2 = clock3;
	memset(textBuff, 0, 60);
	sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 15);
	temp_screen_buffer = pdwScreenBuffer_351628;
	pdwScreenBuffer_351628 = x_DWORD_E9C38_smalltit;
	for (int v2_int = 0; str_BYTE_E25ED_2BB[v2_int].word_0; v2_int++)
		sub_7FCB0_draw_text_with_border(x_DWORD_E9C4C_langindexbuffer[str_BYTE_E25ED_2BB[v2_int].word_12], str_BYTE_E25ED_2BB[v2_int].word_0, v39, str_BYTE_E25ED_2BB[v2_int].word_2, 4, 0, 0);
	pdwScreenBuffer_351628 = temp_screen_buffer;
	ResetMouse_7B5A0();
	for (int v2_int = 0; str_BYTE_E25ED_2BB[v2_int].word_0; v2_int++)
		str_BYTE_E25ED_2BB[v2_int].word_14 = 0;
	str_BYTE_E25ED_2BB[0].word_14 = 1;

	SetCursor_8CD27(xy_DWORD_17DED4_spritestr[110]);
	while (exitFlag != 2) {
		SetFrameStart(std::chrono::system_clock::now());
		int clockNow = j___clock();
		if (x_WORD_180660_VGA_type_resolution & 1)
			CopyScreen((void *)x_DWORD_E9C38_smalltit, (void *)pdwScreenBuffer_351628, 320, 200);
		else
			CopyScreen((void *)x_DWORD_E9C38_smalltit, (void *)pdwScreenBuffer_351628, 640, 480);
		//backgroung
		if (x_DWORD_17DE38str.x_WORD_17DEEE_mouse_buttons & 1) //switch blocks
		{
			for (int j_int = 0; str_BYTE_E25ED_2BB[j_int].word_0; j_int++) {
				type_menuButtons_E1F84 box;
				box.xmin_10 = leftEdge;
				box.ymin_12 = str_BYTE_E25ED_2BB[j_int].word_2 - 4;
				box.sizex_14 = buttonAreaWidth;
				box.sizey_16 = spriteHeightCopy;
				if (InRegion_7B200(&box, x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx, x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony)) {
					for (int k_int = 0; str_BYTE_E25ED_2BB[k_int].word_0; k_int++)
						str_BYTE_E25ED_2BB[k_int].word_14 = 0;
					str_BYTE_E25ED_2BB[0].word_14 = 1;
					allKeysDone = 0;
					break;
				}
			}
		}
		exitFlag = TestMouseRegions_7E1F0();
		if (exitFlag == 1) {
			x_D41A0_BYTEARRAY_4_struct.setting_38402 = 1;
			sub_5BCC0_set_any_variables1();
			for (int l_int = 0; str_BYTE_E25ED_2BB[l_int].word_0; l_int++)
				str_BYTE_E25ED_2BB[l_int].word_14 = 0;
			str_BYTE_E25ED_2BB[0].word_14 = 1;
		}
		uint8_t *keyIter = &x_BYTE_EB39E_keys[0]; //2bc39e
		int v14_int = 0;
		while (keyIter < &x_BYTE_EB39E_keys[9] + 1) //drawing texts
		{
			memset(textBuff, 0, 60);
			if (!str_BYTE_E25ED_2BB[v14_int].word_14) {
				sub_79E10(textBuff, *keyIter);
				sub_7FCB0_draw_text_with_border(textBuff, keyNameX, buttonAreaWidth, str_BYTE_E25ED_2BB[v14_int].word_2, 4, 0, 0);
			}
			keyIter++;
			v14_int++;
		}
		int v2_int = 0;
		for (uint8_t *keyIter2 = &x_BYTE_EB39E_keys[0]; keyIter2 < &x_BYTE_EB39E_keys[9] + 1 && !allKeysDone; ++keyIter2) {
			int keySlotState = str_BYTE_E25ED_2BB[v2_int].word_14;
			if (keySlotState >= 1u) {
				unsigned int elapsedTime = clockNow - clock3;
				if (keySlotState <= 1u) {
					memset(textBuff, 0, 60);
					sub_79E10(textBuff, *keyIter2);
					sub_7FCB0_draw_text_with_border(textBuff, keyNameX, buttonAreaWidth, str_BYTE_E25ED_2BB[v2_int].word_2, 4, 0, 0);
					if (elapsedTime > 0x32) {
						str_BYTE_E25ED_2BB[v2_int].word_14 = 2;
						clock3 = clockNow;
					}
					DrawBitmap_2BB40(leftEdge, str_BYTE_E25ED_2BB[v2_int].word_2, xy_DWORD_17DED4_spritestr[107]);
					if (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode && sub_79E10(textBuff, x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode)) {
						bool isScancode = false;
						if (keyIter2 != &x_BYTE_EB39E_keys[6]) {
							for (uint8_t *keyIter3 = &x_BYTE_EB39E_keys[0]; keyIter3 < &x_BYTE_EB39E_keys[9] + 1; keyIter3++) {
								if (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode == *keyIter3) {
									isScancode = true;
									break;
								}
							}
						}
						if (!isScancode) {
							x_D41A0_BYTEARRAY_4_struct.setting_38402 = 1;
							str_BYTE_E25ED_2BB[v2_int].word_14 = 0;
							v2_int++;
							*keyIter2 = x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode;
							x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode = 0;
							if (str_BYTE_E25ED_2BB[v2_int].word_0) {
								str_BYTE_E25ED_2BB[v2_int].word_14 = 1;
							} else {
								allKeysDone = 1;
								clock3 = clockNow;
							}
						}
					}
				} else if (keySlotState == 2) {
					if ((clockNow - clock3) > 0x32) {
						str_BYTE_E25ED_2BB[v2_int].word_14 = 1;
						clock3 = clockNow;
					}
					DrawBitmap_2BB40(leftEdge, str_BYTE_E25ED_2BB[v2_int].word_2, xy_DWORD_17DED4_spritestr[107]);
					if (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode) {
						if (sub_79E10(textBuff, x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode)) {
							bool isScancode2 = false;
							if (keyIter2 != &x_BYTE_EB39E_keys[9] + 1) {
								for (uint8_t *ii = &x_BYTE_EB39E_keys[0]; ii < &x_BYTE_EB39E_keys[9] + 1; ++ii) {
									if (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode == *ii) {
										isScancode2 = true;
										break;
									}
								}
							}
							if (!isScancode2) {
								x_D41A0_BYTEARRAY_4_struct.setting_38402 = 1;
								*keyIter2 = x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode;
								str_BYTE_E25ED_2BB[v2_int].word_14 = 0;
								x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode = 0;
								v2_int++;
								if (str_BYTE_E25ED_2BB[v2_int].word_0)
									str_BYTE_E25ED_2BB[v2_int].word_14 = 1;
								else
									allKeysDone = 1;
							}
						}
					}
				}
			}
			v2_int++;
		}
		if (allKeysDone >= 1u) {
			unsigned int stepsElapsed = clockNow - clock3;
			if (allKeysDone <= 1u) {
				if (stepsElapsed > 0x32) {
					allKeysDone = 2;
					clock3 = clockNow;
				}
				DrawBitmap_2BB40(283, 381, xy_DWORD_17DED4_spritestr[108]);
			} else if (allKeysDone == 2 && stepsElapsed > 0x32) {
				allKeysDone = 1;
				clock3 = clockNow;
			}
		}
		if (x_WORD_180660_VGA_type_resolution & 1)
			sub_90478_VGA_Blit320(menuFps);
		else
			sub_75200_VGA_Blit640(480, menuFps);
		if (!fadeInDone) {
			sub_90B27_VGA_pal_fadein_fadeout(x_DWORD_17DE38str.palette_17DE38x, 0x20u, 0);
			fadeInDone = 1;
		}
		if ((clockNow - clock2) > 0xA) {
			sub_7A060_get_mouse_and_keyboard_events();
			clock2 = clockNow;
		}
		if (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode == 1)
			exitFlag = 2;
		if (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode == 0x1c) {
			if (allKeysDone)
				exitFlag = 2;
		}
		thread2_wait_for_continue(Thread2_State::KEY_SETTINGS_LOOP);
	}
	WriteConfigDat_81DB0();
	sub_90B27_VGA_pal_fadein_fadeout(0, 0x10u, 0);
	if (x_WORD_180660_VGA_type_resolution & 1) {
		ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 320, 200, 0);
	} else {
		ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 640, 480, 0);
	}
	if (x_WORD_180660_VGA_type_resolution & 1)
		ClearGraphicsBuffer_72883((void *)x_DWORD_E9C38_smalltit, 320, 200, 0);
	else
		ClearGraphicsBuffer_72883((void *)x_DWORD_E9C38_smalltit, 640, 480, 0);
	ResetMouse_7B5A0();
	if (x_WORD_180660_VGA_type_resolution & 1)
		sub_90478_VGA_Blit320();
	else
		sub_75200_VGA_Blit640(480);

	char dataPath[MAX_PATH];
	sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/SCREENS/HSCREEN0.DAT");

	sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.palette_17DE38x, 0, 768);
	sub_7AA70_load_and_decompres_dat_file(dataPath, x_DWORD_17DE38str.x_DWORD_17DE40, x_DWORD_17DE38str.x_DWORD_17DEDC, 168081);
	sub_7AA70_load_and_decompres_dat_file(0, 0, 0, 0);
	SetCursor_8CD27((*filearray_2aa18c[filearrayindex_POINTERSDATTAB].posistruct)[0]);
	sub_41A90_VGA_Palette_install(x_DWORD_17DE38str.palette_17DE38x);
	SetCursor_8CD27(xy_DWORD_17DED4_spritestr[39]);
	if (x_WORD_180660_VGA_type_resolution & 1)
		CopyScreen((void *)pdwScreenBuffer_351628, (void *)x_DWORD_E9C38_smalltit, 320, 200);
	else
		CopyScreen((void *)pdwScreenBuffer_351628, (void *)x_DWORD_E9C38_smalltit, 640, 480);
	return 1;
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
			return LanguageSettingDialog_779E0_mod(var2x);
			break;
		}
		case 0x2590f0: {
			return LoadGameDialog_780F0_mod(var2x); //2590f0 - load xxxxxxxxxxxxxxxxxx
			break;
		}
		case 0x259730: {
			return SaveGameDialog_78730(var2x); //259730 -save xxxxxxxxxxxxxxxxxx
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
			return SetKeysDialog_79610_mod();
			break;
		}
		case 0x25dcf0: {
			return MultiplayerPreviousLevel_7CCF0(/*var*/);
			break;
		}
		case 0x25dd30: {
			return MultiplayerNextLevel_7CD30(/*var*/);
			break;
		}
		case 0x25dca0: {
			return MultiplayerExit_7CCA0(/*var*/);
			break;
		}
		case 0x25dda0: {
			return MultiplayerPlayLevel_7CDA0(/*var*/);
			break;
		}
		case 0x25dbf0: {
			return MultiplayerPreviousWizard_7CBF0(/*var*/);
			break;
		}
		case 0x25dc40: {
			return MultiplayerNextWizard_7CC40(/*var*/);
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

void PlayIntros_83250_mod(char a1) //264250
{
	SetCursor_8CD27((*filearray_2aa18c[filearrayindex_POINTERSDATTAB].posistruct)[0]);
	memset((void *)pdwScreenBuffer_351628, 0, 307200);
	if (x_WORD_180660_VGA_type_resolution != 1) {
		sub_54600_mouse_reset();
		memset((void *)*xadatapald0dat2.colorPalette_var28, 0, 768);
		x_WORD_180660_VGA_type_resolution = 1;
		sub_90D6E_VGA_set_video_mode_320x200_and_Palette((TColor *)*xadatapald0dat2.colorPalette_var28);
		sub_8CEDF_install_mouse();
		SetCursor_8CD27((*filearray_2aa18c[filearrayindex_POINTERSDATTAB].posistruct)[0]); //Set cursor to Null (Don't Draw)
	}
	Intros_76D10_mod(a1);
	sub_54600_mouse_reset();
	memset((void *)*xadatapald0dat2.colorPalette_var28, 0, 768);
	if (x_WORD_180660_VGA_type_resolution & 1)
		ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 320, 200, 0);
	else
		ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 640, 480, 0);

	sub_41A90_VGA_Palette_install((TColor *)*xadatapald0dat2.colorPalette_var28);
	x_WORD_180660_VGA_type_resolution = 8;

#ifdef DEBUG_PALETTE
	//debug palette log
	writePalLog("PlayIntros_83250_mod/sub_90E07_VGA_set_video_mode_640x480_and_Palette((TColor *)*xadatapald0dat2.colorPalette_var28)");
	//debug palette log
#endif //DEBUG_PALETTE

	sub_90E07_VGA_set_video_mode_640x480_and_Palette((TColor *)*xadatapald0dat2.colorPalette_var28);
	MBChangePalette(0);
	sub_8CEDF_install_mouse();
	SetCursor_8CD27((*filearray_2aa18c[filearrayindex_POINTERSDATTAB].posistruct)[0]); //Set cursor to Null (Don't Draw)
	SetCenterScreenForFlyAssistant_6EDB0();
	sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 4);
	ResetMouse_7B5A0();
	SetCursor_8CD27(xy_DWORD_17DED4_spritestr[39]);
	x_DWORD_17DE38str.x_WORD_17DEEE_mouse_buttons = 0;
	x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode = 0;
	LoadSounds_84300(0);
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
		thread2_wait_for_continue(Thread2_State::MAIN_MENU_BEGIN);
		while (!m_ExitMenuLoop_E29DC) {
			//g_state_monitor.Update();

			SetFrameStart(std::chrono::system_clock::now());
			if ((tempMousePosX == x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx) && (tempMousePosY == x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony) && (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode == scanCode)) {
				if ((j___clock() - lastTime) / 100 > 60) //after 1 min run intro
				{
					uint8_t *tempSmalltit = x_DWORD_E9C38_smalltit;
					x_DWORD_E9C38_smalltit = x_DWORD_17DE38str.x_DWORD_17DE44;
					PlayIntros_83250_mod(introIndex);
					thread2_wait_for_continue(Thread2_State::MAIN_MENU_BEGIN);
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
			} else {
				onlyBlit = true;
				//34ee38 20 0
				sub_90B27_VGA_pal_fadein_fadeout_mod(x_DWORD_17DE38str.palette_17DE38x, 0x20u, 0); //tady
			}
			sub_7A060_get_mouse_and_keyboard_events();
			thread2_wait_for_continue(Thread2_State::MAIN_MENU_LOOP);
		}
		ClearPauseMenuState_41BC0();
		D41A0_0.m_GameSettings.m_Display.m_uiScreenSize = 0;
		sub_753D0();
		x_DWORD_E9C38_smalltit = x_DWORD_17DE38str.x_DWORD_17DE44;
	} else {
		ClearPauseMenuState_41BC0();
		D41A0_0.m_GameSettings.m_Display.m_uiScreenSize = 0;
		sub_753D0();
	}
}

void PlayInGameFmv_82670_mod() //263670
{
	uint8_t tempTypeResolution = x_WORD_180660_VGA_type_resolution;
	char dataPath[MAX_PATH];
	sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/SCREENS/HSCREEN0.DAT");
	LastPressedKey_1806E4 = 0;
	if (!x_BYTE_E29E1) {
		if (!(x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 & Setting::MULTIPLAYER_MODE)) {
			DisplaySubtitles_D41C1 = 0;
			x_DWORD_17DE38str.x_DWORD_17DE48c = &x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0];
			x_DWORD_17DE38str.x_DWORD_17DE54 = &x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[301787];
			x_DWORD_17DE38str.x_DWORD_17DEC0 = (bitmap_pos_struct2_t *)(&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[308527]);
			x_DWORD_17DE38str.x_DWORD_17DEC4 = (bitmap_pos_struct2_t *)(&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[310159]);
			sub_7AA70_load_and_decompres_dat_file(dataPath, &x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[301787], 0x164FCD, 860);
			sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.x_DWORD_17DEC0, 0x165329, 548);
			if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 2 || x_D41A0_BYTEARRAY_4_struct.levelnumber_43w > 0x18u) {
				int16_t finalCutSceneIndex = 0;
				int cutSceneIndex = 0;
				while (cutScene_E16E0[cutSceneIndex].levelNumber_4) {
					if (x_D41A0_BYTEARRAY_4_struct.levelnumber_43w + 1 == cutScene_E16E0[cutSceneIndex].levelNumber_4) {
						if (!cutScene_E16E0[cutSceneIndex].overplayed_5) {
							finalCutSceneIndex = cutSceneIndex + 1;
							cutScene_E16E0[cutSceneIndex].overplayed_5 = 1;
						}
						break;
					}
					cutSceneIndex++;
				}
				if (!finalCutSceneIndex) {
					uint16_t levelNumber = x_D41A0_BYTEARRAY_4_struct.levelnumber_43w;
					if (levelNumber > 0x18u) {
						Type_SecretMapScreenPortals_E2970 *secretLevel = GetSecretAndActivedPortal2_824E0(levelNumber);
						if (secretLevel) {
							cutSceneIndex = 0;
							while (cutScene_E16E0[cutSceneIndex].levelNumber_4) {
								if (secretLevel->index_4 + 1 == cutScene_E16E0[cutSceneIndex].levelNumber_4) {
									if (!cutScene_E16E0[cutSceneIndex].overplayed_5) {
										finalCutSceneIndex = cutSceneIndex + 1;
										cutScene_E16E0[cutSceneIndex].overplayed_5 = 1;
									}
									break;
								}
								cutSceneIndex++;
							}
						}
					}
				}
				if (finalCutSceneIndex) {
					if (x_D41A0_BYTEARRAY_4_struct.SelectedLangIndex == 2 && soundAble_E3798 || finalCutSceneIndex >= 6) {
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
					char cutScenePath[MAX_PATH];
					sprintf(cutScenePath, "%s/INTRO/CUT%d.DAT", cdDataPath.c_str(), cutScene_E16E0[cutSceneIndex].fileIndex_6);
					sprintf(printbuffer, "%s", cutScenePath);
					numberOfIntroVideos = cutSceneIndex;
					PlayInfoFmv_mod(0, 1, cutScene_E16E0[cutSceneIndex].pSoundEvent_0, cutScenePath);
					sub_90B27_VGA_pal_fadein_fadeout_mod(0, 0x10u, 0);
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
					if (tempTypeResolution != x_WORD_180660_VGA_type_resolution) {
						sub_54600_mouse_reset();
						memset((void *)*xadatapald0dat2.colorPalette_var28, 0, 768);
						if (x_WORD_180660_VGA_type_resolution & 1)
							ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 320, 200, 0);
						else
							ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 640, 480, 0);
						sub_41A90_VGA_Palette_install((TColor *)*xadatapald0dat2.colorPalette_var28);
						x_WORD_180660_VGA_type_resolution = tempTypeResolution;

#ifdef DEBUG_PALETTE
						//debug palette log
						writePalLog("PlayInGameFmv_82670_mod/sub_90E07_VGA_set_video_mode_640x480_and_Palette((TColor *)*xadatapald0dat2.colorPalette_var28)");
						//debug palette log
#endif //DEBUG_PALETTE

						if (x_WORD_180660_VGA_type_resolution & 1)
							sub_90D6E_VGA_set_video_mode_320x200_and_Palette((TColor *)*xadatapald0dat2.colorPalette_var28);
						else
							sub_90E07_VGA_set_video_mode_640x480_and_Palette((TColor *)*xadatapald0dat2.colorPalette_var28);
						MBChangePalette(0);
						sub_8CEDF_install_mouse();
						SetCursor_8CD27((*filearray_2aa18c[filearrayindex_POINTERSDATTAB].posistruct)[0]); //Set cursor to Null (Don't Draw)
					}
				}
			}
		}
	}
	DisplaySubtitles_D41C1 = 0;
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

	if ((!skipMenus) &&((x_BYTE_D41AD_skip_screen == 1 || (nextMenu_E29D8 != MenuItem::InitLanguage)))) {
		PlayInGameFmv_82670_mod();
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
	if (thread2_state == Thread2_State::SUB_MAIN_BEFORE_LOOP)
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

#ifdef DEBUG_PALETTE
	//debug palette log
	writePalLog("MenusAndIntros_76930_mod/sub_7ADE0(x_BYTE_E29DE)");
	//debug palette log
#endif //DEBUG_PALETTE

	sub_7ADE0(x_BYTE_E29DE);
	if (x_BYTE_E29E1)
		x_BYTE_E29E1 = 0;
	WriteConfigDat_81DB0();
}

void draw_minimap_circle(int16_t x, int16_t y, uint16_t width, uint16_t height, uint8_t color) {
	if (x_WORD_180660_VGA_type_resolution & 1) {
		x >>= 1;
		y >>= 1;
		width >>= 1;
		height >>= 1;
	}
	int cx = x + width / 2;
	int cy = y + height / 2;
	int r = width / 2;
	for (int dy = -r; dy <= r; dy++) {
		int dx = (int)sqrt((double)(r * r - dy * dy));
		uint8_t *row = pdwScreenBuffer_351628 + (cy + dy) * screenWidth_18062C;
		memset(row + cx - dx, color, 2 * dx + 1);
	}
}

uint8_t MyUiBackGroundColorIdx = 254;

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

	//added code
	if (graphics_enhance)
		ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, screenWidth_18062C, screenHeight_180624, MyUiBackGroundColorIdx /* uiBackGroundColorIdx_EB3A8*/);
	//added code

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
			if (graphics_enhance == GRAPHICS_ENHANCE_OFF)
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
					/*
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
					}*/
				}
				if (D41A0_0.m_GameSettings.m_Display.m_wMiniMap) {
					//added code
					if (graphics_enhance)
						switch (D41A0_0.terrain_2FECE.MapType) {
							case MapType_t::Day: {
								draw_minimap_circle(0, 0, 128 * scale, 128 * scale, 107);//blue
								break;
							}
							default: {
								draw_minimap_circle(0, 0, 128 * scale, 128 * scale, 0);//black
								break;
							}
						}
					//added code

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
							Entities_EA3E4[playerEntity->dword_0xA4_164x->str_611.SpellsEnabled_0x333_819x.SpellEnabled[playerEntity->dword_0xA4_164x->str_611.SpellIndexLeft_0x451_1105]],
							false,
							scale);

					//Right
					DrawSpellIcon_2E260(
							spellRightPosX,
							2 * scale,
							Entities_EA3E4[playerEntity->dword_0xA4_164x->str_611.SpellsEnabled_0x333_819x.SpellEnabled[playerEntity->dword_0xA4_164x->str_611.SpellIndexRight_0x453_1107]],
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
			if (graphics_enhance == GRAPHICS_ENHANCE_OFF)
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

			if (graphics_enhance == GRAPHICS_ENHANCE_OFF)
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
						Entities_EA3E4[playerEntity->dword_0xA4_164x->str_611.SpellsEnabled_0x333_819x.SpellEnabled[playerEntity->dword_0xA4_164x->str_611.SpellIndexLeft_0x451_1105]],
						false,
						scale);
			if (x_D41A0_BYTEARRAY_4_struct.rightSpellPlayerIndex_38401)
				DrawSpellIcon_2E260(
						spellRightPosX,
						2,
						Entities_EA3E4[playerEntity->dword_0xA4_164x->str_611.SpellsEnabled_0x333_819x.SpellEnabled[playerEntity->dword_0xA4_164x->str_611.SpellIndexRight_0x453_1107]],
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
}

float MB_Palette_gain[3] = { 1.0f, 1.0f, 1.0f };
float MB_Paletteoffset[3] = { 0.0f, 0.0f, 0.0f };
float MB_Palettesat_multiplier = 1.0f;

void MBChangePalette(int type, int shadow_levels) {
	/*
	if ((thread2_state != Thread2_State::IN_GAME_LOOP) &&(type != 0))
		return;
	*/
	switch (type) {
		case 0: // Reset / Normal
			MB_Palette_gain[0] = 1.0f;
			MB_Palette_gain[1] = 1.0f;
			MB_Palette_gain[2] = 1.0f;
			MB_Paletteoffset[0] = 0.0f;
			MB_Paletteoffset[1] = 0.0f;
			MB_Paletteoffset[2] = 0.0f;
			MB_Palettesat_multiplier = 1.0f;
			break;

		case 1: // Fade In
		{
			float t = (float)x_WORD_181B44 / (shadow_levels-1);
			MB_Palette_gain[0] = t;
			MB_Palette_gain[1] = t;
			MB_Palette_gain[2] = t;
			MB_Paletteoffset[0] = 0.0f;
			MB_Paletteoffset[1] = 0.0f;
			MB_Paletteoffset[2] = 0.0f;
			MB_Palettesat_multiplier = 1.0f;
			break;
		}

		case 2: // Hit – red flash, single-pass
			MB_Palette_gain[0] = 1.0f;
			MB_Palette_gain[1] = 1.0f;
			MB_Palette_gain[2] = 1.0f;
			MB_Paletteoffset[0] = 40.0f / 63.0f;
			MB_Paletteoffset[1] = 0.0f;
			MB_Paletteoffset[2] = 0.0f;
			MB_Palettesat_multiplier = 1.0f;
			break;

		case 3: // R+B suppression, driven by paletteCount_184w
		{
			float t = (float)x_D41A0_BYTEARRAY_4_struct.paletteCount_184w / 256.0f;
			MB_Palette_gain[0] = 1.0f;
			MB_Palette_gain[1] = 1.0f;
			MB_Palette_gain[2] = 1.0f;
			MB_Paletteoffset[0] = (-56.0f * t) / 63.0f;
			MB_Paletteoffset[1] = 0.0f;
			MB_Paletteoffset[2] = (-56.0f * t) / 63.0f;
			MB_Palettesat_multiplier = 1.0f;
			break;
		}

		case 4: // Blue max flash, single-pass
			MB_Palette_gain[0] = 1.0f;
			MB_Palette_gain[1] = 1.0f;
			MB_Palette_gain[2] = 1.0f;
			MB_Paletteoffset[0] = 0.0f;
			MB_Paletteoffset[1] = 0.0f;
			MB_Paletteoffset[2] = 1.0f;
			MB_Palettesat_multiplier = 1.0f;
			break;

		case 5: // Black screen – transitions to subMod=10
			MB_Palette_gain[0] = 0.0f;
			MB_Palette_gain[1] = 0.0f;
			MB_Palette_gain[2] = 0.0f;
			MB_Paletteoffset[0] = 0.0f;
			MB_Paletteoffset[1] = 0.0f;
			MB_Paletteoffset[2] = 0.0f;
			MB_Palettesat_multiplier = 0.0f;
			break;

		case 6: // Cyan flash, single-pass
			MB_Palette_gain[0] = 1.0f;
			MB_Palette_gain[1] = 1.0f;
			MB_Palette_gain[2] = 1.0f;
			MB_Paletteoffset[0] = 48.0f / 63.0f;
			MB_Paletteoffset[1] = 32.0f / 63.0f;
			MB_Paletteoffset[2] = 32.0f / 63.0f;
			MB_Palettesat_multiplier = 1.0f;
			break;

		case 7: // Black & White, single-pass
			MB_Palette_gain[0] = 1.0f;
			MB_Palette_gain[1] = 1.0f;
			MB_Palette_gain[2] = 1.0f;
			MB_Paletteoffset[0] = 0.0f;
			MB_Paletteoffset[1] = 0.0f;
			MB_Paletteoffset[2] = 0.0f;
			MB_Palettesat_multiplier = 0.0f;
			break;

		case 8: // White flash, single-pass
			MB_Palette_gain[0] = 1.0f;
			MB_Palette_gain[1] = 1.0f;
			MB_Palette_gain[2] = 1.0f;
			MB_Paletteoffset[0] = 48.0f / 63.0f;
			MB_Paletteoffset[1] = 48.0f / 63.0f;
			MB_Paletteoffset[2] = 48.0f / 63.0f;
			MB_Palettesat_multiplier = 1.0f;
			break;

		case 9: // Fade Out
		{
			float t = 1.0-((float)x_WORD_181B44 / (shadow_levels - 1));
			MB_Palette_gain[0] = t;
			MB_Palette_gain[1] = t;
			MB_Palette_gain[2] = t;
			MB_Paletteoffset[0] = 0.0f;
			MB_Paletteoffset[1] = 0.0f;
			MB_Paletteoffset[2] = 0.0f;
			MB_Palettesat_multiplier = 1.0f;
			break;
		}

		case 10: // Clear palette
			MB_Palette_gain[0] = 0.0f;
			MB_Palette_gain[1] = 0.0f;
			MB_Palette_gain[2] = 0.0f;
			MB_Paletteoffset[0] = 0.0f;
			MB_Paletteoffset[1] = 0.0f;
			MB_Paletteoffset[2] = 0.0f;
			MB_Palettesat_multiplier = 1.0f;
			break;

		case 11: // Fade In 2
		{
			float t = (float)x_WORD_181B44 / shadow_levels;
			MB_Palette_gain[0] = t;
			MB_Palette_gain[1] = t;
			MB_Palette_gain[2] = t;
			MB_Paletteoffset[0] = 0.0f;
			MB_Paletteoffset[1] = 0.0f;
			MB_Paletteoffset[2] = 0.0f;
			MB_Palettesat_multiplier = 1.0f;
			break;
		}

		case 12: // Fade Out 2
		{
			float t = 1.0 - ((float)x_WORD_181B44 / shadow_levels);
			MB_Palette_gain[0] = t;
			MB_Palette_gain[1] = t;
			MB_Palette_gain[2] = t;
			MB_Paletteoffset[0] = 0.0f;
			MB_Paletteoffset[1] = 0.0f;
			MB_Paletteoffset[2] = 0.0f;
			MB_Palettesat_multiplier = 1.0f;
			break;
		}

		default:
			break;
	}

#ifdef DEBUG_PALETTE
	//debug palette log
	char logMessage[200];
	sprintf(logMessage, "MBChangePalette gain(%f %f %f), offset(%f %f %f), sat(%f)", MB_Palette_gain[0], MB_Palette_gain[1], MB_Palette_gain[2], MB_Paletteoffset[0], MB_Paletteoffset[1], MB_Paletteoffset[2], MB_Palettesat_multiplier);
	writePalLog(logMessage);
	//debug palette log
#endif //DEBUG_PALETTE
}

void PaletteFadeIn_480A0_mod() //2290a0
{
	char dataPath[MAX_PATH];
	unsigned int timeDiff = 0;
	SetMusicVolume_98790(500, 0);
	long time = j___clock();
	do
		timeDiff = j___clock() - time;
	while (timeDiff < 50); //delay 50 mms
	sub_90B27_VGA_pal_fadein_fadeout_mod(0, 0x10u, 0);
	D41A0_0.dword_0x23a = 0;
	sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/PALD-0.DAT");
	DataFileIO::ReadFileAndDecompress(dataPath, xadatapald0dat2.colorPalette_var28);
	sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/CLRD-0.DAT");
	DataFileIO::ReadFileAndDecompress(dataPath, xadataclrd0dat.colorPalette_var28);
	sub_48120();
}

void PaletteChanges_47760_mod() //228760
{
	char dataPath[MAX_PATH];
	TColor **DefaultPal = (TColor **)xadatapald0dat2.colorPalette_var28;
	x_D41A0_BYTEARRAY_4_struct.moveSpeedFlag_181 = 0;
	switch (x_D41A0_BYTEARRAY_4_struct.paletteMod_51) {
		case 0:
		case 1: //Fade out loading screen
		{
			PaletteFadeIn_480A0_mod();
			x_D41A0_BYTEARRAY_4_struct.paletteMod_51++;
			break;
		}
		case 2: {
			x_D41A0_BYTEARRAY_4_struct.paletteMod_51++;
			x_D41A0_BYTEARRAY_4_struct.paletteSubMod_180 = 1;
			memset((void *)*DefaultPal, 0, 768);
			sub_41A90_VGA_Palette_install(*DefaultPal);
			switch (D41A0_0.terrain_2FECE.MapType) {
				case MapType_t::Day: {
					sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/PALD-0.DAT");
					DataFileIO::ReadFileAndDecompress(dataPath, xadatapald0dat2.colorPalette_var28);
					sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/CLRD-0.DAT");
					DataFileIO::ReadFileAndDecompress(dataPath, xadataclrd0dat.colorPalette_var28);
				} break;
				case MapType_t::Night: {
					if (D41A0_0.terrain_2FECE.byte_0x2FED2 & 2) {
						sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/PALF-0.DAT");
						DataFileIO::ReadFileAndDecompress(dataPath, xadatapald0dat2.colorPalette_var28);
					} else {
						sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/PALN-0.DAT");
						DataFileIO::ReadFileAndDecompress(dataPath, xadatapald0dat2.colorPalette_var28);
					}
					sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/CLRN-0.DAT");
					DataFileIO::ReadFileAndDecompress(dataPath, xadataclrd0dat.colorPalette_var28);
					break;
				}
				case MapType_t::Cave: {
					sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/PALC-0.DAT");
					DataFileIO::ReadFileAndDecompress(dataPath, xadatapald0dat2.colorPalette_var28);
					sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/CLRC-0.DAT");
					DataFileIO::ReadFileAndDecompress(dataPath, xadataclrd0dat.colorPalette_var28);
					break;
				}
			}
			qmemcpy((void *)tempPalette_EA3B8x, (void *)*DefaultPal, 0x300u);
			sub_47650(0x300);
			sub_90D27();
			uiBackGroundColorIdx_EB3A8 = (*DefaultPal)[0].red;
			sub_57640();
			//MBChangePalette(0);
			break;
		}
		case 3: {
			switch (x_D41A0_BYTEARRAY_4_struct.paletteSubMod_180) {
				case 1:
					//Fade back
					x_D41A0_BYTEARRAY_4_struct.moveSpeedFlag_181 = 1;
					if (sub_90B27_VGA_pal_fadein_fadeout_mod(*DefaultPal, 4u, 1, 0) == 4) {
						x_D41A0_BYTEARRAY_4_struct.paletteSubMod_180 = 0;
					}
					break;
				case 2:
					//Hit (red flash)
					for (int i = 1; i < 256; i++) {
						x_DWORD_F42A0 = (*DefaultPal)[i].red + 40;
						if (x_DWORD_F42A0 < 0)
							x_DWORD_F42A0 = 0;
						if (x_DWORD_F42A0 > 63)
							x_DWORD_F42A0 = 63;
						x_BYTE_F3FA0arx[i].red = x_DWORD_F42A0;
						x_DWORD_F42A0 = (*DefaultPal)[i].green;
						if (x_DWORD_F42A0 < 0)
							x_DWORD_F42A0 = 0;
						if (x_DWORD_F42A0 > 63)
							x_DWORD_F42A0 = 63;
						x_BYTE_F3FA0arx[i].green = x_DWORD_F42A0;
						x_DWORD_F42A0 = (*DefaultPal)[i].blue;
						if (x_DWORD_F42A0 < 0)
							x_DWORD_F42A0 = 0;
						if (x_DWORD_F42A0 > 63)
							x_DWORD_F42A0 = 63;
						x_BYTE_F3FA0arx[i].blue = x_DWORD_F42A0;
					}
					sub_90D27();
					sub_41A90_VGA_Palette_install(x_BYTE_F3FA0arx);
					x_D41A0_BYTEARRAY_4_struct.paletteSubMod_180 = 1;
					x_D41A0_BYTEARRAY_4_struct.moveSpeedFlag_181 = 1;
					MBChangePalette(2);
					break;
				case 3:
					for (int i = 1; i < 256; i++) {
						x_DWORD_F42A0 = (*DefaultPal)[i].red;
						x_DWORD_F42A0 += -56 * x_D41A0_BYTEARRAY_4_struct.paletteCount_184w >> 8;
						if (x_DWORD_F42A0 < 0)
							x_DWORD_F42A0 = 0;
						if (x_DWORD_F42A0 > 63)
							x_DWORD_F42A0 = 63;
						x_BYTE_F3FA0arx[i].red = x_DWORD_F42A0;
						x_DWORD_F42A0 = (*DefaultPal)[i].green;
						if (x_DWORD_F42A0 < 0)
							x_DWORD_F42A0 = 0;
						if (x_DWORD_F42A0 > 63)
							x_DWORD_F42A0 = 63;
						x_BYTE_F3FA0arx[i].green = x_DWORD_F42A0;
						x_DWORD_F42A0 = (*DefaultPal)[i].blue;
						x_DWORD_F42A0 += -56 * x_D41A0_BYTEARRAY_4_struct.paletteCount_184w >> 8;
						if (x_DWORD_F42A0 < 0)
							x_DWORD_F42A0 = 0;
						if (x_DWORD_F42A0 > 63)
							x_DWORD_F42A0 = 63;
						x_BYTE_F3FA0arx[i].blue = x_DWORD_F42A0;
					}
					sub_90D27();
					sub_41A90_VGA_Palette_install(x_BYTE_F3FA0arx);
					x_D41A0_BYTEARRAY_4_struct.paletteSubMod_180 = 1;
					x_D41A0_BYTEARRAY_4_struct.moveSpeedFlag_181 = 1;
					MBChangePalette(3);
					break;
				case 4:
					for (int i = 1; i < 256; i++) {
						x_DWORD_F42A0 = 255;
						x_BYTE_F3FA0arx[i].red = (*DefaultPal)[i].red;
						x_BYTE_F3FA0arx[i].green = (*DefaultPal)[i].green;
						x_BYTE_F3FA0arx[i].blue = 63;
					}
					sub_90D27();
					sub_41A90_VGA_Palette_install(x_BYTE_F3FA0arx);
					x_D41A0_BYTEARRAY_4_struct.paletteSubMod_180 = 1;
					x_D41A0_BYTEARRAY_4_struct.moveSpeedFlag_181 = 1;
					MBChangePalette(4);
					break;
				case 5:
					memset((void *)*xadatapald0dat2.colorPalette_var28, 0, 768);
					x_D41A0_BYTEARRAY_4_struct.paletteSubMod_180 = 10;
					x_D41A0_BYTEARRAY_4_struct.moveSpeedFlag_181 = 1;
					MBChangePalette(5);
					break;
				case 6:
					for (int i = 1; i < 256; i++) {
						x_DWORD_F42A0 = (*DefaultPal)[i].blue + 48;
						if (x_DWORD_F42A0 < 0)
							x_DWORD_F42A0 = 0;
						if (x_DWORD_F42A0 > 63)
							x_DWORD_F42A0 = 63;
						x_BYTE_F3FA0arx[i].red = x_DWORD_F42A0;
						x_DWORD_F42A0 = (*DefaultPal)[i].green + 32;
						if (x_DWORD_F42A0 < 0)
							x_DWORD_F42A0 = 0;
						if (x_DWORD_F42A0 > 63)
							x_DWORD_F42A0 = 63;
						x_BYTE_F3FA0arx[i].green = x_DWORD_F42A0;
						x_DWORD_F42A0 = (*DefaultPal)[i].blue + 32;
						if (x_DWORD_F42A0 < 0)
							x_DWORD_F42A0 = 0;
						if (x_DWORD_F42A0 > 63)
							x_DWORD_F42A0 = 63;
						x_BYTE_F3FA0arx[i].blue = x_DWORD_F42A0;
					}
					sub_90D27();
					sub_41A90_VGA_Palette_install(x_BYTE_F3FA0arx);
					x_D41A0_BYTEARRAY_4_struct.paletteSubMod_180 = 1;
					x_D41A0_BYTEARRAY_4_struct.moveSpeedFlag_181 = 1;
					MBChangePalette(6);
					break;
				case 7:
					//Black and White
					for (int i = 1; i < 256; i++) {
						x_DWORD_F42A0 = ((*DefaultPal)[i].red + (*DefaultPal)[i].green + (*DefaultPal)[i].blue) / 3;
						x_BYTE_F3FA0arx[i].red = x_DWORD_F42A0;
						x_BYTE_F3FA0arx[i].green = x_DWORD_F42A0;
						x_BYTE_F3FA0arx[i].blue = x_DWORD_F42A0;
					}
					sub_90D27();
					sub_41A90_VGA_Palette_install(x_BYTE_F3FA0arx);
					x_D41A0_BYTEARRAY_4_struct.paletteSubMod_180 = 1;
					x_D41A0_BYTEARRAY_4_struct.moveSpeedFlag_181 = 1;
					MBChangePalette(7);
					break;
				case 8:
					for (int i = 1; i < 256; i++) {
						x_DWORD_F42A0 = (*DefaultPal)[i].red + 48;
						if (x_DWORD_F42A0 < 0)
							x_DWORD_F42A0 = 0;
						if (x_DWORD_F42A0 > 63)
							x_DWORD_F42A0 = 63;
						x_BYTE_F3FA0arx[i].red = x_DWORD_F42A0;
						x_DWORD_F42A0 = (*DefaultPal)[i].green + 48;
						if (x_DWORD_F42A0 < 0)
							x_DWORD_F42A0 = 0;
						if (x_DWORD_F42A0 > 63)
							x_DWORD_F42A0 = 63;
						x_BYTE_F3FA0arx[i].green = x_DWORD_F42A0;
						x_DWORD_F42A0 = (*DefaultPal)[i].blue + 48;
						if (x_DWORD_F42A0 < 0)
							x_DWORD_F42A0 = 0;
						if (x_DWORD_F42A0 > 63)
							x_DWORD_F42A0 = 63;
						x_BYTE_F3FA0arx[i].blue = x_DWORD_F42A0;
					}
					sub_90D27();
					sub_41A90_VGA_Palette_install(x_BYTE_F3FA0arx);
					x_D41A0_BYTEARRAY_4_struct.paletteSubMod_180 = 9;
					x_D41A0_BYTEARRAY_4_struct.moveSpeedFlag_181 = 1;
					MBChangePalette(8);
					break;
				case 9:
					x_D41A0_BYTEARRAY_4_struct.moveSpeedFlag_181 = 1;
					if (sub_90B27_VGA_pal_fadein_fadeout_mod((TColor *)*xadatapald0dat2.colorPalette_var28, 0x10u, 1) == 16) {
						x_D41A0_BYTEARRAY_4_struct.paletteSubMod_180 = 0;
					}
					break;
				case 0xA:
					x_D41A0_BYTEARRAY_4_struct.moveSpeedFlag_181 = 1;
					if (sub_90B27_VGA_pal_fadein_fadeout_mod((TColor *)*xadatapald0dat2.colorPalette_var28, 0x1Cu, 1) == 28) {
						x_D41A0_BYTEARRAY_4_struct.paletteSubMod_180 = 0;
					}
					break;
				default:
					return;
			}
			break;
		}
	}
}

int loc_debug_first_run = 0;
int specialAction = -1;
void DrawAndEventsInGame_47560_mod(int16_t turn) //228560
{
	SetFrameStart(std::chrono::system_clock::now());
	/*
	if ((CommandLineParams.ModeRegressionsTestType() != -1) && (count_begin == 1))
		debugcounter_47560++;
	*/

#ifdef DEBUG_PALETTE
	//debug palette log
	writePalLog("DrawAndEventsInGame_47560_mod/PaletteChanges_47760_mod()");
	//debug palette log
#endif //DEBUG_PALETTE

	PaletteChanges_47760_mod();
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

#ifdef DEBUG_PALETTE
	//debug palette log
	writePalLog("DrawAndEventsInGame_47560_mod/MouseAndKeysEvents_17A00(turn)");
	//debug palette log
#endif //DEBUG_PALETTE

	MouseAndKeysEvents_17A00(turn);

	type_entity_0x6E8E *specialEntity;
	switch (specialAction) {
		case 0:
			specialEntity = Entities_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
			sub_18DA0(specialEntity, 2, 0);
			specialAction = -1;
			break;
		case 1:
			specialEntity = Entities_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
			sub_18DA0(specialEntity, 1, 0);
			specialAction = -1;
			break;
		case 2:
			specialEntity = Entities_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
			sub_18DA0(specialEntity, 2, 0);
			specialAction = -1;
			break;
		case 3:
			specialEntity = Entities_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].playerIndex_0x00a_2BE4_11240];
			sub_18DA0(specialEntity, 1, 0);
			specialAction = -1;
			break;
	}

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
	if (!game_paused || oneFrameRun) {
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
	if (!(gameResWidth == 320 && gameResHeight == 200))
	{
		VGA_Resize(320, 200);
		screenWidth_18062C = 320;
		screenHeight_180624 = 200;
		sub_A0D50_set_viewport(0, 0, 320, 200);
		x_WORD_180660_VGA_type_resolution = 1;
		resindex_begin = 0;
	}
	//fix res on begin level for hidden levels-neoriginal code

	EventDispatcher::I->DispatchEvent(EventType::E_GAME_STATE_CHANGE, GameState::STARTED);
	thread2_wait_for_continue(Thread2_State::IN_GAME_BEGIN);
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
		thread2_wait_for_continue(Thread2_State::IN_GAME_LOOP);
	}
	thread2_wait_for_continue(Thread2_State::IN_GAME_END);
	EventDispatcher::I->DispatchEvent(EventType::E_GAME_STATE_CHANGE, GameState::GAMEPLAY_ENDED);

#ifdef DEBUG_PALETTE
	//debug palette log
	writePalLog("InGameLoop_47320_mod/sub_90E07_VGA_set_video_mode_640x480_and_Palette((TColor *)*xadatapald0dat2.colorPalette_var28)");
	//debug palette log
#endif //DEBUG_PALETTE

	sub_90E07_VGA_set_video_mode_640x480_and_Palette((TColor *)*xadatapald0dat2.colorPalette_var28);
	MBChangePalette(0);
}

void sub_47FC0_load_screen_mod(bool isSecretLevel) //228fc0
{
#ifdef DEBUG_PALETTE
	//debug palette log
	writePalLog("sub_47FC0_load_screen_mod/sub_90B27_VGA_pal_fadein_fadeout_mod(0, 0x10u, 0)");
	//debug palette log
#endif //DEBUG_PALETTE

	char dataPath[MAX_PATH];
	sub_90B27_VGA_pal_fadein_fadeout_mod(0, 0x10u, 0);

	//debug
	//sub_90B27_VGA_pal_fadein_fadeout_mod((TColor *)*xadatapald0dat2.colorPalette_var28, 0x20u, 0);
	//debug

	if (isSecretLevel) {
		sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/SMATITL2.DAT");
		DataFileIO::ReadFileAndDecompress(dataPath, &x_DWORD_E9C38_smalltit);
	} else {
		sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/SMATITLE.DAT");
		DataFileIO::ReadFileAndDecompress(dataPath, &x_DWORD_E9C38_smalltit);
	}
	CopyScreen_85B20(x_DWORD_E9C38_smalltit, pdwScreenBuffer_351628, 400);
	if (x_WORD_180660_VGA_type_resolution & 1)
		sub_90478_VGA_Blit320();
	else
		sub_75200_VGA_Blit640(480);
	thread2_wait_for_continue(Thread2_State::LOAD_SCREEN);
	if (isSecretLevel) {
		sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/SMATITL2.PAL");
		DataFileIO::ReadFileAndDecompress(dataPath, xadatapald0dat2.colorPalette_var28);
	} else {
		sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/SMATITLE.PAL");
		DataFileIO::ReadFileAndDecompress(dataPath, xadatapald0dat2.colorPalette_var28);
	}

#ifdef DEBUG_PALETTE
	//debug palette log
	writePalLog("sub_47FC0_load_screen_mod/sub_90B27_VGA_pal_fadein_fadeout_mod((TColor *)*xadatapald0dat2.colorPalette_var28, 0x20u, 0)");
	//debug palette log
#endif //DEBUG_PALETTE

	sub_90B27_VGA_pal_fadein_fadeout_mod((TColor *)*xadatapald0dat2.colorPalette_var28, 0x20u, 0);
	D41A0_0.dword_0x23a = 1;
	D41A0_0.dword_0x23e = 0;
	D41A0_0.dword_0x242 = 0;
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
			sub_47FC0_load_screen_mod(isSecretLevel); //vga smaltitle
			LevelInitGame_56A30(setLevel, customLevelPath);
			if (CommandLineParams.DoAutoChangeRes()) {
				resindex_begin = 0;
			}

			LoadSpr_47160();

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
					if (!strcmp(forceRender.c_str(), "NG"))
						m_ptrGameRender = (GameRenderInterface *)new GameRenderNG();
					else if (!strcmp(forceRender.c_str(), "Original"))
						m_ptrGameRender = (GameRenderInterface *)new GameRenderOriginal();
					else if (!strcmp(forceRender.c_str(), "HD"))
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
				sub_90B27_VGA_pal_fadein_fadeout_mod(0, 0x10u, 0);
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

							sub_47FC0_load_screen_mod(true);
							LevelInitGame_56A30(actLevel);
							LoadSpr_47160();
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


int sub_main_mod(int argc, char **argv, char *real_cdPathch, char *real_gamePath) {
	godot::UtilityFunctions::print("sub_main_mod begin");
	std::function<void(Scene)> sceneChangeCallBack = SetCurrentScene;
	int exitCode = 0;
	godot::UtilityFunctions::print("sub_main_mod SetTimeStart");
	SetTimeStart();
#ifndef __ANDROID__
	try
#endif
	{
		godot::UtilityFunctions::print("sub_main_mod begin_plugin");
		begin_plugin();
		godot::UtilityFunctions::print("sub_main_mod preconvert");
		preconvert(); //rewrite and remove it later
		*xadataclrd0dat.colorPalette_var28 = (uint8_t *)malloc(4096); //fix it
		signed int v3; // edi
		unsigned __int16 v4; // si
		v3 = 0;
		v4 = 0;

		godot::UtilityFunctions::print("sub_main_mod RegisterEvent");
		//std::cout << "Initializing logger...\n";
		//spdlog::level::level_enum level = GetLoggingLevelFromString(CommandLineParams.GetLogLevelStr().c_str());
		//InitializeLogging(level);
		EventDispatcher::I = new EventDispatcher();
		EventDispatcher::I->RegisterEvent(new Event<Scene>(EventType::E_SCENE_CHANGE, sceneChangeCallBack));
		EventDispatcher::I->DispatchEvent(EventType::E_GAME_STATE_CHANGE, GameState::STARTED);

		godot::UtilityFunctions::print("sub_main_mod gameFolder");
		//SetConfig();
		gameFolder = std::string(real_cdPathch) + "GAME/NETHERW";
		cdFolder = std::string(real_cdPathch) + "CD_Files";
		highResGraphicsFolder = std::string(real_gamePath) + "";
		fixedMenuGraphicsFolder = std::string(real_gamePath) + "fixed/menu";
		inputMapping.Forward = 0x1a; //added
		inputMapping.Backwards = 0x16; //added
		inputMapping.Left = 0x04; //added
		inputMapping.Right = 0x07; //added
		inputMapping.Map = 0x2b; //added
		inputMapping.SpellMenu = 0xe0; //added
		inputMapping.SpellMenuMark = 0xe1; //added
		windowResWidth = 640; //added
		windowResHeight = 480; //added
		gameResWidth = 640; //added
		gameResHeight = 480; //added
		maxGameFps = 1000; //added

		godot::UtilityFunctions::print("sub_main_mod DoDisableGraphicsEnhance");

		if (CommandLineParams.DoDisableGraphicsEnhance()) {
			Logger->debug("Disabling enhanced graphics");
			bigSprites = false;
			bigTextures = false;
			texturepixels = 32;
		}
		//Set Paths for game data
		Logger->debug("Getting Game data paths");
		godot::UtilityFunctions::print("sub_main_mod Getting Game data paths");
		godot::UtilityFunctions::print("sub_main_mod gameFolder: ", godot::String(gameFolder.c_str()));
		gameDataPath = GetSubDirectoryPath(gameFolder.c_str());
		godot::UtilityFunctions::print("sub_main_mod gameDataPath: ", godot::String(gameDataPath.c_str()));
		godot::UtilityFunctions::print("sub_main_mod cdFolder: ", godot::String(cdFolder.c_str()));
		cdDataPath = GetSubDirectoryPath(cdFolder.c_str());
		godot::UtilityFunctions::print("sub_main_mod cdDataPath: ", godot::String(cdDataPath.c_str()));
		godot::UtilityFunctions::print("sub_main_mod highResGraphicsFolder: ", godot::String(highResGraphicsFolder.c_str()));
		highResGraphicsPath = GetSubDirectoryPath(highResGraphicsFolder.c_str());
		godot::UtilityFunctions::print("sub_main_mod highResGraphicsPath: ", godot::String(highResGraphicsPath.c_str()));
		godot::UtilityFunctions::print("sub_main_mod fixedMenuGraphicsFolder: ", godot::String(fixedMenuGraphicsFolder.c_str()));
		fixedMenuGraphicsPath = GetSubDirectoryPath(fixedMenuGraphicsFolder.c_str());
		godot::UtilityFunctions::print("sub_main_mod fixedMenuGraphicsPath: ", godot::String(fixedMenuGraphicsPath.c_str()));
		godot::UtilityFunctions::print("sub_main_mod VGA_Init");
		VGA_Init(windowResWidth, windowResHeight, gameResWidth, gameResHeight, maintainAspectRatio, displayIndex);
		godot::UtilityFunctions::print("sub_main_mod gamepad_init");
		gamepad_init(gameResWidth, gameResHeight);
		if (std::string mainfile = GetSubDirectoryFile(gameFolder.c_str(), "CDATA", "TMAPS0-0.DAT"); !file_exists(mainfile.c_str())) //test original file
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
		godot::UtilityFunctions::print("sub_main_mod initposistruct");
		initposistruct();
		godot::UtilityFunctions::print("sub_main_mod sub_56210_process_command_line");
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
		godot::UtilityFunctions::print("sub_main_mod Initialize");
		Initialize(); //236FDC - 23C8D0//rozdil 1E1000
		godot::UtilityFunctions::print("sub_main_mod thread2_wait_for_continue");
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
	}
#ifndef __ANDROID__
	catch (const thread_exit_exception &e) {
		//Logger->info("Immediate Exit called");
	} catch (const std::exception &e) {
		//Logger->critical("Critical Error: {}", e.what());
		exitCode = -1;
	}
#endif
	//Logger->info("Exited Game");

	thread2_wait_for_continue(Thread2_State::SUB_MAIN_END_FUNCTION);

	return exitCode;
}


//fix sub_90B27_VGA_pal_fadein_fadeout(0, 0x10u, 0);-no ok fade ou engine
//fix changeLanguage-ok
//fix palette in end load screen - better analyze
//fix key ctrl

//fix importer
//fix palette
//sub_68BF0() sub_68C70(jx) struct_byte_0xc_12_15.byte[0] 0x5 0x20

//sub_293D0-hide
//sub_69300-show?
