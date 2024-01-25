#ifndef _KEY_H
#define _KEY_H

#include <conio.h>
#include <stdint.h>

enum KEY{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	KEY_Z,
	KEY_X,
	KEY_V,
	KEY_Q,
	KEY_1,
	KEY_2,
	NOTHING
};

KEY getKey();

#endif