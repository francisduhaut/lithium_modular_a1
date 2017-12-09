/*
 * charge.c
 *
 *  Created on: 24/02/2013
 *      Author: Francis DUHAUT
 */
#include "include.h"

//#pragma address line1			= 0x000C0				/* 16 bytes */
//#pragma address line2			= 0x000D0           	/* 16 bytes */
#pragma ADDRESS version     	= 0x00100
#pragma ADDRESS line1			= 0x00110				/* 16 bytes */
#pragma ADDRESS line2			= 0x00120           	/* 16 bytes */
#pragma ADDRESS LLC           	= 0x00140

#pragma ADDRESS DataR         	= 0x00500           	/* 64 bytes */
#pragma ADDRESS DataW         	= 0x00540           	/* 64 bytes */
#pragma ADDRESS State         	= 0x00580           	/* 64 bytes */
#pragma ADDRESS ChgData       	= 0x005C0           	/* 64 bytes */
#pragma ADDRESS IQData        	= 0x00600           	/* 64 bytes */
#pragma ADDRESS Menu          	= 0x00640           	/* 64 bytes */
#pragma ADDRESS Status        	= 0x00680           	/* 64 bytes */
#pragma ADDRESS ChargerConfig 	= 0x006C0           	/* 64 bytes */
#pragma ADDRESS SerialNumber  	= 0x00700           	/* 64 bytes */
#pragma ADDRESS Host          	= 0x00740           	/* 64 bytes */
#pragma ADDRESS Memo          	= 0x00780           	/* 64 bytes */
#pragma ADDRESS DataLcd       	= 0x007c0           	/* 64 bytes */
#pragma ADDRESS IQSeg         	= 0x00800           	/* 64 bytes */
#pragma ADDRESS ScanAddrArray 	= 0x00840           	/* 4 * 40 = 160 bytes */
#pragma ADDRESS AarsParam     	= 0x008E0           	/* 64 bytes */
#pragma ADDRESS EventStatus    	= 0x00920           	/* 64 bytes  - MODIF R3.2 */
// MODIF BLE
#pragma ADDRESS DataBle			= 0x00960				/* 26 bytes */
#pragma ADDRESS Ble				= 0x0097A				/* 32 bytes */
#pragma ADDRESS Lva 			= 0x0099A           	/* 64 bytes */


extern uchar LcdTemp[128];
extern ulong TimeCheckIdle;
extern ulong CntCANFault;

char line1[LCD_NB_CHAR];         	            		/* Line 1 (16 car) */
char line2[LCD_NB_CHAR];           	          			/* Line 2 (16 car) */
char version[8];

StructDataR DataR;
StructDataW DataW;
StructState State;
StructChgData ChgData;
StructIQData IQData;
StructMenu Menu;
StructStatus Status;
StructChargerConfig ChargerConfig;
StructSerialNumber SerialNumber;
StructHost Host;
StructMemo Memo;              							/* Currrent memo */
StructMemo MemoToSave;        							/* Memo saved in EEPROM */
StructDataLcd DataLcd;
uchar IQSeg[64];
StructAarsParam AarsParam;
ulong ScanAddrArray[NB_SCAN_ADDR];
StructMemo MemoTemp;
StructLLC LLC[MODNB+1];
StructLLCRamRO LLCRamRO[MODNB+1];
StructLLCRamRW LLCRamRW[MODNB+1];

StructE1Status E1Status;			/* MODIF R3.3 */
StructE1Memo E1Memo;				/* MODIF R3.3 */

// MODIF BLE
StructDataBle DataBle;
StructBle Ble;
StructLva Lva;

// BMS
StrcutBMS Bms;

// Display
ulong MnuTimer;
uint EEPSeg;
uchar ModNumber;
uchar FlagThAmbiant;

uint8_t pos, prevpos = 255;
uint8_t FirstEntry = 0;  
uint8_t EntryEvent = 0;  

uint8_t ReDraw = 0;
uint8_t ReDraw2 = 0;

uint8_t MnuValidNbSave = 0;
uint PasswdTmp;
ulong next;

uint MnuListCur[6];          	/* Current value in list */
uint MnuListOff[6];          	/* Offset of the list (case long list) (uint for memo) */
uint MnuValidNb;             	/* Number of valid menus */

uchar MnuListLevel;
uchar MnuLevel;               	/* Level of current display (0 : no menu) */
uchar Mnu[7];                 	/* 1 more, when enter, set next level to 0 */
uint MnuOutTime;             	/* Time for auto exiting menus */

/* Global vars saved for each chrono */
uchar MnuNumVal[16];          	/* 16 variable characters */
uchar MnuNumIndex;           	/* Index for digit modification (0 to 15) */
/* Selected menu in list menu */
uint MnuNumMask;             	/* Current value of the 16 bits */
uint MnuBitVal;              	/* Working segment for edition */

uint MnuMemoVal;             	/* Value of the current displayed memo ( 0 to ...) */
uint MemoPtr;					/* Pointer for read memo */

uchar LcdTemp[128];
ulong TimerMnuOption;           /* Timer for Mnu options (Pump, ev, voy) */


// charge
uchar FlagIQEqual;
uint ItoReach;
ulong TimeStChg;
uint TempProfile;
ulong TimeCheckCurrent = 0;
ulong TimeCheckVbat = 0;
ulong TimeCheckBatOn = 0;
ulong TimeCheckBatOff = 0;
ulong TimeCheckBadBat = 0;
ulong TimeCheckBadBatOff =0;
ulong TimeInitIdleMode = 0;
ulong TimeInitVregPhase3 = 0;
ulong TimeResetRFDongle = 0;
ulong TimeDefIconsMin = 0;
ulong OPPAh;
ulong TimeRemoveMod = 0;
ulong TimerModDef = 0;
ulong TimerDefID = 0;
ulong TimerDisconnect = 0;
ulong TimeOPPIUI;       // MODIF R2.2

// DefChrono
uint TempIDefCur;
uint TempVDefCur;
ulong TimeDefCur;
ulong TimeAckDef;
uint  TempIDefTemp;
uint TempVDefTemp;
ulong TimeDefTemp;
ulong TimeDefTempWait;
ulong TimeDefPump;
ulong TimeDefVbatMax;
uint  TempIDefPowerCut;
uint  TempVDefPowerCut;
uchar CntDefCurTmp;	      		// Counter for current faults
uchar CntDefCur;	        	// Counter for current faults
uchar CntDefTemp;	  			// Counter for temperature faults
ulong CntRefresh;
ulong TimeThAmb;

// OptionChrono
ulong TimePump;
ulong TimeElect;
// ChargePhase
uint  TempVinit;
uint TempProfileTime;
uint TempProfileAh;
uint TempMemoDefault;
ulong TimeIdleWake;

// LoopPhase
uint TempILoop;
uint TempVLoop;
uint TempSlope;
ulong TimeLoop;
uint CntLoop;
// Memo
ulong TimeUpdateMemo;
uint EocVoltOld;        		// mV
uint EocCurOld;            		// A
// Charger
uint PowerMax;           		// W
uint Shunt;			      		// 1/10A
uint Rcable;		            // 1/10mohm
uint RcableLeft;	          	// 1/10mohm
uint RcableRight;          		// 1/10mohm
sint Vcable;		            // mV
ulong Ah100ms;
ulong Wh100ms;
uint ItoReach;
uint SlopeI;
uint SlopeItmp;
sint TempCap;
uint VbatThrld;
uchar FlagLoop;
uchar FlagMonte;
uint ChargerBatCap;
uint IReqMax;

ulong TimeIQ;
ulong TimerDisplay;
ulong TimerIdent;
ulong TimerFloat;

uchar Conditional;            	// To stop conditional charge check in Opportunity
uchar FlagDesul;
uchar FlagSaveSerial;

uchar FlagDerating = 0;                 // MODIF 2.8 ALL
ulong TimeCheckDerating = 0;            // MODIF 2.8 ALL


uchar FlagEfficiency;
ulong TimeCheckEfficiency = 0;
uchar ModIndex[MODNB+1];      	// List of index for available modules
uint ModTabImax[MODNB+1];
uint IbatModMem[MODNB+1];

uint ICons;
uchar ModCurr;                  // Last active module, 0 = no module
uchar ModCurrSave;              // MODIF 2.8 : save  Modcur value
uint  ModImax;                  // Modular Imax calculate with connected module
ulong TimerSecTmp;
ulong TimeDefCurrent;
ulong TimeDefMod;
ulong TimeDefFuse;
uchar FlagDefBatCon;
ulong TimeModCur;
ulong TimeCheckIdle = 0;
ulong TimeBatCon = 0;           // MODIF 2.8

uint IbatMem;
sint IbatOld;
uint IreqOld;
volatile uint ILeft;                     // Current in left branch
volatile uint IRight;                    // Current in right branch
volatile uint VReq;
volatile ulong Vcbl;
volatile sint VReqMem;                  // MODIF 2.8
volatile slong VcblMem;                 // MODIF 2.8
volatile ulong RcblMem;                 // MODIF 2.8
volatile slong Voffset;                 // MODIF 2.8
volatile slong VoffsetComp;             // MODIF 2.8
schar Tambiant;
ulong VbatCellCharger;
ulong VbatModCell;
// MODIF 2.8
//uchar PMod;
uint  PMod;
uchar ChargerID;

uint LoopWidthOld;
uint LoopWidthNew;
sint DeltaLoopWidth;
sint DeltaLoopWidthMax;

// IONIC
uint Iphase3min;
ulong Iphase3est;
uchar InitSoC;
uint  AhDeduction;
uint  AhToReinject;
ulong TimeIncVreg;
uint  TempVbat;
uchar FlagLoopPhase;
// Loop Phase
uchar NbTimeLoopPhase;
uchar CntTimeLoopPhase;
// Closer loop
uchar FlagCloserLoop1;
uchar FlagCloserLoop2;
uchar FlagDoCloserLoop;
uchar CntCloserLoop;
uint Iphase1Tmp;
uint Iphase3Tmp;

uint ModImaxTmp;
uint IDModule;
schar TambCompare;      	// MODIF TH-AMB

uint ILoopLow;
uint ILoopHigh;

uchar IP_DHCP[4];
ulong TimeDefVreg;
ulong TimeCheckVcell = 0;
ulong TimeImaxPhase2;
uchar ForceIdleOff = 0;
uchar ForceMod70A;

// MODIF R2.5 : Flag to reset watchdog in timer_cmt interrupt
uchar FlagWdogReset = 0;
uchar CptWdogReset = 0;
// MODIF 2.8
uchar FlagInit = 0;
ulong TimerChangeIreq =0;
uchar FlagDisconnectInCharge = 0;
ulong TimeCheckRcable = 0;
slong VbatLeft = 0;
slong VbatRight = 0;
schar NbrLeft = 0;
schar NbrRight = 0;
uint  IconsMin = 0;
uchar FlagVRegPhase1 = 0;

// MODIF R3.3
uchar ErrorVmodule = 0;
uchar FlagEqPh2 = OFF;

// MODIF BLE
ulong	TimerBleLedBlink = 0;
ulong	TimeMaxEqual =0;

/**
 * @brief  Init C struct
 * @param  None
 * @retval None
 */
void Init_Charger (void)
{
	int i;
	memset(&Menu, 0 , sizeof(Menu));
	Menu.TypeSegment = TYPE_MENU; Menu.NumSegment = 0;

	memset(&Status, 0 , sizeof(Status));
	Status.TypeSegment = TYPE_STATUS; Status.NumSegment = 1;

	memset(&SerialNumber, 0 , sizeof(SerialNumber));
	SerialNumber.TypeSegment = TYPE_SERIAL; SerialNumber.NumSegment = 2;

	memset(&DataR, 0 , sizeof(DataR));
	DataR.TypeSegment = TYPE_DATAR; DataR.NumSegment = 3;

	memset(&DataW, 0 , sizeof(DataW));
	DataW.TypeSegment = TYPE_DATAR; DataW.NumSegment = 4;

	memset(&State, 0 , sizeof(State));
	State.TypeSegment = TYPE_STATE; State.NumSegment = 5;

	memset(&ChgData, 0 , sizeof(ChgData));
	ChgData.TypeSegment = TYPE_CHGDATA; State.NumSegment = 6;

	memset(&Memo, 0 , sizeof(Memo));
	//Memo.TypeSegment = TYPE_MEMO; Memo.NumSegment = 8;

	memset(&MemoToSave, 0 , sizeof(MemoToSave));
	//MemoToSave.TypeSegment = TYPE_MEMO; MemoToSave.NumSegment = 9;

	memset(&DataLcd, 0 , sizeof(DataLcd));
	DataLcd.TypeSegment = TYPE_DATALCD; DataLcd.NumSegment = 10;

	memset(&ChargerConfig, 0 , sizeof(ChargerConfig));
    memset(&Host, 0, sizeof(Host));     // MODIF R2.2
    
	memset(&line1, 0 , LCD_NB_CHAR);
	memset(&line2, 0 , LCD_NB_CHAR);

	for (i=0; i< MODNB; i++)
	{
		memset(&LLCRamRO[i],  0 , sizeof(StructLLCRamRO));
		memset(&LLCRamRW[i],  0 , sizeof(StructLLCRamRW));
		memset(&LLC[i],       0 , sizeof(StructLLC));
	}

	sprintf((char *)&version[0],VERSION);

	// MODIF R3.3
	memset(&E1Status, 0 , sizeof(StructE1Status));

	// MODIF BLE
	memset(&DataBle, 0 , sizeof(DataBle));
	memset(&Ble, 0 , sizeof(Ble));
	memset(&Lva, 0 , sizeof(Lva));
	
	// MODIF BMS
	memset(&Bms, 0 , sizeof(Bms));

	KEY_Reset();
	ReloadConfigOn;
	TFTBacklightOn;
	FlagSaveSerial = OFF;
	FlagThAmbiant = OFF;
	PMod = 0;
    // MODIF R2.5
    FlagWdogReset = 0;
    
    // Modif R2.1
    for (i=0; i<4; i++)
    {
        IP_DHCP[i] = 0;
    }
    // MODIF 2.8
    FlagInit = 0;
}

/**
 * @brief  Main charge chrono
 * @param  None
 * @retval None
 */
