#include <stdint.h>
#include "playerClass.h"
#include "calc.h"

/**
 * @brief 初始化函数
 */
void Player::init()
{
	floor = 1;
	x = 5;
	y = 5;
	yellowKey = 0;
	blueKey = 0;
	redKey = 0;
	attack = 100;
	defence = 100;
	health = 1000;
}

/**
 * @brief 预测假如攻击某个怪物会发生什么,若能打过则更新伤害值
 * @param Monster 要攻击的怪物
 *
 * @return PREDICTION类型的枚举
 */
PREDICTION Player::PredictAttack(Monster monster)
{
	uint32_t player_health_temp = this->health;
	uint32_t monster_health_temp = monster.health;

	// 攻击大于怪物的防御,无伤击败
	if (this->attack >= monster_health_temp)
	{
		// hurt = 0;
		this->hurt = 0;
		return LIVE;
	}
	// 防御大于怪物的攻击,无伤击败
	else if (this->defence >= monster.attack)
	{
		this->hurt = 0;
		return LIVE;
	}
	// 攻击小于怪物的防御,无法击败
	else if (this->attack <= monster.defence)
	{
		return DIE;
	}
	else
	{
		while (1)
		{
			monster_health_temp = monster_health_temp - (this->attack - monster.defence);
			if (monster_health_temp <= 0)
			{
				this->hurt = health - player_health_temp;
				return LIVE;
			}
			player_health_temp = player_health_temp - (monster.attack - this->defence);
			if (player_health_temp <= 0)
			{
				return DIE;
			}
		}
	}
}

// /**
//  * @brief 预测移动到某一个格子以后会发生什么
//  *
//  * @param Floor_going
//  * @param X_going
//  * @param Y_going
//  *
//  * @return 结果,是一个PREDICT类型的枚举
//  */
// PREDICTION Player::predictMove(uint8_t Floor_going, uint8_t X_going, uint8_t Y_going)
// {
// 	uint8_t Object_Going = map[Floor_going][Y_going][X_going];
// 	if (Object_Going == 1)
// 		return REACHABLE;
// 	else if (Object_Going == 2)
// 		return UNREACHABLE;
// 	else if (Object_Going == 3) // 黄门
// 	{
// 		return UNREACHABLE;
// 	}
// 	else
// 		return UNREACHABLE;
// }

/**
 * @brief 对预定要前往的格子做出反应的函数
 */
void Player::respondToMap(uint8_t floor_going, uint8_t x_going, uint8_t y_going)
{
	//如果将要到达的格子的ID在1-50之间,就调用处理object的函数
	if(map[floor_going][y_going][x_going]>=1&&map[floor_going][y_going][x_going]<=50)reactToObject(floor_going, x_going, y_going);
	//如果将要到达的格子的ID在51-100之间,就调用处理prop的函数
	if(map[floor_going][y_going][x_going]>=51&&map[floor_going][y_going][x_going]<=100)reactToProp(floor_going, x_going, y_going);
}

/**
 * @brief 对一个怪物做出反应的函数
 */

/**
 * @brief 对一个实体做出反应的函数
 */
void Player::reactToObject(uint8_t floor_going, uint8_t x_going, uint8_t y_going)
{
	switch (map[floor_going][y_going][x_going])
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
			map[floor_going][y_going][x_going] = 1;
			yellowKey--;
		}
		break;
	case 4: // 蓝门
		if (this->blueKey > 0)
		{
			map[floor_going][y_going][x_going] = 1;
			blueKey--;
		}
		break;
	case 5: // 红门
		if (this->redKey > 0)
		{
			map[floor_going][y_going][x_going] = 1;
			redKey--;
		}
		break;
	case 6: // 岩浆块
		break;
	case 7: // 星星块
		break;
	case 8: // 怪物看护的门
		break;
	case 9: // 上行楼梯
		/*上楼的函数*/
		break;
	case 10: // 下行楼梯
		/*下楼的函数*/
		break;
	case 11:
		map[floor_going][y_going][x_going] = 1;
		break;
	default:
		break;
	}
}

