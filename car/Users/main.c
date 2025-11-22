#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Motor.h"
#include "Key.h"
#include "Serial.h"
#include "Encoder.h"
#include <string.h>
#include <stdio.h>
#include "Timer.h"
#include <stdlib.h>
#include <math.h>
#include "OLED.h"

int flag=1,send=0,control=0,kt=0;
char k;
float kp1=3.75,ki1=0.9,kd1=0.1,target=0;  // 调整PID参数
float kp2=6,ki2=5.5,kd2=0.18;           // 调整PID参数
float err0=0,err1=0,errsum=0;
int16_t s_1,s_2,out=0;
int16_t master_speed=0;

uint16_t Num_T=0;
int i=0;
int cen=0;//层
int cen0=0;//Main Menu中指针位置，指向二级
int cen1=0;//二级菜单中指针位置，指向最小一级
int zhixin=1;
int time_key=0;
uint8_t KeyNum=0;

char cen_0[4][20]={"Speed Control","PID","Start"};
char cen_1[4][3][20]=
{
    {"Speed_model: ","Speed_dir: "},
	{"kp: ","ki: ","kd: "},
    {"go"}
};
int cen_1num[3][3]={{0,0,0},{0,0,0},{0,0,0}};
int len[3]={2,3,1};
int bianji=0;//0为普通模式，1为编辑模式
void dayin()
{
    if (bianji==1)
    {
        OLED_ShowString(1,15,"E");//第一行
    }
    if (cen==0)
    {
        for (int i=0;i<4;i++)
        {
            if (cen0==i)
            {
                OLED_ShowString(i+1,1,">");
            }
            else
            {
                OLED_ShowString(i+1,1," ");
            }
            OLED_ShowString(i+1,2,cen_0[i]);
        }
    }

    else if (cen==1 && cen0==0)
    {
		OLED_ShowString(1,1,cen_0[0]);
        for (int i=0;i<2;i++)
        {
            if (cen1==i)
            {
                OLED_ShowString(i+2,1,">");
            }
            else
            {
                OLED_ShowString(i+2,1," ");
            }
            OLED_ShowString(i+2,2,cen_1[0][i]);
            OLED_ShowNum(i+2,14,cen_1num[0][i],1);
        }
    }
    else if (cen==1 && cen0==1)
    {
		OLED_ShowString(1,1,cen_0[1]);
        for (int i=0;i<3;i++)
        {
            if (cen1==i)
            {
                OLED_ShowString(i+2,1,">");
            }
            else
            {
                OLED_ShowString(i+2,1," ");
            }
            OLED_ShowString(i+2,2,cen_1[1][i]);
			int j=0;
			int n=0;
			if (cen_1num[1][i]<0)
			{
				n=(-cen_1num[1][i])/10;
				if (n==0)
				{
					OLED_ShowNum(i+2,14,0,1);
					j++;
				}
				
				while(n>0)
				{
					OLED_ShowNum(i+2,14-j,(n)%10,1);
					j++;
					n=n/10;
				}
				OLED_ShowString(i+2,14-j,"-");
				OLED_ShowString(i+2,15,".");
				OLED_ShowNum(i+2,16,-(cen_1num[1][i])%10,1);				
			}
			else
			{
				n=cen_1num[1][i]/10;
				if (n==0)
				{
					OLED_ShowNum(i+2,14,0,1);
				}
				while(n>0)
				{
					OLED_ShowNum(i+2,14-j,n%10,1);
					j++;
					n=n/10;
				}
				OLED_ShowString(i+2,15,".");
				OLED_ShowNum(i+2,16,cen_1num[1][i]%10,1);
			}
        }
    }
    else if (cen==1 && cen0==2)
    {
		OLED_ShowString(1,1,cen_0[2]);
		OLED_ShowString(2,1,">");
		OLED_ShowString(2,2,cen_0[2]);
    }
    else if (cen==1 && cen0==3)
    {
		OLED_ShowString(1,1,cen_0[3]);
		OLED_ShowString(2,1,">");
		OLED_ShowString(2,2,cen_0[3]);
    }	
}




