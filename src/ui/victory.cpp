#include "ui/victory.h"
#include "render/region_display.h"
#include "render/key.h"
#include "sdl_terminal.h"
#include <cstdio>

// ============================================================
// 通关画面渲染
// 说明：后续扩展点——可在此加入更多演出（动画/统计/结局文字等）。
// 渲染与输入分离，便于单独增强画面。
// ============================================================
static void draw_victory_screen(const Player& player)
{
	char buf[64];

	// 隐藏所有界面：清空整个画面
	for (int y = 0; y < 22; y++)
		for (int x = 0; x < 28; x++) {
			gotoxy(x, y);
			addch(' ');
		}

	gotoxy(11, 3);
	SetColor(YELLOW);
	addstr_gbk("胜 利");

	gotoxy(4, 5);
	SetColor(WHITE);
	addstr_gbk("你击败了魔王，拯救了魔塔！");

	// 玩家通关时的属性
	snprintf(buf, sizeof(buf), "攻击力 : %d", player.attack);
	gotoxy(4, 9);  SetColor(WHITE); addstr_gbk(buf);
	snprintf(buf, sizeof(buf), "防御力 : %d", player.defence);
	gotoxy(4, 10); SetColor(WHITE); addstr_gbk(buf);
	snprintf(buf, sizeof(buf), "生命力 : %d", player.health);
	gotoxy(4, 11); SetColor(WHITE); addstr_gbk(buf);

	gotoxy(4, 19);
	SetColor(GREY);
	addstr_gbk("按 Z / X 结束");
}

// 通关画面主流程：渲染 + 等待输入，结束后退出游戏
void showVictory(const Player& player)
{
	draw_victory_screen(player);
	term_present();

	while (1) {
		KEY k = getKey();
		if (k == KEY_Z || k == KEY_X) break;
		if (term_quit_requested()) break;
	}
	term_set_quit();
}
