/*******************************************************************************
 * DISCLAIMER
 *
 *
 *
 *
 *******************************************************************************/
/* Copyright (C) 2013 Enersys. All rights reserved.   */
/*******************************************************************************
 * File Name     : screen.c
 * Version       : 1.00
 * Device        : R5F563NB (RX631)
 * Tool-Chain    : Renesas RX Standard 1.2.0.0
 * H/W Platform  : LIFETECH
 * Description   : Period Update screen functions.
 *
 *******************************************************************************/
/*******************************************************************************
 * History       : Sep. 2013  Ver. 1.00 First Release
 *******************************************************************************/

#include "include.h"

void DisplayHostMessage (void);

extern  const uchar ListWord16_2[2][NBLANG][20];
extern  char  version[8];
extern  uint  ItoReach;
extern  uint PMod;
extern  uchar ModNumber;
extern  uchar ModIndex[MODNB+1];
extern  uchar ptrMod;
extern  uint PMod;

extern uchar  ChargerID;


const uchar ListWord14_Fault[3][NBLANG][14] = {
		{"DF7", "DF7", "DF7", "DF7", "DF7",},  // 0
		{"DF4", "DF4", "DF4", "DF4", "DF4",},  // 1
		{"DF5", "DF5", "DF5", "DF5", "DF5",},  // 2
};

SCREEN_STATES screenstate;
DisplayStruct Display;
uint8_t MainScrNb; 
uint8_t FaultType;

uint  ILcd;
uint  ILcdNew;
uchar FlagModDef = 0;

// MODIF R2.1
extern uchar  LcdTemp[128];

/*******************************************************************************
 * Outline       :
 * Description   : Inicialize update variables
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void UpdateStateInit(void)
{
	uint32_t *ptr = (uint32_t *)&Display;
	*ptr = 0;
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void CreateScreenHeader (void)
{
	uchar NbCellDisplay;

	char text[16];
	uint16_t TWidth,xpos,ypos;		

	/* Create Background */
	CopyPageWindow(3,GetDrawBufferAddress(),356,0,356,0,116,36);

	/* Create Text */
	SetFont((void *) &Font_SvBold);
	SetColor(WHITE);
	SetBackColor(RGBConvert(6,11,14));

	if (ChgData.Profile != LITH)
	{
		NbCellDisplay = Menu.NbCells*2;
	}
	else
	{
		NbCellDisplay = (Menu.NbCells*36)/10;
	}

	/*TOP*/
	if (ILcd > 0)
	{
		if (Menu.CellSize != 255)
		{
			sprintf(&text[0], "%2.2uV%2.2uA %4.4s", NbCellDisplay, (ILcd/10),(char*)&version[0]);
			sprintf((char *)&ChargerConfig.ChargerName[0], "%2.2uV%2.2uA  ", NbCellDisplay, (ILcd/10));
		}
		else if (Menu.ChgType == TYPE_3PHASE)
		{
            if (LLCRamRO[ModIndex[0]].Id == ID_LLC3KW_3648VM)
            {
                sprintf(&text[0], "36-48V %4.4s", (char*)&version[0]);
                sprintf((char *)&ChargerConfig.ChargerName[0], "36-48V ");
            }
            else
            {
                sprintf(&text[0], "24-36-48V %4.4s", (char*)&version[0]);
                sprintf((char *)&ChargerConfig.ChargerName[0], "24-36-48V ");
            }    
			
		}
		else
		{
			sprintf(&text[0], "36-48V %4.4s", (char*)&version[0]);
			sprintf((char *)&ChargerConfig.ChargerName[0], "36-48V ");
		}
	}
	else
	{
		if (Menu.CellSize != 255)
		{
			sprintf(&text[0], "%2.2uV    %4.4s", NbCellDisplay,(char*)&version[0]);
			sprintf((char *)&ChargerConfig.ChargerName[0], "%2.2uV      ", NbCellDisplay);
		}
		else if (Menu.ChgType == TYPE_3PHASE)
		{
			sprintf(&text[0], "24-36-48V %4.4s", (char*)&version[0]);
			sprintf((char *)&ChargerConfig.ChargerName[0], "24-36-48V ");
		}
		else
		{
			sprintf(&text[0], "36-48V %4.4s", (char*)&version[0]);
			sprintf((char *)&ChargerConfig.ChargerName[0], "36-48V ");
		}
	}
	TWidth = GetTextWidth((char*)&text[0],(void *) &Font_SvBold);
	xpos = 418 - (TWidth/2);
	ypos = 2;
	OutTextXY(xpos,ypos,(char*)&text[0]);		  

	/*BOTTOM*/
	sprintf(&text[0], "%s", (char*)&ListWord6_Profile[ChgData.Profile][LANG][0]);
	TWidth = GetTextWidth((char*)&text[0],(void *) &Font_SvBold);
	xpos = 418 - (TWidth/2);
	ypos = 20;	
	OutTextXY(xpos,ypos,(char*)&text[0]);
}

/*******************************************************************************
 * Outline       :
 * Description   : Inicialize update variables
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void UpdateStateForceReloadIcons(void)
{
	Display.ICanShow = 0;
	Display.IUSBShow = 0;
	Display.IManuReg = 0;				
	Display.IDongleShow = 0; 
	Display.IDongleOff = 0;
	Display.ILinkShow = 0;
	Display.IWaterShow = 0;
	Display.IUnBalancedShow = 0;
	Display.IOverDisShow = 0;
	Display.IBatteryShow = 0;
	Display.ICapacityShow = 0;
	Display.IEqualShow = 0;
	Display.ItempShow = 0;
}

/*******************************************************************************
 * Outline       :
 * Description   : Update Time function
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void UpdateDateTime(void)
{
	char text[50];
	static uint16_t TWidth,xpos,ypos;

	/* Copy background */
	CopyPageWindow(3,GetDrawBufferAddress(),356,238,356,238,116,34);

	SetFont((void *) &Font_SvBold);
	SetColor(WHITE);
	SetBackColor(RGBConvert(6,11,14));

	sprintf(&text[0], "%2.2u:%2.2u:%2.2u", State.DateR.Hr, State.DateR.Min, State.DateR.Sec);
	TWidth = GetTextWidth(&text[0],(void *) &Font_SvBold);

	xpos = (437) - (TWidth/2);
	ypos = 256;	
	while(!OutTextXY(xpos,ypos,&text[0]));				

	if (Menu.Area == 0)
	{
		sprintf(&text[0], "%2.2u-%2.2u-%2.2u", State.DateR.Date, State.DateR.Month, (State.DateR.Year % 100));
	}
	else
	{
		sprintf(&text[0], "%2.2u-%2.2u-%2.2u", State.DateR.Month, State.DateR.Date, (State.DateR.Year % 100));  
	}
	TWidth = GetTextWidth(&text[0],(void *) &Font_SvBold);
	xpos = (437) - (TWidth/2);
	ypos = 238;	
	while(!OutTextXY(xpos,ypos,&text[0]));		
    
    if ((Display.BFMOnShow != 0) || (Display.BFMOffShow != 0))
    {
        sprintf(&text[0], "BFM");
        TWidth = GetTextWidth(&text[0],(void *) &Font_SvBold);
	    xpos = (377) - (TWidth/2);
	    ypos = 256;	
	    while(!OutTextXY(xpos,ypos,&text[0]));		
    }
	else if (Display.WifiOnShow != 0)
	{
		Display.WifiOnShow = 0;
		PutImage24BPPExt(370,242,&wifi_green);
	}
	else if (Display.WifiOffShow != 0)
	{
		PutImage24BPPExt(370,242,&wifi_white);
	}
	else if (Display.LanOnShow != 0)
	{
		Display.LanOnShow = 0;
		PutImage24BPPExt(370,242,&lan_green);
	}
	else if (Display.LanOffShow != 0)
	{
		PutImage24BPPExt(370,242,&lan_white);
	}
    
}

