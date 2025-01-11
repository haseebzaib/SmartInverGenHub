/*
 * System_common.cpp
 *
 *  Created on: Nov 6, 2024
 *      Author: user
 */




#include "cstring"
#include "string"
#include "System_sys.hpp"
#include <ctime>
#include <cmath>
#include <cctype>

//TODO
//Put receiving buffer check as well due to overflow check its length as well

namespace System_sys {

Parsing_Checking::Parsing_Checking()
{

}

enum Parsing_Checking::status Parsing_Checking::checkResponse_OK_ERR(const char *buffer,uint16_t bufferSize)
{
	enum status stat = sys_notFound;

    if(std::strstr(buffer,"OK") != nullptr)
    {
    	stat = sys_ok;

    }
    else if(std::strstr(buffer, "ERROR") != nullptr)
    {
    	stat = sys_err;

    }


    return stat;


}

enum Parsing_Checking::status Parsing_Checking::parseDate_Time_Timezone( char *buffer, uint16_t bufferLen ,char *datebuf, char *timebuf, char *timezonebuf)
{
	enum status stat = sys_ok;

    static const char* months[] = {
        "Err", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

	int Ncolonpos;
	char *Scolonpos;
	int Ncommapos;
	char *Scommapos;
	int Npluspos;
	char *Spluspos;
	int Nminuspos;
	char *Sminuspos;


	char *DatePart;
	char *TimePart;
	char *TimezonePart;


	char temp[100];

	char compare_sign[10];
	int8_t useSign = -1;


	std::strncpy(temp,buffer,bufferLen);

	if(!std::strstr((const char *)temp,"+CCLK:"))
	{

		return sys_notFound;

	}

	Scolonpos = std::strchr(temp, ':');


	if(Scolonpos != nullptr)
	{
		Ncolonpos = Scolonpos - temp;
        Ncolonpos = Ncolonpos + 3;
	}
	else
	{
		return sys_notFound;
	}

	Scommapos = std::strchr(temp, ',');
	if(Scommapos != nullptr)
	{
		Ncommapos = Scommapos - temp;
		Ncommapos = Ncommapos + 1;
	}
	else
	{
		return sys_notFound;
	}


	Sminuspos = std::strchr(temp, '-');
	if(Sminuspos != nullptr)
	{
		Nminuspos = Sminuspos - temp;
		std::strcpy(compare_sign,"-");
		useSign = 0;
	}
	else
	{
		Spluspos = std::strchr(temp, '+');
		if(Spluspos != nullptr)
		{
			Npluspos = Spluspos - temp;
			std::strcpy(compare_sign,"+");
			useSign = 1;
		}
		else
		{
		   return sys_notFound;
		}
	}




	DatePart = std::strtok(temp + Ncolonpos  , ",");



	if(DatePart != nullptr)
	{
	    // Split the date part into day, month, and year
	    char* year = std::strtok(DatePart, "/");
	    int year_ = std::atoi(year);
	    char* monthStr = std::strtok(DatePart+3, "/");
	    int month = std::atoi(monthStr);
	    char* day = std::strtok(DatePart+6, "/");
	    int day_ = std::atoi(day);

	    std::snprintf(datebuf, 20, "%02d-%s-%02d", day_, months[month], year_);

	}
	else
	{
		return sys_notFound;
	}




	TimePart = std::strtok(temp + Ncommapos  , compare_sign);

	if(TimePart != nullptr)
	{
		std::strcpy(timebuf,TimePart);
	}
	else
	{

	  return sys_notFound;

	}


	uint16_t len = strlen(TimePart);

	std::strncpy(timezonebuf,buffer + Ncommapos + len  ,3);



	  return stat;

}

enum Parsing_Checking::status Parsing_Checking::parseSkipCharnDTillEndChar(char *buffer, uint16_t bufferLen,char *CheckString,
		                                                         uint8_t skipChar,uint8_t endChar,char *databuf)
{


	enum status stat = sys_notFound;
    uint16_t len = 0;
	char temp[500];

	std::strncpy(temp,buffer,bufferLen);

	if(!std::strstr((const char *)temp,CheckString))
	{

		return sys_notFound;

	}

	//this means we dont want to check anything below just need to check checkstring
	if(databuf == nullptr )
	{
		return sys_ok;
	}

	int Ncolonpos;
	char *Scolonpos;
	int Nendpos;
	char *Sendpos;


	Scolonpos = std::strchr(temp, skipChar);


	if(Scolonpos != nullptr)
	{
		Ncolonpos = Scolonpos - temp;
		if(skipChar == ':')
        {
			Ncolonpos = Ncolonpos + 2;
        }
		else
		{
			Ncolonpos = Ncolonpos + 1;
		}
	}
	else
	{
		return sys_notFound;
	}


	uint16_t buf_index = Ncolonpos;

	while(len < bufferLen - Ncolonpos && buf_index < bufferLen)
	{

		if(buffer[buf_index] != endChar)
		{
			databuf[len] = buffer[buf_index];

		}else if(buffer[buf_index] == endChar)
		{
			databuf[len] = 0x00;
			return sys_ok;
		}


		buf_index++;
		len++;

	}


	  return stat;
}



enum Parsing_Checking::status Parsing_Checking::extractData(const char *buffer,const char *cmd,char midchar,char endchar,char *outputbuf,uint16_t outputSize)
{
	enum status stat = sys_ok;


