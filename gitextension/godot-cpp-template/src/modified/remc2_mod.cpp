#include "remc2_mod.h"

int NewGameDialog_endAction_mod;

bool NewGameDialog_77350_mod_Begin()
{

	bool result = false;

	NewGameDialog_endAction_mod = 0;
	map_not_moving_WORD_E29D6 = false;
	if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 2 && x_D41A0_BYTEARRAY_4_struct.levelnumber_43w == 24) {
		// show credits after finishing the last level
		ShowEndCredits_833C0();//MISSING
	}
	memset((void *)&unk_17DBA8str, 0, sizeof(unk_17DBA8str));
	unk_17DBA8str.x_BYTE_17DBB6 = 2;
	if (x_BYTE_D419C_level_num <= -1) {
		x_BYTE_17E09D = 0;
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
		//skip sub_8CD27_set_cursor((*filearray_2aa18c[filearrayindex_POINTERSDATTAB].posistruct)[0]);

		//skip ResetMouse_7B5A0();
		//skip sub_90B27_VGA_pal_fadein_fadeout(0, 0x10u, 0);
		//skip if (x_WORD_180660_VGA_type_resolution & 1) {
		//skip 	ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 320, 200, getPaletteIndex_5BE80(x_DWORD_17DE38str.palette_17DE38x, 0, 0, 0));
		//skip } else {
		//skip 	ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 640, 480, getPaletteIndex_5BE80(x_DWORD_17DE38str.palette_17DE38x, 0, 0, 0));
		//skip }
		//skip if (x_WORD_180660_VGA_type_resolution & 1)
		//skip 	sub_90478_VGA_Blit320();
		//skip else
		//skip sub_75200_VGA_Blit640(480);
		sub_41A90_VGA_Palette_install(x_DWORD_17DE38str.palette_17DE38x);
		PortalsUpdate_7DD70();
		x_DWORD_17DE38str.x_WORD_17DEEC = 0;
		SetCenterScreenForFlyAssistant_6EDB0();
		sub_8CD27_set_cursor(xy_DWORD_17DED4_spritestr[239]);
		x_DWORD_17DB70str.x_WORD_17DB8A = x_D41A0_BYTEARRAY_4_struct.levelnumber_43w;

		//start loop
	} else {
		m_ExitMenuLoop_E29DC = 1;
		x_D41A0_BYTEARRAY_4_struct.levelnumber_43w = x_BYTE_D419C_level_num;
		//skip if (a1x)
		//skip 	a1x->dword_4 = 0;
		result = true;
	}
	return result;
}

void NewGameDialog_77350_mod_begin() {
			//skip while (!endAction) {
			//skip g_state_monitor.Update();
			SetFrameStart(std::chrono::system_clock::now());
			if (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode == 59) {
				x_DWORD_17DE38str.showHelp_17DF13 = x_D41A0_BYTEARRAY_4_struct.showHelp_10 != 1;
				x_D41A0_BYTEARRAY_4_struct.showHelp_10 = x_D41A0_BYTEARRAY_4_struct.showHelp_10 != 1;
				x_D41A0_BYTEARRAY_4_struct.setting_38402 = 1;
			}
			NewGameDialog_endAction_mod = NewGameDraw_7EAE0(
					&x_DWORD_17DB70str.unk_17DB76_posx,
					&x_DWORD_17DB70str.unk_17DB78_posy,
					&x_DWORD_17DB70str.unk_17DB7E,
					&x_DWORD_17DB70str.unk_17DB80,
					&x_DWORD_17DB70str.x_BYTE_17DB8F,
					&x_DWORD_17DB70str.unk_17DB90);
			if (CommandLineParams.ModeTestRegressionsGame()) {
				x_DWORD_17DB70str.x_BYTE_17DB8E = 1;
				x_D41A0_BYTEARRAY_4_struct.levelnumber_43w = test_regression_level;
				if (mapScreenPortals_E17CC[test_regression_level].activated_18 == 1)
					x_D41A0_BYTEARRAY_4_struct.setting_38545 |= 4u;
				Type_SecretMapScreenPortals_E2970 *v46x = GetSecretAndActivedPortal_824B0(x_D41A0_BYTEARRAY_4_struct.levelnumber_43w);
				if (v46x && v46x->activated_12 == 2)
					x_D41A0_BYTEARRAY_4_struct.setting_38545 |= 0x10u;
				if (x_D41A0_BYTEARRAY_4_struct.levelnumber_43w == 24)
					x_D41A0_BYTEARRAY_4_struct.setting_38545 |= 0x20u;
				NewGameDialog_endAction_mod = 1;
			}
			//skip if (x_WORD_180660_VGA_type_resolution & 1)
			//skip 	sub_90478_VGA_Blit320(menuFps);
			//skip else
			//skip 	sub_75200_VGA_Blit640(480, menuFps);
			//skip sub_7A060_get_mouse_and_keyboard_events();
			//skip }
}

bool NewGameDialog_77350_mod_End()
{
	bool result = false;
		sub_86860_speak_Sound(x_WORD_1803EC);
		D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] = 0;
		EndSample_8D8F0();
		map_not_moving_WORD_E29D6 = false;
		if (NewGameDialog_endAction_mod == 1) {
			m_ExitMenuLoop_E29DC = 1;
			//skip if (a1x)
			//skip a1x->dword_4 = 0;
			result = false;
		} else if (NewGameDialog_endAction_mod == 2) {
			result = true;
		}


		//skip else if (NewGameDialog_endAction_mod == 2 && a1x) {
		//skip	a1x->dword_4 = 1;
		//skip}
		//skip sub_8CD27_set_cursor((*filearray_2aa18c[filearrayindex_POINTERSDATTAB].posistruct)[0]);
		//skip sub_90B27_VGA_pal_fadein_fadeout(0, 0x10u, 0);
	return result;
}
