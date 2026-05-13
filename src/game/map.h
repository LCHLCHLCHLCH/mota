#pragma once
#include <stdint.h>

uint8_t map_get(uint8_t floor, uint8_t x, uint8_t y);
void    map_set(uint8_t floor, uint8_t x, uint8_t y, uint8_t value);
uint8_t map_get_default(uint8_t floor, uint8_t x, uint8_t y);
void    map_init_default();
