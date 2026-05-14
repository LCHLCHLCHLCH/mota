#pragma once

#include <../terminal/sdl_terminal.h>

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
void addch(char ch);
