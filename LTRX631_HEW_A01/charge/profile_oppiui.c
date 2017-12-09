/*
 * profil_OPPIUI.c
 *
 *  Created on: 01/04/2014
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
extern uint Iphase1Tmp;
extern uint Iphase3Tmp;
extern ulong TimeInitVregPhase3;
extern uchar FlagDesul;
extern ulong OPPAh;
extern uint TempVinit;
extern ulong VbatModCell;

void  ProfileOPPIUI(void)
{
	uint VGazOld;

	if (FlagDesul == ON)
	{
		ProfileDESUL();
		return;
	}


	switch (State.Charge) 
	{
	case StateChgStartPhase1:     /* Just at beginning of phase 1 */
		    SlopeI = 2000; 
            // Init current-voltage
    		Memo.CapAutoManu = 1;   // to force display menu capacity
    		Memo.Iphase1 = (Memo.BatCap * 25) / 10;	// 25% Capacity
    		Memo.Iphase3 = ((ulong)Memo.BatCap * 50L) / 100L;	// 5% Capacity
    		Iphase3Tmp =  Memo.Iphase3;
            Memo.VgazCell = 2508L - (((slong)Memo.BatTemp * 35L) / 10L);
            
    		// Init coef
    		ChgData.CoefPhase3 = (ulong)Menu.IonicCoef * 10L; 
    		// Limits
    		if (Memo.Iphase1 < 50)
    			Memo.Iphase1 = 50;
    		else if (Memo.Iphase1 > Menu.Imax)
    			Memo.Iphase1 = Menu.Imax;
    		if (Memo.VgazCell > 2550)
    			Memo.VgazCell = 2550;
    		else if (Memo.VgazCell < 2321)
    			Memo.VgazCell = 2321;

    		// Init time
    		ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (6L * 3600L);
    		ChgData.TimeMaxPhase2 = ChgData.TimerSecCharge + (6L * 3600L);
    		ChgData.TimeMaxProfile = ChgData.TimerSecCharge + (11L * 3600L);

    		if ((Menu.EqualPeriod != 0) && (Menu.CECOnOff == 0)) // Equal day
    			ChgData.TimeEqual = 0xFFFFFFFF - 1L;
    		else
    			ChgData.TimeEqual = 0xFFFFFFFF;   // No Equal

    		// Refresh
    		ChgData.TimeComp = 0xFFFFFFFF;
    		// dV/dt
    		ChgData.ThrlddVdT = -10;
    		ChgData.VdVdTOld = 0;
    		ChgData.VdVdTNew = 0;
    		ChgData.TimedVdT = ChgData.TimerSecCharge + (30L * 60L);

    		// Init Loop
    		DataW.CntLoopPhase = 0;
    		ChgData.TimeLoopPhase = ChgData.TimerSecCharge + (15L * 60L);
    		FlagLoopPhase = OFF;
    		DataW.CntLoopPhase = NB_LOOP;

    		// Start regulation
            TimeCheckCurrent = 0;
  		    TimeCheckVbat = ChgData.TimerSecCharge + TIME_CHECK_VBAT;
   		    DataW.Ireq = Memo.Iphase1;
   		    DataW.VreqCell = 2850;
   		    State.Charge = StateChgPhase1;
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
		else if (ChgData.TimerSecCharge > ChgData.TimeLoopPhase)
		{
			if (FlagLoopPhase == OFF)
			{
				// dVdT : wait 30mn for first memorisation. Analyze every loop (15mn)
				ChgData.VdVdTNew = DataR.VbatCell;
				if (((ChgData.VdVdTNew - ChgData.VdVdTOld) < ChgData.ThrlddVdT) && (ChgData.TimedVdT < ChgData.TimerSecCharge))
				{
					DFdVdTOn;
					DFDisDF5On;
					State.Charge = StateChgStartPhase2;
				}
				else
				{
					if ((ChgData.VdVdTNew > ChgData.VdVdTOld) && (ChgData.TimedVdT < ChgData.TimerSecCharge))
						ChgData.VdVdTOld = ChgData.VdVdTNew;
					// Loop
					FlagLoopPhase = ON;
					DataW.VreqCell = VCELL_MAX - 50;
					State.Phase = StatePhLoopInit;
					FlagLoop = ON;
				}
			}
			else if (FlagLoop == OFF)
			{
				FlagLoopPhase = OFF;
				DataW.CntLoopPhase++;
				ChgData.TimeLoopPhase = ChgData.TimerSecCharge + (15L * 60L);
				DataW.VreqCell = VCELL_MAX - 50;
				DataW.Ireq = Memo.Iphase1;
			}
		}
		else
		{
			TimeCheckVbat = ChgData.TimerSecCharge + TIME_CHECK_VBAT;
		}

		if ((ChgData.TimerSecCharge % 60) == 0)
		{
			DataLcd.ChgSOC = Memo.InitSOC + (((ulong)Memo.ChgAh * 100L) / (ulong)Memo.BatCap);
			if (DataLcd.ChgSOC > 80)
			{
				DataLcd.ChgSOC = 80;
			}
			DataLcd.ChgRestTime = ((8 * (100 - DataLcd.ChgSOC)) / 80) + 1;
			if (IQWIIQLink != 0)
			{
                Memo.VgazCell = 2508L - (((slong)Memo.EocTemp * 35L) / 10L);
				if (Memo.VgazCell > 2550)
				{
					Memo.VgazCell = 2550;
				}
				else if (Memo.VgazCell < 2321)
				{
					Memo.VgazCell = 2321;
				}
			}
		}
		break;

	case StateChgStartPhase2:      /* Beginning of phase 2 */
		Iphase1Tmp = Memo.Iphase1;
		ChgData.TimeMaxPhase2 = (ulong)ChgData.TimerSecCharge + (ulong)ChgData.TimeMaxPhase2;
		// Init Iphase3
		Memo.Iphase3 = ((ulong)Memo.BatCap * 50L) / 100L;	  // 5% Capacity
		Iphase3Tmp =  Memo.Iphase3;
		if (Menu.FloatingOffOn == ON)
			Memo.Iphase3 += Menu.Ifloating;
		if (Memo.Iphase3 < 10)	// Imin = 1A
			Memo.Iphase3 = 10;
		else if (Memo.Iphase3 > Menu.Imax)
			Memo.Iphase3 = Menu.Imax;
		// Init dIdT
		ChgData.TimedIdT = ChgData.TimerSecCharge + (15L * 60L);	// 15min
        ChgData.ThrlddIdT = 100;    // 10A      MODIF R2.2
		ChgData.IdIdTOld = Memo.Iphase1;
		ChgData.IdIdTNew = Memo.Iphase1;
		// Start regulation
		DataW.Ireq = Memo.Iphase1 - 1;  // Change Ireq to reset load sharing
		if (DFdVdT != 0)
			DataW.VreqCell = ChgData.VdVdTOld;
		else
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
		else if (ChgData.TimerSecCharge > ChgData.TimeLoopPhase)
		{
			if (FlagLoopPhase == OFF)
			{
				FlagLoopPhase = ON;
				DataW.VreqCell = VCELL_MAX - 50;
				State.Phase = StatePhLoopInit;
				FlagLoop = ON;
			}
			else if (FlagLoop == OFF)
			{
				FlagLoopPhase = OFF;
				DataW.CntLoopPhase++;
				// Capacity estimation
				// -> do in LoopPhase() for restart after the good current
				ChgData.TimeLoopPhase = ChgData.TimerSecCharge + (15L * 60L);	    // next loop 15mn
				ChgData.TimedIdT = ChgData.TimerSecCharge + (3L * 60L);	          // next didt  3min
				TimeCheckCurrent = ChgData.TimerSecCharge + 20L;
				DataW.Ireq = Memo.Iphase1;
				DataW.VreqCell = Memo.VgazCell;
			}
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
		if ((ChgData.TimerSecCharge % 60) == 0)
		{
			DataLcd.ChgSOC = Memo.InitSOC + (((ulong)Memo.ChgAh * 100L) / (ulong)Memo.BatCap);
			if (DataLcd.ChgSOC > 80)
			{
				DataLcd.ChgSOC = 80;
			}
			DataLcd.ChgRestTime = ((8 * (100 - DataLcd.ChgSOC)) / 80) + 1;
			if (IQWIIQLink != 0)
			{
				VGazOld = Memo.VgazCell;
                Memo.VgazCell = 2508L - (((slong)Memo.EocTemp * 35L) / 10L);
				
				if (Memo.VgazCell > 2550)
				{
					Memo.VgazCell = 2550;
				}
				else if (Memo.VgazCell < 2321)
				{
					Memo.VgazCell = 2321;
				}
				if (VGazOld < Memo.VgazCell)
					Memo.VgazCell = VGazOld;
				DataW.VreqCell = Memo.VgazCell;
			}
		}
		break;

	case StateChgStartPhase3:     /* beginning of phase 3 */
		DataW.CntLoopPhase = NB_LOOP;
		FlagLoopPhase = OFF;
		TimeInitVregPhase3 = ChgData.TimerSecCharge + TIME_VREQ_PH3;
		// Init time
		//ChgData.TimeMaxPhase3 = (uint)(((ulong)(DataR.Ah + OPPAh) * (ulong)ChgData.CoefPhase3 * 36L) / (ulong)Memo.Iphase3); // Tph3 = ((DataR.Ah * Coef) / Iph3) * 3600
		//ChgData.TimeMaxPhase3 = (ulong)ChgData.TimerSecCharge + (ulong)ChgData.TimeMaxPhase3;
		OPPAh = 0;
		// Time ph2+3 = 05H max
		//if (ChgData.TimeMaxPhase3 > ChgData.TimeMaxPhase2)
		ChgData.TimeMaxPhase3 = ChgData.TimeMaxPhase2;
		// Init dVdT
		ChgData.ThrlddVdT = 0;	// dVdT = 0mV
		ChgData.VdVdTOld = 0;	  // No dV/dT the 1st time because of voltage decrease in start of phase 3
		ChgData.VdVdTNew = 0;
		ChgData.TimedVdT = ChgData.TimerSecCharge + (15L * 60L);
		// loop
		ChgData.TimeLoopPhase = ChgData.TimerSecCharge + (6L * 60L) + 25L;	  // 6min25
		// Start regulation
		DataW.Ireq = Memo.Iphase3;
		Iphase3Tmp =  Memo.Iphase3;
		State.Charge = StateChgPhase3;
		break;

	case StateChgPhase3:           /* In phase 3 */
		if (ChgData.TimerSecCharge > TimeInitVregPhase3)
		{ // Delay for no voltage peak when regulation goes to 2850mV/Cell
			DataW.VreqCell = VCELL_MAX - 50;
		}
		// Ah security : Ah < (1.3 * Capacity)
		if (DataR.Ah > ((Memo.BatCap * 13) / 10))
		{
			DFtimeSecuOn;
			//State.Charge = StateChgStAvail;
            State.Charge = StateChgEndLoop;
            DataW.CntLoopPhase = NB_LOOP;
		}
		else if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase3)
		{
			//State.Charge = StateChgStAvail;
            State.Charge = StateChgEndLoop;
            DataW.CntLoopPhase = NB_LOOP;
		}
		else if (ChgData.TimerSecCharge > ChgData.TimeLoopPhase)
		{
			if (FlagLoopPhase == OFF)
			{
				// dVdT
				ChgData.VdVdTNew = DataR.VbatCell;
				if ((ChgData.VdVdTNew - ChgData.VdVdTOld) <= ChgData.ThrlddVdT)
				{
					DFdVdTOn;
					//State.Charge = StateChgStAvail;
                    State.Charge = StateChgEndLoop;
                    DataW.CntLoopPhase = NB_LOOP;
				}
				else
				{
					if (ChgData.VdVdTNew > ChgData.VdVdTOld)
						ChgData.VdVdTOld = ChgData.VdVdTNew;
					// Loop
					FlagLoopPhase = ON;
					DataW.VreqCell = VCELL_MAX - 50;
					State.Phase = StatePhLoopInit;
					FlagLoop = ON;
				}
			}
			else if (FlagLoop == OFF)
			{
				FlagLoopPhase = OFF;
				DataW.CntLoopPhase++;
				ChgData.TimeLoopPhase = ChgData.TimerSecCharge + (6L * 60L) + 25L;	// 6min25
				DataW.VreqCell = VCELL_MAX - 50;
				DataW.Ireq = Memo.Iphase3;
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

	case StateChgEndLoop:	 // 3 Loops at the end of profile
		if (DataW.CntLoopPhase < (NB_LOOP + 3))
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
				DataW.VreqCell = 2850;
				DataW.Ireq = 50;
			}
		}
		else
		{
			State.Charge = StateChgStAvail;
		}
		break;


	case StateChgStAvail:	// Only one end point
		State.Charge = StateChgAvail;
		break;

	case StateChgAvail:
		break;

	case StateChgStartComp:
		State.Charge = StateChgComp;
		break;

	case StateChgComp:
		State.Charge = StateChgStAvail;
		break;

	case StateChgStartEqual:
