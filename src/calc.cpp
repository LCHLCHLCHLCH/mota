#include "calc.h"

namespace {
	constexpr uint16_t RED_POTION_PER_REGION  = 50;
	constexpr uint16_t BLUE_POTION_PER_REGION = 200;

	uint8_t getRegion(uint8_t floor) {
		return (floor - 1) / 10 + 1;
	}
}

uint16_t get_Red_Health_Potion_Value(uint8_t floor) {
	return getRegion(floor) * RED_POTION_PER_REGION;
}

uint16_t get_Blue_Health_Potion_Value(uint8_t floor) {
	return getRegion(floor) * BLUE_POTION_PER_REGION;
}

uint8_t get_Gem_Stone_Value(uint8_t floor) {
	return getRegion(floor);
}
