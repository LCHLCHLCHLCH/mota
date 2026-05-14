// SDL3 魔塔终端版本
#include "sdl_terminal.h"
#include "console_cmd.h"
#include "script/lua_state.h"
#include "game/player.h"
#include "game/monster.h"
#include "render/display.h"
#include "render/key.h"
#include "game/map.h"
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
	script_init();

	Player player;
	player.init();

	Display display;
	StatusBar statusBar;
	Backpack backpack;
	EventManager events;
	events.init();
	player.events = &events;
	player.backpack = &backpack;

	map_init();
	map_init_default();

	SetColor(WHITE);

	while (!term_quit_requested()) {
		touchwin_term();
		display.generateFrame(player);
		statusBar.draw(player);
		hideCursor();
		term_present();

		console_poll(player, events);

		KEY key = getKey();
		if (term_quit_requested()) break;

		if (key == KEY_X) {
			Item* chosen = backpack.selectItem();
			if (chosen && chosen->id == 69) {
				player.freezeLava();
			}
			else if (chosen && chosen->id == 70) {
				int killed = 0;
				uint8_t px = player.x, py = player.y;
				uint8_t dirs[4][2] = {{px, (uint8_t)(py-1)}, {px, (uint8_t)(py+1)}, {(uint8_t)(px-1), py}, {(uint8_t)(px+1), py}};
				for (int i = 0; i < 4; i++) {
					uint8_t tx = dirs[i][0], ty = dirs[i][1];
					uint8_t t = map_get(player.floor, tx, ty);
					if (t >= 101 && t <= 150 && !isBossMonster(t)) {
						map_set(player.floor, tx, ty, 1);
						player.money += getMonsterType(t)->money;
						if (player.events)
							player.events->checkGuardKill(player.floor, tx, ty, player);
						killed++;
					}
				}
				backpack.delItem(chosen);
				if (killed > 0) {
					if (player.events)
						player.events->checkClear(player.floor);
					char _m[64];
					snprintf(_m, sizeof(_m), "炸药炸死了%d个怪物", killed);
					term_set_message(_m);
				} else {
					term_set_message("炸药没有效果");
				}
			}
		} else {
			player.respondToKey(key);
		}
	}

	console_cmd_shutdown();
	term_shutdown();
	return 0;
}
