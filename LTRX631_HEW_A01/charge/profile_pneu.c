/*
 * profil_pneu.c
 *
 *  Created on: 01/09/2013
 *      Author: Francis DUHAUT
 */

#include "include.h"

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
extern uchar FlagDesul;

/************************************************************************************
Name:        Profile PNEU
 ************************************************************************************/
void  ProfilePNEU(void)
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
		Memo.Iphase1 = (Memo.BatCap * 22) / 10;	// 22% Capacity
		Memo.VgazCell = 2505L - (((slong)Memo.BatTemp * 35L) / 10L);

		// Init coef
		ChgData.CoefPhase3 = 70; // Coef = 1.07

		// Limits
		if (Memo.Iphase1 < 50)
			Memo.Iphase1 = 50;
		else if (Memo.Iphase1 > Menu.Imax)
			Memo.Iphase1 = Menu.Imax;
		if (Memo.VgazCell > 2540)
			Memo.VgazCell = 2540;
		else if (Memo.VgazCell < 2278)
			Memo.VgazCell = 2278;
		// Init time
        // MODIF S2.1
		ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (7L * 3600L);
        //ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (14L * 3600L);
		ChgData.TimeMaxPhase2 = ChgData.TimerSecCharge + (14L * 3600L);
		ChgData.TimeMaxProfile = ChgData.TimerSecCharge + (14L * 3600L);

		// Menu Refresh ON ?
		if ((Menu.RefreshOffOn != 0) && (Menu.CECOnOff == 0))
			ChgData.TimeComp = 25L * 3600L;	  // 1st Compensation after 25h
		else
			ChgData.TimeComp = 0xFFFFFFFF;	  // No Comp

		// Menu Profile Equal ON ?
		if ((Menu.EqualPeriod != 0) && (Menu.CECOnOff == 0))
			ChgData.TimeEqual = 0xFFFFFFFF - 1L;
		else if (Menu.CECOnOff == 0)
			ChgData.TimeEqual = 12L * 3600L;  // Equalization after 12h if no day selected
		else
			ChgData.TimeEqual = 0xFFFFFFFF;   // No Equal

		// Init Loop
		DataW.CntLoopPhase = 0;
		ChgData.TimeLoopPhase = 0;
		FlagLoopPhase = OFF;
		// Start regulation
		UsePumpOn;
		if ((Menu.CapAutoManu == 0) && (IQWIIQLink == 0))
			DataW.Ireq = 50;		// make a loop
		else
			DataW.Ireq = Memo.Iphase1;
		DataW.VreqCell = 2850;
		State.Charge = StateChgPhase1;
		TimeCheckVbat = ChgData.TimerSecCharge + TIME_CHECK_VBAT;
		break;

	case StateChgPhase1:          /* In phase 1 */
		// MODIF S2.1
        if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase1)
        //if (DataR.Ah > Memo.BatCap)
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

		if ((ChgData.TimerSecCharge % 60) == 0)
		{
			// Reminding time & Ah estimation
			DataLcd.ChgSOC = Memo.InitSOC + (((ulong)Memo.ChgAh * 100L) / (ulong)Memo.BatCap); // %SoC = %InitSoC + %(Ah/Cap)
			if (DataLcd.ChgSOC > 80)
				DataLcd.ChgSOC = 80;
			DataLcd.ChgRestTime = ((8 * (100 - DataLcd.ChgSOC)) / 80) + 1; //+ 4; // RestTime = 8H * ((100 - %SoC) / 80%) + TmaxPh3

#ifdef LIFEIQ
			// Update Vgaz
			if (IQWIIQLink != 0)
			{
				Memo.VgazCell = 2505L - (((slong)Memo.EocTemp * 35L) / 10L);
				if (Memo.VgazCell > 2540) // Tmin = -10�C
					Memo.VgazCell = 2540;
				else if (Memo.VgazCell < 2278) // Tmax = 65�C
					Memo.VgazCell = 2278;
			}
#endif

		}
		break;

	case StateChgStartPhase2:      /* Beginning of phase 2 */
		// Init Iphase3
		Memo.Iphase3 = (Memo.BatCap * 5) / 10;	// 5% Capacity
		if (Menu.FloatingOffOn == ON)
			Memo.Iphase3 += Menu.Ifloating;
		if (Memo.Iphase3 < 10)	// Imin = 1A
			Memo.Iphase3 = 10;
		else if (Memo.Iphase3 > Menu.Imax)
			Memo.Iphase3 = Menu.Imax;
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
			DFtimeSecuOn;
			State.Charge = StateChgStAvail;
		}
		else if (DataR.Ibat <= Memo.Iphase3)
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

		if ((ChgData.TimerSecCharge % 60) == 0) // Reminding time & Ah estimation
		{
			DataLcd.ChgSOC = Memo.InitSOC + (((ulong)Memo.ChgAh * 100L) / (ulong)Memo.BatCap); // %SoC = %InitSoC + %(Ah/Cap)
			if (DataLcd.ChgSOC < 80)
				DataLcd.ChgSOC = 80;
			else if (DataLcd.ChgSOC > 98)
				DataLcd.ChgSOC = 98;
			DataLcd.ChgRestTime = ((8 * (100 - DataLcd.ChgSOC)) / 80) + 1; //+ 4; // RestTime = 8H * ((100 - %SoC) / 80%) + TmaxPh3
		}
		break;

	case StateChgStartPhase3:     /* beginning of phase 3 */
		TimeInitVregPhase3 = ChgData.TimerSecCharge + TIME_VREQ_PH3;
		// Add 5% if InitSOC < 60%
		if (Memo.InitSOC < 60)
			ChgData.CoefPhase3 += 5;
		// Init time
		ChgData.TimeMaxPhase3 = (uint)(((ulong)DataR.Ah * (ulong)ChgData.CoefPhase3 * 36L) / (ulong)Memo.Iphase3); // Tph3 = ((DataR.Ah * Coef) / Iph3) * 3600
		if (ChgData.TimeMaxPhase3 < (24L * 60L)) // 24mn min
			ChgData.TimeMaxPhase3 = 24L * 60L;
		else if (ChgData.TimeMaxPhase3 > (4L * 3600L)) // 4h max
			ChgData.TimeMaxPhase3 = 4L * 3600L;
		ChgData.TimeMaxPhase3 = ChgData.TimerSecCharge + (ulong)ChgData.TimeMaxPhase3;
		// Init dVdT
		ChgData.TimedVdT = ChgData.TimerSecCharge + (15L * 60L);
		ChgData.ThrlddVdT = -1;
		ChgData.VdVdTOld = 0; // No dV/dT the 1st time because of voltage decrease in start of phase 3
		ChgData.VdVdTNew = 0;
		// Start regulation
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
		else if (ChgData.TimerSecCharge > ChgData.TimedVdT)
		{
			ChgData.VdVdTNew = DataR.VbatCell;
			if ((ChgData.VdVdTNew - ChgData.VdVdTOld) < ChgData.ThrlddVdT)
			{
				DFdVdTOn;
				State.Charge = StateChgStAvail;
			}
			else
			{
				ChgData.VdVdTOld = ChgData.VdVdTNew;
				ChgData.TimedVdT = ChgData.TimerSecCharge + (15L * 60L);
			}
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
		UsePumpOff;
		State.Charge = StateChgAvail;
		break;

	case StateChgAvail:
		break;

	case StateChgStartComp:
		// Init time
		ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (1L * 3600L);	// 1h
		ChgData.CntComp++;
		ChgData.TimeComp = ChgData.TimerSecCharge + (8L * 3600L);	// Rest of 7h before next comp
		// Start regulation
		DataW.Ireq = (Memo.BatCap * 2) / 10;	// 2% Capacity
		DataW.VreqCell = 2350;
		State.Charge = StateChgComp;
		break;

	case StateChgComp:
		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase1)
		{
			State.Charge = StateChgStAvail;
		}
		break;

	case StateChgStartEqual:
		Memo.Iphase1 = (Memo.BatCap * 2) / 10;  // 2% Capacity
		//ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (6L * 3600L); // 6h
		if (Memo.Iphase1 < 30)
			Memo.Iphase1 = 30;
		else if (Memo.Iphase1 > 300)
			Memo.Iphase1 = 300;
		
		ChgData.ThrlddVdT = -10;
		ChgData.VdVdTOld = 0;
		ChgData.VdVdTNew = 0;
		ChgData.TimedVdT = ChgData.TimerSecCharge + (15L * 60L);

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
		else if (ChgData.TimerSecCharge >= ChgData.TimedVdT)
		{
			ChgData.VdVdTNew = DataR.VbatCell;
			if ((ChgData.VdVdTNew - ChgData.VdVdTOld) < ChgData.ThrlddVdT)
			{
				DFdVdTOn;

#ifdef LIFEIQ
				if ((IQWIIQLink != 0) && (FlagIQEqual == ON))
				{
					ResetFlagEqualOn;
				}
				FlagIQEqual = OFF;
#endif

				State.Charge = StateChgStAvail;
			}
			else if (ChgData.VdVdTNew > ChgData.VdVdTOld)
			{
				ChgData.VdVdTOld = ChgData.VdVdTNew;
			}
			ChgData.TimedVdT = ChgData.TimerSecCharge + (15L * 60L);	// 15min
		}
		break;
	}
	if (PumpDef != 0)
	{
		UsePumpOff;
		DFpumpOn;
		ChgData.Profile = IONIC;
		Memo.Profile = ChgData.Profile;
		State.Charge = StateChgStartPhase1;
		Display.ForceReloadHeader = 1;
	}
}
