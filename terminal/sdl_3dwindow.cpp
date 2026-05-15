// 伪 3D 第一人称窗口 (Wolfenstein 3D 风格 raycasting)
#include "sdl_3dwindow.h"
#include "game/player.h"
#include "game/map.h"
#include "game/monster.h"
#include "game/tile_data.h"
#include <SDL3/SDL.h>
#include <cmath>
#include <cstring>
#include <cstdio>

// ============================================================
// 常量
// ============================================================
#define SCR_W   640
#define SCR_H   480
#define TEX_W   320  // 内部分辨率（射线数）
#define TEX_H   240  // 内部高度
#define FOV     1.047197551  // 60° = π/3
#define MAX_DIST 16.0

// ============================================================
// 全局
// ============================================================
static SDL_Window*   g_win3d = NULL;
static SDL_Renderer* g_ren3d = NULL;
static SDL_Texture*  g_tex3d = NULL;
static uint32_t      g_pixels[TEX_W * TEX_H];

// 玩家朝向（弧度，0=北/上，π/2=东/右）
static double g_dir = 0.0;
static bool   g_running = false;
static double g_z_buf[TEX_W];  // 深度缓冲（用于精灵遮挡）

// 精灵结构
struct Sprite {
	double x, y;
	int    tile_id;
};

// ============================================================
// 从玩家位置收集可见精灵
// ============================================================
static int collect_sprites(const Player& ply, Sprite* out, int max_sprites) {
	int count = 0;
	int fx = ply.floor;
	// 收集周围 8x8 范围内的 sprite（怪物、道具、NPC）
	for (int dy = -4; dy <= 4 && count < max_sprites; dy++) {
		for (int dx = -4; dx <= 4 && count < max_sprites; dx++) {
			int mx = (int)ply.x + dx;
			int my = (int)ply.y + dy;
			if (mx < 0 || mx >= 13 || my < 0 || my >= 13) continue;
			uint8_t t = map_get(fx, mx, my);
			if ((t >= 51 && t <= 70) || (t >= 101 && t <= 150) || (t >= 151 && t <= 155)) {
				out[count].x = mx + 0.5;
				out[count].y = my + 0.5;
				out[count].tile_id = t;
				count++;
			}
		}
	}
	return count;
}

// ============================================================
// DDA 射线投射
// ============================================================
static void cast_ray(uint8_t floor, double px, double py, double angle,
                     int& hit_tile, int& side, double& perp_dist) {
	// angle: 0=北(y-), π/2=东(x+)
	double dx = sin(angle);
	double dy = -cos(angle);

	int map_x = (int)px, map_y = (int)py;
	double delta_x = (dx == 0) ? 1e30 : fabs(1.0 / dx);
	double delta_y = (dy == 0) ? 1e30 : fabs(1.0 / dy);

	int step_x, step_y;
	double side_dist_x, side_dist_y;

	if (dx < 0) { step_x = -1; side_dist_x = (px - map_x) * delta_x; }
	else        { step_x =  1; side_dist_x = (map_x + 1.0 - px) * delta_x; }

	if (dy < 0) { step_y = -1; side_dist_y = (py - map_y) * delta_y; }
	else        { step_y =  1; side_dist_y = (map_y + 1.0 - py) * delta_y; }

	// DDA
	hit_tile = 0;
	int s = 0;
	for (int i = 0; i < 64; i++) {
		if (side_dist_x < side_dist_y) {
			side_dist_x += delta_x;
			map_x += step_x;
			s = 0;
		} else {
			side_dist_y += delta_y;
			map_y += step_y;
			s = 1;
		}
		if (map_x < 0 || map_x >= 13 || map_y < 0 || map_y >= 13) break;
		uint8_t t = map_get(floor, map_x, map_y);
		if (t >= 1 && t <= 11) {  // 阻挡视线的 tile
			hit_tile = t;
			side = s;
			if (s == 0) perp_dist = side_dist_x - delta_x;
			else        perp_dist = side_dist_y - delta_y;
			return;
		}
	}
	hit_tile = 0;
	perp_dist = MAX_DIST;
}

// ============================================================
// 绘制垂直条带（墙壁）
// ============================================================
static uint32_t wall_color(int tile, int side) {
	double shade = side ? 0.7 : 1.0;
	switch (tile) {
		case 2:  return ((uint32_t)(200*shade)<<16) | ((uint32_t)(200*shade)<<8) | (uint32_t)(200*shade);  // 墙：灰白
		case 3:  return ((uint32_t)(200*shade)<<16) | ((uint32_t)(180*shade)<<8) | 0;  // 黄门
		case 4:  return 0 | ((uint32_t)(100*shade)<<8) | ((uint32_t)(220*shade)<<16);  // 蓝门
		case 5:  return ((uint32_t)(220*shade)<<16) | 0 | 0;  // 红门
		case 8:  return ((uint32_t)(180*shade)<<16) | ((uint32_t)(180*shade)<<8) | (uint32_t)(180*shade);  // 守卫门：灰
		case 6:  return ((uint32_t)(255*shade)<<16) | ((uint32_t)(60*shade)<<8) | 0;  // 岩浆：橙红
		case 9:  return 0 | ((uint32_t)(200*shade)<<16) | 0;  // 上行楼梯：绿
		case 10: return 0 | 0 | ((uint32_t)(200*shade)<<16);  // 下行楼梯：蓝
		default: return ((uint32_t)(150*shade)<<16) | ((uint32_t)(150*shade)<<8) | (uint32_t)(150*shade);
	}
}

