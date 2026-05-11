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
#include "event/event_manager.h"

int main()
{
	KEY key;
	console_init();

	Player player;
	player.init();

	Display display;
	StatusBar statusBar;
	Backpack backpack;
	EventManager events;
	events.init();

	SetColor(WHITE);

	while (1)
	{
		erase();

		display.generateFrame(player);
		statusBar.draw(player);
		hideCursor();
		refresh();

		key = getKey();

		if (key == KEY_X)
		{
			backpack.selectItem();
		}
		else
		{
			player.respondToKey(key);

			// 检测当前位置的 NPC 事件
			uint8_t tile = map_get(player.floor, player.x, player.y);
			if (tile >= 151 && tile <= 155)
			{
				events.checkTile(player.floor, tile, player);
			}
		}
	}

	console_shutdown();
	return 0;
}
