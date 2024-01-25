#ifndef DISPLAY_CLASS_H
#define DISPLAY_CLASS_H

#include "Cursor.h"
#include "framePrintChar.h"
#include "playerClass.h"
#include <stdint.h>

#define BLANK 0

extern uint8_t map[51][13][13];

class Display
{
public:
	uint8_t last_frame[13][13];
	uint8_t temp_frame[13][13];

	void init(Player ply);
	void store_frame();
	void generateFrame(Player ply);
	void printDifferentia();
};

#endif

