/*
 * charge.h
 *
 *  Created on: 24/02/2013
 *      Author: duhautf
 */

#ifndef CHARGE_H_
#define CHARGE_H_

#include "type.h"


void Init_Charger (void);
void CHARGE_Chrono (void);
void CHARGE_DefChrono (void);
void CHARGE_Phase (void);
void Converter (uchar c);
uint SocOpenVolt (uint vcell, uint techno);
void CHARGE_Init(void);
void CHARGE_LoopPhase (void);
void CHARGE_FloatingPhase (void);
void CHARGE_OptionChrono (void);
void StartMemo (void);
void CloseMemo(void);
void MemoChrono(void);
uchar SocLoop(uint vcell);
void DefaultConfig (void);
ulong CalculTimePump (void);
void ClearTPPLParam(void);
void InitEthernetParam(void);
uchar TestVbatRange(uchar n);
uchar CalculNbCells(void);
void CheckDesulph (void);
void ResetDvDt (void);
void CheckBatTempF (void);
void CheckDF4 (void);

//#define NO_DEFID

#ifdef  ENERSYS_EU
	#if  defined(SAVOYE)
		#define VERSION                 "SA35"
	#elif defined(SUISSE)
		#define VERSION                 "SU35"
	#elif defined(LIMIT_2900W)
		#define VERSION                 "RL35"
	#else
		#define VERSION                 "L3.5"
	#endif	
#endif


#ifdef  ENERSYS_US
    #define VERSION                 "L3.5"
#endif

#ifdef  HAWKER_US
    #define VERSION                 "L3.5"
#endif


//#define DEBUG               1             // set this flag when debug with E1
#define LIFEIQ                              // LIFE IQ charger
//#define DEMO

/* ---------------- MODULE ID ---------------- */
#define NO_ID                   0

#define ID_LLC1KW_12V           0x00
#define ID_LLC1KW_24V           0x01    // LLC 1KW 24V
#define ID_LLC1KW_3648V         0x02    // LLC 1KW 36/48V
#define ID_LLC1KW_7280V         0x04    // LLC 1KW 72/80V

#define ID_LLC3KW_243648V       0x12    // LLC 3KW 24/36/48V 480VAC EU
#define ID_LLC3KW_7280V         0x14    // LLC 3KW 72/80V 480VAC EU
#define ID_LLC3KW_96V           0x18    // LLC 3KW 96V 480VAC EU

#define ID_LLC3KW_243648VUS     0x22    // LLC 3KW 24/36/48V 480VAC US

#define ID_LLC3KW_24VM          0x31    // LLC 3KW 24V 480VAC MONOPHASE
#define ID_LLC3KW_3648VM        0x32    // LLC 3KW 36/48V 480VAC MONOPHASE

#define ID_LLC3KW240_12V        0x40    // LLC 3KW 12V 240VAC
#define ID_LLC3KW240_243648V    0x42    // LLC 3KW 24/36/48V 240VAC
#define ID_LLC3KW240_7280V      0x44    // LLC 3KW 72/80V 240VAC

#define ID_LLC3KW600_243648V    0x52    // LLC 3KW 24/36/48V 600VAC
#define ID_LLC3KW600_7280V      0x54    // LLC 3KW 72/80V 600VAC


#define	BRAND_NEXSYS_COMPACT	0
#define	BRAND_LIFE_COMPACT		1
#define	BRAND_LIFE_IQ			2
#define	BRAND_LIFE_IQ_NEXSYS	3
#define	BRAND_IMPAQ_PLUS		4
#define	BRAND_IMPAQ_NEXSYS		5
#define	BRAND_MODC				6
#define	BRAND_MODC_NEXSYS		7
#define	BRAND_ENCORE			8

#define MODNB                   9           // Number of modules
#define THEMEA                  0
#define THEMEB                  1


#define POWERCYCLING            0
#define TEST_MEMO		        0
//#define TEST_COMIQ

#define	LCD_NB_CHAR			    16			// LCD DISPLAY 16 characters / line
#define	LCD_NB_LINE			    2
#define NBLANG				    5
#define LANG        		    Menu.Langage

#define	DEGRES				    0xDF //0xB2;

#define LowByte(x)         	    (x & 0x00FF)
#define HighByte(x)        	    (x >> 8)
#define HighWord(x)             (*((uint *)&x + 1))
#define LowWord(x)              (*((uint *)&x))

/* ********** Mapping (integers) ********** */
#define ADRRAMRW              0x00500       /* RAM RW */
#define ADRRAMRO              0x00700       /* RAM RO */
#define ADRROM                0x00100       /* -> ROM version */

/* ********** JBus mapping (words) ********** */
#define ADRJBUSFMC            0x0000
#define SIZJBUSFMC            0x0100        /* bytes */

#define ADRJBUSRAMRW          0x1000
#define SIZJBUSRAMRW          0x0300        //0x0180    /* bytes */

#define ADRJBUSRAMRO          0x2000
#define SIZJBUSRAMRO          0x0020        /* 64 bytes */

#define ADRJBUSROM            0x3000
#define SIZJBUSROM            0x0120       /* 64 bytes */

#define ADRJBUSEEP            0x8000
#define SIZJBUSEEP            0x3580       /* 128 + 300 seg of 32 registers */

/* ********** EEPROM segment definition ********** */
#define SEGWDDATAR              0         /* DataR segment (case of Watchdog fault) */
#define SEGWDDATAW              1         /* State segment (case of Watchdog fault) */
#define SEGWDSTATE              2         /* State segment (case of Watchdog fault) */
#define SEGWDCHGDATA            3         /* Charger data for profile */
/* Text for screen display */
/* Easy or WiIQ data */
#define SEGDEFAULT              6         /* Default segment */
#define SEGMENU                 7         /* Menu segment */
#define SEGSTATUS               8         /* Status segment */
#define SEGSERIAL               9         /* Serial number */
#define SEGLANCONFIG            10        /* LAN config */
#define SEGWIFI                 11        /* Wifi config */
#define SEGAARS                 12        /* Seg AARS */
#define SEGCONFIGTOP            13        

#define	SEGE1STATUS				18		  /* MODIF R3.3 */
#define	SEGE2STATUS				19		  /* MODIF R3.3 */
#define	SEGE1TOP				20		  /* MODIF R3.3 */
#define	SEGE1NB					100		  /* MODIF R3.3 */
#define	SEGE2TOP				428		  /* MODIF R3.3 */
#define	SEGE2NB					160		  /* MODIF R3.3 */

#define SEGMEMOTOP              128       /* First memo segment */
#define SEGMEMONB               300       /* Number of memo segment */


#define	TYPE_MENU			    1
#define	TYPE_STATUS			    2
#define	TYPE_DATAR			    3
#define	TYPE_DATAW			    4
#define	TYPE_STATE		        5
#define	TYPE_CHGDATA		    6
#define	TYPE_IQDATA		        7
#define	TYPE_SERIAL			    8
#define	TYPE_DATALCD		    9
#define	TYPE_AARS			    10
#define	TYPE_E1_STATUS	    	11	/* MODIF R3.3 */
#define	TYPE_E2_STATUS	    	12	/* MODIF R3.3 */
#define	TYPE_MEMO			    15
#define	TYPE_MEMO_LVA		    16	/* MODIF 3.2 - Reserved ONBOARD */


// SCI Buffer
#define SCI1SIZE                256
#define SCI6SIZE                256
#define JBCRC                   0xFFFF    	// Jbus start polynom
#define JBTMP                   4		   	// Time of non activity of JBus before frame reset (in ms)

/* Profiles types */
#define PROFIL_NB   		    23
#define IONIC       		    1
#define GEL         		    2
#define COLD        		    3
#define WF200       		    4
#define PNEU        		    5
#define EQUAL       		    6
#define OPP       		        7
#define VRLA       		        8
#define RGT         		    9
#define OPPIUI                  11
#define PREM        		    12
#define TEST                    13
#define FAST                    14
#define XFC         		    16
#define PZM         		    19
#define TPPL        		    20
#define LOWCHG        		    21
// MODIF R2.7
#define NXBLC        		    22
#define LITH        		    23

