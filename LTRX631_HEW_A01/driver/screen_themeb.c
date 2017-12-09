/*******************************************************************************
 * DISCLAIMER
 *
 *
 *
 *
 *******************************************************************************/
/* Copyright (C) 2016 Enersys. All rights reserved.   */
/*******************************************************************************
 * File Name     : screen.c
 * Version       : 1.00
 * Device        : R5F563NB (RX631)
 * Tool-Chain    : Renesas RX Standard 1.2.0.0
 * H/W Platform  : LIFEIQ
 * Description   : Period Update screen functions Theme B
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
extern  schar TambCompare;  // MODIF TH-AMB
extern  uchar ModIndex[MODNB+1];
extern  uchar ptrMod;
extern  uint PMod;
extern  uchar FlagDisconnectInCharge;
extern uchar  ChargerID;

extern const uchar ListWord14_Fault[3][NBLANG][14];
extern uchar FlagDisconnectInCharge;
extern uchar FlagModDef;

const uchar MsgThemeB[11][NBLANG][12] = {
    {"        ",    "       ",      "       ",      "       ",      "       ",},    	// 0
    {"CHARGEUR",    "CHARGER",      "CHARGER",      "CARICA",       "CARGADOR",},    	// 1
    {"EN PAUSE",    "PAUSED",       "PAUSED",       "PAUSA",        "EN PAUSA",},     	// 2
    {"CONNECTER",   "CONNECT",      "CONNECT",      "CONNETTI",     "CONECTAR",},    	// 3
    {"BATTERIE",    "BATTERY",      "BATTERY",      "BATTERIA",     "BATERIA",},    	// 4
    {"DECONNECTE",  "DISCONNECT",   "DISCONNECT",   "DISCONNESSA",  "DESCONECT.",}, 	// 5
    {"EN",          "WHILE",        "WHILE",        "DURANTE",      "EN",},      		// 6
    {"CHARGE",      "CHARGING",     "CHARGING",     "CARICA",       "CARGA",},   		// 7
    {"CHARGE",      "CHARGE",       "CHARGE",       "CARICA",       "CARGA",},     		// 8
    {"COMPLETE",    "COMPLETE",     "COMPLETE",     "COMPLETA",     "TERMINADA",},   	// 9
    {"EGAL",        "EQUAL",        "EQUAL",        "EQUAL",        "IGUAL",},      	// 11
};


/**
 * @brief  Display idle data with THEME B
 * @param  None
 * @retval None
 */