int main(void)
{
	OLED_Init();
	Key_Init();
	Encoder_Init();
	Timer_Init();
	dayin();
	Motor_Init();
	Serial_Init();	
	bianji=1;	
	while (1)
	{
//		if (send>=1)
//		{
//			Serial_Printf("%d,%d\n", s_1,s_2);
//			send=0;
//		}
//		// 串口数据处理
//			if(bianji==1){
//			// 直接速度控制模式
//			if (Serial_RxFlag==1){
//				int i=0;
//				char a[5]={0};
//				while (Serial_RxPacket[i+6] != '\0' && i<4)
//				{
//					a[i]=Serial_RxPacket[i+6];
//					i++;
//				}
//				a[i]='\0';
//				
//				if (atoi(a) <= 60 && atoi(a) >= -60)
//				{
//					target = atoi(a);
//				}
//			
//				err0=0,err1=0,errsum=0;
//				out=0;
//				Serial_RxFlag = 0;  // 清除标志
//			}
//			if (control>=1){
//				err1 = err0;
//				err0 = target - s_1;
//			
//				// 积分限幅，防止积分饱和
//				errsum += err0;
//				if(errsum > 1000) errsum = 1000;
//				if(errsum < -1000) errsum = -1000;
//			
//			out = kp1 * err0 + ki1 * errsum + kd1 * (err0 - err1);
//			
//			// 目标为0时完全停止
//			if (target == 0)
//			{
//				out = 0;
//				errsum = 0;
//			}
//			Motor1_SetSpeed(out);
//			control=0;
//			}
//		}
//		else if (bianji == -1)
//		{
//			// 跟随模式
//			master_speed = s_2;
//			if (control>=1){
//			err1 = err0;
//			err0 = master_speed - s_1;	
//			errsum += err0;
//			if(errsum > 1000) errsum = 1000;
//			if(errsum < -1000) errsum = -1000;
//				
//			out = kp2 * err0 + ki2 * errsum + kd2 * (err0 - err1);
//			
//			Motor1_SetSpeed(out);
//			control=0;
//			}
//		}
		if (cen_1num[0][1]==0)
		{
			
			if (cen_1num[0][0]==0)
			{
				if (Num_T>=50)
				{
					Num_T=0;
					GPIO_Write(GPIOB,~(0x0001<<(i+12)));
					i=(i+1)%3;
				}
			}
			else if (cen_1num[0][0]==1)
			{
				if (Num_T>=100)
				{
					Num_T=0;
					GPIO_Write(GPIOB,~(0x0001<<(i+12)));
					i=(i+1)%3;
				}
			}
			else if (cen_1num[0][0]==2)
			{
				if (Num_T>=20)
				{
					Num_T=0;
					GPIO_Write(GPIOB,~(0x0001<<(i+12)));
					i=(i+1)%3;
				}
			}
		}
		else if (cen_1num[0][1]==1)
		{

			if (cen_1num[0][0]==0)
			{
				if (Num_T>=50)
				{
					Num_T=0;
					GPIO_Write(GPIOB,~(0x0001<<(i+12)));
					i=(i+2)%3;
				}
			}
			else if (cen_1num[0][0]==1)
			{
				if (Num_T>=100)
				{
					Num_T=0;
					GPIO_Write(GPIOB,~(0x0001<<(i+12)));
					i=(i+2)%3;
				}
			}
			else if (cen_1num[0][0]==2)
			{
				if (Num_T>=20)
				{
					Num_T=0;
					GPIO_Write(GPIOB,~(0x0001<<(i+12)));
					i=(i+2)%3;
				}
			}
		}
		

		if (bianji==1 && cen0==1)
		{
			OLED_Clear();
			dayin();
		}
		if (KeyNum==1 && zhixin==1)
        {
			if (bianji==0)
			{
				if (cen==0)
				{
					cen0=(cen0+2)%3;
				}
				else if (cen==1)
				{
					cen1=(cen1+len[cen0]-1)%(len[cen0]);
				}
			}
			else if(bianji==1 && cen==1)
			{
				if (cen0==0 && cen1==0)
				{
					cen_1num[0][0]=(cen_1num[0][0]+1)%3;
				}
				else if(cen0==0 && cen1==1)
				{
					cen_1num[0][1]=(cen_1num[0][1]+1)%2;
				}
				else if(cen0==1 && cen1==0)
				{
					cen_1num[1][0]=cen_1num[1][0]+1;
				}
				else if(cen0==1 && cen1==1)
				{
					cen_1num[1][1]=cen_1num[1][1]+1;
				}	
				else if(cen0==1 && cen1==2)
				{
					cen_1num[1][2]=cen_1num[1][2]+1;
				}				
			}
			KeyNum=0;
			zhixin=0;
			OLED_Clear();
			dayin();
        }
		else if (KeyNum==2 && zhixin==1)
        {
			if (bianji==0)
			{
				if (cen==0)
				{
					cen0=(cen0+1)%3;
				}
				else if (cen==1)
				{
					cen1=(cen1+1)%(len[cen0]);
				}
			}
			else if(bianji==1 && cen==1)
			{
				if (cen0==0 && cen1==0)
				{
					cen_1num[0][0]=(cen_1num[0][0]+2)%3;
				}
				else if(cen0==0 && cen1==1)
				{
					cen_1num[0][1]=(cen_1num[0][1]+1)%2;
				}
				else if(cen0==1 && cen1==0)
				{
					cen_1num[1][0]=cen_1num[1][0]-1;
				}
				else if(cen0==1 && cen1==1)
				{
					cen_1num[1][1]=cen_1num[1][1]-1;
				}	
				else if(cen0==1 && cen1==2)
				{
					cen_1num[1][2]=cen_1num[1][2]-1;
				}				
			}
			KeyNum=0;
			zhixin=0;
			OLED_Clear();
			dayin();
        }
		else if (KeyNum==3 && zhixin==1)
        {
            if (cen==0)
            {
                cen=1;
                cen1=0;
            }
            else if (cen==1 && ((cen0==0) || cen0==1))
            {
                bianji=(bianji+1)%2;
            }
			KeyNum=0;
			zhixin=0;
			OLED_Clear();
			dayin();
        }
		else if (KeyNum==4 && zhixin==1)
        {
            if (cen==1 && bianji==0)
            {
                cen=0;
                cen1=0;
            }
            else if (bianji==1)
            {
                bianji=0;
            }
			KeyNum=0;
			zhixin=0;
			OLED_Clear();
			dayin();
        }
		
	}
}


