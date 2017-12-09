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
/*******************************************************************************
 * External Variables
 *******************************************************************************/
/*******************************************************************************
 * Macro Definitions
 *******************************************************************************/
/******************************************************************************
 * Function Prototypes
 ******************************************************************************/
void CreateAvailableScreen (void)
{
	char text[50];
	uint16_t TWidth,xpos;

	/* Disable PIP */	
	PIPState(0);		

	//CopyPageWindow(MainScrNb,GetDrawBufferAddress(),0,0,0,0,GetMaxX(),GetMaxY());	
	PutImage24BPPExt(0,0,&background_avail);

	CreateScreenHeader();	

	UpdateDateTime();

	/* Put images */
	PutImage24BPPExt(395,45,&jauge_avail);	
	PutImage24BPPExt(20,45,&jauge_time);	

	SetFont((void *) &Font_SvBold);
	SetColor(WHITE);
	SetBackColor(BLACK);
	OutTextXY(435,45,"100%");
	//OutTextXY(447,211,"0%");

	/* Create text */
	SetFont((void *) &Font_UniBold);

	/* Update text */
    if (Menu.Theme == THEMEA)
    {
        SetColor(WHITE);
	    SetBackColor(RGBConvert(0, 61, 1));
        sprintf(&text[0], "%s", &ListWord16_State[4][LANG][0]);
        TWidth = GetTextWidth(&text[0],(void *) &Font_UniBold);
        xpos = GetMaxX()/2 - TWidth/2;
	    //OutTextXY(184,60,(char *)&ListWord16_State[4][LANG][0]);
        OutTextXY(xpos,60,&text[0]);
    }
	
    
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

	Display.UpdateDateTime = 1;	
	Display.ForceTimeUpdate = 1;	
	UpdateStateForceReloadIcons();
}
