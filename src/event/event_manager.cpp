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
	event_count_ = 0;
}

void EventManager::addEvent(const Event& ev) {
	if (event_count_ < MAX_EVENTS)
		events_[event_count_++] = ev;
}

void EventManager::setFlag(uint8_t id) {
	if (id < MAX_FLAGS) flags_[id] = 1;
}

bool EventManager::hasFlag(uint8_t id) const {
	if (id >= MAX_FLAGS) return true;
	return flags_[id] != 0;
}

// ============================================================
// Lua 事件调度：遍历所有楼层的事件表，匹配则执行
// ============================================================
static void run_event_actions(lua_State* L, int ev_idx) {
	lua_getfield(L, ev_idx, "actions");
	int act_count = (int)lua_objlen(L, -1);
	for (int i = 1; i <= act_count; i++) {
		lua_rawgeti(L, -1, i);
		lua_getfield(L, -1, "type");
		const char* type = lua_tostring(L, -1);
		lua_pop(L, 1);

		if (strcmp(type, "say") == 0) {
			lua_getfield(L, -1, "text");
			saySomething((char*)lua_tostring(L, -1));
			lua_pop(L, 1);
		}
		else if (strcmp(type, "replace_all") == 0) {
			lua_getfield(L, -1, "from"); int from = (int)lua_tointeger(L, -1); lua_pop(L, 1);
			lua_getfield(L, -1, "to");   int to   = (int)lua_tointeger(L, -1); lua_pop(L, 1);
			// floor 从事件表获取
			lua_getfield(L, ev_idx, "floor");
			int fl = (lua_isnil(L, -1)) ? 0 : (int)lua_tointeger(L, -1);
			lua_pop(L, 1);
			if (fl >= 0 && fl <= 50) {
				for (uint8_t y = 0; y < 13; y++)
					for (uint8_t x = 0; x < 13; x++)
						if (map_get((uint8_t)fl, x, y) == (uint8_t)from)
							map_set((uint8_t)fl, x, y, (uint8_t)to);
			}
			if (from == 8 && to == 1)
				term_set_message("守卫门已打开");
		}
		lua_pop(L, 1);
	}
	lua_pop(L, 1); // actions table

	// 设置标记
	lua_getfield(L, ev_idx, "set_flag");
	if (lua_isnumber(L, -1)) {
		int flag = (int)lua_tointeger(L, -1);
		// 需要访问 EventManager — 通过 player->events 获取
	}
	lua_pop(L, 1);
}

void EventManager::checkTile(uint8_t floor, uint8_t tile_id, Player& player) {
	lua_State* L = script_init();
	if (!L) return;
	// 确保事件 API 已注册
	if (player.events) lua_register_game_api(L, &player, player.events);

	char file[32];
	snprintf(file, sizeof(file), "floor_%d", floor);
	lua_getglobal(L, "require");
	lua_pushstring(L, file);
	if (lua_pcall(L, 1, 1, 0) != LUA_OK) { lua_pop(L, 1); return; }

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

		// 检查 condition_flag
		lua_getfield(L, -1, "condition_flag");
		int cf = (int)lua_tointeger(L, -1);
		lua_pop(L, 1);
		if (cf > 0 && hasFlag((uint8_t)cf)) { lua_pop(L, 1); continue; }

		run_event_actions(L, lua_gettop(L));
		lua_getfield(L, -1, "set_flag");
		int sf = (int)lua_tointeger(L, -1);
		lua_pop(L, 1);
		if (sf > 0) setFlag((uint8_t)sf);

		lua_pop(L, 1);
		break;
	}
	lua_pop(L, 2); // events + floor table
}

void EventManager::checkClear(uint8_t floor) {
	if (countMonsters(floor) > 0) return;

	lua_State* L = script_init();
	if (!L) return;

	char file[32];
	snprintf(file, sizeof(file), "floor_%d", floor);
	lua_getglobal(L, "require");
	lua_pushstring(L, file);
	if (lua_pcall(L, 1, 1, 0) != LUA_OK) { lua_pop(L, 1); return; }

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
		int cf = (int)lua_tointeger(L, -1);
		lua_pop(L, 1);
		if (cf > 0 && hasFlag((uint8_t)cf)) { lua_pop(L, 1); continue; }

		run_event_actions(L, lua_gettop(L));
		lua_getfield(L, -1, "set_flag");
		int sf = (int)lua_tointeger(L, -1);
		lua_pop(L, 1);
		if (sf > 0) setFlag((uint8_t)sf);

		lua_pop(L, 1);
		break;
	}
	lua_pop(L, 2);
}

