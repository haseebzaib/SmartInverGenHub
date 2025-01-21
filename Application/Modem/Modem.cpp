/*
 * Modem_SimA7672.cpp
 *
 *  Created on: Nov 5, 2024
 *      Author: user
 */

#include "Modem.hpp"
#include "cstdio"
#include "cstring"
#include "System/System_serial.hpp"
#include "atcmd.hpp"
#include "main.h"
#include "System/System_Rtos.hpp"
namespace Modem {

struct simA7672::classInstanceRecorder simA7672::ClassInstances[numberofuart] =
		{ nullptr };

uint8_t simA7672::ClassInstanceIncrementer = 0;

simA7672::simA7672(UART_HandleTypeDef *huart) :
		GlobalTxBufs { simA7672::Txbuffer }, GlobalRxBufs { simA7672::Rxbuffer }, serial_(
				huart), checking() {

	simA7672::ClassInstances[ClassInstanceIncrementer].huart = huart;
	simA7672::ClassInstances[ClassInstanceIncrementer].Instance = this;

	simA7672::ClassInstanceIncrementer++;
	if (simA7672::ClassInstanceIncrementer > numberofuart) {
		simA7672::ClassInstanceIncrementer = 0;
	}

	setModemCallback(simA7672::RxCBStatic, simA7672::TxCBStatic);

}

enum simA7672::status simA7672::init() {
	status stat = simA7672_OK;

	//Run 3 times
	PrepRxTx(atcmd_AT, sizeof(atcmd_AT) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 5000);

	PrepRxTx(atcmd_AT, sizeof(atcmd_AT) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 5000);

	PrepRxTx(atcmd_AT, sizeof(atcmd_AT) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 5000);

	PrepRxTx(atcmd_ATCRESET, sizeof(atcmd_ATCRESET) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 5000);

	System_Rtos::delay(15000);

	/*After reset send two AT just to make gsm responsive*/
	PrepRxTx(atcmd_AT, sizeof(atcmd_AT) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 5000);

	PrepRxTx(atcmd_AT, sizeof(atcmd_AT) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 5000);

	PrepRxTx(atcmd_AT, sizeof(atcmd_AT) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 5000);

	PrepRxTx(atcmd_AT_F0, sizeof(atcmd_AT_F0) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 5000);

	PrepRxTx(atcmd_ATE1, sizeof(atcmd_ATE1) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 5000);

	PrepRxTx(atcmd_ATCREG, sizeof(atcmd_ATCREG) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 5000);

	PrepRxTx(atcmd_ATCGREG, sizeof(atcmd_ATCGREG) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 5000);

	PrepRxTx(atcmd_ATCPMS, sizeof(atcmd_ATCPMS) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 5000);

	PrepRxTx(atcmd_ATCOPS, sizeof(atcmd_ATCOPS) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 5000);

	PrepRxTx(atcmd_ATCMGF, sizeof(atcmd_ATCMGF) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 5000);

	PrepRxTx(atcmd_ATCGATT, sizeof(atcmd_ATCGATT) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 5000);

	return stat;

}

enum simA7672::status simA7672::loop() {
	status stat = simA7672_OK;

	return stat;
}

/*Set functions*/
enum simA7672::status simA7672::setTextMode() {
	status stat = simA7672_OK;

	PrepRxTx(atcmd_ATCMGF, sizeof(atcmd_ATCMGF) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 2000);

	return stat;
}

enum simA7672::status simA7672::setTimeDate(char *TimeDate) {
	status stat = simA7672_OK;

	char buf[50] = { 0 };

	std::sprintf(buf, "AT+CCLK=\"%s\"\r\n", TimeDate);
	PrepRxTx(reinterpret_cast<const uint8_t*>(buf),
			std::strlen(const_cast<const char*>(buf)), 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 2000);

	if (stat != simA7672_OK
			|| checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) != checking.sys_ok) {

		stat = simA7672_ERR;

	}
	return stat;
}

enum simA7672::status simA7672::setAPN(char *apn) {

	status stat = simA7672_OK;
	char buf[150] = { 0 };

	std::sprintf(buf, "AT+CGDCONT=1,\"ip\",\"%s\"\r\n", apn);
	PrepRxTx(reinterpret_cast<const uint8_t*>(buf),
			std::strlen(const_cast<const char*>(buf)), 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 2000);

	if (stat != simA7672_OK
			|| checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) != checking.sys_ok) {

		stat = simA7672_ERR;

	}

	return stat;

}
enum simA7672::status simA7672::activate_deactivatePDP(
		enum pdp_stat activate_deactivate) {

	status stat = simA7672_OK;
	char buf[20] = { 0 };

	std::sprintf(buf, "AT+CGACT=%d,1\r\n", activate_deactivate);
	PrepRxTx(reinterpret_cast<const uint8_t*>(buf),
			std::strlen(const_cast<const char*>(buf)), 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 5000);

	if (stat != simA7672_OK
			|| checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) != checking.sys_ok) {

		stat = simA7672_ERR;

	}

	return stat;

}

