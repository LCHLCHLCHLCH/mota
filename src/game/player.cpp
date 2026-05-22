#include <stdint.h>
#include "game/player.h"
#include "game/monster.h"
#include "event/event_manager.h"
#include "script/lua_bridge.h"
#include <render/cursor.h>
#include "ui/backpack.h"

/**
 * @brief 初始化函数
 */
void Player::init()
{
	floor = 1;
	x = 6;
	y = 11;
	money = 100000;
	yellowKey = 100;
	blueKey = 100;
	redKey = 100;
	attack = 1000;
	defence = 1000;
	health = 10000;
}

/**
 * @brief 预测假如攻击某个怪物会发生什么,若能打过则更新伤害值
 * @param Monster 要攻击的怪物
 *
 * @return LIVE:能打过 DIE:打不过
 */
PREDICTION Player::PredictAttack(Monster monster)
{
	int32_t player_health_temp = this->health;
	int32_t monster_health_temp = monster.health;
	int32_t damage_PlayerToMonster = this->attack - monster.defence;
	int32_t damage_MonsterToPlayer = monster.attack - this->defence;

	if (damage_MonsterToPlayer < 0)
		damage_MonsterToPlayer = 0;
	if (damage_PlayerToMonster < 0)
		damage_PlayerToMonster = 0;

	// 攻击小于怪物的防御,无法击败
	if (damage_PlayerToMonster == 0)
		return DIE;
	else
	{
		while (1)
		{
			// 玩家攻击阶段
			monster_health_temp = monster_health_temp - damage_PlayerToMonster;
			if (monster_health_temp <= 0)
			{
				this->hurt = this->health - player_health_temp;
				return LIVE;
			}
			// 怪物攻击阶段
			player_health_temp = player_health_temp - damage_MonsterToPlayer;
			if (player_health_temp <= 0)
			{
				return DIE;
			}
		}
	}
}

/**
 * @brief 对预定要前往的格子做出反应的函数
 */
void Player::respondToMap(uint8_t floor_going, uint8_t x_going, uint8_t y_going)
{
	uint8_t tile = map_get(floor_going, x_going, y_going);

	// debug 模式：无视碰撞、事件、怪物，纯移动，显示坐标
	if (is_debug()) {
		this->x = x_going;
		this->y = y_going;
		char buf[32];
		snprintf(buf, sizeof(buf), "DEBUG 楼层:%d 坐标:(%d,%d)", this->floor, this->x, this->y);
		term_set_message(buf);
		return;
	}

	// 通用 Lua on_tile 钩子：有事件走 Lua，跳过 C++ 默认行为
	if (this->events && this->events->tryHandleTile(floor_going, x_going, y_going, tile, *this))
		return;

	switch (tile_category(tile)) {
	case TILE_OBJECT:
		reactToObject(floor_going, x_going, y_going);
		break;
	case TILE_PROP:
		reactToProp(floor_going, x_going, y_going);
		break;
	case TILE_MONSTER:
		reactToMonster(floor_going, x_going, y_going);
		break;
	case TILE_NPC:
		// 全部交由 Lua on_tile 事件处理，C++ 无默认行为
		break;
	default:
		break;
	}
}

/**
 * @brief 对一个怪物做出反应的函数
 */
void Player::reactToMonster(uint8_t floor_going, uint8_t x_going, uint8_t y_going)
{
	uint8_t id = map_get(floor_going, x_going, y_going);
	Monster *m = getMonsterType(id);
	PREDICTION prd = this->PredictAttack(*m);
	if (prd == LIVE)
	{
		// 移动
		this->x = x_going;
		this->y = y_going;
		// 清除怪物
		map_kill_monster(floor_going, x_going, y_going, this);
		// 受到伤害
		this->health = this->health - this->hurt;
		// 消息
		{
			char _m[64];
			if (this->hurt > 0)
				snprintf(_m, sizeof(_m), "击败%s，损失%d生命，获得%d金币",
				getMonsterName(id), (int)this->hurt, (int)m->money);
			else
				snprintf(_m, sizeof(_m), "击败%s，获得%d金币",
				getMonsterName(id), (int)m->money);
			term_set_message(_m);
		}
		if (this->events != nullptr)
			this->events->checkClear(floor_going);
	}
	else
	{
		term_set_message("你还不能击败它！");
	}

}

/**
 * @brief 对一个实体做出反应的函数
 */
void Player::reactToObject(uint8_t floor_going, uint8_t x_going, uint8_t y_going)
{
	switch (map_get(floor_going, x_going, y_going))
	{
	case 1: // 空地
		this->x = x_going;
		this->y = y_going;
		break;
	case 2: // 墙
		break;
	case 3: // 黄门
		if (this->yellowKey > 0)
		{
			map_set(floor_going, x_going, y_going, 1);
			yellowKey--;
			term_set_message("使用黄钥匙打开了门。");
		}
		break;
	case 4: // 蓝门
		if (this->blueKey > 0)
		{
			map_set(floor_going, x_going, y_going, 1);
			blueKey--;
			term_set_message("使用蓝钥匙打开了门。");
		}
		break;
	case 5: // 红门
		if (this->redKey > 0)
		{
			map_set(floor_going, x_going, y_going, 1);
			redKey--;
			term_set_message("使用红钥匙打开了门。");
		}
		break;
	case 6: // 岩浆块
		break;
	case 7: // 星星块
		break;
	case 8: // 怪物看护的门
		break;
	case 9: // 上行楼梯
		this->x = x_going;
		this->y = y_going;
		upStair(&(this->floor), &(this->x), &(this->y));
		if (this->floor > maxFloorVisited)
			maxFloorVisited = this->floor;
		break;
	case 10: // 下行楼梯
		this->x = x_going;
		this->y = y_going;
		downStair(&(this->floor), &(this->x), &(this->y));
		break;
	case 11:
		map_set(floor_going, x_going, y_going, 1);
		break;
	default:
		this->x = x_going;
		this->y = y_going;
		break;
	}
}

