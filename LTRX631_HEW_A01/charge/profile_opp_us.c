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
extern ulong OPPAh;
extern uint  Iphase3Tmp;
extern uchar FlagDesul;

void ProfileOPP_US(void)
{
	uint VGazOld;

	OPPAh = DataR.Ah;   // Not Used
	FlagDesul = OFF;

	switch (State.Charge)
	{
	case StateChgStartPhase1:
		SlopeI = 2000;
		Memo.CapAutoManu = 1;   // to force display menu capacity
		Memo.Iphase1 = (Memo.BatCap * 25)/ 10;  // 25% x C5
		Memo.Iphase3 = (Memo.BatCap * 5) / 10;  // 5%  x C5
		Iphase3Tmp =  Memo.Iphase3;
		Memo.VgazCell = 2508L - (((slong)Memo.BatTemp * 35L) / 10L);
		if (Memo.Iphase1 < 50)
		{
			Memo.Iphase1 = 50;
		}
		else if (Memo.Iphase1 > Menu.Imax)
		{
			Memo.Iphase1 = Menu.Imax;
		}
		if (Memo.Iphase3 < 10)
		{
			Memo.Iphase3 = 10;
		}
		else if (Memo.Iphase3 > Menu.Imax)
		{
			Memo.Iphase3 = Menu.Imax;
		}
		if (Memo.VgazCell > 2563)
		{
			Memo.VgazCell = 2563;
		}
		else if (Memo.VgazCell < 2321)
		{
			Memo.VgazCell = 2321;
		}
		ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (6L * 3600L);
		ChgData.TimeMaxProfile = ChgData.TimerSecCharge + (11L * 3600L);


		ChgData.TimeComp = 0xFFFFFFFF;    // No Comp.

		if ((Menu.EqualPeriod != 0) && (Menu.CECOnOff == 0) && (Menu.ChgSkipDaily == 1))  // Equal day and Dailycharge OFF(MODIF R2.2)
			ChgData.TimeEqual = 0xFFFFFFFF - 1L;
		else
			ChgData.TimeEqual = 0xFFFFFFFF;   // No Equal

		DataW.CntLoopPhase = 0;
		ChgData.TimeLoopPhase = 0;
		ChgData.ThrlddVdT = -10;
		ChgData.VdVdTOld = 0;
		ChgData.VdVdTNew = 0;
		ChgData.TimedVdT = ChgData.TimerSecCharge + (30L * 60L);
		DataW.Ireq = Memo.Iphase1;
		DataW.VreqCell = 2850;
		State.Charge = StateChgPhase1;
		TimeCheckVbat = ChgData.TimerSecCharge + TIME_CHECK_VBAT;
		DataW.CntLoopPhase = NB_LOOP;
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
		else if (ChgData.TimerSecCharge > ChgData.TimedVdT )
		{
			ChgData.VdVdTNew = DataR.VbatCell;
			if ((ChgData.VdVdTNew - ChgData.VdVdTOld) < ChgData.ThrlddVdT)
			{
				DFdVdTOn;
				DFDisDF5On;
				State.Charge = StateChgStartPhase2;
				break;
			}
			else if (ChgData.VdVdTOld < ChgData.VdVdTNew)
			{
				ChgData.VdVdTOld = ChgData.VdVdTNew;
			}
			ChgData.TimedVdT = ChgData.TimerSecCharge + (15L * 60L);
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
				Memo.VgazCell = 2508L - (((slong)Memo.EocTemp * 35L) / 10L);
				if (Memo.VgazCell > 2563)
				{
					Memo.VgazCell = 2563;
				}
				else if (Memo.VgazCell < 2321)
				{
					Memo.VgazCell = 2321;
				}
			}
		}
		break;

	case StateChgStartPhase2:
		Memo.Iphase3 = (Memo.BatCap * 5) / 10;
		if (Menu.FloatingOffOn == ON)
		{
			Memo.Iphase3 += Menu.Ifloating;
		}
		ChgData.TimeMaxPhase2 = ChgData.TimerSecCharge + (5L * 3600L);
		ChgData.ThrlddIdT = 100;
		ChgData.IdIdTOld = Menu.Imax;
		ChgData.IdIdTNew = Menu.Imax;
		ChgData.TimedIdT = ChgData.TimerSecCharge + (15L * 60L);
		DataW.Ireq = Memo.Iphase1 - 1;  // Change Ireq to reset load sharing
		if (DFdVdT != 0)
			DataW.VreqCell = ChgData.VdVdTOld;
		else
			DataW.VreqCell = Memo.VgazCell;
		TimeCheckCurrent = ChgData.TimerSecCharge + TIME_CHECK_CUR;
		State.Charge = StateChgPhase2;
		break;

	case StateChgPhase2:
		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase2)
		{
			DFtimeSecuOn;
			State.Charge = StateChgStAvail;
			break;
		}
		else if (DataR.Ibat <= Memo.Iphase3)
		{
			if (ChgData.TimerSecCharge > TimeCheckCurrent)
			{
				State.Charge = StateChgStAvail;
				break;
			}
		}
		else if (ChgData.TimerSecCharge >= ChgData.TimedIdT)
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
				ChgData.TimedIdT = ChgData.TimerSecCharge + (15L * 60L);
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
			else if (DataLcd.ChgSOC > 95)
			{
				DataLcd.ChgSOC = 95;
			}
			DataLcd.ChgRestTime = ((8 * (100 - DataLcd.ChgSOC)) / 80) + 1;
			if (IQWIIQLink != 0)
			{
				VGazOld = Memo.VgazCell;
				Memo.VgazCell = 2508L - (((slong)Memo.EocTemp * 35L) / 10L);
				if (Memo.VgazCell > 2563)
				{
					Memo.VgazCell = 2563;
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

	case StateChgStAvail:
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
		State.Charge = StateChgStAvail;
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
