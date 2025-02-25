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

static float prev_SOC;
static float Left_SOC;

static System_sys::Parsing_Checking parsing;


// Adjust these as needed:
static constexpr float SOC_LOW = 20.0f;  // Turn generator ON if SoC < 20%
static constexpr float SOC_HIGH = 90.0f; // If battery SoC > 85%, we might turn generator OFF
static uint8_t sourceStatus;
static uint32_t DisplayChargeStartTime = 0;
static uint32_t DisplayChargeEndTime = 0;
static sensor_pzem::PZEM_004T::PZEM PZEM1_Data = { 0 };
static sensor_pzem::PZEM_004T::PZEM PZEM2_Data = { 0 };
static sensor_pzem::PZEM_004T::PZEM PZEM3_Data = { 0 };

static RTC_DateTypeDef DDate = {0};
static RTC_TimeTypeDef DTime = {0};

static RTC_TimeTypeDef DTimeCharging_;
static RTC_TimeTypeDef DTimeDischarging_;


static RTC_DateTypeDef set_DDate = {0};
static RTC_TimeTypeDef set_DTime = {0};


uint8_t ManualSourceSelector = 0; // 0 - Batt  1 - Generator
uint8_t ManualSourceSelectorDecider = 0;

static void TurnOffGenerator() {
	HAL_GPIO_WritePin(Relay_cont_GPIO_Port, Relay_cont_Pin, GPIO_PIN_RESET);
}

static void TurnOnGenerator() {
	HAL_GPIO_WritePin(Relay_cont_GPIO_Port, Relay_cont_Pin, GPIO_PIN_SET);
}

static void SwitchingLoadLogic(struct ControlData_Queue *ControlData) {

//also track charging time here then

	if (flash_data_.Auto_Manual == 0) {

		if ((ControlData->SoC <= SOC_LOW || ControlData->DcCurrent < -0.5)
				&& (ControlData->SelectedSource
						== static_cast<uint8_t>(sources::Battery))) {
			TurnOnGenerator();
			ControlData->SelectedSource =
					static_cast<uint8_t>(sources::Generator);
			flash_data_.selectedSource =
					static_cast<uint8_t>(sources::Generator);
			ControlData->batteryChargeDischargeStartTime[0] =
					ControlData->timestamp;
			DisplayChargeStartTime =
					ControlData->batteryChargeDischargeStartTime[0];
			std::memcpy((RTC_TimeTypeDef*) &DTimeCharging_,
					(RTC_TimeTypeDef*) &DTime, sizeof(RTC_TimeTypeDef));
			SaveData();
		} else if ((ControlData->SoC >= SOC_HIGH  || ControlData->DcCurrent > 0.5)
				&& (ControlData->SelectedSource
						== static_cast<uint8_t>(sources::Generator))) {
			TurnOffGenerator();
			ControlData->SelectedSource =
					static_cast<uint8_t>(sources::Battery);
			flash_data_.selectedSource = static_cast<uint8_t>(sources::Battery);
			ControlData->batteryChargeDischargeEndTime[0] =
					ControlData->timestamp;
			DisplayChargeEndTime =
					ControlData->batteryChargeDischargeEndTime[0];
			std::memcpy((RTC_TimeTypeDef*) &DTimeDischarging_,
					(RTC_TimeTypeDef*) &DTime, sizeof(RTC_TimeTypeDef));
			SaveData();
		}

	} else // on Manual
	{
		if (ManualSourceSelector == 1 && ManualSourceSelectorDecider == 1) {
			ManualSourceSelectorDecider = 0;
			TurnOnGenerator();
			ControlData->SelectedSource =
					static_cast<uint8_t>(sources::Generator);
			flash_data_.selectedSource =
					static_cast<uint8_t>(sources::Generator);
			SaveData();
		} else if (ManualSourceSelector == 0
				&& ManualSourceSelectorDecider == 0) {
			ManualSourceSelectorDecider = 1;
			TurnOffGenerator();
			ControlData->SelectedSource =
					static_cast<uint8_t>(sources::Battery);
			flash_data_.selectedSource = static_cast<uint8_t>(sources::Battery);
			SaveData();
		}

	}

}

uint8_t getSourceState() {
	return sourceStatus;
}

void getChargeTimestamp(RTC_TimeTypeDef *DTimeCharging) {
	std::memcpy((RTC_TimeTypeDef*) DTimeCharging,
			(RTC_TimeTypeDef*) &DTimeCharging_, sizeof(RTC_TimeTypeDef));
}
void getDischargeTimestamp(RTC_TimeTypeDef *DTimeDischarging) {
	std::memcpy((RTC_TimeTypeDef*) DTimeDischarging,
			(RTC_TimeTypeDef*) &DTimeDischarging_, sizeof(RTC_TimeTypeDef));
}

