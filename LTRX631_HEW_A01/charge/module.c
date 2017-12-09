/*
 * module.c
 *
 *  Created on: 22/02/2013
 *      Author: Francis DUHAUT
 */

#include "include.h"

extern uint RcableLeft;	                /* 1/10 mohm */
extern uint RcableRight;                /* 1/10 mohm */
extern uint	ItoReach;
extern uint VbatThrld;
extern uint SlopeI;

extern uchar FlagDerating;                     // MODIF 2.8 : derating
extern ulong TimeCheckDerating;                // MODIF 2.8 : derating


extern uchar FlagEfficiency;
extern ulong TimeCheckEfficiency;
extern uchar ModIndex[MODNB+1];                /* List of index for available modules */
extern uint  ModTabImax[MODNB+1];
extern uint  IbatModMem[MODNB+1];

extern uint ICons;
extern uchar ModCurr;                          /* Last active module, 0 = no module */
extern uchar ModCurrSave;                      // MODIF 2.8
extern uint  ModImax;                          /* Modular Imax calculate with connected module */
extern ulong TimeDefFuse;
extern uchar FlagDefBatCon;
extern ulong TimeModCur;
extern ulong TimeCheckIdle;
extern ulong TimeRemoveMod;
extern ulong TimerModDef;
extern ulong TimerDefID;
extern ulong TimerDisconnect;
extern ulong TimeBatCon;

extern uint  IbatMem;
extern sint  IbatOld;
extern uint  IreqOld;
extern uint  ILeft;                             /* Current in left branch */
extern uint  IRight;                            /* Current in right branch */
extern uint  VReq;
extern slong VoffsetComp;                       // MODIF 2.8
extern ulong Vcbl;
extern slong VcblMem;                           // MODIF 2.8
extern ulong RcblMem;                           // MODIF 2.8
extern schar Tambiant;
extern ulong VbatCellCharger;
extern ulong VbatModCell;

extern uchar  FlagLoop;
// MODIF 2.8
//extern uchar  PMod;
extern uint   PMod;
extern uchar  ChargerID;
extern uchar  ModNumber;
extern uint   IDModule;
extern ulong  TimeCheckCurrent;
extern ulong  TimeCheckVcell;
extern uchar  ForceMod70A;
extern ulong  CntCANFault;
extern ulong  TimeDefVreg;

extern slong VbatLeft;
extern slong VbatRight;
extern schar NbrLeft;
extern schar NbrRight;

// MODIF R2.6
extern uint IbatIQ;

uchar ptrMod;
uchar ModNumberTmp = 0;         // MODIF R3.1

// MODIF 2.8
extern uchar FlagInit;


#define MIN_CUR_START         50              			/* courant min 5A pour d�marrer le module */

const uchar ModuleIMax[7][7][1] = {
 //   6    12    18    24   36   40   48 Elts
   	{ 35,  35,   25,   18,   0,   0,   0},              /* output current 230VAC (A) */
    { 35,  20,   14,   11,   0,   0,   0},              /* output current 120VAC (A) */
    {  0,  70,   65,   60,  40,  36,  30},              /* output current 3KW triphase (400VAC) EU */
    {  0,  80,   80,   60,  40,  36,  30},              /* output current 3KW triphase (480VAC) US */
    {  0,  80,   50,   50,   0,   0,   0},              /* output current 3KW single phase (480VAC) */
    { 80,  40,   40,   40,  25,  25,   0},              /* output current 3KW triphase (208VAC) */
    {  0,  80,   80,   60,  40,  36,   0},              /* output current 3KW triphase (600VAC) US */
};


const uint ModulePMax[7][7][1] = {
 //   6     12    18    24    36    40    48 Elts
	#ifdef SUISSE
	{  900,  900,  900,  900,    0,    0,    0},       /* output power 230VAC (A) */
	#else
	{ 1000, 1000, 1000, 1000,    0,    0,    0},       /* output power 230VAC (A) */
	#endif
   	
    {  600,  600,  600,  600,    0,    0,    0},       /* output power 120VAC (A) */
#ifndef LIMIT_2900W
    {    0, 3500, 3500, 3500, 3500, 3500, 3500},       /* output power 3KW triphase (400VAC) EU */
    {    0, 3500, 3500, 3500, 3500, 3500, 3500},       /* output power 3KW triphase (480VAC) US */
#else
	{    0, 2900, 2900, 2900, 2900, 2900, 2900},       /* output power 3KW triphase (400VAC) EU */
    {    0, 2900, 2900, 2900, 2900, 2900, 2900},       /* output power 3KW triphase (480VAC) US */
#endif	
    {    0, 2300, 2800, 2800,    0,    0,    0},       /* output power 3KW single phase (480VAC) */
    { 2100, 2100, 2100, 2100, 2100, 2100,    0},       /* output power 3KW triphase (208VAC) */
    {    0, 3500, 3500, 3500, 3500, 3500,    0},       /* output power 3KW triphase (600VAC) US */
};


/**
 * @brief  Chrono task for module management
 * @param
 *
 * @retval
 */
void MOD_Chrono(void)
{

#ifndef DEF8BAY
    Menu.NbModLeft = 8;
    AarsParam.NbModLeft = 0;
#else
    switch (AarsParam.NbModLeft)
    {   
        case OUTPUT_2_CABLE_6M: 
            Menu.NbModLeft = 3;
            break;
            
        case OUTPUT_2_CABLE_8M: 
            Menu.NbModLeft = 4;
            break;
        
        default:
            Menu.NbModLeft = 8;
            AarsParam.NbModLeft = 0;
        break;
    }
#endif
    
    AarsParam.ImaxLeft = 3200;
    AarsParam.ImaxRight= 3200;

    
	ModList();

#ifdef DEF8BAY    
// MODIF 2.8
    switch (AarsParam.NbModLeft)
    {   // Sort module to share current into left/right output
        case OUTPUT_2_CABLE_6M: // 2x3 modules
        case OUTPUT_2_CABLE_8M: // 2x4 modules
            if (State.Charge >= StateChgStartPhase1
                && (State.Charge <= StateChgAvail))    // Not sort in IQScan / IQ Link / Comp / Equal
                ModSortLeftRight();    
            break;
        
        default:
        break;
    }
    // END
#endif
    
	ModCheckDef();
	ModCalcVbatLLC();

	if (ModCurr != 0)
	{
		/* Actions only if modules available */
		ModCalc();
		ModFindImax();
		// MODIF R2.3
		if (ChgData.Profile == TEST)
        {
            ModI();
        }
        // No load sharing when disconnection
        else if (ChargeDisconnect == 0)
        {
		    ModShare();
        }

#ifndef DEF8BAY
		if (ChgData.Profile != TEST)
        {   
            ModV();
        }
#else
    	if (ChgData.Profile != TEST)
        {
            if ((AarsParam.NbModLeft == OUTPUT_2_CABLE_6M) || (AarsParam.NbModLeft == OUTPUT_2_CABLE_8M))
                ModV_8BAY_R1();
            else
                ModV();
        }
#endif

	}
	else
	{
		/* reset data for wait mode only if not critical fault (DF1/DF2/DF3/TH) */
		if (ModFault() == 0)
		{
			DataR.Ibat = 0;
			DataR.Power = 0;
			DataW.Iconsign = 0;
			DataR.Vbat = 0;
			DataR.VbatMod = 0;
			DataR.Vcharger = 0;
			DataR.VbatCell = 0;
		}
		else
			ModCalcDef();
	}
}


/**
 * @brief  Init module status
 * @param
 *
 * @retval
 */
void Init_MOD (void)
{
	uchar i;
	TimeModCur = 0;
	IreqOld = 0;
	ModNumber = 0;
	TimeRemoveMod = 0;
	TimerModDef = 0;
	TimerDefID = 0;
    TimerDisconnect = 0;

	for (i = 0; i < MODNB; i++)
	{
		LLC[i].Status = 0;
		LLC[i].CntDefCur = 0;
		LLC[i].CntDefTemp = 0;
		LLC[i].TimeModAckDef = State.TimerSec + TIME_DEF_CUR_WAIT;
		CanErrOn(i);
	}
	ModResetEfficiency();
	ModAckDef();
    Mask480VACOn;       // MODIF R2.1
    ForceMod70A = 0;
    FlagDerating = 0;   // MODIF 2.8 : derating
}


/**
 * @brief  Return Imax Module vs  Module ID / Version
 * @param
 *
 * @retval
 */
uint ModFindIMaxModuleID (uchar ModID, uint Ver)
{
	uint imodule;
	uchar i;
    
    // MODIF NEW ID
    switch (ModID)
    {
        case ID_LLC1KW_12V :
        case ID_LLC1KW_24V :
        case ID_LLC1KW_3648V :
        case ID_LLC1KW_7280V :
            if (Menu.VMains == 1)        // 120VAC
		        i = 1;
	        else                         // 230VAC
		        i = 0;
        break;
        
        case ID_LLC3KW_243648V:         // 480VAC EU
        case ID_LLC3KW_7280V:
        case ID_LLC3KW_96V:
            ForceMod70A = 1;
            i = 2;
        break;
        
        case ID_LLC3KW_243648VUS:       // 480VAC US
            if ((Mains480VAC == 0) && (ForceMod70A == 0))
                i = 3;                  // MODIF R2.1
            else                        // MODIF R2.1
                i = 2;                  // MODIF R2.1
        break;
        
        case ID_LLC3KW_24VM:            // 480VAC MONOPHASE
        case ID_LLC3KW_3648VM:
            i = 4;
        break;
        
        case ID_LLC3KW240_12V:          // 240VAC
        case ID_LLC3KW240_243648V:
        case ID_LLC3KW240_7280V:
            i = 5;
        break;
        
        case ID_LLC3KW600_243648V:      // 600VAC
        case ID_LLC3KW600_7280V:
            i = 6;
        break;
        
        default:
            i = 0;
        break;
    }
    
	switch (Menu.NbCells)
	{
	case 6 :
		if (ChgData.Profile != LITH)
		{
#ifdef  ENERSYS_EU
			if ((i < 2) && (Ver >= 155))
				imodule = 360;
			else
#endif        
				imodule = (ModuleIMax[i][0][0] * 10);
		}
		else
		{
			imodule = (ModuleIMax[i][1][0] * 10); // 24V module in lithium
		}
		break;
	case 7  :	// LITH
	case 12 :
#ifdef  ENERSYS_EU    
        if ((i == 0) && (Ver >= 155))
            imodule = 360;
        else
#endif        
        imodule = (ModuleIMax[i][1][0] * 10);
		break;
	case 10 :		// LITH
	case 18 :
		imodule = (ModuleIMax[i][2][0] * 10);
		break;
	case 14 :		// LITH
		imodule = (ModuleIMax[i][3][0] * 10);
		break;
	case 24 :
		if (ChgData.Profile != LITH)
			imodule = (ModuleIMax[i][3][0] * 10);
		else
			imodule = (ModuleIMax[i][5][0] * 10);	// module 80V in Lithium
		break;
	case 20 :		// LITH
	case 30 :       // MODIF 2.8
    case 32 :    
	case 36 :
		imodule = (ModuleIMax[i][4][0] * 10);
		break;
	case 40 :
		imodule = (ModuleIMax[i][5][0] * 10);
		break;
	case 48 :
		imodule = (ModuleIMax[i][6][0] * 10);
		break;
	default :
		imodule = 0;
		break;
	}
    
	return imodule;
}


