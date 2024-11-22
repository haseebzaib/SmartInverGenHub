/*
 * Inverter.cpp
 *
 *  Created on: Nov 13, 2024
 *      Author: user
 */

#include "Inverter.hpp"
#include "cstdio"
#include "cstring"
#include "System/System_serial.hpp"
#include "main.h"
#include "System/System_Rtos.hpp"
#include <ctype.h>
#include <Invertercmd.hpp>

namespace Inverter {

struct Voltronic::classInstanceRecorder Voltronic::ClassInstances[numberofuart] =
		{ nullptr };

uint8_t Voltronic::ClassInstanceIncrementer = 0;

Voltronic::Voltronic(UART_HandleTypeDef *huart, GPIO_TypeDef *GPIOx,
		uint16_t GPIO_Pin) :
		GlobalTxBufs { Voltronic::Txbuffer }, GlobalRxBufs { Voltronic::Rxbuffer }, serial_(
				huart), checking() {

	Voltronic::ClassInstances[ClassInstanceIncrementer].huart = huart;
	Voltronic::ClassInstances[ClassInstanceIncrementer].Instance = this;

	Voltronic::ClassInstanceIncrementer++;
	if (Voltronic::ClassInstanceIncrementer > numberofuart) {
		Voltronic::ClassInstanceIncrementer = 0;
	}

	setInverterCallback(Voltronic::RxCBStatic, Voltronic::TxCBStatic);

	DE_Port_ = GPIOx;
	DE_Pin_ = GPIO_Pin;
	Disable_DE
	; // lower the DE         NAK
}

/*Basic check commands*/
void Voltronic::ProtocolID_Inquiry(char *protoID) {
	status stat = Voltronic_OK;

	PrepRxTx(DevProtoID, sizeof(DevProtoID) - 1, 1000, _CR, 1);
	stat = check_eventTimeout(rx_evt, 1200);
	if (stat == Voltronic_OK) {
		if (Rxbuffer[0] == '(' && Rxbuffer[1] == 'P' && Rxbuffer[2] == 'I') {
			// Copy the two characters representing NN into buf
			protoID[0] = Rxbuffer[3];
			protoID[1] = Rxbuffer[4];
			protoID[2] = '\0'; // Null-terminate the string
			return; // Success
		} else {
			std::strcpy(protoID, "NULL");
		}
	} else {
		std::strcpy(protoID, "NULL");
	}

}
void Voltronic::DeviceID_Inquiry(char *devID) {
	status stat = Voltronic_OK;
	PrepRxTx(DevID, sizeof(DevID) - 1, 1000, _CR, 1);
	stat = check_eventTimeout(rx_evt, 1200);
	if (stat == Voltronic_OK) {

		if (Rxbuffer[0] == '(') {
			std::strncpy(devID, reinterpret_cast<const char*>(Rxbuffer + 1),
					14);
		}

	}

}
void Voltronic::CPUFW_Ver_Inquiry(char *Ver) {
	status stat = Voltronic_OK;
	PrepRxTx(CPUFW, sizeof(CPUFW) - 1, 1000, _CR, 1);
	stat = check_eventTimeout(rx_evt, 1200);
	if (stat == Voltronic_OK) {
		if (Rxbuffer[0] == '(' && Rxbuffer[1] == 'V' && Rxbuffer[2] == 'E'
				&& Rxbuffer[3] == 'R' && Rxbuffer[4] == 'F'
				&& Rxbuffer[5] == 'W' && Rxbuffer[6] == ':') {
			std::strncpy(Ver, reinterpret_cast<const char*>(Rxbuffer + 7), 8);
		}
	}

}

void Voltronic::Model_Inquiry(char *Model) {
	//status stat = Voltronic_OK;

}

void Voltronic::DeviceStat_Inquiry_qpigs(char buf[24][10]) {
	uint16_t len;
	uint8_t i = 0;
	status stat = Voltronic_OK;
	uint8_t *current;
	PrepRxTx(DevGenStatus, sizeof(DevGenStatus) - 1, 1000, _CR, 1);
	stat = check_eventTimeout(rx_evt, 1200);
	if (stat == Voltronic_OK) {

		if (Rxbuffer[0] == '(' && Rxbuffer[1] != 'N' && Rxbuffer[2] != 'A'
				&& Rxbuffer[3] != 'K') {
			current = Rxbuffer;
			while (i < 24
					&& checking.extractdatainsegments(
							reinterpret_cast<char*>(current), buf[i], 10, &len,
							' ') == checking.sys_ok) {
				current += len; //increment buffer to next position we got from function
				i++;
			}

			return;
		}

	}

	/*will reach this point only if something is not right*/
	i = 0;
	while (i < 24) {
		std::strncpy(buf[i], "0", 2);
		i++;
	}

}

void Voltronic::DeviceStat_Inquiry_qipgs2(char buf[3][10]) {

	uint16_t len;
	uint8_t i = 0;
	status stat = Voltronic_OK;
	uint8_t *current;
	PrepRxTx(DevGenStatus2, sizeof(DevGenStatus2) - 1, 1000, _CR, 1);
	stat = check_eventTimeout(rx_evt, 1200);
	if (stat == Voltronic_OK) {

		if (Rxbuffer[0] == '(' && Rxbuffer[1] != 'N' && Rxbuffer[2] != 'A'
				&& Rxbuffer[3] != 'K') {
			current = Rxbuffer;
			while (i < 3
					&& checking.extractdatainsegments(
							reinterpret_cast<char*>(current), buf[i], 10, &len,
							' ') == checking.sys_ok) {
				current += len; //increment buffer to next position we got from function
				i++;
			}

			return;
		}

	}

	/*will reach this point only if something is not right*/
	i = 0;
	while (i < 3) {
		std::strncpy(buf[i], "0", 2);
		i++;
	}

}

void Voltronic::DeviceStat_Inquiry_qipgsN(char buf[29][15]) {

	uint16_t len;
	uint8_t i = 0;
	status stat = Voltronic_OK;
	uint8_t *current;
	PrepRxTx(DevGenStatusN, sizeof(DevGenStatusN) - 1, 1000, _CR, 1);
	stat = check_eventTimeout(rx_evt, 1200);
	if (stat == Voltronic_OK) {

		if (Rxbuffer[0] == '(' && Rxbuffer[1] != 'N' && Rxbuffer[2] != 'A'
				&& Rxbuffer[3] != 'K') {
			current = Rxbuffer;
			while (i < 29
					&& checking.extractdatainsegments(
							reinterpret_cast<char*>(current), buf[i], 15, &len,
							' ') == checking.sys_ok) {
				current += len; //increment buffer to next position we got from function
				i++;
			}

			return;
		}

	}

	/*will reach this point only if something is not right*/
	i = 0;
	while (i < 29) {
		std::strncpy(buf[i], "0", 2);
		i++;
	}

}

void Voltronic::DeviceStat_Inquiry_mode(uint8_t *mode) {
	uint16_t len;
	uint8_t i = 0;
	status stat = Voltronic_OK;
	uint8_t *current;
	char buf[5];
	PrepRxTx(DevModeInquiry, sizeof(DevModeInquiry) - 1, 1000, _CR, 1);
	stat = check_eventTimeout(rx_evt, 1200);
	if (stat == Voltronic_OK) {

		if (Rxbuffer[0] == '(' && Rxbuffer[1] != 'N' && Rxbuffer[2] != 'A'
				&& Rxbuffer[3] != 'K') {
			current = Rxbuffer;
			if (checking.extractdatainsegments(reinterpret_cast<char*>(current),
					buf, 2, &len, ' ') == checking.sys_ok) {

				switch (static_cast<invertermode>(buf[0])) {
				case invertermode::PowerOnMode: {
					*mode = 1;
					break;
				}

				case invertermode::StandByMode: {
					*mode = 2;
					break;
				}

				case invertermode::LineMode: {
					*mode = 3;
					break;
				}

				case invertermode::BatteryMode: {
					*mode = 4;
					break;
				}

				case invertermode::FaultMode: {
					*mode = 5;
					break;
				}

				case invertermode::ShutdownMode: {
					*mode = 6;
					break;
				}

				}

				return;
			}
		}

	}
}

void Voltronic::PrepRxTx(const uint8_t *Txbuf, uint16_t len, uint16_t timeout,
		uint8_t end_char, uint8_t cal_crc) {

	memset(Rxbuffer, 0, BufferLen);
	memset(Txbuffer, 0, BufferLen);
	rst_event(rx_evt);
	rst_event(tx_evt);

	Rx_info.EndChar = end_char;
	Rx_info.Rx = Rxbuffer;
	Rx_info.Rxcount = 0;
	serial_.RxintEn(Rx_info.Rx, BufferLen);

	if (cal_crc == 1) {
		uint16_t crc = CRC16_cal(Txbuf, len - 1);
		memcpy(Txbuffer, Txbuf, len - 1);
		breaking_bits.w = crc;
		Txbuffer[len - 1] = breaking_bits.bytes.Hb;
		len = len + 1;
		Txbuffer[len - 1] = breaking_bits.bytes.Lb;
		len = len + 1;
		Txbuffer[len - 1] = _CR;
		Txbuffer[len] = _EOS;
	} else {
		memcpy(Txbuffer, Txbuf, len);
	}

	Enable_DE
	;
	serial_.TransmitData(Txbuffer, len, timeout);
	Disable_DE
	;
}

inline void Voltronic::set_event(enum eventType event) {
	Event[event] = 1;
}
inline void Voltronic::rst_event(enum eventType event) {
	Event[event] = 0;
}
enum Voltronic::status Voltronic::check_event(enum eventType event) {