void EventManager::checkGuardKill(uint8_t floor, uint8_t killed_x, uint8_t killed_y, Player& player) {
	lua_State* L = script_init();
	if (!L) return;
	if (player.events) lua_register_game_api(L, &player, player.events);

	char file[32];
	snprintf(file, sizeof(file), "floor_%d", floor);
	lua_getglobal(L, "require");
	lua_pushstring(L, file);
	if (lua_pcall(L, 1, 1, 0) != LUA_OK) { lua_pop(L, 1); return; }

	lua_getfield(L, -1, "events");
	if (!lua_istable(L, -1)) { lua_pop(L, 2); return; }

	int n = (int)lua_objlen(L, -1);
	for (int i = 1; i <= n; i++) {
		lua_rawgeti(L, -1, i);
		lua_getfield(L, -1, "trigger");
		const char* trigger = lua_tostring(L, -1);
		lua_pop(L, 1);

		if (!trigger || strcmp(trigger, "on_guard_kill") != 0) { lua_pop(L, 1); continue; }

		// 检查守卫位置
		lua_getfield(L, -1, "guards");
		bool is_guard = false;
		int ng = (int)lua_objlen(L, -1);
		for (int g = 1; g <= ng; g++) {
			lua_rawgeti(L, -1, g);
			lua_getfield(L, -1, "x"); int gx = (int)lua_tointeger(L, -1); lua_pop(L, 1);
			lua_getfield(L, -1, "y"); int gy = (int)lua_tointeger(L, -1); lua_pop(L, 1);
			if (gx == (int)killed_x && gy == (int)killed_y) { is_guard = true; }
			lua_pop(L, 1);
			if (is_guard) break;
		}
		lua_pop(L, 1); // guards table
		if (!is_guard) { lua_pop(L, 1); continue; }

		// 检查所有守卫是否已清除
		lua_getfield(L, -1, "guards");
		bool all_cleared = true;
		ng = (int)lua_objlen(L, -1);
		for (int g = 1; g <= ng; g++) {
			lua_rawgeti(L, -1, g);
			lua_getfield(L, -1, "x"); int gx = (int)lua_tointeger(L, -1); lua_pop(L, 1);
			lua_getfield(L, -1, "y"); int gy = (int)lua_tointeger(L, -1); lua_pop(L, 1);
			if (map_get(floor, (uint8_t)gx, (uint8_t)gy) != 1) { all_cleared = false; }
			lua_pop(L, 1);
			if (!all_cleared) break;
		}
		lua_pop(L, 1);
		if (!all_cleared) { lua_pop(L, 1); continue; }

		lua_getfield(L, -1, "condition_flag");
		int cf = (int)lua_tointeger(L, -1);
		lua_pop(L, 1);
		if (cf > 0 && hasFlag((uint8_t)cf)) { lua_pop(L, 1); continue; }

		run_event_actions(L, lua_gettop(L));
		lua_getfield(L, -1, "set_flag");
		int sf = (int)lua_tointeger(L, -1);
		lua_pop(L, 1);
		if (sf > 0) setFlag((uint8_t)sf);

		lua_pop(L, 1);
		break;
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

// ============================================================
// 祭坛系统保留 C++ 实现
// ============================================================
uint16_t EventManager::getAltarCost() const {
	uint16_t t = altar_times_;
	return 20 + 10 * (t + 1) * t;
}

void EventManager::checkAltar(uint8_t floor, Player& player) {
	uint8_t  ratio = (floor - 1) / 10 + 1;
	uint16_t cost  = getAltarCost();
	uint16_t atk_gain = 2 * ratio;
	uint16_t def_gain = 4 * ratio;
	uint16_t hp_gain  = 100 * (altar_times_ + 1);

	char text_buf[64];
	char choice_atk[32], choice_def[32], choice_hp[32], choice_leave[8];
	snprintf(text_buf, sizeof(text_buf),
		"供奉%d金币，便可以增加你的力量，你想要什么呢……", cost);
	snprintf(choice_hp,  sizeof(choice_hp),  "生命+%d", hp_gain);
	snprintf(choice_atk, sizeof(choice_atk), "攻击+%d", atk_gain);
	snprintf(choice_def, sizeof(choice_def), "防御+%d", def_gain);
	snprintf(choice_leave, sizeof(choice_leave), "离开");

	saySomething(text_buf);

	char* list[4] = { choice_hp, choice_atk, choice_def, choice_leave };
	uint8_t choice = chooseFromSomething(4, list);

	if (choice == 3 || choice == 255) return;
	if (player.money < cost) { saySomething((char*)"你的金币不足，无法供奉！"); return; }

	player.money -= cost;
	switch (choice) {
		case 0: player.health += hp_gain;  break;
		case 1: player.attack += atk_gain; break;
		case 2: player.defence += def_gain; break;
	}
	altar_times_++;
	drainInput();
}