/*******************************************************************************
 * Outline       :
 * Description   : Drawing values to the display
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void IconManagement (void)
{		
	/*USB connected*/
	if (UsbDisk != 0)
	{
		if (Display.IUSBShow != 1)
		{
			PutImage24BPPExt(152,245,&USB_);
			Display.IUSBShow = 1;
		}
	}
	else
	{
		if (Display.IUSBShow == 1)
		{
			CopyPageWindow(MainScrNb,GetDrawBufferAddress(),152,245,152,245,40,24);
			Display.IUSBShow = 0;
		}
	}

	/*RF Dongle*/
	if ((IQRFTrLink  == 0) && (State.Charger < StateChgerChg))
	{
		if (Display.IDongleShow != 1)
		{
			PutImage24BPPExt(296,242,&NORF);
			Display.IDongleShow = 1;
			Display.IDongleOff = 0;
		}
	}
	else
	{
		if ((Display.IDongleShow == 1) || (Display.IDongleOff == 0))
		{
			CopyPageWindow(MainScrNb,GetDrawBufferAddress(),296,242,296,242,32,29);
			Display.IDongleShow = 0;
			Display.IDongleOff = 1;
		}
	}

	/*Equal*/
	if (RequestEqual != 0)
	{
		if (Display.IEqualShow != 1)
		{
			PutImage24BPPExt(80,244,&icon_equal);
			Display.IEqualShow = 1;
		}
	}
	else
	{
		if (Display.IEqualShow == 1)
		{
			CopyPageWindow(MainScrNb,GetDrawBufferAddress(),80,244,80,244,32,24);
			Display.IEqualShow = 0;
		}
	}

	switch (screenstate)
	{
	case DISPLAY_MAINCHARGE:
	case DISPLAY_EQUAL:
	case DISPLAY_AVAIL:
		if (IQWIIQLink  != 0)
		{
			if (Display.ILinkShow != 1)
			{
				PutImage24BPPExt(296,237,&WIIQ);
				Display.ILinkShow = 1;
			}

			if ((IQData.BatWarning & BatWarningLowLevel) != 0)
			{
				if (Display.IWaterShow != 1)
				{
					PutImage24BPPExt(12,243,&water);
					Display.IWaterShow = 1;
				}
			}
			else
			{
				if (Display.IWaterShow == 1)
				{
					Display.IWaterShow = 0;
					CopyPageWindow(MainScrNb,GetDrawBufferAddress(),12,243,12,243,32,24);
				}
			}

			if (((IQData.BatWarning & BatWarningBal1) != 0) || ((IQData.BatWarning & BatWarningBal2) != 0))
			{
				if (Display.IUnBalancedShow != 1)
				{
					PutImage24BPPExt(44,246,&bal);
					Display.IUnBalancedShow = 1;
				}
			}
			else
			{
				if (Display.IUnBalancedShow == 1)
				{
					Display.IUnBalancedShow = 0;
					CopyPageWindow(MainScrNb,GetDrawBufferAddress(),44,243,44,243,32,24);
				}
			}

		}
		else
		{
			if (Display.ILinkShow == 1)
			{
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),296,237,296,237,32,32);
				Display.ILinkShow = 1;
			}		
		}

		break;
	}

}
/*******************************************************************************
 * Outline       :
 * Description   : Drawing values to the display
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void BatterySocManager (void)
{
	static uint8_t PrevSoc = 0xFF;

#ifndef DEMO
	if ((ChgData.TimerSecCharge < (15*60L)) && (ChgData.Profile != LITH))
	{
		PrevSoc = 0;	// MODIF R3.0
		if (Display.ForceSoCUpdate != 0)
		{
			Display.ForceSoCUpdate = 0;
			PutImage24BPPExt(400,44,&jauge_right);
		}
		return;
	}
#endif

if ((PrevSoc != DataLcd.ChgSOC) || (Display.ForceSoCUpdate))
{
	PrevSoc = DataLcd.ChgSOC;

	if (DataLcd.ChgSOC > 20)
	{
		PutImage24BPPExt(400,44,&jauge_right);
		if ((DataLcd.ChgSOC > 95) && (DataLcd.ChgSOC < 100))  //
			PutImage24BPPExtPartial(400,44,((95*184)/100),184,&jauge_charge);
		else
			PutImage24BPPExtPartial(400,44,((DataLcd.ChgSOC*184)/100),184,&jauge_charge);
	}
	else if (DataLcd.ChgSOC <= 20)
	{
		PutImage24BPPExt(400,44,&jauge_right);
		PutImage24BPPExtPartial(400,44,DataLcd.ChgSOC*2,40,&jauge_DF4);
	}

	SetFont((void *) &Font_SvBold);
	SetColor(WHITE);
	SetBackColor(BLACK);
	OutTextXY(435,45,"100%");
	OutTextXY(447,211,"0%");

	Display.ForceSoCUpdate = 0;
}
}


/*******************************************************************************
 * Outline       :
 * Description   : Drawing values to the display
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void ChargeTimeManager (void)
{
	uint a,b;
	static uint8_t PrevPercent = 0xFF;
	uint8_t Percent;
	uint8_t Change = 0;
	uint16_t Phase3Time;
	uint8_t MaxTime;
	static uint8_t PrevMaxTime = 0xFF;
	static uint16_t PrevProfileTime = 0xFFFF;
	static ulong  PrevTimerSec = 0xFFFFFFFF;
	char text[5];

	if (screenstate == DISPLAY_IDLE)
	{
		PrevPercent = 0xFF;
		PrevMaxTime = 0xFF;
		PrevProfileTime = 0xFFFF;
		return;
	}
	else if (screenstate == DISPLAY_MAINCHARGE)
	{

		if (ChgData.TimerSecCharge < (15*60L))
		{
			if (ChgData.Profile != XFC) 
				MaxTime = 8; 		
			else
				MaxTime = 5; 		
		}
		else
		{
			MaxTime = DataLcd.ChgRestTime + ChgData.TimerSecCharge / 3600 + 1;	
		}

		Percent = (((uint32_t)DataLcd.ProfileTime * 100) / (MaxTime * 60));
		if (PrevPercent != Percent) Change = 1;
		if (PrevMaxTime != MaxTime) Change = 1;
		if (PrevProfileTime != DataLcd.ProfileTime) Change = 1;
		//if (PrevTimerSec != State.TimerSec) Change = 1;

	}
	else if (screenstate == DISPLAY_EQUAL)
	{
		// MODIF 3.3
		Phase3Time = TimeMaxEqual - ChgData.TimerSecCharge;
		Percent = 100 - ((Phase3Time*100) / TimeMaxEqual);
		if (PrevPercent != Percent) Change = 1;
		if (PrevTimerSec != State.TimerSec) Change = 1;
		MaxTime = TimeMaxEqual/3600;
		
		/*Phase3Time = ChgData.TimeMaxPhase1 - ChgData.TimerSecCharge;
		Percent = 100 - ((Phase3Time*100) / ChgData.TimeMaxPhase1);
		if (PrevPercent != Percent) Change = 1;
		if (PrevTimerSec != State.TimerSec) Change = 1;
		MaxTime = ChgData.TimeMaxPhase1/3600;*/
	}
	else if (screenstate == DISPLAY_AVAIL)
	{
		MaxTime = (Memo.ChgTime + 2) / 60;
		Percent = 100;
		if (PrevPercent != Percent) Change = 1;
		if (PrevMaxTime != MaxTime) Change = 1;
		if (PrevProfileTime != DataLcd.ProfileTime) Change = 1;
	}

	if ((Display.ForceTimeUpdate) || (Change == 1))
	{	
		if (screenstate == DISPLAY_EQUAL)
		{
			// MODIF 3.3
			Phase3Time = (TimeMaxEqual - ChgData.TimerSecCharge)/60L;
			//Phase3Time = (ChgData.TimeMaxPhase1 - ChgData.TimerSecCharge)/60L;
			
			a = (Phase3Time / 60) % 100;
			b = Phase3Time % 60;
			sprintf(&text[0], "%2.2uH%2.2u", a, b);
		}
		else
		{
			a = (DataLcd.ProfileTime / 60) % 100;
			b = DataLcd.ProfileTime % 60;
			sprintf(&text[0], "%2.2uH%2.2u", a, b);
		}

		CopyPageWindow(MainScrNb,GetDrawBufferAddress(),3,50,3,50,80,30);

		PutImage24BPPExt(20,44,&jauge_left);
		PutImage24BPPExtPartial(21,43,((Percent*184)/100),184,&jauge_time);		
		PrevPercent = Percent;
		PrevMaxTime = MaxTime;
		PrevProfileTime = DataLcd.ProfileTime;

		SetFont((void *) &Font_SvBold);
		SetColor(WHITE);
		SetBackColor(BLACK);
		OutTextXY(3,50,&text[0]);

		PrevTimerSec = State.TimerSec;
		Display.ForceTimeUpdate = 0;
	}
}