static void draw_stripe(int tex_x, int h, int tile, int side) {
	int y0 = (TEX_H - h) / 2;
	if (y0 < 0) y0 = 0;
	int y1 = y0 + h;
	if (y1 > TEX_H) y1 = TEX_H;

	uint32_t c = wall_color(tile, side);

	for (int y = 0; y < y0; y++) {
		// 天花板：暗蓝灰
		int idx = y * TEX_W + tex_x;
		g_pixels[idx] = 0xFF303030;
	}
	for (int y = y0; y < y1; y++) {
		int idx = y * TEX_W + tex_x;
		g_pixels[idx] = c;
	}
	for (int y = y1; y < TEX_H; y++) {
		// 地板：暗灰
		int idx = y * TEX_W + tex_x;
		double floor_shade = 1.0 - (double)(y - TEX_H/2) / (TEX_H/2) * 0.6;
		uint32_t fc = (uint32_t)(80*floor_shade);
		g_pixels[idx] = (fc<<16) | (fc<<8) | fc;
	}
}

// ============================================================
// 用内建位图字体绘制文字（简单的单色精灵渲染）
// ============================================================
static void draw_symbol(int tex_x, int tex_y, int size, const char* gbk, uint32_t color) {
	// 简单实现：画一个彩色方块 + 不画文字（我们用点阵模拟）
	// 对于符号精灵，画一个带颜色的菱形或方块
	int half = size / 2;
	for (int dy = -half; dy < half; dy++) {
		for (int dx = -half; dx < half; dx++) {
			int sx = tex_x + dx;
			int sy = tex_y + dy;
			if (sx < 0 || sx >= TEX_W || sy < 0 || sy >= TEX_H) continue;
			// 菱形/圆形区域
			if (dx*dx + dy*dy < half*half) {
				g_pixels[sy * TEX_W + sx] = color;
			}
		}
	}
}

// ============================================================
// 完整帧渲染
// ============================================================
static void render_frame(const Player& ply) {
	double px = ply.x + 0.5;
	double py = ply.y + 0.5;
	uint8_t floor = ply.floor;

	// 清空
	memset(g_pixels, 0, sizeof(g_pixels));
	memset(g_z_buf, 0, sizeof(g_z_buf));

	// 投射射线
	for (int x = 0; x < TEX_W; x++) {
		double angle = g_dir - FOV/2.0 + FOV * (double)x / (TEX_W - 1);
		int hit_tile, side;
		double dist;
		cast_ray(floor, px, py, angle, hit_tile, side, dist);

		// 鱼眼矫正
		dist = dist * cos(angle - g_dir);
		if (dist < 0.01) dist = 0.01;

		int h = (int)((double)TEX_H / dist);
		if (h > TEX_H * 8) h = TEX_H * 8;

		draw_stripe(x, h, hit_tile, side);
		g_z_buf[x] = dist;
	}

	// 收集并绘制精灵
	Sprite sprites[256];
	int sprite_count = collect_sprites(ply, sprites, 256);

	// 计算精灵距离并排序（远→近）
	struct { Sprite* s; double dist; } sorted[256];
	for (int i = 0; i < sprite_count; i++) {
		double dx = sprites[i].x - px;
		double dy = sprites[i].y - py;
		sorted[i].s = &sprites[i];
		sorted[i].dist = dx*dx + dy*dy;
	}
	// 冒泡排序（简单场景足够）
	for (int i = 0; i < sprite_count - 1; i++)
		for (int j = 0; j < sprite_count - 1 - i; j++)
			if (sorted[j].dist < sorted[j+1].dist) {
				auto tmp = sorted[j]; sorted[j] = sorted[j+1]; sorted[j+1] = tmp;
			}

	// 绘制精灵（从远到近）
	for (int i = 0; i < sprite_count; i++) {
		Sprite* sp = sorted[i].s;
		double dx = sp->x - px;
		double dy = sp->y - py;

		// 变换到相机空间（与 cast_ray 一致的坐标系）
		// forward = (sin(dir), -cos(dir)), right = (cos(dir), sin(dir))
		double fwd_x = sin(g_dir);
		double fwd_y = -cos(g_dir);
		double rgt_x = cos(g_dir);
		double rgt_y = sin(g_dir);

		double cam_x = dx * rgt_x + dy * rgt_y;
		double cam_y = dx * fwd_x + dy * fwd_y;

		if (cam_y <= 0.1) continue;  // 在背后

		int screen_x = (int)((TEX_W / 2) * (1.0 + cam_x / cam_y / (FOV/2.0)));
		int sprite_h = (int)(TEX_H / cam_y);

		if (screen_x < -sprite_h || screen_x >= TEX_W + sprite_h) continue;

		// 深度测试
		int draw_x0 = screen_x - sprite_h / 2;
		int draw_x1 = screen_x + sprite_h / 2;
		bool visible = false;
		for (int sx = draw_x0; sx <= draw_x1; sx++) {
			if (sx >= 0 && sx < TEX_W && cam_y < g_z_buf[sx] + 1.0) {
				visible = true;
				break;
			}
		}
		if (!visible) continue;

		const TileDef& def = g_tile_defs[sp->tile_id];
		uint32_t color;
		switch (def.color) {
			case 0: color = 0xFFFF3030; break;  // RED
			case 1: color = 0xFFFFFF00; break;  // YELLOW
			case 2: color = 0xFF4040FF; break;  // BLUE
			case 3: color = 0xFFFFFFFF; break;  // WHITE
			case 4: color = 0xFF30FF30; break;  // GREEN
			case 5: color = 0xFFFF30FF; break;  // PURPLE
			case 6: color = 0xFFA0A0A0; break;  // GREY
			default: color = 0xFFFF3030;
		}
		draw_symbol(screen_x, TEX_H/2, sprite_h / 3, def.symbol, color);
	}
}

