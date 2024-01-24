#include "playerClass.h"
#include "displayClass.h"
#include "key.h"
#include "map.h"
#include "Cursor.h"
#include "monsterClass.h"
#include "regionDisplay.h"
#include "statusBarClass.h"

extern uint8_t map[5][13][13];
KEY key;

/**
 * @brief 怪物的实例化
 */

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

	// 显示颜色设为白色
	SetColor(WHITE);

	// 显示最初的内容
	display.printDifferentia();
	statusBar.firstShow(player);
	hideCursor();

	//测试的显示代码
	regionPrint(2,14,12,"欢迎来到魔塔,你是第101位挑战者。");

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

		//右边栏更新显示
		statusBar.updateAll(player);

		// 隐藏光标
		hideCursor();
	}
}