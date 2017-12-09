/*
 * profile_xfc.c
 *
 *  Created on: 23 janv. 2013
 *      Author: duhautf
 */
#include "include.h"

extern 	uint SlopeI;
extern	ulong CntRefresh;
extern  ulong TimeCheckVbat;
extern  ulong TimeCheckCurrent;
extern  ulong TimeInitVregPhase3;
extern  ulong VbatModCell;

void  ProfileXFC(void)
{

	switch (State.Charge)
	{
	case StateChgStartPhase1:     /* Just at beginning of phase 1 */

#ifdef LIFEIQ
		if ((IQWIIQLink != 0) && ((IQData.BatWarning & BatWarningEqual) != 0))
		{
			FlagIQEqual = ON;
		}
#endif
		// Init current-voltage
		Memo.CapAutoManu = 1;   // to force display menu capacity
		Memo.Iphase1 = Memo.BatCap * 5;	// 50% Capacity
		Memo.VgazCell = 2480L - ((slong)Memo.BatTemp * 4L);
		// Limits
		if (Memo.Iphase1 < 50)
			Memo.Iphase1 = 50;
		else if (Memo.Iphase1 > Menu.Imax)
			Memo.Iphase1 = Menu.Imax;
		if (Memo.VgazCell > 2500)
			Memo.VgazCell = 2500;
		else if (Memo.VgazCell < 2320)
			Memo.VgazCell = 2320;
		// Init time
		ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (2L * 3600L) + (30L * 60L);
		ChgData.TimeMaxProfile = ChgData.TimerSecCharge + (6L * 3600L) + (30L * 60L);
		ChgData.TimeComp = 0xFFFFFFFF;	// No comp
		ChgData.TimeEqual = 0xFFFFFFFF;	// No equal
		// Start regulation
		SlopeI = 500; // 5A/s
		DataW.Ireq = Memo.Iphase1;
		DataW.VreqCell = 2600;
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
		else
		{
			TimeCheckVbat = ChgData.TimerSecCharge + TIME_CHECK_VBAT;
		}

		if ((ChgData.TimerSecCharge % 60) == 0) // Reminding time & Ah estimation
		{
			DataLcd.ChgSOC = Memo.InitSOC + (((ulong)Memo.ChgAh * 100L) / (ulong)Memo.BatCap);
			if (DataLcd.ChgSOC > 80)
				DataLcd.ChgSOC = 80;
			if (ChgData.TimeMaxPhase2 > ChgData.TimerSecCharge)
				DataLcd.ChgRestTime = ((ChgData.TimeMaxPhase2 - ChgData.TimerSecCharge) / 3600L) + 1;
			else
				DataLcd.ChgRestTime = 1;

#ifdef LIFEIQ
			// Update Vgaz
			if (IQWIIQLink != 0)
			{
				Memo.VgazCell = 2480L - ((slong)Memo.EocTemp * 4L);
				if (Memo.VgazCell > 2500) // Tmin = -5�C
					Memo.VgazCell = 2500;
				else if (Memo.VgazCell < 2320) // Tmax = 40�C
					Memo.VgazCell = 2320;
			}
#endif
		}
		break;

	case StateChgStartPhase2:      /* Beginning of phase 2 */
		ChgData.TimeMaxPhase2 = ChgData.TimerSecCharge + (2L * 3600L) + (30L * 60L);
		// Init dIdT
		ChgData.TimedIdT = ChgData.TimerSecCharge + (15L * 60L);	// 15min
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
			State.Charge = StateChgStartPhase3;
            //State.Charge = StateChgStAvail;
		}
		else if ((DataR.Ibat <= 20) && (ChgData.TimerSecCharge < (5L * 60L))) // < 5mn min no Ia
		{
			if (ChgData.TimerSecCharge > TimeCheckCurrent)
				State.Charge = StateChgStAvail;
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
			if (ChgData.TimeMaxPhase2 > ChgData.TimerSecCharge)
				DataLcd.ChgRestTime = ((ChgData.TimeMaxPhase2 - ChgData.TimerSecCharge) / 3600L) + 1;
			else
				DataLcd.ChgRestTime = 1;

#ifdef LIFEIQ
			// Update Vgaz
			if (IQWIIQLink != 0)
			{
				Memo.VgazCell = 2480L - ((slong)Memo.EocTemp * 4L);
				if (Memo.VgazCell > 2500) // Tmin = -5�C
					Memo.VgazCell = 2500;
				else if (Memo.VgazCell < 2320) // Tmax = 40�C
					Memo.VgazCell = 2320;
			}
#endif

		}
		break;

	case StateChgStartPhase3:
		ChgData.TimeMaxPhase3 = ChgData.TimerSecCharge + (1L * 3600L);
		TimeInitVregPhase3 = ChgData.TimerSecCharge + TIME_VREQ_PH3;
		DataLcd.ChgSOC = 99;
		DataLcd.ChgRestTime = 1;
		// Start regulation
		if ((Memo.Iphase1 / 50) < 5)
			DataW.Ireq = 5;
		else
			DataW.Ireq = Memo.Iphase1 / 50;
		State.Charge = StateChgPhase3;
		break;

	case StateChgPhase3:
		if (ChgData.TimerSecCharge > TimeInitVregPhase3)
		{ // Delay for no voltage peak
			DataW.VreqCell = 2650;
		}
		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase3)
		{
			State.Charge = StateChgStAvail;
		}
		break;

	case StateChgStAvail:	// Only one end point
		State.Charge = StateChgAvail;
		CntRefresh = ChgData.TimerSecCharge;
		break;

	case StateChgAvail:
		//if (DataR.VbatCell < 2150)
		if (VbatModCell < 2150)
		{
			// wait 1mn before comp
			if (ChgData.TimerSecCharge > (CntRefresh + (1L*60L)))
				State.Charge = StateChgStartComp;
		}
		else
		{
			CntRefresh = ChgData.TimerSecCharge;
		}
		break;

	case StateChgStartComp:
		ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (1L * 3600L);  // comp 2H
		// Start regulation
		DataW.Ireq = Memo.Iphase1;
		Memo.VgazCell = 2480L - ((slong)Menu.BatTemp * 4L);
		if (IQWIIQLink != 0)
		{
			Memo.VgazCell = 2480L - ((slong)IQData.BatTemp * 4L);
		}
		if (Memo.VgazCell > 2500)       // Tmin = -5�C
			Memo.VgazCell = 2500;
		else if (Memo.VgazCell < 2320)  // Tmax = 40�C
			Memo.VgazCell = 2320;
		DataW.VreqCell = Memo.VgazCell;
		State.Charge = StateChgComp;
		CntRefresh = ChgData.TimerSecCharge;
		break;

	case StateChgComp:
		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase1)
		{
			State.Charge = StateChgStAvail;
		}
		else if (DataR.Ibat < ICONS_MIN)
		{
			if (ChgData.TimerSecCharge > (CntRefresh + 60L))
				State.Charge = StateChgStAvail;
		}
		else
		{
			CntRefresh = ChgData.TimerSecCharge;
		}
		break;

	case StateChgStartEqual:
		if (FlagIQEqual == ON)  // WiIQ Equal
		{
			Memo.Iphase1 = (Memo.BatCap * 2) / 10;// 2% Capacity
			//ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (2L * 3600L); // 2h
			// Start regulation
			DataW.Ireq = Memo.Iphase1;
			DataW.VreqCell = 2650;
			State.Charge = StateChgEqual;
		}
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
		break;


	}
}
