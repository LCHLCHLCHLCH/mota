// SDL3 版本的 StatusBar（替代 src/render/status_bar.cpp）
#include <cstdio>
#include "render/status_bar.h"
#include "sdl_terminal.h"

void StatusBar::draw(const Player& ply) {
	char buf[16];

	gotoxy(18, 0);
	term_printw("%d", ply.floor);

	gotoxy(15, 2);  colorPrint(RED, (char*)"血");
	gotoxy(15, 4);  colorPrint(YELLOW, (char*)"钥");
	gotoxy(15, 5);  colorPrint(BLUE, (char*)"钥");
	gotoxy(15, 6);  colorPrint(RED, (char*)"钥");
	gotoxy(15, 8);  colorPrint(RED, (char*)"攻");
	gotoxy(15, 9);  colorPrint(BLUE, (char*)"防");
	gotoxy(15, 11); colorPrint(YELLOW, (char*)"金");

	// 数值用 term_draw_text 直接 GDI 绘制，紧凑无空隙
	snprintf(buf, sizeof(buf), "%d", ply.health);
	term_draw_text(17 * 22, 2 * 22, 100, 22, buf, COLOR_WHITE, 0);

	snprintf(buf, sizeof(buf), "%d", ply.yellowKey);
	term_draw_text(17 * 22, 4 * 22, 100, 22, buf, COLOR_WHITE, 0);

	snprintf(buf, sizeof(buf), "%d", ply.blueKey);
	term_draw_text(17 * 22, 5 * 22, 100, 22, buf, COLOR_WHITE, 0);

	snprintf(buf, sizeof(buf), "%d", ply.redKey);
	term_draw_text(17 * 22, 6 * 22, 100, 22, buf, COLOR_WHITE, 0);

	snprintf(buf, sizeof(buf), "%d", ply.attack);
	term_draw_text(17 * 22, 8 * 22, 100, 22, buf, COLOR_WHITE, 0);

	snprintf(buf, sizeof(buf), "%d", ply.defence);
	term_draw_text(17 * 22, 9 * 22, 100, 22, buf, COLOR_WHITE, 0);

	snprintf(buf, sizeof(buf), "%d", ply.money);
	term_draw_text(17 * 22, 11 * 22, 100, 22, buf, COLOR_WHITE, 0);
}
