#pragma once

#include <stdio.h>
#include <render/cursor.h>
#include "game/player.h"

class StatusBar
{
public:
	void draw(const Player& ply);
};
