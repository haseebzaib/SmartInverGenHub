/*
 * app_main.cpp
 *
 *  Created on: Nov 1, 2024
 *      Author: user
 */


#include "app_main.hpp"
#include "Init.hpp"


System_Rtos::freertos_Tasks ModemTaskHandler(ModemTask,"Modem",_StackSize_Modem, (void*) 1,24);
System_Rtos::freertos_Tasks InverterTaskHandler(InverterTask,"Inverter",_StackSize_Inverter, (void*) 1,24);
System_Rtos::freertos_Tasks ControlnDDisplayTaskHandler(ControlnDDisplayTask,"ControlDis",_StackSize_ControlnDDisplay, (void*) 1,24);


uint8_t queuebuffer[1*sizeof(struct ModemData_Queue)];
System_Rtos::freertos_queues ModemDataQueue(1,sizeof(struct ModemData_Queue),	queuebuffer);

uint8_t queuebuffer_1[1*sizeof(struct ControlData_Queue)];
System_Rtos::freertos_queues ControlDataQueue(1,sizeof(struct ControlData_Queue),	queuebuffer_1);

uint8_t queuebuffer_2[1*sizeof(struct InverterData_Queue)];
System_Rtos::freertos_queues InverterDataQueue(1,sizeof(struct InverterData_Queue),	queuebuffer_2);

const char kaHexAsc[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void UniqueID::CharToHexasc(uint8_t *pPos, uint8_t Wert)
{
     *pPos++ = kaHexAsc[Wert >> 4];
     *pPos = kaHexAsc[Wert & 0x0f];
}

char *UniqueID::GetUid()
{
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




void app_mainCpp()
{

	ModemTaskHandler.createTask();
	InverterTaskHandler.createTask();
    ControlnDDisplayTaskHandler.createTask();
    ModemDataQueue.queueCreate();
    ControlDataQueue.queueCreate();
    InverterDataQueue.queueCreate();

}



// All the functions that need to be used in C files transport from here
extern "C"
{

    void app_mainC()
    {
    	app_mainCpp();
    }


}