	status stat = Voltronic_OK;

	if (Event[event]) {
		stat = Voltronic_OK;
	} else if (!Event[event]) {
		stat = Voltronic_ERR;
	}

	return stat;

}
enum Voltronic::status Voltronic::check_eventForever(enum eventType event) {
	status stat = Voltronic_OK;

	while (check_event(event) != Voltronic_OK) {
		//wait here forever
	}

	return stat;

}
enum Voltronic::status Voltronic::check_eventTimeout(enum eventType event,
		uint16_t timeout) {
	status stat = Voltronic_OK;
	uint32_t prev_time = 0;

	prev_time = HAL_GetTick(); //we get new time before going into while
	while ((check_event(event) != Voltronic_OK)
			&& (HAL_GetTick() - prev_time < timeout)) {
		//wait here until get event or time passes
		System_Rtos::delay(10);
	}

	if (check_event(event) != Voltronic_OK) {
		stat = Voltronic_ERR;
	}

	return stat;

}

uint16_t Voltronic::CRC16_cal(const uint8_t *P, uint16_t len) {
	uint8_t temp;
	uint8_t i;
	uint16_t c;
	uint8_t TT;
	uint16_t crc = 0xFFFF;

	for (c = 0; c < len; c++) {
		temp = P[c];
		crc = crc ^ temp;

		for (i = 0; i < 8; i++) {
			TT = crc & 1;
			crc = crc >> 1;
			crc = crc & 0x7FFF;

			if (TT == 1) {
				crc = crc ^ 0xA001;
			}

			crc = crc & 0xFFFF;
		}
	}

	return crc;
}

/*Interrupt callbacks*/

void Voltronic::RxCB(uint8_t data) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvolatile"
	*Rx_info.Rx++ = data;
	Rx_info.Rxcount++;

	if (data == Rx_info.EndChar) {
		set_event(rx_evt);
		serial_.RxintDis();
	}
#pragma GCC diagnostic pop
}

void Voltronic::TxCB(UART_HandleTypeDef *huart) {

}

void Voltronic::RxCBStatic(UART_HandleTypeDef *huart, uint8_t data) {
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
void Voltronic::TxCBStatic(UART_HandleTypeDef *huart) {

}

}
