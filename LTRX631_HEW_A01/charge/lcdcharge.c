/*
 * lcdcharge.c
 *
 *  Created on: 22 févr. 2013
 *      Author: duhautf
 */

#include "include.h"


extern char   version[8];
extern uchar  FlagThAmbiant;

const uchar ListWord16_State[25][NBLANG][17] = {
 		{"","","","",},	//0
 		{"LOADING","LOADING","BOOTEN","LOADING","LOADING",},	//1
 		{"ATTENTE","WAIT","BEREIT"," ATTESA","WAIT",},	//2
 		{"CHARGE","CHARGE","LADUNG","CARICA","CARGA",},	//3
 		{"DISPO","AVAIL","GELADEN","DISPO","DISPO",},	//4
 		{"COMP.","COMP.","COMP.","COMP.","COMP.",},	//5
 		{"EGAL.","EQUAL.","AUSGLH","EQUAL.","IGUAL.",},	//6
 		{"USBCONNECTE","USBATTACH","USBATTACH","USBATTACH","USBATTACH",},	//7
 		{"ERREURBUSCAN","CANBUSERROR","CANBUSERROR","CANBUSERROR","CANBUSERROR",},	//8
 		{"POWERMODULEOFF","POWERMODULEOFF","POWERMODULEOFF","POWERMODULEOFF","POWERMODULEOFF",},	//9
 		{"MANUMODE","MANUMODE","MANUMODE","MANUMODE","MANUMODE",},	//10
 		{"PasdeMEMO","NoMEMO","NoMEMO","NoMEMO","NoMEMO",},	//11
 		{"","","","","",},	//12
 		{"DF1","DF1","DF1","DF1","DF1",},	//13
 		{"DF2","DF2","DF2","DF2","DF2",},	//14
 		{"DF3","DF3","DF3","DF3","DF3",},	//15
 		{"TH","TH","TH","TH","TH",},	//16
 		{"DEFEEP","DEFEEP","DEFEEP","DEFEEP","DEFEEP",},	//17
 		{"DEFRTC","DEFRTC","DEFRTC","DEFRTC","DEFRTC",},	//18
 		{"DEFDISPLAY","DEFDISPLAY","DEFDISPLAY","DEFDISPLAY","DEFDISPLAY",},	//19
 		{"IQ SCAN","IQ SCAN","IQ SCAN","IQ SCAN","IQ SCAN",},	//20
 		{"IQ LINK","IQ LINK","IQ LINK","IQ LINK","IQ LINK",},	//21
 		{"NIVEAU EAU","WATER LEVEL","ELYT.STAND","WATER LEVEL","WATER LEVEL",},	//22
 		{"BAT TEMP","BAT TEMP","BAT TEMP","BAT TEMP","BAT TEMP",},	//23
 		{"CHARGE BLOQUEE","BLOCK OUT CHARGE","LADEABBRUCH","BLOCK OUT CHARGE","BLOCK OUT CHARGE",},	//24
};


