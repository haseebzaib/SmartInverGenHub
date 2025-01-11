/*
 * CC.hpp
 *
 *  Created on: Jan 11, 2025
 *      Author: user
 */

#ifndef SOC_SOC_HPP_
#define SOC_SOC_HPP_


#include "main.h"

#define BATTERYCAPACITY 100.0f // 100Ah


namespace SOC {


uint8_t getSoCVal();
void CC_Loop(float *SoC,float BattCurrent, float BattVoltage);
void CC_Init(float BattSoC,float IntervalSec);


}

#endif /* SOC_SOC_HPP_ */
