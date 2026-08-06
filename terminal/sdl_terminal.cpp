#include "sdl_terminal.h"
#include <render/cursor.h>
#include <windows.h>
#include <cstdio>
#include <cstring>
#include <cstdarg>

// ============================================================
// 内部状态
// ============================================================
static SDL_Window*   g_window    = nullptr;
static SDL_Renderer* g_renderer  = nullptr;

static int g_cols = 28, g_rows = 22;
static int g_cell_w = 22, g_cell_h = 22;
static int g_win_w = 616, g_win_h = 484;

// SDL_ttf 字体
static TTF_Font* g_ttf_font = nullptr;
// 大字号字体（巨型 Boss 角色渲染用，约 3 倍 cell 尺寸）
static TTF_Font* g_ttf_font_big = nullptr;

// 调色板
static SDL_Color g_palette[16];

// 当前颜色
static int g_cur_fg = 4;
static int g_cur_bg = 0;

// 光标
static int g_cursor_x = 0;
static int g_cursor_y = 0;

// 退出标志
static bool g_quit = false;

// 浅色模式
static bool g_light_mode = false;
static bool g_darkened   = false;

// 底部消息
static char g_message[128] = "";

// 高 DPI 显示缩放系数（跟随系统显示缩放，如 150% 时为 1.5）
static float g_scale = 1.0f;

// Cell 缓冲
struct Cell {
	wchar_t ch;
	int     fg;
	int     bg;
};
static Cell* g_cells = nullptr;

// 延迟直接绘制
#define MAX_DEFERRED 256
struct DeferredDraw {
	int  px, py, pw, ph;
	char text[64];
	int  fg, bg;
	bool big;   // true 时用大字号字体渲染
};
static DeferredDraw g_deferred[MAX_DEFERRED];
static int          g_deferred_count = 0;

// ============================================================
// wchar_t → UTF-8 转换（用于 SDL_ttf 文本渲染）
// ============================================================
static char* wchar_to_utf8(const wchar_t* wstr, int wlen) {
	if (wlen <= 0) return NULL;
	int ulen = WideCharToMultiByte(CP_UTF8, 0, wstr, wlen, NULL, 0, NULL, NULL);
	if (ulen <= 0) return NULL;
	char* utf8 = new char[ulen + 1];
	WideCharToMultiByte(CP_UTF8, 0, wstr, wlen, utf8, ulen, NULL, NULL);
	utf8[ulen] = 0;
	return utf8;
}

// ============================================================
// 调色板
// ============================================================
static void init_palette() {
	g_palette[0]  = { 0,   0,   0,   255 };  // BLACK
	g_palette[1]  = { 255, 50,  50,  255 };  // RED
	g_palette[2]  = { 255, 255, 0,   255 };  // YELLOW
	g_palette[3]  = { 60,  60,  255, 255 };  // BLUE
	g_palette[4]  = { 255, 255, 255, 255 };  // WHITE
	g_palette[5]  = { 50,  255, 50,  255 };  // GREEN
	g_palette[6]  = { 255, 50,  255, 255 };  // PURPLE
	g_palette[7]  = { 160, 160, 160, 255 };  // GREY
	g_palette[8]  = { 50,  255, 255, 255 };  // CYAN / LIGHT_GREEN
	for (int i = 9; i < 16; i++)
		g_palette[i] = { 0, 0, 0, 255 };
}

// ============================================================
// 颜色映射
// ============================================================
static int color_to_idx(COLOR c) {
	switch (c) {
		case RED:         return 1;
		case YELLOW:      return 2;
		case BLUE:        return 3;
		case WHITE:       return 4;
		case GREEN:       return 5;
		case PURPLE:      return 6;
		case GREY:        return 7;
		case LIGHT_GREEN: return 8;
		default:          return 4;
	}
}

static void attr_to_colors(int attr, int& fg, int& bg) {
	switch (attr) {
		case 7 * 16 | 7:  fg = 4; bg = 4; break;
		case 64 | 7:      fg = 4; bg = 1; break;
		case 1 * 16 | 7:  fg = 4; bg = 3; break;
		case 9 * 16 | 7:  fg = 4; bg = 3; break;
		default:          fg = 4; bg = 0; break;
	}
}

