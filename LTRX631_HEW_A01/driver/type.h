/*
 * type.h
 *
 *  Created on: 23 févr. 2013
 *      Author: duhautf
 */

#ifndef TYPE_H_
#define TYPE_H_

typedef unsigned char uchar;
typedef signed char schar;
typedef unsigned short uint;
typedef signed short sint;
typedef unsigned long ulong;
typedef signed long slong;

typedef unsigned char BYTE;
typedef unsigned char XCHAR;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned long GFX_COLOR;

typedef union
{
	uint32_t Val;
	uint16_t w[2];
	uint8_t  v[4];
	struct
	{
		uint16_t LW;
		uint16_t HW;
	} word;
	struct
	{
		uint8_t LB;
		uint8_t HB;
		uint8_t UB;
		uint8_t MB;
	} byte;
} UINT32_VAL;


// IMPORTANT : ALL STRUCT MUST TO BE ALIGN ON 4 BYTES DUE TO 32 BITS CPU without PRAGMA PACK define
#pragma pack
typedef struct
{
	uint8_t 	ChrgrEna;		// 1 byte	: 1 = charge enable
	uint16_t	ChrgrAcMax;		// 2 bytes	: max AC power
	uint16_t	ChrgrDcU;		// 2 bytes	: Voltage regulation request
	uint16_t	ChrgrDcI;		// 2 bytes	: Current regulation request
	sint		Ibat;			// 2 bytes signed 	: 525 = 52,5A
	sint		Pbat;			// 2 bytes signed 	: 100 = 1,00kW
	uint16_t	U_Pack;			// 2 bytes 	: 2500 = 25,00V
	uint16_t	Soc;			// 2 bytes 	: State of charge %
	uint16_t	Soh;			// 2 bytes 	: State of health %
	sint		Temp;			// 2 bytes signed 	: 525 = 52,5°C
	uint8_t		unused[45];
} StrcutBMS;					// 64 bytes					

#pragma pack
typedef struct
{
	uchar  Year;       		// 1 byte
	uchar  Month;      		// 1 byte
	uchar  Date;       		// 1 byte
	uchar  Hr;         		// 1 byte
	uchar  Min;        		// 1 byte
} StructDateMemo; 			// -> 5 bytes

typedef struct
{	// Must define uint for the sscanf function
	uint  Year;       		// 2 bytes
	uint  Month;      		// 2 bytes
	uint  Date;       		// 2 bytes
	uint  Hr;         		// 2 byte
	uint  Min;        		// 2 bytes
	uint  Sec;        		// 2 bytes
} StructDateTime;     	// -> 12 bytes

#pragma pack
typedef struct
{
	uint TypeSegment : 4;
	uint NumSegment	 : 12;			// 2 bytes

	ulong LvaOn			: 1;		// 4 bytes
	ulong Valert 		: 1;
	ulong Valarm 		: 1;
	ulong Talert 		: 1;
	ulong Talarm 		: 1;
	ulong UseBuzzer 	: 1;
	ulong UseLed 		: 1;
	ulong BatOn			: 1;
	ulong unused 		: 24;

	uint VoltageLevelAlert;			// 2 bytes : voltage level alert (mV/cell)
	uint VoltageLevelAlarm;			// 2 bytes : voltage level alarm (mV/cell)
	uint VoltageStopCond;			// 2 bytes : voltage stop condition (mV/cell)

	uchar Soc;						// 1 bytes : relay setting
	uchar unused1;					// 1 bytes

	schar TempLevelAlert; 			// 1 bytes : temperature Level Alert (�C)
	schar TempLevelAlarm;			// 1 bytes : temperature Level Alarm (�C)
	schar TempLevelCold;			// 1 bytes : temperature Level COLD (�C)
	uchar TempLevelThrld;			// 1 bytes : temperature Threshold (stop condition Alarm)

	uint MemoPointer 	: 15;		// 2 bytes : Memo pointer
	uint RstPointer  	: 1;		//			 Flag reset pointer

	uint GbCntVoltageLevelAlert;	// 2 bytes : global counter Voltage Alert
	uint GbCntVoltageLevelAlarm;	// 2 bytes : global counter Voltage Alarm
	uint GbCntTempLevelAlert;		// 2 bytes : global counter temperature Alert
	uint GbCntTempLevelAlarm;		// 2 bytes : global counter temperature Alert

	uint PaCntVoltageLevelAlert;	// 2 bytes : partial counter Voltage Alert
	uint PaCntVoltageLevelAlarm;	// 2 bytes : partial counter Voltage Alarm
	uint PaCntTempLevelAlert;		// 2 bytes : partial counter temperature Alert
	uint PaCntTempLevelAlarm;		// 2 bytes : partial counter temperature Alarm

	StructDateMemo GlobalDataReset;	 // 5 bytes : date/time global reset counter
	StructDateMemo PartialDataReset; // 5 bytes : date/time last partial reset counter

	slong Vbat;						// 4 bytes : Battery Voltage (2515 = 25,15V)
	slong VbatCell;					// 4 bytes : Battery Voltage per Cell (mV)
	ulong Vgain;					// 4 bytes : Gain to AD conversion Battery Voltage
	sint Voffset;					// 2 bytes : off to AD conversion Battery Voltage
	sint Ibat;						// 2 bytes : Sensor current (50 = 50A)

	uint  Crc;			    		// 2 bytes
} StructLva;    					// -> 64 bytes
#pragma unpack


