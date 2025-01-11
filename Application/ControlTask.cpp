/*
 * ControlTask.cpp
 *
 *  Created on: Nov 8, 2024
 *      Author: user
 */

#include "app_main.hpp"
#include "System/System_Rtos.hpp"
#include "System/System_sys.hpp"
#include "Sensor/sensor_liquidMeas.hpp"
#include "Sensor/sensor_TempHumd.hpp"
#include "Sensor/sensor_pzem.hpp"
#include "Sensor/sensor_DcHall.hpp"
#include "i2c.h"
#include "rtc.h"
#include "Init.hpp"
#include "cstdio"
#include "cstring"
#include "cstdlib"
#include "gpio.h"

RTC_DateTypeDef sDate;
RTC_TimeTypeDef sTime;

System_sys::Parsing_Checking parsing;

uint32_t timestamp;
char time[20];
char date[20];
char timezone[5];
char months[14][5] = { "NULL", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
		"Aug", "Sep", "Oct", "Nov", "Dec", };

uint8_t batt_charging_status;

// Adjust these as needed:
static constexpr float SOC_LOW  = 20.0f;  // Turn generator ON if SoC < 20%
static constexpr float SOC_HIGH  = 95.0f; // If battery SoC > 85%, we might turn generator OFF



uint8_t batt_chargetimekeeping_Flag = 0;

char batTime[20];

uint8_t sourceStatus;

uint32_t DisplayChargeStartTime = 0;
uint32_t DisplayChargeEndTime = 0;


static void TurnOffGenerator()
{
	  HAL_GPIO_WritePin(RelayCont_GPIO_Port, RelayCont_Pin, GPIO_PIN_RESET);
}

static void TurnOnGenerator()
{
	  HAL_GPIO_WritePin(RelayCont_GPIO_Port, RelayCont_Pin, GPIO_PIN_SET);
}

static void SwitchingLoadLogic(struct ControlData_Queue *ControlData)
{


//also track charging time here then

	if(ControlData->SoC <= SOC_LOW)
	{
		TurnOnGenerator();
		ControlData->SelectedSource = static_cast<uint8_t>(sources::Generator);
		ControlData->batteryChargeDischargeStartTime[0] = ControlData->timestamp;
		DisplayChargeStartTime = ControlData->batteryChargeDischargeStartTime[0];
	}
	else if(ControlData->SoC <= SOC_HIGH)
	{
		TurnOffGenerator();
		ControlData->SelectedSource = static_cast<uint8_t>(sources::Battery);
		ControlData->batteryChargeDischargeEndTime[0] = ControlData->timestamp;
		DisplayChargeEndTime = ControlData->batteryChargeDischargeEndTime[0];
	}





}

uint8_t getSourceState()
{
	return sourceStatus;
}

uint32_t getChargeTimestamp()
{
   return DisplayChargeStartTime;
}

uint32_t getDischargeTimestamp()
{
	  return DisplayChargeEndTime;
}


void ControlTask(void *pvParameters) {



	struct ControlData_Queue ControlData = { 0 };

	struct SoCData_Queue SoCData = { 0 };

	std::strcpy(ControlData.uniqueID, UniqueID::GetUid());

	ControlData.SelectedSource = static_cast<uint8_t>(sources::Battery);

	sourceStatus = ControlData.SelectedSource;
	sensor_pzem::PZEM_004T::PZEM PZEM1_Data;
	sensor_pzem::PZEM_004T::PZEM PZEM2_Data;
	sensor_pzem::PZEM_004T::PZEM PZEM3_Data;

	TurnOffGenerator();



	while (1) {

		stmRTC.getTime(nullptr, nullptr, &ControlData.timestamp);

		liquidSensor.Measurement_loop(&ControlData.fuelPer,
				&ControlData.fuelConsp, ControlData.timestamp,
				&ControlData.refuelingStartTime, &ControlData.refuelingEndTime);

		PZEM1.read(&PZEM1_Data);
		PZEM2.read(&PZEM2_Data);
		PZEM3.read(&PZEM3_Data);

		ControlData.V_1 = PZEM1_Data.voltage;
		ControlData.I_1 = PZEM1_Data.current;
		ControlData.P_1 = PZEM1_Data.power;
		ControlData.V_2 = PZEM2_Data.voltage;
		ControlData.I_2 = PZEM2_Data.current;
		ControlData.P_2 = PZEM2_Data.power;
		ControlData.V_3 = PZEM3_Data.voltage;
		ControlData.I_3 = PZEM3_Data.current;
		ControlData.P_3 = PZEM3_Data.power;
		ControlData.Freq = PZEM1_Data.frequency;



		ControlDataQueue.queueSend(reinterpret_cast<void*>(&ControlData));

		if(SoCDataQueue.queueReceive((void *)&SoCData) == SoCDataQueue.queues_recived)
			{
             /*Do something here when you want to receive something*/
			ControlData.DcVolt = SoCData.BattVolt;
			ControlData.DcCurrent = SoCData.BattCurr;
			ControlData.SoC = SoCData.BattSoC;
			}


		SwitchingLoadLogic(&ControlData);
		sourceStatus = ControlData.SelectedSource;


		HAL_GPIO_TogglePin(alive_led_GPIO_Port, alive_led_Pin);
		System_Rtos::delay(2000);
	}

}
