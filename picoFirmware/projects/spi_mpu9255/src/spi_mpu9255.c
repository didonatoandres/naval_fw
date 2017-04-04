/*
 * @brief I2CM bus master example using interrupt mode
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"
#include "i2c.h"
#include "spi.h"
#include "mpu9255.h"

#define SIZE 30*1024
#define ACC_DATA_SIZE 6

extern uint8_t flag;
extern SPIM_XFER_T 	xfer;
extern uint8_t flag2;
/*==================[internal data definition]===============================*/

/** @brief used for delay counter */
static uint32_t pausems_count;


static void pausems(uint32_t t)
{
	pausems_count = t;
	while (pausems_count != 0) {
		__WFI();
	}
}


void SysTick_Handler(void)
{
	if(pausems_count > 0) pausems_count--;
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/


int main(void)
{
	int i,accIndex = 0;;

	uint8_t acc_buffer[SIZE];

	/* Generic Initialization */
	initHardware();

	/* Clear activity LED */
	Board_LED_Set(0, false);

	initMPU9255();

	readAccelData(acc_data);


	while (1) {
		pausems(100);
		if(flag == 1){
			readAccelData(acc_data);
			for(i = 0; i < ACC_DATA_SIZE; i++){
				acc_buffer[accIndex] = acc_data[i];
				accIndex ++;
				if(accIndex == SIZE)
					sendData();
					accIndex = 0;
			}
			flag = 0;
		}


//		DEBUGOUT("Hola %d",data[i]);
//		DEBUGOUT("Acc X: %f \t  Gyro X: %f \r\n",acc_data[0]);
//		DEBUGOUT("Acc Y: %f \t  Gyro Y: %f \r\n",acc_data[1]);
//		DEBUGOUT("Acc Z: %f \t  Gyro Z: %f \r\n",acc_data[2]);
//		DEBUGOUT("-------------------------------------------------\r\n");
	//	DEBUGOUT("-------------------------------------------------\r\n");

		/* Toggle LED to show activity. */
//		Board_LED_Toggle(0);
	}

	/* Code never reaches here. Only used to satisfy standard main() */
	return 0;
}