#pragma pack
typedef struct
{
	uint TypeSegment: 4;
	uint NumSegment	: 12;	// 2 bytes
	ulong AdcIBat;			  // 4 bytes : absolute value
	ulong AdcVBat;			  // 4 bytes : absolute value
	ulong AdcFuse;  			// 4 bytes : absolute value
	ulong AdcTemp;	  		// 4 bytes : absolute value
	ulong AdcProgI;		  	// 4 bytes : absolute value
	ulong AdcProgU;			  // 4 bytes : absolute value
	uint  Ibat;		    		// 2 bytes : 1/10A
	uint  Vbat;				    // 2 bytes : mV
	uint  VbatCell; 			// 2 bytes : mV/cell
	uint  Vcharger;	  		// 2 bytes : mV
	uint  Vfuse;	  	  	// 2 bytes : mV
	schar TempC;		    	// 1 bytes : �C
	uchar BodyType;       // 1 bytes :
	uint  Power;    			// 2 bytes : W
	uint  Ah;	      			// 2 bytes : Ah
	uint  Wh;	      			// 2 bytes : Wh
	uint  PwmTon;		    	// 2 bytes
	uint  PwmToff;	  		// 2 bytes
	uint  VbatMod;        // 2 bytes RX CHANGE
	uint  ProgI;			    // 2 bytes : mV
	uint  ProgU;	    		// 2 bytes : mV
	uint  Vloop1; 		    // 2 bytes : mV
	uint  Vloop2;	  	    // 2 bytes : mV
	ulong TimeBatteryRest;	  	// 4 bytes MODIF IQ RX
	uint  Crc;			    	// 2 bytes
} StructDataR;   	     	// -> 64 bytes

#pragma pack
typedef struct
{
	uint  TypeSegment: 4;
	uint  NumSegment : 12;// 2 bytes
	uint  Ireq;			    	// 2 bytes : 1/10A
	uint  VreqCell;	  		// 2 bytes : mV
	ulong Iconsign;		  	// 4 bytes : mA
	uchar BackRed;	  		// 1 byte
	uchar BackGreen;	  	// 1 byte
	uchar BackBlue;	  		// 1 byte
	//-- Loop --
	uchar CntLoopPhase;   // 1 byte
	sint  ILoopLow;       // 2 bytes
	sint  ILoopHigh;      // 2 bytes
	sint  VLoopLow;       // 2 bytes
	sint  VLoopHigh;      // 2 bytes
	//-- Com IQ --
	uint  IdentCharger;   // 2 bytes
	uint  IQWIIQAddr;     // 2 bytes : Address of Wi-IQ to be read
	uint  IQSegAddr;      // 2 bytes : Address of Wi-IQ segment to be read
	ulong RW;             // 4 bytes : use for bit read/write MOD V3.4
	uchar Unused0[30];		// 34 bytes
	uint  Crc;				    // 2 bytes
} StructDataW;   	     	// -> 64 bytes

#pragma pack
#define		Key		State.UnionKey.StructBits
typedef struct
{
	uint  TypeSegment: 4;
	uint  NumSegment : 12;// 2 bytes
	ulong RW;             // 4 bytes
	ulong RO;				      // 4 bytes
	union
	{
		uint key;					      // 2 bytes
		struct
		{
			uint	StartShort : 1;	// short press
			uint	StartLong : 1;	// long press
			uint	Start : 1;		  // save state
			uint 	UpShort : 1;
			uint	UpLong : 1;
			uint	Up : 1;
			uint	MenuShort : 1;
			uint	MenuLong : 1;
			uint	Menu : 1;
			uint	DownShort : 1;
			uint	DownLong : 1;
			uint	Down : 1;
			uint  RightShort : 1;
			uint  RightLong : 1;
			uint  Right : 1;
			uint  MenuReset : 1;
		}
		StructBits;
	}  UnionKey;

	uint	CursorPos : 15;
	uint	CursorOn : 1;				  // 2 bytes
	uint  Watchdog;				// 2 bytes
	uint  Charger;			  // 2 bytes
	uint  Charge;			    // 2 bytes
	uint  Phase;			    // 2 bytes
	ulong Def;				    // 4 bytes
	ulong Mask;           // 4 bytes mask for menu
	ulong TimerMs;			  // 4 bytes : ms
	ulong TimerSec;			  // 4 bytes : s
	StructDateTime DateW;	// 12 bytes
	StructDateTime DateR;	// 12 bytes
	uint  Crc;				    // 2 bytes
} StructState;				  // -> 64 bytes


#pragma pack
typedef struct
{	// Charge
	uint  TypeSegment: 4;
	uint  NumSegment : 12;// 2 bytes
	uchar CoefPhase3;		  // 1 bytes
	uchar Profile    : 7; // 1 bytes
	uchar StopTimerChg:1;
	ulong TimerSecCharge;	// 4 bytes
	ulong TimeStartCharge;// 4 bytes
	ulong TimeMaxProfile;	// 4 bytes
	ulong TimeMaxPhase1;	// 4 bytes
	ulong TimeMaxPhase2;	// 4 bytes
	ulong TimeMaxPhase3;	// 4 bytes
	ulong TimeLoopPhase;	// 4 bytes
	ulong TimedIdT;			  // 4 bytes
	sint  ThrlddIdT;		  // 2 bytes
	sint  IdIdTOld;			  // 2 bytes
	sint  IdIdTNew;			  // 2 bytes
	ulong TimedVdT;			  // 4 bytes
	sint  ThrlddVdT;		  // 2 bytes
	sint  VdVdTOld;			  // 2 bytes
	sint  VdVdTNew;			  // 2 bytes
	uint  CntComp;			  // 2 bytes
	ulong TimeComp;			  // 4 bytes
	ulong TimeEqual;		  // 4 bytes
	uint  Crc;				  // 2 bytes
} StructChgData;			  // -> 64 bytes

