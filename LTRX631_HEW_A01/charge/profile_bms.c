/*
 * profil_bms.c
 *
 *  Created on: 23/10/2017
 *      Author: Francis DUHAUT
 */
 
#include "include.h"

extern uint	SlopeI;

uint IreqMem = 0;

/************************************************************************************
Name:        Update current slope
 ************************************************************************************/
void UpdateSlopeCurrent (void)
{
	if (IreqMem >= DataW.Ireq)
	{
		SlopeI = 2000;	// 10A/s
	}
	else
	{
		SlopeI = 5000;	// 50A/s
	}
	IreqMem = DataW.Ireq;
}

/************************************************************************************
Name:        Profile BMS Lithium
 ************************************************************************************/
void profile_bms (void)
{
	switch (State.Charge)     /* Select current phase */
	{
		case StateChgStartPhase1:
			SlopeI = 1000;	// 10A/s
			Memo.CapAutoManu = 1;   	// to force display menu capacity
			DataW.Ireq = 0;
			IreqMem = DataW.Ireq;
			DataW.VreqCell = 0;
			ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (20L * 3600L);
   			ChgData.TimeMaxProfile = ChgData.TimerSecCharge + (20L * 3600L);
			ChgData.TimeEqual = 0xFFFFFFFF;   // No Equal
			State.Charge = StateChgPhase1;
		break;
		
		case StateChgPhase1:
			DataW.Ireq = Bms.ChrgrDcI;
			DataLcd.ChgSOC = Bms.Soc;
			if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase1)
			{
				DFtimeSecuOn;
				State.Charge = StateChgStAvail;
			}
			else if (Bms.Soc == 100)
			{
				State.Charge = StateChgStAvail;
			}
			UpdateSlopeCurrent();
		break;
		
		case StateChgStAvail:	
			State.Charge = StateChgAvail;
			DataW.Ireq = Bms.ChrgrDcI;
			UpdateSlopeCurrent();
		break;

		case StateChgAvail:
			DataW.Ireq = Bms.ChrgrDcI;
			UpdateSlopeCurrent();
		break;
		
	}	
}
