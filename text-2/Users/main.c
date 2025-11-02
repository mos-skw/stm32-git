#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Motor.h"
#include "Key.h"
#include "Serial.h"
#include "Encoder.h"
#include <string.h>
#include <stdio.h>
#include "Timer.h"


uint8_t KeyNum;
uint8_t bianji;
char k;
int16_t speed=0;
float kp=0.1,ki=0,kd=0,target;
float actual[2],out[2],err0[2],err1[2],errsum[2];
int main(void)
{
	OLED_Init();
	Timer_Init();
	Motor_Init();
	Key_Init();
	Serial_Init();
	Encoder_Init();
	
	bianji=0;
	OLED_ShowString(1,1,"Speed:");
	while (1)
	{
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==0 && bianji==0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==0);
		Delay_ms(20);
		KeyNum=1;
	}
		else if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==0 && bianji==1)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==0);
		Delay_ms(20);
		KeyNum=0;
	}         //按键切换模式

		if (KeyNum==1)
		{
			bianji=1;
		}
		else if(KeyNum==0)
		{
			bianji=0;
		}
		
		if (bianji==1)
		{
			OLED_ShowString(4,1,"move");
		}
		else if (bianji==0)
		{
			OLED_ShowString(4,1,"    ");  //OLED上显示更换模式
		}
		
		if (bianji==0)
		{
			int len=strlen(Serial_RxPacket);
			for (int i=0;i<len;i++)
			{
				if (Serial_RxPacket[i]=='\0')
				{
					break;
				}
				k=Serial_RxPacket[i];
				speed=speed*10+(int)k;
			}
			Motor1_SetSpeed(speed);//电脑输入，电机旋转
		}
		else if(bianji==1)
		{
			
		}   //从动
		

		
	}
}



void TIM2_IRQHandler(void)    //PID
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		target=speed;
		actual[0]=Encoder_Get1();
		actual[1]=Encoder_Get2();
		for (int i=0;i<2;i++)
		{
			err1[i]=err0[i];
			err0[i]=target-actual[i];
			errsum[i]+=err0[i];
			
			out[i]=kp*err0[i]+ki*errsum[i]+kd*(err0[i]-err1[i]);
			if (out[i]>100)
			{
				out[i]=100;
			}
			else if (out[i]<-100)
			{
				out[i]=-100;
			}
			if (actual[i]==0&&target==0)
			{
				out[i]=0;
				errsum[i]=0;
			}
			if (i==0)
			{
				Motor1_SetSpeed(out[i]);
			}
			else if (i==1)
			{
				Motor2_SetSpeed(out[i]);
			}
		}
		printf("%f,%f,%f,%f,%f\n",actual[0],actual[1],out[0],out[1],target);
		TIM_ClearITPendingBit(TIM2 , TIM_IT_Update);
	}
}
