#include "render/display.h"
#include "render/frame_print.h"
#include <render/cursor.h>
#include "game/map.h"

void Display::init(const Player&) {}

void Display::generateFrame(const Player& ply)
{
	for (uint8_t i = 0; i < 13; i++)
	{
		for (uint8_t j = 0; j < 13; j++)
		{
			gotoxy(j, i);
			framePrintChar(map_get(ply.floor, j, i));
		}
	}

	gotoxy(ply.x, ply.y);
	framePrintChar(255);
}
