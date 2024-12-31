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

UIStruct MenuArray[] = { { MenuNo::Main, UI::main, { MenuNo::Main },
		MenuNo::Main, MenuNo::Main, MenuNo::Main, MenuNo::Main, MenuNo::Main } };

void main(u8g2_t *u8g2) {

	do {

		do {

			char time[50];
			RTC_DateTypeDef DDate;
			RTC_TimeTypeDef DTime;
			stmRTC.getTime(&DDate, &DTime, nullptr);
			std::sprintf(time, "%02d:%02d:%02d", DTime.Hours, DTime.Minutes,
					DTime.Seconds);

			u8g2_ClearBuffer(u8g2);

			u8g2_SetFont(u8g2, u8g2_font_6x10_mf);
			u8g2_DrawStr(u8g2, 0, 10, time);

			u8g2_SendBuffer(u8g2);

			System_Rtos::delay(1000);
		} while (1);

	} while (1);

}

void loop(u8g2_t *u8g2) {
	uint16_t index;
	if (CurrMenu == MenuNo::None) {
		CurrMenu = MenuNo::Main;
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