const uchar ListWord6_Profile[24][NBLANG][7] = {
		{"","","","",},//0
#ifdef  ENERSYS_EU 
		{"HDUTY","HDUTY","HDUTY","HDUTY","HDUTY",},         // 1
#else
		{"IONIC","IONIC","IONIC","IONIC","IONIC",},
#endif
#ifdef  ENERSYS_EU 
		{"GEL","GEL","GEL","GEL","GEL",},
#endif
#ifdef  ENERSYS_US 
		{"GELBLC","GELBLC","GELBLC","GELBLC","GELBLC",},
#endif
#ifdef  HAWKER_US 
		{"EVLTN","EVLTN","EVLTN","EVLTN","EVLTN",},
#endif
		{"FROID","COLD","COLD","COLD","COLD",},             // 3
		{"WL20","WL20","WL20","WL20","WL20",},              // 4
		{"PNEU","PNEU","PNEU","PNEU","PNEU",},              // 5
		{"EGAL","EQUAL","AUSGL","EQUAL","IGUAL",},          // 6
		{"OPP","OPP","OPP","OPP","OPP",},                   // 7
#ifndef HAWKER_US     
		{"VRLA","VRLA","VRLA","VRLA","VRLA",},              // 8
#else
		{"ENVLNK","ENVLNK","ENVLNK","ENVLNK","ENVLNK",},    // 8
#endif    
		{"RGT","AGM","AGM","AGM","AGM",},                   // 9
		{"","","","","",},                                  // 10
//#ifndef HAWKER_US        
#ifdef ENERSYS_EU
		{"OPPIUI", "OPPIUI", "OPPIUI", "OPPIUI","OPPIUI",},  // 11
#else
        {"CMP CH", "CMP CH", "CMP CH", "CMP CH","CMP CH",},  // 11
#endif
		{"PREM","PREM","PREM","PREM","PREM",},              // 12
		{"TEST","TEST","TEST","TEST","TEST",},              // 13
		{"","","","","",},                                  // 14
		{"","","","","",},                                  // 15
#ifdef  ENERSYS_US      
		{"NexSys","NexSys","NexSys","NexSys","NexSys",},    // 16
#endif
#ifdef  HAWKER_US
		{"FXSTND","FXSTND","FXSTND","FXSTND","FXSTND",},
#endif
#ifdef  ENERSYS_EU
		{"NXSTND","NXSTND","NXSTND","NXSTND","NXSTND",},
#endif    
		{"","","","","",},                             		// 17
		{"","","","","",},                             		// 18
		{"STDWL","STDWL","STDWL","STDWL","STDWL",},         // 19
#ifdef  HAWKER_US
		{"FXFAST","FXFAST","FXFAST","FXFAST","FXFAST",},     // 20
#else
		{"NXFAST","NXFAST","NXFAST","NXFAST","NXFAST",},     // 20
#endif
        {"LOWCHG","LOWCHG","LOWCHG","LOWCHG","LOWCHG",},     // 21
#ifdef  HAWKER_US
		{"FXBLOC","FXBLOC","FXBLOC","FXBLOC","FXBLOC",},     // 22
#else
		{"NXBLOC","NXBLOC","NXBLOC","NXBLOC","NXBLOC",},     // 22
#endif
		{"LITH  ","LITH  ","LITH  ","LITH  ","LITH  ",},     // 23
};

const uchar ListWord4_State[3][NBLANG][5] = {
		{"DF4 ", "DF4 ", "DF4 ", "DF4 ","DF4 ",},          	// 0
		{"DF7 ", "DF7 ", "DF7 ", "DF7 ","DF7 ",},          	// 1
};



extern uchar 	MnuLevel;               	        /* Level of current display (0 : no menu) */
extern ulong  TimerDisplay;
extern ulong  TimerIdent;


/**
 * @brief  Manage LED state in chrono
 * @param  None
 * @retval None
 */
