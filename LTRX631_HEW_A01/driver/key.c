/*
 * switch.c
 *
 *  Created on: 22 févr. 2013
 *      Author			: 	duhautf
 */

#include "include.h"

#define KEYSHORT          25          	/* Length for short key based on TimerMs */
#define KEYLONG           1500        	/* Length for long key based on TimerMs */

extern uchar MnuLevel;          	    /* Level of current display (0 : no menu) */
extern uint MnuListCur[6];
ulong  CptKeyShort;  			        /* Based on TimerMs */
ulong  CptKeyLong;   				    /* Based on TimerSec */
// MODIF R2.7
uchar  PLCPulseOffOnSave;

extern ulong VbatModCell;
extern uchar FlagDisconnectInCharge;    // MODIF 2.8

/**
 * @brief  Key manage in chrono
 * @param  None
 * @retval None
 */
void KEY_Chrono (void)
{
	//if (ManuReg != 0)
	//  return;

	//if ((MnuListCur[MnuLevel] == 1) && (Key.MenuLong == 1))
	//{
	//MnuLevel = 0;
	//MnuListCur[1] = 0;
	//Key.MenuLong = 0;
	//KEY_Reset();
	//}

	if (Key.StartShort == 1)		//short press on Start button
	{
		if (MnuLevel == 0)
		{
			if ((StartStop == 0) && (ChgData.Profile != LITH))
			{
				StartStopOn;
			}
			else
			{
				if (ChgData.Profile != LITH)
					StartStopOff;
				ForceEqualOff;
                FlagDisconnectInCharge = 0; // MODIF 2.8
			}
			if ((Key.Right != 0)
					&& (ChgData.Profile != GEL) // No manual equal on this profile
					&& (ChgData.Profile != RGT)
					&& (ChgData.Profile != XFC)
					&& (ChgData.Profile != TPPL)
					&& (Menu.CECOnOff == 0))
			{
				ManuEqualOn;
				Key.RightShort = 0;
			}
			Key.StartShort = 0;
            // MODIF R2.3
            if ((Key.Up != 0) && ((Key.Down != 0)))
            {
                TestSwOn;
                Key.UpShort = 0;
                Key.DownShort = 0;
            }
		}
	}
	else
	{
		if (ForceEqual == 0)
		{
			ManuEqualOff;
		}
	}

	// Force equal mode
	if ((Key.UpShort !=0) && (ForceEqual != 0))
	{
		if (DataW.Ireq < 300)
			DataW.Ireq += 10; // +1A
		Key.UpShort = 0;
	}
	if ((Key.DownShort !=0) && (ForceEqual != 0))
	{
		if (DataW.Ireq > 10)
			DataW.Ireq -= 10; // -1A
		Key.DownShort = 0;
	}

	switch (State.Charger)
	{
	case StateChgerStChg:
	case StateChgerChg:
		if ((Key.RightShort == 1) && (MnuLevel == 0))
		{
			if ((RequestEqual == 0) && (ManuEqual == 0)
					//&& (ChgData.Profile != GEL) 	// No request equal on this profile
					&& (ChgData.Profile != RGT)
					//&& (ChgData.Profile != XFC)   // MODIF R2.6
					&& (ChgData.Profile != EQUAL)
					&& (Menu.CECOnOff == 0))
			{
				RequestEqualOn;
				if ((State.Charger == StateChgerChg) && (State.Charge == StateChgAvail))
					ChgData.TimeEqual = ChgData.TimerSecCharge;
			}
			else
			{
				if ((RequestEqual != 0) && (State.Charger == StateChgerChg) && (State.Charge == StateChgAvail))
				{
					ChgData.TimeEqual = 0xFFFFFFFF;
				}
				RequestEqualOff;
			}
			Key.RightShort = 0;
		}

		if (Key.StartLong == 1)
		{
			InstStartOn;
			IQData.BatWarning &= ~BatDefLowLevel; // Start with a WiIQ low level
			if ((BadBatDef != 0) && (State.Charger == StateChgerChg) && (VbatModCell < 1800) && (VbatModCell > 1400) && (StateIdle == 0))
			{
				State.Charger = StateChgerStChg;
				BadBatDefOff;
				ForceEqualOn;
				ChgData.Profile = EQUAL;
				ManuEqualOn;
			}
			Key.StartLong = 0;
		}
		if ((State.Charger == StateChgerChg) && (State.Charge == StateChgEqual))
			RequestEqualOff; 	// equal started -> equal resetted
		break;
	default:
		RequestEqualOff;
		InstStartOff;
		break;
	}

}


/**
 * @brief  Reset key state
 * @param  None
 * @retval None
 */
void KEY_Reset (void)
{
	Key.DownShort = 0;
	Key.DownLong = 0;
	Key.Down = 0;
	Key.Menu = 0;
	Key.MenuLong = 0;
	Key.MenuShort = 0;
	Key.Start = 0;
	Key.StartLong = 0;
	Key.StartShort = 0;
	Key.Up = 0;
	Key.UpLong = 0;
	Key.UpShort = 0;
	Key.Right = 0;
	Key.RightLong = 0;
	Key.RightShort = 0;
	CptKeyShort = 0;
	CptKeyLong = 0;
    //PLCPulseOffOnSave = 0;
}


/**
 * @brief  Key timer called every ms
 * @param  None
 * @retval None
 */
