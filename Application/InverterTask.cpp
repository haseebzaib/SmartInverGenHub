/*
 * InterfaceTask.cpp
 *
 *  Created on: Nov 8, 2024
 *      Author: user
 */

#include "app_main.hpp"
#include "Inverter/Inverter.hpp"
#include "System/System_Rtos.hpp"
#include "System/System_sys.hpp"
#include "Init.hpp"
#include "cstdio"
#include "cstring"
#include "gpio.h"
#include "main.h"

Inverter::Voltronic Voltronic(&INVERTER_USART3,DE_GPIO_Port,DE_Pin);

char protoID[20];
char devID[20];
char ver[20];




void InverterTask(void *pvParameters) {

	InverterData_Queue InverterData = {0};

	System_Rtos::delay(1000);
	while (1) {

		//Voltronic.ProtocolID_Inquiry(protoID);
		//Voltronic.DeviceID_Inquiry(devID);
		//Voltronic.CPUFW_Ver_Inquiry(ver);
		Voltronic.DeviceStat_Inquiry_qpigs(InverterData.inverterData_qpigs);
		//Voltronic.DeviceStat_Inquiry_qipgs2(InverterData.inverterData_qpigs2);
		//Voltronic.DeviceStat_Inquiry_qipgsN(InverterData.inverterData_qpigsN);
		Voltronic.DeviceStat_Inquiry_mode(&InverterData.device_mode);

		InverterDataQueue.queueSend(reinterpret_cast<void*>(&InverterData));

		System_Rtos::delay(1000);
	}

}



