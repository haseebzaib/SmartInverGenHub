/*
 * app_main.hpp
 *
 *  Created on: Nov 1, 2024
 *      Author: user
 */

#ifndef APP_MAIN_HPP_
#define APP_MAIN_HPP_

#include <stdint.h>
#include <stdio.h>
#include "main.h"
#include "main.h"
#include "usart.h"
#include "adc.h"
#include "gpio.h"




void app_mainCpp();




//Functions to be used in C files
#ifdef __cplusplus
extern "C"
{
#endif

void app_mainC();  // C function to call into Cpp event loop from main
void My_UART_IRQHandlerC(UART_HandleTypeDef *huart);  // C function to call into Cpp event loop from main
void My_Button_IRQHandlerC();

#ifdef __cplusplus
}
#endif


#endif /* APP_MAIN_HPP_ */
