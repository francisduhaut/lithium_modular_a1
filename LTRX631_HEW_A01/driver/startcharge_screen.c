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

void CreateStartChargeScreen (void)
{	
	/* Disable PIP */
	PIPState(0);

	/* Create background */
	//CopyPageWindow(MainScrNb,GetDrawBufferAddress(),0,0,0,0,GetMaxX(),GetMaxY());	
	PutImage24BPPExt(0,0,&background_charge_equal);

	CreateScreenHeader();
	UpdateDateTime();

	PutImage24BPPExt((GetMaxX() - timer_WIDTH)/2,90,&timer);

	UpdateStateForceReloadIcons();

	IconManagement();

	Display.UpdateDateTime = 1;
}
