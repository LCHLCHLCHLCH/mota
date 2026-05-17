#pragma once

class Player;
class EventManager;
class Display;
class StatusBar;

extern "C" {
#include "lua.h"
}

void lua_register_game_api(lua_State* L, Player* player, EventManager* events);
void lua_set_frame_context(Display* display, StatusBar* statusBar);
bool is_debug();
