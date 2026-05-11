// SDL3 键盘输入（替代 src/key.cpp）
#include "render/key.h"
#include "sdl_terminal.h"

KEY getKey() {
	int ch = getch_term();
	switch (ch) {
		case 0x103: return UP;     // KEY_UP
		case 0x102: return DOWN;   // KEY_DOWN
		case 0x104: return LEFT;   // KEY_LEFT
		case 0x105: return RIGHT;  // KEY_RIGHT
		case 'v': case 'V': return KEY_V;
		case 'z': case 'Z': return KEY_Z;
		case 'x': case 'X': return KEY_X;
		case 'q': case 'Q': return KEY_Q;
		case 'e': case 'E': return KEY_E;
		case '1': return KEY_1;
		case '2': return KEY_2;
		default:   return NOTHING;
	}
}