/* Ascii char */
#define SPACE       		    32
#define	OFF					    0
#define	ON                      1
// MODIF R2.5					 
#define TIME_INIT   		    15L  		// Delay for hardware init (s) 15s for 600VAC modules
// END
#define TIME_STCHG      	    15L	 		// Delay before start of charge (s)

/* Battery Thresholds voltage */
#define VBAT_THRLD      	    1500	    // Min battery voltage = 15V
#define VFUSE_THRLD     	    900			// Vbat - Vfuse = 7V max



#define OVERDIS_THRLD	  	    10			// % Max Init SoC for overdischarge

// MODIF 2.8
#ifndef ENERSYS_EU
	#define TEMP_THRLD_COLD         15 			// Temperature threshold for cold profile
#else
	#define TEMP_THRLD_COLD         11 			// Temperature threshold for cold profile
#endif

#define VBAT_LR_THRLD           200L        // Voltage threshold left/right */

#define BATCAP_LO       	    65L  		// % min of nominal bat cap
#define BATCAP_HI       	    125L 		// % max of nominal bat cap
#define BATCAP_C5       	    625L 		// IMax coef for C5 calculation

#define XFC_25AH
#define TIME_DEFCONSMIN         60L
#ifdef XFC_25AH
    #define ICONS_MIN           3
#else
    #define ICONS_MIN           10
#endif

#define TIME_VREQ_PH3           10L
#define TIME_CHECK_CUR          14L
#define TIME_CHECK_VBAT         2L
#define TIME_CHECK_IDLE         15L
#define TIME_DEF_CUR_WAIT       8L
#define TIME_ACK_DEF            5L
#define RETRY_DFTH              3
#define RETRY_DFCUR             3

#define THRLD_IMAX              66          // % of Imax module
#define MAX_MOD_DEF_TEMP        3           // Max Def Temp per module
#define MAX_MOD_DEF_CUR         3           // Max Def Cur per module

/* Loops */
#define NB_LOOP     		    4     		// Number of loops for capacity measure
#define TIME_LOOP   		    180L  		// Time between loops
#define VCELL_MAX   		    2900  		// 2.90V/cell
/* End  of charge types */
#define PARTIAL     		    0
#define CPLT        		    1

/* End  of charge types */
#define NO_MEMO     		    0
#define START_MEMO  		    1
#define IN_MEMO     		    2
#define CLOSE_MEMO  		    3

// States
/* Charger states */
#define StateChgerInit          0x00
#define StateChgerIdle          0x01
#define StateChgerStChg         0x02
#define StateChgerChg           0x03
#define StateChgerManu          0x04

/* Charge states */
#define StateChgInit            0x00
#define StateChgIQScan          0x10
#define StateChgIQLink          0x11
#define StateChgIQEnd           0x12
#define StateChgStartPhase1     0x20
#define StateChgPhase1          0x21
#define StateChgIncVregPhase1   0x22		// for IONIC
#define StateChgStartPhase2     0x23
#define StateChgPhase2          0x24
#define StateChgStartPhase3     0x25
#define StateChgPhase3          0x26
#define StateChgStartPhase4     0x27        // New 2/14/11 for VRLA profile
#define StateChgPhase4          0x28        // New 2/14/11 for VRLA profile
#define StateChgEndLoop         0x29	    // Changed from 27 to 29 for VRLA profile 2/14/11
#define StateChgStAvail         0x30
#define StateChgAvail           0x31
#define StateChgStartComp       0x40
#define StateChgComp            0x41
#define StateChgStartEqual      0x50
#define StateChgEqual           0x51


/* Phase states */
#define StatePh1                0x00
#define StatePh2                0x01
#define StatePhLoopInit         0x02
#define StatePhLoopSet          0x03
#define StatePhLoopLand         0x04
#define StatePhFloatInit        0x05
#define StatePhFloat            0x06
#define StatePhFloatStop        0x07
// Phase Def
#define StatePhDefCur           0x10
#define StatePhDefCurWait       0x20
#define StatePhDefCurSet        0x30
#define StatePhDefFuse          0x40
#define StatePhDefTemp          0x50
#define StatePhDefTempWait      0x60
#define StatePhDefTempSet       0x70
#define StatePhDefBadBat        0x80
#define StatePhDefBatHighTp     0x90
#define StatePhDefPowerCut      0xA0
#define StatePhDefBatLowLevel   0xB0
#define StatePhDefID            0xC0
#define StatePhDefVreg          0xD0			// MODIF 3.3
#define StatePhDefTechno        0xE0            // MODIF 2.8

/* ********** State.RW (ulong) ********** */
#define BitStartStop        (ulong)0x00000001  // 1 = charger On
#define BitInitConfig       (ulong)0x00000002  // Inhibit keyboard
#define BitHostMsg1         (ulong)0x00000004  // 1 : Host msg 1 On
#define BitUsbWriteProfile  (ulong)0x00000008  // 1 : led green on
#define BifDefTechno        (ulong)0x00000010  // MODIF 2.8 -> 1 : Techno fault
#define BitManuConsign      (ulong)0x00000020  // 1 : force consign manual mode
#define BitReloadMenu       (ulong)0x00000040  // 1 : force to reload menu segment
#define BitManuEqual        (ulong)0x00000080  // 1 = Manual equal request
#define BitRequestEqual     (ulong)0x00000100  // 1 = Equal requested
#define BitPeriodEqual      (ulong)0x00000200  // 1 = Equal allowed
#define BitManuOption       (ulong)0x00000400  // 1 = Option manu
#define BitManuReg          (ulong)0x00000800  // 1 : force regulated manual mode (read)
#define BitReloadConfig     (ulong)0x00001000  // 1 : force to reload the config from EEPROM
#define BitMenuWrite        (ulong)0x00002000  // Write all config in Eeprom
#define BitRtcWrite         (ulong)0x00004000  // Bit indicating to update Rtc with new values
#define BitRtcRead          (ulong)0x00008000  // Bit indicating to update RtcRam with new values
#define BitStatusWrite      (ulong)0x00010000  // Bit indicating to execute hardcopy
#define BitMemoWrite        (ulong)0x00020000  // Bit indicating to record profile
#define BitStartInst        (ulong)0x00040000  // 1 = instant start without delay
#define BitCAN      	    (ulong)0x00080000  // 1 = CAN BUS ok
#define BitHostMsg0         (ulong)0x00100000  // 1 = indicate to display host message 0
#define BitReadConfig       (ulong)0x00200000  // 1 = indicate to read config
#define BitPowerCut         (ulong)0x00400000  // 1 = indicate power cut mode
#define BitWDog             (ulong)0x00800000  // 1 = indicate WDOG is not refresh
#define BitLoadWIIQ         (ulong)0x01000000  // 1 = indicate request of read WIIQ -> 0 = read done
#define BitEraseIQFlash     (ulong)0x02000000  // 1 = eraze RF flash memory
#define BitUsbWriteMemo     (ulong)0x04000000  // 1 = indicate request write memo
#define BitAutoStart        (ulong)0x08000000  // 0 = indicate auto start
#define BitResetFlagEqual   (ulong)0x10000000  // 1 = indicate to reset flag Equal
#define BitReadTPPL         (ulong)0x20000000  // 1 = indicate to read TPPL param
#define BitForceEqual       (ulong)0x40000000  // 1 = indicate to force equal when DF3
#define BitBleLedBlink    	(ulong)0x80000000  // 1 : blink BLE led

