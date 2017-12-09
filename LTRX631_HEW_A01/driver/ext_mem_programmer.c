/*******************************************************************************
 * DISCLAIMER
 *
 *
 *
 *
 *******************************************************************************/
/* Copyright (C) 2013 Enersys. All rights reserved.   */
/*******************************************************************************
 * File Name     : ext_mem_programmer.c
 * Version       : 1.00
 * Device        : R5F563NB (RX631)
 * Tool-Chain    : Renesas RX Standard 1.2.0.0
 * H/W Platform  : LIFETECH
 * Description   : Program external memory with hex file input
 *
 *******************************************************************************/

#include "include.h"

extern void USB_Chrono (void);
extern void usb_cpu_WdogRefresh(void);

/******************************************************************************
Global Variables
 *******************************************************************************/
FIL     myFile;
FATFS       usb_gFatfs_ext;
uint8_t myData[512];
size_t numBytes;
uint16_t pointer = 0;
uint16_t readBytes;

uint8_t asciiBuffer[1024];
uint8_t asciiRec[200];
uint8_t hexRec[100];

T_REC record;

volatile uint32_t temp1[4];
volatile uint32_t MarkerAddress = 0;
volatile uint32_t Marker = 0;

volatile FILINFO Fileinfo;

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint8_t ProgramExternalImages(void)
{
	volatile uint16_t i;

	if (UsbDisk == 0)
	{
		return 255;
	}

	// Open the hex file
	// Make sure the file is present..
	if (R_tfat_f_open(&myFile, "external.hex", (TFAT_FA_OPEN_EXISTING | TFAT_FA_READ)) != TFAT_FR_OK)
	{
		return 255;
	}

	//Checking marker
	R_tfat_f_stat("external.hex",&Fileinfo);
	R_tfat_f_lseek(&myFile,Fileinfo.fsize - 256);

	if (R_tfat_f_read (&myFile,&asciiBuffer[pointer], 256, &readBytes) != TFAT_FR_OK)
	{
		return 255;
	}

	for (i = 0; i < 256;i++)
	{
		if (asciiBuffer[i] == ':') 
		{   
			ConvertAsciiToHex(&asciiBuffer[i+1],hexRec);

			if (hexRec[0] == 2)
			{
				MarkerAddress = hexRec[4] << 24 | hexRec[5] << 16;
			}
			else if (hexRec[0] == 8)
			{
				MarkerAddress |= hexRec[1] << 8 | hexRec[2];
				Marker = hexRec[11] << 24 | hexRec[10] << 16 | hexRec[9] << 8 | hexRec[8];

				MarkerAddress/=2;

				ReadWordArray((MarkerAddress),&temp1[0],4);

				if (temp1[1] == Marker)
				{
					return 253;	//image present
				}
				break; //start upload		
			}				
		}
	}	

	R_tfat_f_lseek(&myFile, 0);


	// Erase Flash (Block Erase the program Flash)
	ChipErase();
	// Initialize the state-machine to read the records.
	record.status = REC_NOT_FOUND;

	while(1)
	{
		// Call USB tasks.
		USB_Chrono();

		usb_cpu_WdogRefresh();

		// Blink LED

		// For a faster read, read 512 bytes at a time and buffer it.
		if (R_tfat_f_read (&myFile,&asciiBuffer[pointer], 512, &readBytes) != TFAT_FR_OK)
		{
			return 255;
		}

		if(readBytes == 0)
		{
			// Nothing to read. Come out of this loop
			// break;
			R_tfat_f_close (&myFile); 
			return 0;
		}

		for(i = 0; i < (readBytes + pointer); i ++)
		{
			// This state machine seperates-out the valid hex records from the read 512 bytes.
			switch(record.status)
			{
			case REC_FLASHED:
			case REC_NOT_FOUND:
				if(asciiBuffer[i] == ':')
				{
					// We have a record found in the 512 bytes of data in the buffer.
					record.start = &asciiBuffer[i];
					record.len = 0;
					record.status = REC_FOUND_BUT_NOT_FLASHED;
				}
				break;
			case REC_FOUND_BUT_NOT_FLASHED:
				if((asciiBuffer[i] == 0x0A) || (asciiBuffer[i] == 0xFF))
				{
					// We have got a complete record. (0x0A is new line feed and 0xFF is End of file)
					// Start the hex conversion from element
					// 1. This will discard the ':' which is
					// the start of the hex record.
					ConvertAsciiToHex(&record.start[1],hexRec);
					if (WriteHexRecord2Flash(hexRec) == 1)
					{
						return 0;
					}
					record.status = REC_FLASHED;
				}
				break;
			}
			// Move to next byte in the buffer.
			record.len ++;
		}

		if(record.status == REC_FOUND_BUT_NOT_FLASHED)
		{
			// We still have a half read record in the buffer. The next half part of the record is read
			// when we read 512 bytes of data from the next file read.
			memcpy(asciiBuffer, record.start, record.len);
			pointer = record.len;
			record.status = REC_NOT_FOUND;
		}
		else
		{
			pointer = 0;
		}
	}//while(1)

	return 0;
}

