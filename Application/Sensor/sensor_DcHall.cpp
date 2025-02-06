/*
 * sensor_DcHall.cpp
 *
 *  Created on: Jan 10, 2025
 *      Author: user
 */


#include "adc.h"
#include "sensor_DcHall.hpp"



namespace sensor_DcHall {





DcHall::DcHall(ADC_HandleTypeDef *hadc) : DcHallSemaphore()
{

	hadc_sensor = hadc;
	DcHallSemaphore.semaphoreCreate();
	RawCurrent = 0.0;
	emaCurrent = 0.0;
	GetCurrent = 0.0;
	offset_system = 0.002f;

}

float DcHall::getOffset()
{
  return offset_system;
}

void  DcHall::setOffset(float offset)
{
	offset_system = offset;
}

float DcHall::getCurrent()
{
	return GetCurrent;
}

DcHall::status DcHall::getCurrent(float *DcCurrent)
{

	enum status stat = DcHall_OK;
	//uint32_t adcaverage[samples] = {0};
	uint32_t adcValue = 0;

	if (DcHallSemaphore.semaphoreTake(4000)
			== System_Rtos::freertos_semaphore::semaphore_recived) {
		if(hadc_sensor == NULL)
		{
			return DcHall_ERR;
		}
	    for(uint32_t i = 0; i< samples; i++)
	    {
	        CheckError(HAL_ADC_Start(hadc_sensor),DcHall_ERR,HAL_OK);
	        CheckError(HAL_ADC_PollForConversion(hadc_sensor, 15000),DcHall_ERR,HAL_OK); //take the value
	        adcValue += HAL_ADC_GetValue(hadc_sensor);
	    }
	    HAL_ADC_Stop(hadc_sensor);


	    adcValue = adcValue/samples;

	    float voltage = ((adcValue / Resolution) * Vref) - offset_system ;


	    float voltageOffset = voltage - midpoint;   // relative to midpoint
	    RawCurrent = voltageOffset / sensitivity;  // I = dV / sensitivity

//	    /emaCurrent = alpha * RawCurrent + (1.0f - alpha) * emaCurrent;

	    GetCurrent = RawCurrent;
	    *DcCurrent = RawCurrent;

		DcHallSemaphore.semaphoreGive();

	}


	  return stat;
}


}