// ============================================================
// 加载字体
// ============================================================
static void load_font() {
	if (g_ttf_font) { TTF_CloseFont(g_ttf_font); g_ttf_font = nullptr; }
	if (g_ttf_font_big) { TTF_CloseFont(g_ttf_font_big); g_ttf_font_big = nullptr; }
	int size = (int)((g_cell_h - 2) * g_scale + 0.5f);
	if (size < 8) size = 8;
	char exe_path[MAX_PATH];
	GetModuleFileNameA(NULL, exe_path, MAX_PATH);
	char* last_slash = strrchr(exe_path, '\\');
	if (last_slash) {
		*last_slash = 0;
		char font_path[MAX_PATH];
		snprintf(font_path, sizeof(font_path), "%s\\simsun.ttc", exe_path);
		if (GetFileAttributesA(font_path) != INVALID_FILE_ATTRIBUTES) {
			g_ttf_font = TTF_OpenFont(font_path, size);
			g_ttf_font_big = TTF_OpenFont(font_path, size * 3);
		}
	}
	if (!g_ttf_font) {
		// 回退到系统字体
		char sys_path[MAX_PATH];
		GetWindowsDirectoryA(sys_path, MAX_PATH);
		char font_path[MAX_PATH];
		snprintf(font_path, sizeof(font_path), "%s\\Fonts\\simsun.ttc", sys_path);
		g_ttf_font = TTF_OpenFont(font_path, size);
		g_ttf_font_big = TTF_OpenFont(font_path, size * 3);
	}
}

// ============================================================
// 窗口适配：等比缩放画面以填满当前窗口（居中留边），
// 同时兼顾高 DPI 缩放与用户手动拖拽 / 最大化
// ============================================================
static void apply_window_fit() {
	int w = 0, h = 0;
	SDL_GetWindowSizeInPixels(g_window, &w, &h);
	if (w <= 0 || h <= 0) return;

	float sx = (float)w / g_win_w;
	float sy = (float)h / g_win_h;
	float scale = (sx < sy) ? sx : sy;
	if (scale < 0.2f) scale = 0.2f;

	bool changed = (scale != g_scale);
	g_scale = scale;

	int vw = (int)(g_win_w * scale + 0.5f);
	int vh = (int)(g_win_h * scale + 0.5f);
	SDL_Rect vp = { (w - vw) / 2, (h - vh) / 2, vw, vh };
	SDL_SetRenderViewport(g_renderer, &vp);
	SDL_SetRenderScale(g_renderer, scale, scale);

	if (changed || !g_ttf_font) load_font();
}

// ============================================================
// Cell 操作
// ============================================================
static Cell* cell_at(int col, int row) {
	if (col < 0 || col >= g_cols || row < 0 || row >= g_rows)
		return nullptr;
	return &g_cells[row * g_cols + col];
}

static void put_cell(int col, int row, wchar_t ch, int fg, int bg) {
	Cell* c = cell_at(col, row);
	if (!c) return;
	c->ch = ch;
	c->fg = fg;
	c->bg = bg;
}

static void put_str(int col, int row, const wchar_t* s, int fg, int bg) {
	int start_col = col;
	while (*s) {
		if (col >= g_cols) { row++; col = start_col; }
		if (row >= g_rows) break;
		put_cell(col, row, *s, fg, bg);
		col++;
		s++;
	}
}

