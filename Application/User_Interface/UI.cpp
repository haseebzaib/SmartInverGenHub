/*
 * UI.cpp
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
#include "button.hpp"
#include "UI_txt.hpp"
#include "sensor_DcHall.hpp"
#include "SOC.hpp"


#define SCROLLING

#define scrollTiming 2
#define TotalEnterIndexes 10

uint8_t FuelPercent = 0;
uint32_t temp = 0;
uint32_t humid = 0;


struct SettingMenuCodes SettingsMenuCodes[TotalSettingsSub] =
{
  MenuNo::SetTimeDate,
  MenuNo::SetFuelMeas,
  MenuNo::SetSoCnDCurr,

};

namespace UI {

char display_buffer[255] = { 0 };
enum MenuNo CurrMenu = MenuNo::None;

using UIFunc = void(*)(u8g2_t *u8g2);

struct UIStruct {
	enum MenuNo MenuNo;
	UIFunc function;
	enum MenuNo Enter[TotalEnterIndexes];
	enum MenuNo ESC;
	enum MenuNo DN;
	enum MenuNo UP;
	enum MenuNo LFT;
	enum MenuNo RGT;
};

UIStruct MenuArray[] = {
	{ MenuNo::Power, UI::power, { MenuNo::Settings },MenuNo::Power, MenuNo::Power, MenuNo::Power, MenuNo::fuel_temp_humd,MenuNo::network },
	{ MenuNo::network, UI::network, { MenuNo::Settings },MenuNo::network, MenuNo::network, MenuNo::network, MenuNo::Power,MenuNo::source },
	{ MenuNo::source, UI::source, { MenuNo::Settings },MenuNo::source, MenuNo::source, MenuNo::source, MenuNo::network,MenuNo::Battery },
	{ MenuNo::Battery, UI::Battery, { MenuNo::Settings },MenuNo::Battery, MenuNo::Battery, MenuNo::Battery, MenuNo::source,MenuNo::Alarms },
	{ MenuNo::Alarms, UI::Alarms, { MenuNo::Settings },MenuNo::Alarms, MenuNo::Alarms, MenuNo::Alarms, MenuNo::Battery,MenuNo::fuel_temp_humd },
	{ MenuNo::fuel_temp_humd, UI::fuel_temp_humd,{ MenuNo::Settings }, MenuNo::fuel_temp_humd, MenuNo::fuel_temp_humd,MenuNo::fuel_temp_humd, MenuNo::Alarms, MenuNo::Power },
	{ MenuNo::Settings, UI::Settings, {  MenuNo::SetTimeDate,MenuNo::SetFuelMeas, MenuNo::SetSoCnDCurr },MenuNo::Settings, MenuNo::Settings, MenuNo::Settings, MenuNo::Power,MenuNo::Power },

	/*Sub menus*/
	{ MenuNo::SetTimeDate, UI::UI_Subs::SetTimeDate,{MenuNo::Settings},MenuNo::Settings,MenuNo::SetTimeDate,MenuNo::SetTimeDate, MenuNo::Settings,MenuNo::SetTimeDate },
	{ MenuNo::SetFuelMeas, UI::UI_Subs::SetFuelMeas,{MenuNo::SetFuelMeas},MenuNo::Settings,MenuNo::SetFuelMeas,MenuNo::SetFuelMeas, MenuNo::Settings,MenuNo::SetFuelMeas },
	{ MenuNo::SetSoCnDCurr, UI::UI_Subs::SetSoCnDCurr,{MenuNo::SetSoCnDCurr},MenuNo::Settings,MenuNo::SetSoCnDCurr,MenuNo::SetSoCnDCurr, MenuNo::Settings,MenuNo::SetSoCnDCurr },



};


