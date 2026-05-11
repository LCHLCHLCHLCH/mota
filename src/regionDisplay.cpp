#include "regionDisplay.h"

void regionPrint(uint8_t start_place_x, uint8_t start_place_y, uint8_t line_width, char *s) {
	gotoxy(start_place_x, start_place_y);
	uint8_t column = 0;
	uint8_t line = 0;
	char *ptr = s;
	while (*ptr) {
		addch(*ptr);
		ptr++;
		column++;
		if (column == line_width * 2) {
			line++;
			column = 0;
			gotoxy(start_place_x, start_place_y + line);
		}
	}
}

void regionErase(uint8_t start_place_x, uint8_t start_place_y, uint8_t line_width, uint16_t len) {
	gotoxy(start_place_x, start_place_y);
	uint8_t column = 0;
	uint8_t line = 0;
	for (uint16_t i = 0; i < len; i++) {
		addch(' ');
		column++;
		if (column == line_width * 2) {
			line++;
			column = 0;
			gotoxy(start_place_x, start_place_y + line);
		}
	}
}

void regionEmphasize(uint8_t start_place_x, uint8_t start_place_y, uint8_t line_width, char* s) {
	SetConsoleColor(9 * 16 | 7);
	gotoxy(start_place_x, start_place_y);
	uint8_t column = 0;
	uint8_t line = 0;
	char *ptr = s;
	while (*ptr) {
		addch(*ptr);
		ptr++;
		column++;
		if (column == line_width * 2) {
			line++;
			column = 0;
			gotoxy(start_place_x, start_place_y + line);
		}
	}
	SetColor(WHITE);
}

void regionPrint(uint8_t start_place_x, uint8_t start_place_y, char *s) {
	gotoxy(start_place_x, start_place_y);
	char *ptr = s;
	while (*ptr) {
		addch(*ptr);
		ptr++;
	}
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
	char *ptr = s;
	while (*ptr) {
		addch(*ptr);
		ptr++;
	}
	SetColor(WHITE);
}
