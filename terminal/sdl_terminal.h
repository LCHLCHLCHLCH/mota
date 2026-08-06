#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

// ============================================================
// SDL3 终端初始化 / 关闭 / 渲染
// ============================================================
bool term_init(const char* title, int cols, int rows, int cell_w, int cell_h);
void term_shutdown();
void term_present();
SDL_Window* term_get_window();
TTF_Font*   term_get_ttf_font();
float       term_get_display_scale();

// ============================================================
// ncurses 兼容 API（与 src/render/cursor.h 一致）
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

// SDL3 特有刷新函数（替代 ncurses 的 touchwin / refresh / erase）
void refresh_term();
void touchwin_term();
void erase_term();

// ============================================================
// 键盘输入（替代 ncurses getch）
// ============================================================
int  getch_term();
bool term_quit_requested();
void term_set_quit();
void term_set_light_mode(bool on);
void term_set_darkened(bool on);
bool term_is_darkened();

// ============================================================
// printf 风格输出（替代 ncurses printw）
// ============================================================
int  term_printw(const char* fmt, ...);

// ============================================================
// 直接 SDL 文本（绕过 cell，用于状态栏数值等需要紧凑排版的场景）
// 坐标单位：像素
// ============================================================
void term_draw_text(int px, int py, int pw, int ph,
                    const char* text, int fg_color, int bg_color);
// 大字号文本（巨型 Boss 角色渲染用）
void term_draw_big_text(int px, int py, int pw, int ph,
                        const char* text, int fg_color, int bg_color);
void term_clear_draws();

// ============================================================
// 底部消息系统（走一步后自动消失）
// ============================================================
void term_set_message(const char* msg);
void term_clear_message();

// ============================================================
// 颜色常量（与 src/render/cursor.h 中的值保持一致）
// ============================================================
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

#define ATTR_LAVA      100
#define ATTR_WALL      101
#define ATTR_STAR      102
#define ATTR_EMPHASIS  103
