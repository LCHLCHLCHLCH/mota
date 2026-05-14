#include "game/monster.h"
#include "game/tile_data.h"

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
 * @brief 战士
 * @note id = 119
 */
Monster Soldier = {210, 200, 65, 45};

/**
 * @brief 幽灵
 * @note id = 120
 */
Monster SlimeMan = {320, 140, 20, 30};

/**
 * @brief 中级卫兵
 * @note id = 121
 */
Monster BlueGuard = {100, 180, 110, 50};

/**
 * @brief 双手剑士
 * @note id = 122
 */
Monster Swordsman = {100, 680, 50, 55};

/**
 * @brief 魔龙
 * @note id = 123
 */
Monster MagicDragon = {1500, 600, 250, 800};

/**
 * @brief 骑士
 * @note id = 124
 */
Monster RedKnight = {160, 230, 105, 65};

/**
 * @brief 骑士队长
 * @note id = 125
 */
Monster YellowKnight = {120, 150, 50, 100};

/**
 * @brief 初级巫师
 * @note id = 126
 */
Monster BrownWizard = {220, 370, 110, 80};

/**
 * @brief 高级巫师
 * @note id = 127
 */
Monster RedWizard = {200, 380, 130, 90};

/**
 * @brief 史莱姆王
 * @note id = 128
 */
Monster SlimeLord = {360, 310, 20, 40};

/**
 * @brief 吸血蝙蝠
 * @note id = 129
 */
Monster RedBat = {200, 390, 90, 50};

/**
 * @brief 黑暗骑士
 * @note id = 130
 */
Monster DarkKnight = {180, 430, 210, 120};

/**
 * @brief 魔法警卫
 * @note id = 131
 */
Monster WhiteKing = {230, 450, 100, 100};

/**
 * @brief 高级卫兵
 * @note id = 132
 */
Monster RedGuard = {180, 460, 360, 200};

/**
 * @brief 假魔王
 * @note id = 133
 */
Monster BlackKing = {1000, 500, 0, 1000};

/**
 * @brief 假魔王(被封印)
 * @note id = 134
 */
Monster SealedBlackKing = {1000, 500, 0, 1000};

/**
 * @brief 真魔王
 * @note id = 135
 */
Monster RedKing = {8000, 5000, 1000, 500};

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
		case 119:
			a = &Soldier;
			break;
		case 120:
			a = &SlimeMan;
			break;
		case 121:
			a = &BlueGuard;
			break;
		case 122:
			a = &Swordsman;
			break;
		case 123:
			a = &MagicDragon;
			break;
		case 124:
			a = &RedKnight;
			break;
		case 125:
			a = &YellowKnight;
			break;
		case 126:
			a = &BrownWizard;
			break;
		case 127:
			a = &RedWizard;
			break;
		case 128:
			a = &SlimeLord;
			break;
		case 129:
			a = &RedBat;
			break;
		case 130:
			a = &DarkKnight;
			break;
		case 131:
			a = &WhiteKing;
			break;
		case 132:
			a = &RedGuard;
			break;
		case 133:
			a = &BlackKing;
			break;
		case 134:
			a = &SealedBlackKing;
			break;
		case 135:
			a = &RedKing;
			break;
		default:
			a = &ErrMaster;
	}

	//返回获取的怪物的地址
	return a;
}

const char* getMonsterName(uint8_t id) {
	return g_tile_defs[id].name;
}


bool isBossMonster(uint8_t id) {
	switch (id) {
		case 115:  // 巨型章鱼
		case 116:  // 吸血鬼
		case 117:  // 大法师
		case 123:  // 魔龙
		case 128:  // 史莱姆王
		case 132:  // 高级卫兵
		case 133:  // 假魔王
		case 134:  // 假魔王(封印)
		case 135:  // 真魔王
			return true;
		default:
			return false;
	}
}