void CHARGE_Chrono (void)
{
	ulong OPPAhTmp;
    
    //ManuSwOn;
	//ManuRegOn;
    
	/********************************************************************/
	/* For debug - will be removed                                      */
	/********************************************************************/

	//MaskPasswdHighOff;
	//MaskPasswdLowOff;
	//MaskPasswd2Off;
	//MaskPasswd3Off;
	

#ifdef DEMO
    MaskPasswdHighOff;
	MaskPasswdLowOff;
	MaskPasswd2Off;
	MaskPasswd3Off;

	DataR.Vcharger = (Menu.NbCells*200) + (Memo.ChgTime*50);// + ((DataR.VbatCell + State.DateR.Sec + State.TimerMs + 1) % 50L);
    if (DataR.Vcharger > (270*Menu.NbCells))
        DataR.Vcharger = 270*Menu.NbCells;
	DataR.Vbat = DataR.Vcharger;
	DataR.Vfuse = DataR.Vbat;
	DataR.VbatCell = ((ulong)DataR.Vbat * 10L) / (ulong)Menu.NbCells;
	VbatModCell = DataR.VbatCell;
	DataR.Ibat = DataW.Ireq;
    DataR.Power = (ulong)(DataR.Vbat * DataR.Ibat) / 1000;
	//Menu.Imax = 1050;
    Menu.Imax = Menu.ImaxCharger;
    ModImaxTmp = Menu.ImaxCharger;
	ItoReach = DataW.Ireq;
	DataLcd.ChgSOC = SocLoop(DataR.VbatCell) + 5;
	DataLcd.ChgRestTime = ((8 * (100 - DataLcd.ChgSOC)) / 80) + 1;
    IdleOff;
    IdleStateOff;
#endif

    // 3 Phases charger
	if (Menu.ChgType == TYPE_3PHASE)
	{   
        MaskBPOn;
        Mask1PHOff;     // MODIF 2.8
    }
	else
	{
        MaskBPOff;
        Mask1PHOn;      // MODIF 2.8
    }

    // MODIF 3.3 : Nexsys profile management (add HAWKER_US)
    if (Menu.XFCOffOn != 0)
    {
        MaskXFCOn;
        if ((Menu.Profile != GEL) && (Menu.Profile != TPPL) && (Menu.Profile != XFC) && (Menu.Profile != NXBLC))
        {
            Menu.Profile = TPPL;
            MenuWriteOn;
        }
        MaskNOXFCOff;
    }
    else
    {
        MaskXFCOff;
        MaskNOXFCOn;
    }

    // MODIF R2.5
    FilPilOn;


	MemoChrono();			  // Manage Memo
	CHARGE_OptionChrono();    // Manage option (Pump, EV ...)

	//  AutoStart On/Off
    // MODIF R2.7
	//if (Menu.AutoStartOffOn == 0)
    if ((Menu.AutoStartOffOn == 0) && (Menu.PLCPulseOffOn == 0))
	{
		AutoStartOn;
	}
	else
	{
		AutoStartOff;
	}

	switch (State.Charger)
	{
	case StateChgerInit:
		StartStopOff;
		EasyKitReadOff;
		EasyKitLinkOff;
		DataR.Vcharger = 0;
		DataR.Ibat = 0;
		CHARGE_Init();
		ForceEqualOff;
		TimeInitIdleMode = 0;
		if(State.TimerSec >= TIME_INIT)
		{
			Init_SCI1();  // Init SCI1 after init time
			//necessarry if LAN is selected and charger start with Test software running on SCI1
			ChgData.Profile = Menu.Profile;
			State.Charger = StateChgerIdle;
            #ifdef SAVOYE
            WriteJ1939AdrOn;
            #endif
			if ((Menu.AutoStartOffOn == 0) && (Menu.Profile != LITH))
			{
				AutoStartOn;
				StartStopOn;
			}
			else
			{
				AutoStartOff;
				StartStopOff;
			}
            // MODIF NBCELL AUTO
		    if (Menu.CellSize == 255)
			    Menu.NbCells = CalculNbCells();
		    else
			    Menu.NbCells = Menu.CellSize;    
		}
        // MODIF P2.5
        ModAckDef();
		break;

	case StateChgerIdle:
		// MODIF NBCELL AUTO
		if (Menu.CellSize == 255)
			Menu.NbCells = CalculNbCells();
		else
			Menu.NbCells = Menu.CellSize;

#ifdef DEMO
		Memo.ChgTime = 0;
#endif
		Converter(OFF);
		ModAckDef();
		CheckBatTempF();
		// Update profile
		ChgData.Profile = Menu.Profile;
		MaskChgOff;		    	  	// Mask menu out of charge
		StateProfileOff;
		if (ManuReg != 0) 			// Manu mode
		{
			Converter(ON);
			State.Charger = StateChgerManu;
		}
		else if ((StartStop != 0)
				&& (Battery != 0)
				&& (MnuLevel == 0))
			//&& (ChargeDisconnect == 0))
		{
			RFDongle(OFF);          	// Reset RF Dongle
			MaskChgOn;      	      	// Mask menu for charge
			TimeStChg = 0;
			DataLcd.DelayStChg = 0;
            StartMemo();
			KEY_Reset();

			// Charge delay / time of day
			if (Menu.ChgDelayHour == 0) // Delay
			{
				TimeStChg = (ulong)Menu.ChgDelayVal * 60L;
			}
			else if ((Menu.ChgDelayHour != 0) && (RtcValDef == 0)) // Hour
			{
				if (Menu.ChgDelayVal > (((uint)State.DateR.Hr * 60L) + (uint)State.DateR.Min))
					TimeStChg = (ulong)Menu.ChgDelayVal * 60L;
				else
					TimeStChg = ((ulong)Menu.ChgDelayVal + (24L * 60L)) * 60L;
				TimeStChg -= ((ulong)State.DateR.Hr * 3600L) + ((ulong)State.DateR.Min * 60L) + (ulong)State.DateR.Hr;
			}

			// Reset delayed charge in TEST, OPP or DailyChargeDef
            // MODIF R2.3
            if (TestSw != 0)
            {
                TestSwOff;
                TempProfile = ChgData.Profile;
                ChgData.Profile = TEST;
                InstStartOn;
            }
            
			if ((ChgData.Profile == TEST) || ( ChgData.Profile == OPP ) || DailyChargeDef)
			{
				TimeStChg = 0;
			}
            else if (IQRFTrLink != 0)
            {
                TimeStChg += TIME_STCHG + ((DataR.VbatCell + State.DateR.Sec + State.TimerMs + 1) % 50L);
            }

			if (ManuEqual != 0) // Manual equal
			{
				TempProfile = ChgData.Profile;
				ChgData.Profile = EQUAL;
				// MODIF 2.8 : add delay on manual equal when Idle is ON
				if (IdleState != 0)
					 TimeStChg = TIME_STCHG;
				else
					InstStartOn;
			}
            
            
			// BLOCK OUT CHARGE
			BlockOutDefOff;
#define BO_CCC (Menu.BlockOutStart<Menu.BlockOutEnd)                                    // T1 < T2
#define BO_DDD (Menu.BlockOutStart>Menu.BlockOutEnd)                                    // T1 > T2
#define BO_EEE (Menu.BlockOutStart<=(((uint)State.DateR.Hr*60L)+(uint)State.DateR.Min)) // T1   <=  time
#define BO_FFF ((((uint)State.DateR.Hr*60L)+(uint)State.DateR.Min)<=Menu.BlockOutEnd)   // time <=  T2
			if ( (Menu.BlockOutEnable != 0) && ( ( (BO_CCC) && ( (BO_EEE) && (BO_FFF) ) ) || ( (BO_DDD) && ( (BO_EEE) || (BO_FFF) ) ) ) )
			{
				if BO_CCC     // T1 < T2
				    TimeStChg =  (Menu.BlockOutEnd - (State.DateR.Hr*60L + State.DateR.Min)) * 60L;
				else          // T1 > T2
					TimeStChg =  ((24L*60L) - (State.DateR.Hr*60L + State.DateR.Min) + Menu.BlockOutEnd) * 60L;

				BlockOutDefOn;
			}
			else if (Menu.ChgSkipDelay != 0)
			{
                if (IdleState != 0)
				    TimeStChg = TIME_STCHG;
                else    
                    TimeStChg = 5L;
                //MODIF R2.5    
                /*if (IQRFTrLink != 0)
                    TimeStChg = TIME_STCHG + ((DataR.VbatCell + State.DateR.Sec + State.TimerMs + 1) % 50L);
                else
                    TimeStChg = TIME_STCHG;*/
			}
            else if (Menu.InitDOD > 0)
				TimeStChg += 60L;   // 1min added to measure open voltage
			else if (TimeStChg < TIME_STCHG)
				TimeStChg += TIME_STCHG;
			
			if (Menu.Profile == LITH)
				TimeStChg = 5L;
			
			if ((AarsParam.EasyKitOnOff != 0) && (AarsParam.EasyKitAddr != 0))
			{
				if (TimeStChg < 180)  // 3mn min with EasyKit
					TimeStChg = 180;
			}
			TimeResetRFDongle = State.TimerMs + 500;
			TimeStChg += State.TimerSec;
			State.Charger = StateChgerStChg;
			ModResetCntAh();
		}
		break;

	case StateChgerStChg:
		// MODIF NBCELL AUTO
		if (Menu.CellSize == 255)
			Menu.NbCells = CalculNbCells();
		else
			Menu.NbCells = Menu.CellSize;

		if (State.TimerMs > TimeResetRFDongle)
			RFDongle(ON);              // Start RF Dongle
		ModAckDef();
		if ((State.TimerSec > (TimeStChg-180)) && (AarsParam.EasyKitOnOff != 0) && (AarsParam.EasyKitAddr != 0)
				&& (EasyKitRead == 0))
		{
			EasyKitReadOn;
		}
		if (EasyKitLink != 0)
		{
			CHARGE_Init();
            CheckDF4();
			State.Charger = StateChgerChg;
			State.Charge = StateChgInit;
			State.Phase = StatePh1;
		}
		else if ((State.TimerSec >= TimeStChg) || (InstStart != 0))
		{
			EasyKitReadOff;   // Modification for EasyKit
			IQInWIIQScanOff;  // if not EasyKit -> restart with iQScan
			IQInWIIQLinkOff;
			IQData.StateIQ = StateIQOff;
			IQData.State1 = State1Off;
			InstStartOff;
			CHARGE_Init();
			StartMemo();
			State.Charger = StateChgerChg;
			State.Charge = StateChgInit;
			State.Phase = StatePh1;
			CheckDF4();
		}
		else if ((Battery == 0) || (StartStop == 0) || (ManuReg != 0))
		{
			State.Charger = StateChgerIdle;
			EasyKitReadOff;
			EasyKitLinkOff;
		}
		DataLcd.DelayStChg = TimeStChg - State.TimerSec; // Update DelayStChg
		break;

	case StateChgerChg:
		DataLcd.DelayStChg = 0;
		switch (State.Phase)
		{
		case StatePh1:
			State.Phase = StatePh2;
			CHARGE_Phase();
			break;

		case StatePh2:
			// Modif P1.0 : wait module is waked before start charge (idle mode only)
			if (TempVinit == 0)
			{
                CheckDF4();
				ModAckDef();
				break;
			}
           
			State.Phase = StatePh1;
			switch (ChgData.Profile)	// Select profile
			{
			case LITH :
				profile_bms();
			break;
				
			case IONIC:
            case COLD:
				if (Memo.BatTemp < TEMP_THRLD_COLD)
				{
					ChgData.Profile = COLD;
                    if (Memo.Profile != EQUAL)
                        Memo.Profile = ChgData.Profile;
					#ifndef ENERSYS_EU  // MODIF 2.8
					ProfileCOLD();
					#else
					ProfileCOLD_EU();
					#endif
				}
				else
                {
                    ChgData.Profile = IONIC;
                    if (Memo.Profile != EQUAL)
                        Memo.Profile = ChgData.Profile;
					ProfileIONIC();
                }
                break;
            case LOWCHG:
                ProfileLOWCHG();
                break;
			case GEL:
				ProfileGEL();
				break;
			case WF200:
				ProfileWF200();
				break;
			case PNEU:
				ProfilePNEU();
				break;
			case RGT:
				ProfileRGT();
				break;
			// MODIF R2.6
            //case XFC:
			//	ProfileXFC();
			//	break;
			case PZM:
				ProfilePZM();
				break;
			// MODIF R2.7
            case XFC:
            case TPPL:
            case NXBLC:
                // MODIF 3.3 : NexSys/Flex profile in HAWKER_US
				ProfileTPPL();
				break;
			case EQUAL:
				ProfileEQUAL();
				break;

			case OPP:
#define AAA (DailyChargeDef==0)                                                      // test for not yet in Daily Charge
#define BBB ((Menu.SkipComIQ == 0) && (IQWIIQLink != 0))                             // if "scan-for-WIIQ" wait for link    /* jmf 03/15/2013 */
#define CCC (Menu.ChgDailyStart<Menu.ChgDailyEnd)                                    // T1 < T2(0400H)     /* 1B-List #19 & #7 jmf 05/09/2012 */
#define DDD (Menu.ChgDailyStart>Menu.ChgDailyEnd)                                    // T1 > T2(0400H)     /* 1B-List #19 & #7 jmf 05/09/2012 */
#define EEE (Menu.ChgDailyStart<=(((uint)State.DateR.Hr*60L)+(uint)State.DateR.Min)) // T1   <=  time      /* 1B-List #19 jmf 05/09/2012 */
#define FFF ((((uint)State.DateR.Hr*60L)+(uint)State.DateR.Min)<=Menu.ChgDailyEnd)   // time <=  T2(0400H) /* 1B-List #19 & #7 jmf 05/09/2012 */
#define GGG ((State.Charge != StateChgIQScan) && (State.Charge != StateChgIQLink) && (State.Charge != StateChgIQEnd)) /* jmf 01/21/2013 */

				if ((AAA) && (GGG) && (Menu.ChgSkipDaily == 0))  // Do Daily Charge  /* 1B-List #13 jmf 05/10/2012 */
				{
					if ( /*(BBB) &&*/ ( ( (CCC) && ( (EEE) && (FFF) ) ) || ( (DDD) && ( (EEE) || (FFF) ) ) ) )
					{
                        if ((State.Charge == StateChgAvail) && ((TimeOPPIUI + TIME_CHECK_IDLE) > State.TimerSec))
                        {
                            ForceIdleOff = 1;               // Force Idle off
                            ModAckDef();                    // Ack fault module
                        }
                        else
                        {
                            ForceIdleOff = 0;
                            Converter(OFF);					// stop any charge in progress
    						CloseMemo();                    // FD : record memo before daily charge
    						ModAckDef();
    						ChgData.Profile = OPPIUI;		// set up Daily Charge (IONIC Profile)
    						OPPAhTmp = OPPAh;               // Save OPP Ah
    						CHARGE_Init();
    						OPPAh = OPPAhTmp;				// Restore OPP Ah
    						TempVinit = VbatModCell;
                            CheckDesulph();
    						StartMemo();
                            CheckDF4();
    						if (IQWIIQLink != 0)
    						{
    							memcpy(&Memo.BatSN[0], &IQData.BatSN[0], 14);
    							Memo.BatCap = IQData.BatCap;
    							Memo.CapAutoManu = 1;
    							Memo.BatTemp = IQData.BatTemp;
    							Memo.BatTechno = IQData.BatTechno;
    							Memo.BatWarning = IQData.BatWarning;
    							if (IQData.BatTemp > 55)
    								Memo.BatWarning |= 0x04; // High temp
    								Memo.EocTemp = IQData.BatTemp;
    						}
    						State.Charge = StateChgStartPhase1;
    						State.Charger = StateChgerChg;	// set up Daily Charge (OPPIUI Profile)
    						State.Phase = StatePh1;
    						Conditional = 0;				// disallow Conditional Charge
    						DailyChargeDefOn;				// disallow repeated Daily Charges
    						CreateScreenHeader();           // converted to Daily, do not continue Opportunity    
    						break;							
                        }
						
					}
                    else
                        TimeOPPIUI = State.TimerSec;
				}
                #ifndef ENERSYS_EU  // MODIF 2.8
                    ProfileOPP_US();
                #else
                    ProfileOPP();
                #endif
				
				break;

			case OPPIUI:
				if (Memo.BatTemp < TEMP_THRLD_COLD)
				{
					ChgData.Profile = COLD;
                    Memo.Profile = ChgData.Profile;
					#ifndef ENERSYS_EU  // MODIF 2.8
					ProfileCOLD();
					#else
					ProfileCOLD_EU();
					#endif
				}
				else
					ProfileOPPIUI();
				break;

			case VRLA:
				ProfileVRLA();
				break;
            
            case TEST:
                ProfileTEST();
                break;
            
			default:
				break;
			}
			break;
			case StatePhLoopInit:
			case StatePhLoopSet:
			case StatePhLoopLand:
				CHARGE_LoopPhase();
				break;
			case StatePhFloatInit:
			case StatePhFloat:
			case StatePhFloatStop:
				CHARGE_FloatingPhase();
				break;

			default:
				break;
		}

		// BLOCK OUT CHARGE
		if ( (Menu.BlockOutEnable != 0) && ( ( (BO_CCC) && ( (BO_EEE) && (BO_FFF) ) ) || ( (BO_DDD) && ( (BO_EEE) || (BO_FFF) ) ) ) )
		{
			if (BlockOutDef == 0)
			{
				BatteryOff;
			}
		}

		if ((Battery == 0) || (StartStop == 0) || (ManuReg != 0))
		{
			CloseMemo();
			DailyChargeDefOff;
			State.Phase &=  0x0F;
			State.Charger = StateChgerIdle;
			ForceEqualOff;
			EasyKitReadOff;
			EasyKitLinkOff;
		}
		break;

		case StateChgerManu:
			ChgData.Profile = Menu.Profile;
			if (ManuReg == 0)
			{
				Converter(OFF);
				State.Charger = StateChgerIdle;
			}
			break;
	}

	/* Start/Stop */
	if ((Battery == 0) && (AutoStart != 0))   // Set start if no battery and AutoStart= ON for next battery connexion
	{
		StartStopOn;
	}
	// MODIF R2.7 : PLC
    //if ((Battery == 0) && (AutoStart == 0))   // Reset start if no battery and AutoStart = OFF
    if ((Battery == 0) && (AutoStart == 0) && (Menu.PLCPulseOffOn == 0))   // Reset start if no battery and AutoStart = OFF
	{
		StartStopOff;
	}
}

