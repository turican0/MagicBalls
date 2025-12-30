#include "../engine/engine_support.h"
#include "port_sdl_joystick.h"
#include "port_sdl_vga_mouse.h"
#include "port_time.h"

#include <cstdint>

#include "../engine/sub_main_mouse.h"
#include "../engine/read_config.h"
#include "../engine/CommandLineParser.h"

#ifdef USE_DOSBOX
extern DOS_Device* DOS_CON;
#endif //USE_DOSBOX

//SDL_Window* m_window = nullptr;
//SDL_Renderer* m_renderer = nullptr;
//SDL_Texture* m_texture = nullptr;
//SDL_Surface* m_gamePalletisedSurface = nullptr;
//SDL_Surface* m_gameRGBASurface = nullptr;
SDL_Color m_currentPalletColours[256];
uint8_t m_fontBuffer[256 * 256];
//SDL_Surface* m_surfaceFont = nullptr;
uint8_t m_smallFontBuffer[128 * 128];
//SDL_Surface* m_smallSurfaceFont = nullptr;

uint8_t LastPressedKey_1806E4; //3516e4
int8_t pressedKeys_180664[128]; // idb

uint16_t m_iOrigw = 640;
uint16_t m_iOrigh = 480;

uint16_t m_iWindowWidth = 640;
uint16_t m_iWindowHeight = 480;

bool m_bMaintainAspectRatio = true;

bool settingWindowGrabbed = true;
bool settingWASD = false;

const char* default_caption = "Magic Carpet 2 - Community Update";

bool inited = false;
uint8 tempPalettebuffer[768];

int oldWidth;

bool subBlitLock = false;

// Initalize Color Masks.
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
//Uint32 redMask = 0xff000000;
//Uint32 greenMask = 0x00ff0000;
//Uint32 blueMask = 0x0000ff00;
//Uint32 alphaMask = 0x000000ff;
#else
Uint32 redMask = 0x000000ff;
Uint32 greenMask = 0x0000ff00;
Uint32 blueMask = 0x00ff0000;
Uint32 alphaMask = 0xff000000;
#endif

std::vector<SDL_Rect> GetDisplays()
{
	std::vector<SDL_Rect> displayBounds;
	return displayBounds;
}

SDL_Rect GetDisplayByIndex(uint8_t index)
{
	SDL_Rect display;
	display.x = 0;
	display.y = 0;
	display.w = 0;
	display.h = 0;

	std::vector<SDL_Rect> displayBounds = GetDisplays();

	if (index < displayBounds.size())
	{
		return displayBounds[index];
	}

	return display;
}

SDL_Rect FindDisplayByResolution(uint32_t width, uint32_t height)
{
	SDL_Rect display;
	display.x = 0;
	display.y = 0;
	display.w = width;
	display.h = height;

	std::vector<SDL_Rect> displayBounds = GetDisplays();

	for (int i = 0; i < displayBounds.size(); i++) {
		if (width <= displayBounds[i].w && height <= displayBounds[i].h)
			return displayBounds[i];
	}
	return display;
}

void VGA_Init(uint32_t /*flags*/, int windowWidth, int windowHeight, int gameResWidth, int gameResHeight, bool maintainAspectRatio, int displayIndex) {
}

uint8_t *VGA_Get_Palette() {
	return tempPalettebuffer;
}

uint16_t lastResHeight=0;

void SetPalette(SDL_Color* colours) {
}

void Set_basic_Palette0() {
}
void Set_basic_Palette1() {
}

void Set_basic_Palette3() {
}

/*
void putpixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
}
*/

void Draw_debug_matrix0() {
};

void Draw_black() {
};

void Draw_debug_matrix1() {
};

bool VGA_LoadFont()
{
	bool success = true;
	return success;
}

int lastpoz = 0;
int textwidth = 40;
int textheight = 30;

void Draw_letter(int letter_number, int pozx, int pozy) {
};

void Draw_letterToBuffer(int letter_number, int pozx, int pozy, uint8_t* buffer, char fontSize) {
};

void VGA_GotoXY(int x, int y) {
	lastpoz = y * textwidth + x;
};
int VGA_WhereX() {
	return(lastpoz % textwidth);
};
int VGA_WhereY() {
	return((int)(lastpoz / textwidth));
};

POSITION VGA_WhereXY() {
	POSITION result;
	result.x = (lastpoz % textwidth);
	result.y = ((int)(lastpoz / textwidth));
	return result;
};

void VGA_Draw_string(char* wrstring) {
}

