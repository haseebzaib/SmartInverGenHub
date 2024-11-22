/*
 * ModemTask.cpp
 *
 *  Created on: Nov 8, 2024
 *      Author: user
 */

#include "app_main.hpp"
#include "Modem/Modem.hpp"
#include "System/System_Rtos.hpp"
#include "System/System_sys.hpp"
#include "Init.hpp"
#include "cstdio"
#include "cstring"


Modem::simA7672 simA7672(&GSM_USART2);
//System_sys::Parsing_Checking parsing;

struct ModemData_TaskUsage {
 char SetTimeDate[50];
};

struct ModemData_TaskUsage ModemData_TaskUsage;

enum routine {
	net_enable = 0, mqtt_enable = 1, mqtt_connect = 2, mqtt_loop = 3,
};


char command_buffer[255] = {0};


void ModemTask(void *pvParameters) {

	simA7672.init();

	struct ModemData_Queue ModemData = { 0 };

	struct ControlData_Queue ControlData = {0};

	Modem::simA7672::UE_systemInfo cpsiInfo;
	Modem::simA7672::pdp_stat pdpinfo;
	Modem::simA7672::status modeminfo;
	Modem::simA7672::mqtt_flags mqttstartinfo = Modem::simA7672::mqtt_ERR;
	Modem::simA7672::mqtt_flags mqttserverinfo = Modem::simA7672::mqtt_ERR;

	enum routine mqtt_routine = net_enable;


	std::strcpy(ModemData.serverAddr, "tcp://apfp7i6y92d6b-ats.iot.us-east-1.amazonaws.com:8883");
	std::strcpy(ModemData.apn, "zonginternet");
	std::strcpy(ModemData.mqttSubTopic, "TEST1");
	std::strcpy(ModemData.mqttPubTopic, "devicedata");


	uint8_t mqtt_counter = 0;


	//std::strcpy(ModemData_TaskUsage.SetTimeDate, "24/11/15,16:38:55+20");

	//simA7672.setTimeDate(ModemData_TaskUsage.SetTimeDate);



	while (1) {

		//simA7672.getTimeDate(ModemData.time, ModemData.date,
		//		ModemData.timezone);

		//ModemData.timestamp = parsing.convertToEpoch(ModemData.date, ModemData.time, ModemData.timezone);
		simA7672.getSim(ModemData.pin);
		simA7672.getNetwork(ModemData.networkStat);
		simA7672.getNetworkPDP(ModemData.networkStatpdp);
		simA7672.getSignalQ(ModemData.quality);
		simA7672.getUEsystemInfo(&cpsiInfo);
		simA7672.getPDPstatus(&pdpinfo);





		ModemDataQueue.queueSend(reinterpret_cast<void*>(&ModemData));
		System_Rtos::delay(50);


		if(ControlDataQueue.queueReceive((void *)&ControlData) == ControlDataQueue.queues_recived)
			{
             /*Do something here when you want to receive something*/
			}





		switch (mqtt_routine) {

		case net_enable: {

			/*Enable Net support*/

			if ((std::strstr(ModemData.networkStat, networkInfo[0])
					|| std::strstr(ModemData.networkStat, networkInfo[1]))
					&& (std::strstr(ModemData.networkStatpdp, networkInfo[0])
							|| std::strstr(ModemData.networkStatpdp, networkInfo[1]))
					&& cpsiInfo != Modem::simA7672::no_service) {


				modeminfo = simA7672.setAPN(ModemData.apn);

				simA7672.sslversionset();
				simA7672.sslauthset();
				simA7672.sslcacertset();
				simA7672.ssldevicecertset();
				simA7672.ssldevicekeyset();


				if (modeminfo == Modem::simA7672::simA7672_OK) {
	//				simA7672.activate_deactivatePDP(Modem::simA7672::pdp_deactivated);
	//				simA7672.activate_deactivatePDP(Modem::simA7672::pdp_activate);

					if (mqttserverinfo == Modem::simA7672::mqtt_serverConnected) {

						simA7672.mqttDisconnectServer(ModemData.mqtt_client_index);
						mqttserverinfo = Modem::simA7672::mqtt_ERR;
					}

					if (mqttstartinfo == Modem::simA7672::mqtt_OK) {
						simA7672.mqttReleaseClient(ModemData.mqtt_client_index);
						simA7672.mqttStop();
						mqttstartinfo = Modem::simA7672::mqtt_ERR;
					}

					mqtt_routine = mqtt_enable;

				}

			}

			break;
		}

		case mqtt_enable: {

			/*Enable MQTT*/
			if ((std::strstr(ModemData.networkStat, networkInfo[0])
					|| std::strstr(ModemData.networkStat, networkInfo[1]))
					&& (std::strstr(ModemData.networkStatpdp, networkInfo[0])
							|| std::strstr(ModemData.networkStatpdp, networkInfo[1]))
					&& cpsiInfo != Modem::simA7672::no_service
					&& mqttstartinfo != Modem::simA7672::mqtt_OK) {

				simA7672.activate_deactivatePDP(Modem::simA7672::pdp_deactivated);
				simA7672.activate_deactivatePDP(Modem::simA7672::pdp_activate);
				mqttstartinfo = simA7672.mqttStart(&ModemData.mqtt_client_index);
				if (mqttstartinfo == Modem::simA7672::mqtt_OK) {

					mqttstartinfo = simA7672.mqttSetClient(
							ModemData.mqtt_client_index,
							const_cast<char*>("STM32"),1);

					if(mqttstartinfo == Modem::simA7672::mqtt_OK)
					{

						simA7672.mqttsslenable(ModemData.mqtt_client_index);
						mqtt_routine = mqtt_connect;
					}
					else
					{
						simA7672.mqttReleaseClient(ModemData.mqtt_client_index);
						simA7672.mqttStop();
					}

				} else {
					simA7672.mqttReleaseClient(ModemData.mqtt_client_index);
					simA7672.mqttStop();
				}

			}

			break;
		}

		case mqtt_connect: {



			/*Connect to Mqtt Server*/
			if (mqttstartinfo == Modem::simA7672::mqtt_OK
					&& mqttserverinfo != Modem::simA7672::mqtt_serverConnected) {

				mqttserverinfo = simA7672.mqttConnectServer(
						ModemData.mqtt_client_index, ModemData.serverAddr);

				if (mqttserverinfo == Modem::simA7672::mqtt_serverConnected) {

					mqtt_routine = mqtt_loop;

				} else {
					simA7672.mqttDisconnectServer(ModemData.mqtt_client_index);

					mqtt_routine = net_enable;
				}

			}

			break;
		}

		case mqtt_loop: {

			mqttserverinfo = simA7672.getmqttconnectionstatus(ModemData.serverAddr);
			mqtt_counter++;
			/*get retries as well as sometimes we can get some issue */
			if (mqttserverinfo != Modem::simA7672::mqtt_serverConnected && mqtt_counter > 3) {
				ModemData.internet = 0;
				simA7672.mqttDisconnectServer(ModemData.mqtt_client_index);
				mqtt_routine = net_enable;
			}
			else if(mqttserverinfo == Modem::simA7672::mqtt_serverConnected )
			{
				ModemData.internet = 1;
				mqtt_counter = 0;
				std::sprintf(reinterpret_cast<char *>(simA7672.Txbuffer),sendbufMQTT,ControlData.uniqueID,ControlData.timestamp,ControlData.fuelPer
						,ControlData.fuelConsp,ControlData.refuelingStartTime,ControlData.refuelingEndTime,
						ControlData.temp,ControlData.humid);
				//simA7672.mqttPubData(ModemData.mqtt_client_index,ModemData.mqttPubTopic,reinterpret_cast<char *>(simA7672.Txbuffer),std::strlen(reinterpret_cast<char *>(simA7672.Txbuffer)));
				if(simA7672.mqttsubTopicAndRead(ModemData.mqtt_client_index, ModemData.mqttSubTopic, command_buffer,255) == Modem::simA7672::mqtt_msgrecv)
				{
                      /*take flag to do something*/
				}
				simA7672.mqttunsubTopic(ModemData.mqtt_client_index,  ModemData.mqttSubTopic);
			}

			break;
		}

		default:
			break;

		}




	}

}

