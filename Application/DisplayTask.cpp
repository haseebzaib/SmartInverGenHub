#include "app_main.hpp"
#include "cstdio"
#include "cstring"
#include "cstdlib"
#include "u8g2.h"
#include "spi.h"
#include "Init.hpp"
#include "System/System_Rtos.hpp"
#include "System/System_sys.hpp"
#include "rtc.h"
#include "UI.hpp"
#include "img.hpp"

u8g2_t u8g2_M;


uint8_t u8x8_stm32_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
  {
      /* STM32 supports HW SPI, Remove unused cases like U8X8_MSG_DELAY_XXX & U8X8_MSG_GPIO_XXX */
      switch(msg)
      {
      case U8X8_MSG_GPIO_AND_DELAY_INIT:
          /* Insert codes for initialization */
          break;
      case U8X8_MSG_DELAY_MILLI:
          /* ms Delay */
       	System_Rtos::delay(arg_int);
          break;
      case U8X8_MSG_GPIO_CS:
          /* Insert codes for SS pin control */
            HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, (GPIO_PinState)arg_int);

          break;
      case U8X8_MSG_GPIO_DC:
          /* Insert codes for DC pin control */
            HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, (GPIO_PinState)arg_int);

          break;
      case U8X8_MSG_GPIO_RESET:
          /* Insert codes for RST pin control */
           HAL_GPIO_WritePin(DISPLAY_RES_GPIO_Port, DISPLAY_RES_Pin,  (GPIO_PinState)arg_int);

          break;
      }
      return 1;
  }

uint8_t u8x8_byte_stm32_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
  {
      switch(msg) {
      case U8X8_MSG_BYTE_SEND:
          /* Insert codes to transmit data */
          HAL_SPI_Transmit(&OLEDSpi,(uint8_t *)arg_ptr, arg_int, 5000);
           break;
      case U8X8_MSG_BYTE_INIT:
          /* Insert codes to begin SPI transmission */

          break;
      case U8X8_MSG_BYTE_SET_DC:
          /* Control DC pin, U8X8_MSG_GPIO_DC will be called */
          u8x8_gpio_SetDC(u8x8, arg_int);
          break;
      case U8X8_MSG_BYTE_START_TRANSFER:
          /* Select slave, U8X8_MSG_GPIO_CS will be called */
          u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);
        	System_Rtos::delay(1);

          break;
      case U8X8_MSG_BYTE_END_TRANSFER:
    	  	System_Rtos::delay(1);
          /* Insert codes to end SPI transmission */
          u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
          break;
      default:
          return 0;
      }
      return 1;
  }


void DisplayTask(void *pvParameters)
{

  u8g2_Setup_ssd1309_128x64_noname0_f(&u8g2_M, U8G2_R0, u8x8_byte_stm32_hw_spi,u8x8_stm32_gpio_and_delay);
  u8g2_InitDisplay(&u8g2_M);
  u8g2_ClearBuffer(&u8g2_M);
  u8g2_SetPowerSave(&u8g2_M, 0);
  u8g2_DrawXBM(&u8g2_M,32,0,smartgrid.w,smartgrid.h,smartgrid.img);
  u8g2_SendBuffer(&u8g2_M);
  System_Rtos::delay(2000);


	while(1)
	{

		UI::loop(&u8g2_M);
		System_Rtos::delay(500);

	}

}
