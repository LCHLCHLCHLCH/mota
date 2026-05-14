#include "console_cmd.h"
#include "save_system.h"
#include "game/player.h"
#include "game/map.h"
#include "event/event_manager.h"
#include "sdl_terminal.h"
#include "game/monster.h"
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include <windows.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <process.h>

// ============================================================
// 线程安全的命令队列
// ============================================================
static CRITICAL_SECTION g_cs;
static char  g_cmd_queue[8][256];
static int   g_cmd_head = 0, g_cmd_tail = 0;
static bool  g_cmd_thread_running = false;

static lua_State* g_L = NULL;
static Player*        g_player = NULL;
static EventManager*  g_events = NULL;

static void cmd_push(const char* cmd) {
	EnterCriticalSection(&g_cs);
	int next = (g_cmd_tail + 1) % 8;
	if (next != g_cmd_head) {
		strncpy(g_cmd_queue[g_cmd_tail], cmd, 255);
		g_cmd_queue[g_cmd_tail][255] = 0;
		g_cmd_tail = next;
	}
	LeaveCriticalSection(&g_cs);
}
static bool cmd_pop(char* out) {
	EnterCriticalSection(&g_cs);
	if (g_cmd_head == g_cmd_tail) { LeaveCriticalSection(&g_cs); return false; }
	strcpy(out, g_cmd_queue[g_cmd_head]);
	g_cmd_head = (g_cmd_head + 1) % 8;
	LeaveCriticalSection(&g_cs);
	return true;
}

// ============================================================
// Lua 可调用的 C++ 函数
// ============================================================
static int l_set(lua_State* L) {
	const char* attr = luaL_checkstring(L, 1);
	int val = (int)luaL_checkinteger(L, 2);
	Player& p = *g_player;

	if      (strcmp(attr, "health") == 0 || strcmp(attr, "hp") == 0)    p.health = val;
	else if (strcmp(attr, "attack") == 0 || strcmp(attr, "atk") == 0)   p.attack = val;
	else if (strcmp(attr, "defence") == 0 || strcmp(attr, "def") == 0)  p.defence = val;
	else if (strcmp(attr, "money") == 0 || strcmp(attr, "gold") == 0)   p.money = val;
	else if (strcmp(attr, "yellow") == 0)  p.yellowKey = (uint8_t)val;
	else if (strcmp(attr, "blue") == 0)    p.blueKey   = (uint8_t)val;
	else if (strcmp(attr, "red") == 0)     p.redKey    = (uint8_t)val;
	else if (strcmp(attr, "floor") == 0)   { p.floor = (uint8_t)val; if (p.floor > p.maxFloorVisited) p.maxFloorVisited = p.floor; }
	else if (strcmp(attr, "x") == 0)       p.x = (uint8_t)val;
	else if (strcmp(attr, "y") == 0)       p.y = (uint8_t)val;
	else { lua_pushstring(L, "unknown attribute"); lua_error(L); }
	printf("%s = %d\n", attr, val);
	return 0;
}

static int l_info(lua_State* L) {
	(void)L;
	Player& p = *g_player;
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
		g_player->floor = (uint8_t)f;
		if (g_player->floor > g_player->maxFloorVisited) g_player->maxFloorVisited = g_player->floor;
		printf("已传送到楼层 %d\n", f);
	} else printf("楼层范围: 0-50\n");
	return 0;
}

static int l_killall(lua_State* L) {
	(void)L;
	int count = 0;
	for (uint8_t y = 0; y < 13; y++)
		for (uint8_t x = 0; x < 13; x++) {
			uint8_t t = map_get(g_player->floor, x, y);
			if (t >= 101 && t <= 150) { map_set(g_player->floor, x, y, 1); count++; }
		}
	printf("已清除 %d 个怪物\n", count);
	return 0;
}

static int l_give(lua_State* L) {
	int id = (int)luaL_checkinteger(L, 1);
	Player& p = *g_player;
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
		default: printf("未知道具 ID\n"); break;
	}
	return 0;
}

