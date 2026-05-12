// SDL3 魔塔终端版本
#include "sdl_terminal.h"
#include "console_cmd.h"
#include "game/player.h"
#include "render/display.h"
#include "render/key.h"
#include "game/map.h"
#include "game/monster.h"
#include "render/region_display.h"
#include "render/status_bar.h"
#include "event/dialog.h"
#include "ui/backpack.h"
#include "event/event_manager.h"

int main(int argc, char** argv) {
	(void)argc; (void)argv;

	if (!term_init("魔塔 - SDL3", 28, 22, 22, 22))
		return 1;

	console_welcome();

	Player player;
	player.init();

	Display display;
	StatusBar statusBar;
	Backpack backpack;
	EventManager events;
	events.init();
	player.events = &events;

	SetColor(WHITE);

	while (!term_quit_requested()) {
		touchwin_term();
		display.generateFrame(player);
		statusBar.draw(player);
		hideCursor();
		term_present();

		console_poll(player);

		KEY key = getKey();
		if (term_quit_requested()) break;

		if (key == KEY_X)
			backpack.selectItem();
		else
			player.respondToKey(key);
	}

	console_cmd_shutdown();
	term_shutdown();
	return 0;
}
