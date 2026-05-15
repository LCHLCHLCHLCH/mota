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

static int g_last_choice = -1;

void EventManager::init() {
	std::memset(flags_, 0, sizeof(flags_));
	altar_times_ = 0;
}

void EventManager::setFlag(uint8_t id) { if (id < MAX_FLAGS) flags_[id] = 1; }
bool EventManager::hasFlag(uint8_t id) const { return (id >= MAX_FLAGS) ? true : (flags_[id] != 0); }

static bool check_if_choice(lua_State* L, int idx) {
	lua_getfield(L, idx, "if_choice");
	if (lua_isnil(L, -1)) { lua_pop(L, 1); return true; }
	bool ok;
	if (lua_istable(L, -1)) {
		ok = false;
		int nc = (int)lua_objlen(L, -1);
		for (int i = 1; i <= nc; i++) {
			lua_rawgeti(L, -1, i);
			if ((int)lua_tointeger(L, -1) == g_last_choice) ok = true;
			lua_pop(L, 1);
		}
	} else {
		ok = ((int)lua_tointeger(L, -1) == g_last_choice);
	}
	lua_pop(L, 1);
	return ok;
}

static void run_event_actions(lua_State* L, int ev_idx, uint8_t floor, Player* player) {
	g_last_choice = -1;

	lua_getfield(L, ev_idx, "actions");
	int n = (int)lua_objlen(L, -1);
	for (int i = 1; i <= n; i++) {
		lua_rawgeti(L, -1, i);
		lua_getfield(L, -1, "type");
		const char* type = lua_tostring(L, -1);
		lua_pop(L, 1);

		if (strcmp(type, "say") == 0) {
			if (!check_if_choice(L, -1)) { lua_pop(L, 1); continue; }
			lua_getfield(L, -1, "text");
			saySomething((char*)lua_tostring(L, -1));
			lua_pop(L, 1);
		}
		else if (strcmp(type, "choose") == 0 && player) {
			lua_getfield(L, -1, "choices");
			char* list[8];
			int nc = (int)lua_objlen(L, -1);
			if (nc > 8) nc = 8;
			for (int j = 1; j <= nc; j++) {
				lua_rawgeti(L, -1, j);
				list[j-1] = (char*)lua_tostring(L, -1);
				lua_pop(L, 1);
			}
			g_last_choice = (int)chooseFromSomething((uint8_t)nc, list);
			lua_pop(L, 1);
		}
		else if (strcmp(type, "replace_all") == 0) {
			if (!check_if_choice(L, -1)) { lua_pop(L, 1); continue; }
			lua_getfield(L, -1, "from"); int from = (int)lua_tointeger(L, -1); lua_pop(L, 1);
			lua_getfield(L, -1, "to");   int to   = (int)lua_tointeger(L, -1); lua_pop(L, 1);
			for (uint8_t y = 0; y < 13; y++)
				for (uint8_t x = 0; x < 13; x++)
					if (map_get(floor, x, y) == (uint8_t)from)
						map_set(floor, x, y, (uint8_t)to);
			if (from == 8 && to == 1)
				term_set_message("守卫门已打开");
		}
		else if (strcmp(type, "add_health") == 0 && player) {
			if (!check_if_choice(L, -1)) { lua_pop(L, 1); continue; }
			lua_getfield(L, -1, "value");
			player->health += (uint32_t)lua_tointeger(L, -1);
			lua_pop(L, 1);
		}
		else if (strcmp(type, "add_attack") == 0 && player) {
			if (!check_if_choice(L, -1)) { lua_pop(L, 1); continue; }
			lua_getfield(L, -1, "value");
			player->attack += (uint32_t)lua_tointeger(L, -1);
			lua_pop(L, 1);
		}
		else if (strcmp(type, "add_defence") == 0 && player) {
			if (!check_if_choice(L, -1)) { lua_pop(L, 1); continue; }
			lua_getfield(L, -1, "value");
			player->defence += (uint32_t)lua_tointeger(L, -1);
			lua_pop(L, 1);
		}
		else if (strcmp(type, "add_money") == 0 && player) {
			if (!check_if_choice(L, -1)) { lua_pop(L, 1); continue; }
			lua_getfield(L, -1, "value");
			player->money += (uint32_t)lua_tointeger(L, -1);
			lua_pop(L, 1);
		}
		else if (strcmp(type, "take_money") == 0 && player) {
			if (!check_if_choice(L, -1)) { lua_pop(L, 1); continue; }
			lua_getfield(L, -1, "value");
			uint32_t val = (uint32_t)lua_tointeger(L, -1);
			lua_pop(L, 1);
			if (player->money >= val) player->money -= val;
		}
		else if (strcmp(type, "set_tile") == 0) {
			if (!check_if_choice(L, -1)) { lua_pop(L, 1); continue; }
			lua_getfield(L, -1, "x"); int tx = (int)lua_tointeger(L, -1); lua_pop(L, 1);
			lua_getfield(L, -1, "y"); int ty = (int)lua_tointeger(L, -1); lua_pop(L, 1);
			lua_getfield(L, -1, "value"); int tv = (int)lua_tointeger(L, -1); lua_pop(L, 1);
			map_set(floor, (uint8_t)tx, (uint8_t)ty, (uint8_t)tv);
		}
		else if (strcmp(type, "msg") == 0) {
			if (!check_if_choice(L, -1)) { lua_pop(L, 1); continue; }
			lua_getfield(L, -1, "text");
			term_set_message((char*)lua_tostring(L, -1));
			lua_pop(L, 1);
		}
		else if (strcmp(type, "call") == 0) {
			lua_getfield(L, -1, "func");
			if (lua_isfunction(L, -1)) {
				if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
					fprintf(stderr, "event call error: %s\n", lua_tostring(L, -1));
					lua_pop(L, 1);
				}
			} else { lua_pop(L, 1); }
		}

		lua_pop(L, 1);
	}
	lua_pop(L, 1);
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

