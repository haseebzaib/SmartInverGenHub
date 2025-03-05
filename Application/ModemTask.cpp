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
#include "System_rtc.hpp"
#include "Sensor/sensor_liquidMeas.hpp"
#include "Sensor/sensor_DcHall.hpp"
#include "SOC/SOC.hpp"

Modem::simA7672 simA7672(&GSM_U);

static System_sys::Parsing_Checking parsing;

uint32_t currentTime = 0;
uint32_t prevTime = 0;
uint32_t setTime = 30000; //60 seconds

struct ModemData_Queue ModemData = { 0 };
//System_sys::Parsing_Checking parsing;

struct ModemData_TaskUsage {
	char SetTimeDate[50];
};

struct ModemData_TaskUsage ModemData_TaskUsage;

enum routine {
	net_enable = 0, mqtt_enable = 1, mqtt_connect = 2, mqtt_loop = 3,
};

char command_buffer[255] = { 0 };
char send_command_buffer[255] = { 0 };
uint8_t ModemDataReceived = 0;

#define totalModemCmds 9

using GSMCMDCallback = void (*)();

struct GSMCMDList {

	char cmd[20];
	GSMCMDCallback Callback;

};

static void send_ACK_NACK(uint8_t ACK_NACK) {
	simA7672.mqttPubData(ModemData.mqtt_client_index, ModemData.mqttSubTopic,
			ACK_NACK == 1 ?
					const_cast<char*>("NACK") : const_cast<char*>("ACK"),
			ACK_NACK == 1 ? 4 : 3);
}

static void send_data(char *Data, uint16_t size) {
	simA7672.mqttPubData(ModemData.mqtt_client_index, ModemData.mqttSubTopic,
			Data, size);
}

static void Modem_setTimedate() {
	char outbuf1[20];
	char outbuf2[20];
	uint16_t outlen1 = 0;
	uint16_t outlen2 = 0;
	uint32_t extractedEpoch = 0;
	int8_t extractedTimezone = 0;
	parsing.extractdatainsegments(command_buffer, outbuf1, 20, &outlen1, ':',
			'|');
	parsing.extractdatainsegments(command_buffer, outbuf2, 20, &outlen2, '|',
			'\0');
	extractedEpoch = std::atoi(outbuf1);
	extractedTimezone = std::atoi(outbuf2);
	flash_data_.zone = extractedTimezone;
	send_ACK_NACK(stmRTC.setTime(extractedEpoch, extractedTimezone));
}

static void Modem_setFuelMeasurement() {
	char outbuf1[20];
	char outbuf2[20];
	uint16_t outlen1 = 0;
	uint16_t outlen2 = 0;
	float zeroSpan = 00.0;
	float fullSpan = 00.0;
	parsing.extractdatainsegments(command_buffer, outbuf1, 20, &outlen1, ':',
			'|');
	parsing.extractdatainsegments(command_buffer, outbuf2, 20, &outlen2, '|',
			'\0');
	zeroSpan = std::atof(outbuf1);
	fullSpan = std::atof(outbuf2);
	liquidSensor.setParameters(zeroSpan, fullSpan);
	flash_data_.fullSpan = fullSpan;
	flash_data_.zeroSpan = zeroSpan;
	SaveData();
	send_ACK_NACK(0);
}

static void Modem_setSoCnDCur() {
	char outbuf1[20];
	char outbuf2[20];
	uint16_t outlen1 = 0;
	uint16_t outlen2 = 0;
	float soc = 0.0;
	float currentoffset = 0.0;
	parsing.extractdatainsegments(command_buffer, outbuf1, 20, &outlen1, ':',
			'|');
	parsing.extractdatainsegments(command_buffer, outbuf2, 20, &outlen2, '|',
			'\0');
	soc = std::atof(outbuf1);
	currentoffset = std::atof(outbuf2);
	DCCurrentSensor.setOffset(currentoffset);
	SOC::CC_Init(soc, 1);
	flash_data_.currentOffset = currentoffset;
	flash_data_.SOC = soc;
	SaveData();
	send_ACK_NACK(0);
}