#pragma pack
typedef struct
{
	uint  TypeSegment :  4;
	uint  NumSegment  : 12;   // 2 bytes
	uchar Langage     :  4;
	uchar Area        :  4;	  // 1 byte
	uchar Contrast    :  6;   // 1 byte : 40 to 52
	uchar TimeAuto    :  1;
	uchar SkipComIQ   :  1;
	uchar Profile;			  // 1 byte
	uchar CableSection;		  // 1 bytes : mm�
	uint  CableLength;		  // 2 bytes : dm
	uint  BatCap	    : 15; // 2 bytes : Ah
	uint  CapAutoManu :  1;
	schar BatTemp;			  // 1 byte : �C
	uchar InitDOD;            /* 1 byte : % */
	uint  ChgDelayVal : 14;	  // 2 bytes
	uint  ChgDelayHour:  1;
	uint  ChgSkipDelay:  1;
	uint  Vfloating	  : 15;   // 2 bytes : mV
	uint  FloatingOffOn: 1;
	uint  Ifloating;		  // 2 bytes : 1/10A
	uint  EVtime  	  : 15;   // 2 bytes : s
	uint  EVOnOff     :  1;
	uint  Iequal      : 15;	  // 2 bytes : 1/10A
	uint  RefreshOffOn:  1;
	uchar EqualDelay;		  // 1 byte : h
	uchar EqualTime;		  // 1 byte : h
	uchar EqualPeriod;		  // 1 byte
	uchar Network;			      // 1 byte
	schar BatHighTemp;        // 1 byte
	uchar JbEsc;              // 1 byte
	uchar JbSpeed;            // 1 byte
	uchar Status;             // 1 byte
	uchar DODMax;       	  // 1 byte 
	uchar AutoStartOffOn;     // 1 byte

	uint  ChgType:        2;  // 2 bytes :  0 = single phase / 1 = 3 phases
	uint  VMains:         2;  //            0 = 230VAC
	                          //            1 = 110VAC
	uint  TimeDisplaySave: 7;
	uint  BatteryRest:    5;

	uint  ChgDailyStart: 14;
	uint  ChgDailyHour:   1;
	uint  ChgSkipDaily:   1;
	uint  ChgDailyEnd:   14;
	uint  CECOnOff:       1;
	uint  CellsAutoManu:  1;
	uint  IonicCoef;
	//sint  unused1;
    uint  PLCPulseOffOn:     1;	// 2 bytes : not used in ONBOARD
    uint  Theme:             2; //           not used in ONBOARD
    uint  ModuleID1_ID2:     2; //           0 = master / 1 = slave1  / 2 = slave2
	uint  ExternalCTNOff:	 1;	//			 1 = External CTN not used
	uint  BLEOff:	 		 1;	//			 1 = BLE module off
	uint  Branding :		 4;	// see BRAND_XXXX define in charge.h
	uint  unused2 :          5;
    
	schar BatTempF;
	uchar CellSize;             // using in AUTO mode

	uint  Imax;				    // 2 bytes : 1/10A
	uint  ImaxCharger;          // 2 bytes
	uint  Passwd;			    // 2 bytes
	uint  VncProfileTime;	    // 2 bytes
	uint  CoefCharge:    14;	// 2 bytes : 100 = 1.100
	uint  CoefOnOff:      1;
	uint  LoopOnOff:      1;
	uint  NbCells:        8;    // 1 bytes
	uint  NbModLeft:      8;    // 1 bytes
	uchar VRLAIphase3;			// 1 bytes
	uchar Daylight;             // 1 bytes
	uint  BlockOutStart: 15;    // 2 bytes
	uint  BlockOutEnable: 1;
	uint  BlockOutEnd:   15;    // 2 bytes
	uint  XFCOffOn:       1;
	uint  Crc;			        // 2 bytes
} StructMenu;			        // -> 64 bytes

#pragma pack
typedef struct
{
	uint  TypeSegment: 4;
	uint  NumSegment : 12;  	// 2 bytes
	uint  MemoPointer;      	// 2 bytes
	uchar NbRstPointer;		  	// 1 byte
	uchar MemoStatus;		    // 1 byte
	uint  NbCharge;			    // 2 bytes
	uint  NbPartialCharge;  	// 2 bytes
	uint  NbEqual;			    // 2 bytes
	uint  NbPartialEqual;	  	// 2 bytes
	uint  NbConverterDef; 		// 2 bytes
	uint  NbNetworkDef;		  	// 2 bytes
	uint  NbOtherCurDef;	  	// 2 bytes
	uint  NbFuseDef;	  	  	// 2 bytes
	uint  NbTempDef;		    // 2 bytes
	uint  NbPumpDef;		    // 2 bytes
	uint  NbBadBatDef;		  	// 2 bytes
	uint  NbOverdisDef; 		// 2 bytes
	uint  NbdIdTDef;		    // 2 bytes
	uint  NbdVdTDef;		    // 2 bytes
	uint  NbTimeSecuDef;  		// 2 bytes
	uint  NbOtherBatDef;	  	// 2 bytes
	ulong NbTotalAh;        	// 4 bytes
	//ulong NbPartialAh;      	// 4 bytes
	uint WarningC;      		// 2 bytes	MODIF R3.2
	uint Unused1;      			// 2 bytes	MODIF R3.2
	//uint PourcMaintAh;      	// 2 bytes
	uint Unused2;      			// 2 bytes	MODIF R3.2
	uint NbThModule;			// 2 bytes	MODIF R3.1
	StructDateTime NextDaylight;
	uint Crc;				    // 2 bytes
} StructStatus;			  	  	// -> 64 bytes