/*Get Functions*/
void simA7672::getSim(char *pin) {
	status stat;

	PrepRxTx(atcmd_ATCPIN, sizeof(atcmd_ATCPIN) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 2000);
	if (stat == simA7672_OK
			&& checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) == checking.sys_ok) {
		if (checking.parseSkipCharnDTillEndChar((char*) Rxbuffer,
				Rx_info.Rxcount, (char*) "+CPIN:", ':', _CR, pin)
				== checking.sys_ok) {
			return;
		} else {
			std::strcpy(pin, "--------");
		}

	} else {
		std::strcpy(pin, "--------");
	}

}
void simA7672::getPin() {

}
void simA7672::getNetwork(char *networkStat) {
	status stat;

	PrepRxTx(atcmd_GATCREG, sizeof(atcmd_GATCREG) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 2000);

	if (stat == simA7672_OK
			&& checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) == checking.sys_ok) {
		if (checking.parseSkipCharnDTillEndChar((char*) Rxbuffer,
				Rx_info.Rxcount, (char*) "+CREG:", ',', _CR, networkStat)
				== checking.sys_ok) {
			switch (*networkStat) {
			case '0': {
				std::strcpy(networkStat, "Not-Reg");

				break;
			}
			case '1': {
				std::strcpy(networkStat, "Reg-Home");

				break;
			}
			case '2': {
				std::strcpy(networkStat, "Searching");

				break;
			}
			case '3': {
				std::strcpy(networkStat, "Reg-denied");

				break;
			}
			case '4': {
				std::strcpy(networkStat, "Unknown");

				break;
			}
			case '5': {
				std::strcpy(networkStat, "Reg-Roaming");

				break;
			}
			case '6': {
				std::strcpy(networkStat, "Reg-SMS");

				break;
			}
			case '7': {
				std::strcpy(networkStat, "Reg-SMSR");

				break;
			}
			default: {
				std::strcpy(networkStat, "--------");

				break;
			}
			}
			return;
		} else {
			std::strcpy(networkStat, "--------");
		}

	} else {
		std::strcpy(networkStat, "--------");
	}

}

void simA7672::getNetworkPDP(char *networkStatpdp) {
	status stat;

	PrepRxTx(atcmd_GATCGREG, sizeof(atcmd_GATCGREG) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 2000);

	if (stat == simA7672_OK
			&& checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) == checking.sys_ok) {
		if (checking.parseSkipCharnDTillEndChar((char*) Rxbuffer,
				Rx_info.Rxcount, (char*) "+CGREG:", ',', _CR, networkStatpdp)
				== checking.sys_ok) {
			switch (*networkStatpdp) {
			case '0': {
				std::strcpy(networkStatpdp, "Not-Reg");

				break;
			}
			case '1': {
				std::strcpy(networkStatpdp, "Reg-Home");

				break;
			}
			case '2': {
				std::strcpy(networkStatpdp, "Searching");

				break;
			}
			case '3': {
				std::strcpy(networkStatpdp, "Reg-denied");

				break;
			}
			case '4': {
				std::strcpy(networkStatpdp, "Unknown");

				break;
			}
			case '5': {
				std::strcpy(networkStatpdp, "Reg-Roaming");

				break;
			}
			case '6': {
				std::strcpy(networkStatpdp, "Reg-SMS");

				break;
			}
			default: {
				std::strcpy(networkStatpdp, "--------");

				break;
			}
			}
			return;
		} else {
			std::strcpy(networkStatpdp, "--------");
		}

	} else {
		std::strcpy(networkStatpdp, "--------");
	}

}

void simA7672::getPDPstatus(enum pdp_stat *status_pdp) {

	status stat;

	char buf[10] = { 0 };

	*status_pdp = pdp_noReply;

	PrepRxTx(atcmd_GATCGACT, sizeof(atcmd_GATCGACT) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 2000);

	if (stat == simA7672_OK
			&& checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) == checking.sys_ok) {

		if (checking.parseSkipCharnDTillEndChar((char*) Rxbuffer,
				Rx_info.Rxcount, (char*) "+CGACT:", ':', ',', buf)
				== checking.sys_ok) {
			switch (*buf) {
			case '0': {

				*status_pdp = pdp_deactivated;
				break;
			}

			case '1': {
				*status_pdp = pdp_activate;
				break;
			}

			}

		}

	}

}

