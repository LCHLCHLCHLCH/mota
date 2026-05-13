#include "game/item.h"

const char* getItemName(uint8_t id) {
	switch (id) {
		case 51: return "黄钥匙";
		case 52: return "蓝钥匙";
		case 53: return "红钥匙";
		case 54: return "红血瓶";
		case 55: return "蓝血瓶";
		case 56: return "红宝石";
		case 57: return "蓝宝石";
		case 58: return "铁剑";
		case 59: return "铁盾";
		case 60: return "银剑";
		case 61: return "银盾";
		case 62: return "骑士剑";
		case 63: return "骑士盾";
		case 64: return "圣剑";
		case 65: return "圣盾";
		case 66: return "神圣剑";
		case 67: return "神圣盾";
		case 68: return "楼层传送器";
		case 69: return "冰霜魔法";
		default: return "未知道具";
	}
}
