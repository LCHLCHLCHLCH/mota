#pragma once
#include <stdint.h>
#include "game/map.h"
#include "render/key.h"
#include "game/monster.h"
#include "game/calc.h"
#include "event/dialog.h"

class EventManager;
class Backpack;

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

	bool     hasTeleporter = false;
	uint8_t  maxFloorVisited = 1;

	EventManager* events = nullptr;
	Backpack*     backpack = nullptr;

	void init();
	PREDICTION PredictAttack(Monster monster);
	void respondToKey(KEY key);

	void respondToMap(uint8_t floor_going, uint8_t x_going, uint8_t y_going);
	void reactToObject(uint8_t floor_going, uint8_t x_going, uint8_t y_going);
	void reactToProp(uint8_t floor_going, uint8_t x_going, uint8_t y_going);
	void reactToMonster(uint8_t floor_going, uint8_t x_going, uint8_t y_going);

	void freezeLava();

private:
	void findStair(uint8_t target_floor, uint8_t stair_id,
	               uint8_t& out_x, uint8_t& out_y);
	void teleportTo(uint8_t target_floor, uint8_t stair_id);
};

void upStair(uint8_t *Floor, uint8_t *X, uint8_t *Y);
void downStair(uint8_t *Floor, uint8_t *X, uint8_t *Y);