	//if buffer is null dont run as it does not make sense to run after this
	if(outputbuf == nullptr)
	{
		return sys_err;
	}

     char* commandStart = std::strstr(buffer, cmd);
    if (!commandStart) {
        // Command not found
        return sys_notFound;
    }



    // Move the pointer to the end of the command
    commandStart += std::strlen(cmd);
    commandStart++; //to skip space


     //Check for the mid character and skip it
     char* midCharPos = std::strchr(commandStart, midchar);
    if (!midCharPos) {
        // Mid character not found
        return sys_notFound;
    }
    commandStart = midCharPos + 1;



    // Copy the remaining part until the end character or end of string
    uint16_t i = 0;
    while (*commandStart != endchar && i < outputSize - 1) {
    	outputbuf[i++] = *commandStart++;
    }
    outputbuf[i] = '\0'; // Null-terminate the output buffer


    return stat;

}

enum Parsing_Checking::status Parsing_Checking::extractdatainsegments(char *input,char *output,uint16_t output_len,uint16_t *get_len,uint8_t skip_char)
{

	enum status stat = sys_ok;


	   if (!input || !output || output_len == 0 || !get_len) {
	        return sys_err; // Invalid parameters
	    }


	   uint16_t len = 0;


	   // Skip leading spaces (if any)
	    while (*input == skip_char || *input == '(') {
	        input++;
	    }

	    // Extract characters until space or end of string
	      while (*input != '\0' && *input != skip_char && len < output_len - 1) {
	          output[len++] = *input++;
	      }

	    // Null-terminate the output
	     output[len] = '\0';

	     // If no data was extracted, return false
	        if (len == 0) {
	            return sys_err;
	        }

	        // Store the extracted length
	         *get_len = len+1;

    return stat;

}

enum Parsing_Checking::status Parsing_Checking::binarystringToUint8(char *binaryString,uint8_t *result)
{
	enum status stat = sys_ok;


	  // Check if the input string is valid
	    size_t length = std::strlen(binaryString);

	    if (length == 0 || length > 8) {
	        return sys_err;
	    }

		*result = 0;
	    // Iterate through each character in the string
	    for (size_t i = 0; i < length; ++i) {
	        char c = binaryString[i];
	        if (c != '0' && c != '1') {
	        	   return sys_err;
	        }

	        // Shift result left by 1 bit and add the current bit
	        *result = (*result << 1) | (c - '0');
	    }



