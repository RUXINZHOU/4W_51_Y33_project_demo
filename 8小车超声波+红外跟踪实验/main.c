/********************************* �����к�̫�������޹�˾ *******************************
* ʵ �� �� ��С��������+�������ʵ��
* ʵ��˵�� ��ʹ�ó������ж�ǰ��������ľ��룬ʹ�ú������ж���������ƶ�����
* ʵ��ƽ̨ ������7�š�51��Ƭ����Сϵͳ
* ���ӷ�ʽ ����ο�interface.h�ļ�
* ע    �� ��1.���������ӵĲ��죬����ת��ʱ��Ҫ����ʵ����Ҫ���ڲ��ٵĲ�ֵ
*            2.��Ҫ���ĵ��ں�����ϵĸ�Ӧ�����Լ����������ƾ���
*            3.����ʹ�ò�������鱾��Ϊ������
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
unsigned char tick_200ms = 0;//ˢ����ʾ

char ctrl_comm;//����ָ��
char ctrl_comm_last = COMM_STOP;//��һ�ε�ָ��
//unsigned char continue_time = 0;

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


//�ϰ����ѯ�ʹ���,һ�������ϰ����ֱֹ��
void BarrierProc()
{
	
		if(VOID_L_IO == BARRIER_N && VOID_R_IO == BARRIER_Y)//��ת
	{
		CarRight();
		Delayms(30);
		return;
	}
	else if(VOID_L_IO == BARRIER_Y && VOID_R_IO == BARRIER_N)//��ת
	{
		CarLeft();
		Delayms(30);
		return;
	}	
	
	if((distance_cm >= 5 && distance_cm <= 7) || distance_cm > 50) //
	{
		CarStop();//����
		return;
	}
	
	if(VOID_L_IO == VOID_R_IO)
	{
		if(distance_cm < 5) CarBack();
		if(distance_cm > 7) CarGo();	
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
	LCD1602Init();
	Timer0Init();
	UltraSoundInit();
	//5ms ִ��һ��
	while(1)
	{ 
		if(tick_5ms >= 5)
		{
			tick_5ms = 0;
			tick_200ms++;
			if(tick_200ms >= 40)
			{
				tick_200ms = 0;
				LCD1602WriteDistance(distance_cm);
			}
			
			//do something
			Distance();
			BarrierProc();
//			if(ctrl_comm_last != ctrl_comm)//ָ����仯
//			{
//				ctrl_comm_last = ctrl_comm;
//				switch(ctrl_comm)
//				{
//					case COMM_UP:    CarGo();break;
//					case COMM_DOWN:  CarBack();break;
//					case COMM_LEFT:  CarLeft();break;
//					case COMM_RIGHT: CarRight();break;
//					case COMM_STOP:  CarStop();break;
//					default : break;
//				}
//				LCD1602WriteCommand(ctrl_comm);
//			}
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
}

