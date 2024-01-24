#include "event.h"

void saySomething(char * s)
{
	regionPrint(2,13,s);
	getch();
	getch();
	getch();
	getch();
	getch();
	getch();
	getch();
	regionErase(2,13,strlen(s));
}