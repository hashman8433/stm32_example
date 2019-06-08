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
** Descriptions:		DS18B20�¶ȴ�����ʵ��				
**--------------------------------------------------------------------------------------------------------
** Modified by:			FiYu
** Modified date:		
** Version:				
** Descriptions:		
** Rechecked by:			
**********************************************************************************************************/
/****-----���Ķ�ReadMe.txt����ʵ��-----***********/

#include "stm32f10x.h"
#include "delay.h"
#include "ds18b20.h"
#include "usart.h"

/**************************************************************************************
 * ��  �� : GPIO/USART1��ʼ������
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************************/
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable the GPIO_LED Clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE); 						 
		
  GPIO_DeInit(GPIOB);	 //������GPIOA�Ĵ�������Ϊȱʡֵ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //�������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_DeInit(GPIOA);	 //������GPIOA�Ĵ�������Ϊȱʡֵ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //�������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB , GPIO_Pin_9);   //��ʼ״̬��Ϩ��ָʾ��LED1
}

/**************************************************************************************
 * ��  �� : MAIN����
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************************/
int main(void)
{
	SystemInit();			//����ϵͳʱ��72MHZ
	GPIO_Configuration();
	USART1_Init();    //��ʼ������TIM
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
		delay_ms(1500);	     	// 1.5s ��ȡһ���¶�ֵ 
	} 
	
}

/*********************************END FILE********************************************/