/**
 * @brief  Return Pmax Module vs  Module ID
 * @param
 *
 * @retval
 */
uint ModFindPMaxModuleID (uchar ModID)
{
	uint pmodule;
	uchar i;
    
    // MODIF NEW ID
    switch (ModID)
    {
        case ID_LLC1KW_12V :
        case ID_LLC1KW_24V :
        case ID_LLC1KW_3648V :
        case ID_LLC1KW_7280V :
            if (Menu.VMains == 1)        // 120VAC
		        i = 1;
	        else                         // 230VAC
		        i = 0;
        break;
        
        case ID_LLC3KW_243648V:         // 480VAC EU
        case ID_LLC3KW_7280V:
        case ID_LLC3KW_96V:
            i = 2;
        break;
        
        case ID_LLC3KW_243648VUS:       // 480VAC US
            if ((Mains480VAC == 0) && (ForceMod70A == 0))
                i = 3;                  // MODIF R2.1
            else                        // MODIF R2.1
                i = 2;                  // MODIF R2.1
        break;
        
        case ID_LLC3KW_24VM:            // 480VAC MONOPHASE
        case ID_LLC3KW_3648VM:
            i = 4;
        break;
        
        case ID_LLC3KW240_12V:          // 240VAC
        case ID_LLC3KW240_243648V:
        case ID_LLC3KW240_7280V:
            i = 5;
        break;
        
        case ID_LLC3KW600_243648V:      // 600VAC
        case ID_LLC3KW600_7280V:
            i = 6;
        break;
        
        default:
            i = 0;
        break;
    }
    
	switch (Menu.NbCells)
	{
	case 6 :
		if (ChgData.Profile != LITH)
			pmodule = (ModulePMax[i][0][0]);
		else
			pmodule = (ModulePMax[i][1][0]); // 24V module
		break;
	case 7 :		// LITH
	case 12 :
		pmodule = (ModulePMax[i][1][0]);
		break;
	case 10 :		// LITH
	case 18 :
		pmodule = (ModulePMax[i][2][0]);
		break;
	case 14 :		// LITH
		pmodule = (ModulePMax[i][3][0]);
		break;
	case 24 :
		if (ChgData.Profile != LITH)
			pmodule = (ModulePMax[i][3][0]);
		else
			pmodule = (ModulePMax[i][5][0]);	// LITH
		break;
	case 20 :		// LITH
	case 30 :       // MODIF 2.8
    case 32 : 
    case 36 :
		pmodule = (ModulePMax[i][4][0]);
		break;
	case 40 :
		pmodule = (ModulePMax[i][5][0]);
		break;
	case 48 :
		pmodule = (ModulePMax[i][6][0]);
		break;
	default :
		pmodule = 0;
		break;
	}
    
	return pmodule;
}


/**
 * @brief  Calculate Imax for each module vs NbCells
 * @param
 *
 * @retval
 */
void ModFindImax (void)
{
	uchar i;
    
   
	switch (LLCRamRO[ModIndex[0]].Id)
    {
        case ID_LLC1KW_12V :
        case ID_LLC1KW_24V :
        case ID_LLC1KW_3648V :
        case ID_LLC1KW_7280V :
            Mask480VACOn;               // MODIF R2.1    
        break;
        
        case ID_LLC3KW_243648V:         // 480VAC EU
            Mask480VACOff;              // MODIF R2.1
        break;
            
        case ID_LLC3KW_7280V:
        case ID_LLC3KW_96V:
            Mask480VACOn;                // MODIF R2.1
        break;
        
        case ID_LLC3KW_243648VUS:       // 480VAC US
            Mask480VACOff;              // MODIF R2.1
        break;
        
        case ID_LLC3KW_24VM:            // 480VAC MONOPHASE
        case ID_LLC3KW_3648VM:
            Mask480VACOn;               // MODIF R2.1
        break;
        
        case ID_LLC3KW240_12V:          // 240VAC
        case ID_LLC3KW240_243648V:
        case ID_LLC3KW240_7280V:
            Mask480VACOn;               // MODIF R2.1
        break;
        
        case ID_LLC3KW600_243648V:      // 600VAC
        case ID_LLC3KW600_7280V:
            Mask480VACOn;
        break;
        
        default:
            Mask480VACOn;
        break;
    }

	i = 0;
	do
	{	
        ModTabImax[i] = ModFindIMaxModuleID(LLCRamRO[i].Id, LLCRamRO[i].Ver[0]);    // MODIF R2.1
		i++;
	} while (i < MODNB);

}


/**
 * @brief  Calculate Vbat when charger is in critical fault
 * @param
 *
 * @retval
 */
void ModCalcDef(void)
{
	ulong VBat;
	ulong VBatMod;
	ulong VbatCell;
	uint i, j;
	i = 0;
	j = 0;
	VBat = 0;
	VBatMod = 0;

	do
	{
		if (CanErr(i) == 0)
		{
            if (LLCRamRO[i].VBat >= 0)
            {
			    VBat += LLCRamRO[i].VBat;
			    VBatMod += LLCRamRO[i].VFuse;
            }
			j++;
		}
		/*if (ModPwmMin(i) != 0)
      ChargeDisconnectOn;*/
		i ++;
	}
	while (i < MODNB);
	DataR.Ibat = 0;
	DataR.Vbat = VBat / j;
	DataR.VbatMod = VBatMod / j;
	VbatCell = ((ulong)DataR.Vbat * 10L) / (ulong)Menu.NbCells;
	DataR.VbatCell = VbatCell;
}


/**
 * @brief  Calculate Vbat, Ibat when charger works
 * @param
 *
 * @retval
 */
void ModCalc(void)
{
	/* Calculates values */
	/* Modules : 1 to ModCurr */
	uint IShuntLeft;                        /* Current in left shunts */
	uint IShuntRight;                       /* Current in right shunts */
	uint Power;
	slong VBat;                             /* VBat no weighted */
	slong VBatMod;
	slong VBatW;                            /* VBat weighted with current */
	slong VBatWMod;                         /* VCharger weighted with current */
	slong VbatCell;
	long u;
	ulong uMod;

	uchar i;

	uchar FlagDisableCharge;

	IShuntLeft = 0;
	IShuntRight = 0;
	ICons = 0;
	Power = 0;

	if (Menu.NbModLeft < 3)
		Menu.NbModLeft = 3;

	/* Calculates current first */
	i = 0;

	while ((i < ModCurr) && (i < MODNB))         /* Must take ModCurr module */
	{
		if (ModIndex[i] < Menu.NbModLeft)
		{
			IShuntLeft += LLCRamRO[ModIndex[i]].IBat;
		}
		else
		{
			IShuntRight += LLCRamRO[ModIndex[i]].IBat;
		}
		// MODIF 3.3 : lifenetwork
		if ((sint)LLCRamRO[ModIndex[i]].Power > 0)
			Power += LLCRamRO[ModIndex[i]].Power;
		ICons += (LLCRamRO[ModIndex[i]].ISlope >> 16);
		i ++;
	}

	ILeft = IShuntLeft;
	IRight = IShuntRight;
    
    if (ChgData.Profile != TEST)
	    DataR.Ibat = ILeft + IRight;            /* 1/10 A */
    else
    {   // MODIF R2.3 for TEST profile
        i = 0;
        IShuntLeft = 0;
        while (i < MODNB) 
        {
            IShuntLeft += LLCRamRO[i].IBat;
            i++;
        }
        DataR.Ibat = IShuntLeft;
    }    
    
    // MODIF R2.6
    if (Menu.FloatingOffOn == ON)
        IbatIQ = DataR.Ibat - Menu.Ifloating;
    else
        IbatIQ = DataR.Ibat;
	
    if (((IbatOld - ChgData.ThrlddIdT - 50) > (sint)DataR.Ibat) && (ChgData.Profile != IONIC) && (State.Charge < StateChgEndLoop))
	{
		ChgData.IdIdTOld = Menu.Imax;
	}
	IbatOld = DataR.Ibat;
	
	// MODIF 3.3 : lifenetwork
	if (PowerCut != 0)
		Power = 0;
		
	DataR.Power = Power;
	DataW.Iconsign = ICons * 100;
	VBat = 0;
	VBatMod = 0;
	i = 0;

	while ((i < ModCurr) && (i < MODNB))         /* Must take ModCurr module */
	{
        if (LLCRamRO[ModIndex[i]].VBat >= 0)
        { 
		    VBat += LLCRamRO[ModIndex[i]].VBat;
		    VBatMod += LLCRamRO[ModIndex[i]].VFuse;
        }
		i ++;
	}

	/* Voltage : average weighted with current (Ishunt) */
	VBatW = 0;
	VBatWMod = 0;
	i = 0;
	while ((i < ModCurr) && (i < MODNB))         /* Must take ModCurr module */
	{
		uMod = LLCRamRO[ModIndex[i]].VBat;
		uMod = LLCRamRO[ModIndex[i]].IBat * uMod;
		if (ModIndex[i] < Menu.NbModLeft)
		{
			/* Voltage */
			u = LLCRamRO[ModIndex[i]].VBat;
			/* Cable compensation */
			u = u - ((ulong)ILeft * (ulong)RcableLeft) / 1000L;
			/* Current weighted */
			u = LLCRamRO[ModIndex[i]].IBat * u;
		}
		else
		{
			/* Voltage */
			u = LLCRamRO[ModIndex[i]].VBat;
			/* Cable compensation */
			u = u - ((ulong)IRight * (ulong)RcableRight) / 1000L;
			/* Current weighted */
			u = LLCRamRO[ModIndex[i]].IBat * u;
		}
		/* Test if positive value */
		if (0 < u)
			VBatW += u;

		VBatWMod += uMod;
		i++;
	}

    if ((VBatW < 0) || (VBatWMod < 0))
    {
        VBatW = 0;
        VBatWMod = 0;
    }

	// Check fil pilot for all module connected
	// MODIF R2.7
    //FlagDisableCharge = 0;
    FlagDisableCharge = 1;
	i = 0;
	// MODIF R2.7
    //while ((i < MODNB) && (FilPil != 0))
    while (i < MODNB)
	{
		// MODIF R2.7 : VLMFB
        if (((LLCRamRO[i].VLmfb * 10) > (VbatThrld*Menu.NbCells)) && (CanErr(i) == 0) && (LLCWarn(i) == 0))
        //if (((LLCRamRO[i].VLmfb * 10) < (VbatThrld*Menu.NbCells)) && (CanErr(i) == 0))
        // MODIF R2.7
        //FlagDisableCharge = 1;
        FlagDisableCharge = 0;
        i++;
	}
    

	/* Calculation of VBat and VCharger */
    if ((State.Charge == StateChgAvail) 
        && (StateIdle != 0))    // MODIF R2.2
    {
        DataR.Vbat = 0;
        DataR.VbatMod = 0;
        DataR.Ibat = 0;
    }
	// MODIF R2.7
    //else if ((FilPil != 0) && (FlagDisableCharge == 1))
    else if (FlagDisableCharge == 1)
	{
		DataR.Vbat = 0;   /* Vbat = 0 if FilPilot disconnected*/
		DataR.VbatMod = 0;
	}
	else if ((10 <= DataR.Ibat) && (VBatW != 0)) /* Min current for correction */
	{ // change level 10A to 1A
		DataR.Vbat = (VBatW / DataR.Ibat);// - 30;
		DataR.VbatMod = (VBatWMod / DataR.Ibat);// - 30;
	}
	else
	{
		if (ModCurr > 0)
		{
			DataR.Vbat = VBat / (ulong)ModCurr;
			DataR.VbatMod = VBatMod / (ulong)ModCurr;
		}
		else
		{
			DataR.Vbat = VBat;
			DataR.VbatMod = VBatMod;
		}
	}
	VbatCell = ((ulong)DataR.Vbat * 10L) / (ulong)Menu.NbCells;
	DataR.VbatCell = VbatCell;

	/* Calculation of ItoReach  */
	if ((ICons <= DataW.Ireq) && (ICons >= (DataW.Ireq - 10)))
	{
		ItoReach = DataW.Ireq;
	}
	else if ((ICons >= DataW.Ireq) && (ICons <= (DataW.Ireq + 10)))
	{
		ItoReach = DataW.Ireq;
	}
	else if ((VbatCellCharger > DataW.VreqCell) && (DataW.VreqCell != 0))
    {
        if (ChgData.TimerSecCharge > TimeCheckVcell)
            ItoReach = DataW.Ireq;
    }
    else
    {
        TimeCheckVcell = ChgData.TimerSecCharge + 2L;
    }

	// MODIF R2.2
	if ((ICons <= ModImax) && (ICons >= (ModImax - 10)))
	{
		if (ICons < DataW.Ireq + 50)
            ItoReach = DataW.Ireq;
	}
	else if ((ICons >= ModImax) && (ICons <= (ModImax + 10)))
	{
		ItoReach = DataW.Ireq;
	}

	/* VCharger calculation */
	DataR.Vcharger = DataR.VbatMod;
	DataR.Vfuse = DataR.VbatMod;
	VbatCellCharger = ((ulong)DataR.Vcharger * 10L) / (ulong)Menu.NbCells;
}


