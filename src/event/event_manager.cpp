#include "event/event_manager.h"
#include "game/player.h"
#include "game/map.h"
#include "script/lua_state.h"
#include "script/lua_bridge.h"
extern "C" {
#include "lua.h"
#include "lauxlib.h"
}
#include <cstring>
#include <cstdio>

void EventManager::init() {
	std::memset(flags_, 0, sizeof(flags_));
	altar_times_ = 0;
}

void EventManager::setFlag(uint8_t floor, uint8_t id) {
	if (floor < MAX_FLOORS && id < MAX_FLAGS) flags_[floor][id] = 1;
}
bool EventManager::hasFlag(uint8_t floor, uint8_t id) const {
	if (floor >= MAX_FLOORS || id >= MAX_FLAGS) return true;
	return flags_[floor][id] != 0;
}

// 执行事件的 run 函数（传入触发坐标，供 Lua 事件使用）
static bool run_event_run(lua_State* L, int ev_idx, int px, int py) {
	lua_getfield(L, ev_idx, "run");
	if (!lua_isfunction(L, -1)) { lua_pop(L, 1); return false; }
	lua_pushinteger(L, px);
	lua_pushinteger(L, py);
	if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
		fprintf(stderr, "event run error: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return false;
	}
	return true;
}

// 每次移动后调用楼层定义的 on_step 函数（如有）
static bool load_floor_events(lua_State* L, uint8_t floor);
void EventManager::callOnStep(uint8_t floor, Player& player)
{
	lua_State* L = script_init();
	if (!L) return;
	lua_register_game_api(L, &player, this);
	if (!load_floor_events(L, floor)) return;

	lua_getfield(L, -1, "on_step");
	if (lua_isfunction(L, -1)) {
		if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
			fprintf(stderr, "on_step error: %s\n", lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	} else {
		lua_pop(L, 1);
	}
	lua_pop(L, 1); // floor module
}

// 读取事件的 once/condition_flag，返回是否应该跳过（已触发过）
// 若不需要跳过且 once 为 true，自动分配内部 flag ID = event_index
static bool check_event_skip(lua_State* L, int ev_idx, uint8_t floor, int event_index,
                             EventManager* em, bool* out_once, int* out_auto_flag) {
	*out_once = false;
	*out_auto_flag = -1;

	// once 字段优先
	lua_getfield(L, ev_idx, "once");
	bool has_once = lua_toboolean(L, -1);
	lua_pop(L, 1);

	if (has_once) {
		*out_once = true;
		int auto_id = event_index;
		if (auto_id >= MAX_FLAGS) auto_id = MAX_FLAGS - 1;
		*out_auto_flag = auto_id;
		return em->hasFlag(floor, (uint8_t)auto_id);
	}

	// 回退到 condition_flag
	lua_getfield(L, ev_idx, "condition_flag");
	int cf = (int)lua_tointeger(L, -1);
	lua_pop(L, 1);
	if (cf > 0 && em->hasFlag(floor, (uint8_t)cf)) return true;
	return false;
}

// 根据 once 或 set_flag 设置 flag
static void apply_event_flag(lua_State* L, int ev_idx, uint8_t floor, EventManager* em,
                             bool is_once, int auto_flag) {
	if (is_once) {
		if (auto_flag >= 0 && auto_flag < MAX_FLAGS)
			em->setFlag(floor, (uint8_t)auto_flag);
	} else {
		lua_getfield(L, ev_idx, "set_flag");
		int sf = (int)lua_tointeger(L, -1);
		lua_pop(L, 1);
		if (sf > 0) em->setFlag(floor, (uint8_t)sf);
	}
}

static bool load_floor_events(lua_State* L, uint8_t floor) {
	char file[32];
	snprintf(file, sizeof(file), "floor_%d", floor);
	lua_getglobal(L, "require");
	lua_pushstring(L, file);
	if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
		fprintf(stderr, "Lua require '%s' failed: %s\n", file, lua_tostring(L, -1));
		lua_pop(L, 1);
		return false;
	}
	return true;
}

// ============================================================
// 事件分发
// ============================================================

bool EventManager::tryHandleTile(uint8_t floor, uint8_t px, uint8_t py, uint8_t tile_id, Player& player) {
	lua_State* L = script_init();
	if (!L) return false;
	if (player.events) lua_register_game_api(L, &player, player.events);

	if (!load_floor_events(L, floor)) return false;

	lua_getfield(L, -1, "events");
	if (!lua_istable(L, -1)) { lua_pop(L, 2); return false; }

	int n = (int)lua_objlen(L, -1);
	for (int i = 1; i <= n; i++) {
		lua_rawgeti(L, -1, i);
		lua_getfield(L, -1, "trigger");
		const char* trigger = lua_tostring(L, -1);
		lua_pop(L, 1);
		if (!trigger || strcmp(trigger, "on_tile") != 0) { lua_pop(L, 1); continue; }

		lua_getfield(L, -1, "x");
		int ev_x = lua_isnil(L, -1) ? -1 : (int)lua_tointeger(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "y");
		int ev_y = lua_isnil(L, -1) ? -1 : (int)lua_tointeger(L, -1);
		lua_pop(L, 1);

		bool match;
		if (ev_x >= 0 && ev_y >= 0) {
			match = (ev_x == (int)px && ev_y == (int)py);
		} else {
			lua_getfield(L, -1, "tile");
			match = ((int)lua_tointeger(L, -1) == (int)tile_id);
			lua_pop(L, 1);
		}

		if (!match) { lua_pop(L, 1); continue; }

		bool is_once; int auto_flag;
		if (check_event_skip(L, lua_gettop(L), floor, i - 1, this, &is_once, &auto_flag))
			{ lua_pop(L, 1); continue; }

		run_event_run(L, lua_gettop(L), px, py);
		apply_event_flag(L, lua_gettop(L), floor, this, is_once, auto_flag);

		lua_pop(L, 1); // event
		lua_pop(L, 2); // events + floor table
		return true;
	}
	lua_pop(L, 2);
	return false;
}

