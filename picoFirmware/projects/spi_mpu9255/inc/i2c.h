/*
 * i2c.h
 *
 *  Created on: Sep 5, 2016
 *      Author: sfmaudet
 */

#ifndef I2C_H_
#define I2C_H_

#include "board.h"

/* I2C peripheral clock is set to 6MHz */
#define I2C_CLK_DIVIDER     (2)

/* 400KHz I2C bit-rate */
#define I2C_BITRATE         (400000)

/** 7-bit I2C addresses of 3-axis sensor on sensor board */
#define I2C_ADDR_7BIT       0x68


#define LPC_I2C_PORT        LPC_I2C0
#define LPC_I2C_INTHAND     I2C0_IRQHandler
#define LPC_IRQNUM          I2C0_IRQn

 void SetupXferRecAndExecute(uint8_t devAddr,
								   uint8_t *txBuffPtr,
								   uint16_t txSize,
								   uint8_t *rxBuffPtr,
								   uint16_t rxSize);

void i2c_init(void);

#endif /* I2C_H_ */
