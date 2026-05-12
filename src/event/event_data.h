#pragma once
#include "event_types.h"

// 一个动作节点
struct EventAction {
	ActionType  type;
	uint8_t     param;      // 通用参数（钥匙颜色、道具数量、from_id 等）
	uint8_t     param2;     // 辅助参数（to_id 等）
	const char* text;       // SAY 的文本（指向静态字符串）
};

// 一个完整事件
struct Event {
	uint8_t         floor_;           // 所在楼层
	EventTrigger    trigger;          // 触发方式
	uint8_t         trigger_param;    // ON_TILE: tile ID; ON_GUARD_KILL: 守卫数量
	uint8_t         condition_flag;   // 此标记未设置时才触发（0 = 总是触发）
	uint8_t         set_flag;         // 完成后设置的标记（0 = 不设）
	uint8_t         action_count;
	const EventAction* actions;

	// ON_GUARD_KILL: 守卫位置（最多 4 个）
	uint8_t         guard_x[4];
	uint8_t         guard_y[4];
};