void KEY_Timer(void)
{
	/* Launch on key interrupt and on timer ms */
	ulong Ms;
	Ms = State.TimerMs;
    
    // MODIF R2.7 : PLC
    if (PLCPulseOffOnSave != PORT9.PIDR.BIT.B7)
    {
        PLCPulseOffOnSave = PORT9.PIDR.BIT.B7;
        if ((PORT9.PIDR.BIT.B7 == 0) && (Menu.PLCPulseOffOn == 1))
            StartStopOn;
        else if (Menu.PLCPulseOffOn == 1)   
            StartStopOff;
    }
    if (Menu.PLCPulseOffOn == 1)
    {
        Menu.AutoStartOffOn =  1;
        if ((Battery == 0) && (PORT9.PIDR.BIT.B7 == 0))
        {
          StartStopOn;
        }
    }


        
	/* Set Key0 detection  SW3 */
    // MODIF R2.7 : PLC
	//if ((PORTF.PIDR.BIT.B5 == 0) || (PORT9.PIDR.BIT.B7 == 0))
    if ((PORTF.PIDR.BIT.B5 == 0) || ((PORT9.PIDR.BIT.B7 == 0) && (Menu.PLCPulseOffOn == 0)))
	{ 								/* Key is pressed */
		if (Key.Start == 0)
		{                         	/* Key was previously not pressed, timers are set */
			Key.Start = 1;
			CptKeyShort = (Ms + KEYSHORT);
			CptKeyLong = (Ms + KEYLONG);
		}
	}
	else
	{                           	/* Key is released */
		if (Key.Start == 1)
		{
			if ((CptKeyShort <= Ms) && (Ms <= CptKeyLong))
				Key.StartShort = 1;
		}
		Key.Start = 0;
	}

	if (Key.Start == 1)
	{                           	/* Key has been pressed */
		if (CptKeyLong <= Ms)
		{                         	/* Key is still pressed, time > Tlong */
			Key.StartLong = 1;
			if (Restart != 0)
				StateWDogOn;
		}
	}



	/* Set Key1 detection  SW2 */
	if (PORT0.PIDR.BIT.B2 == 0)
	{ 								/* Key is pressed */
		if ((Key.Menu == 0) && (Key.MenuReset == 0))
		{                         	/* Key was previously not pressed, timers are set */
			Key.Menu = 1;
			CptKeyShort = (Ms + KEYSHORT);
			CptKeyLong = (Ms + KEYLONG);
		}
	}
	else
	{                           	/* Key is released */
		if (Key.Menu == 1)
		{
			if ((CptKeyShort <= Ms) && (Ms <= CptKeyLong))
				Key.MenuShort = 1;

		}
		Key.Menu = 0;
		Key.MenuReset = 0;
	}

	if (Key.Menu == 1)
	{                           	/* Key has been pressed */
		if (CptKeyLong <= Ms)
		{                         	/* Key is still pressed, time > Tlong */
			Key.MenuLong = 1;
		}
	}


	/* Set Key2 detection SW1 */
	if (PORT0.PIDR.BIT.B3 == 0)
	{ 								/* Key is pressed */
		if (Key.Up == 0)
		{                         	/* Key was previously not pressed, timers are set */
			Key.Up = 1;
			CptKeyShort = (Ms + KEYSHORT);
			CptKeyLong = (Ms + KEYLONG);
		}
	}
	else
	{                           	/* Key is released */
		if (Key.Up == 1)
		{
			if ((CptKeyShort <= Ms) && (Ms <= CptKeyLong))
				Key.UpShort = 1;
		}
		Key.Up = 0;
	}

	if (Key.Up == 1)
	{                           	/* Key has been pressed */
		if (CptKeyLong <= Ms)
		{                         	/* Key is still pressed, time > Tlong */
			Key.UpLong = 1;
		}
	}


	/* Set Key3 detection SW4 */
	if (PORT0.PIDR.BIT.B5 == 0)
	{ 								/* Key is pressed */
		if (Key.Down == 0)
		{                         	/* Key was previously not pressed, timers are set */
			Key.Down = 1;
			CptKeyShort = (Ms + KEYSHORT);
			CptKeyLong = (Ms + KEYLONG);
		}
	}
	else
	{                           	/* Key is released */
		if (Key.Down == 1)
		{
			if ((CptKeyShort <= Ms) && (Ms <= CptKeyLong))
				Key.DownShort = 1;
		}
		Key.Down = 0;
	}

	if (Key.Down == 1)
	{                           	/* Key has been pressed */
		if (CptKeyLong <= Ms)
		{                         	/* Key is still pressed, time > Tlong */
			Key.DownLong = 1;
		}
	}

	/* Set Key4 detection SW5 */
	if ((PORTJ.PIDR.BIT.B5 == 0) || (PORTG.PIDR.BIT.B0 == 0))
	{ 								/* Key is pressed */
		if (Key.Right == 0)
		{                         	/* Key was previously not pressed, timers are set */
			Key.Right = 1;
			CptKeyShort = (Ms + KEYSHORT);
			CptKeyLong = (Ms + KEYLONG);
		}
	}
	else
	{                           	/* Key is released */
		if (Key.Right == 1)
		{
			if ((CptKeyShort <= Ms) && (Ms <= CptKeyLong))
				Key.RightShort = 1;
		}
		Key.Right = 0;
	}

	if (Key.Right == 1)
	{                           	/* Key has been pressed */
		if (CptKeyLong <= Ms)
		{                         	/* Key is still pressed, time > Tlong */
			Key.RightLong = 1;
		}
	}


	if ((Key.StartShort == 1) || (Key.MenuShort == 1) || (Key.UpShort == 1) || (Key.DownShort == 1) || (Key.RightShort == 1))
	{
		if (TFTBacklight == 0)
			KEY_Reset();
		RestartOff;
		LCD_RefreshScreenSaver();
	}
}
