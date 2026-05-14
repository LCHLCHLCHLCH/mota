#include "console_cmd.h"
#include "save_system.h"
#include "game/player.h"
#include "event/event_manager.h"
#include "script/lua_state.h"
#include "script/lua_bridge.h"
extern "C" {
#include "lua.h"
#include "lauxlib.h"
}
#include <windows.h>
#include <cstdio>
#include <cstring>
#include <process.h>

static CRITICAL_SECTION g_cs;
static char  g_cmd_queue[8][256];
static int   g_cmd_head = 0, g_cmd_tail = 0;
static bool  g_cmd_thread_running = false;

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

static unsigned __stdcall console_thread(void* param) {
	(void)param;
	char buf[256];
	while (g_cmd_thread_running) {
		if (fgets(buf, sizeof(buf), stdin)) {
			size_t len = strlen(buf);
			while (len > 0 && (buf[len-1] == '\n' || buf[len-1] == '\r')) buf[--len] = 0;
			if (len > 0) cmd_push(buf);
			if (g_cmd_thread_running) { printf("> "); fflush(stdout); }
		} else Sleep(100);
	}
	return 0;
}

void console_welcome() {
	InitializeCriticalSection(&g_cs);
	printf("========================================\n");
	printf("  魔塔 SDL3 - 调试控制台 (Lua)\n");
	printf("  直接输入 Lua 代码，输入 help 查看帮助\n");
	printf("========================================\n\n");

	g_cmd_thread_running = true;
	_beginthreadex(NULL, 0, console_thread, NULL, 0, NULL);
	printf("> "); fflush(stdout);
}

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
		printf("  give(id)             给予道具\n");
		printf("  save/load(name)      存档/读档\n");
		printf("  light(\"on\"/\"off\")    切换浅色模式\n");
		printf("\n可直接输入任意 Lua 表达式:\n");
		printf("  print(2+3)\n");
		printf("  for i=1,5 do print(i) end\n");
		printf("\n");
		return;
	}

	lua_State* L = script_init();
	if (!L) { printf("Lua not initialized\n"); return; }

	int err = luaL_loadstring(L, line);
	if (err == LUA_OK) err = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (err != LUA_OK) {
		printf("error: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return;
	}

	int n = lua_gettop(L);
	if (n > 0) {
		lua_getglobal(L, "print");
		lua_insert(L, 1);
		lua_pcall(L, n, 0, 0);
	}
}

void console_poll(Player& player, EventManager& events) {
	// 确保 Lua state 存在并注册 API
	lua_State* L = script_init();
	if (L) lua_register_game_api(L, &player, &events);

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
}
