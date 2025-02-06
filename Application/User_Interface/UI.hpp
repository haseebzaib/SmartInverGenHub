/*
 * UI.hpp
 *
 *  Created on: Dec 31, 2024
 *      Author: user
 */

#ifndef USER_INTERFACE_UI_HPP_
#define USER_INTERFACE_UI_HPP_


#include "main.h"
#include "u8g2.h"


#define TotalSettingsSub  3

enum class MenuNo {
	Power,
	network,
	source,
	Battery,
	Alarms,
	fuel_temp_humd,
	Settings,
	SetTimeDate,
	SetFuelMeas,
	SetSoCnDCurr,
    TotalMenus,
	None,
};

struct SettingMenuCodes {
	enum MenuNo code;
};

extern struct SettingMenuCodes SettingsMenuCodes[TotalSettingsSub];


namespace UI {

extern char display_buffer[255];


namespace UI_helper {
/*Helper Functions*/
void set_UIcode(enum MenuNo UIcode);
enum MenuNo get_UIcode();
void common_iconsMain(u8g2_t *u8g2);
void SubMenuControlInfo(u8g2_t *u8g2);


}


/*Settings*/
void Settings(u8g2_t *u8g2);


namespace UI_Subs {

void SetTimeDate(u8g2_t *u8g2);
void SetFuelMeas(u8g2_t *u8g2);
void SetSoCnDCurr(u8g2_t *u8g2);

}

/*Main functions*/
void fuel_temp_humd(u8g2_t *u8g2);
void Alarms(u8g2_t *u8g2);
void Battery(u8g2_t *u8g2);
void source(u8g2_t *u8g2);
void network(u8g2_t *u8g2);
void power(u8g2_t *u8g2);



/*loop*/
void loop(u8g2_t *u8g2);

}


#endif /* USER_INTERFACE_UI_HPP_ */