#pragma pack
typedef struct
{	// MODIF R3.3
	uint  TypeSegment: 4;
	uint  NumSegment : 12;		// 2 bytes
	uint  EventPointer;      	// MODIF 3.3 : 2 bytes
	uchar NbRstPointer;			// 1 byte
	uint  NbTHMod[8];  			// 16 bytes
	uint  NbDFCMod[8]; 			// 16 bytes
	uchar Unused[25];       	// 25 bytes
	uint  Crc;	            	// 2 bytes
	
} StructE1Status;				// -> 64 bytes

#pragma pack
typedef struct
{	// MODIF R3.3
	uint  TypeSegment: 4;
	uint  NumSegment : 12;		// 2 bytes
	StructDateMemo EventDate;	// 5 bytes
	uchar SlotNumber;			// 1 bytes
	uint  Id;					// 2 bytes	RAMRO
	ulong RamRoDef;				// 4 bytes	RAMRO
	ulong RamRoStatus;			// 4 bytes	RAMRO
	uint  Ver[2];				// 4 bytes	RAMRO
	ulong TimerSec; 			// 4 bytes	RAMRO
	uint  Temp[4];				// 8 bytes	RAMRO
	sint  VLmfb;				// 2 bytes	RAMRO
	sint  VFuse;   				// 2 bytes	RAMRO
	sint  VBat;					// 2 bytes	RAMRO
	sint  IBat;					// 2 bytes	RAMRO
	uint  VReq;					// 2 bytes	RAMRW
	uint  IReq;					// 2 bytes	RAMRW
	uint  Slope;  				// 2 bytes	RAMRW
	uint  PowerMax;				// 2 bytes	RAMRW
	uint  VbatCharger;			// 2 bytes	CHARGER DATA
	uint  IbatCharger;			// 2 bytes	CHARGER DATA
	uint  StateCharger;		  	// 2 bytes	CHARGER DATA
	uint  StateCharge;		    // 2 bytes	CHARGER DATA
	uint  StatePhase;		    // 2 bytes	CHARGER DATA
	uchar Unused[2];       		// 2 bytes
	uint  Crc;	            	// 2 bytes
} StructE1Memo;				// -> 64 bytes

#pragma pack
typedef struct
{
	uint  TypeSegment: 4;
	uint  NumSegment : 12;	// 2 bytes
	uchar SerNum[10];       // 10 bytes : Serial number in EEPROM
	uchar RtcRam[10];       // 10 bytes : Serial number in RTC
	uchar Unused[40];       // 40 bytes
	uint  Crc;	            // 2 bytes
} StructSerialNumber;		// -> 64 bytes

#pragma pack
typedef struct
{
	uint  TypeSegment: 4;
	uint  NumSegment : 12;	// 2 bytes
	uchar HostMsgText0[20]; // 20 bytes
	uchar HostMsgText1[20]; // 20 bytes
	uchar Unused[20];       /* 20 bytes */
	uint  Crc;	            // 2 bytes
} StructHost;		  // -> 64 bytes

#pragma pack
typedef struct      /* EEPROM only */
{
	uint  TypeSegment: 4;
	uint  NumSegment : 12;	// 2 bytes
	uint  Unused0;
	uchar Unused1;
	uchar Unused2;
	uint  DHCP: 1;
	uint  A10100: 2;
	uint  Encrypt : 1;
	uint  Security : 3;
	uint  Config2: 9;	    // 2 bytes
	uchar Unused[6];        // 6 bytes
	uchar EthParam[16];     // 16 bytes
	uchar SSID[32];         // 32 bytes
	uint  Crc;            	// 2 bytes
} StructLanConfig;			// -> 64 bytes

#pragma pack
typedef struct          /* EEPROM only */
{
	uint  TypeSegment: 4;
	uint  NumSegment : 12;  // 2 bytes
	uchar WepKey[26];       // 26 bytes
	uchar WLanKey[32];      // 34 bytes
	uchar Unused[2];        // 6 bytes
	uint  Crc;              // 2 bytes
} StructWifiConfig;         // -> 64 bytes

#pragma pack
typedef struct
{
	uint  TypeSegment: 4;
	uint  NumSegment	: 12;	/* 2 bytes */
	uint  ProgImin;			    /* 2 bytes : mV */
	uint  ProgImax;			    /* 2 bytes : mV */
	uint  ProgUmin;			    /* 2 bytes : mV */
	uint  ProgUmax;			    /* 2 bytes : mV */
	uchar ChargerName[18];      /* 18 bytes */
	uint  Imin;		 		    /* 2 bytes : 1/10A */
	uint  ImaxCharger; 		    // 2 bytes : 1/10A
	uint  Iwindow;			    // 2 bytes : 1/10A
	uint  IconsignInc;		    // 2 bytes : mA
	schar TempCTNmin;		    // 1 byte : �C
	schar TempCTNmax;		    // 1 byte : �C
	schar TempCTNrestart;	    // 1 byte : �C
	uchar NbCells;			    // 1 byte : cells
	uint  VcellMax;			    // 2 bytes : mV/cell
	uint  VcellThrldLow;	    // 2 bytes : mV/cell
	uint  VcellThrldHigh;	    // 2 bytes : mV/cell
	uint  Rshunt;			    // 2 bytes : �ohm
	uint  BatCap;			    // 2 bytes : Ah
	uint  CableLength;		    // 2 bytes : dm
	uchar CableSection;		    // 1 bytes : mm�
	uchar BodyType;             /* Body type  */
	uchar Unused0[8];		    // 8 bytes
	uint  Crc;				    // 2 bytes
} StructChargerConfig;	        // -> 64 bytes

