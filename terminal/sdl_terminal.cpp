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
static SDL_Texture*  g_texture   = nullptr;

static int g_cols = 28, g_rows = 22;
static int g_cell_w = 22, g_cell_h = 22;
static int g_win_w = 616, g_win_h = 484;
static int g_row_bytes = 0;

// GDI 资源
static HDC     g_mem_dc   = nullptr;
static HBITMAP g_mem_bmp  = nullptr;
static HBITMAP g_old_bmp  = nullptr;
static HFONT   g_font     = nullptr;
static BYTE*   g_dib_bits = nullptr;

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

// 浅色模式：背景白、前景白字变黑、彩色不变
static bool g_light_mode = false;

// 底部消息
static char g_message[128] = "";

// Cell 缓冲
struct Cell {
	wchar_t ch;
	int     fg;
	int     bg;
};
static Cell* g_cells = nullptr;

// 状态栏数值直接绘制（绕过 cell，持久保留跨帧）
struct DeferredDraw {
	int  px, py, pw, ph;
	char text[64];
	int  fg, bg;
};
static DeferredDraw g_deferred[16];
static int          g_deferred_count = 0;

// ============================================================
// GBK → UTF-8 转换（用于窗口标题等 SDL API）
// ============================================================
static char* gbk_to_utf8(const char* gbk) {
	int wlen = MultiByteToWideChar(936, 0, gbk, -1, NULL, 0);
	if (wlen <= 0) return NULL;
	wchar_t* wbuf = new wchar_t[wlen];
	MultiByteToWideChar(936, 0, gbk, -1, wbuf, wlen);
	int ulen = WideCharToMultiByte(CP_UTF8, 0, wbuf, wlen, NULL, 0, NULL, NULL);
	char* utf8 = new char[ulen];
	WideCharToMultiByte(CP_UTF8, 0, wbuf, wlen, utf8, ulen, NULL, NULL);
	delete[] wbuf;
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
		case 7 * 16 | 7:  fg = 4; bg = 4; break;  // WALL
		case 64 | 7:      fg = 4; bg = 1; break;  // LAVA
		case 1 * 16 | 7:  fg = 4; bg = 3; break;  // STAR
		case 9 * 16 | 7:  fg = 4; bg = 3; break;  // EMPH
		default:          fg = 4; bg = 0; break;
	}
}

// ============================================================
// GDI 字体：高度 = cell 高度 - 2px 留白
// ============================================================
static void create_font() {
	char exe_path[MAX_PATH];
	GetModuleFileNameA(NULL, exe_path, MAX_PATH);
	char* last_slash = strrchr(exe_path, '\\');
	if (last_slash) {
		*last_slash = 0;
		char font_path[MAX_PATH];
		snprintf(font_path, sizeof(font_path), "%s\\simsun.ttc", exe_path);
		if (GetFileAttributesA(font_path) != INVALID_FILE_ATTRIBUTES) {
			WCHAR wpath[MAX_PATH];
			MultiByteToWideChar(CP_ACP, 0, font_path, -1, wpath, MAX_PATH);
			AddFontResourceExW(wpath, FR_PRIVATE, 0);
		}
	}

	int font_h = g_cell_h - 2;
	if (font_h < 10) font_h = 10;
	g_font = CreateFontW(
		font_h, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN,
		L"SimSun"
	);
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
		if (col >= g_cols) {
			row++;
			col = start_col;
		}
		if (row >= g_rows) break;
		put_cell(col, row, *s, fg, bg);
		col++;
		s++;
	}
}