/**
 * @brief  Manage different charge phase
 * @param  None
 * @retval None
 */
void CHARGE_Phase (void)
{
    uint OffsetIPhase2;

	switch (State.Charge)
	{
	case StateChgInit:
		ClearTPPLParam();           /* Clear TPPL param*/
		if (EasyKitLink == 0)
		{
			StartMemo();              /* Init Memo -> only if no EasyKitLink */
		}
		TempVinit = VbatModCell;
		StartMemo();
		if (Menu.Profile == LITH)
		{
			State.Charge = StateChgStartPhase1;
			State.Phase = StatePh1;	
		}
		else if (BadBatDef != 0)
		{
			DFbadBatOn;
			State.Phase = StatePhDefBadBat;
		}
		else if ( (IQRFTrLink == 0) || (EasyKitLink != 0) ||  // No link if no key or link already done or Easy Kit Link
                (ChgData.Profile == TEST) || // or in TEST profile
				(ChgData.Profile == EQUAL) ) // or in equal
		{
			if (EasyKitLink != 0)
			{
				TimeIQ = ChgData.TimerSecCharge + 5L;  // Tempo before SoC for decrease of Ubat
				State.Charge = StateChgIQEnd;
			}
			else
			{
				State.Charge = StateChgStartPhase1;
			}
			State.Phase = StatePh1;
		}
		else if (EasyKitLink == 0)
		{
			StartMemo();
			IQInWIIQScanOn;
			State.Charge = StateChgIQScan;
			State.Phase = StatePh1;
            // MODIF 2.8
            FlagDisconnectInCharge = 1;
		}
		break;

	case StateChgIQScan:
		if (IQInWIIQScan == 0) // Timeout on IQInWIIQScan
		{
			if (IQWIIQScan != 0)
			{
				IQInWIIQLinkOn;
				State.Charge = StateChgIQLink;
				State.Phase = StatePh1;
			}
			else
			{
				State.Charge = StateChgStartPhase1;
				State.Phase = StatePh1;
			}
		}
		break;

	case StateChgIQLink:
		if (IQInWIIQLink == 0)
		{
			Converter(OFF);
            // MODIF 2.8
            if ((IQWIIQLink != 0) && (IQData.BatCap == 0))
            {
                IQWIIQLinkOff;
            }
            // END
			if (IQWIIQLink != 0)
			{
				// Update IQ data in memo
				// Set flag for reading TPPL param (offsetVreg / Ah+ / Ah-)
				memcpy(&Memo.BatSN[0], &IQData.BatSN[0], 14);
				Memo.BatCap = IQData.BatCap;
				Memo.CapAutoManu = 1; // Force "CapAutoManu = 1" to display capacity
				Memo.BatTemp = IQData.BatTemp;
				Memo.BatTechno = IQData.BatTechno;
				Memo.BatWarning = IQData.BatWarning;
				if (IQData.BatTemp > 55)
					Memo.BatWarning |= 0x04; // High temp
				Memo.EocTemp = IQData.BatTemp;
				switch (IQData.BatTechno)
				{
				case TEC_PBO:
				case TEC_PZQ:
					ChgData.Profile = IONIC;
					break;
				case TEC_COLD:
                    ChgData.Profile = COLD;
                    break;
                case TEC_GEL:
					ChgData.Profile = GEL;
					break;
#ifdef  ENERSYS_EU                     
				case TEC_WF:
                    ChgData.Profile = WF200;
					break;
				case TEC_PNEU:
					ChgData.Profile = PNEU;
					break;
				case TEC_PREM:
				case TEC_PZM:
					ChgData.Profile = PZM;
					break;
                case TEC_LOWCHG:
					ChgData.Profile = LOWCHG;
					break;    
#endif                    
				case TEC_XFC:
                    //#ifndef HAWKER_US
                    // MODIF 3.3 : add NexSys profile in HAWKER_US
                    if (Menu.XFCOffOn != 0)
			            ChgData.Profile = NXBLC;
                    //#else
                    //ChgData.Profile = XFC;
                    //#endif
					break;
//#ifndef HAWKER_US
				case TEC_TPPL:
                    // MODIF 3.3 : add NexSys profile in HAWKER_US
                    if (Menu.XFCOffOn != 0)
                    {
                        if (ChgData.Profile != XFC)
                            ChgData.Profile = TPPL;
                    }                            
					break;
//#endif
				case TEC_HGB:
					ChgData.Profile = IONIC;
					break;
				case TEC_VRLA:
					ChgData.Profile = VRLA;
					break;
				case TEC_OPP:
					ChgData.Profile = OPP;
					break;
				}
				Memo.Profile = ChgData.Profile;
                
                // MODIF 2.8
                if (Menu.XFCOffOn != 0)
                {   
                    if ( (IQData.BatTechno != TEC_GEL) 
                    && (IQData.BatTechno != TEC_TPPL) 
                    && (IQData.BatTechno != TEC_XFC))
                    
                    {
                        DefTechnoOn;
                    }
                }
                else
                {
                    if ((IQData.BatTechno == TEC_TPPL) 
                        || (IQData.BatTechno == TEC_XFC))
                    {
                        DefTechnoOn;
                    }
                }
                
			}
            
            // MODIF 2.8
            switch (AarsParam.NbModLeft)
            {
                case OUTPUT_2_CABLE_6M:  // 2 output cable
                case OUTPUT_2_CABLE_8M:  // 2 output cable
                    SendInChargeDataOn;
                break;
            }
            

			TimeIQ = ChgData.TimerSecCharge + 5L; // Tempo before SoC for decrease of Ubat
			State.Charge = StateChgIQEnd;
			State.Phase = StatePh1;
		}
		// Security
		if ((VBatMaxDef != 0) && (ChgData. Profile != LITH)) // High voltage
		{
			DFovervoltOn;
			State.Charge = StateChgStAvail;
			State.Phase = StatePh1; // Begin next loop with StatePh1
		}
		break;
	case StateChgIQEnd:
		if (ChgData.TimerSecCharge > TimeIQ)
		{
			State.Charge = StateChgStartPhase1;
			State.Phase = StatePh1;
		}
		break;

	case StateChgStartPhase1:
#ifdef DEMO
		IQWIIQLinkOn;
		IQRFTrLinkOn;
		sprintf (&IQData.BatSN[0], "ZD-514215");
		IQData.BatTemp = 20 + (State.TimerSec % 30);
        if (IQData.BatTemp > 56)
            IQData.BatTemp = 56;
		IQData.BatWarning = BatWarningBal1;
		IQData.BatCap = (Menu.Imax * 6)/10;
		IQData.BatTechno = TEC_OPP;
		memcpy(&Memo.BatSN[0], &IQData.BatSN[0], 14);
		Memo.BatCap = IQData.BatCap;
		Memo.CapAutoManu = 1;
		Memo.BatTemp = IQData.BatTemp;
		Memo.BatTechno = IQData.BatTechno;
		Memo.BatWarning = IQData.BatWarning;
		if (IQData.BatTemp > 55)
			Memo.BatWarning |= 0x04; // High temp
			Memo.EocTemp = IQData.BatTemp;
        ModImaxTmp = 1050;    
#endif
        Memo.InitSOC = SocOpenVolt(TempVinit, ChgData.Profile); // %SOC estimation

        /*if (Memo.InitSOC < OVERDIS_THRLD)
        {   
            if (DFoverdis != 0)
                DFoverdisOn;
        }*/
        if (Memo.InitSOC > (100 - Menu.InitDOD))
        {
        	State.Charge = StateChgStAvail;
        	State.Phase = StatePh1;   // Begin next loop with StatePh1
        }
        else
        {
        	StateProfileOn;
        	Converter(ON);
        }
        TimeDefIconsMin = State.TimerSec + TIME_DEFCONSMIN;
        ModImaxTmp = ModImax;
        // MODIF 2.8
        FlagDisconnectInCharge = 1;
        break;

    case StateChgStartPhase2:       // MODIF R2.2
        TimeImaxPhase2 = ChgData.TimerSecCharge + (1L * 3600L);
        TimerChangeIreq = ChgData.TimerSecCharge + (15L * 60L);   // MODIF 2.8
        break;

	case StateChgPhase1:
	case StateChgIncVregPhase1:
	case StateChgPhase2:
	case StateChgStartPhase3:
	case StateChgPhase3:
	case StateChgStartPhase4:
	case StateChgPhase4:

        // Save profile charge time & Ah for available display
		DataLcd.ProfileTime = Memo.ChgTime;
		DataLcd.ProfileAh = Memo.ChgAh;

		if (ChgData. Profile == LITH)
		{
			break;
		}

        if (State.Charge == StateChgPhase3) // MODIF R2.2
        {
            if ((ChgData.TimerSecCharge < (5L * 60L)) // < 5mn min no Ia
            && (ChgData.Profile != OPPIUI))
			{
                State.Charge = StateChgStAvail;
                State.Phase = StatePh1; // MODIF R2.2 : begin next loop with StatePh1 to Convert OFF
            }
        }
#ifdef DEMO
		IQWIIQLinkOn;
		IQRFTrLinkOn;
		sprintf (&IQData.BatSN[0], "ZD-514215");
		Memo.BatWarning |= 0x04; // High temp
		Memo.EocTemp = IQData.BatTemp;
		IQData.BatWarning = BatWarningBal1;
#endif
        // Security
        OffsetIPhase2 = (Memo.Iphase1 * 6) / 100;   // 6%
		
		// MODIF 2.8
		if (Memo.BatCap <= 50)
			IconsMin = 5;
		else
			IconsMin = 15;	
		
        if (OffsetIPhase2 < 40)
            OffsetIPhase2 = 40;
        if (DataR.Ah > ((Memo.BatCap * 15) / 10))  // Cap max = Cap * 1.5
        {   // Modif R2.2
            DFotherBatOn;
            State.Charge = StateChgStAvail;
            State.Phase = StatePh1; // MODIF R2.2 : begin next loop with StatePh1 to Convert OFF
        }
        else if ((State.Charge == StateChgPhase2) 
            && (ChgData.Profile != IONIC)
            && (ChgData.TimerSecCharge > TimeImaxPhase2) && (DataR.Ibat > (Memo.Iphase1 - OffsetIPhase2)))
        {   // Modif R2.2
            DFtimeSecuOn;
			DFdIdTOn;		// MODIF 2.9
            State.Charge = StateChgStAvail;
            State.Phase = StatePh1; // MODIF R2.2 : begin next loop with StatePh1 to Convert OFF
        }
        else if (VBatMaxDef != 0 ) // High voltage
		{
			DFovervoltOn;
			State.Charge = StateChgStAvail;
			State.Phase = StatePh1; // MODIF R2.2 : begin next loop with StatePh1 to Convert OFF
		}
		// MODIF 2.8
		else if ((DataR.Ibat < IconsMin) && (ModCurr != 0))// Courant Min
		{
			if (State.TimerSec > TimeDefIconsMin)
			{
				DFconsMinOn;
				State.Charge = StateChgStAvail;
				State.Phase = StatePh1; // MODIF R2.2 : begin next loop with StatePh1 to Convert OFF
			}
		}
		else if (ChgData.TimerSecCharge > ChgData.TimeMaxProfile) // Time security for profile
		{
			DFtimeSecuOn;
			State.Charge = StateChgStAvail;
			State.Phase = StatePh1; // MODIF R2.2 : begin next loop with StatePh1 to Convert OFF
		}
		else
		{
			TimeDefIconsMin = State.TimerSec + TIME_DEFCONSMIN;
		}


		// MODIF R2.8 : add test to ModImaxTmp
        //if (DataR.Ah >= ModImaxTmp)  // Cap max = Imax (StartPhase1) * 10
        if ((DataR.Ah >= ModImaxTmp) &&  (ChgData.Profile != LOWCHG) && (ModImaxTmp !=0))  // Cap max = Imax (StartPhase1) * 10 - Not in LOWCHG
		{
			DFotherBatOn;
			State.Charge = StateChgStAvail;
            State.Phase = StatePh1; // MODIF R2.2 : begin next loop with StatePh1 to Convert OFF
		}

		// Wi-IQ temperature
		if (IQWIIQLink != 0)
		{
			Memo.EocTemp = IQData.BatTemp; // Update end of charge temperature
			// MODIF V2.7
			if (Memo.ChgAh > IQData.BatCap)
			{
                // MODIF 2.8 : test if bit set to not increase too much DF5 counter
                if (DFotherBat != 0)
                {
                }
                else
                    DFotherBatOn;
			    }
		}
		break;

	case StateChgStAvail:
		SlopeI = 500;
		// MODIF BMS
		if (ChgData.Profile != LITH)
		{
			Converter(OFF);
		}
		DataLcd.ChgSOC = 100;
#ifdef LIFEIQ
		if (FlagIQEqual == ON)
		{ // Force equal in 5 minutes if requested by WiIQ
			ChgData.TimeEqual = ChgData.TimerSecCharge + (5L * 60L);
		}
#endif
		// Save profile charge time & Ah for available display
		DataLcd.ProfileTime = Memo.ChgTime;
		DataLcd.ProfileAh = Memo.ChgAh;
		// Update comp & equal time at end of profile)
		if (StateProfile != 0)
		{
			StateProfileOff;
			if (ChgData.TimeComp < 0xFFFFFFFF)
				ChgData.TimeComp += ChgData.TimerSecCharge;

			if (((ChgData.TimeEqual < (0xFFFFFFFF - 1)) || (RequestEqual != 0)) && (FlagIQEqual == OFF))
			{
				if (RequestEqual != 0)    // Request Equal from button
				{
					ChgData.TimeEqual = 0;
				}
				if (Menu.EqualDelay == 0) // Check if Equal delay
					ChgData.TimeEqual += ChgData.TimerSecCharge;
				else
					ChgData.TimeEqual = ChgData.TimerSecCharge + ((ulong)Menu.EqualDelay * 3600L);

				if ( ChgData.TimeEqual < (ChgData.TimerSecCharge + (5L * 60L))) // in 5mn minimum
					ChgData.TimeEqual = ChgData.TimerSecCharge + (5L * 60L);
			}
			// Daily equal
			else if ( (Menu.EqualPeriod != 0)
					&& (PeriodEqual != 0)
					&& (ChgData.TimeEqual == (0xFFFFFFFF - 1))
					&& (FlagIQEqual == OFF) ) // Equal day
			{
				if (Menu.EqualDelay == 0)
					ChgData.TimeEqual = ChgData.TimerSecCharge + (5L * 60L);
				else
					ChgData.TimeEqual = ChgData.TimerSecCharge + ((ulong)Menu.EqualDelay * 3600L);
			}
			// Start Electrovalve
			if ((DataLcd.ProfileAh > 20) && (Menu.EVtime > 0))
				StateElectOn;
			// Save profile memo
			CloseMemo();
			DataR.TimeBatteryRest = ChgData.TimerSecCharge + (Menu.BatteryRest * 3600L);
		}
		TimeIdleWake = ChgData.TimerSecCharge;
		// Restore charge data before comp or equal
		if ((TempProfileTime != 0) && (TempProfileAh != 0))
		{
			DataLcd.ProfileTime = TempProfileTime;
			DataLcd.ProfileAh = TempProfileAh;
			Memo.Default |= TempMemoDefault;
			Display.ForceTimeUpdate = 1;
		}
		break;

	case StateChgAvail:
        // MODIF 2.8
        FlagDisconnectInCharge = 0;
        // MODIF R2.7 : no float in NXFAST NXSTND NXBLOC PZM LOWCHG
		if ((Menu.FloatingOffOn == ON) 
            && (ChgData.Profile != TPPL) 
            && (ChgData.Profile != XFC)
            && (ChgData.Profile != NXBLC)
            //&& (ChgData.Profile != PZM)
            && (ChgData.Profile != LOWCHG))
		{
            // MODIF R2.7 : no float in Idle Mode
            if (IdleState != 0)
            {
                // Do nothing
            }
            else
			{
			    State.Phase = StatePhFloatInit;
				break;
			}
		}

		if (RequestEqual != 0)
		{ // Idle mode
			if (IdleState != 0)
				ModAckDef();                                      // ack fault due to start module
			ChgData.TimeEqual = TimeIdleWake + TIME_CHECK_IDLE;   // wait 5 seconds for have CAN communication
			if ((TimeIdleWake + TIME_CHECK_IDLE) <= ChgData.TimerSecCharge)
			{
				State.Charge = StateChgStartEqual;
				State.Phase = StatePh1;                             // Begin next loop with StatePh1
			}
		}
		// Daily equal
		else if ( (Menu.EqualPeriod != 0)
				&& (PeriodEqual != 0)
				&& (ChgData.TimeEqual == (0xFFFFFFFF - 1))
				&& (FlagIQEqual == OFF) ) // Equal day
		{
			if (Menu.EqualDelay == 0)
				ChgData.TimeEqual = ChgData.TimerSecCharge + (5L * 60L);
			else
				ChgData.TimeEqual = ChgData.TimerSecCharge + ((ulong)Menu.EqualDelay * 3600L);
		}
		else if((ChgData.TimerSecCharge >= (ChgData.TimeEqual - TIME_CHECK_IDLE - 2L))
				&& (ChgData.TimeEqual != 0xFFFFFFFF))
			//&& ((Menu.EqualPeriod == 0) || ((Menu.EqualPeriod != 0) && (PeriodEqual != 0))))

		{
			if (IdleState != 0)
			{ // Idle mode
				ModAckDef();    // ack fault due to start module
				if ((TimeIdleWake + TIME_CHECK_IDLE) <= ChgData.TimerSecCharge)
				{
					State.Charge = StateChgStartEqual;
					State.Phase = StatePh1; // Begin next loop with StatePh1
				}
			}
			else
				State.Charge = StateChgStartEqual;
			State.Phase = StatePh1; // Begin next loop with StatePh1
		}
		else if (ChgData.TimerSecCharge >= ChgData.TimeComp)
		{ // Idle mode
			if (IdleState != 0)
				ModAckDef();                                        // ack fault due to start module

			if ((TimeIdleWake + TIME_CHECK_IDLE) <= ChgData.TimerSecCharge)
			{
				State.Charge = StateChgStartComp;
				State.Phase = StatePh1;                             // Begin next loop with StatePh1
			}
		}
		else
		{
			TimeIdleWake = ChgData.TimerSecCharge;
		}
		break;

	case StateChgStartComp:
		SlopeI = 500;
		TempProfileTime = DataLcd.ProfileTime;
		TempProfileAh = DataLcd.ProfileAh;
		TempMemoDefault = Memo.Default;
		ModAckDef();    // MODIF P0.5
		Converter(ON);
		break;

	case StateChgStartEqual:
		SlopeI = 500;
		TempProfileTime = DataLcd.ProfileTime;
		TempProfileAh = DataLcd.ProfileAh;
		TempMemoDefault = Memo.Default;
		CloseMemo(); // Save profile memo : End date = Start equal
		// Init equal memo
		TempCap = Memo.BatCap;
		TempProfile = ChgData.Profile;
		ChgData.Profile = EQUAL;
		StartMemo();
		Memo.BatCap = TempCap; // Use capacity of main charge
		ChgData.Profile = TempProfile;
		Memo.InitSOC = SocOpenVolt(DataR.VbatCell, ChgData.Profile);
		Memo.EndSOC = Memo.InitSOC;	// MODIF 3.3
		// Init time
		ChgData.TimeEqual = 0xFFFFFFFF;
		// Init Voltage
		Memo.VgazCell =  2850;
		// Init dVdT
		ChgData.ThrlddVdT = -50;
		ChgData.VdVdTOld = 0;
		ChgData.VdVdTNew = 0;
		ChgData.TimedVdT = ChgData.TimerSecCharge + (15L * 60L);	// 15min
		ModAckDef();    // MODIF P0.5
		Converter(ON);
		Memo.Iphase1 = Menu.Iequal; // Init current
		ChgData.TimeMaxPhase1 = ChgData.TimerSecCharge + ((ulong)Menu.EqualTime * 3600L); // Init time
        if (IQWIIQLink != 0)
		{
		    memcpy(&Memo.BatSN[0], &IQData.BatSN[0], 14);
			Memo.BatCap = IQData.BatCap;
			Memo.CapAutoManu = 1;
			Memo.BatTemp = IQData.BatTemp;
			Memo.BatTechno = IQData.BatTechno;
			Memo.BatWarning = IQData.BatWarning;
			if (IQData.BatTemp > 55)
			    Memo.BatWarning |= 0x04; // High temp
			Memo.EocTemp = IQData.BatTemp;
		}
		break;

	case StateChgComp:
	case StateChgEqual:
		SlopeI = 500;
		// Security
		if ((VBatMaxDef != 0) && (ChgData. Profile != LITH)) // High voltage
		{
			DFovervoltOn;
			State.Charge = StateChgStAvail;
			State.Phase = StatePh1; // Begin next loop with StatePh1
		}
		if (State.Charge == StateChgComp)	// MODIF 3.3
			DataLcd.ChgRestTime = ((ChgData.TimeMaxPhase1 - ChgData.TimerSecCharge) / 3600L) + 1L;
		else
			DataLcd.ChgRestTime = ((TimeMaxEqual - ChgData.TimerSecCharge) / 3600L) + 1L;
		break;

	default:
		break;
	}
}

