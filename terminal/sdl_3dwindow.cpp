// 伪 3D 第一人称窗口 (Wolfenstein 3D 风格 raycasting)
#include "sdl_3dwindow.h"
#include "sdl_terminal.h"
#include "game/player.h"
#include "game/map.h"
#include "game/tile_data.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <windows.h>
#include <cmath>
#include <cstring>

#define SCR_W   640
#define SCR_H   480
#define TEX_W   320
#define TEX_H   240
#define FOV     1.047197551
#define MAX_DIST 16.0

static SDL_Window*   g_win3d = NULL;
static SDL_Renderer* g_ren3d = NULL;
static SDL_Texture*  g_tex3d = NULL;
static uint32_t      g_pixels[TEX_W * TEX_H];
static double g_dir = 0.0;
static double g_z_buf[TEX_W];
static bool   g_3d_ready = false;

// 阻挡视线的 tile（不包括岩浆 6，使其渲染为地板）
static bool is_wall(uint8_t t) {
	return t == 2 || t == 3 || t == 4 || t == 5 || t == 7 || t == 8 || t == 11;
}

// ============================================================
// Sprite
// ============================================================
struct Sprite { double x, y; int tile_id; };

static int collect_sprites(const Player& ply, Sprite* out, int max) {
	int n = 0;
	for (int dy = -5; dy <= 5 && n < max; dy++)
		for (int dx = -5; dx <= 5 && n < max; dx++) {
			int mx = (int)ply.x + dx, my = (int)ply.y + dy;
			if (mx < 0 || mx >= 13 || my < 0 || my >= 13) continue;
			uint8_t t = map_get(ply.floor, mx, my);
			if (t == 0 || t == 1 || is_wall(t)) continue;
			out[n].x = mx + 0.5; out[n].y = my + 0.5; out[n].tile_id = t;
			n++;
		}
	return n;
}

// ============================================================
// DDA 射线
// ============================================================
static void cast_ray(uint8_t fl, double px, double py, double a,
                     int& tile, int& side, double& dist) {
	double dx = sin(a), dy = -cos(a);
	int mx = (int)px, my = (int)py;
	double ddx = (dx == 0) ? 1e30 : fabs(1.0 / dx);
	double ddy = (dy == 0) ? 1e30 : fabs(1.0 / dy);
	int sx, sy; double sdx, sdy;
	if (dx < 0) { sx = -1; sdx = (px - mx) * ddx; }
	else        { sx =  1; sdx = (mx + 1.0 - px) * ddx; }
	if (dy < 0) { sy = -1; sdy = (py - my) * ddy; }
	else        { sy =  1; sdy = (my + 1.0 - py) * ddy; }
	for (int i = 0; i < 64; i++) {
		if (sdx < sdy) { sdx += ddx; mx += sx; side = 0; }
		else           { sdy += ddy; my += sy; side = 1; }
		if (mx < 0 || mx >= 13 || my < 0 || my >= 13) break;
		uint8_t t = map_get(fl, mx, my);
		if (is_wall(t)) { tile = t; dist = (side == 0) ? sdx - ddx : sdy - ddy; return; }
	}
	tile = 0; dist = MAX_DIST;
}

// ============================================================
// 墙壁颜色
// ============================================================
static uint32_t wall_col(int t, int side) {
	double s = side ? 0.7 : 1.0;
	switch (t) {
		case 2: return ((uint32_t)(200*s)<<16)|((uint32_t)(200*s)<<8)|(uint32_t)(200*s);
		case 3: return ((uint32_t)(200*s)<<16)|((uint32_t)(180*s)<<8)|0;
		case 4: return 0|((uint32_t)(100*s)<<8)|((uint32_t)(220*s)<<16);
		case 5: return ((uint32_t)(220*s)<<16)|0|0;
		case 7: return 0|((uint32_t)(100*s)<<16)|((uint32_t)(200*s)<<8);
		case 8: case 11: return ((uint32_t)(180*s)<<16)|((uint32_t)(180*s)<<8)|(uint32_t)(180*s);
		default: return ((uint32_t)(120*s)<<16)|((uint32_t)(120*s)<<8)|(uint32_t)(120*s);
	}
}

static uint32_t floor_col(uint8_t t, double shade) {
	if (t == 6) { int r=(int)(255*shade), g=(int)(50*shade); return (r<<16)|(g<<8)|0; }
	if (t == 7) { int c=(int)(100*shade); return (c<<16)|((int)(200*shade)<<8)|c; }
	int c=(int)(70*shade); return (c<<16)|(c<<8)|c;
}

