#pragma once
#ifndef PORT_SDL_VGA_MOUSE_H
#define PORT_SDL_VGA_MOUSE_H

#include <cstdint>
#include <stdio.h>
#include <assert.h>
#include <sstream>
#include <stddef.h>
#include <stdarg.h>
/*
#include "SDL2/SDL.h"

#include "port_sdl_sound.h"
#include "port_filesystem.h"
*/
#define  SCENE_PREAMBLE_MENU  0x1
#define         SCENE_FLIGHT  0x2
#define    SCENE_FLIGHT_MENU  0x3
#define     SCENE_SPELL_MENU  0x4
#define           SCENE_DEAD  0x5

typedef struct
{
	int32_t x;
	int32_t y;
} POSITION;

typedef struct SDL_Color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} SDL_Color;

typedef struct SDL_Rect {
	int x, y;
	int w, h;
} SDL_Rect;

void VGA_Init(int windowWidth, int windowHeight, int gameResWidth, int gameResHeight, bool maintainAspectRatio, int displayIndex);
void VGA_Init(uint32_t flags, int windowWidth, int windowHeight, int gameResWidth, int gameResHeight, bool maintainAspectRatio, int displayIndex);
bool VGA_LoadFont();
void VGA_close();
void VGA_Resize(int width, int height);
void VGA_Blit(uint8_t *srcBuffer);
void SubBlit(uint16_t originalResWidth, uint16_t originalResHeight);
void VGA_Debug_Blit(int width, int height, uint8_t *srcBuffer);
void VGA_Set_Palette(uint8_t *Palettebuffer, bool saveWithoutModification = false);
void VGA_Set_Palette2(uint8_t *Palettebuffer);
void SetPalette(SDL_Color* colors);
void VGA_Write_basic_Palette(uint8_t *Palettebuffer);
uint8_t *VGA_Get_Palette(bool withoutModification=false);
void VGA_Draw_string(char* wrstring);
void VGA_Draw_stringXYtoBuffer(const char* wrstring, int x, int y, uint8_t* buffer, char fontSize = 'M');
void Draw_letter(int letter_number, int pozx, int pozy);
void Draw_letterToBuffer(int letter_number, int pozx, int pozy, uint8_t* buffer, char fontSize = 'M');
//void VGA_Set_Cursor(posistruct posstrcursor,int count);
void VGA_GotoXY(int x, int y);
int VGA_WhereX(); //returns current text cursor (x) coordinate
int VGA_WhereY(); //returns current text cursor (y) coordinate
POSITION VGA_WhereXY();
void Set_basic_Palette0();
void Set_basic_Palette1();
void VGA_Set_file_Palette(char* filename);
int16_t VGA_get_shift_status();
bool VGA_check_standart_input_status();
uint16_t VGA_read_char_from_buffer();
void VGA_Set_mouse(int16_t a1, int16_t a2);
void setPress(bool locpressed, uint16_t loclastchar);

void VGA_mouse_clear_keys();
void VGA_cleanKeyBuffer();
void Draw_debug_matrix1();
void Draw_black();
//std::vector<SDL_Rect> GetDisplays();
SDL_Rect GetDisplayByIndex(uint8_t index);
SDL_Rect FindDisplayByResolution(uint32_t width, uint32_t height);

extern uint8_t LastPressedKey_1806E4; // weak//3516e4
extern int8_t pressedKeys_180664[128]; // idb
extern uint8_t tempVGABuffer[];

#endif //PORT_SDL_VGA_MOUSE
