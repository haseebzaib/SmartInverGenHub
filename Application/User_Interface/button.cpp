/*
 * button.cpp
 *
 *  Created on: Jan 1, 2025
 *      Author: user
 */

#include "button.hpp"
#include "app_main.hpp"
#include "System_Rtos.hpp"

extern "C"
{
    void My_Button_IRQHandlerC()
    {
    	button::btn_irq();
    }
}

namespace button {

//p denotes show that these are pins
#define pEnter_BT  HAL_GPIO_ReadPin(BTN_ENTR_GPIO_Port, BTN_ENTR_Pin)
#define pUP_BT     HAL_GPIO_ReadPin(BTN_UP_GPIO_Port , BTN_UP_Pin )
#define pDWN_BT    HAL_GPIO_ReadPin(BTN_DWN_GPIO_Port , BTN_DWN_Pin )
#define pRGHT_BT   HAL_GPIO_ReadPin(BTN_RGT_GPIO_Port, BTN_RGT_Pin)
#define pLFT_BT    HAL_GPIO_ReadPin(BTN_LFT_GPIO_Port, BTN_LFT_Pin)


System_Rtos::freertos_events eventsRTOS;



struct Button_t Button_;


static void setTimer(uint32_t* timer)
{
	*timer = HAL_GetTick();
}

static uint8_t checkTimer(uint32_t* timer, uint32_t msTime)
{
	uint8_t ret = RESET;
	ret = ((HAL_GetTick() - *timer) > msTime)  ? ENABLE : DISABLE;
	return ret;
}


void btn_init()
{
	eventsRTOS.eventCreate();

	Button_.btn_code = button::btncodes::cNONE;
	Button_.buttonFlag = 1;
	Button_.buttonTimer = 0;
	Button_.Sel_BT =button_identification::iNONE;
}
void gen_event(enum btncodes btnCode)
{
	Button_.btn_code = btnCode;
	eventsRTOS.eventGive();
}
enum btncodes get_event()
{

	eventsRTOS.eventTake();
	return Button_.btn_code;

}
enum btncodes get_eventTimed(uint32_t time)
{
	eventsRTOS.eventTakeTimed(time);
	return Button_.btn_code;
}

enum btncodes get_code()
{
	return Button_.btn_code;
}

void resetCode(enum btncodes btnCode)
{
	Button_.btn_code = btnCode;
}


void btn_irq()
{
	if (Button_.buttonFlag) {

         if (!pEnter_BT) {
				setTimer(&Button_.buttonTimer); //to set debounce
				Button_.buttonFlag = RESET;
				Button_.Sel_BT = button_identification::iEnter_BT;
			}  else if (!pUP_BT) {
				setTimer(&Button_.buttonTimer); //to set debounce
				Button_.buttonFlag = RESET;
				Button_.Sel_BT = button_identification::iUP_BT;
			} else if (!pDWN_BT) {
				setTimer(&Button_.buttonTimer); //to set debounce
				Button_.buttonFlag = RESET;
				Button_.Sel_BT = button_identification::iDWN_BT;
			} else if (!pRGHT_BT) {
				setTimer(&Button_.buttonTimer); //to set debounce
				Button_.buttonFlag = RESET;
				Button_.Sel_BT = button_identification::iRGHT_BT;
			} else if (!pLFT_BT) {
				setTimer(&Button_.buttonTimer); //to set debounce
				Button_.buttonFlag = RESET;
				Button_.Sel_BT = button_identification::iLFT_BT;
			}

		} else if ((!Button_.buttonFlag)
				&& (checkTimer(&Button_.buttonTimer, 120))) //check debounce
			{
			switch (Button_.Sel_BT) {
			case button_identification::iEnter_BT: {
				if (!pEnter_BT) {
					Button_.buttonFlag = SET;
					Button_.Sel_BT = button_identification::iNONE;
					gen_event(btncodes::cEnter_BT);
				} else {
					Button_.buttonFlag = SET;
					Button_.Sel_BT = button_identification::iNONE;
				}


				break;
			}
			case button_identification::iUP_BT: {
				if (!pUP_BT) {
					Button_.buttonFlag = SET;
					Button_.Sel_BT = button_identification::iNONE;
					gen_event(btncodes::cUP_BT);
				} else {
					Button_.buttonFlag = SET;
					Button_.Sel_BT = button_identification::iNONE;
				}

				break;
			}
			case button_identification::iDWN_BT: {
				if (!pDWN_BT) {
					Button_.buttonFlag = SET;
					Button_.Sel_BT = button_identification::iNONE;
					gen_event(btncodes::cDWN_BT);
				} else {
					Button_.buttonFlag = SET;
					Button_.Sel_BT = button_identification::iNONE;
				}

				break;
			}
			case button_identification::iRGHT_BT: {
				if (!pRGHT_BT) {
					Button_.buttonFlag = SET;
					Button_.Sel_BT = button_identification::iNONE;
					gen_event(btncodes::cRGHT_BT);
				} else {
					Button_.buttonFlag = SET;
					Button_.Sel_BT = button_identification::iNONE;
				}

				break;
			}
			case button_identification::iLFT_BT: {
				if (!pLFT_BT) {
					Button_.buttonFlag = SET;
					Button_.Sel_BT = button_identification::iNONE;
					gen_event(btncodes::cLFT_BT);
				} else {
					Button_.buttonFlag = SET;
					Button_.Sel_BT = button_identification::iNONE;
				}

				break;
			}
			default: {
				Button_.buttonFlag = SET;
				Button_.Sel_BT = button_identification::iNONE;
				break;
			}
			}
		}
}

}
