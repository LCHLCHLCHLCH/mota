#pragma once

class Player;
class EventManager;

extern "C" {
#include "lua.h"
}

void lua_register_game_api(lua_State* L, Player* player, EventManager* events);
