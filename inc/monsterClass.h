#ifndef MONSTER_CLASS_H
#define MONSTER_CLASS_H

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

#endif