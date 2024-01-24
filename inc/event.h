#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>
#include "eventMap.h"
#include "regionDisplay.h"
#include <conio.h>

extern uint8_t map[5][13][13];

void saySomething(char * s);
void chooseFromSomething(uint8_t num, char*[]);

void upStair();

#endif