#include "game/map.h"
#include "game/player.h"
#include "game/monster.h"
#include "event/event_manager.h"
#include "script/lua_state.h"
extern "C" {
#include "lua.h"
#include "lauxlib.h"
}
#include <cstring>
#include <cstdio>

static uint8_t g_map[51][13][13];
static uint8_t g_map_default[51][13][13];
static bool    g_map_ready = false;

void map_init() {
	if (g_map_ready) return;
	lua_State* L = script_init();
	if (!L) return;

	for (int f = 0; f <= 50; f++) {
		char file[32];
		snprintf(file, sizeof(file), "floor_%d", f);
		lua_getglobal(L, "require");
		lua_pushstring(L, file);
		if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
			fprintf(stderr, "map load fail floor %d: %s\n", f, lua_tostring(L, -1));
			lua_pop(L, 1);
			continue;
		}

		lua_getfield(L, -1, "map");
		if (!lua_istable(L, -1)) { lua_pop(L, 2); continue; }

		for (int y = 0; y < 13; y++) {
			lua_rawgeti(L, -1, y + 1);
			for (int x = 0; x < 13; x++) {
				lua_rawgeti(L, -1, x + 1);
				g_map[f][y][x] = (uint8_t)lua_tointeger(L, -1);
				lua_pop(L, 1);
			}
			lua_pop(L, 1);
		}
		lua_pop(L, 2);
	}

	g_map_ready = true;
}

void map_init_default() {
	if (!g_map_ready) map_init();
	memcpy(g_map_default, g_map, sizeof(g_map));
}

uint8_t map_get(uint8_t floor, uint8_t x, uint8_t y) {
	if (floor > 50 || x >= 13 || y >= 13) return 0;
	return g_map[floor][y][x];
}

void map_set(uint8_t floor, uint8_t x, uint8_t y, uint8_t value) {
	if (floor > 50 || x >= 13 || y >= 13) return;
	g_map[floor][y][x] = value;
}

uint8_t map_get_default(uint8_t floor, uint8_t x, uint8_t y) {
	if (floor > 50 || x >= 13 || y >= 13) return 0;
	return g_map_default[floor][y][x];
}

// ============================================================
// 杀死怪物并通知事件系统
// 为保证可靠性，Lua 事件和 C++ 内联检查双路径并行
// ============================================================
int map_kill_monster(uint8_t floor, uint8_t x, uint8_t y, Player* player) {
	uint8_t t = map_get(floor, x, y);
	if (t < 101 || t > 150) return 0;

	map_set(floor, x, y, 1);
	int gold = getMonsterType(t)->money;

	if (player) {
		player->money += gold;

		// Lua 事件系统
		if (player->events)
			player->events->checkGuardKill(floor, x, y, *player);

		// C++ 内联守卫检查（双保险）
		lua_State* L = script_init();
		if (L) {
			char file[32];
			snprintf(file, sizeof(file), "floor_%d", floor);
			lua_getglobal(L, "require");
			lua_pushstring(L, file);
			if (lua_pcall(L, 1, 1, 0) == LUA_OK) {
				lua_getfield(L, -1, "events");
				if (lua_istable(L, -1)) {
					int n = (int)lua_objlen(L, -1);
					for (int i = 1; i <= n; i++) {
						lua_rawgeti(L, -1, i);
						lua_getfield(L, -1, "trigger");
						const char* trig = lua_tostring(L, -1);
						lua_pop(L, 1);

						if (!trig || strcmp(trig, "on_guard_kill") != 0) { lua_pop(L, 1); continue; }

						lua_getfield(L, -1, "guards");
						bool is_guard = false;
						int ng = (int)lua_objlen(L, -1);
						for (int g = 1; g <= ng; g++) {
							lua_rawgeti(L, -1, g);
							lua_getfield(L, -1, "x"); int gx = (int)lua_tointeger(L, -1); lua_pop(L, 1);
							lua_getfield(L, -1, "y"); int gy = (int)lua_tointeger(L, -1); lua_pop(L, 1);
							if (gx == (int)x && gy == (int)y) { is_guard = true; lua_pop(L, 1); break; }
							lua_pop(L, 1);
						}
						lua_pop(L, 1);
						if (!is_guard) { lua_pop(L, 1); continue; }

						bool all_cleared = true;
						lua_getfield(L, -1, "guards");
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

						// 检查 condition_flag
						lua_getfield(L, -1, "condition_flag");
						int cf = (int)lua_tointeger(L, -1); lua_pop(L, 1);
						if (cf > 0 && player->events && player->events->hasFlag((uint8_t)cf)) { lua_pop(L, 1); continue; }

						// 执行 actions
						lua_getfield(L, -1, "actions");
						int an = (int)lua_objlen(L, -1);
						for (int ai = 1; ai <= an; ai++) {
							lua_rawgeti(L, -1, ai);
							lua_getfield(L, -1, "type");
							const char* atype = lua_tostring(L, -1);
							lua_pop(L, 1);

							if (strcmp(atype, "replace_all") == 0) {
								lua_getfield(L, -1, "from"); int from = (int)lua_tointeger(L, -1); lua_pop(L, 1);
								lua_getfield(L, -1, "to");   int to   = (int)lua_tointeger(L, -1); lua_pop(L, 1);
								for (int my = 0; my < 13; my++)
									for (int mx = 0; mx < 13; mx++)
										if (map_get(floor, (uint8_t)mx, (uint8_t)my) == (uint8_t)from)
											map_set(floor, (uint8_t)mx, (uint8_t)my, (uint8_t)to);
								if (from == 8 && to == 1)
									term_set_message("守卫门已打开");
							}
							lua_pop(L, 1);
						}
						lua_pop(L, 1); // actions

						// 设置 flag
						lua_getfield(L, -1, "set_flag");
						int sf = (int)lua_tointeger(L, -1); lua_pop(L, 1);
						if (sf > 0 && player->events) player->events->setFlag((uint8_t)sf);

						lua_pop(L, 1); break; // event_i
					}
				}
				lua_pop(L, 2); // events + floor table
			} else {
				lua_pop(L, 1); // error string
			}
		}
	}

	return gold;
}
