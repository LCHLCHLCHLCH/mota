#include<iostream>
#include<windows.h>
#include <conio.h>
using namespace std;

	int tem[10][10]={
	{1,1,1,1,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1,1,1}
	}; //当前帧的内容 
	int last[10][10]={
	{1,1,1,1,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1,1,1}
	};  //上一帧的内容 
	int speed =200; //游戏速度 
	int px=2,py=2; //位置 
	int ch;//键盘 
	int key=72;
	//第一层的地图 
	int map [10][10]={
	{1,1,1,1,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1,1,1}
	}; 

//移动光标的函数 
	void GotoXY(int x, int y)  // 移动 
{
 	HANDLE hout;  //屏幕尺寸 变量 
	COORD coord={x,y};  //光标坐标 变量  光标x标  光标y标 
	hout=GetStdHandle(STD_OUTPUT_HANDLE);  //获得屏幕尺寸 
	SetConsoleCursorPosition(hout,coord);  //移动光标 
}

void keydown()
{
	if (_kbhit())//如果用户按下了键盘中的某个键
	{
		fflush(stdin);//清空缓冲区的字符

		//getch()读取方向键的时候，会返回两次，第一次调用返回0或者224，第二次调用返回的才是实际值
		key = _getch();//第一次调用返回的不是实际值
		key = _getch();//第二次调用返回实际值
	}

	/*
	控制台按键所代表的数字
	*“↑”：72
	*“↓”：80
	*“←”：75
	*“→”：77
	*/
	switch (key)
	{
	case 75:
		px=px-1;//往左
		break;
	case 77:
		px=px+1;//往右
		break;
	case 72:
		py=py-1;//往上
		break;
	case 80:
		py=py+1;//往下
		break;
	}
	key=0; 
	GotoXY(1,11);
	return;
}

	
	
	void init(){
	//初始化地图 
	for (int i=0;i<10;i++){
		for(int j=0;j<10;j++)
		{
			tem[i][j]=map[i][j];
		}	
	}
}

	void update(){
		//将tem写入last 
		for (int i=0;i<10;i++){
		for(int j=0;j<10;j++)
		{
			last[i][j]=tem[i][j];
		}	
	}
		//将地图以及角色写入tem 
		for (int i=0;i<10;i++){
			for(int j=0;j<10;j++)
			{
				 tem[i][j]=map[i][j];
			}	
		}
		tem[px][py]=2;
	}

//绘制地图的函数 
	void draw(){
		for(int t=0;t<10;t++){
			for(int u=0;u<10;u++){
				if(tem[t][u]!=last[t][u]){
					GotoXY(2*t,u);
					if(tem[t][u]==0)cout<<"·";
					if(tem[t][u]==1)cout<<"■";
					if(tem[t][u]==2)cout<<"勇";
				}
			}
		}
	}

//隐藏光标	
void HideCursor(){
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(handle, &CursorInfo);//获取控制台光标信息
	CursorInfo.bVisible = false; //隐藏控制台光标
	SetConsoleCursorInfo(handle, &CursorInfo);//设置控制台光标状态
} 

//主函数 
int main()
{	
	for(int m=0;m<10;m++){
		for(int n=0;n<10;n++){
			if(tem[m][n]==0)cout<<"·";
			if(tem[m][n]==1)cout<<"■";
			if(tem[m][n]==2)cout<<"勇";
		}
		cout<<endl; 
	} 

	//渲染 //这是当前帧输出的内容
	for(int m=0;m<10;m++){
		for(int n=0;n<10;n++){
			tem[m][n]=map[m][n];
		}
	} 
	init(); 
	
	//游戏主函数
	while(1){
		keydown();
		update(); 
		draw(); 
		HideCursor();
		Sleep(200);
	} 
	
	GotoXY(5,3);
	cout<<"\b"<<"改";
	GotoXY(-1,0); 
//	cout<<"〓╳△▽◆★∷⊙骷武巫\n"; 

	system("pause");
}
