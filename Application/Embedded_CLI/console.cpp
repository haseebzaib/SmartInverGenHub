/*
 * console.c
 *
 *  Created on: Dec 23, 2023
 *      Author: hzaib
 */

/************************************************************************/
/* Program:    Enviornmental_Sensor    Date: Dec 23, 2023               */
/* Version:     V1                                                      */
/* Autor: Haseeb Zaib                                                   */
/* This file handles all the functions related to Console	            */
/************************************************************************/

/**
 * Esc[A (key up) and Esc[B (key down) navigates through history
 * Tab moves cursor to the end of autocompleted command
 * backspace removes last typed character
 */

#include "main.h"
#include "app_main.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include "cstddef"
#include <ctype.h>
#include <Embedded_CLI/embedded_cli.hpp>
#include "usart.h"
#include <array>
#include <utility>  // For std::forward
#include "xmodem.hpp"

static EmbeddedCli *cli;
// Definitions for CLI UART peripheral
#define UART_CLI_PERIPH &CONSOLE
#define UART_RX_BUFF_SIZE 1
#define CLI_BUFFER_SIZE 2048
#define CLI_RX_BUFFER_SIZE 16
#define CLI_CMD_BUFFER_SIZE 32
#define CLI_HISTORY_SIZE 32
#define CLI_MAX_BINDING_COUNT 32
uint32_t max_wait_time = 300000; //5min wait time
uint32_t prev_max_wait_time = 0;


/**
 * Definition of the cli_printf() buffer size.
 * Can make smaller to decrease RAM usage,
 * make larger to be able to print longer strings.
 */
// Default buffer size can be defined as a constant:
constexpr size_t  CLI_PRINT_BUFFER_SIZE = 300;
// UART buffers
uint8_t UART_CLI_rxBuffer[UART_RX_BUFF_SIZE] = { 0 };

//static CLI_UINT cliBuffer[BYTES_TO_CLI_UINTS(CLI_BUFFER_SIZE)];

bool cliIsReady = false;
uint8_t cli_other = 0;
uint8_t flag_cli = 0;
uint8_t invoke_time = 0;
uint8_t invoke_date = 0;

uint8_t oad_transmit(uint8_t data, uint32_t timeout)
{

}
uint8_t oad_receive(uint8_t *data,uint16_t length ,uint32_t timeout)
{

}

// STM32 UART callback function, to pass received characters to the embedded-cli
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart == UART_CLI_PERIPH && cliIsReady) {
		HAL_UART_Receive_IT(UART_CLI_PERIPH, UART_CLI_rxBuffer,
		UART_RX_BUFF_SIZE);

		if (!cli_other) {
			char c = UART_CLI_rxBuffer[0];
			embeddedCliReceiveChar(cli, c);
		} else {
			flag_cli = 1;
		}
	}


}

static void writeCharToCli(EmbeddedCli *embeddedCli, char c) {
	static uint8_t c_to_send = c;
	HAL_UART_Transmit(UART_CLI_PERIPH, &c_to_send, 1, 100);
}

template <std::size_t BufferSize = CLI_PRINT_BUFFER_SIZE, typename... Args>
void cli_printf(EmbeddedCli* cli, const char* format, Args&&... args) {
    // Use a static buffer in static storage to keep stack usage low.
    static std::array<char, BufferSize> buffer{};

    int length = 0;
    if constexpr (sizeof...(args) == 0) {
        // If no additional arguments are given,
        // simply copy the format string into the buffer.
        length = std::snprintf(buffer.data(), buffer.size(), "%s", format);
    } else {
        // Otherwise, use snprintf to format the string with the arguments.
        length = std::snprintf(buffer.data(), buffer.size(), format, std::forward<Args>(args)...);
    }

    if (length < 0) {

        return;
    }

    // Call the CLI print function with the formatted string.
    embeddedCliPrint(cli, buffer.data());
}
void receiveString(EmbeddedCli *cli, char *buffer, size_t bufferSize) {
	size_t index = 0;
	int i, j;
	prev_max_wait_time = HAL_GetTick();
	while (1) {
		if (flag_cli) {
			if (UART_CLI_rxBuffer[0] == '\x0d' || index == bufferSize - 1) {
				// Exit the loop when Enter key is pressed or buffer is full
				buffer[index] = '\0'; // Null-terminate the string
				flag_cli = 0;
				// Convert the entered string to lowercase
//				for (size_t i = 0; buffer[i]; i++) {
//					buffer[i] = tolower(buffer[i]);
//				}
				//Iterating each character and removing non alphabetical characters.
				for (i = 0; buffer[i] != '\0'; ++i) {
					while (!((buffer[i] >= 'a' && buffer[i] <= 'z')
							|| (buffer[i] >= 'A' && buffer[i] <= 'Z')
							|| buffer[i] == '_' || buffer[i] == ':'
							|| (buffer[i] >= 0x30 && buffer[i] <= 0x39)
							|| buffer[i] == '\0')) {
						for (j = i; buffer[j] != '\0'; ++j) {
							buffer[j] = buffer[j + 1];
						}
						buffer[j] = '\0';
					}
				}
				break;
			} else {
				// Check if the received character is an alphabet character
				if ((UART_CLI_rxBuffer[0] == '\b'
						|| UART_CLI_rxBuffer[0] == 0x7F)) {
					cli->writeChar(cli, '\b');
					cli->writeChar(cli, ' ');
					cli->writeChar(cli, '\b');
					--index;
					buffer[index] = '\0';
				} else {
					buffer[index++] = UART_CLI_rxBuffer[0];
					cli->writeChar(cli, UART_CLI_rxBuffer[0]);
				}
			}
			flag_cli = 0;
		}

		//incase no usb then also break it
		if ((HAL_GetTick() - prev_max_wait_time > max_wait_time)) { //|| !HAL_GPIO_ReadPin(USB_DETECT_GPIO_Port, USB_DETECT_Pin)
			//we break the loop if time goes above 5min
			break;
		}

	}
}

void onClearCLI(EmbeddedCli *cli, char *args, void *context) {
	cli_printf(cli, "\33[2J");
}


/**
 * Initializes the Embedded CLI instance and sets up command bindings.
 */
void initializeEmbeddedCli() {
	cli = embeddedCliNewDefault();

	CliCommandBinding clear_binding = { .name = "clear", .help =
			"Clears the console", .tokenizeArgs = true, .context = NULL,
			.binding = onClearCLI };



	// EmbeddedCli *cli = getCliPointer;debug_scd4x_PM25
	embeddedCliAddBinding(cli, clear_binding);

	// Assign character write function
	cli->writeChar = writeCharToCli;
	// cli->onCommand = onCommand;
	// CLI has now been initialized, set bool to true to enable interrupts.
	cliIsReady = true;
	cli_other = 0;
}

void console_process(void) {
	embeddedCliProcess(cli);

}

void console_init(void) {
	initializeEmbeddedCli();
	HAL_UART_Receive_IT(UART_CLI_PERIPH, UART_CLI_rxBuffer, UART_RX_BUFF_SIZE);
}