// ============================================================
// 绘制一列（墙壁+天花板+地板）
// ============================================================
static void draw_column(int x, int wh, int tile, int side, uint8_t fl,
                        double px, double py, double a) {
	int y0 = (TEX_H - wh) / 2; if (y0 < 0) y0 = 0;
	int y1 = y0 + wh; if (y1 > TEX_H) y1 = TEX_H;
	uint32_t wc = wall_col(tile, side);
	for (int y = 0; y < y0; y++) g_pixels[y * TEX_W + x] = 0xFF303030;
	for (int y = y0; y < y1; y++) g_pixels[y * TEX_W + x] = wc;

	double rdx = sin(a), rdy = -cos(a);
	for (int y = y1; y < TEX_H; y++) {
		double rd = 0.5 * TEX_H / (y - TEX_H/2 + 0.001);
		double fx = px + rd * rdx, fy = py + rd * rdy;
		int fmx = (int)fx, fmy = (int)fy;
		double shade = 1.0 - (double)(y - TEX_H/2) / (TEX_H/2) * 0.5;
		uint32_t fc = (fmx>=0&&fmx<13&&fmy>=0&&fmy<13) ?
			floor_col(map_get(fl, fmx, fmy), shade) : floor_col(0, shade);
		g_pixels[y * TEX_W + x] = fc;
	}
}

// ============================================================
// GBK → UTF-8 用于 SDL_ttf（与 addstr_gbk 完全一致的转换逻辑）
// ============================================================
static char* gbk_utf8(const char* gbk) {
	int len = (int)strlen(gbk);
	if (len == 0) return NULL;
	int wlen = MultiByteToWideChar(936, 0, gbk, len, NULL, 0);
	if (wlen <= 0) return NULL;
	wchar_t* wb = new wchar_t[wlen + 1];
	MultiByteToWideChar(936, 0, gbk, len, wb, wlen);
	wb[wlen] = 0;
	int ulen = WideCharToMultiByte(CP_UTF8, 0, wb, wlen, NULL, 0, NULL, NULL);
	if (ulen <= 0) { delete[] wb; return NULL; }
	char* u8 = new char[ulen + 1];
	WideCharToMultiByte(CP_UTF8, 0, wb, wlen, u8, ulen, NULL, NULL);
	u8[ulen] = 0;
	delete[] wb;
	return u8;
}

