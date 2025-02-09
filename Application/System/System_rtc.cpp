/*
 * System_rtc.cpp
 *
 *  Created on: Dec 31, 2024
 *      Author: user
 */

#include "System_rtc.hpp"
#include "cstdio"
#include "cstring"
#include "cstdlib"

namespace System_rtc {

stmRTC::stmRTC(RTC_HandleTypeDef *hrtc, int8_t timezone) :
		checking(), rtcSemaphore() {
	Internal_handler = hrtc;
	zone = timezone;
	rtcSemaphore.semaphoreCreate();
}

uint8_t stmRTC::getTime(RTC_DateTypeDef *DDate, RTC_TimeTypeDef *DTime,
		uint32_t *timestamp) {
	int err = 1;
	if (rtcSemaphore.semaphoreTake(4000)
			== System_Rtos::freertos_semaphore::semaphore_recived) {
		RTC_DateTypeDef Date;
		RTC_TimeTypeDef Time;
		HAL_RTC_GetTime(Internal_handler, &Time, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(Internal_handler, &Date, RTC_FORMAT_BIN);

		if (timestamp != nullptr) {
			*timestamp = checking.convertToEpoch(Date, Time, zone);

		}
		if (DDate != nullptr) {
			std::memcpy(DDate, &Date, sizeof(RTC_DateTypeDef));
		}

		if (DTime != nullptr) {
			std::memcpy(DTime, &Time, sizeof(RTC_TimeTypeDef));
		}

		rtcSemaphore.semaphoreGive();

		err = 0;
	} else {
		if (timestamp != nullptr) {
			*timestamp = 0;

		}
		if (DDate != nullptr) {
			std::memcpy(DDate, 0, sizeof(RTC_DateTypeDef));
		}

		if (DTime != nullptr) {
			std::memcpy(DTime, 0, sizeof(RTC_TimeTypeDef));
		}

	}

	return err;

}

uint8_t stmRTC::getTime(RTC_DateTypeDef *DDate, RTC_TimeTypeDef *DTime,
		uint32_t *timestamp, int8_t *timezone) {
	int err = 1;
	if (rtcSemaphore.semaphoreTake(1000)
			== System_Rtos::freertos_semaphore::semaphore_recived) {

		RTC_DateTypeDef Date;
		RTC_TimeTypeDef Time;
		HAL_RTC_GetTime(Internal_handler, &Time, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(Internal_handler, &Date, RTC_FORMAT_BIN);

		*timezone = zone;

		if (timestamp != nullptr) {
			*timestamp = checking.convertToEpoch(Date, Time, zone);

		}
		if (DDate != nullptr) {
			std::memcpy(DDate, &Date, sizeof(RTC_DateTypeDef));
		}

		if (DTime != nullptr) {
			std::memcpy(DTime, &Time, sizeof(RTC_TimeTypeDef));
		}

		rtcSemaphore.semaphoreGive();

		err = 0;
	} else {
		if (timestamp != nullptr) {
			*timestamp = 0;

		}
		if (DDate != nullptr) {
			std::memcpy(DDate, 0, sizeof(RTC_DateTypeDef));
		}

		if (DTime != nullptr) {
			std::memcpy(DTime, 0, sizeof(RTC_TimeTypeDef));
		}

	}

	return err;

}

uint8_t stmRTC::setTime(uint32_t epoch, int8_t timezone) {

	int err = 1;
	if (rtcSemaphore.semaphoreTake(1000)
			== System_Rtos::freertos_semaphore::semaphore_recived) {
		checking.convertEpochToSTMTime(Internal_handler, epoch, timezone);
		zone = timezone;
		rtcSemaphore.semaphoreGive();
		err = 0;
	}

	return err;

}

uint8_t stmRTC::setTime(RTC_DateTypeDef *DDate, RTC_TimeTypeDef *DTime,
		int8_t timezone) {
	int err = 1;
	if (rtcSemaphore.semaphoreTake(1000)
			== System_Rtos::freertos_semaphore::semaphore_recived) {
		HAL_RTC_SetTime(Internal_handler, DTime, RTC_FORMAT_BIN);
		HAL_RTC_SetDate(Internal_handler, DDate, RTC_FORMAT_BIN);
		zone = timezone;

		RTC_DateTypeDef Date;
		RTC_TimeTypeDef Time;
		HAL_RTC_GetTime(Internal_handler, &Time, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(Internal_handler, &Date, RTC_FORMAT_BIN);

		rtcSemaphore.semaphoreGive();
		err = 0;
	}

	return err;
}

uint8_t stmRTC::epochToTimeString(uint32_t epoch, int8_t timezone,
		char *Timestring) {
	int err = 1;
	if (rtcSemaphore.semaphoreTake(1000)
			== System_Rtos::freertos_semaphore::semaphore_recived) {
		checking.convertEpochToTimeString(epoch, timezone, Timestring);
		rtcSemaphore.semaphoreGive();

		err = 0;
	}

	return err;
}

void stmRTC::setTimezone(int8_t timezone)
{
	zone = timezone;
}
}

