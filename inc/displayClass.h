#pragma once

#include "Cursor.h"
#include "framePrintChar.h"
#include "playerClass.h"
#include "map.h"
#include <stdint.h>

#define BLANK 0

class Display
{
public:
	uint8_t last_frame[13][13];
	uint8_t temp_frame[13][13];

	void init(const Player& ply);
	void store_frame();
	void generateFrame(const Player& ply);
	void printDifferentia();
};
