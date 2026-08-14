#include "ui/backpack.h"
#include "game/tile_data.h"
#include "script/lua_bridge.h"
#include "sdl_terminal.h"
#include <cstdio>
#include <cstring>

void Backpack::addItem(uint8_t id, const char* name, int uses)
{
	items.push_back({id, (char*)name, uses});
}

void Backpack::removeItem(size_t index)
{
	if (index < items.size())
		items.erase(items.begin() + index);
}

// 列表显示名称：有次数限制的道具附上剩余次数，如 "中心对称飞行器×3"
static void itemDisplayName(const Item& it, char* buf, size_t bufsize)
{
	if (it.uses > 0)
		snprintf(buf, bufsize, "%s×%d", it.name, it.uses);
	else
		snprintf(buf, bufsize, "%s", it.name);
}

int Backpack::selectItem()
{
	if (items.empty()) {
		saySomething((char*)"背包里没有道具!");
		return -1;
	}

	const int perPage = 8;
	int pageStart = 0;
	int selected  = 0;
	const int count = (int)items.size();
	char buf[128];
	char desc[256];

	// 隐藏右侧状态栏等所有 UI，避免与详情冲突
	term_clear_draws();
	term_clear_message();
	for (int r = 0; r < 22; r++) regionErase(15, r, 13);

	// 标题
	regionPrint(2, 13, (char*)"背包");

	while (1) {
		// 修正页码与选中位置（光标到底后按下键自动滚动）
		if (selected < pageStart) pageStart = selected;
		if (selected >= pageStart + perPage) pageStart = selected - perPage + 1;
		int rows = count - pageStart;
		if (rows > perPage) rows = perPage;

		// 绘制列表（行 14-21）：道具图标 + 名称
		for (int r = 14; r <= 21; r++) regionErase(2, r, 13);
		for (int i = 0; i < rows; i++) {
			int idx = pageStart + i;
			const Item& it = items[idx];
			int row = 14 + i;
			const TileDef& def = g_tile_defs[it.id];
			gotoxy(2, row);
			if (def.symbol[0])
				colorPrint((COLOR)def.color, (char*)def.symbol);
			itemDisplayName(it, buf, sizeof(buf));
			if (idx == selected)
				regionEmphasize(3, row, buf);
			else
				regionPrint(3, row, buf);
		}

		// 右侧详情面板（cols 15-27）：顶部道具名，下方描述
		for (int r = 0; r <= 6; r++) regionErase(15, r, 13);
		regionErase(15, 21, 13);
		{
			const Item& it = items[selected];
			gotoxy(15, 0);
			colorPrint(WHITE, (char*)it.name);

			// 描述（按字符数自动换行，最多 6 行）
			if (lua_item_desc(it.id, desc, sizeof(desc))) {
				const char* p = desc;
				int row = 1;
				while (*p && row <= 6) {
					const char* q = p;
					int chars = 0;
					while (chars < 12 && *q) {
						unsigned char c = (unsigned char)*q;
						int clen = (c >= 0xF0) ? 4 : (c >= 0xE0) ? 3 : (c >= 0xC0) ? 2 : 1;
						q += clen;
						chars++;
					}
					int bytes = (int)(q - p);
					char tmp[64];
					memcpy(tmp, p, (size_t)bytes);
					tmp[bytes] = 0;
					regionPrint(15, row, tmp);
					p = q;
					row++;
				}
			}

			snprintf(buf, sizeof(buf), "%d/%d", selected + 1, count);
			regionPrint(15, 21, buf);
		}

		term_present();

		KEY key = getKey();
		switch (key) {
		case UP:
			if (selected > 0) selected--;
			break;
		case DOWN:
			if (selected < count - 1) selected++;
			break;
		case LEFT:
			if (pageStart > 0) { pageStart = (pageStart >= perPage) ? pageStart - perPage : 0; selected = pageStart; }
			break;
		case RIGHT:
			if (pageStart + perPage < count) { pageStart += perPage; selected = pageStart; }
			break;
		case KEY_Z:
			return selected;
		case KEY_X:
			for (int r = 13; r <= 21; r++) regionErase(2, r, 26);
			for (int r = 0; r < 22; r++) regionErase(15, r, 13);
			term_present();
			return -1;
		default:
			break;
		}
	}
}