void simA7672::getUEsystemInfo(enum UE_systemInfo *status_ue) {

	status stat;

	char buf[15] = { 0 };

	*status_ue = no_service;

	PrepRxTx(atcmd_GATCPSI, sizeof(atcmd_GATCPSI) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 5000);

	if (stat == simA7672_OK
			&& checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) == checking.sys_ok) {

		if (checking.parseSkipCharnDTillEndChar((char*) Rxbuffer,
				Rx_info.Rxcount, (char*) "+CPSI:", ':', ',', buf)
				== checking.sys_ok) {
			if (std::strstr(buf, "NO SERVICE")) {
				*status_ue = no_service;
			} else if (std::strstr(buf, "GSM")) {
				*status_ue = gsm;
			} else if (std::strstr(buf, "WCDMA")) {
				*status_ue = wcdma;
			} else if (std::strstr(buf, "LTE")) {
				*status_ue = lte;
			}

		}

	}

}

void simA7672::getSignalQ(char *quality) {

	status stat;

	char buffer[10] = { 0 };
	int8_t csq = 0;
	PrepRxTx(atcmd_ATCSQ, sizeof(atcmd_ATCSQ) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 2000);

	if (stat == simA7672_OK
			&& checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) == checking.sys_ok) {
		if (checking.parseSkipCharnDTillEndChar((char*) Rxbuffer,
				Rx_info.Rxcount, (char*) "+CSQ:", ':', ',', buffer)
				== checking.sys_ok) {

			csq = std::atoi(buffer);
			if (csq >= 99) {
				csq = 0;

			} else {
				csq = (2 * (csq)) - 113;
			}

			std::sprintf(quality, "%02d dbm", csq);

			return;
		} else {
			std::strcpy(quality, "--------");
		}

	} else {
		std::strcpy(quality, "--------");
	}

}
void simA7672::getTimeDate(char *Time, char *Date, char *timezone) {
	status stat;

	PrepRxTx(atcmd_ATCCLK, sizeof(atcmd_ATCCLK) - 1, 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 5000);

	if (stat == simA7672_OK
			&& checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) == checking.sys_ok) {
		if (checking.parseDate_Time_Timezone((char*) Rxbuffer, Rx_info.Rxcount,
				Date, Time, timezone) == checking.sys_ok) {
			return;
		} else {
			std::strcpy(Date, "ER-ER-ER");
			std::strcpy(Time, "ER:ER:ER");
			std::strcpy(timezone, "ERR");
		}

	} else if (stat == simA7672_OK
			&& checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) == checking.sys_err) {
		/*ToDo*/
	} else {
		/*ToDo*/
	}

}

enum simA7672::mqtt_flags simA7672::getmqttconnectionstatus(char *serverAddr) {
	status stat = simA7672_ERR;
	mqtt_flags statMqtt = mqtt_serverDisconnected;

	char buf[100] = { 0 };

	PrepRxTx(atcmd_GATCMQTTCONNECT, sizeof(atcmd_GATCMQTTCONNECT) - 1, 1000,
			_LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 8000);
	if (stat == simA7672_OK
			&& checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) == checking.sys_ok) {
		if (checking.parseSkipCharnDTillEndChar((char*) Rxbuffer,
				Rx_info.Rxcount, (char*) "+CMQTTCONNECT:", '"', '"', buf)
				== checking.sys_ok) {

			if (std::strstr(serverAddr, const_cast<const char*>(buf))) {
				statMqtt = mqtt_serverConnected;
			}

		}

	}

	return statMqtt;
}

enum simA7672::mqtt_flags simA7672::mqttStart(uint8_t *mqtt_index) {

	status stat = simA7672_ERR;
	mqtt_flags statMqtt = mqtt_ERR;
	char buf[10] = { 0 };

	PrepRxTx(atmcd_ATCMQTTSTART, sizeof(atmcd_ATCMQTTSTART) - 1, 1000, '+', 2,_LF,
			Notification_mode);
	stat = check_eventTimeout(rx_evt, 10000);

	if (stat == simA7672_OK
			&& checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) == checking.sys_ok) {
		if (checking.parseSkipCharnDTillEndChar((char*) Rxbuffer,
				Rx_info.Rxcount, (char*) "+CMQTTSTART:", ':', _LF, buf)
				== checking.sys_ok) {

			statMqtt = mqtt_OK;

			*mqtt_index = std::atoi(buf);

		}

	}

	return statMqtt;

}

enum simA7672::mqtt_flags simA7672::mqttStop() {
	status stat = simA7672_ERR;
	mqtt_flags statMqtt = mqtt_ERR;
	char buf[10] = { 0 };
	PrepRxTx(atcmd_ATCMQTTSTOP, sizeof(atcmd_ATCMQTTSTOP) - 1, 1000, '+',2, _LF,
			Notification_mode);
	stat = check_eventTimeout(rx_evt, 10000);

	if (stat == simA7672_OK
			&& checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) == checking.sys_ok) {
		if (checking.parseSkipCharnDTillEndChar((char*) Rxbuffer,
				Rx_info.Rxcount, (char*) "+CMQTTSTOP:", ':', _LF, buf)
				== checking.sys_ok) {

			statMqtt = mqtt_OK;

		}

	}

	return statMqtt;
}

