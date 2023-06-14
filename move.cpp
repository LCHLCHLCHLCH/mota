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
	}; //��ǰ֡������ 
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
	};  //��һ֡������ 
	int speed =200; //��Ϸ�ٶ� 
	int px=2,py=2; //λ�� 
	int ch;//���� 
	int key=72;
	//��һ��ĵ�ͼ 
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

//�ƶ����ĺ��� 
	void GotoXY(int x, int y)  // �ƶ� 
{
 	HANDLE hout;  //��Ļ�ߴ� ���� 
	COORD coord={x,y};  //������� ����  ���x��  ���y�� 
	hout=GetStdHandle(STD_OUTPUT_HANDLE);  //�����Ļ�ߴ� 
	SetConsoleCursorPosition(hout,coord);  //�ƶ���� 
}

void keydown()
{
	if (_kbhit())//����û������˼����е�ĳ����
	{
		fflush(stdin);//��ջ��������ַ�

		//getch()��ȡ�������ʱ�򣬻᷵�����Σ���һ�ε��÷���0����224���ڶ��ε��÷��صĲ���ʵ��ֵ
		key = _getch();//��һ�ε��÷��صĲ���ʵ��ֵ
		key = _getch();//�ڶ��ε��÷���ʵ��ֵ
	}

	/*
	����̨���������������
	*��������72
	*��������80
	*��������75
	*��������77
	*/
	switch (key)
	{
	case 75:
		px=px-1;//����
		break;
	case 77:
		px=px+1;//����
		break;
	case 72:
		py=py-1;//����
		break;
	case 80:
		py=py+1;//����
		break;
	}
	key=0; 
	GotoXY(1,11);
	return;
}

	
	
	void init(){
	//��ʼ����ͼ 
	for (int i=0;i<10;i++){
		for(int j=0;j<10;j++)
		{
			tem[i][j]=map[i][j];
		}	
	}
}

	void update(){
		//��temд��last 
		for (int i=0;i<10;i++){
		for(int j=0;j<10;j++)
		{
			last[i][j]=tem[i][j];
		}	
	}
		//����ͼ�Լ���ɫд��tem 
		for (int i=0;i<10;i++){
			for(int j=0;j<10;j++)
			{
				 tem[i][j]=map[i][j];
			}	
		}
		tem[px][py]=2;
	}

//���Ƶ�ͼ�ĺ��� 
	void draw(){
		for(int t=0;t<10;t++){
			for(int u=0;u<10;u++){
				if(tem[t][u]!=last[t][u]){
					GotoXY(2*t,u);
					if(tem[t][u]==0)cout<<"��";
					if(tem[t][u]==1)cout<<"��";
					if(tem[t][u]==2)cout<<"��";
				}
			}
		}
	}

//���ع��	
void HideCursor(){
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(handle, &CursorInfo);//��ȡ����̨�����Ϣ
	CursorInfo.bVisible = false; //���ؿ���̨���
	SetConsoleCursorInfo(handle, &CursorInfo);//���ÿ���̨���״̬
} 

//������ 
int main()
{	
	for(int m=0;m<10;m++){
		for(int n=0;n<10;n++){
			if(tem[m][n]==0)cout<<"��";
			if(tem[m][n]==1)cout<<"��";
			if(tem[m][n]==2)cout<<"��";
		}
		cout<<endl; 
	} 

	//��Ⱦ //���ǵ�ǰ֡���������
	for(int m=0;m<10;m++){
		for(int n=0;n<10;n++){
			tem[m][n]=map[m][n];
		}
	} 
	init(); 
	
	//��Ϸ������
	while(1){
		keydown();
		update(); 
		draw(); 
		HideCursor();
		Sleep(200);
	} 
	
	GotoXY(5,3);
	cout<<"\b"<<"��";
	GotoXY(-1,0); 
//	cout<<"���w��������ˡ�������\n"; 

	system("pause");
}
