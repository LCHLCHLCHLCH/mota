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

// 执行可选启动脚本（scripts/startup.lua），在开局前运行脚本内命令；无则保持默认
void lua_apply_startup(Player& player);

// 道具系统调度（C++ 侧调用，内部访问 Lua items 表）
bool lua_item_on_pickup(uint8_t tile_id);
bool lua_item_on_acquire(uint8_t item_id);
bool lua_item_on_use(uint8_t item_id);

// 读取道具描述文本（items[id].desc），成功写入 out 并返回 true
bool lua_item_desc(uint8_t item_id, char* out, size_t out_size);
