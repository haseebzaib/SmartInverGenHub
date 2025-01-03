/*
 * sensor_liquid.hpp
 *
 *  Created on: Nov 1, 2024
 *      Author: user
 */

#ifndef SENSOR_SENSOR_LIQUIDMEAS_HPP_
#define SENSOR_SENSOR_LIQUIDMEAS_HPP_

#include "main.h"
#include "System_Rtos.hpp"

namespace sensor_liquidMeas
{

    class liquidSensor {


    public:

    	  enum status
          {
    	     liquidSensor_OK = 0,
    		 liquidSensor_ERR = 1,
    	   };

    	  enum unit
		  {
    		Meter = 0,
			CentiMeter = 1,
			Foot = 2,
			Inch = 3,
    	  };

    	liquidSensor(ADC_HandleTypeDef *hadc,float LowSpan,float FullSpan,float VoltOut);
    	void Measurement_loop(uint8_t *getlevel,float *consumption,uint32_t curEpochTime,uint32_t *startEpochtime,uint32_t *endEpochtime);
    	enum status getLevel(uint8_t *getlevel);
        void consumptionliters(float *consumption);
        void refuelingDetection(uint32_t curEpochTime,uint32_t *startEpochtime,uint32_t *endEpochtime);
        void setParameters(float LowSpan,float FullSpan);
        void getParameters(float *LowSpan,float *FullSpan);


    private:
#define CheckError(func, returnStat, checkStat) \
  if (func != checkStat)          \
  {                            \
	LiquidMeasSemaphore.semaphoreGive();\
    return returnStat;               \
  }

        static constexpr float Resolution = 4096.0;
        static constexpr float Vref = 3.3;
        static constexpr float min_cur = 4.0;
        static constexpr float max_cur = 20.0;
        static constexpr float max_liters = 500;
        static constexpr int32_t refueling_threshold = 5;
        static constexpr uint32_t refuel_stabilize_time = 5000;

        float tank_volumeLiters;
    	float fuel_level;
    	float prev_volumeLiters = -1;
    	int32_t volume_change = 0;
    	float current_volume_change = 0;
    	float prev_volume_change = -1;
    	uint8_t refueling_active = 0;
        uint32_t last_consumption_time = 0;
        uint32_t refueling_start_time = 0;
        uint32_t refueling_prev_time = 0;

        float InternalLowSpan;
        float InternalFullSpan;
        float InternalVoltOut;


    	ADC_HandleTypeDef *hadc_sensor;
    	System_Rtos::freertos_semaphore LiquidMeasSemaphore;





    };



}



#endif /* SENSOR_SENSOR_LIQUIDMEAS_HPP_ */
