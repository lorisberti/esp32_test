/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_subghz_phy.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sys_app.h"
#include "usart_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define  UART_DATA_LEN          64 
#define  TRX_SEND_LEN           12
#define  FREQ_MAX               960
#define  FREQ_MIN               150

 
#define  AT_CONFREQ             "AT+CONFREQ"
#define  AT_CONPWER             "AT+CONPWER"

#define  POWER_MAX              22
#define  POWER_INVAL            0x7F

#define  PWER_FREQ              470000000
//#define  printf_debug(...)     do{  HAL_UART1_Write(data_buff,sprintf ((char*)data_buff , __VA_ARGS__)) ;}while(0)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

 uint16_t uart_value = 0;
 
 uint16_t rf_power = 22;
 uint16_t rf_chan = 470;
 

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

extern void RF_Send(uint8_t *p,uint16_t len);
extern void RF_SetChan(uint16_t chan);
extern void RF_SetPower(uint8_t power);
uint16_t  Config_POWER(uint8_t *p,uint16_t len);


/**
  * @brief  Configures key freq.
  * @param   
  * @retval none
  */

/*
**************************************************************************
*
*
*
*/ 

#define  TX_DEV        0
#define  RX_DEV        0
#define BUFF_SIZE      100
uint8_t recv_buff[BUFF_SIZE]={0};
uint8_t g_recv_len=0;
volatile uint8_t g_recv_done_flag=0;

//void RxCpltCallback(uint8_t *rxChar, uint16_t size, uint8_t error)
//{
//	//UTIL_ADV_TRACE_FSend("\r\nsize is %d\r\n",size);
//	//HAL_UART_Transmit(&huart1,rxChar,size,0xff);
//	//UTIL_ADV_TRACE_FSend("\r\nsize is %d\r\n",size);
//	  
//		recv_buff[g_recv_len]=*rxChar;
//		if(recv_buff[g_recv_len]=='\n')
//		{
//			g_recv_done_flag=1;
//		}
//		g_recv_len++;

//  /* USER CODE BEGIN CMD_GetChar_2 */

//  /* USER CODE END CMD_GetChar_2 */

//	
//}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance==USART1)
	{
		g_recv_len=Size;
		g_recv_done_flag=1;
	}

}

/**
  * @brief  Configures key GPIO.
  * @param   
  * @retval none
  */
	
uint16_t  Config_FREQ(uint8_t *p,uint16_t len)
{
    uint32_t  freq = 0;
    if(len >= strlen(AT_CONFREQ))
    {
       if (strncmp((const char *)p, AT_CONFREQ, strlen(AT_CONFREQ)) == 0) 
       {
         if( sscanf(((const char *)p),"AT+CONFREQ=%d\r\n",&freq) == 1)
         {
           if(freq < FREQ_MIN || freq > FREQ_MAX)
           {
                freq = 0;
           }       
         }  
       }
    }
    return freq;
}

/**
  * @brief  Configures key GPIO.
  * @param   
  * @retval none
  */
uint16_t  Config_POWER(uint8_t *p,uint16_t len)
{
    uint32_t  power = POWER_INVAL;
    if(len >= strlen(AT_CONPWER))
    { 
       if (strncmp((const char *)p, AT_CONPWER, strlen(AT_CONPWER)) == 0) 
       {
           if( sscanf(((const char *)p),"AT+CONPWER=%d\r\n",&power) == 1)
           {
               if(power > POWER_MAX)
               {
                    power = POWER_MAX;
               }         
           }
       }
    }
    return power;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  
  uint8_t trx_buff[TRX_SEND_LEN];

  

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_SubGHz_Phy_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_UARTEx_ReceiveToIdle_IT(&huart1,recv_buff,BUFF_SIZE);
//	vcom_ReceiveInit(RxCpltCallback);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
   while (1)
  {
		if(g_recv_done_flag==1)
		{
			
			 if((uart_value = Config_FREQ(recv_buff,g_recv_len))!= 0)
        {       
          rf_chan = uart_value;
          RF_SetChan(rf_chan);
         UTIL_ADV_TRACE_FSend("RF_FREQ is : %d Mhz\r\n",uart_value);           
        } 
        else if((uart_value = Config_POWER(recv_buff,g_recv_len))!= POWER_INVAL)
        {
           rf_power = uart_value;
           RF_SetPower(rf_power);
           UTIL_ADV_TRACE_FSend("RF_PWER is : %d dBm\r\n",uart_value);           
        }
				else
				{
					//Radio.send
					RF_Send(recv_buff,g_recv_len);
				}
			g_recv_done_flag=0;
			g_recv_len=0;
			memset(recv_buff,0,sizeof(recv_buff));
			uart_value=0;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1,recv_buff,BUFF_SIZE);
			
		

//			HAL_UART_Transmit(&huart1,recv_buff,g_recv_len,0xff);
//			g_recv_done_flag=0;
//			g_recv_len=0;
//			memset(recv_buff,0,sizeof(recv_buff));
//			
 
   }
 
    


    /* USER CODE END WHILE */
   // MX_SubGHz_Phy_Process();

    /* USER CODE BEGIN 3 */
  
 }
	return 0;
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSEDiv = RCC_HSE_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 12;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK3|RCC_CLOCKTYPE_HCLK
                              |RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