/**
 * @brief  List all available modules and add into ModIndex
 * @param
 *
 * @retval
 */
void ModList(void)
{
	uchar i;
	uchar j;
	uchar k;
	uint ModImaxTmp;
	sint TambTmp;
	uchar PrevModNumber;
	// Presence of module
    // MODIF 2.8
	//if (State.TimerSec < 1)
	//	PMod = PM1 + (PM2 << 1) + (PM3 << 2) + (PM4 << 3) + (PM5 << 4) + (PM6 << 5) + (PM7 << 6) + (PM8 << 7);
    if (State.TimerSec < 1)
    {
        if (FlagInit == 0)  // check mod only at startup
		    PMod = PM1 + (PM2 << 1) + (PM3 << 2) + (PM4 << 3) + (PM5 << 4) + (PM6 << 5) + (PM7 << 6) + (PM8 << 7);
    }
    else
        FlagInit = 1;
        
	/* Set critical faults */
	i = 0;
	PrevModNumber = 0;
	ModNumberTmp = 0;	// MODIF R3.1
	do
	{
		LLC[i].TambTmp = (schar)((LLCRamRO[i].Temp[0] >> 8) - 64); // MODIF TH-AMB
		/* Test for critical faults */
		if ( (ModDefTempStop(i) != 0)
                || (ModDefTempCon(i) != 0) // MODIF R3.1
                || (LLCWarn(i) != 0)    // MODIF VLMFB
				|| (ModPwmMax(i) != 0)
				|| (ModOtherDef(i) != 0)
                || (ModVFuse(i) != 0)     // MODIF R2.2
				|| (LLC[i].CntDefTemp > MAX_MOD_DEF_TEMP)
				|| (LLC[i].CntDefCur > MAX_MOD_DEF_CUR))
		{
            // MODIF 2.8
            if (ModCanColision(i) != 0)
                ModSimpleAckDef(i);
                
            if ((ModDefTempStop(i) != 0) && (LLCErr(i) == 0))
			{
				LLC[i].CntDefTemp++;
				ModResetEfficiency();
				ResetDvDt();
			}
            
			if (((ModPwmMax(i) != 0) || (ModOtherDef(i) != 0)) && (LLCErr(i) == 0))
			{
				LLC[i].CntDefCur++;
				ModResetEfficiency();
				ResetDvDt();
			}
			//LLCErrOn(i);
			//LLCRamRW[i].IReq = 0;
            // MODIF R2.5
            if ((State.Charger <= StateChgerStChg) && (ModVFuse(i) != 0))
            {
                LLCErrOff(i);
            }
            else
            {
                // MODIF R2.7 : module in error only if no Idle state (due to Imax refresh on screen)
                //LLCErrOn(i);
			    //LLCRamRW[i].IReq = 0;
                if (State.Charger > StateChgerIdle)
                {
					if ((LLCErr(i) == 0) && (ModDefTempStop(i) != 0))
					{	// MODIF R3.1
						// TH Module management
						Status.NbThModule++;
						StatusWriteOn;
					}
					// MODIF R3.3 : save memo when module start in Fault (only in charge, filter disconnection)
					if ((LLCErr(i) == 0) && (State.Charger > StateChgerStChg) && ((LLCRamRO[i].VLmfb * 10) >= (VbatThrld*Menu.NbCells)))
						SaveE1Memo(i);

                    LLCErrOn(i);
			        LLCRamRW[i].IReq = 0;
                }
                else
                {
                    LLCErrOff(i);
                }
            }
		}
		// Test if Vlmfb bit is set in charge
		else if ( ((State.Charge & 0x0F) > StateChgInit)
				&& (State.Charger > StateChgerStChg)
				&& (ModVLmfb(i) != 0))
		{
			ModSimpleAckDef(i);
			ChgData.IdIdTOld = Menu.Imax;
			ResetDvDt();
		}
		else
		{
			/* Reset Current for dIdT analyse when module restart after a critical fault */
			if (LLCErr(i) != 0)
			{
				ModResetEfficiency();
				ResetDvDt();
				ChgData.IdIdTOld = Menu.Imax;
                ChgData.TimedIdT += (2*60L);        // MODIF 2.8 : delayed next didt measurement
			}
			LLCErrOff(i);
		}
		i ++;
	}
	while (i < MODNB);

	/* Fill all ModIndex, no communication error, no internal error*/
	i = 0;                            /* Index of physical module */
	j = 0;                            /* Index of available module */
	k = 0;
	ModImaxTmp = 0;
	TambTmp = 0;

	do
	{
		ModIndex[i] = MODNB;
		if (CanErr(i) == 0)
		{
			PrevModNumber++;
			ModNumberTmp++;	// MODIF R3.1
			if ((LLCRamRW[i].IReq != 0) && (LLCRamRW[i].VReq != 0))
			{
				TambTmp += (schar)((LLCRamRO[i].Temp[0] >> 8) - 64); // To be modified with correct input
				k++;
			}
			if (LLCErr(i) == 0)
			{
				ModIndex[j] = i;              /* No error, add module to list */
				ModImaxTmp += ModTabImax[i];  /* Calculate Modular Imax */
				j ++;
			}
		}
		else
		{
			if ((PMod & (1 << i)) == 0)
				PrevModNumber++;
			LLCErrOff(i); // clear LLCErr status because in CAN error
		}
		i ++;
	}
	//while (i < MODNB);
	while (i < (MODNB-1));  // MODIF 2.8 : must stop to 8 (without PrevModNumber is increase by 1 due to test ((PMod & (1 << i)) == 0) 

    // MODIF 2.8
    ModIndex[8] = MODNB;
    ModIndex[9] = MODNB;
        
	if (PrevModNumber > ModNumber)
		ModNumber = PrevModNumber;
    
    // MODIF 2.8 : add ModNumber limitation
    if  (ModNumber > 8)
        ModNumber = 8;    

	/* Update ModCurr pointer if a module is disconnected during charge */
	while ((ModIndex[ModCurr-1] == MODNB) && (ModCurr > 0))
	{
		IbatModMem[ModIndex[ModCurr - 1]] = 0;
		ModCurr--;
	}

	if (k > 0)
		Tambiant = (schar)(TambTmp / k);
	else
	{
		if (ModIndex[0] != MODNB)
			Tambiant = (schar)((LLCRamRO[ModIndex[0]].Temp[0] >> 8) - 64);
		else
			Tambiant = 0;
	}

	/* Update Imax only if Idle mode is OFF */
    // MODIF R2.5
	//if (StateIdle == OFF)
    if ((StateIdle == OFF) || (State.Charger == StateChgInit))
	{
        // MODIF R2.5
		//if ((TimeCheckIdle < State.TimerMs) && (ChargeDisconnect == 0))
        if ((TimeCheckIdle < State.TimerSec) && (ChargeDisconnect == 0))
		{
			ModImax = ModImaxTmp;
            // MODIF R2.5
			//TimeCheckIdle = State.TimerMs + 500;
            TimeCheckIdle = State.TimerSec+ 1L;
		}
	}
	else
        // MODIF R2.4
		//TimeCheckIdle = State.TimerMs + 4000;
        TimeCheckIdle = State.TimerSec + 8L;

	if (Menu.ImaxCharger == 0)
		Menu.ImaxCharger = 3200;

	if ((Menu.ImaxCharger < ModImax) && (State.Charger > StateChgerIdle))
	{
		Menu.Imax = Menu.ImaxCharger;
	}
	else
		Menu.Imax = ModImax;

	/* Select first module if none selected, put to 0 if no module */
	if (ModIndex[0] == MODNB)
		ModCurr = 0;
	else
	{
		if (ModCurr == 0)
		{
			ModCurr = 1;
		}
	}
}

/**
 * @brief  New Share current methode : use all module for loop and regulation
 *         Manage ModCur pointer to Voltage regulation module
 * @param
 *
 * @retval
 */
 
