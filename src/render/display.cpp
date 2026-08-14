#include "render/display.h"
#include "render/frame_print.h"
#include <render/cursor.h>
#include "game/map.h"
#include "game/monster.h"
#include "game/tile_data.h"

void Display::init(const Player&) {}

void Display::generateFrame(const Player& ply)
{
	for (uint8_t i = 0; i < 13; i++)
	{
		for (uint8_t j = 0; j < 13; j++)
		{
			gotoxy(j, i);
			framePrintChar(map_get(ply.floor, j, i));
		}
	}

	gotoxy(ply.x, ply.y);
	framePrintChar(255);
}

// ============================================================
// 怪物威胁指示灯：绿=无伤击败，黄=受伤可击败，红=无法击败
// 仅持有怪物手册时绘制；黑屏演出（darken_map）时不显示，避免剧透
// 多格 Boss（怪物身躯）整块只画一个灯，位于块的右下角
// ============================================================
void drawMonsterLights(const Player& ply)
{
	if (!ply.hasMonsterBook) return;
	if (term_is_darkened()) return;

	const int cell = 22;  // 与 term_init 的 cell 尺寸一致
	const int dot  = 5;   // 指示灯边长

	auto drawLight = [&](int x, int y, int32_t dmg) {
		int color;
		if (dmg < 0 || (uint32_t)dmg >= ply.health) color = 1;   // 无法击败 → RED
		else if (dmg == 0)                            color = 5;   // 无伤 → GREEN
		else                                          color = 2;   // YELLOW
		int px = x * cell + cell - dot - 1;
		int py = y * cell + cell - dot - 1;
		term_draw_text(px, py, dot, dot, " ", color, color);
	};

	for (uint8_t y = 0; y < 13; y++)
		for (uint8_t x = 0; x < 13; x++)
		{
			uint8_t t = map_get(ply.floor, x, y);
			if (tile_category(t) != TILE_MONSTER) continue;
			drawLight(x, y, SimulateCombat(ply, t));
		}

	// 多格 Boss 身躯：整块只画一个灯（在块右下角）
	for (uint8_t y = 0; y < 13; y++)
		for (uint8_t x = 0; x < 13; x++)
		{
			if (map_get(ply.floor, x, y) != TILE_MONSTER_BODY) continue;
			bool hasBelow = (y + 1 < 13) && map_get(ply.floor, x, y + 1) == TILE_MONSTER_BODY;
			bool hasRight = (x + 1 < 13) && map_get(ply.floor, x + 1, y) == TILE_MONSTER_BODY;
			if (hasBelow || hasRight) continue;   // 不是块的右下角格
			uint8_t boss = bodyBossForFloor(ply.floor);
			if (boss == 0) continue;
			drawLight(x, y, SimulateCombat(ply, boss));
		}
}

// ============================================================
// 巨型 Boss 渲染：检测怪物身躯块，叠加一个覆盖整块的大号首领字符
// ============================================================
void drawBossGlyphs(const Player& ply)
{
	if (term_is_darkened()) return;

	const int cell = 22;
	for (uint8_t y = 0; y < 13; y++)
		for (uint8_t x = 0; x < 13; x++)
		{
			if (map_get(ply.floor, x, y) != TILE_MONSTER_BODY) continue;
			// 找块的左上角（上方/左方都不是身躯）
			if (x > 0 && map_get(ply.floor, x - 1, y) == TILE_MONSTER_BODY) continue;
			if (y > 0 && map_get(ply.floor, x, y - 1) == TILE_MONSTER_BODY) continue;

			int w = 0, h = 0;
			while (x + w < 13 && map_get(ply.floor, x + w, y) == TILE_MONSTER_BODY) w++;
			while (y + h < 13 && map_get(ply.floor, x, y + h) == TILE_MONSTER_BODY) h++;

			uint8_t boss = bodyBossForFloor(ply.floor);
			if (boss == 0) continue;
			const char* sym = g_tile_defs[boss].symbol;
			if (!sym[0]) continue;

			// 首领字符颜色 → 调色板索引（当前两个首领都是红色 = 1）
			int color = 1;
			term_draw_big_text(x * cell, y * cell, w * cell, h * cell, sym, color, 0);

			x += (uint8_t)(w - 1);
		}
}