static void Modem_setAutoManualMode() {
	char outbuf1[20];
	uint16_t outlen1 = 0;
	uint8_t Auto_ManualSelector;
	parsing.extractdatainsegments(command_buffer, outbuf1, 20, &outlen1, ':',
			'\0');
	Auto_ManualSelector = std::atoi(outbuf1);
	flash_data_.Auto_Manual = Auto_ManualSelector;
	SaveData();
	send_ACK_NACK(0);
}

static void Modem_setGenerator() {
	char outbuf1[20];
	uint16_t outlen1 = 0;
	uint8_t ManualSourceSelectorOpt;

	if (flash_data_.Auto_Manual == 1) {
		parsing.extractdatainsegments(command_buffer, outbuf1, 20, &outlen1,
				':', '\0');
		ManualSourceSelectorOpt = std::atoi(outbuf1);
		ManualSourceSelector = ManualSourceSelectorOpt;
		ManualSourceSelectorDecider = ManualSourceSelectorOpt;
		send_ACK_NACK(0);
	} else {
		send_ACK_NACK(1);
	}

}

static void Modem_setFixSOCCharging() {
	char outbuf1[20];
	uint16_t outlen1 = 0;
	parsing.extractdatainsegments(command_buffer, outbuf1, 20, &outlen1, ':',
			'\0');
	SOC::setSoCChargingFlag(std::atoi(outbuf1));
	send_ACK_NACK(0);
}

static void Modem_setTankSettings() {
	char outbuf1[20];
	char outbuf2[20];
	char outbuf3[20];
	char outbuf4[20];
	uint16_t outlen1 = 0;
	uint16_t outlen2 = 0;
	uint16_t outlen3 = 0;
	uint16_t outlen4 = 0;

	uint8_t tankType_;
	float max_liters_;
	//in meters
	float width_;
	float length_;
	float radius_;

	char *ptr = command_buffer;

	parsing.extractdatainsegments_(&ptr,outbuf1, 20, &outlen1, ':','|');

	tankType_ = std::atoi(outbuf1);

	if (tankType_ == 0) {
		parsing.extractdatainsegments_(&ptr, outbuf2, 20, &outlen2,
				'|', '|');



		parsing.extractdatainsegments_(&ptr, outbuf3, 20, &outlen3,
				'|', '\0');

		max_liters_ = std::atof(outbuf2);
		radius_ = std::atof(outbuf3);


		flash_data_.tanktype = tankType_;
		flash_data_.max_liters = max_liters_;
		flash_data_.radius = radius_;

		liquidSensor.setTankType(flash_data_.tanktype);
		liquidSensor.setTankMaxLiters(flash_data_.max_liters);
		liquidSensor.setTankRadius(flash_data_.radius);

		SaveData();

	} else if (tankType_ == 1) {
		parsing.extractdatainsegments_(&ptr, outbuf2, 20, &outlen2,
				'|', '|');
		parsing.extractdatainsegments_(&ptr, outbuf3, 20, &outlen3,
				'|', '|');
		parsing.extractdatainsegments_(&ptr, outbuf4, 20, &outlen4,
				'|', '\0');

		max_liters_ = std::atof(outbuf2);
		width_ = std::atof(outbuf3);
		length_ = std::atof(outbuf4);


		flash_data_.tanktype = tankType_;
		flash_data_.max_liters = max_liters_;
		flash_data_.width = width_;
		flash_data_.length = length_;

		liquidSensor.setTankType(flash_data_.tanktype);
		liquidSensor.setTankMaxLiters(flash_data_.max_liters);
		liquidSensor.setTankWidth(flash_data_.width);
		liquidSensor.setTankLength(flash_data_.length);

		SaveData();

	}

	send_ACK_NACK(0);

}

static void Modem_getSoCnDCur() {

	char data[20];
	sprintf(data, "%f|%f", SOC::getSoCVal(), DCCurrentSensor.getOffset());
	send_data(data, std::strlen(data));

}

