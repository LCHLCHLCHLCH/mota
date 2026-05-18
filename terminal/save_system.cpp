#include "save_system.h"
#include "game/player.h"
#include "game/map.h"
#include "event/event_manager.h"
#include "ui/backpack.h"
#include <windows.h>
#include <cstdio>
#include <cstring>

// ============================================================
// 辅助：确保 saves/ 目录存在
// ============================================================
static bool ensure_save_dir(char* path_out, size_t size) {
	char exe_path[MAX_PATH];
	GetModuleFileNameA(NULL, exe_path, MAX_PATH);
	char* last_slash = strrchr(exe_path, '\\');
	if (last_slash) *last_slash = 0;

	snprintf(path_out, size, "%s\\saves", exe_path);
	CreateDirectoryA(path_out, NULL);
	return true;
}

// ============================================================
// 保存
// ============================================================
bool save_game(const char* name, const Player& player, const EventManager& events) {
	char dir[MAX_PATH];
	ensure_save_dir(dir, sizeof(dir));

	char path[MAX_PATH];
	snprintf(path, sizeof(path), "%s\\%s.sav", dir, name);

	FILE* f = fopen(path, "w");
	if (!f) {
		printf("无法创建存档文件: %s\n", path);
		return false;
	}

	fprintf(f, "# MotaSDL Save - %s\n", name);

	// --- 玩家属性 ---
	fprintf(f, "player.health=%u\n", player.health);
	fprintf(f, "player.attack=%u\n", player.attack);
	fprintf(f, "player.defence=%u\n", player.defence);
	fprintf(f, "player.money=%u\n", player.money);
	fprintf(f, "player.yellowKey=%u\n", player.yellowKey);
	fprintf(f, "player.blueKey=%u\n", player.blueKey);
	fprintf(f, "player.redKey=%u\n", player.redKey);
	fprintf(f, "player.x=%u\n", player.x);
	fprintf(f, "player.y=%u\n", player.y);
	fprintf(f, "player.floor=%u\n", player.floor);
	fprintf(f, "player.hurt=%u\n", player.hurt);
	fprintf(f, "player.hasTeleporter=%d\n", player.hasTeleporter ? 1 : 0);
	fprintf(f, "player.maxFloorVisited=%u\n", player.maxFloorVisited);

	// --- 事件标记（每层独立） ---
	for (int fl = 0; fl < 51; fl++) {
		for (int fi = 0; fi < 16; fi++) {
			if (events.hasFlag((uint8_t)fl, (uint8_t)fi))
				fprintf(f, "event.flag.%d.%d=1\n", fl, fi);
		}
	}
	fprintf(f, "event.altar_times=%u\n", events.getAltarTimes());

	// --- 背包（存储物品 ID） ---
	if (player.backpack && !player.backpack->items.empty()) {
		for (size_t idx = 0; idx < player.backpack->items.size(); idx++) {
			fprintf(f, "backpack.item.%zu=%u\n", idx, player.backpack->items[idx].id);
		}
	}

	// --- 地图变更 ---
	fprintf(f, "# map changes: floor.x.y=value\n");
	for (int fl = 0; fl <= 50; fl++) {
		for (int y = 0; y < 13; y++) {
			for (int x = 0; x < 13; x++) {
				uint8_t cur = map_get(fl, x, y);
				uint8_t def = map_get_default(fl, x, y);
				if (cur != def) {
					fprintf(f, "map.%d.%d.%d=%u\n", fl, x, y, cur);
				}
			}
		}
	}

	fclose(f);
	printf("已保存: %s (%s)\n", name, path);
	return true;
}

// ============================================================
// 读取
// ============================================================
bool load_game(const char* name, Player& player, EventManager& events) {
	char dir[MAX_PATH];
	ensure_save_dir(dir, sizeof(dir));

	char path[MAX_PATH];
	snprintf(path, sizeof(path), "%s\\%s.sav", dir, name);

	FILE* f = fopen(path, "r");
	if (!f) {
		printf("找不到存档文件: %s\n", path);
		return false;
	}

	// 先恢复所有地图为默认状态
	for (int fl = 0; fl <= 50; fl++)
		for (int y = 0; y < 13; y++)
			for (int x = 0; x < 13; x++)
				map_set(fl, x, y, map_get_default(fl, x, y));

	// 重置玩家
	player.init();
	events.init();

	// 背包物品 ID 列表
	uint8_t bp_ids[16];
	int     bp_count = 0;

	// 逐行解析
	char line[256];
	while (fgets(line, sizeof(line), f)) {
		size_t len = strlen(line);
		while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r'))
			line[--len] = 0;
		if (len == 0 || line[0] == '#') continue;

		char key[64] = "";
		unsigned int val = 0, val2 = 0;
		uint8_t tkey = 0, tkey2 = 0;

		// player.*
		if (sscanf(line, "player.health=%u", &val) == 1) player.health = val;
		else if (sscanf(line, "player.attack=%u", &val) == 1) player.attack = val;
		else if (sscanf(line, "player.defence=%u", &val) == 1) player.defence = val;
		else if (sscanf(line, "player.money=%u", &val) == 1) player.money = val;
		else if (sscanf(line, "player.yellowKey=%u", &val) == 1) player.yellowKey = (uint8_t)val;
		else if (sscanf(line, "player.blueKey=%u", &val) == 1) player.blueKey = (uint8_t)val;
		else if (sscanf(line, "player.redKey=%u", &val) == 1) player.redKey = (uint8_t)val;
		else if (sscanf(line, "player.x=%u", &val) == 1) player.x = (uint8_t)val;
		else if (sscanf(line, "player.y=%u", &val) == 1) player.y = (uint8_t)val;
		else if (sscanf(line, "player.floor=%u", &val) == 1) player.floor = (uint8_t)val;
		else if (sscanf(line, "player.hurt=%u", &val) == 1) player.hurt = val;
		else if (sscanf(line, "player.hasTeleporter=%u", &val) == 1) player.hasTeleporter = (val != 0);
		else if (sscanf(line, "player.maxFloorVisited=%u", &val) == 1) player.maxFloorVisited = (uint8_t)val;

		// event.*
		else if (sscanf(line, "event.flag.%hhu.%hhu=%u", &tkey, &tkey2, &val) == 3) events.setFlag(tkey, tkey2);
		else if (sscanf(line, "event.altar_times=%u", &val) == 1) events.setAltarTimes((uint8_t)val);

		// backpack.*
		else if (sscanf(line, "backpack.item.%d=%u", (int*)&val, &val2) == 2) {
			if (val < 16 && bp_count < 16)
				bp_ids[bp_count++] = (uint8_t)val2;
		}

		// map.*
		else {
			unsigned int fl, x, y;
			if (sscanf(line, "map.%u.%u.%u=%u", &fl, &x, &y, &val) == 4) {
				if (fl <= 50 && x < 13 && y < 13)
					map_set((uint8_t)fl, (uint8_t)x, (uint8_t)y, (uint8_t)val);
			}
		}
	}

	// 重建背包
	if (player.backpack) {
		player.backpack->items.clear();
		for (int i = 0; i < bp_count; i++) {
			player.backpack->addItem(bp_ids[i], getItemName(bp_ids[i]));
		}
	}

	fclose(f);
	printf("已加载: %s\n", name);
	return true;
}
