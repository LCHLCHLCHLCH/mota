#include "event/event_manager.h"
#include "playerClass.h"
#include "dialog.h"
#include "map.h"
#include <Cursor.h>
#include <cstring>
#include <cstdio>

// ============================================================
// 事件动作数据
// ============================================================

static const char* TEXT_3F_OLD_MAN =
	"我可以给你一本怪物手册，你可以用它预测该楼层各怪物对你造成的伤害。";

static const EventAction ACT_3F_OLD_MAN[] = {
	{ ActionType::SAY,      0, 0, TEXT_3F_OLD_MAN },
	{ ActionType::SET_FLAG, 1, 0, nullptr },
	{ ActionType::END,      0, 0, nullptr },
};

static const char* TEXT_4F_OLD_MAN =
	"有些门不能用钥匙打开，只有当你击败它的守卫后才会自动打开。";

static const EventAction ACT_4F_OLD_MAN[] = {
	{ ActionType::SAY,      0, 0, TEXT_4F_OLD_MAN },
	{ ActionType::SET_FLAG, 3, 0, nullptr },
	{ ActionType::END,      0, 0, nullptr },
};

static const char* TEXT_2F_PRISON =
	"所有牢门都已打开！";

static const EventAction ACT_2F_PRISON_OPEN[] = {
	{ ActionType::REPLACE_ALL, 8, 1, nullptr },
	{ ActionType::SAY,         0, 0, TEXT_2F_PRISON },
	{ ActionType::SET_FLAG,    2, 0, nullptr },
	{ ActionType::END,         0, 0, nullptr },
};

// ============================================================
// EventManager
// ============================================================
void EventManager::init()
{
	event_count_ = 0;
	altar_times_ = 0;
	std::memset(flags_, 0, sizeof(flags_));

	// --- ON_TILE ---
	addEvent({ 3, EventTrigger::ON_TILE, 151, 1, 1, 2, ACT_3F_OLD_MAN });
	addEvent({ 4, EventTrigger::ON_TILE, 151, 3, 3, 2, ACT_4F_OLD_MAN });

	// --- ON_CLEAR ---
	// 第2层：当该层怪物全部死亡，打开所有怪物看护的门
	addEvent({ 2, EventTrigger::ON_CLEAR, 0, 2, 2, 3, ACT_2F_PRISON_OPEN });
}

void EventManager::addEvent(const Event& ev)
{
	if (event_count_ < MAX_EVENTS)
		events_[event_count_++] = ev;
}

void EventManager::setFlag(uint8_t id)
{
	if (id < MAX_FLAGS) flags_[id] = 1;
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

void EventManager::checkClear(uint8_t floor)
{
	// 先检查该层是否还有活着的怪物
	if (countMonsters(floor) > 0) return;

	for (uint8_t i = 0; i < event_count_; i++)
	{
		const Event& ev = events_[i];
		if (ev.trigger != EventTrigger::ON_CLEAR) continue;
		if (ev.floor_ != floor) continue;
		if (ev.condition_flag != 0 && hasFlag(ev.condition_flag)) continue;

		Player dummy;
		dummy.init();
		executeEvent(ev, dummy);
		return;
	}
}

uint8_t EventManager::countMonsters(uint8_t floor)
{
	uint8_t count = 0;
	for (uint8_t y = 0; y < 13; y++)
		for (uint8_t x = 0; x < 13; x++)
		{
			uint8_t t = map_get(floor, x, y);
			if (t >= 101 && t <= 150)
				count++;
		}
	return count;
}

// ============================================================
// 动作执行
// ============================================================
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
		for (uint8_t y = 0; y < 13; y++)
			for (uint8_t x = 0; x < 13; x++)
				if (map_get(ev_floor, x, y) == act.param)
					map_set(ev_floor, x, y, act.param2);
		break;

	default:
		break;
	}
}

// ============================================================
// 祭坛系统（数据来源: 51/events.js "商店"）
// ============================================================
uint16_t EventManager::getAltarCost() const
{
	// 20 + 10 * (times + 1) * times
	uint16_t t = altar_times_;
	return 20 + 10 * (t + 1) * t;
}

void EventManager::checkAltar(uint8_t floor, Player& player)
{
	uint8_t  ratio = (floor - 1) / 10 + 1;   // 区域 1-5
	uint16_t cost  = getAltarCost();
	uint16_t atk_gain = 2 * ratio;
	uint16_t def_gain = 4 * ratio;
	uint16_t hp_gain  = 100 * (altar_times_ + 1);

	char text_buf[64];
	char choice_atk[32], choice_def[32], choice_hp[32], choice_leave[8];
	snprintf(text_buf, sizeof(text_buf),
		"供奉%d金币，便可以增加你的力量，你想要什么呢……", cost);
	snprintf(choice_hp,  sizeof(choice_hp),  "生命+%d", hp_gain);
	snprintf(choice_atk, sizeof(choice_atk), "攻击+%d", atk_gain);
	snprintf(choice_def, sizeof(choice_def), "防御+%d", def_gain);
	snprintf(choice_leave, sizeof(choice_leave), "离开");

	// 先显示提示
	saySomething(text_buf);

	char* list[4] = { choice_hp, choice_atk, choice_def, choice_leave };
	uint8_t choice = chooseFromSomething(4, list);

	if (choice == 3 || choice == 255)
		return;  // 离开或取消

	if (player.money < cost)
	{
		saySomething((char*)"你的金币不足，无法供奉！");
		return;
	}

	player.money -= cost;

	switch (choice)
	{
	case 0: player.health += hp_gain;  break;
	case 1: player.attack += atk_gain; break;
	case 2: player.defence += def_gain; break;
	}

	altar_times_++;
	drainInput();
}
