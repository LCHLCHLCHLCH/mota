#include "Cursor.h"

// Map foreground COLOR to curses color constant
static int curses_fg(COLOR c) {
	switch (c) {
		case RED:    return COLOR_RED;
		case YELLOW: return COLOR_YELLOW;
		case BLUE:   return COLOR_BLUE;
		case WHITE:  return COLOR_WHITE;
		case GREEN:  return COLOR_GREEN;
		case PURPLE: return COLOR_MAGENTA;
		case GREY:   return COLOR_WHITE;   // curses has no grey
		case LIGHT_GREEN: return COLOR_CYAN;
		default:     return COLOR_WHITE;
	}
}

// Color pair IDs
enum {
	PAIR_WHITE = 1,
	PAIR_RED,
	PAIR_YELLOW,
	PAIR_BLUE,
	PAIR_GREEN,
	PAIR_MAGENTA,
	PAIR_CYAN,
	PAIR_GREY,
	PAIR_LAVA,       // white on red
	PAIR_WALL,       // white on white
	PAIR_STAR,       // white on blue
	PAIR_EMPHASIS,   // white on blue (same bg, different purpose)
};

static int color_to_pair(COLOR c) {
	switch (c) {
		case RED:    return PAIR_RED;
		case YELLOW: return PAIR_YELLOW;
		case BLUE:   return PAIR_BLUE;
		case WHITE:  return PAIR_WHITE;
		case GREEN:  return PAIR_GREEN;
		case PURPLE: return PAIR_MAGENTA;
		case GREY:   return PAIR_GREY;
		case LIGHT_GREEN: return PAIR_CYAN;
		default:     return PAIR_WHITE;
	}
}

static int current_pair = PAIR_WHITE;

void console_init() {
	initscr();
	start_color();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);

	// Foreground-color pairs (on black)
	init_pair(PAIR_WHITE,   COLOR_WHITE,   COLOR_BLACK);
	init_pair(PAIR_RED,     COLOR_RED,     COLOR_BLACK);
	init_pair(PAIR_YELLOW,  COLOR_YELLOW,  COLOR_BLACK);
	init_pair(PAIR_BLUE,    COLOR_BLUE,    COLOR_BLACK);
	init_pair(PAIR_GREEN,   COLOR_GREEN,   COLOR_BLACK);
	init_pair(PAIR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(PAIR_CYAN,    COLOR_CYAN,    COLOR_BLACK);
	init_pair(PAIR_GREY,    COLOR_WHITE,   COLOR_BLACK);

	// Special background pairs
	init_pair(PAIR_LAVA,     COLOR_WHITE, COLOR_RED);
	init_pair(PAIR_WALL,     COLOR_WHITE, COLOR_WHITE);
	init_pair(PAIR_STAR,     COLOR_WHITE, COLOR_BLUE);
	init_pair(PAIR_EMPHASIS, COLOR_WHITE, COLOR_BLUE);
}

void console_shutdown() {
	endwin();
}

void gotoxy(int x, int y) {
	move(y, x);
}

void hideCursor() {
	curs_set(0);
}

void SetConsoleColor(int attr) {
	int pair = PAIR_WHITE;
	switch (attr) {
		case 7 * 16 | 7:  pair = PAIR_WALL;  break;  // white bg white fg
		case 64 | 7:      pair = PAIR_LAVA;   break;  // red bg white fg
		case 1 * 16 | 7:  pair = PAIR_STAR;   break;  // blue bg white fg
		case 9 * 16 | 7:  pair = PAIR_EMPHASIS; break; // blue bg white fg
		default:          pair = PAIR_WHITE;  break;
	}
	current_pair = pair;
	attron(COLOR_PAIR(pair));
}

void SetColor(COLOR a) {
	current_pair = color_to_pair(a);
	attr_set(0, current_pair, nullptr);
}

void colorPrint(COLOR c, char *s) {
	int pair = color_to_pair(c);
	attron(COLOR_PAIR(pair));
	addstr(s);
	attroff(COLOR_PAIR(pair));
	attr_set(0, current_pair, nullptr);
}
