/*
 * UI_helper.cpp
 *
 *  Created on: Dec 31, 2024
 *      Author: user
 */




#include "cstdio"
#include "cstring"
#include "cstdlib"
#include "u8g2.h"
#include "spi.h"
#include "Init.hpp"
#include "System/System_Rtos.hpp"
#include "System/System_sys.hpp"
#include "rtc.h"
#include "UI.hpp"
#include "img.hpp"





void UI::common_iconsMain(u8g2_t *u8g2)
{
	char time[50];
	char date[50];
	RTC_DateTypeDef DDate;
	RTC_TimeTypeDef DTime;
	stmRTC.getTime(&DDate, &DTime, nullptr);
	std::sprintf(time, "%02d:%02d:%02d", DTime.Hours, DTime.Minutes,DTime.Seconds);
	std::sprintf(date, "%02d/%02d/%02d", DDate.Date,DDate.Month,DDate.Year);
	u8g2_SetFont(u8g2, u8g2_font_5x8_mf);
	u8g2_DrawStr(u8g2, 0, 10, time);
	u8g2_DrawStr(u8g2, 88, 10, date);
	u8g2_DrawXBM(u8g2,55,48,setting.w,setting.h,setting.img);
	u8g2_DrawXBM(u8g2,112,48,leftrightArrow.w,leftrightArrow.h,leftrightArrow.img);






}