// ============================================================
// SDL 渲染一个 cell
// ============================================================
static void draw_cell_sdl(int col, int row) {
	Cell* c = cell_at(col, row);
	if (!c) return;

	int cx = col * g_cell_w;
	int cy = row * g_cell_h;
	SDL_FRect rect = { (float)cx, (float)cy, (float)g_cell_w, (float)g_cell_h };

	if (g_darkened) {
		SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
		SDL_RenderFillRect(g_renderer, &rect);
		return;
	}

	SDL_Color& bg_c = g_palette[c->bg];
	SDL_SetRenderDrawColor(g_renderer, bg_c.r, bg_c.g, bg_c.b, bg_c.a);
	SDL_RenderFillRect(g_renderer, &rect);

	if (c->ch != L' ') {
		SDL_Color& fg_c = g_palette[c->fg];
		char* utf8 = wchar_to_utf8(&c->ch, 1);
		if (utf8) {
			SDL_Surface* surf = TTF_RenderText_Blended(g_ttf_font, utf8, 0, fg_c);
			if (surf) {
				SDL_Texture* tex = SDL_CreateTextureFromSurface(g_renderer, surf);
				if (tex) {
					float tw = (float)surf->w;
					float th = (float)surf->h;
					float s = g_scale;
					SDL_FRect trect = {
						cx + (g_cell_w - tw / s) * 0.5f,
						cy + (g_cell_h - th / s) * 0.5f,
						tw / s, th / s
					};
					SDL_RenderTexture(g_renderer, tex, NULL, &trect);
					SDL_DestroyTexture(tex);
				}
				SDL_DestroySurface(surf);
			}
			delete[] utf8;
		}
	}
}

// ============================================================
// SDL 渲染文本（状态栏数值、底部消息等）
// ============================================================
static void draw_text_rect(int px, int py, int pw, int ph,
                           const char* utf8_text, SDL_Color fg, SDL_Color bg,
                           TTF_Font* font) {
	SDL_FRect rect = { (float)px, (float)py, (float)pw, (float)ph };
	SDL_SetRenderDrawColor(g_renderer, bg.r, bg.g, bg.b, bg.a);
	SDL_RenderFillRect(g_renderer, &rect);

	SDL_Surface* surf = TTF_RenderText_Blended(font, utf8_text, 0, fg);
	if (surf) {
		SDL_Texture* tex = SDL_CreateTextureFromSurface(g_renderer, surf);
		if (tex) {
			float tw = (float)surf->w;
			float th = (float)surf->h;
			float s = g_scale;
			SDL_FRect trect = {
				px + (pw - tw / s) * 0.5f,
				py + (ph - th / s) * 0.5f,
				tw / s, th / s
			};
			SDL_RenderTexture(g_renderer, tex, NULL, &trect);
			SDL_DestroyTexture(tex);
		}
		SDL_DestroySurface(surf);
	}
}

