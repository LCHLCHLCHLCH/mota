#pragma once
#include <stdint.h>
#include "game/tile_data.h"

class Player;

void map_init();
void map_init_default();
uint8_t map_get(uint8_t floor, uint8_t x, uint8_t y);
void    map_set(uint8_t floor, uint8_t x, uint8_t y, uint8_t value);
uint8_t map_get_default(uint8_t floor, uint8_t x, uint8_t y);

// 杀死指定位置的怪物并通知事件系统，返回金币数
int map_kill_monster(uint8_t floor, uint8_t x, uint8_t y, Player* player);
