/*
 * System_Rtos.hpp
 *
 *  Created on: Nov 8, 2024
 *      Author: user
 */

#ifndef SYSTEM_SYSTEM_RTOS_HPP_
#define SYSTEM_SYSTEM_RTOS_HPP_

#include "main.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"



namespace System_Rtos {

void delay(uint32_t d);

class freertos_Tasks {
public:
	freertos_Tasks(TaskFunction_t pxTaskCode_,
			const char * pcName_,		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
			const configSTACK_DEPTH_TYPE usStackDepth_,
			void * const pvParameters_,
			UBaseType_t uxPriority_);

	void createTask();
	void suspend();
	void resume();



private:
	TaskFunction_t pxTask;
    char pcName[15];
	configSTACK_DEPTH_TYPE usStackDepth;
	void *pvParameters;
	UBaseType_t uxPriority;
	TaskHandle_t taskHandle;




};


class freertos_queues {
public:
	enum queues_stat{
		queues_recived = 0,
		queues_sent = 1,
		queues_nothing = 2,
		queues_err = 3,
		queues_ok =4,
	};

	freertos_queues(uint16_t queLen_,uint16_t itemsize_, uint8_t *pucQueueStorage_);
	void queueCreate();
	enum queues_stat queueSend(void *SendBuf);
	enum queues_stat queueReceive(void *RecvBuf);

private:

	QueueHandle_t xQueue;
	StaticQueue_t xStaticQueue;

	uint16_t queLen;
	uint16_t itemsize;
	uint8_t *pucQueueStorage;





};




}



#endif /* SYSTEM_SYSTEM_RTOS_HPP_ */