void Display_idle_themeb (void)
{
    char text[50];
    uint16_t TWidth,xpos,ypos;
    
    if ((Battery != 0) && (StartStop == 0))
    {
        PutImage24BPPExt(0,0,&background_defaut);
        CreateScreenHeader();
        UpdateStateForceReloadIcons();
	    IconManagement();
	    Display.UpdateDateTime = 1;
        
        SetColor(WHITE);
        SetFont((void *) &unispace_bd_30);
        
        sprintf(&text[0], "%s", &MsgThemeB[0][LANG][0]);    // "       "
		TWidth = GetTextWidth(&text[0],(void *) &unispace_bd_30);
		xpos = GetMaxX()/2 - TWidth/2;
		ypos = 60;
		OutTextXY(xpos,ypos,&text[0]);
        
        sprintf(&text[0], "%s", &MsgThemeB[1][LANG][0]);    // CHARGER
		TWidth = GetTextWidth(&text[0],(void *) &unispace_bd_30);
		xpos = GetMaxX()/2 - TWidth/2;
		ypos = 60 + GetTextHeight((void *) &unispace_bd_30);
        OutTextXY(xpos,ypos,&text[0]);
        
        sprintf(&text[0], "%s", &MsgThemeB[2][LANG][0]);    // PAUSED
		TWidth = GetTextWidth(&text[0],(void *) &unispace_bd_30);
		xpos = GetMaxX()/2 - TWidth/2;
		ypos = 60 + 2 * GetTextHeight((void *) &unispace_bd_30);
        OutTextXY(xpos,ypos,&text[0]);
        
    }
    else
    {
        if ((FlagDisconnectInCharge != 0) && ((State.TimerSec % 3) == 0) )
        {
            PutImage24BPPExt(0,0,&background_defaut);
            CreateScreenHeader();
            UpdateStateForceReloadIcons();
	        IconManagement();
	        Display.UpdateDateTime = 1;
            SetColor(WHITE);
            SetFont((void *) &unispace_bd_30);
                       
            sprintf(&text[0], "%s", &MsgThemeB[4][LANG][0]);    // BATTERY
			TWidth = GetTextWidth(&text[0],(void *) &unispace_bd_30);
			xpos = GetMaxX()/2 - TWidth/2;
			ypos = 60;
			OutTextXY(xpos,ypos,&text[0]);
            
            sprintf(&text[0], "%s", &MsgThemeB[5][LANG][0]);    // DISCONNECT
			TWidth = GetTextWidth(&text[0],(void *) &unispace_bd_30);
			xpos = GetMaxX()/2 - TWidth/2;
			ypos = 60 + GetTextHeight((void *) &unispace_bd_30);
            OutTextXY(xpos,ypos,&text[0]);
            
            sprintf(&text[0], "%s", &MsgThemeB[6][LANG][0]);    // WHILE
			TWidth = GetTextWidth(&text[0],(void *) &unispace_bd_30);
			xpos = GetMaxX()/2 - TWidth/2;
			ypos = 60 + 2 * GetTextHeight((void *) &unispace_bd_30);
            OutTextXY(xpos,ypos,&text[0]);
            
            sprintf(&text[0], "%s", &MsgThemeB[7][LANG][0]);    // CHARGING
			TWidth = GetTextWidth(&text[0],(void *) &unispace_bd_30);
			xpos = GetMaxX()/2 - TWidth/2;
			ypos = 60 + 3 * GetTextHeight((void *) &unispace_bd_30);
            OutTextXY(xpos,ypos,&text[0]);
        }
        else
        {
            PutImage24BPPExt(0,0,&background_wait_menu_password);
            CreateScreenHeader();
            UpdateStateForceReloadIcons();
	        IconManagement();
	        Display.UpdateDateTime = 1;
            SetColor(WHITE);
            SetFont((void *) &unispace_bd_30);
                       
            sprintf(&text[0], "%s", &MsgThemeB[0][LANG][0]);    // "       "
			TWidth = GetTextWidth(&text[0],(void *) &unispace_bd_30);
			xpos = GetMaxX()/2 - TWidth/2;
			ypos = 60;
            OutTextXY(xpos,ypos,&text[0]);
            
            sprintf(&text[0], "%s", &MsgThemeB[3][LANG][0]);    // CONNECT
			TWidth = GetTextWidth(&text[0],(void *) &unispace_bd_30);
			xpos = GetMaxX()/2 - TWidth/2;
			ypos = 60 + GetTextHeight((void *) &unispace_bd_30);
            OutTextXY(xpos,ypos,&text[0]);
            
            sprintf(&text[0], "%s", &MsgThemeB[4][LANG][0]);    // BATTERY
			TWidth = GetTextWidth(&text[0],(void *) &unispace_bd_30);
			xpos = GetMaxX()/2 - TWidth/2;
			ypos = 60 + 2 * GetTextHeight((void *) &unispace_bd_30);
            OutTextXY(xpos,ypos,&text[0]);
        }             
    }
	
	DisplayHostMessage();
}

/**
 * @brief  Display charge data with THEME B
 * @param  None
 * @retval None
 */
