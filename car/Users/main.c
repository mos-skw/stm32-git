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
#include "Hongwai.h"

int w1=0,w2=0,w3=0,w4=0;
int flag=0,control=0;
char k;
float kp1=3.75,ki1=0.9,kd1=0.1,target_1=0,target_2=0;  // 调整PID参数
float err0_1=0,err1_1=0,errsum_1=0;
float err0_2=0,err1_2=0,errsum_2=0;
int16_t s_1,s_2,out_1=0,out_2=0;			//_1为左，_2为右
int16_t master_speed=0;

int i=0;
int cen=0;//层
int time_key=0;

char cen_0[20]={"Start"};
char cen_1[20]={"> Go"};
int bianji=-1;//0为普通模式，1为编辑模式

void dayin()
{
    if (cen==1&&bianji==1)
    {
		OLED_Clear();
    }
	else if (cen==0)
	{
		OLED_ShowString(1,1,cen_0);
	}
	else if (cen==1)
	{
		OLED_ShowString(1,1,cen_0);
		OLED_ShowString(2,1,cen_1);
	}
}



int main(void)
{
	Hongwai_Init();
	OLED_Init();
	Key_Init();
	Encoder_Init();
	Timer_Init();
	dayin();
	Motor_Init();	
	bianji=-1;
	while (1)
	{
	//红外
		if(w2==1 && w3==1)//直行
		{
			target_1=-50;
			target_2=50;
		}
		else if(w1==1)//左大弯
		{
			target_1=-15;
			target_2=50;
		}
		else if(w4==1)//右大弯
		{
			target_1=-50;
			target_2=15;
		}
		else if(w2==1 && w3==0)//左小弯
		{
			target_1=-25;
			target_2=50;
		}
		else if(w2==0 && w3==1)//右小弯
		{
			target_1=-50;
			target_2=25;
		}
		else if(w1==1 && w4==1)//十字
		{
			target_1=-50;
			target_2=50;
		}
		
		if (control>=1){
				err1_1 = err0_1;
				err0_1 = target_1 - s_1;
			
				// 积分限幅，防止积分饱和
				errsum_1 += err0_1;
				if(errsum_1 > 1000) errsum_1 = 1000;
				if(errsum_1 < -1000) errsum_1 = -1000;
			
			out_1 = kp1 * err0_1 + ki1 * errsum_1 + kd1 * (err0_1 - err1_1);
			
			// 目标为0时完全停止
			if (target_1 == 0)
			{
				out_1 = 0;
				errsum_1 = 0;
			}
			
			err1_2 = err0_2;
			err0_2 = target_2 - s_2;
			
				// 积分限幅，防止积分饱和
				errsum_2 += err0_2;
				if(errsum_2 > 1000) errsum_2 = 1000;
				if(errsum_2 < -1000) errsum_2 = -1000;
			
			out_2 = kp1 * err0_2 + ki1 * errsum_2 + kd1 * (err0_2 - err1_2);
			
			// 目标为0时完全停止
			if (target_2 == 0)
			{
				out_2 = 0;
				errsum_2 = 0;
			}
			Motor1_SetSpeed(out_1);
			Motor2_SetSpeed(out_2);
			control=0;
		}
		dayin();
	}
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
		control++;
		s_1 = Encoder_Get1();
		s_2 = Encoder_Get2();
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==1)
		{
			w1=0;
		}
		else{
			w1=1;
		}
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)==1)
		{
			w2=0;
		}
		else{
			w2=1;
		}
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==1)
		{
			w3=0;
		}
		else{
			w3=1;
		}
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)==1)
		{
			w4=0;
		}
		else{
			w4=1;
		}
		if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==0)
		{
			if (time_key>=2)
			{
				cen=1;
			}
			if (time_key>=2 && cen==1)
			{
				bianji=-bianji;
			}
			time_key++;
		}
			TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
	
}

