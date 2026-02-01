#include "remc2_mod.h"

int NewGameDialog_endAction_mod;

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
		x_BYTE_D41C0 = 1;
	nextMenu_E29D8 = MenuItem::SetToIntro;*/
}

void sub_7A110_load_hscreen_mod(char a1, char a2) //25b110
{
	char dataPath[MAX_PATH];
	//int v2; // eax

	x_DWORD_17DE38str.palette_17DE38x = (TColor *)*xadatapald0dat2.colorPalette_var28;
	x_DWORD_17DE38str.x_WORD_17DEEC = 0;
	x_DWORD_17DE38str.x_DWORD_17DE40 = pdwScreenBuffer_351628;
	x_DWORD_17DE38str.x_DWORD_17DEE0_filedesc = NULL;
	x_DWORD_17DE38str.x_DWORD_17DEDC = 0;
	x_DWORD_17DE38str.x_DWORD_17DE48c = x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226;
	//skip FadeClearBlit_7B5D0(); //25c5d0

	sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/SCREENS/HSCREEN0.DAT");

	if (a1 == 8) {
		switch (a2) {
			case 4:
				x_DWORD_17DE38str.x_DWORD_17DED8 = (bitmap_pos_struct2_t *)&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 0x36f44]; //(uint8_t*)&loc_36F44 + x_DWORD_17DE48c;
				x_DWORD_17DE38str.x_DWORD_17DED4 = (bitmap_pos_struct2_t *)&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 0x36c44]; //(uint8_t*)&loc_36CAA + x_DWORD_17DE48c;
				x_DWORD_17DE38str.x_DWORD_17DE4C = &x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 0x36f44 + 2]; //(uint8_t*)&loc_36F44 + x_DWORD_17DE48c + 2;
				x_DWORD_17DE38str.x_DWORD_17DE54 = &x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 0x36f44 + 16386]; //(uint8_t*)&loc_36F44 + x_DWORD_17DE48c + 16386;
				x_DWORD_17DE38str.x_DWORD_17DEC4 = (bitmap_pos_struct2_t *)&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 0x36f44 + 24631]; //(uint8_t*)&loc_36F44 + x_DWORD_17DE48c + 24631;
				x_DWORD_17DE38str.x_DWORD_17DE50 = (char *)&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 0x36f44 + 24631]; //(uint8_t*)&loc_36F44 + x_DWORD_17DE48c + 24631;
				x_DWORD_17DE38str.x_DWORD_17DEC0 = (bitmap_pos_struct2_t *)&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 0x36f44 + 22999]; //(uint8_t*)&loc_36F44 + x_DWORD_17DE48c + 22999;
				qmemcpy(&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 0x36f44 + 24631],
						//(char *)&loc_36F44 + x_DWORD_17DE48c + 24631,
						D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].WizardName_0x39f_2BFA_12157,
						0x20u);
				x_DWORD_E9C38_smalltit = (uint8_t *)x_DWORD_17DE38str.x_DWORD_17DE50 + 32;
				sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.palette_17DE38x, 0, 768);
				sub_7AA70_load_and_decompres_dat_file(dataPath, x_DWORD_E9C38_smalltit, x_DWORD_17DE38str.x_DWORD_17DEDC, 168081);
				sub_7AA70_load_and_decompres_dat_file(dataPath, x_DWORD_17DE38str.x_DWORD_17DE48c, x_DWORD_17DE38str.x_DWORD_17DEDC, 102213);
				sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.x_DWORD_17DED4, x_DWORD_17DE38str.x_DWORD_17DEDC, 411); //19b
				sub_7AA70_load_and_decompres_dat_file(dataPath, x_DWORD_17DE38str.x_DWORD_17DE54, 0x13ACCA, 1226);
				sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.x_DWORD_17DEC0, 0x13B194, 548); //25ba70
				sub_7AA70_load_and_decompres_dat_file(0, 0, 0, 0);
				if (x_WORD_180660_VGA_type_resolution & 1)
					sub_98709_create_index_dattab_power(x_DWORD_17DE38str.x_DWORD_17DED4, x_DWORD_17DE38str.x_DWORD_17DED8, x_DWORD_17DE38str.x_DWORD_17DE48c, xy_DWORD_17DED4_spritestr);
				else
					sub_9874D_create_index_dattab(x_DWORD_17DE38str.x_DWORD_17DED4, x_DWORD_17DE38str.x_DWORD_17DED8, x_DWORD_17DE38str.x_DWORD_17DE48c, xy_DWORD_17DED4_spritestr);
				if (x_WORD_180660_VGA_type_resolution & 1)
					sub_98709_create_index_dattab_power(x_DWORD_17DE38str.x_DWORD_17DEC0, x_DWORD_17DE38str.x_DWORD_17DEC4, x_DWORD_17DE38str.x_DWORD_17DE54, xy_DWORD_17DEC0_spritestr);
				else
					sub_9874D_create_index_dattab(x_DWORD_17DE38str.x_DWORD_17DEC0, x_DWORD_17DE38str.x_DWORD_17DEC4, x_DWORD_17DE38str.x_DWORD_17DE54, xy_DWORD_17DEC0_spritestr);
				x_DWORD_17DE38str.x_WORD_17DF06 = 72;
				x_DWORD_17DE38str.x_WORD_17DF08 = 68;
				x_DWORD_17DE38str.x_WORD_17DF0A = 69;
				x_DWORD_17DE38str.x_WORD_17DF0C = 70;
				x_DWORD_17DE38str.x_WORD_17DF0E = 71;
				break;
			case 6: //adress 25b533
				x_DWORD_17DE38str.x_DWORD_17DE54 = &x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 301787]; //(uint8_t*)x_DWORD_17DE48c + 301787;//49adb * //44ffef
				x_DWORD_17DE38str.x_DWORD_17DEC4 = (bitmap_pos_struct2_t *)&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 316179]; //(uint8_t*)x_DWORD_17DE48c + 316179;//4d313 //453827
				x_DWORD_17DE38str.x_DWORD_17DE58 = &x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 316179]; //x_DWORD_17DE48c + 316179; * //453827
				x_DWORD_17DE38str.x_DWORD_17DEC8 = (bitmap_pos_struct2_t *)&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 322754]; //x_DWORD_17DE48c + 322754; * //4551d6
				x_DWORD_17DE38str.x_DWORD_17DECC = (bitmap_pos_struct2_t *)&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 324380]; //x_DWORD_17DE48c + 324380; //455830
				x_DWORD_17DE38str.x_DWORD_17DED4 = (bitmap_pos_struct2_t *)&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 324380]; //(uint8_t*)x_DWORD_17DE48c + 324380; *//455830
				x_DWORD_17DE38str.x_DWORD_17DED8 = (bitmap_pos_struct2_t *)&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 326258]; //(uint8_t*)x_DWORD_17DE48c + 326258; //455f86
				x_DWORD_17DE38str.x_DWORD_17DE60 = x_DWORD_17DE38str.x_DWORD_17DE44; //* //3f52a4
				x_DWORD_17DE38str.x_DWORD_17DE64_game_world_map = &x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 326258]; //x_DWORD_17DE48c + 326258; * //455f86
				x_DWORD_17DE38str.x_DWORD_17DE3C = (TcolNext *)&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 0x12C000 + 326258]; //(uint8_t*)&unk_12C000 + x_DWORD_17DE48c + 326258;//* //581f86
				x_DWORD_17DE38str.x_DWORD_17DEC0 = (bitmap_pos_struct2_t *)&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 314541]; //(uint8_t*)x_DWORD_17DE48c + 314541; * //4531c1
				x_DWORD_17DE38str.x_DWORD_17DE5C_border_bitmap = &x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 0x12C000 + 326258 + 0x4000]; //x_DWORD_17DE3C + 0x4000;//ok *//585f86
				sub_7AA70_load_and_decompres_dat_file(dataPath, x_DWORD_17DE38str.x_DWORD_17DE54, 0x1641FC, 1214); //*
				sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.x_DWORD_17DEC0, 0x1646BA, 589);
				sub_7AA70_load_and_decompres_dat_file(dataPath, x_DWORD_17DE38str.x_DWORD_17DE58, 0x164907, 1191);
				sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.x_DWORD_17DEC8, 0x164DAE, 543); //xx
				sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.palette_17DE38x, 0x13A9CA, 768); //ok
				sub_7AA70_load_and_decompres_dat_file(dataPath, x_DWORD_17DE38str.x_DWORD_17DE64_game_world_map, 0xB2C47, 0x87D83);
				sub_7AA70_load_and_decompres_dat_file(dataPath, x_DWORD_17DE38str.x_DWORD_17DE48c, 0x783BD, 103577); //ok
				sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.x_DWORD_17DED4, 0x91856, 1027);
				sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.x_DWORD_17DE3C, 0x13B3B8, 6760);
				sub_7AA70_load_and_decompres_dat_file(dataPath, x_DWORD_17DE38str.x_DWORD_17DE60, 0x13CE20, 20581);
				sub_7AA70_load_and_decompres_dat_file(dataPath, x_DWORD_17DE38str.x_DWORD_17DE5C_border_bitmap, 0x141E85, 13195);
				sub_7AA70_load_and_decompres_dat_file(0, 0, 0, 0);
				if (x_WORD_180660_VGA_type_resolution & 1) {
					sub_98709_create_index_dattab_power(x_DWORD_17DE38str.x_DWORD_17DED4, x_DWORD_17DE38str.x_DWORD_17DED8, x_DWORD_17DE38str.x_DWORD_17DE48c, xy_DWORD_17DED4_spritestr);
					sub_98709_create_index_dattab_power(x_DWORD_17DE38str.x_DWORD_17DEC0, x_DWORD_17DE38str.x_DWORD_17DEC4, x_DWORD_17DE38str.x_DWORD_17DE54, xy_DWORD_17DEC0_spritestr); //here
					sub_98709_create_index_dattab_power(x_DWORD_17DE38str.x_DWORD_17DEC8, x_DWORD_17DE38str.x_DWORD_17DECC, x_DWORD_17DE38str.x_DWORD_17DE58, xy_DWORD_17DEC8_spritestr);
				} else {
					sub_9874D_create_index_dattab(x_DWORD_17DE38str.x_DWORD_17DED4, x_DWORD_17DE38str.x_DWORD_17DED8, x_DWORD_17DE38str.x_DWORD_17DE48c, xy_DWORD_17DED4_spritestr);
					sub_9874D_create_index_dattab(x_DWORD_17DE38str.x_DWORD_17DEC0, x_DWORD_17DE38str.x_DWORD_17DEC4, x_DWORD_17DE38str.x_DWORD_17DE54, xy_DWORD_17DEC0_spritestr); //here
					//34eec0 34eec4 34ee54
					sub_9874D_create_index_dattab(x_DWORD_17DE38str.x_DWORD_17DEC8, x_DWORD_17DE38str.x_DWORD_17DECC, x_DWORD_17DE38str.x_DWORD_17DE58, xy_DWORD_17DEC8_spritestr);
				}

				x_DWORD_17DE38str.x_WORD_17DF06 = 254;
				x_DWORD_17DE38str.x_WORD_17DF08 = 255;
				x_DWORD_17DE38str.x_WORD_17DF0A = 256;
				x_DWORD_17DE38str.x_WORD_17DF0C = 257;
				x_DWORD_17DE38str.x_WORD_17DF0E = 258;
				//x_DWORD_17DE38str.x_DWORD_17DEC0 += 6;
				x_DWORD_17DE38str.x_DWORD_17DEC0++;
				xy_DWORD_17DEC0_spritestr = &xy_DWORD_17DEC0_spritestr[1];
				break;
			case 7:
				x_DWORD_17DE38str.x_DWORD_17DE48c += 371200;
				x_DWORD_17DE38str.x_DWORD_17DE54 = x_DWORD_17DE38str.x_DWORD_17DE48c + 42428;
				x_DWORD_17DE38str.x_DWORD_17DEC4 = (bitmap_pos_struct2_t *)(x_DWORD_17DE38str.x_DWORD_17DE48c + 50673);
				x_DWORD_17DE38str.x_DWORD_17DED4 = (bitmap_pos_struct2_t *)(x_DWORD_17DE38str.x_DWORD_17DE48c + 50673);
				x_DWORD_17DE38str.x_DWORD_17DED8 = (bitmap_pos_struct2_t *)(x_DWORD_17DE38str.x_DWORD_17DE48c + 50871);
				x_DWORD_17DE38str.x_DWORD_17DE3C = (TcolNext *)(x_DWORD_17DE38str.x_DWORD_17DE48c + 50871);
				x_DWORD_17DE38str.x_DWORD_17DE4C = x_DWORD_17DE38str.x_DWORD_17DE48c + 51639;
				x_DWORD_17DE38str.x_DWORD_17DEC0 = (bitmap_pos_struct2_t *)(x_DWORD_17DE38str.x_DWORD_17DE48c + 49041);
				x_DWORD_17DE38str.x_DWORD_17DE50 = (char *)x_DWORD_17DE38str.x_DWORD_17DE48c + 68023;
				sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.palette_17DE38x, 0x42471, 768);
				sub_7AA70_load_and_decompres_dat_file(dataPath, x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226, x_DWORD_17DE38str.x_DWORD_17DEDC, 82233);
				sub_7AA70_load_and_decompres_dat_file(dataPath, x_DWORD_17DE38str.x_DWORD_17DE48c, 0x568AA, 21216);
				sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.x_DWORD_17DED4, 375690, 148);
				sub_7AA70_load_and_decompres_dat_file(dataPath, x_DWORD_17DE38str.x_DWORD_17DE54, 0x13ACCA, 1226);
				sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.x_DWORD_17DEC0, 0x13B194, 548);
				if (x_WORD_180660_VGA_type_resolution & 1) {
					sub_98709_create_index_dattab_power(x_DWORD_17DE38str.x_DWORD_17DED4, x_DWORD_17DE38str.x_DWORD_17DED8, x_DWORD_17DE38str.x_DWORD_17DE48c, xy_DWORD_17DED4_spritestr);
					sub_98709_create_index_dattab_power(x_DWORD_17DE38str.x_DWORD_17DEC0, x_DWORD_17DE38str.x_DWORD_17DEC4, x_DWORD_17DE38str.x_DWORD_17DE54, xy_DWORD_17DEC0_spritestr);
				} else {
					sub_9874D_create_index_dattab(x_DWORD_17DE38str.x_DWORD_17DED4, x_DWORD_17DE38str.x_DWORD_17DED8, x_DWORD_17DE38str.x_DWORD_17DE48c, xy_DWORD_17DED4_spritestr);
					sub_9874D_create_index_dattab(x_DWORD_17DE38str.x_DWORD_17DEC0, x_DWORD_17DE38str.x_DWORD_17DEC4, x_DWORD_17DE38str.x_DWORD_17DE54, xy_DWORD_17DEC0_spritestr);
				}
				break;
			case 12:
				sub_7AA70_load_and_decompres_dat_file(dataPath, x_DWORD_E9C38_smalltit, 0x91C59, 134382);
				sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.palette_17DE38x, 0xB2947, 768);
				break;
			case 14:
				/*
				34EE48->55CCE4[4D54A]

				*/
				//[x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226+0x4D54A]

				x_DWORD_17DE38str.x_DWORD_17DE54 = &x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 224426];
				x_DWORD_17DE38str.x_DWORD_17DEC4 = (bitmap_pos_struct2_t *)&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 232671];
				x_DWORD_17DE38str.x_DWORD_17DED4 = (bitmap_pos_struct2_t *)&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 232671];
				x_DWORD_17DE38str.x_DWORD_17DED8 = (bitmap_pos_struct2_t *)&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 233337];
				x_DWORD_17DE38str.x_DWORD_17DE4C = &x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 233339];
				x_DWORD_17DE38str.x_DWORD_17DE50 = (char *)&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 249723];
				x_DWORD_17DE38str.x_DWORD_17DEC0 = (bitmap_pos_struct2_t *)&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 231039];
				qmemcpy(
						&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A + 249723],
						D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].WizardName_0x39f_2BFA_12157,
						0x20u); //fix it
				sub_7AA70_load_and_decompres_dat_file(dataPath, x_DWORD_17DE38str.x_DWORD_17DE54, 0x13ACCA, 1226);
				sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.x_DWORD_17DEC0, 0x13B194, 548);
				sub_7AA70_load_and_decompres_dat_file(dataPath, &x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A], 168849, 102213);
				sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.x_DWORD_17DED4, 271062, 411); //19b
				sub_7AA70_load_and_decompres_dat_file(dataPath, x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226, 0x91C59, 134382);
				sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.palette_17DE38x, 0xB2947, 768);
				if (x_WORD_180660_VGA_type_resolution & 1) {
					sub_98709_create_index_dattab_power(x_DWORD_17DE38str.x_DWORD_17DED4, x_DWORD_17DE38str.x_DWORD_17DED8, &x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A], xy_DWORD_17DED4_spritestr);
					sub_98709_create_index_dattab_power(x_DWORD_17DE38str.x_DWORD_17DEC0, x_DWORD_17DE38str.x_DWORD_17DEC4, x_DWORD_17DE38str.x_DWORD_17DE54, xy_DWORD_17DEC0_spritestr);
				} else {
					sub_9874D_create_index_dattab(x_DWORD_17DE38str.x_DWORD_17DED4, x_DWORD_17DE38str.x_DWORD_17DED8, &x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[0x4D54A], xy_DWORD_17DED4_spritestr);
					sub_9874D_create_index_dattab(x_DWORD_17DE38str.x_DWORD_17DEC0, x_DWORD_17DE38str.x_DWORD_17DEC4, x_DWORD_17DE38str.x_DWORD_17DE54, xy_DWORD_17DEC0_spritestr);
				}

				//xy_DWORD_17DED4_spritestr[37]
				//-34eed4
				// 3dc74800d7c94800472c0b00050000004001c800
				// 48c73d - 000000000000020000005d5fc2220000
				//-34eed8
				// d7c94800
				//48c9d7(rozdil 29a) - 000000000000000000000000000000000
				//-34ee48
				//5e3a4500
				//453e5e - 6f005d3f3a13423a13200b

				//pote
				//-34eed4
				// 3dc74800d7c94800472c0b00050000004001c800
				// 48c73d - 5e3a45000000603a45

				//dale [0034eed4]->48c73d->5e3a4500 ->6f005d3f3a13
				//+ d2=48c80f->47ae48 -> 26c2c2c2c2

				//48ae47002633 //47ae48->26c2c2c2c2c2

				/*
				34ee48=354f78->5e3a45
				34eed4=354f70->48c73d->000000000000000000

				*/
				break;
			case 15:
				sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_E9C38_smalltit, 0x145210, 126188);
				sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.palette_17DE38x, 0x163EFC, 0x300);
				break;
			default:
				return;
		}
	}
}