enum simA7672::mqtt_flags simA7672::mqttSetClient(uint8_t mqtt_index,
		char *client,uint8_t activate_ssl) {
	status stat = simA7672_ERR;
	mqtt_flags statMqtt = mqtt_ERR;
	char buf[80] = { 0 };

	std::sprintf(buf, "AT+CMQTTACCQ=%d,\"%s\",%d\r\n", mqtt_index, client,activate_ssl);
	PrepRxTx(reinterpret_cast<const uint8_t*>(buf),
			std::strlen(const_cast<const char*>(buf)), 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 8000);

	if (stat == simA7672_OK
			&& checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) == checking.sys_ok) {

		statMqtt = mqtt_OK;

	}

	return statMqtt;
}

enum simA7672::mqtt_flags simA7672::mqttReleaseClient(uint8_t mqtt_index) {
	status stat = simA7672_ERR;
	mqtt_flags statMqtt = mqtt_ERR;

	char buf[80] = { 0 };

	std::sprintf(buf, "AT+CMQTTREL=%d\r\n", mqtt_index);
	PrepRxTx(reinterpret_cast<const uint8_t*>(buf),
			std::strlen(const_cast<const char*>(buf)), 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 8000);

	if (stat == simA7672_OK
			&& checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) == checking.sys_ok) {
		statMqtt = mqtt_OK;

	}

	return statMqtt;
}

enum simA7672::mqtt_flags simA7672::mqttConnectServer(uint8_t mqtt_index,
		char *url) {

	status stat = simA7672_ERR;
	mqtt_flags statMqtt = mqtt_ERR;
	char buf[150] = { 0 };
	char recv_buf[5] = { 0 };

	std::sprintf(buf, "AT+CMQTTCONNECT=%d,\"%s\",60,1\r\n", mqtt_index, url);
	PrepRxTx(reinterpret_cast<const uint8_t*>(buf),
			std::strlen(const_cast<const char*>(buf)), 1000, '+',2, _LF,
			Notification_mode);
	stat = check_eventTimeout(rx_evt, 30000); //more delay as this commands take sometimes to generate notification

	if (stat == simA7672_OK) {
		if (checking.parseSkipCharnDTillEndChar((char*) Rxbuffer,
				Rx_info.Rxcount, (char*) "+CMQTTCONNECT:", ',', _LF, recv_buf)
				== checking.sys_ok) {

			if (std::strstr(recv_buf, "0")) {
				statMqtt = mqtt_serverConnected;
			}

		}

	}

	return statMqtt;
}

enum simA7672::mqtt_flags simA7672::mqttDisconnectServer(uint8_t mqtt_index) {

	status stat = simA7672_ERR;
	mqtt_flags statMqtt = mqtt_ERR;
	char buf[150] = { 0 };
	char recv_buf[5] = { 0 };

	std::sprintf(buf, "AT+CMQTTDISC=%d,120\r\n", mqtt_index);
	PrepRxTx(reinterpret_cast<const uint8_t*>(buf),
			std::strlen(const_cast<const char*>(buf)), 1000, '+',2, _LF,
			Notification_mode);
	stat = check_eventTimeout(rx_evt, 10000);

	if (stat == simA7672_OK) {
		if (checking.parseSkipCharnDTillEndChar((char*) Rxbuffer,
				Rx_info.Rxcount, (char*) "+CMQTTDISC:", ',', _LF, recv_buf)
				== checking.sys_ok) {

			if (std::strstr(recv_buf, "0")) {
				statMqtt = mqtt_serverDisconnected;
			}

		}

	}

	return statMqtt;

}

