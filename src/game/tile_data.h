#pragma once
#include <stdint.h>

// 地图 tile 分类
enum TileCategory : uint8_t {
	TILE_OBJECT  = 0,  // 地形/实体
	TILE_PROP    = 1,  // 可拾取道具
	TILE_MONSTER = 2,  // 怪物
	TILE_NPC     = 3,  // NPC / 祭坛
	TILE_NONE    = 4,  // 无效
};

// 每个 tile 的自描述属性
struct TileDef {
	TileCategory category;
	uint8_t      color;        // COLOR 枚举值 (见 cursor.h)
	const char*  symbol;       // 显示用字符（GBK）
	const char*  name;         // 中文名称
};

// 256 个 tile 的定义表
extern const TileDef g_tile_defs[256];

// 多格 Boss 身躯 tile id（必须在怪物区间 101-150 之外）
#define TILE_MONSTER_BODY 156

// 便捷访问
inline TileCategory tile_category(uint8_t id) { return g_tile_defs[id].category; }
inline const char*  tile_name(uint8_t id)     { return g_tile_defs[id].name; }