#ifdef ENERSYS_US
		// New EQ algo
		ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + ((ulong)Menu.EqualTime * 3600L);
		TimeMaxEqual = ChgData.TimeMaxPhase1 + ((ulong)AarsParam.EqTimePhase2 * 3600L);
		Memo.Iphase1 = (Memo.BatCap * 45)/100;
		DataW.Ireq = Memo.Iphase1;
		FlagEqPh2 = ON;
#else
		TimeMaxEqual = ChgData.TimeMaxPhase1;
		FlagEqPh2 = OFF;
		Memo.Iphase1 = Iphase3Tmp;
		DataW.Ireq = Memo.Iphase1;
#endif
		ChgData.ThrlddVdT = -30;
		ChgData.VdVdTOld = 0;
		ChgData.VdVdTNew = 0;
		ChgData.TimedVdT = ChgData.TimerSecCharge + (15L * 60L);


		DataW.VreqCell = 2850;
		State.Charge = StateChgEqual;
		break;

	case StateChgEqual:
		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase1)
		{

			if ((IQWIIQLink != 0) && (FlagIQEqual == ON))
			{
				ResetFlagEqualOn;
			}
			FlagIQEqual = OFF;

			// New EQ algo
			if (FlagEqPh2 == ON)
			{
				FlagEqPh2 = OFF;
				ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + ((ulong)AarsParam.EqTimePhase2 * 3600L);
				TimeMaxEqual = ChgData.TimeMaxPhase1;
				DataW.Ireq = (Memo.BatCap * 25)/100;
				DataW.VreqCell = 2850;
				ChgData.VdVdTOld = 0;
				ChgData.VdVdTNew = 0;
				ChgData.TimedVdT = ChgData.TimerSecCharge + (15L * 60L);
			}
			else
				State.Charge = StateChgStAvail;
		}
		else if (ChgData.TimerSecCharge >= ChgData.TimedVdT)
		{
			ChgData.VdVdTNew = DataR.VbatCell;
			if ((ChgData.VdVdTNew - ChgData.VdVdTOld) < ChgData.ThrlddVdT)
			{
				if ((IQWIIQLink != 0) && (FlagIQEqual == ON))
				{
					ResetFlagEqualOn;
				}
				FlagIQEqual = OFF;

				#ifdef ENERSYS_US
				DFdVdTOn;
				//DFDisDF5On;
				#endif

				State.Charge = StateChgStAvail;
			}
#ifdef ENERSYS_EU
			else if (ChgData.VdVdTNew > ChgData.VdVdTOld)
#endif
			{
				ChgData.VdVdTOld = ChgData.VdVdTNew;
			}
			ChgData.TimedVdT = ChgData.TimerSecCharge + (15L * 60L);	// 15min
		}
		break;
	}
}
