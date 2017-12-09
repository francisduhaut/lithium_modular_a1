/*
 * profil_rgt.c
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
Name:        Profile RGT
 ************************************************************************************/
void  ProfileRGT(void)
{
	FlagDesul = OFF;

	switch (State.Charge)     /* Select current phase */
	{
	case StateChgStartPhase1:     /* Just at beginning of phase 1 */
		SlopeI = 2000;              // 20A/s

		// Init current-voltage
		ChargerBatCap = (ulong)(Menu.Imax * BATCAP_C5) / 1000L;
		if ((Menu.CapAutoManu == 0) && (IQWIIQLink == 0))
			Memo.BatCap = ChargerBatCap;
		Memo.Iphase1 = Memo.BatCap * 2;	// 20% Capacity
		Memo.VgazCell = 2437L - (((slong)Memo.BatTemp * 35L) / 10L);
		// Limits
		if (Memo.Iphase1 < 50)
			Memo.Iphase1 = 50;
		else if (Memo.Iphase1 > Menu.Imax)
			Memo.Iphase1 = Menu.Imax;
		if (Memo.VgazCell > 2472)
			Memo.VgazCell = 2472;
		else if (Memo.VgazCell < 2280)
			Memo.VgazCell = 2280;
		// Init time
		ChgData.TimeMaxProfile = ChgData.TimerSecCharge + (14L * 3600L);
		ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (6L * 3600L);	// 6h
		ChgData.TimeComp = 0xFFFFFFFF;	// No Comp
		ChgData.TimeEqual = 0xFFFFFFFF; // No Equal
		// Init Loop
		DataW.CntLoopPhase = 0;
		ChgData.TimeLoopPhase = 0;
		FlagLoopPhase = OFF;
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
				// only 1 loop for capacity
				DataW.Ireq = Memo.Iphase1;
			}
		}
		else
		{
			TimeCheckVbat = ChgData.TimerSecCharge + TIME_CHECK_VBAT;
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
				Memo.VgazCell = 2437L - (((slong)Memo.BatTemp * 35L) / 10L);
				if (Memo.VgazCell > 2472) // Tmin = -10�C
					Memo.VgazCell = 2472;
				else if (Memo.VgazCell < 2280) // Tmax = 45�C
					Memo.VgazCell = 2280;
			}
#endif
		}
		break;

	case StateChgStartPhase2:      /* Beginning of phase 2 */
		// Init Iphase3
		Memo.Iphase3 = (((ulong)Memo.BatCap * 22L) / 100L);	// 2.2% Capacity
		if (Menu.FloatingOffOn == ON)
			Memo.Iphase3 += Menu.Ifloating;
		if (Memo.Iphase3 < 15) // 1.5A min
			Memo.Iphase3 = 15;
		else if (Memo.Iphase3 > Menu.Imax)
			Memo.Iphase3 = Menu.Imax;
		// Init time
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
			DFtimeSecuOn;
			State.Charge = StateChgStAvail;
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

#ifdef LIFEIQ
			// Update Vgaz
			if (IQWIIQLink != 0)
			{
				Memo.VgazCell = 2437L - (((slong)Memo.BatTemp * 35L) / 10L);
				if (Memo.VgazCell > 2472) // Tmin = -10�C
					Memo.VgazCell = 2472;
				else if (Memo.VgazCell < 2280) // Tmax = 45�C
					Memo.VgazCell = 2280;
				DataW.VreqCell = Memo.VgazCell;
			}
#endif

		}
		break;

	case StateChgStartPhase3:     /* beginning of phase 3 */
		// Init time
		TimeInitVregPhase3 = ChgData.TimerSecCharge + TIME_VREQ_PH3;
		ChgData.TimeMaxPhase3 = (((Memo.TimePhase1 + Memo.TimePhase2) / 7L) * 60L) + (2L * 3600L); // Tph3 = ((Tph1 + Tph2) / 7) + 2H
		if ((Memo.TimePhase1 + Memo.TimePhase2) < 15L)
		{
			ChgData.TimeMaxPhase3 = 0;
		}
		else if (ChgData.TimeMaxPhase3 > (3L * 3600L))
		{
			ChgData.TimeMaxPhase3 = 3L * 3600L;
		}
		ChgData.TimeMaxPhase3 = ChgData.TimerSecCharge + ChgData.TimeMaxPhase3;
		// Init dVdT
		ChgData.TimedVdT = ChgData.TimerSecCharge + (30L * 60L);	// 30min
		ChgData.ThrlddVdT = -1;
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
			if ((ChgData.VdVdTNew - ChgData.VdVdTOld) < ChgData.ThrlddVdT)
			{
				DFdVdTOn;
				State.Charge = StateChgStAvail;
			}
			ChgData.VdVdTOld = ChgData.VdVdTNew;
			ChgData.TimedVdT = ChgData.TimerSecCharge + (30L * 60L);
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
	}
}