/*******************************************************************************
 * Outline       :
 * Description   : Drawing values to the display
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void TFT_Chrono(void)
{
	char text[50];

	uint32_t Temp;
	static uint32_t Store;
    static uint32_t LoadingStep;
	uint16_t TWidth,THeight,xpos,ypos;
    
	uint  a,b,c,d;
	uint  i, j;
	ulong TimeRest;

	uint8_t Page;
    
	switch (screenstate)
	{
	case CREATE_LOADING :
		MainScrNb = SCREEN_WAIT_MENU;
		CreateLoadingScreen();
		screenstate = DISPLAY_LOADING;
		SetBacklight(1);
		DisplayBrightness(100);
		Display.Update = 1;
        LoadingStep = 1;
		break;

	case CREATE_IDLE:
		MainScrNb = SCREEN_WAIT_MENU;
		CreateIdleScreen();
		screenstate = DISPLAY_IDLE;
		Display.Update = 1;
		break;

	case CREATE_STARTCHARGE:
		MainScrNb = SCREEN_CHARGE;
		CreateStartChargeScreen();
		screenstate = DISPLAY_STARTCHARGE;
		DataLcd.ChgSOC = 0;
		Display.Update = 1;
		break;				

	case CREATE_MAINCHARGE:
		MainScrNb = SCREEN_CHARGE;
		CreateMainChargeScreen();
		screenstate = DISPLAY_MAINCHARGE;
		Display.Update = 1;
		break;		

	case CREATE_EQUAL:
		MainScrNb = SCREEN_CHARGE;
		CreateEqualScreen();
		screenstate = DISPLAY_EQUAL;
		Display.Update = 1;
		break;		

	case CREATE_AVAILABLE:
		MainScrNb = SCREEN_AVAIL;
		screenstate = DISPLAY_AVAIL;
		CreateAvailableScreen();
		Display.Update = 1;
		break;

	case CREATE_FAULT:
		MainScrNb = SCREEN_FAULT;
		CreateFaultScreen();
		screenstate = DISPLAY_FAULT;
		Display.Update = 1;
		break;	


	case DISPLAY_LOADING:
		Temp = State.TimerSec;
		if (Store != Temp)
		{
			/*Clear previus message*/
			SetColor(BLACK);
			Bar(50,50,400,200);
			/*Put new message*/
			SetColor(WHITE);
			SetBackColor(BLACK);
			SetFont((void *) &Font_UniBold);

			/* Get text parameters */
			TWidth = GetTextWidth((char*)&ListWord16_State[1][LANG][0],(void *) &Font_UniBold);
			THeight = GetTextHeight((void *) &Font_UniBold);

			xpos = (GetMaxX()/2) - (TWidth/2);
			ypos = (GetMaxY()/2) - (THeight/2);
			while(!OutTextXY(xpos,ypos,(char*)&ListWord16_State[1][LANG][0]));
			sprintf(&text[0], "%1.1lus", (TIME_INIT - State.TimerSec));
			while(!OutTextXY((GetMaxX()/2)-20,GetMaxY()/2+15,&text[0]));
			PutImage24BPPExt((GetMaxX() - timer_WIDTH)/2,50,&loading);
			Display.Update = 1;
		}
		Store = Temp;
		ILcdNew = Menu.Imax;
		if (ILcdNew > Menu.ImaxCharger)
			ILcdNew = Menu.ImaxCharger;

		if (ILcd != ILcdNew)
			ILcd = ILcdNew;
            
        if (LoadingStep < 9)
        {	
            /*Load background images*/
            Page = GetDrawBufferAddress();

            switch (LoadingStep)
            {
            case 1:
            case 2:
                SetActivePage(3);
                PutImage24BPPExt(0,0,&background_avail);
                LoadingStep++;
                break;
            case 3:
            case 4:
                SetActivePage(5);
                PutImage24BPPExt(0,0,&background_charge_equal);
                LoadingStep++;
                break;
            case 5:
            case 6:
                SetActivePage(6);
                PutImage24BPPExt(0,0,&background_wait_menu_password);
                LoadingStep++;
                break;
            case 7:
            case 8:
                SetActivePage(7);
                PutImage24BPPExt(0,0,&background_defaut);
                LoadingStep++;
                break;

            }
            SetActivePage(Page);			
        }    
		break;

	case DISPLAY_IDLE:
		if (Display.UpdateDateTime == 1)
		{

   			ChargeTimeManager();        
            DisplayHostMessage();

			ILcdNew = Menu.Imax;
			if (ILcdNew > Menu.ImaxCharger)
				ILcdNew = Menu.ImaxCharger;

			if (ILcd != ILcdNew)
			{
				ILcd = ILcdNew;
				CreateScreenHeader();
				UpdateStateForceReloadIcons();
			}

            // MODIF 2.8
			//if (((CANState != 0) && (CANTimeout == 0)))
            if ((CANState != 0) && (CANTimeout == 0) && (Menu.Theme == THEMEA))
			{
				if (Display.PipEnabled == 0)
				{
					CreatePIPLayer(197,238,cable_WIDTH-9,cable_HEIGHT/2-1);
					PIPDSA(88,160,4);
					PIPState(1);
					Display.PipEnabled = 1;
				}

				/* Battery Connected */
				if (Battery != 0)
				{
					PIPDSA(88,160,4);
				}
				else
				{
					if ((State.TimerSec % 2) == 0)
					{
						PIPDSA(88,160 + cable_HEIGHT/2,4);
					}
					else
					{
						PIPDSA(88,160,4);
					}
				}
			}
			else
			{
				if (Display.PipEnabled == 1)
				{
					PIPState(0);
					Display.PipEnabled = 0;
				}
			}

			SetColor(WHITE);
			//SetFont((void *) &Font_UniBold);
            SetFont((void *) &unispace_bd_30);
          
			if (State.Charger == StateChgerManu)
			{   
                SetFont((void *) &Font_UniBold);
				sprintf(&text[0], "MANU");
				TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);
				xpos = GetMaxX()/2 - TWidth/2;
				ypos = 70;
				OutTextXY(xpos,ypos,&text[0]);
			}
            else if (Menu.Theme == THEMEB)
            {
                Display_idle_themeb();
            }
			else
			{
				sprintf(&text[0], "      ");
				TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);
				xpos = GetMaxX()/2 - TWidth/2;
				ypos = 70;
				OutTextXY(xpos,ypos,&text[0]);
			}

			/* Update Date/Time */
			UpdateDateTime();

			/* Icon management */
			IconManagement();

			Display.UpdateDateTime = 0;
			Display.Update = 1;
		}
		break;		

	case DISPLAY_STARTCHARGE:

		if (Display.UpdateDateTime == 1)
		{
			ILcdNew = Menu.Imax;
			if (ILcdNew > Menu.ImaxCharger)
				ILcdNew = Menu.ImaxCharger;

			if (ILcd != ILcdNew)
			{
				ILcd = ILcdNew;
				CreateScreenHeader();
				UpdateStateForceReloadIcons();
			}

			/* Update Date/Time */
			UpdateDateTime();
            DisplayHostMessage();
               
			/* Icon management */
			IconManagement();
			Display.UpdateDateTime = 0;
			Display.Update = 1;

		}
		else if (Display.UpdateCurrent == 1)
		{
			/* Update back counter */
			CopyPageWindow(MainScrNb,GetDrawBufferAddress(),136,150,136,150,216,40);

			SetBackColor(RGBConvert(0, 0, 255));
			SetColor(WHITE);
			SetFont((void *) &Font_UniBold);
			a = (DataLcd.DelayStChg / 3600) % 24;       /* Hours */
			b = (DataLcd.DelayStChg / 60) % 60;         /* Min */
			c = DataLcd.DelayStChg % 60;                /* Sec */
			sprintf(&text[0], "%2.2u:%2.2u:%2.2u", a, b, c);
			TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);
			xpos = GetMaxX()/2 - TWidth/2;
			ypos = 150;
			OutTextXY(xpos,ypos,&text[0]);
			if (BlockOutDef != 0)
			{
				SetFont((void *) &Font_SvBold);
				sprintf(&text[0], "%s", &ListWord16_State[24][LANG][0]);
				ypos = 192;
				xpos = (GetMaxX()/2)	- (GetTextWidth(&text[0],(void *) &Font_SvBold)/2);
				OutTextXY(xpos,ypos,&text[0]);
			}
            if ((EasyKitRead != 0) && (EasyKitLink == 0))
            {
                sprintf(&text[0], "Easy Read...");
				xpos = (GetMaxX()/2)	- (GetTextWidth(&text[0],(void *) &Font_SvBold)/2);
                ypos = 192;
				SetFont((void *) &Font_SvBold);
				SetColor(WHITE);
				SetBackColor(RGBConvert(0, 0, 255));
				OutTextXY(xpos,ypos,&text[0]);
            }            
			Display.UpdateCurrent = 0;
			Display.Update = 1;
		}

		/* Display capacity */
		if (Display.ICapacityShow != 1)
		{
			CopyPageWindow(MainScrNb,GetDrawBufferAddress(),200,240,200,240,96,32);
			SetColor(WHITE);
			SetFont((void *) &Font_SvBold);
			SetBackColor(RGBConvert(0, 0, 0));
			if ((IQWIIQLink != 0) || (Memo.CapAutoManu == 1))
				sprintf(&text[0], "C=%uAh", Memo.BatCap);
			else if (Menu.CapAutoManu != 0)
				sprintf(&text[0], "C=%uAh", Menu.BatCap);
			else
				sprintf(&text[0], "C=AUTO");
			TWidth = GetTextWidth(&text[0],(void *) &Font_SvBold);
			xpos = GetMaxX()/2 - TWidth/2;
			OutTextXY(xpos,250,&text[0]);
			Display.ICapacityShow = 1;
		}
		break;

	case DISPLAY_MAINCHARGE:
        // MODIF 2.8
		if (Menu.Theme == THEMEB)
        {
            Display_charge_themeb();
            break;
        }        

		if (Display.ForceReloadHeader == 1)
		{
			Display.ForceReloadHeader = 0;
			CreateScreenHeader();
			UpdateStateForceReloadIcons();
			IconManagement();
			UpdateDisplayNow();
		}

		if (Display.UpdateDateTime == 1)
		{
			/* Update Date/Time */
			UpdateDateTime();
            
			/* Icon management */
			IconManagement();
            
            if (ChargeDisconnect == 0)
            {
                CopyPageWindow(MainScrNb,GetDrawBufferAddress(),128,60,128,60,176,GetTextHeight((void *) &Font_UniBold));

    			if ((((State.TimerSec % 6) / 2) == 0)  && (ChgData.Profile != TEST))            /* DISPLAY VBAT/CELL: 2.25v*/
    			{
    				Display.ICapacityShow = 0;

    				a = (DataR.VbatCell + 5) / 1000;                /* Integer part */
    				b = ((DataR.VbatCell + 5) / 10) % 100;          /* Decimal part */
    				sprintf(&text[0], " %3u.%02uv", a, b);
    				/* Update text */
    				SetFont((void *) &Font_UniBold);
    				SetColor(WHITE);
    				SetBackColor(RGBConvert(0, 0, 61));
    				TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);
    				xpos = 300 - TWidth;
    				ypos = 60;
    				OutTextXY(xpos,ypos,&text[0]);
    			}
    			else if ((((State.TimerSec % 6) / 2) == 1)  && (ChgData.Profile != TEST))       /* DISPLAY VBAT: 24.5V */
    			{ 
    				a = (DataR.Vbat + 5) / 100;                     /* Integer part */
    				b = ((DataR.Vbat + 5) / 10) % 10;               /* Decimal part */
    				sprintf(&text[0], " %3u.%01uV", a, b);
    				/* Update text */
    				SetFont((void *) &Font_UniBold);
    				SetColor(WHITE);
    				SetBackColor(RGBConvert(0, 0, 61));

    				TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);			
    				xpos = 300 - TWidth;
    				ypos = 60;
    				OutTextXY(xpos,ypos,&text[0]);
    			}
    			else if ((((State.TimerSec % 6) / 2) == 2)  && (ChgData.Profile != TEST))       /* DISPLAY Ah: 451Ah */
    			{
    				a = (DataR.Vbat + 5) / 100;                     /* Integer part */
    				b = ((DataR.Vbat + 5) / 10) % 10;               /* Decimal part */
    				sprintf(&text[0], " %3u.%01uV", a, b);
    				/* Update text */
    				SetFont((void *) &Font_UniBold);
    				SetColor(WHITE);
    				SetBackColor(RGBConvert(0, 0, 61));

    				TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);
    				xpos = 300 - TWidth;
    				ypos = 60;
    				OutTextXY(xpos,ypos,&text[0]);
    			}
                else if (ChgData.Profile == TEST)
                {
                    sprintf(&text[0], "MOD%1u", (ptrMod + 1));
                    /* Update text */
    				SetFont((void *) &Font_UniBold);
    				SetColor(WHITE);
    				SetBackColor(RGBConvert(0, 0, 61));

    				TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);
    				xpos = 300 - TWidth;
    				ypos = 60;
    				OutTextXY(xpos,ypos,&text[0]);
                }
            }
              
			if (((State.TimerSec % 6) / 2) == 0)
				Display.ItempShow = 0;

			/*Both error posible - display in left top corner */
			if (((State.TimerSec % 6) / 2) == 0)
			{
				if (DFpump != 0)
				{
					Display.ItempShow = 1;
					CopyPageWindow(MainScrNb,GetDrawBufferAddress(),16,6,16,6,120,24);
					PutImage24BPPExt(20,6,&warning_charge);

					sprintf(&text[0], "%s", &ListWord14_Fault[0][LANG][0]);
					xpos = 50;
					ypos = 6;
					SetFont((void *) &Font_Inst);
					SetColor(WHITE);
					SetBackColor(RGBConvert(0, 0, 0));
					OutTextXY(xpos,ypos,&text[0]);
				}
#ifdef ENERSYS_EU
				else if (/*(DFtimeSecu != 0) ||*/ (DFotherBat != 0) || (DFdIdT != 0) || (DFconsMin != 0) || (DFDisDF5 != 0)) // DF5 in EU
				{
                    // MODIF R2.7 : no DF5 on display in Nexsys
                    if ((DFotherBat != 0) && ((ChgData.Profile == XFC) || (ChgData.Profile == TPPL) || (ChgData.Profile == NXBLC)))
                    {
                        // No DF5 display
                    }
                    else
                    {
    					Display.ItempShow = 1;
    					CopyPageWindow(MainScrNb,GetDrawBufferAddress(),16,6,16,6,120,24);
    					PutImage24BPPExt(20,6,&warning_charge);

    					sprintf(&text[0], "%s", &ListWord14_Fault[2][LANG][0]);
    					xpos = 50;
    					ypos = 6;
    					SetFont((void *) &Font_Inst);
    					SetColor(WHITE);
    					SetBackColor(RGBConvert(0, 0, 0));
    					OutTextXY(xpos,ypos,&text[0]);
                    }
				}
#else
				else if (/*(DFtimeSecu != 0) ||*/ (DFotherBat != 0) || (DFconsMin != 0) || (DFDisDF5 != 0))  // DF5 in US
				{
                    // MODIF R2.7 : no DF5 on display in Nexsys
                    if ((DFotherBat != 0) && ((ChgData.Profile == XFC) || (ChgData.Profile == TPPL) || (ChgData.Profile == NXBLC)))
                    {
                        // No DF5 display
                    }
                    else
                    {
    					Display.ItempShow = 1;
    					CopyPageWindow(MainScrNb,GetDrawBufferAddress(),16,6,16,6,120,24);
    					PutImage24BPPExt(20,6,&warning_charge);

    					sprintf(&text[0], "%s", &ListWord14_Fault[2][LANG][0]);
    					xpos = 50;
    					ypos = 6;
    					SetFont((void *) &Font_Inst);
    					SetColor(WHITE);
    					SetBackColor(RGBConvert(0, 0, 0));
    					OutTextXY(xpos,ypos,&text[0]);
                    }
				}
				else if (DFdIdT != 0)   // DF7 in US
				{
					Display.ItempShow = 1;
					CopyPageWindow(MainScrNb,GetDrawBufferAddress(),16,6,16,6,120,24);
					PutImage24BPPExt(20,6,&warning_charge);

					sprintf(&text[0], "%s", &ListWord14_Fault[0][LANG][0]);
					xpos = 50;
					ypos = 6;
					SetFont((void *) &Font_Inst);
					SetColor(WHITE);
					SetBackColor(RGBConvert(0, 0, 0));
					OutTextXY(xpos,ypos,&text[0]);
				}
#endif
			}
			else if ((DFoverdis != 0) && (((State.TimerSec % 6) / 2) == 1))
			{
				Display.ItempShow = 1;
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),16,6,16,6,120,24);
				PutImage24BPPExt(20,6,&warning_charge);

				sprintf(&text[0], "%s", &ListWord14_Fault[1][LANG][0]);
				xpos = 50;
				ypos = 6;
				SetFont((void *) &Font_Inst);
				SetColor(WHITE);
				SetBackColor(RGBConvert(0, 0, 0));
				OutTextXY(xpos,ypos,&text[0]);
			}

			//ModDefOn;
			//ModNumber = 6;
			if (((ModDef != 0) && (((State.TimerSec % 4) / 2) == 0)) && (ChargeDisconnect == 0))
			{
				FlagModDef = 1;
				Display.ItempShow = 1;
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),140,148,140,148,temp_charge_WIDTH,temp_charge_HEIGHT);
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),16,6,16,6,120,24);
				
				// MODIF A3.3
				//PutImage24BPPExt(20,6,&warning_charge);
				//sprintf(&text[0], "DFMOD");
				sprintf(&text[0], "     ");
				
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),160,(192-module_HEIGHT),160,(192-module_HEIGHT),200,module_HEIGHT);
				for (i=0; i<ModNumber; i++)
				{     
					// MODIF A3.3
					if ((ModVFuse (i) != 0)
						|| (LLCWarn(i) != 0)) 
					{
						PutImage24BPPExt(20,6,&warning_charge);
						sprintf(&text[0], "DFMOD");	
					}
					
					// MODIF A3.3		
					/*if (ModDefTempStop(i) != 0)
						sprintf(&text[0], "TH"); */                     /* If module in thermal fault change to TH */
					if ((CanErr(i) != 0) && ((PMod & (1 << i)) == 0))
						// MODIF 2.8
                        //PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 2 * i) - ((module_DF_WIDTH * 2 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module);
                        PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 1 * i) - ((module_DF_WIDTH * 1 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module);
					else if (/*(ModPwmMax(i) != 0)    
							|| (ModDefTempStop(i) != 0)
                            ||*/ (ModVFuse (i) != 0)
							/*|| (ModOtherDef(i) != 0)*/
                            //|| (ModDefProt(i) != 0)     // MODIF R2.7
                            || (LLCWarn(i) != 0)        // MODIF VLMFB
							/*|| (LLC[i].CntDefTemp > MAX_MOD_DEF_TEMP)	
							|| (LLC[i].CntDefCur > MAX_MOD_DEF_CUR)*/
							|| ((CanErr(i) != 0) && ((PMod & (1 << i)) != 0) && (i < ModNumber) && (StateIdle ==0)))
						// MODIF 2.8
						//PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 2 * i) - ((module_DF_WIDTH * 2 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module_DF);
                        PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 1 * i) - ((module_DF_WIDTH * 1 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module_DF);
					else
                        // MODIF 2.8
						//PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 2 * i) - ((module_DF_WIDTH * 2 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module_vide);
                        PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 1 * i) - ((module_DF_WIDTH * 1 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module_vide);
				}
				xpos = 50;
				ypos = 6;
				SetFont((void *) &Font_Inst);
				SetColor(WHITE);
				SetBackColor(RGBConvert(0, 0, 0));
				OutTextXY(xpos,ypos,&text[0]);
			}
			else if ((((State.TimerSec % 4) / 2) == 1)  && (State.Charge > StateChgIQLink))
			{
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),140,148,140,148,temp_charge_WIDTH,temp_charge_HEIGHT);
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),160,(192-module_HEIGHT),160,(192-module_HEIGHT),200,module_HEIGHT);
				SetColor(WHITE);
				SetBackColor(RGBConvert(0, 0, 61));
				//SetFont((void *) &Font_Menu);
				SetFont((void *) &Font_Inst);
				sprintf(&text[0], "%u Ah", Memo.ChgAh);
				xpos = (GetMaxX()/2)	- (GetTextWidth(&text[0],(void *) &Font_Inst)/2);
				OutTextXY(xpos,163,&text[0]);
			}

			// Clear ModDef information
			if ((ModDef == 0) && (FlagModDef == 1))
			{
				FlagModDef = 0;
				if ((DFpump == 0) && (DFoverdis == 0))
				{
					CopyPageWindow(MainScrNb,GetDrawBufferAddress(),6,6,6,6,140,24);
				}
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),166,(192-module_HEIGHT),166,(192-module_HEIGHT),200,module_HEIGHT);
			}

			/* Clear previous message */
			CopyPageWindow(MainScrNb,GetDrawBufferAddress(),136,192,136,192,200,GetTextHeight((void *) &Font_Inst));

			/* Update text */
			SetFont((void *) &Font_SvBold);
			SetColor(WHITE);
			SetBackColor(RGBConvert(0, 0, 61));
			ypos = 192;
			xpos = GetMaxX()/2;

			/* Display capacity */
			if (Display.ICapacityShow != 1)
			{
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),200,240,200,240,96,32);
				SetColor(WHITE);
				SetFont((void *) &Font_SvBold);
				SetBackColor(RGBConvert(0, 0, 0));
				if ((IQWIIQLink != 0) || (Memo.CapAutoManu == 1))
					sprintf(&text[0], "C=%uAh", Memo.BatCap);
				else if (Menu.CapAutoManu != 0)
					sprintf(&text[0], "C=%uAh", Menu.BatCap);
				else
					sprintf(&text[0], "C=AUTO");
				TWidth = GetTextWidth(&text[0],(void *) &Font_SvBold);
				xpos = GetMaxX()/2 - TWidth/2;
				OutTextXY(xpos,250,&text[0]);
				Display.ICapacityShow = 1;
			}

			if (BmsState != 0)
			{
				a = Bms.ChrgrDcU / 100;                     /* Integer part */
				b = (Bms.ChrgrDcU / 10) % 10;
				c = Bms.ChrgrDcI / 10;
				d = Bms.ChrgrDcI % 10;

				sprintf(&text[0], "BMS %3u.%01uV %1u.%01uA", a, b, c , d);
				xpos = (GetMaxX()/2)	- (GetTextWidth(&text[0],(void *) &Font_SvBold)/2 + 12);
				OutTextXY(xpos,ypos,&text[0]);

				/* Soc Management */
				BatterySocManager();

				/* Soc Management */
				ChargeTimeManager();

				if (Display.ItempShow != 1)
				{
					Display.ItempShow = 1;
					/* update Wi-IQ Temp */
					/*CopyPageWindow(MainScrNb,GetDrawBufferAddress(),164,147,164,147,32,GetTextHeight((void *) &Font_Menu));
					CopyPageWindow(MainScrNb,GetDrawBufferAddress(),160,(192-module_HEIGHT),160,(192-module_HEIGHT),210,module_HEIGHT);
					PutImage24BPPExt(140,148,&temp_charge);
					SetColor(WHITE);
					SetBackColor(RGBConvert(0, 0, 61));
					SetFont((void *) &Font_Menu);
					if (Menu.Area == 0)
						sprintf(&text[0], "%+2.2d'C", (Bms.Temp/10));
					else
						sprintf(&text[0], "%+2.2d'F", (Bms.Temp/10));
					OutTextXY(164,163,&text[0]);*/
				}

			}
			else if (IQWIIQLink == 0)  /* No WiIQ link */
			{
				if ((State.Charge == StateChgIQLink) || (State.Charge == StateChgIQScan))
				{
					switch (State.Charge)
					{
					case StateChgIQScan: // charger is in IQ Scan
						if (ItoReach != DataW.Ireq)
							sprintf(&text[0], "%10s  start...", &ListWord16_State[20][LANG][0]);
						else
							sprintf(&text[0], "%10s  %2d BAT", &ListWord16_State[20][LANG][0], IQData.ScanAddrLen);
#ifdef DEMO
						sprintf(&text[0], "%10s  %2d BAT", &ListWord16_State[20][LANG][0], (ChgData.TimerSecCharge/2));
#endif
						xpos = (GetMaxX()/2)	- (GetTextWidth(&text[0],(void *) &Font_SvBold)/2 + 12);
						OutTextXY(xpos,ypos,&text[0]);
						break;

					case StateChgIQLink: // charger is in IQ link step
						sprintf(&text[0], "%10s  %2d BAT", &ListWord16_State[21][LANG][0], IQData.ScanAddrLen);
						//xpos = (GetMaxX()/2)	- (GetTextWidth(&ListWord16_State[21][LANG][0],(void *) &Font_Inst)/2);
						xpos = (GetMaxX()/2)	- (GetTextWidth(&text[0],(void *) &Font_SvBold)/2 + 12);
						OutTextXY(xpos,ypos,&text[0]);
						break;

					}
				}
				else
				{

					/* Soc Management */
					BatterySocManager();					

					/* Soc Management */
					ChargeTimeManager();

					if (Display.ItempShow != 1)
					{				
						Display.ItempShow = 1;
						/* update Wi-IQ Temp */
						CopyPageWindow(MainScrNb,GetDrawBufferAddress(),164,147,164,147,32,GetTextHeight((void *) &Font_Menu));
						CopyPageWindow(MainScrNb,GetDrawBufferAddress(),160,(192-module_HEIGHT),160,(192-module_HEIGHT),210,module_HEIGHT);
						PutImage24BPPExt(140,148,&temp_charge);
						SetColor(WHITE);
						SetBackColor(RGBConvert(0, 0, 61));
						SetFont((void *) &Font_Menu);
						if (Menu.Area == 0)
							sprintf(&text[0], "%+2.2d'C", Menu.BatTemp);
						else
							sprintf(&text[0], "%+2.2d'F", Menu.BatTempF);
						OutTextXY(164,163,&text[0]);
					}

				}

			}
			else
			{
				/* Soc Management */
				BatterySocManager();

				/* Soc Management */
				ChargeTimeManager();

				if (Display.ItempShow != 1)
				{
					Display.ItempShow = 1;
					/* update Wi-IQ Temp */
					CopyPageWindow(MainScrNb,GetDrawBufferAddress(),164,147,164,147,32,GetTextHeight((void *) &Font_Menu));
					CopyPageWindow(MainScrNb,GetDrawBufferAddress(),160,(192-module_HEIGHT),160,(192-module_HEIGHT),210,module_HEIGHT);
					PutImage24BPPExt(140,148,&temp_charge);
					SetColor(WHITE);
					SetBackColor(RGBConvert(0, 0, 61));
					SetFont((void *) &Font_Menu);
					if (Menu.Area == 0)
						sprintf(&text[0], "%+2.2d'C", IQData.BatTemp);
					else
						sprintf(&text[0], "%+2.2d'F",((sint)IQData.BatTemp * 9) / 5 + 32 );
					OutTextXY(164,163,&text[0]);
				}

				sprintf(&text[0], "%14.14s", &IQData.BatSN[0]);
				xpos = (GetMaxX()/2)	- (GetTextWidth(&text[0],(void *) &Font_Menu)/2 + 12);

				SetFont((void *) &Font_Menu);
				SetColor(WHITE);
				SetBackColor(RGBConvert(0, 0, 61));
				OutTextXY(xpos,ypos,&text[0]);
			}

			Display.UpdateDateTime = 0;
			Display.Update = 1;
		}

		if (Display.UpdateCurrent == 1)
		{
			/* Clear previous message */
			CopyPageWindow(MainScrNb,GetDrawBufferAddress(),164,110,164,110,264,GetTextHeight((void *) &Font_UniBold));

			/* Update text */
			SetFont((void *) &Font_UniBold);
			SetColor(WHITE);
			SetBackColor(RGBConvert(0, 0, 60));
            // MODIF R2.3
            if (ChgData.Profile == TEST)
            {   // Show current with dA ptrMod
                a = (LLCRamRO[ptrMod].IBat / 10);
                b = LLCRamRO[ptrMod].IBat % 10;
			    sprintf(&text[0], "%1u.%01u A", a, b);
            }
            else
            {   
                a = (DataR.Ibat + 5) / 10;
			    sprintf(&text[0], "%3u A", a);
            }
			

			TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);
			xpos = 300 - TWidth;
			ypos = 110;
			OutTextXY(xpos,ypos,&text[0]);

			Display.UpdateCurrent = 0;
			Display.Update = 1;
            
            DisplayHostMessage();
		}
		break;

	case DISPLAY_FAULT:
		if (Display.UpdateDateTime == 1)
		{
			/* Update Date/Time */
			UpdateDateTime();

			/* Icon management */
			IconManagement();
            
			//FAULT_TEMP
			switch (State.Phase & 0xF0)
			{ // High TP Bat
			case StatePhDefBatHighTp :
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),GetMaxX()/2,120,GetMaxX()/2,100,120,GetTextHeight((void *) &Font_Inst));

				SetColor(WHITE);
				SetBackColor(RGBConvert(255, 0, 0));
				SetFont((void *) &Font_Inst);
				if (Menu.Area == 0)
					sprintf(&text[0], "%+2.2d'C", IQData.BatTemp);
				else
					sprintf(&text[0], "%+2.2d'F",((sint)IQData.BatTemp * 9) / 5 + 32 );
				OutTextXY(GetMaxX()/2,100,&text[0]);
				break;

			case StatePhDefID :
                ChargerID = ModChargerID();
				for (i=0; i<ModNumber; i++)
				{
					if ((State.TimerSec % 2) == 1)
					{
						/*if ( ( (LLCRamRO[i].Id != (ModChargerID() & LLCRamRO[i].Id)) 
                        //|| (LLCRamRO[i].Id != LLCRamRO[ModIndex[0]].Id )
                        || ((LLCRamRO[i].Id != LLCRamRO[ModIndex[0]].Id ) && (LLCRamRO[i].Id != (ModChargerID() & LLCRamRO[i].Id)))
                        || ((LLCRamRO[i].Id == ID_LLC3KW_3648VM) && (Menu.CellSize < 18)) )   
                            && (CanErr(i) == 0))*/
                        // MODIF R2.7
                        if (((LLCRamRO[i].Id != (ChargerID & LLCRamRO[i].Id)) 
                        || (LLCRamRO[ModIndex[0]].Id != LLCRamRO[i].Id)
                        || ((LLCRamRO[i].Id == ID_LLC3KW_3648VM) && (Menu.CellSize < 18)) )
                        && (CanErr(i) == 0) && (ModIndex[0] != MODNB))    
						{
                            if ((LLCRamRO[ModIndex[0]].Id == ID_LLC3KW_243648V) && (LLCRamRO[i].Id == ID_LLC3KW_243648VUS)) // MODIF R2.1
                			{
                                // Can work with module ID_LLC3KW_243648V &&  ID_LLC3KW_243648VUS
                			    // MODIF 2.8
                                //PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 2 * i) - ((module_DF_WIDTH * 2 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module);
                                PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 1 * i) - ((module_DF_WIDTH * 1 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module);
                            }
                            else if ((LLCRamRO[ModIndex[0]].Id == ID_LLC3KW_243648VUS) && (LLCRamRO[i].Id == ID_LLC3KW_243648V))
                			{
                                // Can work with module ID_LLC3KW_243648V &&  ID_LLC3KW_243648VUS
                			    // MODIF 2.8
                                //PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 2 * i) - ((module_DF_WIDTH * 2 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module);
                                PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 1 * i) - ((module_DF_WIDTH * 1 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module);
                            }
                            else
                            {
                                // MODIF 2.8
                                //PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 2 * i) - ((module_DF_WIDTH * 2 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module_DF);
                                PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 1 * i) - ((module_DF_WIDTH * 1 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module_DF);
                            
                            }
                        }
					}
					else
					{
                        // MODIF 2.8
						//PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 2 * i) - ((module_DF_WIDTH * 2 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module);
                        PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 1 * i) - ((module_DF_WIDTH * 1 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module);
					}
				}
				break;

			case StatePhDefCurWait :
			case StatePhDefCurSet :
				for (i=0; i<ModNumber; i++)
				{
					if ((CanErr(i) != 0) && ((PMod & (1 << i)) == 0))
						// MODIF 2.8
                        //PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 2 * i) - ((module_DF_WIDTH * 2 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module_vide);
                        PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 1 * i) - ((module_DF_WIDTH * 1 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module_vide);
					else if ((ModPwmMax(i) != 0)                    /* Display in RED the faulty module  */
							|| (ModDefTempStop(i) != 0)
							|| (ModOtherDef(i) != 0)
                            //|| (ModDefProt(i) != 0)     // MODIF R2.7
                            || (LLCWarn(i) != 0)        // MODIF VLMFB
                            || (ModVFuse (i) != 0)
							|| (LLC[i].CntDefTemp > MAX_MOD_DEF_TEMP)
							|| (LLC[i].CntDefCur > MAX_MOD_DEF_CUR)
							|| ((CanErr(i) != 0) && ((PMod & (1 << i)) != 0) && (i < ModNumber) && (StateIdle ==0)))
						// MODIF 2.8
                        //PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 2 * i) - ((module_DF_WIDTH * 2 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module_DF);
                        PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 1 * i) - ((module_DF_WIDTH * 1 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module_DF);
					else
                        // MODIF 2.8
						//PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 2 * i) - ((module_DF_WIDTH * 2 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module);
                        PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 1 * i) - ((module_DF_WIDTH * 1 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module);
                        
				}
				sprintf(&text[0], "DFMOD");
				xpos = 50;
				ypos = 6;
				SetFont((void *) &Font_Inst);
				SetColor(WHITE);
				SetBackColor(RGBConvert(0, 0, 0));
				OutTextXY(xpos,ypos,&text[0]);
				PutImage24BPPExt(20,6,&warning_charge);
				break;

			}
			Display.UpdateDateTime = 0;
			Display.Update = 1;
            DisplayHostMessage();
		}
		break;

	case DISPLAY_AVAIL:
        // MODIF 2.8
		if (Menu.Theme == THEMEB)
        {
            Display_avail_themeb();
            break;
        }  
        
		if (Display.UpdateDateTime == 1)
		{
			/* Update Date/Time */
			UpdateDateTime();
			
			/* Icon management */
			IconManagement();

			/* Time Management */
			ChargeTimeManager();

			if (Menu.FloatingOffOn == ON)
			{
                // MODIF R2.6
                if (ChgData.Profile != TPPL)
                {
                    SetFont((void *) &Font_Menu);
    				SetBackColor(RGBConvert(0, 255, 1));
    				SetColor(WHITE);
    				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),200,110,200,110,144,GetTextHeight((void *) &Font_Menu)*2+7);

    				if ((State.TimerSec % 4)/2 == 0)
    				{
    					sprintf(&text[0], "FLOATING");
    					xpos = 200;
    					ypos = 110;
    					OutTextXY(xpos,ypos,&text[0]);
    				}
    				else if ((State.TimerSec % 4)/2 == 1)
    				{
    					a = (DataR.Ibat + 5) / 10;
    					b = (DataR.VbatCell + 5) / 1000;                 /* Integer part */
    					c = ((DataR.VbatCell + 5) / 10) % 100;           /* Decimal part */
    					sprintf(&text[0], "%3uA %1u.%2.2uV", a, b , c);
    					xpos = 200;
    					ypos = 110;
    					OutTextXY(xpos,ypos,&text[0]);
    				}
                }
                else
                {
                    SetFont((void *) &Font_Menu);
    				SetBackColor(RGBConvert(0, 255, 1));
    				SetColor(WHITE);
                    if ((State.TimerSec % 4)/2 == 0)
    				{
                        CopyPageWindow(MainScrNb,GetDrawBufferAddress(),200,110,200,110,144,GetTextHeight((void *) &Font_Menu)*2+7);
                        a = Menu.Ifloating / 10;
    					sprintf(&text[0], "ILOAD ON - %3uA", a);
    					xpos = 200;
    					ypos = 110;
    					OutTextXY(xpos,ypos,&text[0]);
    				}
                }

			}
			else if (ChgData.TimeEqual == (0xFFFFFFFF-1))
			{
				SetFont((void *) &Font_Menu);
				SetBackColor(RGBConvert(0, 255, 1));
				SetColor(WHITE);
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),182,110,182,110,144,GetTextHeight((void *) &Font_Menu)*2+7);

				if ((State.TimerSec % 2) == 1)
				{
					for(i = 0; i < 7; i++)
					{
						Putchar(20+(i+9)*18,110,(char *)&ListWord16_2[0][LANG][i], WHITE);
					}
					j = 1;
					for (i = 0; i < 8; i++)
					{
						if ((Menu.EqualPeriod & j) != 0)
							text[0] = 36;
						else
							text[0] = 32;
						Putchar(182+(i*18),130,&text[0], WHITE);
						j = j << 1;
					}
				}
			}
			else if (ChgData.TimeEqual < (0xFFFFFFFF-1))
			{
				SetFont((void *) &Font_Menu);
				SetBackColor(RGBConvert(0, 255, 1));
				SetColor(WHITE);
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),182,110,182,110,160,GetTextHeight((void *) &Font_Menu)*2+7);

				if ((State.TimerSec % 2) == 1)
				{
					TimeRest = ( ChgData.TimeEqual - ChgData.TimerSecCharge);
					a = TimeRest / 3600;              /* Hours */
					b = (TimeRest / 60) % 60;         /* Min */
					c = TimeRest % 60;                /* Sec */
					sprintf(&text[0], "  %2.2u:%2.2u:%2.2u", a, b, c);
					text[0] = 36;
					TWidth = GetTextWidth(&text[0],(void *) &Font_Menu);
					xpos = 200;
					ypos = 110;
					OutTextXY(xpos,ypos,&text[0]);
				}
			}

			// Display AH & capacity
			if (((State.TimerSec % 4) / 2) == 1)
			{
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),140,148,140,148,temp_charge_WIDTH,temp_charge_HEIGHT);
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),166,(192-module_HEIGHT),166,(192-module_HEIGHT),200,module_HEIGHT);
				SetColor(WHITE);
				SetBackColor(RGBConvert(0, 0, 61));
				//SetFont((void *) &Font_Menu);
				SetFont((void *) &Font_Inst);
				sprintf(&text[0], "%u Ah", DataLcd.ProfileAh); //Memo.ChgAh);
				xpos = (GetMaxX()/2)	- (GetTextWidth(&text[0],(void *) &Font_Inst)/2);
				OutTextXY(xpos,163,&text[0]);
			}
            

			/*Both error posible - display in left top corner */
			//if ((DFpump != 0) && (((State.TimerSec % 6) / 2) == 0))
            if ((DFpump != 0) && ((State.TimerSec % 6)  == 1))
			{
				Display.ItempShow = 1;
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),16,6,16,6,120,24);
				PutImage24BPPExt(20,6,&warning_charge);

				sprintf(&text[0], "%s", &ListWord14_Fault[0][LANG][0]);
				xpos = 50;
				ypos = 6;
				SetFont((void *) &Font_Inst);
				SetColor(WHITE);
				SetBackColor(RGBConvert(0, 0, 0));
				OutTextXY(xpos,ypos,&text[0]);
			}
            //else if ((DFoverdis != 0) && (((State.TimerSec % 6) / 2) == 1))
            else if ((DFoverdis != 0) && ((State.TimerSec % 6)  == 2))
			{
				Display.ItempShow = 1;
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),16,6,16,6,120,24);
				PutImage24BPPExt(20,6,&warning_charge);

				sprintf(&text[0], "%s", &ListWord14_Fault[1][LANG][0]);
				xpos = 50;
				ypos = 6;
				SetFont((void *) &Font_Inst);
				SetColor(WHITE);
				SetBackColor(RGBConvert(0, 0, 0));
				OutTextXY(xpos,ypos,&text[0]);
			}