// ============================================================
// 逐行渲染：每 cell 独立绘制，保证位置稳定
// ============================================================
static void render_row_gdi(int row) {
	for (int col = 0; col < g_cols; col++) {
		Cell* c = cell_at(col, row);
		if (!c) continue;

		int cx = col * g_cell_w;
		int cy = row * g_cell_h;
		RECT rect = { cx, cy, cx + g_cell_w, cy + g_cell_h };

		SDL_Color& bg_c = g_palette[c->bg];
		HBRUSH brush = CreateSolidBrush(RGB(bg_c.r, bg_c.g, bg_c.b));
		FillRect(g_mem_dc, &rect, brush);
		DeleteObject(brush);

		if (c->ch != L' ') {
			SDL_Color& fg_c = g_palette[c->fg];
			SetTextColor(g_mem_dc, RGB(fg_c.r, fg_c.g, fg_c.b));
			SetBkMode(g_mem_dc, TRANSPARENT);
			SelectObject(g_mem_dc, g_font);
			DrawTextW(g_mem_dc, &c->ch, 1, &rect,
				DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
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
	g_row_bytes = g_win_w * 4;

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

	char* title_utf8 = gbk_to_utf8(title);
	g_window = SDL_CreateWindow(title_utf8 ? title_utf8 : title, g_win_w, g_win_h, 0);
	delete[] title_utf8;
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
		SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING,
		g_win_w, g_win_h);
	if (!g_texture) {
		g_texture = SDL_CreateTexture(g_renderer,
			SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING,
			g_win_w, g_win_h);
	}
	if (!g_texture) {
		fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
		return false;
	}

	// GDI DIB section
	HDC screen_dc = GetDC(NULL);
	g_mem_dc = CreateCompatibleDC(screen_dc);

	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = g_win_w;
	bmi.bmiHeader.biHeight = -g_win_h;  // top-down
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	g_mem_bmp = CreateDIBSection(g_mem_dc, &bmi, DIB_RGB_COLORS,
		(void**)&g_dib_bits, NULL, 0);
	g_old_bmp = (HBITMAP)SelectObject(g_mem_dc, g_mem_bmp);
	ReleaseDC(NULL, screen_dc);

	create_font();

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
			bi.biWidth = w;
			bi.biHeight = -h;
			bi.biPlanes = 1;
			bi.biBitCount = 32;
			bi.biCompression = BI_RGB;

			int rowBytes = w * 4;
			BYTE* px = new BYTE[h * rowBytes];
			HDC hdc = GetDC(NULL);
			GetDIBits(hdc, hBmp, 0, h, px, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
			ReleaseDC(NULL, hdc);

			SDL_Surface* surf = SDL_CreateSurfaceFrom(w, h,
				SDL_PIXELFORMAT_BGRA8888, px, rowBytes);
			if (surf) {
				SDL_SetWindowIcon(g_window, surf);
				SDL_DestroySurface(surf);
			}
			delete[] px;
			DeleteObject(hBmp);
		}
	}

	return true;
}

void term_shutdown() {
	delete[] g_cells;
	g_cells = nullptr;

	if (g_font)    DeleteObject(g_font);
	if (g_old_bmp) SelectObject(g_mem_dc, g_old_bmp);
	if (g_mem_bmp) DeleteObject(g_mem_bmp);
	if (g_mem_dc)  DeleteDC(g_mem_dc);

	if (g_texture)   SDL_DestroyTexture(g_texture);
	if (g_renderer)  SDL_DestroyRenderer(g_renderer);
	if (g_window)    SDL_DestroyWindow(g_window);
	SDL_Quit();
}

// ============================================================
// 帧渲染
// ============================================================
void term_present() {
	int total_pixels = g_win_w * g_win_h;

	// 1. 清空 DIB
	memset(g_dib_bits, 0, total_pixels * 4);

	// 2. 逐 cell 渲染
	for (int row = 0; row < g_rows; row++)
		render_row_gdi(row);

	// 3. 执行延迟直接绘制（状态栏数值等）
	for (int i = 0; i < g_deferred_count; i++) {
		DeferredDraw& d = g_deferred[i];
		RECT rect = { d.px, d.py, d.px + d.pw, d.py + d.ph };

		SDL_Color& bg_c = g_palette[d.bg];
		HBRUSH brush = CreateSolidBrush(RGB(bg_c.r, bg_c.g, bg_c.b));
		FillRect(g_mem_dc, &rect, brush);
		DeleteObject(brush);

		int wlen = MultiByteToWideChar(936, 0, d.text, -1, NULL, 0);
		if (wlen > 0) {
			wchar_t* wbuf = new wchar_t[wlen];
			MultiByteToWideChar(936, 0, d.text, -1, wbuf, wlen);
			SDL_Color& fg_c = g_palette[d.fg];
			SetTextColor(g_mem_dc, RGB(fg_c.r, fg_c.g, fg_c.b));
			SetBkMode(g_mem_dc, TRANSPARENT);
			SelectObject(g_mem_dc, g_font);
			DrawTextW(g_mem_dc, wbuf, wlen - 1, &rect,
				DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			delete[] wbuf;
		}
	}
	// (不清除 g_deferred，使其在多次 term_present 间保持)

	// 4. 底部消息
	if (g_message[0]) {
		int msg_y = (g_rows - 1) * g_cell_h;
		RECT rect = { 0, msg_y, g_win_w, msg_y + g_cell_h };
		HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
		FillRect(g_mem_dc, &rect, brush);
		DeleteObject(brush);

		int wlen = MultiByteToWideChar(936, 0, g_message, -1, NULL, 0);
		if (wlen > 0) {
			wchar_t* wbuf = new wchar_t[wlen];
			MultiByteToWideChar(936, 0, g_message, -1, wbuf, wlen);
			SetTextColor(g_mem_dc, RGB(255, 255, 255));
			SetBkMode(g_mem_dc, TRANSPARENT);
			SelectObject(g_mem_dc, g_font);
			DrawTextW(g_mem_dc, wbuf, wlen - 1, &rect,
				DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			delete[] wbuf;
		}
	}

	// 5. 确保 GDI 写入完成
	GdiFlush();

	// 6. 修复 alpha 通道
	for (int i = 0; i < total_pixels; i++)
		g_dib_bits[i * 4 + 3] = 255;

	// 7. 上传并呈现
	SDL_UpdateTexture(g_texture, NULL, g_dib_bits, g_row_bytes);
	SDL_RenderTexture(g_renderer, g_texture, NULL, NULL);
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
	g_cursor_x = 0;
	g_cursor_y = 0;
	g_cur_fg = 4;
	g_cur_bg = 0;
}

void refresh_term() {
	term_present();
}

// ============================================================
// ncurses 兼容层
// ============================================================
void console_init() {
	term_init("魔塔 - SDL3", 28, 22, 22, 22);
}

void console_shutdown() {
	term_shutdown();
}

void gotoxy(int x, int y) {
	g_cursor_x = x;
	g_cursor_y = y;
}

void hideCursor() {}

void drainInput() {}

bool term_quit_requested() {
	return g_quit;
}

void term_set_light_mode(bool on) {
	if (g_light_mode == on) return;
	g_light_mode = on;
	SDL_Color tmp = g_palette[0];
	g_palette[0] = g_palette[4];
	g_palette[4] = tmp;
}

void term_set_message(const char* msg) {
	strncpy(g_message, msg, 127);
	g_message[127] = 0;
}

void term_clear_message() {
	g_message[0] = 0;
}

void SetConsoleColor(int attr) {
	attr_to_colors(attr, g_cur_fg, g_cur_bg);
}

void SetColor(COLOR a) {
	g_cur_fg = color_to_idx(a);
	g_cur_bg = 0;
}

void colorPrint(COLOR c, char* s) {
	int prev_fg = g_cur_fg;
	g_cur_fg = color_to_idx(c);
	addstr_gbk(s);
	g_cur_fg = prev_fg;
}

void addstr_gbk(const char* s) {
	int len = (int)strlen(s);
	if (len == 0) return;

	int wlen = MultiByteToWideChar(936, 0, s, len, NULL, 0);
	if (wlen <= 0) return;

	wchar_t* buf = new wchar_t[wlen + 1];
	MultiByteToWideChar(936, 0, s, len, buf, wlen);
	buf[wlen] = 0;

	put_str(g_cursor_x, g_cursor_y, buf, g_cur_fg, g_cur_bg);
	g_cursor_x += wlen;

	delete[] buf;
}

void addch(char ch) {
	if (g_cursor_x >= g_cols) {
		g_cursor_x = 0;
		g_cursor_y++;
	}
	put_cell(g_cursor_x, g_cursor_y, (wchar_t)ch, g_cur_fg, g_cur_bg);
	g_cursor_x++;
}

// ============================================================
// 键盘输入
// ============================================================
int getch_term() {
	for (;;) {
		SDL_Event ev;
		if (!SDL_WaitEventTimeout(&ev, 100)) { return -1; }

		switch (ev.type) {
		case SDL_EVENT_QUIT:
			g_quit = true;
			return 0;

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
// printf 风格输出
// ============================================================
int term_printw(const char* fmt, ...) {
	char buf[256];
	va_list args;
	va_start(args, fmt);
	int len = vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	if (len <= 0) return 0;

	int wlen = MultiByteToWideChar(936, 0, buf, len, NULL, 0);
	if (wlen <= 0) return 0;

	wchar_t* wbuf = new wchar_t[wlen + 1];
	MultiByteToWideChar(936, 0, buf, len, wbuf, wlen);
	wbuf[wlen] = 0;

	put_str(g_cursor_x, g_cursor_y, wbuf, g_cur_fg, g_cur_bg);
	g_cursor_x += wlen;

	delete[] wbuf;
	return 0;
}

// ============================================================
// 直接 GDI 文本（绕过 cell 系统，数值无空隙）
// ============================================================
void term_draw_text(int px, int py, int pw, int ph,
                    const char* text, int fg_color, int bg_color) {
	if (g_deferred_count >= 16) return;
	DeferredDraw& d = g_deferred[g_deferred_count++];
	d.px = px; d.py = py; d.pw = pw; d.ph = ph;
	strncpy(d.text, text, 63);
	d.text[63] = 0;
	d.fg = fg_color;
	d.bg = bg_color;
}

void term_clear_draws() {
	g_deferred_count = 0;
}
