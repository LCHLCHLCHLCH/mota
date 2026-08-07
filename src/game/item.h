#pragma once
#include <stdint.h>

struct Item
{
	uint8_t id;
	char*   name;
	int     uses;   // 剩余使用次数；-1 表示不限次数
};

const char* getItemName(uint8_t id);
