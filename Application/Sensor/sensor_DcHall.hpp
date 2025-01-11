/*
 * sensor_DcHall.hpp
 *
 *  Created on: Jan 10, 2025
 *      Author: user
 */

#ifndef SENSOR_SENSOR_DCHALL_HPP_
#define SENSOR_SENSOR_DCHALL_HPP_

#include "main.h"
#include "System_Rtos.hpp"


namespace sensor_DcHall
{


class DcHall {

public:

	  enum status
    {
		  DcHall_OK = 0,
		  DcHall_ERR = 1,
	   };

	DcHall(ADC_HandleTypeDef *hadc);

	enum status getCurrent(float *DcCurrent); //in Ampere



private:
#define CheckError(func, returnStat, checkStat) \
  if (func != checkStat)          \
  {                            \
	DcHallSemaphore.semaphoreGive();\
    return returnStat;               \
  }

	  static constexpr float Vref = 3.3f;
	  static constexpr float Resolution = 4096.0f;
	  static constexpr float midpoint = 1.650f;
	  static constexpr float sensitivity = 0.0048f; //midpoint / current = 1.65V / 300A = 5.5mV/A
	  static constexpr float offset_system = 0.002f;
	   float RawCurrent;
	   float emaCurrent ;
	   static constexpr float alpha = 0.3f;


	ADC_HandleTypeDef *hadc_sensor;
	System_Rtos::freertos_semaphore DcHallSemaphore;





};



}



#endif /* SENSOR_SENSOR_DCHALL_HPP_ */