bool NewGameDialog_77350_mod_Begin()
{

	bool result = false;

	StopMusic_8E020(); //26f020 //!!!!!!!!ADDED
	StartMusic_8E160(4, 0x7Fu); //26f160 //!!!!!!!!ADDED

	//pdwScreenBuffer_351628 = NULL; //!!!!!!!!ADDED ONLY FOR TESTS

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
		sub_7A110_load_hscreen_mod(x_WORD_180660_VGA_type_resolution, 6); //here init sprites
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



void sub_7C120_draw_bitmap_640_mod(int16_t posx, int16_t posy, bitmap_pos_struct_t tempstr, __int16 posx2, __int16 posy2, int index,int type=0) //25d120
{
	//DrawBitmap_2BB40(posx, posy, tempstr); //ebp
	//save posx,posy,tempstr,posx2,posy2,index
	graphics_queue_add_action("drawBitmap", posx2, posy2, index,type);
}

void DrawAnimSprite_81CA0_mod(__int16 posx, __int16 posy, type_x_BYTE_E25ED_db_str *a5x) //262ca0
{
	int time = j___clock();
	if (a5x->byte_20 == 0) {
		if (!a5x->byte_20) {
			a5x->byte_20 = 2;
			a5x->time1_0 = time;
			a5x->time2_4 = time;
		}
	} else if (a5x->byte_20 == 1) {
		if ((time - a5x->time2_4) >> 3 >= 1) {
			if (a5x->frameIndex_16 <= a5x->lastFrame_14 - 2) {
				a5x->frameIndex_16++;
			} else {
				if (a5x->byte_21 == 1) {
					a5x->byte_20 = 0;
				}
				a5x->frameIndex_16 = a5x->firstFrame_12;
			}
			a5x->time2_4 = time;
		}
		sub_7C120_draw_bitmap_640_mod(
				a5x->x1_8 - posx,
				a5x->y1_10 - posy,
				xy_DWORD_17DED4_spritestr[a5x->frameIndex_16], a5x->x1_8, a5x->y1_10, a5x->frameIndex_16);
		if (x_DWORD_17DE38str.x_WORD_17DEEE_mouse_buttons & 1)
			a5x->frameIndex_16++;
	} else if (a5x->byte_20 == 2) {
		time = (time - a5x->time1_0) / 0x64;
		if (time > a5x->time3_18) {
			time = a5x->time4_22;
			a5x->byte_20 = 1;
			if (time != -1)
				sub_8F100_sound_proc19(
						0,
						a5x->word_24,
						(unsigned __int8)x_BYTE_E1324,
						64,
						0x64u,
						a5x->word_28,
						a5x->word_26);
		}
	}
}

bool DrawFrameAnim_7E5A0_mod(__int16 posx, __int16 posy, Type_MapScreenPortals_E17CC *mapPortal, __int16 beginIndex, __int16 endIndex) //25f5a0
{
	int actTime = j___clock();
	bool result = false;
	if ((actTime - mapPortal->time_0) >> 3 >= 1) {
		if (mapPortal->spriteIndex_16 <= endIndex) {
			mapPortal->spriteIndex_16++;
		} else {
			result = true;
			mapPortal->spriteIndex_16 = beginIndex;
		}
		mapPortal->time_0 = actTime;
	}
	sub_7C120_draw_bitmap_640_mod(mapPortal->portalPosX_12 - posx, mapPortal->portalPosY_14 - posy, xy_DWORD_17DED4_spritestr[mapPortal->spriteIndex_16], mapPortal->portalPosX_12, mapPortal->portalPosY_14, mapPortal->spriteIndex_16);
	return result;
}

void sub_7E840_draw_textbox_with_line_mod(type_E24BCx *a1x, __int16 posx2, __int16 posy2) {
	//save posx,posy,tempstr,posx2,pos2,index
	graphics_queue_add_action("drawTextBox", posx2, posy2, a1x->textIndex_0,0);
}

void DrawAnimTextsAndPlaySounds_7D400_mod(__int16 posx, __int16 posy, char a4) //25e400
{
	type_E24BCx textBoxStr[2];
	Type_MapScreenPortals_E17CC mapPortal;

	int index2 = -1;
	int index3 = -1;
	int time = j___clock();
	for (int i = 0; x_BYTE_E26C8_str[i].x1_8; i++) {
		if (mapScreenPortals_E17CC[24].activated_18 != 1 || x_BYTE_E26C8_str[i].firstFrame_12 != 85 && x_BYTE_E26C8_str[i].firstFrame_12 != 86)
			DrawAnimSprite_81CA0_mod(posx, posy, &x_BYTE_E26C8_str[i]);
	}
	//VGA_Debug_Blit(640, 480, pdwScreenBuffer_351628);
	for (int i = 0; mapScreenPortals_E17CC[i].viewPortPosX_4; i++) //draw new game flag
	{
		if (mapScreenPortals_E17CC[i].activated_18 == 1) {
			index2 = i;
			DrawFrameAnim_7E5A0_mod(posx, posy, &mapScreenPortals_E17CC[i], 37, 43);
		} else if (mapScreenPortals_E17CC[i].activated_18 == 2) {
			if (a4 == 3 || a4 == 5) {
				index3 = i;
				if (mapScreenPortals_E17CC[i].byte_19) {
					if (mapScreenPortals_E17CC[i].byte_19 == 1) {
						if (DrawFrameAnim_7E5A0_mod(posx, posy, &mapScreenPortals_E17CC[i], 70, 83)) {
							mapScreenPortals_E17CC[i].spriteIndex_16 = 33;
							mapScreenPortals_E17CC[i].byte_19 = 2;
						}
					} else {
						DrawFrameAnim_7E5A0_mod(posx, posy, &mapScreenPortals_E17CC[i], 33, 35);
					}
				} else {
					//Play Level Portal Opening Sound
					sub_8F100_sound_proc19(0, 41, 127, 64, 0x64u, 0, 2u);
					mapScreenPortals_E17CC[i].byte_19 = 1;
				}
			}
		}
		if (mapScreenPortals_E17CC[i].activated_18 == 2)
			break;
	}
	for (int i = 0; secretMapScreenPortals_E2970[i].activated_12; i++) {
		if (secretMapScreenPortals_E2970[i].activated_12 < 1u)
			continue;
		if (secretMapScreenPortals_E2970[i].activated_12 == 1u) {
			memset(&mapPortal, 0, 22);
			mapPortal.time_0 = secretMapScreenPortals_E2970[i].time_0;
			mapPortal.spriteIndex_16 = secretMapScreenPortals_E2970[i].spriteIndex_14;
			mapPortal.portalPosX_12 = secretMapScreenPortals_E2970[i].posX_8;
			mapPortal.portalPosY_14 = secretMapScreenPortals_E2970[i].posY_10;
			DrawFrameAnim_7E5A0_mod(posx, posy, &mapPortal, 305, 311);
			secretMapScreenPortals_E2970[i].time_0 = mapPortal.time_0;
			secretMapScreenPortals_E2970[i].spriteIndex_14 = mapPortal.spriteIndex_16;
			continue;
		}
		if (secretMapScreenPortals_E2970[i].activated_12 == 2 && a4 == 3) {
			if (!secretMapScreenPortals_E2970[i].byte_16) {
				sub_8F100_sound_proc19(0, 41, 127, 64, 0x64u, 0, 2u);
				secretMapScreenPortals_E2970[i].byte_16 = 1;
				continue;
			}
			if (secretMapScreenPortals_E2970[i].byte_16 != 1) {
				memset(&mapPortal, 0, 22);
				mapPortal.time_0 = secretMapScreenPortals_E2970[i].time_0;
				mapPortal.spriteIndex_16 = secretMapScreenPortals_E2970[i].spriteIndex_14;
				mapPortal.portalPosX_12 = secretMapScreenPortals_E2970[i].posX_8;
				mapPortal.portalPosY_14 = secretMapScreenPortals_E2970[i].posY_10;
				DrawFrameAnim_7E5A0_mod(posx, posy, &mapPortal, 270, 272);
				secretMapScreenPortals_E2970[i].time_0 = mapPortal.time_0;
				secretMapScreenPortals_E2970[i].spriteIndex_14 = mapPortal.spriteIndex_16;
				continue;
			}
			memset(&mapPortal, 0, 22);
			mapPortal.time_0 = secretMapScreenPortals_E2970[i].time_0;
			mapPortal.spriteIndex_16 = secretMapScreenPortals_E2970[i].spriteIndex_14;
			mapPortal.portalPosX_12 = secretMapScreenPortals_E2970[i].posX_8;
			mapPortal.portalPosY_14 = secretMapScreenPortals_E2970[i].posY_10;
			if (DrawFrameAnim_7E5A0_mod(posx, posy, &mapPortal, 70, 83)) {
				secretMapScreenPortals_E2970[i].spriteIndex_14 = 270;
				secretMapScreenPortals_E2970[i].byte_16 = 2;
			} else {
				secretMapScreenPortals_E2970[i].spriteIndex_14 = mapPortal.spriteIndex_16;
			}
			secretMapScreenPortals_E2970[i].time_0 = mapPortal.time_0;
		}
	}
	if (a4 == 3 && x_D41A0_BYTEARRAY_4_struct.showHelp_10 == 1) {
		bitmap_pos_struct2_t *tempPosStr1 = x_DWORD_17DE38str.x_DWORD_17DEC0;
		bitmap_pos_struct2_t *tempPosStr2 = x_DWORD_17DE38str.x_DWORD_17DEC4;
		x_DWORD_17DE38str.x_DWORD_17DEC0 = x_DWORD_17DE38str.x_DWORD_17DEC8;
		x_DWORD_17DE38str.x_DWORD_17DEC4 = x_DWORD_17DE38str.x_DWORD_17DECC;
		bitmap_pos_struct_t *tempSpriteStr = xy_DWORD_17DEC0_spritestr;
		xy_DWORD_17DEC0_spritestr = xy_DWORD_17DEC8_spritestr;
		if (x_DWORD_17DB70str.x_WORD_17DB74 == 0) {
			if (!x_DWORD_17DB70str.x_WORD_17DB74) {
				x_DWORD_17DB70str.x_WORD_17DB74 = (index3 != -1) + 1;
				x_DWORD_17DB70str.time_17DB70 = j___clock();
			}
		} else if (x_DWORD_17DB70str.x_WORD_17DB74 == 1u) {
			if (index2 != -1) {
				if (3 == x_DWORD_17DB70str.x_BYTE_17DB8F) {
					memset(textBoxStr, 0, 36);
					textBoxStr[0] = str_E24F2[0];
					textBoxStr[0].minx2_2 = mapScreenPortals_E17CC[index2].portalPosX_12 - 80 - posx;
					textBoxStr[0].miny2_4 = mapScreenPortals_E17CC[index2].portalPosY_14 - 60 - posy;
					textBoxStr[0].minx_6 = mapScreenPortals_E17CC[index2].portalPosX_12 + 16 - posx;
					textBoxStr[0].miny_8 = mapScreenPortals_E17CC[index2].portalPosY_14 - 60 - posy;
					textBoxStr[0].maxx_12 = mapScreenPortals_E17CC[index2].portalPosX_12 + 16 - posx;
					textBoxStr[0].maxy_14 = mapScreenPortals_E17CC[index2].portalPosY_14 - 4 - posy;
					textBoxStr[0].textIndex_0 = 464;
					sub_7E840_draw_textbox_with_line_mod(textBoxStr, 238, 264);
					if (index3 != -1) {
						if ((time - x_DWORD_17DB70str.time_17DB70) / 0x64u > 8) {
							x_DWORD_17DB70str.time_17DB70 = time;
							x_DWORD_17DB70str.x_WORD_17DB74 = 2;
						}
					}
				}
			}
		} else if (x_DWORD_17DB70str.x_WORD_17DB74 == 2) {
			if (index3 != -1) {
				if (x_DWORD_17DB70str.x_BYTE_17DB8F == 3) {
					memset(textBoxStr, 0, 36);
					textBoxStr[0].minx2_2 = mapScreenPortals_E17CC[index3].portalPosX_12 - 80 - posx;
					textBoxStr[0].miny2_4 = mapScreenPortals_E17CC[index3].portalPosY_14 - 60 - posy;
					textBoxStr[0].minx_6 = mapScreenPortals_E17CC[index3].portalPosX_12 + 16 - posx;
					textBoxStr[0].miny_8 = mapScreenPortals_E17CC[index3].portalPosY_14 - 60 - posy;
					textBoxStr[0].maxx_12 = mapScreenPortals_E17CC[index3].portalPosX_12 + 16 - posx;
					textBoxStr[0].maxy_14 = mapScreenPortals_E17CC[index3].portalPosY_14 - 4 - posy;
					textBoxStr[0].textIndex_0 = 465;
					sub_7E840_draw_textbox_with_line_mod(textBoxStr, 238, 264);
					if (index2 != -1) {
						if ((time - x_DWORD_17DB70str.time_17DB70) / 0x64u > 8) {
							x_DWORD_17DB70str.time_17DB70 = time;
							x_DWORD_17DB70str.x_WORD_17DB74 = 1;
						}
					}
				}
			}
		}
		x_DWORD_17DE38str.x_DWORD_17DEC0 = tempPosStr1;
		x_DWORD_17DE38str.x_DWORD_17DEC4 = tempPosStr2;
		xy_DWORD_17DEC0_spritestr = tempSpriteStr;
	}
	if (x_DWORD_17DB70str.x_BYTE_17DB8E)
		return;

	if (x_DWORD_17DB70str.x_WORD_17DB8A != -1) {
		int index4 = -1;
		for (int i = 0; mapScreenPortals_E17CC[i].viewPortPosX_4; i++) {
			if (mapScreenPortals_E17CC[i].activated_18 == 1)
				index4 = i;
		}
		for (int i = 0; mapScreenPortals_E17CC[i].viewPortPosX_4; i++) {
			if (i == x_DWORD_17DB70str.x_WORD_17DB8A) {
				SetAnimationVariables_7DA70(mapScreenPortals_E17CC[i].portalPosX_12, mapScreenPortals_E17CC[i].portalPosY_14, mapScreenPortals_E17CC[index4].portalPosX_12, mapScreenPortals_E17CC[index4].portalPosY_14, posx, posy);
				return;
			}
		}
		for (int i = 0; secretMapScreenPortals_E2970[i].activated_12; i++) {
			if (x_DWORD_17DB70str.x_WORD_17DB8A == secretMapScreenPortals_E2970[i].levelNumber_6) {
				SetAnimationVariables_7DA70(secretMapScreenPortals_E2970[i].posX_8, secretMapScreenPortals_E2970[i].posY_10, mapScreenPortals_E17CC[secretMapScreenPortals_E2970[i].index_4].portalPosX_12, mapScreenPortals_E17CC[secretMapScreenPortals_E2970[i].index_4].portalPosY_14, posx, posy);
				return;
			}
		}
		return;
	}
	if ((index2 != -1) && (index3 != -1)) {
		SetAnimationVariables_7DA70(mapScreenPortals_E17CC[index2].portalPosX_12, mapScreenPortals_E17CC[index2].portalPosY_14, mapScreenPortals_E17CC[index3].portalPosX_12, mapScreenPortals_E17CC[index3].portalPosY_14, posx, posy);
		return;
	}
	if ((index2 != -1) && (index3 != -1))
		return;
	if (index2 >= 0) {
		SetAnimationVariables_7DA70(mapScreenPortals_E17CC[index2].portalPosX_12, mapScreenPortals_E17CC[index2].portalPosY_14, mapScreenPortals_E17CC[23].portalPosX_12, mapScreenPortals_E17CC[23].portalPosY_14, posx, posy);
	}
}

void DrawAndSoundDragonAndFire_81EE0_mod(__int16 a5, __int16 a6) //262ee0
{
	bool objectResult = false;
	signed __int16 result; // ax
	uint8_t *temp_screen_buffer; // ST1C_4

	int time = j___clock();
	int index = 0;
	for (result = str_WORD_E20A4[0].array_word_18.beginX_2; result; result = str_WORD_E20A4[index].array_word_18.beginX_2) {
		if (str_WORD_E20A4[index].byte_42 == 1) {
			switch (str_WORD_E20A4[index].byte_43) {
				case 0u:
					str_WORD_E20A4[index].time_0 = time;
					str_WORD_E20A4[index].byte_43++;
					break;
				case 1u:
					if ((time - str_WORD_E20A4[index].time_0) / 0x64u > str_WORD_E20A4[index].array_word_18.stateY_11)
						str_WORD_E20A4[index].byte_43++;
					break;
				case 2u:
					str_WORD_E20A4[index].array_word_18.actX_0 = str_WORD_E20A4[index].array_word_18.beginX_2;
					str_WORD_E20A4[index].array_word_18.actY_1 = str_WORD_E20A4[index].array_word_18.beginY_3;
					CreateAnimObject_7E8D0(&str_WORD_E20A4[index].array_word_45, str_WORD_E20A4[index].array_word_18.maxX_4, str_WORD_E20A4[index].array_word_18.maxY_5, str_WORD_E20A4[index].array_word_18.beginX_2, str_WORD_E20A4[index].array_word_18.beginY_3, 2, 2);
					str_WORD_E20A4[index].array_word_18.act_7 = str_WORD_E20A4[index].array_word_18.begin_6;
					str_WORD_E20A4[index].byte_43++;
					sub_8F100_sound_proc19(0, str_WORD_E20A4[index].word_10, x_BYTE_E1324, 64, 0x64u, 0, str_WORD_E20A4[index].byte_12);
					break;
				case 3u:
					for (int i = 0; i < 4; i++) {
						objectResult = MoveAnimObject_7E9D0(&str_WORD_E20A4[index].array_word_18.actX_0, &str_WORD_E20A4[index].array_word_18.actY_1, &str_WORD_E20A4[index].array_word_45);
					}
					if ((unsigned int)(time - str_WORD_E20A4[index].time_0) >> 3 >= 1) {
						if (str_WORD_E20A4[index].array_word_18.act2_8 <= str_WORD_E20A4[index].array_word_18.act_7 - 1)
							str_WORD_E20A4[index].array_word_18.act2_8++;
						else
							str_WORD_E20A4[index].array_word_18.act2_8 = str_WORD_E20A4[index].array_word_18.begin_6;
						str_WORD_E20A4[index].time_0 = time;
					}
					sub_7C120_draw_bitmap_640_mod(str_WORD_E20A4[index].array_word_18.actX_0 - a5, str_WORD_E20A4[index].array_word_18.actY_1 - a6, xy_DWORD_17DED4_spritestr[str_WORD_E20A4[index].array_word_18.act2_8], str_WORD_E20A4[index].array_word_18.actX_0, str_WORD_E20A4[index].array_word_18.actY_1, str_WORD_E20A4[index].array_word_18.act2_8);
					if (objectResult) {
						str_WORD_E20A4[index].array_word_18.act2_8 = str_WORD_E20A4[index].array_word_18.begin2_9;
						str_WORD_E20A4[index].time_0 = time;
						str_WORD_E20A4[index].byte_43++;
						sub_8F100_sound_proc19(0, str_WORD_E20A4[index].word_14, x_BYTE_E1324, 64, 0x64u, 0, str_WORD_E20A4[index].byte_16);
					}
					break;
				case 4u:
					objectResult = false;
					if ((unsigned int)(time - str_WORD_E20A4[index].time_0) >> 3 >= 1) {
						if (str_WORD_E20A4[index].array_word_18.act2_8 <= str_WORD_E20A4[index].array_word_18.stateX_10 - 1) {
							str_WORD_E20A4[index].array_word_18.act2_8++;
						} else {
							objectResult = true;
							str_WORD_E20A4[index].array_word_18.act2_8 = str_WORD_E20A4[index].array_word_18.stateX_10;
						}
						str_WORD_E20A4[index].time_0 = time;
					}
					if (!objectResult) {
						sub_7C120_draw_bitmap_640_mod(str_WORD_E20A4[index].array_word_18.actX_0 - a5, str_WORD_E20A4[index].array_word_18.actY_1 - a6, xy_DWORD_17DED4_spritestr[str_WORD_E20A4[index].array_word_18.act2_8], str_WORD_E20A4[index].array_word_18.actX_0, str_WORD_E20A4[index].array_word_18.actY_1, str_WORD_E20A4[index].array_word_18.act2_8);
					} else {
						str_WORD_E20A4[index].byte_43 = 0;
					}
					break;
			}
		} else if (str_WORD_E20A4[index].byte_42 == 2u) {
			if (x_DWORD_17DB70str.x_BYTE_17DB8F != 4) {
				int i = 0;
				for (i = 0; mapScreenPortals_E17CC[i].viewPortPosX_4 && mapScreenPortals_E17CC[i].activated_18 != 2; i++)
					;
				if (i == 25)
					i = 24;
				if (i == str_WORD_E20A4[index].byte_44) {
					switch (str_WORD_E20A4[index].byte_43) {
						case 0:
							str_WORD_E20A4[index].time_0 = time;
							str_WORD_E20A4[index].byte_43++;
							break;
						case 1:
							str_WORD_E20A4[index].array_word_18.actX_0 = str_WORD_E20A4[index].array_word_18.beginX_2;
							str_WORD_E20A4[index].array_word_18.actY_1 = str_WORD_E20A4[index].array_word_18.beginY_3;
							CreateAnimObject_7E8D0(&str_WORD_E20A4[index].array_word_45, str_WORD_E20A4[index].array_word_18.maxX_4, str_WORD_E20A4[index].array_word_18.maxY_5, str_WORD_E20A4[index].array_word_18.beginX_2, str_WORD_E20A4[index].array_word_18.beginY_3, 2, 2);
							str_WORD_E20A4[index].array_word_18.act2_8 = str_WORD_E20A4[index].array_word_18.begin_6;
							str_WORD_E20A4[index].byte_43++;
							sub_8F100_sound_proc19(0, str_WORD_E20A4[index].word_10, x_BYTE_E1324, 64, 0x64u, 0, str_WORD_E20A4[index].byte_12);
							break;
						case 2:
							for (int j = 0; j < 4; j++) {
								objectResult = MoveAnimObject_7E9D0(&str_WORD_E20A4[index].array_word_18.actX_0, &str_WORD_E20A4[index].array_word_18.actY_1, &str_WORD_E20A4[index].array_word_45);
							}
							if ((unsigned int)(time - str_WORD_E20A4[index].time_0) >> 3 >= 1) {
								if (str_WORD_E20A4[index].array_word_18.act2_8 <= str_WORD_E20A4[index].array_word_18.act_7 - 1)
									str_WORD_E20A4[index].array_word_18.act2_8++;
								else
									str_WORD_E20A4[index].array_word_18.act2_8 = str_WORD_E20A4[index].array_word_18.begin_6;
								str_WORD_E20A4[index].time_0 = time;
							}
							sub_7C120_draw_bitmap_640_mod(str_WORD_E20A4[index].array_word_18.actX_0 - a5, str_WORD_E20A4[index].array_word_18.actY_1 - a6, xy_DWORD_17DED4_spritestr[str_WORD_E20A4[index].array_word_18.act2_8], str_WORD_E20A4[index].array_word_18.actX_0, str_WORD_E20A4[index].array_word_18.actY_1, str_WORD_E20A4[index].array_word_18.act2_8);
							if (objectResult) {
								str_WORD_E20A4[index].array_word_18.act2_8 = str_WORD_E20A4[index].array_word_18.begin2_9;
								str_WORD_E20A4[index].time_0 = time;
								str_WORD_E20A4[index].byte_43++;
								sub_8F100_sound_proc19(0, str_WORD_E20A4[index].word_14, x_BYTE_E1324, 64, 0x64u, 0, str_WORD_E20A4[index].byte_16);
							}
							break;
						case 3:
							objectResult = false;
							if ((unsigned int)(time - str_WORD_E20A4[index].time_0) >> 3 >= 1) {
								if (str_WORD_E20A4[index].array_word_18.act2_8 <= str_WORD_E20A4[index].array_word_18.stateX_10 - 1) {
									str_WORD_E20A4[index].array_word_18.act2_8++;
								} else {
									objectResult = true;
									str_WORD_E20A4[index].array_word_18.act2_8 = str_WORD_E20A4[index].array_word_18.begin2_9;
								}
								str_WORD_E20A4[index].time_0 = time;
							}
							if (!objectResult) {
								sub_7C120_draw_bitmap_640_mod(str_WORD_E20A4[index].array_word_18.actX_0 - a5, str_WORD_E20A4[index].array_word_18.actY_1 - a6, xy_DWORD_17DED4_spritestr[str_WORD_E20A4[index].array_word_18.act2_8], str_WORD_E20A4[index].array_word_18.actX_0, str_WORD_E20A4[index].array_word_18.actY_1, str_WORD_E20A4[index].array_word_18.act2_8);
							} else {
								str_WORD_E20A4[index].byte_43++;
							}
							break;
						default:
							break;
					}
				} else {
					str_WORD_E20A4[index].byte_43 = 0;
				}
			}
		} else if (str_WORD_E20A4[index].byte_42 == 3) {
			int i = 0;
			for (int i = 0; mapScreenPortals_E17CC[i].viewPortPosX_4 && mapScreenPortals_E17CC[i].activated_18 != 2; i++)
				;
			if (i)
				i--;
			if (i == str_WORD_E20A4[index].byte_44) {
				if (str_WORD_E20A4[index].byte_43 < 1u) {
					if (!str_WORD_E20A4[index].byte_43) {
						str_WORD_E20A4[index].time_0 = time;
						str_WORD_E20A4[index].byte_43++;
					}
				} else if (str_WORD_E20A4[index].byte_43 <= 1u) {
					str_WORD_E20A4[index].array_word_18.actX_0 = str_WORD_E20A4[index].array_word_18.beginX_2;
					str_WORD_E20A4[index].array_word_18.actY_1 = str_WORD_E20A4[index].array_word_18.beginY_3;
					CreateAnimObject_7E8D0(&str_WORD_E20A4[index].array_word_45, str_WORD_E20A4[index].array_word_18.maxX_4, str_WORD_E20A4[index].array_word_18.maxY_5, str_WORD_E20A4[index].array_word_18.beginX_2, str_WORD_E20A4[index].array_word_18.beginY_3, 2, 2);
					str_WORD_E20A4[index].array_word_18.act2_8 = str_WORD_E20A4[index].array_word_18.begin_6;
					str_WORD_E20A4[index].byte_43++;
					sub_8F100_sound_proc19(0, str_WORD_E20A4[index].word_10, (unsigned __int8)x_BYTE_E1324, 64, 0x64u, 0, str_WORD_E20A4[index].byte_12);
				} else if (str_WORD_E20A4[index].byte_43 == 2) {
					for (int k = 0; k < 4; k++) {
						objectResult = MoveAnimObject_7E9D0(&str_WORD_E20A4[index].array_word_18.actX_0, &str_WORD_E20A4[index].array_word_18.actY_1, &str_WORD_E20A4[index].array_word_45);
					}
					if ((unsigned int)(time - str_WORD_E20A4[index].time_0) >> 3 >= 1) {
						if (str_WORD_E20A4[index].array_word_18.act2_8 <= str_WORD_E20A4[index].array_word_18.act_7 - 1)
							++str_WORD_E20A4[index].array_word_18.act2_8;
						else
							str_WORD_E20A4[index].array_word_18.act2_8 = str_WORD_E20A4[index].array_word_18.begin_6;
						str_WORD_E20A4[index].time_0 = time;
					}
					if (mapScreenPortals_E17CC[24].activated_18 == 1) {
						int tempWidth = screenWidth_18062C;
						int tempHeight = screenHeight_180624;
						temp_screen_buffer = pdwScreenBuffer_351628;
						pdwScreenBuffer_351628 = x_DWORD_17DE38str.x_DWORD_17DE64_game_world_map;
						x_DWORD_180648_map_resolution2_x = 1280;
						screenWidth_18062C = 1280;
						x_DWORD_180644_map_resolution2_y = 960;
						screenHeight_180624 = 960;
						sub_7C120_draw_bitmap_640_mod(str_WORD_E20A4[index].array_word_18.actX_0, str_WORD_E20A4[index].array_word_18.actY_1, xy_DWORD_17DED4_spritestr[str_WORD_E20A4[index].array_word_18.act2_8], str_WORD_E20A4[index].array_word_18.actX_0, str_WORD_E20A4[index].array_word_18.actY_1, str_WORD_E20A4[index].array_word_18.act2_8,1);
						screenWidth_18062C = tempWidth;
						screenHeight_180624 = tempHeight;
						x_DWORD_180648_map_resolution2_x = tempWidth;
						x_DWORD_180644_map_resolution2_y = tempHeight;
						pdwScreenBuffer_351628 = temp_screen_buffer;
					} else {
						sub_7C120_draw_bitmap_640_mod(str_WORD_E20A4[index].array_word_18.actX_0 - a5, str_WORD_E20A4[index].array_word_18.actY_1 - a6, xy_DWORD_17DED4_spritestr[str_WORD_E20A4[index].array_word_18.act2_8], str_WORD_E20A4[index].array_word_18.actX_0, str_WORD_E20A4[index].array_word_18.actY_1, str_WORD_E20A4[index].array_word_18.act2_8);
					}
				}
			} else {
				str_WORD_E20A4[index].byte_43 = 0;
			}
		}
		index++;
	}
}

void sub_82510_mod(/*__int16 a1*/ /*, int *a2*/) //263510
{
	int v2; // eax
	int v3; // edx
	__int16 v4; // dx
	//char* v5; // esi
	int v6y; // edx
	int v7; // eax
	int v8; // ecx
	//int result; // eax
	//char v10[36]; // [esp+0h] [ebp-2Ch]
	type_E24BCx v10x[2];
	//char v11[2]; // [esp+10h] [ebp-1Ch]
	bitmap_pos_struct2_t *v12x; // [esp+24h] [ebp-8h]
	//uint8_t* v13; // [esp+28h] [ebp-4h]
	bitmap_pos_struct2_t *v13x; // [esp+28h] [ebp-4h]
	bitmap_pos_struct_t *v13s;

	uint8_t switchbit = 0;

	v13x = x_DWORD_17DE38str.x_DWORD_17DEC0;
	v12x = x_DWORD_17DE38str.x_DWORD_17DEC4;
	x_DWORD_17DE38str.x_DWORD_17DEC0 = x_DWORD_17DE38str.x_DWORD_17DEC8;
	x_DWORD_17DE38str.x_DWORD_17DEC4 = x_DWORD_17DE38str.x_DWORD_17DECC;

	v13s = xy_DWORD_17DEC0_spritestr; //fixed
	xy_DWORD_17DEC0_spritestr = xy_DWORD_17DEC8_spritestr; //fixed

	if (x_D41A0_BYTEARRAY_4_struct.showHelp_10 == 1) {
		switchbit = unk_17DBA8str.x_BYTE_17DBB6; //*((x_BYTE *)a2 + 14);
		if (switchbit == 2) {
			v2 = j___clock();
			v3 = unk_17DBA8str.unk_17DBA8;
			unk_17DBA8str.unk_17DBAC = v2;
			if ((v2 - v3) / 0x64u > 1) {
				v4 = unk_17DBA8str.unk_17DBB4 + 1;
				unk_17DBA8str.unk_17DBB4 = v4;
				//if (!unk_E2516[9 * v4 + 1])
				if (!str_E2516[v4].minx2_2)
					unk_17DBA8str.unk_17DBB4 = 0;
				unk_17DBA8str.unk_17DBA8 = unk_17DBA8str.unk_17DBAC; //a2[0] = a2[1];
			}
			memset(v10x, 0, 36); //3550c0
			//v5 = (char*)&unk_E2516[9 * unk_17DBA8str.unk_17DBB4];
			//v6x = str_E23E0;
			v6y = 0;
			v10x[0] = str_E2516[unk_17DBA8str.unk_17DBB4];
			//qmemcpy(v10x, v5, 0x12u);
			//qmemcpy(&v11, v5 + 0x10u, 2u);
			while (str_E23E0[v6y].byte_25) {
				//if (str_E23E0[v6y].byte_23 && str_E23E0[v6y].byte_22 == (int)(unk_E2516[8 + 9 * unk_17DBA8str.unk_17DBB4] >> 8))// x_BYTE_E2527[18 * *((signed __int16 *)a2 + 6)] )
				if (str_E23E0[v6y].canSelect_23 && str_E23E0[v6y].byte_22 == str_E2516[unk_17DBA8str.unk_17DBB4].byte_16) // x_BYTE_E2527[18 * *((signed __int16 *)a2 + 6)] )
				{
					str_E23E0[v6y].gold_color_24 = 1;
					break;
				}
				//v6 += 44;
				v6y++;
			}
			sub_7E840_draw_textbox_with_line(v10x, 238, 264); //draw help
		} else if (switchbit == 3) {
			v7 = j___clock();
			v8 = unk_17DBA8str.unk_17DBA8;
			unk_17DBA8str.unk_17DBAC = v7;
			if ((v7 - v8) / 0x64u > 1)
				unk_17DBA8str.x_BYTE_17DBB5 = 2;
		}
	}
	x_DWORD_17DE38str.x_DWORD_17DEC0 = v13x;
	x_DWORD_17DE38str.x_DWORD_17DEC4 = v12x;

	xy_DWORD_17DEC0_spritestr = v13s; //fixed
}

//----- (0007E320) --------------------------------------------------------
signed int DrawBitmapAndPlaySound_7E320_mod() //25f320
{
	type_E24BCx textBoxStr[2];
	for (int i = 0; str_E23E0[i].byte_25; i++) //draw buttons
	{
		if (str_E23E0[i].canSelect_23) {
			sub_7C120_draw_bitmap_640_mod(str_E23E0[i].xmin_10, str_E23E0[i].ymin_12, xy_DWORD_17DED4_spritestr[str_E23E0[i].byte_21], str_E23E0[i].xmin_10, str_E23E0[i].ymin_12, str_E23E0[i].byte_21,1);
		}
	}
	if (x_DWORD_17DB70str.x_BYTE_17DB8E)
		return 0;
	int index = 0;
	if (str_E23E0[index].byte_25) {
		do //adress 25f57c
		{
			if (str_E23E0[index].selected_8 && str_E23E0[index].dword_0) {
				uint8_t dialogIndex = GetMapMenuDialogIndex_7E320(str_E23E0[index].dword_0, &str_E23E0[index]);
				if (!dialogIndex) //
					return 1;
				str_E23E0[index].selected_8 = 0;
				ResetMouse_7B5A0();
				return dialogIndex;
			}
			index++;
		} while (str_E23E0[index].byte_25);
	}
	for (int i = 0; str_E23E0[i].byte_25; i++) {
		if (str_E23E0[i].canSelect_23 && str_E23E0[i].gold_color_24)
			sub_7C120_draw_bitmap_640_mod(str_E23E0[i].xmin_10, str_E23E0[i].ymin_12, xy_DWORD_17DED4_spritestr[str_E23E0[i].byte_20], str_E23E0[i].xmin_10, str_E23E0[i].ymin_12, str_E23E0[i].byte_20,1);
		str_E23E0[i].selected_8 = 0;
		str_E23E0[i].gold_color_24 = 0;
	}
	for (int i = 0;; i++) {
		if (!str_E23E0[i].byte_25) {
			sub_82510_mod();
			return 0;
		}
		if (str_E23E0[i].canSelect_23) {
			str_E23E0[i].sizex_14 = xy_DWORD_17DED4_spritestr[str_E23E0[i].byte_20].width_4;
			str_E23E0[i].sizey_16 = xy_DWORD_17DED4_spritestr[str_E23E0[i].byte_20].height_5;
			if (InRegion_7B200(&str_E23E0[i], x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx, x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony)) {
				if (x_DWORD_17DE38str.x_WORD_17DEEE_mouse_buttons & 1) {
					str_E23E0[i].selected_8 = 1;
					ResetMouse_7B5A0();
					sub_8F100_sound_proc19(0, 14, 127, 64, 0x64u, 0, 3u);
				} else {
					str_E23E0[i].gold_color_24 = 1;
					if (x_D41A0_BYTEARRAY_4_struct.showHelp_10 != 1)
						return 0;
					int index2 = 0;
					if (!str_E2516[index2].minx2_2)
						return 0;
					do //adress 25f4e7
					{
						if (str_E2516[index2].byte_17 == str_E23E0[i].byte_22) {
							memset(textBoxStr, 0, 36);
							textBoxStr[0] = str_E2516[index2];
							bitmap_pos_struct_t *tempSpriteStr = xy_DWORD_17DEC0_spritestr;
							bitmap_pos_struct2_t *tempx_DWORD_17DEC4 = x_DWORD_17DE38str.x_DWORD_17DEC4;
							xy_DWORD_17DEC0_spritestr = xy_DWORD_17DEC8_spritestr;
							x_DWORD_17DE38str.x_DWORD_17DEC4 = x_DWORD_17DE38str.x_DWORD_17DECC;
							sub_7E840_draw_textbox_with_line_mod(textBoxStr, 238, 264);
							xy_DWORD_17DEC0_spritestr = tempSpriteStr;
							x_DWORD_17DE38str.x_DWORD_17DEC4 = tempx_DWORD_17DEC4;
							return 0;
						}
						index2++;
					} while (str_E2516[index2].minx2_2);
					return 0;
				}
			}
		}
	}
}

void sub_7FCB0_draw_text_with_border_mod(char *textString, int32_t a3, int32_t a4, int a5, uint8_t a6, unsigned __int8 a7, uint32_t a8, int textIndex) {
	graphics_queue_add_action("drawTextBox", a3, a5, textIndex, 1);
}

void DrawText_80C30_mod(__int16 posX, __int16 posY, __int16 addWidth) //261c30
{
	int index2 = -1;
	for (int i = 0; mapScreenPortals_E17CC[i].viewPortPosX_4; i++) {
		if (mapScreenPortals_E17CC[i].activated_18 == 2) {
			index2 = i;
			break;
		}
	}
	if (posY + posX > 0) {
		for (int ii = 0; secretMapScreenPortals_E2970[ii].activated_12; ii++) {
			if (secretMapScreenPortals_E2970[ii].activated_12 != 3 && index2 == secretMapScreenPortals_E2970[ii].index_4) {
				x_DWORD_17DE28str.x_BYTE_17DE34 = 3;
				return;
			}
		}
		GetFont_6FC50(1);
		uint8_t colorIndex = getPaletteIndex_5BE80(x_DWORD_17DE38str.palette_17DE38x, 0x3Fu, 0x3Fu, 0x3Fu);
		sub_7FCB0_draw_text_with_border_mod(x_DWORD_E9C4C_langindexbuffer[23 + index2], posX + 4 * GetLetterWidth_6FC10(), posX + addWidth - 3 * GetLetterWidth_6FC10(), posY, 5, colorIndex, 1, 23 + index2);
		//"You must explore the outer Netherworlds while you learn its magic. Your first destination is the ancient city of Jahwl."+
	}
	if (x_DWORD_17DE28str.x_BYTE_17DE34 != 3 && x_D41A0_BYTEARRAY_4_struct.setting_byte3_24 & 0x40 && !x_BYTE_17E09D) {
		x_BYTE_17E09D = 1;
		if (index2 != -1)
			sub_86EB0(index2, 0, 0);
	}
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
			DrawAnimTextsAndPlaySounds_7D400_mod(*posx, *posy, *a5);
			DrawAndSoundDragonAndFire_81EE0_mod(*posx, *posy);
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
			//skip DrawNetGameMapBackground_85C8B(x_DWORD_17DE38str.x_DWORD_17DE64_game_world_map, pdwScreenBuffer_351628, *posx, *posy, 160, 480);
			DrawAnimTextsAndPlaySounds_7D400_mod(*posx, *posy, *a5);
			DrawAndSoundDragonAndFire_81EE0_mod(*posx, *posy);
			if (x_DWORD_17DB70str.x_BYTE_17DB8E) {
				result = sub_80D40_move_graphics_and_play_sounds(*posx, *posy, x_DWORD_17DB70str.x_WORD_17DB84, x_DWORD_17DB70str.x_WORD_17DB86, x_DWORD_17DB70str.x_WORD_17DB88);
				if (result)
					x_DWORD_17DB70str.x_BYTE_17DB8E = 0;
			}
			//skip sub_85CC3_draw_round_frame((unsigned __int16 *)x_DWORD_17DE38str.x_DWORD_17DE5C_border_bitmap);
			map_not_moving_WORD_E29D6 = true;
			if (*a5 == 4) {
				DrawEndGameTable_82C20(x_DWORD_17DB70str.x_WORD_17DB8A);
			} else if (*a5 == 5) {
				DrawEndGameTable_82C20(x_DWORD_17DB70str.x_WORD_17DB8C);
			}
		}

		result2 = DrawBitmapAndPlaySound_7E320_mod();
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
					DrawText_80C30_mod(130, textIndex, 380);
				} else if (x_DWORD_17DE28str.x_BYTE_17DE34 <= 2u || x_DWORD_17DE28str.x_BYTE_17DE34 == 3) {
					DrawText_80C30_mod(0, 0, 0);
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

std::string REMC2mod_customLevelPath = "";
bool REMC2mod_isSecretLevel; // al
int16_t REMC2mod_setLevel = -1;
bool REMC2mod_skipMenus = false;

void sub_46830_main_loop_mod_before_cycle(signed int a2, unsigned __int16 a3) //227830
{ //graphics already inited
	int v5; // edx
	std::string customLevelPath = "";
	//unsigned __int8 v9; // al
	//unsigned __int8 v10; // al
	//Type_SecretMapScreenPortals_E2970 *v13; // eax
	v5 = 0;
	x_D41A0_BYTEARRAY_4_struct.setting_30 = 0; //2a51a4
	D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].byte_0x004_2BE0_11234 = 0;
	REMC2mod_setLevel = CommandLineParams.GetSetLevel();
	customLevelPath = CommandLineParams.GetCustomLevelPath();
	if (REMC2mod_setLevel > -1 || customLevelPath.length() > 0)
		REMC2mod_skipMenus = true;
}

