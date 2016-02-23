/********************************* �����к�̫�������޹�˾ *******************************
* ʵ �� �� ������������Զ�����ʵ��
* ʵ��˵�� ����������ģ����ڶ����ת���ϣ�ͨ��ת�����������ȡǰ��������Լ��ұߵ��ϰ������
* ʵ��ƽ̨ ������7�š�51��Ƭ����Сϵͳ
* ���ӷ�ʽ ����ο�interface.h�ļ�
* ע    �� �������ת�ǶȽ��鲻Ҫʹ��0���180�㣬������΢���м�һ�㣬����ѡȡ20���160 �㣬���������ת����ȷ
* ��    �� ����̫���Ӳ�Ʒ�з���    QQ ��1909197536
* ��    �� ��http://shop120013844.taobao.com/
****************************************************************************************/

#include <reg52.h>
#include <intrins.h>
#include <stdio.h>
#include "LCD1602.h"
#include "interface.h"
#include "motor.h"
#include "UltrasonicCtrol.h"

//ȫ�ֱ�������
unsigned int speed_count=0;//ռ�ձȼ����� 50��һ����
char front_left_speed_duty=SPEED_DUTY;
char front_right_speed_duty=SPEED_DUTY;
char behind_left_speed_duty=SPEED_DUTY;
char behind_right_speed_duty=SPEED_DUTY;

unsigned char tick_5ms = 0;//5ms����������Ϊ�������Ļ�������
unsigned char tick_1ms = 0;//1ms����������Ϊ����Ļ���������

char ctrl_comm;//����ָ��
char ctrl_comm_last = COMM_STOP;//��һ�ε�ָ��
//unsigned char continue_time = 0;

unsigned char duoji_count=0;
unsigned char zhuanjiao = 11;

/*******************************************************************************
* �� �� �� ��Delayms
* �������� ��ʵ�� ms������ʱ
* ��    �� ��ms
* ��    �� ����
*******************************************************************************/
void Delayms(unsigned int ms)
{
	unsigned int i,j;
	for(i=0;i<ms;i++)
	#if FOSC == 11059200L
		for(j=0;j<114;j++);
	#elif FOSC == 12000000L
	  for(j=0;j<123;j++);
	#elif FOSC == 24000000L
		for(j=0;j<249;j++);
	#elif FOSC == 48000000L
		for(j=0;j<715;j++);
	#else
		for(j=0;j<114;j++);
	#endif
}

