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
#include "sensor_DcVolt.hpp"
#include "SOC/SOC.hpp"
#include "i2c.h"
#include "rtc.h"
#include "Init.hpp"
#include "cstdio"
#include "cstring"
#include "cstdlib"
#include "gpio.h"
#include "math.h"
#include "iwdg.h"

float prev_SOC;
float Left_SOC;
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
sensor_pzem::PZEM_004T::PZEM PZEM1_Data = {0};
sensor_pzem::PZEM_004T::PZEM PZEM2_Data = {0};
sensor_pzem::PZEM_004T::PZEM PZEM3_Data = {0};
RTC_DateTypeDef DDate;
RTC_TimeTypeDef DTime;
RTC_TimeTypeDef DTimeCharging_;
RTC_TimeTypeDef DTimeDischarging_;
char ChargingTime[20] = "Null";
char DischargingTime[20] = "Null";
uint8_t flag = 0;

static void TurnOffGenerator()
{
	  HAL_GPIO_WritePin(Relay_cont_GPIO_Port, Relay_cont_Pin, GPIO_PIN_RESET);
}

static void TurnOnGenerator()
{
	  HAL_GPIO_WritePin(Relay_cont_GPIO_Port, Relay_cont_Pin, GPIO_PIN_SET);
}

static void SwitchingLoadLogic(struct ControlData_Queue *ControlData)
{


//also track charging time here then



	if(ControlData->SoC <= SOC_LOW && (ControlData->SelectedSource ==  static_cast<uint8_t>(sources::Battery)))
	{
		TurnOnGenerator();
		ControlData->SelectedSource = static_cast<uint8_t>(sources::Generator);
		ControlData->batteryChargeDischargeStartTime[0] = ControlData->timestamp;
		DisplayChargeStartTime = ControlData->batteryChargeDischargeStartTime[0];
		std::memcpy((RTC_TimeTypeDef *)&DTimeCharging_,(RTC_TimeTypeDef *)&DTime,sizeof(RTC_TimeTypeDef));
	}
	else if(ControlData->SoC >= SOC_HIGH && (ControlData->SelectedSource == static_cast<uint8_t>(sources::Generator)))
	{
		TurnOffGenerator();
		ControlData->SelectedSource = static_cast<uint8_t>(sources::Battery);
		ControlData->batteryChargeDischargeEndTime[0] = ControlData->timestamp;
		DisplayChargeEndTime = ControlData->batteryChargeDischargeEndTime[0];
		std::memcpy((RTC_TimeTypeDef *)&DTimeDischarging_,(RTC_TimeTypeDef *)&DTime,sizeof(RTC_TimeTypeDef));
	}

}

uint8_t getSourceState()
{
	return sourceStatus;
}

void getChargeTimestamp(RTC_TimeTypeDef *DTimeCharging)
{
	std::memcpy((RTC_TimeTypeDef *)DTimeCharging,(RTC_TimeTypeDef *)&DTimeCharging_,sizeof(RTC_TimeTypeDef));
}
void getDischargeTimestamp(RTC_TimeTypeDef *DTimeDischarging)
{
	std::memcpy((RTC_TimeTypeDef *)DTimeDischarging,(RTC_TimeTypeDef *)&DTimeDischarging_,sizeof(RTC_TimeTypeDef));
}

sensor_pzem::PZEM_004T::PZEM getACData1()
{
	return (PZEM1_Data);
}
sensor_pzem::PZEM_004T::PZEM getACData2()
{
	return (PZEM2_Data);
}
sensor_pzem::PZEM_004T::PZEM getACData3()
{
	return (PZEM3_Data);
}


void getACData1(float *voltage,float *current,float *power)
{
	*voltage = PZEM1_Data.voltage;
    *current = (PZEM1_Data.current/10)*2;
    *power   = (PZEM1_Data.power/10)*2;
}
void getACData2(float *voltage,float *current,float *power)
{
	*voltage = PZEM2_Data.voltage;
    *current = (PZEM2_Data.current/10)*2;
    *power   = (PZEM2_Data.power/10)*2;
}
void getACData3(float *voltage,float *current,float *power)
{
	*voltage = PZEM3_Data.voltage;
    *current = (PZEM3_Data.current/10)*2;
    *power   = (PZEM3_Data.power/10)*2;

}


void ControlTask(void *pvParameters) {



	struct ControlData_Queue ControlData = { 0 };

	std::strcpy(ControlData.uniqueID, UniqueID::GetUid());

	ControlData.SelectedSource = static_cast<uint8_t>(sources::Battery);

	sourceStatus = ControlData.SelectedSource;



	TurnOffGenerator();

	getSaveData();



	SOC::CC_Init(flash_data_.SOC, 1);
	DCCurrentSensor.setOffset(flash_data_.currentOffset);

	liquidSensor.setParameters(flash_data_.zeroSpan, flash_data_.fullSpan);
	stmRTC.setTimezone(flash_data_.zone);

	prev_SOC = flash_data_.SOC;

	stmRTC.getTime(&DDate, &DTime, &ControlData.timestamp);
	ControlData.batteryChargeDischargeEndTime[0] = ControlData.timestamp;
	std::memcpy((RTC_TimeTypeDef *)&DTimeDischarging_,(RTC_TimeTypeDef *)&DTime,sizeof(RTC_TimeTypeDef));
    HAL_IWDG_Refresh(&hiwdg); //20second

	while (1) {
		HAL_IWDG_Refresh(&hiwdg); //20second
		stmRTC.getTime(&DDate, &DTime, &ControlData.timestamp);
		AHT20.measure(&ControlData.temp, &ControlData.humid);
		liquidSensor.Measurement_loop(&ControlData.fuelPer,
				&ControlData.fuelConsp, ControlData.timestamp,
				&ControlData.refuelingStartTime, &ControlData.refuelingEndTime);

		PZEM1.read(&PZEM1_Data);
		PZEM2.read(&PZEM2_Data);
		PZEM3.read(&PZEM3_Data);

		getACData1(&ControlData.V_1,&ControlData.I_1,&ControlData.P_1);
		getACData2(&ControlData.V_2,&ControlData.I_2,&ControlData.P_2);
		getACData3(&ControlData.V_3,&ControlData.I_3 ,&ControlData.P_3);

		ControlData.Freq = (PZEM1_Data.frequency + PZEM2_Data.frequency + PZEM3_Data.frequency) / 3;



		DCCurrentSensor.getCurrent(&ControlData.DcCurrent);
		DCVoltageMeasurement.getVoltage(&ControlData.DcVolt);

		SOC::CC_Loop(&ControlData.SoC,ControlData.DcCurrent,ControlData.DcVolt);


		Left_SOC = fabsf(ControlData.SoC - prev_SOC);

		if(Left_SOC > 5) //if SOC greater than this percent, we basically saving eey 5per reduction or increment of SOC
		{
			prev_SOC = ControlData.SoC;
			flash_data_.SOC =  ControlData.SoC;
			SaveData();
		}

		SwitchingLoadLogic(&ControlData);
		sourceStatus = ControlData.SelectedSource;
		ControlDataQueue.queueSend(reinterpret_cast<void*>(&ControlData));
		HAL_GPIO_TogglePin(alive_led_GPIO_Port, alive_led_Pin);
		System_Rtos::delay(100);
        HAL_IWDG_Refresh(&hiwdg); //20second
	}

}
