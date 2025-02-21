/*
 * sensor_liquid.cpp
 *
 *  Created on: Nov 1, 2024
 *      Author: user
 */

#include "sensor_liquidMeas.hpp"
#include "adc.h"
#include "cmath"

namespace sensor_liquidMeas {

liquidSensor::liquidSensor(ADC_HandleTypeDef *hadc, float LowSpan,
		float FullSpan, float VoltOut) :
		LiquidMeasSemaphore() {
	hadc_sensor = hadc;
	LiquidMeasSemaphore.semaphoreCreate();

	InternalLowSpan = LowSpan;
	InternalFullSpan = FullSpan;
	InternalVoltOut = VoltOut;
}

void liquidSensor::Measurement_loop(uint8_t *getlevel, float *consumption,
		uint32_t curEpochTime, uint32_t *startEpochtime,
		uint32_t *endEpochtime) {
	getLevel(getlevel);
	consumptionliters(consumption);
	refuelingDetection(curEpochTime, startEpochtime, endEpochtime);
}

liquidSensor::status liquidSensor::getLevel(uint8_t *getlevel) {

	enum status stat = liquidSensor_OK;
	if (LiquidMeasSemaphore.semaphoreTake(1000)
			== System_Rtos::freertos_semaphore::semaphore_recived) {
		float totalSpan = InternalFullSpan - InternalLowSpan;

		float current_mA;
		uint32_t adcValue = 0;

		if (hadc_sensor == NULL) {
			return liquidSensor_ERR;
		}
		for (int i = 0; i < 500; i++) {
			CheckError(HAL_ADC_Start(hadc_sensor), liquidSensor_ERR, HAL_OK);
			CheckError(HAL_ADC_PollForConversion(hadc_sensor, 10000),
					liquidSensor_ERR, HAL_OK); //take the value
			adcValue += HAL_ADC_GetValue(hadc_sensor);
		}
		HAL_ADC_Stop(hadc_sensor);

		adcValue = adcValue / 500;
		float voltage = (adcValue / Resolution) * Vref;
		current_mA = ((voltage / InternalVoltOut) * (max_cur - min_cur)) + min_cur;
		fuel_level = (current_mA - min_cur) / (max_cur - min_cur) * totalSpan;

		//tank_volumeLiters = (fuel_level / totalSpan) * max_liters;
		//*getlevel = (tank_volumeLiters / max_liters) * 100; //get percentage

		if(tanktype)
		{
			tank_volumeLiters = (width * length * fuel_level) * 1000;
		}
		else
		{
			tank_volumeLiters = (PI_cal * pow(radius,2) * fuel_level) * 1000;
		}


		*getlevel = (tank_volumeLiters / max_liters) * 100; //get percentage

		LiquidMeasSemaphore.semaphoreGive();
	} else {
		stat = liquidSensor_ERR;
	}

	return stat;
}

void liquidSensor::consumptionliters(float *consumption) {
	if (LiquidMeasSemaphore.semaphoreTake(1000)
			== System_Rtos::freertos_semaphore::semaphore_recived) {
		uint32_t current_time = HAL_GetTick();

		if (prev_volumeLiters < tank_volumeLiters) {
			prev_volumeLiters = tank_volumeLiters;
			last_consumption_time = current_time;
			LiquidMeasSemaphore.semaphoreGive();
			return;
		}

		if (current_time - last_consumption_time >= 3600000) { // 1hour
			*consumption = prev_volumeLiters - tank_volumeLiters;

			// Update for next interval
			prev_volumeLiters = tank_volumeLiters;
			last_consumption_time = current_time;
		}

		LiquidMeasSemaphore.semaphoreGive();

	}

}

void liquidSensor::refuelingDetection(uint32_t curEpochTime,
		uint32_t *startEpochtime, uint32_t *endEpochtime) {
	if (LiquidMeasSemaphore.semaphoreTake(1000) == System_Rtos::freertos_semaphore::semaphore_recived) {
		current_volume_change = tank_volumeLiters;

		refueling_start_time = HAL_GetTick();
		refueling_detect_start_time = refueling_start_time;

		if(refueling_detect_prev_time <= 0)
		{
			refueling_detect_prev_time = refueling_detect_start_time;
		}


		if (prev_volume_change < 0.0) {
			prev_volume_change = current_volume_change;
			LiquidMeasSemaphore.semaphoreGive();
			return;
		}

		// Calculate the change in volume
		volume_change = static_cast<int32_t>(current_volume_change - prev_volume_change);



		if (!refueling_active) {
			// Detect start of refueling
			if (volume_change > 0 && ((refueling_detect_start_time - refueling_detect_prev_time) >= refuel_detect_stabilize_time)) {
				refueling_active = 1;
				refueling_prev_time = refueling_start_time;
				*startEpochtime = curEpochTime;
				//*endEpochtime = 0; //make it zero so we know we are refueling now to erase previous end time so user dont get confused
			}
			else if(volume_change <= 0)
			{
				refueling_detect_prev_time = refueling_detect_start_time;
			}
		} else {
			  if((volume_change <= 0) && ((refueling_start_time - refueling_prev_time) >= refuel_stabilize_time)) {
					refueling_active = 0;
					*endEpochtime = curEpochTime;
				}
			  else if(volume_change > 0) {
				refueling_prev_time = refueling_start_time;
			}
		}

		// Update prev_volume for the next iteration
		prev_volume_change = current_volume_change;

		LiquidMeasSemaphore.semaphoreGive();
	}

}


void liquidSensor::setTankType(uint8_t tanktype_)
{
	if (LiquidMeasSemaphore.semaphoreTake(30000)
			== System_Rtos::freertos_semaphore::semaphore_recived) {
		tanktype = tanktype_;
		LiquidMeasSemaphore.semaphoreGive();
	}
}
void liquidSensor::setTankMaxLiters(float max_liters_)
{
	if (LiquidMeasSemaphore.semaphoreTake(30000)
			== System_Rtos::freertos_semaphore::semaphore_recived) {
		max_liters = max_liters_;
		LiquidMeasSemaphore.semaphoreGive();
	}
}
void liquidSensor::setTankWidth(float width_)
{
	if (LiquidMeasSemaphore.semaphoreTake(30000)
			== System_Rtos::freertos_semaphore::semaphore_recived) {
		width = width_;
		LiquidMeasSemaphore.semaphoreGive();
	}
}
void liquidSensor::setTankLength(float length_)
{
	if (LiquidMeasSemaphore.semaphoreTake(30000)
			== System_Rtos::freertos_semaphore::semaphore_recived) {
		length = length_;
		LiquidMeasSemaphore.semaphoreGive();
	}
}
void liquidSensor::setTankRadius(float radius_)
{
	if (LiquidMeasSemaphore.semaphoreTake(30000)
			== System_Rtos::freertos_semaphore::semaphore_recived) {
		radius = radius_;
		LiquidMeasSemaphore.semaphoreGive();
	}
}

void liquidSensor::setParameters(float LowSpan, float FullSpan) {
	if (LiquidMeasSemaphore.semaphoreTake(30000)
			== System_Rtos::freertos_semaphore::semaphore_recived) {
		InternalLowSpan = LowSpan;
		InternalFullSpan = FullSpan;
		LiquidMeasSemaphore.semaphoreGive();
	}
}

void liquidSensor::getParameters(float *LowSpan, float *FullSpan) {
	if (LiquidMeasSemaphore.semaphoreTake(30000)
			== System_Rtos::freertos_semaphore::semaphore_recived) {
		*LowSpan = InternalLowSpan;
		*FullSpan = InternalFullSpan;
		LiquidMeasSemaphore.semaphoreGive();
	}
}

}
