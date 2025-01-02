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
#include "System_rtc.hpp"

void ModemTask(void * pvParameters);
void ControlTask(void * pvParameters);
void DisplayTask(void *pvParameters);


#define _StackSize_Modem 512
#define _StackSize_Control 512
#define _StackSize_Display 512


extern System_Rtos::freertos_Tasks ModemTaskHandler;
extern System_Rtos::freertos_Tasks ControlTaskHandler;
extern System_Rtos::freertos_Tasks DisplayTaskHandler;


extern System_Rtos::freertos_queues ModemDataQueue;
extern System_Rtos::freertos_queues ControlDataQueue;


extern System_rtc::stmRTC stmRTC;


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
		        "\"batteryLevel\":\"%d\","\
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



namespace UniqueID {


void CharToHexasc(uint8_t *pPos, uint8_t Wert);

char *GetUid();

}

#endif /* INIT_HPP_ */
