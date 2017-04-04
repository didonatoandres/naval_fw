/*
 * spi.h
 *
 *  Created on: Sep 26, 2016
 *      Author: sfmaudet
 */

#ifndef PROJECTS_SPI_MPU9255_INC_SPI_H_
#define PROJECTS_SPI_MPU9255_INC_SPI_H_

#include "board.h"

#define GPIO_PININT_PIN 					24  			/* GPIO pin number mapped to PININT */
#define GPIO_PININT_PORT 					0  				/* GPIO port number mapped to PININT */

#define SPI_CLK_FREC 500000

#define GPIO_PININT_INDEX PININTSELECT0						/* PININT index used for GPIO mapping */
#define PININT_IRQ_HANDLER PIN_INT0_IRQHandler  			/* GPIO interrupt IRQ function name */
#define PININT_NVIC_NAME PIN_INT0_IRQn  					/* GPIO interrupt NVIC interrupt name */

#define CS_HIGH()   Chip_GPIO_SetPinOutHigh(LPC_GPIO, 0, 14)
#define CS_LOW()    Chip_GPIO_SetPinOutLow(LPC_GPIO, 0, 14)


void SPI_tranfer(uint8_t * rxBuf,uint8_t * txBuf, uint32_t Length);

void cs_low (void);
void cs_high (void);

void initHardware(void);

void PININT_IRQ_HANDLER(void);

#endif /* PROJECTS_SPI_MPU9255_INC_SPI_H_ */
