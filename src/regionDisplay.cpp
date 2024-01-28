#include "regionDisplay.h"

/**
 * @brief 在某个区域显示一段文字
 * @param start_place_x 开始位置的x坐标
 * @param start_place_y 开始位置的y坐标
 * @param line_width 行宽
 * @param s 要显示的字符
 */
void regionPrint(uint8_t start_place_x, uint8_t start_place_y, uint8_t line_width, char *s)
{
	gotoxy(start_place_x, start_place_y);
	uint8_t column = 0;
	uint8_t line = 0;
	char *ptr = s;
	while (*ptr)
	{
		printf("%c", *ptr);
		ptr++;
		column++;
		if (column == line_width * 2)
		{
			line++;
			column = 0;
			gotoxy(start_place_x, start_place_y + line);
		}
	}
}

/**
 * @brief 在某个区域清除一段文字
 * @param start_place_x 开始位置的x坐标
 * @param start_place_y 开始位置的y坐标
 * @param line_width 行宽
 * @param len 要清除字符的长度
 */
void regionErase(uint8_t start_place_x, uint8_t start_place_y, uint8_t line_width, uint16_t len)
{
	gotoxy(start_place_x, start_place_y);
	uint8_t column = 0;
	uint8_t line = 0;
	for (uint16_t i = 0; i < len; i++)
	{
		printf("%c", ' ');
		column++;
		if (column == line_width * 2)
		{
			line++;
			column = 0;
			gotoxy(start_place_x, start_place_y + line);
		}
	}
}

/**
 * @brief 在某个区域以风格2显示一行文字
 * @param start_place_x 开始位置的x坐标
 * @param start_place_y 开始位置的y坐标
 * @param line_width 行宽
 * @param len 要清除字符的长度
 */
void regionEmphasize(uint8_t start_place_x, uint8_t start_place_y, uint8_t line_width, char* s)
{
	//将输出设置为强调色
	SetConsoleColor(9*16|7);//背景蓝色,前景白色
	//输出
	gotoxy(start_place_x, start_place_y);
	uint8_t column = 0;
	uint8_t line = 0;
	char *ptr = s;
	while (*ptr)
	{
		printf("%c", *ptr);
		ptr++;
		column++;
		if (column == line_width * 2)
		{
			line++;
			column = 0;
			gotoxy(start_place_x, start_place_y + line);
		}
	}
	//将输出设置为正常颜色
	SetColor(WHITE);
}

/**
 * @brief 在某个区域显示一段文字
 * @param start_place_x 开始位置的x坐标
 * @param start_place_y 开始位置的y坐标
 * @param s 要显示的字符
 *
 * @note 这个是不限宽的
 */
void regionPrint(uint8_t start_place_x, uint8_t start_place_y, char *s)
{
	gotoxy(start_place_x, start_place_y);
	uint8_t column = 0;
	uint8_t line = 0;
	char *ptr = s;
	while (*ptr)
	{
		printf("%c", *ptr);
		ptr++;
	}
}

/**
 * @brief 在某个区域清除一段文字
 * @param start_place_x 开始位置的x坐标
 * @param start_place_y 开始位置的y坐标
 * @param len 要清除字符的长度
 *
 * @note 这个是不限宽的
 */
void regionErase(uint8_t start_place_x, uint8_t start_place_y, uint16_t len)
{
	gotoxy(start_place_x, start_place_y);
	uint8_t column = 0;
	uint8_t line = 0;
	for (uint16_t i = 0; i < len; i++)
	{
		printf("%c", ' ');
		column++;
	}
	gotoxy(0,14);
	hideCursor();
}

/**
 * @brief 在某个区域以风格2显示一段文字
 * @param start_place_x 开始位置的x坐标
 * @param start_place_y 开始位置的y坐标
 * @param len 要清除字符的长度
 *
 * @note 这个是不限宽的
 */
void regionEmphasize(uint8_t start_place_x, uint8_t start_place_y, char* s)
{
	//将输出设置为强调色
	SetConsoleColor(9*16|7);//背景蓝色,前景白色
	//输出
	gotoxy(start_place_x, start_place_y);
	uint8_t column = 0;
	uint8_t line = 0;
	char *ptr = s;
	while (*ptr)
	{
		printf("%c", *ptr);
		ptr++;
	}
	//将输出设置为正常颜色
	SetColor(WHITE);
}