#ifndef ENERSYS_EU
            //else if (((DFdIdT != 0) || (DFtimeSecu != 0) || (DFDisDF5 != 0) || (DFotherBat != 0)) && (((State.TimerSec % 6) / 2) == 2))
			else if (((DFdIdT != 0) /*|| (DFtimeSecu != 0)*/ || (DFDisDF5 != 0) || (DFotherBat != 0)) && ((State.TimerSec % 6) == 3))
			{ 
                // MODIF R2.7 : no DF5 on display in Nexsys
                if ( (DFotherBat != 0) && ((ChgData.Profile == XFC) || (ChgData.Profile == TPPL) || (ChgData.Profile == NXBLC)) )
                {
                    // No DF5 display
                }
                else
                {
    				Display.ItempShow = 1;
    				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),16,6,16,6,120,24);
    				PutImage24BPPExt(20,6,&warning_charge);
                    if (DFdIdT != 0)    // DF7
    				    sprintf(&text[0], "%s", &ListWord14_Fault[0][LANG][0]);
                    else                // DF5
                        sprintf(&text[0], "%s", &ListWord14_Fault[2][LANG][0]);
    				xpos = 50;
    				ypos = 6;
    				SetFont((void *) &Font_Inst);
    				SetColor(WHITE);
    				SetBackColor(RGBConvert(0, 0, 0));
    				OutTextXY(xpos,ypos,&text[0]);
                }
			}
