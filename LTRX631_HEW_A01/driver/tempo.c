/*
 * lcd.c
 *
 *  Created on: 22 févr. 2013
 *      Author			: 	duhautf
 */

#include "include.h"

extern void usb_cpu_WdogRefresh (void);


/******************************************************************************
 * Outline        : Delay_us
 * Description    : Delay routine for LCD or any other devices.
 * Argument       : (uint32_t) units - time in, approximately, microseconds
 * Return value   : None
 ******************************************************************************/
void Delay_us(const uint32_t usec)
{
	/* Declare counter value variable based on usec input and DELAY_TIMING */
	volatile uint32_t counter = usec * DELAY_TIMING_US;

	/* Decrement the counter and wait */
	while (counter--)
	{
		usb_cpu_WdogRefresh();
	}
}

/******************************************************************************
 * Outline        : Delay_s
 * Description    : Delay routine for LCD or any other devices.
 * Argument       : (uint8_t) units - time in, approximately, seconds
 * Return value   : None
 ******************************************************************************/
void Delay_s(const uint8_t sec)
{
	/* Declare counter value variable based on usec input and DELAY_TIMING */
	volatile uint32_t counter = ((uint32_t)sec) * DELAY_TIMING_S;

	/* Decrement the counter and wait */
	while (counter--)
	{
		;
	}
}

