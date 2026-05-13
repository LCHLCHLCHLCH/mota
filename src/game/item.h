#pragma once
#include <stdint.h>

class Item
{
public:
	Item*   lastItem;
	Item*   nextItem;
	uint8_t id;
	char*   name;
};

const char* getItemName(uint8_t id);
