#include "script/lua_bridge.h"
#include "script/lua_state.h"
#include "game/player.h"
#include "game/map.h"
#include "game/monster.h"
#include "game/tile_data.h"
#include "event/event_manager.h"
#include "event/dialog.h"
#include "sdl_terminal.h"
#include "save_system.h"
#include "ui/backpack.h"
#include "ui/monster_book.h"
#include "sdl_3dwindow.h"
#include "render/display.h"
#include "render/status_bar.h"
extern "C" {
#include "lua.h"
#include "lauxlib.h"
}
#include <cstdio>
#include <cstring>

// ============================================================
// Lua 可调用的标记读写
// ============================================================
static EventManager* g_ev = NULL;
static Player*     g_ply = NULL;

// 前向声明（定义在 g_display/g_statusBar 之后）
static void refresh_and_present();

static int l_has_flag(lua_State* L) {
	int id = (int)luaL_checkinteger(L, 1);
	bool result = false;
	if (g_ev && g_ply) result = g_ev->hasFlag(g_ply->floor, (uint8_t)id);
	lua_pushboolean(L, result);
	return 1;
}

static int l_set_flag(lua_State* L) {
	int id = (int)luaL_checkinteger(L, 1);
	if (g_ev && g_ply) g_ev->setFlag(g_ply->floor, (uint8_t)id);
	return 0;
}

// ============================================================
// 动作：对话、消息、地图替换
// ============================================================
static int l_say(lua_State* L) {
	const char* text = luaL_checkstring(L, 1);
	refresh_and_present();
	saySomething((char*)text);
	return 0;
}

static int l_msg(lua_State* L) {
	const char* text = luaL_checkstring(L, 1);
	term_set_message(text);
	return 0;
}

static int l_replace_all(lua_State* L) {
	int floor = (int)luaL_checkinteger(L, 1);
	int from  = (int)luaL_checkinteger(L, 2);
	int to    = (int)luaL_checkinteger(L, 3);
	for (uint8_t y = 0; y < 13; y++)
		for (uint8_t x = 0; x < 13; x++)
			if (map_get((uint8_t)floor, x, y) == (uint8_t)from)
				map_set((uint8_t)floor, x, y, (uint8_t)to);
	if (from == 8 && to == 1)
		term_set_message("守卫门已打开");
	return 0;
}

static int l_count_monsters(lua_State* L) {
	int floor = (int)luaL_checkinteger(L, 1);
	int count = 0;
	for (uint8_t y = 0; y < 13; y++)
		for (uint8_t x = 0; x < 13; x++) {
			uint8_t t = map_get((uint8_t)floor, x, y);
			if (t >= 101 && t <= 150) count++;
		}
	lua_pushinteger(L, count);
	return 1;
}

// ============================================================
// 控制台用 API
// ============================================================
static Display*    g_display = NULL;
static StatusBar*  g_statusBar = NULL;

static bool g_debug = false;

// 重新生成并呈现一帧（sleep / say 等阻塞操作前调用）
static void refresh_and_present() {
	if (!g_display || !g_statusBar || !g_ply) return;
	touchwin_term();
	g_display->generateFrame(*g_ply);
	g_statusBar->draw(*g_ply);
	term_present();
}

static int l_set(lua_State* L) {
	const char* attr = luaL_checkstring(L, 1);
	int val = (int)luaL_checkinteger(L, 2);
	Player& p = *g_ply;
	if      (strcmp(attr, "health") == 0)  p.health = val;
	else if (strcmp(attr, "attack") == 0)  p.attack = val;
	else if (strcmp(attr, "defence") == 0) p.defence = val;
	else if (strcmp(attr, "money") == 0)   p.money = val;
	else if (strcmp(attr, "yellow") == 0)  p.yellowKey = (uint8_t)val;
	else if (strcmp(attr, "blue") == 0)    p.blueKey   = (uint8_t)val;
	else if (strcmp(attr, "red") == 0)     p.redKey    = (uint8_t)val;
	else if (strcmp(attr, "floor") == 0)   { p.floor = (uint8_t)val; if (p.floor > p.maxFloorVisited) p.maxFloorVisited = p.floor; }
	else if (strcmp(attr, "x") == 0)       p.x = (uint8_t)val;
	else if (strcmp(attr, "y") == 0)       p.y = (uint8_t)val;
	return 0;
}

