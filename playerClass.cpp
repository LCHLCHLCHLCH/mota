#include <stdint.h>
#include "playerClass.h"

/**
 * @brief 初始化函数
 */
void Player::init()
{
	floor = 1;
	x = 5;
	y = 5;
	yellowKey = 0;
}

/**
 * @brief 预测假如攻击某个怪物会发生什么
 * @param Monster 要攻击的怪物
 *
 * @return PREDICTION类型的枚举
 */
PREDICTION Player::PredictAttack(Monster monster)
{
	uint32_t player_health_temp = health;
	uint32_t monster_health_temp = monster.health;

	// 攻击大于怪物的防御,无伤击败
	if (attack >= monster_health_temp)
	{
		hurt = 0;
		return LIVE;
	}
	// 防御大于怪物的攻击,无伤击败
	else if (defence >= monster.attack)
	{
		hurt = 0;
		return LIVE;
	}
	// 攻击小于怪物的防御,无法击败
	else if (attack <= monster.defence)
	{
		return DIE;
	}
	else
	{
		while (1)
		{
			monster_health_temp = monster_health_temp - (attack - monster.defence);
			if (monster_health_temp <= 0)
			{
				hurt = health - player_health_temp;
				return LIVE;
			}
			player_health_temp = player_health_temp - (monster.attack - defence);
			if (player_health_temp <= 0)
			{
				return DIE;
			}
		}
	}
}

/**
 * @brief 预测移动到某一个格子以后会发生什么
 *
 * @param Floor_going
 * @param X_going
 * @param Y_going
 *
 * @return 结果,是一个PREDICT类型的枚举
 */
PREDICTION Player::predictMove(uint8_t Floor_going, uint8_t X_going, uint8_t Y_going)
{
	uint8_t Object_Going = map[Floor_going][Y_going][X_going];
	if (Object_Going == 1)
		return REACHABLE;
	else if (Object_Going == 2)
		return UNREACHABLE;
	else if (Object_Going == 3) // 黄门
	{
		return UNREACHABLE;
	}
}

/**
 * @brief 对按键做出反应的函数
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
		X_going = x;
		Y_going = y - 1;
		break;
	case DOWN:
		// y++;
		X_going = x;
		Y_going = y + 1;
		break;
	case LEFT:
		// x--;
		X_going = x - 1;
		Y_going = y;
		break;
	case RIGHT:
		// x++;
		X_going = x + 1;
		Y_going = y;
		break;
	}

	PREDICTION prd = predictMove(floor, X_going, Y_going); // 按下按键以后,楼层不会移动
	// switch (prd)
	// {
	// case REACHABLE:
	// 	x = X_going;
	// 	y = Y_going;
	// 	break;
	// }
	if (prd == REACHABLE)
	{
		x = X_going;
		y = Y_going;
	}
}