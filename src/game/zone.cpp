#include "game/zone.h"
#include "game/map.h"
#include "game/monster.h"
#include "sdl_terminal.h"
#include <cstring>

// 当前楼层区域表
static int32_t g_zone_domain[13][13];   // 领域伤害（固定值，可叠加）
static bool    g_zone_clamp[13][13];    // 是否夹击格

void recomputeZoneDamage(uint8_t floor)
{
	std::memset(g_zone_domain, 0, sizeof(g_zone_domain));
	std::memset(g_zone_clamp, 0, sizeof(g_zone_clamp));

	// 领域：初级巫师(126)=100、高级巫师(127)=200，四向相邻
	for (uint8_t y = 0; y < 13; y++)
		for (uint8_t x = 0; x < 13; x++)
		{
			uint8_t t = map_get(floor, x, y);
			int32_t v = 0;
			if (t == 126) v = 100;
			else if (t == 127) v = 200;
			else continue;

			if (y > 0)  g_zone_domain[y - 1][x] += v;
			if (y < 12) g_zone_domain[y + 1][x] += v;
			if (x > 0)  g_zone_domain[y][x - 1] += v;
			if (x < 12) g_zone_domain[y][x + 1] += v;
		}

	// 夹击：魔法警卫(131)，右/下 2 格处存在同种守卫则中间格为夹击点
	for (uint8_t y = 0; y < 13; y++)
		for (uint8_t x = 0; x < 13; x++)
		{
			if (map_get(floor, x, y) != 131) continue;
			if (x + 2 < 13 && map_get(floor, x + 2, y) == 131)
				g_zone_clamp[y][x + 1] = true;
			if (y + 2 < 13 && map_get(floor, x, y + 2) == 131)
				g_zone_clamp[y + 1][x] = true;
		}
}

bool checkZoneEntry(Player& player, uint8_t floor, uint8_t x, uint8_t y, uint8_t tile)
{
	// 不可进入的格子不触发区域伤害（墙/门/岩浆/星/守卫门/NPC/身躯等）
	bool enterable = (tile == 1 || tile == 9 || tile == 10 || tile == 11) ||
	                 (tile >= 51 && tile <= 73) ||
	                 (tile >= 101 && tile <= 150);
	if (!enterable) return true;

	int32_t domain = g_zone_domain[y][x];
	bool isClamp = g_zone_clamp[y][x];
	if (domain <= 0 && !isClamp) return true;

	// 神圣盾免疫
	if (player.hasHolyShield) return true;

	// 夹击伤害（动态）：对扣除领域伤害后的血量取半
	int32_t clampDamage = 0;
	if (isClamp)
	{
		int32_t base = (int32_t)player.health - domain;
		if (base > 1) clampDamage = base / 2;
	}
	int32_t total = domain + clampDamage;

	// 阻止规则：血量为 1 不能进夹击格；血量不足以承受总伤害则阻止
	if ((isClamp && player.health <= 1) || player.health <= (uint32_t)total)
	{
		term_set_message("生命不足，无法承受伤害，无法前进！");
		return false;
	}

	// 若是怪物格：扣完区域伤害后剩余血量还要能打赢才允许
	if (tile >= 101 && tile <= 150)
	{
		int32_t postZone = (int32_t)player.health - total;
		int32_t fightDmg = SimulateCombatHealth(player, (uint32_t)postZone, tile);
		if (fightDmg < 0 || (uint32_t)fightDmg >= (uint32_t)postZone)
		{
			term_set_message("你还不能击败它！");
			return false;
		}
	}

	// 允许：扣除区域伤害
	player.health -= (uint32_t)total;
	return true;
}