void EventManager::checkTile(uint8_t floor, uint8_t tile_id, Player& player) {
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
		if (!trigger || strcmp(trigger, "on_tile") != 0) { lua_pop(L, 1); continue; }

		lua_getfield(L, -1, "tile");
		if ((int)lua_tointeger(L, -1) != (int)tile_id) { lua_pop(L, 2); continue; }
		lua_pop(L, 1);

		lua_getfield(L, -1, "condition_flag");
		int cf = (int)lua_tointeger(L, -1); lua_pop(L, 1);
		if (cf > 0 && hasFlag((uint8_t)cf)) { lua_pop(L, 1); continue; }

		run_event_actions(L, lua_gettop(L), floor, &player);

		lua_getfield(L, -1, "set_flag");
		int sf = (int)lua_tointeger(L, -1); lua_pop(L, 1);
		if (sf > 0) setFlag((uint8_t)sf);

		lua_pop(L, 1); break;
	}
	lua_pop(L, 2);
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

		lua_getfield(L, -1, "condition_flag");
		int cf = (int)lua_tointeger(L, -1); lua_pop(L, 1);
		if (cf > 0 && hasFlag((uint8_t)cf)) { lua_pop(L, 1); continue; }

		run_event_actions(L, lua_gettop(L), floor, NULL);

		lua_getfield(L, -1, "set_flag");
		int sf = (int)lua_tointeger(L, -1); lua_pop(L, 1);
		if (sf > 0) setFlag((uint8_t)sf);

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

		lua_getfield(L, -1, "condition_flag");
		int cf = (int)lua_tointeger(L, -1); lua_pop(L, 1);
		if (cf > 0 && hasFlag((uint8_t)cf)) { lua_pop(L, 1); continue; }

		run_event_actions(L, lua_gettop(L), floor, &player);

		lua_getfield(L, -1, "set_flag");
		int sf = (int)lua_tointeger(L, -1); lua_pop(L, 1);
		if (sf > 0) setFlag((uint8_t)sf);

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

uint16_t EventManager::getAltarCost() const {
	uint16_t t = altar_times_;
	return 20 + 10 * (t + 1) * t;
}