/**
 * @brief  Initialization of charge parameters
 * @param  None
 * @retval None
 */
void  CHARGE_Init (void)
{
	ArclessDefOff;
	ResetFlagEqualOff;
	PowerMax = ((ulong)Menu.Imax * (ulong)Menu.NbCells * 24L) / 100L;	// Pmax = Imax x 2,4V/cell x NbCells
	Rcable = (((((ulong)Menu.CableLength * 3600L) / (ulong)Menu.CableSection) + 500L) / 100L) + 10L; // 1/10mohm - R(mohm) = ((3.6 * L / S) + 0.5) + 1
	RcableLeft = Rcable;
	RcableRight = Rcable;
    // MODIF 2.8
    RcblMem = Rcable;
	SlopeI = 2000; // 20A/s
	CntDefTemp = 0;
	CntDefCur = 0;
	CntDefCurTmp = 0;
	DataLcd.ProfileTime = 0;
	DataLcd.ProfileAh = 0;
	DataLcd.ChgRestTime = 0;
	DataLcd.ChgSOC = 0;
	ChgData.StopTimerChg = OFF;
	ChgData.TimerSecCharge = 0;
	ChgData.TimeMaxProfile = ChgData.TimerSecCharge + (14L * 3600L);
	ChgData.CntComp = 0;
	ChgData.TimeComp = 0xFFFFFFFF;

	if ((ChgData.Profile == GEL) || (ChgData.Profile == XFC) || (ChgData.Profile == EQUAL))
		ChgData.TimeEqual = 0xFFFFFFFF;
	else
		ChgData.TimeEqual = 0xFFFFFFFF - 1L;

	if (ChgData.Profile != EQUAL)
	{
		TempProfileTime = 0;
		TempProfileAh = 0;
		TempMemoDefault = 0;
	}

	TimeCheckCurrent = 0;
	TimeDefIconsMin = State.TimerSec + TIME_DEFCONSMIN;
	TimeCheckBatOn = State.TimerMs + 2000L;
	TimeCheckBatOff = State.TimerMs + 500L;
    
	TimeCheckBadBat = 0;
	TimeCheckBadBatOff = 0;
	TimeModCur = 0;
	IReqMax = Menu.Imax;
	ModResetEfficiency();
	FlagLoop = OFF;
	FlagThAmbiant = OFF;
	LoopWidthOld = 0;
	LoopWidthNew = 0;
	DeltaLoopWidth = 0;
	DeltaLoopWidthMax = 0;
#ifdef  LIFEIQ
	FlagIQEqual = OFF;
#endif

	// Force config
	Menu.CECOnOff = 0;
    DefTechnoOff;
    TimeOPPIUI = 0;
    
    // MODIF 2.8
    VReqMem = 0;
    Voffset = 0;
    FlagDisconnectInCharge = 0;
    CntHighTpBat = 0;
    TimeCheckRcable = 0;
	FlagVRegPhase1 = 0;
	// MODIF 3.2
	TimeCheckDerating = 0;
}

/**
 * @brief  Manage charge default in chrono
 * @param  None
 * @retval None
 */
