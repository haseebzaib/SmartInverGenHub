/*
 * System_serial.cpp
 *
 *  Created on: Nov 1, 2024
 *      Author: user
 */

#include "System_serial.hpp"
#include "usart.h"
#include "app_main.hpp"

 DisplayRxCB LocalDisplayRxCB = nullptr ;
 ModemRxCB LocalModemRxCB = nullptr ;
 InverterRxCB LocalInverterRxCB= nullptr ;

 DisplayTxCB LocalDisplayTxCB = nullptr ;
 ModemTxCB LocalModemTxCB = nullptr ;
 InverterTxCB LocalInverterTxCB = nullptr ;

void setDisplayCallback(DisplayRxCB Rxcb, DisplayTxCB TxCB) {
	LocalDisplayRxCB = Rxcb;
	LocalDisplayTxCB = TxCB;
}
void setModemCallback(ModemRxCB Rxcb, ModemTxCB TxCB) {
	LocalModemRxCB = Rxcb;
	LocalModemTxCB = TxCB;
}
void setInverterCallback(InverterRxCB Rxcb, InverterTxCB TxCB) {
	LocalInverterRxCB = Rxcb;
	LocalInverterTxCB = TxCB;
}

HAL_StatusTypeDef My_UART_Receive_Endless(UART_HandleTypeDef *huart) {


	uint16_t uhErr = (uint16_t) huart->ErrorCode;
	uint16_t uhData;

	if (huart->RxState == HAL_UART_STATE_BUSY_RX) {

		uhData = ((uint16_t) READ_REG(huart->Instance->DR)) & 0x00FF;

		if (uhErr != HAL_UART_ERROR_NONE)
			uhData |= (uhErr << 8);


		if (huart == &HMI_UART5) {

			if(LocalDisplayRxCB)
			{
				LocalDisplayRxCB(huart,(uint8_t)uhData);
			}

		}

		if (huart == &GSM_USART2) {
			if(LocalModemRxCB)
			{
				LocalModemRxCB(huart,(uint8_t)uhData);
			}

		}

		if (huart == &INVERTER_USART3) {

			if(LocalInverterRxCB)
			{
				LocalInverterRxCB(huart,(uint8_t)uhData);
			}

		}

		return HAL_OK;
	} else {
		/* Clear RXNE interrupt flag */
		__HAL_UART_FLUSH_DRREGISTER(huart);

		return HAL_BUSY;
	}
}


void My_UART_Transmit(UART_HandleTypeDef *huart, uint8_t iZei) {

	  /* Check that a Tx process is ongoing */
	  if (huart->gState == HAL_UART_STATE_BUSY_TX)
	  {


	      huart->Instance->DR = iZei;
	      __HAL_UART_ENABLE_IT(huart, UART_IT_TXE);
	 }
}


void My_UART_IRQHandler(UART_HandleTypeDef *huart) {

	  uint32_t isrflags   = READ_REG(huart->Instance->SR);
	  uint32_t cr1its     = READ_REG(huart->Instance->CR1);
	  uint32_t cr3its     = READ_REG(huart->Instance->CR3);
	  uint32_t errorflags = 0x00U;

	  /* If no error occurs */
	  /* If no error occurs */
	  errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
	  if (errorflags == RESET)
	  {
	    /* UART in mode Receiver ---------------------------------------------------*/
	     if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
	    {

	      My_UART_Receive_Endless(huart);


	      return;
	    }
	  }

	  /* If some errors occur */
	  cr3its = READ_REG(huart->Instance->CR3);
	  if(   (errorflags != RESET)
	     && (   ((cr3its & USART_CR3_EIE) != RESET)
	         || ((cr1its & (USART_CR1_RXNEIE | USART_CR1_PEIE)) != RESET)) )
	  {
	    /* UART parity error interrupt occurred -------------------------------------*/
	    if(((isrflags & USART_SR_PE) != RESET) && ((cr1its & USART_CR1_PEIE) != RESET))
	    {

	      __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_PE);

	      huart->ErrorCode |= HAL_UART_ERROR_PE;



	    }

	    /* UART frame error interrupt occurred --------------------------------------*/
	    if(((isrflags & USART_SR_FE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
	    {
	    	__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_FE);

	      huart->ErrorCode |= HAL_UART_ERROR_FE;

	    }

	    /* UART noise error interrupt occurred --------------------------------------*/
	    if(((isrflags & USART_SR_NE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
	    {
	    	__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_NE);

	      huart->ErrorCode |= HAL_UART_ERROR_NE;
	    }

	    /* UART Over-Run interrupt occurred -----------------------------------------*/
	    if(((isrflags & USART_SR_ORE) != RESET) &&
	       (((cr1its & USART_CR1_RXNEIE) != RESET) || ((cr3its & USART_CR3_EIE) != RESET)))
	    {
	    	__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_ORE);

	      huart->ErrorCode |= HAL_UART_ERROR_ORE;

	    }

	    /* Call UART Error Call back function if need be --------------------------*/
	    if(huart->ErrorCode != HAL_UART_ERROR_NONE)
	    {
	      /* UART in mode Receiver ---------------------------------------------------*/
	      if(((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
	      {
	            // Erfassungs-SS weitermachen
	             huart->ErrorCode = HAL_UART_ERROR_NONE;   // loeschen
	            //__HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);  // Clear RXNE interrupt flag

	             __HAL_UART_FLUSH_DRREGISTER(huart);
	      }

	      /* If Overrun error occurs, or if any error occurs in DMA mode reception,
	         consider error as blocking */
	      if (1)
	      {
	        /* Non Blocking error : transfer could go on.
	           Error is notified to user through user error callback */
	        HAL_UART_ErrorCallback(huart);
	        huart->ErrorCode = HAL_UART_ERROR_NONE;
	      }
	    }



	    return;

	  } /* End if some error occurs */



	  /* UART in mode Transmitter ------------------------------------------------*/
	  if(((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
	  {
			if (huart == &HMI_UART5) {

				if(LocalDisplayTxCB)
				{
					LocalDisplayTxCB(huart);
				}

			}

			if (huart == &GSM_USART2) {
				if(LocalModemTxCB)
				{
					LocalModemTxCB(huart);
				}

			}

			if (huart == &INVERTER_USART3) {

				if(LocalInverterTxCB)
				{
					LocalInverterTxCB(huart);
				}

			}
	    return;
	  }



}



extern "C"
{


    void My_UART_IRQHandlerC(UART_HandleTypeDef *huart)
    {
    	My_UART_IRQHandler(huart);
    }
}

namespace System_serial {

//UART_HandleTypeDef *serial::Internal_UartHandler;

serial::serial(UART_HandleTypeDef *huart) {
	Internal_UartHandler = huart;
}

void serial::TransmitData(const uint8_t *data, uint16_t size, uint32_t timeout) {

	HAL_UART_Transmit(Internal_UartHandler, data, size, timeout);

}


void serial::RxintEn(uint8_t *buf,uint16_t size)
{
//	/__HAL_UART_CLEAR_OREFLAG(Internal_UartHandler);
	//buffer and length can be dummy as this function is just used to enable interrupts for specific uart
	HAL_UART_Receive_IT(Internal_UartHandler, buf, size);
}


void serial::TxintEn(uint8_t *buf,uint16_t size)
{

	HAL_UART_Transmit_IT(Internal_UartHandler,buf, size);
}


void serial::RxintDis()
{
  HAL_UART_AbortReceive_IT(Internal_UartHandler);
}


void serial::TxintDis()
{
	HAL_UART_AbortTransmit_IT(Internal_UartHandler);
}

}