#define StartStop       	(State.RW & BitStartStop)
#define StartStopOn     	(State.RW |= BitStartStop)
#define StartStopOff    	(State.RW &= ~BitStartStop)
#define InitConfig       	(State.RW & BitInitConfig)
#define InitConfigOn     	(State.RW |= BitInitConfig)
#define InitConfigOff    	(State.RW &= ~BitInitConfig)
#define HostMsg1            (State.RW & BitHostMsg1)
#define HostMsg1On          (State.RW |= BitHostMsg1)
#define HostMsg1Off         (State.RW &= ~BitHostMsg1)
#define UsbWriteProfile     (State.RW & BitUsbWriteProfile)
#define UsbWriteProfileOn   (State.RW |= BitUsbWriteProfile)
#define UsbWriteProfileOff  (State.RW &= ~BitUsbWriteProfile)
#define DefTechno      	    (State.RW & BifDefTechno)       // MODIF 2.8
#define DefTechnoOn      	(State.RW |= BifDefTechno)      // MODIF 2.8
#define DefTechnoOff     	(State.RW &= ~BifDefTechno)     // MODIF 2.8
#define ManuConsign     	(State.RW & BitManuConsign)     // NOT USED
#define ManuConsignOn   	(State.RW |= BitManuConsign)    // NOT USED
#define ManuConsignOff  	(State.RW &= ~BitManuConsign)   // NOT USED
#define ReloadMenu      	(State.RW & BitReloadMenu)
#define ReloadMenuOn    	(State.RW |= BitReloadMenu)
#define ReloadMenuOff   	(State.RW &= ~BitReloadMenu)
#define ManuEqual       	(State.RW & BitManuEqual)
#define ManuEqualOn     	(State.RW |= BitManuEqual)
#define ManuEqualOff    	(State.RW &= ~BitManuEqual)
#define RequestEqual    	(State.RW & BitRequestEqual)
#define RequestEqualOn  	(State.RW |= BitRequestEqual)
#define RequestEqualOff 	(State.RW &= ~BitRequestEqual)
#define PeriodEqual     	(State.RW & BitPeriodEqual)
#define PeriodEqualOn   	(State.RW |= BitPeriodEqual)
#define PeriodEqualOff  	(State.RW &= ~BitPeriodEqual)
#define ManuOption      	(State.RW & BitManuOption)
#define ManuOptionOn    	(State.RW |= BitManuOption)
#define ManuOptionOff   	(State.RW &= ~BitManuOption)
#define ManuReg         	(State.RW & BitManuReg)
#define ManuRegOn       	(State.RW |= BitManuReg)
#define ManuRegOff      	(State.RW &= ~BitManuReg)
#define ReloadConfig    	(State.RW & BitReloadConfig)
#define ReloadConfigOn  	(State.RW |= BitReloadConfig)
#define ReloadConfigOff 	(State.RW &= ~BitReloadConfig)
#define MenuWrite        	(State.RW & BitMenuWrite)
#define MenuWriteOn      	(State.RW |= BitMenuWrite)
#define MenuWriteOff     	(State.RW &= ~BitMenuWrite)
#define RtcWrite        	(State.RW & BitRtcWrite)
#define RtcWriteOn      	(State.RW |= BitRtcWrite)
#define RtcWriteOff     	(State.RW &= ~BitRtcWrite)
#define RtcRead        		(State.RW & BitRtcRead)
#define RtcReadOn      		(State.RW |= BitRtcRead)
#define RtcReadOff     		(State.RW &= ~BitRtcRead)
#define StatusWrite         (State.RW & BitStatusWrite)
#define StatusWriteOn       (State.RW |= BitStatusWrite)
#define StatusWriteOff      (State.RW &= ~BitStatusWrite )
#define MemoWrite      		(State.RW & BitMemoWrite)
#define MemoWriteOn    		(State.RW |= BitMemoWrite)
#define MemoWriteOff   		(State.RW &= ~BitMemoWrite)
#define InstStart       	(State.RW & BitStartInst)
#define InstStartOn     	(State.RW |= BitStartInst)
#define InstStartOff    	(State.RW &= ~BitStartInst)
#define CANState     		(State.RW & BitCAN)
#define CANStateOn     		(State.RW |= BitCAN)
#define CANStateOff     	(State.RW &= ~BitCAN)
#define HostMsg0        	(State.RW & BitHostMsg0)
#define HostMsg0On      	(State.RW |= BitHostMsg0)
#define HostMsg0Off     	(State.RW &= ~BitHostMsg0)
#define ReadConfig          (State.RW & BitReadConfig)
#define ReadConfigOn        (State.RW |= BitReadConfig)
#define ReadConfigOff       (State.RW &= ~BitReadConfig)
#define PowerCut        	(State.RW & BitPowerCut)
#define PowerCutOn      	(State.RW |= BitPowerCut)
#define PowerCutOff     	(State.RW &= ~BitPowerCut)
#define StateWDog    	    (State.RW & BitWDog)
#define StateWDogOn  	    (State.RW |= BitWDog)
#define StateWDogOff 	    (State.RW &= ~BitWDog)
#define LoadWIIQ        	(State.RW & BitLoadWIIQ)
#define LoadWIIQOn      	(State.RW |= BitLoadWIIQ)
#define LoadWIIQOff     	(State.RW &= ~BitLoadWIIQ)
#define UsbWriteMemo    	(State.RW & BitUsbWriteMemo)
#define UsbWriteMemoOn  	(State.RW |= BitUsbWriteMemo)
#define UsbWriteMemoOff 	(State.RW &= ~BitUsbWriteMemo)
#define AutoStart       	(State.RW & BitAutoStart)
#define AutoStartOn     	(State.RW |= BitAutoStart)
#define AutoStartOff    	(State.RW &= ~BitAutoStart)
#define ResetFlagEqual  	(State.RW & BitResetFlagEqual)
#define ResetFlagEqualOn    (State.RW |= BitResetFlagEqual)
#define ResetFlagEqualOff   (State.RW &= ~BitResetFlagEqual)
#define ReadTPPL        	(State.RW & BitReadTPPL)
#define ReadTPPLOn      	(State.RW |= BitReadTPPL)
#define ReadTPPLOff     	(State.RW &= ~BitReadTPPL)
#define ForceEqual      	(State.RW & BitForceEqual)
#define ForceEqualOn        (State.RW |= BitForceEqual)
#define ForceEqualOff       (State.RW &= ~BitForceEqual)
#define EraseIQFlash        (State.RW & BitEraseIQFlash)
#define EraseIQFlashOn      (State.RW |= BitEraseIQFlash)
#define EraseIQFlashOff     (State.RW &= ~BitEraseIQFlash)
#define EraseIQFlash        (State.RW & BitEraseIQFlash)
#define EraseIQFlashOn      (State.RW |= BitEraseIQFlash)
#define EraseIQFlashOff     (State.RW &= ~BitEraseIQFlash)
#define BleLedBlink         (State.RW & BitBleLedBlink)
#define BleLedBlinkOn       (State.RW |= BitBleLedBlink)
#define BleLedBlinkOff      (State.RW &= ~BitBleLedBlink)

