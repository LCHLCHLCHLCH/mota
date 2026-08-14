#include "script/lua_state.h"
extern "C" {
#include "lualib.h"
#include "lauxlib.h"
}
#include <cstdio>
#include <cstring>
#include <windows.h>

static lua_State* g_L = NULL;

lua_State* script_init() {
	if (g_L) return g_L;
	g_L = luaL_newstate();
	if (!g_L) return NULL;
	luaL_openlibs(g_L);

	// 设置 Lua 脚本搜索路径：scripts/?.lua 与 scripts/map/?.lua
	char exe_path[MAX_PATH];
	GetModuleFileNameA(NULL, exe_path, MAX_PATH);
	char* last_slash = strrchr(exe_path, '\\');
	if (last_slash) *last_slash = 0;

	lua_getglobal(g_L, "package");
	lua_getfield(g_L, -1, "path");
	const char* old_path = lua_tostring(g_L, -1);
	char new_path[1024];
	snprintf(new_path, sizeof(new_path), "%s\\scripts\\?.lua;%s\\scripts\\map\\?.lua;%s",
	         exe_path, exe_path, old_path);
	lua_pop(g_L, 1);
	lua_pushstring(g_L, new_path);
	lua_setfield(g_L, -2, "path");
	lua_pop(g_L, 1);

	return g_L;
}

void script_close() {
	if (g_L) { lua_close(g_L); g_L = NULL; }
}
