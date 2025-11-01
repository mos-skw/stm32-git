#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Motor.h"
#include "Key.h"
#include "Serial.h"
#include "Key.h"
#include "Encoder.h"


uint8_t KeyNum;
uint8_t bianji;

int main(void)
{
	OLED_Init();
	Motor_Init();
	Key_Init();
	Serial_Init();
	
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
	}

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
			OLED_ShowString(4,1,"    ");
		}
	}
}
