/*
 * Modem_SimA7672.hpp
 *
 *  Created on: Nov 5, 2024
 *      Author: user
 */

#ifndef MODEM_MODEM_HPP_
#define MODEM_MODEM_HPP_

#include "main.h"
#include "System/System_serial.hpp"
#include "System/System_sys.hpp"

namespace Modem {

class simA7672 {

public:
	enum status {
		simA7672_OK = 0, simA7672_ERR = 1
	};

	enum pdp_stat {
		pdp_deactivated = 0,
		pdp_activate = 1,
		pdp_noReply = 2,
	};

	struct classInstanceRecorder {
		UART_HandleTypeDef *huart;
		simA7672 *Instance;
	};

	enum UE_systemInfo {
		no_service = 0,
		gsm = 1,
		wcdma = 2,
		lte = 3,
	};

	enum mqtt_flags {
		mqtt_OK = 0,
		mqtt_ERR = 1,
		mqtt_serverConnected = 2,
		mqtt_serverDisconnected = 3,
		mqtt_msgsent =4,
		mqtt_msgrecv =5,
	};

	enum ssl_flags{
		ssl_OK = 0,
		ssl_ERR = 1,
	};


    /*same for all instances*/
	static struct classInstanceRecorder ClassInstances[numberofuart];
	static uint8_t ClassInstanceIncrementer;
	static constexpr uint16_t BufferLen = 2048;

	uint8_t Rxbuffer[BufferLen] = { 0 };
	uint8_t Txbuffer[BufferLen] = { 0 };
	//Access this
	uint8_t *const GlobalTxBufs[1];
	uint8_t *const GlobalRxBufs[1];





	simA7672(UART_HandleTypeDef *huart);
	enum status init();
	enum status loop();

	/*Set functions*/
	enum status setTextMode();
	enum status setTimeDate(char *TimeDate);
	enum status setAPN( char *apn);
	enum status activate_deactivatePDP(enum pdp_stat activate_deactivate);



	/*Get Functions*/
	void getSim(char *pin);
	void getPin();
	void getNetwork(char *networkStat);
	void getNetworkPDP(char *networkStatpdp);
	void getPDPstatus(enum pdp_stat *status_pdp);
	void getUEsystemInfo(enum UE_systemInfo *status_ue);
	void getSignalQ(char *quality);
	void getTimeDate(char *Time,char *Date,char *timezone);



	/*MQTT related get functions*/
	enum mqtt_flags getmqttconnectionstatus(char *serverAddr);

	/*MQTT related set functions*/
	enum mqtt_flags mqttStart(uint8_t *mqtt_index);
	enum mqtt_flags mqttStop();
	enum mqtt_flags mqttSetClient(uint8_t mqtt_index,char *client,uint8_t activate_ssl);
	enum mqtt_flags mqttReleaseClient(uint8_t mqtt_index);
	enum mqtt_flags mqttConnectServer(uint8_t mqtt_index,char *url); //only TCP supported rightnow
	enum mqtt_flags mqttDisconnectServer(uint8_t mqtt_index);
	enum mqtt_flags mqttPubData(uint8_t mqtt_index,char *pubTopic,char *pubPayload,uint16_t len);
	enum mqtt_flags mqttsubTopicAndRead(uint8_t mqtt_index,char *subTopic,char *Message,uint16_t msgbufferSize);
	enum mqtt_flags mqttunsubTopic(uint8_t mqtt_index,char *subTopic);


	/*SSL setting*/
	enum ssl_flags mqttsslenable(uint8_t mqtt_index);
	enum ssl_flags sslversionset();
	enum ssl_flags sslauthset();
	enum ssl_flags sslcacertset();
	enum ssl_flags ssldevicecertset();
	enum ssl_flags ssldevicekeyset();

private:
#define CheckError(func, returnStat, checkStat) \
  if (func != checkStat)       \
  {                            \
    return returnStat;         \
  }

	enum Rxmode {
		CMD_mode = 1, MSG_mode = 2, Notification_mode = 3,
	};