void LCD_Chrono (void)
{
	/* Detect changes only in charger state */

	static uint ChargerCopy = 0xFFFF;		/*To be sure to update at startup*/
	static uint PhaseCopy = 0xFFFF;
	static uint ChargeCopy = 0xFFFF;
	static uint MnuLevelCopy = 0x0000;		/*It should start with no meny entry*/


	if (RefreshLCD != 0)
	{
		RefreshLCDOff;
		LCD_BackLightManagement();

		/* Actions done every second */
		DataLcd.Ibat = DataR.Ibat;
		DataLcd.Vbat = DataR.Vbat;
		DataLcd.VbatCell = DataR.VbatCell;
		DataLcd.Vcharger = DataR.Vcharger;
		DataLcd.Vfuse = DataR.Vfuse;
		DataLcd.TempC = DataR.TempC;
		/* A voir avec IQ */
		if (IQWIIQLink != 0)
			DataLcd.BatTemp = IQData.BatTemp;
		else
			DataLcd.BatTemp = Menu.BatTemp;
		DataLcd.Power = DataR.Power;
		DataLcd.StateRW = State.RW;
		DataLcd.StateRO = State.RO;
		DataLcd.StateDef = State.Def;
		DataLcd.StateIQ = IQData.StateIQ;
		DataLcd.StateCharger = State.Charger;
		DataLcd.StateCharge = State.Charge;
		DataLcd.StatePhase = State.Phase;
		DataLcd.PwmTon = DataR.PwmTon;
		DataLcd.PwmToff = DataR.PwmToff;
		DataLcd.Profile = ChgData.Profile;
		DataLcd.BodyType = 4; // Using by LIFE-NETWORK and MONITOR

		// DATA BLE update
		DataBle.DeviceType = BLE_ONBOARD_TYPE;
		DataBle.StateCharger = DataLcd.StateCharger;
		DataBle.StateCharge = DataLcd.StateCharge;
		DataBle.StatePhase = DataLcd.StatePhase;
		DataBle.Ibat = DataLcd.Ibat;
		DataBle.Vbat = DataLcd.Vbat;
		DataBle.VbatCell = DataLcd.VbatCell;
		DataBle.ProfileTime = DataLcd.ProfileTime;
		DataBle.ProfileAh = DataLcd.ProfileAh;
		DataBle.ChgRestTime = DataLcd.ChgRestTime;
		DataBle.ChgSOC = DataLcd.ChgSOC;
		DataBle.TempC = DataLcd.TempC;
		DataBle.TempB = DataLcd.BatTemp;
		DataBle.MemoPointer = Status.MemoPointer;
		DataBle.NbRstPointer = Status.NbRstPointer;
		DataBle.WarningC = Status.WarningC;

		if ((((ChargerCopy != State.Charger) || (PhaseCopy != (State.Phase & 0xF0)) || (ChargeCopy != State.Charge)) && (MnuLevel == 0))
		/*&& (ChargeDisconnect == 0)*/)
		{
			/* If state changed and we are not in the menu */

			switch (State.Charger)
			{
			case StateChgerInit:
				//Loading screen
				screenstate = CREATE_LOADING;
				break;

			case StateChgerIdle:
			case StateChgerManu:
				//Idle screen
				screenstate = CREATE_IDLE;
				break;

			case StateChgerStChg:
				//WaitScreen
				screenstate = CREATE_STARTCHARGE;
				break;

			case StateChgerChg:

				switch (State.Phase & 0xF0)
				{
				case  StatePhDefCurWait:
				case  StatePhDefCurSet:
					// FaultPage(DFC)
					FaultType = FAULT_DFC;
					sprintf(&FaultMsg[0],"DF-CUR");
					screenstate = CREATE_FAULT;
					break;

				case  StatePhDefCur:
					// FaultPage(DF1)
					FaultType = FAULT_DF;
					if (DefCon != 0)	// MODIF R3.1
						sprintf(&FaultMsg[0],"TH-LOCK");
					else
						sprintf(&FaultMsg[0],"DF1");	
					screenstate = CREATE_FAULT;
					break;
                    
                case  StatePhDefTechno :
                    // MODIF 2.8
                    FaultType = FAULT_DF;
					sprintf(&FaultMsg[0],"DF-TECHNO");
					screenstate = CREATE_FAULT;
					break;

                case  StatePhDefVreg :
                	// MODIF 3.3
                    FaultType = FAULT_DF;
                	sprintf(&FaultMsg[0],"DF-VREG");
                	screenstate = CREATE_FAULT;
                	break;

				case  StatePhDefFuse:
					// FaultPage(DF2)
					FaultType = FAULT_DF;
					sprintf(&FaultMsg[0],"DF2");
					screenstate = CREATE_FAULT;
					break;

				case  StatePhDefBadBat:
					// FaultPage(DF3)
					FaultType = FAULT_DF;
					sprintf(&FaultMsg[0],"DF3");
					screenstate = CREATE_FAULT;
					break;

				case  StatePhDefTempWait:
				case  StatePhDefTempSet:
				case  StatePhDefTemp:
					// FaultPage(TH)
					FaultType = FAULT_DF;
					if (FlagThAmbiant == OFF)
						sprintf(&FaultMsg[0],"TH");
					else
						sprintf(&FaultMsg[0],"TH-Amb");
					screenstate = CREATE_FAULT;
					break;

				case StatePhDefBatLowLevel :
					// FaultPage(LOW LEVEL)
					FaultType = FAULT_WATER;
					sprintf((char *)&FaultMsg[0],(char *)&ListWord16_State[22][LANG][0]);
					screenstate = CREATE_FAULT;
					break;

				case StatePhDefBatHighTp :
					// FaultPage(HIGH TP BAT)
					FaultType = FAULT_TEMP;
					sprintf((char *)&FaultMsg[0],(char *)&ListWord16_State[23][LANG][0]);
					screenstate = CREATE_FAULT;
					break;

				case StatePhDefID :
					// FaultPage(DEF ID)
					FaultType = FAULT_ID;
					sprintf((char *)&FaultMsg[0],"DF-ID");
					screenstate = CREATE_FAULT;
					break;

				case StatePhDefPowerCut:
					FaultType = FAULT_POWERCUT;
					sprintf(&FaultMsg[0],"POWERCUT");
					screenstate = CREATE_FAULT;
					break;

				default :
					switch  (State.Charge)
					{
					case  StateChgStartPhase1:
					case  StateChgIQEnd:
						Display.ForceReloadHeader = 1;
						break;
					case  StateChgIQScan:
					case  StateChgIQLink:
					//case  StateChgStartPhase1:
					case  StateChgPhase1:
					case  StateChgIncVregPhase1:
					case  StateChgStartPhase2:
					case  StateChgPhase2:
					case  StateChgStartPhase3:
					case  StateChgPhase3:
					case  StateChgEndLoop:

						if (ChgData.Profile == EQUAL)
							// EqualPage() -> Manual EQUAL
							screenstate = CREATE_EQUAL;
						else
							// ChargePage()
							if (screenstate != DISPLAY_MAINCHARGE)
							{
								screenstate = CREATE_MAINCHARGE;
							}
						break;

					case  StateChgStAvail:
					case  StateChgAvail:
						// AvailPage()
						screenstate = CREATE_AVAILABLE;
						break;

					case  StateChgComp:
					case  StateChgEqual:
						// Compensation/Auto Equal Page
						screenstate = CREATE_EQUAL;
						break;

					default:
						break;

					}
					break;
				}
				break;
			}

			ChargerCopy = State.Charger;
			PhaseCopy = (State.Phase & 0xF0);
			ChargeCopy = State.Charge;
		}

		//if (ManuReg == 0)     // No menu menu access during manu mode
		LCDMenu();

		if ((MnuLevelCopy > 0) && (MnuLevel == 0))
		{
			/* Quit event from Menu */
			PhaseCopy = 0xFFFF; /* To create refresh event */
		}
		else if ((MnuLevelCopy == 0) && (MnuLevel > 0))
		{
			/* Entry event to Menu */
			screenstate = DISPLAY_MENU;
		}

		MnuLevelCopy = MnuLevel;
	}
}

