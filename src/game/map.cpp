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
		snprintf(file, sizeof(file), "f%d", f);

		// 清除 require 缓存，确保热加载生效
		lua_getglobal(L, "package");
		lua_getfield(L, -1, "loaded");
		lua_pushstring(L, file);
		lua_pushnil(L);
		lua_settable(L, -3);
		lua_pop(L, 2);

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

void map_reload() {
	g_map_ready = false;
	std::memset(g_map, 0, sizeof(g_map));
	map_init();
	map_init_default();
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
// ============================================================
int map_kill_monster(uint8_t floor, uint8_t x, uint8_t y, Player* player) {
	uint8_t t = map_get(floor, x, y);
	if (t < 101 || t > 150) return 0;

	map_set(floor, x, y, 1);
	int gold = getMonsterType(t)->money;

	if (player) {
		// 幸运金币：击败敌人获得 2 倍金钱
		player->money += player->hasLuckyCoin ? gold * 2 : gold;

		// Lua 事件系统
		if (player->events)
			player->events->checkGuardKill(floor, x, y, *player);
	}

	return gold;
}
