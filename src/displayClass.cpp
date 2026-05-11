#include "displayClass.h"
#include "Cursor.h"
#include "framePrintChar.h"
#include <stdint.h>
#include "playerClass.h"

void Display::init(const Player& ply)
{
	for (uint8_t i = 0; i < 13; i++)
		for (uint8_t j = 0; j < 13; j++)
			last_frame[i][j] = BLANK;

	for (uint8_t i = 0; i < 13; i++)
		for (uint8_t j = 0; j < 13; j++)
			temp_frame[i][j] = map_get(ply.floor, j, i);
	temp_frame[ply.y][ply.x] = 255;
}

void Display::store_frame()
{
	for (uint8_t i = 0; i < 13; i++)
		for (uint8_t j = 0; j < 13; j++)
			last_frame[i][j] = temp_frame[i][j];
}

void Display::generateFrame(const Player& ply)
{
	for (uint8_t i = 0; i < 13; i++)
		for (uint8_t j = 0; j < 13; j++)
			temp_frame[i][j] = map_get(ply.floor, j, i);
	temp_frame[ply.y][ply.x] = 255;
}

void Display::printDifferentia()
{
	for (uint8_t i = 0; i < 13; i++)
	{
		for (uint8_t j = 0; j < 13; j++)
		{
			if (temp_frame[i][j] != last_frame[i][j])
			{
				gotoxy(2 * j, i);
				framePrintChar(temp_frame[i][j]);
			}
		}
	}
}
