/*
 * profil_cold.c
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
extern uchar FlagLoop;
extern uchar FlagLoopPhase;
extern ulong TimeInitVregPhase3;
extern ulong TimeCheckVbat;
extern uchar FlagDesul;
extern uint Iphase1Tmp;
extern uint Iphase3Tmp;
extern uint TempVinit;
extern ulong VbatModCell;
extern ulong TimePhase3;

/************************************************************************************
Name:        Profile COLD
 ************************************************************************************/
void  ProfileCOLD_EU(void)
{

	if (FlagDesul == ON)
	{
		ProfileDESUL();
		return;
	}

	if (Menu.LoopOnOff == 1)
	{
		ProfileCOLD_loop_EU();
		return;
	}

	switch (State.Charge)     /* Select current phase */
	{
	case StateChgStartPhase1:     /* Just at beginning of phase 1 */
		SlopeI = 2000;              // 20A/s

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
   		Memo.Iphase1 = (Memo.BatCap * 25L) / 10L; // 25% of capacity
   		// MODIF COLD
        //Memo.VgazCell = 2492L - (((slong)Memo.BatTemp * 65L) / 10L) + (((slong)Memo.BatTemp * (slong)Memo.BatTemp * 6L) / 100L);
        if ((slong)Memo.BatTemp < -10)
            Memo.VgazCell = 2560;
        else    
            Memo.VgazCell = 2400L - (((slong)Memo.BatTemp - 30) * 4);
            
   		// CEC
   		if (Menu.CECOnOff == 1)
   			ChgData.CoefPhase3 = 80;  // 1.08 with CEC On
   		else
   			ChgData.CoefPhase3 = (ulong)Menu.IonicCoef * 10L;

   		// Limits
   		if (Memo.Iphase1 < 50)
   			Memo.Iphase1 = 50;
   		else if (Memo.Iphase1 > Menu.Imax)
   			Memo.Iphase1 = Menu.Imax;
   		if (Memo.Iphase3 < 10)	// Imin = 1A
   			Memo.Iphase3 = 10;
   		else if (Memo.Iphase3 > Menu.Imax)
   			Memo.Iphase3 = Menu.Imax;
   		if (Memo.VgazCell > 2563)
   			Memo.VgazCell = 2563;
   		else if (Memo.VgazCell < 2321)
   			Memo.VgazCell = 2321;
   		// Init time
   		ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (6L * 3600L);
   		ChgData.TimeMaxProfile = ChgData.TimerSecCharge + (13L * 3600L);
				
   		// Equal
   		if ((Menu.EqualPeriod != 0) && (Menu.CECOnOff == 0))     // Equal day
   		{
   			ChgData.TimeEqual = 0xFFFFFFFF - 1L;
   		}
   		else
   			ChgData.TimeEqual = 0xFFFFFFFF;   // No Equal


   		// Menu Refresh ON ?
   		if ((Menu.RefreshOffOn != 0) && (Menu.CECOnOff == 0))
   			ChgData.TimeComp = (4L * 60L) + 25L;	  // 1st Compensation after 4mn25
   		else
   			ChgData.TimeComp = 0xFFFFFFFF;	        // No Comp

   		// Init Loop
   		DataW.CntLoopPhase = 0;
   		ChgData.TimeLoopPhase = 0;
   		FlagLoopPhase = OFF;
        // Start regulation
   		ChgData.ThrlddVdT = -10;
   		ChgData.VdVdTOld = 0;
   		ChgData.VdVdTNew = 0;
   		ChgData.TimedVdT = ChgData.TimerSecCharge + (15L * 60L);
   		Iphase1Tmp = Memo.Iphase1;                // Save Iphase 1 for COMP and EQUAL
   		if ((Menu.CapAutoManu == 0) && (IQWIIQLink == 0))
   			DataW.Ireq = 50;		// make a loop
   		else
   			DataW.Ireq = Memo.Iphase1;
   		DataW.VreqCell = VCELL_MAX - 50;
   		State.Charge = StateChgPhase1;
   		TimeCheckVbat = ChgData.TimerSecCharge + TIME_CHECK_VBAT;
   		Memo.Profile = ChgData.Profile;       // Force to COLD for MEMO
	break;

	case StateChgPhase1:          /* In phase 1 */
		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase1)
		{
			DFtimeSecuOn;
			State.Charge = StateChgStAvail;
		}
		else if (DataR.Ah >= Memo.Iphase1)    // Cap max = IPhase1 * 10
		{
			DFotherBatOn;
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
				Iphase1Tmp = Memo.Iphase1;                // Save Iphase 1 for COMP and EQUAL
			}
		}
		else
		{
			TimeCheckVbat = ChgData.TimerSecCharge + 5L;
		}

		if (ChgData.TimerSecCharge >= ChgData.TimedVdT)
		{
			ChgData.VdVdTNew = DataR.VbatCell;
			if ((ChgData.VdVdTNew - ChgData.VdVdTOld) < ChgData.ThrlddVdT)
			{
				DFdVdTOn;
				State.Charge = StateChgStAvail;
				break;
			}
			else if (ChgData.VdVdTNew > ChgData.VdVdTOld)
			{
				ChgData.VdVdTOld = ChgData.VdVdTNew;
			}
			ChgData.TimedVdT = ChgData.TimerSecCharge + (15L * 60L);
		}

		if ((ChgData.TimerSecCharge % 60) == 0) // Reminding time & Ah estimation
		{
			DataLcd.ChgSOC = Memo.InitSOC + (((ulong)Memo.ChgAh * 100L) / (ulong)Memo.BatCap); // %SoC = %InitSoC + %(Ah/Cap)
			if (DataLcd.ChgSOC > 80)
				DataLcd.ChgSOC = 80;
			DataLcd.ChgRestTime = ((8 * (100 - DataLcd.ChgSOC)) / 80) + 1; //+ 4; // RestTime = 8H * ((100 - %SoC) / 80%) + TmaxPh3

		}
		break;

	case StateChgStartPhase2:      /* Beginning of phase 2 */
		Iphase1Tmp = Memo.Iphase1;  // Save Iphase 1 for COMP and EQUAL
		// Init Iphase3
		/*if (Memo.BatTemp < 5)
			Memo.Iphase3 = (Memo.BatCap * 10) / 27;
		else if (Memo.BatTemp < 11)
			Memo.Iphase3 = (Memo.BatCap * 10) / 26;
		else if (Memo.BatTemp < 16)
			Memo.Iphase3 = (Memo.BatCap * 10) / 25;
		else*/
			Memo.Iphase3 = (Memo.BatCap * 3) / 10;
        
        if (Menu.FloatingOffOn == ON)
			Memo.Iphase3 += Menu.Ifloating;
		Iphase3Tmp = Memo.Iphase3;
        
		// Init time
		ChgData.TimeMaxPhase2 = ChgData.TimerSecCharge + (6L * 3600L);	// 6h
		// Init dIdT
		ChgData.TimedIdT = ChgData.TimerSecCharge + (3L * 60L);	        // 3min
		ChgData.ThrlddIdT = 100;                                        // 10 A
		ChgData.IdIdTOld = Menu.Imax;
		ChgData.IdIdTNew = Menu.Imax;
		// Start regulation
		DataW.Ireq = Memo.Iphase1 - 1;  // Change Ireq to reset load sharing
		DataW.VreqCell = Memo.VgazCell;
		State.Charge = StateChgPhase2;
		TimeCheckCurrent = ChgData.TimerSecCharge + TIME_CHECK_CUR;
		ChgData.TimeLoopPhase = ChgData.TimerSecCharge + 1540L;	        // 25mn40
		break;

	case StateChgPhase2:           /* In phase 2 */
		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase2)
		{
			DFtimeSecuOn;
			State.Charge = StateChgStAvail;
		}
		else if (DataR.Ah >= Memo.Iphase1)    // Cap max = IPhase1 * 10
		{
			DFotherBatOn;
			State.Charge = StateChgStAvail;
		}

		if (DataR.Ibat <= Memo.Iphase3)
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
				ChgData.TimedIdT = ChgData.TimerSecCharge + (3L * 60L);	  // 3min
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
			DataLcd.ChgRestTime = ((8 * (100 - DataLcd.ChgSOC)) / 80) + 1; //+ 4; // RestTime = 8H * ((100 - %SoC) / 80%) + TmaxPh3
		}
		break;

	case StateChgStartPhase3:     /* beginning of phase 3 */
		// Init time
		TimeInitVregPhase3 = ChgData.TimerSecCharge + TIME_VREQ_PH3;
		//ChgData.TimeMaxPhase3 = (uint)(((ulong)DataR.Ah * (ulong)ChgData.CoefPhase3 * 36L) / (ulong)Memo.Iphase3); // Tph3 = ((DataR.Ah * Coef) / Iph3) * 3600
		ChgData.TimeMaxPhase3 = (3600L * 3L); 
		ChgData.TimeMaxPhase3 = ChgData.TimerSecCharge + ChgData.TimeMaxPhase3;
		TimePhase3 = (3600L * 1) + (30 * 60L);
		TimePhase3 += ChgData.TimerSecCharge;
		// Init dVdT
        ChgData.ThrlddVdT = 2;   	// 2mV/Cells
		ChgData.VdVdTOld = 0;     	// No dV/dT the 1st time because of voltage decrease in start of phase 3
		ChgData.VdVdTNew = 0;
		ChgData.TimedVdT = ChgData.TimerSecCharge + (1L * 60L);
		// Start regulation
		DataW.Ireq = Memo.Iphase3;
		State.Charge = StateChgPhase3;
		break;

	case StateChgPhase3:           /* In phase 3 */
		if (ChgData.TimerSecCharge > TimeInitVregPhase3)
		{ // Delay for no voltage peak when regulation goes to 2850mV/Cell
			DataW.VreqCell = VCELL_MAX;
		}

		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase3)
		{
			State.Charge = StateChgStAvail;
		}
		else if (DataR.Ah > ((Memo.BatCap * 13) / 10))
		{
			DFtimeSecuOn;
			State.Charge = StateChgStAvail;
			break;
		}
		else if ((ChgData.TimerSecCharge > TimePhase3) && (DFdVdT!= 0))
        {
            State.Charge = StateChgStAvail;
        }

		if (ChgData.TimerSecCharge > ChgData.TimedVdT)
		{
			ChgData.VdVdTNew = DataR.VbatCell;
			if ((ChgData.VdVdTNew - ChgData.VdVdTOld) <= ChgData.ThrlddVdT)
			{
				DFdVdTOn;
				//State.Charge = StateChgStAvail;
				break;
			}
			else
			{
				if (ChgData.VdVdTNew > ChgData.VdVdTOld)
				{
					ChgData.VdVdTOld = ChgData.VdVdTNew;
				}
			}
			ChgData.TimedVdT = ChgData.TimerSecCharge + (5L * 60L);
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
		ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + 10L;
		ChgData.CntComp++;
		ChgData.TimeComp = ChgData.TimerSecCharge + (10L * 60L) + 10L;	// Next refresh after 10mn
		// Start regulation
		DataW.Ireq = Iphase1Tmp;
		DataW.VreqCell = 2800;
		State.Charge = StateChgComp;
		break;

	case StateChgComp:
		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase1)
		{
			State.Charge = StateChgStAvail;
		}
		break;

	case StateChgStartEqual:
		ChgData.ThrlddVdT = -30;
		ChgData.VdVdTOld = 0;
		ChgData.VdVdTNew = 0;
		ChgData.TimedVdT = ChgData.TimerSecCharge + (15L * 60L);

		Memo.Iphase1 = Iphase3Tmp;
		DataW.Ireq = Memo.Iphase1;
		DataW.VreqCell = 2800;
		TimeMaxEqual = ChgData.TimeMaxPhase1;
		State.Charge = StateChgEqual;
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
}
