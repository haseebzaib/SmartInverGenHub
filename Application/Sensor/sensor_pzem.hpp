/*
 * sensor_pzem.hpp
 *
 *  Created on: Nov 22, 2024
 *      Author: user
 */

#ifndef SENSOR_SENSOR_PZEM_HPP_
#define SENSOR_SENSOR_PZEM_HPP_


#include "main.h"
#include "System_serial.hpp"
#include "System_Rtos.hpp"

namespace sensor_pzem
{

class PZEM_004T {
public:

	typedef struct {
		float voltage;
		float current;
		float power;
		float energy;
		float frequency;
		float pf;
		uint16_t alarms;
	} PZEM;



	PZEM_004T(UART_HandleTypeDef *huart);
	void init();
	void read(PZEM *pzemhandle);



private:
	UART_HandleTypeDef *huart_;
	System_serial::serial serial_;

	static constexpr uint8_t buf[8] = {0xF8, 0x04, 0x00, 0x00,0x00,0x0A, 0x64, 0x64};
	static constexpr uint8_t rst_buf[4] = {0xF8, 0x42, 0xC2, 0x41};
	uint8_t res_buf[25];
	System_Rtos::freertos_semaphore PzemSemaphore;



};


}



#endif /* SENSOR_SENSOR_PZEM_HPP_ */
