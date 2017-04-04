/*------------------------------------------------------------------------/
/RTC control module
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2011, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-------------------------------------------------------------------------*/

#include "chip.h"
#include "board.h"
#include "rtc.h"


int rtc_initialize (void)
{
	static int init;

	if (init) /* Already initialized */
		return 1;

	/* RTC Block section ------------------------------------------------------ */
	Chip_RTC_Init(LPC_RTC);

	/* Set current time for RTC */
	/* Current time is 8:00:00PM, 2013-01-31 */



	init = 1;

	return 1;
}

int rtc_gettime (RTC *rtc)
{
	/* TODO stub */
	rtc->hour = 20;
	rtc->mday = 31;
	rtc->min = 0;
	rtc->month = 1;
	rtc->sec = 0;
	rtc->wday = 0;
	rtc->year = 2013;

  return 1;
}

int rtc_settime (const RTC *rtc)
{

  return 1;
}

/**
 * @brief	User Provided Timer Function for FatFs module
 * @return	Nothing
 * @note	This is a real time clock service to be called from FatFs module.
 * Any valid time must be returned even if the system does not support a real time clock.
 * This is not required in read-only configuration.
 */
DWORD get_fattime()
{
	RTC rtc;

	/* Get local time */
	rtc_gettime(&rtc);

	/* Pack date and time into a DWORD variable */
	return ((DWORD) (rtc.year - 1980) << 25)
		   | ((DWORD) rtc.month << 21)
		   | ((DWORD) rtc.mday << 16)
		   | ((DWORD) rtc.hour << 11)
		   | ((DWORD) rtc.min << 5)
		   | ((DWORD) rtc.sec >> 1);
}
