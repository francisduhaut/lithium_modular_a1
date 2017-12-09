/*
 * profil_equal.c
 *
 *  Created on: 01/09/2013
 *      Author: Francis DUHAUT
 */

#include "include.h"


extern 	uint SlopeI;
extern	ulong CntRefresh;
extern 	sint TempCap;
extern 	uint ItoReach;
extern 	uchar TempProfile;

void  ProfileEQUAL(void)
{
	switch (State.Charge)     /* Select current phase */
	{
	case StateChgStartPhase1:     /* Just at beginning of phase 1 */
		SlopeI = 500;            // 5A/s
		// Init current-voltage
		Memo.Iphase1 = Menu.Iequal;
		// Limits
		if (Memo.Iphase1 > Menu.Imax)
			Memo.Iphase1 = Menu.Imax;
		// Init time
		if (ForceEqual == 0)
			ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + ((ulong)Menu.EqualTime * 3600L);
		else
			ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (30L *60L);
		ChgData.TimeMaxProfile = ChgData.TimeMaxPhase1 + 10L;
		ChgData.TimeComp = 0xFFFFFFFF;	// No comp
		ChgData.TimeEqual = 0xFFFFFFFF;	// No equal
		// Init dVdT
		ChgData.ThrlddVdT = -50;
		ChgData.VdVdTOld = 0;
		ChgData.VdVdTNew = 0;
		ChgData.TimedVdT = ChgData.TimerSecCharge + (15L * 60L);	// 15min
		// Start regulation
		if (ForceEqual != 0)
			DataW.Ireq = 50;   // 5A in force equal mode
		else
			DataW.Ireq = Memo.Iphase1;
		switch (TempProfile)
		{
		case XFC :
		case TPPL:
			DataW.VreqCell = 2600;
			break;

		default :
			DataW.VreqCell = 2850;
			break;
		}
		State.Charge = StateChgPhase1;
		TimeMaxEqual = ChgData.TimeMaxPhase1;
		break;

		case StateChgPhase1:          /* In phase 1 */
			if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase1)
			{
				State.Charge = StateChgStAvail;
			}
			else if (ChgData.TimerSecCharge >= ChgData.TimedVdT)
			{
				ChgData.VdVdTNew = DataR.VbatCell;
				if ((ChgData.VdVdTNew - ChgData.VdVdTOld) < ChgData.ThrlddVdT)
				{
					DFdVdTOn;
					#ifdef ENERSYS_US
					//DFDisDF5On;
					#endif
					State.Charge = StateChgStAvail;
				}
				ChgData.VdVdTOld = ChgData.VdVdTNew;
				ChgData.TimedVdT = ChgData.TimerSecCharge + (15L * 60L);	// 15min
			}
			if ((ChgData.TimerSecCharge % 60) == 0) // Reminding time & Ah estimation
			{
				DataLcd.ChgSOC = Memo.InitSOC + (((ulong)Memo.ChgAh * 100L) / (ulong)Memo.BatCap); // %SoC = %InitSoC + %(Ah/Cap)
				if (DataLcd.ChgSOC > 98)
					DataLcd.ChgSOC = 98;
				if (ChgData.TimeMaxPhase1 > ChgData.TimerSecCharge)
					DataLcd.ChgRestTime = ((ChgData.TimeMaxPhase1 - ChgData.TimerSecCharge) / 3600L) + 1;
				else
					DataLcd.ChgRestTime = 1;
			}
			break;

		case StateChgStAvail:	// Only one end point
			if (ForceEqual != 0)
			{
				InstStartOn;
				State.Charger = StateChgerIdle;
				ForceEqualOff;
			}
			State.Charge = StateChgAvail;
			break;

		case StateChgAvail:
			break;
	}
}
