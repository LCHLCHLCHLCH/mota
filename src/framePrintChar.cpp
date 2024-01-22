#include "framePrintChar.h"
#include "Cursor.h"

void framePrintChar(uint8_t ch)
{
	switch (ch)
	{
	//实体的部分
	case 1:
		printf("·");
		break;
	case 2:
		printf("■");
		break;
	case 3:
		colorPrint(YELLOW,(char*)"〓");
		break;
	case 4:
		colorPrint(BLUE, (char*)"〓");
		break;
	case 5:
		colorPrint(RED, (char*)"〓");
		break;
	case 6:
		SetConsoleColor(64);
		printf("  ");
		SetColor(WHITE);
		break;
	case 7:
		SetConsoleColor(1*16|7);//蓝底,白背景
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
	//道具
	case 51:
		colorPrint(YELLOW, (char*)"钥");
	case 52:
		colorPrint(BLUE, (char*)"钥");
	case 53:
		colorPrint(RED, (char*)"钥");
	case 54:
		colorPrint(RED, (char*)"★");
	case 55:
		colorPrint(BLUE, (char*)"★");
	case 56:
		colorPrint(RED, (char*)"◆");
	case 57:
		colorPrint(BLUE, (char*)"◆");
	case 58:
		colorPrint(WHITE, (char*)"剑");
	//player
	case 255:
		colorPrint(BLUE, (char*)"勇");
		break;
	//显示错误
	default:
		printf("？");
		break;
	}
}