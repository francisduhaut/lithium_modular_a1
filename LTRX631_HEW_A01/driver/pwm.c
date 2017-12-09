#include "include.h"

/*******************************************************************************
 * Outline       : Init_PWM
 * Description   : Function configures the MTU's output pin MTIOC3C. It then
 *                  initialises the MTU's channel 3 to output a 1KHz PWM signal,
 *                  with interrupt generations enabled.
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void Init_PWM(void)
{    
	/* Configure PC6 as an output */
	PORTC.PDR.BIT.B6 = 1;

	/* Configure PC6 for peripheral function */
	PORTC.PMR.BIT.B6 = 1;

	/* Enable write to PFSWE bit */
	MPC.PWPR.BYTE = 0x00;

	/* Disable write protection to PFS registers */
	MPC.PWPR.BYTE = 0x40;

	/* Configure PC6 for MTIOC3C outputs */
	MPC.PC6PFS.BIT.PSEL = 0x1;

	/* Enable write protection to PFS registers */
	MPC.PWPR.BYTE = 0x80;

	/* Protection off */
	SYSTEM.PRCR.WORD = 0xA503;

	/* Cancel the MTU3 module clock stop mode */
	MSTP_MTU3 = 0x0;

	/* Protection on */
	SYSTEM.PRCR.WORD = 0xA500;

	/* Set MTU3 TGIC3 interrupt priority level to 7 */
	IPR(MTU3,TGIC3) = 0x7;
	/* Enable MTU3 TGIA3 interrupts */
	IEN(MTU3,TGIC3) = 0x1;
	/* Clear MTU3 TGIA3 interrupt flag */
	IR(MTU3,TGIC3) = 0x0;

	/* Set MTU3 TGIB3 interrupt priority level to 7 */
	IPR(MTU3,TGID3) = 0x7;
	/* Enable MTU3 TGIB3 interrupts */
	IEN(MTU3,TGID3) = 0x1;
	/* Clear MTU3 TGIB3 interrupt flag */
	IR(MTU3,TGID3) = 0x0;

	/* Clock PCLK/1=48MHz, count at falling edge,
       TCNT cleared by TGRC compare match */
	MTU3.TCR.BYTE = 0xA8;

	/* Operate timers in pwm mode */
	MTU3.TMDR.BYTE = 0x02;

	/* Initial MTIOC3C output is 1 and toggled at
       TGRC compare match. TRGD output is disabled. */
	MTU3.TIORL.BYTE = 0x37;


	/* Enable TGIEC interrupts */
	MTU3.TIER.BYTE = 0x04;

	/* Enable access to protected MTU registers */
	MTU.TRWER.BIT.RWE = 0x1;

	/* Use TGRC to set the pwm cycle to 1KHz*/
	MTU3.TGRC = 48000;

	/* TGRD used to set the pwm duty cycle to 90% */
	MTU3.TGRD = 18000;

	/* Start the TCNT counter */
	MTU.TSTR.BIT.CST3 = 0x1;
}
/*******************************************************************************
 * End of function Init_PWM
 *******************************************************************************/

/*******************************************************************************
 * Outline      : Excep_TPU9_TGIA9
 * Description  : MTU3 TGIA3 interrupt handler. This function updates the duty
 *                 cycle by 1%, resets the duty to 10% if it reaches 100%, and
 *                 automatically toggles the MTIOC3A pin on every TGRA output
 *                 compare match.
 * Argument     : none
 * Return value : none
 *******************************************************************************/
void Excep_TPU9_TGIC9(void)
{    
	/* Stop the TCNT counter */
	MTU.TSTR.BIT.CST3 = 0x0;

	MTU3.TGRD =  (Menu.Contrast * 48000) / 100L;

	/* Start the TCNT counter */
	MTU.TSTR.BIT.CST3 = 0x1;
}
/*******************************************************************************
 * End of function Excep_TPU9_TGIA9
 *******************************************************************************/