enum simA7672::mqtt_flags simA7672::mqttPubData(uint8_t mqtt_index,
		char *pubTopic, char *pubPayload, uint16_t len) {

	mqtt_flags statMqtt = mqtt_ERR;

	statMqtt = mqttSetPubTopic(mqtt_index,pubTopic);
	if(statMqtt == mqtt_OK)
	{
	 statMqtt = mqttSetPubPayload(mqtt_index,pubPayload,len);
	 if(statMqtt == mqtt_OK)
	  {
	    statMqtt = mqttPublishData(mqtt_index);
	  }
	}

	return statMqtt;

}
enum simA7672::mqtt_flags simA7672::mqttsubTopicAndRead(uint8_t mqtt_index,
		char *subTopic, char *Message,uint16_t msgbufferSize) {
	status stat = simA7672_ERR;
	mqtt_flags statMqtt = mqtt_ERR;

	char buf[150] = { 0 };


	std::sprintf(buf, "AT+CMQTTSUB=%d,%d,1\r\n", mqtt_index,
			std::strlen(const_cast<const char*>(subTopic)));
	PrepRxTx(reinterpret_cast<const uint8_t*>(buf),
			std::strlen(const_cast<const char*>(buf)), 1000, '>', MSG_mode);
	stat = check_eventTimeout(rx_evt, 1000);

	//Rx_info.reset_timeout =1;
	if (stat == simA7672_OK) {
		if (checking.parseSkipCharnDTillEndChar((char*) Rxbuffer,
				Rx_info.Rxcount, (char*) ">", ',', _LF, nullptr)
				== checking.sys_ok) {

			PrepRxTx(reinterpret_cast<const uint8_t*>(subTopic),
					std::strlen(const_cast<const char*>(subTopic)), 1000,'+' ,5,_LF,
					Notification_mode);
			stat = check_eventTimeout(rx_evt,1000);

			if (stat == simA7672_OK
					&& checking.checkResponse_OK_ERR((const char*) Rxbuffer,
							Rx_info.Rxcount) == checking.sys_ok) {

				if(checking.extractData((const char*) Rxbuffer, "+CMQTTRXPAYLOAD:", '\n', '\r', Message, msgbufferSize) == checking.sys_ok)
				{

					statMqtt = mqtt_msgrecv;
				}
				else
				{
					statMqtt = mqtt_OK;
				}

			}

		}

	}
	//Rx_info.reset_timeout =0;
	return statMqtt;
}
enum simA7672::mqtt_flags simA7672::mqttunsubTopic(uint8_t mqtt_index,
		char *subTopic) {
	status stat = simA7672_ERR;
	mqtt_flags statMqtt = mqtt_ERR;

	char buf[150] = { 0 };
	char recv_buf[5] = { 0 };

	std::sprintf(buf, "AT+CMQTTUNSUB=%d,%d,0\r\n", mqtt_index,
			std::strlen(const_cast<const char*>(subTopic)));
	PrepRxTx(reinterpret_cast<const uint8_t*>(buf),
			std::strlen(const_cast<const char*>(buf)), 1000, '>', MSG_mode);
	stat = check_eventTimeout(rx_evt, 1000);
	if (stat == simA7672_OK) {
		if (checking.parseSkipCharnDTillEndChar((char*) Rxbuffer,
				Rx_info.Rxcount, (char*) ">", ',', _LF, nullptr)
				== checking.sys_ok) {

			PrepRxTx(reinterpret_cast<const uint8_t*>(subTopic),
					std::strlen(const_cast<const char*>(subTopic)), 1000, '+',1, _LF,
					Notification_mode);
			stat = check_eventTimeout(rx_evt, 10000);

			if (stat == simA7672_OK) {
				if (checking.parseSkipCharnDTillEndChar((char*) Rxbuffer,
						Rx_info.Rxcount, (char*) "+CMQTTUNSUB:", ',', _LF,
						recv_buf) == checking.sys_ok) {

					if (std::strstr(recv_buf, "0")) {
						statMqtt = mqtt_OK;
					}

				}

			}
		}
	}

	return statMqtt;
}

enum simA7672::ssl_flags simA7672::mqttsslenable(uint8_t mqtt_index)
{
	status stat = simA7672_ERR;
	ssl_flags sslstat = ssl_ERR;

	char buf[80] = { 0 };

	std::sprintf(buf, "AT+CMQTTSSLCFG=%d,0\r\n", mqtt_index);
	PrepRxTx(reinterpret_cast<const uint8_t*>(buf),
			std::strlen(const_cast<const char*>(buf)), 1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 8000);

	if (stat == simA7672_OK
			&& checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) == checking.sys_ok) {

		sslstat = ssl_OK;

	}

	return sslstat;
}
enum simA7672::ssl_flags simA7672::sslversionset()
{
	status stat = simA7672_ERR;
	ssl_flags sslstat = ssl_ERR;


	PrepRxTx(atcmd_ATCSSLCFG_ver, sizeof(atcmd_ATCSSLCFG_ver) - 1,1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 2000);

	if (stat == simA7672_OK
			&& checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) == checking.sys_ok) {

		sslstat = ssl_OK;

	}

	return sslstat;
}
enum simA7672::ssl_flags simA7672::sslauthset()
{
	status stat = simA7672_ERR;
	ssl_flags sslstat = ssl_ERR;


	PrepRxTx(atcmd_ATCSSLCFG_auth, sizeof(atcmd_ATCSSLCFG_auth) - 1,1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 2000);

	if (stat == simA7672_OK
			&& checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) == checking.sys_ok) {

		sslstat = ssl_OK;

	}

	return sslstat;
}
enum simA7672::ssl_flags simA7672::sslcacertset()
{
	status stat = simA7672_ERR;
	ssl_flags sslstat = ssl_ERR;


	PrepRxTx(atcmd_ATCSSLCFG_ca, sizeof(atcmd_ATCSSLCFG_ca) - 1,1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 2000);

