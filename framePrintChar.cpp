#include "framePrintChar.h"
#include "Cursor.h"

void framePrintChar(uint8_t ch)
{
	switch (ch)
	{
	case 1:
		printf("·");
		break;
	case 2:
		printf("■");
		break;
	case 255:
		colorPrint(BLUE, "勇");
		break;
	default:
		printf("??");
		break;
	}
}