void ModShare(void)
{
    uint i,j;
    uint irem, irem_thrld;
    uint iTotal;
    uint iCompLoop;
	uint ireqtmp;
    
    i = 0;
    j = 0;

    while (ModIndex[i] != MODNB)
    {
			
        if (IbatModMem[ModIndex[i]] == 0)
        {
            j++;
        }
        i++;
    }
    if (j > ModCurr)
    ModCurr = j;
    else
    ModCurr = i;

    if (FlagLoop == ON)
    {
        ModResetEfficiency();
        ModCurr = i;
    }
    else if (IreqOld != DataW.Ireq)
    {
        IreqOld = DataW.Ireq;
        ModResetEfficiency();
        ModCurr = i;
    }

    if (Menu.ChgType == TYPE_1PHASE)
        irem_thrld = (ModFindIMaxModuleID(LLCRamRO[ModIndex[ModCurr - 1]].Id, LLCRamRO[ModIndex[ModCurr - 1]].Ver[0]) * THRLD_IMAX)/100; // MODIF R2.1
    else
        irem_thrld = (ModFindIMaxModuleID(LLCRamRO[ModIndex[ModCurr-1]].Id, LLCRamRO[ModIndex[ModCurr-1]].Ver[0]) * 80)/100;  // MODIF R2.1

    // Adjust ModCurr to requested current (not in loop & IQ Scan/Link)
    while ( (DataW.Ireq <= (((ModCurr-1) * irem_thrld) + MIN_CUR_STOP))
    && (FlagLoop == OFF)
    && (ModCurr > 1)
    // MODIF 2.8 : adjust mod curr in IQScan & IQ Link due to 8 Bay / 2 outputs
    //&& (State.Charge >= StateChgStartPhase1))
    && (State.Charge > StateChgInit))
    // END
    {
        ModCurr--;
        if (ModCurr == 1)
        {
            LLCRamRW[ModIndex[ModCurr - 1]].IReq = DataW.Ireq;
            LLCRamRW[ModIndex[ModCurr - 1]].Slope = (SlopeI / ModCurr) / 10;
        }
    }

	// MODIF 2.8 : IQScan / IQLink
	if ( (FlagLoop == OFF) 
		&& ((State.Charge == StateChgIQScan) || (State.Charge == StateChgIQLink))
		&& ((AarsParam.NbModLeft == OUTPUT_2_CABLE_6M) || (AarsParam.NbModLeft == OUTPUT_2_CABLE_8M)) )
	{
		if ((ModIndex[1] >= Menu.NbModLeft) && (ModCurr > 1))
		{
			ModCurr = 1;
			if (DataW.Ireq > ModFindIMaxModuleID(LLCRamRO[ModIndex[0]].Id, LLCRamRO[ModIndex[0]].Ver[0]))	
				DataW.Ireq = ModFindIMaxModuleID(LLCRamRO[ModIndex[0]].Id, LLCRamRO[ModIndex[0]].Ver[0]);
		}
		if ((ModIndex[0] >= Menu.NbModLeft) && (State.Charge == StateChgIQScan))
		{
			State.Charge = StateChgStartPhase1;
			State.Phase = StatePh1;
		}
	}
	
    irem = DataW.Ireq / ModCurr;
    iTotal = DataW.Ireq;

    // Share current to respect THRLD_IMAX value on requested current (not in LOOP and IQ Scan/Link)
    // MODIF 2.8 : Share current in IQ Scan/Link due to 8 bay / 2 outputs
    //if ((irem < irem_thrld) && (FlagLoop == OFF) && (State.Charge >= StateChgStartPhase1))
    if ((irem < irem_thrld) && (FlagLoop == OFF) && (State.Charge > StateChgInit))
    // END
    {
        i = 0;
     
        while (iTotal > irem_thrld)
        {
            iTotal -= irem_thrld;
            LLCRamRW[ModIndex[i]].IReq = irem_thrld;
            LLCRamRW[ModIndex[i]].Slope = (SlopeI / ModCurr) / 10;
            i++;
        }
        LLCRamRW[ModIndex[i]].IReq = iTotal;
        LLCRamRW[ModIndex[i]].Slope = (SlopeI / ModCurr) / 10;
        ModCurr = (i + 1);
    }
    else
    {
        // Share requested current beetween connected module (only if Thrld current limitation is off)
        i = 0;
        // No sharing if Ireq < 10A & IBat < 10A  -> use ModCurr = 1
        iCompLoop = ModFindIMaxModuleID(LLCRamRO[ModIndex[0]].Id, LLCRamRO[ModIndex[0]].Ver[0]) / 2;
        if (iCompLoop > 250)  // 25A max
        iCompLoop = 250;
        else if (iCompLoop < 100)
        iCompLoop = 100;

        if ((DataW.Ireq < 100) && (DataR.Ibat < iCompLoop))
        {
            ModCurr = 1;
            irem = DataW.Ireq;
        }
        if (FlagEfficiency == 0)
        {
            do
            {
                if (ModTabImax[ModIndex[i]] <= irem) // MODIF R2.2
                {
                    LLCRamRW[ModIndex[i]].IReq = ModTabImax[ModIndex[i]];       /* Fill modules with IMax */
                }
                else
                    LLCRamRW[ModIndex[i]].IReq = irem;
                    
                LLCRamRW[ModIndex[i]].Slope = (SlopeI / ModCurr) / 10;
                i ++;
            }
            while (i < ModCurr);
        }
    }


	// MODIF 2.8 : bug with PwmMin in loop (not take PwmMin information during the loop)
    //while ( (ModPwmMin(ModIndex[ModCurr - 1]) != 0) ||
	// MODIF 3.2 : fix bug with ModCurr management when PwmMin occur
	//while ( ((ModPwmMin(ModIndex[ModCurr - 1]) != 0)  && (FlagLoop == OFF)) ||
	while ( ((ModPwmMin(ModIndex[ModCurr - 1]) != 0)  && (FlagLoop == OFF) && (ModCurr > 1)) ||
        ( (LLCRamRO[ModIndex[ModCurr - 1]].IBat < MIN_CUR_STOP)
        && (IbatModMem[ModIndex[ModCurr - 1]] >= MIN_CUR_STOP)
        && (ModCurr > 1) && (FlagLoop == OFF)
        && (ItoReach == DataW.Ireq)
        // MODIF 2.8 :
        //&& (State.Charge >= StateChgStartPhase1) ))
        && (State.Charge > StateChgInit) )) 
        // END
    {
        IbatModMem[ModIndex[ModCurr - 1]] = MIN_CUR_STOP;
        ModCurr --;
        // Set IReq to Max current when a module is removed
        LLCRamRW[ModIndex[ModCurr - 1]].IReq = ModFindIMaxModuleID(LLCRamRO[ModIndex[ModCurr - 1]].Id, LLCRamRO[ModIndex[ModCurr - 1]].Ver[0]);
		// MODIF 2.8 : check Ireq limitation when change module
		ireqtmp = 0;
		i = 0;
		for (i=0; i<ModCurr; i++)
		{
			ireqtmp += LLCRamRW[ModIndex[i]].IReq;
		}
		if (ireqtmp > DataW.Ireq)
		{
			LLCRamRW[ModIndex[ModCurr - 1]].IReq -= (ireqtmp - DataW.Ireq);
		}	
        
    }
    
    // MODIF 2.8
    if ((ModCurrSave != ModCurr) 
        && (State.Charge == StateChgPhase2)
        && (FlagLoop == OFF))
    {
        ModCurrSave = ModCurr;
        if ((Memo.TimePhase2 != 0) && (ChgData.TimerSecCharge > TimeCheckRcable))
        {
            TimeCheckRcable = ChgData.TimerSecCharge - 1L;
            //VReqMem = LLCRamRO[ModIndex[ModCurr - 1]].VBat;
            //VcblMem = (((ulong)ILeft * (ulong)RcableLeft) / 1000L) + (((ulong)IRight * (ulong)RcableRight) / 1000L);
            VcblMem = (slong)LLCRamRO[ModIndex[ModCurr - 1]].VBat - (((slong)DataW.VreqCell * (slong)Menu.NbCells)/10L);
            if (VcblMem > 0)
            {
                if (ModIndex[ModCurr - 1] > 3)
                    RcblMem = ((ulong)VcblMem * 1000L) / (ulong)IRight;
                else
                    RcblMem = ((ulong)VcblMem * 1000L) / (ulong)ILeft;    
            }
        }
    }


    if ((LLCRamRO[ModIndex[ModCurr - 1]].IBat >= MIN_CUR_STOP)
            && (FlagLoop == OFF)
            // MODIF 2.8
            //&& (State.Charge >= StateChgStartPhase1))
            && (State.Charge > StateChgInit))
            // END
    IbatModMem[ModIndex[ModCurr - 1]] = LLCRamRO[ModIndex[ModCurr - 1]].IBat;

    // Modif R2.2 : protection is Vbat is over regulation point to remove module (10mV/Cell)
    if ( (((LLCRamRW[ModIndex[ModCurr - 1]].VReq * 10) + (10 * Menu.NbCells)) < (LLCRamRO[ModIndex[ModCurr - 1]].VBat * 10)) 
            && (State.Charger > StateChgerStChg) 
            && (State.Charge > StateChgStartPhase1)
            && (FlagLoop == OFF)
            && (LLCRamRW[ModIndex[ModCurr - 1]].VReq != 0)
            && (LLCRamRW[ModIndex[ModCurr - 1]].IReq != 0))
    {
        if ((State.TimerSec > (TimeDefVreg+8L)) 
        && (LLCRamRO[ModIndex[ModCurr - 1]].IBat < MIN_CUR_STOP)
        && (ModCurr > 1))
        {
           IbatModMem[ModIndex[ModCurr - 1]] = MIN_CUR_STOP;
           ModCurr--;
           TimeDefVreg = State.TimerSec;
           ItoReach = DataW.Ireq;
        }
        else if ((State.TimerSec > (TimeDefVreg+120L))
        && (ModCurr > 1))
        {
           IbatModMem[ModIndex[ModCurr - 1]] = MIN_CUR_STOP;
           ModCurr--;
           TimeDefVreg = State.TimerSec;              
           ItoReach = DataW.Ireq;
        }
    }
    else
    {
        TimeDefVreg = State.TimerSec;
    }

	// Limit Ireq Module to irem_thrld
	if ((ModCurr > 1) && (LLCRamRO[ModIndex[ModCurr-1]].IBat < irem_thrld)
			&& (DataW.Ireq != 0)
			&& (ItoReach == DataW.Ireq)
			&& (FlagLoop == OFF)
			&& (State.Charge >= StateChgStartPhase1))
	{
		if (TimeCheckEfficiency < State.TimerMs)
		{
			i = 0;
			j = 2;
			while ((i == 0) && ( j <= ModCurr))
			{
                if (LLCRamRW[ModIndex[ModCurr-j]].IReq > irem_thrld)
				{
					LLCRamRW[ModIndex[ModCurr-j]].IReq --;
					FlagEfficiency = 1;
					i = 1;
				}
				else
					j++;
			}
            if (Menu.ChgType == TYPE_1PHASE)
            {
			    //TimeCheckEfficiency = State.TimerMs + 500L;
                TimeCheckEfficiency = State.TimerMs + (1000L / ModCurr);
            }    
            else
            {
                //TimeCheckEfficiency = State.TimerMs + 1000L;
                // MODIF 2.8
                #ifdef DEF8BAY
                    TimeCheckEfficiency = State.TimerMs + (4000L / ModCurr);
                #else
                    TimeCheckEfficiency = State.TimerMs + (2000L / ModCurr);
                #endif    
            }
		}
	}
	else
    {
		if (Menu.ChgType == TYPE_1PHASE)
        {   
            if (ModCurr > 1)
            TimeCheckEfficiency = State.TimerMs + (500L / ModCurr);
            else
	        TimeCheckEfficiency = State.TimerMs + 500L;
        }
        else
        {
            if (ModCurr > 1)
            TimeCheckEfficiency = State.TimerMs + (2000L / ModCurr);
            else
	        TimeCheckEfficiency = State.TimerMs + 1000L;
        }
    }
    
    if ((Menu.ChgType == TYPE_3PHASE) 
			&& (DataW.Ireq != 0)
			&& (ItoReach == DataW.Ireq)
			&& (FlagLoop == OFF)
			&& (State.Charge > StateChgStartPhase1))
        CheckDerating();
    else if ((Menu.ChgType == TYPE_1PHASE) 
			&& (DataW.Ireq != 0)
			&& (ItoReach == DataW.Ireq)
			&& (FlagLoop == OFF)
			&& (State.Charge > StateChgStartPhase1))
        CheckDeratingSinglePhase();

    // Reset Efficiency if IBat last module if higher than Ibat(module -1) + 4A
    if ( (FlagLoop == OFF)
            && (LLCRamRO[ModIndex[ModCurr-1]].IBat >= (LLCRamRO[ModIndex[ModCurr-2]].IBat + 30)) // MODIF R2.2
            && (((LLCRamRW[ModIndex[ModCurr - 1]].VReq * 10) - (12 * Menu.NbCells)) > (LLCRamRO[ModIndex[ModCurr - 1]].VBat * 10)) 
            && (FlagEfficiency == 1)
            && (ModCurr > 1)
            && (State.Charge >= StateChgStartPhase1)
            && (FlagDerating == 0)
			&& (ChgData.Profile != LITH))         // MODIF 2.8 : derating
    {   
        if ((TimeModCur + 60L) < State.TimerSec)
        {
            ModResetEfficiency();
            TimeModCur = State.TimerSec;
        }
    }
    // Reset Efficiency if FlagEfficiency is set and VbatModule under voltage regulation (12mV/Cells) // MODIF R2.2
    else if ( (FlagLoop == OFF)
			&& (((LLCRamRW[ModIndex[ModCurr - 1]].VReq * 10) - (12 * Menu.NbCells)) > (LLCRamRO[ModIndex[ModCurr - 1]].VBat * 10)) 
			&& (LLCRamRO[ModIndex[ModCurr-1]].IBat >= (LLCRamRW[ModIndex[ModCurr - 1]].IReq - 30))
            && (DataW.Ireq >= (ModFindIMaxModuleID(LLCRamRO[ModIndex[ModCurr-1]].Id, LLCRamRO[ModIndex[ModCurr - 1]].Ver[0]) * (ModCurr - 1)))
			&& (State.Charge >= StateChgStartPhase1)
            && (FlagEfficiency == 1)
            && (ModIndex[ModCurr] != MODNB) 
            && (FlagDerating == 0)
			&& (ChgData.Profile != LITH))         // MODIF 2.8 : derating
	{
		if ((TimeModCur + 60L) < State.TimerSec)
		{
			// Use next module to increase current
            ModCurr++;
            LLCRamRW[ModIndex[ModCurr - 1]].IReq = ModFindIMaxModuleID(LLCRamRO[ModIndex[ModCurr-1]].Id, LLCRamRO[ModIndex[ModCurr - 1]].Ver[0]);
			IbatModMem[ModIndex[ModCurr - 1]] = 0;
            TimeCheckEfficiency = State.TimerMs + 5000L;
            TimeModCur = State.TimerSec;
		}
	}
    // Reset Efficiency if FlagEfficiency is set and VbatModule under voltage regulation (12mV/Cells) // MODIF R2.2
    else if ( (FlagLoop == OFF)
			&& (((LLCRamRW[ModIndex[ModCurr - 1]].VReq * 10) - (14 * Menu.NbCells)) > (LLCRamRO[ModIndex[ModCurr - 1]].VBat * 10)) 
			&& (State.Charge >= StateChgStartPhase1)
            && (FlagEfficiency == 1)
            && (FlagDerating == 0)
			&& (ChgData.Profile != LITH))         // MODIF 2.8 : derating
	{
      if ((TimeModCur + 120L) < State.TimerSec)
        {
            ModResetEfficiency();
            TimeModCur = State.TimerSec;
        }  
    }
    else
        TimeModCur = State.TimerSec;

}