static void Modem_getSystemMode() {

	char data[10];
	sprintf(data, "%d", flash_data_.Auto_Manual);
	send_data(data, std::strlen(data));

}

static struct GSMCMDList GSMCMDList_[totalModemCmds] = { { "setTimedate:",
		Modem_setTimedate }, //format setTimedate:epoch|timezone    Reply:ACK/NACK
		{ "setFuelMeasurement:", Modem_setFuelMeasurement }, //format setFuelMeasurement:zerospan|fullspan  Reply:ACK/NACK
		{ "setSoCnDCur:", Modem_setSoCnDCur }, //format setSoCnDCur:SOC|CurrentOffset    Reply:ACK/NACK
		{ "setAutoManualMode:", Modem_setAutoManualMode }, //format setAutoManualMode:(0 - Auto   1 - Manual) Reply:ACK/NACK
		{ "setGenerator:", Modem_setGenerator }, //format setGenerator: (0 - GeneratorOff   1 - GeneratorOn)  Reply:ACK/NACK
		{ "setFixSOCCharging:", Modem_setFixSOCCharging }, //format setFixSOCCharging:(0 - turnoff 1 - turnOm)

		{ "setTankSettings:", Modem_setTankSettings },/*depending on tank type you can send different kind of format
		 All the values should be in meter for measurement
		 TankType = 0 (Cylnderical)
		 Tanktype = 1 (Square)
		 format setTankSettings:TankType|TotalLiters|Width|Length|Radius
		 Now if TankType = 0 you can only send Radius like this
		 setTankSettings:(TankType=0)|(TotalLiters=500)|(Radius=value in meter)
		 now if Tanktype=1 you can send width and length like following
		 setTankSettings:(TankType=1)|(TotalLiters=500)|(Width=value in meter)| (length=value in meter)*/

		{ "getSoCnDCur", Modem_getSoCnDCur }, //format getSoCnDCur     Reply: SoC|CurrentOffset
		{ "getSystemMode", Modem_getSystemMode }, //format getSystemMode   Reply: Auto/Manual   0 - Auto   1 - Manual
		};

static void ModemCMDReceived() {
	memset(send_command_buffer, 0, 255);

	for (int i = 0; i < totalModemCmds; i++) {

		if (std::strstr(command_buffer, GSMCMDList_[i].cmd)) {
			GSMCMDList_[i].Callback();
			break;
		}

	}

	ModemDataReceived = 0;
	memset(command_buffer, 0, 255);
}

char* getModemNetwork() {
	return ModemData.networkStat;
}
char* getSignalQuality() {
	return ModemData.quality;
}

char* getModemData() {
	static char Modemdata[20];

	if (ModemData.internet == 1) {
		std::strcpy(Modemdata, "Conn");
	} else {
		std::strcpy(Modemdata, "DisCon");
	}

	return (Modemdata);

}

