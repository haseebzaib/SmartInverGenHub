/*
 * sensor_DcVolt.hpp
 *
 *  Created on: Jan 11, 2025
 *      Author: user
 */

#ifndef SENSOR_SENSOR_DCVOLT_HPP_
#define SENSOR_SENSOR_DCVOLT_HPP_


#include "main.h"
#include "System_Rtos.hpp"



namespace sensor_DcVolt
{

class DcVolt {
public:
	  enum status
  {
		  DcVolt_OK = 0,
		  DcVolt_ERR = 1,
	   };

	  DcVolt(ADC_HandleTypeDef *hadc);
	  enum status getVoltage(float *DcVolt); //in Ampere

private:

#define CheckError(func, returnStat, checkStat) \
  if (func != checkStat)          \
  {                            \
	DcVoltSemaphore.semaphoreGive();\
    return returnStat;               \
  }

	  static constexpr float Vref = 3.3f;
	  static constexpr float Resolution = 4096.0f;
	  static constexpr float offset_system = -0.130f;
	  static constexpr float ratio = 15.42; //r1+r2/r2 598.82k/365k
	  ADC_HandleTypeDef *hadc_sensor;
	  System_Rtos::freertos_semaphore DcVoltSemaphore;

};


}


#endif /* SENSOR_SENSOR_DCVOLT_HPP_ */
