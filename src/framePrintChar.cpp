#include "framePrintChar.h"
#include "Cursor.h"

void framePrintChar(uint8_t ch)
{
	switch (ch)
	{
	// 实体的部分
	case 1:
		printf("·");
		break;
	case 2:
		printf("■");
		break;
	case 3:
		colorPrint(YELLOW, (char *)"〓");
		break;
	case 4:
		colorPrint(BLUE, (char *)"〓");
		break;
	case 5:
		colorPrint(RED, (char *)"〓");
		break;
	case 6:
		SetConsoleColor(64);
		printf("  ");
		SetColor(WHITE);
		break;
	case 7:
		SetConsoleColor(1 * 16 | 7); // 蓝底,白背景
		printf("★");
		SetColor(WHITE);
		break;
	case 8:
		printf("〓");
		break;
	case 9:
		printf("△");
		break;
	case 10:
		printf("▽");
		break;
	case 11:
		printf("■");
		break;
	// 道具
	case 51:
		colorPrint(YELLOW, (char *)"钥");
		break;
	case 52:
		colorPrint(BLUE, (char *)"钥");
		break;
	case 53:
		colorPrint(RED, (char *)"钥");
		break;
	case 54:
		colorPrint(RED, (char *)"★");
		break;
	case 55:
		colorPrint(BLUE, (char *)"★");
		break;
	case 56:
		colorPrint(RED, (char *)"◆");
		break;
	case 57:
		colorPrint(BLUE, (char *)"◆");
		break;
	case 58:
		colorPrint(GREY, (char *)"剑");
		break;
	case 59:
		colorPrint(GREY, (char *)"盾");
		break;
	case 60:
		colorPrint(WHITE, (char *)"剑");
		break;
	case 61:
		colorPrint(WHITE, (char *)"盾");
		break;
	case 62:
		colorPrint(YELLOW, (char *)"剑");
		break;
	case 63:
		colorPrint(YELLOW, (char *)"盾");
		break;
	case 64:
		colorPrint(LIGHT_GREEN, (char *)"剑");
		break;
	case 65:
		colorPrint(LIGHT_GREEN, (char *)"盾");
		break;
	case 66:
		colorPrint(RED, (char *)"剑");
		break;
	case 67:
		colorPrint(RED, (char *)"盾");
		break;
	// 这一区域是怪物
	case 101:
		colorPrint(GREEN, (char *)"⊙");
		break;
	case 102:
		colorPrint(RED, (char *)"⊙");
		break;
	case 103:
		colorPrint(PURPLE, (char *)"蝠");
		break;
	case 104:
		colorPrint(PURPLE, (char *)"法");
		break;
	case 105:
		colorPrint(WHITE, (char *)"骷");
		break;
	case 106:
		colorPrint(BLUE, (char *)"骷");
		break;
	case 107:
		colorPrint(YELLOW, (char *)"卫");
		break;
	case 108:
		colorPrint(RED, (char *)"骷");
		break;
	case 109:
		colorPrint(PURPLE, (char *)"⊙");
		break;
	case 110:
		colorPrint(RED, (char *)"蝠");
		break;
	// player
	case 255:
		colorPrint(BLUE, (char *)"勇");
		break;
	// 显示错误
	default:
		printf("？");
		break;
	}
}