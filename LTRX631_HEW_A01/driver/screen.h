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

/*******************************************************************************
* User Includes (Project Level Includes)
*******************************************************************************/
#include "loading_screen.h"
#include "idle_screen.h"
#include "startcharge_screen.h"
#include "maincharge_screen.h"
#include "fault_screen.h"
#include "equal_screen.h"
#include "available_screen.h"

/*****************************************************************************
 * SECTION: SCREEN STATES
 *****************************************************************************/
typedef enum
{   
	CREATE_LOADING = 0,
  DISPLAY_LOADING,    
    
	CREATE_IDLE,
  DISPLAY_IDLE,

  CREATE_STARTCHARGE,
  DISPLAY_STARTCHARGE,

	CREATE_AVAILABLE,
	DISPLAY_AVAIL,

	CREATE_FAULT,
	DISPLAY_FAULT,

	CREATE_MAINCHARGE,
	DISPLAY_MAINCHARGE,

	CREATE_EQUAL,
	DISPLAY_EQUAL,

	
	DISPLAY_MENU,		/*Dummy, nothing to do*/
  NOTHING1,
	NOTHING2,
	
} SCREEN_STATES;

typedef struct
{
    ulong Update : 1;
	ulong UpdateDateTime : 1;
	ulong UpdateCurrent : 1;
	ulong ForceSoCUpdate : 1;
    ulong ForceReloadHeader : 1;
	ulong ICanShow :1;
	ulong IUSBShow :1;
	ulong IManuReg :1;				
	ulong IDongleShow :1;
    ulong IDongleOff :1;
	ulong ILinkShow :1;
	ulong IWaterShow :1;
	ulong IUnBalancedShow :1;
	ulong IOverDisShow :1;
	ulong IBatteryShow :1;
	ulong ICapacityShow :1;
	ulong PipEnabled :1;
	ulong ItempShow :1;
    ulong IEqualShow :1;
    ulong ForceTimeUpdate :1;
    ulong LanOnShow :1;
    ulong LanOffShow :1;
    ulong WifiOnShow :1;
    ulong WifiOffShow :1;
    ulong BFMOnShow :1;
    ulong BFMOffShow :1;
	ulong Unused : 6;	
}DisplayStruct;


#define SCREEN_WAIT_MENU 6
#define SCREEN_CHARGE 5
#define SCREEN_FAULT 7
#define SCREEN_AVAIL 3

#define FAULT_DFC       0
#define FAULT_DF        1
#define FAULT_WATER     2
#define FAULT_TEMP      3
#define FAULT_ID        4
#define FAULT_POWERCUT  5


/*******************************************************************************
* Variables
*******************************************************************************/
extern SCREEN_STATES    screenstate;    // current state of main demo state mashine
extern DisplayStruct Display;
extern uint8_t MainScrNb;
extern uint8_t FaultType;

/******************************************************************************
* Function Prototypes
******************************************************************************/
void UpdateStateInit(void);
void TFT_Chrono(void);
void CreateScreenHeader (void);
void UpdateStateForceReloadIcons(void);
void UpdateDateTime(void);
void IconManagement (void);
void BatterySocManager (void);
void ChargeTimeManager (void);
