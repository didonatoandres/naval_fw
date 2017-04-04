/* Copyright 2014, ChaN
 * Copyright 2016, Matias Marando
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

/** @brief This is an SD card write example using the FatFs library
 */

/** \addtogroup spi SD card write via SPI
 ** @{ */

/*==================[inclusions]=============================================*/

#include "board.h"
#include "ff.h"
#include "sd_spi.h"

/*==================[macros and definitions]=================================*/

#define FILENAME "newfile.txt"

/*==================[internal data declaration]==============================*/

static uint32_t ms_ticks;  /**< 1ms timeticks counter */
static FATFS fs;           /**< FatFs work area needed for each volume */
static FIL fp;             /**< File object needed for each open file */

/*==================[internal functions declaration]=========================*/

/** @brief Hardware initialization function
 *  @return none
 */
static void initHardware(void);

/*==================[internal functions definition]==========================*/

static void initHardware(void)
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

	Chip_SPIM_SetClockRate(LPC_SPI0, 100000);

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
}

/*==================[external functions definition]==========================*/

void SysTick_Handler(void)
{
    ms_ticks++;

    if (ms_ticks >= 10) {
        ms_ticks = 0;
        disk_timerproc();   /* Disk timer process */
    }
}

int main(void)
{
    UINT nbytes;
    FRESULT r;

    initHardware();

    /* Give a work area to the default drive */
    r = f_mount(&fs, "", 0);
    if (r != FR_OK) {
        /* If this fails, it means that the function could
         * not register a file system object.
         * Check whether the SD card is correctly connected */
    }

    /* Create/open a file, then write a string and close it */
    r = f_open(&fp, FILENAME, FA_WRITE | FA_CREATE_ALWAYS);
    if (r == FR_OK) {
        r = f_write(&fp, "Writing some text using picoCIAA!!!\r\n", 37, &nbytes);

        r = f_close(&fp);

        if (nbytes == 37) {
            /* Toggle a LED if the write operation was successful */
            Board_LED_Toggle(0);
        }
    }

    while (1) {
        __WFI();
    }

    return 0;
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
