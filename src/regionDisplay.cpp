#include "regionDisplay.h"

void regionPrint(uint8_t start_place_x, uint8_t start_place_y, uint8_t line_width, char *s) {
	gotoxy(start_place_x, start_place_y);
	addstr_gbk(s);
}

void regionErase(uint8_t start_place_x, uint8_t start_place_y, uint8_t line_width, uint16_t len) {
	gotoxy(start_place_x, start_place_y);
	for (uint16_t i = 0; i < len; i++) {
		addch(' ');
	}
}

void regionEmphasize(uint8_t start_place_x, uint8_t start_place_y, uint8_t line_width, char* s) {
	SetConsoleColor(9 * 16 | 7);
	gotoxy(start_place_x, start_place_y);
	addstr_gbk(s);
	SetColor(WHITE);
}

void regionPrint(uint8_t start_place_x, uint8_t start_place_y, char *s) {
	gotoxy(start_place_x, start_place_y);
	addstr_gbk(s);
}

void regionErase(uint8_t start_place_x, uint8_t start_place_y, uint16_t len) {
	gotoxy(start_place_x, start_place_y);
	for (uint16_t i = 0; i < len; i++) {
		addch(' ');
	}
	gotoxy(0, 14);
	hideCursor();
}

void regionEmphasize(uint8_t start_place_x, uint8_t start_place_y, char* s) {
	SetConsoleColor(9 * 16 | 7);
	gotoxy(start_place_x, start_place_y);
	addstr_gbk(s);
	SetColor(WHITE);
}
