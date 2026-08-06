#pragma once

#include <stdint.h>

class Monster
{
public:
	uint32_t health;
	uint32_t attack;
	uint32_t defence;
	uint32_t money;
};

Monster* getMonsterType(uint8_t id);
const char* getMonsterName(uint8_t id);
bool isBossMonster(uint8_t id);

// 怪物身躯(136)所在楼层对应的首领怪物 id；无则返回 0
uint8_t bodyBossForFloor(uint8_t floor);