int n[4]={10,10,10,10};
void TIM2_IRQHandler(void)
{
//	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
//	{
//		control++;
//		send++;
//		s_1 = Encoder_Get1();
//		s_2 = Encoder_Get2();
//	// 简化按键处理
//		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==0)
//		{
//			if(kt>=1)
//			{
//				bianji=-bianji;
//				kt=0;
//				err0=0,err1=0,errsum=0;
//				out=0;
//			}
//			kt++;
//		}
//		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
//	}
	
	if (TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
		Num_T++;
		if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)==0)
		{
			if (time_key>=2)
			{
				KeyNum=1;
				if (time_key>=100)
				{
					if (n[0]>=10)
					{
						zhixin=1;
						n[0]=0;
					}
					n[0]++;
				}
			}
			time_key++;
		}
		else if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)==0)
		{
			if (time_key>=2)
			{
				KeyNum=2;
				if (time_key>=100)
				{
					if (n[1]>=10)
					{
						zhixin=1;
						n[1]=0;
					}
					n[1]++;
				}
			}
			time_key++;
		}
		else if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==0)
		{
			if (time_key>=2)
			{
				KeyNum=3;
				if (time_key>=100)
				{
					if (n[2]>=10)
					{
						zhixin=1;
						n[2]=0;
					}
					n[2]++;
				}
			}
			time_key++;
		}
		else if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_9)==0)
		{
			if (time_key>=2)
			{
				KeyNum=4;
				if (time_key>=100)
				{
					if (n[3]>=10)
					{
						zhixin=1;
						n[3]=0;
					}
					n[3]++;
				}
			}
			time_key++;
		}
		else
		{
			time_key=0;
			KeyNum=0;
			zhixin=1;
		}
			TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
	
}

