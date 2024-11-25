/*
 * InterfaceTask.cpp
 *
 *  Created on: Nov 8, 2024
 *      Author: user
 */

#include "app_main.hpp"
#include "Inverter/Inverter.hpp"
#include "System/System_Rtos.hpp"
#include "System/System_sys.hpp"
#include "Sensor/sensor_pzem.hpp"
#include "Init.hpp"
#include "cstdio"
#include "cstring"
#include "gpio.h"
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "cmath"

Inverter::Voltronic Voltronic(&INVERTER_U,DE_GPIO_Port,DE_Pin);
sensor_pzem::PZEM_004T PZEM1(&PZEM1_U);
sensor_pzem::PZEM_004T PZEM2(&PZEM2_U);
sensor_pzem::PZEM_004T PZEM3(&PZEM3_U);

char protoID[20];
char devID[20];
char ver[20];

#define NUM_CHANNELS 3
#define SAMPLES_PER_CHANNEL 500
#define BUFFER_SIZE (NUM_CHANNELS * SAMPLES_PER_CHANNEL)
uint16_t adcData[BUFFER_SIZE];

// Per-channel data arrays (optional)
uint16_t channel_1_data[SAMPLES_PER_CHANNEL];
uint16_t channel_2_data[SAMPLES_PER_CHANNEL];
uint16_t channel_3_data[SAMPLES_PER_CHANNEL];

__IO uint8_t flag = 0;

#define ADC_MAX 4095.0  // 12-bit ADC
#define V_REF 3.3     // ADC reference voltage
#define ZMPT_SCALE 1.0 // Adjust based on calibration

float calculate_rms(uint16_t *adc_buffer, uint16_t buffer_size, float vref, float adc_max_value) {
    float sum_square = 0.0;
    float dc_offset = vref / 2.0;  // DC offset (e.g., 1.65V for 3.3V Vref)

    for (uint16_t i = 0; i < buffer_size; i++) {
        // Convert ADC value to voltage
        float voltage = (adc_buffer[i] / adc_max_value) * vref;

        // Subtract DC offset to get the AC component
        float ac_component = voltage - dc_offset;

        // Accumulate squared values
        sum_square += ac_component * ac_component;
    }

    // Calculate mean of squared values
    float mean_square = sum_square / buffer_size;

    // Return the RMS value
    return sqrt(mean_square);
}


void InverterTask(void *pvParameters) {

	InverterData_Queue InverterData = {0};

	System_Rtos::delay(1000);

	PZEM1.init();
	PZEM2.init();
	PZEM3.init();

HAL_ADC_Start_DMA(&hadc2,(uint32_t *) adcData, BUFFER_SIZE);
HAL_TIM_Base_Start(&htim8);

	while (1) {

//		PZEM1.read(&InverterData.pzem1_data);
//		PZEM2.read(&InverterData.pzem2_data);
//		PZEM3.read(&InverterData.pzem3_data);
//
//		//Voltronic.ProtocolID_Inquiry(protoID);
//		//Voltronic.DeviceID_Inquiry(devID);
//		//Voltronic.CPUFW_Ver_Inquiry(ver);
		Voltronic.DeviceStat_Inquiry_qpigs(InverterData.inverterData_qpigs);
//		//Voltronic.DeviceStat_Inquiry_qipgs2(InverterData.inverterData_qpigs2);
//		//Voltronic.DeviceStat_Inquiry_qipgsN(InverterData.inverterData_qpigsN);
		Voltronic.DeviceStat_Inquiry_mode(&InverterData.device_mode);



		if(flag)
		{
			flag = 0;

			   // Perform RMS calculation
			 InverterData.rms_volt1 = calculate_rms(channel_1_data, SAMPLES_PER_CHANNEL,V_REF,ADC_MAX);
			 if( InverterData.rms_volt1 < 0.03)
					 {
						 InverterData.rms_volt1 = 0;
					 }
					 else
					 {
						 InverterData.rms_volt1 =  InverterData.rms_volt1*500.0;
					 }


			 InverterData.rms_volt2 = calculate_rms(channel_2_data, SAMPLES_PER_CHANNEL,V_REF,ADC_MAX);
			 if( InverterData.rms_volt2 < 0.03)
					 {
						 InverterData.rms_volt2 = 0;
					 }
					 else
					 {
						 InverterData.rms_volt2 =  InverterData.rms_volt2*500.0;
					 }

			 InverterData.rms_volt3 = calculate_rms(channel_3_data, SAMPLES_PER_CHANNEL,V_REF,ADC_MAX);
			 if( InverterData.rms_volt3 < 0.03)
			 {
				 InverterData.rms_volt3 = 0;
			 }
			 else
			 {
				 InverterData.rms_volt3 =  InverterData.rms_volt3*500.0;
			 }

			 HAL_ADC_Start_DMA(&hadc2,(uint32_t *) adcData, BUFFER_SIZE);

		     InverterDataQueue.queueSend(reinterpret_cast<void*>(&InverterData));

		}

		InverterDataQueue.queueSend(reinterpret_cast<void*>(&InverterData));

		System_Rtos::delay(100);


	}

}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	 if (hadc->Instance == ADC2)
	 {
	// Separate data for each channel
    for (int i = 0; i < SAMPLES_PER_CHANNEL; i++) {
        channel_1_data[i] = adcData[i * NUM_CHANNELS];     // Channel 1
        channel_2_data[i] = adcData[i * NUM_CHANNELS + 1]; // Channel 2
        channel_3_data[i] = adcData[i * NUM_CHANNELS + 2]; // Channel 3
    }

    flag = 1;
	 }
    // Process each channel's data (e.g., calculate RMS)
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
	 if (hadc->Instance == ADC2)
	 {
    // Same processing as full-complete but for the first half of the buffer
    for (int i = 0; i < SAMPLES_PER_CHANNEL / 2; i++) {
        channel_1_data[i] = adcData[i * NUM_CHANNELS];     // Channel 1
        channel_2_data[i] = adcData[i * NUM_CHANNELS + 1]; // Channel 2
        channel_3_data[i] = adcData[i * NUM_CHANNELS + 2]; // Channel 3
    }

	 }
}
