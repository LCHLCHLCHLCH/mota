#include "key.h"

KEY getKey(){
	uint8_t ch = getch();
	if(ch == 224){
		ch = getch();
		if(ch == 72)return UP;
		else if(ch == 80)return DOWN;
		else if(ch == 75)return LEFT;
		else if(ch == 77)return RIGHT;
		else return ERR;
	}
	else if(ch == 118||ch == 86)return KEY_V;
	else{
		return ERR;
	}
}