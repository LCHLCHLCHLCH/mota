#include "sdl_terminal.h"
#include <Cursor.h>
#include <windows.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>

// ============================================================
// 内部状态
// ============================================================
static SDL_Window*   g_window   = nullptr;
static SDL_Renderer* g_renderer = nullptr;
static SDL_Texture*  g_texture  = nullptr;

static int g_cols, g_rows, g_cell_w, g_cell_h;
static int g_win_w, g_win_h;

// GDI 资源
static HDC     g_mem_dc     = nullptr;
static HBITMAP g_mem_bmp    = nullptr;
static HBITMAP g_old_bmp    = nullptr;
static HFONT   g_font       = nullptr;
static HBRUSH  g_bg_brush   = nullptr;

// 颜色表
static SDL_Color g_palette[16];
static int       g_cur_fg = 7;   // 默认白前景
static int       g_cur_bg = 0;   // 默认黑背景
static int       g_cur_pair = 1;

// 光标
static int g_cursor_x = 0;
static int g_cursor_y = 0;

// 脏标记
static bool g_dirty = true;

// 屏幕缓冲（记录每个 cell 的字符和颜色，用于 GDI 重绘）
struct Cell {
	wchar_t ch;
	int     fg;   // 调色板索引
	int     bg;
	bool    dirty;
};
static Cell* g_cells = nullptr;

// ============================================================
// 调色板初始化
// ============================================================
static void init_palette() {
	g_palette[0] = { 0,   0,   0,   255 };  // BLACK
	g_palette[1] = { 255, 50,  50,  255 };  // RED
	g_palette[2] = { 255, 255, 0,   255 };  // YELLOW
	g_palette[3] = { 60,  60,  255, 255 };  // BLUE
	g_palette[4] = { 255, 255, 255, 255 };  // WHITE (纯白)
	g_palette[5] = { 50,  255, 50,  255 };  // GREEN
	g_palette[6] = { 255, 50,  255, 255 };  // PURPLE
	g_palette[7] = { 160, 160, 160, 255 };  // GREY
	g_palette[8] = { 50,  255, 255, 255 };  // CYAN / LIGHT_GREEN
	for (int i = 9; i < 16; i++)
		g_palette[i] = { 0, 0, 0, 255 };
}

static int color_to_idx(int c) {
	switch (c) {
		case COLOR_RED:         return 1;
		case COLOR_YELLOW:      return 2;
		case COLOR_BLUE:        return 3;
		case COLOR_WHITE:       return 4;
		case COLOR_GREEN:       return 5;
		case COLOR_PURPLE:      return 6;
		case COLOR_GREY:        return 7;
		case COLOR_LIGHT_GREEN: return 8;
		default:                return 4;
	}
}

// 特殊属性 → 前景/背景色
static void attr_to_colors(int attr, int& fg, int& bg) {
	switch (attr) {
		case 7 * 16 | 7:  fg = 4; bg = 4; break;  // WALL:  白底白字
		case 64 | 7:      fg = 4; bg = 1; break;  // LAVA:  红底白字
		case 1 * 16 | 7:  fg = 4; bg = 3; break;  // STAR:  蓝底白字
		case 9 * 16 | 7:  fg = 4; bg = 3; break;  // EMPH:  蓝底白字
		default:          fg = 4; bg = 0; break;  // 默认
	}
}

// ============================================================
// GDI 字体创建（支持 CJK）
// ============================================================
static void create_font() {
	g_font = CreateFontW(
		g_cell_h, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN,
		L"SimSun"  // 宋体，Windows 自带 CJK 等宽字体
	);
}

// ============================================================
// GDI 渲染一个 cell
// ============================================================
static void draw_cell_gdi(int col, int row) {
	int idx = row * g_cols + col;
	Cell& c = g_cells[idx];
	if (!c.dirty) return;
	c.dirty = false;

	int x = col * g_cell_w;
	int y = row * g_cell_h;
	RECT rect = { x, y, x + g_cell_w, y + g_cell_h };

	// 背景
	SDL_Color& bg_c = g_palette[c.bg];
	HBRUSH brush = CreateSolidBrush(RGB(bg_c.r, bg_c.g, bg_c.b));
	FillRect(g_mem_dc, &rect, brush);
	DeleteObject(brush);

	// 前景（文字）
	if (c.ch != L' ') {
		SDL_Color& fg_c = g_palette[c.fg];
		SetTextColor(g_mem_dc, RGB(fg_c.r, fg_c.g, fg_c.b));
		SetBkMode(g_mem_dc, TRANSPARENT);
		SelectObject(g_mem_dc, g_font);

		// 居中绘制
		RECT text_rect = rect;
		DrawTextW(g_mem_dc, &c.ch, 1, &text_rect,
			DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
}

// ============================================================
// 公共接口
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
		g_cells[i].dirty = true;
	}

	init_palette();

	// SDL3 初始化
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
		return false;
	}

	g_window = SDL_CreateWindow(title, g_win_w, g_win_h, 0);
	if (!g_window) {
		fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
		return false;
	}

	g_renderer = SDL_CreateRenderer(g_window, NULL);
	if (!g_renderer) {
		fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
		return false;
	}

	g_texture = SDL_CreateTexture(g_renderer,
		SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING,
		g_win_w, g_win_h);
	if (!g_texture) {
		fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
		return false;
	}

	// GDI 初始化
	HDC screen_dc = GetDC(NULL);
	g_mem_dc = CreateCompatibleDC(screen_dc);

	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = g_win_w;
	bmi.bmiHeader.biHeight = -g_win_h;  // top-down
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	void* bits = nullptr;
	g_mem_bmp = CreateDIBSection(g_mem_dc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
	g_old_bmp = (HBITMAP)SelectObject(g_mem_dc, g_mem_bmp);
	ReleaseDC(NULL, screen_dc);

	create_font();

	return true;
}

