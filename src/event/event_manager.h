#pragma once
#include <stdint.h>

class Player;

#define MAX_FLOORS 51
#define MAX_FLAGS  16

class EventManager {
public:
	void init();
	void checkClear(uint8_t floor);
	// 通用 on_tile 钩子：如果该坐标有 Lua 事件则执行并返回 true
	bool tryHandleTile(uint8_t floor, uint8_t x, uint8_t y, uint8_t tile_id, Player& player);
	void checkGuardKill(uint8_t floor, uint8_t killed_x, uint8_t killed_y, Player& player);
	// 每次移动后的 Lua 钩子（楼层文件可定义 on_step）
	void callOnStep(uint8_t floor, Player& player);
	bool hasFlag(uint8_t floor, uint8_t id) const;
	void setFlag(uint8_t floor, uint8_t id);
	uint8_t  getAltarTimes() const { return altar_times_; }
	void     setAltarTimes(uint8_t t) { altar_times_ = t; }

private:
	uint8_t flags_[MAX_FLOORS][MAX_FLAGS];
	uint8_t altar_times_;

	static uint8_t countMonsters(uint8_t floor);
};
