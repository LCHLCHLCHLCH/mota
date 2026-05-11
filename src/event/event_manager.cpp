#include "event/event_manager.h"
#include "playerClass.h"
#include "dialog.h"
#include "map.h"
#include <cstring>

// ============================================================
// 事件动作数据（静态存储，指针指向这些字符串）
// ============================================================

// 事件 #1: 第3层老人 — 对话
static const char* TEXT_3F_OLD_MAN =
	"我可以给你一本怪物手册，你可以用它预测该楼层各怪物对你造成的伤害。";

static const EventAction ACT_3F_OLD_MAN[] = {
	{ ActionType::SAY,      0, 0, TEXT_3F_OLD_MAN },
	{ ActionType::SET_FLAG, 1, 0, nullptr },
	{ ActionType::END,      0, 0, nullptr },
};

// 事件 #2: 第2层 — 击杀两个中级卫兵后打开牢门
static const EventAction ACT_2F_PRISON_OPEN[] = {
	{ ActionType::REPLACE_ALL, 8, 1, nullptr },  // 所有怪物门(8) → 空地(1)
	{ ActionType::SAY,         0, 0, (const char*)"所有牢门都已打开！" },
	{ ActionType::SET_FLAG,    2, 0, nullptr },
	{ ActionType::END,         0, 0, nullptr },
};

// ============================================================
// 事件注册表（在 init() 中统一注册）
// ============================================================
void EventManager::init()
{
	event_count_ = 0;
	std::memset(flags_, 0, sizeof(flags_));
	std::memset(kills_, 0, sizeof(kills_));

	// --- ON_TILE 事件 ---

	// 第3层 老人(151)
	addEvent({
		/* floor_         */ 3,
		/* trigger        */ EventTrigger::ON_TILE,
		/* trigger_param  */ 151,
		/* trigger_count  */ 0,
		/* condition_flag */ 1,
		/* set_flag       */ 1,
		/* action_count   */ 2,
		/* actions        */ ACT_3F_OLD_MAN,
	});

	// --- ON_KILL 事件 ---

	// 第2层：击杀 2 个中级卫兵(121) → 牢门全开
	addEvent({
		/* floor_         */ 2,
		/* trigger        */ EventTrigger::ON_KILL,
		/* trigger_param  */ 121,   // 中级卫兵
		/* trigger_count  */ 2,     // 需要杀 2 个
		/* condition_flag */ 2,     // flag 2 未设置时检查
		/* set_flag       */ 2,     // 完成后设 flag 2
		/* action_count   */ 3,
		/* actions        */ ACT_2F_PRISON_OPEN,
	});
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
	if (id >= MAX_FLAGS) return true;
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
		if (ev.trigger_param != tile_id) continue;
		if (ev.condition_flag != 0 && hasFlag(ev.condition_flag)) continue;

		executeEvent(ev, player);
		return;
	}
}

void EventManager::checkKill(uint8_t floor, uint8_t monster_id)
{
	if (floor >= 51 || monster_id >= 256) return;

	kills_[floor][monster_id]++;

	for (uint8_t i = 0; i < event_count_; i++)
	{
		const Event& ev = events_[i];
		if (ev.trigger != EventTrigger::ON_KILL) continue;
		if (ev.floor_ != floor) continue;
		if (ev.trigger_param != monster_id) continue;
		if (ev.condition_flag != 0 && hasFlag(ev.condition_flag)) continue;

		// 检查击杀数是否达标
		if (kills_[floor][monster_id] < ev.trigger_count) continue;

		// 需要 player 引用，但 kill 回调时没有上下文
		// 折中：只执行非 player 相关动作
		Player dummy;
		dummy.init();
		executeEvent(ev, dummy);
		return;
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
		if (act.param == 0)
			player.health += get_Red_Health_Potion_Value(player.floor);
		else
			player.health += get_Blue_Health_Potion_Value(player.floor);
		break;

	case ActionType::GIVE_GEM:
		if (act.param == 0)
			player.attack += get_Gem_Stone_Value(player.floor);
		else
			player.defence += get_Gem_Stone_Value(player.floor);
		break;

	case ActionType::GIVE_EQUIP: {
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

	case ActionType::REPLACE_ALL:
		// 将 ev_floor 层所有 from_id(param) 替换为 to_id(param2)
		for (uint8_t y = 0; y < 13; y++)
			for (uint8_t x = 0; x < 13; x++)
				if (map_get(ev_floor, x, y) == act.param)
					map_set(ev_floor, x, y, act.param2);
		break;

	default:
		break;
	}
}
