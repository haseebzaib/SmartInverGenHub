/*
 * System_rtc.hpp
 *
 *  Created on: Dec 31, 2024
 *      Author: user
 */

#ifndef SYSTEM_SYSTEM_RTC_HPP_
#define SYSTEM_SYSTEM_RTC_HPP_

#include "main.h"
#include "System_sys.hpp"
#include "System_Rtos.hpp"

namespace System_rtc {


class stmRTC {

public:
	stmRTC(RTC_HandleTypeDef *hrtc,int8_t timezone);
    uint8_t getTime(RTC_DateTypeDef *DDate,RTC_TimeTypeDef *DTime,uint32_t *timestamp);
    uint8_t getTime(RTC_DateTypeDef *DDate,RTC_TimeTypeDef *DTime,uint32_t *timestamp, int8_t *timezone);
    uint8_t setTime(uint32_t epoch,int8_t timezone);
    uint8_t setTime(RTC_DateTypeDef *DDate,RTC_TimeTypeDef *DTime,int8_t timezone);

private:
    int8_t zone;
    RTC_HandleTypeDef *Internal_handler;
	System_sys::Parsing_Checking checking;
    System_Rtos::freertos_semaphore rtcSemaphore;


};




}



#endif /* SYSTEM_SYSTEM_RTC_HPP_ */