#pragma pack
typedef struct
{
	uint  TypeSegment: 4;
	uint  NumSegment : 12;      // 2 bytes
	uchar BatSN[14];		    // 14 bytes
	uint  BatCap      : 15;	    // 2 bytes : Ah
	uint  CapAutoManu :  1;
	uchar BatVolt;			    // 1 byte : V
	schar BatTemp;			    // 1 byte : �C
	uchar BatTechno;		    // 1 byte
	uchar BatWarning;		    // 1 byte
	uchar Profile;			    // 1 byte
	//uchar Phase;			    // 1 byte
	//uchar CFC;			    // 1 byte
	uchar Phase : 7;			// 1 byte : MODIF 3.3
	uchar CFC : 1;
	uchar EndSOC;				// 1 byte : MODIF 3.3 add end % charge to BLE compatibility
	uchar InitSOC;			    // 1 byte : %
	uint  SocVoltage;		    // 2 bytes : mV
	uint  Iphase1;              // 2 bytes : 1/10A
	uint  VgazCell;			    // 2 bytes : mV
	uint  Iphase3;			    // 2 bytes : 1/10A
	uint  TimePhase1;		    // 2 bytes : mn
	uint  TimePhase2;		    // 2 bytes : mn
	uint  TimePhase3;		    // 2 bytes : mn
	uint  EocVoltage;		    // 2 bytes : mV
	uchar EocCurrent;		    // 1 byte  : A
	schar EocTemp;		        // 1 byte  : �C
	uint  ChgTime;			    // 2 bytes : mn
	uint  ChgAh;			    // 2 bytes : Ah
	uint  ChgWh   :15;		    // 2 byte : 10W
	uint  TypeEoc : 1;
	uint  Default;			    // 2 bytes
	StructDateMemo SocDate;	    // 5 bytes
	StructDateMemo EocDate;	    // 5 bytes
	uint  Crc;				    // 2 bytes
} StructMemo;				    // -> 64 bytes


#pragma pack
typedef struct
{
	uint  TypeSegment: 4;
	uint  NumSegment : 12;  // 2 bytes
	uint  Ibat;             // 2 bytes
	uint  Vbat;             // 2 bytes
	uint  VbatCell;         // 2 bytes : mV/cell
	uint  Vcharger;         // 2 bytes : mV
	uint  Vfuse;            // 2 bytes : mV
	schar TempC;            // 1 bytes : �C
	schar BatTemp;			// 1 byte : �C
	uint  Power;            // 2 bytes : W
	ulong DelayStChg;       // 4 bytes
	uint  ProfileTime;      // 2 bytes : mn
	uint  ProfileAh;        // 2 bytes : Ah
	uchar ChgRestTime;      // 1 byte : H
	uchar ChgSOC;           // 1 byte : %
	ulong StateRW;          // 4 bytes
	ulong StateRO;          // 4 bytes
	ulong StateDef;         // 4 bytes
	ulong StateIQ;          // 4 bytes
	uint  StateCharger;     // 2 bytes
	uint  StateCharge;      // 2 bytes
	uint  StatePhase;       // 2 bytes
	uint  PwmTon;           // 2 bytes
	uint  PwmToff;          // 2 bytes
	uchar Profile;          // 1 byte
	uchar BodyType;         // 1 byte
	ulong TimeBatteryRest;  // 4 bytes
	uchar Unused[4];        // 4 bytes
	uint  Crc;              // 2 bytes
} StructDataLcd;          // -> 64 bytes

#pragma pack
typedef struct
{
	uint  TypeSegment: 4;
	uint  NumSegment : 12;  // 2 bytes
	ulong RFTrAddr;			    // 4 bytes
	ulong WIIQAddr;			    // 4 bytes
	uchar BatSN[14];		    // 14 bytes
	uint  BatCap;			      // 2 bytes : Ah
	uchar NbCells;			    // 1 byte
	uchar NbBalCells;		    // 1 byte
	schar BatTemp;			    // 1 byte : �C
	uchar BatTechno;		    // 1 byte
	uchar BatWarning;		    // 1 byte
	uchar InitSOC;			    // 1 byte : %
	sint  Ibat;				      // 2 bytes : 1/10A
	uint  Vbat;				      // 2 bytes : 1/100V
	uint  VbatCell;			    // 2 bytes : 1/100V
	uint  Vbalance;			    // 2 bytes : 1/100V
	uchar StateIQ;		      // 1 byte
	uchar StateIQPh;		    // 1 byte
	uchar State1;			      // 1 byte
	uchar State2;			      // 1 byte
	ulong Status;		        // 4 bytes
	uchar ScanAddrLen;			// 1 byte
	uchar ScanAddrPtr;			// 1 byte
	uchar CheckAddrPtr;			// 1 byte
	uchar CntCurStep;			  // 1 byte
	ulong RFTrNewAddr;      // 4 bytes
	uchar RFTrVers[4];      // 4 bytes
	uchar RFTrCom     : 1;
	uchar Unused0     : 7;  // 1 byte
	uchar Unused1;          // 1 byte
	uint  Crc;				      // 2 bytes
} StructIQData;			      // -> 64 bytes

