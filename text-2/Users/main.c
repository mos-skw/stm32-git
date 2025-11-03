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

int flag=1;
uint8_t KeyNum;
uint8_t bianji;
char k;
int16_t speed=0,p=0;
float kp1=0.01,ki1=0,kd1=0,target=0;  // 调整PID参数
float kp2=0.01,ki2=0,kd2=0;           // 调整PID参数
float actual[2],out[2],err0[2]={0},err1[2],errsum[2];

// 添加输出限制和死区补偿
#define OUTPUT_LIMIT 100
#define DEAD_ZONE 10

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
	OLED_ShowString(2,1,"Target:0");
	
	// 初始停止电机
	Motor1_SetSpeed(0);
	Motor2_SetSpeed(0);
	
	while (1)
	{
		// 简化按键处理
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==0)
		{
			Delay_ms(20);
			if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==0)
			{
				bianji = !bianji;  // 切换模式
				while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==0); // 等待释放
				Delay_ms(20);
				
				// 模式切换时重置PID
				errsum[0] = 0;
				errsum[1] = 0;
				err0[0] = 0;
				err0[1] = 0;
				err1[0] = 0;
				err1[1] = 0;
			}
		}

		// 显示当前模式
		if (bianji==1)
		{
			OLED_ShowString(4,1,"Follow");
		}
		else
		{
			OLED_ShowString(4,1,"Direct ");
		}
		
		// 串口数据处理
		if (bianji==0 && Serial_RxFlag==1)
		{
			speed = 0;
			flag = 1;
			
			int len=strlen(Serial_RxPacket);
			for (int i=0;i<len;i++)
			{
				if (Serial_RxPacket[i]=='\0') break;
				
				if (Serial_RxPacket[i]=='-')
				{
					flag=-1;
				}
				else if(Serial_RxPacket[i] >= '0' && Serial_RxPacket[i] <= '9')
				{
					speed = speed * 10 + (Serial_RxPacket[i] - '0');
				}
			}
			speed *= flag;		
			target = speed;
			
			// 显示目标速度
			OLED_ShowSignedNum(2,8,(int16_t)target,5);
			
			Serial_RxFlag = 0;  // 清除标志
		}
		
		// 显示实际速度
		OLED_ShowSignedNum(1,7,(int16_t)actual[0],5);
		Serial_Printf("%.lf,%.1f\n", actual[0],actual[1]);
		Delay_ms(10);
	}
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		if(bianji==0){
			// 直接速度控制模式
			actual[0] = Encoder_Get1();
			
			err1[0] = err0[0];
			err0[0] = target - actual[0];
			
			// 积分限幅，防止积分饱和
			errsum[0] += err0[0];
			if(errsum[0] > 1000) errsum[0] = 1000;
			if(errsum[0] < -1000) errsum[0] = -1000;
			
			out[0] = kp1 * err0[0] + ki1 * errsum[0] + kd1 * (err0[0] - err1[0]);
			
			// 输出限幅
			if (out[0] > OUTPUT_LIMIT) out[0] = OUTPUT_LIMIT;
			else if (out[0] < -OUTPUT_LIMIT) out[0] = -OUTPUT_LIMIT;
			
			// 死区补偿
			if(out[0] > 0 && out[0] < DEAD_ZONE) out[0] = DEAD_ZONE;
			else if(out[0] < 0 && out[0] > -DEAD_ZONE) out[0] = -DEAD_ZONE;
			
			// 目标为0时完全停止
			if (target == 0)
			{
				out[0] = 0;
				errsum[0] = 0;
			}
			
			Motor1_SetSpeed((int16_t)out[0]);
		}
		else
		{
			// 跟随模式
			float master_speed = Encoder_Get1();
			actual[1] = Encoder_Get2();
			
			err1[1] = err0[1];
			err0[1] = master_speed - actual[1];
			
			// 积分限幅
			errsum[1] += err0[1];
			if(errsum[1] > 1000) errsum[1] = 1000;
			if(errsum[1] < -1000) errsum[1] = -1000;
			
			out[1] = kp2 * err0[1] + ki2 * errsum[1] + kd2 * (err0[1] - err1[1]);
			
			// 输出限幅和死区补偿
			if (out[1] > OUTPUT_LIMIT) out[1] = OUTPUT_LIMIT;
			else if (out[1] < -OUTPUT_LIMIT) out[1] = -OUTPUT_LIMIT;
			
			if(out[1] > 0 && out[1] < DEAD_ZONE) out[1] = DEAD_ZONE;
			else if(out[1] < 0 && out[1] > -DEAD_ZONE) out[1] = -DEAD_ZONE;
			
			Motor2_SetSpeed((int16_t)out[1]);
		}
	
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
