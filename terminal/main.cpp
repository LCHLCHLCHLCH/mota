// SDL3 魔塔终端版本
#include "sdl_terminal.h"

// 游戏逻辑头文件（stubs 目录会提供不依赖 ncurses 的 Cursor.h）
#include "playerClass.h"
#include "displayClass.h"
#include "key.h"
#include "map.h"
#include "monster.h"
#include "regionDisplay.h"
#include "statusBar.h"
#include "dialog.h"
#include "backpack.h"
#include "event/event_manager.h"

int main(int argc, char** argv) {
	(void)argc; (void)argv;

	if (!term_init("魔塔 - SDL3", 40, 16, 16, 24))
		return 1;

	Player player;
	player.init();

	Display display;
	StatusBar statusBar;
	Backpack backpack;
	EventManager events;
	events.init();
	player.events = &events;

	SetColor(WHITE);

	while (1) {
		touchwin_term();
		display.generateFrame(player);
		statusBar.draw(player);
		hideCursor();
		term_present();

		KEY key = getKey();

		if (key == KEY_X)
			backpack.selectItem();
		else
			player.respondToKey(key);
	}

	term_shutdown();
	return 0;
}
