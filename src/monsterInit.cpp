#include "monsterClass.h"

/**
 * @brief 绿史莱姆
 * @note id = 101
 */
Monster GreenSlime = {35, 18, 1, 1};

/**
 * @brief 红史莱姆
 * @note id = 102
 */
Monster RedSlime = {45, 20, 2, 2};

/**
 * @brief 小蝙蝠
 * @note id = 103
 */
Monster Bat = {35, 38, 3, 3};

/**
 * @brief 初级法师
 * @note id = 104
 */
Monster Priest = {60, 32, 8, 5};

/**
 * @brief 骷髅
 * @note id = 105
 */
Monster SkeletonC = {50, 42, 6, 6};

/**
 * @brief 骷髅士兵
 * @note id = 106
 */
Monster SkeletonB = {55, 52, 12, 8};

/**
 * @brief 初级卫兵
 * @note id = 107
 */
Monster GateKeeperC = {50, 48, 22, 12};

/**
 * @brief 骷髅队长
 * @note id = 108
 */
Monster SkeletonA = {100, 65, 15, 30};

/**
 * @brief 大史莱姆
 * @note id = 109
 */
Monster BigSlime = {130, 60, 3, 8};

/**
 * @brief 大蝙蝠
 * @note id = 110
 */
Monster BigBat = {60, 100, 8, 12};

/**
 * @brief 高级法师
 * @note id = 111
 */
Monster SuperionPriest = {100, 95, 30, 22};

/**
 * @brief 兽人
 * @note id = 112
 */
Monster Zombie = {260, 85, 5, 18};

/**
 * @brief 兽人武士
 * @note id = 113
 */
Monster ZombieKnight = {320, 120, 15, 30};

/**
 * @brief 石头人
 * @note id = 114
 */
Monster Rock = {20, 100, 68, 28};

/**
 * @brief 巨型章鱼
 * @note id = 115
 */
Monster GiantOctopus = {1200, 180, 20, 100};

/**
 * @brief 吸血鬼
 * @note id = 116
 */
Monster Vampire = {444, 199, 66, 144};

/**
 * @brief 大法师
 * @note id = 117
 */
Monster GreatMagicMaster = {4500, 560, 310, 1000};

/**
 * @brief 鬼战士
 * @note id = 118
 */
Monster GhostSoldier = {220, 180, 30, 35};

/**
 * @brief 要是能碰到这个怪物说明出问题了
 * @note id = 150
 */
Monster ErrMaster = {UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX};

/**
 * @brief 根据怪物id获取怪物的类
 */
Monster * getMonsterType(uint8_t id)
{
	Monster * a; // 要攻击的怪物的地址
	switch (id)
	{
		case 101:
			a = &GreenSlime;
			break;
		case 102:
			a = &RedSlime;
			break;
		case 103:
			a = &Bat;
			break;
		case 104:
			a = &Priest;
			break;
		case 105:
			a = &SkeletonC;
			break;
		case 106:
			a = &SkeletonB;
			break;
		case 107:
			a = &GateKeeperC;
			break;
		case 108:
			a = &SkeletonA;
			break;
		case 109:
			a = &BigSlime;
			break;
		case 110:
			a = &BigBat;
			break;
		case 111:
			a = &SuperionPriest;
			break;
		case 112:
			a = &Zombie;
			break;
		case 113:
			a = &ZombieKnight;
			break;
		case 114:
			a = &Rock;
			break;
		case 115:
			a = &GiantOctopus;
			break;
		case 116:
			a = &Vampire;
			break;
		case 117:
			a = &GreatMagicMaster;
			break;
		case 118:
			a = &GhostSoldier;
			break;
		default:
			a = &ErrMaster;
	}

	//返回获取的怪物的地址
	return a;
}