/* ********** State.RO (ulong) ********** */
#define BitSCI1Car     		(ulong)0x00000001  // Character to be transmitted in SCI1
#define BitSCI6Car     		(ulong)0x00000002  // Character to be transmitted in SCI6
#define BitRefreshLCD 		(ulong)0x00000004  // Rtc type : 0 = 0xA0, 1 = 0xD0
#define BitYellow  		    (ulong)0x00000008
#define BitStateConverter   (ulong)0x00000010  // 1 = converter On
#define BitStateFan    		(ulong)0x00000020  // 1 = fan On
#define BitElect      		(ulong)0x00000040  // Electrovanne
#define BitPump				(ulong)0x00000080  // Pump on or off
#define BitUsePump			(ulong)0x00000100  // Pump on or off
#define BitGreen			(ulong)0x00000200  // Green
#define BitRed				(ulong)0x00000400  // Red
#define BitManu				(ulong)0x00000800  // Manual mode
#define BitTest				(ulong)0x00001000  // Test mode
#define BitTmrSec   		(ulong)0x00002000  // Set every second
#define BitUsbDisk    		(ulong)0x00004000  // 1 = USB disk ready
#define BitMemoSave			(ulong)0x00008000  // Saves a new memo in EEPROM
#define BitMemoUpdate 		(ulong)0x00010000  // 1 = update memo (in profile or equal)
#define BitIdle      		(ulong)0x00020000  // 1 = Idle mode ON
#define BitRTCInit     		(ulong)0x00040000  // 1 = RTC is init
#define BitProfile    		(ulong)0x00080000  // 1 = profile
#define BitUsbRfProg   		(ulong)0x00100000  // 1 = enter the USB RF flash procedure
#define BitVncRdf     		(ulong)0x00200000  // 1 = new data available after a RDF command
#define BitCANTimeout  		(ulong)0x00400000  // 1 : CAN Timeout detected
#define BitBmsState 		(ulong)0x00800000  // 1 : BMS connected
#define BitU2XonXoff  		(ulong)0x01000000  // 1 : transmit suspended on Uart1
#define BitI2CBusy   		(ulong)0x02000000  // 1 : I2C bus is busy
#define BitRestart    		(ulong)0x04000000  // 1 : restart for bootloader
#define BitTFTBacklight		(ulong)0x08000000  // 0 : Vnc host, 1 : Vnc device
#define BitVncTr			(ulong)0x10000000  // 1 : transmit mode active for the Vinculum
#define BitTimeSummer 		(ulong)0x20000000  // 1 : summer time
#define BitUsbE1Memo 		(ulong)0x40000000  // MODIF R3.3 : 1 : Write E1 Memo
#define BitChargeDisconnect	(ulong)0x80000000  // 1 : hardware fault (arcless, no bat, overvoltage)


#define ConverterOff		{StateConverterOff;}
#define ConverterOn			{StateConverterOn;}


#define SCI1Car        		(State.RO & BitSCI1Car)
#define SCI1CarOn      		(State.RO |= BitSCI1Car)
#define SCI1CarOff     		(State.RO &= ~BitSCI1Car)
#define SCI6Car        		(State.RO & BitSCI6Car)
#define SCI6CarOn      		(State.RO |= BitSCI6Car)
#define SCI6CarOff     		(State.RO &= ~BitSCI6Car)
#define RefreshLCD     		(State.RO & BitRefreshLCD)
#define RefreshLCDOn     	(State.RO |= BitRefreshLCD)
#define RefreshLCDOff 		(State.RO &= ~BitRefreshLCD)
#define StateYellow			(State.RO & BitYellow)
#define StateYellowOn		(State.RO |= BitYellow)
#define StateYellowOff	    (State.RO &= ~BitYellow)
#define StateConverter		(State.RO & BitStateConverter)
#define StateConverterOn	(State.RO |= BitStateConverter)
#define StateConverterOff	(State.RO &= ~BitStateConverter)
#define StateFan        	(State.RO & BitStateFan)
#define StateFanOn      	(State.RO |= BitStateFan)
#define StateFanOff     	(State.RO &= ~BitStateFan)
#define StateElect      	(State.RO & BitElect)
#define StateElectOn    	(State.RO |= BitElect)
#define StateElectOff   	(State.RO &= ~BitElect)
#define StatePump       	(State.RO & BitPump)
#define StatePumpOn     	(State.RO |= BitPump)
#define StatePumpOff    	(State.RO &= ~BitPump)
#define UsePump         	(State.RO & BitUsePump)
#define UsePumpOn       	(State.RO |= BitUsePump)
#define UsePumpOff      	(State.RO &= ~BitUsePump)
#define StateGreen      	(State.RO & BitGreen)
#define StateGreenOn    	(State.RO |= BitGreen)
#define StateGreenOff   	(State.RO &= ~BitGreen)
#define StateRed		  	(State.RO & BitRed)
#define StateRedOn      	(State.RO |= BitRed)
#define StateRedOff     	(State.RO &= ~BitRed)
#define ManuSw			  	(State.RO & BitManu)
#define ManuSwOn		  	(State.RO |= BitManu)
#define ManuSwOff	  		(State.RO &= ~BitManu)
#define TestSw				(State.RO & BitTest)
#define TestSwOn		  	(State.RO |= BitTest)
#define TestSwOff	  		(State.RO &= ~BitTest)
#define TmrSec				(State.RO & BitTmrSec)
#define TmrSecOn		  	(State.RO |= BitTmrSec)
#define TmrSecOff			(State.RO &= ~BitTmrSec)
#define UsbDisk			  	(State.RO & BitUsbDisk)
#define UsbDiskOn	  		(State.RO |= BitUsbDisk)
#define UsbDiskOff			{(State.RO &= ~BitUsbDisk); VncCfgPtrSave = 0; VncCfgPtrOpen = 0;}
#define MemoSave			(State.RO & BitMemoSave)
#define MemoSaveOn			(State.RO |= BitMemoSave)
#define MemoSaveOff			(State.RO &= ~BitMemoSave)
#define MemoUpdate			(State.RO & BitMemoUpdate)
#define MemoUpdateOn		(State.RO |= BitMemoUpdate)
#define MemoUpdateOff		(State.RO &= ~BitMemoUpdate)
#define StateIdle			(State.RO & BitIdle)
#define StateIdleOn		    (State.RO |= BitIdle)
#define StateIdleOff		(State.RO &= ~BitIdle)
#define UsbRfProg		    (State.RO & BitUsbRfProg)
#define UsbRfProgOn	  	    (State.RO |= BitUsbRfProg)
#define UsbRfProgOff 		(State.RO &= ~BitUsbRfProg)
#define VncRdf		      	(State.RO & BitVncRdf)
#define VncRdfOn	    	(State.RO |= BitVncRdf)
#define VncRdfOff       	(State.RO &= ~BitVncRdf)
#define CANTimeout 	     	(State.RO & BitCANTimeout)
#define CANTimeoutOn        (State.RO |= BitCANTimeout)
#define CANTimeoutOff       (State.RO &= ~BitCANTimeout)
#define BmsState      		(State.RO & BitBmsState)
#define BmsStateOn    		(State.RO |= BitBmsState)
#define BmsStateOff   		(State.RO &= ~BitBmsState)
#define U2XonXoff			(State.RO & BitU2XonXoff)
#define U2Xoff    			(State.RO |= BitU2XonXoff)
#define U2Xon       	  	(State.RO &= ~BitU2XonXoff)
#define I2CBusy         	(State.RO & BitI2CBusy)             // NOT USED
#define I2CBusyOn       	(State.RO |= BitI2CBusy)            // NOT USED
#define I2CBusyOff      	(State.RO &= ~BitI2CBusy)           // NOT USED
#define TFTBacklight	    (State.RO & BitTFTBacklight)
#define TFTBacklightOn	    (State.RO |= BitTFTBacklight)
#define TFTBacklightOff	 	(State.RO &= ~BitTFTBacklight)
#define VncTr				(State.RO & BitVncTr)               // NOT USED
#define VncTrOn				(State.RO |= BitVncTr)              // NOT USED
#define VncTrOff	  		(State.RO &= ~BitVncTr)             // NOT USED
//#define PrgJbus         	(State.RO & BitPrgJbus)             // NOT USED
//#define PrgJbusOn       	(State.RO |= BitPrgJbus)            // NOT USED
#define Restart             (State.RO & BitRestart)
#define RestartOn           (State.RO |= BitRestart)
#define RestartOff          (State.RO &= ~BitRestart)
#define RTCInit			  	(State.RO & BitRTCInit)             // NOT USED
#define RTCInitOn		  	(State.RO |= BitRTCInit)
#define RTCInitOff		 	(State.RO &= ~BitRTCInit)
#define StateProfile		(State.RO & BitProfile)
#define StateProfileOn		(State.RO |= BitProfile)
#define StateProfileOff		(State.RO &= ~BitProfile)
#define TimeSummer      	(State.RO & BitTimeSummer)          // NOT USED
#define TimeSummerOn    	(State.RO |= BitTimeSummer)         // NOT USED
#define TimeSummerOff   	(State.RO &= ~BitTimeSummer)        // NOT USED
#define UsbE1Memo      		(State.RO & BitUsbE1Memo )          // MODIF R3.3
#define UsbE1MemoOn    		(State.RO |= BitUsbE1Memo )         // MODIF R3.3
#define UsbE1MemoOff   		(State.RO &= ~BitUsbE1Memo )        // MODIF R3.3
#define ChargeDisconnect    (State.RO & BitChargeDisconnect)
#define ChargeDisconnectOn  (State.RO |= BitChargeDisconnect)
#define ChargeDisconnectOff (State.RO &= ~BitChargeDisconnect)

