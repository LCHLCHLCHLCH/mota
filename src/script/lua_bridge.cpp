#include "script/lua_bridge.h"
#include "game/player.h"
#include "game/map.h"
#include "game/monster.h"
#include "game/tile_data.h"
#include "event/event_manager.h"
#include "event/dialog.h"
#include "sdl_terminal.h"
#include "save_system.h"
#include "ui/backpack.h"
#include "sdl_3dwindow.h"
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

static int l_has_flag(lua_State* L) {
	int id = (int)luaL_checkinteger(L, 1);
	lua_pushboolean(L, g_ev ? g_ev->hasFlag((uint8_t)id) : 0);
	return 1;
}

static int l_set_flag(lua_State* L) {
	int id = (int)luaL_checkinteger(L, 1);
	if (g_ev) g_ev->setFlag((uint8_t)id);
	return 0;
}

// ============================================================
// 动作：对话、消息、地图替换
// ============================================================
static int l_say(lua_State* L) {
	const char* text = luaL_checkstring(L, 1);
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
static Player* g_ply = NULL;

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
	printf("%s = %d\n", attr, val);
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
	Player& p = *g_ply;
	switch (id) {
		case 51: p.yellowKey++; printf("获得黄钥匙\n"); break;
		case 52: p.blueKey++;   printf("获得蓝钥匙\n"); break;
		case 53: p.redKey++;    printf("获得红钥匙\n"); break;
		case 58: p.attack += 10;  printf("攻击+10\n"); break;
		case 59: p.defence += 10; printf("防御+10\n"); break;
		case 60: p.attack += 20;  printf("攻击+20\n"); break;
		case 61: p.defence += 20; printf("防御+20\n"); break;
		case 62: p.attack += 40;  printf("攻击+40\n"); break;
		case 63: p.defence += 40; printf("防御+40\n"); break;
		case 64: p.attack += 50;  printf("攻击+50\n"); break;
		case 65: p.defence += 50; printf("防御+50\n"); break;
		case 66: p.attack += 100;  printf("攻击+100\n"); break;
		case 67: p.defence += 100; printf("防御+100\n"); break;
		case 68: p.hasTeleporter = true; printf("获得楼层传送器\n"); break;
	case 69:
	case 70: {
		static Item cmdItem;
		cmdItem.name = (char*)getItemName((uint8_t)id);
		cmdItem.id = (uint8_t)id;
		if (p.backpack) p.backpack->addItem(&cmdItem);
		printf("获得%s\n", getItemName((uint8_t)id));
		break;
	}
	default: printf("未知道具 ID\n"); break;
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
	g_ply->init();
	if (g_ev) g_ev->init();
	for (int fl = 0; fl <= 50; fl++)
		for (int y = 0; y < 13; y++)
			for (int x = 0; x < 13; x++)
				map_set((uint8_t)fl, (uint8_t)x, (uint8_t)y,
					map_get_default((uint8_t)fl, (uint8_t)x, (uint8_t)y));
	printf("游戏已重启\n");
	return 0;
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
}
