#pragma once

#ifdef SDL3_BUILD
// SDL3 版本：使用 sdl_terminal 提供的函数
#include <../terminal/sdl_terminal.h>
#else
// ncurses 版本
#include <render/ncursesw/curses.h>
#endif

enum COLOR {
	RED,
	YELLOW,
	BLUE,
	WHITE,
	GREEN,
	PURPLE,
	GREY,
	LIGHT_GREEN
};

#define ATTR_LAVA      100
#define ATTR_WALL      101
#define ATTR_STAR      102
#define ATTR_EMPHASIS  103

void console_init();
void console_shutdown();
void drainInput();
void gotoxy(int x, int y);
void hideCursor();
void SetConsoleColor(int attr);
void SetColor(COLOR a);
void colorPrint(COLOR c, char *s);
void addstr_gbk(const char* s);
#ifdef SDL3_BUILD
void addch(char ch);
#endif