int drawCounter = 0;
void VGA_Draw_stringXYtoBuffer(const char* wrstring, int x, int y, uint8_t* buffer, char fontSize) {
}

void VGA_Init(int windowWidth, int windowHeight, int gameResWidth, int gameResHeight, bool maintainAspectRatio, int displayIndex) {
}

SDL_Rect dst;

void VGA_Resize(int width, int height) {
	m_iOrigw = width;
	m_iOrigh = height;
}

FILE* fptpal;
void SavePal(uint8* Palettebuffer, char* filename)
{
}

void VGA_Set_file_Palette(char* filename) {
	uint8_t Palettebuffer[768];
	fptpal = fopen(filename, "rb");
	fread(Palettebuffer, 768, 1, fptpal);
	fclose(fptpal);

	SDL_Color colors[256];
	for (int i = 0; i < 256; i++) {
		colors[i].r = 4 * Palettebuffer[i * 3];
		colors[i].g = 4 * Palettebuffer[i * 3 + 1];
		colors[i].b = 4 * Palettebuffer[i * 3 + 2];
	}
	SetPalette(colors);
}

void VGA_Set_Palette(uint8* Palettebuffer) {
}

void VGA_Set_Palette2(uint8* Palettebuffer) {
}

void VGA_Write_basic_Palette(uint8* Palettebuffer) {
}

void VGA_test() {
}