    return stat;

}

uint32_t Parsing_Checking::convertToEpoch(char *simTime)
{
	 int year, month, day, hour, minute, second, timezoneOffsetQuarters;
	    char sign;

	    // Parse the SIM time string
	    sscanf(simTime, "%2d/%2d/%2d,%2d:%2d:%2d%c%2d",
	           &year, &month, &day, &hour, &minute, &second, &sign, &timezoneOffsetQuarters);

	    // Convert `year` to full year (handles 2-digit format)
	    year += (year < 70) ? 2000 : 1900;

	    // Calculate timezone offset in minutes from the SIM string
	    int timezoneOffsetMinutes = timezoneOffsetQuarters * 15;
	    if (sign == '-') {
	        timezoneOffsetMinutes = -timezoneOffsetMinutes;
	    }

	    // Apply the default timezone offset if no specific offset is provided in the SIM string
	    //timezoneOffsetMinutes += defaultTimezoneOffsetMinutes;

	    // Convert date and time to seconds since 1970-01-01 (UTC)
	    uint32_t epoch = 0;

	    // Add seconds for each complete year
	    for (int y = 1970; y < year; y++) {
	        epoch += isLeapYear(y) ? DAYS_IN_LEAP_YEAR : DAYS_IN_YEAR;
	    }
	    epoch *= SECONDS_IN_DAY;

	    // Add seconds for each complete month in the current year
	    for (int m = 1; m < month; m++) {
	        epoch += daysInMonth(year, m) * SECONDS_IN_DAY;
	    }

	    // Add seconds for the days, hours, minutes, and seconds
	    epoch += (day - 1) * SECONDS_IN_DAY;
	    epoch += hour * SECONDS_IN_HOUR;
	    epoch += minute * SECONDS_IN_MINUTE;
	    epoch += second;

	    // Adjust for the final timezone offset
	    epoch -= timezoneOffsetMinutes * SECONDS_IN_MINUTE;

	    return epoch;
}
// Convert the separate buffers to an epoch timestamp specific format
uint32_t Parsing_Checking::convertToEpoch(char *date, char *time, char *zone)
{
	 int day, year, hour, minute, second, timezoneOffset;
	    char monthStr[10];

	    // Parse the date buffer (e.g., "01-Jan-70")
	    sscanf(date, "%2d-%3s-%2d", &day, &monthStr[0], &year);
	    year += (year < 70) ? 2000 : 1900; // Convert year to full format

	    int month = getMonthNumber(monthStr);

	    // Parse the time buffer (e.g., "00:01:11")
	    sscanf(time, "%2d:%2d:%2d", &hour, &minute, &second);

	    // Parse the zone buffer (e.g., "+00" or "-08")
	    char sign;
	    int offsetHours;
	    sscanf(zone, "%c%d", &sign, &offsetHours);
	    timezoneOffset = offsetHours * 15; // Convert offset hours to minutes
	    if (sign == '-') timezoneOffset = -timezoneOffset;

	    // Convert date and time to seconds since 1970-01-01 (UTC)
	    uint32_t epoch = 0;

	    // Add seconds for each complete year
	    for (int y = 1970; y < year; y++) {
	        epoch += isLeapYear(y) ? DAYS_IN_LEAP_YEAR : DAYS_IN_YEAR;
	    }
	    epoch *= SECONDS_IN_DAY;

	    // Add seconds for each complete month in the current year
	    for (int m = 1; m < month; m++) {
	        epoch += daysInMonth(year, m) * SECONDS_IN_DAY;
	    }

	    // Add seconds for the days, hours, minutes, and seconds
	    epoch += (day - 1) * SECONDS_IN_DAY;
	    epoch += hour * SECONDS_IN_HOUR;
	    epoch += minute * SECONDS_IN_MINUTE;
	    epoch += second;

	    // Adjust for the timezone offset
	    epoch -= timezoneOffset * SECONDS_IN_MINUTE;

	    return epoch;
}

uint32_t Parsing_Checking::convertToEpoch(RTC_DateTypeDef sDate,RTC_TimeTypeDef sTime,int8_t zone)
{
	 int day,month ,year, hour, minute, second, timezoneOffset;


	    day = sDate.Date;
		month = sDate.Month;
	    year = sDate.Year;

	    year += (year < 70) ? 2000 : 1900; // Convert year to full format

	    hour = sTime.Hours;
		minute = sTime.Minutes;
		second = sTime.Seconds;

	    timezoneOffset = zone * 60; // Convert offset hours to minutes


	    // Convert date and time to seconds since 1970-01-01 (UTC)
	    uint32_t epoch = 0;

	    // Add seconds for each complete year
	    for (int y = 1970; y < year; y++) {
	        epoch += isLeapYear(y) ? DAYS_IN_LEAP_YEAR : DAYS_IN_YEAR;
	    }
	    epoch *= SECONDS_IN_DAY;

	    // Add seconds for each complete month in the current year
	    for (int m = 1; m < month; m++) {
	        epoch += daysInMonth(year, m) * SECONDS_IN_DAY;
	    }

	    // Add seconds for the days, hours, minutes, and seconds
	    epoch += (day - 1) * SECONDS_IN_DAY;
	    epoch += hour * SECONDS_IN_HOUR;
	    epoch += minute * SECONDS_IN_MINUTE;
	    epoch += second;

	    // Adjust for the timezone offset
	    epoch -= timezoneOffset * SECONDS_IN_MINUTE;

	    return epoch;
}

void Parsing_Checking::convertEpochToSimTime(uint32_t epoch,int timezone,char *recv_buf)
{

	int TimezoneQuaters;
	int TimezoneOffsetMinutes;

	TimezoneQuaters = timezone*4;
	TimezoneOffsetMinutes = TimezoneQuaters*15;



	 // Adjust epoch time by adding the default timezone offset to convert UTC to local time
	    epoch += TimezoneOffsetMinutes * SECONDS_IN_MINUTE;

	    // Convert adjusted epoch time to a UTC tm structure
	    time_t rawTime = epoch;
	    struct tm* timeinfo = gmtime(&rawTime);  // gmtime gives us the time in UTC

	    // Extract date and time components
	    int year = timeinfo->tm_year + 1900;  // tm_year is years since 1900
	    int month = timeinfo->tm_mon + 1;     // tm_mon is months since January (0-11)
	    int day = timeinfo->tm_mday;
	    int hour = timeinfo->tm_hour;
	    int minute = timeinfo->tm_min;
	    int second = timeinfo->tm_sec;



	    char sign = (TimezoneOffsetMinutes < 0) ? '-' : '+';
	    // Format the time in SIM A7672 format "yy/MM/dd,hh:mm:ss±zz"

      std::sprintf(recv_buf,"%02d/%02d/%02d,%02d:%02d:%02d%c%02d",year%100,month,day,hour,minute,second,sign,timezone);


}

void Parsing_Checking::convertEpochToSTMTime(RTC_HandleTypeDef *hrtc,uint32_t epoch,int timezone)
{
uint8_t prevYear;
	RTC_DateTypeDef gDate;
	RTC_TimeTypeDef gTime;

	RTC_DateTypeDef gDate1;
	RTC_TimeTypeDef gTime1;

	int TimezoneQuaters;
	int TimezoneOffsetMinutes;

	TimezoneQuaters = timezone*4;
	TimezoneOffsetMinutes = TimezoneQuaters*15;



	 // Adjust epoch time by adding the default timezone offset to convert UTC to local time
	    epoch += TimezoneOffsetMinutes * SECONDS_IN_MINUTE;

	    // Convert adjusted epoch time to a UTC tm structure
	    time_t rawTime = epoch;
	    struct tm* timeinfo = gmtime(&rawTime);  // gmtime gives us the time in UTC

	    // Extract date and time components
	    gDate.Year = (timeinfo->tm_year + 1900) % 100;  // tm_year is years since 1900
	    prevYear = gDate.Year;
	    gDate.Month = timeinfo->tm_mon + 1;     // tm_mon is months since January (0-11)
	    gDate.Date = timeinfo->tm_mday;
	    gTime.Hours = timeinfo->tm_hour;
	    gTime.Minutes = timeinfo->tm_min;
	    gTime.Seconds = timeinfo->tm_sec;

	    HAL_RTC_SetTime(hrtc, &gTime, RTC_FORMAT_BIN);
	    HAL_RTC_SetDate(hrtc, &gDate, RTC_FORMAT_BIN);

		HAL_RTC_GetTime(hrtc, &gTime1, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(hrtc, &gDate1, RTC_FORMAT_BIN);

		if(gDate1.Year != prevYear)
		{
			gDate.Year = prevYear;
			 HAL_RTC_SetDate(hrtc, &gDate, RTC_FORMAT_BIN);


				HAL_RTC_GetDate(hrtc, &gDate1, RTC_FORMAT_BIN);
		}

}

void Parsing_Checking::convertEpochToTimeString(uint32_t epoch,int timezone,char *Timestring)
{
	uint8_t prevYear;

		int TimezoneQuaters;
		int TimezoneOffsetMinutes;

		TimezoneQuaters = timezone*4;
		TimezoneOffsetMinutes = TimezoneQuaters*15;


		int hour;
		int min;
		int sec;


		 // Adjust epoch time by adding the default timezone offset to convert UTC to local time
		    epoch += TimezoneOffsetMinutes * SECONDS_IN_MINUTE;

		    // Convert adjusted epoch time to a UTC tm structure
		    time_t rawTime = epoch;
		    struct tm* timeinfo = gmtime(&rawTime);  // gmtime gives us the time in UTC

		    // Extract date and time components

		    hour = timeinfo->tm_hour;
		    min = timeinfo->tm_min;
		    sec = timeinfo->tm_sec;

		    std::sprintf(Timestring,"%02d:%02d:%02d", hour, sec);



}

bool Parsing_Checking::isLeapYear(int year)
{
	  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}
int Parsing_Checking::daysInMonth(int year, int month)
{
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    return monthDays[month - 1];
}
int Parsing_Checking::getMonthNumber(char *monthStr)
{
    if (std::strstr(monthStr, "Jan")) return 1;
    if (std::strstr(monthStr, "Feb"))  return 2;
    if (std::strstr(monthStr, "Mar")) return 3;
    if (std::strstr(monthStr, "Apr")) return 4;
    if (std::strstr(monthStr, "May")) return 5;
    if (std::strstr(monthStr, "Jun")) return 6;
    if (std::strstr(monthStr, "Jul")) return 7;
    if (std::strstr(monthStr, "Aug")) return 8;
    if (std::strstr(monthStr, "Sep")) return 9;
    if (std::strstr(monthStr, "Oct")) return 10;
    if (std::strstr(monthStr, "Nov")) return 11;
    if (std::strstr(monthStr, "Dec")) return 12;
    return -1; // Invalid month
}



}
