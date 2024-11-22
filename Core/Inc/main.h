/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define GSM_USART2 huart2
#define PZEM1_USART1 huart1
#define HMI_UART5 huart5
#define PZEM3_UART4 huart4
#define INVERTER_USART3 huart3
#define PZEM2_USART6 huart6
#define alive_led_Pin GPIO_PIN_13
#define alive_led_GPIO_Port GPIOC
#define ADC1_0_LevelSensor_Pin GPIO_PIN_0
#define ADC1_0_LevelSensor_GPIO_Port GPIOA
#define GSM_USART2_TX_Pin GPIO_PIN_2
#define GSM_USART2_TX_GPIO_Port GPIOA
#define GSM_USART2_RX_Pin GPIO_PIN_3
#define GSM_USART2_RX_GPIO_Port GPIOA
#define DE_Pin GPIO_PIN_15
#define DE_GPIO_Port GPIOE
#define INVERTER_USART3_TX_Pin GPIO_PIN_10
#define INVERTER_USART3_TX_GPIO_Port GPIOB
#define INVERTER_USART3_RX_Pin GPIO_PIN_11
#define INVERTER_USART3_RX_GPIO_Port GPIOB
#define PZEM2_USART6_TX_Pin GPIO_PIN_6
#define PZEM2_USART6_TX_GPIO_Port GPIOC
#define PZEM2_USART6_RX_Pin GPIO_PIN_7
#define PZEM2_USART6_RX_GPIO_Port GPIOC
#define PZEM1_USART1_TX_Pin GPIO_PIN_9
#define PZEM1_USART1_TX_GPIO_Port GPIOA
#define PZEM1_USART1_RX_Pin GPIO_PIN_10
#define PZEM1_USART1_RX_GPIO_Port GPIOA
#define PZEM3_UART4_TX_Pin GPIO_PIN_10
#define PZEM3_UART4_TX_GPIO_Port GPIOC
#define PZEM3_UART4_RX_Pin GPIO_PIN_11
#define PZEM3_UART4_RX_GPIO_Port GPIOC
#define HMI_UART5_TX_Pin GPIO_PIN_12
#define HMI_UART5_TX_GPIO_Port GPIOC
#define HMI_UART5_RX_Pin GPIO_PIN_2
#define HMI_UART5_RX_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
