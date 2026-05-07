#pragma once
#include <algorithm>
#include "../remc2/engine/EventsFunctions.h"
#include "../remc2/engine/MenusAndIntros.h"
#include "../remc2/engine/PlayerInput.h"
#include "../remc2/engine/DatTabIndexes.h"

#include "../remc2/engine/Graphics.h"
#include "../remc2/engine/Level.h"
#include "../remc2/engine/Network.h"
#include "../remc2/engine/GameUI.h"

struct GraphicsAction {
	std::string action;
	int x = 0;
	int y = 0;
	int index = 0;
	int type = 0;
};

enum class typeStateMenu2 {
	Zero,
	AfterMenu,
	MapMenuSelected,
	ExitGameSelected,
	changeLangSelected,
	setKeysSelected,
	multiplayerSelected,
	animFlvSelected,

	RunGameFromMapMenuSelected,
	ExitMapMenuSelected
};

struct typeStateMenu {
	enum class Name {
		MapMenu,
		MainMenu,
		AnimFlv,
		InGame,
		Zero,
		sub_46830_main_loop_mod
	};

	enum class State {
		Begin,
		BeginAfterScreen,
		BeginAfterSecret,
		Step,
		End,
		EndPostSecretScreen,
		Zero,
		afterBegin
	};

	Name name;
	State state;

	bool operator==(const typeStateMenu &other) const {
		return name == other.name && state == other.state;
	}
};

enum class Thread1_State {
	BEGIN,
	CONTINUE,
	BEGIN_ANIM,
	RUNNING,
	DONE
};

enum class Thread2_State {
	BEGIN,
	SUB_MAIN_END_FUNCTION,
	SUB_MAIN_BEFORE_LOOP,
	SHOW_WELCOME_SCREEN_LOOP,
	PLAY_INFO_FLV_LOOP,
	MAIN_MENU_LOOP,
	MAP_MENU_LOOP,
	MAP_MENU_BEGIN,
	IN_GAME_LOOP,
	FADEIN_FADEOUT_LOOP,
	LANGUAGE_SETTING_CLICKED,
	MAIN_MENU_BEGIN,
	INTRO_BEGIN,
	IN_GAME_BEGIN,
	IN_GAME_END,
	LOAD_SCREEN,
	KEY_SETTINGS_LOOP,
	RUNNING
};


extern int NewGameDialog_endAction;
extern typeStateMenu2 actState;
//extern int globalAnimIndex;
extern bool PlayInfoFmv_break;
extern bool setLoadScreen;
extern Type_SecretMapScreenPortals_E2970 *secretsModPortals;
extern unsigned __int16 tempActLevel;

extern std::mutex main_mutex;
extern std::condition_variable main_cv;
extern bool thread1_turn;

extern Thread1_State thread1_state;
extern Thread2_State thread2_state;

extern bool thread1_waiting;
extern bool thread2_waiting;
extern uint8_t MyUiBackGroundColorIdx;

extern float MB_Palette_gain[3];
extern float MB_Paletteoffset[3];
extern float MB_Palettesat_multiplier;

const int GRAPHICS_ENHANCE_ON = 1;
const int GRAPHICS_ENHANCE_OFF = 0;
extern int graphics_enhance;
extern int numberOfIntroVideos;

extern int game_paused;
extern bool oneFrameRun;

extern int specialAction;

void InitLanguage_76A40_mod_only_language();

void sub_main_mod_end();

int sub_main_mod(int argc, char **argv, char *real_cdPathch, char *real_gamePath);

void thread2_wait_for_continue(Thread2_State sendstate);
void thread1_wait_for_continue(Thread1_State sendstate);
void thread1_continue(Thread2_State sendstate);
void thread2_continue(Thread1_State sendstate);
void MBChangePalette(int type,int shadow_level=0);
