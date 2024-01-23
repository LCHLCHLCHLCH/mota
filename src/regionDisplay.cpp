#include "regionDisplay.h"

/**
 * @brief 在某个区域显示一段文字
 * @param start_place_x 开始位置的x坐标
 * @param start_place_y 开始位置的y坐标
 * @param line_width 行宽
 * @param s 要显示的字符
 */
void regionPrint(uint8_t start_place_x, uint8_t start_place_y, uint8_t line_width, std::string s)
{
	gotoxy(start_place_x, start_place_y);
	uint8_t column = 0;
	uint8_t line = 0;
	char *ptr = &s[0];
	while (*ptr)
	{
		printf("%c", *ptr);
		ptr++;
		column++;
		if (column == line_width*2)
		{
			line++;
			column = 0;
			gotoxy(start_place_x, start_place_y+line);
		}
	}
}