static int l_info(lua_State* L) {
	(void)L;
	Player& p = *g_ply;
	printf("\n==== 玩家状态 ====\n");
	printf("楼层: %d   坐标: (%d, %d)\n", p.floor, p.x, p.y);
	printf("生命: %d\n", p.health);
	printf("攻击: %d    防御: %d\n", p.attack, p.defence);
	printf("金币: %d\n", p.money);
	printf("黄钥匙: %d  蓝钥匙: %d  红钥匙: %d\n", p.yellowKey, p.blueKey, p.redKey);
	printf("传送器: %s\n", p.hasTeleporter ? "有" : "无");
	printf("\n");
	return 0;
}

static int l_tp(lua_State* L) {
	int f = (int)luaL_checkinteger(L, 1);
	if (f >= 0 && f <= 50) {
		g_ply->floor = (uint8_t)f;
		if (g_ply->floor > g_ply->maxFloorVisited) g_ply->maxFloorVisited = g_ply->floor;
		printf("已传送到楼层 %d\n", f);
	} else printf("楼层范围: 0-50\n");
	return 0;
}

static int l_killall(lua_State* L) {
	(void)L;
	int count = 0;
	for (uint8_t y = 0; y < 13; y++)
		for (uint8_t x = 0; x < 13; x++) {
			uint8_t t = map_get(g_ply->floor, x, y);
			if (t >= 101 && t <= 150) { map_set(g_ply->floor, x, y, 1); count++; }
		}
	printf("已清除 %d 个怪物\n", count);
	return 0;
}

static int l_give(lua_State* L) {
	int id = (int)luaL_checkinteger(L, 1);
	if (lua_item_on_acquire((uint8_t)id)) {
		printf("给予道具 %d\n", id);
	} else {
		printf("未知道具 ID: %d\n", id);
	}
	return 0;
}

static int l_light(lua_State* L) {
	const char* arg = luaL_optstring(L, 1, "");
	if (strcmp(arg, "on") == 0 || strcmp(arg, "1") == 0)
		{ term_set_light_mode(true); printf("light mode on\n"); }
	else if (strcmp(arg, "off") == 0 || strcmp(arg, "0") == 0)
		{ term_set_light_mode(false); printf("light mode off\n"); }
	else printf("usage: light(\"on\") or light(\"off\")\n");
	return 0;
}

static int l_restart(lua_State* L) {
	(void)L;
	map_reload();
	g_ply->init();
	if (g_ev) g_ev->init();
	printf("游戏已重启\n");
	return 0;
}

// ============================================================
// 道具系统 Lua API（供 items.lua 中的 on_acquire / on_use 调用）
// ============================================================
static int l_add_yellow_key(lua_State* L) {
	int n = (int)luaL_optinteger(L, 1, 1);
	if (g_ply) g_ply->yellowKey = (uint8_t)(g_ply->yellowKey + n);
	return 0;
}
static int l_take_yellow_key(lua_State* L) {
	int n = (int)luaL_optinteger(L, 1, 1);
	if (!g_ply) { lua_pushboolean(L, 0); return 1; }
	if (g_ply->yellowKey >= (uint8_t)n) {
		g_ply->yellowKey -= (uint8_t)n;
		lua_pushboolean(L, 1);
	} else {
		lua_pushboolean(L, 0);
	}
	return 1;
}
static int l_add_blue_key(lua_State* L) {
	int n = (int)luaL_optinteger(L, 1, 1);
	if (g_ply) g_ply->blueKey = (uint8_t)(g_ply->blueKey + n);
	return 0;
}
static int l_add_red_key(lua_State* L) {
	int n = (int)luaL_optinteger(L, 1, 1);
	if (g_ply) g_ply->redKey = (uint8_t)(g_ply->redKey + n);
	return 0;
}
static int l_set_teleporter(lua_State* L) {
	bool v = lua_toboolean(L, 1);
	if (g_ply) g_ply->hasTeleporter = v;
	return 0;
}
static int l_set_monster_book(lua_State* L) {
	bool v = lua_toboolean(L, 1);
	if (g_ply) g_ply->hasMonsterBook = v;
	return 0;
}
static int l_set_cross(lua_State* L) {
	bool v = lua_toboolean(L, 1);
	if (g_ply) g_ply->hasCross = v;
	return 0;
}
static int l_show_monster_book(lua_State* L) {
	(void)L;
	if (g_ply) showMonsterBook(*g_ply);
	return 0;
}
static int l_battle_monster(lua_State* L) {
	int id = (int)luaL_checkinteger(L, 1);
	if (!g_ply) { lua_pushboolean(L, 0); return 1; }
	PREDICTION prd = g_ply->PredictAttack((uint8_t)id);
	if (prd == LIVE) {
		g_ply->health -= g_ply->hurt;
		lua_pushboolean(L, 1);
	} else {
		lua_pushboolean(L, 0);
	}
	return 1;
}
static int l_backpack_add(lua_State* L) {
	int id = (int)luaL_checkinteger(L, 1);
	if (!g_ply || !g_ply->backpack) return 0;
	g_ply->backpack->addItem((uint8_t)id, getItemName((uint8_t)id));
	return 0;
}
static int l_backpack_has(lua_State* L) {
	int id = (int)luaL_checkinteger(L, 1);
	if (!g_ply || !g_ply->backpack || g_ply->backpack->items.empty()) {
		lua_pushboolean(L, 0); return 1;
	}
	for (auto& it : g_ply->backpack->items)
		if (it.id == (uint8_t)id) { lua_pushboolean(L, 1); return 1; }
	lua_pushboolean(L, 0);
	return 1;
}
static int l_freeze_lava(lua_State* L) {
	(void)L;
	if (g_ply) g_ply->freezeLava();
	return 0;
}
static int l_detonate(lua_State* L) {
	(void)L;
	if (!g_ply) { lua_pushinteger(L, 0); return 1; }
	int killed = 0;
	uint8_t px = g_ply->x, py = g_ply->y;
	uint8_t dirs[4][2] = {{px,(uint8_t)(py-1)},{px,(uint8_t)(py+1)},{(uint8_t)(px-1),py},{(uint8_t)(px+1),py}};
	for (int i = 0; i < 4; i++) {
		uint8_t tx = dirs[i][0], ty = dirs[i][1];
		uint8_t t = map_get(g_ply->floor, tx, ty);
		if (t >= 101 && t <= 150 && !isBossMonster(t)) {
			map_kill_monster(g_ply->floor, tx, ty, g_ply);
			killed++;
		}
	}
	lua_pushinteger(L, killed);
	return 1;
}

