#include "ui/monster_book.h"
#include "game/monster.h"
#include "game/map.h"
#include "game/tile_data.h"
#include "render/region_display.h"
#include "render/display.h"
#include "render/key.h"
#include "sdl_terminal.h"
#include <cstdio>

void showMonsterBook(Player& player)
{
	// 收集当前楼层存在的怪物类型（按首次出现顺序，去重）
	uint8_t ids[50];
	int count = 0;
	for (uint8_t y = 0; y < 13; y++)
		for (uint8_t x = 0; x < 13; x++)
		{
			uint8_t t = map_get(player.floor, x, y);
			uint8_t add = 0;
			if (tile_category(t) == TILE_MONSTER) add = t;
			else if (t == TILE_MONSTER_BODY) add = bodyBossForFloor(player.floor);  // 多格 Boss 身躯
			if (add == 0) continue;
			bool dup = false;
			for (int i = 0; i < count; i++)
				if (ids[i] == add) { dup = true; break; }
			if (!dup) ids[count++] = add;
		}

	if (count == 0)
	{
		saySomething((char*)"本层没有怪物。");
		return;
	}

	// 隐藏右侧状态栏等所有 UI，避免与手册详情冲突
	term_clear_draws();
	term_clear_message();
	for (int r = 0; r < 22; r++) regionErase(15, r, 13);

	// 保持多格 Boss 与威胁指示灯可见（手册打开期间）
	drawBossGlyphs(player);
	drawMonsterLights(player);

	const int perPage = 8;
	int pageStart = 0;
	int selected = 0;
	char buf[64];

	// 标题
	snprintf(buf, sizeof(buf), "怪物手册  第%d层", player.floor);
	regionPrint(2, 13, buf);

	while (1)
	{
		// 修正页码与选中位置
		if (selected < pageStart) pageStart = selected;
		if (selected >= pageStart + perPage) pageStart = selected - perPage + 1;
		int rows = count - pageStart;
		if (rows > perPage) rows = perPage;

		// 绘制列表（行 14-21）：怪物图标 + 名称，蓝框只框名称
		for (int r = 14; r <= 21; r++) regionErase(2, r, 12);
		for (int i = 0; i < rows; i++)
		{
			int idx = pageStart + i;
			uint8_t id = ids[idx];
			const char* name = getMonsterName(id);
			int row = 14 + i;

			// 怪物图标（正常色，不受选中蓝框影响）
			gotoxy(2, row);
			colorPrint((COLOR)g_tile_defs[id].color, (char*)g_tile_defs[id].symbol);

			if (idx == selected)
				regionEmphasize(3, row, (char*)name);
			else
				regionPrint(3, row, (char*)name);
		}

		// 右侧详情面板（cols 15-27, rows 0-6）
		for (int r = 0; r <= 6; r++) regionErase(15, r, 13);
		{
			Monster* m = getMonsterType(ids[selected]);
			int32_t dmg = SimulateCombat(player, ids[selected]);
			const char* name = getMonsterName(ids[selected]);

			// 名称颜色沿用指示灯语义：绿=无伤，黄=受伤可击败，红=无法击败
			COLOR ncolor;
			if (dmg < 0 || (uint32_t)dmg >= player.health) ncolor = RED;
			else if (dmg == 0) ncolor = GREEN;
			else               ncolor = YELLOW;

			gotoxy(15, 0);
			colorPrint(ncolor, (char*)name);

			snprintf(buf, sizeof(buf), "生命 %d", m->health);  regionPrint(15, 1, buf);
			snprintf(buf, sizeof(buf), "攻击 %d", m->attack);  regionPrint(15, 2, buf);
			snprintf(buf, sizeof(buf), "防御 %d", m->defence); regionPrint(15, 3, buf);
			{
				int32_t gold = player.hasLuckyCoin ? m->money * 2 : m->money;
				snprintf(buf, sizeof(buf), "金币 %d", gold);   regionPrint(15, 4, buf);
			}

			// 承受伤害：可击败→黄色耗血；血量不足→红色耗血；无法攻击→红色"无法攻击"
			COLOR dcolor;
			if (dmg < 0) {
				snprintf(buf, sizeof(buf), "无法攻击");
				dcolor = RED;
			} else if ((uint32_t)dmg >= player.health) {
				snprintf(buf, sizeof(buf), "耗血 %d", dmg);
				dcolor = RED;
			} else {
				snprintf(buf, sizeof(buf), "耗血 %d", dmg);
				dcolor = YELLOW;
			}
			gotoxy(15, 5);
			colorPrint(dcolor, buf);

			snprintf(buf, sizeof(buf), "%d/%d", selected + 1, count);
			regionPrint(15, 6, buf);
		}

		term_present();

		KEY key = getKey();
		switch (key)
		{
		case UP:
			if (selected > 0) selected--;
			break;
		case DOWN:
			if (selected < count - 1) selected++;
			break;
		case LEFT:
			if (pageStart > 0) { pageStart = (pageStart >= perPage) ? pageStart - perPage : 0; selected = pageStart; }
			break;
		case RIGHT:
			if (pageStart + perPage < count) { pageStart += perPage; selected = pageStart; }
			break;
		case KEY_X:
			for (int r = 13; r <= 21; r++) regionErase(2, r, 26);
			for (int r = 0; r < 22; r++) regionErase(15, r, 13);
			term_present();
			return;
		default:
			break;
		}
	}
}