/**
 * @brief  Manage backlight state in chrono
 * @param  None
 * @retval None
 */
void LCD_BackLightManagement (void)
{
	if ((DataW.IdentCharger != 0) && (TimerIdent == 0))
	{
		TimerIdent = State.TimerSec + ((DataW.IdentCharger * 2L)/10);
	}

	if ((TimerIdent > State.TimerSec) && (DataW.IdentCharger != 0))
	{
		if ((State.TimerMs % 500) < 250)
			SetBacklight(0);
		else
		{
			SetBacklight(1);
		}
	}
	else
	{
		if (TimerIdent != 0)
		{
			LCD_RefreshScreenSaver();
			TFTBacklightOn;
			SetBacklight(1);
			TimerIdent = 0;
			DataW.IdentCharger = 0;
		}
		LCD_ScreenSaver();
	}
}

/**
 * @brief  Manage LED update
 * @param  None
 * @retval None
 */
void LCD_LedRefresh (void)
{
	static uchar StepInit = 0;
	
	if (BleLedBlink != 0)
	{
		if (State.TimerMs % 100 == 0)
			{
				switch (StepInit)
				{
					case 0 :
						StateYellowOn;
	                	StateRedOn;
	                	StateGreenOn;
						StepInit++;
					break;
				
					case 1 :
					 	StateYellowOff;
	                	StateRedOff;
	                	StateGreenOff;
						StepInit = 0;
					break;
					
					default :
						StepInit = 0;
					break;
				}
			}
			if (StateGreen != 0)
				LED_AVAIL = 1;
			else
				LED_AVAIL= 0;
			if (StateYellow != 0)
				LED_CHARGE = 1;
			else
				LED_CHARGE= 0;
			if (StateRed != 0)
				LED_FAULT = 1;
			else
				LED_FAULT= 0;		
			return;		
	}
	else if (ManuOption == 0)
	{
		switch (State.Charger)
		{
		case StateChgerInit:
			LCD_RefreshScreenSaver();
			if (State.TimerSec % 3 == 0)
			{
				StateYellowOff;
				StateRedOff;
				StateGreenOn;
				//BuzzerOn;
			}
			else if (State.TimerSec % 3 == 1)
			{
				StateYellowOn;
				StateRedOff;
				StateGreenOff;
				//BuzzerOff;
			}
			else
			{
				StateYellowOff;
				StateRedOn;
				StateGreenOff;
				//BuzzerOn;
			}
			break;

		case StateChgerIdle:
			if ((((CANState == 0) || (CANTimeout != 0)) && (IdleState == OFF))
					|| (((CANState == 0) || (CANTimeout != 0)) && (StateIdle == OFF) && (Battery != 0))  )
			{
				StateYellowOff;
				StateGreenOff;
#ifndef DEMO
				LCD_LedFaultBlink(100, 0);
				OUT_FAULT = 0;
#else
				StateRedOff;
				OUT_FAULT = 1;
#endif
			}
			else
			{
				StateYellowOff;
				StateRedOff;
				StateGreenOff;
				//BuzzerOff;
				OUT_FAULT = 1;
			}
			break;

		case StateChgerStChg:
			StateYellowOff;
			StateRedOff;
			StateGreenOff;
			//BuzzerOff;
			OUT_FAULT = 1;
			LCD_RefreshScreenSaver();
			break;

		case StateChgerChg:
			switch (State.Phase & 0xF0)
			{
            case StatePhDefTechno:  // MODIF 2.8
            case StatePhDefVreg:	// MODIF 3.3
			case StatePhDefCur:
			case StatePhDefFuse:
			case StatePhDefBadBat:
			case StatePhDefTemp:
			case StatePhDefBatLowLevel:
			case StatePhDefBatHighTp:
				// Critical fault : Fault LED blinking
				StateYellowOff;
				StateGreenOff;
				LCD_LedFaultBlink(500, 1);
				OUT_FAULT = 0;
				LCD_RefreshScreenSaver();
				break;

			case  StatePhDefTempWait:
				// Thermal fault : Avail LED blinking
				StateRedOff;
				LCD_LedYellowBlink(500, 1);
				StateGreenOff;
				LCD_RefreshScreenSaver();
				break;

			case  StatePhDefID:
				StateYellowOff;
				StateGreenOff;
				LCD_LedFaultBlink(500, 1);
				OUT_FAULT = 0;
				LCD_RefreshScreenSaver();
				break;

			default :
				if (State.Charge == StateChgAvail)
				{ // Battery Rest : Avail LED blinking
					if (DataR.TimeBatteryRest > ChgData.TimerSecCharge)
					{
						StateRedOff;
						StateYellowOff;
						LCD_LedGreenBlink(1000, 1);
					}
					else
					{
						StateYellowOff;
						// MODIF 2.8 turn On Green after LedFaultBlink. Otherwise Green blink....
                    	// MODIF A3.3
						/*if ((ModDef != 0) && (ChargeDisconnect == 0))
							LCD_LedFaultBlink(500, 1);
						else*/
							StateRedOff;
						StateGreenOn;
					}
                    
					OUT_FAULT = 1;
				}
				else
				{
                    // MODIF 2.8
					//StateYellowOn;
					// MODIF A3.3
					/*if ((ModDef != 0) && (ChargeDisconnect == 0))
						LCD_LedFaultBlink(500, 1);
					else*/
						StateRedOff;
                    // MODIF 2.8 : turn On Charge after LedFaultBlink. Otherwise charge blink....
					StateYellowOn;    
					StateGreenOff;
					OUT_FAULT = 1;
					LCD_RefreshScreenSaver();
				}
				break;

			}
			break;
		}
	}
	else
	{ // No screen saver in MANU mode
		LCD_RefreshScreenSaver();
	}
	

	if (StateGreen != 0)
	{
		OUT_AVAIL = 0;
		LED_AVAIL = 1;
	}
	else
	{
		if ((State.Charge == StateChgAvail) && (DataR.TimeBatteryRest > ChgData.TimerSecCharge))
			OUT_AVAIL = 0; // Battery Rest : Avail is set
		else
			OUT_AVAIL = 1;
		LED_AVAIL= 0;
	}

	if (StateYellow != 0)
	{
		OUT_CHARGE = 0;
		LED_CHARGE = 1;
	}
	else
	{
		if ((State.Charger == StateChgerChg) && ((State.Phase & 0xF0) == StatePhDefTempWait))
			OUT_CHARGE = 0; // Charge is ON on TH no critical fault
		else
			OUT_CHARGE = 1;
		LED_CHARGE = 0;
	}

	if (StateRed != 0)
		LED_FAULT= 1;
	else
		LED_FAULT= 0;

}


