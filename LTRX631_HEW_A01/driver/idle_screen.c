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
void IconManagement(void);

void CreateIdleScreen (void)
{
	//CopyPageWindow(MainScrNb,GetDrawBufferAddress(),0,0,0,0,GetMaxX(),GetMaxY());	
	PutImage24BPPExt(0,0,&background_wait_menu_password);

	CreateScreenHeader();	
	UpdateDateTime();

    // MODIF 2.8
    if (Menu.Theme == THEMEA)
    {
    
	/* Put images */
#ifdef ENERSYS_EU
	    if (Menu.XFCOffOn != 0)
	        PutImage24BPPExt((GetMaxX()/2 - nexsys_WIDTH/2),GetMaxY()/2 - nexsys_HEIGHT/2,&nexsys);
        else
            PutImage24BPPExt((GetMaxX()/2 - lifeIQModular_WIDTH/2),GetMaxY()/2 - lifeIQModular_HEIGHT/2,&lifeIQModular);
#endif
#ifdef ENERSYS_US
	   if (Menu.XFCOffOn != 0)
	        PutImage24BPPExt((GetMaxX()/2 - nexsys_WIDTH/2),GetMaxY()/2 - nexsys_HEIGHT/2,&nexsys);
        else
            PutImage24BPPExt((GetMaxX()/2 - enforcerimpaq_WIDTH/2),GetMaxY()/2 - enforcerimpaq_HEIGHT/2,&enforcerimpaq);    
#endif
#ifdef HAWKER_US
	   PutImage24BPPExt((GetMaxX()/2 - lifetechmod1c_WIDTH/2),GetMaxY()/2 - lifetechmod1c_HEIGHT/2,&lifetechmod1c);
#endif

    }
    
	/* Create Animation (default battery unconnected) */
	Display.PipEnabled = 0;

	UpdateStateForceReloadIcons();

	IconManagement();

	Display.UpdateDateTime = 1;	
	Display.Update = 1;
}


void DisplayNexSys (void)
{
    uint16_t TWidth,THeight,xpos,ypos;
    char NexsysText[] = "NexSys+";
    
    SetColor(WHITE);
	SetBackColor(BLACK);
	SetFont((void *) &Font_UniBold);   
    TWidth = GetTextWidth((char*)&NexsysText[0],(void *) &Font_UniBold);
	THeight = GetTextHeight((void *) &Font_UniBold);
    
    xpos = (GetMaxX()/2) - (TWidth/2);
	ypos = (GetMaxY()/2) - (THeight/2);
	while(!OutTextXY(xpos,ypos,(char*)&NexsysText[0]));
}