bool endAnim = false;

FILE *animTempfile;
Type_SoundEvent_E17CC *tempPSoundEvent;
void PlayInfoFmv_mod_begin(__int16 a1, __int16 a2, Type_SoundEvent_E17CC *pSoundEvent, char *path) {
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

bool PlayInfoFmv_mod_step() {
	if ((LastPressedKey_1806E4 != 1) && (!endAnim) && animTempfile) {
		SetFrameStart(std::chrono::system_clock::now());
		if (x_WORD_17DB5A)
			endAnim = true;
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
	return endAnim;
}

void PlayInfoFmv_mod_end() {
	if (animTempfile) {
		DataFileIO::Close(x_DWORD_17DB38_intro_file_handle);
	}
	//screenWidth_18062C = oldScreenWidth;
	//x_WORD_180660_VGA_type_resolution = old_VGA_type_resolution;
}

void Intros_76D10_mod_begin(char introType) //257d10
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

	sub_2EB40();
	if (soundAble_E3798 && x_D41A0_BYTEARRAY_4_struct.SelectedLangIndex == 2) {
		x_BYTE_D41C1 = 0;
		x_BYTE_D41C0 = 0;
	} else {
		x_BYTE_D41C0 = 1;
		x_BYTE_D41C1 = 1;
	}
	sub_8CD27_set_cursor((*filearray_2aa18c[filearrayindex_POINTERSDATTAB].posistruct)[0]);
	/*skip
	switch (introType) {
		case 0:
#ifndef debug_hide_graphics
//skip		ShowWelcomeScreen_83850();//frog logo and wait
#endif
			PlayInfoFmv(1, 1, str_E17CC_0, introPath); //257160 intro .. 2b27cc
			sub_2EB40();
			x_BYTE_D41C1 = 0;
			x_BYTE_D41C0 = 0;
			while (sub_9A10A_check_keyboard()) {
				LastPressedKey_1806E4 = 0;
				sub_7A060_get_mouse_and_keyboard_events();
			}
			j___delay(50);
			sprintf(introPath, "%s/%s", cdDataPath.c_str(), "INTRO/INTRO2.DAT");
			PlayInfoFmv(1, 1, str_E17CC_0x160, introPath); //E192C
			break;
		case 1:
			PlayInfoFmv(1, 1, str_E17CC_0, introPath);
			break;
		case 2:
			LastPressedKey_1806E4 = 0;
			x_BYTE_D41C1 = 0;
			x_BYTE_D41C0 = 0;
			sprintf(introPath, "%s/%s", cdDataPath.c_str(), "INTRO/INTRO2.DAT");
			PlayInfoFmv(1, 1, str_E17CC_0x160, introPath); //E192C
			break;
	}
*/
}

void Intros_76D10_mod_end(char introType) //257d10
{	
	sub_90B27_VGA_pal_fadein_fadeout(0, 0x10u, 0);
	EndSample_8D8F0();
	StopMusic_8E020();
	FadeClearBlit_7B5D0();
	nextMenu_E29D8 = MenuItem::MainMenu;
	x_BYTE_D41C0 = 0;
	x_BYTE_D41C1 = 0;
	if (sub_9A10A_check_keyboard())
		sub_7A060_get_mouse_and_keyboard_events();
	x_DWORD_17DE38str.x_BYTE_17DF11_last_key_status = 0;
	x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode = 0;
	if (!introType)
		LoadAndSetGraphicsAndPalette_7AC00();
}

void MenusAndIntros_76930_mod_begin(bool skipMenus) //257930
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
		sub_82670();
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

	InitLanguage_76A40();
	SetToIntro_76CF0();
	/*
	do {
		//skip g_state_monitor.Update();
		//2b39d8
		switch (nextMenu_E29D8) {
			case MenuItem::InitLanguage:
				InitLanguage_76A40(); //257A40
				break;
			case MenuItem::SetToIntro:
				SetToIntro_76CF0(); //257cf0 nastavi x_WORD_E29D8 na 3
				break;
			case MenuItem::LeaveX:
				//skip _wcpp_1_unwind_leave__131(); //257d00 asi konec
				break;
			case MenuItem::Intros:
				Intros_76D10(0); //257d10
				nextMenu_E29D8 = MenuItem::MainMenu;
				break;
			case MenuItem::MainMenu:
				MainMenu_76FA0(); //257fa0
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
*/
}

