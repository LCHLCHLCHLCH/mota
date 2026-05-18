#pragma once

#include <vector>
#include "game/item.h"
#include "render/region_display.h"
#include "event/dialog.h"
#include "render/key.h"

class Backpack
{
public:
	std::vector<Item> items;

	void addItem(uint8_t id, const char* name);
	void removeItem(size_t index);

	// 进入道具选择界面，返回选中项的 index，-1 表示取消 / 背包为空
	int selectItem();
};
