/*******************************************************************************
* DISCLAIMER
*
* 
*
*******************************************************************************/
/* Copyright (C) 2014 Enersys. All rights reserved.   */
/*******************************************************************************
* File Name     : canbootloader
* Version       : 1.00
* Device        : R5F563NB (RX631)
* Tool-Chain    : Renesas RX Standard 1.2.0.0
* H/W Platform  : LIFETECH
* Description   : CAN based Bootloader for modules
*******************************************************************************/
/*******************************************************************************
* History       : March. 2014  Ver. 1.00 First Release			      */

/* Chrono states */

#define SEND 0
#define WAIT 1 
#define RECV 2
#define SUSPEND 3


extern char MsgError[32]; 

/* Chrono steps */

#define SEARCHDEVICE 6 
#define BOOTCONNECT 0
#define BOOTACTIONSERVICE 1
#define READLINE 2
#define WRITEFLASH 3
#define BOOTDISCONNECT 4 
#define BOOTEND 5

/* Chrono Structs */

typedef struct
{   
	uint16_t ProjectID;
	uint16_t AppVersion;
	uint16_t BootVersion;
	uint16_t uC_PARTNO;
	uint16_t uC_CLASSID;
	uint16_t CRCField;
	uint32_t Length;
	uint16_t Reserved[8];
}AppHeaderStruct;

/* Functions */
uint32_t SendBootCommandConnect(void);
uint32_t SendBootCommandActionService(void);
uint32_t SendBootCommandDisconnect(void);
uint32_t SendBootCommandSetMTA(void);
uint32_t SendBootCommandProgram(uint8_t *ptr,uint8_t bytes);
uint8_t StartBootloaderChronoSearch(uint8_t type);
uint8_t StartBootloaderChrono(uint16_t Input,uint8_t Input2);
void StopBootloaderChrono(void);
uint8_t GetBootloaderChronoStatus(void);
uint8_t GetFileHeaderInformation (uint8_t *Ptr,uint8_t *Ptr2);
void GenerateUpdateList (uint8_t Nb);
uint8_t BootloaderChrono (void);
