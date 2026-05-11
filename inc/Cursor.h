#pragma once

#include <ncursesw/curses.h>

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

// Special background attributes (used by SetConsoleColor)
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
