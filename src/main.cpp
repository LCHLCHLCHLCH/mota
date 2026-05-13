#include "game/player.h"
#include "render/display.h"
#include "render/key.h"
#include "game/map.h"
#include <render/cursor.h>
#include "game/monster.h"
#include "render/region_display.h"
#include "render/status_bar.h"
#include "event/dialog.h"
#include "ui/backpack.h"
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

	map_init_default();

	SetColor(WHITE);

	while (1)
	{
		touchwin(stdscr);

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