/********************************************************************
 * Function: 	ConvertAsciiToHex()
 * Precondition:
 * Input: 		Ascii buffer and hex buffer.
 * Output:
 * Side Effects:	No return from here.
 * Overview: 	Converts ASCII to Hex.
 * Note:		 	None.
 ********************************************************************/
void ConvertAsciiToHex(uint8_t* asciiRec, uint8_t* hexRec)
{
	uint8_t i = 0;
	uint8_t k = 0;
	uint8_t hex;


	while((asciiRec[i] >= 0x30) && (asciiRec[i] <= 0x66))
	{
		// Check if the ascci values are in alpha numeric range.

		if(asciiRec[i] < 0x3A)
		{
			// Numerical reperesentation in ASCII found.
			hex = asciiRec[i] & 0x0F;
		}
		else
		{
			// Alphabetical value.
			hex = 0x09 + (asciiRec[i] & 0x0F);
		}

		// Following logic converts 2 bytes of ASCII to 1 byte of hex.
		k = i%2;

		if(k)
		{
			hexRec[i/2] |= hex;

		}
		else
		{
			hexRec[i/2] = (hex << 4) & 0xF0;
		}
		i++;
	}

}

/********************************************************************
 * Function: 	WriteHexRecord2Flash()
 * Precondition:
 * Input: 		None.
 * Output:
 * Side Effects:	No return from here.
 * Overview: 	Writes Hex Records to Flash.
 * Note:		 	None.
 ********************************************************************/
int WriteHexRecord2Flash(uint8_t* HexRecord)
{
	static T_HEX_RECORD HexRecordSt;
	uint8_t Checksum = 0;
	uint8_t i;
	//	UINT WrData;
	//	UINT RdData;
	//void* ProgAddress;
	uint16_t result;

	HexRecordSt.RecDataLen = HexRecord[0];
	HexRecordSt.RecType = HexRecord[3];
	HexRecordSt.Data = &HexRecord[4];

	// Hex Record checksum check.
	for(i = 0; i < HexRecordSt.RecDataLen + 5; i++)
	{
		Checksum += HexRecord[i];
	}

	if(Checksum != 0)
	{
		//Error. Hex record Checksum mismatch.
		//Indicate Error by switching ON all LEDs.
		//Error();
		// Do not proceed further.
		//while(1);
		return -1;
	}
	else
	{
		// Hex record checksum OK.
		switch(HexRecordSt.RecType)
		{
		case DATA_RECORD:  //Record Type 00, data record.
			HexRecordSt.Address.byte.MB = 0;
			HexRecordSt.Address.byte.UB = 0;
			HexRecordSt.Address.byte.HB = HexRecord[1];
			HexRecordSt.Address.byte.LB = HexRecord[2];

			// Derive the address.
			HexRecordSt.Address.Val = HexRecordSt.Address.Val + HexRecordSt.ExtLinAddress.Val + HexRecordSt.ExtSegAddress.Val;

			while(HexRecordSt.RecDataLen) // Loop till all bytes are done.
			{
				result = WriteByteArray(HexRecordSt.Address.Val/2,HexRecordSt.Data,HexRecordSt.RecDataLen);
				HexRecordSt.RecDataLen = 0;

				if(result != 0)
				{
					return -1;
				}
			}
			break;

		case EXT_SEG_ADRS_RECORD:  // Record Type 02, defines 4th to 19th bits of the data address.
			HexRecordSt.ExtSegAddress.byte.MB = 0;
			HexRecordSt.ExtSegAddress.byte.UB = HexRecordSt.Data[0];
			HexRecordSt.ExtSegAddress.byte.HB = HexRecordSt.Data[1];
			HexRecordSt.ExtSegAddress.byte.LB = 0;
			// Reset linear address.
			HexRecordSt.ExtLinAddress.Val = 0;
			break;

		case EXT_LIN_ADRS_RECORD:   // Record Type 04, defines 16th to 31st bits of the data address.
			HexRecordSt.ExtLinAddress.byte.MB = HexRecordSt.Data[0];
			HexRecordSt.ExtLinAddress.byte.UB = HexRecordSt.Data[1];
			HexRecordSt.ExtLinAddress.byte.HB = 0;
			HexRecordSt.ExtLinAddress.byte.LB = 0;
			// Reset segment address.
			HexRecordSt.ExtSegAddress.Val = 0;
			break;

		case END_OF_FILE_RECORD:  //Record Type 01, defines the end of file record.
			HexRecordSt.ExtSegAddress.Val = 0;
			HexRecordSt.ExtLinAddress.Val = 0;
			// Disable any interrupts here before jumping to the application.
			return 1;
			break;

		default:
			HexRecordSt.ExtSegAddress.Val = 0;
			HexRecordSt.ExtLinAddress.Val = 0;
			break;
		}
	}
	return 0;
}

/*********************End of File************************************/
