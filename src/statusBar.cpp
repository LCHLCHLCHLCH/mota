#include "statusBar.h"

void StatusBar::draw(const Player& ply)
{
	// 楼层
	gotoxy(32, 0);
	printw("%d   ", ply.floor);

	// 标签
	gotoxy(28, 2);
	colorPrint(RED, (char *)"血");
	gotoxy(28, 4);
	colorPrint(YELLOW, (char *)"钥");
	gotoxy(28, 5);
	colorPrint(BLUE, (char *)"钥");
	gotoxy(28, 6);
	colorPrint(RED, (char *)"钥");
	gotoxy(28, 8);
	colorPrint(RED, (char *)"攻");
	gotoxy(28, 9);
	colorPrint(BLUE, (char *)"防");
	gotoxy(28, 11);
	colorPrint(YELLOW, (char *)"金");

	// 数值
	gotoxy(31, 2);
	printw("%-7d", ply.health);
	gotoxy(31, 4);
	printw("%-3d", ply.yellowKey);
	gotoxy(31, 5);
	printw("%-3d", ply.blueKey);
	gotoxy(31, 6);
	printw("%-3d", ply.redKey);
	gotoxy(31, 8);
	printw("%-3d", ply.attack);
	gotoxy(31, 9);
	printw("%-3d", ply.defence);
	gotoxy(31, 11);
	printw("%-3d", ply.money);
}
