/*
 * CC.cpp
 *
 *  Created on: Jan 11, 2025
 *      Author: user
 */




#include <SOC.hpp>

namespace SOC {

static float g_SoC = 0;
static float sampleIntervalSec = 0;
static float sampleIntervalHr = 0;
static uint32_t previousTime = 0;  // global or static variable






uint8_t getSoCVal()
{
	return g_SoC;
}





void CC_Loop(float *SoC,float BattCurrent, float BattVoltage)
{

    uint32_t currentTime = HAL_GetTick();
    uint32_t elapsedMs   = currentTime - previousTime;
    previousTime         = currentTime;

    float deltaT_hours = (float)elapsedMs / 3600000.0f; // 1000 ms/s, 3600 s/hr

	 // Calculate deltaSoC in %
	    float deltaSoC = (BattCurrent * deltaT_hours / BATTERYCAPACITY) * 100.0f;


	    // Update SoC
	    // Positive I => discharge => SoC goes DOWN => subtract deltaSoC
	     g_SoC = g_SoC - deltaSoC;



	     // Clamp SoC
	     if (g_SoC > 100.0f) {
	    	 g_SoC = 100.0f;
	     }
	     if (g_SoC < 0.0f)  {
	    	 g_SoC = 0.0f;
	     }

	     //Implement hybrid model for SoC calculations



	     *SoC = g_SoC;

}

void CC_Init(float BattSoC,float IntervalSec)
{
	g_SoC = BattSoC;
	sampleIntervalSec = IntervalSec;

	sampleIntervalHr  = sampleIntervalSec / 3600.0f;
}


}
