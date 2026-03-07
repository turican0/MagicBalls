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


extern int NewGameDialog_endAction;
extern typeStateMenu2 actState;
extern int globalAnimIndex;
extern bool PlayInfoFmv_break;
extern bool setLoadScreen;
extern Type_SecretMapScreenPortals_E2970 *secretsModPortals;

void InitLanguage_76A40_mod_only_language();

void sub_main_mod_begin(int argc, char **argv, char *real_cdPathch);
void sub_main_mod_end();

void sub_46830_main_loop_mod(unsigned __int16 actLevel, typeStateMenu newState);