	enum eventType {
		rx_evt = 0, tx_evt = 1,
	};

	enum asci_code {
		_EOS = 0x00,
		_SOH = 0x01,
		_STX = 0x02,
		_ETX = 0x03,
		_ENQ = 0x05,
		_ACK = 0x06,
		_HT = 0x09,
		_LF = 0x0A,
		_FF = 0x0C,
		_CR = 0x0D,
		_XON = 0x11,
		_XOF = 0x13,
		_NAK = 0x15,
		_CAN = 0x18,
		_CT0 = 0x18,
		_CT1 = 0x19,
		_CTRLZ = 0x1A,
		_ESC = 0x1B,
		_FS = 0x1C,
		_GS = 0x1D,
		_US = 0x1F,
		_SP = 0x20,
		_QM = 0x22,
		_AP = 0x60,
		_Snd = 0x3E,
	};

	struct Rx_Handling {
		uint8_t end_char;
		uint8_t detect_endchar;
		uint8_t detect_nextlinechar;
		enum Rxmode rx_mode;
		uint8_t end_char_count;
		uint8_t total_char;
		int8_t release_event;
		uint8_t *Rx;
		uint16_t Rxcount;
		uint8_t *Tx;
		uint16_t Txcount;
		uint8_t start_char_notiMode;
		uint8_t start_char_notiModeCount;
		uint8_t reset_timeout ;
	};

	__IO struct Rx_Handling Rx_info;

	System_serial::serial serial_;
	System_sys::Parsing_Checking checking;


	static constexpr uint8_t total_event = 2;
	__IO uint8_t Event[total_event];

	uint32_t prev_time = 0;


	enum mqtt_flags mqttSetPubTopic(uint8_t mqtt_index,char *pubTopic);
	enum mqtt_flags mqttSetPubPayload(uint8_t mqtt_index,char *pubPayload,uint16_t len);
	enum mqtt_flags mqttPublishData(uint8_t mqtt_index);


	void PrepRx(uint8_t end_char, enum Rxmode Rxmode);
	void PrepRx(uint8_t start_char_notiMode,uint8_t start_char_notiModeCount,uint8_t end_char, enum Rxmode Rxmode);



	void PrepTx(const uint8_t *Txbuf, uint16_t len, uint16_t timeout);
	void PrepTx(const uint8_t *Txbuf, uint16_t len, uint8_t erase_txBuf,uint16_t timeout);


	void PrepRxTx(const uint8_t *Txbuf, uint16_t len, uint16_t timeout,
			uint8_t end_char, enum Rxmode Rxmode);
	void PrepRxTx(const uint8_t *Txbuf, uint16_t len, uint8_t erase_txBuf, uint16_t timeout,
			uint8_t end_char, enum Rxmode Rxmode);
	void PrepRxTx(const uint8_t *Txbuf, uint16_t len, uint16_t timeout,uint8_t start_char_notiMode,uint8_t start_char_notiModeCount,
			uint8_t end_char, enum Rxmode Rxmode);
	void PrepRxTx(const uint8_t *Txbuf, uint16_t len,uint8_t erase_txBuf, uint16_t timeout,uint8_t start_char_notiMode,uint8_t start_char_notiModeCount,
				uint8_t end_char, enum Rxmode Rxmode);


	inline void set_event(enum eventType event);
	inline void rst_event(enum eventType event);
	enum status check_event(enum eventType event);
	enum status check_eventForever(enum eventType event);
	enum status check_eventTimeout(enum eventType event, uint16_t timeout);

	/*Interrupt callbacks changes for each instance*/
	void RxCB(uint8_t data);
	void TxCB(UART_HandleTypeDef *huart);

	/*Remain same for each instance*/
	static void RxCBStatic(UART_HandleTypeDef *huart, uint8_t data);
	static void TxCBStatic(UART_HandleTypeDef *huart);

};

}

#endif /* MODEM_MODEM_HPP_ */
