/*
 * sensor_liquid.cpp
 *
 *  Created on: Nov 1, 2024
 *      Author: user
 */


#include "sensor_liquidMeas.hpp"
#include "adc.h"

namespace sensor_liquidMeas {


liquidSensor::liquidSensor(ADC_HandleTypeDef *hadc)
{
	hadc_sensor = hadc;
}

void liquidSensor::Measurement_loop(enum unit unit_,float LowSpan,float fullSpan,float VoltOut,uint8_t *getlevel,float *consumption,uint32_t curEpochTime,uint32_t *startEpochtime,uint32_t *endEpochtime)
{
	getLevel(unit_,LowSpan,fullSpan,VoltOut,getlevel);
	consumptionliters(consumption);
	refuelingDetection(curEpochTime,startEpochtime,endEpochtime);
}

liquidSensor::status liquidSensor::getLevel(enum unit unit_,float LowSpan,float fullSpan,float VoltOut,uint8_t *getlevel)
{


	enum status stat = liquidSensor_OK;
	float totalSpan = fullSpan - LowSpan;

	float current_mA;

	  uint32_t adcaverage[10] = {0};
	  uint32_t adcValue = 0;




	if(hadc_sensor == NULL)
	{
		return liquidSensor_ERR;
	}
    for(int i = 0; i< 10; i++)
    {
        CheckError(HAL_ADC_Start(hadc_sensor),liquidSensor_ERR,HAL_OK);
        CheckError(HAL_ADC_PollForConversion(hadc_sensor, 10000),liquidSensor_ERR,HAL_OK); //take the value
        adcaverage[i] = HAL_ADC_GetValue(hadc_sensor);
    }
    HAL_ADC_Stop(hadc_sensor);
    for(int i = 0; i< 10; i++)
    {
    	adcValue = adcValue +  adcaverage[i];
    }
    adcValue = adcValue/10;
    float voltage = (adcValue / Resolution) * Vref;
    current_mA = ((voltage/VoltOut) *  (max_cur - min_cur)) + min_cur;
    fuel_level = (current_mA - min_cur) / (max_cur - min_cur) * totalSpan;
    tank_volumeLiters = (fuel_level/totalSpan) * max_liters;
	*getlevel = (tank_volumeLiters/max_liters) * 100; //get percentage

	  return stat;
}

void liquidSensor::consumptionliters(float *consumption)
{

    uint32_t current_time = HAL_GetTick();

    if(prev_volumeLiters < tank_volumeLiters)
    {
        prev_volumeLiters = tank_volumeLiters;
        last_consumption_time = current_time;

        return;
    }

    if (current_time - last_consumption_time >= 3600000) { // 1hour
    	*consumption = prev_volumeLiters - tank_volumeLiters;

        // Update for next interval
        prev_volumeLiters = tank_volumeLiters;
        last_consumption_time = current_time;
    }

}

void liquidSensor::refuelingDetection(uint32_t curEpochTime,uint32_t *startEpochtime,uint32_t *endEpochtime)
{
	current_volume_change = tank_volumeLiters;
    if(prev_volume_change < 0)
    {
    	prev_volume_change = current_volume_change;

        return;
    }

    // Calculate the change in volume
  volume_change = current_volume_change - prev_volume_change;

     refueling_start_time = HAL_GetTick();


  if (!refueling_active) {
	    // Detect start of refueling
      if (volume_change > refueling_threshold) {
    	  refueling_active = 1;
    	  refueling_prev_time = refueling_start_time;
    	  *startEpochtime = curEpochTime;
      }
  }
  else
  {
	  if (volume_change <= 0) {
		  if ((refueling_start_time - refueling_prev_time) >= refuel_stabilize_time) {

			  refueling_active = 0;
			  *endEpochtime = curEpochTime;

		  }
	  }
  }

  // Update prev_volume for the next iteration
	prev_volume_change = current_volume_change;

}


}