void term_shutdown() {
	delete[] g_cells;
	g_cells = nullptr;

	if (g_font) DeleteObject(g_font);
	if (g_old_bmp) SelectObject(g_mem_dc, g_old_bmp);
	if (g_mem_bmp) DeleteObject(g_mem_bmp);
	if (g_mem_dc) DeleteDC(g_mem_dc);

	if (g_texture) SDL_DestroyTexture(g_texture);
	if (g_renderer) SDL_DestroyRenderer(g_renderer);
	if (g_window) SDL_DestroyWindow(g_window);
	SDL_Quit();
}

void term_present() {
	// 更新所有脏 cell 到 GDI 位图
	for (int row = 0; row < g_rows; row++)
		for (int col = 0; col < g_cols; col++)
			draw_cell_gdi(col, row);

	// 上传 GDI 位图到 SDL3 纹理
	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = g_win_w;
	bmi.bmiHeader.biHeight = -g_win_h;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	void* bits = nullptr;
	GetDIBits(g_mem_dc, g_mem_bmp, 0, g_win_h, NULL, &bmi, DIB_RGB_COLORS);

	BYTE* dib_bits = new BYTE[g_win_w * g_win_h * 4];
	GetDIBits(g_mem_dc, g_mem_bmp, 0, g_win_h, dib_bits, &bmi, DIB_RGB_COLORS);

	SDL_UpdateTexture(g_texture, NULL, dib_bits, g_win_w * 4);
	delete[] dib_bits;

	SDL_RenderTexture(g_renderer, g_texture, NULL, NULL);
	SDL_RenderPresent(g_renderer);
}

// ============================================================
// 写入 cell
// ============================================================
static void put_cell(int col, int row, wchar_t ch, int fg, int bg) {
	if (col < 0 || col >= g_cols || row < 0 || row >= g_rows) return;
	int idx = row * g_cols + col;
	g_cells[idx].ch = ch;
	g_cells[idx].fg = fg;
	g_cells[idx].bg = bg;
	g_cells[idx].dirty = true;
	g_dirty = true;
}

static void put_str(int col, int row, const wchar_t* s, int fg, int bg) {
	while (*s) {
		put_cell(col, row, *s, fg, bg);
		col += (*s >= 0x80 || *s > 255) ? 2 : 1;  // CJK = 2 cols
		s++;
	}
}

// ============================================================
// ncurses 兼容层
// ============================================================
void console_init() {
	term_init("魔塔 - SDL3", 40, 16, 16, 24);
}

void console_shutdown() {
	term_shutdown();
}

void gotoxy(int x, int y) {
	g_cursor_x = x;
	g_cursor_y = y;
}

void hideCursor() {}

void SetConsoleColor(int attr) {
	attr_to_colors(attr, g_cur_fg, g_cur_bg);
}

void SetColor(COLOR a) {
	g_cur_fg = color_to_idx(a);
	g_cur_bg = 0;
}

void colorPrint(COLOR c, char* s) {
	g_cur_fg = color_to_idx(c);
	addstr_gbk(s);
}

void addstr_gbk(const char* s) {
	int len = (int)strlen(s);
	if (len == 0) return;
	int wlen = MultiByteToWideChar(936, 0, s, len, NULL, 0);
	if (wlen <= 0) return;
	wchar_t* buf = (wchar_t*)alloca((wlen + 1) * sizeof(wchar_t));
	MultiByteToWideChar(936, 0, s, len, buf, wlen);
	buf[wlen] = 0;
	put_str(g_cursor_x, g_cursor_y, buf, g_cur_fg, g_cur_bg);
	g_cursor_x += len;  // GBK: byte count = column count
}

void addch(char ch) {
	put_cell(g_cursor_x, g_cursor_y, (wchar_t)ch, g_cur_fg, g_cur_bg);
	g_cursor_x++;
}

void drainInput() {}

void refresh_term() {
	term_present();
}

void touchwin_term() {
	// 清除所有 cell，防止上一帧残留
	for (int i = 0; i < g_cols * g_rows; i++) {
		g_cells[i].ch = L' ';
		g_cells[i].fg = 4;
		g_cells[i].bg = 0;
		g_cells[i].dirty = true;
	}
}

void erase_term() {
	for (int i = 0; i < g_cols * g_rows; i++) {
		g_cells[i].ch = L' ';
		g_cells[i].fg = 4;
		g_cells[i].bg = 0;
		g_cells[i].dirty = true;
	}
	g_cursor_x = 0;
	g_cursor_y = 0;
	g_cur_fg = 4;
	g_cur_bg = 0;
}

// ============================================================
// 键盘输入
// ============================================================
int getch_term() {
	for (;;) {
		SDL_Event ev;
		if (!SDL_WaitEvent(&ev)) continue;

		switch (ev.type) {
		case SDL_EVENT_QUIT:
			return 'q';

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

		default:
			break;
		}
	}
}

// ============================================================
// printw 实现
// ============================================================
int term_printw(const char* fmt, ...) {
	char buf[128];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	// GBK 转换
	int len = (int)strlen(buf);
	if (len == 0) return 0;
	int wlen = MultiByteToWideChar(936, 0, buf, len, NULL, 0);
	if (wlen <= 0) return 0;
	wchar_t* wbuf = (wchar_t*)alloca((wlen + 1) * sizeof(wchar_t));
	MultiByteToWideChar(936, 0, buf, len, wbuf, wlen);
	wbuf[wlen] = 0;
	put_str(g_cursor_x, g_cursor_y, wbuf, g_cur_fg, g_cur_bg);
	g_cursor_x += len;
	return 0;
}
