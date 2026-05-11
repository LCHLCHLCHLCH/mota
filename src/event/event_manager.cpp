#include "event/event_manager.h"
#include "playerClass.h"
#include "dialog.h"
#include "map.h"

// ============================================================
// 事件动作数据（静态存储，指针指向这些字符串）
// ============================================================

// 事件 #1: 第3层老人 — 对话后获得怪物手册
static const char* TEXT_3F_OLD_MAN =
	"我可以给你一本怪物手册，你可以用它预测该楼层各怪物对你造成的伤害。";

static const EventAction ACT_3F_OLD_MAN[] = {
	{ ActionType::SAY,      0, TEXT_3F_OLD_MAN },
	{ ActionType::SET_FLAG, 1, nullptr },
	{ ActionType::END,      0, nullptr },
};

// ============================================================
// 事件注册表（在 init() 中统一注册）
// ============================================================
void EventManager::init()
{
	event_count_ = 0;
	for (uint8_t i = 0; i < MAX_FLAGS; i++)
		flags_[i] = 0;

	// --- 注册所有事件 ---

	// 第3层 老人(151)
	addEvent({
		/* floor_         */ 3,
		/* trigger        */ EventTrigger::ON_TILE,
		/* trigger_tile   */ 151,
		/* condition_flag */ 1,   // flag 1 未设置时才触发
		/* set_flag       */ 1,   // 完成后设 flag 1
		/* action_count   */ 2,
		/* actions        */ ACT_3F_OLD_MAN,
	});

	// TODO: 更多事件在此注册
}

// ============================================================
// 内部方法
// ============================================================
void EventManager::addEvent(const Event& ev)
{
	if (event_count_ < MAX_EVENTS)
		events_[event_count_++] = ev;
}

void EventManager::setFlag(uint8_t id)
{
	if (id < MAX_FLAGS)
		flags_[id] = 1;
}

bool EventManager::hasFlag(uint8_t id) const
{
	if (id >= MAX_FLAGS) return true;  // 无效 ID 视为已触发
	return flags_[id] != 0;
}

// ============================================================
// 对外接口
// ============================================================
void EventManager::checkTile(uint8_t floor, uint8_t tile_id, Player& player)
{
	for (uint8_t i = 0; i < event_count_; i++)
	{
		const Event& ev = events_[i];
		if (ev.trigger != EventTrigger::ON_TILE) continue;
		if (ev.floor_ != floor) continue;
		if (ev.trigger_tile != tile_id) continue;

		// 检查条件：condition_flag 为 0 则总是触发，否则需标记未设置
		if (ev.condition_flag != 0 && hasFlag(ev.condition_flag))
			continue;

		executeEvent(ev, player);
		return;  // 一帧只触发一个事件
	}
}

void EventManager::executeEvent(const Event& ev, Player& player)
{
	for (uint8_t i = 0; i < ev.action_count; i++)
	{
		const EventAction& act = ev.actions[i];
		if (act.type == ActionType::END) break;
		executeAction(act, player, ev.floor_);
	}
	// 事件完成后设置标记，防止重复触发
	if (ev.set_flag != 0)
		setFlag(ev.set_flag);
}

void EventManager::executeAction(const EventAction& act, Player& player, uint8_t ev_floor)
{
	switch (act.type)
	{
	case ActionType::SAY:
		saySomething((char*)act.text);
		break;

	case ActionType::SET_FLAG:
		setFlag(act.param);
		break;

	case ActionType::GIVE_KEY:
		switch (act.param) {
			case 0: player.yellowKey++; break;
			case 1: player.blueKey++;   break;
			case 2: player.redKey++;    break;
		}
		break;

	case ActionType::GIVE_POTION:
		if (act.param == 0)  // 红血瓶
			player.health += get_Red_Health_Potion_Value(player.floor);
		else                 // 蓝血瓶
			player.health += get_Blue_Health_Potion_Value(player.floor);
		break;

	case ActionType::GIVE_GEM:
		if (act.param == 0)  // 红宝石
			player.attack += get_Gem_Stone_Value(player.floor);
		else                 // 蓝宝石
			player.defence += get_Gem_Stone_Value(player.floor);
		break;

	case ActionType::GIVE_EQUIP: {
		// act.param = 道具 ID (58=铁剑, 59=铁盾, ... 67=神圣盾)
		uint8_t id = act.param;
		if      (id == 58) player.attack  += 10;
		else if (id == 59) player.defence += 10;
		else if (id == 60) player.attack  += 20;
		else if (id == 61) player.defence += 20;
		else if (id == 62) player.attack  += 40;
		else if (id == 63) player.defence += 40;
		else if (id == 64) player.attack  += 50;
		else if (id == 65) player.defence += 50;
		else if (id == 66) player.attack  += 100;
		else if (id == 67) player.defence += 100;
		break;
	}

	case ActionType::TAKE_MONEY:
		if (player.money >= act.param)
			player.money -= act.param;
		break;

	case ActionType::CHANGE_TILE:
		// act.param 编码：高 4 位 = y 偏移, 低 4 位 = x 偏移
		// 实际 tile 位置：以事件所在楼层和触发点为基准
		// 暂未实现具体定位逻辑
		break;

	default:
		break;
	}
}
