#include "console_cmd.h"
#include "game/player.h"
#include "game/map.h"
#include <windows.h>
#include <conio.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>

static char g_input_buf[256];
static int  g_input_len = 0;

// ============================================================
// 欢迎信息
// ============================================================
void console_welcome() {
	printf("========================================\n");
	printf("  魔塔 SDL3 - 调试控制台\n");
	printf("  输入 help 查看可用命令\n");
	printf("========================================\n\n");
}

// ============================================================
// 命令处理
// ============================================================
static void print_help() {
	printf("\n");
	printf("==== 可用命令 ====\n");
	printf("  help                     显示此帮助\n");
	printf("  set health   <value>     设置生命值\n");
	printf("  set attack   <value>     设置攻击力\n");
	printf("  set defence  <value>     设置防御力\n");
	printf("  set money    <value>     设置金币\n");
	printf("  set yellow   <value>     设置黄钥匙数量\n");
	printf("  set blue     <value>     设置蓝钥匙数量\n");
	printf("  set red      <value>     设置红钥匙数量\n");
	printf("  set floor    <value>     设置当前楼层\n");
	printf("  set x        <value>     设置玩家 X 坐标\n");
	printf("  set y        <value>     设置玩家 Y 坐标\n");
	printf("  give equip   <id>        给予装备 (58-67)\n");
	printf("  give item    <id>        给予道具 (51-68)\n");
	printf("  tpfloor      <floor>     传送到指定楼层\n");
	printf("  killall                 清除当前楼层所有怪物\n");
	printf("\n");
}

static void set_attr(Player& player, const char* attr, int value) {
	if (strcmp(attr, "health") == 0 || strcmp(attr, "hp") == 0) {
		player.health = value;
		printf("生命值已设置为 %d\n", value);
	}
	else if (strcmp(attr, "attack") == 0 || strcmp(attr, "atk") == 0) {
		player.attack = value;
		printf("攻击力已设置为 %d\n", value);
	}
	else if (strcmp(attr, "defence") == 0 || strcmp(attr, "def") == 0) {
		player.defence = value;
		printf("防御力已设置为 %d\n", value);
	}
	else if (strcmp(attr, "money") == 0 || strcmp(attr, "gold") == 0) {
		player.money = value;
		printf("金币已设置为 %d\n", value);
	}
	else if (strcmp(attr, "yellow") == 0 || strcmp(attr, "yellowkey") == 0) {
		player.yellowKey = (uint8_t)value;
		printf("黄钥匙已设置为 %d\n", value);
	}
	else if (strcmp(attr, "blue") == 0 || strcmp(attr, "bluekey") == 0) {
		player.blueKey = (uint8_t)value;
		printf("蓝钥匙已设置为 %d\n", value);
	}
	else if (strcmp(attr, "red") == 0 || strcmp(attr, "redkey") == 0) {
		player.redKey = (uint8_t)value;
		printf("红钥匙已设置为 %d\n", value);
	}
	else if (strcmp(attr, "floor") == 0) {
		if (value >= 0 && value <= 50) {
			player.floor = (uint8_t)value;
			if (player.floor > player.maxFloorVisited)
				player.maxFloorVisited = player.floor;
			printf("楼层已设置为 %d\n", value);
		} else {
			printf("楼层范围: 0-50\n");
		}
	}
	else if (strcmp(attr, "x") == 0) {
		player.x = (uint8_t)value;
		printf("X 坐标已设置为 %d\n", value);
	}
	else if (strcmp(attr, "y") == 0) {
		player.y = (uint8_t)value;
		printf("Y 坐标已设置为 %d\n", value);
	}
	else {
		printf("未知属性: %s\n", attr);
		printf("可用属性: health attack defence money yellow blue red floor x y\n");
	}
}

