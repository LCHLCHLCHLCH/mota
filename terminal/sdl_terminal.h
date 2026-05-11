#pragma once
#include <SDL3/SDL.h>

// 初始化终端（创建窗口、字体、颜色等）
bool term_init(const char* title, int cols, int rows, int cell_w, int cell_h);

// 关闭终端
void term_shutdown();

// 渲染一帧
void term_present();

// ============================================================
// 替代 ncurses 的 API（函数签名与 Cursor.h / dialog.h 兼容）
// ============================================================
void console_init();
void console_shutdown();
void gotoxy(int x, int y);
void hideCursor();
void SetConsoleColor(int attr);
void SetColor(int a);
void colorPrint(int c, char* s);
void addstr_gbk(const char* s);
void addch(char ch);
void drainInput();
void refresh_term();
void touchwin_term();
void erase_term();

// 输入（替代 key.h）
int  getch_term();

// 特殊颜色常量（与原 Cursor.h 一致）
#define ATTR_LAVA      100
#define ATTR_WALL      101
#define ATTR_STAR      102
#define ATTR_EMPHASIS  103

enum {
	COLOR_RED = 0,
	COLOR_YELLOW,
	COLOR_BLUE,
	COLOR_WHITE,
	COLOR_GREEN,
	COLOR_PURPLE,
	COLOR_GREY,
	COLOR_LIGHT_GREEN,
};
