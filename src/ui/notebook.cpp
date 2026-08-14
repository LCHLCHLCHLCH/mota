#include "ui/notebook.h"
#include "render/region_display.h"
#include "render/display.h"
#include "render/key.h"
#include "sdl_terminal.h"
#include <cstdio>
#include <cstring>

void showNotebook(Player& player)
{
	if (player.dialogueLog.empty()) {
		saySomething((char*)"记事本里还没有记录任何对话。");
		return;
	}

	// 隐藏右侧状态栏等所有 UI，避免与记事本内容冲突
	term_clear_draws();
	term_clear_message();
	for (int r = 0; r < 22; r++) regionErase(15, r, 13);

	const int count = (int)player.dialogueLog.size();
	const int perPage = 8;
	int pageStart = 0;
	int selected = 0;
	char buf[128];

	// 标题
	snprintf(buf, sizeof(buf), "记事本");
	regionPrint(2, 13, buf);

	while (1)
	{
		// 修正页码与选中位置
		if (selected < pageStart) pageStart = selected;
		if (selected >= pageStart + perPage) pageStart = selected - perPage + 1;
		int rows = count - pageStart;
		if (rows > perPage) rows = perPage;

		// 绘制列表（行 14-21）：x层老人/商人
		for (int r = 14; r <= 21; r++) regionErase(2, r, 12);
		for (int i = 0; i < rows; i++)
		{
			int idx = pageStart + i;
			const DialogueEntry& e = player.dialogueLog[idx];
			snprintf(buf, sizeof(buf), "%d层%s", e.floor, e.label);
			if (idx == selected)
				regionEmphasize(3, 14 + i, buf);
			else
				regionPrint(3, 14 + i, buf);
		}

		// 右侧详情面板（cols 15-27）：对话内容，每行最多 12 字自动换行
		for (int r = 0; r < 22; r++) regionErase(15, r, 13);
		{
			const DialogueEntry& e = player.dialogueLog[selected];
			const size_t total = strlen(e.text);
			const char* p = e.text;
			int row = 0;
			while (*p && row < 20)
			{
				// 按"字符"计数换行：汉字 3 字节、数字 1 字节，按字节切会切破 UTF-8 汉字
				const char* q = p;
				int chars = 0;
				while (chars < 12 && (size_t)(q - e.text) < total)
				{
					unsigned char c = (unsigned char)*q;
					int clen = (c >= 0xF0) ? 4 : (c >= 0xE0) ? 3 : (c >= 0xC0) ? 2 : 1;
					q += clen;
					chars++;
				}
				if ((size_t)(q - e.text) > total) q = e.text + total;
				int bytes = (int)(q - p);
				char tmp[64];
				memcpy(tmp, p, (size_t)bytes);
				tmp[bytes] = 0;
				regionPrint(15, row, tmp);
				p = q;
				row++;
			}
			snprintf(buf, sizeof(buf), "%d/%d", selected + 1, count);
			regionPrint(15, 21, buf);
		}

		term_present();

		KEY key = getKey();
		switch (key)
		{
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
		case KEY_X:
			for (int r = 13; r <= 21; r++) regionErase(2, r, 26);
			for (int r = 0; r < 22; r++) regionErase(15, r, 13);
			term_present();
			return;
		default:
			break;
		}
	}
}
