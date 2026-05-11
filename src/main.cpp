#include "playerClass.h"
#include "displayClass.h"
#include "key.h"
#include "map.h"
#include "Cursor.h"
#include "monster.h"
#include "regionDisplay.h"
#include "statusBar.h"
#include "dialog.h"
#include "backpack.h"

int main()
{
	KEY key;
	console_init();

	Player player;
	player.init();

	Display display;
	StatusBar statusBar;
	Backpack backpack;

	SetColor(WHITE);

	while (1)
	{
		// clear() 重置 ncurses 缓冲区，消除双宽度字符的
		// continuation-column 脏标记
		clear();

		display.generateFrame(player);
		statusBar.updateAll(player);
		hideCursor();
		refresh();

		key = getKey();

		if (key == KEY_X)
			backpack.selectItem();
		else
			player.respondToKey(key);
	}

	console_shutdown();
	return 0;
}