	if (stat == simA7672_OK
			&& checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) == checking.sys_ok) {

		sslstat = ssl_OK;

	}

	return sslstat;
}
enum simA7672::ssl_flags simA7672::ssldevicecertset()
{
	status stat = simA7672_ERR;
	ssl_flags sslstat = ssl_ERR;


	PrepRxTx(atcmd_ATCSSLCFG_device, sizeof(atcmd_ATCSSLCFG_device) - 1,1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 2000);

	if (stat == simA7672_OK
			&& checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) == checking.sys_ok) {

		sslstat = ssl_OK;

	}

	return sslstat;
}
enum simA7672::ssl_flags simA7672::ssldevicekeyset()
{
	status stat = simA7672_ERR;
	ssl_flags sslstat = ssl_ERR;


	PrepRxTx(atcmd_ATCSSLCFG_key, sizeof(atcmd_ATCSSLCFG_key) - 1,1000, _LF, CMD_mode);
	stat = check_eventTimeout(rx_evt, 2000);

	if (stat == simA7672_OK
			&& checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) == checking.sys_ok) {

		sslstat = ssl_OK;

	}

	return sslstat;

}

enum simA7672::mqtt_flags simA7672::mqttSetPubTopic(uint8_t mqtt_index,char *pubTopic)
{

	status stat = simA7672_ERR;
	mqtt_flags statMqtt = mqtt_ERR;

	char buf[150] = { 0 };

	std::sprintf(buf, "AT+CMQTTTOPIC=%d,%d\r\n", mqtt_index,
			std::strlen(const_cast<const char*>(pubTopic)));
	PrepRxTx(reinterpret_cast<const uint8_t*>(buf),
			std::strlen(const_cast<const char*>(buf)),0, 1000, '>', MSG_mode);
	stat = check_eventTimeout(rx_evt, 1000);

	if (stat == simA7672_OK) {
		if (checking.parseSkipCharnDTillEndChar((char*) Rxbuffer,
				Rx_info.Rxcount, (char*) ">", ',', _LF, nullptr)
				== checking.sys_ok) {

			PrepRxTx(reinterpret_cast<const uint8_t*>(pubTopic),
					std::strlen(const_cast<const char*>(pubTopic)),0, 1000,_LF,
					CMD_mode);
			stat = check_eventTimeout(rx_evt, 8000);

			if (stat == simA7672_OK && checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) == checking.sys_ok) {

				statMqtt = mqtt_OK;

			}
		}
	}
	return statMqtt;

}
enum simA7672::mqtt_flags simA7672::mqttSetPubPayload(uint8_t mqtt_index,char *pubPayload,uint16_t len)
{
	status stat = simA7672_ERR;
	mqtt_flags statMqtt = mqtt_ERR;

	char buf[150] = { 0 };

	std::sprintf(buf, "AT+CMQTTPAYLOAD=%d,%d\r\n", mqtt_index,len);
	PrepRxTx(reinterpret_cast<const uint8_t*>(buf),
			std::strlen(const_cast<const char*>(buf)),0, 1000, '>', MSG_mode);
	stat = check_eventTimeout(rx_evt, 1000);

	if (stat == simA7672_OK) {
		if (checking.parseSkipCharnDTillEndChar((char*) Rxbuffer,
				Rx_info.Rxcount, (char*) ">", ',', _LF, nullptr)
				== checking.sys_ok) {

			PrepRxTx(reinterpret_cast<const uint8_t*>(pubPayload),
					len,0, 4000,_LF,
					CMD_mode);
			stat = check_eventTimeout(rx_evt, 8000);

			if (stat == simA7672_OK && checking.checkResponse_OK_ERR((const char*) Rxbuffer,
					Rx_info.Rxcount) == checking.sys_ok) {

				statMqtt = mqtt_OK;

			}
		}
	}
	return statMqtt;
}
enum simA7672::mqtt_flags simA7672::mqttPublishData(uint8_t mqtt_index)
{
	status stat = simA7672_ERR;
	mqtt_flags statMqtt = mqtt_ERR;

	char buf[150] = { 0 };
	char recv_buf[5] = { 0 };

	std::sprintf(buf, "AT+CMQTTPUB=%d,1,60\r\n", mqtt_index);
	PrepRxTx(reinterpret_cast<const uint8_t*>(buf),
			std::strlen(const_cast<const char*>(buf)),0, 1000, '+',2, _LF,
			Notification_mode);
	stat = check_eventTimeout(rx_evt, 5000);

	if (stat == simA7672_OK) {
		if (checking.parseSkipCharnDTillEndChar((char*) Rxbuffer,
				Rx_info.Rxcount, (char*) "+CMQTTPUB:", ',', _LF, recv_buf)
				== checking.sys_ok) {

			if (std::strstr(recv_buf, "0")) {
				statMqtt = mqtt_msgsent;
			}

		}

	}

	return statMqtt;
}

