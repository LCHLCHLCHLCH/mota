#pragma once
#include <stdint.h>

// 地图 tile 分类
enum TileCategory : uint8_t {
	TILE_OBJECT  = 0,  // 地形/实体 (空地, 墙, 门, 楼梯, 岩浆等)
	TILE_PROP    = 1,  // 可拾取道具 (钥匙, 血瓶, 宝石, 装备等)
	TILE_MONSTER = 2,  // 怪物
	TILE_NPC     = 3,  // NPC / 祭坛
	TILE_NONE    = 4,  // 未使用
};

// 根据 tile ID 返回分类
TileCategory tile_category(uint8_t id);

uint8_t map_get(uint8_t floor, uint8_t x, uint8_t y);
void    map_set(uint8_t floor, uint8_t x, uint8_t y, uint8_t value);
uint8_t map_get_default(uint8_t floor, uint8_t x, uint8_t y);
void    map_init_default();
