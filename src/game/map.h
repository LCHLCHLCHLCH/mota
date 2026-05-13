#pragma once
#include <stdint.h>

// 地图 tile 分类边界
#define TILE_OBJECT_MIN    1
#define TILE_OBJECT_MAX    50
#define TILE_PROP_MIN      51
#define TILE_PROP_MAX      100
#define TILE_MONSTER_MIN   101
#define TILE_MONSTER_MAX   150
#define TILE_NPC_MIN       151
#define TILE_NPC_MAX       155

#define TILE_IS_OBJECT(t)   ((t) >= TILE_OBJECT_MIN  && (t) <= TILE_OBJECT_MAX)
#define TILE_IS_PROP(t)     ((t) >= TILE_PROP_MIN    && (t) <= TILE_PROP_MAX)
#define TILE_IS_MONSTER(t)  ((t) >= TILE_MONSTER_MIN && (t) <= TILE_MONSTER_MAX)
#define TILE_IS_NPC(t)      ((t) >= TILE_NPC_MIN     && (t) <= TILE_NPC_MAX)

uint8_t map_get(uint8_t floor, uint8_t x, uint8_t y);
void    map_set(uint8_t floor, uint8_t x, uint8_t y, uint8_t value);
uint8_t map_get_default(uint8_t floor, uint8_t x, uint8_t y);
void    map_init_default();
