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
extern void usb_cpu_WdogRefresh(void);

void CreateLoadingScreen(void)
{
	char text[16];
	uint16_t TWidth,THeight,xpos,ypos;
	uint32_t temp1[20];

	/* Set font type */
	SetFont((void *) &Font_UniBold);
	SetColor(BLACK);	
	SetBackColor(BLACK);
	Bar(0,0,480,270);
	ReadWordArray((GRC_CRC32_EXTERNAL_ADDR)>>1,&temp1[0],4);

	if (temp1[1] != GRC_CRC32_EXTERNAL_MARKER)
	{
		xpos = (GetMaxX()/2-120);
		ypos = (GetMaxY()/2);
		SetColor(WHITE);
		sprintf(&text[0], "NO IMAGES");
		while(!OutTextXY(xpos,ypos,&text[0]));
		SetActivePage(0);
		SetBacklight(1);
		DisplayBrightness(100);
		while (1)
		{
			usb_cpu_WdogRefresh();
		}
	}

	/* Get text parameters */
	TWidth = GetTextWidth((char*)&ListWord16_State[1][LANG][0],(void *) &Font_UniBold);
	THeight = GetTextHeight((void *) &Font_UniBold);
	SetColor(WHITE);
	xpos = (GetMaxX()/2) - (TWidth/2);
	ypos = (GetMaxY()/2) - (THeight/2);	
	while(!OutTextXY(xpos,ypos,(char*)&ListWord16_State[1][LANG][0]));
	if (TIME_INIT >= State.TimerSec)
		sprintf(&text[0], "%1.1lus", (TIME_INIT - State.TimerSec));
	while(!OutTextXY((GetMaxX()/2)-20,GetMaxY()/2+15,&text[0]));

	PutImage24BPPExt((GetMaxX() - timer_WIDTH)/2,50,&loading);

	/* Make copy background for text and images on next available buffer (4) */
	SetActivePage(4);

	PutImage24BPPExt(88,160,&cable);

	SetActivePage(0);
}
