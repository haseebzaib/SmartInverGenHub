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

enum class MenuNo {
	Power,
	network,
	source,
	Battery,
	Alarms,
	fuel_temp_humd,
    None,



};


namespace UI {

extern char display_buffer[255];


/*Helper Functions*/
void common_iconsMain(u8g2_t *u8g2);




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
