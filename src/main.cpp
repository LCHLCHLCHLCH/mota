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
		// erase() 重置缓冲区但不触发物理清屏，避免闪烁
		erase();

		display.generateFrame(player);
		statusBar.draw(player);
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