/**
 * @brief 对道具做出反应的函数
 */
void Player::reactToProp(uint8_t floor_going, uint8_t x_going, uint8_t y_going)
{
	switch (map[floor_going][y_going][x_going])
	{
	case 51: // 黄钥匙
		this->x = x_going;
		this->y = y_going;
		map[floor_going][y_going][x_going] = 1;
		this->yellowKey++;
		break;
	case 52: // 蓝钥匙
		this->x = x_going;
		this->y = y_going;
		map[floor_going][y_going][x_going] = 1;
		this->blueKey++;
		break;
	case 53: // 红钥匙
		this->x = x_going;
		this->y = y_going;
		map[floor_going][y_going][x_going] = 1;
		this->redKey++;
		break;
	case 54: // 红血瓶
		this->x = x_going;
		this->y = y_going;
		map[floor_going][y_going][x_going] = 1;
		this->health = this->health + get_Red_Health_Potion_Value(this->floor);
		break;
	case 55: // 蓝血瓶
		this->x = x_going;
		this->y = y_going;
		map[floor_going][y_going][x_going] = 1;
		this->health = this->health + get_Blue_Health_Potion_Value(this->floor);
		break;
	case 56: // 红宝石
		this->x = x_going;
		this->y = y_going;
		map[floor_going][y_going][x_going] = 1;
		this->attack = this->attack + get_Gem_Stone_Value(this->floor);
		break;
	case 57: // 蓝宝石
		this->x = x_going;
		this->y = y_going;
		map[floor_going][y_going][x_going] = 1;
		this->defence = this->defence + get_Gem_Stone_Value(this->floor);
		break;
	case 58: // 铁剑
		this->x = x_going;
		this->y = y_going;
		map[floor_going][y_going][x_going] = 1;
		this->attack = this->attack + 10;
		break;
	case 59: // 铁盾
		this->x = x_going;
		this->y = y_going;
		map[floor_going][y_going][x_going] = 1;
		this->defence = this->defence + 10;
		break;
	case 60: // 银剑
		this->x = x_going;
		this->y = y_going;
		map[floor_going][y_going][x_going] = 1;
		this->attack = this->attack + 20;
		break;
	case 61: // 银盾
		this->x = x_going;
		this->y = y_going;
		map[floor_going][y_going][x_going] = 1;
		this->defence = this->defence + 20;
		break;
	case 62: // 骑士剑
		this->x = x_going;
		this->y = y_going;
		map[floor_going][y_going][x_going] = 1;
		this->attack = this->attack + 40;
		break;
	case 63: // 骑士盾
		this->x = x_going;
		this->y = y_going;
		map[floor_going][y_going][x_going] = 1;
		this->defence = this->defence + 40;
		break;
	case 64: // 圣剑
		this->x = x_going;
		this->y = y_going;
		map[floor_going][y_going][x_going] = 1;
		this->attack = this->attack + 50;
		break;
	case 65: // 圣盾
		this->x = x_going;
		this->y = y_going;
		map[floor_going][y_going][x_going] = 1;
		this->defence = this->defence + 50;
		break;
	}
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

	switch (key)
	{
	case UP:
		// y--;
		X_going = this->x;
		Y_going = this->y - 1;
		break;
	case DOWN:
		// y++;
		X_going = this->x;
		Y_going = this->y + 1;
		break;
	case LEFT:
		// x--;
		X_going = this->x - 1;
		Y_going = this->y;
		break;
	case RIGHT:
		// x++;
		X_going = this->x + 1;
		Y_going = this->y;
		break;
	}

	// PREDICTION prd = predictMove(this->floor, X_going, Y_going);
	// if (prd == REACHABLE)
	// {
	// 	this->x = X_going;
	// 	this->y = Y_going;
	// }
	respondToMap(floor, X_going, Y_going);
}