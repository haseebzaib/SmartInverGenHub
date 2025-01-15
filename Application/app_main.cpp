/*
 * app_main.cpp
 *
 *  Created on: Nov 1, 2024
 *      Author: user
 */

#include "app_main.hpp"
#include "Init.hpp"
#include "rtc.h"
#include "i2c.h"
#include "button.hpp"
#include "usart.h"
#include "adc.h"
#include "cstring"
#include "string"
#include <ctime>
#include <cmath>
#include <cctype>

struct flash_data flash_data_;

System_Rtos::freertos_Tasks ModemTaskHandler(ModemTask, "Modem",
		_StackSize_Modem, (void*) 1, 24);
System_Rtos::freertos_Tasks ControlTaskHandler(ControlTask, "Control",
		_StackSize_Control, (void*) 1, 24);
System_Rtos::freertos_Tasks DisplayTaskHandler(DisplayTask, "Display",
		_StackSize_Display, (void*) 1, 24);
//System_Rtos::freertos_Tasks SoCTaskHandler(SoCTask,"SoC",_StackSize_SoC, (void*) 1,24);

uint8_t queuebuffer[1 * sizeof(struct ModemData_Queue)];
System_Rtos::freertos_queues ModemDataQueue(1, sizeof(struct ModemData_Queue),
		queuebuffer);

uint8_t queuebuffer_1[1 * sizeof(struct ControlData_Queue)];
System_Rtos::freertos_queues ControlDataQueue(1,
		sizeof(struct ControlData_Queue), queuebuffer_1);

//uint8_t queuebuffer_2[1*sizeof(struct SoCData_Queue)];
//System_Rtos::freertos_queues SoCDataQueue(1,sizeof(struct SoCData_Queue),	queuebuffer_2);

System_Rtos::freertos_semaphore FlashSemaphore;

System_rtc::stmRTC stmRTC(&hrtc, 5);
sensor_liquidMeas::liquidSensor liquidSensor(&hadc1, 0.0, 10.0, 3.3);
sensor_TempHumd::AHT20 AHT20(&hi2c1, 0x38);

sensor_pzem::PZEM_004T PZEM1(&PZEM1_U);
sensor_pzem::PZEM_004T PZEM2(&PZEM2_U);
sensor_pzem::PZEM_004T PZEM3(&PZEM3_U);

sensor_DcHall::DcHall DCCurrentSensor(&hadc2);
sensor_DcVolt::DcVolt DCVoltageMeasurement(&hadc3);

const char kaHexAsc[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F' };

#define Address_Mem 0x080E0000

void UniqueID::CharToHexasc(uint8_t *pPos, uint8_t Wert) {
	*pPos++ = kaHexAsc[Wert >> 4];
	*pPos = kaHexAsc[Wert & 0x0f];
}

char* UniqueID::GetUid() {
	static uint8_t sUid[13];
	uint32_t iWafer;

	iWafer = HAL_GetUIDw1() & 0x00FFul;
	// 7 lot numbers are already in ASCII
	*(uint32_t*) (&sUid[0]) = HAL_GetUIDw1() >> 8;
	*(uint32_t*) (&sUid[3]) = HAL_GetUIDw2();

	// Convert wafer number to 1 ASCII character
	if (iWafer >= 36)
		sUid[7] = (iWafer >= 62) ? 'z' : ('a' + (iWafer - 36));
	// Display with 10 + 26 + 26 symbols
	else
		sUid[7] = ((iWafer >= 10) ? 'A' : '0') + iWafer;

	// Convert X and Y to 4 ASCII-HEX
	CharToHexasc(&sUid[8], (HAL_GetUIDw0() >> 16) & 0x00FFul);
	CharToHexasc(&sUid[10], HAL_GetUIDw0() & 0x00FFul);
	sUid[12] = 0;
	return ((char*) sUid);
}

void SaveData() {

	if (FlashSemaphore.semaphoreTake(10000)
			== System_Rtos::freertos_semaphore::semaphore_recived) {

		taskENTER_CRITICAL();

		uint32_t ZielAdr;
		uint32_t WrAdr;
		uint32_t QuellAdr = (uint32_t) &flash_data_;
		static FLASH_EraseInitTypeDef EraseInitStruct;
		uint32_t SECTORError;

		HAL_FLASH_Unlock();
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPERR);
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR);
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGAERR);
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGPERR);
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGSERR);

		EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
		EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
		EraseInitStruct.Sector = FLASH_SECTOR_11;
		EraseInitStruct.NbSectors = 1;

		if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK) {
			return;
		}

//  Par-Flash schreiben

		ZielAdr = Address_Mem + sizeof(struct flash_data);
		WrAdr = Address_Mem; // Pointer on 1st entry

		while (WrAdr < ZielAdr) {

			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, WrAdr,
					*(uint64_t*) QuellAdr) == HAL_OK)

			{
				WrAdr += 4;
				QuellAdr += 4;

			} else {

				HAL_FLASH_Lock();
				return;

			}

		}
		HAL_FLASH_Lock();

		taskEXIT_CRITICAL();

		FlashSemaphore.semaphoreGive();

	}

}

void getSaveData() {

	uint8_t save = 0;

	struct flash_data *pFlashData;

	pFlashData = (struct flash_data *) Address_Mem;



	if(pFlashData->validPattern != 0xFEED)
	{
		flash_data_.validPattern =  0xFEED;
		flash_data_.zeroSpan     =  0;
		flash_data_.fullSpan     =  5;
		flash_data_.SOC          =  100;
		flash_data_.zone         =  +5;
	}
	else
	{
		std::memcpy(&flash_data_,pFlashData, sizeof(struct flash_data));
	}







}

void app_mainCpp() {

	ModemTaskHandler.createTask();
	ControlTaskHandler.createTask();
	DisplayTaskHandler.createTask();
	//SoCTaskHandler.createTask();

	ModemDataQueue.queueCreate();
	ControlDataQueue.queueCreate();
	//SoCDataQueue.queueCreate();
	FlashSemaphore.semaphoreCreate();

	button::btn_init();

}

// All the functions that need to be used in C files transport from here
extern "C" {

void app_mainC() {
	app_mainCpp();
}

}