/* ********** State.Mask (uint) ********** */
#define BitMask1PH          0x08000000              // MODIF 2.8
#define BitMaskXFC          0x04000000              // MODIF 2.8 : change bit value  - 1 = Show only XFC
#define BitMaskNOXFC        0x02000000              // 1 = Show Menu 400/480VAC - MODIF R2.1
#define BitMask480VAC       0x01000000              // 1 = Show Menu 400/480VAC - MODIF R2.1
#define BitMaskPasswd3      0x00400000              // 1 = Password 3
#define BitMaskPasswd2      0x00200000              // 1 = Password 2
#define BitMaskJbus         0x00100000              // 1 = JBus network
#define BitMaskAutoExit     0x00040000  			// 1 = auto exit
#define BitMaskInit         0x00020000  			// 1 = init
#define BitMaskChg          0x00010000  			// 1 = charge
#define BitMaskPasswdTmp    0x00004000  			// 1 = password temp correct entered
#define BitMaskPasswdHigh   0x00002000  			// 1 = password high correct entered
#define BitMaskPasswdLow    0x00001000  			// 1 = password low correct entered
#define BitMaskArea0        0x00000800  			// 1 = region 0 selected
#define BitMaskArea1        0x00000400  			// 1 = region 1 selected
#define BitMaskBP           0x00000200  			// 1 = Backplane
#define BitMaskLan          0x00000100  			// 1 = Ethernet RJ45
#define BitMaskWLan         0x00000080  			// 1 = Ethernet WIFI no encryption
#define BitMaskWAsc         0x00000040  			// 1 = Ethernet WIFI Wep encryption
#define BitMaskWHex         0x00000020  			// 1 = Ethernet WIFI Wpa encryption
#define BitMaskUsb          0x00000010  			// 1 = Usb allowed

#define MaskNOXFC           (State.Mask & BitMaskNOXFC)
#define MaskNOXFCOn         (State.Mask |= BitMaskNOXFC)
#define MaskNOXFCOff        (State.Mask &= ~BitMaskNOXFC)
#define Mask480VAC          (State.Mask & BitMask480VAC)      // MODIF R2.1
#define Mask480VACOn        (State.Mask |= BitMask480VAC)     // MODIF R2.1
#define Mask480VACOff       (State.Mask &= ~BitMask480VAC)    // MODIF R2.1
#define MaskXFC             (State.Mask & BitMaskXFC)
#define MaskXFCOn           (State.Mask |= BitMaskXFC)
#define MaskXFCOff          (State.Mask &= ~BitMaskXFC)
#define MaskJbusOn          (State.Mask |= BitMaskJbus)
#define MaskJbusOff         (State.Mask &= ~BitMaskJbus)
#define MaskInitOn          (State.Mask |= BitMaskInit)
#define MaskInitOff         (State.Mask &= ~BitMaskInit)
#define MaskChgOn           (State.Mask |= BitMaskChg)
#define MaskChgOff          (State.Mask &= ~BitMaskChg)
#define MaskPasswd2         (State.Mask & BitMaskPasswd2)
#define MaskPasswd2On       (State.Mask |= BitMaskPasswd2)
#define MaskPasswd2Off      (State.Mask &= ~BitMaskPasswd2)
#define MaskPasswd3         (State.Mask & BitMaskPasswd3)
#define MaskPasswd3On       (State.Mask |= BitMaskPasswd3)
#define MaskPasswd3Off      (State.Mask &= ~BitMaskPasswd3)
#define MaskPasswdTmpOn     (State.Mask |= BitMaskPasswdTmp)
#define MaskPasswdTmpOff    (State.Mask &= ~BitMaskPasswdTmp)
#define MaskPasswdLow       (State.Mask & BitMaskPasswdLow)
#define MaskPasswdLowOn     (State.Mask |= BitMaskPasswdLow)
#define MaskPasswdLowOff    (State.Mask &= ~BitMaskPasswdLow)
#define MaskPasswdHigh      (State.Mask & BitMaskPasswdHigh)
#define MaskPasswdHighOn    (State.Mask |= BitMaskPasswdHigh)
#define MaskPasswdHighOff   (State.Mask &= ~BitMaskPasswdHigh)
#define MaskArea0           (State.Mask & BitMaskArea0)
#define MaskArea0On         (State.Mask |= BitMaskArea0)
#define MaskArea0Off        (State.Mask &= ~BitMaskArea0)
#define MaskArea1           (State.Mask & BitMaskArea1)
#define MaskArea1On         (State.Mask |= BitMaskArea1)
#define MaskArea1Off        (State.Mask &= ~BitMaskArea1)
#define MaskBP              (State.Mask & BitMaskBP)
#define MaskBPOn            (State.Mask |= BitMaskBP)
#define MaskBPOff           (State.Mask &= ~BitMaskBP)
#define MaskLan             (State.Mask & BitMaskLan)
#define MaskLanOn           (State.Mask |= BitMaskLan)
#define MaskLanOff          (State.Mask &= ~BitMaskLan)
#define MaskWLan            (State.Mask & BitMaskWLan)
#define MaskWLanOn          (State.Mask |= BitMaskWLan)
#define MaskWLanOff         (State.Mask &= ~BitMaskWLan)
#define MaskWAscOn          (State.Mask |= BitMaskWAsc)
#define MaskWAscOff         (State.Mask &= ~BitMaskWAsc)
#define MaskWHexOn          (State.Mask |= BitMaskWHex)
#define MaskWHexOff         (State.Mask &= ~BitMaskWHex)
#define MaskUsbOn           (State.Mask |= BitMaskUsb)
#define MaskUsbOff          (State.Mask &= ~BitMaskUsb)
// MODIF 2.8
#define Mask1PH             (State.Mask & BitMask1PH)
#define Mask1PHOn           (State.Mask |= BitMask1PH)
#define Mask1PHOff          (State.Mask &= ~BitMask1PH)

/* ********** State.Def (ulong) : internal default ********** */
#define BitDefNetwk	    	(ulong)0x00000001  		// Serial network fault
#define BitDefDailyCharge   (ulong)0x00000002  		// New 6/2/10 for OPP charging
#define BitBlockOut 	    (ulong)0x00000004  		// 1B-List #C jmf 05/17/2012
#define BitDefBattery		(ulong)0x00000008  		// 1 = indicates a connected battery
#define BitDefConverter		(ulong)0x00000010  		// 1 = indicates current fault
#define BitDefNetwork		(ulong)0x00000020  		// 1 = indicates current fault
#define BitDefArcless	  	(ulong)0x00000040  		// 1 = indicates save pump fault
#define BitDefOtherCur		(ulong)0x00000080  		// 1 = indicates current fault
#define BitDefFuse			(ulong)0x00000100  		// 1 = indicates VFuse too low
#define BitDefTempLow		(ulong)0x00000200  		// 1 = indicates low temperature fault
#define BitDefTempStart		(ulong)0x00000400  		// 1 = indicates TempsStart < temperature
#define BitDefTempHigh		(ulong)0x00000800  		// 1 = indicates high temperature fault
#define BitDefPump			(ulong)0x00001000  		// 1 = indicates pump fault
#define BitDefBadBat		(ulong)0x00002000  		// 1 = indicates unappropried battery
#define BitDefVBatMax		(ulong)0x00004000  		// 1 = indicates battery voltage max
#define BitDefBatHighTp 	(ulong)0x00008000  		// 1 = indicates battery high temperature
#define BitDefConfig		(ulong)0x00010000  		// 1 = indicates Config (I2C) fault
#define BitDefMenu			(ulong)0x00020000  		// 1 = indicates Menu (I2C) fault
#define BitDefRtc		  	(ulong)0x00040000  		// 1 = indicates RTC fault
#define BitDefEep		  	(ulong)0x00080000  		// 1 = indicates I2C EEPROM fault
#define BitDefRtcVal		(ulong)0x00100000  		// 1 = indicates I2C RTC value fault
#define BitDefPCA8574 		(ulong)0x00200000  		// 1 = indicates I2C PCA8584 fault
#define BitDefAD7997 	  	(ulong)0x00400000  		// 1 = indicates I2C AD7997 fault
#define BitDefCon     		(ulong)0x01000000  		// 1 = Connector fault
#define BitDefTemp1     	(ulong)0x02000000  		// 1 = indicates I2C RTC value fault
#define BitDefTemp2     	(ulong)0x04000000  		// 1 = indicates I2C RTC value fault
#define BitDefTemp3     	(ulong)0x08000000  		// 1 = indicates I2C RTC value fault
#define BitDefBatLowLevel   (ulong)0x10000000
#define BitDefBatCon        (ulong)0x20000000     // 1 = indicates battery connected fault (left/right)
#define BitDefMod           (ulong)0x40000000     // 1 = indicates critical module fault
#define BitDefID            (ulong)0x80000000     // 1 = ID module fault

