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

int NewGameDraw_7EAE0_mod(int16_t *posx, int16_t *posy, __int16 *portPosX, __int16 *portPosY, int8_t *a5, type_animStruct *animStruct) {
	type_WORD_E1F84 selectVar;
	int index2;
	int index3 = -1;
	int result2;
	int result = 0;
	int index4;

	switch (*a5) {
		case 1:
			index2 = -1;
			for (int i = 0; mapScreenPortals_E17CC[i].viewPortPosX_4; i++) {
				if (mapScreenPortals_E17CC[i].activated_18 == 1) {
					*posx = mapScreenPortals_E17CC[i].viewPortPosX_4;
					*posy = mapScreenPortals_E17CC[i].viewPortPosY_6;
					index3 = i;
					index2 = i;
				}
			}
			if ((index2 != -1) && x_DWORD_17DB70str.x_WORD_17DB8A != -1 && index3 != x_DWORD_17DB70str.x_WORD_17DB8A) {
				for (int i = 0; mapScreenPortals_E17CC[i].viewPortPosX_4; i++) {
					if (i == x_DWORD_17DB70str.x_WORD_17DB8A) {
						*posx = mapScreenPortals_E17CC[i].viewPortPosX_4;
						*posy = mapScreenPortals_E17CC[i].viewPortPosY_6;
						*a5 = 3;
						break;
					}
				}
				if (*a5 != 3) {
					for (int ii = 0; secretMapScreenPortals_E2970[ii].activated_12; ii++) {
						if (x_DWORD_17DB70str.x_WORD_17DB8A == secretMapScreenPortals_E2970[ii].levelNumber_6) {
							*posx = mapScreenPortals_E17CC[secretMapScreenPortals_E2970[ii].index_4].viewPortPosX_4;
							*posy = mapScreenPortals_E17CC[secretMapScreenPortals_E2970[ii].index_4].viewPortPosY_6;
							*a5 = 3;
							break;
						}
					}
				}
			} else {
				x_DWORD_17DB70str.x_WORD_17DB8A = -1;
			}
			memset(&x_DWORD_17DE28str, 0, 13);
			if (x_DWORD_17DB70str.x_WORD_17DB8A == -1) {
				for (int jx = 0; mapScreenPortals_E17CC[jx].viewPortPosX_4 != 0; jx++) {
					if (mapScreenPortals_E17CC[jx].activated_18 == 2) {
						*portPosX = mapScreenPortals_E17CC[jx].viewPortPosX_4;
						*portPosY = mapScreenPortals_E17CC[jx].viewPortPosY_6;
						if (jx) {
							*a5 = 2;
							CreateAnimObject_7E8D0(animStruct, *portPosX, *portPosY, *posx, *posy, 4, 4);
						} else {
							*posx = mapScreenPortals_E17CC[jx].viewPortPosX_4;
							*posy = mapScreenPortals_E17CC[jx].viewPortPosY_6;
							if (*posy > 480)
								*posy = 480;
							if (*posx > 640)
								*posx = 640;
							*a5 = 3;
							if (jx < 24) {
								x_DWORD_17DE28str.x_WORD_17DE30_posx = *posx;
								x_DWORD_17DE28str.x_WORD_17DE32_posy = *posy;
								x_DWORD_17DE28str.x_BYTE_17DE34 = ((x_D41A0_BYTEARRAY_4_struct.setting_byte3_24 & 0x40) != 0) + 1;
								x_DWORD_17DE28str.time_17DE28 = j___clock();
							}
						}
						break;
					}
				}
				if (*a5 != 1 || x_DWORD_17DB70str.x_BYTE_17DB8E)
					MapMenuPortalsDraw_81760();
			} else {
				for (int jx = 0; mapScreenPortals_E17CC[jx].viewPortPosX_4 != 0; jx++) {
					if (mapScreenPortals_E17CC[jx].activated_18 == 2) {
						if (jx < 24) {
							x_DWORD_17DE28str.x_WORD_17DE30_posx = *posx;
							x_DWORD_17DE28str.x_WORD_17DE32_posy = *posy;
							x_DWORD_17DE28str.x_BYTE_17DE34 = ((x_D41A0_BYTEARRAY_4_struct.setting_byte3_24 & 0x40) != 0) + 1;
							x_DWORD_17DE28str.time_17DE28 = j___clock();
						}
						break;
					}
				}
				*a5 = 3;
				MapMenuPortalsDraw_81760();
			}
			break;
		case 2:
			if (MoveAnimObject_7E9D0(posx, posy, animStruct)) {
				*a5 = 3;
				memset(&x_DWORD_17DE28str, 0, 13);
				x_DWORD_17DE28str.x_WORD_17DE30_posx = *posx;
				x_DWORD_17DE28str.x_WORD_17DE32_posy = *posy;
				x_DWORD_17DE28str.x_BYTE_17DE34 = ((x_D41A0_BYTEARRAY_4_struct.setting_byte3_24 & 0x40) != 0) + 1;
				x_DWORD_17DE28str.time_17DE28 = j___clock();
			}
			break;
		case 3:
			if (!x_DWORD_17DB70str.x_BYTE_17DB8E) {
				map_not_moving_WORD_E29D6 = true;
				if (x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx) {
					if (x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx >= MOUSE_MAX_X) {
						*posx += x_DWORD_17DB70str.x_WORD_17DB82_shift_step;
						map_not_moving_WORD_E29D6 = false;
					}
				} else {
					*posx -= x_DWORD_17DB70str.x_WORD_17DB82_shift_step;
					map_not_moving_WORD_E29D6 = false;
				}

				if (x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony) {
					if (x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony >= MOUSE_MAX_Y) {
						*posy += x_DWORD_17DB70str.x_WORD_17DB82_shift_step;
						map_not_moving_WORD_E29D6 = false;
					}
				} else {
					*posy -= x_DWORD_17DB70str.x_WORD_17DB82_shift_step;
					map_not_moving_WORD_E29D6 = false;
				}
				*posx = std::clamp(*posx, MOUSE_MIN, MOUSE_MAX_X);
				*posy = std::clamp(*posy, MOUSE_MIN, MOUSE_MAX_Y);
				if (map_not_moving_WORD_E29D6) {
					x_DWORD_17DB70str.x_WORD_17DB82_shift_step = 0;
				} else {
					x_DWORD_17DB70str.x_WORD_17DB82_shift_step += 4;
					if (x_DWORD_17DB70str.x_WORD_17DB82_shift_step > 24)
						x_DWORD_17DB70str.x_WORD_17DB82_shift_step = 24;
				}
			}
			break;
		case 4:
			if (x_DWORD_17DB70str.x_WORD_17DB8A > 24) {
				Type_SecretMapScreenPortals_E2970 *tempPortal = GetSecretAndActivedPortal2_824E0(x_DWORD_17DB70str.x_WORD_17DB8A);
				if (tempPortal) {
					*posx = mapScreenPortals_E17CC[tempPortal->index_4].viewPortPosX_4;
					*posy = mapScreenPortals_E17CC[tempPortal->index_4].viewPortPosY_6;
				}
			} else {
				*posx = mapScreenPortals_E17CC[x_DWORD_17DB70str.x_WORD_17DB8A].viewPortPosX_4;
				*posy = mapScreenPortals_E17CC[x_DWORD_17DB70str.x_WORD_17DB8A].viewPortPosY_6;
			}

			if (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode || x_DWORD_17DE38str.x_WORD_17DEEE_mouse_buttons) {
				*a5 = 1;
				x_DWORD_17DE38str.x_WORD_17DEEE_mouse_buttons = 0;
				x_DWORD_17DE38str.x_BYTE_17DF11_last_key_status = 0;
				x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode = 0;
			}
			index4 = -1;
			for (int i = 0; mapScreenPortals_E17CC[i].viewPortPosX_4; i++) {
				if (mapScreenPortals_E17CC[i].activated_18 == 2) {
					index4 = i;
					break;
				}
			}
			int tempPosX;
			if (index4 != -1) {
				mapScreenPortals_E17CC[index4 - 1].activated_18 = 2;
				tempPosX = mapScreenPortals_E17CC[index4].viewPortPosX_4;
				mapScreenPortals_E17CC[index4].viewPortPosX_4 = 0;
			}
			MapMenuPortalsDraw_81760();
			if (index4 != -1) {
				mapScreenPortals_E17CC[index4 - 1].activated_18 = 1;
				mapScreenPortals_E17CC[index4].viewPortPosX_4 = tempPosX;
			}
			break;
		case 5:
			if (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode || x_DWORD_17DE38str.x_WORD_17DEEE_mouse_buttons) {
				x_DWORD_17DE38str.x_WORD_17DEEE_mouse_buttons = 0;
				*a5 = 3;
				x_DWORD_17DE38str.x_BYTE_17DF11_last_key_status = 0;
				x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode = 0;
			}
			break;
		default:
			break;
	}
	if (*a5 != 1 || x_DWORD_17DB70str.x_BYTE_17DB8E || (*a5 == 1 && x_DWORD_17DB70str.x_WORD_17DB8A != -1)) {
		if (map_not_moving_WORD_E29D6) {
			DrawNetGameMapBackground_85C8B(x_DWORD_17DE38str.x_DWORD_17DE64_game_world_map, pdwScreenBuffer_351628, *posx, *posy, 160, 480);
			DrawAnimTextsAndPlaySounds_7D400(*posx, *posy, *a5);
			DrawAndSoundDragonAndFire_81EE0(*posx, *posy);
			if (x_DWORD_17DB70str.x_BYTE_17DB8E) {
				result = sub_80D40_move_graphics_and_play_sounds(*posx, *posy, x_DWORD_17DB70str.x_WORD_17DB84, x_DWORD_17DB70str.x_WORD_17DB86, x_DWORD_17DB70str.x_WORD_17DB88);
				if (result) {
					x_DWORD_17DB70str.x_BYTE_17DB8E = 0;
				}
			}
			sub_85CC3_draw_round_frame((uint16_t *)x_DWORD_17DE38str.x_DWORD_17DE5C_border_bitmap);
			if (*a5 == 4) {
				DrawEndGameTable_82C20(x_DWORD_17DB70str.x_WORD_17DB8A);
			} else if (*a5 == 5) {
				DrawEndGameTable_82C20(x_DWORD_17DB70str.x_WORD_17DB8C);
			}
		} else {
			DrawNetGameMapBackground_85C8B(x_DWORD_17DE38str.x_DWORD_17DE64_game_world_map, pdwScreenBuffer_351628, *posx, *posy, 160, 480);
			DrawAnimTextsAndPlaySounds_7D400(*posx, *posy, *a5);
			DrawAndSoundDragonAndFire_81EE0(*posx, *posy);
			if (x_DWORD_17DB70str.x_BYTE_17DB8E) {
				result = sub_80D40_move_graphics_and_play_sounds(*posx, *posy, x_DWORD_17DB70str.x_WORD_17DB84, x_DWORD_17DB70str.x_WORD_17DB86, x_DWORD_17DB70str.x_WORD_17DB88);
				if (result)
					x_DWORD_17DB70str.x_BYTE_17DB8E = 0;
			}
			sub_85CC3_draw_round_frame((unsigned __int16 *)x_DWORD_17DE38str.x_DWORD_17DE5C_border_bitmap);
			map_not_moving_WORD_E29D6 = true;
			if (*a5 == 4) {
				DrawEndGameTable_82C20(x_DWORD_17DB70str.x_WORD_17DB8A);
			} else if (*a5 == 5) {
				DrawEndGameTable_82C20(x_DWORD_17DB70str.x_WORD_17DB8C);
			}
		}
		result2 = sub_7E320_draw_bitmaps_and_play_sounds();
		if (!x_DWORD_17DB70str.x_BYTE_17DB8E && !result && !result2) {
			if (*a5 == 3) {
				if (x_DWORD_17DE38str.x_BYTE_17DF11_last_key_status == 111 || x_DWORD_17DE38str.x_BYTE_17DF11_last_key_status == 79) {
					if (x_DWORD_17DE28str.x_BYTE_17DE34 == 1) {
						x_DWORD_17DE28str.time_17DE28 = x_DWORD_17DE28str.time2_17DE2C;
						x_BYTE_17E09D = x_DWORD_17DE28str.x_BYTE_17DE34;
						x_DWORD_17DE28str.x_BYTE_17DE34 = 2;
					} else if (x_DWORD_17DE28str.x_BYTE_17DE34 == 2) {
						int time = j___clock();
						x_DWORD_17DE28str.time2_17DE2C = time;
						x_DWORD_17DE28str.x_BYTE_17DE34 = 1;
						x_DWORD_17DE28str.time_17DE28 = time;
						x_BYTE_17E09D = 0;
					}
				}
				if (x_DWORD_17DE28str.x_BYTE_17DE34 == 1) {
					int time = j___clock();
					x_DWORD_17DE28str.time2_17DE2C = time;
					if ((time - x_DWORD_17DE28str.time_17DE28) / 0x64u > 0xF) {
						x_DWORD_17DE28str.time_17DE28 = time;
						x_DWORD_17DE28str.x_BYTE_17DE34 = 2;
					}
					int textIndex = (x_DWORD_17DE28str.x_WORD_17DE32_posy < MOUSE_MAX_Y) ? 280 : 60;
					DrawText_80C30(130, textIndex, 380);
				} else if (x_DWORD_17DE28str.x_BYTE_17DE34 <= 2u || x_DWORD_17DE28str.x_BYTE_17DE34 == 3) {
					DrawText_80C30(0, 0, 0);
				}
			}
		}
		if (result2 == 2 && !result && !x_DWORD_17DB70str.x_BYTE_17DB8E)
			result = result2;

		if (!result2 && !result) {
			if (*a5 == 3 && !result && !x_DWORD_17DB70str.x_BYTE_17DB8E) {
				if (x_DWORD_17DE38str.x_WORD_17DEEE_mouse_buttons & 1) {
					bool found = false;
					for (int i = 0; mapScreenPortals_E17CC[i].viewPortPosX_4; i++) {
						selectVar.xmin_10 = mapScreenPortals_E17CC[i].portalPosX_12 - *posx;
						selectVar.ymin_12 = mapScreenPortals_E17CC[i].portalPosY_14 - *posy;
						selectVar.sizex_14 = mapScreenPortals_E17CC[i].word_8;
						selectVar.sizey_16 = mapScreenPortals_E17CC[i].word_10;
						if (InRegion_7B200(&selectVar, x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx, x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony)) {
							x_DWORD_17DB70str.x_WORD_17DB84 = mapScreenPortals_E17CC[i].portalPosX_12;
							x_DWORD_17DB70str.x_WORD_17DB86 = mapScreenPortals_E17CC[i].portalPosY_14;
							x_DWORD_17DB70str.x_WORD_17DB88 = mapScreenPortals_E17CC[i].activated_18;
							sub_80D40_move_graphics_and_play_sounds(*posx, *posy, mapScreenPortals_E17CC[i].portalPosX_12, mapScreenPortals_E17CC[i].portalPosY_14, mapScreenPortals_E17CC[i].activated_18);
							x_DWORD_17DB70str.x_BYTE_17DB8E = 1;
							x_D41A0_BYTEARRAY_4_struct.levelnumber_43w = i;
							if (mapScreenPortals_E17CC[i].activated_18 == 1)
								x_D41A0_BYTEARRAY_4_struct.setting_38545 |= 4u;
							Type_SecretMapScreenPortals_E2970 *tempPortal = GetSecretAndActivedPortal_824B0(x_D41A0_BYTEARRAY_4_struct.levelnumber_43w);
							if (tempPortal && tempPortal->activated_12 == 2)
								x_D41A0_BYTEARRAY_4_struct.setting_38545 |= 0x10u;
							if (x_D41A0_BYTEARRAY_4_struct.levelnumber_43w == 24)
								x_D41A0_BYTEARRAY_4_struct.setting_38545 |= 0x20u;
							found = true;
							break;
						}
						if (mapScreenPortals_E17CC[i].activated_18 == 2)
							break;
					}
					if (!found && !result && !x_DWORD_17DB70str.x_BYTE_17DB8E) {
						for (int i = 0; secretMapScreenPortals_E2970[i].posX_8; i++) {
							if (secretMapScreenPortals_E2970[i].activated_12 != 3) {
								selectVar.xmin_10 = secretMapScreenPortals_E2970[i].posX_8 - *posx;
								selectVar.ymin_12 = secretMapScreenPortals_E2970[i].posY_10 - *posy;
								selectVar.sizex_14 = 40;
								selectVar.sizey_16 = 40;
								if (InRegion_7B200(&selectVar, x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx, x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony)) {
									if (secretMapScreenPortals_E2970[i].activated_12 == 1)
										x_D41A0_BYTEARRAY_4_struct.setting_38545 |= 4u;
									x_DWORD_17DB70str.x_WORD_17DB84 = secretMapScreenPortals_E2970[i].posX_8;
									x_DWORD_17DB70str.x_WORD_17DB86 = secretMapScreenPortals_E2970[i].posY_10;
									x_DWORD_17DB70str.x_WORD_17DB88 = 1;
									sub_80D40_move_graphics_and_play_sounds(*posx, *posy, secretMapScreenPortals_E2970[i].posX_8, secretMapScreenPortals_E2970[i].posY_10, 1);
									x_D41A0_BYTEARRAY_4_struct.levelnumber_43w = secretMapScreenPortals_E2970[i].levelNumber_6;
									x_DWORD_17DB70str.x_BYTE_17DB8E = 1;
									break;
								}
							}
						}
					}
				} else if (x_DWORD_17DE38str.x_WORD_17DEEE_mouse_buttons & 2) {
					bool found_r = false;
					for (int i = 0; mapScreenPortals_E17CC[i].viewPortPosX_4; i++) {
						selectVar.xmin_10 = mapScreenPortals_E17CC[i].portalPosX_12 - *posx;
						selectVar.ymin_12 = mapScreenPortals_E17CC[i].portalPosY_14 - *posy;
						selectVar.sizex_14 = mapScreenPortals_E17CC[i].word_8;
						selectVar.sizey_16 = mapScreenPortals_E17CC[i].word_10;
						if (InRegion_7B200(&selectVar, x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx, x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony)) {
							if (mapScreenPortals_E17CC[i].activated_18 == 1) {
								x_DWORD_17DB70str.x_WORD_17DB8C = i;
								*a5 = 5;
								x_DWORD_17DE38str.x_WORD_17DEEE_mouse_buttons = 0;
							}
							found_r = true;
							break;
						}
					}
					if (!found_r && *a5 != 5) {
						for (int kk = 0; secretMapScreenPortals_E2970[kk].posX_8; kk++) {
							if (secretMapScreenPortals_E2970[kk].activated_12 != 3) {
								selectVar.xmin_10 = secretMapScreenPortals_E2970[kk].posX_8 - *posx;
								selectVar.ymin_12 = secretMapScreenPortals_E2970[kk].posY_10 - *posy;
								selectVar.sizex_14 = 40;
								selectVar.sizey_16 = 40;
								if (InRegion_7B200(&selectVar, x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx, x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony)) {
									if (secretMapScreenPortals_E2970[kk].activated_12 == 1) {
										*a5 = 5;
										x_DWORD_17DB70str.x_WORD_17DB8C = secretMapScreenPortals_E2970[kk].levelNumber_6;
									}
									break;
								}
							}
						}
					}
				}
			}
			if (!result && x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode == 1)
				result = 2;
		}
	}
	return result;
}


void NewGameDialog_77350_mod_Step() {
			//skip while (!endAction) {
			//skip g_state_monitor.Update();
			SetFrameStart(std::chrono::system_clock::now());
			if (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode == 59) {
				x_DWORD_17DE38str.showHelp_17DF13 = x_D41A0_BYTEARRAY_4_struct.showHelp_10 != 1;
				x_D41A0_BYTEARRAY_4_struct.showHelp_10 = x_D41A0_BYTEARRAY_4_struct.showHelp_10 != 1;
				x_D41A0_BYTEARRAY_4_struct.setting_38402 = 1;
			}
			NewGameDialog_endAction_mod = NewGameDraw_7EAE0_mod(
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
