#pragma once
#include <stdint.h>
#include <vector>
#include <cstring>
#include "game/map.h"
#include "render/key.h"
#include "game/monster.h"
#include "event/dialog.h"

class EventManager;
class Backpack;

// 记事本：一条有用的对话记录
// label: 左侧列表标签（如"老人"/"商人"，显示时自动加楼层前缀）
// text:  右侧显示的对话具体内容
struct DialogueEntry {
	uint8_t floor;
	char label[16];
	char text[160];
};

enum PREDICTION
{
	DIE,
	LIVE,
	REACHABLE,
	UNREACHABLE
};

class Player
{
public:
	uint32_t health;
	uint32_t attack;
	uint32_t defence;
	uint32_t money;

	uint8_t redKey;
	uint8_t blueKey;
	uint8_t yellowKey;

	uint8_t x;
	uint8_t y;
	uint8_t floor;
	uint8_t direction = 0;  // 0=上 1=下 2=左 3=右

	uint32_t hurt = 0;

	bool     hasTeleporter = false;
	bool     hasMonsterBook = false;
	bool     hasCross = false;
	bool     hasHolyShield = false;
	bool     hasLuckyCoin = false;
	bool     hasDragonSlayer = false;
	uint8_t  maxFloorVisited = 1;

	EventManager* events = nullptr;
	Backpack*     backpack = nullptr;

	// 记事本对话记录（按时间先后，相同楼层+说话人+文本去重）
	std::vector<DialogueEntry> dialogueLog;

	void init();
	void recordDialogue(uint8_t floor, const char* label, const char* text);
	PREDICTION PredictAttack(uint8_t monster_id);
	void respondToKey(KEY key);

	void respondToMap(uint8_t floor_going, uint8_t x_going, uint8_t y_going);
	void reactToObject(uint8_t floor_going, uint8_t x_going, uint8_t y_going);
	void reactToProp(uint8_t floor_going, uint8_t x_going, uint8_t y_going);
	void reactToMonster(uint8_t floor_going, uint8_t x_going, uint8_t y_going);

	void freezeLava();

private:
	void findStair(uint8_t target_floor, uint8_t stair_id,
	               uint8_t& out_x, uint8_t& out_y);
	void teleportTo(uint8_t target_floor, uint8_t stair_id);
};

void upStair(uint8_t *Floor, uint8_t *X, uint8_t *Y);
void downStair(uint8_t *Floor, uint8_t *X, uint8_t *Y);

// 模拟战斗，返回击败怪物损失的生命值；无法击败（破不了防或战败）返回 -1
// 持有十字架时对兽人/兽人武士/吸血鬼（112/113/116）攻击力加倍
int32_t SimulateCombat(const Player& player, uint8_t monster_id);

// 同 SimulateCombat，但可指定玩家生命值（用于区域伤害后的战斗判定）
int32_t SimulateCombatHealth(const Player& player, uint32_t health, uint8_t monster_id);