bool NewGameDialog_77350_mod(type_WORD_E1F84 *a1x) //258350
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
		sub_8CD27_set_cursor((*filearray_2aa18c[filearrayindex_POINTERSDATTAB].posistruct)[0]); //dword_EB394

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
		sub_8CD27_set_cursor(xy_DWORD_17DED4_spritestr[239]);
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
				endAction = 1;
			}
			if (x_WORD_180660_VGA_type_resolution & 1)
				sub_90478_VGA_Blit320(menuFps);
			else
				sub_75200_VGA_Blit640(480, menuFps);
			sub_7A060_get_mouse_and_keyboard_events();
		}
		sub_86860_speak_Sound(x_WORD_1803EC);
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
		sub_8CD27_set_cursor((*filearray_2aa18c[filearrayindex_POINTERSDATTAB].posistruct)[0]);
		sub_90B27_VGA_pal_fadein_fadeout(0, 0x10u, 0);
		result = true;
	} else {
		m_ExitMenuLoop_E29DC = 1;
		x_D41A0_BYTEARRAY_4_struct.levelnumber_43w = x_BYTE_D419C_level_num;
		if (a1x)
			a1x->dword_4 = 0;
		result = true;
	}
	return result;
}

void MainMenu_76FA0_mod_begin() //257fa0
{
	//fixed
	help_VGA_type_resolution = 0;
	//fixed
	bool onlyBlit = false;
	uint16_t introIndex = 1;

	sub_86860_speak_Sound(x_WORD_1803EC); //267860
	LoadSounds_84300(0); //265300
	memset(&x_DWORD_17DBB8, 0, 16);
	x_BYTE_17DBC6 = 2;
	x_DWORD_17DE38str.x_WORD_17DF04 = -1;
	x_DWORD_17DE38str.x_DWORD_17DE44 = x_DWORD_E9C38_smalltit;
	SetCenterScreenForFlyAssistant_6EDB0();
	StopMusic_8E020(); //26f020
	StartMusic_8E160(4, 0x7Fu); //26f160
	x_WORD_17DE26 = 0;
	VGA_cleanKeyBuffer();
	if (x_BYTE_E29E1 || x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 & 0x10 || (NewGameDialog_77350(0), !m_ExitMenuLoop_E29DC)) {
		x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 &= 0xEFu;
		sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 4);
		ResetMouse_7B5A0();
		sub_8CD27_set_cursor(xy_DWORD_17DED4_spritestr[39]);
		x_DWORD_17DE38str.showHelp_17DF13 = x_D41A0_BYTEARRAY_4_struct.showHelp_10;
		x_DWORD_17DBB8[0] = j___clock();
		int lastTime = j___clock();
		int16_t tempMousePosX = x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx;
		int16_t tempMousePosY = x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony;
		int scanCode = x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode;
		/*
		while (!m_ExitMenuLoop_E29DC) {
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
			if (DrawAndServe_7B250()) //25c250
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
				sub_90B27_VGA_pal_fadein_fadeout(x_DWORD_17DE38str.palette_17DE38x, 0x20u, 0); //tady
			}
			sub_7A060_get_mouse_and_keyboard_events();
		}
		sub_41BC0();
		D41A0_0.m_GameSettings.m_Display.m_uiScreenSize = 0;
		sub_753D0();
		x_DWORD_E9C38_smalltit = x_DWORD_17DE38str.x_DWORD_17DE44;
		*/
	} /* else {
		sub_41BC0();
		D41A0_0.m_GameSettings.m_Display.m_uiScreenSize = 0;
		sub_753D0();
	}*/
}

