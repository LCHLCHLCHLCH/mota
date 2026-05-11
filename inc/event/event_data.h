#pragma once
#include "event_types.h"

// 一个动作节点
struct EventAction {
	ActionType type;
	uint8_t    param;      // 通用参数（钥匙颜色、道具数量、偏移量等）
	const char* text;      // SAY / CHOICE 的文本（指向静态字符串）
};

// 一个完整事件
struct Event {
	uint8_t         floor_;         // 所在楼层
	EventTrigger    trigger;        // 触发方式
	uint8_t         trigger_tile;   // 触发 tile ID
	uint8_t         condition_flag; // 此标记未设置时才触发（0 = 总是触发）
	uint8_t         set_flag;       // 完成后设置的标记（0 = 不设）
	uint8_t         action_count;
	const EventAction* actions;
};
