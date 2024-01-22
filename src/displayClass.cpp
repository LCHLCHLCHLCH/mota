#include "displayClass.h"
#include "Cursor.h"
#include "framePrintChar.h"
#include <stdint.h>
#include "playerClass.h"

/**
 * @brief 初始化函数
 */
void Display::init(Player ply)
{
	for (uint8_t i = 0; i < 13; i++)
	{
		for (uint8_t j = 0; j < 13; j++)
		{
			last_frame[i][j] = BLANK;
		}
	}
	for (uint8_t i = 0; i < 13; i++)
	{
		for (uint8_t j = 0; j < 13; j++)
		{
			temp_frame[i][j] = map[ply.floor][i][j];
			temp_frame[ply.y][ply.x] = 255;
		}
	}
}

/**
 * @brief 将temp_frame的内容写入last_frame
 * @attention 可以优化
 */
void Display::store_frame()
{
	for (uint8_t i = 0; i < 13; i++)
	{
		for (uint8_t j = 0; j < 13; j++)
		{
			last_frame[i][j] = temp_frame[i][j];
		}
	}
}

/**
 * @brief 生成新的一帧
 * @param 提供player
 * @attention 可优化
 */
void Display::generateFrame(Player ply)
{
	for (uint8_t i = 0; i < 13; i++)
	{
		for (uint8_t j = 0; j < 13; j++)
		{
			temp_frame[i][j] = map[ply.floor][i][j];
			temp_frame[ply.y][ply.x] = 255;
		}
	}
}

/**
 * @brief 根据last_frame和temp_frame的区别进行输出
 */
void Display::printDifferentia()
{
	for (uint8_t i = 0; i < 13; i++)
	{
		for (uint8_t j = 0; j < 13; j++)
		{
			if (temp_frame[i][j] != last_frame[i][j])
			{
				gotoxy(2 * j, i);
				framePrintChar(temp_frame[i][j]);
			}
		}
	}
}