void Settings(u8g2_t *u8g2)
{
	enum button::btncodes btncodes;
	int8_t sel_sub = 0;
do {

	button::resetCode(button::btncodes::cNONE);
	do {
		u8g2_ClearBuffer(u8g2);

		u8g2_DrawXBM(u8g2, 104, 0, imgcont::Settings.w, imgcont::Settings.h,imgcont::Settings.img);

		u8g2_SetFontMode(u8g2, 1);
		u8g2_SetDrawColor(u8g2, 2);
		u8g2_SetFont(u8g2, u8g2_font_5x8_mf);

		uint8_t strSize = std::strlen(UI_txts::Settings[sel_sub]);


		u8g2_DrawBox(u8g2, 2, 3 + (8*sel_sub) , (strSize * 5) + 1 , 9);




		for(int i=0; i < TotalSettingsSub;i++)
		{
			u8g2_DrawStr(u8g2, 3, 10 + (8*i), UI_txts::Settings[i]);
		}








		u8g2_SendBuffer(u8g2);

		btncodes = button::get_event();
	}while (btncodes == button::btncodes::cNONE);

	if(btncodes == button::btncodes::cDWN_BT)
	{
		sel_sub++;
	}
	else if(btncodes == button::btncodes::cUP_BT)
	{
		sel_sub--;
        if(sel_sub < 0)
        {
        	sel_sub = TotalSettingsSub - 1;
        }
	}
	else if(btncodes == button::btncodes::cEnter_BT)
	{
		UI::UI_helper::set_UIcode(SettingsMenuCodes[sel_sub].code);
	}


	sel_sub = sel_sub % TotalSettingsSub;

}while (btncodes != button::btncodes::cRGHT_BT && btncodes != button::btncodes::cLFT_BT &&  btncodes != button::btncodes::cEnter_BT);

}

