#ifndef REGION_DISPLAY_H
#define REGION_DISPLAY_H

#include "Cursor.h"
#include <stdio.h>
#include <stdint.h>
#include <cstring>

void regionPrint(uint8_t start_place_x, uint8_t start_place_y, uint8_t line_width, char *s);
void regionErase(uint8_t start_place_x, uint8_t start_place_y, uint8_t line_width, uint16_t len);

void regionPrint(uint8_t start_place_x, uint8_t start_place_y, char *s);
void regionErase(uint8_t start_place_x, uint8_t start_place_y, uint16_t len);

#endif