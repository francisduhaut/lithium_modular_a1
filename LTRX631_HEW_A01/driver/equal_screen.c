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
void CreateEqualScreen (void)
{
	char text[50];
	uint16_t TWidth,xpos;

	/* Disable PIP */
	PIPState(0);

	/* Create background */
	//CopyPageWindow(MainScrNb,GetDrawBufferAddress(),0,0,0,0,GetMaxX(),GetMaxY());
	PutImage24BPPExt(0,0,&background_charge_equal);

	CreateScreenHeader();
	UpdateDateTime();

	PutImage24BPPExt(400,44,&jauge_charge);

	SetFont((void *) &Font_SvBold);
	SetColor(WHITE);
	SetBackColor(BLACK);
	OutTextXY(435,45,"100%");
	OutTextXY(447,211,"0%");
    
    // MODIF 2.8
    if (Menu.Theme == THEMEA)
    {
	    PutImage24BPPExt(263,158,&equal);
    }

	/* Display capacity */
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

	UpdateStateForceReloadIcons();
	Display.UpdateCurrent = 1;
	Display.UpdateDateTime = 1;
	Display.ForceTimeUpdate = 1;
}