void fuel_temp_humd(u8g2_t *u8g2) {
	enum button::btncodes btncodes;
	button::resetCode(button::btncodes::cNONE);
	uint8_t scroll = 0;

	char buffer[3][10];
	do {
		u8g2_ClearBuffer(u8g2);
		UI::UI_helper::common_iconsMain(u8g2);

		u8g2_DrawXBM(u8g2, 48, 2, imgcont::SensorInfo.w, imgcont::SensorInfo.h,
				imgcont::SensorInfo.img);

		liquidSensor.getLevel(&FuelPercent);
		AHT20.measure(&temp, &humid);

		u8g2_SetFontMode(u8g2, 1);
		u8g2_SetDrawColor(u8g2, 2);
		u8g2_SetFont(u8g2, u8g2_font_5x8_mf);

		u8g2_DrawBox(u8g2, 2, 17, 26, 9);
		u8g2_DrawStr(u8g2, 3, 25, "Fuel:");
		std::sprintf(buffer[0],"%d%%",FuelPercent);
		u8g2_DrawStr(u8g2, 5, 35, buffer[0]);

		u8g2_DrawBox(u8g2, 2, 40, 26, 9);
		u8g2_DrawStr(u8g2, 3, 48, "Temp:");
		std::sprintf(buffer[1],"%ldC",temp);
		u8g2_DrawStr(u8g2, 5, 58, buffer[1]);

		u8g2_DrawBox(u8g2, 86, 17, 31, 9);
		u8g2_DrawStr(u8g2, 88, 25, "Humid:");
		std::sprintf(buffer[2],"%ld%%",humid);
		u8g2_DrawStr(u8g2, 88, 35, buffer[2]);

		u8g2_SendBuffer(u8g2);

		btncodes = button::get_eventTimed(1000);
		if(btncodes == button::btncodes::cNONE )
		  {
#ifdef SCROLLING
			scroll++;
#endif
		  }
		else
		{
			scroll = 0;
		}


	} while (btncodes == button::btncodes::cNONE && scroll < scrollTiming);

	if(btncodes == button::btncodes::cEnter_BT)
	{
		UI::UI_helper::set_UIcode(MenuNo::Settings);
	}

	if(scroll >= scrollTiming)
	{


		button::resetCode( button::btncodes::cRGHT_BT);
	}


}
void Alarms(u8g2_t *u8g2) {
	enum button::btncodes btncodes;
	button::resetCode(button::btncodes::cNONE);
	uint8_t scroll = 0;
	do {

		u8g2_ClearBuffer(u8g2);
		UI::UI_helper::common_iconsMain(u8g2);
		u8g2_DrawXBM(u8g2, 48, 2, imgcont::Alarms.w, imgcont::Alarms.h,
				imgcont::Alarms.img);

		u8g2_SetFontMode(u8g2, 1);
		u8g2_SetDrawColor(u8g2, 2);
		u8g2_SetFont(u8g2, u8g2_font_5x8_mf);

		u8g2_DrawBox(u8g2, 2, 17, 51, 9);
		u8g2_DrawStr(u8g2, 3, 25, "Frequency:");
		u8g2_DrawStr(u8g2, 5, 35, "Normal"); //Over // under

		u8g2_DrawBox(u8g2, 2, 40, 46, 9);
		u8g2_DrawStr(u8g2, 3, 48, "Overload:");
		u8g2_DrawStr(u8g2, 5, 58, "Normal"); //Over under

		u8g2_DrawBox(u8g2, 81, 17, 51, 9);
		u8g2_DrawStr(u8g2, 82, 25, "Phaseload:");
		u8g2_DrawStr(u8g2, 82, 35, "Normal"); //Over under

		u8g2_SendBuffer(u8g2);
		btncodes = button::get_eventTimed(1000);
		if(btncodes == button::btncodes::cNONE )
		  {
#ifdef SCROLLING
			scroll++;
#endif
		  }
		else
		{
			scroll = 0;
		}


	} while (btncodes == button::btncodes::cNONE && scroll < scrollTiming);

	if(btncodes == button::btncodes::cEnter_BT)
	{
		UI::UI_helper::set_UIcode(MenuNo::Settings);
	}

	if(scroll >= scrollTiming)
	{


		button::resetCode( button::btncodes::cRGHT_BT);
	}

}
void Battery(u8g2_t *u8g2) {
	enum button::btncodes btncodes;
	button::resetCode(button::btncodes::cNONE);
	uint8_t scroll = 0;
	do {
		float soc;
		float curr;

		char StringStartTime[20];
		char StringEndTime[20];

		RTC_TimeTypeDef DTimeCharging_;
		RTC_TimeTypeDef DTimeDischarging_;

		getChargeTimestamp(&DTimeCharging_);
		getDischargeTimestamp(&DTimeDischarging_);

		std::sprintf(StringStartTime, "%02d:%02d:%02d", DTimeCharging_.Hours, DTimeCharging_.Minutes,DTimeCharging_.Seconds);

		std::sprintf(StringEndTime, "%02d:%02d:%02d", DTimeDischarging_.Hours, DTimeDischarging_.Minutes,DTimeDischarging_.Seconds);

		curr = DCCurrentSensor.getCurrent();




		u8g2_ClearBuffer(u8g2);

		soc = SOC::getSoCVal();

		char currentA[10];

		std::sprintf(currentA,"%.01f%%/%.02f",soc,curr);


		UI::UI_helper::common_iconsMain(u8g2);

		u8g2_DrawXBM(u8g2, 48, 2, imgcont::Battery.w, imgcont::Battery.h,
				imgcont::Battery.img);

		u8g2_SetFontMode(u8g2, 1);
		u8g2_SetDrawColor(u8g2, 2);
		u8g2_SetFont(u8g2, u8g2_font_5x8_mf);

		u8g2_DrawBox(u8g2, 2, 17, 41, 9);
		u8g2_DrawStr(u8g2, 3, 25, "Percent:");
		u8g2_DrawStr(u8g2, 1, 35, currentA);

		u8g2_DrawBox(u8g2, 2, 40, 46, 9);
		u8g2_DrawStr(u8g2, 3, 48, "ChrgTime:");
		u8g2_DrawStr(u8g2, 5, 58, StringStartTime);

		u8g2_DrawBox(u8g2, 81, 17, 51, 9);
		u8g2_DrawStr(u8g2, 82, 25, "DchrgTime:");
		u8g2_DrawStr(u8g2, 82, 35, StringEndTime);

		u8g2_SendBuffer(u8g2);
		btncodes = button::get_eventTimed(1000);

		if(btncodes == button::btncodes::cNONE )
		  {
#ifdef SCROLLING
			scroll++;
#endif
		  }
		else
		{
			scroll = 0;
		}


	} while (btncodes == button::btncodes::cNONE && scroll < scrollTiming);

	if(btncodes == button::btncodes::cEnter_BT)
	{
		UI::UI_helper::set_UIcode(MenuNo::Settings);
	}

	if(scroll >= scrollTiming)
	{


		button::resetCode( button::btncodes::cRGHT_BT);
	}

}
void source(u8g2_t *u8g2) {
	enum button::btncodes btncodes;
	button::resetCode(button::btncodes::cNONE);
	uint8_t scroll = 0;
	do {

		uint8_t source;
		u8g2_ClearBuffer(u8g2);
		UI::UI_helper::common_iconsMain(u8g2);
		u8g2_DrawXBM(u8g2, 48, 2, imgcont::Source.w, imgcont::Source.h,
				imgcont::Source.img);

		source = getSourceState();
		char selec_Source[3][20];
		if(source == 0)
		{
		    std::strcpy(selec_Source[0], "Active");
		    std::strcpy(selec_Source[1], "Idle");
		    std::strcpy(selec_Source[2], "Standby");
		}
		else if(source == 1)
		{
		    std::strcpy(selec_Source[0], "Charging");
		    std::strcpy(selec_Source[1], "Active");
		    std::strcpy(selec_Source[2], "Standby");
		}
		else if(source == 2)
		{
		    std::strcpy(selec_Source[0], "Charging");
		    std::strcpy(selec_Source[1], "Idle");
		    std::strcpy(selec_Source[2], "Active");
		}

		u8g2_SetFontMode(u8g2, 1);
		u8g2_SetDrawColor(u8g2, 2);
		u8g2_SetFont(u8g2, u8g2_font_5x8_mf);

		u8g2_DrawBox(u8g2, 2, 17, 41, 9);
		u8g2_DrawStr(u8g2, 3, 25, "Battery:");
		u8g2_DrawStr(u8g2, 2, 35, selec_Source[0]);  //Off

		u8g2_DrawBox(u8g2, 2, 40, 51, 9);
		u8g2_DrawStr(u8g2, 3, 48, "Generator:");
		u8g2_DrawStr(u8g2, 5, 58, selec_Source[1]); //running

		u8g2_DrawBox(u8g2, 84, 17, 31, 9);
		u8g2_DrawStr(u8g2, 85, 25, "Solar:");
		u8g2_DrawStr(u8g2, 82, 35,selec_Source[2]); //Online

		u8g2_SendBuffer(u8g2);
		btncodes = button::get_eventTimed(1000);
		if(btncodes == button::btncodes::cNONE )
		  {
#ifdef SCROLLING
			scroll++;
#endif
		  }
		else
		{
			scroll = 0;
		}


	} while (btncodes == button::btncodes::cNONE && scroll < scrollTiming);

	if(btncodes == button::btncodes::cEnter_BT)
	{
		UI::UI_helper::set_UIcode(MenuNo::Settings);
	}

	if(scroll >= scrollTiming)
	{


		button::resetCode( button::btncodes::cRGHT_BT);
	}

}
void network(u8g2_t *u8g2) {
	enum button::btncodes btncodes;
	button::resetCode(button::btncodes::cNONE);
	uint8_t scroll = 0;
	do {

		u8g2_ClearBuffer(u8g2);
		UI::UI_helper::common_iconsMain(u8g2);

		u8g2_DrawXBM(u8g2, 48, 2, imgcont::Network.w, imgcont::Network.h,
				imgcont::Network.img);

		char buf[3][20] = {
				" ",
				" ",
				" "
		};
	    std::strcpy(buf[0], getModemNetwork());
		std::strcpy(buf[1], getSignalQuality());
	    std::strcpy(buf[2], getModemData());

	    u8g2_SetFontMode(u8g2, 1);
		u8g2_SetDrawColor(u8g2, 2);
		u8g2_SetFont(u8g2, u8g2_font_5x8_mf);

		u8g2_DrawBox(u8g2, 2, 17, 41, 9);
		u8g2_DrawStr(u8g2, 3, 25, "Network:");
		u8g2_DrawStr(u8g2, 2, 35, buf[0]);

		u8g2_DrawBox(u8g2, 2, 40, 46, 9);
		u8g2_DrawStr(u8g2, 3, 48, "Signal-Q:");
		u8g2_DrawStr(u8g2, 5, 58, buf[1]);

		u8g2_DrawBox(u8g2, 84, 17, 26, 9);
		u8g2_DrawStr(u8g2, 85, 25, "Data:");
		u8g2_DrawStr(u8g2, 83, 35, buf[2]);

		u8g2_SendBuffer(u8g2);
		btncodes = button::get_eventTimed(1000);
		if(btncodes == button::btncodes::cNONE )
		  {
#ifdef SCROLLING
			scroll++;
#endif
		  }
		else
		{
			scroll = 0;
		}

	} while (btncodes == button::btncodes::cNONE && scroll < scrollTiming);

	if(btncodes == button::btncodes::cEnter_BT)
	{
		UI::UI_helper::set_UIcode(MenuNo::Settings);
	}

	if(scroll >= scrollTiming)
	{


		button::resetCode( button::btncodes::cRGHT_BT);
	}
}