/*******************************************************************************
* �� �� �� ��Timer0Init
* �������� ����ʱ��0��ʼ��
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void Timer0Init()
{
	TMOD|=0x01; //���ö�ʱ��0������ʽΪ1
	TH0=(65536-(FOSC/12*TIME_US)/1000000)/256;
	TL0=(65536-(FOSC/12*TIME_US)/1000000)%256;
	ET0=1; //������ʱ��0�ж�
	TR0=1;	//������ʱ��	
	EA=1;  //�����ж�
}


void DuojiMid()
{
	zhuanjiao = 11;
	Delayms(150);//��ʱ1s
}

void DuojiRight()
{
	zhuanjiao = 6;
	Delayms(150);//��ʱ1s
}

void DuojiLeft()
{
	zhuanjiao = 18;
	Delayms(150);//��ʱ1s
}

///��ȡ��������ľ���,����ǰ�������Ϊ��ǰ
void GetAllDistance(unsigned int *dis_left,unsigned int *dis_right,unsigned int *dis_direct)
{
	CarStop();
	GetDistanceDelay();
	*dis_direct = distance_cm;
	
	DuojiRight();
	Delayms(100);
	GetDistanceDelay();//��ȡ�ұ߾���
	*dis_right = distance_cm;
	
	DuojiMid();
	DuojiLeft();
	Delayms(100);
	GetDistanceDelay();//��ȡ��߾���
	*dis_left = distance_cm;
	
	DuojiMid();//��λ
}

//�ϰ����ѯ�ʹ���,һ�������ϰ����ֱֹ��
void BarrierProcV1()
{
	if(distance_cm < 20)//ǰ�����ϰ���
	{
		unsigned int dis_left;//��߾���
		unsigned int dis_right;//�ұ߾���
		unsigned int dis_direct;//�ұ߾���
		if(distance_cm < 14)
		{
			CarBack();
			Delayms(500);
		}
		
		GetAllDistance(&dis_left,&dis_right,&dis_direct);
		
		if((dis_left < 14) || (dis_right < 14))
		{
			CarBack();
			Delayms(500);
			GetAllDistance(&dis_left,&dis_right,&dis_direct);
		}
		
		if(dis_left <= dis_right)
		{
			CarRight();
			Delayms(400);
		}else
		{
			CarLeft();
			Delayms(400);
		}
		
		GetAllDistance(&dis_left,&dis_right,&dis_direct);
		if(distance_cm < 14)
		{
			CarBack();
			Delayms(500);
			GetAllDistance(&dis_left,&dis_right,&dis_direct);
		}
		
		if(dis_direct >= dis_left && dis_direct >= dis_right)//ǰ����Զ����ǰ��
		{
				CarGo();
		   GetDistanceDelay();
		   Delayms(100);
			return;
		}
		
		if((dis_left < 14) || (dis_right < 14))
		{
			CarBack();
			Delayms(500);
			GetAllDistance(&dis_left,&dis_right,&dis_direct);
		}
		
		if(dis_left <= dis_right)
		{
			CarRight();
			Delayms(400);
		}else
		{
			CarLeft();
			Delayms(400);
		}
		//CarGo();
		GetDistanceDelay();		
		
	}else
	{
		CarGo();
	}
}


void BarrierProc()
{
//	if(VOID_R_IO == BARRIER_Y)
//	{
//			CarBack();
//			Delayms(300);
//	}
//	if(VOID_L_IO == BARRIER_Y)
//	{
//			CarBack();
//			Delayms(300);
//	}
		if(distance_cm < 10)//ǰ�����ϰ���
	{
		unsigned int dis_left;//��߾���
		unsigned int dis_right;//�ұ߾���
		unsigned int dis_direct;//�ұ߾���
		if(distance_cm < 8)
		{
			CarBack();
			Delayms(400);
		}
		
		while(1)
		{
			GetAllDistance(&dis_left,&dis_right,&dis_direct);
			if(dis_direct < 5)
			{
				CarBack();
				Delayms(300);
				continue;
			}
			else if((dis_left < 5) || (dis_right < 5))
			{
				CarBack();
				Delayms(300);
				continue;
			}
			else if(dis_direct >= dis_left && dis_direct >= dis_right)//ǰ��������Զ
			{
				CarGo();
				Delayms(600);
				return;
			}
			else if(dis_left <= dis_right)//��ת
			{
				CarRight();
				Delayms(500);
			}
			else if(dis_right < dis_left)
			{
				CarLeft();
				Delayms(500);
			}
		}
	}
	else
	{
		CarGo();
	}
}

/*******************************************************************************
* �� �� �� ��main
* �������� ��������
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void main()
{
	CarStop();	
//	LCD1602Init();
	Timer0Init();
//	while(1)
//	{
//		DuojiMid();
//		Delayms(1000);
//		DuojiLeft();
//		Delayms(1000);
//		DuojiMid();
//		Delayms(1000);
//		DuojiRight();
//		Delayms(1000);
//		
//	}
	distance_cm = 15;
	UltraSoundInit();
	DuojiMid();
	Delayms(1000);
	
	//5ms ִ��һ��
	while(1)
	{ 
		if(tick_5ms >= 5)
		{
			tick_5ms = 0;

			//do something
			Distance();
			BarrierProc();
		}
	}
}

/*******************************************************************************
* �� �� �� ��Timer0Int
* �������� ����ʱ��0�жϺ��� �� ÿ��TIME_MS ms����
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void Timer0Int() interrupt 1
{
	TH0=(65536-(FOSC/12*TIME_US)/1000000)/256;
	TL0=(65536-(FOSC/12*TIME_US)/1000000)%256;
	tick_1ms++;
	if(tick_1ms >= 10)
	{
		tick_1ms = 0;
		speed_count++;
		tick_5ms++;
		if(speed_count >= 50)
		{
			speed_count = 0;
		}
		CarMove();
	}
	
		duoji_count++;
	if(duoji_count <= zhuanjiao)
	{
		DUOJI_IO = 0; //�������һ�������������Ҫ���� modfied by LC 2015.09.19 22:39
	}
	else
	{
		DUOJI_IO = 1;
	}
	if(duoji_count >= 200)//20ms
	{
		duoji_count = 0;
	}
	
}

