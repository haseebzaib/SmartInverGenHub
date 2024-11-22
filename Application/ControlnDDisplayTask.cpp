/*
 * ControlnDDisplayTask.cpp
 *
 *  Created on: Nov 8, 2024
 *      Author: user
 */

#include "app_main.hpp"
#include "System/System_Rtos.hpp"
#include "System/System_sys.hpp"
#include "Display/Display_HMI.hpp"
#include "Sensor/sensor_liquidMeas.hpp"
#include "Sensor/sensor_TempHumd.hpp"
#include "Inverter/Invertercmd.hpp"
#include "Init.hpp"
#include "i2c.h"
#include "rtc.h"
#include "cstdio"
#include "cstring"
#include "cstdlib"

RTC_DateTypeDef sDate;
RTC_TimeTypeDef sTime;



sensor_liquidMeas::liquidSensor liquidSensor(&hadc1);
sensor_TempHumd::AHT20 AHT20(&hi2c1, 0x38);
HMI::DWIN LCD(&HMI_UART5);

System_sys::Parsing_Checking parsing;

uint32_t timestamp;
char time[20];
char date[20];
char timezone[5];
char months[14][5] = { "NULL", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
		"Aug", "Sep", "Oct", "Nov", "Dec", };



uint8_t batt_charging_status;


static constexpr uint8_t AC_charge_on = 0x05;
static constexpr uint8_t SCC_charge_on = 0x06;
static constexpr uint8_t SCC_AC_charge_on = 0x07;


uint8_t batt_chargetimekeeping_Flag = 0;


char batTime[20];



void ControlnDDisplayTask(void *pvParameters) {

	struct ModemData_Queue ModemData = { 0 };

	struct ControlData_Queue ControlData = { 0 };

	InverterData_Queue InverterData = { 0 };

	System_Rtos::delay(1000);
	LCD.Startup();

	parsing.convertEpochToSTMTime(&hrtc,1732186868,5);


	std::strcpy(ControlData.uniqueID, UniqueID::GetUid());

	while (1) {

		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
		ControlData.timestamp = parsing.convertToEpoch(sDate, sTime, 5);



		std::sprintf(time, "%02d:%02d:%02d", sTime.Hours, sTime.Minutes,
				sTime.Seconds);
		std::sprintf(date, "%02d-%s-%02d", sDate.Date, months[sDate.Month],
				sDate.Year);

		if (ModemDataQueue.queueReceive((void*) &ModemData)
				== ModemDataQueue.queues_recived) {

			LCD.SendTextData(LCD.Network, ModemData.networkStat);
			LCD.SendTextData(LCD.SigQ, ModemData.quality);
			LCD.SendTextData(LCD.Net,ModemData.internet == 1 ? const_cast<char *>("Conn") : const_cast<char *>("DisConn"));
		}

		if (InverterDataQueue.queueReceive((void*) &InverterData) == InverterDataQueue.queues_recived) {


			ControlData.batteryLevel = std::atoi(const_cast<const char *>(InverterData.inverterData_qpigs[static_cast<int>(inverter_data_qpigs_cmd::batt_cap)]));

			parsing.binarystringToUint8(InverterData.inverterData_qpigs[static_cast<int>(inverter_data_qpigs_cmd::dev_stat_1)], &batt_charging_status);
            if(InverterData.device_mode == 3)
            {
            	LCD.SendTextData(LCD.source_bat,  const_cast<char *>("OFF"));
            	LCD.SendTextData(LCD.source_gen, const_cast<char *>("ON"));
            	LCD.SendTextData(LCD.source_sol, const_cast<char *>("OFF"));
            }
            else if(InverterData.device_mode == 4)
            {
            	LCD.SendTextData(LCD.source_bat,  const_cast<char *>("ON"));
            	LCD.SendTextData(LCD.source_gen, const_cast<char *>("OFF"));
            	LCD.SendTextData(LCD.source_sol, const_cast<char *>("OFF"));
            }
            else {
            	LCD.SendTextData(LCD.source_bat,  const_cast<char *>("OFF"));
            	LCD.SendTextData(LCD.source_gen, const_cast<char *>("OFF"));
            	LCD.SendTextData(LCD.source_sol, const_cast<char *>("OFF"));
            }


			LCD.SendNumericData(LCD.Battery,static_cast<uint16_t>(ControlData.batteryLevel));

		}


		if((batt_charging_status & 0x07) == SCC_AC_charge_on
				|| (batt_charging_status & 0x07) == SCC_charge_on || (batt_charging_status & 0x07) == AC_charge_on)
		{
			if(!batt_chargetimekeeping_Flag)
			{
				batt_chargetimekeeping_Flag = 1;
				ControlData.batteryChargeStartTime = ControlData.timestamp;
				std::sprintf(batTime, "%02d:%02d:%02d", sTime.Hours, sTime.Minutes,
						sTime.Seconds);
				LCD.SendTextData(LCD.chrgT,  batTime);
			}
		}
		else
		{
			if(batt_chargetimekeeping_Flag)
			{
				batt_chargetimekeeping_Flag = 0;
				ControlData.batteryChargeEndTime = ControlData.timestamp;
				std::sprintf(batTime, "%02d:%02d:%02d", sTime.Hours, sTime.Minutes,
							sTime.Seconds);
				LCD.SendTextData(LCD.dchgT, batTime);
			}
		}


		liquidSensor.Measurement_loop(sensor_liquidMeas::liquidSensor::Meter,
				0.0, 10.0, 3.3, &ControlData.fuelPer, &ControlData.fuelConsp,
				ControlData.timestamp, &ControlData.refuelingStartTime,
				&ControlData.refuelingEndTime);
		AHT20.measure(&ControlData.temp, &ControlData.humid);


		LCD.SendTextData(LCD.Time, time);
		LCD.SendTextData(LCD.Date, date);

		LCD.SendNumericData(LCD.TankLevel,
				static_cast<uint16_t>(ControlData.fuelPer));

		LCD.SendNumericData(LCD.Temperature,
				static_cast<uint16_t>(ControlData.temp));
		LCD.SendNumericData(LCD.Humidity,
				static_cast<uint16_t>(ControlData.humid));

		ControlDataQueue.queueSend(reinterpret_cast<void*>(&ControlData));

		HAL_GPIO_TogglePin(alive_led_GPIO_Port, alive_led_Pin);
		System_Rtos::delay(150);
	}

}
