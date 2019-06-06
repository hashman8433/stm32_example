/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#define HIGH GPIO_PIN_SET
#define LOW GPIO_PIN_RESET
#define LED_ON HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, LOW);
#define LED_OFF HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, HIGH);

#include "stm32f4xx_hal.h"


/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void delay_us(uint16_t);
static void delay_ms(uint16_t);
static float DS18B20_Get_Temp(void);
static uint8_t DS18B20_Check(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
float templature;
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, HIGH);
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
		
		if(!DS18B20_Check()) {
			templature = DS18B20_Get_Temp();
			LED_ON;
			// printf("\r\n no ds18b20 exit \r\n");
		}else {
			LED_OFF;
		}
		delay_ms(200);
		
  }
  /* USER CODE END 3 */

}



/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

}

/**
	ds18b20 引脚 推挽 输出模式
*/
static void DS18B20_Mode_OUT(void) {
	/* 溫度计 引脚 B7 */
	GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOB_CLK_ENABLE();
	
  /*Configure GPIO pin : PE5 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	
}

/**
	切换为输入模式
*/
static void DS1820_MODE_IN(void) {
	
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = GPIO_PIN_6; 
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;	 // 浮空输入模式
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}

/**
	复位信号
*/
static void DS18B20_Rst(void) {
	
	DS18B20_Mode_OUT();
	delay_us(15);
	/* 拉高 B5 电平 */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, LOW);
	delay_us(750);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, HIGH);
	delay_us(15);
	
}

/**
	检测 是否返回 低电平脉冲
*/
static uint8_t DS18B20_Presence(void) {
	
	uint8_t pulse_time = 0;
	
	delay_us(2);
	DS1820_MODE_IN();
	
	while ( HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) && pulse_time < 100 ) {
		pulse_time++;
		delay_us(1);
	}
	
	if( pulse_time >=100 )  
		return 1;          
	else                 
		pulse_time = 0; 
	
	while( !HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) && pulse_time<240 )  
	{
		pulse_time++;
		delay_us(1);
	}	
	if( pulse_time >=240 )
		return 1;        
	else
		return 0;
	
}

static uint8_t DS18B20_Check(void) {
	DS18B20_Rst();
	return DS18B20_Presence();
}


/**
	读取 ds18b20 字节
*/
uint8_t DS18B20_Read_Byte(void)
{
	uint8_t i, j, dat = 0;	
	
	for(i=0; i<8; i++) 
	{
		j = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);
		dat = (dat) | (j<<i);
	}
	
	return dat;																																																																																
}

void DS18B20_Write_Byte(uint8_t dat)
{
	uint8_t i, testb;
	DS18B20_Mode_OUT();
	
	for( i=0; i<8; i++ )
	{
		testb = dat&0x01;
		dat = dat>>1;		
		
		if (testb)
		{			
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, LOW);
			delay_us(8);   //1us < ?a???óê± < 15us
			
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, HIGH);
			delay_us(58);    //58us+8us>60us
		}		
		else
		{			
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, LOW);
			/* 60us < Tx 0 < 120us */
			delay_us(70);
			
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, HIGH);
			/* 1us < Trec(???′ê±??) < ?T??′ó*/
			delay_us(2);
		}
	}
}


/**************************************************************************************
	获取温度信息
 **************************************************************************************/

uint8_t tpmsb, tplsb;
short s_tem;
float f_tem;

float DS18B20_Get_Temp(void)
{
	
	
	DS18B20_Rst();	   
	DS18B20_Presence();	 
	DS18B20_Write_Byte(0XCC);				/* ì?1y ROM */
	DS18B20_Write_Byte(0X44);				/* ?aê?×a?? */
	
	DS18B20_Rst();
  DS18B20_Presence();
	DS18B20_Write_Byte(0XCC);				/* ì?1y ROM */
  DS18B20_Write_Byte(0XBE);				/* ?á???è?μ */
	
	tplsb = DS18B20_Read_Byte();		 
	tpmsb = DS18B20_Read_Byte(); 
	
	s_tem = tpmsb<<8;
	s_tem = s_tem | tplsb;
	
	if( s_tem < 0 )		/* ?o???è */
		f_tem = (~s_tem+1) * 0.0625;	
	else
		f_tem = s_tem * 0.0625;
	
	return f_tem;
}


/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOE_CLK_ENABLE();
	
  /*Configure GPIO pin : PE5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	
}

void delay_us(uint16_t us)
{
  uint16_t i;
	
	do
  {
    i = 6;
		while(i--)__nop();
  } while (--us);
}

void delay_ms(uint16_t ms)
{
  do
  {
    delay_us(250);
    delay_us(250);
    delay_us(250);
    delay_us(250);
  } while (--ms);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