void power(u8g2_t *u8g2) {

	enum button::btncodes btncodes;
	button::resetCode(button::btncodes::cNONE);
	uint8_t scroll = 0;


	float V1=0;
	float I1=0;
	float P1=0;

	float V2=0;
	float I2=0;
	float P2=0;

	float V3=0;
	float I3=0;
	float P3=0;

	do {

		u8g2_ClearBuffer(u8g2);


		getACData1(&V1,&I1,&P1);
		getACData2(&V2,&I2,&P2);
		getACData3(&V3,&I3,&P3);

        char V_1[10];
        char I_1[10];
        char V_2[10];
        char I_2[10];
        char V_3[10];
        char I_3[10];

        std::sprintf(V_1,"%.1f",V1);
        std::sprintf(I_1,"%.1f",I1);
        std::sprintf(V_2,"%.1f",V2);
        std::sprintf(I_2,"%.1f",I2);
        std::sprintf(V_3,"%.1f",V3);
        std::sprintf(I_3,"%.1f",I3);


		UI::UI_helper::common_iconsMain(u8g2);
		u8g2_SetFont(u8g2, u8g2_font_5x8_mf);

		u8g2_DrawXBM(u8g2, 10, 16, imgcont::Phase.w, imgcont::Phase.h,
				imgcont::Phase.img);
		u8g2_DrawStr(u8g2, 5, 38, V_1);
		u8g2_DrawStr(u8g2, 5, 46, I_1);

		u8g2_DrawXBM(u8g2, 54, 10, imgcont::Phase.w, imgcont::Phase.h,
				imgcont::Phase.img);
		u8g2_DrawStr(u8g2, 50, 32, V_2);
		u8g2_DrawStr(u8g2, 50, 40, I_2);

		u8g2_DrawXBM(u8g2, 100, 16, imgcont::Phase.w, imgcont::Phase.h,
				imgcont::Phase.img);
		u8g2_DrawStr(u8g2, 96, 38, V_3);
		u8g2_DrawStr(u8g2, 96, 46, I_3);

		u8g2_SendBuffer(u8g2);

		btncodes = button::get_eventTimed(1000);
		if(btncodes == button::btncodes::cNONE )
		  {
#ifdef SCROLLING
			scroll++;
#endif
		  }
		else
		{
			scroll = 0;
		}

	} while (btncodes == button::btncodes::cNONE && scroll < scrollTiming);

	if(btncodes == button::btncodes::cEnter_BT )
	{
		UI::UI_helper::set_UIcode(MenuNo::Settings);
	}

	if(scroll >= scrollTiming)
	{


		button::resetCode( button::btncodes::cRGHT_BT);
	}


}

