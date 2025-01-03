/*
 * sensor_dht.hpp
 *
 *  Created on: Nov 1, 2024
 *      Author: user
 */

#ifndef SENSOR_SENSOR_TEMPHUMD_HPP_
#define SENSOR_SENSOR_TEMPHUMD_HPP_


#include "main.h"
#include "gpio.h"
#include "System/System_Rtos.hpp"


namespace sensor_TempHumd
{

  class DHT
  {
  public:
	  enum status
	  {
		  DHT_OK = 0,
		  DHT_ERR = 1,
	  };

	  DHT(TIM_HandleTypeDef *htim,GPIO_TypeDef *port,uint16_t pin);

	  enum status read(uint8_t *temp,uint8_t *humidity);




  private:
	  struct internal_dht
	  {
			GPIO_TypeDef* port;	///GPIO Port ex:GPIOA
			uint16_t pin; ///GPIO pin ex:GPIO_PIN_2
			TIM_HandleTypeDef *htim; /// timer for measure time ex:htim3
			uint8_t temperature; ///Temperature value
			uint8_t humidty; ///humidity value
	  };
	  struct internal_dht internal_dht_;
	  static constexpr uint8_t OUTPUT = 1;
      static constexpr uint8_t INPUT = 0;
      void set_gpio_mode(uint8_t pMode);
      System_Rtos::freertos_semaphore DHTSemaphore;


  };





  class AHT20 {
public:
	  enum status
	  {
		  AHT_OK = 0,
		  AHT_ERR = 1,
	  };
	  AHT20( I2C_HandleTypeDef *ui2c,uint8_t devAddr,uint32_t timeout = 1000);
      void SoftReset();
      void measure(uint32_t *temp_,uint32_t *humid_);


private:
#define AHT20_ADDRESS           0x38
#define AHT20_SOFT_RESET		0xBA
#define AHT20_TIMEOUT			1000

      uint32_t timeout_;
	  I2C_HandleTypeDef *_aht20_ui2c;
	  uint8_t devAddr_;

	  uint8_t  DATA_State;
	  	uint8_t  DATA_CRCreg;
	  	uint32_t  DATA_Temp;
	  	uint32_t  DATA_Humi;

      enum status check_Device();
	  uint32_t temp();
      uint32_t humid();

      System_Rtos::freertos_semaphore AHT20Semaphore;

  };
}




#endif /* SENSOR_SENSOR_TEMPHUMD_HPP_ */
