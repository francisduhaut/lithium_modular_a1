/*
 * profil_gel.c
 *
 *  Created on: 01/09/2013
 *      Author: Francis DUHAUT
 */

#include "include.h"

uchar FlagGelIPhase2;
uchar FlagGelTimePhase3;

extern uint	SlopeI;
extern uint ChargerBatCap;
extern uint ModImax;                          /* Imax calculate with connected module */
extern ulong CntRefresh;
extern sint TempCap;
extern uint ItoReach;
extern ulong TimeCheckCurrent;
extern ulong TimeCheckVbat;
extern uchar FlagLoop;
extern uchar FlagLoopPhase;
extern ulong TimeInitVregPhase3;
extern uint Iphase3Tmp;
extern uchar FlagDesul;

/************************************************************************************
Name:        Profile GEL
 ************************************************************************************/
void  ProfileGEL(void)
{
	FlagDesul = OFF;

	switch (State.Charge)     /* Select current phase */
	{
	case StateChgStartPhase1:     /* Just at beginning of phase 1 */

		SlopeI = 2000;            // 20A/s
#ifdef LIFEIQ
		if ((IQWIIQLink != 0) && ((IQData.BatWarning & BatWarningEqual) != 0))
		{
			FlagIQEqual = ON;
		}
#endif
		ChargerBatCap = (ulong)(Menu.Imax * BATCAP_C5) / 1000L;
		if ((Menu.CapAutoManu == 0) && (IQWIIQLink == 0))
			Memo.BatCap = ChargerBatCap;
		// Init current-voltage
		Memo.Iphase1 = Memo.BatCap * 2;	// 20% Capacity
		Memo.Iphase3 = (Memo.BatCap / 10) + 5;	  // 1%  Capacity
		Iphase3Tmp = Memo.Iphase3;
		Memo.VgazCell = 2492L - (((slong)Memo.BatTemp * 65L) / 10L) + (((slong)Memo.BatTemp * (slong)Memo.BatTemp * 6L) / 100L);
		// Init coef
		ChgData.CoefPhase3 = 62; // Coef = 1.062
		// Limits
		if (Memo.Iphase1 < 50)
			Memo.Iphase1 = 50;
		else if (Memo.Iphase1 > Menu.Imax)
			Memo.Iphase1 = Menu.Imax;
		if (Memo.VgazCell > 2563)
			Memo.VgazCell = 2563;
		else if (Memo.VgazCell < 2321)
			Memo.VgazCell = 2321;
		// Init time
		ChgData.TimeMaxProfile = ChgData.TimerSecCharge + (14L * 3600L);
		ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (6L * 3600L);	// 6h
		if (Menu.CECOnOff == 0)
			ChgData.TimeComp = 5L * 3600L;	  // 1st Compensation after 5h
		else
			ChgData.TimeComp = 0xFFFFFFFF;    // No Comp.

		if ((Menu.EqualPeriod != 0) && (Menu.CECOnOff == 0))    // Equal day
			ChgData.TimeEqual = 0xFFFFFFFF - 1L;
		else if (Menu.CECOnOff == 0)
			ChgData.TimeEqual = 8L * 3600L;  // Equalization after 08h if no day selected
		else
			ChgData.TimeEqual = 0xFFFFFFFF;   // No Equal

		// Init Loop
		DataW.CntLoopPhase = 0;
		ChgData.TimeLoopPhase = 0;
		FlagLoopPhase = OFF;
		// Init Flag
		FlagGelIPhase2 = OFF;
		FlagGelTimePhase3 = OFF;
		// Start regulation
		if ((Menu.CapAutoManu == 0) && (IQWIIQLink == 0))
			DataW.Ireq = 50;		// make a loop
		else
			DataW.Ireq = Memo.Iphase1;
		DataW.VreqCell = 2850;
		State.Charge = StateChgPhase1;
		TimeCheckVbat = ChgData.TimerSecCharge + TIME_CHECK_VBAT;
		break;

	case StateChgPhase1:          /* In phase 1 */
		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase1)
		{
			DFtimeSecuOn;
			State.Charge = StateChgStAvail;
		}
		else if (DataR.VbatCell >= Memo.VgazCell)
		{
			if (TimeCheckVbat < ChgData.TimerSecCharge)
			{
				State.Charge = StateChgStartPhase2;
				break;
			}
		}
		else if ((Menu.CapAutoManu == 0) && (ChgData.TimerSecCharge > ChgData.TimeLoopPhase)
				&& (DataW.CntLoopPhase < NB_LOOP) && (IQWIIQLink == 0))
		{
			if (FlagLoopPhase == OFF)
			{
				FlagLoopPhase = ON;
				DataW.VreqCell = 2850;
				State.Phase = StatePhLoopInit;
				FlagLoop = ON;
			}
			else if (FlagLoop == OFF)
			{
				FlagLoopPhase = OFF;
				DataW.CntLoopPhase++;
				// Capacity estimation
				// -> do in LoopPhase() for restart after the good current
				ChgData.TimeLoopPhase = ChgData.TimerSecCharge + TIME_LOOP;	// 3mn
				DataW.VreqCell = 2850;
				DataW.Ireq = Memo.Iphase1;
			}
		}
		else
		{
			TimeCheckVbat = ChgData.TimerSecCharge + 5L;
		}

		if ((ChgData.TimerSecCharge % 60) == 0) // Reminding time & Ah estimation
		{
			DataLcd.ChgSOC = Memo.InitSOC + (((ulong)Memo.ChgAh * 100L) / (ulong)Memo.BatCap); // %SoC = %InitSoC + %(Ah/Cap)
			if (DataLcd.ChgSOC > 80)
				DataLcd.ChgSOC = 80;
			DataLcd.ChgRestTime = ((8 * (100 - DataLcd.ChgSOC)) / 80) + 1; //+ 3; // RestTime = 8H * ((100 - %SoC) / 80%) + TmaxPh3

#ifdef LIFEIQ
			// Update Vgaz
			if (IQWIIQLink != 0)
			{
				Memo.VgazCell = 2492L - (((slong)Memo.EocTemp * 65L) / 10L) + (((slong)Memo.EocTemp * (slong)Memo.EocTemp * 6L) / 100L);
				if (Memo.VgazCell > 2563) // Tmin
					Memo.VgazCell = 2563;
				else if (Memo.VgazCell < 2321) // Tmax
					Memo.VgazCell = 2321;
			}
#endif
		}
		break;

	case StateChgStartPhase2:      /* Beginning of phase 2 */
		// Init Iphase3
		Memo.Iphase3 = (Memo.BatCap / 10) + 5;	    // 1%  Capacity
		if (Menu.FloatingOffOn == ON)
			Memo.Iphase3 += Menu.Ifloating;
		if (Memo.Iphase3 < 15) // 1.5A min
			Memo.Iphase3 = 15;
		else if (Memo.Iphase3 > Menu.Imax)
			Memo.Iphase3 = Menu.Imax;
		// Init time
		Iphase3Tmp = Memo.Iphase3;
		ChgData.TimeMaxPhase2 = ChgData.TimerSecCharge + (6L * 3600L);	// 6h
		// Init dIdT
		ChgData.TimedIdT = ChgData.TimerSecCharge + (15L * 60L);	// 15min
		ChgData.ThrlddIdT = Memo.Iphase1 / 35;
		// MODIF R3.4
		if (ChgData.ThrlddIdT < 50)
			ChgData.ThrlddIdT = 50;
		ChgData.IdIdTOld = Menu.Imax;
		ChgData.IdIdTNew = Menu.Imax;
		// Start regulation
		DataW.Ireq = Memo.Iphase1 - 1;  // Change Ireq to reset load sharing
		DataW.VreqCell = Memo.VgazCell;
		State.Charge = StateChgPhase2;
		TimeCheckCurrent = ChgData.TimerSecCharge + TIME_CHECK_CUR;
		break;

	case StateChgPhase2:           /* In phase 2 */
		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase2)
		{
			if ((DataR.Ibat > (Memo.BatCap / 10)) && (DataR.Ibat < ((Memo.BatCap * 2) / 10))) // 1% < Ibat < 2%Cap
			{
				FlagGelIPhase2 = ON;
				Memo.Iphase3 = DataR.Ibat;
				State.Charge = StateChgStartPhase3;
			}
			else
			{
				DFtimeSecuOn;
				State.Charge = StateChgStAvail;
			}
		}
		else if (DataR.Ibat < Memo.Iphase3)
		{
			if (ChgData.TimerSecCharge > TimeCheckCurrent)
				State.Charge = StateChgStartPhase3;
		}
		else if (ChgData.TimerSecCharge >= ChgData.TimedIdT)
		{
			ChgData.IdIdTNew = DataR.Ibat;
			if ((ChgData.IdIdTNew - ChgData.IdIdTOld) > ChgData.ThrlddIdT)
			{
				DFdIdTOn;
				State.Charge = StateChgStAvail;
			}
			else
			{
				if (ChgData.IdIdTNew < ChgData.IdIdTOld)
					ChgData.IdIdTOld = ChgData.IdIdTNew;
				ChgData.TimedIdT = ChgData.TimerSecCharge + (15L * 60L);	// 15min
			}
		}
		else
		{
			TimeCheckCurrent = ChgData.TimerSecCharge + TIME_CHECK_CUR;
		}

		if ((ChgData.TimerSecCharge % 60) == 0)
		{
			// Reminding time & Ah estimation
			DataLcd.ChgSOC = Memo.InitSOC + (((ulong)Memo.ChgAh * 100L) / (ulong)Memo.BatCap); // %SoC = %InitSoC + %(Ah/Cap)
			if (DataLcd.ChgSOC < 80)
				DataLcd.ChgSOC = 80;
			else if (DataLcd.ChgSOC > 98)
				DataLcd.ChgSOC = 98;
			DataLcd.ChgRestTime = ((8 * (100 - DataLcd.ChgSOC)) / 80) + 1; //+ 3; // RestTime = 8H * ((100 - %SoC) / 80%) + TmaxPh3

			// Update Vgaz
			/*if (IQWIIQLink != 0)
			{
				Memo.VgazCell = 2492L - (((slong)Memo.EocTemp * 65L) / 10L) + (((slong)Memo.EocTemp * (slong)Memo.EocTemp * 6L) / 100L);
				if (Memo.VgazCell > 2563)
					Memo.VgazCell = 2563;
				else if (Memo.VgazCell < 2321)
					Memo.VgazCell = 2321;
				DataW.VreqCell = Memo.VgazCell;
			}*/
		}
		break;

	case StateChgStartPhase3:     /* beginning of phase 3 */
		// Init time
		TimeInitVregPhase3 = ChgData.TimerSecCharge + TIME_VREQ_PH3;
		ChgData.TimeMaxPhase3 = ((ulong)DataR.Ah * (ulong)ChgData.CoefPhase3 * 36L) / (ulong)Memo.Iphase3; // Tph3 = ((DataR.Ah * Coef) / Iph3) * 3600
		if ((Memo.TimePhase1 + Memo.TimePhase2) < 15L)
		{
			ChgData.TimeMaxPhase3 = 0;
		}
		else if (FlagGelIPhase2 == ON)
		{
			ChgData.TimeMaxPhase3 = (2L * 3600L) + (30L * 60L);
		}
		else if (ChgData.TimeMaxPhase3 < 3600L)
		{
			ChgData.TimeMaxPhase3 = 3600L;
		}
		else if (ChgData.TimeMaxPhase3 > ((2L * 3600L) + (30L * 60L)))
		{
			FlagGelTimePhase3 = ON;
			ChgData.TimeMaxPhase3 = 3L * 3600L;
		}
		ChgData.TimeMaxPhase3 = ChgData.TimerSecCharge + ChgData.TimeMaxPhase3;
		// Init dVdT
		ChgData.TimedVdT = ChgData.TimerSecCharge + (30L * 60L);	// 30min
		ChgData.ThrlddVdT = 12;
		ChgData.VdVdTOld = 0; // No dV/dT the 1st time because of voltage decrease in start of phase 3
		ChgData.VdVdTNew = 0;
		// start regulation
		DataW.Ireq = Memo.Iphase3;
		State.Charge = StateChgPhase3;
		break;

	case StateChgPhase3:           /* In phase 3 */
		if (ChgData.TimerSecCharge > TimeInitVregPhase3)
		{ // Delay for no voltage peak when regulation goes to 2850mV/Cell
			DataW.VreqCell = VCELL_MAX - 50;
		}
		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase3)
		{
			State.Charge = StateChgStAvail;
		}
		else if (ChgData.TimerSecCharge >= ChgData.TimedVdT)
		{
			ChgData.VdVdTNew = DataR.VbatCell;
			//if ((ChgData.VdVdTNew - ChgData.VdVdTOld) < ChgData.ThrlddVdT)
            if (((ChgData.VdVdTNew - ChgData.VdVdTOld) > ChgData.ThrlddVdT) && (Memo.TimePhase3 >= ((2L * 60L) + 30L - 1L))
                && (Memo.TimePhase3 < ((2L * 60L) + 30L + 1L)))
			{
                // -1mn to be sure entering in "if" after 2H30
                // Stop charge if dVdt < 12mV and TimePhase3 = 02H30 else  TimePhase3 = 03H00
                ChgData.TimeMaxPhase3 = ChgData.TimerSecCharge + (30L * 60L);
			}
            else if ((ChgData.VdVdTNew - ChgData.VdVdTOld) < -12)	
		    {  
                DFdVdTOn;
			    State.Charge = StateChgStAvail;
			}
			/*else if ((FlagGelTimePhase3 == ON) && (Memo.TimePhase3 >= ((2L * 60L) + 30L - 1L)))
			{   // -1mn to be sure entering in "if" after 2H30
				if ((ChgData.VdVdTNew - ChgData.VdVdTOld) > 5)
				{
					FlagGelTimePhase3 = OFF;
				}
				else
				{
					State.Charge = StateChgStAvail;
				}
			}*/
			ChgData.VdVdTOld = ChgData.VdVdTNew;
			ChgData.TimedVdT = ChgData.TimerSecCharge + (30L * 60L);	// 30min
		}
		if ((ChgData.TimerSecCharge % 60) == 0) // Reminding time & Ah estimation
		{
			DataLcd.ChgSOC = 99;
			if (ChgData.TimeMaxPhase3 > ChgData.TimerSecCharge)
				DataLcd.ChgRestTime = ((ChgData.TimeMaxPhase3 - ChgData.TimerSecCharge) / 3600L) + 1;
			else
				DataLcd.ChgRestTime = 1;
		}
		break;

	case StateChgStAvail:	// Only one end point
		State.Charge = StateChgAvail;
		break;

	case StateChgAvail:
		break;

	case StateChgStartComp:
		// Init time
		ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (5L * 60L);	// 5mn
		ChgData.CntComp++;
		if (ChgData.CntComp == 1)
			ChgData.TimeComp = ChgData.TimerSecCharge + (10L * 3600L);	// 3h after 1st comp -> Equal 2h - then wait 5h before 2nd comp => 10h
		else if (ChgData.CntComp < 15)
			ChgData.TimeComp = ChgData.TimerSecCharge + (5L * 3600L) + (5L * 60L);	// Rest of 5h before next comp
		else
			ChgData.TimeComp = 0xFFFFFFFF;	// No other comp
		// Start regulation
		DataW.Ireq = Iphase3Tmp;	// 1% Capacity
		DataW.VreqCell = 2850;
		State.Charge = StateChgComp;
		break;

	case StateChgComp:
		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase1)
		{
			State.Charge = StateChgStAvail;
		}
		break;

	case StateChgStartEqual:
