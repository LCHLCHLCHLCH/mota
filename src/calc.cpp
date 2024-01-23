#include "calc.h"

/**
 * @brief 根据楼层计算红血瓶的值
 */
uint16_t get_Red_Health_Potion_Value(uint8_t floor)
{
	uint8_t region = (floor-1) / 10 + 1;
	return (region*50);
}

/**
 * @brief 根据楼层计算蓝血瓶的值
 */
uint16_t get_Blue_Health_Potion_Value(uint8_t floor)
{
	uint8_t region = (floor-1) / 10 + 1;
	return (region*200);
}

/**
 * @brief 根据楼层计算宝石的值
 */
uint8_t get_Gem_Stone_Value(uint8_t floor)
{
	uint8_t region = (floor-1) / 10 + 1;
	return (region);
}
