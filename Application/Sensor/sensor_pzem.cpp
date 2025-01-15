/*
 * sensor_pzem.cpp
 *
 *  Created on: Nov 22, 2024
 *      Author: user
 */

#include "System_serial.hpp"
#include "System_Rtos.hpp"
#include "sensor_pzem.hpp"
#include "cstdio"
#include "cstring"
#include "cstdlib"
namespace sensor_pzem {

PZEM_004T::PZEM_004T(UART_HandleTypeDef *huart) :
		serial_(huart), PzemSemaphore() {
	huart_ = huart;
	PzemSemaphore.semaphoreCreate();
}

void PZEM_004T::init() {
	if (PzemSemaphore.semaphoreTake(1000)
			== System_Rtos::freertos_semaphore::semaphore_recived) {
	serial_.TransmitData(rst_buf, 4, 1000);
	serial_.ReceiveData(res_buf, 25, 500);
	PzemSemaphore.semaphoreGive();
	}
}
void PZEM_004T::read(PZEM *pzemhandle) {

	if (PzemSemaphore.semaphoreTake(1000)
			== System_Rtos::freertos_semaphore::semaphore_recived) {
	serial_.TransmitData(rst_buf, 4, 1000);
	serial_.ReceiveData(res_buf, 25, 500);


	System_Rtos::delay(1000);

	serial_.TransmitData(buf, 8, 1000);
	serial_.ReceiveData(res_buf, 25, 1000);


	pzemhandle->voltage = ((uint32_t)res_buf[3] << 8 | (uint32_t)res_buf[4]) / 10.0;
	pzemhandle->current = ((uint32_t)res_buf[5] << 8 | (uint32_t)res_buf[6]
			| (uint32_t)res_buf[7] << 24 | (uint32_t)res_buf[8] << 16) / 100.0;
	pzemhandle->power = ((uint32_t)res_buf[9] << 8 | (uint32_t)res_buf[10]
			| (uint32_t)res_buf[11] << 24 | (uint32_t)res_buf[12] << 16);
	pzemhandle->energy = ((uint32_t)res_buf[13] << 8 | (uint32_t)res_buf[14]
			| (uint32_t)res_buf[15] << 24 | (uint32_t)res_buf[16] << 16);
	pzemhandle->frequency = ((uint32_t)res_buf[17] << 8 | (uint32_t)res_buf[18]) / 10.0;
	pzemhandle->pf = ((uint32_t)res_buf[19] << 8 | (uint32_t)res_buf[20]) / 100.0;
	pzemhandle->alarms = ((uint32_t)res_buf[21] << 8 | (uint32_t)res_buf[22]);

	PzemSemaphore.semaphoreGive();
	}
	else
	{
		memset(pzemhandle,0,sizeof(PZEM));
	}

}

void PZEM_004T::read(PZEM *pzemhandle,uint32_t timeout)
{

	if (PzemSemaphore.semaphoreTake(timeout)
			== System_Rtos::freertos_semaphore::semaphore_recived) {
	serial_.TransmitData(rst_buf, 4, 1000);
	serial_.ReceiveData(res_buf, 25, 500);


	System_Rtos::delay(1000);

	serial_.TransmitData(buf, 8, 1000);
	serial_.ReceiveData(res_buf, 25, 1000);


	pzemhandle->voltage = ((uint32_t)res_buf[3] << 8 | (uint32_t)res_buf[4]) / 10.0;
	pzemhandle->current = ((uint32_t)res_buf[5] << 8 | (uint32_t)res_buf[6]
			| (uint32_t)res_buf[7] << 24 | (uint32_t)res_buf[8] << 16) / 100.0;
	pzemhandle->power = ((uint32_t)res_buf[9] << 8 | (uint32_t)res_buf[10]
			| (uint32_t)res_buf[11] << 24 | (uint32_t)res_buf[12] << 16);
	pzemhandle->energy = ((uint32_t)res_buf[13] << 8 | (uint32_t)res_buf[14]
			| (uint32_t)res_buf[15] << 24 | (uint32_t)res_buf[16] << 16);
	pzemhandle->frequency = ((uint32_t)res_buf[17] << 8 | (uint32_t)res_buf[18]) / 10.0;
	pzemhandle->pf = ((uint32_t)res_buf[19] << 8 | (uint32_t)res_buf[20]) / 100.0;
	pzemhandle->alarms = ((uint32_t)res_buf[21] << 8 | (uint32_t)res_buf[22]);

	PzemSemaphore.semaphoreGive();
	}

}

}
