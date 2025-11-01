#include "stm32f10x.h"                  // Device header
#include "Delay.h"
//uint8_t KeyNum=0;
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure_A;
	GPIO_InitStructure_A.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure_A.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_4;
	GPIO_InitStructure_A.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure_A);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure_C;
	GPIO_InitStructure_C.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure_C.GPIO_Pin=GPIO_Pin_14;
	GPIO_InitStructure_C.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure_C);

}

/*
uint8_t Key_GetNum(void)
{

	if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)==0)
	{
		Delay_ms(5);
		KeyNum=1;
	}
	else if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==0)
	{
		Delay_ms(5);
		KeyNum=2;
	}
	else if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==0)
	{
		Delay_ms(5);
		KeyNum=3;
	}
	else if (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14)==0)
	{
		Delay_ms(5);
		KeyNum=4;
	}
	return KeyNum;
}

uint8_t Key_GetNum(void)
{
	uint8_t Temp;
	Temp=KeyNum;
	KeyNum=0;
	return Temp;
}


void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0) == SET)
	{
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
		{
			KeyNum=1;
		}
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

void EXTI2_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line2) == SET)
	{
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2))
		{
			KeyNum=2;
		}
		EXTI_ClearITPendingBit(EXTI_Line2);
	}
}

void EXTI4_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line4) == SET)
	{
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4))
		{
			KeyNum=3;
		}
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}

void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line14) == SET)
	{
		if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14))
		{
			KeyNum=4;
		}
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
}
*/

