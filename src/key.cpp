#include "key.h"

KEY getKey(){
	uint8_t ch = getch();
	if(ch == 224){
		ch = getch();
		if(ch == 72)return UP;
		else if(ch == 80)return DOWN;
		else if(ch == 75)return LEFT;
		else if(ch == 77)return RIGHT;
		else return NOTHING;
	}
	else if(ch == 118||ch == 86)return KEY_V;
	else if(ch == 122||ch == 90)return KEY_Z;
	else if(ch == 120||ch == 88)return KEY_X;
	else{
		return NOTHING;
	}
}