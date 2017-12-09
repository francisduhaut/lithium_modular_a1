/*
 * profil_vrla.c
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

uint MaxVoltVRLA;  
ulong TimeMaxPhase4;

void  ProfileVRLA(void)
{
	FlagDesul = OFF;

#ifdef  HAWKER_US
    ProfileGEL();
#else

	switch (State.Charge)
	{
	case StateChgStartPhase1:
		SlopeI = 2000;
        
		if ((IQWIIQLink != 0) && ((IQData.BatWarning & BatWarningEqual) != 0))
		{
			FlagIQEqual = ON;
		}

		Memo.CapAutoManu = 1;   // to force display menu capacity
		Memo.Iphase1 = (Memo.BatCap * 18)/10;
		Memo.Iphase3 = ((Memo.BatCap * Menu.VRLAIphase3) / 10) + 5;	// 1% < Capacity < 5%
		Memo.VgazCell = 2457L - (((slong)Memo.BatTemp * 35L) / 10L);
		MaxVoltVRLA = 2635L - (((slong)Memo.BatTemp * 35L) / 10L);

		if (Memo.Iphase1 < 50)
		{
			Memo.Iphase1 = 50;
		}
		else if (Memo.Iphase1 > Menu.Imax)
		{
			Memo.Iphase1 = Menu.Imax;
		}
		if (Memo.VgazCell > 2527)       // -20 DegC
		{
			Memo.VgazCell = 2527;
		}
		else if (Memo.VgazCell < 2229)  // +65 DegC
		{
			Memo.VgazCell = 2229;
		}
		ChgData.TimeMaxProfile = ChgData.TimerSecCharge + (14L * 3600L) + (30L * 60L); // 14H30
		ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (6L * 3600L);  // 06H
		ChgData.TimeComp = 0xFFFFFFFF;

		if ((Menu.EqualPeriod != 0) && (Menu.CECOnOff == 0)) // Equal day
		{
			ChgData.TimeEqual = 0xFFFFFFFF - 1L;
		}
		else
			ChgData.TimeEqual = 0xFFFFFFFF;   // No Equal

		DataW.CntLoopPhase = 0;
		ChgData.TimeLoopPhase = 0;
		DataW.Ireq = Memo.Iphase1;
		DataW.VreqCell = 2850;
		State.Charge = StateChgPhase1;
		TimeCheckVbat = ChgData.TimerSecCharge + TIME_CHECK_VBAT;
		break;

	case StateChgPhase1:
		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase1)
		{
			DFtimeSecuOn;
			State.Charge = StateChgStAvail;
			break;
		}
		else if (DataR.VbatCell >= Memo.VgazCell)
		{
			if (TimeCheckVbat < ChgData.TimerSecCharge)
			{
				State.Charge = StateChgStartPhase2;
				break;
			}
		}
		else
		{
			TimeCheckVbat = ChgData.TimerSecCharge + 5L;
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
				Memo.VgazCell =  2457L - (((slong)Memo.EocTemp * 35L) / 10L);
				MaxVoltVRLA = 2635L - (((slong)Memo.EocTemp * 35L) / 10L);
				if (Memo.VgazCell > 2527)
				{
					Memo.VgazCell = 2527;
				}
				else if (Memo.VgazCell < 2229)
				{
					Memo.VgazCell = 2229;
				}
				//DataW.VreqCell = Memo.VgazCell;
			}
		}
		break;

	case StateChgStartPhase2:
		if (Menu.FloatingOffOn == ON)
		{
			Memo.Iphase3 += Menu.Ifloating;
		}
		if (Memo.Iphase3 < 15)
		{
			Memo.Iphase3 = 15;
		}
		else if (Memo.Iphase3 > Menu.Imax)
		{
			Memo.Iphase3 = Menu.Imax;
		}
		ChgData.TimeMaxPhase2 = ChgData.TimerSecCharge + (6L * 3600L);
		ChgData.ThrlddIdT = Menu.Imax / 35;
		// MODIF R3.4
		if (ChgData.ThrlddIdT < 50)
			ChgData.ThrlddIdT = 50;
			
		ChgData.IdIdTOld = Menu.Imax;
		ChgData.IdIdTNew = Menu.Imax;
		ChgData.TimedIdT = ChgData.TimerSecCharge + (1L * 60L);

		DataW.Ireq = Memo.Iphase1 - 1;  // Change Ireq to reset load sharing
		DataW.VreqCell = Memo.VgazCell;
		State.Charge = StateChgPhase2;
		TimeCheckCurrent = ChgData.TimerSecCharge + TIME_CHECK_CUR;
		break;

	case StateChgPhase2:
		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase2)
		{
			DFtimeSecuOn;
			State.Charge = StateChgStAvail;
		}
		else if (DataR.Ibat <= Memo.Iphase3)
		{
			if (ChgData.TimerSecCharge > TimeCheckCurrent)
			{
				State.Charge = StateChgStartPhase3;
			}
		}
		else if (ChgData.TimerSecCharge >= ChgData.TimedIdT)
		{
			ChgData.IdIdTNew = DataR.Ibat;
			if ((ChgData.IdIdTNew - ChgData.IdIdTOld) > ChgData.ThrlddIdT)
			{
				DFdIdTOn;
				State.Charge = StateChgStAvail;
				State.Phase = StatePh1;
				break;
			}
			else
			{
				if (ChgData.IdIdTNew < ChgData.IdIdTOld)
				{
					ChgData.IdIdTOld = ChgData.IdIdTNew;
				}
				ChgData.TimedIdT = ChgData.TimerSecCharge + (5L * 60L);
			}
		}
		else
		{
			TimeCheckCurrent = ChgData.TimerSecCharge + TIME_CHECK_CUR;
		}

		if ((ChgData.TimerSecCharge % 60) == 0)
		{
			DataLcd.ChgSOC = Memo.InitSOC + (((ulong)Memo.ChgAh * 100L) / (ulong)Memo.BatCap);
			if (DataLcd.ChgSOC < 80)
			{
				DataLcd.ChgSOC = 80;
			}
			else if (DataLcd.ChgSOC > 98)
			{
				DataLcd.ChgSOC = 98;
			}
			DataLcd.ChgRestTime = ((8 * (100 - DataLcd.ChgSOC)) / 80) + 1;
		}
		break;

	case StateChgStartPhase3:
		TimeInitVregPhase3 = ChgData.TimerSecCharge + TIME_VREQ_PH3;
		ChgData.TimeMaxPhase3 = 6L * 3600;
		if ((Memo.TimePhase1 + Memo.TimePhase2) < 15L)
		{
			ChgData.TimeMaxPhase3 = 0;
		}
		ChgData.TimeMaxPhase3 = ChgData.TimerSecCharge + ChgData.TimeMaxPhase3;
		ChgData.ThrlddVdT = -10;
		ChgData.VdVdTOld = 0;
		ChgData.VdVdTNew = 0;
		ChgData.TimedVdT = ChgData.TimerSecCharge + (30L * 60L);
		DataW.Ireq = Memo.Iphase3;
		State.Charge = StateChgPhase3;
		break;

	case StateChgPhase3:
		if (ChgData.TimerSecCharge > TimeInitVregPhase3)
		{ // Delay for no voltage peak when regulation goes to 2850mV/Cell
			DataW.VreqCell = VCELL_MAX - 50;
		}
		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase3)
		{
			State.Charge = StateChgStAvail;
			break;
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
			ChgData.TimedVdT = ChgData.TimerSecCharge + (5L * 60L);
		}
		if ((ChgData.TimerSecCharge % 60) == 0)
		{
			DataLcd.ChgSOC = 99;
			if (ChgData.TimeMaxPhase3 > ChgData.TimerSecCharge)
			{
				DataLcd.ChgRestTime = ((ChgData.TimeMaxPhase3 - ChgData.TimerSecCharge) / 3600L) + 1;
			}
			else
			{
				DataLcd.ChgRestTime = 1;
			}
		}
		//MaxVoltVRLA = 2635L - (((slong)Memo.BatTemp * 35L) / 10L);
		if ( DataR.VbatCell >= MaxVoltVRLA )
		{
			State.Charge = StateChgStartPhase4;
		}
		break;

	case StateChgStartPhase4:
		TimeMaxPhase4 = 2L * 3600;
		TimeMaxPhase4 = ChgData.TimerSecCharge + TimeMaxPhase4;
		ChgData.ThrlddIdT = Menu.Imax / 35;
		ChgData.IdIdTOld = Memo.Iphase1;
		ChgData.IdIdTNew = Memo.Iphase1;
		ChgData.TimedIdT = ChgData.TimerSecCharge + (1L * 60L);
		DataW.Ireq = Memo.Iphase3 + (Menu.Imax / 35) + 50;  //Memo.Iphase1;
		DataW.VreqCell = MaxVoltVRLA;
		State.Charge = StateChgPhase4;
		ModResetEfficiency();
		break;

	case StateChgPhase4:
		if (ChgData.TimerSecCharge >= ChgData.TimeMaxPhase3)
		{
			State.Charge = StateChgStAvail;
			break;
		}
		else if (ChgData.TimerSecCharge >= TimeMaxPhase4)
		{
			State.Charge = StateChgStAvail;
			break;
		}
		if (ChgData.TimerSecCharge >= ChgData.TimedIdT)
		{
			ChgData.IdIdTNew = DataR.Ibat;
			if ((ChgData.IdIdTNew - ChgData.IdIdTOld) > ChgData.ThrlddIdT)
			{
				DFdIdTOn;
				State.Charge = StateChgStAvail;
				break;
			}
			else
			{
				if (ChgData.IdIdTNew < ChgData.IdIdTOld)
				{
					ChgData.IdIdTOld = ChgData.IdIdTNew;
				}
				ChgData.TimedIdT = ChgData.TimerSecCharge + (5L * 60L);
			}
		}
		break;
	case StateChgStAvail:
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
		Memo.Iphase1 = ((Memo.BatCap * Menu.VRLAIphase3) / 10) + 5;	// 1% < Capacity < 5%
		ChgData.ThrlddVdT = -10;
		ChgData.VdVdTOld = 0;
		ChgData.VdVdTNew = 0;
		ChgData.TimedVdT = ChgData.TimerSecCharge + (15L * 60L);
		DataW.Ireq = Memo.Iphase1;
		MaxVoltVRLA = 2635L - (((slong)Memo.EocTemp * 35L) / 10L);
		DataW.VreqCell = MaxVoltVRLA;
		State.Charge = StateChgEqual;
		TimeMaxEqual = ChgData.TimeMaxPhase1;
		break;

	case StateChgEqual:
		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase1)
		{
			if (IQWIIQLink != 0)
			{
				ResetFlagEqualOn;
			}
			State.Charge = StateChgStAvail;
			break;
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
		break;
	}
#endif    
}
