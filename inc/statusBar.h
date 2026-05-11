#pragma once

#include <stdio.h>
#include "Cursor.h"
#include "playerClass.h"

class StatusBar
{
	public:
	void firstShow(const Player& ply);
	void updateAll(const Player& ply);
};