void Display_charge_themeb (void)
{
    
        char text[50];
        uint16_t TWidth,xpos,ypos,i;
        uint  a,b,c,d;
        

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
                CopyPageWindow(MainScrNb,GetDrawBufferAddress(),128,42 + 2 * GetTextHeight((void *) &Font_UniBold),128,42 + 2 * GetTextHeight((void *) &Font_UniBold),176,GetTextHeight((void *) &Font_UniBold));
                
                SetColor(WHITE);
                SetFont((void *) &unispace_bd_30);
                sprintf(&text[0], "%s", &MsgThemeB[7][LANG][0]);    // CHARGING
			    TWidth = GetTextWidth(&text[0],(void *) &unispace_bd_30);
			    xpos = GetMaxX()/2 - TWidth/2;
			    ypos = 55;
                OutTextXY(xpos,ypos,&text[0]);

    			if ((((State.TimerSec % 6) / 2) == 0)  && (ChgData.Profile != TEST))            /* DISPLAY VBAT/CELL: 2.25v*/
    			{
    				Display.ICapacityShow = 0;

    				a = (DataR.VbatCell + 5) / 1000;                /* Integer part */
    				b = ((DataR.VbatCell + 5) / 10) % 100;          /* Decimal part */
    				sprintf(&text[0], " %3u.%02uv", a, b);
    				SetFont((void *) &Font_UniBold);
    				SetColor(WHITE);
    				SetBackColor(RGBConvert(0, 0, 61));
    				TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);
    				xpos = 300 - TWidth;
    				ypos = 42 + 2 * GetTextHeight((void *) &Font_UniBold);
    				OutTextXY(xpos,ypos,&text[0]);
    			}
    			else if ((((State.TimerSec % 6) / 2) == 1)  && (ChgData.Profile != TEST))       /* DISPLAY VBAT: 24.5V */
    			{ 
    				a = (DataR.Vbat + 5) / 100;                     /* Integer part */
    				b = ((DataR.Vbat + 5) / 10) % 10;               /* Decimal part */
    				sprintf(&text[0], " %3u.%01uV", a, b);
    				SetFont((void *) &Font_UniBold);
    				SetColor(WHITE);
    				SetBackColor(RGBConvert(0, 0, 61));

    				TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);			
    				xpos = 300 - TWidth;
    				ypos = 42 + 2 * GetTextHeight((void *) &Font_UniBold);
    				OutTextXY(xpos,ypos,&text[0]);
    			}
    			else if ((((State.TimerSec % 6) / 2) == 2)  && (ChgData.Profile != TEST))       /* DISPLAY Ah: 451Ah */
    			{
    				sprintf(&text[0], "%uAh", Memo.ChgAh);
                    SetFont((void *) &Font_UniBold);
    				SetColor(WHITE);
    				SetBackColor(RGBConvert(0, 0, 61));

    				TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);
                    xpos = GetMaxX()/2 - TWidth/2;
    				ypos = 42 + 2 * GetTextHeight((void *) &Font_UniBold);
    				OutTextXY(xpos,ypos,&text[0]);
    			}
                else if (ChgData.Profile == TEST)
                {
                    sprintf(&text[0], "MOD%1u", (ptrMod + 1));
    				SetFont((void *) &Font_UniBold);
    				SetColor(WHITE);
    				SetBackColor(RGBConvert(0, 0, 61));

    				TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);
    				xpos = 300 - TWidth;
    				ypos = 45 + 2 * GetTextHeight((void *) &Font_UniBold);
    				OutTextXY(xpos,ypos,&text[0]);
                }
            }
              
			/*Both error posible - display in left top corner */
			if (((State.TimerSec % 6) / 2) == 0)
			{
				if (DFpump != 0)
				{
					//Display.ItempShow = 1;
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

            // Debug    
			/*ModDefOn;
			ModNumber = 6;*/
            // End
			
            if (((ModDef != 0) && (((State.TimerSec % 4) / 2) == 0)) && (ChargeDisconnect == 0))
			{
				FlagModDef = 1;
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),16,6,16,6,120,24);
				// MODIF A3.3
				//PutImage24BPPExt(20,6,&warning_charge);
				//sprintf(&text[0], "DFMOD");
				sprintf(&text[0], "     ");
				
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),160,(176-module_HEIGHT),160,(176-module_HEIGHT),200,module_HEIGHT+14);
				for (i=0; i<ModNumber; i++)
				{   
					// MODIF A3.3
					if ((ModVFuse (i) != 0)
						|| (LLCWarn(i) != 0)) 
					{
						PutImage24BPPExt(20,6,&warning_charge);
						sprintf(&text[0], "DFMOD");	
					}
					
                    /*if (ModDefTempStop(i) != 0)
						sprintf(&text[0], "TH"); */        
					if ((CanErr(i) != 0) && ((PMod & (1 << i)) == 0))						
                        PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 1 * i) - ((module_DF_WIDTH * 1 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 35,&module);
					else if (/*(ModPwmMax(i) != 0)			// MODIF A3.3
							|| (ModDefTempStop(i) != 0)
                            ||*/ (ModVFuse (i) != 0)
							/*|| (ModOtherDef(i) != 0)*/
                            //|| (ModDefProt(i) != 0)     // MODIF R2.7
                            || (LLCWarn(i) != 0)        // MODIF VLMFB
							/*|| (LLC[i].CntDefTemp > MAX_MOD_DEF_TEMP)
							|| (LLC[i].CntDefCur > MAX_MOD_DEF_CUR)*/
							|| ((CanErr(i) != 0) && ((PMod & (1 << i)) != 0) && (i < ModNumber) && (StateIdle ==0)))
                        PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 1 * i) - ((module_DF_WIDTH * 1 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 35,&module_DF);
					else                  
                        PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 1 * i) - ((module_DF_WIDTH * 1 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 35,&module_vide);
				}
				xpos = 50;
				ypos = 6;
				SetFont((void *) &Font_Inst);
				SetColor(WHITE);
				SetBackColor(RGBConvert(0, 0, 0));
				OutTextXY(xpos,ypos,&text[0]);
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

			SetFont((void *) &Font_SvBold);
			SetColor(WHITE);
			SetBackColor(RGBConvert(0, 0, 61));
			ypos = 192;
			xpos = GetMaxX()/2;

			/* Display capacity  scroll with BatTemp (Menu or WiIQ */
            if (((State.TimerSec % 4) / 2) == 0)
            {
                Display.ICapacityShow = 1;
                if (((IQWIIQLink != 0) && (IQData.BatCap != 0)) || (Memo.CapAutoManu == 1))
				{
				    sprintf(&text[0], "C=%uAh", Memo.BatCap);
				}
				else if (Menu.CapAutoManu != 0)
				    sprintf(&text[0], "C=%uAh", Menu.BatCap);
				else
				    sprintf(&text[0], "C=AUTO");       
            }	
            if (((State.TimerSec % 4) / 2) == 1)
            {
                Display.ICapacityShow = 1;
                if (Menu.Area == 0)
                {
                	if (BmsState != 0)
                		sprintf(&text[0], "%+2.2d'C", (Bms.Temp/10));
                	else if (IQWIIQLink != 0)
                        sprintf(&text[0], "%+2.2d'C", IQData.BatTemp);
                    else
				        sprintf(&text[0], "%+2.2d'C", Menu.BatTemp);
                }
				else
                {
					if (BmsState != 0)
						sprintf(&text[0], "%+2.2d'F",((sint)(Bms.Temp/10) * 9) / 5 + 32 );
					else if (IQWIIQLink != 0)
                        sprintf(&text[0], "%+2.2d'F",((sint)IQData.BatTemp * 9) / 5 + 32 );      
                    else
				        sprintf(&text[0], "%+2.2d'F",((sint)Menu.BatTemp * 9) / 5 + 32 );      
                }
            }    
            if (Display.ICapacityShow == 1)
            {
                CopyPageWindow(MainScrNb,GetDrawBufferAddress(),190,240,190,240,100,32);
				SetColor(WHITE);
				SetFont((void *) &Font_Menu);
				SetBackColor(RGBConvert(0, 0, 0));
                TWidth = GetTextWidth(&text[0],(void *) &Font_Menu);
				xpos = GetMaxX()/2 - TWidth/2;
				OutTextXY(xpos,250,&text[0]);
				Display.ICapacityShow = 0;    
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

			}
			else if ((IQWIIQLink == 0))  /* No WiIQ link */
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
				}
			}
			else
			{
				/* Soc Management */
				BatterySocManager();

				/* Soc Management */
				ChargeTimeManager();

				sprintf(&text[0], "%14.14s", &IQData.BatSN[0]);
				xpos = (GetMaxX()/2)	- (GetTextWidth(&text[0],(void *) &Font_SvBold)/2 + 12);

				SetFont((void *) &Font_SvBold);
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
			CopyPageWindow(MainScrNb,GetDrawBufferAddress(),164,90,164,90,264,GetTextHeight((void *) &Font_UniBold));

			/* Update text */
			SetFont((void *) &Font_UniBold);
			SetColor(WHITE);
			SetBackColor(RGBConvert(0, 0, 60));
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
			ypos = 90;
			OutTextXY(xpos,ypos,&text[0]);

			Display.UpdateCurrent = 0;
			Display.Update = 1;
            
            DisplayHostMessage();
		}
    
}