// ============================================================
// 注册所有 API
// ============================================================
static int l_save_cmd(lua_State* L) {
	const char* name = luaL_checkstring(L, 1);
	save_game(name, *g_ply, *g_ev);
	return 0;
}

static int l_load_cmd(lua_State* L) {
	const char* name = luaL_checkstring(L, 1);
	load_game(name, *g_ply, *g_ev);
	return 0;
}

static int l_open3d(lua_State* L) {
	(void)L;
	run_3d_window(term_get_window(), *g_ply);
	return 0;
}

static int l_close3d(lua_State* L) {
	(void)L;
	shutdown_3d_window();
	return 0;
}


// ============================================================
// debug 模式（无视碰撞、事件、怪物）
// ============================================================
bool is_debug() { return g_debug; }

static int l_debug_on(lua_State* L) {
	(void)L; g_debug = true; term_set_message("debug ON"); return 0;
}

static int l_debug_off(lua_State* L) {
	(void)L; g_debug = false; term_set_message("debug OFF"); return 0;
}
void lua_set_frame_context(Display* display, StatusBar* statusBar) {
	g_display   = display;
	g_statusBar = statusBar;
}

void lua_register_game_api(lua_State* L, Player* player, EventManager* events) {
	g_ply = player;
	g_ev  = events;

	lua_register(L, "say",          l_say);
	lua_register(L, "msg",          l_msg);
	lua_register(L, "has_flag",     l_has_flag);
	lua_register(L, "set_flag",     l_set_flag);
	lua_register(L, "replace_all",  l_replace_all);
	lua_register(L, "count_monsters", l_count_monsters);

	lua_register(L, "set",   l_set);
	lua_register(L, "info",  l_info);
	lua_register(L, "tp",    l_tp);
	lua_register(L, "killall", l_killall);
	lua_register(L, "give",  l_give);
	lua_register(L, "light", l_light);
	lua_register(L, "save",  l_save_cmd);
	lua_register(L, "load",  l_load_cmd);
	lua_register(L, "restart", l_restart);
	lua_register(L, "open3d",  l_open3d);
	lua_register(L, "close3d", l_close3d);

	// 数值操作
	lua_register(L, "add_health",  [](lua_State* L)->int { g_ply->health += (uint32_t)luaL_checkinteger(L,1); return 0; });
	lua_register(L, "add_attack",  [](lua_State* L)->int { g_ply->attack += (uint32_t)luaL_checkinteger(L,1); return 0; });
	lua_register(L, "add_defence", [](lua_State* L)->int { g_ply->defence += (uint32_t)luaL_checkinteger(L,1); return 0; });
	lua_register(L, "add_money",   [](lua_State* L)->int { g_ply->money  += (uint32_t)luaL_checkinteger(L,1); return 0; });
	lua_register(L, "take_money",  [](lua_State* L)->int {
		uint32_t v = (uint32_t)luaL_checkinteger(L,1);
		if (g_ply->money >= v) { g_ply->money -= v; lua_pushboolean(L, 1); }
		else lua_pushboolean(L, 0);
		return 1;
	});
	lua_register(L, "altar_times",  [](lua_State* L)->int { lua_pushinteger(L, g_ev->getAltarTimes()); return 1; });
	lua_register(L, "player_floor", [](lua_State* L)->int { lua_pushinteger(L, g_ply->floor); return 1; });
	lua_register(L, "choose_menu",  [](lua_State* L)->int {
		int n = lua_gettop(L);
		if (n > 8) n = 8;
		char* list[8];
		for (int i = 0; i < n; i++) list[i] = (char*)lua_tostring(L, i+1);
		lua_pushinteger(L, chooseFromSomething(n, list));
		return 1;
	});
	lua_register(L, "drain", [](lua_State* L)->int { drainInput(); return 0; });
	lua_register(L, "altar_tick", [](lua_State* L)->int {
		(void)L; g_ev->setAltarTimes(g_ev->getAltarTimes() + 1); return 0; });
	lua_register(L, "get_tile", [](lua_State* L)->int {
		int x = (int)luaL_checkinteger(L, 1);
		int y = (int)luaL_checkinteger(L, 2);
		lua_pushinteger(L, map_get(g_ply->floor, (uint8_t)x, (uint8_t)y));
		return 1;
	});
	lua_register(L, "set_tile", [](lua_State* L)->int {
		int x = (int)luaL_checkinteger(L, 1);
		int y = (int)luaL_checkinteger(L, 2);
		int v = (int)luaL_checkinteger(L, 3);
		map_set(g_ply->floor, (uint8_t)x, (uint8_t)y, (uint8_t)v);
		return 0;
	});
	lua_register(L, "set_tile_floor", [](lua_State* L)->int {
		int f = (int)luaL_checkinteger(L, 1);
		int x = (int)luaL_checkinteger(L, 2);
		int y = (int)luaL_checkinteger(L, 3);
		int v = (int)luaL_checkinteger(L, 4);
		map_set((uint8_t)f, (uint8_t)x, (uint8_t)y, (uint8_t)v);
		return 0;
	});
	lua_register(L, "get_tile_floor", [](lua_State* L)->int {
		int f = (int)luaL_checkinteger(L, 1);
		int x = (int)luaL_checkinteger(L, 2);
		int y = (int)luaL_checkinteger(L, 3);
		lua_pushinteger(L, map_get((uint8_t)f, (uint8_t)x, (uint8_t)y));
		return 1;
	});
	lua_register(L, "sleep_ms", [](lua_State* L)->int {
		int ms = (int)luaL_checkinteger(L, 1);
		if (ms <= 0) return 0;
		refresh_and_present();
		uint32_t end = SDL_GetTicks() + (uint32_t)ms;
		while (SDL_GetTicks() < end) {
			SDL_Event ev;
			while (SDL_PollEvent(&ev)) {
				if (ev.type == SDL_EVENT_QUIT) return 0;
			}
			SDL_Delay(10);
		}
		return 0;
	});
	lua_register(L, "darken_map", [](lua_State* L)->int {
		(void)L; term_set_darkened(true); return 0;
	});
	lua_register(L, "lighten_map", [](lua_State* L)->int {
		(void)L; term_set_darkened(false); return 0;
	});
	lua_register(L, "debug_on",  l_debug_on);
	lua_register(L, "debug_off", l_debug_off);

	// 道具系统 API
	lua_register(L, "add_yellow_key", l_add_yellow_key);
	lua_register(L, "take_yellow_key", l_take_yellow_key);
	lua_register(L, "add_blue_key",   l_add_blue_key);
	lua_register(L, "add_red_key",    l_add_red_key);
	lua_register(L, "set_teleporter", l_set_teleporter);
	lua_register(L, "set_monster_book", l_set_monster_book);
	lua_register(L, "set_cross", l_set_cross);
	lua_register(L, "show_monster_book", l_show_monster_book);
	lua_register(L, "battle_monster", l_battle_monster);
	lua_register(L, "backpack_add",   l_backpack_add);
	lua_register(L, "backpack_has",   l_backpack_has);
	lua_register(L, "freeze_lava",    l_freeze_lava);
	lua_register(L, "detonate",       l_detonate);
	lua_register(L, "player_dir",     [](lua_State* L)->int { lua_pushinteger(L, g_ply ? g_ply->direction : 0); return 1; });
	lua_register(L, "player_x",       [](lua_State* L)->int { lua_pushinteger(L, g_ply ? g_ply->x : 0); return 1; });
	lua_register(L, "player_y",       [](lua_State* L)->int { lua_pushinteger(L, g_ply ? g_ply->y : 0); return 1; });
}