/**
 * @brief  Blink LED Fault with buzzer
 * @param  None
 * @retval None
 */
void LCD_LedFaultBlink (ulong bltime, uchar buzstate)
{
	if (State.TimerMs % bltime == 0)
	{
		StateYellowOff;
		StateGreenOff;

		if (StateRed != 0)
			StateRedOff;
		else
			StateRedOn;
	}
}


/**
 * @brief  Blink LED Avail
 * @param  None
 * @retval None
 */
void LCD_LedGreenBlink (ulong bltime, uchar buzstate)
{
	if (State.TimerMs % bltime == 0)
	{
		if (StateGreen != 0)
			StateGreenOff;
		else
			StateGreenOn;
	}
}


/**
 * @brief  Blink LED Charge
 * @param  None
 * @retval None
 */
void LCD_LedYellowBlink (ulong bltime, uchar buzstate)
{
	if (State.TimerMs % bltime == 0)
	{
		if (StateYellow != 0)
			StateYellowOff;
		else
			StateYellowOn;
	}
}


/**
 * @brief  Turn OFF/ON LCD backlight if refresh elapsed time
 *
 * @param  None
 * @retval None
 */
void LCD_ScreenSaver (void)
{
	if (State.TimerSec < 5)
		return;

	if ((State.TimerSec > TimerDisplay) && (BackOnOff != 0) && (UsbDisk == 0))
	{
		if (TFTBacklight != 0)
		{
			TFTBacklightOff;
			SetBacklight(0);
		}
	}
	else
	{
		if (TFTBacklight == 0)
		{
			TFTBacklightOn;
			SetBacklight(1);
		}
		if (UsbDisk != 0)
			TimerDisplay = State.TimerSec + (ulong)(Menu.TimeDisplaySave * 60L);
	}
}


/**
 * @brief  Refresh Screen saver Timer
 *
 * @param  None
 * @retval None
 */
void LCD_RefreshScreenSaver (void)
{
	TimerDisplay = State.TimerSec + (ulong)(Menu.TimeDisplaySave * 60L);
}
