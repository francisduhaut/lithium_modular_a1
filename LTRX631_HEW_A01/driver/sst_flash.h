/*******************************************************************************
* DISCLAIMER
*
*
*
*
*******************************************************************************/
/* Copyright (C) 2013 Enersys. All rights reserved.   */
/*******************************************************************************
* File Name     : sst_flash.h
* Version       : 1.00
* Device        : R5F563NB (RX631)
* Tool-Chain    : Renesas RX Standard 1.2.0.0
* H/W Platform  : LIFETECH
* Description   : SST Flash Controller Rutine.
*******************************************************************************/
/*******************************************************************************
* History       : Sep. 2013  Ver. 1.00 First Release
*******************************************************************************/

/*******************************************************************************
* User Includes (Project Level Includes)
*******************************************************************************/
//#pragma pack

typedef struct
{
	uint16_t Unused0;
	uint16_t DeviceID;
	uint16_t Unused[14];
	uint16_t QueryStr[3];
	uint32_t PrimaryOEM;
	uint32_t PrimaryExtendedTable;
	uint32_t AlternateOEM;
	uint32_t AlternateExtendedTable;
	uint16_t VddMin;	
	uint16_t VddMax;
	uint16_t VppMin;
	uint16_t VppMax;
	uint16_t WordProgramTypTime;
	uint16_t ProgramBufferTypTime;
	uint16_t SectorEraseTypTime;
	uint16_t ChipEraseTypTime;
	uint16_t WordProgramMaxTime;
	uint16_t ProgramBufferMaxTime;
	uint16_t SectorEraseMaxTime;
	uint16_t ChipEraseMaxTime;	
	uint16_t DeviceSize;
	uint32_t InterfaceDescription;
	uint32_t MaxMultiByteWrite;
	uint16_t NumberOfEraseSBSize;
	uint16_t Unused1[16];
}CFIstruct;

#define FCS  	PORT6.PODR.BIT.B4
#define FCS_DIR PORT6.PDR.BIT.B4

void InitFlash(void);
uint8_t ReadCFIInfo(uint16_t *ptr);
uint16_t ReadProductID(void);
uint16_t ReadWord (uint32_t Address);
uint8_t WriteWord (uint32_t Address, uint16_t Data);
uint8_t ReadWordArray (uint32_t Address,uint16_t *ptr,uint16_t NbofRead);
uint8_t ReadDWordArray (uint32_t Address,uint32_t *ptr,uint16_t NbofRead);
uint8_t WriteByteArray (uint32_t Address,uint8_t *ptr,uint16_t NbofWrite);
uint8_t WriteWordArray (uint32_t Address,uint16_t *ptr,uint16_t NbofWrite);
uint8_t ChipErase (void);
uint8_t SectorErase (uint32_t Address);