void CHARGE_DefChrono (void)
{
	uchar i;
	// MODIF R3.3
	//uchar a;
	//uchar j;
	//sint VbatModule[8];

	if (Menu.ChgType == TYPE_3PHASE)
	{
		TambCompare = 54;
	}
	else
	{
		TambCompare = 56;
	}

	if ((State.Charge & 0x0F) > StateChgInit
			&& (State.Charger > StateChgerStChg))
	{
		if (ForceEqual == 0)
			VbatThrld = 1600;   // Per cell
	}
	else
	{
		VbatThrld = 900;        // Per cell
	}


	if (IdleState == OFF)
	{
        // MODIF 2.8
		//if (State.TimerSec == 0)
        if ((State.TimerSec < 1) && (FlagInit == 0))  // check mod only at startup
		{ // Read if module is plugged
			IdleOn;
		}
		else
		{
			IdleOff;
		}
		// Test if battery is connected or not
		// MODIF 2.8 : add test to FlagDefBatCon
		//if (VbatModCell > VbatThrld)// Battery connected or not
        if ((VbatModCell > VbatThrld) && (FlagDefBatCon == 0))
		{
			if (State.TimerMs > TimeCheckBatOn)
			{
				TimeCheckBatOn = State.TimerMs + 500L;
				BatteryOn;
			}
			TimeCheckBatOff = State.TimerMs + 500L;
		}
		else
		{
			if (State.TimerMs > TimeCheckBatOff)
			{
				TimeCheckBatOff = State.TimerMs + 500L;
				BatteryOff;
				BadBatDefOff;
				ForceEqualOff;
			}
			TimeCheckBatOn = State.TimerMs + 500L;
		}
	}
	else
	{
		// MODIF 2.8
		//if (State.TimerSec == 0)
        if ((State.TimerSec < 1) && (FlagInit == 0))  // check mod only at startup
		{ // Read if module is plugged
			IdleOn;
		}
		else if (State.TimerSec < TIME_CHECK_IDLE)
		{
			IdleOff;
		}

#if DEBUG == 1
		else if (PresBat1Input != 0)
#else
			else if ((PresBat1Input != 0) || (PresBat2Input != 0))
#endif
			{
				if ((State.Charger > StateChgerStChg)
						&& (State.Charge == StateChgAvail)
						&& (Menu.FloatingOffOn == OFF)
						&& (ChgData.Profile != XFC)     // No IDLE mode in XFC due to refresh charge
						&& (ChgData.Profile != TPPL)    // No IDLE mode in TPPL due to refresh charge
                        && (ChgData.Profile != NXBLC)   // MODIF R2.7 : No IDLE mode in NXBLOC due to refresh charge
#ifdef ENERSYS_EU
                        && (ChgData.Profile != PZM)     // No IDLE mode in PZM due to refresh charge
#endif
                        && (ChgData.Profile != LOWCHG)  // No IDLE mode in LOWCHG due to refresh charge
						&& ((TimeIdleWake + 1L) >= ChgData.TimerSecCharge)
						&& ((TimeIdleWake - 1L) <= ChgData.TimerSecCharge)
                        && (ForceIdleOff == 0))
				{
					IdleOn;
				}
				else
				{
					IdleOff;
				}
				if ((State.TimerMs > TimeCheckBatOn) && (CANState != 0) && (CANTimeout == 0) && (ModCurr != 0))
				{
                    if ((VbatModCell == 0) && (State.Charge != StateChgAvail))	// MODIF 3.3 : manage fil pilot with Idle Mode 
					{
                    	 BatteryOff;
                    }
                    else 
					if (((Menu.NbModLeft == 3) || (Menu.NbModLeft == 4)) && ((PresBat1Input == 0) || (PresBat2Input == 0)))
					{
						BatteryOff;
                    }
                    else
                    {   
                    	TimeCheckBatOn = State.TimerMs + 5000L;
                        BatteryOn;
                    }
				}
				TimeCheckBatOff = State.TimerMs + 500;
			}
			else
			{   // MODIF R2.5
				//if ((State.TimerMs > TimeCheckBatOff) && (State.Charger > StateChgerInit) && (VbatModCell < VbatThrld))
                if ((State.TimerMs > TimeCheckBatOff) && (State.Charger > StateChgerInit) && ((VbatModCell < VbatThrld) || (StateIdle !=0)))
				{
					IdleOn;
					TimeCheckBatOff = State.TimerMs + 500L;
					BatteryOff;
					BadBatDefOff;
					ForceEqualOff;
				}
                TimeCheckBatOn = State.TimerMs + 5000L;
			}
	}


	// DF3
	// MODIF R3.3 : not used for the moment
	/*if (State.Charger == StateChgerStChg)
	{	// Check only in start charge
		a = 0;
		ErrorVmodule = 0;

		for (j=0; j<(MODNB-1); j++)
		{
			if ((CanErr(j) == 0) && (LLCRamRO[j].VFuse > 500))
			{
				ErrorVmodule += CheckInputVoltage (LLCRamRO[j].VLmfb, LLCRamRO[j].VBat, LLCRamRO[j].VFuse, 280, 100);
				VbatModule[a] = LLCRamRO[j].VBat;
				a++;
			}
		}
		ErrorVmodule += CheckVbatModule2Module (&VbatModule[0], a, 60);
	}
	// MODIF R3.3
	if (ErrorVmodule !=0)
	{
		BadBatDefOn;
		TimeCheckBadBatOff = State.TimerSec + 1L;
	}
	else*/ if ((Menu.ChgType == TYPE_1PHASE) && (Menu.CellSize == 255) && (Menu.NbCells < 18))
	{
        BadBatDefOn;    // DF3 if charger 1phase 36/48V and 12V or 24V battery
    }
    else if ((LLCRamRO[ModIndex[0]].Id == ID_LLC3KW_3648VM) && (Menu.CellSize == 255) && (Menu.NbCells < 18))
    {
        BadBatDefOn;    // DF3 if charger 1phase 480VAC 36/48V and 12V or 24V battery
    }
    else if ((Menu.CellSize == 255) && (Menu.NbCells > 24))
    {                   // MODIF R2.2
        BadBatDefOn;    // DF3 if charger in AUTO and battery > 48V
    }

    // MODIF 2.8
    //else if ((VbatModCell < 1700) || (VbatModCell > 2400)) 
	else if (((VbatModCell < 1700) || (VbatModCell > 2400)) && (AarsParam.NbModLeft == 0))
	{
		if (State.TimerSec > TimeCheckBadBat)
		{
			if (ForceEqual == 0)
			{
				BadBatDefOn;
			}
			TimeCheckBadBatOff = State.TimerSec + 5L;
		}
	}
    // MODIF 2.8
    else if ( ( (((VbatLeft / NbrLeft) + VBAT_LR_THRLD) < (VbatRight / NbrRight))
            || (((VbatLeft / NbrLeft) - VBAT_LR_THRLD) > (VbatRight / NbrRight)) )
            && (VbatLeft > VBAT_THRLD)
            && (VbatRight > VBAT_THRLD)
            && ((AarsParam.NbModLeft == OUTPUT_2_CABLE_6M) || (AarsParam.NbModLeft == OUTPUT_2_CABLE_8M)) )
    {
        BadBatDefOn;
        TimeCheckBadBatOff = State.TimerSec + 5L;
    }
    // MODIF 2.8
    else if (((VbatModCell < 1700) || (VbatModCell > 2400)) 
        && ((AarsParam.NbModLeft == OUTPUT_2_CABLE_6M) || (AarsParam.NbModLeft == OUTPUT_2_CABLE_8M))
        && (VbatLeft > VBAT_THRLD)
        && (VbatRight > VBAT_THRLD))
	{
		if (State.TimerSec > TimeCheckBadBat)
		{
			if (ForceEqual == 0)
			{
				BadBatDefOn;
			}
			TimeCheckBadBatOff = State.TimerSec + 2L;
		}
	}
	else
	{
		if (State.TimerSec > TimeCheckBadBatOff)
		{
			TimeCheckBadBat = State.TimerSec + 2L;
			BadBatDefOff;
		}
	}

	// VBAT MAX
	if (DataR.VbatCell > (VCELL_MAX - 50))
	{
		if (State.TimerSec > TimeDefVbatMax)
			VBatMaxDefOn;
		else
			VBatMaxDefOff;
	}
	else
	{
		VBatMaxDefOff;
		TimeDefVbatMax = State.TimerSec + 3L;
	}

	// Test pump
	if (StatePump != 0)
	{
		if ((PumpInput != 0) && (State.TimerSec > TimeDefPump))
			PumpDefOn;
		else
			PumpDefOff;
	}
	else
	{
		PumpDefOff;
		TimeDefPump = State.TimerSec + 10L;
	}

	// Test battery high temperature (with com IQ)
    // MODIF 2.8
	//if ((IQWIIQLink != 0) && (IQData.BatTemp > Menu.BatHighTemp) && (EasyKitLink == 0))
    if ((IQWIIQLink != 0) && (IQData.BatTemp > Menu.BatHighTemp) && (EasyKitLink == 0) && (CntHighTpBat >= 3))
		BatHighTpDefOn;
	else
		BatHighTpDefOff;


	// Test battery Low Level (with com IQ)
	if ((IQWIIQLink != 0)
			&& ((IQData.BatWarning & BatDefLowLevel) != 0)
			&& (ChgData.Profile != GEL)
			&& (ChgData.Profile != RGT)
			&& (ChgData.Profile != XFC)
			&& (ChgData.Profile != TPPL)
            && (ChgData.Profile != NXBLC))  // MODIF R2.7
	{
		BatLowLevelDefOn;
	}
	else
		BatLowLevelDefOff;

	/* Manage default in states */
	switch (State.Charger)
	{
	case StateChgerInit:
		State.Phase &= 0x0F;
		break;

	case StateChgerIdle:
	case StateChgerStChg:
#ifndef NO_DEFID
		if (ModDefID != 0)
		{
			State.Charger = StateChgerChg;
			StartStopOn;
			BatteryOn;
			State.Phase |= StatePhDefID;
		}
		else
#endif
            State.Phase &= 0x0F;
		break;

	case StateChgerChg:
		switch (State.Phase & 0xF0)
		{
		if ((State.Phase & 0xF0) != 0)
			TimeDefIconsMin = State.TimerSec + TIME_DEFCONSMIN;

		// Fuse default
		case StatePhDefFuse:
			Converter(OFF);
			ModResetEfficiency();
			if ((Battery != 0) && (FuseDef == 0))
			{
				DFfuseOff;
				InstStartOn;
				State.Charger = StateChgerStChg;
			}
			break;

		case StatePhDefVreg:	// MODIF 3.3
            Converter(OFF);
            if (DFconverter != 0)
            DFconverterOn;    // For memo saving
            break;

        case StatePhDefTechno:
            Converter(OFF);
            break;

		case StatePhDefCurWait:
			Converter(OFF);
			ModResetEfficiency();
			if (FlagLoop == ON)
			{ // Force to restart in loop phase after a current fault
				State.Phase = StatePhLoopInit;
				State.Phase |= StatePhDefCurWait;
			}
			if (ChgData.TimerSecCharge > TimeDefCur)
			{
				DFnetworkOn;    // To save into memo CFC
				State.Phase = (State.Phase & 0x0F) | StatePhDefCurSet;
				TimeAckDef = State.TimerSec + TIME_ACK_DEF;
			}
			break;

		case StatePhDefCurSet:
			/* Current def counter */
			if (CntDefCur > RETRY_DFCUR)
			{
				DFconverterOn;
				State.Phase = (State.Phase & 0x0F) | StatePhDefCur;
			}
			else
			{
				ModAckDef();
				if (TimeAckDef < State.TimerSec)
				{
					Converter(ON);
					DataW.Ireq = TempIDefCur;
					DataW.VreqCell = TempVDefCur;
					TimeCheckCurrent = ChgData.TimerSecCharge + TIME_CHECK_CUR;
					ChgData.IdIdTOld = Menu.Imax;
					State.Phase &= 0x0F;
				}
			}
			break;

		case StatePhDefCur:
			Converter(OFF);
            if ((CntCANFault != 0) && (CANTimeout == 0) && (LLCRamRO[ModIndex[0]].TimerSec > 8)) // MODIF R2.1
            {
                CntCANFault = 0;
            }
			break;

			// Charger temperature default
		case StatePhDefTempWait:
			Converter(OFF);
			ModResetEfficiency();
            // MODIF 2.8
			//ModClearDefTemp();            // Clear Def temp counter
			ChgData.StopTimerChg = ON;    // Stop 'TimerSecCharge'
			if ((FlagThAmbiant == ON) && (TimeThAmb > (15*60L)))
                FlagThAmbiant = OFF;
            if (TempHighDef != 0)
				TimeDefTempWait = State.TimerSec;
			if ((TempHighDef == 0) && (State.TimerSec > (TimeDefTempWait + 2L)) && (FlagThAmbiant == OFF)) // && ((schar)Tambiant < (TambCompare - 4)))
			{
                Tambiant = 0;
				ChgData.StopTimerChg = OFF; // enable 'TimerSecCharge'
				Converter(ON);
				DataW.Ireq = TempIDefTemp;
				DataW.VreqCell = TempVDefTemp;
				ChgData.IdIdTOld = Menu.Imax;
				State.Phase &= 0x0F;
			}
            TimeDefIconsMin = State.TimerSec + TIME_DEFCONSMIN;
            TimeCheckCurrent = ChgData.TimerSecCharge + TIME_CHECK_CUR;
			break;
		case StatePhDefTemp:
			Converter(OFF);
			break;
			// Bad battery default
		case StatePhDefBadBat:
			Converter(OFF);
			ModResetEfficiency();
			if (BadBatDef == 0)
			{
				DFbadBatOff;
				InstStartOn;
				State.Charger = StateChgerStChg;
			}
			break;

			// Analysis of // Battery high temperature default
		case StatePhDefBatHighTp:
			Converter(OFF);
			if ((IQWIIQLink != 0) && (IQData.BatTemp < (Menu.BatHighTemp - 5)))
			{
				DFBatHighTpOff;
				InstStartOn;
				State.Charger = StateChgerStChg;
			}
			break;

			// WiIQ critical low level
		case StatePhDefBatLowLevel:
			Converter(OFF);
			if ((IQWIIQLink != 0) && ((IQData.BatWarning & BatDefLowLevel) == 0))
			{
				BatLowLevelDefOff;
				Converter(ON);
				Memo.BatWarning = IQData.BatWarning;
				Memo.BatWarning |= BatDefLowLevel;
				CloseMemo();    // record memo with LowLevel fault...
				CHARGE_Init();   // restart charge
				State.Charge = StateChgStartPhase1;
				State.Phase = StatePh1;
			}
			break;

		case StatePhDefID:
			Converter(OFF);
			if (ModDefID == 0)
			{
				// MODIF 2.8
				//State.Charge = StateChgStartPhase1;
				State.Charge = StateChgInit;
				State.Phase = StatePh1;
			}
			break;

			// Power cut state request
		case StatePhDefPowerCut:
			Converter(OFF);
			ChgData.StopTimerChg = ON;    // Stop 'TimerSecCharge'
			if (PowerCut == 0)
			{
				ChgData.StopTimerChg = OFF; // Restart 'TimerSecCharge'
				ModResetEfficiency();
				Converter(ON);
				DataW.Ireq = TempIDefPowerCut;
				DataW.VreqCell = TempVDefPowerCut;
				ChgData.IdIdTOld = Menu.Imax;		// MODIF 3.3 : powercut
				State.Phase &= 0x0F;
			}
			TimeDefIconsMin = State.TimerSec + TIME_DEFCONSMIN;	// MODIF 3.3 : powercut
            TimeCheckCurrent = ChgData.TimerSecCharge + TIME_CHECK_CUR;	
			break;

		default:
            // MODIF R2.1
            FlagThAmbiant = OFF;
            
			if (ModDefID != 0)
			{
				State.Phase |= StatePhDefID;
			}
            else if(DefTechno != 0)
            {
                State.Phase |= StatePhDefTechno;
            }
			else if (FuseDef != 0)
			{
				DFfuseOn;
				State.Phase |= StatePhDefFuse;
			}
			else if (BatHighTpDef != 0)
			{
				DFBatHighTpOn;
				State.Phase |= StatePhDefBatHighTp;
			}
			else if ((BatLowLevelDef != 0) && (State.Charge >= StateChgStartPhase1))
			{
				State.Phase |= StatePhDefBatLowLevel;
			}
			else if (PowerCut != 0)
			{
				TempIDefPowerCut = DataW.Ireq;
				TempVDefPowerCut = DataW.VreqCell;
				State.Phase |= StatePhDefPowerCut;
			}
            // MODIF R2.7 : remove TH ambiant
            /*
			else if(((schar)Tambiant > TambCompare) && (StateConverter != 0) && (ChgData.TimerSecCharge > 30L)) // MODIF TH-AMB
			{
                FlagThAmbiant = ON;
                TimeThAmb = 0;
				TempIDefTemp = DataW.Ireq;
				TempVDefTemp = DataW.VreqCell;
            	TimeDefTempWait = State.TimerSec;
            	State.Phase |= StatePhDefTempWait;
		    }
            */
			else  if((TempHighDef != 0) && (StateConverter != 0))
			{
                // MODIF 2.8 : TH
                CntDefTemp = RETRY_DFTH;
				for (i=0; i<MODNB; i++)
                {
                    if ( (/*(ModDefProt(i) != 0)  // MODIF P2.7 : DEFPROT
                    ||*/ (LLCWarn(i) != 0)        // MODIF P2.6 : VLMFB
				    || (ModPwmMax(i) != 0)
				    || (ModOtherDef(i) != 0)
                    || (ModVFuse(i) != 0)) && (CanErr(i) == 0) ) 
                    {
                        
                    }
                    else if ((LLC[i].CntDefTemp <= MAX_MOD_DEF_TEMP) && (CanErr(i) == 0))
                        CntDefTemp = 0;
                }
				if (CntDefTemp < RETRY_DFTH)
				{
					CntDefTemp++;
					TempIDefTemp = DataW.Ireq;
					TempVDefTemp = DataW.VreqCell;
					TimeDefTempWait = State.TimerSec;
					State.Phase |= StatePhDefTempWait;
				}
				else
				{
					DFtempOn;
					State.Phase |= StatePhDefTemp;
				}
			}
			else if((ConverterDef != 0) && (StateConverter != 0) && (ChargeDisconnect == 0)) // MODIF R2.2 : add disconnection
			{
				CntDefCurTmp++;
				// MODIF 2.8
				if (CntDefCurTmp > 2)
				{
					CntDefCurTmp = 0;
					CntDefCur++;
				}
				TempIDefCur = DataW.Ireq;
				TempVDefCur = DataW.VreqCell;
				TimeDefCur = ChgData.TimerSecCharge + TIME_DEF_CUR_WAIT;
				// MODIF R3.1
				if (DefCon != 0)
					State.Phase |= StatePhDefCur;
				else
					State.Phase |= StatePhDefCurWait;
			}
			break;
		}
		break;
	}
}


