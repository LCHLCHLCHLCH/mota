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
	player.events = &events;

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
			// 保存移动前位置，用于 NPC 碰壁回弹
			uint8_t prev_x = player.x;
			uint8_t prev_y = player.y;

			player.respondToKey(key);

			// 检测目标位置的交互
			uint8_t tile = map_get(player.floor, player.x, player.y);
			if (tile == 155)
			{
				events.checkAltar(player.floor, player);
				// NPC / 祭坛 不可穿越，弹回原位
				player.x = prev_x;
				player.y = prev_y;
			}
			else if (tile >= 151 && tile <= 154)
			{
				events.checkTile(player.floor, tile, player);
				player.x = prev_x;
				player.y = prev_y;
			}

			// 排空输入缓冲，防止菜单方向键残留导致角色自动移动
			nodelay(stdscr, TRUE);
			while (getch() != ERR) {}
			nodelay(stdscr, FALSE);
		}
	}

	console_shutdown();
	return 0;
}
