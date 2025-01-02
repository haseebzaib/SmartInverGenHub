/*
 * UI_Subs.cpp
 *
 *  Created on: Jan 2, 2025
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
#include "button.hpp"
#include "UI_txt.hpp"

uint32_t epochTimePos[10] = {
		1000000000,
		100000000,
		10000000,
		1000000,
		100000,
		10000,
		1000,
		100,
		10,
		1,
};

int8_t datetime[14] = {
    10,
	1,
	10,
	1,
	10,
	1,
	10,
	1,
	10,
	1,
	10,
	1,
	10,
	1
};



void UI::SetTimeDate(u8g2_t *u8g2) {
	enum button::btncodes btncodes;
	char epochtimeStr[50];

	uint32_t epochTime;
	RTC_DateTypeDef DDate;
	RTC_TimeTypeDef DTime;

	int8_t timezone;
	stmRTC.getTime(&DDate, &DTime, &epochTime,&timezone);

	int8_t selEpochPos = 0;
	int8_t prevselPos = 0;





	do {
		button::resetCode(button::btncodes::cNONE);

		do {
			u8g2_ClearBuffer(u8g2);
			u8g2_SetFontMode(u8g2, 1);
			u8g2_SetDrawColor(u8g2, 2);
			u8g2_SetFont(u8g2, u8g2_font_5x8_mf);

			u8g2_DrawStr(u8g2, 10, 10, "Set Time 24hr Format");


			std::sprintf(epochtimeStr, "%02d:%02d:%02d|%02d/%02d/%02d %02d", DTime.Hours, DTime.Minutes,DTime.Seconds,
					                                                         DDate.Date,DDate.Month,DDate.Year,timezone);


        	u8g2_DrawStr(u8g2, 5, 30, epochtimeStr);








         	if(timezone < -9 && selEpochPos >= 12)
            	{
         		u8g2_DrawBox(u8g2, 5 + (5*(prevselPos+1))  , 22, 5, 9);
            	}
         	else
         	{
         		u8g2_DrawBox(u8g2, 5 + (5*prevselPos)  , 22, 5, 9);
         	}




			u8g2_SendBuffer(u8g2);
			btncodes = button::get_event();
		} while (btncodes == button::btncodes::cNONE);

        switch(btncodes)
        {

        case button::btncodes::cRGHT_BT:
        {
        	selEpochPos++;

        	if(selEpochPos < 6)
        	{
        		if(selEpochPos > 2)
        		{
        			if(selEpochPos >= 4)
        			 {
        				prevselPos = selEpochPos+2;
        			 }
        			else
        			{
        				prevselPos = selEpochPos+1;
        			}
        		}
        		else {
        			if(selEpochPos == 2)
        			{
        				prevselPos = prevselPos+2;
        			}
        			else
        			{
        				prevselPos = selEpochPos;
        			}
        		}

        	}
        	else
        	{
        		if(selEpochPos == 6 || selEpochPos == 8 || selEpochPos == 10 || selEpochPos == 12)
        		{
        			prevselPos = prevselPos+2;
        		}
        		else
        		{
        			prevselPos = prevselPos+1;
        		}
        	}


        	break;
        }



        case button::btncodes::cDWN_BT:
        {

        	switch(selEpochPos)
        	{
        	case 0:
        	case 1:
        	{
        		DTime.Hours = DTime.Hours -  datetime[selEpochPos];
        		break;
        	}
        	case 2:
        	case 3:
        	{
        		DTime.Minutes = DTime.Minutes -  datetime[selEpochPos];
        		break;
        	}
        	case 4:
        	case 5:
        	{
        		DTime.Seconds = DTime.Seconds -  datetime[selEpochPos];
        		break;
        	}
        	case 6:
        	case 7:
        	{
        		DDate.Date = DDate.Date -  datetime[selEpochPos];
        		break;
        	}
        	case 8:
        	case 9:
        	{
        		DDate.Month =  DDate.Month -  datetime[selEpochPos];
        		break;
        	}
        	case 10:
        	case 11:
        	{
        		DDate.Year = DDate.Year -  datetime[selEpochPos];
        		break;
        	}
        	case 12:
        	case 13:
        	{
        		timezone = timezone -  datetime[selEpochPos];
        		break;

        	}


        	}


        	break;
        }

        case button::btncodes::cUP_BT:
        {
        	switch(selEpochPos)
        	{
        	case 0:
        	case 1:
        	{
        		DTime.Hours = DTime.Hours +  datetime[selEpochPos];
        		break;
        	}
        	case 2:
        	case 3:
        	{
        		DTime.Minutes = DTime.Minutes +  datetime[selEpochPos];
        		break;
        	}
        	case 4:
        	case 5:
        	{
        		DTime.Seconds = DTime.Seconds +  datetime[selEpochPos];
        		break;
        	}
        	case 6:
        	case 7:
        	{
        		DDate.Date = DDate.Date +  datetime[selEpochPos];
        		break;
        	}
        	case 8:
        	case 9:
        	{
        		DDate.Month =  DDate.Month +  datetime[selEpochPos];
        		break;
        	}
        	case 10:
        	case 11:
        	{
        		DDate.Year = DDate.Year +  datetime[selEpochPos];
        		break;
        	}
        	case 12:
        	case 13:
        	{
        		timezone = timezone +  datetime[selEpochPos];
        		break;
        	}


        	}

        	break;
        }

        case button::btncodes::cEnter_BT:
        {

        	stmRTC.setTime(&DDate,&DTime, timezone);

        	break;
        }

        }

        selEpochPos = selEpochPos % 14;
        prevselPos = prevselPos % 20;


        if(DTime.Hours > 24)
        {
        	DTime.Hours = 24;
        }
        if(DTime.Minutes > 59)
        {
        	DTime.Minutes = 59;
        }
        if(DTime.Seconds > 59)
        {
          DTime.Seconds = 59;
        }

        if(DDate.Date > 31)
        {
        	DDate.Date = 31;
        }

        if(DDate.Month > 12)
        {
        	DDate.Month = 12;
        }

        if(DDate.Year > 99)
        {
        	DDate.Year = 99;
        }

        if(timezone > 13)
        {
        	timezone = 13;
        }
        if(timezone < -11)
        {
        	timezone = -11;

        }




	} while (btncodes !=  button::btncodes::cEnter_BT && btncodes !=  button::btncodes::cLFT_BT);



}




void UI::SetFuelMeas(u8g2_t *u8g2) {
	enum button::btncodes btncodes;
	do {
		button::resetCode(button::btncodes::cNONE);
		do {
			u8g2_ClearBuffer(u8g2);

			u8g2_SendBuffer(u8g2);
			btncodes = button::get_event();
		} while (btncodes == button::btncodes::cNONE);

	} while (btncodes != button::btncodes::cLFT_BT);

}
