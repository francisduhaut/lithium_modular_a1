/*
 * profil_ionic.c
 *
 *  Created on: 01/09/2013
 *      Author: Francis DUHAUT
 */

#include "include.h"

extern uint Iphase3min;
extern ulong Iphase3est;
extern uchar InitSoC;
extern uint AhDeduction;
extern uint AhToReinject;
extern ulong TimeIncVreg;
extern uint TempVbat;
extern uchar FlagLoopPhase;
// Loop Phase
extern uchar NbTimeLoopPhase;
extern uchar CntTimeLoopPhase;
// Closer loop
extern uchar FlagCloserLoop1;
extern uchar FlagCloserLoop2;
extern uchar FlagDoCloserLoop;
extern uchar CntCloserLoop;
extern uint Iphase1Tmp;
extern uint Iphase3Tmp;

extern uint	SlopeI;
extern uint ItoReach;
extern ulong VbatCellCharger;
extern uint ChargerBatCap;
extern uchar FlagLoop;
extern ulong TimeCheckCurrent;
extern ulong TimeInitVregPhase3;
extern uchar FlagDesul;

extern uint LoopWidthOld;
extern uint LoopWidthNew;
extern sint DeltaLoopWidth;
extern sint DeltaLoopWidthMax;


/************************************************************************************
Name:        LoopWidth : Detection of a maximum characterizing the break of slope
 ************************************************************************************/
void LoopWidth(void)
{

	if (DataR.Vloop2 > DataR.Vloop1)
	{	
		LoopWidthNew = DataR.Vloop2 - DataR.Vloop1;
	}
	else
		LoopWidthNew = 0;
	if ((DataW.CntLoopPhase >= 2) && (DataW.VreqCell > 2300))
	{
		DeltaLoopWidth = LoopWidthNew - LoopWidthOld;
		if (DeltaLoopWidth > DeltaLoopWidthMax)
			DeltaLoopWidthMax = DeltaLoopWidth;
		LoopWidthOld = LoopWidthNew;
		if ((DeltaLoopWidthMax >= 15) && (DeltaLoopWidth < 5) && (LoopWidthNew >= 45))
		{
			State.Charge = StateChgStartPhase3;
		}
	}
}
/************************************************************************************
Name:        Profile IONIC
 ************************************************************************************/