#else
            //else if (((DFdIdT != 0) || (DFtimeSecu != 0) || (DFDisDF5 != 0) || (DFotherBat != 0)) && (((State.TimerSec % 6) / 2) == 2))
	        else if (((DFdIdT != 0) /*|| (DFtimeSecu != 0)*/ || (DFDisDF5 != 0) || (DFotherBat != 0)) && ((State.TimerSec % 6) == 3))
			{   // DF5
            
                // MODIF R2.7 : no DF5 on display in Nexsys
                if ((DFotherBat != 0) && ((ChgData.Profile == XFC) || (ChgData.Profile == TPPL) || (ChgData.Profile == NXBLC)))
                {
                    // No DF5 display
                }
                else
                {
    				Display.ItempShow = 1;
    				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),16,6,16,6,120,24);
    				PutImage24BPPExt(20,6,&warning_charge);

    				sprintf(&text[0], "%s", &ListWord14_Fault[2][LANG][0]);
    				xpos = 50;
    				ypos = 6;
    				SetFont((void *) &Font_Inst);
    				SetColor(WHITE);
    				SetBackColor(RGBConvert(0, 0, 0));
    				OutTextXY(xpos,ypos,&text[0]);
                }
			}
#endif
            else if (((ModDef != 0) && ((State.TimerSec % 6) == 4)) && (ChargeDisconnect == 0))
			{
				Display.ItempShow = 1;
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),16,6,16,6,120,24);
				// MODIF A3.3
				for (i=0; i<ModNumber; i++)
				{
					// MODIF A3.3
					if ((ModVFuse (i) != 0)
						|| (LLCWarn(i) != 0)) 
					{
						PutImage24BPPExt(20,6,&warning_charge);
						sprintf(&text[0], "DFMOD");	
					}
				}
            }            

			/* Display capacity */
			if (Display.ICapacityShow != 1)
			{
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),200,240,200,240,96,32);
				SetColor(WHITE);
				SetFont((void *) &Font_SvBold);
				SetBackColor(RGBConvert(0, 0, 0));
				if ((IQWIIQLink != 0) || (Memo.CapAutoManu == 1))
					sprintf(&text[0], "C=%uAh", Memo.BatCap);
				else if (Menu.CapAutoManu != 0)
					sprintf(&text[0], "C=%uAh", Menu.BatCap);
				else
					sprintf(&text[0], "C=AUTO");
				TWidth = GetTextWidth(&text[0],(void *) &Font_SvBold);
				xpos = GetMaxX()/2 - TWidth/2;
				OutTextXY(xpos,250,&text[0]);
				Display.ICapacityShow = 1;
			}

			if (IQWIIQLink != 0)
			{
				Display.ILinkShow = 0;
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),192,147,192,147,32,GetTextHeight((void *) &Font_Menu));

				sprintf(&text[0], "%14.14s", &IQData.BatSN[0]);
				xpos = (GetMaxX()/2)	- (GetTextWidth(&text[0],(void *) &Font_Menu)/2 + 12);
				ypos = 192;
				SetFont((void *) &Font_Menu);
				SetColor(WHITE);
				SetBackColor(RGBConvert(0, 255, 1));
				OutTextXY(xpos,ypos,&text[0]);
			}
			
            DisplayHostMessage();
            
			Display.UpdateDateTime = 0;
			Display.Update = 1;
		}

		break;

	case DISPLAY_EQUAL:

        // MODIF 2.8
		if (Menu.Theme == THEMEB)
        {
            Display_equal_themeb();
            break;
        } 

		if (Display.UpdateDateTime == 1)
		{
			/* Update Date/Time */
			UpdateDateTime();
            
			/* Icon management */
			IconManagement();

            if (ChargeDisconnect == 0)
            {
                /* Clear previous message */
    			CopyPageWindow(MainScrNb,GetDrawBufferAddress(),128,60,128,60,180,GetTextHeight((void *) &Font_UniBold));

    			if (((State.TimerSec % 4) / 2) == 0)
    			{
    				a = (DataR.VbatCell + 5) / 1000;                 /* Integer part */
    				b = ((DataR.VbatCell + 5) / 10) % 100;           /* Decimal part */
    				sprintf(&text[0], "%3u.%02u v", a, b);
    			}
    			else if (((State.TimerSec % 4) / 2) == 1)
    			{
    				a = (DataR.Vbat + 5) / 100;                     /* Integer part */
    				b = ((DataR.Vbat + 5) / 10) % 10;               /* Decimal part */
    				sprintf(&text[0], "%3u.%01u V", a, b);
    			}

    			/* Update text */
    			SetFont((void *) &Font_UniBold);
    			SetColor(WHITE);
    			SetBackColor(RGBConvert(0, 0, 61));

    			TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);
    			xpos = 310 - TWidth;
    			ypos = 60;
    			OutTextXY(xpos,ypos,&text[0]);    
            }
			

			ChargeTimeManager();

			/* Update text */
			SetFont((void *) &Font_Inst);
			SetColor(BLACK);
			ypos = 190;
			xpos = GetMaxX()/2;

			if (((ModDef != 0) && (((State.TimerSec % 4) / 2) == 0)) && (ChargeDisconnect == 0))
			{
				FlagModDef = 1;
				Display.ItempShow = 1;
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),140,148,140,148,temp_charge_WIDTH,temp_charge_HEIGHT);
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),16,6,16,6,120,24);
				// MODIF A3.3
				//PutImage24BPPExt(20,6,&warning_charge);
				//sprintf(&text[0], "DFMOD");
				sprintf(&text[0], "     ");
				
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),166,(192-module_HEIGHT),166,(192-module_HEIGHT),200,module_HEIGHT);
				for (i=0; i<ModNumber; i++)
				{
					// MODIF A3.3
					if ((ModVFuse (i) != 0)
						|| (LLCWarn(i) != 0)) 
					{
						PutImage24BPPExt(20,6,&warning_charge);
						sprintf(&text[0], "DFMOD");	
					}
					
					// MODIF A3.3
					/*if (ModDefTempStop(i) != 0)
						sprintf(&text[0], "TH"); */
					if ((CanErr(i) != 0) && ((PMod & (1 << i)) == 0))
						// MODIF 2.8
                        //PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 2 * i) - ((module_DF_WIDTH * 2 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module);
                        PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 1 * i) - ((module_DF_WIDTH * 1 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module);
					else if (/*(ModPwmMax(i) != 0) 
							|| (ModDefTempStop(i) != 0)
                            ||*/ (ModVFuse (i) != 0)
							/*|| (ModOtherDef(i) != 0)*/
                            //|| (ModDefProt(i) != 0)     // MODIF R2.7
                            || (LLCWarn(i) != 0)        // MODIF VLMFB
							/*|| (LLC[i].CntDefTemp > MAX_MOD_DEF_TEMP)
							|| (LLC[i].CntDefCur > MAX_MOD_DEF_CUR)*/
							|| ((CanErr(i) != 0) && ((PMod & (1 << i)) != 0) && (i < ModNumber) && (StateIdle ==0)))
						// MODIF 2.8
                        //PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 2 * i) - ((module_DF_WIDTH * 2 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module_DF);
                        PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 1 * i) - ((module_DF_WIDTH * 1 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module_DF);
					else
                        // MODIF 2.8
						//PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 2 * i) - ((module_DF_WIDTH * 2 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module_vide);
                        PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 1 * i) - ((module_DF_WIDTH * 1 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 40,&module_vide);
				}
				xpos = 50;
				ypos = 6;
				SetFont((void *) &Font_Inst);
				SetColor(WHITE);
				SetBackColor(RGBConvert(0, 0, 0));
				OutTextXY(xpos,ypos,&text[0]);
			}
			else if (((State.TimerSec % 4) / 2) == 1)
			{
				// Display Equal Image
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),166,(192-module_HEIGHT),166,(192-module_HEIGHT),200,module_HEIGHT);
				PutImage24BPPExt(263,158,&equal);
			}
			// Clear ModDef information
			if ((ModDef == 0) && (FlagModDef == 1))
			{
				FlagModDef = 0;
				if ((DFpump == 0) && (DFoverdis == 0))
				{
					CopyPageWindow(MainScrNb,GetDrawBufferAddress(),6,6,6,6,140,24);
				}
			}

			if (IQWIIQLink != 0)
			{

				if (Display.ItempShow != 1)
				{
					PutImage24BPPExt(140,148,&temp_charge);
					Display.ItempShow = 1;
				}

				//update Temp//
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),164,147,164,147,32,GetTextHeight((void *) &Font_Menu));

				SetColor(WHITE);
				SetBackColor(RGBConvert(0, 0, 61));
				SetFont((void *) &Font_Menu);
				if (Menu.Area == 0)
					sprintf(&text[0], "%+2.2d'C", IQData.BatTemp);
				else
					sprintf(&text[0], "%+2.2d'F",((sint)IQData.BatTemp * 9) / 5 + 32 );
				OutTextXY(164,163,&text[0]);

				sprintf(&text[0], "%14.14s", &IQData.BatSN[0]);
				xpos = (GetMaxX()/2)	- (GetTextWidth(&text[0],(void *) &Font_Menu)/2);

				SetFont((void *) &Font_Menu);
				SetColor(WHITE);
				SetBackColor(RGBConvert(0, 0, 61));
				OutTextXY(xpos,ypos,&text[0]);
			}

            DisplayHostMessage();
                
			Display.UpdateDateTime = 0;
			Display.Update = 1;
		}

		if (Display.UpdateCurrent == 1)
		{
			/* Clear previous message */
			CopyPageWindow(MainScrNb,GetDrawBufferAddress(),184,110,184,110,224,GetTextHeight((void *) &Font_UniBold));

			/* Update text */
			SetFont((void *) &Font_UniBold);
			SetColor(WHITE);
			SetBackColor(RGBConvert(0, 0, 60));

			a = (DataR.Ibat + 5) / 10;
			sprintf(&text[0], "%3u A", a);

			TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);
			xpos = 310 - TWidth;
			ypos = 110;
			OutTextXY(xpos,ypos,&text[0]);

			Display.UpdateCurrent = 0;
			Display.Update = 1;
		}
		break;		



	case DISPLAY_MENU:
		/* Nothing to do */
		break;

	}
	if (Display.Update == 1)
	{
		UpdateDisplayNow();	
		Display.Update = 0;
	}
}

