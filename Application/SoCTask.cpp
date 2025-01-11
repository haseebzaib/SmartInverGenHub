/*
 * SoCTask.cpp
 *
 *  Created on: Jan 11, 2025
 *      Author: user
 */

#include <SOC.hpp>
#include "Init.hpp"
#include "cstdio"
#include "cstring"
#include "cstdlib"
#include "app_main.hpp"
#include "System/System_Rtos.hpp"
#include "System/System_sys.hpp"
#include "Sensor/sensor_DcHall.hpp"
#include "sensor_DcVolt.hpp"

/*
 * Battery state of charge task
 *
 * */
void SoCTask(void *pvParameters) {


	struct SoCData_Queue SoCData = { 0 };

	SOC::CC_Init(100.0f, 1);
	while (1) {


		DCCurrentSensor.getCurrent(&SoCData.BattCurr);
		DCVoltageMeasurement.getVoltage(&SoCData.BattVolt);


		SOC::CC_Loop(&SoCData.BattSoC,SoCData.BattCurr,SoCData.BattVolt);



		SoCDataQueue.queueSend(reinterpret_cast<void*>(&SoCData));
		System_Rtos::delay(1000);
	}
}