void  ProfileIONIC(void)
{
    if (FlagDesul == ON)
	{
	    ProfileDESUL();
		return;
	}
        
	switch (State.Charge)     /* Select current phase */
	{
        
	case StateChgStartPhase1:     /* Just at beginning of phase 1 */
    
        SlopeI = 2000;            // 20A/sec
        
#ifdef LIFEIQ
		if ((IQWIIQLink != 0) && ((IQData.BatWarning & BatWarningEqual) != 0))
		{
			FlagIQEqual = ON;
		}
#endif

		ChgData.CoefPhase3 = (ulong)Menu.IonicCoef * 10L;

		// Init time
		ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (12L * 3600L);
		ChgData.TimeMaxPhase2 = ChgData.TimerSecCharge + (12L * 3600L);
		ChgData.TimeMaxProfile = ChgData.TimerSecCharge + (14L * 3600L);
		ChgData.TimeLoopPhase = 0;
		if ((Menu.RefreshOffOn != 0) && (Menu.CECOnOff == 0))
			ChgData.TimeComp = 10L * 60L;	// 1st Compensation after 10mn
		else
			ChgData.TimeComp = 0xFFFFFFFF;

		if ((Menu.EqualPeriod != 0) && (Menu.CECOnOff == 0))  // Equal day
			ChgData.TimeEqual = 0xFFFFFFFF - 1L;
#ifdef ENERSYS_EU
		else if (Menu.CECOnOff == 0)
			ChgData.TimeEqual = 24L * 3600L;  // Equalization after 24h
#endif
		else
			ChgData.TimeEqual = 0xFFFFFFFF;   // No Equal

		// Init loop
		DataW.CntLoopPhase = 0;
		FlagLoopPhase = OFF;
		AhDeduction = 0;

		// Start regulation
		ChargerBatCap = (ulong)(Menu.Imax * BATCAP_C5) / 1000L;
		if ((Menu.CapAutoManu == 0) && (IQWIIQLink == 0))
			Memo.BatCap = ChargerBatCap;

		Memo.Iphase1 = (Memo.BatCap * 18) / 10;	// 0.18 x Capacity


		/* Set low and high limit */
		if (Memo.Iphase1 < 50)
			Memo.Iphase1 = 50;
		else if (Memo.Iphase1 > Menu.Imax)
			Memo.Iphase1 = Menu.Imax;
		Iphase3Tmp =  Memo.Iphase1 / 5; // For Equal
		DataW.Ireq = 50;		// 5A
		DataW.VreqCell = VCELL_MAX - 50;
		State.Charge = StateChgPhase1;
		TimeCheckCurrent = ChgData.TimerSecCharge + 10L;
		break;

	case StateChgPhase1:          /* In phase 1 */
		// Time security
		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase1)
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
				InitSoC = SocLoop(DataR.Vloop1);	    // %SoC
				AhDeduction = DataR.Ah;						    // Ah already injected
				DataW.CntLoopPhase++;
				// Capacity estimation
				// -> do in LoopPhase() for restart after the good current
				if (DataW.CntLoopPhase <= 2)
					ChgData.TimeLoopPhase = ChgData.TimerSecCharge + 180L;	// 3mn
				else
					ChgData.TimeLoopPhase = ChgData.TimerSecCharge + 385L;	// 6mn25s
				// Increase Vreg
				Iphase3Tmp =  Memo.Iphase1 / 5; // For Equal
				DataW.VreqCell = 2600;
				DataW.Ireq = Memo.Iphase1;
				//TimeIncVreg = ChgData.TimerSecCharge + 4L;	// Wait 4s before measure
                TimeIncVreg = ChgData.TimerSecCharge + (Memo.Iphase1 / 200) + 4L;
				State.Charge = StateChgIncVregPhase1;
				TimeCheckCurrent = ChgData.TimerSecCharge + 10L;
			}
		}
		else if (DataR.Ibat <= 50)  // 5A
		{
			if (ChgData.TimerSecCharge > TimeCheckCurrent)
			{
				State.Charge = StateChgStAvail;
				break;
			}
		}
		else
		{
			TimeCheckCurrent = ChgData.TimerSecCharge + 10L;
		}
		break;


	case StateChgIncVregPhase1:
		//if (ChgData.TimerSecCharge > TimeIncVreg)
        if ((ChgData.TimerSecCharge > TimeIncVreg) 
            || (DataR.Ibat > (Memo.Iphase1 - 20)))//MODIF R/P/U1.5
		{
			// MODIF 2.8
			if ((AarsParam.NbModLeft == OUTPUT_2_CABLE_6M) || (AarsParam.NbModLeft == OUTPUT_2_CABLE_8M))
			{
				if (DataR.VbatCell > 2400)
            		DataW.VreqCell = 2400;
            	else
				{
					FlagVRegPhase1 = 1;
					DataW.VreqCell = VbatCellCharger;
				}
				
			}
			else
			{
				if (VbatCellCharger > 2400)
            		DataW.VreqCell = 2400;
            	else
					DataW.VreqCell = VbatCellCharger;
			}
			DataW.Ireq = Memo.Iphase1;
			if (DataW.CntLoopPhase <= NB_LOOP)
				State.Charge = StateChgPhase1;
			else
			{
				State.Charge = StateChgStartPhase2;
			}
			LoopWidth();
			TimeCheckCurrent = ChgData.TimerSecCharge + 10L;
		}
		break;

	case StateChgStartPhase2:      /* Beginning of phase 2 */
		// Init current
		Memo.Iphase1 =  (Memo.BatCap * 25) / 10;	// 25% of Capacity
		if (Memo.Iphase1 > Menu.Imax)
			Memo.Iphase1 = Menu.Imax;
		Iphase1Tmp = Memo.Iphase1;                // Save Iphase 1 for COMP and EQUAL
		DataW.Ireq = Memo.Iphase1 - 1;            // Change Ireq to reset load sharing
		Iphase3min = Memo.BatCap / 4;             // 2.5% Capacity
		// Limits
		if (Iphase3min < (Memo.Iphase1 / 10))
			Iphase3min = Memo.Iphase1 / 10;	        // 10% Iphase1
		else if (Iphase3min > (Memo.Iphase1 / 5))
			Iphase3min = Memo.Iphase1 / 4;	        // 25% Iphase1
		Memo.Iphase3 = Iphase3min;
		if (Menu.FloatingOffOn == ON)
			Memo.Iphase3 += Menu.Ifloating;
		// Init Loop
		FlagCloserLoop1 = OFF;
		FlagCloserLoop2 = OFF;
		FlagDoCloserLoop = ON;
		CntCloserLoop = 0;
		FlagLoopPhase = OFF;
		ChgData.TimeLoopPhase = ChgData.TimerSecCharge + 385L;	// 6mn25s
		NbTimeLoopPhase = 4;
		CntTimeLoopPhase = 0;
		// Init dIdT
		ChgData.ThrlddIdT = 100; // dIdT max = 10A
		ChgData.IdIdTOld = Menu.Imax;
		ChgData.IdIdTNew = Menu.Imax;
		State.Charge = StateChgPhase2;
		break;

	case StateChgPhase2:           /* In phase 2 */

		// Time security
		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase2)
		{
			DFtimeSecuOn;
			State.Charge = StateChgStAvail;
		}
        else if (DataR.Ah > ((Memo.BatCap * 13) / 10))
		{
			DFtimeSecuOn;
			State.Charge = StateChgStAvail;
		}
		else if (DataR.Ah >= Memo.Iphase1)    // Cap max = IPhase1 * 10
		{
			DFotherBatOn;
			State.Charge = StateChgStAvail;
		}
		else if (ChgData.TimerSecCharge >= ChgData.TimeLoopPhase)
		{
			if (FlagLoopPhase == OFF)       // Before loop
			{
				TempVbat = VbatCellCharger;		// backup the voltage before the loop
				// Closer Loop
				if (TempVbat > 2320)
				{
					NbTimeLoopPhase = 1;	      // Closer loop if VbatCell > 2.32V
				}
				if (DataW.VreqCell > 2400)    // dIdT
				{
					ChgData.IdIdTNew = DataR.Ibat;
                    // MODIF R2.7
					//if ((ChgData.IdIdTNew - ChgData.IdIdTOld) > ChgData.ThrlddIdT)
                    if (((ChgData.IdIdTNew - ChgData.IdIdTOld) > ChgData.ThrlddIdT) && (FlagCloserLoop1 != ON)) // No DF7 / No stop dIdt in Closer Loop
					{
						State.Charge = StateChgStAvail;
						DFdIdTOn;
					}
					else if (ChgData.IdIdTNew < ChgData.IdIdTOld)
						ChgData.IdIdTOld = ChgData.IdIdTNew;
				}

				if( (TempVbat > 2600) || ((DataR.Ibat <= Iphase3min) && (TempVbat > 2350)) ) // Vbat & Ibat security
				{
					Memo.Iphase3 = DataR.Ibat;  // Update Iphase3
					State.Charge = StateChgStartPhase3;
					break;
				}
				else
				{
					FlagLoopPhase = ON;
					Memo.Iphase3 = DataR.Ibat;  // Update Iphase3
					DataW.Ireq = Memo.Iphase3;
					CntTimeLoopPhase++;
					if (CntTimeLoopPhase >= NbTimeLoopPhase)  // Start loop
					{
						CntTimeLoopPhase = 0;
						DataW.CntLoopPhase++;
						State.Phase = StatePhLoopInit;
						FlagLoop = ON;
					}
				}
			}
			else if (FlagLoop == OFF)   // After loop
			{
				FlagLoopPhase = OFF;
				TimeCheckCurrent = ChgData.TimerSecCharge + TIME_CHECK_CUR;
				DataW.Ireq = Memo.Iphase1;
				ChgData.TimeLoopPhase = ChgData.TimerSecCharge + 385L;	// 6mn25s
				FlagDoCloserLoop = ON;
				LoopWidth();
				if (FlagCloserLoop1 == OFF)
				{
					// Increase Vreg
					if (((sint)DataW.VreqCell - (sint)TempVbat) < 20)
					{
						if ((DataW.VreqCell >= 2350) || (((sint)DataW.VreqCell - (sint)TempVbat) >= 10))
							DataW.VreqCell += 3;
						else
							DataW.VreqCell += 7;
					}
					if (DataW.VreqCell > (VCELL_MAX-50))
						DataW.VreqCell = (VCELL_MAX-50);
				}
			}
		}

		// Charge Percentage
		if ((DataW.CntLoopPhase >= NB_LOOP) && (FlagDoCloserLoop == ON))
		{
#ifndef DEMO
			DataLcd.ChgSOC = SocLoop(DataR.Vloop1);
			DataLcd.ChgRestTime = ((8 * (100 - DataLcd.ChgSOC)) / 80) + 1;
#endif
		}

		// Charge Percentage
		if ((DataW.CntLoopPhase >= 6) && (FlagDoCloserLoop == ON))
		{
			DataLcd.ChgSOC = SocLoop(DataR.Vloop1);
			DataLcd.ChgRestTime = ((8 * (100 - DataLcd.ChgSOC)) / 80) + 1;
			if (DataLcd.ChgSOC >= 85)
			{
				if (DataLcd.ChgSOC > InitSoC)
					Iphase3est = (((((ulong)DataR.Ah - (ulong)AhDeduction) * 100L) / (((ulong)DataLcd.ChgSOC - (ulong)InitSoC) * 20L)) + 1L) * 10L;
				else
					Iphase3est = ((((ulong)DataR.Ah - (ulong)AhDeduction) * 100L) + 1L) * 10L;
				if (Iphase3est < Iphase3min)
					Iphase3est = Iphase3min;
				else if (Iphase3est > (Iphase1Tmp / 2))
					Iphase3est = Iphase1Tmp / 2;


				// MODIF FD - add time to check Ibat
				if (DataR.Ibat > ((uint)Iphase3est + 10))
				{
					TimeCheckCurrent = ChgData.TimerSecCharge + TIME_CHECK_CUR;
				}

				if ((InitSoC < 80) &&
						(((DataR.Ibat < Iphase3est) && (ItoReach == DataW.Ireq) && (ChgData.TimerSecCharge > TimeCheckCurrent)) || (FlagCloserLoop1 == ON)))
				{
					// MODIF R2.7
                    //if (CntCloserLoop < 10)
                    if (CntCloserLoop < 3)
					{
						FlagCloserLoop1 = ON;
						FlagDoCloserLoop = OFF;
						CntCloserLoop++;
						DataW.Ireq = Iphase3est;
						DataW.VreqCell = 2850;
						ModResetEfficiency();
						ChgData.TimeLoopPhase = ChgData.TimerSecCharge + 180;
						NbTimeLoopPhase = 1;
					}
					else
					{
						State.Charge = StateChgStartPhase3;
					}
				}
			}
		}
		break;

	case StateChgStartPhase3:
		// Init Iphase3
		TimeInitVregPhase3 = ChgData.TimerSecCharge + TIME_VREQ_PH3;
		if (Memo.Iphase3 < Iphase3min)
			Memo.Iphase3 = Iphase3min;
		else if (Memo.Iphase3 > (Iphase1Tmp / 2))
			Memo.Iphase3 = Iphase1Tmp / 2;
		// Init Loop
		ChgData.TimeLoopPhase = ChgData.TimerSecCharge + 385L; // 6mn25s
		// Init dVdT
		ChgData.ThrlddVdT = -5;
		ChgData.VdVdTOld = 0;   // No dV/dT the 1st time because of voltage decrease in start of phase 3
		ChgData.VdVdTNew = 0;
		// Init Ahphase3
        // MODIF R2.7
        //if ((FlagCloserLoop1 == ON) && (CntCloserLoop >= 10))
        if ((FlagCloserLoop1 == ON) && (CntCloserLoop >= 3))
		{
            //if (ChgData.CoefPhase3 >= 50)
            if (ChgData.CoefPhase3 >= 20)
                //ChgData.CoefPhase3 = ChgData.CoefPhase3 - 50;		    // -> reduction CF if complete phase 4 only
                ChgData.CoefPhase3 = ChgData.CoefPhase3 - 20;		    // -> reduction CF if complete phase 4 only
            else    
                ChgData.CoefPhase3 = 0;
        }
		AhToReinject = ((ulong)DataR.Ah * (((ulong)ChgData.CoefPhase3 / 10L) + 100L)) / 100L;
		DataW.Ireq = Memo.Iphase3;
		Iphase3Tmp = Memo.Iphase3;
		State.Charge = StateChgPhase3;
        ModResetEfficiency();
		break;

	case StateChgPhase3:           /* In phase 3 */
		if (ChgData.TimerSecCharge > TimeInitVregPhase3)
		{ // Delay for no voltage peak when regulation goes to 2850mV/Cell
			DataW.VreqCell = VCELL_MAX - 50;
		}
		if (DataR.Ah > ((Memo.BatCap * 13) / 10))
		{
			DFtimeSecuOn;
			State.Charge = StateChgStAvail;
		}
		else if (DataR.Ah >= AhToReinject)
		{
			State.Charge = StateChgStAvail;
		}
		else if (ChgData.TimerSecCharge >= ChgData.TimeLoopPhase)
		{
			// dVdT
			ChgData.VdVdTNew = DataR.VbatCell;
			if ((ChgData.VdVdTNew - ChgData.VdVdTOld) < ChgData.ThrlddVdT)
			{
				DFdVdTOn;
				State.Charge = StateChgStAvail;
			}
			else
			{
				ChgData.VdVdTOld = ChgData.VdVdTNew;
				// Loop
				ChgData.TimeLoopPhase = ChgData.TimerSecCharge + 385L; // 6mn25s
				State.Phase = StatePhLoopInit;  // Loops
				FlagLoop = ON;
			}
		}
		if ((ChgData.TimerSecCharge % 60) == 0) // Reminding time & Ah estimation
		{
			DataLcd.ChgSOC = 99;
			if (AhToReinject > DataR.Ah)
				DataLcd.ChgRestTime = (((AhToReinject - DataR.Ah) * 10) / Memo.Iphase3) + 1;
			else
				DataLcd.ChgRestTime = 1;
		}
		break;

	case StateChgStAvail:	// Only one end point
#ifdef LIFEIQ
		if (FlagIQEqual == ON)
		{ // Force equal in 5 minutes if requested by WiIQ
			ChgData.TimeEqual = ChgData.TimerSecCharge + (5L * 60L);
		}
#endif
		State.Charge = StateChgAvail;
		break;

	case StateChgAvail:
		break;

	case StateChgStartComp:
		ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + 10L;
		ChgData.CntComp++;
		ChgData.TimeComp = ChgData.TimerSecCharge + (10L * 60L);	// Next refresh after 10mn
		// Start regulation
		DataW.Ireq = Iphase1Tmp / 3;
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
#ifdef ENERSYS_US
		// New EQ algo
		ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + ((ulong)Menu.EqualTime * 3600L);
		TimeMaxEqual = ChgData.TimeMaxPhase1 + ((ulong)AarsParam.EqTimePhase2 * 3600L);
		if (Memo.CapAutoManu != 0)
			Memo.Iphase1 = (Memo.BatCap * 45)/100;
		else
			Memo.Iphase1 = Iphase3Tmp;
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
				if (Memo.CapAutoManu != 0)
					DataW.Ireq = (Memo.BatCap * 25)/100;
				else
					DataW.Ireq = Iphase3Tmp/2;
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