// ============================================================
// 创建并运行 3D 窗口
// ============================================================
static bool g_3d_ready = false;  // 窗口和 renderer 创建成功标志

bool run_3d_window(SDL_Window* main_win, Player& player) {
	// 如果没有 main window，在当前位置创建
	if (!main_win) return false;

	// 创建 3D 窗口（与主窗口并排）
	int mx, my, mw, mh;
	SDL_GetWindowPosition(main_win, &mx, &my);
	SDL_GetWindowSize(main_win, &mw, &mh);

	g_win3d = SDL_CreateWindow("魔塔 - 3D", SCR_W, SCR_H, SDL_WINDOW_HIDDEN);
	if (!g_win3d) { fprintf(stderr, "3D window: %s\n", SDL_GetError()); return false; }

	SDL_SetWindowPosition(g_win3d, mx + mw + 20, my);

	g_ren3d = SDL_CreateRenderer(g_win3d, NULL);
	if (!g_ren3d) { SDL_DestroyWindow(g_win3d); g_win3d = NULL; return false; }

	g_tex3d = SDL_CreateTexture(g_ren3d, SDL_PIXELFORMAT_XRGB8888,
		SDL_TEXTUREACCESS_STREAMING, TEX_W, TEX_H);
	if (!g_tex3d) { SDL_DestroyRenderer(g_ren3d); SDL_DestroyWindow(g_win3d);
		g_ren3d = NULL; g_win3d = NULL; return false; }

	SDL_ShowWindow(g_win3d);

	// 初始朝向
	g_dir = 0.0;
	g_running = true;
	g_3d_ready = true;

	// 单独的事件线程/循环集成在主循环中
	return true;
}

// ============================================================
// 由主循环调用：渲染 3D 帧
// ============================================================
void render_3d_frame(const Player& player) {
	if (!g_3d_ready) return;

	render_frame(player);

	SDL_UpdateTexture(g_tex3d, NULL, g_pixels, TEX_W * 4);
	SDL_RenderTexture(g_ren3d, g_tex3d, NULL, NULL);
	SDL_RenderPresent(g_ren3d);

	// 处理 3D 窗口事件
	SDL_Event ev;
	while (SDL_PollEvent(&ev)) {
		if (ev.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
			if (SDL_GetWindowFromID(ev.window.windowID) == g_win3d) {
				SDL_HideWindow(g_win3d);
			}
		}
	}
}

// ============================================================
// 通知移动方向
// ============================================================
void notify_3d_move(int dx, int dy) {
	if (dx == 0 && dy == 0) return;
	g_dir = atan2((double)dx, -(double)dy);  // 0=上, π/2=右
}

// ============================================================
// 关闭
// ============================================================
void shutdown_3d_window() {
	g_3d_ready = false;
	if (g_tex3d)  { SDL_DestroyTexture(g_tex3d); g_tex3d = NULL; }
	if (g_ren3d)  { SDL_DestroyRenderer(g_ren3d); g_ren3d = NULL; }
	if (g_win3d)  { SDL_DestroyWindow(g_win3d); g_win3d = NULL; }
}