// ============================================================
// 道具系统调度函数（C++ 侧调用，桥接到 Lua items 表）
// ============================================================
static bool lua_item_dispatch(uint8_t id, const char* field) {
	lua_State* L = script_init();
	if (!L) return false;

	// 压栈: require + "items" → pcall → items_module
	lua_getglobal(L, "require");
	lua_pushstring(L, "items");
	if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
		fprintf(stderr, "require items: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);  // pop error string
		return false;
	}
	// 栈: ..., items_module

	lua_getfield(L, -1, "items");   // 栈: ..., items_module, items_table
	lua_pushinteger(L, id);          // 栈: ..., items_module, items_table, id
	lua_gettable(L, -2);             // 栈: ..., items_module, items_table, item_def
	if (!lua_istable(L, -1)) {
		lua_pop(L, 3);  // pop item_def, items_table, items_module
		return false;
	}

	lua_getfield(L, -1, field);     // 栈: ..., items_module, items_table, item_def, handler
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 4);  // pop handler, item_def, items_table, items_module
		return false;
	}

	// lua_pcall 弹出 handler，压入 1 个返回值（用于控制道具是否消耗）
	// 栈变为: ..., items_module, items_table, item_def, result
	if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
		// 错误时栈: ..., items_module, items_table, item_def, error_msg
		fprintf(stderr, "item %d %s: %s\n", id, field, lua_tostring(L, -1));
		lua_pop(L, 4);  // pop error, item_def, items_table, items_module
		return false;
	}

	// 若 handler 返回 boolean false 则不消耗道具
	bool consumed = true;
	if (lua_isboolean(L, -1))
		consumed = lua_toboolean(L, -1);
	lua_pop(L, 1);  // pop result

	lua_pop(L, 3);  // pop item_def, items_table, items_module
	return consumed;
}