// ============================================================
// 将文字渲染到像素缓冲（近大远小缩放）
// ============================================================
static void draw_text(int cx, int cy, int sprite_h, const char* gbk, uint32_t color) {
	TTF_Font* font = term_get_ttf_font();
	if (!font || !gbk || !gbk[0]) return;
	if (sprite_h < 6) sprite_h = 6;

	char* u8 = gbk_utf8(gbk);
	if (!u8) return;

	SDL_Color fg = { (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color, 255 };
	SDL_Surface* src = TTF_RenderText_Blended(font, u8, 0, fg);
	delete[] u8;
	if (!src) return;

	// 缩放文字以匹配 sprite 大小（近大远小）
	int tw = src->w, th = src->h;
	int target_h = sprite_h * 3 / 4;
	if (target_h < 8) target_h = 8;
	tw = tw * target_h / th;
	th = target_h;
	if (tw < 4) tw = 4;

	// SDL_BlitSurfaceScaled 缩放，保证质量
	SDL_Surface* dst = SDL_CreateSurface(tw, th, SDL_PIXELFORMAT_ARGB8888);
	if (!dst) { SDL_DestroySurface(src); return; }
	SDL_SetSurfaceBlendMode(src, SDL_BLENDMODE_NONE);
	SDL_BlitSurfaceScaled(src, NULL, dst, NULL, SDL_SCALEMODE_LINEAR);
	SDL_DestroySurface(src);

	int x0 = cx - tw / 2, y0 = cy - th / 2;
	SDL_LockSurface(dst);
	uint8_t* sp = (uint8_t*)dst->pixels;
	int pitch = dst->pitch;

	for (int y = 0; y < th; y++) {
		for (int x = 0; x < tw; x++) {
			int px = x0 + x, py = y0 + y;
			if (px < 0 || px >= TEX_W || py < 0 || py >= TEX_H) continue;
			uint8_t* p = sp + y * pitch + x * 4;
			if (p[3] > 0)
				g_pixels[py * TEX_W + px] = (0xFFu<<24) | (p[2]<<16) | (p[1]<<8) | p[0];
		}
	}
	SDL_UnlockSurface(dst);
	SDL_DestroySurface(dst);
}

// ============================================================
// 完整帧
// ============================================================
static void render_frame(const Player& ply) {
	double px = ply.x + 0.5, py = ply.y + 0.5;
	uint8_t fl = ply.floor;
	memset(g_pixels, 0, sizeof(g_pixels));
	memset(g_z_buf, 0, sizeof(g_z_buf));

	for (int x = 0; x < TEX_W; x++) {
		double a = g_dir - FOV/2.0 + FOV * x / (TEX_W - 1);
		int tile, side; double dist;
		cast_ray(fl, px, py, a, tile, side, dist);
		dist = dist * cos(a - g_dir);
		if (dist < 0.01) dist = 0.01;
		int h = (int)(TEX_H / dist);
		if (h > TEX_H * 8) h = TEX_H * 8;
		draw_column(x, h, tile, side, fl, px, py, a);
		g_z_buf[x] = dist;
	}

	Sprite sprites[300];
	int n = collect_sprites(ply, sprites, 300);
	struct { Sprite* s; double d; } sd[300];
	for (int i = 0; i < n; i++) {
		double dx = sprites[i].x - px, dy = sprites[i].y - py;
		sd[i].s = &sprites[i]; sd[i].d = dx*dx + dy*dy;
	}
	for (int i = 0; i < n-1; i++)
		for (int j = 0; j < n-1-i; j++)
			if (sd[j].d < sd[j+1].d) { auto t = sd[j]; sd[j] = sd[j+1]; sd[j+1] = t; }

	double fwx = sin(g_dir), fwy = -cos(g_dir);
	double rwx = cos(g_dir), rwy = sin(g_dir);

	for (int i = 0; i < n; i++) {
		Sprite* sp = sd[i].s;
		double dx = sp->x - px, dy = sp->y - py;
		double cx = dx * rwx + dy * rwy;
		double cy = dx * fwx + dy * fwy;
		if (cy <= 0.1) continue;
		int sx = (int)((TEX_W/2) * (1.0 + cx / cy / (FOV/2.0)));
		int sh = (int)(TEX_H / cy);
		if (sx < -sh || sx >= TEX_W + sh) continue;
		bool vis = false;
		for (int tx = sx - sh/2; tx <= sx + sh/2; tx++)
			if (tx >= 0 && tx < TEX_W && cy < g_z_buf[tx] + 0.5) { vis = true; break; }
		if (!vis) continue;

		const char* sym = g_tile_defs[sp->tile_id].symbol;
		if (!sym || !sym[0]) continue;
		uint32_t col;
		uint8_t c = g_tile_defs[sp->tile_id].color;
		if      (c == 0) col = 0xFFFF3030;
		else if (c == 1) col = 0xFFFFFF00;
		else if (c == 2) col = 0xFF4040FF;
		else if (c == 3) col = 0xFFFFFFFF;
		else if (c == 4) col = 0xFF30FF30;
		else if (c == 5) col = 0xFFFF30FF;
		else if (c == 6) col = 0xFFA0A0A0;
		else if (c == 7) col = 0xFF30FFFF;
		else             col = 0xFFFF3030;
		draw_text(sx, TEX_H/2, sh, sym, col);
	}
}

// ============================================================
// 窗口
// ============================================================
bool run_3d_window(SDL_Window* main_win, Player& player) {
	(void)player;
	if (!main_win) return false;
	int mx, my, mw, mh;
	SDL_GetWindowPosition(main_win, &mx, &my);
	SDL_GetWindowSize(main_win, &mw, &mh);
	g_win3d = SDL_CreateWindow("魔塔 - 3D", SCR_W, SCR_H, SDL_WINDOW_HIDDEN);
	if (!g_win3d) return false;
	SDL_SetWindowPosition(g_win3d, mx + mw + 20, my);
	g_ren3d = SDL_CreateRenderer(g_win3d, NULL);
	if (!g_ren3d) { SDL_DestroyWindow(g_win3d); g_win3d = NULL; return false; }
	g_tex3d = SDL_CreateTexture(g_ren3d, SDL_PIXELFORMAT_XRGB8888,
		SDL_TEXTUREACCESS_STREAMING, TEX_W, TEX_H);
	SDL_ShowWindow(g_win3d);
	g_dir = 0.0; g_3d_ready = true;
	return true;
}

void render_3d_frame(const Player& player) {
	if (!g_3d_ready) return;
	render_frame(player);
	SDL_UpdateTexture(g_tex3d, NULL, g_pixels, TEX_W * 4);
	SDL_RenderTexture(g_ren3d, g_tex3d, NULL, NULL);
	SDL_RenderPresent(g_ren3d);
	SDL_Event ev;
	while (SDL_PollEvent(&ev))
		if (ev.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
			SDL_GetWindowFromID(ev.window.windowID) == g_win3d)
			SDL_HideWindow(g_win3d);
}

void notify_3d_move(int dx, int dy) {
	if (dx || dy) g_dir = atan2((double)dx, -(double)dy);
}

void shutdown_3d_window() {
	g_3d_ready = false;
	if (g_tex3d) { SDL_DestroyTexture(g_tex3d); g_tex3d = NULL; }
	if (g_ren3d) { SDL_DestroyRenderer(g_ren3d); g_ren3d = NULL; }
	if (g_win3d) { SDL_DestroyWindow(g_win3d); g_win3d = NULL; }
}
