#pragma once
#include <stdint.h>

struct Item
{
	uint8_t id;
	char*   name;
};

const char* getItemName(uint8_t id);
