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

	// 测试背包
	Item Item1 = {nullptr, nullptr, (char*)"1item"};
	Item Item2 = {nullptr, nullptr, (char*)"2item"};
	Item Item3 = {nullptr, nullptr, (char*)"3item"};
	Item Item4 = {nullptr, nullptr, (char*)"4item"};
	Item Item5 = {nullptr, nullptr, (char*)"5item"};
	Item Item6 = {nullptr, nullptr, (char*)"6item"};
	Item Item7 = {nullptr, nullptr, (char*)"7item"};
	Item Item8 = {nullptr, nullptr, (char*)"8item"};
	Item Item9 = {nullptr, nullptr, (char*)"9item"};
	Item Item10 = {nullptr, nullptr, (char*)"xitem"};
	Item Item11 = {nullptr, nullptr, (char*)"yitem"};
	backpack.addItem(&Item1);
	backpack.addItem(&Item2);
	backpack.addItem(&Item3);
	backpack.addItem(&Item4);
	backpack.addItem(&Item5);
	backpack.addItem(&Item6);
	backpack.addItem(&Item7);
	backpack.addItem(&Item8);
	backpack.addItem(&Item9);
	backpack.addItem(&Item10);
	backpack.addItem(&Item11);
	backpack.selectItem();

	while (1)
	{
		// 按键检测
		key = getKey();

		// 玩家移动
		player.respondToKey(key);

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