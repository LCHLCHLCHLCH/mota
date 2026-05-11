#include "statusBar.h"

/**
 * @brief 右侧状态边栏初始化
 * @param Player 传入一个player对象,将player对象的初始属性显示在侧面
 */
void StatusBar::firstShow(const Player& ply)
{
	// show the UI
	// gotoxy(28, 0);
	// printw("楼层");
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
	// gotoxy(28, 11);
	// colorPrint(WHITE, (char *)"武器");
	// gotoxy(28, 12);
	// colorPrint(WHITE, (char *)"防具");

	// show the num
	// gotoxy(32, 0);
	// printw("%d", ply.floor);
	gotoxy(31, 2);
	printw("%d", ply.health);
	gotoxy(31, 4);
	printw("%d", ply.yellowKey);
	gotoxy(31, 5);
	printw("%d", ply.blueKey);
	gotoxy(31, 6);
	printw("%d", ply.redKey);
	gotoxy(31, 8);
	printw("%d", ply.attack);
	gotoxy(31, 9);
	printw("%d", ply.defence);
	gotoxy(31, 11);
	printw("%d", ply.money);
	// gotoxy(33, 11);
	// printw("%d", ply.health);
	// gotoxy(33, 12);
	// printw("%d", ply.health);
}

void StatusBar::updateAll(const Player& ply)
{
	gotoxy(32, 0);
	printw("    ");
	gotoxy(32, 0);
	printw("%d", ply.floor);
	gotoxy(31, 2);
	printw("         ");
	gotoxy(31, 2);
	printw("%d", ply.health);
	gotoxy(31, 4);
	printw("    ");
	gotoxy(31, 4);
	printw("%d", ply.yellowKey);
	gotoxy(31, 5);
	printw("    ");
	gotoxy(31, 5);
	printw("%d", ply.blueKey);
	gotoxy(31, 6);
	printw("    ");
	gotoxy(31, 6);
	printw("%d", ply.redKey);
	gotoxy(31, 8);
	printw("    ");
	gotoxy(31, 8);
	printw("%d", ply.attack);
	gotoxy(31, 9);
	printw("    ");
	gotoxy(31, 9);
	printw("%d", ply.defence);
	gotoxy(31, 11);
	printw("    ");
	gotoxy(31, 11);
	printw("%d", ply.money);
	// gotoxy(33, 11);
	// printw("    ");
	// gotoxy(33, 11);
	// printw("%d", ply.health);
	// gotoxy(33, 12);
	// printw("    ");
	// gotoxy(33, 12);
	// printw("%d", ply.health);
}