/**
 * @brief  Display available data with THEME B
 * @param  None
 * @retval None
 */
void Display_avail_themeb (void)
{
        char text[50];
		uchar i;
		
        uint16_t TWidth,xpos,ypos;
        
        if (Display.UpdateDateTime == 1)
		{
			/* Update Date/Time */
			UpdateDateTime();
            
			/* Icon management */
			IconManagement();

			/* Time Management */
			ChargeTimeManager();

            SetFont((void *) &Font_UniBold);
            SetColor(WHITE);
	        SetBackColor(RGBConvert(0, 61, 1));
        
            sprintf(&text[0], "%s", &MsgThemeB[8][LANG][0]);    // "       "
		    TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);
		    xpos = GetMaxX()/2 - TWidth/2;
		    ypos = 50;
		    OutTextXY(xpos,ypos,&text[0]);
        
            sprintf(&text[0], "%s", &MsgThemeB[9][LANG][0]);    // CHARGER
		    TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);
		    xpos = GetMaxX()/2 - TWidth/2;
		    ypos = 50 + GetTextHeight((void *) &Font_UniBold);
            OutTextXY(xpos,ypos,&text[0]);
        
            sprintf(&text[0], "%uAh", DataLcd.ProfileAh); //Memo.ChgAh);
		    TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);
		    xpos = GetMaxX()/2 - TWidth/2;
		    ypos = 50 + 2 * GetTextHeight((void *) &Font_UniBold);
            OutTextXY(xpos,ypos,&text[0]);
            

			/*Both error posible - display in left top corner */
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
			else if (((DFdIdT != 0) || /*(DFtimeSecu != 0) ||*/ (DFDisDF5 != 0) || (DFotherBat != 0)) && ((State.TimerSec % 6) == 3))
			{ 
                // MODIF R2.7 : no DF5 on display in Nexsys
                if ( (DFotherBat != 0) && ((ChgData.Profile == XFC) || (ChgData.Profile == TPPL) || (ChgData.Profile == NXBLC)) )
                {
                    // No DF5 display
                }
                else
                {
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
	        else if (((DFdIdT != 0) || /*(DFtimeSecu != 0) ||*/ (DFDisDF5 != 0) || (DFotherBat != 0)) && ((State.TimerSec % 6) == 3))
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
				sprintf(&text[0], "%14.14s", &IQData.BatSN[0]);
				xpos = (GetMaxX()/2)	- (GetTextWidth(&text[0],(void *) &Font_SvBold)/2 + 12);
				ypos = 192;
				SetFont((void *) &Font_SvBold);
				SetColor(WHITE);
				SetBackColor(RGBConvert(0, 255, 1));
				OutTextXY(xpos,ypos,&text[0]);
			}
			
            DisplayHostMessage();
            
			Display.UpdateDateTime = 0;
			Display.Update = 1;
		}

}

