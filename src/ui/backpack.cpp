#include "ui/backpack.h"
#include <cstdio>

void Backpack::addItem(uint8_t id, const char* name, int uses)
{
	items.push_back({id, (char*)name, uses});
}

void Backpack::removeItem(size_t index)
{
	if (index < items.size())
		items.erase(items.begin() + index);
}

// 显示名称：有次数限制的道具附上剩余次数，如 "中心对称飞行器(剩余3次)"
static void itemDisplayName(const Item& it, char* buf, size_t bufsize)
{
	if (it.uses > 0)
		snprintf(buf, bufsize, "%s(剩余%d次)", it.name, it.uses);
	else
		snprintf(buf, bufsize, "%s", it.name);
}

int Backpack::selectItem()
{
	if (items.empty()) {
		saySomething((char*)"背包里没有道具!");
		return -1;
	}

	size_t pageStart = 0;
	int    selected  = 0;
	char   buf[64];

	while (1) {
		size_t numInPage = items.size() - pageStart;
		if (numInPage > 5) numInPage = 5;
		if (selected >= (int)numInPage) selected = (int)numInPage - 1;

		// 绘制当前页
		for (size_t i = 0; i < numInPage; i++) {
			itemDisplayName(items[pageStart + i], buf, sizeof(buf));
			if ((int)i == selected)
				regionEmphasize(2, 13 + i, buf);
			else
				regionPrint(2, 13 + i, buf);
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
			for (size_t i = 0; i < numInPage; i++) {
				itemDisplayName(items[pageStart + i], buf, sizeof(buf));
				regionErase(2, 13 + i, strlen(buf));
			}
			pageStart = (pageStart >= 5) ? pageStart - 5 : 0;
			selected  = 0;
			break;
		case RIGHT:
			for (size_t i = 0; i < numInPage; i++) {
				itemDisplayName(items[pageStart + i], buf, sizeof(buf));
				regionErase(2, 13 + i, strlen(buf));
			}
			if (pageStart + 5 < items.size())
				pageStart += 5;
			selected = 0;
			break;
		case KEY_Z:
			for (size_t i = 0; i < numInPage; i++) {
				itemDisplayName(items[pageStart + i], buf, sizeof(buf));
				regionErase(2, 13 + i, strlen(buf));
			}
			return (int)(pageStart + selected);
		case KEY_X:
			for (size_t i = 0; i < numInPage; i++) {
				itemDisplayName(items[pageStart + i], buf, sizeof(buf));
				regionErase(2, 13 + i, strlen(buf));
			}
			return -1;
		default:
			break;
		}

		// 更新光标位置
		if (lastSelected != selected) {
			itemDisplayName(items[pageStart + lastSelected], buf, sizeof(buf));
			regionPrint(2, 13 + lastSelected, buf);
			itemDisplayName(items[pageStart + selected], buf, sizeof(buf));
			regionEmphasize(2, 13 + selected, buf);
#ifdef SDL3_BUILD
			term_present();
#endif
		}
	}
}