/**
 * @brief  Manage loop
 * @param  None
 * @retval None
 */
void CHARGE_LoopPhase (void)
{
    uint ImaxLoop;
    
    // MODIF 2.8
    ulong TimeMaxLoop;
    RcblMem = Rcable;
    
    ImaxLoop =  IReqMax;
    
    // MODIF R2.5
    TimeUpdateMemo = State.TimerSec + 30L;
    
    // MODIF 2.8 : use Memo..CapAutoManu instead of Menu.CapAutoManu
    //if ((Menu.CapAutoManu != 0) || (IQWIIQLink != 0))
    if ((Memo.CapAutoManu != 0) || (IQWIIQLink != 0))
    {   // cap manu : limit to 0.3xC
        if (IReqMax > ((Memo.BatCap * 30) / 10))
            ImaxLoop = (Memo.BatCap * 30) / 10;
    }
    
    // MODIF 2.8
    TimeMaxLoop = ImaxLoop / 100;   // Security time in loop (Imax loop / 10A/s)
    
	/* Manage loop */
	switch (State.Phase)
	{
	case StatePhLoopInit:
		TempILoop = DataW.Ireq;
		TempVLoop = DataW.VreqCell;
		TempSlope = SlopeI;
		SlopeI = 6000;
		CntLoop = 0;
		FlagLoop = ON;
		DataW.ILoopLow = (ImaxLoop * 2) / 10;  // 20% Imax
		DataW.ILoopHigh = (ImaxLoop * 8) / 10; // 80% Imax
		if (DataW.ILoopLow < 100)
			DataW.ILoopLow = 100;
		else if (DataW.ILoopLow > 500)
			DataW.ILoopLow = 500;
		DataW.Ireq = 50;
		// MODIF 2.8
		//TimeLoop = ChgData.TimerSecCharge + 40L; // Slope security
        TimeLoop = ChgData.TimerSecCharge + TimeMaxLoop;
		State.Phase = StatePhLoopSet;
		FlagMonte = 0;
		break;
	case StatePhLoopSet:
		if (CntLoop == 3) // 2nd rise of loop
		{
			DISABLE_IRQ();
			if (DataR.Ibat < DataW.ILoopLow)
            {
				DataW.VLoopLow = DataR.VbatCell;
                ILoopLow = DataR.Ibat;
            }
            if (DataR.Ibat < DataW.ILoopHigh)
			{
                DataW.VLoopHigh = DataR.VbatCell;
                ILoopHigh = DataR.Ibat;
            }
			ENABLE_IRQ();
		}
		if (((ItoReach == DataW.Ireq) && (FlagMonte == 1)) || (ChgData.TimerSecCharge > TimeLoop)
				//|| ((FlagMonte == 0) && (DataR.Ibat <=50)))
				|| ((FlagMonte == 0) && (ItoReach == DataW.Ireq) && (DataR.Ibat <= 200)) )
		{
			switch (CntLoop)
			{
			case 0:
				if (DataW.CntLoopPhase == 0)  // Wait time for 5A first loop
					TimeLoop = ChgData.TimerSecCharge + 6L;
				else
					TimeLoop = ChgData.TimerSecCharge + 2L; 
				DataW.Ireq = 50;
				break;
			case 1:
			case 3:
				TimeLoop = ChgData.TimerSecCharge + 2L;
				break;
			case 2:
				TimeLoop = ChgData.TimerSecCharge + 6L; 
				break;
			case 4:
				TimeLoop = ChgData.TimerSecCharge + 1L; 
				break;
			case 5:
				TimeLoop = ChgData.TimerSecCharge;
				break;
			case 6:
			default:
				TimeLoop = ChgData.TimerSecCharge + 2L;
				break;
			}
			State.Phase = StatePhLoopLand;
		}
		break;
	case StatePhLoopLand:
		if (ChgData.TimerSecCharge > TimeLoop)
		{
			SlopeI = TempSlope;
			CntLoop++;
			switch (CntLoop)
			{
			case 1:
			case 3:
				DataR.Vloop1 = DataR.VbatCell;
				DataW.Ireq = ImaxLoop;
				FlagMonte = 1;
				if (State.Charge != StateChgEndLoop)
					DataW.VreqCell = 2800; //VCELL_MAX;
				else
					DataW.VreqCell = 2800;
				State.Phase = StatePhLoopSet;
				break;
			case 2:
			case 4:
				DataW.Ireq = 50;
				FlagMonte = 0;
				if (State.Charge != StateChgEndLoop)
					DataW.VreqCell = 2800;
				else
					DataW.VreqCell = 2800;
				State.Phase = StatePhLoopSet;
				break;
			case 5:
				DataR.Vloop2 = DataR.VbatCell;
				State.Phase = StatePhLoopSet;
				break;
			case 6:
				if (State.Charge != StateChgEndLoop)
					DataW.VreqCell = VCELL_MAX;
				else
					DataW.VreqCell = 2800;
				if (DataW.CntLoopPhase < NB_LOOP)
				{
					if ((Menu.CapAutoManu == 0) && (IQWIIQLink == 0))
					{
						TempCap = (((ulong)(DataW.ILoopHigh - DataW.ILoopLow) * 10L) / ((ulong)(DataW.VLoopHigh - DataW.VLoopLow))) * 4L;
                        //TempCap = (((ulong)(ILoopHigh - ILoopLow) * 10L) / ((ulong)(DataW.VLoopHigh - DataW.VLoopLow))) * 4L;
						if (TempCap < (((ulong)ChargerBatCap * BATCAP_LO) / 100L))
							TempCap = (((ulong)ChargerBatCap * BATCAP_LO) / 100L);
						else if (TempCap > (((ulong)ChargerBatCap * BATCAP_HI) / 100L))
							TempCap = (((ulong)ChargerBatCap * BATCAP_HI) / 100L);
						if( (DataW.CntLoopPhase == 0) || ((DataW.CntLoopPhase >= 1) && (TempCap > Memo.BatCap)) )
							Memo.BatCap = TempCap;
					}

					switch (ChgData.Profile)
					{

					case IONIC:
                        Memo.Iphase1 =  (Memo.BatCap * 18) / 10;
						break;
                        
                    case COLD:
                        Memo.Iphase1 =  (Memo.BatCap * 25) / 10;
                        break;
                        
					case GEL:
					case PZM:
					case RGT:
						Memo.Iphase1 =  (Memo.BatCap * 20) / 10;
						break;

					case PNEU:
						Memo.Iphase1 =  (Memo.BatCap * 22) / 10;
						break;

					case VRLA:
						Memo.Iphase1 =  (Memo.BatCap * 18L) / 10L;
						break;

					default :
                        Memo.Iphase1 =  (Memo.BatCap * 20) / 10;
						break;
					}
					if (Memo.Iphase1 > IReqMax) // IReqMax is set to Menu.Imax in ChargeInit()
					Memo.Iphase1 = IReqMax;   // Menu.Imax can be modified during the charge if a module is losted
					// Is the reason to use IReqMax to limit current after a loop
					TempILoop = Memo.Iphase1;
				}
				else
				{
					//DataW.Ireq = TempILoop;
				}
				FlagMonte = 1;
				State.Phase = StatePhLoopSet;
				break;
			case 7:
			default:
				DataW.VreqCell = TempVLoop;
				DataW.Ireq = TempILoop;
				FlagLoop = OFF; // end of loop
				State.Phase = StatePh1;
				TimeDefIconsMin = State.TimerSec + TIME_DEFCONSMIN;
                // MODIF 2.8
                ModCurrSave = ModCurr;
                TimeCheckRcable = ChgData.TimerSecCharge + 60L;
				break;
			}
			// MODIF 2.8
		    //TimeLoop = ChgData.TimerSecCharge + 40L; // Slope security
            TimeLoop = ChgData.TimerSecCharge + TimeMaxLoop;
		}
		break;
	default:
		break;
	}
}

/**
 * @brief  Manage floating
 * @param  None
 * @retval None
 */
void CHARGE_FloatingPhase (void)
{
	uint i;
	//static  uint TimerStopFloating;
	static  ulong TimerStopFloating;

	switch (State.Phase)
	{
	case StatePhFloatInit:
		for (i=0; i<MODNB; i++)
		{
			if (ModIndex[i] != MODNB)
				ModSimpleAckDef(ModIndex[i]);
		}
		ModSimpleAckDef(ModIndex[0]);
		Converter(ON);
		SlopeItmp = SlopeI;
		SlopeI = 100;
		DataW.VreqCell = Menu.Vfloating;
		//DataW.Ireq = 100; // 10A limitation
		DataW.Ireq = 300; // MODIF R3.3 : 30A limitation
		ItoReach = DataW.Ireq; // Skip set current
		State.Phase = StatePhFloat;
		TimerFloat = ChgData.TimerSecCharge + 5L;
		TimerStopFloating = ChgData.TimerSecCharge + (5L * 60);
		break;
	case StatePhFloat:
		/*if ((DataR.VbatCell >= (Menu.Vfloating + 15)) || (DataR.VbatCell < (Menu.Vfloating-10)))
		{
			if (TimerFloat < ChgData.TimerSecCharge)
			{
				Converter(OFF);
				State.Phase = StatePhFloatStop;
				TimerFloat = ChgData.TimerSecCharge + 5L;
			}
		}
		else
			TimerFloat = ChgData.TimerSecCharge + 5L;*/

		if (DataR.VbatCell >= (Menu.Vfloating + 15))
		{
			if (TimerFloat < ChgData.TimerSecCharge)
			{
				Converter(OFF);
				State.Phase = StatePhFloatStop;
				TimerFloat = ChgData.TimerSecCharge + 10L;
			}
		}
		else if ((DataR.VbatCell < (Menu.Vfloating-20)) && (DataR.Ibat >(DataW.Ireq - 5)))
		{
			if (TimerFloat < ChgData.TimerSecCharge)
			{
				Converter(OFF);
				State.Phase |= StatePhDefVreg;
			}
		}
		else if (DataR.VbatCell < (Menu.Vfloating-20))
		{
			TimerFloat = ChgData.TimerSecCharge + 60L;
		}
		else
			TimerFloat = ChgData.TimerSecCharge + 10L;

		// Stop converter 2s every 5 min in single phase
		if ((TimerStopFloating < ChgData.TimerSecCharge) && (Menu.ChgType == TYPE_1PHASE))
		{
			Converter(OFF);
			if ((TimerStopFloating + 2L) < ChgData.TimerSecCharge)
				State.Phase = StatePhFloatInit;
		}

		if (RequestEqual != 0)
		{
			RequestEqualOff;
		}
		break;
	case StatePhFloatStop:
		SlopeI = SlopeItmp;
		if (DataR.VbatCell < (Menu.Vfloating-10))
		{
			if (TimerFloat < ChgData.TimerSecCharge)
				State.Phase = StatePhFloatInit;
		}
		break;
	default:
		break;
	}
}

