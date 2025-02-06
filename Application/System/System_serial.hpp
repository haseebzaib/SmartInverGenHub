/*
 * System_serial.hpp
 *
 *  Created on: Nov 1, 2024
 *      Author: user
 */

#ifndef SYSTEM_SYSTEM_SERIAL_HPP_
#define SYSTEM_SYSTEM_SERIAL_HPP_

#include "main.h"
#include "functional"

#define numberofuart 3


    using ModemRxCB= void(*)(UART_HandleTypeDef *,uint8_t);
    using InverterRxCB=void(*)(UART_HandleTypeDef *,uint8_t);


    using ModemTxCB= void(*)(UART_HandleTypeDef *);
    using InverterTxCB=void(*)(UART_HandleTypeDef *);



   void setModemCallback(ModemRxCB Rxcb,ModemTxCB TxCB);
   void setInverterCallback(InverterRxCB Rxcb,InverterTxCB TxCB);


HAL_StatusTypeDef My_UART_Receive_Endless (UART_HandleTypeDef *huart);
void My_UART_Transmit(UART_HandleTypeDef *huart, uint8_t iZei);
void My_UART_IRQHandler(UART_HandleTypeDef *huart);





namespace System_serial {

class serial {
public:
	serial(UART_HandleTypeDef *huart);
	uint8_t ReceiveData(uint8_t *data,uint16_t size,uint32_t timeout);
    void TransmitData(const uint8_t *data,uint16_t size,uint32_t timeout = 1000);
    void RxintEn(uint8_t *buf,uint16_t size);
    void TxintEn(uint8_t *buf,uint16_t size);
    void RxintDis();
    void TxintDis();


private:
      UART_HandleTypeDef *Internal_UartHandler;




};


}


#endif /* SYSTEM_SYSTEM_SERIAL_HPP_ */
