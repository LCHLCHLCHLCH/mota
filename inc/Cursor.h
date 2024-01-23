#ifndef CURSOR_H
#define CURSOR_H

#include <string>

enum COLOR{
	RED,
	YELLOW,
	BLUE,
	WHITE,
	GREEN,
	PURPLE,
	GREY,
	LIGHT_GREEN
};

void gotoxy(int x, int y);
void hideCursor();
void SetConsoleColor(int color);
void SetColor(COLOR a);
void colorPrint(COLOR c, char * s);
// void colorPrint(COLOR c, std::string s);

#endif

/*
0 = 黑色 8 = 灰色
1 = 蓝色 9 = 淡蓝色
2 = 绿色 10 = 淡绿色
3 = 浅绿色 11 = 淡浅绿色
4 = 红色 12 = 淡红色
5 = 紫色 13 = 淡紫色
6 = 黄色 14 = 淡黄色
7 = 白色 15 = 亮白色
*/