static void give_item(Player& player, int id) {
	printf("给予道具 ID %d\n", id);
	// 直接修改属性
	switch (id) {
		case 51: player.yellowKey++; printf("获得黄钥匙\n"); break;
		case 52: player.blueKey++;   printf("获得蓝钥匙\n"); break;
		case 53: player.redKey++;    printf("获得红钥匙\n"); break;
		case 58: player.attack += 10;  printf("攻击+10\n"); break;
		case 59: player.defence += 10; printf("防御+10\n"); break;
		case 60: player.attack += 20;  printf("攻击+20\n"); break;
		case 61: player.defence += 20; printf("防御+20\n"); break;
		case 62: player.attack += 40;  printf("攻击+40\n"); break;
		case 63: player.defence += 40; printf("防御+40\n"); break;
		case 64: player.attack += 50;  printf("攻击+50\n"); break;
		case 65: player.defence += 50; printf("防御+50\n"); break;
		case 66: player.attack += 100;  printf("攻击+100\n"); break;
		case 67: player.defence += 100; printf("防御+100\n"); break;
		case 68: player.hasTeleporter = true; printf("获得楼层传送器\n"); break;
		default: printf("未知道具 ID\n"); break;
	}
}

static void process_command(const char* cmd, Player& player) {
	// 跳过前导空白
	while (*cmd == ' ' || *cmd == '\t') cmd++;
	if (*cmd == 0) return;

	char word1[32] = "", word2[32] = "", word3[32] = "";
	sscanf(cmd, "%31s %31s %31s", word1, word2, word3);

	if (strcmp(word1, "help") == 0 || strcmp(word1, "h") == 0 || strcmp(word1, "?") == 0) {
		print_help();
	}
	else if (strcmp(word1, "set") == 0) {
		if (word2[0] == 0) {
			printf("用法: set <属性> <值>\n");
		} else {
			set_attr(player, word2, atoi(word3));
		}
	}
	else if (strcmp(word1, "give") == 0) {
		if (strcmp(word2, "equip") == 0 || strcmp(word2, "item") == 0) {
			give_item(player, atoi(word3));
		} else {
			printf("用法: give equip <id>  或  give item <id>\n");
		}
	}
	else if (strcmp(word1, "tpfloor") == 0 || strcmp(word1, "tp") == 0) {
		int f = atoi(word2);
		if (f >= 0 && f <= 50) {
			player.floor = (uint8_t)f;
			if (player.floor > player.maxFloorVisited)
				player.maxFloorVisited = player.floor;
			printf("已传送到楼层 %d\n", f);
		} else {
			printf("楼层范围: 0-50\n");
		}
	}
	else if (strcmp(word1, "killall") == 0) {
		int count = 0;
		for (uint8_t y = 0; y < 13; y++)
			for (uint8_t x = 0; x < 13; x++) {
				uint8_t t = map_get(player.floor, x, y);
				if (t >= 101 && t <= 150) {
					map_set(player.floor, x, y, 1);
					count++;
				}
			}
		printf("已清除 %d 个怪物\n", count);
	}
	else if (strcmp(word1, "info") == 0 || strcmp(word1, "status") == 0) {
		printf("\n==== 玩家状态 ====\n");
		printf("楼层: %d   坐标: (%d, %d)\n", player.floor, player.x, player.y);
		printf("生命: %d\n", player.health);
		printf("攻击: %d    防御: %d\n", player.attack, player.defence);
		printf("金币: %d\n", player.money);
		printf("黄钥匙: %d  蓝钥匙: %d  红钥匙: %d\n",
			player.yellowKey, player.blueKey, player.redKey);
		printf("传送器: %s\n", player.hasTeleporter ? "有" : "无");
		printf("\n");
	}
	else {
		printf("未知命令: %s (输入 help 查看帮助)\n", word1);
	}
}

// ============================================================
// 非阻塞轮询
// ============================================================
void console_poll(Player& player) {
	while (_kbhit()) {
		char ch = (char)_getch();
		if (ch == '\r' || ch == '\n') {
			if (g_input_len > 0) {
				g_input_buf[g_input_len] = 0;
				printf("\n");
				process_command(g_input_buf, player);
				g_input_len = 0;
				printf("\n> ");
				fflush(stdout);
			}
		}
		else if (ch == '\b') {
			if (g_input_len > 0) {
				g_input_len--;
				printf("\b \b");
				fflush(stdout);
			}
		}
		else if (ch >= ' ' && g_input_len < 255) {
			g_input_buf[g_input_len++] = ch;
			putchar(ch);
			fflush(stdout);
		}
	}
}
