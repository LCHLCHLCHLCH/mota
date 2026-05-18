#include "ui/backpack.h"

void Backpack::addItem(uint8_t id, const char* name)
{
	items.push_back({id, (char*)name});
}

void Backpack::removeItem(size_t index)
{
	if (index < items.size())
		items.erase(items.begin() + index);
}

int Backpack::selectItem()
{
	if (items.empty()) {
		saySomething((char*)"背包里没有道具!");
		return -1;
	}

	size_t pageStart = 0;
	int    selected  = 0;

	while (1) {
		size_t numInPage = items.size() - pageStart;
		if (numInPage > 5) numInPage = 5;
		if (selected >= (int)numInPage) selected = (int)numInPage - 1;

		// 绘制当前页
		for (size_t i = 0; i < numInPage; i++) {
			if ((int)i == selected)
				regionEmphasize(2, 13 + i, items[pageStart + i].name);
			else
				regionPrint(2, 13 + i, items[pageStart + i].name);
		}
#ifdef SDL3_BUILD
		term_present();
#endif

		KEY key = getKey();
		int  lastSelected = selected;

		switch (key) {
		case UP:
			if (selected > 0) selected--;
			break;
		case DOWN:
			if (selected < (int)numInPage - 1) selected++;
			break;
		case LEFT:
			for (size_t i = 0; i < numInPage; i++)
				regionErase(2, 13 + i, strlen(items[pageStart + i].name));
			pageStart = (pageStart >= 5) ? pageStart - 5 : 0;
			selected  = 0;
			break;
		case RIGHT:
			for (size_t i = 0; i < numInPage; i++)
				regionErase(2, 13 + i, strlen(items[pageStart + i].name));
			if (pageStart + 5 < items.size())
				pageStart += 5;
			selected = 0;
			break;
		case KEY_Z:
			for (size_t i = 0; i < numInPage; i++)
				regionErase(2, 13 + i, strlen(items[pageStart + i].name));
			return (int)(pageStart + selected);
		case KEY_X:
			for (size_t i = 0; i < numInPage; i++)
				regionErase(2, 13 + i, strlen(items[pageStart + i].name));
			return -1;
		default:
			break;
		}

		// 更新光标位置
		if (lastSelected != selected) {
			regionPrint(2, 13 + lastSelected, items[pageStart + lastSelected].name);
			regionEmphasize(2, 13 + selected, items[pageStart + selected].name);
#ifdef SDL3_BUILD
			term_present();
#endif
		}
	}
}
