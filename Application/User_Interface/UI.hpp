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
	Main,
	Setting,
    None,



};


namespace UI {

extern char display_buffer[255];


void main(u8g2_t *u8g2);


void loop(u8g2_t *u8g2);

}


#endif /* USER_INTERFACE_UI_HPP_ */