/**
 * @brief  Distribute voltage over active modules
 *         DataW.VreqCell : requested voltage
 *         ModCurr - 1 : last active module in ModIndex
 * @param
 *
 * @retval
 */
void ModV(void)
{
	uchar i;

	i = 0;
	do
	{
		if ((i + 1) < ModCurr)
		{
			//VReq = (ulong)((ulong)DataW.VreqCell * (ulong)Menu.NbCells)/10L;
			/* Calculates battery voltage */
			if (ChgData.Profile != LITH)
				VReq = (ulong)((ulong)DataW.VreqCell * (ulong)Menu.NbCells)/10L;
			else if (Converter != 0)
				VReq = Bms.ChrgrDcU;
			else	
				VReq = 0;
					
			if (ChgData.Profile == IONIC)
			{
				Vcbl = 0;
			}
			else if (ModIndex[i] < Menu.NbModLeft)
			{
				Vcbl = ((ulong)ILeft * (ulong)RcableLeft) / 1000L;
                //Vcbl += ((ulong)LLCRamRO[ModIndex[i]].IBat * (ulong)RES_CONNECTEUR_PM) / 1000L;
			}
			else
			{
				Vcbl = ((ulong)IRight * (ulong)RcableRight) / 1000L;
                //Vcbl += ((ulong)LLCRamRO[ModIndex[i]].IBat * (ulong)RES_CONNECTEUR_PM) / 1000L;
			}
			

			if (ChargeDisconnect != 0)
			{
				VReq = 0;
				Vcbl = 0;
			}
			// add 1V on other module
			if ((DataW.VreqCell != 0) && (ChgData.Profile != LITH))
				LLCRamRW[ModIndex[i]].VReq = VReq + Vcbl + 100;
			else if ((Converter != 0) && (VReq != 0))
				LLCRamRW[ModIndex[i]].VReq = VReq + Vcbl + 100;
			else
				LLCRamRW[ModIndex[i]].VReq = 0;
		}
		else
		{
			// Module for voltage regulation
			if ((i + 1) == ModCurr)
			{
				/* Calculates battery voltage */
				//VReq = (ulong)((ulong)DataW.VreqCell * (ulong)Menu.NbCells)/10L;
				/* Calculates battery voltage */
				if (ChgData.Profile != LITH)
					VReq = (ulong)((ulong)DataW.VreqCell * (ulong)Menu.NbCells)/10L;
				else
					VReq = Bms.ChrgrDcU;

				if (ChgData.Profile == IONIC)
				{
					Vcbl = 0;
				}
				else if (ModIndex[i] < Menu.NbModLeft)
				{
					Vcbl = ((ulong)ILeft * (ulong)RcableLeft) / 1000L;
                    //Vcbl += ((ulong)LLCRamRO[ModIndex[i]].IBat * (ulong)RES_CONNECTEUR_PM) / 1000L;
				}
				else
				{
					Vcbl = ((ulong)IRight * (ulong)RcableRight) / 1000L;
                    //Vcbl += ((ulong)LLCRamRO[ModIndex[i]].IBat * (ulong)RES_CONNECTEUR_PM) / 1000L;
				}
				

				if (ChargeDisconnect != 0)
				{
					VReq = 0;
					Vcbl = 0;
				}
				
				if ((DataW.VreqCell != 0) && (ChgData.Profile != LITH))
					LLCRamRW[ModIndex[i]].VReq = VReq + Vcbl;
				else if ((Converter != 0) && (VReq != 0))
					LLCRamRW[ModIndex[i]].VReq = VReq + Vcbl;
				else
					LLCRamRW[ModIndex[i]].VReq = 0;
			}
			else
			{
				LLCRamRW[ModIndex[i]].VReq = 0;
			}
		}

		/* Security for VReq : 2.90 V/e */
		if ((LLCRamRW[ModIndex[i]].VReq > (290 * Menu.NbCells)) && (ChgData.Profile != LITH))
			LLCRamRW[ModIndex[i]].VReq = 290 * Menu.NbCells;
		i ++;
	}
	while (i < MODNB);

}


/**
 * @brief  Distribute voltage over active modules
 *         DataW.VreqCell : requested voltage
 *         ModCurr - 1 : last active module in ModIndex
 * @param
 *
 * @retval
 */
void ModV_8BAY_R1(void)
{
	uchar i;

    Vcbl = (((ulong)ILeft * (ulong)RcblMem) / 1000L) + (((ulong)IRight * (ulong)RcblMem) / 1000L);
    
	i = 0;
	do
	{
		if ((i + 1) < ModCurr)
		{
			if (ChgData.Profile != LITH)
				VReq = (ulong)((ulong)DataW.VreqCell * (ulong)Menu.NbCells)/10L;
			else
				VReq = Bms.ChrgrDcU;

			/*if (ChgData.Profile == IONIC)
			{
				Vcbl = 0;
			}*/			

			if (ChargeDisconnect != 0)
			{
				VReq = 0;
				Vcbl = 0;
			}
            
			// add 1V on other module
			if ((DataW.VreqCell != 0) && (ChgData.Profile != LITH))
				LLCRamRW[ModIndex[i]].VReq = VReq + (Vcbl/2) + 100;
			else if ((Converter != 0) && (VReq != 0))
				LLCRamRW[ModIndex[i]].VReq = VReq + (Vcbl/2) + 100;
			else
				LLCRamRW[ModIndex[i]].VReq = 0;
		}
		else
		{
			// Module for voltage regulation
			if ((i + 1) == ModCurr)
			{
				/* Calculates battery voltage */
				if (ChgData.Profile != LITH)
					VReq = (ulong)((ulong)DataW.VreqCell * (ulong)Menu.NbCells)/10L;
				else
					VReq = Bms.ChrgrDcU;
               
				if (ChargeDisconnect != 0)
				{
					VReq = 0;
					Vcbl = 0;
				}
				
                
				if ((DataW.VreqCell != 0) && (ChgData.Profile != LITH))
                {
                    if (DataR.Vbat > VReq)
                        VoffsetComp = DataR.Vbat - VReq;
                    else
                        VoffsetComp = 0;
						
                    if (ModIndex[i] > 3)        
                        Vcbl = (((ulong)IRight * (ulong)RcblMem) / 1000L);
                    else
                        Vcbl = (((ulong)ILeft * (ulong)RcblMem) / 1000L);
						
					if (FlagVRegPhase1 == 1)
					{
						FlagVRegPhase1 = 0;
						VReq = LLCRamRO[ModIndex[i]].VBat - (slong)Vcbl;
						DataW.VreqCell = (VReq / Menu.NbCells) * 10;
					}	
                    LLCRamRW[ModIndex[i]].VReq = (slong)VReq + (slong)Vcbl - VoffsetComp;
				 }
				 else if ((Converter != 0) && (VReq != 0))
				 {
					if (DataR.Vbat > VReq)
                        VoffsetComp = DataR.Vbat - VReq;
                    else
                        VoffsetComp = 0;
						
                    if (ModIndex[i] > 3)        
                        Vcbl = (((ulong)IRight * (ulong)RcblMem) / 1000L);
                    else
                        Vcbl = (((ulong)ILeft * (ulong)RcblMem) / 1000L);	
                    LLCRamRW[ModIndex[i]].VReq = (slong)VReq + (slong)Vcbl - VoffsetComp;					 
				 }
				 else
					LLCRamRW[ModIndex[i]].VReq = 0;
					
			}
			else
			{
				LLCRamRW[ModIndex[i]].VReq = 0;
			}
		}

		/* Security for VReq : 2.90 V/e */
		if ((LLCRamRW[ModIndex[i]].VReq > (290 * Menu.NbCells)) && (ChgData.Profile != LITH))
			LLCRamRW[ModIndex[i]].VReq = 290 * Menu.NbCells;
		i ++;
	}
	while (i < MODNB);

}


/**
 * @brief  Distribute voltage over active modules
 *         DataW.VreqCell : requested voltage
 *         ModCurr - 1 : last active module in ModIndex
 * @param
 *
 * @retval
 */
