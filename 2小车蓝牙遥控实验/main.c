/********************************* �����к�̫�������޹�˾ *******************************
* ʵ �� �� ��С������ң��ʵ��
* ʵ��˵�� ��ʹ���ֻ���������ģ���ͨ���ֻ�����ָ��������С��
* ʵ��ƽ̨ ������7�š�51��Ƭ����Сϵͳ
* ���ӷ�ʽ ����ο�interface.h�ļ�
* ע    �� ��ָ������������ͲŻ�ʹС����������ֹͣ����ָ���С����ֹͣ
* ��    �� ����̫���Ӳ�Ʒ�з���    QQ ��1909197536
* ��    �� ��http://shop120013844.taobao.com/
****************************************************************************************/

#include <reg52.h>
#include <intrins.h>
#include <stdio.h>
#include "LCD1602.h"
#include "interface.h"
#include "motor.h"

//ȫ�ֱ�������
unsigned int speed_count=0;//ռ�ձȼ����� 50��һ����
char front_left_speed_duty=SPEED_DUTY;
char front_right_speed_duty=SPEED_DUTY;
char behind_left_speed_duty=SPEED_DUTY;
char behind_right_speed_duty=SPEED_DUTY;

unsigned char tick_5ms = 0;//5ms����������Ϊ�������Ļ�������
unsigned char tick_1ms = 0;//1ms����������Ϊ����Ļ���������

char ctrl_comm;//����ָ��
unsigned char continue_time = 0;
unsigned char bt_rec_flag=0;//�������Ʊ�־λ

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

char putchar(char ch)
{ 
	/* Place your implementation of fputc here */ 
	SBUF=(unsigned char)ch; //�����յ������ݷ��뵽���ͼĴ���
	while(!TI);		  //�ȴ������������
	TI=0;		 //���������ɱ�־λ	
	return ch;
}

/*******************************************************************************
* �� �� �� ��UsartConfiguration
* �������� ����������
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void UsartConfiguration()
{
	SCON = 0X50;			//����Ϊ������ʽ1	10λ�첽�շ���
	TMOD |= 0x20; //���ü�����������ʽ2  8λ�Զ���װ������	
	PCON = 0X80;//�����ʼӱ�	SMOD = 1  19200
	TH1 = 256 -(FOSC/12/32/(BAUD/2)); //���������
	TL1 = 256 -(FOSC/12/32/(BAUD/2));
	TR1 = 1; //�򿪶�ʱ��	
	ES=1;//�򿪴���
	EA = 1;//�����ж�
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


/*******************************************************************************
* �� �� �� ��main
* �������� ��������
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void main()
{
	CarStop();	
	UsartConfiguration();
	LCD1602Init();
	Timer0Init();
	
	//5ms ִ��һ��
	while(1)
	{ 
		if(tick_5ms >= 5)
		{
			tick_5ms = 0;
			continue_time--;//200ms �޽���ָ���ͣ��
			if(continue_time == 0)
			{
				continue_time = 1;
				CarStop();
			}
			//do something
			if(bt_rec_flag == 1)//���յ������ź�
			{
				bt_rec_flag = 0;
				switch(ctrl_comm)
				{
					case COMM_UP:    CarGo();break;
					case COMM_DOWN:  CarBack();break;
					case COMM_LEFT:  CarLeft();break;
					case COMM_RIGHT: CarRight();break;
					case COMM_STOP:  CarStop();break;
					default : break;
				}
				LCD1602WriteCommand(ctrl_comm);
			}
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

/*******************************************************************************
* �� �� �� ��UsartInt
* �������� �������жϷ�����
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void UsartInt() interrupt 4
{
	unsigned char rec_data;
	if(RI == 1)
	{
		rec_data=SBUF; 
		bt_rec_flag = 1;
		ctrl_comm = rec_data;
		RI = 0;           //��������жϱ�־λ
		continue_time = 40;
	}	 
}