#pragma pack
typedef struct
{
	uint  TypeSegment: 4;
	uint  NumSegment : 12;        // 2 bytes
	ulong WIIQAddr;			          // 4 bytes
	uchar TempControl;            // 1 byte : 0 = OFF / 1 = ON
	schar MaxBatTemp;			        // 1 byte : �C
	uchar StartCurrentFactor;     // 1 bytes : % x C
	uchar FinalCurrentFactor;     // 1 byte : % x C
	uchar TimeMaxPhase1;          // 1 byte : hour
	uchar TimeMaxPhase2;          // 1 byte : hour
	uchar TimeMaxProfile;         // 1 byte : hour
	uint  VreqCell;	  		        // 2 bytes : mV
	uint  EqualVmaxCell;	        // 2 bytes : mV
	uchar EqualTime;              // 1 bytes : hour
	uint  EqualCurrentFactor;     // 2 bytes : % x C
	uchar EqualPeriod;            // 1 byte : day in the WEEK
	uchar FlagEqual;              // 1 byte :
	uchar NbCell;                 // 1 byte : NbCells
	uchar TotalTime;              // 1 byte : Total time for profile 2
	uchar FlagTpControl;          // 1 byte : 1 = temperature control with WiIQ
	uchar VregInc;                // 1 byte : Vreg increment for temperature control
	uchar Dummy1;
	uint  AhPlus;                 // 2 byte : Ah+ received from WiIQ AARS
	uint  AhNeg;                  // 2 byte : Ah- received from WiIQ AARS
	sint  OffsetVreg;             // 2 byte : OffsetVreg received from WiIQ AARS
	uint  ImaxLeft;               // 2 byte : I maximum left
	uint  ImaxRight;              // 2 byte : I maximum right
	uchar MaxInputCurrent;        // 1 byte : maximum input current (20 = 20A)
	uchar FlagLimitPower;         // 1 byte :
	uint  MaxInputPower;          // 2 byte : maximum input power (201 = 20.1kW)
	uchar NbModLeft;              // 1 byte : nbr of module in left side
	ulong EasyKitAddr;	          // 4 bytes
	uchar EasyKitOnOff;           // 1 bytes
	uchar IstartFast;             // 1 bytes
	uint  Vregfast;				  // 2 bytes : MODIF A1.4
	uchar DailyChargeDuration;    // 1 byte : time of daily charge (mn)
	uchar EqTimePhase2;       	  // 1 byte : Equal Time Phase2
	uchar Unused[10];
	uint  Crc;                    // 2 bytes
} StructAarsParam; 		        /* 64 bytes */


// MODIF BLE
#pragma pack
typedef struct
{
	uchar DeviceType;			// 1 bytes
	uint  StateCharger;     	// 2 bytes
	uint  StateCharge;      	// 2 bytes
	uint  StatePhase;       	// 2 bytes
	uint  Ibat;             	// 2 bytes
	uint  Vbat;             	// 2 bytes
	uint  VbatCell;         	// 2 bytes : mV/cell
	uint  ProfileTime;      	// 2 bytes : mn
	sint  ProfileAh;        	// 2 bytes : Ah
	uchar ChgRestTime;      	// 1 byte : H
	uchar ChgSOC;           	// 1 byte : %
	schar TempC;            	// 1 bytes : degC
	schar TempB;		    	// 1 byte
	uint  WarningC;				// 2 bytes
	uint  MemoPointer;      	// 2 bytes
	uchar NbRstPointer;		  	// 1 byte
} StructDataBle;          		// -> 26 bytes
#pragma unpack

#pragma pack
typedef struct
{
	uchar version[4];			// 4 bytes
	uchar MacAddress[6];		// 6 bytes
	uint  ModMasterVer;			// 2 bytes
	uint  ModSlave1Ver;			// 2 bytes
	uint  ModSlave2Ver;			// 2 bytes
	uchar Type[16];				// 16 bytes
} StructBle;					// 32 bytes
#pragma unpack

#pragma pack
typedef struct
{
	uchar Ver[8];               /* 8  bytes */
	uchar Date[14];             /* 14 bytes */
	uint  SoftID;               /* 2  bytes */
	uint  StructSize;           /* 2  bytes */
	uchar Unused[38];           /* 38 bytes */
} StructDataRom;              /* 8 bytes */

#pragma unpack
/** LLCRamRO : contents read only parameters of the module */
typedef struct
{
	/* Auto acquit faults bypassed by test : 8 */
	ulong CTN : 1;                    /**< 1 : CTN fault (one at least shorted or open) */
	ulong TempCon : 1;                /**< 1 : Temperature Connector. */
	ulong TempStop : 1;               /**< 1 : temp too high, must stop. */
	ulong Dummy1 : 1;

	ulong VLmfb : 1;                  /**< 1 : VLmfb. */
	ulong VBatMax : 1;                /**< 1 : Overvoltage on module */
	ulong Act : 1;                    /**< 1 : No Activity CANBUS RW */
	ulong Dummy2 : 1;                 /**< 1 : wrong mains frequency */

	/* Non auto acquit faults bypassed by test : 8 */
	ulong Burn : 4;                   /**< 1 : burn sequences pending  */
	ulong ModTest : 1;                /**< 1 : module non tested  */
	ulong ModCal : 1;                 /**< 1 : module non calibrated */
	ulong ModVerif : 1;               /**< 1 : module non verified */
	ulong CanCollision : 1;           /**< 1 : another Can node has send message with module Id */

	/* Critical fault bits : 16 */
	ulong VBatMin : 1;                /**< 1 : VBat non OK */
	ulong VFuseMin : 1;               /**< 1 : VFuse non OK */
	ulong Fuse : 1;                   /**< 1 : fuse broken */
	ulong Vref : 1;                   /**< 1 : Vref fault */

	ulong V12v : 1;                   /**< 1 : 12v voltage incorrect */
	ulong EepValid : 1;               /**< 1 : EepCfg not valid */
	ulong Soft : 1;                   /**< 1 : software is not compatible with hardware */
	ulong ClockFail : 1;              /**< 1 : external crystal oscillator failure */

	ulong PwmCal : 1;                 /**< 1 : Calib of Pwm not possible */
	ulong PwmMax : 1;                 /**< 1 : Pwm reach maximum value (min frequency) */
}
StructBitDef;

/* UnionRamRODef */
#define   BitCTN            	(ulong)0x00000001
#define   BitTempCon            (ulong)0x00000002
#define   BitVLmfb              (ulong)0x00000010
#define   BitVBatMax            (ulong)0x00000020
#define   BitFMains             (ulong)0x00000080