void MainMenu_76FA0_mod_end() //257fa0
{
	/*
	//fixed
	help_VGA_type_resolution = 0;
	//fixed
	bool onlyBlit = false;
	uint16_t introIndex = 1;

	sub_86860_speak_Sound(x_WORD_1803EC); //267860
	LoadSounds_84300(0); //265300
	memset(&x_DWORD_17DBB8, 0, 16);
	x_BYTE_17DBC6 = 2;
	x_DWORD_17DE38str.x_WORD_17DF04 = -1;
	x_DWORD_17DE38str.x_DWORD_17DE44 = x_DWORD_E9C38_smalltit;
	SetCenterScreenForFlyAssistant_6EDB0();
	StopMusic_8E020(); //26f020
	StartMusic_8E160(4, 0x7Fu); //26f160
	x_WORD_17DE26 = 0;
	VGA_cleanKeyBuffer();
	if (x_BYTE_E29E1 || x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 & 0x10 || (NewGameDialog_77350(0), !m_ExitMenuLoop_E29DC)) {
		x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 &= 0xEFu;
		sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 4);
		ResetMouse_7B5A0();
		sub_8CD27_set_cursor(xy_DWORD_17DED4_spritestr[39]);
		x_DWORD_17DE38str.showHelp_17DF13 = x_D41A0_BYTEARRAY_4_struct.showHelp_10;
		x_DWORD_17DBB8[0] = j___clock();
		int lastTime = j___clock();
		int16_t tempMousePosX = x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx;
		int16_t tempMousePosY = x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony;
		int scanCode = x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode;
		while (!m_ExitMenuLoop_E29DC) {
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
			if (DrawAndServe_7B250()) //25c250
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
				sub_90B27_VGA_pal_fadein_fadeout(x_DWORD_17DE38str.palette_17DE38x, 0x20u, 0); //tady
			}
			sub_7A060_get_mouse_and_keyboard_events();
		}
		*/
		sub_41BC0();
		D41A0_0.m_GameSettings.m_Display.m_uiScreenSize = 0;
		sub_753D0();
		x_DWORD_E9C38_smalltit = x_DWORD_17DE38str.x_DWORD_17DE44;
		/*
	} else {
		sub_41BC0();
		D41A0_0.m_GameSettings.m_Display.m_uiScreenSize = 0;
		sub_753D0();
	}*/
}

