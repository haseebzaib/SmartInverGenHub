/*
 * button.hpp
 *
 *  Created on: Jan 1, 2025
 *      Author: user
 */

#ifndef USER_INTERFACE_BUTTON_HPP_
#define USER_INTERFACE_BUTTON_HPP_


#include "main.h"


namespace button {

	enum class btncodes
	{
		//c denotes as an identifier for that button code
		cNONE=0,
		cEnter_BT=10,
		cUP_BT=30,
		cDWN_BT=40,
		cRGHT_BT=50,
		cLFT_BT=60,

	};

	 enum class button_identification {
		//i denotes as an identifier to that button
		iNONE = 0,
		iEnter_BT,
		iESC_BT,
		iUP_BT,
		iDWN_BT,
		iRGHT_BT,
		iLFT_BT,
		iCALB_BT,
		iNCALB_BT,
	};

	struct Button_t{
		uint32_t buttonTimer; //to debounce the button
		uint8_t buttonTimerEnable;//redundant
		uint8_t buttonFlag;//to know if button is pressed
		enum button_identification Sel_BT;//check correct button after debounce
		enum btncodes btn_code;//code that transfers to main program
	};



	void btn_init();
	void gen_event(enum btncodes btnCode);
	enum btncodes get_event();
	enum btncodes get_eventTimed(uint32_t time);
	enum btncodes get_code();
    void resetCode(enum btncodes btnCode);




    void btn_irq();


}



#endif /* USER_INTERFACE_BUTTON_HPP_ */
