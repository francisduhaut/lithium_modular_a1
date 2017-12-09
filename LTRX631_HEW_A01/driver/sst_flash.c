/*******************************************************************************
 * DISCLAIMER
 *
 *
 *
 *
 *******************************************************************************/
/* Copyright (C) 2013 Enersys. All rights reserved.   */
/*******************************************************************************
 * File Name     : sst_flash.c
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

#include "include.h"

/*******************************************************************************
 * Local Function Prototypes
 *******************************************************************************/


void Delay_us2(const uint32_t usec);
uint8_t WriteIWord (uint32_t Address, uint16_t Data);
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void InitFlash(void)
{
	BSC.CS4MOD.WORD = 0x0301;
	BSC.CS4WCR1.BIT.CSRWAIT = 5;
	BSC.CS4WCR1.BIT.CSWWAIT = 3;
	BSC.CS4WCR1.BIT.CSPRWAIT = 3;
	BSC.CS4WCR1.BIT.CSPWWAIT = 2;
	BSC.CS4WCR2.BIT.CSON = 0;
	BSC.CS4WCR2.BIT.WDON = 1;
	BSC.CS4WCR2.BIT.WRON = 1;
	BSC.CS4WCR2.BIT.RDON = 2;
	BSC.CS4WCR2.BIT.AWAIT = 7;
	BSC.CS4WCR2.BIT.WDOFF = 2;
	BSC.CS4WCR2.BIT.CSWOFF = 1;
	BSC.CS4WCR2.BIT.CSROFF = 1;
	BSC.CS4CR.WORD = 0x0001;
	BSC.CS4REC.WORD = 0;

	BSC.CSRECEN.WORD = 0x0000;
	BSC.BEREN.BYTE = 0x03;
	BSC.BUSPRI.WORD = 0x0000;

	MPC.PFCSE.BIT.CS4E = 0;
	MPC.PFCSS1.BYTE = 0x00;
	MPC.PFAOE0.BYTE = 0xFF;
	MPC.PFAOE1.BYTE = 0x3F;
	MPC.PFBCR0.BYTE = 0x11;
	MPC.PFBCR1.BYTE = 0x00;	

	FCS = 1;
	FCS_DIR = 1;

}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint8_t ReadSecurityID (uint16_t *ptr)
{
	uint16_t Data;
	FCS = 0;
	WriteIWord(0x555,0x00AA);
	WriteIWord(0x2AA,0x0055);
	WriteIWord(0x555,0x0088);
	FCS = 1;
	Delay_us2(1u);
	FCS = 0;
	for (Data =0;Data<16;Data++)
	{
		*ptr++ = ReadWord(Data);	
	}
	FCS = 1;
	Delay_us2(1u);
	FCS = 0;
	WriteIWord(0x555,0x00AA);
	WriteIWord(0x2AA,0x0055);
	WriteIWord(0x555,0x00F0);
	FCS = 1;
	return 0;		
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint8_t ReadCFIInfo(uint16_t *ptr)
{
	uint16_t Data;
	FCS = 0;
	WriteIWord(0x555,0x00AA);
	WriteIWord(0x2AA,0x0055);
	WriteIWord(0x555,0x0098);
	FCS = 1;
	Delay_us2(1u);
	FCS = 0;
	for (Data =0;Data<60;Data++)
	{
		*ptr++ = ReadWord(Data);	
	}
	FCS = 1;
	Delay_us2(1u);
	FCS = 0;
	WriteIWord(0x555,0x00AA);
	WriteIWord(0x2AA,0x0055);
	WriteIWord(0x555,0x00F0);
	FCS = 1;
	return 0;	
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint16_t ReadProductID(void)
{
	uint16_t Data;
	FCS = 0;
	WriteIWord(0x555,0x00AA);
	WriteIWord(0x2AA,0x0055);
	WriteIWord(0x555,0x0090);
	FCS = 1;
	Delay_us2(1u);
	FCS = 0;
	//Data = ReadWord(0x0000);
	Data = ReadWord(0x0001);
	FCS = 1;
	Delay_us2(1u);
	FCS = 0;
	WriteIWord(0x555,0x00AA);
	WriteIWord(0x2AA,0x0055);
	WriteIWord(0x555,0x00F0);
	FCS = 1;
	return Data;
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint16_t ReadWord (uint32_t Address)
{
	uint16_t * dram = (uint16_t *)0x04000000ul;
	uint16_t Data;
	dram += Address;
	Data = *dram;
	return Data;
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint8_t WriteIWord (uint32_t Address, uint16_t Data)
{
	uint16_t * dram = (uint16_t *)0x04000000ul;
	dram += Address; 
	*dram = Data;	
	return 0;
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint8_t WriteWord (uint32_t Address, uint16_t Data)
{
	uint16_t Temp;
	FCS = 0;
	WriteIWord(0x555,0x00AA);
	WriteIWord(0x2AA,0x0055);
	WriteIWord(0x555,0x00A0);
	WriteIWord(Address,Data);
	Delay_us(12u);
	FCS = 1;
	Delay_us(1u);
	FCS = 0;
	Temp = ReadWord(Address);
	FCS = 1;
	if (Temp != Data)
	{
		return 1;
	}
	return 0;
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint8_t ReadWordArray (uint32_t Address,uint16_t *ptr,uint16_t NbofRead)
{
	uint16_t i;
	FCS = 0;
	for (i = 0;i<NbofRead;i++)
	{
		*ptr++ = ReadWord(Address++);
	}
	FCS = 1;
	return 0;	
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint8_t ReadDWordArray (uint32_t Address,uint32_t *ptr,uint16_t NbofRead)
{
	static uint16_t i,j,k;
	static uint32_t m;
	FCS = 0;
	for (i = 0;i<NbofRead;i++)
	{
		j = ReadWord(Address++);
		k = ReadWord(Address++);
		m = ((uint32_t)k<<16) | (uint32_t)j;
		*ptr++ = m;
	}
	FCS = 1;
	return 0;	
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint8_t WriteByteArray (uint32_t Address,uint8_t *ptr,uint16_t NbofWrite)
{
	uint16_t i,j,m;
	uint8_t k,l;

	for (i = 0;i<(NbofWrite / 2);i++)
	{
		k = (*ptr++);
		l= (*ptr++);
		m = (l<<8) | k;	
		j += WriteWord(Address++,m);
	}	
	if (j != 0)
	{
		return 1;
	}
	return 0;		
}	

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint8_t WriteWordArray (uint32_t Address,uint16_t *ptr,uint16_t NbofWrite)
{
	uint16_t i,j = 0;

	for (i = 0;i<NbofWrite;i++)
	{
		j += WriteWord(Address++,*ptr++);
	}	
	if (j != 0)
	{
		return 1;
	}
	return 0;		
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint8_t ChipErase (void)
{
	FCS = 0;
	WriteIWord(0x555,0x00AA);
	WriteIWord(0x2AA,0x0055);
	WriteIWord(0x555,0x0080);
	WriteIWord(0x555,0x00AA);
	WriteIWord(0x2AA,0x0055);
	WriteIWord(0x555,0x0010);
	Delay_us(50500u);
	FCS = 1;		
	return 0;
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint8_t SectorErase (uint32_t Address)
{
	FCS = 0;
	WriteIWord(0x555,0x00AA);
	WriteIWord(0x2AA,0x0055);
	WriteIWord(0x555,0x0080);
	WriteIWord(0x555,0x00AA);
	WriteIWord(0x2AA,0x0055);
	WriteIWord(Address,0x0050);
	FCS = 1;	
	Delay_us2(18500u);
	return 0;	
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void Delay_us2(const uint32_t usec)
{
	/* Declare counter value variable based on usec input and DELAY_TIMING */
	volatile uint32_t counter = usec * DELAY_TIMING_US;

	/* Decrement the counter and wait */
	while (counter--)
	{
		//usb_cpu_WdogRefresh();
	}
}

