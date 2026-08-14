// SDL3 魔塔终端版本
#include "sdl_terminal.h"
#include "console_cmd.h"
#include "script/lua_state.h"
#include "script/lua_bridge.h"
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
	lua_register_game_api(script_init(), &player, &events);
	lua_set_frame_context(&display, &statusBar);

	map_init();
	map_init_default();

	// 执行可选启动脚本（scripts/startup.lua），不存在则使用默认初始状态
	lua_apply_startup(player);

	SetColor(WHITE);

	uint8_t prevFloor = player.floor;

	while (!term_quit_requested()) {
		touchwin_term();
		display.generateFrame(player);
		statusBar.draw(player);
		drawBossGlyphs(player);
		drawMonsterLights(player);
		hideCursor();
		term_present();

		console_poll(player, events);

		KEY key = getKey();
		if (term_quit_requested()) break;

		if (key == KEY_X) {
			int chosen = backpack.selectItem();
			if (chosen >= 0) {
				bool used = lua_item_on_use(backpack.items[chosen].id);
				if (backpack.items[chosen].uses > 0) {
					// 有次数限制的道具：成功使用一次减一，用完移除
					if (used) {
						backpack.items[chosen].uses--;
						if (backpack.items[chosen].uses <= 0)
							backpack.removeItem(chosen);
					}
				} else if (used) {
					backpack.removeItem(chosen);
				}
			}
		} else {
			player.respondToKey(key);
		}

		// 楼层变化 → 触发该层的首次到达事件（first_arrive）
		if (player.floor != prevFloor) {
			prevFloor = player.floor;
			events.checkFirstArrive(player.floor, player);
		}
	}

	console_cmd_shutdown();
	script_close();
	term_shutdown();
	return 0;
}
