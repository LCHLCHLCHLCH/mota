#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>
#include "eventMap.h"
#include "regionDisplay.h"
#include "key.h"
#include <conio.h>
#include <cstring>

extern uint8_t map[5][13][13];

void saySomething(char * s);
uint8_t chooseFromSomething(uint8_t num, char** list);

// void upStair();

#endif