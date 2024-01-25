#include "event.h"

/**
 * @brief 显示一段对话
 */
void saySomething(char *s)
{
	// regionPrint(2, 13, s);
	regionEmphasize(2, 13, s);
	while (1)
	{
		KEY k = getKey();
		if (k == KEY_Z)
			break;
		if (k == KEY_X)
			break;
	}
	regionErase(2, 13, strlen(s));
}

/**
 * @brief 提供几个可供选择的选项
 * @param num 选项的个数
 * @param char*[] 由选项里面各个字符串的首元素的地址组成的数组
 *
 * @return 被选中的选项,若为255则什么也没选中
 */
uint8_t chooseFromSomething(uint8_t num, char **list)
{
	uint8_t chosenOption = 0; // 被选中的选项
	uint8_t lastOption;
	KEY key;
	// 绘制最开始的样子
	for (int i = 0; i < num; i++)
	{
		if (i == 0)
			regionEmphasize(2, 13 + i, list[i]);
		else
			regionPrint(2, 13 + i, list[i]);
	}
	while (1)
	{
		key = getKey();
		switch (key)
		{
		case UP:
			if (chosenOption > 0)
			{
				lastOption = chosenOption;
				chosenOption--;
			}
			break;
		case DOWN:
			if (chosenOption < num - 1)
			{
				lastOption = chosenOption;
				chosenOption++;
			}
			break;
		case KEY_Z:
			for (int i = 0; i < num; i++)
			{
				regionErase(2, 13 + i, strlen(list[i]));
			}
			return chosenOption;
			break;
		case KEY_X:
			for (int i = 0; i < num; i++)
			{
				regionErase(2, 13 + i, strlen(list[i]));
			}
			return 255;
			break;
		}
		// 更新显示
		regionPrint(2, 13 + lastOption, list[lastOption]);
		regionEmphasize(2, 13 + chosenOption, list[chosenOption]);
	}
}
