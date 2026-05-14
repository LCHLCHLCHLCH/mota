#include "game/item.h"
#include "game/tile_data.h"

const char* getItemName(uint8_t id) {
	return g_tile_defs[id].name;
}
