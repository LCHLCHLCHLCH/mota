#include "key.h"
#include <ncursesw/curses.h>

KEY getKey() {
	int ch = getch();
	switch (ch) {
		case KEY_UP:    return UP;
		case KEY_DOWN:  return DOWN;
		case KEY_LEFT:  return LEFT;
		case KEY_RIGHT: return RIGHT;
		case 'v': case 'V': return KEY_V;
		case 'z': case 'Z': return KEY_Z;
		case 'x': case 'X': return KEY_X;
		case '1': return KEY_1;
		case '2': return KEY_2;
		default:   return NOTHING;
	}
}