void simA7672::PrepRx(uint8_t end_char, enum Rxmode Rxmode) {

	memset(Rxbuffer, 0, BufferLen);
	rst_event(rx_evt);

	Rx_info.end_char = end_char;
	Rx_info.rx_mode = Rxmode;
	Rx_info.detect_endchar = 0;
	Rx_info.end_char_count = 0;
	Rx_info.release_event = -1;
	Rx_info.Rx = Rxbuffer;
	Rx_info.Rxcount = 0;
	serial_.RxintEn(Rx_info.Rx, BufferLen);

}

void simA7672::PrepRx(uint8_t start_char_notiMode, uint8_t start_char_notiModeCount, uint8_t end_char,
		enum Rxmode Rxmode) {

	memset(Rxbuffer, 0, BufferLen);
	rst_event(rx_evt);

	Rx_info.end_char = end_char;
	Rx_info.rx_mode = Rxmode;
	Rx_info.detect_endchar = 0;
	Rx_info.end_char_count = 0;
	Rx_info.release_event = -1;
	Rx_info.Rx = Rxbuffer;
	Rx_info.Rxcount = 0;
	Rx_info.start_char_notiMode = start_char_notiMode;
	Rx_info.start_char_notiModeCount = start_char_notiModeCount;
	serial_.RxintEn(Rx_info.Rx, BufferLen);

}
void simA7672::PrepTx(const uint8_t *Txbuf, uint16_t len, uint16_t timeout) {
	memset(Txbuffer, 0, BufferLen);
	rst_event(tx_evt);
	serial_.TransmitData(Txbuf, len, timeout);

}

void simA7672::PrepTx(const uint8_t *Txbuf, uint16_t len, uint8_t erase_txBuf,
		uint16_t timeout) {
	if (erase_txBuf == 1) {
		memset(Txbuffer, 0, BufferLen);
	}
	rst_event(tx_evt);
	serial_.TransmitData(Txbuf, len, timeout);

}
void simA7672::PrepRxTx(const uint8_t *Txbuf, uint16_t len, uint16_t timeout,
		uint8_t end_char, enum Rxmode Rxmode) {

	memset(Rxbuffer, 0, BufferLen);
	memset(Txbuffer, 0, BufferLen);
	rst_event(rx_evt);
	rst_event(tx_evt);

	Rx_info.end_char = end_char;
	Rx_info.rx_mode = Rxmode;
	Rx_info.detect_endchar = 0;
	Rx_info.end_char_count = 0;
	Rx_info.release_event = -1;
	Rx_info.Rx = Rxbuffer;
	Rx_info.Rxcount = 0;
	serial_.RxintEn(Rx_info.Rx, BufferLen);
	serial_.TransmitData(Txbuf, len, timeout);

}
void simA7672::PrepRxTx(const uint8_t *Txbuf, uint16_t len, uint8_t erase_txBuf, uint16_t timeout,
		uint8_t end_char, enum Rxmode Rxmode)
{
	memset(Rxbuffer, 0, BufferLen);
	if (erase_txBuf == 1) {
		memset(Txbuffer, 0, BufferLen);
	}
	rst_event(rx_evt);
	rst_event(tx_evt);

	Rx_info.end_char = end_char;
	Rx_info.rx_mode = Rxmode;
	Rx_info.detect_endchar = 0;
	Rx_info.end_char_count = 0;
	Rx_info.release_event = -1;
	Rx_info.Rx = Rxbuffer;
	Rx_info.Rxcount = 0;
	serial_.RxintEn(Rx_info.Rx, BufferLen);
	serial_.TransmitData(Txbuf, len, timeout);
}

void simA7672::PrepRxTx(const uint8_t *Txbuf, uint16_t len, uint16_t timeout,
		uint8_t start_char_notiMode,uint8_t start_char_notiModeCount,uint8_t end_char, enum Rxmode Rxmode) {

	memset(Rxbuffer, 0, BufferLen);
	memset(Txbuffer, 0, BufferLen);
	rst_event(rx_evt);
	rst_event(tx_evt);

	Rx_info.end_char = end_char;
	Rx_info.rx_mode = Rxmode;
	Rx_info.detect_endchar = 0;
	Rx_info.end_char_count = 0;
	Rx_info.release_event = -1;
	Rx_info.Rx = Rxbuffer;
	Rx_info.Rxcount = 0;
	Rx_info.start_char_notiMode = start_char_notiMode;
	Rx_info.start_char_notiModeCount = start_char_notiModeCount-1;
	serial_.RxintEn(Rx_info.Rx, BufferLen);
	serial_.TransmitData(Txbuf, len, timeout);

}