void MainMenu_76FA0_mod() //257fa0
{
	//fixed
	help_VGA_type_resolution = 0;
	//fixed
	bool onlyBlit = false;
	uint16_t introIndex = 1;

	sub_86860_speak_Sound(x_WORD_1803EC); //267860
	LoadSounds_84300(0); //265300
	memset(&x_DWORD_17DBB8, 0, 16);
	x_BYTE_17DBC6 = 2;
	x_DWORD_17DE38str.x_WORD_17DF04 = -1;
	x_DWORD_17DE38str.x_DWORD_17DE44 = x_DWORD_E9C38_smalltit;
	SetCenterScreenForFlyAssistant_6EDB0();
	StopMusic_8E020(); //26f020
	StartMusic_8E160(4, 0x7Fu); //26f160
	x_WORD_17DE26 = 0;
	VGA_cleanKeyBuffer();
	if (x_BYTE_E29E1 || x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 & 0x10 || (NewGameDialog_77350(0), !m_ExitMenuLoop_E29DC)) {
		x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 &= 0xEFu;
		sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 4);
		ResetMouse_7B5A0();
		sub_8CD27_set_cursor(xy_DWORD_17DED4_spritestr[39]);
		x_DWORD_17DE38str.showHelp_17DF13 = x_D41A0_BYTEARRAY_4_struct.showHelp_10;
		x_DWORD_17DBB8[0] = j___clock();
		int lastTime = j___clock();
		int16_t tempMousePosX = x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx;
		int16_t tempMousePosY = x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony;
		int scanCode = x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode;
		while (!m_ExitMenuLoop_E29DC) {
			SetFrameStart(std::chrono::system_clock::now());
			if ((tempMousePosX == x_DWORD_17DE38str.x_DWORD_17DEE4_mouse_positionx) && (tempMousePosY == x_DWORD_17DE38str.x_DWORD_17DEE6_mouse_positiony) && (x_DWORD_17DE38str.x_BYTE_17DF10_get_key_scancode == scanCode)) {
				/*
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
				*/
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
			if (DrawAndServe_7B250()) //25c250
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

void MenusAndIntros_76930_mod_end() //257930
{
	sub_7ADE0(x_BYTE_E29DE);
	if (x_BYTE_E29E1)
		x_BYTE_E29E1 = 0;
	WriteConfigDat_81DB0();
}

void sub_46830_main_loop_mod_begin_cycle() //227830
{ //graphics already inited
	//skip while (1) {
		if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].byte_0x004_2BE0_11234) {
			return;
		}
		sub_48350(); //fix it //229350
		MenusAndIntros_76930_mod_begin(REMC2mod_skipMenus); //set language, intro, menu, atd. //257930
	//skip }
}

int REMC2_tempa2;
int REMC2_tempa3;

void InGameLoop_47320_mod_begin()
{
	x_D41A0_BYTEARRAY_4_struct.paletteMod_51 = 0;
	uint32_t gameTurn = 0;
	D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.word[1] = 0;
	/*
	while (1) {
		if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].byte_0x004_2BE0_11234 || D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 8)
			break; //end level
		DrawAndEventsInGame_47560(REMC2_tempa2, x_DWORD_17DB54_game_turn2);
		if (gameTurn < 2) {
			StopMusic_8E020();
			if (gameTurn == 1)
				StartMusic_8E160(D41A0_0.maptypeMusic_0x235, 0x7Fu);
			gameTurn++;
		}
	}
	sub_90E07_VGA_set_video_mode_640x480_and_Palette((TColor *)*xadatapald0dat2.colorPalette_var28);
	*/
}

void InGameLoop_47320_mod_end()
{
	/*
	x_D41A0_BYTEARRAY_4_struct.paletteMod_51 = 0;
	uint32_t gameTurn = 0;
	D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.word[1] = 0;
	while (1) {
		if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].byte_0x004_2BE0_11234 || D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 8)
			break; //end level
		DrawAndEventsInGame_47560(REMC2_tempa2, x_DWORD_17DB54_game_turn2);
		if (gameTurn < 2) {
			StopMusic_8E020();
			if (gameTurn == 1)
				StartMusic_8E160(D41A0_0.maptypeMusic_0x235, 0x7Fu);
			gameTurn++;
		}
	}
	*/
	sub_90E07_VGA_set_video_mode_640x480_and_Palette((TColor *)*xadatapald0dat2.colorPalette_var28);
}

bool sub_46830_main_loop_mod_end_cycle_part1() //227830
{
	//skip while (1) {
	if (!D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].byte_0x004_2BE0_11234) {
		REMC2mod_isSecretLevel = x_D41A0_BYTEARRAY_4_struct.levelnumber_43w > 24 && x_D41A0_BYTEARRAY_4_struct.levelnumber_43w < 50;
		sub_47FC0_load_screen(REMC2mod_isSecretLevel);
		LevelInitGame_56A30(REMC2mod_setLevel, REMC2mod_customLevelPath);
		if (CommandLineParams.DoAutoChangeRes()) {
			resindex_begin = 0;
		}
		sub_47160();
		return true;
	} else
		return false;
	//skip}
}

