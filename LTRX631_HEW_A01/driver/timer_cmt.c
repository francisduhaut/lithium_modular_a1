/*
 * timer_cmt2.c
 *
 *  Created on: 23 févr. 2013
 *      Author: duhautf
 */

#include "include.h"

extern char 	line1[LCD_NB_CHAR+1];         		/* Line 1 (16 car) */
extern char 	line2[LCD_NB_CHAR+1];           	/* Line 2 (16 car) */

extern ulong Ah100ms;
extern ulong Wh100ms;
extern uchar FlagLoop;
extern ulong TimeThAmb;
extern ulong TimeCheckBatOff;
extern ulong TimeCheckBatOn;
extern ulong TimerCANRamRW;

extern  ulong U0TimeOut;
extern  ulong SCI1TimeOut;

extern char SendUart0;

// MODIF R2.5
extern uchar FlagWdogReset;
extern void usb_cpu_WdogRefresh(void);
extern uchar CptWdogReset;

// MODIF 3.2
/*#ifndef HAWKER_US	// No TPPL in Hawker US
extern ulong TimerVgaz;
extern uchar FlagIbat;
#endif*/

/**
 * @brief  This function initialises a CMT timer to generate a compare
 *         match interrupt every 1ms.
 * @param  None
 * @retval None
 */
void Init_Timer(void)
{
	/* Protection off */
	SYSTEM.PRCR.WORD = 0xA503;

	/* Cancel the CMT2 module clock stop mode */
	MSTP_CMT2 = 0;

	/* Protection on */
	SYSTEM.PRCR.WORD = 0xA500;

	/* Set CMT2 interrupt priority level to 10*/
	IPR(CMT2,CMI2) = 0x0A;
	/* Enable CMT2 interrupts */
	IEN(CMT2,CMI2) = 0x1;
	/* Clear CMT2 interrupt flag */
	IR(CMT2,CMI2) = 0x0;

	/* Select the PCLK clock division as PCLK/8  */
	/* -> PCLK = 48Mhz / 8 -> count at 6Mh  -> for 1ms int -> CMCOR = 0.001*6000000 = 6000 */
	CMT2.CMCR.BIT.CKS = 0x0;
	CMT2.CMCOR = 6000;

	/* Enable the compare match interrupt */
	CMT2.CMCR.BIT.CMIE = 1;

	/* Start CMT2 count */
	CMT.CMSTR1.BIT.STR2 = 1;
    
    // MOIF R2.5
    CptWdogReset = 0;
}


/**
 * @brief  The CMT2 interrupt handler is called after every  1ms
 *         -> see vect.h for interrupt vector definition
 * @param  None
 * @retval None
 */
void Excep_CMT2_CMI2(void)
{
	State.TimerMs++;
	
	// CAN BMS timer
	if (can1_timer_tx_bms != 0)
		can1_timer_tx_bms--;
	if (TimerBmsRX != 0)
		TimerBmsRX--;
	
	CAN_bms_Chrono();

	if (State.TimerMs == 0) 	// MODIF R3.1
		TimerCANRamRW = 0;
    
    // MODIF R2.1
    if (State.TimerMs == 0)
    {
        TimeCheckBatOn = 0;
        TimeCheckBatOff = 0;
        U0TimeOut = 0;
        SCI1TimeOut = 0;
    }

	TimeoutMse++;

	Uart0Timer();

	Timer_SCI1();	// Check Timeout on SCI1
	KEY_Timer();
	LCD_LedRefresh();

	/* Calculation of Ah & Wh, must be in Timer */
	if ((State.TimerMs % 100) == 0)
	{
		if (BleLedBlink != 0)
		{
			if (State.TimerMs > TimerBleLedBlink)
				BleLedBlinkOff;
			else if (DataW.IdentCharger == 0)
				DataW.IdentCharger = 30;
		}
		else
		{
			TimerBleLedBlink = State.TimerMs + 5000L;
		}
		
        // MODIF R2.5
        if ((FlagWdogReset != 0) && (CptWdogReset < 40))
        {
            usb_cpu_WdogRefresh();
            CptWdogReset++;
        }
        else if (FlagWdogReset == 0)
            CptWdogReset = 0;
        // END
        
		if ((State.Charger > StateChgerIdle) && (State.Charge != StateChgAvail))
		{
			// Ah+ only if charger in charge and not in availaible
			Ah100ms += DataR.Ibat;    /* In 1/10 A */
			if (360000 < Ah100ms)     /* = (3600 * 10[1/100ms] * 10[1/10A]) */
			{
				Ah100ms -= 360000;
				DataR.Ah++;
			}
		}
		Wh100ms += DataR.Power;
		if (360000 < Wh100ms)       /* = (3600 * 10[1/100ms] * 10[10Wh]) */
		{
			Wh100ms -= 360000;
			DataR.Wh++;
		}
		// MODIF 3.2
		/*#ifndef HAWKER_US	// No TPPL in Hawker US
		if ((DataR.VbatCell > (Memo.VgazCell + 50)) && (FlagIbat == 2))
			TimerVgaz++;
		#endif*/
	}
	if ((State.TimerMs % 1000) == 0)
	{
		State.TimerSec++;
        TimeThAmb++;
		if (ChgData.StopTimerChg == OFF)
			ChgData.TimerSecCharge++;				
		Display.UpdateDateTime = 1;

		SendUart0 = 1;

	}

	if ((State.TimerMs % 250) == 0)
	{
		RefreshLCDOn;
		if (FlagLoop == ON)
			Display.UpdateCurrent = 1;
	}

	if ((State.TimerMs % 500) == 0)
	{
		if (FlagLoop == OFF)
			Display.UpdateCurrent = 1;
	}

#ifdef DEMO
	/*if ((State.TimerSec % 60) == 0)
		StartStopOn;
	else if((State.TimerSec % 600) == 400)
	{ 
        if (State.Charge < StateChgStAvail)
		    State.Charge = StateChgStAvail;
	    Memo.ChgAh += 1;
		
		
	}
	else if((State.TimerSec % 600) == 595)
		StartStopOff;*/
#endif

#ifdef TEST_COMIQ
    if ((State.TimerSec % 600) == 0)
    StartStopOn;
    else if( ((State.TimerSec % 600) == 595) || ((State.Charger == StateChgerChg) && (State.Charge == StateChgPhase1)) )
    StartStopOff;
#endif    

}
