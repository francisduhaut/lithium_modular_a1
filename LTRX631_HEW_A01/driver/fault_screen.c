/*******************************************************************************
 * DISCLAIMER
 *
 *
 *
 *******************************************************************************/
/* Copyright (C) 2013 Enersys. All rights reserved.   */
/*******************************************************************************
 * File Name     :
 * Version       : 1.00
 * Device        : R5F563NB (RX631)
 * Tool-Chain    : Renesas RX Standard 1.2.0.0
 * H/W Platform  : LIFETECH
 * Description   :
 *******************************************************************************/
/*******************************************************************************
 * History       : Sep. 2013  Ver. 1.00 First Release
 *******************************************************************************/
/*******************************************************************************
 * User Includes (Project Level Includes)
 *******************************************************************************/
#include "include.h"

extern void IconManagement(void);	

/*******************************************************************************
 * External Variables
 *******************************************************************************/
char FaultMsg[16];


/*******************************************************************************
 * Macro Definitions
 *******************************************************************************/
/******************************************************************************
 * Function Prototypes
 ******************************************************************************/
void CreateFaultScreen (void)
{	
	uint16_t TWidth,xpos; //,ypos;

	/* Disable PIP */	
	PIPState(0);

	//SetActivePage(7); // MODIF PB DF3
	//PutImage24BPPExt(0,0,&background_defaut); // MODIF PB DF3

	SetActivePage(GetDrawBufferAddress());
	// Use Main charge background for ID fault / DF-Current Fault
	if ((FaultType == FAULT_ID) || (FaultType == FAULT_DFC) || (FaultType == FAULT_POWERCUT))
	{
		MainScrNb = SCREEN_CHARGE;
		PutImage24BPPExt(0,0,&background_charge_equal);
	}
	else
		PutImage24BPPExt(0,0,&background_defaut);

	//SetActivePage(GetDrawBufferAddress());
	//CopyPageWindow(MainScrNb,GetDrawBufferAddress(),0,0,0,0,GetMaxX(),GetMaxY());	


	CreateScreenHeader();	

	UpdateDateTime();

	/* Put images */	
	switch (FaultType)
	{
	case FAULT_DF:
		PutImage24BPPExt((GetMaxX()/2 - warning_defaut_WIDTH/2),GetMaxY()/2 - warning_defaut_HEIGHT/2 - 25,&warning_defaut);
		break;

	case FAULT_WATER:
		PutImage24BPPExt((GetMaxX()/2 - water_level_WIDTH/2),GetMaxY()/2 - water_level_HEIGHT/2 - 25,&water_level);
		break;

	case FAULT_TEMP:
		PutImage24BPPExt((GetMaxX()/2 - bat_temp_WIDTH)-5,GetMaxY()/2 - bat_temp_HEIGHT/2 - 25,&bat_temp);
		break;

	case FAULT_DFC:
	case FAULT_ID:
	case FAULT_POWERCUT:
		break;
	}

	if ((FaultType != FAULT_ID) && (FaultType != FAULT_DFC) && (FaultType != FAULT_POWERCUT))
	{
		/* Create text */
		SetFont((void *) &Font_Inst);
		SetColor(WHITE);
		SetBackColor(RGBConvert(255, 0, 0));
		TWidth = GetTextWidth((char *)&FaultMsg,(void *) &Font_Inst);
		xpos = GetMaxX()/2 - TWidth/2;
		OutTextXY(xpos,184,&FaultMsg[0]);
	}
	else
	{
		/* Update text */
		SetFont((void *) &Font_UniBold);
		SetColor(WHITE);
		SetBackColor(RGBConvert(0, 0, 61));
		TWidth = GetTextWidth((char *)&FaultMsg,(void *) &Font_UniBold);
		xpos = GetMaxX()/2 - TWidth/2;
		OutTextXY(xpos,100,&FaultMsg[0]);
	}


	//UpdateDisplayNow();
	UpdateStateForceReloadIcons();
	IconManagement();
	Display.UpdateDateTime = 1;
}
