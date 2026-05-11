#include "playerClass.h"
#include "displayClass.h"
#include "key.h"
#include "map.h"
#include "Cursor.h"
#include "monsterClass.h"
#include "regionDisplay.h"
#include "statusBarClass.h"
#include "event.h"
#include "backpack.h"

extern uint8_t map[51][13][13];
KEY key;

int main()
{
	system("cls");

	// player init
	Player player;
	player.init();

	// display init
	Display display;
	display.init(player);

	// status bar init
	StatusBar statusBar;

	// backpack init
	Backpack backpack;

	// 显示颜色设为白色
	SetColor(WHITE);

	// 显示最初的内容
	display.printDifferentia();
	statusBar.firstShow(player);
	hideCursor();

	while (1)
	{
		// 按键检测
		key = getKey();

		if (key == KEY_X)
		{
			// 打开背包
			backpack.selectItem();
		}
		else
		{
			// 玩家移动
			player.respondToKey(key);
		}

		// 画面更新
		display.store_frame();
		display.generateFrame(player);
		display.printDifferentia();

		// 右边栏更新显示
		statusBar.updateAll(player);

		// 隐藏光标
		hideCursor();
	}
}