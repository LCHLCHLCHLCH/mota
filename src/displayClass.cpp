#include "displayClass.h"
#include "framePrintChar.h"
#include <Cursor.h>
#include "map.h"

void Display::init(const Player&) {}

void Display::generateFrame(const Player& ply)
{
	for (uint8_t i = 0; i < 13; i++)
	{
		for (uint8_t j = 0; j < 13; j++)
		{
			gotoxy(2 * j, i);
			framePrintChar(map_get(ply.floor, j, i));
		}
	}

	// 绘制玩家
	gotoxy(2 * ply.x, ply.y);
	framePrintChar(255);
}