void ModemTask(void *pvParameters) {

	simA7672.init();

	struct ControlData_Queue ControlData = { 0 };

	Modem::simA7672::UE_systemInfo cpsiInfo;
	Modem::simA7672::pdp_stat pdpinfo;
	Modem::simA7672::status modeminfo;
	Modem::simA7672::mqtt_flags mqttstartinfo = Modem::simA7672::mqtt_ERR;
	Modem::simA7672::mqtt_flags mqttserverinfo = Modem::simA7672::mqtt_ERR;

	enum routine mqtt_routine = net_enable;

	std::strcpy(ModemData.serverAddr,
			"tcp://apfp7i6y92d6b-ats.iot.us-east-1.amazonaws.com:8883");
	std::strcpy(ModemData.apn, "jazzconnect.mobilinkworld.com");
	std::strcpy(ModemData.mqttSubTopic, "devicereceive");
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

		if (ControlDataQueue.queueReceive((void*) &ControlData)
				== ControlDataQueue.queues_recived) {
			/*Do something here when you want to receive something*/
		}

		switch (mqtt_routine) {

		case net_enable: {

			/*Enable Net support*/

			if ((std::strstr(ModemData.networkStat, networkInfo[0])
					|| std::strstr(ModemData.networkStat, networkInfo[1]))
					&& (std::strstr(ModemData.networkStatpdp, networkInfo[0])
							|| std::strstr(ModemData.networkStatpdp,
									networkInfo[1]))
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

					if (mqttserverinfo
							== Modem::simA7672::mqtt_serverConnected) {

						simA7672.mqttDisconnectServer(
								ModemData.mqtt_client_index);
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
							|| std::strstr(ModemData.networkStatpdp,
									networkInfo[1]))
					&& cpsiInfo != Modem::simA7672::no_service
					&& mqttstartinfo != Modem::simA7672::mqtt_OK) {

				simA7672.activate_deactivatePDP(
						Modem::simA7672::pdp_deactivated);
				simA7672.activate_deactivatePDP(Modem::simA7672::pdp_activate);
				mqttstartinfo = simA7672.mqttStart(
						&ModemData.mqtt_client_index);
				if (mqttstartinfo == Modem::simA7672::mqtt_OK) {


					mqttstartinfo = simA7672.mqttSetClient(
							ModemData.mqtt_client_index,
							ControlData.uniqueID, 1);

					if (mqttstartinfo == Modem::simA7672::mqtt_OK) {

						simA7672.mqttsslenable(ModemData.mqtt_client_index);
						mqtt_routine = mqtt_connect;
					} else {
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
					&& mqttserverinfo
							!= Modem::simA7672::mqtt_serverConnected) {

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

			mqttserverinfo = simA7672.getmqttconnectionstatus(
					ModemData.serverAddr);
			mqtt_counter++;
			/*get retries as well as sometimes we can get some issue */
			if (mqttserverinfo != Modem::simA7672::mqtt_serverConnected
					&& mqtt_counter > 3) {
				ModemData.internet = 0;
				simA7672.mqttDisconnectServer(ModemData.mqtt_client_index);
				mqtt_routine = net_enable;
			} else if (mqttserverinfo
					== Modem::simA7672::mqtt_serverConnected) {
				ModemData.internet = 1;
				mqtt_counter = 0;
				std::sprintf(reinterpret_cast<char*>(simA7672.Txbuffer),
						sendbufMQTT, ControlData.uniqueID,
						ControlData.timestamp, ControlData.V_1, ControlData.V_2,
						ControlData.V_3, ControlData.I_1, ControlData.I_2,
						ControlData.I_3, ControlData.P_1, ControlData.P_2,
						ControlData.P_3, ControlData.Energy, ControlData.Freq,
						ControlData.SelectedSource, ControlData.fuelPer,
						ControlData.fuelConsp, ControlData.refuelingStartTime,
						ControlData.refuelingEndTime, ControlData.SoC,
						ControlData.DcVolt, ControlData.DcCurrent,
						ControlData.batteryChargeDischargeStartTime[0],
						ControlData.batteryChargeDischargeEndTime[0],
						ControlData.temp, ControlData.humid);

				currentTime = HAL_GetTick();
				if (currentTime - prevTime > setTime) {
					simA7672.mqttPubData(ModemData.mqtt_client_index,
							ModemData.mqttPubTopic,
							reinterpret_cast<char*>(simA7672.Txbuffer),
							std::strlen(
									reinterpret_cast<char*>(simA7672.Txbuffer)));
					prevTime = currentTime;
				}

				if (simA7672.mqttsubTopicAndRead(ModemData.mqtt_client_index,
						ModemData.mqttSubTopic, command_buffer, 255)
						== Modem::simA7672::mqtt_msgrecv) {
					/*take flag to do something*/

					if(std::strstr(command_buffer,ControlData.uniqueID))
					{
						ModemDataReceived = 1;
					}

				}
				simA7672.mqttunsubTopic(ModemData.mqtt_client_index,
						ModemData.mqttSubTopic);
			}

			break;
		}

		default:
			break;

		}

		if (ModemDataReceived == 1) {
			ModemCMDReceived();
		}

		System_Rtos::delay(500);
	}

}

