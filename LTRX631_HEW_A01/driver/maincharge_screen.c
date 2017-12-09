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

void CreateMainChargeScreen (void)
{	
	/* Disable PIP */	
	PIPState(0);

	/* Create background */
	//CopyPageWindow(MainScrNb,GetDrawBufferAddress(),0,0,0,0,GetMaxX(),GetMaxY());	
	PutImage24BPPExt(0,0,&background_charge_equal);


	CreateScreenHeader();
	UpdateDateTime();

	UpdateStateForceReloadIcons();

	Display.ForceSoCUpdate = 1;
	Display.ForceTimeUpdate = 1;
	Display.UpdateCurrent = 1;
	Display.UpdateDateTime = 1;			

	IconManagement();
}
