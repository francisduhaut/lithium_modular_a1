/*
 * hwsetup.c
 *
 *  Created on: 22 févr. 2013
 *      Author: duhautf
 */

#include "include.h"


/*******************************************************************************
 * Outline       : HardwareSetup
 * Description   : Contains all the setup functions called at device restart
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void HardwareSetup(void)
{
	ConfigureOperatingFrequency();
	ConfigureOutputPorts();
	ConfigureInterrupts();
	EnablePeripheralModules();
}

/*******************************************************************************
 * Outline       : ConfigureOperatingFrequency
 * Description   : Configures the clock settings for each of the device clocks
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void ConfigureOperatingFrequency(void)
{                    
	/* Protection off */
	SYSTEM.PRCR.WORD = 0xA503u;

	/* Stop sub-clock */
	SYSTEM.SOSCCR.BYTE = 0x01u;

	/* Set main oscillator settling time to 10ms (131072 cycles @ 12MHz) */
	SYSTEM.MOSCWTCR.BYTE = 0x0Du;

	/* Set PLL circuit settling time to 10ms (2097152 cycles @ 192MHz) */
	SYSTEM.PLLWTCR.BYTE = 0x0Eu;

	/* Set PLL circuit to x16 */
	SYSTEM.PLLCR.WORD = 0x0F00u;

	/* Start the external 12Mhz oscillator */
	SYSTEM.MOSCCR.BYTE = 0x00u;

	/* Turn on the PLL */
	SYSTEM.PLLCR2.BYTE = 0x00u;

	/* Wait over 12ms (~2075op/s @ 125KHz) */
	for(volatile uint16_t i = 0; i < 2075u; i++)
	{
		nop();
	}

	/* Configure the clocks as follows -
    Clock Description              Frequency
    ----------------------------------------
    PLL Clock frequency...............192MHz
    System Clock Frequency.............96MHz
    Peripheral Module Clock B..........48MHz     
    FlashIF Clock......................48MHz
    External Bus Clock.................48MHz */                 
	//SYSTEM.SCKCR.LONG = 0x21821211u;
	SYSTEM.SCKCR.LONG = 0x21421211u;    

	/* Configure the clocks as follows -
    Clock Description              Frequency
    ----------------------------------------f
    USB Clock..........................48MHz      
    IEBus Clock........................24MHz */ 
	SYSTEM.SCKCR2.WORD = 0x0033u;

	/* Set the clock source to PLL */
	SYSTEM.SCKCR3.WORD = 0x0400u;

	/* Protection on */
	SYSTEM.PRCR.WORD = 0xA500u;
}

/*******************************************************************************
 * Outline       : ConfigureOutputPorts
 * Description   : Configures the port and pin direction settings, and sets the
 *                  pin outputs to a safe level.
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void ConfigureOutputPorts(void)
{

}

/*******************************************************************************
 * Outline       : ConfigureInterrupts
 * Description   : Configures the interrupts used
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void ConfigureInterrupts(void)
{
}

/*******************************************************************************
 * Outline       : EnablePeripheralModules
 * Description   : Enables and configures peripheral devices on the MCU
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void EnablePeripheralModules(void)
{
}
