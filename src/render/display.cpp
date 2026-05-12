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
#ifdef SDL3_BUILD
			gotoxy(j, i);
#else
			gotoxy(2 * j, i);
#endif
			framePrintChar(map_get(ply.floor, j, i));
		}
	}

#ifdef SDL3_BUILD
	gotoxy(ply.x, ply.y);
#else
	gotoxy(2 * ply.x, ply.y);
#endif
	framePrintChar(255);
}
