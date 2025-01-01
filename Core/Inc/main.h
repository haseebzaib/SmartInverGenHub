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
#define PZEM2_U huart6
#define PZEM3_U huart4
#define INVERTER_U huart3
#define PZEM1_U huart1
#define GSM_U huart2
#define OLEDSpi hspi2
#define ADC1_0_LevelSensor_Pin GPIO_PIN_0
#define ADC1_0_LevelSensor_GPIO_Port GPIOA
#define alive_led_Pin GPIO_PIN_1
#define alive_led_GPIO_Port GPIOA
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
#define DISPLAY_DC_Pin GPIO_PIN_12
#define DISPLAY_DC_GPIO_Port GPIOB
#define DISPLAY_RES_Pin GPIO_PIN_14
#define DISPLAY_RES_GPIO_Port GPIOB
#define Relay_cont_Pin GPIO_PIN_8
#define Relay_cont_GPIO_Port GPIOD
#define DISPLAY_CS_Pin GPIO_PIN_9
#define DISPLAY_CS_GPIO_Port GPIOD
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
#define BTN_UP_Pin GPIO_PIN_1
#define BTN_UP_GPIO_Port GPIOD
#define BTN_ENTR_Pin GPIO_PIN_2
#define BTN_ENTR_GPIO_Port GPIOD
#define BTN_DWN_Pin GPIO_PIN_3
#define BTN_DWN_GPIO_Port GPIOD
#define BTN_RGT_Pin GPIO_PIN_4
#define BTN_RGT_GPIO_Port GPIOD
#define BTN_LFT_Pin GPIO_PIN_5
#define BTN_LFT_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
