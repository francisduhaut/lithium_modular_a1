
/*******************************************************************************
* DISCLAIMER
*
* 
*
*******************************************************************************/
/* Copyright (C) 2013 Enersys. All rights reserved.   */
/*******************************************************************************
* File Name     : menutft.h
* Version       : 1.00
* Device        : R5F563NB (RX631)
* Tool-Chain    : Renesas RX Standard 1.2.0.0
* H/W Platform  : LIFETECH
* Description   : This Header file contains the Macro Definitions & prototypes  
*                  for the functions used in graphics.c
*******************************************************************************/
/*******************************************************************************
* History       : Sep. 2013  Ver. 1.00 First Release						  */

#ifndef MENUTFT_H
#define	MENUTFT_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct
{
	uint8_t Index;
	uint8_t SubIndex;
	uint8_t Index2;
	uint8_t SubIndex2;
	uint8_t All;
	char Text[24];
}structTextList;

typedef struct
{
	uint16_t CanID;
	uint8_t SubCanID;
}structUpdateList;



extern uint32_t TimeoutMse;
extern uint8_t FrameRecv;
extern uint8_t RecvFrame[8];

// MODIF 2.8
extern structTextList TextList[20];
extern structUpdateList UpdateList[40];
//extern structTextList TextList[9];
//extern structUpdateList UpdateList[18];

extern uint8_t TextListCurrent;
extern uint8_t TextListOffset;
extern uint8_t TextListMax;
extern uint8_t ListGenerated;
extern uint8_t InBootUpdate;

#define SEND 0
#define WAIT 1 
#define RECV 2
#define SUSPEND 3
#define LIST 4


#define SELECTED    0x0001
#define TITLE       0x0002
#define OPTION       0x1000
#define LONGTEXT		0x0004

void DrawSecondEntryText(char *text);
void DrawSecondEntryImage(BYTE position, BYTE prevposition,IMAGE_EXTERNAL *img);
void DrawSecondEntryBackground(void);
void DrawListBackground(void);
void DrawLine2 (BYTE position, BYTE level, char *text,WORD Status);
void DrawLine3 (BYTE position, BYTE level, char *text,WORD Status);
void DrawLineAdd (BYTE position, char *text);
void DrawSlider(WORD NbOfElements,WORD position,BYTE Alignment);
void Putchar(WORD x,WORD y,char *c,uint8_t Select);
void DrawMenuFirstEntryBackground (void);
void DrawMenuFirstEntryPartialSelected (IMAGE_EXTERNAL *img);
void DrawMenuFirstEnrtyPartial(uint8_t pos,uint8_t prevpos,IMAGE_EXTERNAL *img);
void DrawMenuFirstEnrtyText(uint8_t pos, char *ptr, uint8_t color);
void DrawLine (BYTE position, BYTE level, char *text,WORD Status);
void DrawDataInputBackground (void);
void DrawLine2Status (BYTE position, BYTE level, char *text,WORD Status);
void Cursor(WORD x,WORD y,GFX_COLOR Color);
void DrawPasswordPointer (uint8_t ptr);
void DrawPasswordIcons(uint8_t ptr);
void DrawPasswordBackground (void);

void DrawSecondEntryBackgroundList(void);
void DrawSecondEntryBackgroundState(void);

void ResetBootloaderApi(void);
void ExitMsg(char* ptr);
void DrawDisplayList(void);
uint8_t BootloaderApi (void);


#ifdef	__cplusplus
}
#endif

#endif	/* MENUTFT_H */

