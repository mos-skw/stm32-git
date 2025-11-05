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

int flag=1,send=0,control=0,kt=0;
int8_t bianji;
char k;
float kp1=3.75,ki1=0.9,kd1=0.1,target=0;  // 调整PID参数
float kp2=6,ki2=5.5,kd2=0.18;           // 调整PID参数
float err0=0,err1=0,errsum=0;
int16_t s_1,s_2,out=0;
int16_t master_speed=0;

int main(void)
{
	Motor_Init();
	Key_Init();
	Timer_Init();
	Encoder_Init();
	Serial_Init();
	
	bianji=1;	
	while (1)
	{
		if (send>=1)
		{
			Serial_Printf("%d,%d\n", s_1,s_2);
			send=0;
		}
		// 串口数据处理
			if(bianji==1){
			// 直接速度控制模式
			if (Serial_RxFlag==1){
				int i=0;
				char a[5]={0};
				while (Serial_RxPacket[i+6] != '\0' && i<4)
				{
					a[i]=Serial_RxPacket[i+6];
					i++;
				}
				a[i]='\0';
				
				if (atoi(a) <= 60 && atoi(a) >= -60)
				{
					target = atoi(a);
				}
			
				err0=0,err1=0,errsum=0;
				out=0;
				Serial_RxFlag = 0;  // 清除标志
			}
			if (control>=1){
				err1 = err0;
				err0 = target - s_1;
			
				// 积分限幅，防止积分饱和
				errsum += err0;
				if(errsum > 1000) errsum = 1000;
				if(errsum < -1000) errsum = -1000;
			
			out = kp1 * err0 + ki1 * errsum + kd1 * (err0 - err1);
			
			// 目标为0时完全停止
			if (target == 0)
			{
				out = 0;
				errsum = 0;
			}
			Motor1_SetSpeed(out);
			control=0;
			}
		}
		else if (bianji == -1)
		{
			// 跟随模式
			master_speed = s_2;
			if (control>=1){
			err1 = err0;
			err0 = master_speed - s_1;	
			errsum += err0;
			if(errsum > 1000) errsum = 1000;
			if(errsum < -1000) errsum = -1000;
				
			out = kp2 * err0 + ki2 * errsum + kd2 * (err0 - err1);
			
			Motor1_SetSpeed(out);
			control=0;
			}
		}
	}
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		control++;
		send++;
		s_1 = Encoder_Get1();
		s_2 = Encoder_Get2();
	// 简化按键处理
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==0)
		{
			if(kt>=1)
			{
				bianji=-bianji;
				kt=0;
				err0=0,err1=0,errsum=0;
				out=0;
			}
			kt++;
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