#define NetwkDef		    (State.Def & BitDefNetwk)       // NOT USED
#define NetwkDefOn	    	(State.Def |= BitDefNetwk)      // NOT USED
#define NetwkDefOff	    	(State.Def &= ~BitDefNetwk)     // NOT USED
#define DailyChargeDef      (State.Def & BitDefDailyCharge)	// New 6/2/10 for OPP charging
#define DailyChargeDefOn	(State.Def |= BitDefDailyCharge)	// New 6/2/10 for OPP charging
#define DailyChargeDefOff	(State.Def &= ~BitDefDailyCharge)	// New 6/2/10 for OPP charging
#define BlockOutDef         (State.Def & BitBlockOut)
#define BlockOutDefOn	    (State.Def |= BitBlockOut)
#define BlockOutDefOff	    (State.Def &= ~BitBlockOut)
#define Battery			  	(State.Def & BitDefBattery)
#define BatteryOn			(State.Def |= BitDefBattery)
#define BatteryOff			(State.Def &= ~BitDefBattery)
#define ConverterDef		(State.Def & BitDefConverter)
#define ConverterDefOn		(State.Def |= BitDefConverter)
#define ConverterDefOff		(State.Def &= ~BitDefConverter)
#define NetworkDef			(State.Def & BitDefNetwork)     // NOT USED
#define NetworkDefOn		(State.Def |= BitDefNetwork)    // NOT USED
#define NetworkDefOff		(State.Def &= ~BitDefNetwork)   // NOT USED
#define ArclessDef			(State.Def & BitDefArcless)     // NOT USED
#define ArclessDefOn		(State.Def |= BitDefArcless)    // NOT USED
#define ArclessDefOff		(State.Def &= ~BitDefArcless)   // NOT USED
#define OtherCurDef			(State.Def & BitDefOtherCur)    // NOT USED
#define OtherCurDefOn       (State.Def |= BitDefOtherCur)   // NOT USED
#define OtherCurDefOff		(State.Def &= ~BitDefOtherCur)  // NOT USED
#define FuseDef			  	(State.Def & BitDefFuse)
#define FuseDefOn		  	(State.Def |= BitDefFuse)
#define FuseDefOff			(State.Def &= ~BitDefFuse)
#define TempLowDef			(State.Def & BitDefTempLow)     // NOT USED
#define TempLowDefOn		(State.Def |= BitDefTempLow)    // NOT USED
#define TempLowDefOff		(State.Def &= ~BitDefTempLow)   // NOT USED
#define TempStartDef		(State.Def & BitDefTempStart)   // NOT USED
#define TempStartDefOn		(State.Def |= BitDefTempStart)  // NOT USED
#define TempStartDefOff		(State.Def &= ~BitDefTempStart) // NOT USED
#define TempHighDef			(State.Def & BitDefTempHigh)
#define TempHighDefOn		(State.Def |= BitDefTempHigh)
#define TempHighDefOff		(State.Def &= ~BitDefTempHigh)
#define PumpDef			  	(State.Def & BitDefPump)
#define PumpDefOn		  	(State.Def |= BitDefPump)
#define PumpDefOff			(State.Def &= ~BitDefPump)
#define BadBatDef			(State.Def & BitDefBadBat)
#define BadBatDefOn			(State.Def |= BitDefBadBat)
#define BadBatDefOff		(State.Def &= ~BitDefBadBat)
#define VBatMaxDef			(State.Def & BitDefVBatMax)
#define VBatMaxDefOn		(State.Def |= BitDefVBatMax)
#define VBatMaxDefOff		(State.Def &= ~BitDefVBatMax)
#define BatHighTpDef		(State.Def & BitDefBatHighTp)
#define BatHighTpDefOn		(State.Def |= BitDefBatHighTp)
#define BatHighTpDefOff		(State.Def &= ~BitDefBatHighTp)
#define ConfigDef		  	(State.Def & BitDefConfig)      // NOT USED
#define ConfigDefOn			(State.Def |= BitDefConfig)     // NOT USED
#define ConfigDefOff		(State.Def &= ~BitDefConfig)    // NOT USED
#define MenuDef			  	(State.Def & BitDefMenu)        // NOT USED
#define MenuDefOn		  	(State.Def |= BitDefMenu)       // NOT USED
#define MenuDefOff			(State.Def &= ~BitDefMenu)      // NOT USED
#define RtcDef			  	(State.Def & BitDefRtc)
#define RtcDefOn		  	(State.Def |= BitDefRtc)
#define RtcDefOff		  	(State.Def &= ~BitDefRtc)
#define EepromDef			(State.Def & BitDefEep)         // NOT USED
#define EepromDefOn			(State.Def |= BitDefEep)        // NOT USED
#define EepromDefOff		(State.Def &= ~BitDefEep)       // NOT USED
#define RtcValDef       	(State.Def & BitDefRtcVal)      // NOT USED
#define RtcValDefOn     	(State.Def |= BitDefRtcVal)     // NOT USED
#define RtcValDefOff    	(State.Def &= ~BitDefRtcVal)    // NOT USED
#define PCA8574Def      	(State.Def & BitDefPCA8574)     // NOT USED
#define PCA8574DefOn    	(State.Def |= BitDefPCA8574)    // NOT USED
#define PCA8574DefOff   	(State.Def &= ~BitDefPCA8574)   // NOT USED
#define AD7997Def       	(State.Def & BitDefAD7997)      // NOT USED
#define AD7997DefOn     	(State.Def |= BitDefAD7997)     // NOT USED
#define AD7997DefOff    	(State.Def &= ~BitDefAD7997)    // NOT USED
#define DefCon        		(State.Def & BitDefCon)		// MODIF R3.1
#define DefConOn      		(State.Def |= BitDefCon)
#define DefConOff     		(State.Def &= ~BitDefCon)
#define Temp1Def        	(State.Def & BitDefTemp1)       // NOT USED
#define Temp1DefOn      	(State.Def |= BitDefTemp1)      // NOT USED
#define Temp1DefOff     	(State.Def &= ~BitDefTemp1)     // NOT USED
#define Temp2Def        	(State.Def & BitDefTemp2)       // NOT USED
#define Temp2DefOn      	(State.Def |= BitDefTemp2)      // NOT USED
#define Temp2DefOff     	(State.Def &= ~BitDefTemp2)     // NOT USED
#define Temp3Def        	(State.Def & BitDefTemp3)       // NOT USED
#define Temp3DefOn      	(State.Def |= BitDefTemp3)      // NOT USED
#define Temp3DefOff     	(State.Def &= ~BitDefTemp3)     // NOT USED
#define BatLowLevelDef  	(State.Def & BitDefBatLowLevel) // NOT USED
#define BatLowLevelDefOn    (State.Def |= BitDefBatLowLevel)
#define BatLowLevelDefOff   (State.Def &= ~BitDefBatLowLevel)
#define ModDef              (State.Def & BitDefMod)
#define ModDefOn            (State.Def |= BitDefMod)
#define ModDefOff           (State.Def &= ~BitDefMod)
#define ModDefID            (State.Def & BitDefID)          // NOT USED
#define ModDefIDOn          (State.Def |= BitDefID)         // NOT USED
#define ModDefIDOff         (State.Def &= ~BitDefID)        // NOT USED

