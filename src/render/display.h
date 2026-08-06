#pragma once

#include "game/player.h"

class Display
{
public:
	void init(const Player& ply);
	void generateFrame(const Player& ply);
};

// 在怪物格子右下角绘制红/黄/绿威胁指示灯（持有怪物手册时生效）
void drawMonsterLights(const Player& ply);

// 叠加多格 Boss 的巨型字符（怪物身躯块）
void drawBossGlyphs(const Player& ply);