bool sub_46830_main_loop_mod_end_cycle_part2() //227830
{
	char dataPath[MAX_PATH];//added
	switch (D41A0_0.terrain_2FECE.MapType) { //added
		case MapType_t::Day: //added
			sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/PALD-0.DAT"); //added
			break; //added
		case MapType_t::Night: //added
			sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/PALN-0.DAT"); //added
			break; //added
		case MapType_t::Cave: //added
			sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/PALC-0.DAT"); //added
			break; //added
	} //added
	DataFileIO::ReadFileAndDecompress(dataPath, xadatapald0dat2.colorPalette_var28);//added
	VGA_Set_Palette(xadatapald0dat2.colorPalette_var28[0], true);//added

	if (!D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].byte_0x004_2BE0_11234) {
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
			//InGameLoop_47320_mod(REMC2_tempa2);			
			return true;
	}
	return false;
}

bool sub_46830_main_loop_mod_end_cycle_part3() //227830
{
		if (m_ptrGameRender != nullptr) {
			delete m_ptrGameRender;
			m_ptrGameRender = nullptr;
		}
		sub_53CC0_close_movie();
		EndSample_8D8F0();
		StopMusic_8E020();
		sub_86860_speak_Sound(x_WORD_1803EC); //get graphics parametres?
		sub_59BF0_sound_proc11_volume();
		sub_90B27_VGA_pal_fadein_fadeout(0, 0x10u, 0);
		if (x_WORD_180660_VGA_type_resolution & 1) {
			ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 320, 200, getPaletteIndex_5BE80((TColor *)*xadatapald0dat2.colorPalette_var28, 0, 0, 0));
		} else {
			ClearGraphicsBuffer_72883((void *)pdwScreenBuffer_351628, 640, 480, getPaletteIndex_5BE80((TColor *)*xadatapald0dat2.colorPalette_var28, 0, 0, 0));
		}
		if (x_WORD_180660_VGA_type_resolution & 1)
			sub_90478_VGA_Blit320(maxGameFps);
		else
			sub_75200_VGA_Blit640(480, maxGameFps);
		if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 2 && !(x_D41A0_BYTEARRAY_4_struct.setting_38545 & 4)) {
			sub_6DB50(1, 0);
		}
		sub_713A0();
		if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 4) {
			sub_56D60(REMC2_tempa3, 0);
			D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] = 4;
		} else if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 2) {
			sub_5C530();
			if (x_D41A0_BYTEARRAY_4_struct.setting_38545 & 0x20)
				sub_6E0D0();
		} else {
			D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] |= 8;
		}
		if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 0x10) {
			if (x_D41A0_BYTEARRAY_4_struct.levelnumber_43w >= 0x18u) {
				if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 2)
					sub_5C530();
				return false;
			}
			if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 2) {
				Type_SecretMapScreenPortals_E2970 *secretMapScreenPortal = GetSecretAndActivedPortal_824B0(REMC2_tempa3);
				if (secretMapScreenPortal) {
					x_D41A0_BYTEARRAY_4_struct.levelnumber_43w = secretMapScreenPortal->levelNumber_6;
					sub_47FC0_load_screen(true);
					LevelInitGame_56A30(REMC2_tempa3);
					sub_47160();
				}
			}
		} else if (D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dw_w_b_0_2BDE_11230.byte[2] & 0xA) {
			return false; //must be here
		}
		return true;
}