static int l_save(lua_State* L) {
	const char* name = luaL_checkstring(L, 1);
	save_game(name, *g_player, *g_events);
	return 0;
}

static int l_load(lua_State* L) {
	const char* name = luaL_checkstring(L, 1);
	load_game(name, *g_player, *g_events);
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

// ============================================================
// 输入线程
// ============================================================
static unsigned __stdcall console_thread(void* param) {
	(void)param;
	char buf[256];
	while (g_cmd_thread_running) {
		if (fgets(buf, sizeof(buf), stdin)) {
			size_t len = strlen(buf);
			while (len > 0 && (buf[len-1] == '\n' || buf[len-1] == '\r'))
				buf[--len] = 0;
			if (len > 0) cmd_push(buf);
			if (g_cmd_thread_running) { printf("> "); fflush(stdout); }
		} else Sleep(100);
	}
	return 0;
}

// ============================================================
// 初始化
// ============================================================
void console_welcome() {
	InitializeCriticalSection(&g_cs);
	printf("========================================\n");
	printf("  魔塔 SDL3 - 调试控制台 (Lua)\n");
	printf("  直接输入 Lua 代码，输入 help 查看帮助\n");
	printf("========================================\n\n");

	g_L = luaL_newstate();
	luaL_openlibs(g_L);

	// 注册 Game API
	lua_register(g_L, "set",    l_set);
	lua_register(g_L, "info",   l_info);
	lua_register(g_L, "tp",     l_tp);
	lua_register(g_L, "killall",l_killall);
	lua_register(g_L, "give",   l_give);
	lua_register(g_L, "save",   l_save);
	lua_register(g_L, "load",   l_load);
	lua_register(g_L, "light",  l_light);

	g_cmd_thread_running = true;
	_beginthreadex(NULL, 0, console_thread, NULL, 0, NULL);
	printf("> "); fflush(stdout);
}

// ============================================================
// 处理一行输入：help 走 C++，其余走 Lua
// ============================================================
static void process_line(const char* line) {
	while (*line == ' ' || *line == '\t') line++;
	if (*line == 0) return;

	if (strcmp(line, "help") == 0 || strcmp(line, "h") == 0 || strcmp(line, "?") == 0) {
		printf("\n==== Lua 控制台 ====\n");
		printf("  help / h / ?         显示此帮助\n");
		printf("  set(\"attr\", value)    设置属性\n");
		printf("  info()               显示玩家状态\n");
		printf("  tp(floor)            传送到楼层\n");
		printf("  killall()            清除当前层怪物\n");
		printf("  give(id)             给予道具 (51-68)\n");
		printf("  save(\"name\")         保存游戏\n");
		printf("  load(\"name\")         读取存档\n");
		printf("  light(\"on\"/\"off\")    切换浅色模式\n");
		printf("\n可直接输入任意 Lua 表达式:\n");
		printf("  print(2+3)\n");
		printf("  for i=1,5 do print(i) end\n");
		printf("\n");
		return;
	}

	// 尝试作为 Lua 语句执行
	int err = luaL_loadstring(g_L, line);
	if (err == LUA_OK) {
		err = lua_pcall(g_L, 0, LUA_MULTRET, 0);
	}
	if (err != LUA_OK) {
		printf("error: %s\n", lua_tostring(g_L, -1));
		lua_pop(g_L, 1);
		return;
	}

	// 打印返回值
	int n = lua_gettop(g_L);
	if (n > 0) {
		lua_getglobal(g_L, "print");
		lua_insert(g_L, 1);
		lua_pcall(g_L, n, 0, 0);
	}
}

void console_poll(Player& player, EventManager& events) {
	g_player = &player;
	g_events = &events;

	char cmd[256];
	while (cmd_pop(cmd)) {
		if (cmd[0] == 0) continue;
		process_line(cmd);
		printf("> "); fflush(stdout);
	}
}

void console_cmd_shutdown() {
	g_cmd_thread_running = false;
	cmd_push("");
	DeleteCriticalSection(&g_cs);
	if (g_L) { lua_close(g_L); g_L = NULL; }
}
