#include "include.h"

signed int DEF_DELTAI;
#define DEF_DELTAU	12

ulong DeadTime;
ulong TimeCurPhase2;
ulong TimeStartEqual;
ulong TimeCheckDiDvDt;
ulong TimePhase3;
uchar Index;
uint TabPoint[15];

ulong	TimerVgaz;
uchar	FlagIbat;

signed int DeltaI;
signed int DeltaU;

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
extern uint VbatModCell;

/************************************************************************************
Name:        Profile TPPL
************************************************************************************/
void  ProfileTPPL(void)
{
    uint i;
    
   
    switch (State.Charge)     /* Select current phase */
    {
    case StateChgStartPhase1:     /* Just at beginning of phase 1 */
        DeltaU = 0;
        FlagIQEqual = OFF;
        if ((IQWIIQLink != 0) && ((IQData.BatWarning & BatWarningEqual) != 0))
        {
            FlagIQEqual = ON;
        }
        // Init current-voltage
        ChgData.CoefPhase3 = 30; // Coef = 1.030
        DeadTime = 0;
        
        // Start with Menu capacity if No WiIQ
        if (IQWIIQLink == 0)
        {
            Memo.BatCap = Menu.BatCap;
            Memo.CapAutoManu = 1;       // MODIF R2.6 : To display cap in Memo
        }
        
        
        if (ChgData.Profile == NXBLC)
            Memo.Iphase1 = ((ulong)Memo.BatCap * (ulong)70)/10L;	// 0.7C
        else if (ChgData.Profile != XFC)
            Memo.Iphase1 = ((ulong)Memo.BatCap * (ulong)40)/10L;	// 0.4C XFCION
        else
            Memo.Iphase1 = ((ulong)Memo.BatCap * (ulong)20)/10L;	// 0.20C XFCEVO
            
        Memo.VgazCell = 2350 + (30L * 3) - ((slong)Memo.EocTemp * 3);	// david  Vreg = f (T�C)  3mV /�C
        /* Set low and high limit */
        if (Memo.Iphase1 < ChargerConfig.Imin)
        Memo.Iphase1 = ChargerConfig.Imin;
        else if (Memo.Iphase1 > Menu.Imax)
        Memo.Iphase1 = Menu.Imax;
        
        if (((Memo.Iphase1*10) / Memo.BatCap) < 18) // Out of range
        {
            // MODIF 2.8
            if (DFotherBat != 0)
            {
            }
            else
                DFotherBatOn;
        }

        // Init security time
        ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (5 * 3600L);   // david  5h phase 1
        ChgData.TimeMaxPhase2 = ChgData.TimerSecCharge + (5 * 3600L);	// david  5h phase 2
        ChgData.TimeMaxProfile = ChgData.TimerSecCharge + (12 * 3600L);	// david 12h max
        
        ChgData.TimeComp = 0xFFFFFFFF;	// No comp
        if (IQWIIQLink != 0)
        ChgData.TimeEqual = 0xFFFFFFFF;         // Equal done by WiIQ
        else
        ChgData.TimeEqual = 0xFFFFFFFF - 1;     // Equal set by Day - use menu

        // Start regulation
        DataW.Ireq = Memo.Iphase1;
        
        if (DataW.Ireq > Menu.Imax)  
        DataW.Ireq = Menu.Imax;
        
        State.Charge = StateChgPhase1;
        // Update VGaz Cell
        if (IQWIIQLink != 0)
        {
            Memo.VgazCell = 2350 + (30L * 3) - ((slong)Memo.EocTemp * 3);	// david  Vreg = f (T�C)  3mV /�C
        }
        if ((Memo.VgazCell > 2455) && (Menu.FloatingOffOn == 0))    // No AGV -5�C min
            Memo.VgazCell = 2455;
        else if ((Memo.VgazCell > 2410) && (Menu.FloatingOffOn != 0))   // AGV +10�C min
            Memo.VgazCell = 2410;
        else if (Memo.VgazCell < 2320)  // +40�C max
            Memo.VgazCell = 2320;
        
        DataW.VreqCell = Memo.VgazCell + 100;
        
        if (DataR.VbatCell > (Memo.VgazCell+100))
        {
            State.Charge = StateChgStAvail;
        }
        
        TimeCheckVbat = ChgData.TimerSecCharge + TIME_CHECK_VBAT;
        
        // MODIF 2.8
        if (Memo.SocVoltage < 2080)
            SlopeI = 2*2000;    // 20A/s
        else if (Memo.SocVoltage < 2100)
            SlopeI = 2*1000;    // 10A/s
        else 
            SlopeI = 2*500;     // 5A/s  
        
		
        // Init dVdT
        ChgData.TimedVdT = ChgData.TimerSecCharge + (30L * 60L);
        ChgData.ThrlddVdT = -10;
        ChgData.VdVdTOld = 0; // No dV/dT the 1st time because of voltage decrease in start of phase 3
        ChgData.VdVdTNew = 0;

        // MODIF R2.6
        Memo.Iphase3 = Memo.BatCap /10; // David Safety  1%

		// MODIF 3.2
		if (Memo.Iphase3 < 10)
			Memo.Iphase3 = 10;	
		if (Memo.SocVoltage >= 2150)
		{
			State.Charge = StateChgStAvail;
		}
        break;

    case StateChgPhase1:          /* In phase 1 */
        if ((ChgData.TimerSecCharge % 60) == 0) // Reminding time & Ah estimation
        {
            DataLcd.ChgSOC = Memo.InitSOC + (((ulong)Memo.ChgAh * 100L) / (ulong)Memo.BatCap); // %SoC = %InitSoC + %(Ah/Cap)
            if (DataLcd.ChgSOC > 80)
            DataLcd.ChgSOC = 80;
            if (ChgData.TimeMaxProfile > ChgData.TimerSecCharge)
            DataLcd.ChgRestTime = ((ChgData.TimeMaxProfile - ChgData.TimerSecCharge) / 3600L) + 1;
            else
            DataLcd.ChgRestTime = 1;
            // Update VGaz Cell
            if (IQWIIQLink != 0)
            {
                Memo.VgazCell = 2350 + (30L * 3) - ((slong)Memo.EocTemp * 3);	// david  Vreg = f (T�C)  -3mV /�C
                if ((Memo.VgazCell > 2455) && (Menu.FloatingOffOn == 0))    // No AGV -5�C min
                    Memo.VgazCell = 2455;
                else if ((Memo.VgazCell > 2410) && (Menu.FloatingOffOn != 0))   // AGV +10�C min
                    Memo.VgazCell = 2410;
                else if (Memo.VgazCell < 2320)  // +40�C max
                    Memo.VgazCell = 2320;
            }
        }
        if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase1)
        {
            //DFtimeSecuOn;
            State.Charge = StateChgStartPhase2;
            Memo.VgazCell = DataR.VbatCell;
        }
        else if (ChgData.TimerSecCharge > ChgData.TimedVdT)
        {
            ChgData.VdVdTNew = DataR.VbatCell;
            if ((ChgData.VdVdTNew - ChgData.VdVdTOld) < ChgData.ThrlddVdT)
            {
                State.Charge = StateChgStartPhase2;
                Memo.VgazCell = DataR.VbatCell;
            }
            else
            {
                ChgData.VdVdTOld = ChgData.VdVdTNew;
                ChgData.TimedVdT = ChgData.TimerSecCharge + (15L * 60L);
            }
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
        break;

    case StateChgStartPhase2:      /* Beginning of phase 2 */
        // Start regulation
        DataW.Ireq = Memo.Iphase1;
        
        if (DataW.Ireq > Menu.Imax)  
        DataW.Ireq = Menu.Imax;
        DataW.VreqCell = Memo.VgazCell;
        State.Charge = StateChgPhase2;
        TimeCurPhase2 = ChgData.TimerSecCharge;
        // MODIF R2.7
        //TimeCheckDiDvDt = ChgData.TimerSecCharge;
        TimeCheckDiDvDt = ChgData.TimerSecCharge + (15L * 60L);	    // 15min
        Index = 0;
        DEF_DELTAI = Memo.Iphase1/35;
        ChgData.TimeMaxPhase2 = ChgData.TimerSecCharge + (5 * 3600L);
        Memo.Iphase3 = Memo.BatCap /10; // David Safety  1%
        
        // MODIF R2.6 : add DiDt in phase 2
        //ChgData.TimedIdT = ChgData.TimerSecCharge + (15L * 60L);	// 15min
        ChgData.TimedIdT = ChgData.TimerSecCharge + (30L * 60L);	// 30min
        // MODIF R2.7
		ChgData.ThrlddIdT = 50; // 5A
		ChgData.IdIdTOld = Menu.Imax;
		ChgData.IdIdTNew = Menu.Imax;
        break;

    case StateChgPhase2:           /* In phase 2 */
    
        // MODIF R2.6
        SlopeI = 100; // 1A/s

        if (ChgData.TimerSecCharge > (TimeCheckDiDvDt + 60L))
        {
            TimeCheckDiDvDt = ChgData.TimerSecCharge;
            if (Index >= 15)
            {
                // calcul Di/DT
                DeltaI = (signed int)(TabPoint[0]) - (signed int)(DataR.Ibat);
                if (DeltaI <= DEF_DELTAI)
                {
                    Memo.Iphase3 = DataR.Ibat;
                    State.Charge = StateChgStartPhase3;
                }
                for(i=0;i<14;i++)
                {
                    TabPoint[i] = TabPoint[i+1];
                }
                TabPoint[14] = DataR.Ibat;
            }
            else
            {
                TabPoint[Index] = DataR.Ibat;
                Index++;
            }
        }
        // MODIF R2.6
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
        else if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase2)
        {
            //State.Charge = StateChgStAvail;
            Memo.Iphase3 = DataR.Ibat;
            State.Charge = StateChgStartPhase3;
        }
        else if (DataR.Ibat < Memo.Iphase3)
        { // Security if a new module is started
            if (ChgData.TimerSecCharge > (TimeCurPhase2 + 10L))
            {
                State.Charge = StateChgStAvail;
            }
        }
        else
        {
            TimeCurPhase2 = ChgData.TimerSecCharge;
        }

        if ((ChgData.TimerSecCharge % 60) == 0) // Reminding time & Ah estimation
        {
            DataLcd.ChgSOC = Memo.InitSOC + (((ulong)Memo.ChgAh * 100L) / (ulong)Memo.BatCap); // %SoC = %InitSoC + %(Ah/Cap)
            if (DataLcd.ChgSOC < 80)
            DataLcd.ChgSOC = 80;
            else if (DataLcd.ChgSOC > 98)
            DataLcd.ChgSOC = 98;
            if (ChgData.TimeMaxProfile > ChgData.TimerSecCharge)
            DataLcd.ChgRestTime = ((ChgData.TimeMaxProfile - ChgData.TimerSecCharge) / 3600L) + 1;
            else
            DataLcd.ChgRestTime = 1;
        }
        break;

    case StateChgStartPhase3:      /* Beginning of phase 3 */
        
        // MODIF R2.7
        TimeInitVregPhase3 = ChgData.TimerSecCharge + 30L; //TIME_VREQ_PH3;
        
        if (Menu.FloatingOffOn != 0)
        {
            if (Memo.Iphase3 >= ((Memo.Iphase1 / 5) + Menu.Ifloating) ) // 20% Imax + IFloating
            Memo.Iphase3 = (Memo.Iphase1 / 5) + Menu.Ifloating;
            else if (Memo.Iphase3 <= ((Memo.Iphase1 / 20) + Menu.Ifloating)) // 5% Imax + IFloating
            Memo.Iphase3 = (Memo.Iphase1 / 20) + Menu.Ifloating;
        }
        else
        {
            if (Memo.Iphase3 >= (Memo.Iphase1 / 5) ) // 20% Imax
            Memo.Iphase3 = Memo.Iphase1 / 5;
            else if (Memo.Iphase3 <= (Memo.Iphase1 / 20)) // 5% Imax
            Memo.Iphase3 = Memo.Iphase1 / 20;
        }
        

        if (Menu.FloatingOffOn != 0)
        TimePhase3 = (ulong)(((ulong)DataR.Ah * (ulong)ChgData.CoefPhase3 * 36L) / ((ulong)Memo.Iphase3 - (ulong)Menu.Ifloating) ); // Tph3 = ((DataR.Ah * Coef) / (Iph3 - Ifloating)) * 3600
        else
        TimePhase3 = (ulong)(((ulong)DataR.Ah * (ulong)ChgData.CoefPhase3 * 36L) / (ulong)Memo.Iphase3); // Tph3 = ((DataR.Ah * Coef) / Iph3) * 3600  
        TimePhase3 += ChgData.TimerSecCharge;

        if (Menu.FloatingOffOn != 0)
        ChgData.TimeMaxPhase3 = (ulong)(((ulong)DataR.Ah * 50L * 36L) / ((ulong)Memo.Iphase3 - (ulong)Menu.Ifloating) ); // Coef 1.05 max
        else
        ChgData.TimeMaxPhase3 = (ulong)(((ulong)DataR.Ah * 50L * 36L) / (ulong)Memo.Iphase3); // Coef 1.05 max
        
        if (ChgData.TimeMaxPhase3 > (3 * 3600L)) // 3H max.
        ChgData.TimeMaxPhase3 = 3 * 3600L;
        ChgData.TimeMaxPhase3 += ChgData.TimerSecCharge;

        // Init dVdT
        Index = 0;
        DFdVdTOff;
        TimeCheckDiDvDt = ChgData.TimerSecCharge;

        // Start regulation
        DataW.Ireq = Memo.Iphase3 - 2;
        
        /*if (Menu.FloatingOffOn != 0)  
        DataW.VreqCell = 2600;
        else
        DataW.VreqCell = 2650;	*/
        State.Charge = StateChgPhase3;
        break;


    case StateChgPhase3:           /* In phase 3 */
    
        // MODIF R2.6
        SlopeI = 100; // 1A/s
        
        // MODIF R2.7
        if (ChgData.TimerSecCharge > TimeInitVregPhase3)
        { // Delay for no voltage peak when regulation goes to 2850mV/Cell
            if (Menu.FloatingOffOn != 0)  
                DataW.VreqCell = 2600;
            else
                DataW.VreqCell = 2650;
            DataW.Ireq = Memo.Iphase3;    	
        }
        
        if (ChgData.TimerSecCharge > (TimeCheckDiDvDt + 60L))
        {
            TimeCheckDiDvDt = ChgData.TimerSecCharge;
            if (Index >= 15)
            {
                // calcul Du/DT
                DeltaU = (signed int)(DataR.VbatCell) - (signed int)(TabPoint[0]);
                if (DeltaU <= DEF_DELTAU)
                {
                    DFdVdTOn;
                }
                for(i=0;i<14;i++)
                {
                    TabPoint[i] = TabPoint[i+1];
                }
                TabPoint[14] = DataR.VbatCell;
            }
            else
            {
                TabPoint[Index] = DataR.VbatCell;
                Index++;
            }
        }
        else if ((ChgData.TimerSecCharge > TimePhase3) && (DFdVdT!= 0)) // Coef 1.03 mini and dVdt
        {
            State.Charge = StateChgStAvail;
        }
        else if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase3) // Coef 1.05
        {
            State.Charge = StateChgStAvail;
        }
        break;

    case StateChgStAvail:	// Only one end point
        State.Charge = StateChgAvail;
        CntRefresh = ChgData.TimerSecCharge;
        if (FlagIQEqual == ON)
        {   // Force equal in 5 minutes if requested by WiIQ
            TimeStartEqual = ChgData.TimerSecCharge + (5L * 60L);
        }  
        break;

    case StateChgAvail:
        if (FlagIQEqual == ON)
        {
            if (ChgData.TimerSecCharge >= TimeStartEqual)
            {
                FlagIQEqual = OFF;
                State.Charge = StateChgStartEqual;  // Force start of equal
                State.Phase = StatePh1; // Begin next loop with StatePh1
                TimeStartEqual = 0xFFFFFFFF;
            }
        }
        // MODIF 3.2 : modif
        /*else if (IdleState != 0)
        {
            CntRefresh = ChgData.TimerSecCharge;
        }*/
        // Refresh if Vbat < 2.09V (AGV) or Vbat < 2.15V (no AGV)
        else if (((DataR.VbatCell < 2090) && (Menu.FloatingOffOn != 0)) || ((DataR.VbatCell < 2150) && (Menu.FloatingOffOn == 0)))
        { // wait 1mn before comp
            if (ChgData.TimerSecCharge > (CntRefresh + (1L*60L)))
            State.Charge = StateChgStartComp;
        }
        else
        {
            CntRefresh = ChgData.TimerSecCharge;
        }
        break;

    case StateChgStartEqual:
        Memo.Iphase1 = (Memo.BatCap * 2)/10;	// 2%
        ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (2 * 3600L);		// David 2h equal
        // Start regulation
        DataW.Ireq = Memo.Iphase1;
        if (Menu.FloatingOffOn == ON)
        {
            DataW.Ireq += Menu.Ifloating;
            DataW.VreqCell = 2600;
        }
        else
        {
            DataW.VreqCell = 2650;
        }
        State.Charge = StateChgEqual;
		TimeMaxEqual = ChgData.TimeMaxPhase1;
        break;

    case StateChgEqual:
        // MODIF R2.6
        SlopeI = 100; // 1A/s
        
        if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase1)
        {
            State.Charge = StateChgStAvail;
            //AarsParam.FlagEqual = 2;  // equal complete
            //EepWriteOn;
            FlagIQEqual = OFF;
            if (IQWIIQLink != 0)
            {
                ResetFlagEqualOn;
            }
        }
        break;

    case StateChgStartComp:
        ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + (1L * 3600L);  // comp 1H
        // Start regulation
		// MODIF 3.2 : add 1A
        DataW.Ireq = Memo.Iphase3 + 10;
        //if (Menu.FloatingOffOn == ON)
        //DataW.Ireq += Menu.Ifloating;
        DataW.VreqCell = 2350 + (30L * 3) - ((slong)Memo.EocTemp * 3);	// david  Vreg = f (T�C)  3mV /�C
        if (DataW.VreqCell > 2410)
        DataW.VreqCell = 2410;
        else if (DataW.VreqCell < 2320)
        DataW.VreqCell = 2320;
        State.Charge = StateChgComp;
        CntRefresh = ChgData.TimerSecCharge;
        break;

    case StateChgComp:
        // MODIF R2.6
        SlopeI = 100; // 1A/s
        
        if (ChgData.TimerSecCharge > ChgData.TimeMaxPhase1)
        {
            State.Charge = StateChgStAvail;
        }
        else if (DataR.Ibat < 5)
        {
            if (ChgData.TimerSecCharge > (CntRefresh + 60L))
            State.Charge = StateChgStAvail;
        }
        else
        {
            CntRefresh = ChgData.TimerSecCharge;
        }
        break;
    }

}
