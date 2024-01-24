#include "statusBarClass.h"

/**
 * @brief 右侧状态边栏初始化
 * @param Player 传入一个player对象,将player对象的初始属性显示在侧面
 */
void StatusBar::firstShow(Player ply)
{
	// show the UI
	// gotoxy(28, 0);
	// printf("楼层");
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
	// printf("%d", ply.floor);
	gotoxy(31, 2);
	printf("%d", ply.health);
	gotoxy(31, 4);
	printf("%d", ply.yellowKey);
	gotoxy(31, 5);
	printf("%d", ply.blueKey);
	gotoxy(31, 6);
	printf("%d", ply.redKey);
	gotoxy(31, 8);
	printf("%d", ply.attack);
	gotoxy(31, 9);
	printf("%d", ply.defence);
	gotoxy(31, 9);
	printf("%d", ply.money);
	// gotoxy(33, 11);
	// printf("%d", ply.health);
	// gotoxy(33, 12);
	// printf("%d", ply.health);
}

void StatusBar::updateAll(Player ply)
{
	// gotoxy(32, 0);
	// printf("    ");
	// gotoxy(32, 0);
	// printf("%d", ply.floor);
	gotoxy(31, 2);
	printf("         ");
	gotoxy(31, 2);
	printf("%d", ply.health);
	gotoxy(31, 4);
	printf("    ");
	gotoxy(31, 4);
	printf("%d", ply.yellowKey);
	gotoxy(31, 5);
	printf("    ");
	gotoxy(31, 5);
	printf("%d", ply.blueKey);
	gotoxy(31, 6);
	printf("    ");
	gotoxy(31, 6);
	printf("%d", ply.redKey);
	gotoxy(31, 8);
	printf("    ");
	gotoxy(31, 8);
	printf("%d", ply.attack);
	gotoxy(31, 9);
	printf("    ");
	gotoxy(31, 9);
	printf("%d", ply.defence);
	gotoxy(31, 11);
	printf("    ");
	gotoxy(31, 11);
	printf("%d", ply.money);
	// gotoxy(33, 11);
	// printf("    ");
	// gotoxy(33, 11);
	// printf("%d", ply.health);
	// gotoxy(33, 12);
	// printf("    ");
	// gotoxy(33, 12);
	// printf("%d", ply.health);
}