/* ********** Memo.Default (uint) : charge default CFC ********** */
#define MaskCritDF      	0x007F  /* Mask DF1,DF2,DF3,TH */
#define MaskWarnDF      	0x1D80  /* Mask DF4,DF5,DF7,PMP */

#define BitDFconverter		0x0001  /* 1 = indicates converter fault (DF1) */
#define BitDFnetwork		0x0002  /* 1 = indicates network fault (DF1) */
#define BitDFotherCur		0x0004  /* 1 = indicates other current fault (DF1) */
#define BitDFDisDF5  	    0x0008  /* 1 = indicates DF5 */
#define BitDFfuse		  	0x0010  /* 1 = indicates Vfuse too low (DF2) */
#define BitDFbadBat			0x0020  /* 1 = indicates unappropried battery (DF3) */
#define BitDFtemp		  	0x0040  /* 1 = indicates unappropried battery (TH) */
#define BitDFpump		  	0x0080  /* 1 = indicates pump fault (DF7) */
#define BitDFdIdT		  	0x0100  /* 1 = indicates rise current (DF5) */
#define BitDFdVdT		  	0x0200  /* 1 = indicates fall voltage (no DF5!) */
#define BitDFtimeSecu		0x0400  /* 1 = indicates time security in profile (DF5) */
#define BitDFotherBat		0x0800  /* 1 = indicates other battery fault (DF5) */
#define BitDFoverdis		0x1000  /* 1 = indicates overdischarged battery (DF4) */
#define BitDFovervolt	  	0x2000  /* 1 = indicates overvoltage : Vbat > VbatMax */
#define BitDFconsMin	  	0x4000  /* 1 = indicates Iconsign min reached */
#define BitDFBatHighTp  	0x8000  /* 1 = indicates battery high temperaure */

#define DFconverter			(Memo.Default & BitDFconverter)
#define DFconverterOn		{Memo.Default |= BitDFconverter; Status.NbConverterDef++; WarningCurrentOn;}
#define DFconverterOff		(Memo.Default &= ~BitDFconverter)
#define DFnetwork		  	(Memo.Default & BitDFnetwork)
#define DFnetworkOn			{Memo.Default |= BitDFnetwork; Status.NbNetworkDef++; WarningCurrentOn;}
#define DFnetworkOff		(Memo.Default &= ~BitDFnetwork)
#define DFotherCur			(Memo.Default & BitDFotherCur)
#define DFotherCurOn		{Memo.Default |= BitDFotherCur; Status.NbOtherCurDef++; WarningCurrentOn;}
#define DFotherCurOff		(Memo.Default &= ~BitDFotherCur)
#define DFDisDF5	  	    (Memo.Default & BitDFDisDF5)
#define DFDisDF5On	  		(Memo.Default |= BitDFDisDF5)
#define DFDisDF5Off		    (Memo.Default &= ~BitDFDisDF5)
#define DFfuse			    (Memo.Default & BitDFfuse)
#define DFfuseOn		  	{Memo.Default |= BitDFfuse; Status.NbFuseDef++; WarningFuseOn;}
#define DFfuseOff		  	(Memo.Default &= ~BitDFfuse)
#define DFbadBat			(Memo.Default & BitDFbadBat)
#define DFbadBatOn			{Memo.Default |= BitDFbadBat; Status.NbBadBatDef++; WarningBadBatOn;}
#define DFbadBatOff			(Memo.Default &= ~BitDFbadBat)
#define DFtemp		  		(Memo.Default & BitDFtemp)
#define DFtempOn	  		{Memo.Default |= BitDFtemp; Status.NbTempDef++;}
#define DFtempOff	  		(Memo.Default &= ~BitDFtemp)
#define DFpump		   		(Memo.Default & BitDFpump)
#define DFpumpOn		  	{Memo.Default |= BitDFpump; Status.NbPumpDef++;}
#define DFpumpOff	  		(Memo.Default &= ~BitDFpump)
#define DFdIdT		  		(Memo.Default & BitDFdIdT)
#define DFdIdTOn	  		{Memo.Default |= BitDFdIdT; Status.NbdIdTDef++;}
#define DFdIdTOff		  	(Memo.Default &= ~BitDFdIdT)
#define DFdVdT		  		(Memo.Default & BitDFdVdT)
#define DFdVdTOn	  		{Memo.Default |= BitDFdVdT; WarningBatterydVdTOn; Status.NbdVdTDef++;}
#define DFdVdTOff		    (Memo.Default &= ~BitDFdVdT)
#define DFtimeSecu			(Memo.Default & BitDFtimeSecu)
#define DFtimeSecuOn		{Memo.Default |= BitDFtimeSecu; Status.NbTimeSecuDef++; WarningBatterySecuAhOn;}
#define DFtimeSecuOff		(Memo.Default &= ~BitDFtimeSecu)
#define DFotherBat			(Memo.Default & BitDFotherBat)
#define DFotherBatOn		{Memo.Default |= BitDFotherBat; Status.NbOtherBatDef++; WarningBatterySecuAhOn;}
#define DFotherBatOff		(Memo.Default &= ~BitDFotherBat)
#define DFoverdis			(Memo.Default & BitDFoverdis)
#define DFoverdisOn			{Memo.Default |= BitDFoverdis; Status.NbOverdisDef++; WarningOverDischargeOn;}
#define DFoverdisOff		(Memo.Default &= ~BitDFoverdis)
#define DFovervolt   	 	(Memo.Default & BitDFovervolt)
#define DFovervoltOn		(Memo.Default |= BitDFovervolt)
#define DFovervoltOff		(Memo.Default &= ~BitDFovervolt)
#define DFconsMin  	    	(Memo.Default & BitDFconsMin)
#define DFconsMinOn		  	(Memo.Default |= BitDFconsMin)
#define DFconsMinOff		(Memo.Default &= ~BitDFconsMin)
#define DFBatHighTp  	  	(Memo.Default & BitDFBatHighTp)
#define DFBatHighTpOn		{Memo.Default |= BitDFBatHighTp; WarningBatteryTempOn;}
#define DFBatHighTpOff		(Memo.Default &= ~BitDFBatHighTp)


/* ********** Memo.CFC (uint) : new charge CFC  ********** */
#define BitBatDisconnect    0x0001  /* 1 = indicates battery disconnection */

#define BatDisconnect		(Memo.CFC & BitBatDisconnect)
#define BatDisconnectOn		(Memo.CFC |= BitBatDisconnect)
#define BatDisconnectOff	(Memo.CFC &= ~BitBatDisconnect)

#define BitCanPending       0x0001         /* 1 = function pending (request sent) */
#define BitCanErr           0x0002         /* 1 = Error (slave not responding) */
#define BitLLCWarn          0x0004         /* 1 = LLC warning usable */
#define BitLLCErr           0x0008         /* 1 = LLC unusable */

