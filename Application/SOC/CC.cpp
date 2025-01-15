/*
 * CC.cpp
 *
 *  Created on: Jan 11, 2025
 *      Author: user
 */




#include <SOC.hpp>
#include "math.h"
#include "Init.hpp"

namespace SOC {

static float g_SoC = 0;
static float sampleIntervalSec = 0;
static float sampleIntervalHr = 0;
static uint32_t previousTime = 0;  // global or static variable



static const float REST_CURRENT_THRESHOLD_A = 1.0f;
static const float REST_CURRENT_THRESHOLD_B = 2.0f;
// Time (ms) to remain under threshold before we consider the battery "at rest"
static const uint32_t REST_TIME_REQUIRED_MS = 20000; // 20 seconds to be at safe side


static bool   g_isStable   = false;
static float  g_lastCurrentA = 0.0f;
static uint32_t g_stableStartMs = 0;

// -------- Voltage->SoC Table (for 48V LiFePO4) --------
static float voltagePoints[] = {
    40.0f, // 0%
    48.0f, // 10%
    51.2f, // 20%
    51.5f, // 30%
    52.0f, // 40%
    52.2f, // 50%
    52.3f, // 60%
    52.8f, // 70%
    53.1f, // 80%
    53.6f, // 90%
    54.4f  // 100% (rest)
};
static float socPoints[] = {
     0.0f,  // matches 40.0V
    10.0f,  // matches 48.0V
    20.0f,  // matches 51.2V
    30.0f,  // matches 51.5V
    40.0f,  // matches 52.0V
    50.0f,  // matches 52.2V
    60.0f,  // matches 52.3V
    70.0f,  // matches 52.8V
    80.0f,  // matches 53.1V
    90.0f,  // matches 53.6V
    100.0f  // matches 54.4V
};
#define NUM_POINTS (sizeof(voltagePoints)/sizeof(voltagePoints[0]))




uint8_t getSoCVal()
{
	return g_SoC;
}



static float SoCFromVoltage_48V(float measuredVoltage)
{
    // 1) Clamp voltage to [min, max]
    if (measuredVoltage <= voltagePoints[0]) {
        return 0.0f;
    }
    if (measuredVoltage >= voltagePoints[NUM_POINTS - 1]) {
        return 100.0f;
    }

    // 2) Find segment and do linear interpolation
    for (int i = 0; i < NUM_POINTS - 1; i++)
    {
        float vLow  = voltagePoints[i];
        float vHigh = voltagePoints[i + 1];

        if (measuredVoltage >= vLow && measuredVoltage < vHigh)
        {
            float socLow  = socPoints[i];
            float socHigh = socPoints[i + 1];

            float frac = (measuredVoltage - vLow) / (vHigh - vLow);
            float soc  = socLow + frac * (socHigh - socLow);
            return soc;
        }
    }

    // Should not reach here if the table is consistent
    return 0.0f;
}

void CheckCurrentStability(float currentA, uint32_t nowMs)
{
	float SoCBattVoltage;
    // Compare current to last iteration
    float deltaI = fabsf(currentA - g_lastCurrentA);

    if (deltaI >= REST_CURRENT_THRESHOLD_A && deltaI < REST_CURRENT_THRESHOLD_B )
    {
        // If we just entered "stable" zone, record the time
        if (!g_isStable)
        {
            g_isStable     = true;
            g_stableStartMs = nowMs;
        }
        else
        {
            // Already stable, check how long
            uint32_t stableDuration = nowMs - g_stableStartMs;
            if (stableDuration > REST_TIME_REQUIRED_MS)
            {
                // [E] We have been resting for enough time => do voltage correction
	                 DCVoltageMeasurement.getVoltage(&SoCBattVoltage);
	                 float socFromVolt = SoCFromVoltage_48V(SoCBattVoltage);

	                 // Weighted blend to avoid abrupt jumps
	                 float alpha = 0.95f; // 95% coulomb counting, 5% voltage correction
	                 float correctedSoC = alpha * g_SoC + (1.0f - alpha) * socFromVolt;

	                 // Apply correction
	                 g_SoC = correctedSoC;
            }
        }
    }
    else
    {
        // Current changed significantly => not stable
        g_isStable = false;
    }

    g_lastCurrentA = currentA;
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

	     if(BattCurrent > -2.0f)
	     {
	     CheckCurrentStability(BattCurrent,currentTime);
	     }




	     *SoC = g_SoC;

}

void CC_Init(float BattSoC,float IntervalSec)
{
	g_SoC = BattSoC;
	sampleIntervalSec = IntervalSec;

	sampleIntervalHr  = sampleIntervalSec / 3600.0f;
}


}
