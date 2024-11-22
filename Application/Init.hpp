/*
 * Init.hpp
 *
 *  Created on: Nov 8, 2024
 *      Author: user
 */

#ifndef INIT_HPP_
#define INIT_HPP_

//#include "main.h"
//#include "app_main.hpp"
#include "System/System_Rtos.hpp"


void ModemTask(void * pvParameters);
void InverterTask(void * pvParameters);
void ControlnDDisplayTask(void * pvParameters);



#define _StackSize_Modem 512
#define _StackSize_Inverter 512
#define _StackSize_ControlnDDisplay 512


extern System_Rtos::freertos_Tasks ModemTaskHandler;
extern System_Rtos::freertos_Tasks InverterTaskHandler;
extern System_Rtos::freertos_Tasks ControlnDDisplayTaskHandler;

extern System_Rtos::freertos_queues ModemDataQueue;
extern System_Rtos::freertos_queues ControlDataQueue;
extern System_Rtos::freertos_queues InverterDataQueue;


struct ModemData_Queue {
	//char time[10];
	//char date[10];
	//char timezone[5];
	//uint32_t timestamp;

	char pin[15];
	char networkStat[15];
	char networkStatpdp[15];
	char quality[10];
	char apn[20];
	char serverAddr[100];
	char mqttPubTopic[20];
	char mqttSubTopic[20];
	uint8_t internet;
	uint8_t mqtt_client_index;
};


static constexpr char networkInfo[2][12] = {
		"Reg-Home",
		"Reg-Roaming"
};

#define sendbufMQTT \
		"{" \
		    "\"System Info\":{" \
		        "\"Unique ID\":\"%s\","\
		        "\"Time stamp\":\"%ld\""\
		    "},"\
		    "\"sourceRelatedProperties\":{"\
		        "\"V1\":\"230.5\","\
		        "\"V2\":\"229.8\","\
		        "\"V3\":\"231.0\","\
		        "\"I1\":\"10.2\","\
		        "\"I2\":\"9.8\","\
		        "\"I3\":\"11.1\","\
		        "\"P1\":\"2.3\","\
		        "\"P2\":\"2.2\","\
		        "\"P3\":\"2.5\","\
		        "\"energy\":\"150.75\","\
		        "\"frequency\":\"50.0\""\
		    "},"\
		    "\"source\":{"\
		        "\"sourceIdentification\":\"2\","\
		        "\"startTime\":\"1731929299\","\
		        "\"endTime\":\"1731929299\""\
		    "},"\
		    "\"alarms\":{"\
		        "\"frequency\":\"0\","\
		        "\"overload\":\"1\","\
		        "\"phaseLoss\":\"0\","\
		        "\"fuelTampering\":\"1\""\
		    "},"\
		    "\"fuel\":{"\
		        "\"fuelLevel\":\"%d\","\
		        "\"fuelConsumption\":\"%.2f\","\
		        "\"refuelingStartTime\":\"%ld\","\
		        "\"refuelingEndTime\":\"%ld\""\
		    "},"\
		    "\"battery\":{"\
		        "\"batteryLevel\":\"85\","\
		        "\"batteryChargeStartTime\":\"1731929299\","\
		        "\"batteryChargeEndTime\":\"1731929299\""\
		    "},"\
		    "\"IDs\":{"\
		        "\"fuelTankID\":\"123456789\","\
		        "\"generatorID\":\"987654321\","\
		        "\"batteryID\":\"192837465\""\
		    "},"\
		    "\"temperatureAndHumidity\":{"\
		        "\"temperature\":\"%ld\","\
		        "\"humidity\":\"%ld\""\
		    "}"\
		"}"\




struct ControlData_Queue {
   char uniqueID[13];
   uint32_t timestamp;


   float V_1;
   float V_2;
   float V_3;
   float I_1;
   float I_2;
   float I_3;
   float P_1;
   float P_2;
   float P_3;
   float Energy;
   float Freq;




   uint8_t sourceIdentification;
   uint32_t sourceIdentStartTime;
   uint32_t sourceIdentEndTime;

   uint8_t AlarmFreq;
   uint8_t AlarmOverload;
   uint8_t AlarmPhaseloss;
   uint8_t AlarmFuelTampering;


   uint8_t fuelPer;
   float fuelConsp;
   uint32_t refuelingStartTime;
   uint32_t refuelingEndTime;

   int batteryLevel;
   uint32_t batteryChargeStartTime;
   uint32_t batteryChargeEndTime;


   uint32_t temp;
   uint32_t humid;



};

