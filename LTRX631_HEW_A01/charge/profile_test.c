/*
 * profil_test.c
 *
 *  Created on: 01/09/2013
 *      Author: Francis DUHAUT
 */

#include "include.h"

extern 	uint SlopeI;
extern	ulong CntRefresh;
extern  uchar ModIndex[MODNB+1];

void  ProfileTEST(void)
{

	switch (State.Charge)
	{
	case StateChgStartPhase1:     /* Just at beginning of phase 1 */
		// Init current-voltage
		Memo.Iphase1 = Menu.Imax;	// 50% Capacity
		// Init time
		ChgData.TimeMaxPhase1 = 120L;
		ChgData.TimeMaxPhase2 = 0;
		ChgData.TimeMaxProfile = 120L;
		ChgData.TimeComp = 0xFFFFFFFF;	// No comp
		ChgData.TimeEqual = 0xFFFFFFFF;	// No equal
		// Start regulation
		SlopeI = 500; // 5A/s
		DataW.Ireq = Memo.Iphase1;
		DataW.VreqCell = 2850;
		State.Charge = StateChgPhase1;
		break;

	case StateChgPhase1:          /* In phase 1 */
		if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase1)
		{
			State.Charge = StateChgStAvail;
		}
		break;
        
    case StateChgStAvail:
        State.Charge = StateChgAvail;
    break;

    case StateChgAvail:
		break;     
	
    }
}