// ============================================================
// 初始化 / 关闭
// ============================================================
bool term_init(const char* title, int cols, int rows, int cell_w, int cell_h) {
	g_cols = cols; g_rows = rows;
	g_cell_w = cell_w; g_cell_h = cell_h;
	g_win_w = cols * cell_w;
	g_win_h = rows * cell_h;

	g_cells = new Cell[cols * rows];
	for (int i = 0; i < cols * rows; i++) {
		g_cells[i].ch = L' ';
		g_cells[i].fg = 4;
		g_cells[i].bg = 0;
	}

	init_palette();

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
		return false;
	}

	TTF_Init();

	g_window = SDL_CreateWindow(title, g_win_w, g_win_h, SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
	if (!g_window) {
		fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
		return false;
	}

	// 初始窗口尺寸 = 设计分辨率 × 显示器缩放，保证在高分屏上物理尺寸合适
	float dpi = SDL_GetWindowDisplayScale(g_window);
	if (dpi < 1.0f) dpi = 1.0f;
	SDL_SetWindowSize(g_window,
		(int)(g_win_w * dpi + 0.5f), (int)(g_win_h * dpi + 0.5f));

	g_renderer = SDL_CreateRenderer(g_window, NULL);
	if (!g_renderer) {
		fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
		return false;
	}

	// 按窗口尺寸计算渲染缩放并加载相应字号字体
	apply_window_fit();
	if (!g_ttf_font) {
		fprintf(stderr, "Failed to load font\n");
		return false;
	}

	// 设置窗口图标
	{
		HBITMAP hBmp = (HBITMAP)LoadImageW(NULL, L"logo/logo2.png",
			IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (hBmp) {
			BITMAP bm;
			GetObjectW(hBmp, sizeof(bm), &bm);
			int w = bm.bmWidth, h = bm.bmHeight;
			BITMAPINFOHEADER bi = {};
			bi.biSize = sizeof(bi);
			bi.biWidth = w; bi.biHeight = -h;
			bi.biPlanes = 1; bi.biBitCount = 32; bi.biCompression = BI_RGB;
			int rb = w * 4;
			BYTE* px = new BYTE[h * rb];
			HDC hdc = GetDC(NULL);
			GetDIBits(hdc, hBmp, 0, h, px, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
			ReleaseDC(NULL, hdc);
			SDL_Surface* surf = SDL_CreateSurfaceFrom(w, h,
				SDL_PIXELFORMAT_BGRA8888, px, rb);
			if (surf) { SDL_SetWindowIcon(g_window, surf); SDL_DestroySurface(surf); }
			delete[] px;
			DeleteObject(hBmp);
		}
	}

	SDL_ShowWindow(g_window);
	return true;
}

SDL_Window* term_get_window() { return g_window; }
TTF_Font* term_get_ttf_font() { return g_ttf_font; }
float term_get_display_scale() { return g_scale; }

void term_shutdown() {
	delete[] g_cells; g_cells = nullptr;
	if (g_ttf_font) { TTF_CloseFont(g_ttf_font); g_ttf_font = nullptr; }
	TTF_Quit();
	if (g_renderer) { SDL_DestroyRenderer(g_renderer); g_renderer = nullptr; }
	if (g_window)   { SDL_DestroyWindow(g_window); g_window = nullptr; }
	SDL_Quit();
}

// ============================================================
// 帧渲染
// ============================================================
void term_present() {
	// 1. 清屏
	SDL_Color& bg0 = g_palette[0];
	SDL_SetRenderDrawColor(g_renderer, bg0.r, bg0.g, bg0.b, 255);
	SDL_RenderClear(g_renderer);

	// 2. 逐 cell 渲染
	for (int row = 0; row < g_rows; row++)
		for (int col = 0; col < g_cols; col++)
			draw_cell_sdl(col, row);

	// 3. 延迟直接绘制（状态栏数值等）
	for (int i = 0; i < g_deferred_count; i++) {
		DeferredDraw& d = g_deferred[i];
		draw_text_rect(d.px, d.py, d.pw, d.ph, d.text,
			g_palette[d.fg], g_palette[d.bg],
			d.big ? g_ttf_font_big : g_ttf_font);
	}

	// 4. 底部消息
	if (g_message[0]) {
		draw_text_rect(0, (g_rows - 1) * g_cell_h, g_win_w, g_cell_h,
			g_message, g_palette[4], g_palette[0], g_ttf_font);
	}

	// 5. 呈现
	SDL_RenderPresent(g_renderer);
}

// ============================================================
// 清屏 / 刷新
// ============================================================
void touchwin_term() {
	for (int i = 0; i < g_cols * g_rows; i++) {
		g_cells[i].ch = L' ';
		g_cells[i].fg = 4;
		g_cells[i].bg = 0;
	}
}

void erase_term() {
	touchwin_term();
	g_cursor_x = 0; g_cursor_y = 0;
	g_cur_fg = 4; g_cur_bg = 0;
}

void refresh_term() { term_present(); }

// ============================================================
// ncurses 兼容层
// ============================================================
void console_init()   { term_init("魔塔 - SDL3", 28, 22, 22, 22); }
void console_shutdown(){ term_shutdown(); }
void gotoxy(int x, int y) { g_cursor_x = x; g_cursor_y = y; }
void hideCursor() {}
void drainInput() {}

void SetConsoleColor(int attr) { attr_to_colors(attr, g_cur_fg, g_cur_bg); }

void SetColor(COLOR a) { g_cur_fg = color_to_idx(a); g_cur_bg = 0; }

void colorPrint(COLOR c, char* s) {
	int prev_fg = g_cur_fg;
	g_cur_fg = color_to_idx(c);
	addstr_gbk(s);
	g_cur_fg = prev_fg;
}

void addstr_gbk(const char* s) {
	int len = (int)strlen(s);
	if (len == 0) return;
	int wlen = MultiByteToWideChar(CP_UTF8, 0, s, len, NULL, 0);
	if (wlen <= 0) return;
	wchar_t* buf = new wchar_t[wlen + 1];
	MultiByteToWideChar(CP_UTF8, 0, s, len, buf, wlen);
	buf[wlen] = 0;
	put_str(g_cursor_x, g_cursor_y, buf, g_cur_fg, g_cur_bg);
	g_cursor_x += wlen;
	delete[] buf;
}

void addch(char ch) {
	if (g_cursor_x >= g_cols) { g_cursor_x = 0; g_cursor_y++; }
	put_cell(g_cursor_x, g_cursor_y, (wchar_t)ch, g_cur_fg, g_cur_bg);
	g_cursor_x++;
}

// ============================================================
// 键盘输入
// ============================================================
int getch_term() {
	for (;;) {
		SDL_Event ev;
		if (!SDL_WaitEventTimeout(&ev, 100)) return -1;
		switch (ev.type) {
		case SDL_EVENT_QUIT: g_quit = true; return 0;
		case SDL_EVENT_WINDOW_RESIZED:
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
		case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
			apply_window_fit(); break;
		case SDL_EVENT_KEY_DOWN: {
			int code = (int)ev.key.key;
			switch (code) {
				case SDLK_UP:     return 0x103;
				case SDLK_DOWN:   return 0x102;
				case SDLK_LEFT:   return 0x104;
				case SDLK_RIGHT:  return 0x105;
				default:          return code;
			}
		}
		default: break;
		}
	}
}

// ============================================================
// printf 风格输出
// ============================================================
int term_printw(const char* fmt, ...) {
	char buf[256];
	va_list args;
	va_start(args, fmt);
	int len = vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	if (len <= 0) return 0;
	int wlen = MultiByteToWideChar(CP_UTF8, 0, buf, len, NULL, 0);
	if (wlen <= 0) return 0;
	wchar_t* wbuf = new wchar_t[wlen + 1];
	MultiByteToWideChar(CP_UTF8, 0, buf, len, wbuf, wlen);
	wbuf[wlen] = 0;
	put_str(g_cursor_x, g_cursor_y, wbuf, g_cur_fg, g_cur_bg);
	g_cursor_x += wlen;
	delete[] wbuf;
	return 0;
}

// ============================================================
// 消息 / 退出手柄
// ============================================================
bool term_quit_requested() { return g_quit; }

void term_set_light_mode(bool on) {
	if (g_light_mode == on) return;
	g_light_mode = on;
	SDL_Color tmp = g_palette[0];
	g_palette[0] = g_palette[4];
	g_palette[4] = tmp;
}

void term_set_darkened(bool on) { g_darkened = on; }
bool term_is_darkened()         { return g_darkened; }

void term_set_message(const char* msg) {
	strncpy(g_message, msg, 127);
	g_message[127] = 0;
}

void term_clear_message() { g_message[0] = 0; }

// ============================================================
// 延迟绘制
// ============================================================
void term_draw_text(int px, int py, int pw, int ph,
                    const char* text, int fg_color, int bg_color) {
	if (g_deferred_count >= MAX_DEFERRED) return;
	DeferredDraw& d = g_deferred[g_deferred_count++];
	d.px = px; d.py = py; d.pw = pw; d.ph = ph;
	strncpy(d.text, text, 63);
	d.text[63] = 0;
	d.fg = fg_color; d.bg = bg_color;
	d.big = false;
}

void term_draw_big_text(int px, int py, int pw, int ph,
                        const char* text, int fg_color, int bg_color) {
	if (g_deferred_count >= MAX_DEFERRED) return;
	DeferredDraw& d = g_deferred[g_deferred_count++];
	d.px = px; d.py = py; d.pw = pw; d.ph = ph;
	strncpy(d.text, text, 63);
	d.text[63] = 0;
	d.fg = fg_color; d.bg = bg_color;
	d.big = true;
}

void term_clear_draws() { g_deferred_count = 0; }
