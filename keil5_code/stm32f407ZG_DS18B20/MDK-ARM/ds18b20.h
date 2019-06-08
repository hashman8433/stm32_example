#ifndef __DS18B20_H
#define	__DS18B20_H

#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"

#define HIGH  1
#define LOW   0

#define DS18B20_CLK     RCC_APB2Periph_GPIOB
#define DS18B20_PIN     GPIO_Pin_6                  
#define DS18B20_PORT		GPIOB 

//���κ꣬��������������һ��ʹ��,����ߵ�ƽ��͵�ƽ
#define DS18B20_DATA_OUT(a)	if (a)	\
					GPIO_SetBits(GPIOB,GPIO_Pin_6);\
					else		\
					GPIO_ResetBits(GPIOB,GPIO_Pin_6)
 //��ȡ���ŵĵ�ƽ
#define  DS18B20_DATA_IN()	   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)

typedef struct
{
	u8  humi_int;		//ʪ�ȵ���������
	u8  humi_deci;	 	//ʪ�ȵ�С������
	u8  temp_int;	 	//�¶ȵ���������
	u8  temp_deci;	 	//�¶ȵ�С������
	u8  check_sum;	 	//У���
		                 
}DS18B20_Data_TypeDef;

u8 DS18B20_Init(void);
float DS18B20_Get_Temp(void);
#endif /* __DS18B20_H */