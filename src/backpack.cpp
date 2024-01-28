#include "backpack.h"

/**
 * @brief 在背包里添加一个道具
 * @param 要添加的道具的地址
 */
void Backpack::addItem(Item *i)
{
	if (this->isEmpty)
	{
		this->headPtr = i;
		this->tailPtr = i;

		i->lastItem = nullptr;
		i->nextItem = nullptr;

		this->isEmpty = false;
	}
	else
	{
		i->lastItem = this->tailPtr;
		i->lastItem->nextItem = i;
		this->tailPtr = i;
	}
}

/**
 * @brief 在背包里删除一个道具
 * @param 要删除的道具的地址
 */
void Backpack::delItem(Item *i)
{
	if (this->headPtr == this->tailPtr && this->headPtr == i) // 这是唯一一件物品了
	{
		this->headPtr = nullptr;
		this->tailPtr = nullptr;
		this->isEmpty = true;
	}
	else
	{
		if (i->lastItem != nullptr)
			i->lastItem->nextItem = i->nextItem;
		if (i->nextItem != nullptr)
			i->nextItem->lastItem = i->lastItem;
	}
}

/**
 * @brief 进入选择道具的界面
 *
 * @param 如果选中了某个道具则返回该道具的地址;否则返回空指针
 */
Item *Backpack::selectItem()
{
	Item *firstItemOnPage = this->headPtr;
	Item *lastItemOnPage = firstItemOnPage;
	Item *itemPtr;
	Item *itemList[5];
	uint8_t numInThePage;
	uint16_t returnValue;
	uint8_t chosenOption = 0; // 被选中的选项
	uint8_t returnType = 0;	  // 定义了以何种方式返回
	uint8_t choosed = 0;

	// 背包里没有道具的情况
	if (this->isEmpty == true)
	{
		saySomething((char *)"背包里没有道具!");
	}
	else // 背包里有道具
	{
		while (1)
		{
			numInThePage = 0;
			itemPtr = firstItemOnPage;
			// 补全列表,并统计有几个可供选择的选项
			for (uint8_t k = 0; k < 5; k++)
			{
				itemList[k] = itemPtr;
				numInThePage++;
				if (itemPtr->nextItem == nullptr)
					break;
				else
					itemPtr = itemPtr->nextItem;
			}

			// choose
			returnValue = chooseFromPage(numInThePage, itemList);

			// extract
			choosed = (returnValue >> 15) & 0x01;
			returnType = (returnValue >> 8) & 0x7F;
			chosenOption = (uint8_t)((returnValue >> 15) & 0x01);

			if (choosed) // 按下了z,做出了选择
			{
				return itemList[chosenOption];
			}
			else if (returnType == 1) // 左翻页
			{
				// firstItemOnPage左移五个单位
				for (uint8_t j = 0; j < 5; j++)
				{
					if (firstItemOnPage->lastItem != nullptr)
					{
						firstItemOnPage = firstItemOnPage->lastItem;
					}
				}
			}
			else if (returnType == 2) // 右翻页
			{
				// itemPtr右移五个单位
				for (uint8_t j = 0; j < 5; j++)
				{
					if (firstItemOnPage->nextItem != nullptr)
					{
						firstItemOnPage = firstItemOnPage->nextItem;
					}
				}
			}
			else if (returnType == 0) // 退出
			{
				return nullptr;
			}
			else
			{
				return nullptr; // 出问题了
			}
		}
	}
}

/**
 * @brief 从一页中选择一个选项
 * @note 该函数由event.h中的chooseFromSomething改写而来
 * @note 该函数能返回的信息:是否进行了选择;是否翻到上一页或下一页;选中了该页的第几个选项
 *
 * @return 一个uint16_t
 * @return 返回值的第一位:是否选中了某一项
 * @return 返回值的第2-8位:0代表按下了x键返回;1代表按下了左键返回;2代表按下了右键返回
 * @return 返回值的后9-16位:返回的选项
 */
uint16_t chooseFromPage(uint8_t num, Item **list)
{
	uint16_t returnValue = 0;
	uint16_t chosenOption = 0; // 被选中的选项

	uint16_t returnType = 0; // 定义了以何种方式返回
	uint16_t choosed = 0;

	uint8_t lastOption;
	KEY key;
	// 绘制最开始的样子
	for (int i = 0; i < num; i++)
	{
		if (i == 0)
			regionEmphasize(2, 13 + i, list[i]->name);
		else
			regionPrint(2, 13 + i, list[i]->name);
	}
	while (1)
	{
		key = getKey();
		lastOption = chosenOption;
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
		case LEFT:
			//在退出之前先擦掉
			for (int i = 0; i < num; i++)
			{
				regionErase(2, 13 + i, strlen(list[i]->name));
			}
			choosed = 0x0000;	// 0000_0000_0000_0000
			returnType = 0x100; // 0000_0001_0000_0000
			returnValue = choosed | returnType | chosenOption;
			return returnValue;
		case RIGHT:
			//在退出之前先擦掉
			for (int i = 0; i < num; i++)
			{
				regionErase(2, 13 + i, strlen(list[i]->name));
			}
			choosed = 0x0000;	// 0000_0000_0000_0000
			returnType = 0x200; // 0000_0010_0000_0000
			returnValue = choosed | returnType | chosenOption;
			return returnValue;
		case KEY_Z:
			for (int i = 0; i < num; i++)
			{
				regionErase(2, 13 + i, strlen(list[i]->name));
			}
			choosed = 0x8000; // 1000_0000_0000_0000
			returnType = 0;	  // 这里的值随意了,用不上
			returnValue = choosed | returnType | chosenOption;
			return returnValue;
			break;
		case KEY_X:
			for (int i = 0; i < num; i++)
			{
				regionErase(2, 13 + i, strlen(list[i]->name));
			}
			choosed = 0x0000; // 0000_0000_0000_0000
			returnType = 0;	  //
			chosenOption = 255;
			returnValue = choosed | returnType | chosenOption;
			return returnValue;
			break;
		}
		// 更新显示
		regionPrint(2, 13 + lastOption, list[lastOption]->name);
		regionEmphasize(2, 13 + chosenOption, list[chosenOption]->name);
	}
}