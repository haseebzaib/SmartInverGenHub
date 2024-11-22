/*
 * Inverter.hpp
 *
 *  Created on: Nov 13, 2024
 *      Author: user
 */

#ifndef INVERTER_INVERTER_HPP_
#define INVERTER_INVERTER_HPP_


#include "main.h"
#include "System/System_serial.hpp"
#include "System/System_sys.hpp"

namespace Inverter {

class Voltronic {
public:
	enum status {
		Voltronic_OK = 0, Voltronic_ERR = 1
	};

	struct classInstanceRecorder {
		UART_HandleTypeDef *huart;
		Voltronic *Instance;
	};

    /*same for all instances*/
	static struct classInstanceRecorder ClassInstances[numberofuart];
	static uint8_t ClassInstanceIncrementer;
	static constexpr uint16_t BufferLen = 255;

	uint8_t Rxbuffer[BufferLen] = { 0 };
	uint8_t Txbuffer[BufferLen] = { 0 };
	//Access this
	uint8_t *const GlobalTxBufs[1];
	uint8_t *const GlobalRxBufs[1];


	Voltronic(UART_HandleTypeDef *huart,GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);

	/*Basic check commands*/
	void ProtocolID_Inquiry(char *protoID);
    void DeviceID_Inquiry(char *devID);
    void CPUFW_Ver_Inquiry(char *Ver);
    void Model_Inquiry(char *Model);
    /*Check Status commands*/
    void DeviceStat_Inquiry_qpigs(char buf[24][10]);
    void DeviceStat_Inquiry_qipgs2(char buf[3][10]);
    void DeviceStat_Inquiry_qipgsN(char buf[29][15]);
    void DeviceStat_Inquiry_mode(uint8_t *mode);





private:
#define CheckError(func, returnStat, checkStat) \
  if (func != checkStat)       \
  {                            \
    return returnStat;         \
  }

    union MIX_U16Bit {
    	uint16_t w;
    	struct {
    		uint8_t Lb;
    		uint8_t Hb;
    	}bytes;
    };

    union MIX_U16Bit breaking_bits;

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

	enum eventType {
		rx_evt = 0, tx_evt = 1,
	};

	GPIO_TypeDef* DE_Port_;
	uint16_t DE_Pin_;

	System_serial::serial serial_;
	System_sys::Parsing_Checking checking;

#define Enable_DE   HAL_GPIO_WritePin(DE_Port_, DE_Pin_, GPIO_PIN_SET);
#define Disable_DE  HAL_GPIO_WritePin(DE_Port_, DE_Pin_, GPIO_PIN_RESET);

	struct Rx_Handling {
		uint8_t *Rx;
		uint16_t Rxcount;
		uint8_t *Tx;
		uint16_t Txcount;
		uint8_t EndChar; //end char to end at
	};

	__IO struct Rx_Handling Rx_info;

	  static constexpr uint8_t total_event = 2;
	__IO uint8_t Event[total_event];


	void PrepRxTx(const uint8_t *Txbuf, uint16_t len, uint16_t timeout,
			uint8_t end_char,uint8_t cal_crc);



	inline void set_event(enum eventType event);
	inline void rst_event(enum eventType event);
	enum status check_event(enum eventType event);
	enum status check_eventForever(enum eventType event);
	enum status check_eventTimeout(enum eventType event, uint16_t timeout);


	uint16_t CRC16_cal(const uint8_t *P,uint16_t len);

	/*Interrupt callbacks changes for each instance*/
	void RxCB(uint8_t data);
	void TxCB(UART_HandleTypeDef *huart);

	/*Remain same for each instance*/
	static void RxCBStatic(UART_HandleTypeDef *huart, uint8_t data);
	static void TxCBStatic(UART_HandleTypeDef *huart);


};




}


#endif /* INVERTER_INVERTER_HPP_ */
