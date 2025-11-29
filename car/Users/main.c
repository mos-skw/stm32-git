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
float pout_1,iout_1,dout_1;
float pout_2,iout_2,dout_2;

int i=0;
int cen=0;//层
int time_key=0;

char cen_0[20]={"Start"};
char cen_1[20]={"> Go"};
int bianji=-1;//0为普通模式，1为编辑模式

float min_float(float a, float b)
{
    return (a >= b) ? b : a;
}
float max_float(float a, float b)
{
    return (a >= b) ? a : b;
}

void dayin()
{
	if (cen==0)
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
		if(w1==1 && w4==1)//十字
		{
			target_1=-45;
			target_2=45;
		}
		else if(w1==1 && w2==0)//左大弯
		{
			target_1=-45;
			target_2=5;
		}
		else if(w4==1 && w3==0)//右大弯
		{
			target_1=-5;
			target_2=45;
		}
		else if(w2==1 && w3==0)//左小弯
		{
			target_1=-45;
			target_2=15;
		}
		else if(w2==0 && w3==1)//右小弯
		{
			target_1=-15;
			target_2=45;
		}
		else if(w2==1 && w3==1)//直行
		{
			target_1=-45;
			target_2=45;
		}
		
		if(cen==1)
		{
		if (control>=1){
				err1_1 = err0_1;
				err1_2 = err0_2;
				err0_1 = target_1 - s_1;
				err0_2 = target_2 - s_2;
				errsum_1 += err0_1;
				errsum_2 += err0_2;
				pout_1=kp1*err0_1;
				pout_2=kp1*err0_2;
				if (err0_1 < 1000 && err0_1 > -1000)
				{
					iout_1=ki1*errsum_1;
					iout_1=min_float(iout_1,1000);
					iout_1=max_float(iout_1,-1000);
				}
				else
				{
					iout_1=0;
				}
				if (err0_2 < 1000 && err0_2 > -1000)
				{
					iout_2=ki1*errsum_2;
					iout_2=min_float(iout_2,1000);
					iout_2=max_float(iout_2,-1000);
				}
				else
				{
					iout_2=0;
				}				
				dout_1=kd1*(err0_1-err1_1);
				out_1 = pout_1 + iout_1 + dout_1;	
				dout_2=kd1*(err0_2-err1_2);
				out_2 = pout_2 + iout_2 + dout_2;	
				if (out_1 > 100)
				{
					out_1 = 100;
				}
				if (out_1 < -100)
				{
					out_1 = -100;
				}			
				Motor1_SetSpeed(out_1);
				if (out_2 > 100)
				{
					out_2 = 100;
				}
				if (out_2 < -100)
				{
					out_2 = -100;
				}			
				Motor2_SetSpeed(out_2);
				control = 0;
			}
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
			if (time_key>=4 && cen==1)
			{
				bianji=-bianji;
			}
			time_key++;
		}
			TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
	
}

