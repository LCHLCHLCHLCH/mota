#pragma once

#include <cstdint>

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

// 道具系统调度（C++ 侧调用，内部访问 Lua items 表）
bool lua_item_on_pickup(uint8_t tile_id);
bool lua_item_on_acquire(uint8_t item_id);
bool lua_item_on_use(uint8_t item_id);
