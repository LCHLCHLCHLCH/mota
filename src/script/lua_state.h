#pragma once
extern "C" {
#include "lua.h"
}

lua_State* script_init();
void       script_close();
