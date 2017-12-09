/*
 * profil_desul.c
 *
 *  Created on: 01/09/2013
 *      Author: Francis DUHAUT
 */
#include "include.h"

extern 	uint SlopeI;
extern	ulong CntRefresh;
extern  uchar FlagDesul;
extern  uint ChargerBatCap;
ulong DisconnectTimeOut;

void  ProfileDESUL(void)
{

	switch (State.Charge)
	{
	case StateChgStartPhase1:     /* Just at beginning of phase 1 */
		// Init current-voltage
		ChargerBatCap = (ulong)(Menu.Imax * BATCAP_C5) / 1000L;
		if ((Menu.CapAutoManu == 0) && (IQWIIQLink == 0))
			Memo.BatCap = ChargerBatCap;

		Memo.Iphase1 = (Memo.BatCap * 5) / 10;	// 5% Capacity
		SlopeI = 500; // 5A/s
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
		ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (15L * 60L);
		TimeMaxEqual = ChgData.TimeMaxPhase1;
		ChgData.TimeMaxProfile = ChgData.TimerSecCharge + (24L * 3600L);
		ChgData.TimeComp = 0xFFFFFFFF;	// No comp
		ChgData.TimeEqual = 0xFFFFFFFF;	// No equal
		// Start regulation
		DataW.Ireq = Memo.Iphase1;
		DataW.VreqCell = 2850;
		
		State.Charge = StateChgPhase1;
		DisconnectTimeOut = ChgData.TimerSecCharge;
		break;

	case StateChgPhase1:          /* In phase 1 */
		
		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase1)
		{
			if (DataR.VbatCell >= 1950)
			{
				FlagDesul = OFF;
				State.Charge = StateChgStartPhase1;
			}
			else
			{
				ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (15L * 60L);
				TimeMaxEqual = ChgData.TimeMaxPhase1;
			}
		}
		else if (ChgData.TimerSecCharge > (ChgData.TimeMaxProfile - 10L))
		{
			FlagDesul = OFF;
			State.Charge = StateChgStartPhase1;
		}
		else
		{
			DisconnectTimeOut = ChgData.TimerSecCharge;
		}
		break;

	case StateChgStAvail:	// Only one end point
		State.Charge = StateChgAvail;
		break;

	case StateChgAvail:
		break;

	}
}