void sub_46830_main_loop_mod_end_cycle_part4() //227830
{
	nextMenu_E29D8 = MenuItem::MainMenu;
	REMC2mod_skipMenus = false;
	REMC2mod_setLevel = -1;
	REMC2mod_customLevelPath = "";
}

void sub_main_mod_begin(int argc, char **argv,char *real_cdPathch) {
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
	sprintf(gameFolder, "%sGAME/NETHERW", real_cdPathch); //added
	sprintf(cdFolder, "%sCD_Files", real_cdPathch); //added
	gameDataPath = GetSubDirectoryPath(gameFolder);//added
	cdDataPath = GetSubDirectoryPath(cdFolder);//added
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
	sub_46830_main_loop_mod_before_cycle(v3, v4); //227830


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
}

void Intro_begin(int introType) {
	Intros_76D10_mod_begin(0);

	char introPath[MAX_PATH];
	if (introType == 1) {
		sprintf(introPath, "%s/%s", cdDataPath.c_str(), "INTRO/INTRO.DAT");
		PlayInfoFmv_mod_begin(1, 1, str_E17CC_0, introPath);
	} else if (introType == 2) {
		sprintf(introPath, "%s/%s", cdDataPath.c_str(), "INTRO/INTRO2.DAT");
		PlayInfoFmv_mod_begin(1, 1, str_E17CC_0x160, introPath);
	}
}

/*
structure for animation:
support_begin();//game-REMC2BeginGame
	sub_main_mod_begin((char *)real_cdPath.utf8().get_data());//game
-------------------------------------------------------------------------
		sub_46830_main_loop_mod_begin_cycle();//action-REMC2BeginItem-only first time
-------------------------------------------------------------------------
			Intros_76D10_mod_begin(int animIndex);//action-anim-REMC2BeginAnim
				PlayInfoFmv_mod_begin(1, 1, str_E17CC_0, introPath);//action-anim
-------------------------------------------------------------------------
				PlayInfoFmv_mod_step();//action-anim-step
-------------------------------------------------------------------------
				PlayInfoFmv_mod_end()//action-anim-REMC2EndAnim
			Intros_76D10_mod_end(int animIndex);//action-anim
-------------------------------------------------------------------------
											sub_46830_main_loop_mod_end_cycle();//action - not use this
-------------------------------------------------------------------------
	sub_main_mod_end();//game-REMC2EndGame
support_end();//game
*/

std::vector<GraphicsAction> graphics_queue;

std::vector<GraphicsAction> graphics_queue_get_pending_actions() {
	return graphics_queue;
}

void graphics_queue_add_action(const std::string &action, int x, int y, int index,int type) {
	GraphicsAction ga;
	ga.action = action;
	ga.x = x;
	ga.y = y;
	ga.index = index;
	ga.type = type;
	graphics_queue.push_back(ga);
}
/*
void graphics_queue_add_actionSA(const GraphicsAction &sa) {
	graphics_queue.push_back(sa);
}

void graphics_queue_add_action(const GraphicsAction &sa) {
	graphics_queue.push_back(sa);
}
*/
void graphics_queue_clear() {
	graphics_queue.clear();
}

/*
size_t graphics_queue_size() {
	return graphics_queue.size();
}

bool graphics_queue_empty() {
	return graphics_queue.empty();
}*/
