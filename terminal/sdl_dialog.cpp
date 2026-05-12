// SDL3 版本的对话框（替代 src/event/dialog.cpp）
#include <cstring>
#include "render/key.h"
#include "render/region_display.h"
#include "sdl_terminal.h"

void saySomething(char *s) {
	regionEmphasize(2, 13, s);
	term_present();
	while (1) {
		KEY k = getKey();
		if (k == KEY_Z) break;
		if (k == KEY_X) break;
	}
	regionErase(2, 13, strlen(s));
	term_present();
	drainInput();
}

uint8_t chooseFromSomething(uint8_t num, char **list) {
	uint8_t chosenOption = 0;
	uint8_t lastOption;
	KEY key;
	for (int i = 0; i < num; i++) {
		if (i == 0)
			regionEmphasize(2, 13 + i, list[i]);
		else
			regionPrint(2, 13 + i, list[i]);
	}
	term_present();
	while (1) {
		key = getKey();
		switch (key) {
		case UP:
			if (chosenOption > 0) { lastOption = chosenOption; chosenOption--; }
			break;
		case DOWN:
			if (chosenOption < num - 1) { lastOption = chosenOption; chosenOption++; }
			break;
		case KEY_Z:
			for (int i = 0; i < num; i++)
				regionErase(2, 13 + i, strlen(list[i]));
			term_present();
			return chosenOption;
		case KEY_X:
			for (int i = 0; i < num; i++)
				regionErase(2, 13 + i, strlen(list[i]));
			term_present();
			return 255;
		default: break;
		}
		regionPrint(2, 13 + lastOption, list[lastOption]);
		regionEmphasize(2, 13 + chosenOption, list[chosenOption]);
		term_present();
	}
}
