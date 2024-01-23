#include "Cursor.h"

#define WINDOWS

#ifdef WINDOWS
#include <windows.h>
#include <stdio.h>
#include <string>

/**
 * @brief 移动控制台的光标
 * @param x:横坐标
 * @param y:纵坐标
 */
void gotoxy(int x, int y)
{
	COORD position;
	position.X = x;
	position.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
}

/**
 * @brief 隐藏控制台光标
 */
void hideCursor()
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(handle, &CursorInfo); // 获取控制台光标信息
	CursorInfo.bVisible = false;			   // 隐藏控制台光标
	SetConsoleCursorInfo(handle, &CursorInfo); // 设置控制台光标状态
}

/**
 * @brief 设置控制台显示的颜色
 */
void SetConsoleColor(int color)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}

/**
 * @brief 设置具体的颜色
 */
void SetColor(COLOR a)
{
	switch (a)
	{
	case RED:
		SetConsoleColor(FOREGROUND_INTENSITY | 4);
		break;
	case BLUE:
		SetConsoleColor(FOREGROUND_INTENSITY | 1);
		break;
	case YELLOW:
		SetConsoleColor(FOREGROUND_INTENSITY | 6);
		break;
	case WHITE:
		SetConsoleColor(FOREGROUND_INTENSITY | 7);
		break;
	case GREEN:
		SetConsoleColor(FOREGROUND_INTENSITY | 2);
		break;
	case GREY:
		SetConsoleColor(FOREGROUND_INTENSITY | 8);
		break;
	case PURPLE:
		SetConsoleColor(FOREGROUND_INTENSITY | 13);
		break;
	case LIGHT_GREEN:
		SetConsoleColor(FOREGROUND_INTENSITY | 11);
		break;
	};
}

/**
 * @brief 以某个具体的颜色输出
 */
// void colorPrint(COLOR c, const char* s)
void colorPrint(COLOR c, char *s)
// void colorPrint(COLOR c, std::string s)
{
	SetColor(c);
	printf("%s", s);
	SetColor(WHITE);
}

#endif

#ifdef LINUX
#include <stdio.h>

void gotoxy(int x, int y)
{
	printf("\033[%d;%dH", y, x);
}

void hideCursor()
{
	printf("\e[?25l");
}

#endif
