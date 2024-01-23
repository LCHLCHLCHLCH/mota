#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include <stdio.h>
#include "Cursor.h"
#include "playerClass.h"

class StatusBar
{
	public:
	void firstShow(Player ply);
	void updateAll(Player ply);
};

#endif