sensor_pzem::PZEM_004T::PZEM getACData1() {
	return (PZEM1_Data);
}
sensor_pzem::PZEM_004T::PZEM getACData2() {
	return (PZEM2_Data);
}
sensor_pzem::PZEM_004T::PZEM getACData3() {
	return (PZEM3_Data);
}

void getACData1(float *voltage, float *current, float *power) {
	*voltage = PZEM1_Data.voltage;
	*current = (PZEM1_Data.current / 10) * 2;
	*power = (PZEM1_Data.power / 10) * 2;
}
void getACData2(float *voltage, float *current, float *power) {
	*voltage = PZEM2_Data.voltage;
	*current = (PZEM2_Data.current / 10) * 2;
	*power = (PZEM2_Data.power / 10) * 2;
}
void getACData3(float *voltage, float *current, float *power) {
	*voltage = PZEM3_Data.voltage;
	*current = (PZEM3_Data.current / 10) * 2;
	*power = (PZEM3_Data.power / 10) * 2;

}

void ControlTask(void *pvParameters) {

	struct ControlData_Queue ControlData = { 0 };

	std::strcpy(ControlData.uniqueID, UniqueID::GetUid());

	getSaveData();

	if (flash_data_.selectedSource == 0) {
		TurnOffGenerator();
		ControlData.SelectedSource = static_cast<uint8_t>(sources::Battery);
		ManualSourceSelector = 0;
		ManualSourceSelectorDecider = 1;
	} else {
		TurnOnGenerator();
		ControlData.SelectedSource = static_cast<uint8_t>(sources::Generator);
		ManualSourceSelector = 1;
		ManualSourceSelectorDecider = 0;
	}

	sourceStatus = ControlData.SelectedSource;

	SOC::CC_Init(flash_data_.SOC, 1);
	DCCurrentSensor.setOffset(flash_data_.currentOffset);

	liquidSensor.setParameters(flash_data_.zeroSpan, flash_data_.fullSpan);

	liquidSensor.setTankType(flash_data_.tanktype);
	liquidSensor.setTankMaxLiters(flash_data_.max_liters);
	liquidSensor.setTankWidth(flash_data_.width);
	liquidSensor.setTankLength(flash_data_.length);
	liquidSensor.setTankRadius(flash_data_.radius);

	stmRTC.setTimezone(flash_data_.zone);

	prev_SOC = flash_data_.SOC;


//	set_DTime.Hours = 23;
//	set_DTime.Minutes = 58;
//	set_DTime.Seconds = 35;
////	set_DTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
////	set_DTime.StoreOperation = RTC_STOREOPERATION_RESET;
//
//	set_DDate.WeekDay = RTC_WEEKDAY_MONDAY;
//	set_DDate.Month = RTC_MONTH_JANUARY;
//	set_DDate.Date = 1;
//	set_DDate.Year = 0;
//
//
//	stmRTC.setTime(&set_DDate,&set_DTime, flash_data_.zone);

	stmRTC.getTime(&DDate, &DTime, &ControlData.timestamp);
	ControlData.batteryChargeDischargeEndTime[0] = ControlData.timestamp;
	std::memcpy((RTC_TimeTypeDef*) &DTimeDischarging_,
			(RTC_TimeTypeDef*) &DTime, sizeof(RTC_TimeTypeDef));
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

		getACData1(&ControlData.V_1, &ControlData.I_1, &ControlData.P_1);
		getACData2(&ControlData.V_2, &ControlData.I_2, &ControlData.P_2);
		getACData3(&ControlData.V_3, &ControlData.I_3, &ControlData.P_3);

		ControlData.Freq = (PZEM1_Data.frequency + PZEM2_Data.frequency
				+ PZEM3_Data.frequency) / 3;

		DCCurrentSensor.getCurrent(&ControlData.DcCurrent);
		DCVoltageMeasurement.getVoltage(&ControlData.DcVolt);

		SOC::CC_Loop(&ControlData.SoC, ControlData.DcCurrent,
				ControlData.DcVolt);

		Left_SOC = fabsf(ControlData.SoC - prev_SOC);

		if (Left_SOC > 5) //if SOC greater than this percent, we basically saving eey 5per reduction or increment of SOC
				{
			prev_SOC = ControlData.SoC;
			flash_data_.SOC = ControlData.SoC;
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