void simA7672::PrepRxTx(const uint8_t *Txbuf, uint16_t len, uint8_t erase_txBuf,
		uint16_t timeout, uint8_t start_char_notiMode, uint8_t start_char_notiModeCount,uint8_t end_char,
		enum Rxmode Rxmode) {

	memset(Rxbuffer, 0, BufferLen);
	if (erase_txBuf == 1) {
		memset(Txbuffer, 0, BufferLen);
	}
	rst_event(rx_evt);
	rst_event(tx_evt);

	Rx_info.end_char = end_char;
	Rx_info.rx_mode = Rxmode;
	Rx_info.detect_endchar = 0;
	Rx_info.end_char_count = 0;
	Rx_info.release_event = -1;
	Rx_info.Rx = Rxbuffer;
	Rx_info.Rxcount = 0;
	Rx_info.start_char_notiMode = start_char_notiMode;
	Rx_info.start_char_notiModeCount = start_char_notiModeCount-1;
	serial_.RxintEn(Rx_info.Rx, BufferLen);
	serial_.TransmitData(Txbuf, len, timeout);

}

inline void simA7672::set_event(enum eventType event) {
	Event[event] = 1;
}
inline void simA7672::rst_event(enum eventType event) {
	Event[event] = 0;
}
enum simA7672::status simA7672::check_event(enum eventType event) {

	status stat = simA7672_OK;

	if (Event[event]) {
		stat = simA7672_OK;
	} else if (!Event[event]) {
		stat = simA7672_ERR;
	}

	return stat;

}
enum simA7672::status simA7672::check_eventForever(enum eventType event) {
	status stat = simA7672_OK;

	while (check_event(event) != simA7672_OK) {
		//wait here forever
	}

	return stat;

}
enum simA7672::status simA7672::check_eventTimeout(enum eventType event,
		uint16_t timeout) {
	status stat = simA7672_OK;
	 prev_time = 0;

	prev_time = HAL_GetTick(); //we get new time before going into while
	while ((check_event(event) != simA7672_OK)
			&& (HAL_GetTick() - prev_time < timeout)) {
		//wait here until get event or time passes
		System_Rtos::delay(10);
	}

	if (check_event(event) != simA7672_OK) {
		stat = simA7672::simA7672_ERR;
	}

	return stat;

}

/*Interrupt callbacks*/

void simA7672::RxCB(uint8_t data) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvolatile"
	if (Rx_info.rx_mode == CMD_mode) {
		*Rx_info.Rx++ = data;
		Rx_info.Rxcount++;

		if (Rx_info.end_char_count == 1) {
			Rx_info.release_event++;

		}

		if (Rx_info.detect_nextlinechar == 1 && (data == 'O' || data == 'E')) {
			Rx_info.detect_nextlinechar = 0; //reset it
			Rx_info.end_char_count = 1;
			if (data == 'O') {
				Rx_info.total_char = 2;
			} else if (data == 'E') {
				Rx_info.total_char = 5;
			}

		} else {
			Rx_info.detect_nextlinechar = 0; //reset it

		}

		if (data == simA7672::_LF) {
			Rx_info.detect_nextlinechar = 1;

		}

		if (Rx_info.release_event >= Rx_info.total_char) {
			set_event(rx_evt);
			serial_.RxintDis();
		}

	} else if (Rx_info.rx_mode == MSG_mode) {
		*Rx_info.Rx++ = data;
		Rx_info.Rxcount++;
		if (Rx_info.end_char != _LF) {

			if (data == '>') {
				set_event(rx_evt);
				serial_.RxintDis();
			}
		} else {

			if (Rx_info.end_char_count == 1 && data == 'E') {
				Rx_info.release_event = 2;
			}

			if (data == _LF) {
				Rx_info.end_char_count++;
			}

			if (Rx_info.end_char_count == Rx_info.release_event) {
				set_event(rx_evt);
				serial_.RxintDis();
			}
		}

	} else if (Rx_info.rx_mode == Notification_mode) {
		*Rx_info.Rx++ = data;
		Rx_info.Rxcount++;

//		if(Rx_info.reset_timeout)
//		{
//			prev_time = HAL_GetTick();
//		}

		if (data == Rx_info.start_char_notiMode) {
			Rx_info.release_event++;

		}

		if (data == Rx_info.end_char && Rx_info.release_event >= Rx_info.start_char_notiModeCount) {
			set_event(rx_evt);
			serial_.RxintDis();
		}

	}
#pragma GCC diagnostic pop

}

void simA7672::TxCB(UART_HandleTypeDef *huart) {

}

void simA7672::RxCBStatic(UART_HandleTypeDef *huart, uint8_t data) {
	// Find the instance and call its instance method
	uint8_t i = 0;
	for (i = 0; i < numberofuart; i++) {
		if (ClassInstances[i].huart == huart) {
			break;
		}
	}

	if (ClassInstances[i].Instance) {
		ClassInstances[i].Instance->RxCB(data); // Delegate to instance-specific method
	}

}
void simA7672::TxCBStatic(UART_HandleTypeDef *huart) {

}

}

