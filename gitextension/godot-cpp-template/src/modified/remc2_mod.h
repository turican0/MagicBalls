#pragma once
#include <algorithm>
#include "../remc2/engine/EventsFunctions.h"
#include "../remc2/engine/MenusAndIntros.h"
#include "../remc2/engine/PlayerInput.h"
#include "../remc2/engine/DatTabIndexes.h"

struct GraphicsAction {
	std::string action;
	int x = 0;
	int y = 0;
	int index = 0;
	int type = 0;
};

bool NewGameDialog_77350_mod_Begin();
void NewGameDialog_77350_mod_Step();
bool NewGameDialog_77350_mod_End();
void graphics_queue_add_action(const std::string &action, int x, int y, int index, int type);
std::vector<GraphicsAction> graphics_queue_get_pending_actions();
void graphics_queue_clear();
void InitLanguage_76A40_mod_only_language();
