/*
 * Display_HMI.hpp
 *
 *  Created on: Nov 1, 2024
 *      Author: user
 */

#ifndef DISPLAY_DISPLAY_HMI_HPP_
#define DISPLAY_DISPLAY_HMI_HPP_

#include "main.h"
#include "System/System_serial.hpp"

namespace HMI {

class DWIN {

public:
	enum status {
		DWIN_OK = 0, DWIN_ERR = 1,
	};

	enum headers {
		frame1 = 0x5A, frame2 = 0xA5, Send = 0x82, Recv = 0x83,

	};

	enum id {
		TankLevel = 0x2010,
		Time = 0x1010,
		Date = 0x1020,
		Network = 0x8110,
		SigQ = 0x8220,
		Net = 0x8330,
		Temperature = 0x4010,
		Humidity = 0x4020,
		Battery = 0x7010,
		phase1 = 0x3110,
		phase2 = 0x3220,
		phase3 = 0x3330,
		curr1 = 0x3440,
		curr2 = 0x3550,
		curr3 = 0x3660,
		energy = 0x3111,
		pwr1 = 0x3770,
		pwr2 = 0x3880,
		pwr3 = 0x3990,
		source_bat = 0x5110,
		source_gen = 0x5220,
		source_sol = 0x5330,
		alarm_freq = 0x6110,
		alarm_overl = 0x6220,
		alarm_phasel = 0x6330,
		chrgT = 0x7220,
		dchgT = 0x7330,
	};

    DWIN(UART_HandleTypeDef *huart);
    void Startup();
    enum status SendNumericData(uint16_t id,uint16_t val);
    enum status SendFloatData(uint16_t id,float val);
    enum status SendTextData(uint16_t id,char *data);
    enum status clearBox(uint16_t id);

private:
#define CheckError(func, returnStat, checkStat) \
  if (func != checkStat)          \
  {                            \
    return returnStat;               \
  }                                 \


	union MIX_UWORD {
		uint16_t W;
		struct {
			uint8_t Lb;
			uint8_t Hb;
		} B;
	};

	union MIX_UWORD convert_16bit_to_8bit;

	union BYTE_FLOAT {
	    float f;
	    uint8_t B[4];
	};
	union BYTE_FLOAT convert_float_to_bytes;


	System_serial::serial serial_;




	uint8_t stringTohex(char *input,uint8_t *output,uint8_t index,uint8_t len);





};

}

#endif /* DISPLAY_DISPLAY_HMI_HPP_ */
