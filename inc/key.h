#ifndef _KEY_H
#define _KEY_H

#include <conio.h>
#include <stdint.h>

enum KEY{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	KEY_V,
	KEY_Q,
	ERR
};

KEY getKey();

#endif