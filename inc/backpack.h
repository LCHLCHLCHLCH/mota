#pragma once

#include "item.h"
#include "regionDisplay.h"
#include "event.h"
#include "key.h"

class Backpack
{
public:
	bool isEmpty = true; // 判断背包是否是空的
	Item* headPtr;//指向第一件物品的指针
	Item* tailPtr;//指向最后一件物品的指针

	void addItem(Item * i);
	void delItem(Item * i);

	//进入道具选择界面,如果选中了某个道具就返回该道具的地址,否则返回空指针
	Item * selectItem();
};

uint16_t chooseFromPage(uint8_t num, Item **list);