#define CanPending(x)       (LLC[(x)].Status & BitCanPending)
#define CanPendingOn(x)     (LLC[(x)].Status |= BitCanPending)
#define CanPendingOff(x)    (LLC[(x)].Status &= ~BitCanPending)
#define CanErr(x)           (LLC[(x)].Status & BitCanErr)
#define CanErrOn(x)         {LLC[(x)].Status |= BitCanErr; IbatModMem[(x)] = 0;}
#define CanErrOff(x)        (LLC[(x)].Status &= ~BitCanErr)
#define LLCWarn(x)          (LLC[(x)].Status & BitLLCWarn)
#define LLCWarnOn(x)        (LLC[(x)].Status |= BitLLCWarn)
#define LLCWarnOff(x)       (LLC[(x)].Status &= ~BitLLCWarn)
#define LLCErr(x)           (LLC[(x)].Status & BitLLCErr)
#define LLCErrOn(x)         {LLC[(x)].Status |= BitLLCErr; IbatModMem[(x)] = 0;}
#define LLCErrOff(x)        (LLC[(x)].Status &= ~BitLLCErr)

/* ********** Menu.Status (uchar) ********** */
#define BitBackOnOff        (uchar)0x01       /* 1 = Backlight off */
#define BitCurrentSum       (uchar)0x02       /* 1 : current = summer time */
#define BitFilPil           (uchar)0x04       /* 1 : fil pilote active */
#define BitArcless          (uchar)0x08       /* 1 : fil pilote active */
#define BitIdleState        (uchar)0x10       /* 1 : Idle mode active */
#define BitProfileEqual     (uchar)0x20       /* 1 : profile equal active */
#define BitMains480VAC      (uchar)0x40       /* 1 : VMains = 480VAC - MODIF R2.1 */

#define BackOnOff           (Menu.Status & BitBackOnOff)
#define BackOff             (Menu.Status |= BitBackOnOff)
#define BackOn              (Menu.Status &= ~BitBackOnOff)
#define CurrentSum          (Menu.Status & BitCurrentSum)
#define CurrentSumOff       (Menu.Status |= BitCurrentSum)
#define CurrentSumOn        (Menu.Status &= ~BitCurrentSum)
#define FilPil              (Menu.Status & BitFilPil)
#define FilPilOff           (Menu.Status &= ~BitFilPil)
#define FilPilOn            (Menu.Status |= BitFilPil)
#define Arcless             (Menu.Status & BitArcless)
#define ArclessOff          (Menu.Status &= ~BitArcless)
#define ArclessOn           (Menu.Status |= BitArcless)
#define IdleState           (Menu.Status & BitIdleState)
#define IdleStateOff        (Menu.Status &= ~BitIdleState)
#define IdleStateOn         (Menu.Status |= BitIdleState)
#define ProfileEqual        (Menu.Status & BitProfileEqual)
#define ProfileEqualOff     (Menu.Status &= ~BitProfileEqual)
#define ProfileEqualOn      (Menu.Status |= BitProfileEqual)
#define Mains480VAC         (Menu.Status & BitMains480VAC)      // MODIF R2.1
#define Mains480VACOn       (Menu.Status &= ~BitMains480VAC)    // MODIF R2.1
#define Mains480VACOff      (Menu.Status |= BitMains480VAC)     // MODIF R2.1

#define RES_CONNECTEUR_PM   11      /* resistance connecteur Module en 1/10 milliohm */
                                    /* 18 contacts to +Vbat and 18 contact to -Vbat / 10milliohm per contact */
                                    /* = (0.010 / 18) * 2 */
// MODIF 2.8
#define RES_CONNECTEUR_PM_35    20    /* resistance connecteur Module 3.5KW en 1/10 milliohm */                                    
#define MIN_CUR_STOP        20        /* courant  2.0A pour arr�ter le module */

#define TYPE_1PHASE         0
#define TYPE_3PHASE         1

// MODIF 2.8
#define OUTPUT_1_CABLE      0
#define OUTPUT_2_CABLE_6M   1
#define OUTPUT_2_CABLE_8M   2

#define BLE_ONBOARD_TYPE	0xA0

// Status.WarningC (Warning charger)
#define	BitWarningCurrent		0x0001		// warning charger curent fault (DF1 / DFC)
#define	BitWarningFuse			0x0002		// warning charger output fuse (DF2)
#define	BitWarningChargergTemp	0x0004		// warning charger temperature (TH)
#define	BitWarningBadBat		0x0008		// warning charger incorrect battery (DF3)
#define	BitWarningOverDischarge	0x0010		// warning charger overdischarge (DF4)

#define	BitWarningBatteryTemp	0x0100		// warning battery temperature
#define	BitWarningBatteryLevel	0x0200		// warning battery level
#define	BitWarningBatteryBal	0x0400		// warning battery balance
#define	BitWarningBatterydVdT	0x0800		// warning battery charge with dvdt
#define	BitWarningBatterydIdT	0x1000		// warning battery charge with didt
#define	BitWarningBatterySecuTime 0x2000	// warning battery charge with scurity time
#define	BitWarningBatterySecuAh	0x4000		// warning battery charge with security Ah

#define WarningCurrent         	(Status.WarningC & BitWarningCurrent)
#define WarningCurrentOn       	(Status.WarningC |= ~BitWarningCurrent)
#define WarningCurrentOff      	(Status.WarningC &= BitWarningCurrent)
#define WarningFuse         	(Status.WarningC & BitWarningFuse)
#define WarningFuseOn       	(Status.WarningC |= ~BitWarningFuse)
#define WarningFuseOff      	(Status.WarningC &= BitWarningFuse)
#define WarningChargergTemp    	(Status.WarningC & BitWarningChargergTemp)
#define WarningChargergTempOn  	(Status.WarningC |= ~BitWarningChargergTemp)
#define WarningChargergTempOff 	(Status.WarningC &= BitWarningChargergTemp)
#define WarningBadBat    		(Status.WarningC & BitWarningBadBat)
#define WarningBadBatOn  		(Status.WarningC |= ~BitWarningBadBat)
#define WarningBadBatOff 		(Status.WarningC &= BitWarningBadBat)
#define WarningOverDischarge	(Status.WarningC & BitWarningOverDischarge)
#define WarningOverDischargeOn 	(Status.WarningC |= ~BitWarningOverDischarge)
#define WarningOverDischargeOff	(Status.WarningC &= BitWarningOverDischarge)

#define WarningBatteryTemp		(Status.WarningC & BitWarningBatteryTemp)
#define WarningBatteryTempOff 	(Status.WarningC &= ~BitWarningBatteryTemp)
#define WarningBatteryTempOn	(Status.WarningC |= BitWarningBatteryTemp)
#define WarningBatteryLevel		(Status.WarningC & BitWarningBatteryLevel)
#define WarningBatteryLevelOff 	(Status.WarningC &= ~BitWarningBatteryLevel)
#define WarningBatteryLevelOn	(Status.WarningC |= BitWarningBatteryLevel)
#define WarningBatteryBal		(Status.WarningC & BitWarningBatteryBal)
#define WarningBatteryBalOff 	(Status.WarningC &= ~BitWarningBatteryBal)
#define WarningBatteryBalOn		(Status.WarningC |= BitWarningBatteryBal)
#define WarningBatterydVdT		(Status.WarningC & BitWarningBatterydVdT)
#define WarningBatterydVdTOff 	(Status.WarningC &= ~BitWarningBatterydVdT)
#define WarningBatterydVdTOn	(Status.WarningC |= BitWarningBatterydVdT)
#define WarningBatterydIdT		(Status.WarningC & BitWarningBatterydIdT)
#define WarningBatterydIdTOff 	(Status.WarningC &= ~BitWarningBatterydIdT)
#define WarningBatterydIdTOn	(Status.WarningC |= BitWarningBatterydIdT)
#define WarningBatterySecuTime		(Status.WarningC & BitWarningBatterySecuTime)
#define WarningBatterySecuTimeOff 	(Status.WarningC &= ~BitWarningBatterySecuTime)
#define WarningBatterySecuTimeOn	(Status.WarningC |= BitWarningBatterySecuTime)
#define WarningBatterySecuAh		(Status.WarningC & BitWarningBatterySecuAh)
#define WarningBatterySecuAhOff 	(Status.WarningC &= ~BitWarningBatterySecuAh)
#define WarningBatterySecuAhOn		(Status.WarningC |= BitWarningBatterySecuAh)


#endif /* CHARGE_H_ */