/*

Keyboard scan code/character code combinations returned in AH/AL by
INT 16h functions 00h/01h (standard) and functions 01h/11h (extended)

Key                        Normal           Shift           Ctrl             Alt
						 Std     Ext     Std     Ext     Std     Ext     Std     Ext
Esc                     01/1B   01/1B   01/1B   01/1B   01/1B   01/1B     -     01/00
1 !                     02/31   02/31   02/21   02/21     -       -     78/00   78/00
2 @                     03/32   03/32   03/40   03/40   03/00   03/00   79/00   79/00
3 #                     04/33   04/33   04/23   04/23     -       -     7A/00   7A/00
4 $                     05/34   05/34   05/24   05/24     -       -     7B/00   7B/00
5 %                     06/35   06/35   06/25   06/25     -       -     7C/00   7C/00
6 ^                     07/36   07/36   07/5E   07/5E   07/1E   07/1E   7D/00   7D/00
7 &                     08/37   08/37   08/26   08/26     -       -     7E/00   7E/00
8 *                     09/38   09/38   09/2A   09/2A     -       -     7F/00   7F/00
9 (                     0A/39   0A/39   0A/28   0A/28     -       -     80/00   80/00
0 )                     0B/30   0B/30   0B/29   0B/29     -       -     81/00   81/00
- _                     0C/2D   0C/2D   0C/5F   0C/5F   0C/1F   0C/1F   82/00   82/00
= +                     0D/3D   0D/3D   0D/2B   0D/2B     -       -     83/00   83/00
Backspace               0E/08   0E/08   0E/08   0E/08   0E/7F   0E/7F     -     0E/00
Tab                     0F/09   0F/09   0F/00   0F/00     -     94/00     -     A5/00
q Q                     10/71   10/71   10/51   10/51   10/11   10/11   10/00   10/00
w W                     11/77   11/77   11/57   11/57   11/17   11/17   11/00   11/00
e E                     12/65   12/65   12/45   12/45   12/05   12/05   12/00   12/00
r R                     13/72   13/72   13/52   13/52   13/12   13/12   13/00   13/00
t T                     14/74   14/74   14/54   14/54   14/14   14/14   14/00   14/00
y Y                     15/79   15/79   15/59   15/59   15/19   15/19   15/00   15/00
u U                     16/75   16/75   16/55   16/55   16/15   16/15   16/00   16/00
i I                     17/69   17/69   17/49   17/49   17/09   17/09   17/00   17/00
o O                     18/6F   18/6F   18/4F   18/4F   18/0F   18/0F   18/00   18/00
p P                     19/70   19/70   19/50   19/50   19/10   19/10   19/00   19/00
[ {                     1A/5B   1A/5B   1A/7B   1A/7B   1A/1B   1A/1B     -     1A/00
] }                     1B/5D   1B/5D   1B/7D   1B/7D   1B/1D   1B/1D     -     1B/00
Enter                   lC/0D   1C/0D   1C/0D   1C/0D   1C/0A   1C/0A     -     1C/00
Ctrl                      *       *       *       *       *       *       *       *
a A                     1E/61   1E/61   1E/41   1E/41   1E/01   1E/01   1E/00   1E/00
s S                     1F/73   1F/73   1F/53   1F/53   1F/13   1F/13   1F/00   1F/00
d D                     20/64   20/64   20/44   20/44   20/04   20/04   20/00   20/00
f F                     21/66   21/66   21/46   21/46   21/06   21/06   21/00   21/00
g G                     22/67   22/67   22/47   22/47   22/07   22/07   22/00   22/00
h H                     23/68   23/68   23/48   23/48   23/08   23/08   23/00   23/00
j J                     24/6A   24/6A   24/4A   24/4A   24/0A   24/0A   24/00   24/00
k K                     25/6B   25/6B   25/4B   25/4B   25/0B   25/0B   25/00   25/00
l L                     26/6C   26/6C   26/4C   26/4C   26/0C   26/0C   26/00   26/00
; :                     27/3B   27/3B   27/3A   27/3A     -       -       -     27/00
' "                     28/27   28/27   28/22   28/22     -       -       -     28/00
` ~                     29/60   29/60   29/7E   29/7E     -       -       -     29/00
Left Shift                *       *       *       *       *       *       *       *
\ |                     2B/5C   2B/5C   2B/7C   2B/7C   2B/1C   2B/1C     -     2B/00
z Z                     2C/7A   2C/7A   2C/5A   2C/5A   2C/1A   2C/1A   2C/00   2C/00
x X                     2D/78   2D/78   2D/58   2D/58   2D/18   2D/18   2D/00   2D/00
c C                     2E/63   2E/63   2E/43   2E/43   2E/03   2E/03   2E/00   2E/00
v V                     2F/76   2F/76   2F/56   2F/56   2F/16   2F/16   2F/00   2F/00
b B                     30/62   30/62   30/42   30/42   30/02   30/02   30/00   30/00
n N                     31/6E   31/6E   31/4E   31/4E   31/0E   31/0E   31/00   31/00
m M                     32/6D   32/6D   32/4D   32/4D   32/0D   32/0D   32/00   32/00
, <                     33/2C   33/2C   33/3C   33/3C     -       -       -     33/00
. >                     34/2E   34/2E   34/3E   34/3E     -       -       -     34/00
/ ?                     35/2F   35/2F   35/3F   35/3F     -       -       -     35/00
Right Shift               *       *       *       *       *       *       *       *
*                       37/2A   37/2A   37/2A   37/2A     -     96/00     -     37/00
Alt                       *       *       *       *       *       *       *       *
Space                   39/20   39/20   39/20   39/20   39/20   39/20   39/20   39/20
Caps Lock                 *       *       *       *       *       *       *       *
F1                      3B/00   3B/00   54/00   54/00   5E/00   5E/00   68/00   68/00
F2                      3C/00   3C/00   55/00   55/00   5F/00   5F/00   69/00   69/00
F3                      3D/00   3D/00   56/00   56/00   60/00   60/00   6A/00   6A/00
F4                      3E/00   3E/00   57/00   57/00   61/00   61/00   6B/00   6B/00
F5                      3F/00   3F/00   58/00   58/00   62/00   62/00   6C/00   6C/00
F6                      40/00   40/00   59/00   59/00   63/00   63/00   6D/00   6D/00
F7                      41/00   41/00   5A/00   5A/00   64/00   64/00   6E/00   6E/00
F8                      42/00   42/00   5B/00   5B/00   65/00   65/00   6F/00   6F/00
F9                      43/00   43/00   5C/00   5C/00   66/00   66/00   70/00   70/00
F10                     44/00   44/00   5D/00   5D/00   67/00   67/00   71/00   71/00
Num Lock                  *       *       *       *       *       *       *       *
Scroll Lock               *       *       *       *       *       *       *       *
Home 7                  47/00   47/00   47/37   47/37   77/00   77/00   00/#    00/#
Up Arrow 8              48/00   48/00   48/38   48/38     -     8D/00   00/#    00/#
PgUp 9                  49/00   49/00   49/39   49/39   84/00   84/00   00/#    00/#
-                       4A/2D   4A/2D   4A/2D   4A/2D     -     8E/00     -     4A/00
Left Arrow 4            4B/00   4B/00   4B/34   4B/34   73/00   73/00   00/#    00/#
Center Key 5              -     4C/00   4C/35   4C/35     -     8F/00   00/#    00/#
Right Arrow 6           4D/00   4D/00   4D/36   4D/36   74/00   74/00   00/#    00/#
+                       4E/2B   4E/2B   4E/2B   4E/2B     -     90/00     -     4E/00
End 1                   4F/00   4F/00   4F/31   4F/31   75/00   75/00   00/#    00/#
Down Arrow 2            50/00   50/00   50/32   50/32     -     91/00   00/#    00/#
PgDn 3                  51/00   51/00   51/33   51/33   76/00   76/00   00/#    00/#
Ins 0                   52/00   52/00   52/30   52/30     -     92/00   00/#    00/#
Del .                   53/00   53/00   53/2E   53/2E     -     93/00     -       -
SysReq (84-key only)      *       *       *       *       *       *       *       *
PrtSc/SysReq (101-key)    *       *       *       *     72/00   72/00     *       *
Pause/Break (101-key)     *       *       *       *     00/00   00/00     *       *
F11 (101-key)             -     85/00     -     87/00     -     89/00     -     8B/00
F12 (101-key)             -     86/00     -     88/00     -     8A/00     -     8C/00
Home (101-key)          47/00   47/E0   47/00   47/E0   77/00   77/E0     -     97/00
Up Arrow (101-key)      48/00   48/E0   48/00   48/E0     -     8D/E0     -     98/00
Page Up (101-key)       49/00   49/E0   49/00   49/E0   84/00   84/E0     -     99/00
Left Arrow (101-key)    4B/00   4B/E0   4B/00   4B/E0   73/00   73/E0     -     9B/00
Right Arrow (101-key)   4D/00   4D/E0   4D/00   4D/E0   74/00   74/E0     -     9D/00
End (101-key)           4F/00   4F/E0   4F/00   4F/E0   75/00   75/E0     -     9F/00
Down Arrow (101-key)    50/00   50/E0   50/00   50/E0     -     91/E0     -     A0/00
Page Down (101-key)     51/00   51/E0   51/00   51/E0   76/00   76/E0     -     A1/00
Insert (101-key)        52/00   52/E0   52/00   52/E0     -     92/E0     -     A2/00
Delete (101-key)        53/00   53/E0   53/00   53/E0     -     93/E0     -     A3/00
/ (101-key)             35/2F   E0/2F   35/2F   E0/2F     -     95/00     -     A4/00
Enter (101-key)         1C/0D   E0/0D   1C/0D   E0/0D   1C/0A   E0/0A     -     A6/00
Right Alt (101-key)       *       *       *       *       *       *       *       *
Right Ctrl (101-key)      *       *       *       *       *       *       *       *
\ | (102-key)           56/5C   56/5C   56/7C   56/7C     -       -       -       -

- Keystroke ignored
* Processed by the BIOS
# Alt-keypad number returns the specified character code and scan code 0
*/