/*******************************************************************************
 * Outline       :
 * Description   : Display IP address and HostMessage 1 and 2 every 2sec
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void DisplayHostMessage (void)
{
    static char text_msg[50];
	uint16_t xpos,ypos;
    
    StructLanConfig *TempIP;
	TempIP = (StructLanConfig *)&LcdTemp[0];

	if (screenstate == DISPLAY_IDLE)
	{
		ypos = 79;
		SetColor(WHITE);
		CopyPageWindow(MainScrNb,GetDrawBufferAddress(),(GetMaxX()/2)-150,ypos,(GetMaxX()/2)-150,ypos,300,30);

		if (BmsState != 0)
			sprintf(&text_msg[0], "BMS CAN CONNECTED");
		else
			sprintf(&text_msg[0], "                 ");
		xpos = (GetMaxX()/2) - (GetTextWidth(&text_msg[0],(void *) &Font_SvBold)/2);
		ypos = 79;
		OutTextXY(xpos,ypos,&text_msg[0]);
	}

    // MODIF R2.1 -> Display ID Address (DHCP or FIXED)
    if ((Display.WifiOffShow != 0) 
                || (Display.WifiOnShow != 0)
                || (Display.LanOffShow != 0)
                || (Display.LanOnShow != 0))
    {
                Read_SegFlashData (SEGLANCONFIG, (uchar *)TempIP);
                if (((*TempIP).EthParam[0] != 0) 
                    || ((*TempIP).EthParam[1] != 0)
                    || ((*TempIP).EthParam[2] != 0)
                    || ((*TempIP).EthParam[3] != 0))
                {
                    IP_DHCP[0] = (*TempIP).EthParam[0];
                    IP_DHCP[1] = (*TempIP).EthParam[1];
                    IP_DHCP[2] = (*TempIP).EthParam[2];
                    IP_DHCP[3] = (*TempIP).EthParam[3];
                }
                
                /* Host message (could have spaces in it) */
                if ((State.Charger == StateChgerChg) && (State.Charge > StateChgStartPhase1) && ((ChgData.Profile == TPPL) || (ChgData.Profile == XFC)) && (DFotherBat != 0))
                {
                    if (((State.TimerSec % 6) == 1) ||((State.TimerSec % 6) == 2))
                    {
                       Host.HostMsgText0[19] = 0;
                       sprintf(&text_msg[0], "%s", "Battery capacity"); 
                    }
                    else if (((State.TimerSec % 6) == 3) ||((State.TimerSec % 6) == 4))
                    {
                        sprintf(&text_msg[0],"%s", "out of range");
                    }
                }
                else
                {
                    if ((HostMsg0 != 0) && (((State.TimerSec % 6) == 1) ||((State.TimerSec % 6) == 2)))
                    {
                        Host.HostMsgText0[19] = 0;
                        sprintf(&text_msg[0], "%s", &Host.HostMsgText0[0]);
                    }
                    if ((HostMsg1 != 0) && (((State.TimerSec % 6) == 3) ||((State.TimerSec % 6) == 4)))
                    {
                        sprintf(&text_msg[0],"%s", &Host.HostMsgText1[0]);
                    }
                }
                
	            if ((IP_DHCP[0] == 0) && (IP_DHCP[1] == 0) && (IP_DHCP[2] == 0) && (IP_DHCP[3] == 0))
                {
                    //if ((State.TimerSec % 2) == 0)
                    if (((State.TimerSec % 6) == 5) || ((State.TimerSec % 6) == 6))
                        sprintf(&text_msg[0],"     DHCP MODE     ");
                    //else
                    //    sprintf(&text[0],"                   ");
                }
                //else
                else if (((State.TimerSec % 6) == 5) || ((State.TimerSec % 6) == 6))
                {
                    sprintf(&text_msg[0],"%1u.%1u.%1u.%1u", IP_DHCP[0], IP_DHCP[1], IP_DHCP[2], IP_DHCP[3]);
                }
				xpos = (GetMaxX()/2) - (GetTextWidth(&text_msg[0],(void *) &Font_SvBold)/2);
                ypos = 209;
				SetFont((void *) &Font_SvBold);
				SetColor(WHITE);
                //CopyPageWindow(MainScrNb,GetDrawBufferAddress(),xpos,ypos,xpos,ypos,250,40);
                CopyPageWindow(MainScrNb,GetDrawBufferAddress(),(GetMaxX()/2)-150,ypos,(GetMaxX()/2)-150,ypos,300,30);
				OutTextXY(xpos,ypos,&text_msg[0]);
    }
    else if ((State.Charger == StateChgerChg) && (State.Charge > StateChgStartPhase1) 
        && ((ChgData.Profile == TPPL) || (ChgData.Profile == XFC) || (ChgData.Profile == NXBLC)) && (DFotherBat != 0))
    {
            if (((State.TimerSec % 6) == 1) ||((State.TimerSec % 6) == 2))
            {
                Host.HostMsgText0[19] = 0;
                sprintf(&text_msg[0], "%s", "Battery capacity"); 
            }
            else if (((State.TimerSec % 6) == 3) ||((State.TimerSec % 6) == 4))
            {
                sprintf(&text_msg[0],"%s", "out of range");
            }
            xpos = (GetMaxX()/2) - (GetTextWidth(&text_msg[0],(void *) &Font_SvBold)/2);
            ypos = 209;
			SetFont((void *) &Font_SvBold);
			SetColor(WHITE);
            CopyPageWindow(MainScrNb,GetDrawBufferAddress(),(GetMaxX()/2)-150,ypos,(GetMaxX()/2)-150,ypos,300,30);
			OutTextXY(xpos,ypos,&text_msg[0]);
    }
    else
    {
        ypos = 209;
        CopyPageWindow(MainScrNb,GetDrawBufferAddress(),(GetMaxX()/2)-150,ypos,(GetMaxX()/2)-150,ypos,300,30);
    }
}
/*********************End of File************************************/
