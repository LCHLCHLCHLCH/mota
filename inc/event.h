#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>
#include "eventMap.h"
#include "regionDisplay.h"
#include "key.h"
#include <conio.h>
#include <cstring>
#include "playerClass.h"

extern uint8_t map[51][13][13];

void saySomething(char *s);
uint8_t chooseFromSomething(uint8_t num, char **list);

void upStair(uint8_t *Floor, uint8_t *X, uint8_t *Y);
void downStair(uint8_t *Floor, uint8_t *X, uint8_t *Y);

#endif