/**
 * @brief  Manage converter start/stop
 * @param  None
 * @retval None
 */
void Converter (uchar c)
{
	/* Set converter on or off */
	if (c == OFF)
	{
		ItoReach = 0;
		DataW.Iconsign = 0;
		DataW.Ireq = 0;
		DataW.VreqCell = 0;
		ConverterOff;
		StateFanOff;
		//SlopeI = 10000;            // 40A/sec
	}
	else if (StateConverter == 0)         /* Charge was OFF */
	{
		ItoReach = 0;
		DataW.Iconsign = 0;
		DataW.Ireq = 0;
		DataW.VreqCell = 0;
		ConverterOn;
		StateFanOn;
		//SlopeI = 500;            // 20A/sec
	}
}


/**
 * @brief  Lib function. Calcul % State of charge with open voltage
 * @param  V/Cells - charge profile
 * @retval % SOC
 */
uint SocOpenVolt(uint vcell, uint techno)
{
	sint value;

	switch (techno)
	{
	case GEL:
		value = (((slong)vcell * 61538L) / 10000L) - 12231L;
		break;
	case XFC:
	case TPPL:
		value = ((21896L - ((slong)vcell * 10)) * 100 ) / 2331L;
		value = 100L - value;
		break;
	default:
		value = (((slong)vcell * 53333L) / 10000L) - 10467L;
		break;
	}

	switch (techno)
	{
	case XFC:
	case TPPL:
		if (value < 1)
			value = 5;	// Min = 5%
			else if (value > 99)
				value = 99;	// Max = 99%
		break;

	default:
		value = (value + 5) / 10;
		if (value < 1)
			value = 5;	// Min = 5%
		else if (value < 45)
			value += 5; // +5% for overdischarge detection
		else if (value < 50)
			value = 50;
		else if (value > 99)
			value = 99;	// Max = 99%
		break;
	}

	return ((uint)value);
}

/**
 * @brief  Start Memo recording
 * @param
 * @retval
 */
void StartMemo (void)
{
	// Reset struct memo
	(void)memset((void *)&Memo, 0, sizeof(Memo));
	Status.MemoStatus = START_MEMO;
	// ------- Init Memo -------
	Memo.BatCap = Menu.BatCap;
	Memo.CapAutoManu = Menu.CapAutoManu;
	Memo.BatTemp = Menu.BatTemp;
	Memo.BatTechno = 0;
	Memo.BatWarning = 0;
	Memo.Profile = ChgData.Profile;
	////////////////////////////
	Memo.CFC = 0;
	Memo.BatVolt = Menu.NbCells * 2;
	// Updated in "StartChgInit"
	Memo.InitSOC = 0;
	Memo.SocVoltage = VbatModCell;
	// Updated in "Profile"
	Memo.Iphase1 = 0;
	Memo.VgazCell = 0;
	Memo.Iphase3 = 0;
	// Update in "MemoChrono"
	Memo.Phase = State.Charge;
	Memo.TimePhase1 = 0;
	Memo.TimePhase2 = 0;
	Memo.TimePhase3 = 0;
	Memo.EocVoltage = 0;
	Memo.EocCurrent = 0;
	Memo.EocTemp = Menu.BatTemp;
	Memo.ChgTime = 0;
	Memo.ChgAh = 0;
	Memo.ChgWh = 0;
	Memo.TypeEoc = PARTIAL;
	// Update in "DefChrono"
	Memo.Default = 0;
	// Date
	Memo.SocDate.Year = State.DateR.Year % 100;
	Memo.SocDate.Month = State.DateR.Month;
	Memo.SocDate.Date = State.DateR.Date;
	Memo.SocDate.Hr = State.DateR.Hr;
	Memo.SocDate.Min = State.DateR.Min;
	// Update in "CloseMemo"
	Memo.EocDate.Year = State.DateR.Year % 100;
	Memo.EocDate.Month = State.DateR.Month;
	Memo.EocDate.Date = State.DateR.Date;
	Memo.EocDate.Hr = State.DateR.Hr;
	Memo.EocDate.Min = State.DateR.Min;
	/* Init variables for new memo */
	ChgData.TimeStartCharge = ChgData.TimerSecCharge;
	Ah100ms = 0;
	Wh100ms = 0;
	DataR.Ah = 0;
	DataR.Wh = 0;
	TimeUpdateMemo = 0;
	EocVoltOld = DataR.VbatCell;
	if (DataR.Ibat < 2550)
		EocCurOld = DataR.Ibat / 10;
	else
		EocCurOld = 255;
	// Start update memo
	MemoUpdateOn;
}


/**
 * @brief  Close Memo recording
 * @param
 * @retval
 */
void  CloseMemo(void)
{

#if (TEST_MEMO == 1)
	Memo.ChgAh += 1;
#endif

	if( ((Status.MemoStatus == START_MEMO) || (Status.MemoStatus == IN_MEMO)) &&
			((Memo.ChgAh >= 1) || ((Memo.Default & MaskCritDF) != 0)) )   // Ah > 1 or critical fault
	{
		Memo.ChgTime += 1;  // add 1mn
		Memo.EocDate.Year = State.DateR.Year % 100;
		Memo.EocDate.Month = State.DateR.Month;
		Memo.EocDate.Date = State.DateR.Date;
		Memo.EocDate.Hr = State.DateR.Hr;
		Memo.EocDate.Min = State.DateR.Min;
		if (Status.MemoStatus == START_MEMO)
		{
			// Increment status
			if (Memo.Profile == EQUAL)
			{
				Memo.EndSOC = 100;	// MODIF 3.3
				Status.NbEqual++;
				if (Memo.TypeEoc == PARTIAL)
					Status.NbPartialEqual++;
			}
			else
			{
				Status.NbCharge++;
				if (Memo.TypeEoc == PARTIAL)
					Status.NbPartialCharge++;
				else
					Memo.EndSOC = 100;	// MODIF 3.3
			}
			// Increment memo pointer
			Status.MemoPointer++;
			if (Status.MemoPointer < SEGMEMOTOP)
			{
				Status.MemoPointer = SEGMEMOTOP;
			}
			else if (Status.MemoPointer >= (SEGMEMOTOP + SEGMEMONB))
			{
				Status.NbRstPointer++;
				Status.MemoPointer = SEGMEMOTOP;
			}
		}
		Status.NbTotalAh += Memo.ChgAh;
		Status.MemoStatus = IN_MEMO;
		
        // MODIF 2.8 : save battery disconnection information
        if (FlagDisconnectInCharge != 0)
            BatDisconnectOn;
		else
			BatDisconnectOff;
            
        // Save memo
		if (Memo.EndSOC > 100)
			Memo.EndSOC = 100;
		memcpy(&MemoToSave, &Memo, sizeof(StructMemo));
            
		MemoSaveOn;
	}

#ifdef DEMO
	DFoverdisOn;
	DFtimeSecuOn;
#endif

    // MODIF 2.8 : Add Equal test
	// Send charger info to Wi-IQ : CloseMemo called at end of charge
	if ((IQWIIQLink != 0) && (Memo.Profile != EQUAL))
    { 
        switch (AarsParam.NbModLeft)
        {   
            case OUTPUT_2_CABLE_6M:  // 2 output cable
            case OUTPUT_2_CABLE_8M:  // 2 output cable
                SendEndChargeDataOn;
                break;
        
            default:            // 2 output cable
                IQInWIIQCgInfoOn;
            break;
        }
    }
}

/**
 * @brief  Memo manage in chrono
 * @param
 * @retval
 */
void MemoChrono(void)
{
	if (MemoUpdate != 0)
	{
		if (State.Charger == StateChgerChg)
		{
			if( (State.TimerSec > TimeUpdateMemo) && ((State.Phase == StatePh1) || (State.Phase == StatePh2))
					&& (State.Charge != StateChgEndLoop) ) // No memo update into StateChgEndLoop -> PZM))
			{
				Memo.Phase = State.Charge;
				TimeUpdateMemo = State.TimerSec + 30L;
				Memo.EocVoltage = EocVoltOld;
				Memo.EocCurrent = EocCurOld;
				EocVoltOld = DataR.VbatCell;
				Memo.EndSOC = DataLcd.ChgSOC;		// MODIF 3.3

				if (DataR.Ibat < 2550)
					EocCurOld = DataR.Ibat / 10;
				else
					EocCurOld = 255;

				if (ChgData.TimerSecCharge > ChgData.TimeStartCharge)
					Memo.ChgTime = (ChgData.TimerSecCharge - ChgData.TimeStartCharge) / 60L;
				else
					Memo.ChgTime = 0;

				Memo.ChgAh = DataR.Ah;
				Memo.ChgWh = DataR.Wh;

				switch (State.Charge)
				{
				case StateChgPhase1:
					Memo.TimePhase1 = Memo.ChgTime;
					break;

				case StateChgPhase2:
					Memo.TimePhase2 = Memo.ChgTime - Memo.TimePhase1;
					break;

				case StateChgPhase3:
					Memo.TimePhase3 = Memo.ChgTime - Memo.TimePhase2 - Memo.TimePhase1;
					break;

				default:
					break;
				}
			}
			if (State.Charge == StateChgStAvail)
			{
				Memo.TypeEoc = CPLT;
				Memo.EndSOC = 100;		// MODIF 3.3
				MemoUpdateOff;
			}
		}
		else
		{
			Status.MemoStatus = NO_MEMO;
			MemoUpdateOff;
		}
	}
	/* Stores a new memo in EEPROM */
	if (MemoSave != 0)
	{
		StatusWriteOn;
		MemoWriteOn;
		MemoSaveOff;
	}
}

/**
 * @brief	Calculate state of charge with loop voltage
 * @param  None
 * @retval None
 */
uchar SocLoop(uint vcell)
{
	sint value;

	value = (slong)vcell;
	if (vcell <= 2230)
		value = (((slong)vcell * 399L) / 1000L) - 810L; // Slope 1 (Vbat <= 2.23V) : % charge = (0.399 x Vbattery) - 810
	else
		value = (((slong)vcell * 174L) / 1000L) - 309L; // Slope 2 (Vbat > 2.23V) : % charge = (0.174 x Vbattery) - 309
	if (value < 1)
		value = 1;
	else if (value > 99)
		value = 99;
	return ((uchar)value);
}


/**
 * @brief  Default charger setup
 * @param
 * @retval
 */
