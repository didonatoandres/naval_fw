/* Copyright 2016, Pablo Ridolfi
 * All rights reserved.
 *
 * This file is part of Workspace.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*==================[inclusions]=============================================*/

#include "board.h"
#include "uart.h"

/*==================[macros and definitions]=================================*/

/** uart buffers length */
#define UART_BUFFER_LEN 64

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/** @brief hardware initialization function
 *	@return none
 */
static void initHardware(void);

/*==================[internal data definition]===============================*/

/** Ringbuffers for UART operation */
static RINGBUFF_T txrb;
static RINGBUFF_T rxrb;

/** buffers for ringbuffer */
static uint8_t txbuff[UART_BUFFER_LEN];
static uint8_t rxbuff[UART_BUFFER_LEN];

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void initHardware(void)
{
	Board_Init();
	SystemCoreClockUpdate();
	uart0Init(115200);
}

/*==================[external functions definition]==========================*/

void UART0_IRQHandler(void)
{
	Chip_UART_IRQRBHandler(LPC_USART0, &rxrb, &txrb);
}

void uart0Init(uint32_t baudrate)
{
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 0, IOCON_MODE_INACT | IOCON_FUNC1 | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 1, IOCON_MODE_INACT | IOCON_FUNC1 | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF);

	Chip_UART_Init(LPC_USART0);
	Chip_UART_ConfigData(LPC_USART0, UART_CFG_DATALEN_8 | UART_CFG_STOPLEN_1 | UART_CFG_PARITY_NONE);
	Chip_UART_Enable(LPC_USART0);
	Chip_UART_TXEnable(LPC_USART0);
	Chip_UART_SetBaud(LPC_USART0, baudrate);

	RingBuffer_Init(&txrb, txbuff, 1, UART_BUFFER_LEN, NULL);
	RingBuffer_Init(&rxrb, rxbuff, 1, UART_BUFFER_LEN, NULL);

	Chip_UART_ClearStatus(
			LPC_USART0,
			UART_STAT_OVERRUNINT | UART_STAT_DELTARXBRK | UART_STAT_FRM_ERRINT | UART_STAT_PAR_ERRINT |
			UART_STAT_RXNOISEINT);

	Chip_UART_IntEnable(LPC_USART0, UART_INTEN_RXRDY);
	Chip_UART_IntDisable(LPC_USART0, UART_INTEN_TXRDY);

	NVIC_EnableIRQ(UART0_IRQn);
}

uint32_t uart0send(void * data, size_t len)
{
	return Chip_UART_SendRB(LPC_USART0, &txrb, data, len);
}

uint32_t uart0recv(void * data, size_t len)
{
	return Chip_UART_ReadRB(LPC_USART0, &rxrb, data, len);
}

int main(void)
{
	char buffer[32];
	uint32_t rv;

	initHardware();

	uart0send("hello :)\r\n", 10);

	while (1) {
		rv = uart0recv(buffer, 32);
		if (rv > 0) {
			uart0send(buffer, rv);
			/* do some stuff with the RGB led :) */
			Board_LED_Set(0, buffer[0] & 1);
			Board_LED_Set(1, buffer[0] & 2);
			Board_LED_Set(2, buffer[0] & 4);
		}
		__WFI();
	}
}

/*==================[end of file]============================================*/
