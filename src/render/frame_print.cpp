#include "render/frame_print.h"
#include <render/cursor.h>
#include "game/tile_data.h"

void framePrintChar(uint8_t id)
{
	const TileDef& def = g_tile_defs[id];

	switch (id) {
		case 2: case 11: SetConsoleColor(7 * 16 | 7); break;
		case 6:          SetConsoleColor(64 | 7);      break;
		case 7:          SetConsoleColor(1 * 16 | 7);  break;
		default: break;
	}

	if (def.symbol[0])
		colorPrint((COLOR)def.color, (char*)def.symbol);
	else
		addstr_gbk(" ");

	if (id == 2 || id == 6 || id == 7 || id == 11)
		SetColor(WHITE);
}