bool lua_item_on_pickup(uint8_t tile_id) {
	lua_State* L = script_init();
	if (!L) return false;

	// 压栈: require + "items" → pcall → items_module
	lua_getglobal(L, "require");
	lua_pushstring(L, "items");
	if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
		fprintf(stderr, "require items: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);  // pop error
		return false;
	}
	// 栈: ..., items_module

	// 查 pickup_map[tile_id]
	lua_getfield(L, -1, "pickup_map");  // 栈: ..., items_module, pickup_map
	if (!lua_istable(L, -1)) { lua_pop(L, 2); return false; }
	lua_pushinteger(L, tile_id);        // 栈: ..., items_module, pickup_map, tile_id
	lua_gettable(L, -2);                // 栈: ..., items_module, pickup_map, item_id
	if (lua_isnil(L, -1)) { lua_pop(L, 3); return false; }
	int item_id = (int)lua_tointeger(L, -1);
	lua_pop(L, 2);  // pop item_id, pickup_map
	// 栈: ..., items_module

	// 查 items[item_id]
	lua_getfield(L, -1, "items");       // 栈: ..., items_module, items_table
	lua_pushinteger(L, item_id);        // 栈: ..., items_module, items_table, id
	lua_gettable(L, -2);                // 栈: ..., items_module, items_table, item_def
	if (!lua_istable(L, -1)) { lua_pop(L, 3); return false; }

	// 调 on_acquire
	lua_getfield(L, -1, "on_acquire");  // 栈: ..., items_module, items_table, item_def, handler
	if (!lua_isfunction(L, -1)) { lua_pop(L, 4); return false; }

	if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
		fprintf(stderr, "item %d on_acquire: %s\n", item_id, lua_tostring(L, -1));
		lua_pop(L, 4);  // pop error, item_def, items_table, items_module
		return false;
	}

	lua_pop(L, 3);  // pop item_def, items_table, items_module
	return true;
}

bool lua_item_on_acquire(uint8_t item_id) {
	return lua_item_dispatch(item_id, "on_acquire");
}

bool lua_item_on_use(uint8_t item_id) {
	return lua_item_dispatch(item_id, "on_use");
}
