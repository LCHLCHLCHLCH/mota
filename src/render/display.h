#pragma once

#include "game/player.h"

class Display
{
public:
	void init(const Player& ply);
	void generateFrame(const Player& ply);
};
