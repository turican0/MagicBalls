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
	AfterMap,
	AfterMainMenu,
	AfterGame,
	AfterAnim
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
		Step,
		End,
		Zero,
		afterBegin
	};

	Name name;
	State state;

	// ručně operator==
	bool operator==(const typeStateMenu &other) const {
		return name == other.name && state == other.state;
	}
};


extern int NewGameDialog_endAction;
extern typeStateMenu2 actState;
extern int globalAnimIndex;

//bool NewGameDialog_77350_mod_Begin();
//void NewGameDialog_77350_mod_Step();
//bool NewGameDialog_77350_mod_End();
void graphics_queue_add_action(const std::string &action, int x, int y, int index, int type);
std::vector<GraphicsAction> graphics_queue_get_pending_actions();
void graphics_queue_clear();
void InitLanguage_76A40_mod_only_language();

void sub_main_mod_begin(int argc, char **argv, char *real_cdPathch);
void sub_main_mod_end();
void sub_46830_main_loop_mod_begin_cycle();
//void sub_46830_main_loop_mod_end_cycle();
bool sub_46830_main_loop_mod_end_cycle_part1();
bool sub_46830_main_loop_mod_end_cycle_part2();
bool sub_46830_main_loop_mod_end_cycle_part3();
void sub_46830_main_loop_mod_end_cycle_part4();
void Intro_begin(int introType);
void Intros_76D10_mod_begin(char introType);
void PlayInfoFmv_mod_begin(__int16 a1, __int16 a2, Type_SoundEvent_E17CC *pSoundEvent, char *path);
void PlayInfoFmv_mod_end();
void Intros_76D10_mod_end(char introType);
bool PlayInfoFmv_mod_step();

void MenusAndIntros_76930_mod_end();

void MainMenu_76FA0_mod_begin();
void MainMenu_76FA0_mod_end();
void InGameLoop_47320_mod_begin();
void InGameLoop_47320_mod_end();

void sub_46830_main_loop_mod(unsigned __int16 actLevel, typeStateMenu newState);
