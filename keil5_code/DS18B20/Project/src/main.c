/****************************************Copyright (c)****************************************************
**                                        
**                                 
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			
** Last modified Date:          
** Last Version:		   
** Descriptions:							
**--------------------------------------------------------------------------------------------------------
** Created by:			FiYu
** Created date:		2015-12-12
** Version:			    1.0
** Descriptions:		DS18B20温度传感器实验				
**--------------------------------------------------------------------------------------------------------
** Modified by:			FiYu
** Modified date:		
** Version:				
** Descriptions:		
** Rechecked by:			
**********************************************************************************************************/
/****-----请阅读ReadMe.txt进行实验-----***********/

#include "stm32f10x.h"
#include "delay.h"
#include "ds18b20.h"
#include "usart.h"

/**************************************************************************************
 * 描  述 : GPIO/USART1初始化配置
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable the GPIO_LED Clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE); 						 
		
  GPIO_DeInit(GPIOB);	 //将外设GPIOA寄存器重设为缺省值
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_DeInit(GPIOA);	 //将外设GPIOA寄存器重设为缺省值
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB , GPIO_Pin_9);   //初始状态，熄灭指示灯LED1
}

/**************************************************************************************
 * 描  述 : MAIN函数
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
int main(void)
{
	SystemInit();			//设置系统时钟72MHZ
	GPIO_Configuration();
	USART1_Init();    //初始化配置TIM
	GPIO_ResetBits(GPIOB , GPIO_Pin_9);
	delay_ms(500);	
	
	while( DS18B20_Init())	
	{
		printf("\r\n no ds18b20 exit \r\n");
		GPIO_SetBits(GPIOB , GPIO_Pin_9); 
	}	
	printf("\r\n ds18b20 exit \r\n");
	GPIO_SetBits(GPIOB , GPIO_Pin_9); 
	
	for(;;)
	{	
		printf("\r\n temperature %.1f\r\n",DS18B20_Get_Temp());				 
		delay_ms(1500);	     	// 1.5s 读取一次温度值 
	} 
	
}

/*********************************END FILE********************************************/