#define   BitModTest            (ulong)0x00001000
#define   BitModCal             (ulong)0x00002000
#define   BitModVerif           (ulong)0x00004000
#define   BitCanColision        (ulong)0x00008000  

#define   BitVbatMin            (ulong)0x00010000
#define   BitVFuseMin           (ulong)0x00020000
#define   BitFuse               (ulong)0x00040000
#define   BitPwmMax             (ulong)0x02000000

#define   ModDefCTN(x)      	(LLCRamRO[(x)].UnionRamRODef.Def & BitCTN)
#define   ModDefTempCon(x)      (LLCRamRO[(x)].UnionRamRODef.Def & BitTempCon)
#define   ModVLmfb(x)           (LLCRamRO[(x)].UnionRamRODef.Def & BitVLmfb)
#define   ModVBatMax(x)         (LLCRamRO[(x)].UnionRamRODef.Def & BitVBatMax)
//#define   ModIRegul(x)          (LLCRamRO[(x)].UnionRamRODef.Def & BitIRegul) // Not used Now
#define   ModFMains(x)          (LLCRamRO[(x)].UnionRamRODef.Def & BitFMains)

#define   ModVbatMin(x)         (LLCRamRO[(x)].UnionRamRODef.Def & BitVbatMin)
#define   ModVFuse(x)           (LLCRamRO[(x)].UnionRamRODef.Def & BitFuse)
#define   ModPwmMax(x)          (LLCRamRO[(x)].UnionRamRODef.Def & BitPwmMax)
#define   ModCanColision(x)     (LLCRamRO[(x)].UnionRamRODef.Def & BitCanColision)  // MODIF 2.8
// MODIF 3.1 : add CTN fault + add TemCon fault
//#define   ModOtherDef(x)        (LLCRamRO[(x)].UnionRamRODef.Def & 0x08007F60)
//#define   ModOtherDef(x)        (LLCRamRO[(x)].UnionRamRODef.Def & 0x08007F61)
#define   ModOtherDef(x)        (LLCRamRO[(x)].UnionRamRODef.Def & 0x08007F63)


/* UnionRamROStatus */
typedef struct
{
	ulong Run : 1;                    /**< 1 : converter running in close loop, update by AD interrupt */
	ulong TempStart : 1;              /**< 1 : temp too high enough, impossible to start. */
	ulong Fan : 1;                    /**< 1 : FanOn */
	ulong Pfc : 1;                    /**< 1 : PFC on */

	ulong Derating : 1;               /**< 1 : Derating active */
	ulong ErrVal : 1;                 /**< 1 : error in RamRW values. */
	ulong Th : 1;                     /**< 1 : Module in thermal fault */
	ulong PwmMin : 1;

	ulong Disconnect : 1;
}
StructBitStatus;

#define   BitTempStop           (ulong)0x00000040
#define   BitPwmMin             (ulong)0x00000080
#define   BitDisconnect         (ulong)0x00000100
// MODIF R2.7 : add bit DefProt
#define   BitDefProt            (ulong)0x00800000

#define   ModDefTempStop(x)     (LLCRamRO[(x)].UnionRamROStatus.Status & BitTempStop)       // 1 = High temperature
#define   ModPwmMin(x)          (LLCRamRO[(x)].UnionRamROStatus.Status & BitPwmMin)         // 1 = PWM minimum value
#define   ModDisconnect(x)      (LLCRamRO[(x)].UnionRamROStatus.Status & BitDisconnect)     // 1 = Battery Disconnection
// MODIF R2.7 : add Hard Prot
#define   ModDefProt(x)         (LLCRamRO[(x)].UnionRamROStatus.Status & BitDefProt)        // 1 = Hard prot fault