void loop(u8g2_t *u8g2) {
	uint16_t index;
	if (CurrMenu == MenuNo::None) {
		CurrMenu =   MenuNo::Power;//MenuNo::Battery;
	}

	for (index = 0; index <= (uint16_t)MenuNo::TotalMenus; index++) {
		if (CurrMenu == MenuArray[index].MenuNo) // check if this menu is found ?
				{
			break;
		}
	}

	if (index < (uint16_t)MenuNo::TotalMenus) {
		std::memset(UI::display_buffer, 0, sizeof(UI::display_buffer));
		MenuArray[index].function(u8g2);

		/*Add buttons related logic*/

		switch (button::get_code()) {
		case button::btncodes::cEnter_BT: {
			int i = 0;
			for (i = 0; i < TotalEnterIndexes; i++) {

				if (MenuArray[index].Enter[i] == MenuNo::None) {
					CurrMenu = MenuArray[index].MenuNo; //stay in our own menu
					break;
				}

				if (MenuArray[index].Enter[i] == UI::UI_helper::get_UIcode())
				{
					CurrMenu = MenuArray[index].Enter[i];
					break;
				}

			}

			break;
		}
		case button::btncodes::cUP_BT: {
			CurrMenu = MenuArray[index].UP;
			break;
		}
		case button::btncodes::cDWN_BT: {
			CurrMenu = MenuArray[index].DN;
			break;
		}
		case button::btncodes::cRGHT_BT: {
			CurrMenu = MenuArray[index].RGT;
			break;
		}
		case button::btncodes::cLFT_BT: {
			CurrMenu = MenuArray[index].LFT;
			break;
		}

		case button::btncodes::cNONE:
		default:
			break;

		}

	} else {
		CurrMenu = MenuNo::None;
	}

}

}