void ModV_8BAY_R2(void)
{
	uchar i;

    Vcbl = (((ulong)ILeft * (ulong)RcableLeft) / 1000L) + (((ulong)IRight * (ulong)RcableRight) / 1000L);
    
	i = 0;
	do
	{
		if ((i + 1) < ModCurr)
		{
			VReq = (ulong)((ulong)DataW.VreqCell * (ulong)Menu.NbCells)/10L;

			if (ChgData.Profile == IONIC)
			{
				Vcbl = 0;
			}

			if (ChargeDisconnect != 0)
			{
				VReq = 0;
				Vcbl = 0;
			}
                        
			// add 1V or 0.6V on other module (depend of OUTPUT cable
			if (DataW.VreqCell != 0)
                LLCRamRW[ModIndex[i]].VReq = VReq + (Vcbl/2);
			else
				LLCRamRW[ModIndex[i]].VReq = 0;
		}
		else
		{
			// Module for voltage regulation
			if ((i + 1) == ModCurr)
			{
				/* Calculates battery voltage */
				VReq = (ulong)((ulong)DataW.VreqCell * (ulong)Menu.NbCells)/10L;

				if (ChgData.Profile == IONIC)
				{
					Vcbl = 0;
				}
				if (ChargeDisconnect != 0)
				{
					VReq = 0;
					Vcbl = 0;
				}
				if (DataW.VreqCell != 0)
                    LLCRamRW[ModIndex[i]].VReq = VReq + (Vcbl/2);
				else
					LLCRamRW[ModIndex[i]].VReq = 0;
			}
			else
			{
				LLCRamRW[ModIndex[i]].VReq = 0;
			}
		}

		if (LLCRamRW[ModIndex[i]].VReq > (290 * Menu.NbCells))
			LLCRamRW[ModIndex[i]].VReq = 290 * Menu.NbCells;
		i ++;
	}
	while (i < MODNB);
}

/**
 * @brief  Check fault on active modules
 * @param
 *
 * @retval
 */
void ModCheckDef(void)
{
	uchar i, j;

	uchar CntGLobalDefPwm;
	uchar CntGlobalDefFuse;
	uchar CntGlobalDefTempStop;
	uchar CntGlobalDefOther;
	uchar CntGlobalDefID;
	uchar NbrModule;

	uchar CntGlobalDefCon;			// MODIF R3.1

    // MODIF 2.8
    VbatLeft = 0;
    VbatRight = 0;
    NbrLeft = 0;
    NbrRight = 0;

	CntGLobalDefPwm = 0;
	CntGlobalDefFuse = 0;
	CntGlobalDefTempStop = 0;
	CntGlobalDefOther = 0;
	CntGlobalDefID = 0;
	CntGlobalDefCon = 0;			// MODIF 3.1
	ChargerID = ModChargerID();
	IDModule = 0;
	NbrModule = 0;

    // MODIF 2.8
	if (State.Charger < StateChgerChg)
		FlagDefBatCon = 0;

	i = 0;
	j = 0;
	ChargeDisconnectOff;

	do
	{
		// Presence of module
		if ((CanErr(i) != 0) && ((PMod & (1 << i)) != 0) && (i < ModNumber) && (StateIdle ==0))
		{
			j++;
		}

		if ( (ModPwmMax(i) != 0) && (CanErr(i) == 0) && (State.Charger > StateChgerIdle))
		{
			j++;
			CntGLobalDefPwm++;
			if ((LLC[i].TimeModAckDef < State.TimerSec) && (LLC[i].CntDefCur < MAX_MOD_DEF_CUR))
				ModSimpleAckDef(i);
		}
		else if ((ModOtherDef(i) != 0) && (CanErr(i) == 0) && (State.Charger > StateChgerIdle))
		{
			j++;
			CntGlobalDefOther++;
			if ((LLC[i].TimeModAckDef < State.TimerSec) && (LLC[i].CntDefCur < MAX_MOD_DEF_CUR))
				ModSimpleAckDef(i);
			// MODIF R3.1	
			if (ModDefTempCon(i) != 0)
				CntGlobalDefCon++;	
		}
		else
		{
			LLC[i].TimeModAckDef = State.TimerSec + TIME_DEF_CUR_WAIT;
		}

#ifndef NO_DEFID
        // MODIF NEWID                
		if (((LLCRamRO[i].Id != (ChargerID & LLCRamRO[i].Id)) 
            || (LLCRamRO[ModIndex[0]].Id != LLCRamRO[i].Id)
            || ((LLCRamRO[i].Id == ID_LLC3KW_3648VM) && (Menu.CellSize < 18)) )
            && (CanErr(i) == 0) && (LLCRamRO[i].TimerSec > 10L) && (ModIndex[0] != MODNB))
		{   
            if ((LLCRamRO[ModIndex[0]].Id == ID_LLC3KW_243648V) && (LLCRamRO[i].Id == ID_LLC3KW_243648VUS)) // MODIF R2.1
			{
                j++;    // Can work with module ID_LLC3KW_243648V &&  ID_LLC3KW_243648VUS
			    j--;
            }
            else if ((LLCRamRO[ModIndex[0]].Id == ID_LLC3KW_243648VUS) && (LLCRamRO[i].Id == ID_LLC3KW_243648V))
			{
                j++;    // Can work with module ID_LLC3KW_243648V &&  ID_LLC3KW_243648VUS
			    j--;
            }
            else
            {
                j++;
			    CntGlobalDefID++;
            }
		}

		if (CanErr(i) == 0)
		{
			IDModule += LLCRamRO[i].Id;
			NbrModule++;
		}

#endif

        if ((ModVFuse(i) != 0) && (CanErr(i) == 0) && (State.Charger > StateChgerIdle))
        {
	        j++;
	        CntGlobalDefFuse++;
        }
        if (((ModDefTempStop(i) != 0) || (LLC[i].CntDefTemp > MAX_MOD_DEF_TEMP)) && (CanErr(i) == 0))
        {
	        j++;
	        CntGlobalDefTempStop++;
        }

		if (((ModVBatMax(i) != 0) || (ModDisconnect(i) != 0)) && (CanErr(i) == 0) && (State.Charger > StateChgerIdle))
        {
            // MODIF R2.7 : VBAT
            if (LLCWarn(i) == 0)
	            ChargeDisconnectOn;
        }

        
        
        // MODIF R2.7 : VLMFB
        if ((LLCWarn(i) != 0) && (CanErr(i) == 0) && (State.Charger > StateChgerIdle))
        {
            j++;
        }

        
        // MODIF 2.8
        if ((i < Menu.NbModLeft) && (CanErr(i) == 0) /*&& (LLCErr(i) == 0)*/ && ((AarsParam.NbModLeft == OUTPUT_2_CABLE_6M) || (AarsParam.NbModLeft == OUTPUT_2_CABLE_8M)))
        {
            NbrLeft++;
            VbatLeft += LLCRamRO[i].VBat;
        }
        else if ((CanErr(i) == 0) /*&& (LLCErr(i) == 0)*/ && ((AarsParam.NbModLeft == OUTPUT_2_CABLE_6M) || (AarsParam.NbModLeft == OUTPUT_2_CABLE_8M)))
        {
            NbrRight++;
            VbatRight += LLCRamRO[i].VBat;
        }

        
        i++;
	}
	//while (i < MODNB);
	while (i < (MODNB-1));  // MODIF 2.8

    if (ChargeDisconnect == 0)
        TimerDisconnect = State.TimerSec;
        
    if ((TimerDisconnect + 6L) < State.TimerSec)
        ModAckDef();

	/* Fuse fault is set */
    if ((CntGlobalDefFuse > 0) && (ModCurr == 0))   // MODIF R2.2
	{
		if (State.TimerSec > TimeDefFuse + 2L)
			FuseDefOn;
	}
	else
	{
		TimeDefFuse = State.TimerSec;
		FuseDefOff;
	}

	// DefCur fault is set
	if (((CntGLobalDefPwm > 0) || (CntGlobalDefOther > 0)) && (ModCurr == 0))
	{
		ConverterDefOn;
	}
	else
	{
		ConverterDefOff;
	}

	// High Temp fault is set
	if ((CntGlobalDefTempStop > 0) && (ModCurr == 0))
	{
		TempHighDefOn;
	}
	else
	{
		TempHighDefOff;
	}
	
	// MODIF R3.1
	if ((CntGlobalDefCon != 0) && (CntGlobalDefCon == ModNumberTmp))
	{
		DefConOn;
		ConverterDefOn;
	}
	else
	{
		DefConOff; 
	}
	
    
    // MODIF 2.8
    if ((AarsParam.NbModLeft == OUTPUT_2_CABLE_6M) || (AarsParam.NbModLeft == OUTPUT_2_CABLE_8M))
    {
         if ((VbatLeft > VBAT_THRLD) && (VbatRight > VBAT_THRLD))
         {
            FlagDefBatCon = 0;
            TimeBatCon = State.TimerSec;
         }
         else
         {
            if (State.TimerSec > (TimeBatCon + 2L))
                FlagDefBatCon = 1;
         }
    }

#ifndef NO_DEFID
	// Module ID fault
	if ((CntGlobalDefID != 0) && (State.Charger > StateChgerInit))
	{
		if (State.TimerSec > TimerDefID)
			ModDefIDOn;
	}
	else
#endif
	{
		ModDefIDOff;
		if (IdleState == 0)
			TimerDefID = State.TimerSec + 2L;
		else
			TimerDefID = State.TimerSec + 12L;
	}

	if (NbrModule != 0)
		IDModule = IDModule / NbrModule;

	if (j == 0)
	{
		ModDefOff;
		if (IdleState == 0)
			TimerModDef = State.TimerSec + 1L;
		else
			TimerModDef = State.TimerSec + 10L;
	}
	else if (State.TimerSec > TimerModDef)
		ModDefOn;
}

/**
 * @brief  Check for a module in critical fault (DF1, DF2, TH...)
 * @param
 *
 * @retval
 */
uchar ModFault(void)
{
	uchar i;
	uchar FlagFault;
	i = 0;
	FlagFault = 0;
	do
	{
		if (LLCErr(i) != 0)
			FlagFault = 1;
		i++;
	}
	while (i < MODNB);

	return FlagFault;
}


/**
 * @brief  Ack fault bit into LLCRamRW.Status
 * @param
 *
 * @retval
 */
void ModAckDef(void)
{
	uchar i;
	i = 0;

	if (FuseDef != 0)
		return;
	do
	{
		if (ModDefTempStop(i) == 0)
			LLCRamRW[i].UnionStatus.Status |= 0x00000001;    // Ack Fault
		LLC[i].CntDefCur = 0;
		// MODIF R2.8 : not take a module who is in TH fault when charge start
        if (ModDefTempStop(i) != 0)
			LLC[i].CntDefTemp = MAX_MOD_DEF_TEMP + 1;
		else
			LLC[i].CntDefTemp = 0;
		LLC[i].TimeModAckDef = State.TimerSec + TIME_DEF_CUR_WAIT;
		i ++;
	}
	while (i < MODNB);
}

/**
 * @brief  Ack TemCon fault bit into LLCRamRW.Status
 * @param
 *
 * @retval
 */
void ModTempConAckDef(void)
{	// MODIF R3.1
	uchar i;
	i = 0;

	do
	{
		LLCRamRW[i].UnionStatus.Status |= 0x00000020;    // Ack Fault
		i ++;
	}
	while (i < MODNB);
}

/**
 * @brief  Clear def temps counter
 * @param
 *
 * @retval
 */
void ModClearDefTemp(void)
{
	uchar i;
	i = 0;
	do
	{
		LLC[i].CntDefTemp = 0;
		i ++;
	}
	while (i < MODNB);
}

/**
 * @brief  Ack fault bit into LLCRamRW.Status for selected module
 * @param
 *
 * @retval
 */
void ModSimpleAckDef(uchar i)
{
	if (ModDefTempStop(i) == 0)
		LLCRamRW[i].UnionStatus.Status |= 0x00000001;    // Ack Fault
}

/**
 * @brief  Reset Efficiency regulation
 * @param
 *
 * @retval
 */
