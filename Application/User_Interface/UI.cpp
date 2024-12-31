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

}
void Alarms(u8g2_t *u8g2)
{

}
void Battery(u8g2_t *u8g2)
{

}
void source(u8g2_t *u8g2)
{

}
void network(u8g2_t *u8g2)
{

}


void power(u8g2_t *u8g2) {

	do {
		do {

			u8g2_ClearBuffer(u8g2);


			UI::common_iconsMain(u8g2);
			u8g2_SetFont(u8g2, u8g2_font_5x8_mf);

			u8g2_DrawXBM(u8g2,10,16,Phase.w,Phase.h,Phase.img);
			u8g2_DrawStr(u8g2, 5,38 , "220.0V");
			u8g2_DrawStr(u8g2, 5,46 , "020.6A");

			u8g2_DrawXBM(u8g2,54,10,Phase.w,Phase.h,Phase.img);
			u8g2_DrawStr(u8g2, 50,32 , "220.0V");
			u8g2_DrawStr(u8g2, 50,40 , "020.6A");

            u8g2_DrawXBM(u8g2,100,16,Phase.w,Phase.h,Phase.img);
            u8g2_DrawStr(u8g2,96,38 , "220.0V");
            u8g2_DrawStr(u8g2, 96,46 , "020.6A");


			u8g2_SendBuffer(u8g2);
			System_Rtos::delay(1000);
		} while (1);
	} while (1);

}

void loop(u8g2_t *u8g2) {
	uint16_t index;
	if (CurrMenu == MenuNo::None) {
		CurrMenu = MenuNo::Power;
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