/**
 * @brief  Display equal data with THEME B
 * @param  None
 * @retval None
 */
void Display_equal_themeb (void)
{
 
        char text[50];
        uint16_t TWidth,xpos,ypos,i;
        uint  a,b;
           
    	if (Display.UpdateDateTime == 1)
		{
			/* Update Date/Time */
			UpdateDateTime();
            
			/* Icon management */
			IconManagement();

            if (ChargeDisconnect == 0)
            {
                SetColor(WHITE);
                SetFont((void *) &unispace_bd_30);
                sprintf(&text[0], "%s", &MsgThemeB[10][LANG][0]);    // EQUAL
			    TWidth = GetTextWidth(&text[0],(void *) &unispace_bd_30);
			    xpos = GetMaxX()/2 - TWidth/2;
			    ypos = 55;
                OutTextXY(xpos,ypos,&text[0]);

    			if ((((State.TimerSec % 6) / 2) == 0)  && (ChgData.Profile != TEST))            /* DISPLAY VBAT/CELL: 2.25v*/
    			{
                    CopyPageWindow(MainScrNb,GetDrawBufferAddress(),128,42 + 2 * GetTextHeight((void *) &Font_UniBold),128,42 + 2 * GetTextHeight((void *) &Font_UniBold),176,GetTextHeight((void *) &Font_UniBold));
    				Display.ICapacityShow = 0;

    				a = (DataR.VbatCell + 5) / 1000;                /* Integer part */
    				b = ((DataR.VbatCell + 5) / 10) % 100;          /* Decimal part */
    				sprintf(&text[0], " %3u.%02uv", a, b);
    				SetFont((void *) &Font_UniBold);
    				SetColor(WHITE);
    				SetBackColor(RGBConvert(0, 0, 61));
    				TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);
    				xpos = 300 - TWidth;
    				ypos = 42 + 2 * GetTextHeight((void *) &Font_UniBold);
    				OutTextXY(xpos,ypos,&text[0]);
    			}
    			else if ((((State.TimerSec % 6) / 2) == 1)  && (ChgData.Profile != TEST))       /* DISPLAY VBAT: 24.5V */
    			{ 
                    CopyPageWindow(MainScrNb,GetDrawBufferAddress(),128,42 + 2 * GetTextHeight((void *) &Font_UniBold),128,42 + 2 * GetTextHeight((void *) &Font_UniBold),176,GetTextHeight((void *) &Font_UniBold));
    				a = (DataR.Vbat + 5) / 100;                     /* Integer part */
    				b = ((DataR.Vbat + 5) / 10) % 10;               /* Decimal part */
    				sprintf(&text[0], " %3u.%01uV", a, b);
    				SetFont((void *) &Font_UniBold);
    				SetColor(WHITE);
    				SetBackColor(RGBConvert(0, 0, 61));

    				TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);			
    				xpos = 300 - TWidth;
    				ypos = 42 + 2 * GetTextHeight((void *) &Font_UniBold);
    				OutTextXY(xpos,ypos,&text[0]);
    			}
                else if ((((State.TimerSec % 6) / 2) == 2)  && (ChgData.Profile != TEST))       /* DISPLAY VBAT: 24.5V */
    			{ 
                    CopyPageWindow(MainScrNb,GetDrawBufferAddress(),128,42 + 2 * GetTextHeight((void *) &Font_UniBold),128,42 + 2 * GetTextHeight((void *) &Font_UniBold),176,GetTextHeight((void *) &Font_UniBold));
    				sprintf(&text[0], "%s", &MsgThemeB[0][LANG][0]);    // EQUAL
    				SetFont((void *) &Font_UniBold);
    				SetColor(WHITE);
    				SetBackColor(RGBConvert(0, 0, 61));

    				TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);			
    				xpos = 300 - TWidth;
    				ypos = 42 + 2 * GetTextHeight((void *) &Font_UniBold);
    				OutTextXY(xpos,ypos,&text[0]);
    			}
                else if (ChgData.Profile == TEST)
                {
                    sprintf(&text[0], "MOD%1u", (ptrMod + 1));
    				SetFont((void *) &Font_UniBold);
    				SetColor(WHITE);
    				SetBackColor(RGBConvert(0, 0, 61));

    				TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);
    				xpos = 300 - TWidth;
    				ypos = 45 + 2 * GetTextHeight((void *) &Font_UniBold);
    				OutTextXY(xpos,ypos,&text[0]);
                }
            
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
				CopyPageWindow(MainScrNb,GetDrawBufferAddress(),16,6,16,6,120,24);
				// MODIF A3.3
				//PutImage24BPPExt(20,6,&warning_charge);
				//sprintf(&text[0], "DFMOD");
				sprintf(&text[0], "     ");
				
				//CopyPageWindow(MainScrNb,GetDrawBufferAddress(),166,(192-module_HEIGHT),166,(192-module_HEIGHT),200,module_HEIGHT);
                CopyPageWindow(MainScrNb,GetDrawBufferAddress(),160,(176-module_HEIGHT),160,(176-module_HEIGHT),200,module_HEIGHT+14);
				for (i=0; i<ModNumber; i++)
				{
					// MODIF A3.3
					if ((ModVFuse (i) != 0)
						|| (LLCWarn(i) != 0)) 
					{
						PutImage24BPPExt(20,6,&warning_charge);
						sprintf(&text[0], "DFMOD");	
					}
					
					/*if (ModDefTempStop(i) != 0)
						sprintf(&text[0], "TH"); */
						
					if ((CanErr(i) != 0) && ((PMod & (1 << i)) == 0))
                        PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 1 * i) - ((module_DF_WIDTH * 1 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 35,&module);
					else if (/*(ModPwmMax(i) != 0) 
							|| (ModDefTempStop(i) != 0)
                            ||*/ (ModVFuse (i) != 0)
							/*|| (ModOtherDef(i) != 0)*/
                            //|| (ModDefProt(i) != 0)     // MODIF R2.7
                            || (LLCWarn(i) != 0)        // MODIF VLMFB
							/*|| (LLC[i].CntDefTemp > MAX_MOD_DEF_TEMP)
							|| (LLC[i].CntDefCur > MAX_MOD_DEF_CUR)*/
							|| ((CanErr(i) != 0) && ((PMod & (1 << i)) != 0) && (i < ModNumber) && (StateIdle ==0)))
                        PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 1 * i) - ((module_DF_WIDTH * 1 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 35,&module_DF);
					else
                        PutImage24BPPExt((GetMaxX()/2 + module_DF_WIDTH/2 + (module_DF_WIDTH * 1 * i) - ((module_DF_WIDTH * 1 * ModNumber)/2)),GetMaxY()/2 - module_DF_HEIGHT/2 + 35,&module_vide);
				}
				xpos = 50;
				ypos = 6;
				SetFont((void *) &Font_Inst);
				SetColor(WHITE);
				SetBackColor(RGBConvert(0, 0, 0));
				OutTextXY(xpos,ypos,&text[0]);
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
                /* Display capacity */
                if (((State.TimerSec % 4) / 2) == 0)
                {
                    Display.ICapacityShow = 1;
                    if ((IQWIIQLink != 0) || (Memo.CapAutoManu == 1))
    				    sprintf(&text[0], "C=%uAh", Memo.BatCap);
    				else if (Menu.CapAutoManu != 0)
    				    sprintf(&text[0], "C=%uAh", Menu.BatCap);
    				else
    				    sprintf(&text[0], "C=AUTO");       
                }	
                if (((State.TimerSec % 4) / 2) == 1)
                {
                    if (Menu.Area == 0)
                    {
                        if (IQWIIQLink != 0)
                            sprintf(&text[0], "%+2.2d'C", IQData.BatTemp);
                        else
				            sprintf(&text[0], "%+2.2d'C", Menu.BatTemp);
                    }
				    else
                    {
                        if (IQWIIQLink != 0)
                            sprintf(&text[0], "%+2.2d'F",((sint)IQData.BatTemp * 9) / 5 + 32 );      
                        else
				            sprintf(&text[0], "%+2.2d'F",((sint)Menu.BatTemp * 9) / 5 + 32 );      
                    }
                }    
                if (Display.ICapacityShow == 1)
                {
                    CopyPageWindow(MainScrNb,GetDrawBufferAddress(),200,240,200,240,96,32);
    				SetColor(WHITE);
    				SetFont((void *) &Font_SvBold);
    				SetBackColor(RGBConvert(0, 0, 0));
                    TWidth = GetTextWidth(&text[0],(void *) &Font_SvBold);
    				xpos = GetMaxX()/2 - TWidth/2;
    				OutTextXY(xpos,250,&text[0]);
    				Display.ICapacityShow = 0;    
                }

				// WiIQ Serial number
				sprintf(&text[0], "%14.14s", &IQData.BatSN[0]);
				xpos = (GetMaxX()/2)	- (GetTextWidth(&text[0],(void *) &Font_SvBold)/2 + 12);
				ypos = 192;
				SetFont((void *) &Font_SvBold);
				SetColor(WHITE);
				SetBackColor(RGBConvert(0, 255, 1));
				OutTextXY(xpos,ypos,&text[0]);
		                
			}

            DisplayHostMessage();
                
			Display.UpdateDateTime = 0;
			Display.Update = 1;
		}

		if (Display.UpdateCurrent == 1)
		{
			/* Clear previous message */
			CopyPageWindow(MainScrNb,GetDrawBufferAddress(),164,90,164,90,264,GetTextHeight((void *) &Font_UniBold));

			/* Update text */
			SetFont((void *) &Font_UniBold);
			SetColor(WHITE);
			SetBackColor(RGBConvert(0, 0, 60));
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
			ypos = 90;
			OutTextXY(xpos,ypos,&text[0]);

			Display.UpdateCurrent = 0;
			Display.Update = 1;
            
            DisplayHostMessage();
		}
		    
}