#ifdef LIFEIQ
		if (FlagIQEqual == ON)  // WiIQ Equal
		{
			ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (2L * 3600L); // Equal time (2h)
		}
#endif
		else if (Menu.EqualPeriod == 0)
			ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (2L * 3600L); // Force auto equal time (2h)

		Memo.Iphase1 = Iphase3Tmp;	// 1% Capacity
		if (Memo.Iphase1 < 20)
			Memo.Iphase1 = 20;

		// Start regulation
		DataW.Ireq = Memo.Iphase1;
		DataW.VreqCell = 2850;
		State.Charge = StateChgEqual;
		TimeMaxEqual = ChgData.TimeMaxPhase1;
		break;

	case StateChgEqual:
		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase1)
		{
#ifdef LIFEIQ
			if ((IQWIIQLink != 0) && (FlagIQEqual == ON))
			{
				ResetFlagEqualOn;
			}
			FlagIQEqual = OFF;
#endif
			State.Charge = StateChgStAvail;
		}
		/*else if (ChgData.TimerSecCharge >= ChgData.TimedVdT)
			{
				ChgData.VdVdTNew = DataR.VbatCell;
				if ((ChgData.VdVdTNew - ChgData.VdVdTOld) < ChgData.ThrlddVdT)
				{
					DFdVdTOn;
					State.Charge = StateChgStAvail;
				}
				ChgData.VdVdTOld = ChgData.VdVdTNew;
				ChgData.TimedVdT = ChgData.TimerSecCharge + (15L * 60L);	// 15min
			}*/
		break;
	}
}
