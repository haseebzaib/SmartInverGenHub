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
#include "i2c.h"
#include "rtc.h"
#include "Init.hpp"
#include "cstdio"
#include "cstring"
#include "cstdlib"

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

static constexpr uint8_t AC_charge_on = 0x05;
static constexpr uint8_t SCC_charge_on = 0x06;
static constexpr uint8_t SCC_AC_charge_on = 0x07;

uint8_t batt_chargetimekeeping_Flag = 0;

char batTime[20];



void ControlTask(void *pvParameters) {

	struct ModemData_Queue ModemData = { 0 };

	struct ControlData_Queue ControlData = { 0 };



	System_Rtos::delay(1000);


	//parsing.convertEpochToSTMTime(&hrtc, 1732186868, 5);

	std::strcpy(ControlData.uniqueID, UniqueID::GetUid());




	while (1) {



		stmRTC.getTime(nullptr, nullptr, &ControlData.timestamp);

//		std::sprintf(time, "%02d:%02d:%02d", sTime.Hours, sTime.Minutes,
//				sTime.Seconds);
//		std::sprintf(date, "%02d-%s-%02d", sDate.Date, months[sDate.Month],
//				sDate.Year);

		if (ModemDataQueue.queueReceive((void*) &ModemData)
				== ModemDataQueue.queues_recived) {

//			LCD.SendTextData(LCD.Network, ModemData.networkStat);
//			LCD.SendTextData(LCD.SigQ, ModemData.quality);
//			LCD.SendTextData(LCD.Net,
//					ModemData.internet == 1 ?
//							const_cast<char*>("Conn") :
//							const_cast<char*>("DisConn"));
		}

//		if (InverterDataQueue.queueReceive((void*) &InverterData)
//				== InverterDataQueue.queues_recived) {
//
//			ControlData.batteryLevel =
//					std::atoi(
//							const_cast<const char*>(InverterData.inverterData_qpigs[static_cast<int>(inverter_data_qpigs_cmd::batt_cap)]));
//
//			parsing.binarystringToUint8(
//					InverterData.inverterData_qpigs[static_cast<int>(inverter_data_qpigs_cmd::dev_stat_1)],
//					&batt_charging_status);
//			if (InverterData.device_mode == 3) {
////				LCD.SendTextData(LCD.source_bat, const_cast<char*>("OFF"));
////				LCD.SendTextData(LCD.source_gen, const_cast<char*>("ON"));
////				LCD.SendTextData(LCD.source_sol, const_cast<char*>("OFF"));
//			} else if (InverterData.device_mode == 4) {
////				LCD.SendTextData(LCD.source_bat, const_cast<char*>("ON"));
////				LCD.SendTextData(LCD.source_gen, const_cast<char*>("OFF"));
////				LCD.SendTextData(LCD.source_sol, const_cast<char*>("OFF"));
//			} else {
////				LCD.SendTextData(LCD.source_bat, const_cast<char*>("OFF"));
////				LCD.SendTextData(LCD.source_gen, const_cast<char*>("OFF"));
////				LCD.SendTextData(LCD.source_sol, const_cast<char*>("OFF"));
//			}
//
//			ControlData.sourceIdentification = InverterData.device_mode;
//
////			LCD.SendNumericData(LCD.Battery,
////					static_cast<uint16_t>(ControlData.batteryLevel));
//
//			ControlData.V_1 = InverterData.rms_volt1;
//			ControlData.V_2 = InverterData.rms_volt2;
//			ControlData.V_3 = InverterData.rms_volt3;
//
//			ControlData.I_1 = InverterData.pzem1_data.current;
//			ControlData.I_2 = InverterData.pzem2_data.current;
//			ControlData.I_3 = InverterData.pzem3_data.current;
//
//			ControlData.P_1 = InverterData.pzem1_data.power;
//			ControlData.P_2 = InverterData.pzem2_data.power;
//			ControlData.P_3 = InverterData.pzem3_data.power;
//
//			ControlData.Energy = InverterData.pzem1_data.energy;
//
////			LCD.SendFloatData(LCD.phase1, ControlData.V_1);
////			LCD.SendFloatData(LCD.phase2, ControlData.V_2);
////			LCD.SendFloatData(LCD.phase3, ControlData.V_3);
////
////			LCD.SendFloatData(LCD.curr1, ControlData.I_1);
////			LCD.SendFloatData(LCD.curr2, ControlData.I_2);
////			LCD.SendFloatData(LCD.curr3, ControlData.I_3);
////
////			LCD.SendFloatData(LCD.pwr1, ControlData.P_1);
////			LCD.SendFloatData(LCD.pwr2, ControlData.P_2);
////			LCD.SendFloatData(LCD.pwr3, ControlData.P_3);
////
////			LCD.SendFloatData(LCD.energy, ControlData.Energy);
//
//		}

		if ((batt_charging_status & 0x07) == SCC_AC_charge_on
				|| (batt_charging_status & 0x07) == SCC_charge_on
				|| (batt_charging_status & 0x07) == AC_charge_on) {
			if (!batt_chargetimekeeping_Flag) {
				batt_chargetimekeeping_Flag = 1;
				ControlData.batteryChargeStartTime = ControlData.timestamp;
				std::sprintf(batTime, "%02d:%02d:%02d", sTime.Hours,
						sTime.Minutes, sTime.Seconds);
//				LCD.SendTextData(LCD.chrgT, batTime);
			}
		} else {
			if (batt_chargetimekeeping_Flag) {
				batt_chargetimekeeping_Flag = 0;
				ControlData.batteryChargeEndTime = ControlData.timestamp;
				std::sprintf(batTime, "%02d:%02d:%02d", sTime.Hours,
						sTime.Minutes, sTime.Seconds);
//				LCD.SendTextData(LCD.dchgT, batTime);
			}
		}

		liquidSensor.Measurement_loop( &ControlData.fuelPer, &ControlData.fuelConsp,
				ControlData.timestamp, &ControlData.refuelingStartTime,
				&ControlData.refuelingEndTime);
		AHT20.measure(&ControlData.temp, &ControlData.humid);

//		LCD.SendTextData(LCD.Time, time);
//		LCD.SendTextData(LCD.Date, date);
//
//		LCD.SendNumericData(LCD.TankLevel,
//				static_cast<uint16_t>(ControlData.fuelPer));
//
//		LCD.SendNumericData(LCD.Temperature,
//				static_cast<uint16_t>(ControlData.temp));
//		LCD.SendNumericData(LCD.Humidity,
//				static_cast<uint16_t>(ControlData.humid));

		ControlDataQueue.queueSend(reinterpret_cast<void*>(&ControlData));

		HAL_GPIO_TogglePin(alive_led_GPIO_Port, alive_led_Pin);
		System_Rtos::delay(150);
	}

}
