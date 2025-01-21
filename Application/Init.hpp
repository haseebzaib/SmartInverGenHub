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
#include "Sensor/sensor_pzem.hpp"
#include "Sensor/sensor_liquidMeas.hpp"
#include "Sensor/sensor_TempHumd.hpp"
#include "System_rtc.hpp"
#include "Sensor/sensor_DcHall.hpp"
#include "Sensor/sensor_DcVolt.hpp"
#include "sensor_pzem.hpp"

#define PACKED __attribute__((packed))

void ModemTask(void * pvParameters);
void ControlTask(void * pvParameters);
void DisplayTask(void *pvParameters);
void SoCTask(void *pvParameters);
void SaveData();


uint8_t getSourceState();
void getChargeTimestamp(RTC_TimeTypeDef *DTimeCharging);
void getDischargeTimestamp(RTC_TimeTypeDef *DTimeDischarging);

sensor_pzem::PZEM_004T::PZEM getACData1();
sensor_pzem::PZEM_004T::PZEM getACData2();
sensor_pzem::PZEM_004T::PZEM getACData3();

char *getModemNetwork();
char *getSignalQuality();
char *getModemData();

#define _StackSize_Modem 1024
#define _StackSize_Control 1024
#define _StackSize_Display 1024


extern System_Rtos::freertos_Tasks ModemTaskHandler;
extern System_Rtos::freertos_Tasks ControlTaskHandler;
extern System_Rtos::freertos_Tasks DisplayTaskHandler;
//extern System_Rtos::freertos_Tasks SoCTaskHandler;


extern System_Rtos::freertos_queues ModemDataQueue;
extern System_Rtos::freertos_queues ControlDataQueue;
//extern System_Rtos::freertos_queues SoCDataQueue;


extern System_rtc::stmRTC stmRTC;
extern sensor_liquidMeas::liquidSensor liquidSensor;
extern sensor_TempHumd::AHT20 AHT20;

extern sensor_pzem::PZEM_004T PZEM1;
extern sensor_pzem::PZEM_004T PZEM2;
extern sensor_pzem::PZEM_004T PZEM3;

extern sensor_DcHall::DcHall DCCurrentSensor;
extern sensor_DcVolt::DcVolt DCVoltageMeasurement;

struct ModemData_Queue {
	//char time[10];
	//char date[10];
	//char timezone[5];
	//uint32_t timestamp;

	char pin[15];
	char networkStat[15];
	char networkStatpdp[15];
	char quality[10];
	char apn[40];
	char serverAddr[100];
	char mqttPubTopic[20];
	char mqttSubTopic[20];
	uint8_t internet;
	uint8_t mqtt_client_index;
};

//struct SoCData_Queue {
//
//	float BattVolt;
//	float BattCurr;
//	float BattSoC;
//};

struct PACKED flash_data {
	int8_t zone;
	uint16_t validPattern;
    float zeroSpan;
    float fullSpan;
	float SOC;


};

extern struct flash_data flash_data_;

void getSaveData();


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
		        "\"V1\":\"%.2f\","\
		        "\"V2\":\"%.2f\","\
		        "\"V3\":\"%.2f\","\
		        "\"I1\":\"%.2f\","\
		        "\"I2\":\"%.2f\","\
		        "\"I3\":\"%.2f\","\
		        "\"P1\":\"%.2f\","\
		        "\"P2\":\"%.2f\","\
		        "\"P3\":\"%.2f\","\
		        "\"energy\":\"%.2f\","\
		        "\"frequency\":\"%.2f\""\
		    "},"\
		    "\"source\":{"\
		        "\"sourceIdentification\":\"%d\","\
		        "\"startTime\":\"Null\","\
		        "\"endTime\":\"Null\""\
		    "},"\
		    "\"alarms\":{"\
		        "\"frequency\":\"Null\","\
		        "\"overload\":\"Null\","\
		        "\"phaseLoss\":\"Null\","\
		        "\"fuelTampering\":\"Null\""\
		    "},"\
		    "\"fuel\":{"\
		        "\"fuelLevel\":\"%d\","\
		        "\"fuelConsumption\":\"%.2f\","\
		        "\"refuelingStartTime\":\"%ld\","\
		        "\"refuelingEndTime\":\"%ld\""\
		    "},"\
		    "\"battery\":{"\
		        "\"batteryLevel\":\"%.2f\","\
		        "\"batteryChargeStartTime\":\"%ld\","\
		        "\"batteryChargeEndTime\":\"%ld\""\
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


enum class sources : uint8_t {
	Battery = 0,
	Generator,
	Solar,
};

#define Total_Sources 3

enum class batterystates : uint8_t {
	BatteryCharging = 0,
	BatteryDischarging,
	BatteryIDLE,
};


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



   float DcCurrent;
   float DcVolt;
   float SoC;




   uint8_t SelectedSource;
   uint32_t SelectedSourceStartTime[Total_Sources];
   uint32_t SelectedSourceEndTime[Total_Sources];

   uint8_t AlarmFreq;
   uint8_t AlarmOverload;
   uint8_t AlarmPhaseloss;
   uint8_t AlarmFuelTampering;


   uint8_t fuelPer;
   float fuelConsp;
   uint32_t refuelingStartTime;
   uint32_t refuelingEndTime;

   int batteryLevel;
   uint8_t batterystate;
   uint32_t batteryChargeDischargeStartTime[3];
   uint32_t batteryChargeDischargeEndTime[3];




   uint32_t temp;
   uint32_t humid;



};



namespace UniqueID {


void CharToHexasc(uint8_t *pPos, uint8_t Wert);

char *GetUid();

}

#endif /* INIT_HPP_ */
