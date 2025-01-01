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

namespace UI {

char display_buffer[255] = { 0 };
enum MenuNo CurrMenu = MenuNo::None;

using UIFunc = void(*)(u8g2_t *u8g2);

struct UIStruct {
	enum MenuNo MenuNo;
	UIFunc function;
	enum MenuNo Enter[10];
	enum MenuNo ESC;
	enum MenuNo DN;
	enum MenuNo UP;
	enum MenuNo LFT;
	enum MenuNo RGT;
};

UIStruct MenuArray[] = {
{ MenuNo::Power, UI::power, { MenuNo::None },MenuNo::Power, MenuNo::Power, MenuNo::Power, MenuNo::fuel_temp_humd, MenuNo::network },
{ MenuNo::network, UI::network, { MenuNo::None },MenuNo::network, MenuNo::network, MenuNo::network, MenuNo::Power, MenuNo::source },
{ MenuNo::source, UI::source, { MenuNo::None },MenuNo::source, MenuNo::source, MenuNo::source, MenuNo::network, MenuNo::Battery },
{ MenuNo::Battery, UI::Battery, { MenuNo::None },MenuNo::Battery, MenuNo::Battery, MenuNo::Battery, MenuNo::source, MenuNo::Alarms },
{ MenuNo::Alarms, UI::Alarms, { MenuNo::None },MenuNo::Alarms, MenuNo::Alarms, MenuNo::Alarms, MenuNo::Battery, MenuNo::fuel_temp_humd },
{ MenuNo::fuel_temp_humd, UI::fuel_temp_humd, { MenuNo::None },MenuNo::fuel_temp_humd, MenuNo::fuel_temp_humd, MenuNo::fuel_temp_humd, MenuNo::Alarms, MenuNo::Power },


};






void fuel_temp_humd(u8g2_t *u8g2)
{
	do {
		 u8g2_ClearBuffer(u8g2);
		 UI::common_iconsMain(u8g2);

		 u8g2_DrawXBM(u8g2,48,2,imgcont::SensorInfo.w,imgcont::SensorInfo.h,imgcont::SensorInfo.img);

		 u8g2_SetFontMode(u8g2, 1);
		 u8g2_SetDrawColor(u8g2, 2);
		 u8g2_SetFont(u8g2, u8g2_font_5x8_mf);

		 u8g2_DrawBox(u8g2, 2, 17, 26, 9);
		 u8g2_DrawStr(u8g2, 3,25 , "Fuel:");
		 u8g2_DrawStr(u8g2, 5,35 , "90%");


		 u8g2_DrawBox(u8g2, 2, 40, 26, 9);
		 u8g2_DrawStr(u8g2, 3,48 , "Temp:");
		 u8g2_DrawStr(u8g2, 5,58 , "90C");


		 u8g2_DrawBox(u8g2, 86, 17, 31, 9);
		 u8g2_DrawStr(u8g2, 88,25 , "Humid:");
		 u8g2_DrawStr(u8g2, 88,35 , "50%");

		 u8g2_SendBuffer(u8g2);
		 System_Rtos::delay(1000);

	}while(1);

}
void Alarms(u8g2_t *u8g2)
{
 do {

	 u8g2_ClearBuffer(u8g2);
	 UI::common_iconsMain(u8g2);
	 u8g2_DrawXBM(u8g2,48,2,imgcont::Alarms.w,imgcont::Alarms.h,imgcont::Alarms.img);

	 u8g2_SetFontMode(u8g2, 1);
	 u8g2_SetDrawColor(u8g2, 2);
	 u8g2_SetFont(u8g2, u8g2_font_5x8_mf);

	 u8g2_DrawBox(u8g2, 2, 17, 51, 9);
	 u8g2_DrawStr(u8g2, 3,25 , "Frequency:");
	 u8g2_DrawStr(u8g2, 15,35 , "Normal"); //Over // under


	 u8g2_DrawBox(u8g2, 2, 40, 46, 9);
	 u8g2_DrawStr(u8g2, 3,48 , "Overload:");
	 u8g2_DrawStr(u8g2, 5,58 , "Normal"); //Over under


	 u8g2_DrawBox(u8g2, 81, 17, 51, 9);
	 u8g2_DrawStr(u8g2, 82,25 , "Phaseload:");
	 u8g2_DrawStr(u8g2, 82,35 , "Normal"); //Over under


	 u8g2_SendBuffer(u8g2);
	 System_Rtos::delay(1000);
 }while(1);
}
void Battery(u8g2_t *u8g2)
{
 do {

	 u8g2_ClearBuffer(u8g2);
	 UI::common_iconsMain(u8g2);

	 u8g2_DrawXBM(u8g2,48,2,imgcont::Battery.w,imgcont::Battery.h,imgcont::Battery.img);

	 u8g2_SetFontMode(u8g2, 1);
	 u8g2_SetDrawColor(u8g2, 2);
	 u8g2_SetFont(u8g2, u8g2_font_5x8_mf);

	 u8g2_DrawBox(u8g2, 2, 17, 41, 9);
	 u8g2_DrawStr(u8g2, 3,25 , "Percent:");
	 u8g2_DrawStr(u8g2, 15,35 , "98%");


	 u8g2_DrawBox(u8g2, 2, 40, 46, 9);
	 u8g2_DrawStr(u8g2, 3,48 , "ChrgTime:");
	 u8g2_DrawStr(u8g2, 5,58 , "01:25:08");


	 u8g2_DrawBox(u8g2, 81, 17, 51, 9);
	 u8g2_DrawStr(u8g2, 82,25 , "DchrgTime:");
	 u8g2_DrawStr(u8g2, 82,35 , "02:25:08");

	 u8g2_SendBuffer(u8g2);
	 System_Rtos::delay(1000);

 }while(1);
}
void source(u8g2_t *u8g2)
{
	do {
		 u8g2_ClearBuffer(u8g2);
		 UI::common_iconsMain(u8g2);
		 u8g2_DrawXBM(u8g2,48,2,imgcont::Source.w,imgcont::Source.h,imgcont::Source.img);

		 u8g2_SetFontMode(u8g2, 1);
		 u8g2_SetDrawColor(u8g2, 2);
		 u8g2_SetFont(u8g2, u8g2_font_5x8_mf);

		 u8g2_DrawBox(u8g2, 2, 17, 41, 9);
		 u8g2_DrawStr(u8g2, 3,25 , "Battery:");
		 u8g2_DrawStr(u8g2, 2,35 , "Active");  //Off


		 u8g2_DrawBox(u8g2, 2, 40, 51, 9);
		 u8g2_DrawStr(u8g2, 3,48 , "Generator:");
		 u8g2_DrawStr(u8g2, 5,58 , "Idle"); //running


		 u8g2_DrawBox(u8g2, 84, 17, 31, 9);
		 u8g2_DrawStr(u8g2, 85,25 , "Solar:");
		 u8g2_DrawStr(u8g2, 82,35 , "Standby"); //Online

		 u8g2_SendBuffer(u8g2);
		 System_Rtos::delay(1000);
	}while(1);

}
void network(u8g2_t *u8g2)
{
  do{

	 u8g2_ClearBuffer(u8g2);
	 UI::common_iconsMain(u8g2);

	 u8g2_DrawXBM(u8g2,48,2,imgcont::Network.w,imgcont::Network.h,imgcont::Network.img);


	 u8g2_SetFontMode(u8g2, 1);
	 u8g2_SetDrawColor(u8g2, 2);
	 u8g2_SetFont(u8g2, u8g2_font_5x8_mf);

	 u8g2_DrawBox(u8g2, 2, 17, 41, 9);
	 u8g2_DrawStr(u8g2, 3,25 , "Network:");
	 u8g2_DrawStr(u8g2, 2,35 , "Connected");


	 u8g2_DrawBox(u8g2, 2, 40, 46, 9);
	 u8g2_DrawStr(u8g2, 3,48 , "Signal-Q:");
	 u8g2_DrawStr(u8g2, 5,58 , "-50dBm");


	 u8g2_DrawBox(u8g2, 84, 17, 26, 9);
	 u8g2_DrawStr(u8g2, 85,25 , "Data:");
	 u8g2_DrawStr(u8g2, 82,35 , "Disconn");



	 u8g2_SendBuffer(u8g2);
	 System_Rtos::delay(1000);

   }while(1);
}


void power(u8g2_t *u8g2) {


		do {

			u8g2_ClearBuffer(u8g2);


			UI::common_iconsMain(u8g2);
			u8g2_SetFont(u8g2, u8g2_font_5x8_mf);

			u8g2_DrawXBM(u8g2,10,16,imgcont::Phase.w,imgcont::Phase.h,imgcont::Phase.img);
			u8g2_DrawStr(u8g2, 5,38 , "220.0V");
			u8g2_DrawStr(u8g2, 5,46 , "020.6A");

			u8g2_DrawXBM(u8g2,54,10,imgcont::Phase.w,imgcont::Phase.h,imgcont::Phase.img);
			u8g2_DrawStr(u8g2, 50,32 , "220.0V");
			u8g2_DrawStr(u8g2, 50,40 , "020.6A");

            u8g2_DrawXBM(u8g2,100,16,imgcont::Phase.w,imgcont::Phase.h,imgcont::Phase.img);
            u8g2_DrawStr(u8g2,96,38 , "220.0V");
            u8g2_DrawStr(u8g2, 96,46 , "020.6A");


			u8g2_SendBuffer(u8g2);
			System_Rtos::delay(1000);
		} while (1);
}

void loop(u8g2_t *u8g2) {
	uint16_t index;
	if (CurrMenu == MenuNo::None) {
		CurrMenu = MenuNo::fuel_temp_humd;
	}

	for (index = 0; index < (sizeof(MenuArray) / sizeof(MenuArray[0]));
			index++) {
		if (CurrMenu == MenuArray[index].MenuNo) // check if this menu is found ?
				{
			break;
		}
	}

	if (index < (sizeof(MenuArray) / sizeof(MenuArray[0]))) {
		std::memset(UI::display_buffer, 0, sizeof(UI::display_buffer));
		MenuArray[index].function(u8g2);

		/*Add buttons related logic*/

	} else {
		CurrMenu = MenuNo::None;
	}

}

}

