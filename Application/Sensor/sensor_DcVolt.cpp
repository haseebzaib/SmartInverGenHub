/*
 * sensor_DcVolt.cpp
 *
 *  Created on: Jan 11, 2025
 *      Author: user
 */

#include "adc.h"
#include "sensor_DcVolt.hpp"

namespace sensor_DcVolt
{


DcVolt::DcVolt(ADC_HandleTypeDef *hadc) : DcVoltSemaphore()
{

	hadc_sensor = hadc;
	DcVoltSemaphore.semaphoreCreate();

}


DcVolt::status DcVolt::getVoltage(float *DcVolt)
{
	const uint32_t samples = 50;
	enum status stat = DcVolt_OK;
	uint32_t adcValue = 0;

	if (DcVoltSemaphore.semaphoreTake(4000)
			== System_Rtos::freertos_semaphore::semaphore_recived) {

		if(hadc_sensor == NULL)
		{
			return DcVolt_ERR;
		}
	    for(uint32_t i = 0; i< samples; i++)
	    {
	        CheckError(HAL_ADC_Start(hadc_sensor),DcVolt_ERR,HAL_OK);
	        CheckError(HAL_ADC_PollForConversion(hadc_sensor, 15000),DcVolt_ERR,HAL_OK); //take the value
	        adcValue += HAL_ADC_GetValue(hadc_sensor);
	    }

	    adcValue = adcValue/samples;


	    *DcVolt = (float)(( adcValue * (Vref / Resolution))*ratio) - offset_system ;	// 12 bit

		DcVoltSemaphore.semaphoreGive();

	}






	return stat;
}

}