void DefaultConfig (void)
{
    // MODIF 2.8
    uchar   NbModLeft = 0;
    
	// MODIF NBCELL AUTO
	switch (Menu.CellSize)
	{ // valid Cell size
	case 6:
	case 12:
	case 18:
	case 24:
    case 30:    // MODIF 2.8
    case 32:
	case 36:
	case 40:
    case 48:
		Menu.NbCells = Menu.CellSize;
		break;

	case 255:
		Menu.NbCells = 12;
		break;

	default:
		Menu.CellSize = 12;
		break;
	}

	uchar NbCellsTmp =  Menu.CellSize;//Menu.NbCells;
	uchar ChgTypeTmp = Menu.ChgType;
	uchar VmainsTmp = Menu.VMains;

	(void)memset((void *)&Menu, 0, sizeof(Menu));
	Menu.TypeSegment = TYPE_MENU;
	Menu.NumSegment = SEGMENU;
	Menu.BatTemp = 20;
#ifdef  ENERSYS_EU
	Menu.Profile = PZM;
	Menu.Area = 0;
#else
	Menu.Profile = IONIC;
	Menu.Area = 1;
#endif

	if (FlagSaveSerial == OFF)
		Menu.NbCells = 12;
	else
		Menu.NbCells = NbCellsTmp;

	Menu.CellSize = Menu.NbCells;
	Menu.CableLength = 40;

	if (FlagSaveSerial == OFF)
	{
		Menu.ChgType = 0;   // Single phase
		Menu.VMains = 0;    // 230VAC
	}
	else
	{
		Menu.ChgType = ChgTypeTmp;
		Menu.VMains = VmainsTmp;
	}

    if (Menu.ChgType == TYPE_3PHASE)
	{
        // MODIF 2.8 : set default value to cable section
        Menu.CableSection = 50;
        switch (ModNumber)
        {
        #ifdef ENERSYS_EU
        case 1:
        case 2:
            Menu.CableSection = 35;
        break;
        case 3:
        case 4:
            Menu.CableSection = 70;
        break;
        case 5:
        case 6:
        // MODIF 2.8
        case 7: 
        case 8:
            if  ((AarsParam.NbModLeft == OUTPUT_2_CABLE_6M) || (AarsParam.NbModLeft == OUTPUT_2_CABLE_8M))
                Menu.CableSection = 70;
            else
                Menu.CableSection = 95;    
        break;
        default : // MODIF R2.5
            Menu.CableSection = 70;
        break;      
        #else
        case 1:
        case 2:
            Menu.CableSection = 50;
        break;
        case 3:
        case 4:
            Menu.CableSection = 95;
        break;
        case 5:
        case 6:
        // MODIF 2.8
        case 7: 
        case 8:
            Menu.CableSection = 95;  
        break;
        default : // MODIF R2.5
            Menu.CableSection = 95;
        break;      
        #endif
        
        }
    }
	else
		//Menu.CableSection = 25;
    {   // MODIF R2.5
        switch (ModNumber)
        {
        #ifdef ENERSYS_EU
        case 5:
        case 6:
            Menu.CableSection = 50;
        break;
        default :
            Menu.CableSection = 25;
        break;        
        #else
        case 5:
        case 6:
            Menu.CableSection = 70;
        break;
        default :
            Menu.CableSection = 25;
        break;
        #endif
        
        }
    }   // END

	Menu.Langage = 1;
	Menu.AutoStartOffOn = 0;
    
    // MODIF R2.5 : save Menu.NbCells
    NbCellsTmp = Menu.NbCells;
    switch (ModChargerID())
    {
        case ID_LLC1KW_3648V :      // 230VAC MONOPHASE
        case ID_LLC3KW_3648VM :     // 480VAC MONOPHASE
            Menu.NbCells = 18;      // For Imax to 36V Max current
            // MODIF R2.5
            //TimeCheckIdle = State.TimerMs - 1;
            TimeCheckIdle = State.TimerSec - 5;
            ModFindImax();
            ModList();
            break;
                    
        //case ID_LLC3KW_243648V :    // 480VAC US
        //case ID_LLC3KW_243648VUS:   // 480VAC US
        //case ID_LLC3KW240_243648V:  // 240VAC
        //case ID_LLC3KW600_243648V:  // 600VAC 
        case ID_LLC3KW_243648V | ID_LLC3KW_243648VUS | ID_LLC3KW_24VM | ID_LLC3KW240_243648V | ID_LLC3KW600_243648V: // MODIF R2.5
        case ID_LLC3KW_243648V | ID_LLC3KW_243648VUS | ID_LLC3KW_3648VM | ID_LLC3KW240_243648V | ID_LLC3KW600_243648V: // MODIF R2.5
            Menu.NbCells = 12;      // For Imax to 24V Max current
            if (LLCRamRO[ModIndex[0]].Id == ID_LLC3KW_3648VM) // MODIF R2.5
                Menu.NbCells = 18;
            // MODIF R2.5
            //TimeCheckIdle = State.TimerMs - 1;
            TimeCheckIdle = State.TimerSec - 5;
            ModFindImax();
            ModList();
            break;
            
        case ID_LLC1KW_7280V :
        //case ID_LLC3KW240_7280V :
        //case ID_LLC3KW_7280V :
        //case ID_LLC3KW600_7280V :
        case ID_LLC3KW_7280V | ID_LLC3KW240_7280V | ID_LLC3KW600_7280V : // MODIF R2.5
            Menu.NbCells = 36;      // For Imax to 72V Max current
            // MODIF R2.5
            //TimeCheckIdle = State.TimerMs - 1;
            TimeCheckIdle = State.TimerSec - 5;
            ModFindImax();
            ModList();
            break;
        case ID_LLC3KW_96V :
            Menu.NbCells = 48;
            TimeCheckIdle = State.TimerSec - 5;
            ModFindImax();
            ModList();
            break;            
    }
    // MODIF R2.5 : restore Menu.NbCells
    Menu.NbCells = NbCellsTmp;
    
    if (FlagSaveSerial == OFF)
    {
        Menu.Imax = 3200;
        Menu.ImaxCharger = 3200;
    }
    else if (ModImax > 0)
    {
        if (ModImax > 3200)
        {
            Menu.Imax = 3200;
            Menu.ImaxCharger = 3200;
        }
        else
        {
            Menu.Imax = ModImax;
            Menu.ImaxCharger = ModImax;
        }    
    }
    else
    {
        Menu.Imax = 3200;
        Menu.ImaxCharger = 3200;
    }
    
	Menu.BatCap = (ulong)(Menu.Imax * BATCAP_C5) / 1000L;
	Menu.Passwd = 99;
	Menu.CapAutoManu = 0;
	ProfileEqualOff;
	Menu.Iequal = 50;
	Menu.EqualDelay = 0;

#ifdef ENERSYS_EU
	Menu.EqualTime = 6;
	Menu.EqualPeriod = 0x00;      // No Equal
	Menu.Daylight = EUROPE;
#else
	Menu.EqualTime = 4;
	Menu.EqualPeriod = 0x40;      // sunday
	Menu.Daylight = USCANADA;
#endif

    Menu.EVtime = 60;
    Menu.Vfloating = 2250;
    Menu.Ifloating = 50;
    Menu.FloatingOffOn = 0;
    Menu.EVOnOff = 1;
    Menu.TimeDisplaySave = 15;      // 15 mn screen saver
    Menu.CoefCharge = 100;

#ifdef ENERSYS_EU
    Menu.IonicCoef = 15;
    Menu.ChgSkipDelay = 0;  // MODIF R2.5
#else
    Menu.IonicCoef = 9;
    Menu.ChgSkipDelay = 1;  // MODIF R2.5
#endif

#ifdef ENERSYS_US
    Menu.Theme = THEMEB;	// MODIF 3.3
#endif


    Menu.VRLAIphase3 = 1;
    Menu.CECOnOff = 0;
    Menu.BlockOutEnable = 0;
    Menu.VncProfileTime = 10;
    BackOff;                    // screen saver ON
    Menu.Contrast = 30;

    Menu.BatHighTemp = 65;
    Menu.Network = JBUS;
    Menu.JbEsc = 1;
    
    MaskJbusOn;
    MaskLanOff;
    MaskWLanOff;
    MaskWAscOff;
    MaskWHexOff;

    InitEthernetParam();
    EraseIQFlashOn;

    Menu.NbModLeft = 8;

    switch (Menu.Area)
    {
        case 0 :
    	    MaskArea0On;
    	    MaskArea1Off;
    	break ;
        case 1 :
    	    MaskArea1On;
    	    MaskArea0Off;
    	break ;
    }
    Uart0Init();
    
    // MODIF 2.8 : In case of factory reset, we memorize output setting (2x6 or 2x4 modules)
    if ((ModNumber >= 6) && (AarsParam.TypeSegment == TYPE_AARS) && (AarsParam.NumSegment == SEGAARS))
    {
        switch (AarsParam.NbModLeft)
        {   
            case OUTPUT_2_CABLE_6M: 
                Menu.NbModLeft = 3;
                if (ModImax > 3200) // No limitation in 2x3
                {
                    Menu.Imax = ModImax;
                    Menu.ImaxCharger = ModImax;
                }  
                break;
                     
            case OUTPUT_2_CABLE_8M: 
                Menu.NbModLeft = 4;
                if (ModImax > 3200) // No limitation in 2x4
                {
                    Menu.Imax = ModImax;
                    Menu.ImaxCharger = ModImax;
                }  
                break;
        
            default:
                Menu.NbModLeft = 8;
                AarsParam.NbModLeft = 0;
            break;
        }
        NbModLeft = AarsParam.NbModLeft;
    }
    // END
    
	Menu.DODMax = 0;
	
    // MODIF R2.1
    memset(&AarsParam, 0 , sizeof(AarsParam));
    AarsParam.TypeSegment = TYPE_AARS;
	AarsParam.NumSegment = SEGAARS;
    AarsParam.EasyKitAddr = 0x00400000;
    // MODIF 2.8
    AarsParam.NbModLeft = NbModLeft;
    AarsParam.ImaxLeft = 3200;
    AarsParam.ImaxRight = 3200;
    // END
	AarsParam.EqTimePhase2 = 4;
    Write_SegFlashData(SEGAARS, (uchar *)&AarsParam);
}

extern StructWifiConfig *TempWifi;
extern StructLanConfig *Temp;

void InitEthernetParam (void)
{
	uint i;

	// Ethernet default setting
	//StructLanConfig *Temp;
	Temp = (StructLanConfig *)&LcdTemp[0];

	//StructWifiConfig *Temp2;
	TempWifi = (StructWifiConfig *)&LcdTemp[0];

	memset(&(*TempWifi), 0 , sizeof(StructWifiConfig));
	memset(&(*TempWifi).WepKey[0], SPACE, 26);
	memset(&(*TempWifi).WLanKey[0], SPACE, 32);
	sprintf(&(*TempWifi).WLanKey[0], "LIFEPASS");
	Write_SegFlashData (SEGWIFI, (uchar *)TempWifi);

	memset(&(*Temp), 0 , sizeof(StructLanConfig)); i= 0;
	memset(&(*Temp).SSID[0], SPACE, 32);
	sprintf(&(*Temp).SSID[0], "LIFENETWORK");
	(*Temp).Security = 3;   // WPA/PSK TKIP

	// IP address
	(*Temp).EthParam[i] = 192;  i++;  // IP = 192.168.1.20
	(*Temp).EthParam[i] = 168;  i++;
	(*Temp).EthParam[i] = 1;    i++;
	(*Temp).EthParam[i] = 20;   i++;
	// DNS
	(*Temp).EthParam[i] = 0;    i++;  // DNS = 0.0.0.0
	(*Temp).EthParam[i] = 0;    i++;
	(*Temp).EthParam[i] = 0;    i++;
	(*Temp).EthParam[i] = 0;    i++;
	// GATEWAY
	(*Temp).EthParam[i] = 192;  i++;  // GW = 192.168.1.1
	(*Temp).EthParam[i] = 168;  i++;
	(*Temp).EthParam[i] = 1;    i++;
	(*Temp).EthParam[i] = 1;    i++;
	// SUBETMASK
	(*Temp).EthParam[i] = 255;  i++;  // SM = 255.255.255.0
	(*Temp).EthParam[i] = 255;  i++;
	(*Temp).EthParam[i] = 255;  i++;
	(*Temp).EthParam[i] = 0;    i++;
	Write_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
    
    IP_DHCP[0] = (*Temp).EthParam[0];
    IP_DHCP[1] = (*Temp).EthParam[1];
    IP_DHCP[2] = (*Temp).EthParam[2];
    IP_DHCP[3] = (*Temp).EthParam[3];
}

/**
 * @brief	Manage option (Pump, Ev ...)
 * @param  None
 * @retval None
 */
void CHARGE_OptionChrono (void)
{
	/* Pump */
	if ((State.Charger != StateChgerChg) && (ManuOption == 0))
		UsePumpOff;

	/* Electrovalve */
	if ((Menu.EVtime > 0) && (ManuOption == 0))
	{
		if (StateElect != 0)
		{
			if (State.TimerSec > TimeElect)
			{
				StateElectOff;
			}
		}
		else
		{
			TimeElect = State.TimerSec + (ulong)Menu.EVtime;
		}
	}

	if (UsePump == 0)
	{
		PumpOff;
		TimePump = 0;
	}
	else
	{
		if (State.TimerSec > TimePump)
		{
			if (StatePump == 0)
			{
				PumpOn;
				TimePump = State.TimerSec + CalculTimePump (); // Pump ON
			}
			else
			{
				PumpOff;
				TimePump = State.TimerSec + 825L; // Activation every 15mn
			}
		}
	}

	if (StateElect == 0)
	{
		ElectroVOff;
	}
	else
	{
		ElectroVOn;
	}
}

/**
 * @brief	Calcul time when pump is on
 * @param  None
 * @retval None
 */
ulong CalculTimePump (void)
{  
	if (Memo.BatCap > 1160)
		return 255L;
	else if  (Memo.BatCap >= 960)
		return 210L;
	else if  (Memo.BatCap >= 760)
		return 180L;
	else if  (Memo.BatCap >= 560)
		return 160L;
	else if  (Memo.BatCap >= 400)
		return 120L;
	else if  (Memo.BatCap >= 240)
		return 90L;
	else if  (Memo.BatCap >= 160)
		return 60L;
	else
		return 30L;
}

/**
 * @brief	Clear AARS param
 * @param  None
 * @retval None
 */
void ClearTPPLParam(void)
{
	AarsParam.AhPlus = 0;
	AarsParam.AhNeg = 0;
	AarsParam.OffsetVreg = 0;
	MenuWriteOn;
}


/**
 * @brief	Test Vbat Range
 * @param  None
 * @retval None
 */
uchar TestVbatRange(uchar n)
{
	/* check if number cell value are in the range */
	if ((DataR.Vbat <= 235 * n) && (DataR.Vbat >= 180 * n))
		return n;
	else
		return 0;
}

/**
 * @brief	Calcul Nb of Cells
 * @param  None
 * @retval None
 */
uchar CalculNbCells(void)
{
	/* Calculate number of battery cell */
    if (TestVbatRange(48) != 0)
		return 48;
	else if (TestVbatRange(40) != 0)
		return 40;
	else if (TestVbatRange(36) != 0)
		return 72;
	else if (TestVbatRange(24) != 0)
		return 24;
	else if (TestVbatRange(18) != 0)
		return 18;
	else if (TestVbatRange(12) != 0)
		return 12;
	else if (TestVbatRange(6) != 0)
		return 6;
	else
		return 12;
}

/**
 * @brief	Check if Flag Desulph
 * @param  None
 * @retval None
 */
void CheckDesulph (void)
{
	if ((VbatModCell < 1950)
			&& (VbatModCell > 500)
			&& (ChgData.Profile != XFC)
			&& (ChgData.Profile != VRLA)
			&& (ChgData.Profile != RGT)
			&& (ChgData.Profile != PNEU)
			&& (ChgData.Profile != OPP)
			&& (ChgData.Profile != GEL)
			&& (ChgData.Profile != TPPL)
            && (ChgData.Profile != NXBLC))
		FlagDesul = ON;
	else
		FlagDesul = OFF;
}

/**
 * @brief	Check Battery temperature in �F
 * @param  None
 * @retval None
 */
void CheckBatTempF (void)
{
	schar tempF;

	tempF = ((Menu.BatTemp * 9) / 5) + 32;

	if ((Menu.BatTempF > (tempF + 3)) || (Menu.BatTempF < (tempF - 3)))
	{
		Menu.BatTempF = tempF;
		MenuWriteOn;
	}
}

void CheckDF4 (void)
{
    TempVinit = VbatModCell;
    Memo.InitSOC = SocOpenVolt(TempVinit, ChgData.Profile); // %SOC estimation
    Memo.EndSOC = Memo.InitSOC;
	Memo.SocVoltage = TempVinit;  // update voltage
	CheckDesulph();
	if (Memo.InitSOC < OVERDIS_THRLD)
	{
	    if (DFoverdis == 0)
            DFoverdisOn;
	}
	else
	{
	    DFoverdisOff;
    }
}

/**
 * @brief	Reset DvDt value
 * @param  None
 * @retval None
 */
void ResetDvDt (void)
{
	ChgData.VdVdTOld = 0;
	ChgData.VdVdTNew = 0;
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

