#pragma once
#include <stdint.h>

// 触发器类型
enum class EventTrigger : uint8_t {
	ON_TILE,       // 玩家踩上指定 tile
	ON_CLEAR,      // 某楼层所有怪物(101-150)被击杀
};

// 动作类型
enum class ActionType : uint8_t {
	SAY,           // 显示对话
	SET_FLAG,      // 设置全局标记
	GIVE_KEY,      // 给钥匙 (param: 0=黄 1=蓝 2=红)
	GIVE_POTION,   // 给血瓶 (param: 0=红 1=蓝)
	GIVE_GEM,      // 给宝石 (param: 0=红 1=蓝)
	GIVE_EQUIP,    // 给装备 (param: 装备ID, 铁剑=58...神圣盾=67)
	TAKE_MONEY,    // 扣金币
	REPLACE_ALL,   // 将某层所有 from_id 替换为 to_id
	END,           // 动作序列结束标记
};