void ToggleFullscreen() {
}

int mousex, mousey;
bool pressed = false;
uint16_t lastchar = 0;
int events()
{
	return 1;
}

void VGA_Set_mouse(const int16_t x, const int16_t y) {
};

void VGA_Blit(uint8* srcBuffer) {
}

void SubBlit(uint16_t originalResWidth, uint16_t originalResHeight) {
}

void VGA_Init_test() {//only for debug
}

void VGA_Debug_Blit(int width, int height, uint8* buffer) {
	VGA_Blit(buffer);
}

void VGA_close()
{
}

int16_t VGA_get_shift_status() {
	return 0;
}
bool VGA_check_standart_input_status() {
	bool locpressed = pressed;
	//uint16_t loclastchar = lastchar;
	pressed = false;
	return locpressed;
}

uint16_t fixchar(uint16_t loclastchar) {
	return loclastchar;
}

void VGA_cleanKeyBuffer() {
	uint16_t loclastchar = lastchar;
	lastchar = 0;
	loclastchar = fixchar(loclastchar);
	while (loclastchar != 0)
	{
		loclastchar = lastchar;
		lastchar = 0;
		loclastchar = fixchar(loclastchar);
	}
}

uint16_t VGA_read_char_from_buffer() {
	return 0;
}

void setPress(bool locpressed, uint16_t loclastchar) {
	loclastchar = fixchar(loclastchar);

	if (locpressed)
	{
		LastPressedKey_1806E4 = (loclastchar & 0xff00) >> 8;// VGA_read_char_from_buffer();
		pressedKeys_180664[LastPressedKey_1806E4 & 0x7F] = LastPressedKey_1806E4;
	}
	else
	{
		pressedKeys_180664[((loclastchar & 0xff00) >> 8) & 0x7F] = 0;
	}
}

void VGA_mouse_clear_keys() {
	for (int i = 0; i < 128; i++)
		pressedKeys_180664[i] = 0;
}
