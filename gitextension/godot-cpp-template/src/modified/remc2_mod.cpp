#include "remc2_mod.h"

int NewGameDialog_endAction_mod;

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

	pdwScreenBuffer_351628 = NULL; //!!!!!!!!ADDED ONLY FOR TESTS

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

void DrawAnimTextsAndPlaySounds_7D400_mod(__int16 posx, __int16 posy, char a4) //25e400
{
	type_E24BCx textBoxStr[2];
	Type_MapScreenPortals_E17CC mapPortal;

	int index2 = -1;
	int index3 = -1;
	int time = j___clock();
	for (int i = 0; x_BYTE_E26C8_str[i].word_8; i++) {
		if (mapScreenPortals_E17CC[24].activated_18 != 1 || x_BYTE_E26C8_str[i].word_12 != 85 && x_BYTE_E26C8_str[i].word_12 != 86)
			sub_81CA0(posx, posy, &x_BYTE_E26C8_str[i]);
	}
	//VGA_Debug_Blit(640, 480, pdwScreenBuffer_351628);
	for (int i = 0; mapScreenPortals_E17CC[i].viewPortPosX_4; i++) //draw new game flag
	{
		if (mapScreenPortals_E17CC[i].activated_18 == 1) {
			index2 = i;
			DrawFrameAnim_7E5A0(posx, posy, &mapScreenPortals_E17CC[i], 37, 43);
		} else if (mapScreenPortals_E17CC[i].activated_18 == 2) {
			if (a4 == 3 || a4 == 5) {
				index3 = i;
				if (mapScreenPortals_E17CC[i].byte_19) {
					if (mapScreenPortals_E17CC[i].byte_19 == 1) {
						if (DrawFrameAnim_7E5A0(posx, posy, &mapScreenPortals_E17CC[i], 70, 83)) {
							mapScreenPortals_E17CC[i].spriteIndex_16 = 33;
							mapScreenPortals_E17CC[i].byte_19 = 2;
						}
					} else {
						DrawFrameAnim_7E5A0(posx, posy, &mapScreenPortals_E17CC[i], 33, 35);
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
			DrawFrameAnim_7E5A0(posx, posy, &mapPortal, 305, 311);
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
				DrawFrameAnim_7E5A0(posx, posy, &mapPortal, 270, 272);
				secretMapScreenPortals_E2970[i].time_0 = mapPortal.time_0;
				secretMapScreenPortals_E2970[i].spriteIndex_14 = mapPortal.spriteIndex_16;
				continue;
			}
			memset(&mapPortal, 0, 22);
			mapPortal.time_0 = secretMapScreenPortals_E2970[i].time_0;
			mapPortal.spriteIndex_16 = secretMapScreenPortals_E2970[i].spriteIndex_14;
			mapPortal.portalPosX_12 = secretMapScreenPortals_E2970[i].posX_8;
			mapPortal.portalPosY_14 = secretMapScreenPortals_E2970[i].posY_10;
			if (DrawFrameAnim_7E5A0(posx, posy, &mapPortal, 70, 83)) {
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
					sub_7E840_draw_textbox_with_line(textBoxStr, 238, 264);
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
					sub_7E840_draw_textbox_with_line(textBoxStr, 238, 264);
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
			//skip DrawNetGameMapBackground_85C8B(x_DWORD_17DE38str.x_DWORD_17DE64_game_world_map, pdwScreenBuffer_351628, *posx, *posy, 160, 480);
			DrawAnimTextsAndPlaySounds_7D400_mod(*posx, *posy, *a5);
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
