#include "framePrintChar.h"
#include "Cursor.h"

void framePrintChar(uint8_t ch)
{
	uint8_t k=rand()%10;
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
		SetConsoleColor(64 | 7);
		printf("  ");
		SetColor(WHITE);
		break;
	case 7:
		SetConsoleColor(1 * 16 | 7); // 蓝底,白背景
		printf("★");
		// if(k == 1)printf("★");
		// else printf("  ");
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
	case 111:
		colorPrint(RED, (char*)"法");
		break;
	case 112:
		colorPrint(YELLOW, (char*)"兽");
		break;
	case 113:
		colorPrint(RED, (char*)"兽");
		break;
	case 114:
		colorPrint(RED, (char*)"石");
		break;
	case 115:
		colorPrint(RED, (char*)"章");
		break;
	case 116:
		colorPrint(RED, (char*)"血");
		break;
	case 117:
		colorPrint(RED, (char*)"师");
		break;
	case 118:
		colorPrint(BLUE, (char*)"鬼");
		break;
	case 119:
		colorPrint(BLUE, (char*)"战");
		break;
	case 120:
		colorPrint(GREEN, (char*)"幽");
		break;
	case 121:
		colorPrint(BLUE,(char*)"卫");
		break;
	case 122:
		colorPrint(YELLOW, (char*)"武");
		break;
	case 123:
		colorPrint(RED, (char*)"龙");
		break;
	case 124:
		colorPrint(RED, (char*)"骑");
		break;
	case 125:
		colorPrint(YELLOW, (char*)"骑");
		break;
	case 126:
		colorPrint(YELLOW, (char*)"巫");
		break;
	case 127:
		colorPrint(RED, (char*)"巫");
		break;
	case 128:
		colorPrint(GREEN, (char*)"Θ");
		break;
	case 129:
		colorPrint(RED, (char*)"蝠");
		break;
	case 130:
		colorPrint(PURPLE, (char*)"骑");
		break;
	case 131:
		colorPrint(BLUE, (char*)"警");
		break;
	case 132:
		colorPrint(RED, (char*)"卫");
		break;
	case 133:
		colorPrint(RED, (char*)"王");
		break;
	case 134:
		colorPrint(RED, (char*)"王");
		break;
	case 135:
		colorPrint(RED, (char*)"王");
		break;
	case 151:
		colorPrint(WHITE,(char*)"老");
		break;
	case 152:
		colorPrint(RED,(char*)"商");
		break;
	case 153:
		colorPrint(PURPLE,(char*)"公");
		break;
	case 154:
		colorPrint(YELLOW,(char*)"偷");
		break;
	case 155:
		colorPrint(PURPLE,(char*)"祭");
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