void ModResetEfficiency (void)
{
	uint i;

	FlagEfficiency = 0;

	for (i=0; i< MODNB; i++)
	{
		IbatModMem[i] = 0;
	}
	TimeCheckEfficiency = 0;
	TimeCheckCurrent = ChgData.TimerSecCharge + (TIME_CHECK_CUR * 2L);
}


/**
 * @brief  Reset Ah counter
 * @param
 *
 * @retval
 */
void ModResetCntAh(void)
{
	uchar i;
	i = 0;
	do
	{
		LLCRamRO[i].Ver[0] = 0;
		i ++;
	}
	while (i < MODNB);
}


/**
 * @brief  Sort Module by Ah
 * @param
 *
 * @retval
 */
void ModSort(void)
{

	int n, c, d, swap;

	n = 0;

	for (c = 0; c < MODNB; c++)
	{
		if (ModIndex[n] != MODNB)
			n++;
	}

	for (c = 0 ; c < ( n - 1 ); c++)
	{
		for (d = 0 ; d < n - c - 1; d++)
		{
			if (LLCRamRO[ModIndex[d]].CntAh > LLCRamRO[ModIndex[d+1]].CntAh) /* For decreasing order use < */
			{
				swap          = ModIndex[d];
				ModIndex[d]   = ModIndex[d+1];
				ModIndex[d+1] = swap;
			}
		}
	}
}


/**
 * @brief  Calcul VbatMod
 * @param
 *
 * @retval
 */
void ModCalcVbatLLC(void)
{
    uint i, j;
    ulong VBatMod;
    uchar FlagDisableCharge;
    
    // MODIF R3.3
    uchar FlagFilPilote1 = 0;
    uchar FlagFilPilote2 = 0;

    // Check fil pilot for all module connected : R1.9
    // MODIF VLMFB
    //FlagDisableCharge = 0;
    FlagDisableCharge = 1;
    i = 0;
    // MODIF R2.7
    //while ((i < MODNB) && (FilPil != 0))
    while (i < MODNB)
    {
        // MODIF VLMFB
        LLCWarnOff(i);
        if (((LLCRamRO[i].VLmfb * 10) > (VbatThrld*Menu.NbCells)) && (CanErr(i) == 0))
        {
        	FlagDisableCharge = 0;  // MODIF VLMFB
            // MODIF R3.3
            if (i < Menu.NbModLeft)
            	FlagFilPilote1 = 1;		// FP1 connected to 1 module minimum
            else
            	FlagFilPilote2 = 1;		// FP2 connected to 1 module minimum
        }
        // MODIF R2.7 : only for single phase to detect output fuse broken
        if ( (LLCRamRO[i].VBat > 900) 
            && (LLCRamRO[i].VLmfb < 500) 
            && (LLCRamRO[i].VFuse < 500) 
            && (Menu.ChgType == TYPE_1PHASE)
            && (CanErr(i) == 0))
        {
            FlagDisableCharge = 0; 
        }   
        i++;
    }

    i = 0;
    j = 0;
    VBatMod = 0;

    do
    {
        if (CanErr(i) == 0)
        {
            // MODIF R2.7 : only for single phase to detect output fuse broken  
            if ( (LLCRamRO[i].VBat > 900) 
            && (LLCRamRO[i].VLmfb < 500) 
            && (LLCRamRO[i].VFuse < 500) 
            && (Menu.ChgType == TYPE_1PHASE)
            && (CanErr(i) == 0))
            {
                LLCWarnOn(i);
                VBatMod += LLCRamRO[i].VBat;
                j++;
            }  
            else if ( ((LLCRamRO[i].VLmfb * 10) < (VbatThrld*Menu.NbCells)) && (FlagDisableCharge == 0) && (State.Charger > StateChgerIdle))
            {
                LLCWarnOn(i);
            }
            else if (((LLCRamRO[i].VBat * 10) < (VbatThrld*Menu.NbCells)) && ((LLCRamRO[i].VLmfb * 10) > (VbatThrld*Menu.NbCells)) && (State.Charger > StateChgerIdle))
            {
                LLCWarnOn(i);
            }
            else if (LLCRamRO[i].VBat >= 50)
            {
                VBatMod += LLCRamRO[i].VBat;
                j++;
            }
        }
        i ++;
    }
    while (i < MODNB);

    if (CntCANFault == 0)
    {
    	// MODIF R3.3 : if FP1 or FP2 not connected (dual output), charger not start
    	if (((Menu.NbModLeft == 3) || (Menu.NbModLeft == 4)) && ((FlagFilPilote1 == 0) || (FlagFilPilote2 == 0)))
    	{
    		VbatModCell = 0;
    	}
    	else if ((FlagDisableCharge == 0) && (j > 0))
    		VbatModCell = ((VBatMod / j) * 10L) / (ulong)Menu.NbCells;
        else
        	VbatModCell = 0;
    }
}

/**
 * @brief  Return Charger ID
 * @param
 *
 * @retval
 */
uchar ModChargerID(void)
{
	uchar id;

	//switch (Menu.NbCells)
	switch (Menu.CellSize)
	{
	case 6:
		if (ChgData.Profile != LITH)
		{
			id = ID_LLC1KW_12V;
			if (Menu.ChgType == TYPE_3PHASE)
				id = ID_LLC3KW240_12V;
		}
		else
		{
			id = ID_LLC1KW_24V;
			if (Menu.ChgType == TYPE_3PHASE)
				id = ID_LLC3KW_243648V | ID_LLC3KW_243648VUS | ID_LLC3KW_24VM | ID_LLC3KW240_243648V | ID_LLC3KW600_243648V;
		}
		break;

	case 7  :
	case 12 :
	case 14 :
		id = ID_LLC1KW_24V;
		if (Menu.ChgType == TYPE_3PHASE)
			id = ID_LLC3KW_243648V | ID_LLC3KW_243648VUS | ID_LLC3KW_24VM | ID_LLC3KW240_243648V | ID_LLC3KW600_243648V;
		break;

	case 10:
		id = ID_LLC1KW_3648V;
		if (Menu.ChgType == TYPE_3PHASE)
			id = ID_LLC3KW_243648V | ID_LLC3KW_243648VUS | ID_LLC3KW_3648VM | ID_LLC3KW240_243648V | ID_LLC3KW600_243648V;
		break;

	case 18:
	case 24:
		if (ChgData.Profile != LITH)
		{
			id = ID_LLC1KW_3648V;
			if (Menu.ChgType == TYPE_3PHASE)
				id = ID_LLC3KW_243648V | ID_LLC3KW_243648VUS | ID_LLC3KW_3648VM | ID_LLC3KW240_243648V | ID_LLC3KW600_243648V;
		}
		else
		{
			id = ID_LLC1KW_7280V;
			if (Menu.ChgType == TYPE_3PHASE)
				id = ID_LLC3KW_7280V | ID_LLC3KW240_7280V | ID_LLC3KW600_7280V;
		}
		break;

	case 20:
	case 30:    // MODIF 2.8
    case 32:
    case 36:
	case 40:
		id = ID_LLC1KW_7280V;
		if (Menu.ChgType == TYPE_3PHASE)
			id = ID_LLC3KW_7280V | ID_LLC3KW240_7280V | ID_LLC3KW600_7280V;
		break;
    case 48:
        id = ID_LLC3KW_96V;
        break;
		// AUTO
	case 255:
        id = ID_LLC1KW_3648V;
		if (Menu.ChgType == TYPE_3PHASE)
			id = ID_LLC3KW_243648V | ID_LLC3KW_243648VUS | ID_LLC3KW_3648VM | ID_LLC3KW240_243648V | ID_LLC3KW600_243648V;	
		break;

	default:
		id = NO_ID;
		break;
	}

	return id;
}


void ModSelect3Phases(void)
{
	uint IMaxStartMod;
	uchar i;


	IMaxStartMod = 0;
	for (i=0; i<ModCurr; i++)
	{
		/* calcul max current for started module */
		IMaxStartMod += ModTabImax[ModIndex[i]];
	}

	/* Select current working modules */
	/* Switch on next module if (85%*ImaxModule<IReal) + (Ireq>Imax started module) + (Vreq>VbatCell) */
	// david : ajout de MIN_CUR_START_STOP pour �viter d'arr�ter le module si I < MIN_CUR_START_STOP
	if ((((ulong)ModTabImax[ModIndex[ModCurr - 1]] * 80) / 100) <= LLCRamRO[ModIndex[ModCurr - 1]].IBat
			&& (DataW.Ireq > (IMaxStartMod + MIN_CUR_START)) && (DataW.VreqCell > DataR.VbatCell+15) && (ModCurr != MODNB))
	{
		if (ModIndex[ModCurr] < MODNB)          /* Next module not in error */
		{
			if ((DataW.Ireq != 0) && (DataW.VreqCell != 0))
			{
				if (ModCurr < MODNB)
				{
					ModCurr ++;
					IbatMem = 0;
				}
			}
			else
			{
				if (ModCurr < MODNB)
					ModCurr ++;
			}
		}
	}

	/* Switch off current module */
	if (1 < ModCurr)
	{
		/* Previous module at least 70 % load */
		if (LLCRamRO[ModIndex[ModCurr - 2]].IBat <= (((ulong)ModTabImax[ModIndex[ModCurr - 2]] * 70) / 100))
		{
			ModCurr --;
		}
		//else if (ModPwmLow(ModIndex[ModCurr - 1]) != 0)
		//{ /* Remove module on low PWM  */
		//  ModCurr --;
		//}
		else  if ((LLCRamRO[ModIndex[ModCurr - 1]].IBat < MIN_CUR_STOP) && (IbatMem >= MIN_CUR_STOP))
		{ /* Remove module on low current and reset vreq correction for right/left change */
			ModCurr --;
		}

		/* Memorize current */  // DAVID 5 � 20, sinon le module s'arrete avant de d�marrer
		if (LLCRamRO[ModIndex[ModCurr - 1]].IBat >= MIN_CUR_STOP)
			IbatMem = LLCRamRO[ModIndex[ModCurr - 1]].IBat;
	}
}


void ModI(void)
{
	/* Distribute current over available modules */
	/* DataW.Ireq : requested current */
	/* ModCurr - 1 : last active module in ModIndex */

	uchar i;
	uint irem;

	/* Fill first modules */
	i = 0;
	irem = DataW.Ireq;

    if ((ChgData.Profile == TEST) && (State.Charge > StateChgIQEnd) && (State.Charger > StateChgerStChg))
    {
        for (i=0; i<MODNB; i++)
        {
            LLCRamRW[i].VReq = 290 * Menu.NbCells;
        }
        // MODIF R2.3
        switch (State.Charge)     /* Select current phase */
    	{
        case StateChgStartPhase1:     /* Just at beginning of phase 1 */
            ptrMod = 0;
        break;
        
        case StateChgPhase1:          /* In phase 1 */
          if ((ChgData.TimerSecCharge >= (ptrMod*10)) && (ChgData.TimerSecCharge < ((ptrMod*10) + 10)))
          {
            if ((LLCErr(ptrMod) != 0) || (CanErr(ptrMod) != 0))
                ChgData.TimerSecCharge += 10;
            
            for (i=0; i<MODNB; i++)
            {
                if (i != ptrMod)
                    LLCRamRW[i].IReq = 0;
            }        
            LLCRamRW[ptrMod].IReq = 50;
            DataW.VreqCell = 2850;
          }
          else
          {
            if (ptrMod < MODNB)  
                ptrMod++;
          }
          
          if (ptrMod >= MODNB)
          {   // Stop test and available
              ChgData.TimeMaxPhase1 = 0;
              for (i=0; i<MODNB; i++)
              {
                LLCRamRW[i].VReq = 0;
                LLCRamRW[i].IReq = 0;
              }      
          }
          break;

    	default:
         break;
      }
        
    }
    else
    {
        do
        {
            LLCRamRW[ModIndex[i]].Slope = (SlopeI / 10);
        	/* Share current beetween connected module */
        	if (ModTabImax[ModIndex[i]] <= irem)
        	{
        	    LLCRamRW[ModIndex[i]].IReq = ModTabImax[ModIndex[i]];    /* Fill modules with IMax */
        	}
        	else
        	    LLCRamRW[ModIndex[i]].IReq = irem;         /* Fill last modules with irem */

        	/* Calculates new remaining current */
        	irem = irem - LLCRamRW[ModIndex[i]].IReq;
        	i ++;
        }
        while (i < MODNB);
    }
}