enum class inverter_data_qpigs_cmd  {
/*1*/	grid_volt = 0,
/*2*/	grid_freq,
/*3*/	ac_out_volt,
/*4*/	ac_out_freq,
/*5*/	ac_out_app_pwr,
/*6*/	ac_out_act_pwr,
/*7*/	out_load_per,
/*8*/	bus_volt,
/*9*/	batt_volt,
/*10*/	batt_charge_curr,
/*11*/	batt_cap,
/*12*/  inv_heat_sink_temp,
/*13*/  pv1_in_curr,
/*14*/  pv1_in_volt,
/*15*/  batt_volt_scc,
/*16*/  batt_dischrg_curr,
/*17*/  dev_stat_1,
/*18*/  batt_volt_offset_fan,
/*19*/  eeprom_ver,
/*20*/  pv1_chrg_pwr,
/*21*/  dev_stat_2,
/*22*/  solar_feed,
/*23*/  country,
/*24*/  solar_feed_pwr,

};

enum class inverter_data_qipgs2_cmd {
/*1*/   pv2_in_curr = 0,
/*2*/   pv2_in_volt,
/*3*/   pv2_chrg_pwr,
};

enum class inverter_data_qpigsN_cmd {
/*1*/   parallel_num = 0,
/*2*/   serialnumber ,
/*3*/   work_mode,
/*4*/   fault_code,
/*5*/	grid_volt,
/*6*/	grid_freq,
/*7*/	ac_out_volt,
/*8*/	ac_out_freq,
/*9*/	ac_out_app_pwr,
/*10*/	ac_out_act_pwr,
/*11*/	out_load_per,
/*12*/	batt_volt,
/*13*/	batt_charge_curr,
/*14*/	batt_cap,
/*15*/  pv1_in_volt,
/*16*/  total_chrg_curr,
/*17*/  total_ac_out_app_pwr,
/*18*/  total_out_act_pwr,
/*19*/  total_ac_out_per,
/*20*/  inverter_stat,
/*21*/  out_mode,
/*22*/  chrg_source_polarity,
/*23*/  max_charge_curr,
/*24*/  max_charge_range,
/*25*/  max_ac_chrg_curr,
/*26*/  pv1_in_curr,
/*27*/  batt_dischrg_curr,
/*28*/  pv2_in_volt,
/*29*/  pv2_in_curr,

};

enum class inverter_data_qpigsN_faultCodes {
	   FanLockedWhenOff = 1,                 // 01 - Fan is locked when inverter is off
	    OverTemperature = 2,                  // 02 - Over temperature
	    BatteryVoltageTooHigh = 3,            // 03 - Battery voltage is too high
	    BatteryVoltageTooLow = 4,             // 04 - Battery voltage is too low
	    OutputShortCircuited = 5,             // 05 - Output short circuited
	    OutputVoltageTooHigh = 6,             // 06 - Output voltage is too high
	    OverloadTimeout = 7,                  // 07 - Overload time out
	    BusVoltageTooHigh = 8,                // 08 - Bus voltage is too high
	    BusSoftStartFailed = 9,               // 09 - Bus soft start failed
	    PVOverCurrent = 10,                   // 10 - PV over current
	    PVOverVoltage = 11,                   // 11 - PV over voltage
	    DCDCOverCurrent = 12,                 // 12 - DCDC over current
	    BatteryDischargeOverCurrent = 13,     // 13 - Battery discharge over current
	    OverCurrent = 51,                     // 51 - Over current
	    BusVoltageTooLow = 52,                // 52 - Bus voltage is too low
	    InverterSoftStartFailed = 53,         // 53 - Inverter soft start failed
	    OverDCVoltageInACOutput = 55,         // 55 - Over DC voltage in AC output
	    CurrentSensorFailed = 57,             // 57 - Current sensor failed
	    OutputVoltageTooLow = 58,             // 58 - Output voltage is too low
	    PowerFeedbackProtection = 60,         // 60 - Power feedback protection
	    FirmwareVersionInconsistent = 71,     // 71 - Firmware version inconsistent
	    CurrentSharingFault = 72,             // 72 - Current sharing fault
	    CANFault = 80,                        // 80 - CAN fault
	    HostLoss = 81,                        // 81 - Host loss
	    SynchronizationLoss = 82,             // 82 - Synchronization loss
	    BatteryVoltageMismatch = 83,          // 83 - Battery voltage detected different
	    ACInputVoltageFrequencyMismatch = 84, // 84 - AC input voltage and frequency detected different
	    ACOutputCurrentUnbalance = 85,        // 85 - AC output current unbalance
	    ACOutputModeSettingMismatch = 86      // 86 - AC output mode setting is different
};


struct InverterData_Queue{
	char inverterData_qpigs[24][10]; //store all the data
	char inverterData_qpigs2[3][10];
	char inverterData_qpigsN[29][15];

	uint8_t device_mode; //1: poweronmode
	                     //2: standbymode
	                     //3: linemode
	                     //4: battery mode
	                     //5: fault mode
	                     //6: shutdown mode

};


namespace UniqueID {


void CharToHexasc(uint8_t *pPos, uint8_t Wert);

char *GetUid();

}

#endif /* INIT_HPP_ */
