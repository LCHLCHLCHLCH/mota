// SDL3 版本的 StatusBar（替代 src/statusBar.cpp）
#include <cstdio>
#include "render/status_bar.h"
#include "sdl_terminal.h"

// 替代 printw：snprintf + addstr_gbk
static void sdl_printw(const char* fmt, ...) {
	char buf[64];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	addstr_gbk(buf);
}

void StatusBar::draw(const Player& ply) {
	gotoxy(32, 0);
	sdl_printw("%d   ", ply.floor);

	gotoxy(28, 2);  colorPrint(RED, (char*)"血");
	gotoxy(28, 4);  colorPrint(YELLOW, (char*)"钥");
	gotoxy(28, 5);  colorPrint(BLUE, (char*)"钥");
	gotoxy(28, 6);  colorPrint(RED, (char*)"钥");
	gotoxy(28, 8);  colorPrint(RED, (char*)"攻");
	gotoxy(28, 9);  colorPrint(BLUE, (char*)"防");
	gotoxy(28, 11); colorPrint(YELLOW, (char*)"金");

	gotoxy(31, 2);  sdl_printw("%-7d", ply.health);
	gotoxy(31, 4);  sdl_printw("%-3d", ply.yellowKey);
	gotoxy(31, 5);  sdl_printw("%-3d", ply.blueKey);
	gotoxy(31, 6);  sdl_printw("%-3d", ply.redKey);
	gotoxy(31, 8);  sdl_printw("%-3d", ply.attack);
	gotoxy(31, 9);  sdl_printw("%-3d", ply.defence);
	gotoxy(31, 11); sdl_printw("%-3d", ply.money);
}