/**
 * @brief 对道具做出反应的函数
 */
void Player::reactToProp(uint8_t floor_going, uint8_t x_going, uint8_t y_going)
{
	uint8_t tile_id = map_get(floor_going, x_going, y_going);
	// 委托 Lua items 表：tile_id → item_id → on_acquire
	if (lua_item_on_pickup(tile_id)) {
		this->x = x_going;
		this->y = y_going;
		map_set(floor_going, x_going, y_going, 1);
	}
}

/**
 * @brief 冰冻的方法
 */
void Player::freezeLava()
{
	uint8_t target_x;
	uint8_t target_y;

	target_x = this->x + 1;
	target_y = this->y;
	if (map_get(this->floor, target_x, target_y) == 6)
		map_set(this->floor, target_x, target_y, 1);

	target_x = this->x - 1;
	target_y = this->y;
	if (map_get(this->floor, target_x, target_y) == 6)
		map_set(this->floor, target_x, target_y, 1);

	target_x = this->x;
	target_y = this->y + 1;
	if (map_get(this->floor, target_x, target_y) == 6)
		map_set(this->floor, target_x, target_y, 1);

	target_x = this->x;
	target_y = this->y - 1;
	if (map_get(this->floor, target_x, target_y) == 6)
		map_set(this->floor, target_x, target_y, 1);
}

/**
 * @brief 对按键做出反应的函数
 * @note 执行流程:按下按键以后,设定一个目标移动位置。然后对目标位置执行反应函数。在反应函数里面执行移动的动作。
 */
void Player::respondToKey(KEY key)
{
	uint8_t X_going;
	uint8_t Y_going;
	uint8_t Floor_going;

	uint8_t move_flag = 1; // 这个变量标志着角色移动过了

	switch (key)
	{
	case UP:
		this->direction = 0;
		term_clear_message();
		X_going = this->x;
		Y_going = this->y - 1;
		respondToMap(floor, X_going, Y_going);
		break;
	case DOWN:
		this->direction = 1;
		term_clear_message();
		X_going = this->x;
		Y_going = this->y + 1;
		respondToMap(floor, X_going, Y_going);
		break;
	case LEFT:
		this->direction = 2;
		term_clear_message();
		X_going = this->x - 1;
		Y_going = this->y;
		respondToMap(floor, X_going, Y_going);
		break;
	case RIGHT:
		this->direction = 3;
		term_clear_message();
		X_going = this->x + 1;
		Y_going = this->y;
		respondToMap(floor, X_going, Y_going);
		break;
	case KEY_V:
		// 冰冻
		freezeLava();
		break;
	case KEY_Q:
		// 传送：向下到达过的楼层
		if (hasTeleporter && this->floor > 0)
			teleportTo(this->floor - 1, 9);
		break;
	case KEY_E:
		// 传送：向上到达过的楼层
		if (hasTeleporter && this->floor < maxFloorVisited)
			teleportTo(this->floor + 1, 10);
		break;
	case KEY_1:
		// 调试：下楼
		this->floor--;
		break;
	case KEY_2:
		// 调试：上楼
		this->floor++;
		break;
		break;
	case NOTHING:
		// 按下了不相关的键
		break;
	}
}
/**
 * @brief 在目标楼层寻找指定楼梯，找到则返回 true 并设置坐标
 */
static bool findStairOnFloor(uint8_t floor, uint8_t stair_id,
                             uint8_t& out_x, uint8_t& out_y)
{
	for (uint8_t y = 0; y < 13; y++)
		for (uint8_t x = 0; x < 13; x++)
			if (map_get(floor, x, y) == stair_id)
			{
				out_x = x;
				out_y = y;
				return true;
			}
	return false;
}

/**
 * @brief 处理上楼梯：前往上一层的下行楼梯处
 * @note 第44层为隐藏楼层，43→45 直接相连
 * @note 若上一层无下行楼梯则保持坐标不变
 */
void upStair(uint8_t *Floor, uint8_t *X, uint8_t *Y)
{
	uint8_t target = (*Floor) + 1;
	if (target == 44) target = 45;  // 跳过隐藏楼层

	uint8_t nx, ny;
	if (findStairOnFloor(target, 10, nx, ny))
	{
		*X = nx;
		*Y = ny;
	}
	*Floor = target;
}

/**
 * @brief 处理下楼梯：前往下一层的上行楼梯处
 * @note 第44层为隐藏楼层，45→43 直接相连
 * @note 若下一层无上行楼梯则保持坐标不变
 */
void downStair(uint8_t *Floor, uint8_t *X, uint8_t *Y)
{
	uint8_t target = (*Floor) - 1;
	if (target == 44) target = 43;  // 跳过隐藏楼层

	uint8_t nx, ny;
	if (findStairOnFloor(target, 9, nx, ny))
	{
		*X = nx;
		*Y = ny;
	}
	*Floor = target;
}

void Player::findStair(uint8_t target_floor, uint8_t stair_id,
                       uint8_t& out_x, uint8_t& out_y)
{
	if (!findStairOnFloor(target_floor, stair_id, out_x, out_y))
	{
		out_x = 1;
		out_y = 1;
	}
}

void Player::teleportTo(uint8_t target_floor, uint8_t stair_id)
{
	this->floor = target_floor;
	if (!findStairOnFloor(target_floor, stair_id, this->x, this->y))
	{
		this->x = 1;
		this->y = 1;
	}
}