/**
* @brief    Check Output current Derating vs Output diode temperature
* @param    None
*
* @retval   None
*/
void CheckDerating (void)
{
  	#define	TEMP_CONBATPLUS				75
  	#define	TEMP_CONBATMOINS			75
   	#define	TEMP_DIODE					75
	
   	schar TempConBatPlus;
   	schar TempConBatMoins;	
	
   	uchar i;
   	schar TempOutputDiode;
   	uint irem_thrld;
   
   	if (TimeCheckDerating < State.TimerSec)
   	{
		irem_thrld = (ModFindIMaxModuleID(LLCRamRO[ModIndex[ModCurr-1]].Id, LLCRamRO[ModIndex[ModCurr-1]].Ver[0]) * 50)/100;
		//irem_thrld = (ModFindIMaxModuleID(LLCRamRO[ModIndex[ModCurr-1]].Id, LLCRamRO[ModIndex[ModCurr-1]].Ver[0]) * 70)/100;
       	TimeCheckDerating = State.TimerSec + 15L;
       	i = 0;
       	do
       	{ 
			TempConBatMoins		= (schar)((LLCRamRO[ModIndex[i]].Temp[1] >> 8) - 64);
			TempConBatPlus 		= (schar)((LLCRamRO[ModIndex[i]].Temp[1] & 0x00FF) - 64);
        	TempOutputDiode 	= (schar)((LLCRamRO[ModIndex[i]].Temp[3] & 0x00FF) - 64);
           	if ((LLCRamRO[ModIndex[i]].IBat > irem_thrld) 
				&& ((TempOutputDiode >= TEMP_DIODE) ||
				(TempConBatPlus >= TEMP_CONBATPLUS) || 
				(TempConBatMoins >= TEMP_CONBATMOINS)))
           	{
            	if (LLCRamRO[ModIndex[i]].IBat > (irem_thrld + 50)) 
					LLCRamRW[ModIndex[i]].IReq = LLCRamRO[ModIndex[i]].IBat - 50;
				else
					LLCRamRW[ModIndex[i]].IReq = irem_thrld;
			    FlagEfficiency = 1;
                FlagDerating = 1;
                ChgData.VdVdTOld = 0;
           	}
           	i++;
       	} 
       	while (i < MODNB); 
   	}
}

/**
* @brief  Sort Module left/right (2 output management)
* @param
*
* @retval
*/
void ModSortLeftRight (void)
{
    // MODIF 2.8 :add new function
    uchar i;
    sint NbMod = 0;
    uchar NbModLeft = 0;
    uchar NbModRight = 0;
    
    uchar ModIndexLeft[MODNB+1];
    uchar ModIndexRight[MODNB+1];
              
    for (i=0; i<MODNB; i++)
    {
        if (ModIndex[i] < Menu.NbModLeft)
        {
            ModIndexLeft[NbModLeft] = ModIndex[i];
            NbModLeft++;
            NbMod++;
        }
        else if (ModIndex[i] != MODNB)
        {
            ModIndexRight[NbModRight] = ModIndex[i];
            NbModRight++;
            NbMod++;    
        }
    }
    
    if (NbModLeft > NbModRight)
    {
        while (NbMod > 0)
        {
            if (NbModLeft > 0)
            {     
                NbModLeft--;
                NbMod--;
                ModIndex[NbMod] = ModIndexLeft[NbModLeft];
            }
            if ((NbModRight > 0) && (NbMod > 0))
            {
                NbModRight--;
                NbMod--;
                ModIndex[NbMod] = ModIndexRight[NbModRight];
            }
        }
    }
    else if ((NbModLeft < NbModRight) || (NbModLeft == NbModRight))
    {
        while (NbMod > 0)
        {
            if (NbModRight > 0)
            {     
                NbModRight--;
                NbMod--;
                ModIndex[NbMod] = ModIndexRight[NbModRight];
            }
            if ((NbModLeft > 0) && (NbMod > 0))
            {
                NbModLeft--;
                NbMod--;
                ModIndex[NbMod] = ModIndexLeft[NbModLeft];
            }
        }  
    }
}

/**
* @brief    Check Output current Derating vs CTN temperature
* @param    None
*
* @retval   None
*/
void CheckDeratingSinglePhase (void)
{
	
	#define	TEMP_CONBATPLUS				75
	#define	TEMP_CONBATMOINS			75
   
	uchar i;
	schar TempConBatPlus;
	schar TempConBatMoins;
	
	uint irem_thrld;
   
	if (TimeCheckDerating < State.TimerSec)
	{
		irem_thrld = (ModFindIMaxModuleID(LLCRamRO[ModIndex[ModCurr-1]].Id, LLCRamRO[ModIndex[ModCurr-1]].Ver[0]) * 70)/100;
		TimeCheckDerating = State.TimerSec + 15L;
		i = 0;
		do
		{ 
			TempConBatMoins		= (schar)((LLCRamRO[ModIndex[i]].Temp[1] >> 8) - 64);
			TempConBatPlus 		= (schar)((LLCRamRO[ModIndex[i]].Temp[1] & 0x00FF) - 64);
		   
			if ((LLCRamRO[ModIndex[i]].IBat > irem_thrld) && 
				( (TempConBatPlus >= TEMP_CONBATPLUS) || 
				  (TempConBatMoins >= TEMP_CONBATMOINS)) )
			{
                LLCRamRW[ModIndex[i]].IReq = LLCRamRO[ModIndex[i]].IBat - 5;
			    FlagEfficiency = 1;
                FlagDerating = 1;
                ChgData.VdVdTOld = 0;
			}
			i++;
		} 
		while (i < MODNB); 
	}
}

/**
* @brief    Save E1Memo type to E2DataFlash
* @param    SlotNumber (0 to 7)
*
* @retval   None
*/
void SaveE1Memo (uchar SlotNumber)
{
	uchar i;

	// MODIF R3.3
	memset(&E1Memo, 0 , sizeof(StructE1Memo));
	E1Status.EventPointer++;
	
	// MODIF 3.3
	if (E1Status.EventPointer < SEGE1TOP)
	{
		E1Status.EventPointer = SEGE1TOP;
		E1Status.NbRstPointer = 0;
	}
	
	if (E1Status.EventPointer > (SEGE1TOP + SEGE1NB))
	{
		E1Status.EventPointer = SEGE1TOP;
		E1Status.NbRstPointer++;
	}
	E1Memo.NumSegment = E1Status.EventPointer;
	E1Memo.TypeSegment = TYPE_MEMO;
	// Date
	E1Memo.EventDate.Year = State.DateR.Year % 100;
	E1Memo.EventDate.Month = State.DateR.Month;
	E1Memo.EventDate.Date = State.DateR.Date;
	E1Memo.EventDate.Hr = State.DateR.Hr;
	E1Memo.EventDate.Min = State.DateR.Min;
	E1Memo.SlotNumber = SlotNumber + 1;
	E1Memo.Id = LLCRamRO[SlotNumber].Id;
	E1Memo.RamRoDef = LLCRamRO[SlotNumber].UnionRamRODef.Def;
	E1Memo.RamRoStatus = LLCRamRO[SlotNumber].UnionRamROStatus.Status;
	for (i=0; i<2; i++)
	{
		E1Memo.Ver[i] = LLCRamRO[SlotNumber].Ver[i];
	}
	E1Memo.TimerSec = LLCRamRO[SlotNumber].TimerSec;
	for (i=0; i<4; i++)
	{
		E1Memo.Temp[i] = LLCRamRO[SlotNumber].Temp[i];
	}
	E1Memo.VLmfb = LLCRamRO[SlotNumber].VLmfb;
	E1Memo.VFuse = LLCRamRO[SlotNumber].VFuse;
	E1Memo.VBat = LLCRamRO[SlotNumber].VBat;
	E1Memo.IBat = LLCRamRO[SlotNumber].IBat;
	E1Memo.VReq = LLCRamRW[SlotNumber].VReq;
	E1Memo.IReq = LLCRamRW[SlotNumber].IReq;
	E1Memo.Slope = LLCRamRW[SlotNumber].Slope;
	E1Memo.PowerMax = LLCRamRW[SlotNumber].PMax;
	E1Memo.VbatCharger = DataR.Vbat;
	E1Memo.IbatCharger = DataR.Ibat;
	E1Memo.StateCharge = State.Charge;
	E1Memo.StateCharger = State.Charger;
	E1Memo.StatePhase = State.Phase;

	Write_SegFlashData(E1Status.EventPointer, (uchar *)&E1Memo);
	Write_SegFlashData(SEGE1STATUS, (uchar *)&E1Status);
}

/**
* @brief    Check Voltage difference
* @param    V1 V2 V3 OFFSET
*
* @retval   None
*/
uchar CheckInputVoltage (uint V1, uint V2, uint V3, uint OFFSET_V1, uint OFFSET_V2V3)
{	// MODIF R3.3
	uchar ret = 0;

	if ((V1 > (V2 + OFFSET_V1)) || (V1 < (V2 - OFFSET_V1)))
		ret = 1;

	if ((V1 > (V3 + OFFSET_V1)) || (V1 < (V3 - OFFSET_V1)))
		ret = 1;

	if ((V2 > (V3 + OFFSET_V2V3)) || (V2 < (V3 - OFFSET_V2V3)))
		ret = 1;

	return ret;
}

/**
* @brief    Check Voltage difference beetween module
* @param    VbatModule array pointer, number of module, max value
*
* @retval   None
*/
uchar CheckVbatModule2Module (sint *VbatModule, uchar NbrModule, uint OFFSET_V)
{	// MODIF R3.3
	uchar ret = 0;
	uint i;

	if (NbrModule == 1)
		return ret;

	for (i=1; i<NbrModule; i++)
	{
		if ((*(VbatModule) > (*(VbatModule+i) + OFFSET_V)) || (*(VbatModule) < (*(VbatModule+i) - OFFSET_V)))
		{
			ret++;
		}
	}

	return ret;
}

#pragma inline_asm ENABLE_IRQ

/******************************************************************************
Function Name   : ENABLE_IRQ
Description     : Enable IRQ
Arguments       : none
Return value    : none
 ******************************************************************************/
static void ENABLE_IRQ(void)
{
	SETPSW I
}


/******************************************************************************
Pragma variables
 ******************************************************************************/
#pragma inline_asm DISABLE_IRQ

/******************************************************************************
Function Name   : DISABLE_IRQ
Description     : Disable IRQ
Arguments       : none
Return value    : none
 ******************************************************************************/
static void DISABLE_IRQ(void)
{
	CLRPSW I
}
