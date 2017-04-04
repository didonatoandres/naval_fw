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
#include "mpu9255.h"

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
	int loop = 1;	/* Used to fix the unreachable statement warning */

	float giro_final[3],accel_final[3],mag_final[3];

	uint8_t mag_data;
	/* Generic Initialization */
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);
	Board_Init();

	/* Clear activity LED */
	Board_LED_Set(0, false);

	i2c_init();

	initMPU9255();


	while (loop) {
		pausems(100);


		readAccelData(accel_final);
		readGyroData(giro_final);
		readMagnetData(mag_final);
		readMagnetCode(&mag_data);

		DEBUGOUT("Acc X: %f \t  Gyro X: %f \t Mag X: %f \r\n",accel_final[0],giro_final[0],mag_final[0]);
		DEBUGOUT("Acc Y: %f \t  Gyro Y: %f \t Mag Y: %f  \r\n",accel_final[1],giro_final[1],mag_final[1]);
		DEBUGOUT("Acc Z: %f \t  Gyro Z: %f \t Mag Z: %f  \r\n",accel_final[2],giro_final[2],mag_final[2]);
		DEBUGOUT("Mag ID %xH  \r\n",mag_data);


		/* Toggle LED to show activity. */
		Board_LED_Toggle(0);



	}

	/* Code never reaches here. Only used to satisfy standard main() */
	return 0;
}