typedef struct
{
	/* Frame 0 : broadcast */
	union
	{
		ulong Status;                       /**< Address  0 :  8 bytes : general status word (see bit definition) */
		StructBitStatus BitStatus;
	}
	UnionRamROStatus;
	union
	{
		ulong Def;                          /**< Address  0 :  8 bytes : general status word (see bit definition) */
		StructBitDef BitDef;
	}
	UnionRamRODef;

	/* Frame 1 : timers */
	sint  VBat;                           /**< Address 54 :  2 bytes : battery voltage (1/100 V) */
	sint  IBat;                           /**< Address  8 :  2 bytes : I measured at shunt (1/10 A) */
	ulong TimerSec;                       /**< Address 20 :  4 bytes : timer seconds since power up of module (s) */
	/* Frame 2 : external parameters */
	uint  VMains;
	uint  FMains;
	uint  Power;                          /**< Address 14 :  2 bytes : instant power in (* 0.1 W) */
	uint  PortStatus;                     /**< Address 40 :  2 bytes : output port status bit */
	/* Frame 3 : temperatures */
	uint  Temp[4];                        /**< Address 32 :  8 bytes : temperature in °C (+ 64 °C), 2 temp measurement per word */
	/**< Temp[0] - 64 lsb : CPU
                                        /**< Temp[0] - 64 msb : Tamb
                                        /**< Temp[1] - 64 lsb : +Vbat
                                        /**< Temp[1] - 64 msb : -Vbat
                                        /**< Temp[2] - 64 lsb : Ferrite
                                        /**< Temp[2] - 64 msb : Mos LLC
                                        /**< Temp[3] - 64 lsb : Alim Flyback
                                        /**< Temp[3] - 64 msb : PFC
  /* Frame 4 : measured parameters */
	sint   VLmfb;                         /**< Address  4 :  2 bytes : V Fil pilot (1/100 V) : only if fil pilot connected */
	sint   VFuse;                         /**< Address 10 :  2 bytes : V DC fuse (1/100 V) : permanent measure */
	ulong  ISlope;                        /**< Address 16 :  4 bytes : current consign value after slope (1/10 A) */
	/* Warning : IQ16, take only 16 Msb bit */
	/* Frame 5 : requested values controlled */
	uint  VReq;
	uint  IReq;
	uint  Slope;                          /**< Value of slope after control (A/s)*/
	uint  PowerMax;                       /**<            :  2 bytes : maximum power (derated) */
	/* Frame 6 : internal parameters */
	ulong TimerChg;                       /**< Address 24 :  4 bytes : charging time since beginning of cycle (ms) */
	ulong IRegul;                         /**< current after regulation and derating */
	/* Frame 7 */
	uint  TPwm;                           /**< Address 12 :  2 bytes : period value (0.1 ns) */
	uint  Monitor0;                       /* Monitoring value 0 */
	uint  Monitor1;                       /* Monitoring value 1 */
	uint  Dumm;
	/* Frame 8 : counters */
	ulong CntAh;                          /**< Address 56 :  4 bytes : counter for Ah (1/10A * 3600 s) */
	ulong CntPower;                       /**< Address 60 :  4 bytes : cummulative power for current charge cycle (W) */
	/* Frame 9 */
	uint  Id;                             /**< Address 44 :  2 bytes : ModBus or Can address */
	//uint  Dummy;
	uint  CptDef;                         /**< Address 47 :  2 bytes : fault counter */
	uint  TimerFan;                       /**< Address 46 :  2 bytes : counter for stopping fan (sec) */
	/* Frame 10 */
	// MODIF 2.8
	//uint  Du;
	uint  CntReadId;
	uint  Duu;
	uint  Dumm1;
	uint  Dumm2;
	/* Frame 11 : derating */
	uint  PowerMaxVBat;
	uint  PowerMaxTemp;
	uint  PowerMaxMains;
	uint  Dumm3;
	/* Frame 12 */
	uint  FlashVer;                       /**< Address 20 :  2 bytes : software version of FlashApi */
	uint  Ver[2];                         /**< Address 48 :  4 bytes : software version of main program */
	uint  Dummy3;
	/* Frame 13 */
	ulong  VBatRef;                        /**< Non filtered value */
	ulong  VBatOut;                        /**< Filtered value */
	ulong  VFuseRef;                       /**< Non filtered value */
	ulong  VFuseOut;                       /**< Filtered value */
	/* Frame 14 */
	ulong  VLmfbRef;                       /**< Non filtered value */
	ulong  VLmfbOut;                       /**< Filtered value */
	ulong  IBatRef;                        /**< Non filtered value */
	ulong  IBatOut;                        /**< Filtered value */
	/* Frame15 */
	ulong Dummy0;
	ulong Dummy1;
}
StructLLCRamRO; 		                        /* 64 bytes */


/** RamRW : Contents read write parameters of the module */
typedef struct
{
	/* Frame 0 */
	union
	{
		ulong Status;                       /**< Address  0 :  4 bytes : general status word (see bit definition) */
		struct
		{
			/* First word */
			ulong DefAck : 1;                 /**< 1 : acknowledge faults (low order bit). */
			ulong Reload : 1;                 /**< 1 : reload parameters from configuration */
			ulong Vienna : 1;                 /**< 1 : LLC connected to Vienna (no mains and Pfc temp measurement) */
            ulong FilPil : 1;
			ulong Dummy : 12;
			/* Second word : test purposes */
			ulong Test : 1;                   /**< 1 : General test on */
			ulong TestLedGreen : 1;           /**< 1 : test green led (Test set). */
			ulong TestLedYellow : 1;          /**< 1 : test yellow led (Test set). */
			ulong TestLedRed : 1;             /**< 1 : test red led (Test set). */

			ulong TestLMFB : 1;               /**< 1 : force fil pilot driver (Test set) */
			ulong TestPFC : 1;                /**< 1 : PFC test on (Test set) */
			ulong TestFan : 1;                /**< 1 : Fan test on (Test set) */
			ulong TestWd : 1;                 /**< 1 : Disable WatchDog (Test set) */

			ulong Cal : 1;                    /**< 1 : Disable test cal burn bits */
			ulong Burn : 1;                   /**< 1 : Burn sequences pending */
			ulong TestPwm : 1;                /**< 1 : Manual test of PWM (open loop) */
			ulong DacOut : 1;                 /**< 1 : Enables Dac output on both channels */

			ulong InhAct : 1;                 /**< 1 : Disable Activity timeout */
			ulong InhSlope : 1;               /**< 1 : Disable slope (set to maximum) */
		}
		StructBits;
	}
	UnionStatus;
	ulong Ulong;
	/* Frame 1 */
	uint  VReq;
	uint  IReq;
	uint  Slope;                          /**< Value of slope (positive and negative) */
	uint  PMax;
	/* Frame 2 */
	uint  DateHour[3];                    /*  6 bytes */
	uint  TPwm;                           /**< 1 : Frequency for manual test of PWM (*10 Hz) */
	/* Frame 3 */
	uint  DacA0;
	uint  DacS0;
	uint  DacA1;
	uint  DacS1;
}
StructLLCRamRW; 		                  /* 64 bytes */

typedef struct
{
	/* Internal struct for LLC */
	uint  Status;                       /* 2 bytes */
	uint  CntDefCur : 8;                /* 2 bytes */
	uint  CntDefTemp : 8;
	ulong TimeModAckDef;                /* 4 bytes */
	sint  TambTmp;                      // MODIF TH-AMB
} StructLLC;                          /* 8 bytes */


#endif /* TYPE_H_ */
;
