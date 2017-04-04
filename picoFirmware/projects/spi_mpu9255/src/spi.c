/*
 * spi.c
 *
 *  Created on: Sep 26, 2016
 *      Author: sfmaudet
 */
#include "../inc/spi.h"
#include "board.h"
#include "string.h"

SPIM_XFER_T 	xfer;
uint8_t flag = 0;

void cs_high (void)
{
	int i = 100;
	while(i--);

	CS_HIGH();
}

void cs_low (void)
{
	int i = 100;
	while(i--);

	CS_LOW();
}

void SPI_tranfer_NoINT(uint8_t * rxBuf,uint8_t * txBuf, uint32_t Length){

	Chip_SPI_FlushFifos(LPC_SPI0);

	xfer.rxCount = Length;
	xfer.txCount = Length;
	xfer.rxBuff = (uint8_t *)rxBuf;
	xfer.txBuff = (uint8_t *)txBuf;
	xfer.cbFunc = 0;
	xfer.options = SPIM_XFER_OPTION_SIZE(8);
	xfer.sselNum = 0;
	xfer.state = 0;

	Chip_SPIM_Xfer(LPC_SPI0, &xfer);
}



void SPI_tranfer(uint8_t * rxBuf,uint8_t * txBuf, uint32_t Length){

	Chip_SPI_FlushFifos(LPC_SPI0);

	xfer.rxCount = Length;
	xfer.txCount = Length;
	xfer.rxBuff = (uint8_t *)rxBuf;
	xfer.txBuff = (uint8_t *)txBuf;
	xfer.cbFunc = 0;
	xfer.options = SPIM_XFER_OPTION_SIZE(8);
	xfer.sselNum = 0;
	xfer.state = 0;

	Chip_SPIM_Xfer(LPC_SPI0, &xfer);

	/* Sleep until transfers are complete */
		while (xfer.state != SPIM_XFER_STATE_DONE) {
			__WFI();
		}
}


void SPI0_IRQHandler(void)
{
	uint32_t ints = Chip_SPI_GetPendingInts(LPC_SPI0);

	/* Handle SPI master interrupts only */
	if ((ints & (SPI_INTENSET_RXDYEN | SPI_INTENSET_RXOVEN |
			SPI_INTENSET_TXUREN | SPI_INTENSET_SSAEN | SPI_INTENSET_SSDEN)) != 0) {
		/* SPI master handler */
		Chip_SPIM_XferHandler(LPC_SPI0, &xfer);
	}
}


void initHardware(void)
{
	Board_Init();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);

	/* SPI configuration */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 11, (IOCON_FUNC1 | IOCON_MODE_PULLUP | IOCON_DIGITAL_EN));	/* SPI0_SCK */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 12, (IOCON_FUNC1 | IOCON_MODE_PULLUP | IOCON_DIGITAL_EN));	/* SPI0_MOSI */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 13, (IOCON_FUNC1 | IOCON_MODE_PULLUP | IOCON_DIGITAL_EN));	/* SPI0_MISO */
#if 0
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 14, (IOCON_FUNC1 | IOCON_MODE_PULLUP | IOCON_DIGITAL_EN));	/* SPI0_SSEL0 */
#else
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 14, (IOCON_FUNC0 | IOCON_MODE_PULLUP | IOCON_DIGITAL_EN));	/* SPI0_SSEL0, GPIO */
#endif
	//SPI init
	SPI_CFGSETUP_T cfg;
	SPIM_DELAY_CONFIG_T dcfg;

	Chip_SPI_Init(LPC_SPI0);
	Chip_SPI_Enable(LPC_SPI0);

	cfg.lsbFirst = 0;
	cfg.master = 1;
	cfg.mode = SPI_CLOCK_MODE0;
	Chip_SPI_ConfigureSPI(LPC_SPI0, &cfg);

	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 14);
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, 0, 14);

	Chip_SPIM_SetClockRate(LPC_SPI0, SPI_CLK_FREC);

	dcfg.PreDelay = 0;
	dcfg.PostDelay = 0;
	dcfg.FrameDelay = 0;
	dcfg.TransferDelay = 0;
	Chip_SPIM_DelayConfig(LPC_SPI0, &dcfg);

	Chip_SPIM_DisableLoopBack(LPC_SPI0);

	Chip_SPI_EnableInts(LPC_SPI0, (SPI_INTENSET_RXDYEN |
			SPI_INTENSET_RXOVEN | SPI_INTENSET_TXUREN | SPI_INTENSET_SSAEN |
			SPI_INTENSET_SSDEN));
	NVIC_EnableIRQ(SPI0_IRQn);


	Chip_PININT_Init(LPC_PININT);

	Chip_GPIO_SetPinDIRInput(LPC_GPIO, GPIO_PININT_PORT, GPIO_PININT_PIN);

	/* Configure pin as GPIO */
	Chip_IOCON_PinMuxSet(LPC_IOCON, GPIO_PININT_PORT, GPIO_PININT_PIN,(IOCON_FUNC0 | IOCON_DIGITAL_EN | IOCON_GPIO_MODE));


	/* Configure pin interrupt selection for the GPIO pin in Input Mux Block */
	Chip_INMUX_PinIntSel(GPIO_PININT_INDEX, GPIO_PININT_PORT, GPIO_PININT_PIN);

	/* Configure channel interrupt as edge sensitive and falling edge interrupt */
	Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH(GPIO_PININT_INDEX));
	Chip_PININT_SetPinModeEdge(LPC_PININT, PININTCH(GPIO_PININT_INDEX));
	Chip_PININT_EnableIntHigh(LPC_PININT, PININTCH(GPIO_PININT_INDEX));

	/* Enable interrupt in the NVIC */

	NVIC_EnableIRQ(PININT_NVIC_NAME);


	CS_HIGH();
}


void PININT_IRQ_HANDLER(void){
	Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH(GPIO_PININT_INDEX));
	Chip_PININT_ClearRiseStates(LPC_PININT, PININTCH(GPIO_PININT_INDEX));
	flag = 1;
 }
