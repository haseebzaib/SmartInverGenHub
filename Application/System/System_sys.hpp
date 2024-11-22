/*
 * System_common.hpp
 *
 *  Created on: Nov 6, 2024
 *      Author: user
 */

#ifndef SYSTEM_SYSTEM_SYS_HPP_
#define SYSTEM_SYSTEM_SYS_HPP_

#include "main.h"


namespace System_sys {

class Parsing_Checking {
public:
	enum status {
		sys_ok,
		sys_err,
		sys_notFound,
	};


	Parsing_Checking();

	enum status checkResponse_OK_ERR(const char *buffer,uint16_t bufferSize);
    enum status parseDate_Time_Timezone( char *buffer,  uint16_t bufferLen, char *datebuf, char *timebuf, char *timezonebuf );
    enum status parseSkipCharnDTillEndChar(char *buffer, uint16_t bufferLen,char *CheckString,uint8_t skipChar,uint8_t endChar,char *databuf);
    enum status extractData(const char *buffer,const char *cmd,char midchar,char endchar,char *outputbuf,uint16_t outputSize);
    enum status extractdatainsegments(char *input,char *output,uint16_t output_len,uint16_t *get_len,uint8_t skip_char);
    enum status binarystringToUint8(char *binaryString,uint8_t *result);


    int32_t convertToEpoch(char *simTime);
    // Convert the separate buffers to an epoch timestamp specific format
    int32_t convertToEpoch(char *date, char *time, char *zone);
    int32_t convertToEpoch(RTC_DateTypeDef sDate,RTC_TimeTypeDef sTime,int8_t zone);

    void convertEpochToSimTime(int32_t epoch,int timezone,char *recv_buf);
    void convertEpochToSTMTime(RTC_HandleTypeDef *hrtc,int32_t epoch,int timezone);


private:
    static constexpr int SECONDS_IN_MINUTE = 60;
    static constexpr int SECONDS_IN_HOUR = 3600;
    static constexpr int SECONDS_IN_DAY = 86400;
    static constexpr int DAYS_IN_YEAR = 365;
    static constexpr int DAYS_IN_LEAP_YEAR = 366;
    static constexpr int monthDays[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


    bool isLeapYear(int year);
    int daysInMonth(int year, int month);
    int getMonthNumber(char *monthStr);
};


}
#endif
