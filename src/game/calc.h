#pragma once
#include <stdint.h>

inline uint16_t get_Red_Health_Potion_Value(uint8_t floor) {
	uint8_t region = (floor - 1) / 10 + 1;
	return region * 50;
}

inline uint16_t get_Blue_Health_Potion_Value(uint8_t floor) {
	uint8_t region = (floor - 1) / 10 + 1;
	return region * 200;
}

inline uint8_t get_Gem_Stone_Value(uint8_t floor) {
	uint8_t region = (floor - 1) / 10 + 1;
	return region;
}
