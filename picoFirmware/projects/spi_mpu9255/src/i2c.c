/*
 * i2c.c
 *
 *  Created on: Sep 5, 2016
 *      Author: sfmaudet
 */

#include "i2c.h"


/* I2CM transfer record */
static I2CM_XFER_T          i2cmXferRec;


/* Initializes pin muxing for I2C interface - note that SystemInit() may
   already setup your pin muxing at system startup */
static void Init_I2C_PinMux(void)
{
#if defined(BOARD_NXP_LPCXPRESSO_54102)
	/* Connect the I2C1_SDA and I2C1_SCL signals to port pins */
#if (I2C_BITRATE > 400000)
	/* Enable Fast Mode Plus for I2C pins */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 23, (IOCON_FUNC1 | IOCON_FASTI2C_EN | IOCON_DIGITAL_EN));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 24, (IOCON_FUNC1 | IOCON_FASTI2C_EN | IOCON_DIGITAL_EN));

#else
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 23, (IOCON_FUNC1 | IOCON_DIGITAL_EN));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 24, (IOCON_FUNC1 | IOCON_DIGITAL_EN));
#endif

#else
	/* Configure your own I2C pin muxing here if needed */
#warning "No I2C pin muxing defined"
#endif
}

/* Setup I2C */
 static void setupI2CMaster(void)
{
	/* Enable I2C clock and reset I2C peripheral */
	Chip_I2C_Init(LPC_I2C_PORT);

	/* Setup clock rate for I2C */
	Chip_I2C_SetClockDiv(LPC_I2C_PORT, I2C_CLK_DIVIDER);

	/* Setup I2CM transfer rate */
	Chip_I2CM_SetBusSpeed(LPC_I2C_PORT, I2C_BITRATE);

	/* Enable I2C master interface */
	Chip_I2CM_Enable(LPC_I2C_PORT);
}

/* Function to wait for I2CM transfer completion */
static void WaitForI2cXferComplete(I2CM_XFER_T *xferRecPtr)
{
	/* Test for still transferring data */
	while (xferRecPtr->status == I2CM_STATUS_BUSY) {
		/* Sleep until next interrupt */
		__WFI();
	}
}

/* Function to setup and execute I2C transfer request */
 void SetupXferRecAndExecute(uint8_t devAddr,
								   uint8_t *txBuffPtr,
								   uint16_t txSize,
								   uint8_t *rxBuffPtr,
								   uint16_t rxSize)
{
	/* Setup I2C transfer record */
	i2cmXferRec.slaveAddr = devAddr;
	i2cmXferRec.status = 0;
	i2cmXferRec.txSz = txSize;
	i2cmXferRec.rxSz = rxSize;
	i2cmXferRec.txBuff = txBuffPtr;
	i2cmXferRec.rxBuff = rxBuffPtr;

	Chip_I2CM_Xfer(LPC_I2C_PORT, &i2cmXferRec);
	/* Enable Master Interrupts */
	Chip_I2C_EnableInt(LPC_I2C_PORT, I2C_INTENSET_MSTPENDING | I2C_INTENSET_MSTRARBLOSS | I2C_INTENSET_MSTSTSTPERR);
	/* Wait for transfer completion */
	WaitForI2cXferComplete(&i2cmXferRec);
	/* Disable all Interrupts */
	Chip_I2C_DisableInt(LPC_I2C_PORT, I2C_INTENSET_MSTPENDING | I2C_INTENSET_MSTRARBLOSS | I2C_INTENSET_MSTSTSTPERR);

	if (i2cmXferRec.status != I2CM_STATUS_OK) {
		DEBUGOUT("\r\nI2C error: %d\r\n", i2cmXferRec.status);
	}
}


 void LPC_I2C_INTHAND(void)
{
	uint32_t state = Chip_I2C_GetPendingInt(LPC_I2C_PORT);

	/* Error handling */
	if (state & (I2C_INTENSET_MSTRARBLOSS | I2C_INTENSET_MSTSTSTPERR)) {
		Chip_I2CM_ClearStatus(LPC_I2C_PORT, I2C_STAT_MSTRARBLOSS | I2C_STAT_MSTSTSTPERR);
	}

	/* Call I2CM ISR function with the I2C device and transfer rec */
	if (state & I2C_INTENSET_MSTPENDING) {
		Chip_I2CM_XferHandler(LPC_I2C_PORT, &i2cmXferRec);
		if (i2cmXferRec.status == I2CM_STATUS_OK) {
			Chip_I2C_DisableInt(LPC_I2C_PORT, I2C_INTENSET_MSTPENDING);
		}
	}
}



void i2c_init(void){

	/* Setup I2C pin muxing */
	Init_I2C_PinMux();

	/* Setup I2C and master */
	setupI2CMaster();

	/* Enable the interrupt for the I2C */
	NVIC_EnableIRQ(LPC_IRQNUM);
}
