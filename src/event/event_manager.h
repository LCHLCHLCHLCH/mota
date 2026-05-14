#pragma once
#include "event_data.h"

class Player;  // forward declaration

#define MAX_FLAGS  64
#define MAX_EVENTS 64

class EventManager {
public:
	void init();
	void checkTile(uint8_t floor, uint8_t tile_id, Player& player);
	void checkClear(uint8_t floor);
	void checkGuardKill(uint8_t floor, uint8_t killed_x, uint8_t killed_y, Player& player);
	void checkAltar(uint8_t floor, Player& player);

	bool hasFlag(uint8_t id) const;
	void setFlag(uint8_t id);
	uint8_t getAltarTimes() const { return altar_times_; }
	void    setAltarTimes(uint8_t t) { altar_times_ = t; }

private:
	uint8_t flags_[MAX_FLAGS];
	uint8_t event_count_;
	Event   events_[MAX_EVENTS];
	uint8_t altar_times_;  // 全局祭坛使用次数

	static uint8_t countMonsters(uint8_t floor);
	uint16_t getAltarCost() const;

	void executeEvent(const Event& ev, Player& player);
	void executeAction(const EventAction& act, Player& player, uint8_t ev_floor);
	void addEvent(const Event& ev);
};
