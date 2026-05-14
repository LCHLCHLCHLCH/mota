#pragma once
#include <stdint.h>
#include "game/tile_data.h"

void map_init();         // 从 Lua 文件加载所有楼层地图
void map_init_default(); // 保存当前地图为默认状态
uint8_t map_get(uint8_t floor, uint8_t x, uint8_t y);
void    map_set(uint8_t floor, uint8_t x, uint8_t y, uint8_t value);
uint8_t map_get_default(uint8_t floor, uint8_t x, uint8_t y);
