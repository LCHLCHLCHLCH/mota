#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H
#include <stdint.h>
#include "key.h"
#include "monsterClass.h"
#include "calc.h"

extern uint8_t map[5][13][13];

enum PREDICTION
{
	DIE,
	LIVE,
	REACHABLE,
	UNREACHABLE
};

class Player
{
public:
	uint32_t health;
	uint32_t attack;
	uint32_t defence;
	uint32_t money;

	uint8_t redKey;
	uint8_t blueKey;
	uint8_t yellowKey;

	uint8_t x;
	uint8_t y;
	uint8_t floor;

	uint32_t hurt = 0;

	void init();
	PREDICTION PredictAttack(Monster monster);
	// PREDICTION predictMove(uint8_t Floor_going, uint8_t X_going, uint8_t Y_going);
	void respondToKey(KEY key);

	void respondToMap(uint8_t floor_going, uint8_t x_going, uint8_t y_going);
	void reactToObject(uint8_t floor_going, uint8_t x_going, uint8_t y_going);
	void reactToProp(uint8_t floor_going, uint8_t x_going, uint8_t y_going);
	void reactToMonster(uint8_t floor_going, uint8_t x_going, uint8_t y_going);

	void freezeLava();
};

#endif