void EventManager::checkClear(uint8_t floor) {
	if (countMonsters(floor) > 0) return;
	lua_State* L = script_init();
	if (!L) return;
	if (!load_floor_events(L, floor)) return;

	lua_getfield(L, -1, "events");
	if (!lua_istable(L, -1)) { lua_pop(L, 2); return; }

	int n = (int)lua_objlen(L, -1);
	for (int i = 1; i <= n; i++) {
		lua_rawgeti(L, -1, i);
		lua_getfield(L, -1, "trigger");
		const char* trigger = lua_tostring(L, -1);
		lua_pop(L, 1);
		if (!trigger || strcmp(trigger, "on_clear") != 0) { lua_pop(L, 1); continue; }

		bool is_once; int auto_flag;
		if (check_event_skip(L, lua_gettop(L), floor, i - 1, this, &is_once, &auto_flag))
			{ lua_pop(L, 1); continue; }

		run_event_run(L, lua_gettop(L), 0, 0);
		apply_event_flag(L, lua_gettop(L), floor, this, is_once, auto_flag);

		lua_pop(L, 1); break;
	}
	lua_pop(L, 2);
}

void EventManager::checkGuardKill(uint8_t floor, uint8_t killed_x, uint8_t killed_y, Player& player) {
	lua_State* L = script_init();
	if (!L) return;
	if (player.events) lua_register_game_api(L, &player, player.events);

	if (!load_floor_events(L, floor)) return;

	lua_getfield(L, -1, "events");
	if (!lua_istable(L, -1)) { lua_pop(L, 2); return; }

	int n = (int)lua_objlen(L, -1);
	for (int i = 1; i <= n; i++) {
		lua_rawgeti(L, -1, i);
		lua_getfield(L, -1, "trigger");
		const char* trigger = lua_tostring(L, -1);
		lua_pop(L, 1);
		if (!trigger || strcmp(trigger, "on_guard_kill") != 0) { lua_pop(L, 1); continue; }

		lua_getfield(L, -1, "guards");
		bool is_guard = false;
		int ng = (int)lua_objlen(L, -1);
		for (int g = 1; g <= ng; g++) {
			lua_rawgeti(L, -1, g);
			lua_getfield(L, -1, "x"); int gx = (int)lua_tointeger(L, -1); lua_pop(L, 1);
			lua_getfield(L, -1, "y"); int gy = (int)lua_tointeger(L, -1); lua_pop(L, 1);
			if (gx == (int)killed_x && gy == (int)killed_y) { is_guard = true; lua_pop(L, 1); break; }
			lua_pop(L, 1);
		}
		lua_pop(L, 1);
		if (!is_guard) { lua_pop(L, 1); continue; }

		lua_getfield(L, -1, "guards");
		bool all_cleared = true;
		ng = (int)lua_objlen(L, -1);
		for (int g = 1; g <= ng; g++) {
			lua_rawgeti(L, -1, g);
			lua_getfield(L, -1, "x"); int gx = (int)lua_tointeger(L, -1); lua_pop(L, 1);
			lua_getfield(L, -1, "y"); int gy = (int)lua_tointeger(L, -1); lua_pop(L, 1);
			if (map_get(floor, (uint8_t)gx, (uint8_t)gy) != 1) { all_cleared = false; lua_pop(L, 1); break; }
			lua_pop(L, 1);
		}
		lua_pop(L, 1);
		if (!all_cleared) { lua_pop(L, 1); continue; }

		bool is_once; int auto_flag;
		if (check_event_skip(L, lua_gettop(L), floor, i - 1, this, &is_once, &auto_flag))
			{ lua_pop(L, 1); continue; }

		run_event_run(L, lua_gettop(L), killed_x, killed_y);
		apply_event_flag(L, lua_gettop(L), floor, this, is_once, auto_flag);

		lua_pop(L, 1); break;
	}
	lua_pop(L, 2);
}

uint8_t EventManager::countMonsters(uint8_t floor) {
	uint8_t count = 0;
	for (uint8_t y = 0; y < 13; y++)
		for (uint8_t x = 0; x < 13; x++) {
			uint8_t t = map_get(floor, x, y);
			if (t >= 101 && t <= 150) count++;
		}
	return count;
}
