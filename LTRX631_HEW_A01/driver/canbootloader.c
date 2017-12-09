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


#include "include.h"

FIL     myFile2;
size_t numBytes2;
uint16_t BufferPointer = 0;
uint16_t SearchPointer = 0;
uint16_t readBytes2;

uint8_t asciiBuffer2[512];

uint8_t asciiRec2[200];
uint8_t hexRec2[100];

FILINFO Fileinfo2;
uint32_t ReadBytes = 0;

T_REC record2;

uint8_t BootloaderChronoState = SUSPEND;
uint8_t BootloaderChronoStep = 3;
uint8_t RetryCount = 0;
uint8_t Percent = 0;
char MsgError[32]; 
uint16_t CanID;
uint8_t CanSubID;

uint32_t FlashAddress = 0;

uint32_t 		ap_status2;
can_frame_t     can_dataframe2[8];

uint8_t UpdateMTA = 0;
uint8_t WritePointer = 0;

uint32_t LastMTA = 0;
uint8_t ReadPointer = 0;
uint8_t NbOfbytes = 0;

uint8_t SearchType = 0;
uint8_t Counter = 0;
uint8_t CounterStep = 0;
uint8_t CounterSubID[5];	//Maximum 4 + Ending

uint16_t Timeout = 1500;

// MODIF 2.8 : manague module pos 8
extern uchar id_sa_36;

uint32_t SendBootCommandSearch(uint8_t ID, uint8_t SubID)
{
	can_dataframe2[0].id = 0x18000000 + (0xE0 << 16) + ((((ID+1) << 2) | SubID) << 8) + 0;
	can_dataframe2[0].dlc = 8;
	can_dataframe2[0].data[0] = 0x01;	//Command
	can_dataframe2[0].data[1] = 0x00;	//msg counter
	can_dataframe2[0].data[2] = (((ID+1) << 2) | SubID);	//Address
	can_dataframe2[0].data[3] = 0x00;	//Address
	can_dataframe2[0].data[4] = 0x00;
	can_dataframe2[0].data[5] = 0x00;
	can_dataframe2[0].data[6] = 0x00;
	can_dataframe2[0].data[7] = 0x00;
	TimeoutMse = 0;
	FrameRecv = 0;	
	return R_CAN_TxSet(CH_0, (CANBOX_0_RAMRW_TX + 0), &can_dataframe2[0], DATA_FRAME);			
}


uint32_t SendBootCommandConnect(void)
{
	can_dataframe2[0].id = 0x18000000 + (0xE0 << 16) + ((((CanID+1) << 2) | CanSubID) << 8) + 0;
	can_dataframe2[0].dlc = 8;
	can_dataframe2[0].data[0] = 0x01;	//Command
	can_dataframe2[0].data[1] = 0x00;	//msg counter
	can_dataframe2[0].data[2] = (((CanID+1) << 2) | CanSubID);	//Address
	can_dataframe2[0].data[3] = 0x00;	//Address
	can_dataframe2[0].data[4] = 0x00;
	can_dataframe2[0].data[5] = 0x00;
	can_dataframe2[0].data[6] = 0x00;
	can_dataframe2[0].data[7] = 0x00;
	TimeoutMse = 0;
	FrameRecv = 0;	
	return R_CAN_TxSet(CH_0, (CANBOX_0_RAMRW_TX + 0), &can_dataframe2[0], DATA_FRAME);			
}


uint32_t SendBootCommandActionService(void)
{
	can_dataframe2[0].id = 0x18000000 + (0xE0 << 16) + ((((CanID+1) << 2) | CanSubID) << 8) + 0;
	can_dataframe2[0].dlc = 8;
	can_dataframe2[0].data[0] = 0x21;	//Command
	can_dataframe2[0].data[1] = 0x00;	
	can_dataframe2[0].data[2] = 0x00;
	can_dataframe2[0].data[3] = 0x00;	
	can_dataframe2[0].data[4] = 0x00;
	can_dataframe2[0].data[5] = 0x00;
	can_dataframe2[0].data[6] = 0x00;
	can_dataframe2[0].data[7] = 0x00;
	TimeoutMse = 0;
	FrameRecv = 0;	
	return R_CAN_TxSet(CH_0, (CANBOX_0_RAMRW_TX + 0), &can_dataframe2[0], DATA_FRAME);		
}

uint32_t SendBootCommandDisconnect(void)
{
	can_dataframe2[0].id = 0x18000000 + (0xE0 << 16) + ((((CanID+1) << 2) | CanSubID) << 8) + 0;
	can_dataframe2[0].dlc = 8;
	can_dataframe2[0].data[0] = 0x07;	//Command
	can_dataframe2[0].data[1] = 0x00;	//msg counter
	can_dataframe2[0].data[2] = 0x01;	//final disconnect	
	can_dataframe2[0].data[3] = 0x00;	
	can_dataframe2[0].data[4] = (((CanID+1) << 2) | CanSubID);	//Address
	can_dataframe2[0].data[5] = 0x00;//Address
	can_dataframe2[0].data[6] = 0x00;
	can_dataframe2[0].data[7] = 0x00;
	TimeoutMse = 0;
	FrameRecv = 0;	
	return R_CAN_TxSet(CH_0, (CANBOX_0_RAMRW_TX + 0), &can_dataframe2[0], DATA_FRAME);	
	
}

uint32_t SendBootCommandSetMTA(void)
{
    // MODIF 2.8 : the module answer with address 32 instead of 36 in V1.68 after canboot..??? Bug ???
    if (CanID ==8)
        CanID = 7;
        
	can_dataframe2[0].id = 0x18000000 + (0xE0 << 16) + ((((CanID+1) << 2) | CanSubID) << 8) + 0;
	can_dataframe2[0].dlc = 8;
	can_dataframe2[0].data[0] = 0x02;	//Command
	can_dataframe2[0].data[1] = 0x00;	
	can_dataframe2[0].data[2] = 0x00;
	can_dataframe2[0].data[3] = 0x00;	
	can_dataframe2[0].data[4] = (FlashAddress>>24)&0xFF;
	can_dataframe2[0].data[5] = (FlashAddress>>16)&0xFF;
	can_dataframe2[0].data[6] = (FlashAddress>>8)&0xFF;
	can_dataframe2[0].data[7] = (FlashAddress)&0xFF;
	TimeoutMse = 0;
	FrameRecv = 0;	
	return R_CAN_TxSet(CH_0, (CANBOX_0_RAMRW_TX + 0), &can_dataframe2[0], DATA_FRAME);		
}

uint32_t SendBootCommandProgram(uint8_t *ptr,uint8_t bytes)
{
	can_dataframe2[0].id = 0x18000000 + (0xE0 << 16) + ((((CanID+1) << 2) | CanSubID) << 8) + 0;
	can_dataframe2[0].dlc = 8;
	can_dataframe2[0].data[0] = 0x18;	//Command
	can_dataframe2[0].data[1] = 0x00;	
	can_dataframe2[0].data[2] = bytes;
	can_dataframe2[0].data[3] = *ptr++;	
	can_dataframe2[0].data[4] =	*ptr++;
	can_dataframe2[0].data[5] = *ptr++;
	can_dataframe2[0].data[6] = *ptr++;		//can be overlap memory, but its only read, and values can be?
	can_dataframe2[0].data[7] = *ptr++;
	TimeoutMse = 0;
	FrameRecv = 0;	
	return R_CAN_TxSet(CH_0, (CANBOX_0_RAMRW_TX + 0), &can_dataframe2[0], DATA_FRAME);	
	
	
}

uint8_t StartBootloaderChronoSearch(uint8_t type)
{
	uint8_t i;
	
	Counter = 0;			//Indicate the current base address for search
	CounterStep = 0;		//Indicate the current sub address
	
	SearchType = type;
	
	if (Menu.ChgType == 0)
	{
		/* 1 Phase */
		CounterSubID[0] = 0x01;   	//LLC Subaddress
		CounterSubID[1] = 0xFF; 	//Ending
	}
	else
	{
		/* 3 Phase */
		switch (SearchType)
		{
			case 2:
					CounterSubID[0] = 0x00;   	//PRC Subaddress
					CounterSubID[1] = 0xFF; 	//Ending			
					break;
			case 3:
					CounterSubID[0] = 0x02;   	//PFC Subaddress
					CounterSubID[1] = 0xFF; 	//Ending
					break;			
			case 4:
					CounterSubID[0] = 0x00;   	//PRC Subaddress
					CounterSubID[1] = 0x02;   	//PFC Subaddress
					CounterSubID[2] = 0xFF; 	//Ending
					break;			
		}
	}
	
	Timeout = 50;
	
	for (i = 0;i<9;i++)
	{
		TextList[i].Index = 0xFE;		    //Invalid
		TextList[i].SubIndex = 0xFE;		//Invalid
		TextList[i].Index2 = 0xFE;		    //Invalid
		TextList[i].SubIndex2 = 0xFE;		//Invalid
		TextList[i].All = 0x00;			    //No all update
		TextList[i].Text[0] = 0x00;		    //Empty
	}
	
	TextListMax = 0;
	BootloaderChronoStep = SEARCHDEVICE;
	BootloaderChronoState = SEND;
	return 0;
}



uint8_t StartBootloaderChrono(uint16_t Input,uint8_t Input2)
{
	BootloaderChronoStep = 0;
	BootloaderChronoState = SEND;
	// MODIF 2.8 : manage address 36 with module 8 and V168
    if ((id_sa_36 == 1) && (Input == 7))
        CanID = Input + 1;
    else  
	    CanID = Input;
    
	CanSubID = Input2;
	Percent = 0;
	Timeout = 1500;
	SearchPointer = 0;
	
	/* Decide what we will update */
	if (CanSubID == 1)
	{
		/*LLC update*/
		
		if (R_tfat_f_stat("LLC631.hex",&Fileinfo2) == TFAT_FR_OK)
		{
			R_tfat_f_open(&myFile2, "LLC631.hex", (TFAT_FA_OPEN_EXISTING | TFAT_FA_READ));
		}
		else
		{
			/*File missing, no update*/
			return 255;	
		}
		
	}
	else if (CanSubID == 0)
	{
		/*PRC (7kW control card) update*/
		
		if (R_tfat_f_stat("PRC631.hex",&Fileinfo2) == TFAT_FR_OK)
		{
			R_tfat_f_open(&myFile2, "PRC631.hex", (TFAT_FA_OPEN_EXISTING | TFAT_FA_READ));				
		}
		else
		{
			/*File missing, no update*/
			return 255;	
		}
			
	}
	else if (CanSubID == 2)
	{
		/*Vienna update*/
		
		if (R_tfat_f_stat("PFC631.hex",&Fileinfo2) == TFAT_FR_OK)
		{
			R_tfat_f_open(&myFile2, "PFC631.hex", (TFAT_FA_OPEN_EXISTING | TFAT_FA_READ));				
		}
		else
		{
			/*File missing, no update*/
			return 255;	
		}
	}
	record2.status = REC_NOT_FOUND;
	ReadBytes = 0;
	FlashAddress = 0;
	return 0;
}

void StopBootloaderChrono(void)
{
	BootloaderChronoState = SUSPEND;
	BootloaderChronoStep = 3;
	//Percent = 0;
}

uint8_t GetBootloaderChronoStatus(void)
{
	return Percent;	
}


uint8_t SearchHeaderContent(uint8_t *Out, uint8_t LineNb)
{
	uint8_t i = 0,j,k;
	
	j = 0;
	k = 0;
	for(i = 0;i < 250;i++)	
	{
		if ((k == 0) && (asciiBuffer2[i] == ':'))
		{
			record2.start = &asciiBuffer2[i];
			k = 1;
		}
		else if ((k == 1) && (asciiBuffer2[i] == 0x0A))
		{
			//if (((Menu.ChgType == 0) && (j == 1)) || ((Menu.ChgType != 0) && (j == 0)))	//Linecount
			if (j == LineNb)
			{	                
				ConvertAsciiToHex(&record2.start[1],hexRec2);
				for (k = 0;k<32;k++)
				{
					*Out++ = hexRec2[k+5];
					*Out++ = hexRec2[k+4];
					k++;
				}
				break;     					
			}
			j++;
			k = 0;
		}
	}
	return 0;
}


uint8_t GetFileHeaderInformation (uint8_t *Ptr,uint8_t *Ptr2)
{
	uint8_t ret = 0;

	if (Menu.ChgType == 0)
	{
		/* Read TI hex file Header information(Orig: LLC) */
		
		R_tfat_f_open(&myFile2, "LLC631.hex", (TFAT_FA_OPEN_EXISTING | TFAT_FA_READ));
		R_tfat_f_read (&myFile2,&asciiBuffer2[0], 256, &readBytes2);	/* read first segment */
		SearchHeaderContent(Ptr,1);
		R_tfat_f_close (&myFile2); 
		
		ret = 1;
	}
	else
	{
		/* Read Freescale hex file Header information (Orig: 7kW control card) */
		
		if (R_tfat_f_open(&myFile2, "PRC631.hex", (TFAT_FA_OPEN_EXISTING | TFAT_FA_READ)) == TFAT_FR_OK)
		{
			R_tfat_f_read (&myFile2,&asciiBuffer2[0], 256, &readBytes2);	/* read first segment */
			SearchHeaderContent(Ptr,0);
			R_tfat_f_close (&myFile2); 
			ret = 2;  //only prc
		}
		
		/* Read TI hex file Header information(Orig: Vienna) */
		
		if (R_tfat_f_open(&myFile2, "PFC631.hex", (TFAT_FA_OPEN_EXISTING | TFAT_FA_READ)) == TFAT_FR_OK)
		{
			R_tfat_f_read (&myFile2,&asciiBuffer2[0], 256, &readBytes2);	/* read first segment */
			SearchHeaderContent(Ptr2,1);
			R_tfat_f_close (&myFile2); 
			if (ret != 2) ret = 3; //only vienna
			else ret = 4; //both
		}		
	}
	return ret;
}

void GenerateUpdateList (uint8_t Nb)
{
	uint8_t i,j = 0;
	
	if (Nb == 0xFF)
	{
		for (i = 0; i < TextListMax-1;i++)
		{
			if (TextList[i+1].Index != 0xFE)
			{
				UpdateList[j].CanID = TextList[i+1].Index;
				UpdateList[j++].SubCanID = TextList[i+1].SubIndex;
			}
		
			if (TextList[i+1].Index2 != 0xFE)
			{
				UpdateList[j].CanID = TextList[i+1].Index2;
				UpdateList[j++].SubCanID = TextList[i+1].SubIndex2;			
			}		
		}
		
		UpdateList[j].CanID = 0xFFFF;
	}
	else
	{
		if (TextList[Nb].Index != 0xFE)
		{
			UpdateList[j].CanID = TextList[Nb].Index;
			UpdateList[j++].SubCanID = TextList[Nb].SubIndex;
		}
		
		if (TextList[Nb].Index2 != 0xFE)
		{
			UpdateList[j].CanID = TextList[Nb].Index2;
			UpdateList[j++].SubCanID = TextList[Nb].SubIndex2;			
		}
		
		UpdateList[j].CanID = 0xFFFF;
	}
}


uint8_t BootloaderChrono (void)
{
	/* There is no display function here */
	
	uint16_t i;
	uint8_t Temp;
	static uint16_t Linecounter = 0;

	
	switch(BootloaderChronoState)
	{
	
		case SEND:	
		
			switch(BootloaderChronoStep)
			{
				
				case BOOTCONNECT:
					
					if (SendBootCommandConnect() != 0)		
					{
						RetryCount++;
						if (RetryCount > 0)
						{
							Percent = 101;
							sprintf(&MsgError[0],"Connect Send Can Error!");
						 	BootloaderChronoState = SUSPEND;	//Error
							return 1;	
						}
						BootloaderChronoState = SEND;	
						return 0;	
					}							
					BootloaderChronoState = RECV;				
				
				break;
				
				case BOOTACTIONSERVICE:
					
					if (SendBootCommandActionService() != 0)		
					{
						RetryCount++;
						if (RetryCount > 0)
						{
							Percent = 101;
							sprintf(&MsgError[0],"ActionService Send Can Error!");
						 	BootloaderChronoState = SUSPEND;	//Error
							return 1;	
						}
						BootloaderChronoState = SEND;	
						return 0;	
					}							
					BootloaderChronoState = RECV;					
				
				break;
				
				case READLINE:
			
					i = 0;
					
					Percent = (ReadBytes*100) /(Fileinfo2.fsize + 1);
					
					ReadPointer = 0;
					do
					{
						R_tfat_f_read (&myFile2,&Temp, 1, &readBytes2);
						ReadBytes++;
						if ((record2.status == REC_NOT_FOUND) && (Temp == ':'))
						{
							ReadPointer = 0;
			                record2.status = REC_FOUND_BUT_NOT_FLASHED;							
						}
						else if ((record2.status == REC_FOUND_BUT_NOT_FLASHED) && ((Temp == 0x0A)))
						{
							asciiBuffer2[ReadPointer + 1] = 0;
							ConvertAsciiToHex(&asciiBuffer2[0],hexRec2);
			                
							record2.status = REC_NOT_FOUND;
							
							NbOfbytes = hexRec2[0];
							Linecounter++;
							
							WritePointer = 0;
							BootloaderChronoStep = WRITEFLASH;
							BootloaderChronoState = SEND;
							return 0;
							
						}
						else if ((record2.status == REC_FOUND_BUT_NOT_FLASHED) && ((Temp == 0xFF)))
						{
							//End of file (may file error)
							//Percent = 101;
							//sprintf(&MsgError[0],"End Of File!");
					 		//BootloaderChronoState = SUSPEND;	//Error								
							
							BootloaderChronoStep = BOOTDISCONNECT;
							BootloaderChronoState = SEND;
							return 0;							
							
						}
						else if (record2.status == REC_FOUND_BUT_NOT_FLASHED)
						{
							asciiBuffer2[ReadPointer++] = Temp;
						}
						
					i++;
					}while(i<200);			
			
					if (i>199)
					{
						Percent = 101;
						sprintf(&MsgError[0],"Record not found!");
					 	BootloaderChronoState = SUSPEND;	//Error		
						return 1;					
					}

			

					
				break;
				
				case WRITEFLASH:
					
						if (hexRec2[3] == 0x04)
						{
							//Update HighField variable
							FlashAddress = FlashAddress | (hexRec2[4] << 24)  | hexRec2[5] << 16;
							UpdateMTA = 1;
							BootloaderChronoStep = READLINE;
						}	
						else if (hexRec2[3] == 0x00)
						{
							if (UpdateMTA)
							{
								//Write MTA
								FlashAddress = (FlashAddress & 0xFFFF0000) | (hexRec2[1]<<8) | hexRec2[2];
								
								if (SendBootCommandSetMTA() != 0)		
								{
									RetryCount++;
									if (RetryCount > 0)
									{
											Percent = 101;
											sprintf(&MsgError[0],"Internal CAN Error!");
										 	BootloaderChronoState = SUSPEND;	//Error
											return 1;	
									}
									BootloaderChronoState = SEND;	
									return 0;	
								}							
								
								BootloaderChronoState = RECV;	
								
							}
							else
							{
								if (WritePointer == 0)
								{
									i = (hexRec2[1]<<8) | hexRec2[2];
									if ((FlashAddress & 0xFFFF) != i)
									{
										UpdateMTA = 1;
										return 0;	
									}
								}
								
								//Send Frame by 4 byte
								//if (WritePointer < hexRec2[0])
								if (NbOfbytes != 0)
								{
									//send CanMsg
									//how many bytes?
									if (((int8_t)NbOfbytes - 5) >= 0)
									{
										Temp = 5;
									}
									else
									{
										Temp = NbOfbytes % 5;
									}
									
									if (SendBootCommandProgram(&hexRec2[4+WritePointer],Temp) != 0)		
									{
										RetryCount++;
										if (RetryCount > 0)
										{
											Percent = 101;
											sprintf(&MsgError[0],"Internal CAN Error!");
										 	BootloaderChronoState = SUSPEND;	//Error	
											return 0;	
										}
										BootloaderChronoState = SEND;	
										return 0;	
									}									
									
									NbOfbytes -= Temp;
									BootloaderChronoState = RECV;
									WritePointer += Temp;
									//FlashAddress += 2;
								}
								else
								{
									//end of line
									FlashAddress += (WritePointer / 2);
									BootloaderChronoState = SEND;
									BootloaderChronoStep = READLINE;	
								}
							}
						}
						else if ((hexRec2[3] == 0x01) && (NbOfbytes == 0))
						{
							R_tfat_f_close (&myFile2); 
							BootloaderChronoStep = BOOTDISCONNECT;
							BootloaderChronoState = SEND;							
						}
				break;	
				
				case BOOTDISCONNECT:

					if (SendBootCommandDisconnect() != 0)		
					{
						RetryCount++;
						if (RetryCount > 10)
						{
							Percent = 101;
							sprintf(&MsgError[0],"Disconnect Send Can Error!");
						 	BootloaderChronoState = SUSPEND;	//Error
							return 1;	
						}
						BootloaderChronoState = SEND;	
						return 0;	
					}							
					BootloaderChronoState = RECV;			
				
				break;
				
				case BOOTEND:
				
						sprintf(&MsgError[0],"Succes!");	
						Percent = 100;
		        		StopBootloaderChrono();
				break;
				
				case SEARCHDEVICE:
				
					if (Counter < 9)
					{
						/* NO Version read, just search */

						/* Check module is in boot mode */
						if (SendBootCommandSearch(Counter,CounterSubID[CounterStep]) != 0)
						{							
							/*Can Error??*/
							Percent = 101;
							sprintf(&MsgError[0],"Internal CAN Error!");
						 	BootloaderChronoState = SUSPEND;	//Error	
							break;	
						}	

						BootloaderChronoState = RECV;
						
					}
					else
					{	

						TextListMax = 0;
						/*Generate Text Output*/
						for (i = 1;i<9;i++)
						{
							if ((TextList[i].Index != 0xFE) && (TextList[i].Index2 != 0xFE))
							{
								/*PRC & Vienna Found*/
								if (LLCRamRO[TextList[i].Index].Ver[0] != 0) /*&& (Vienna version) )*/
								{
									sprintf(&TextList[i].Text[0],"MOD %d : R%d.%d / NoFW",TextList[i].Index+1,LLCRamRO[TextList[i].Index].Ver[0]/100,LLCRamRO[TextList[i].Index].Ver[0]%100);	
								}
								/*else if (just)
								{
									
								}
								else if (just)
								{
									
								}								
								*/
								else
								{
									/*No FW on Both*/
									sprintf(TextList[i].Text,"MOD %d : NoFW / NoFW",TextList[i].Index+1);
								}
								TextListMax++;
							}
							else if (TextList[i].Index != 0xFE)
							{
								/*Just PRC Found*/
								/*PRC & Vienna Found*/
								if (LLCRamRO[TextList[i].Index].Ver[0] != 0)
								{
									sprintf(&TextList[i].Text[0],"MOD %d : R%d.%d",TextList[i].Index+1,LLCRamRO[TextList[i].Index].Ver[0]/100,LLCRamRO[TextList[i].Index].Ver[0]%100);	
								}
								else
								{
									/*No FW on Both*/
									sprintf(TextList[i].Text,"MOD %d : NoFW",TextList[i].Index+1);
								}	
								TextListMax++;							
							}
							else if (TextList[i].Index2 != 0xFE)
							{
								/*Just Vienna Found*/
								sprintf(TextList[i].Text,"MOD %d :     / NoFW",TextList[i].Index2+1);
								TextListMax++;								
							}
							else
							{
								/*Nothing Found*/
							/*Go to next*/
							}
						}

						if (TextListMax == 0)
						{
							ListGenerated = 255;
							BootloaderChronoState = SUSPEND;	
							return 1;	
						}
						
						if (TextListMax > 1) 	//means 2, add all for begin
						{
							sprintf(&TextList[0].Text[0],"All");
							//TextList[0].Index = 0xFF;
							TextList[0].All = 0xFF;
							TextListMax++;
						}
						else
						{
							for (i = 0;i<TextListMax;i++)
							{
								memcpy(&TextList[i].Text[0],&TextList[i+1].Text[0],24);		//Range can be outside if more than 8 module (no overlap, just stupid datas)
								TextList[i].Index = TextList[i+1].Index;
								TextList[i].SubIndex = TextList[i+1].SubIndex;
								TextList[i].Index2 = TextList[i+1].Index2;
								TextList[i].SubIndex2 = TextList[i+1].SubIndex2;
							}	
						}
					
						TextListCurrent = 0;
						TextListOffset = 0;
						ListGenerated = 1;
						BootloaderChronoState = SUSPEND;
										
					}						
				
				break;
				
			}
		
		break;
		
		case RECV:
		
			//if (BootloaderChronoStep == WRITEFLASH)
			{
				if (FrameRecv == 1)
				{
					if (BootloaderChronoStep == SEARCHDEVICE)
					{
						/* Add module, Check next*/
						/* Only two scan is implemented instead of four */
						
						switch (CounterStep)
						{
							case 0: // MODIF 2.8 : show module answer with address 36 in position 8
                                    if ((id_sa_36 ==1) && (Counter == 8))
									    TextList[TextListMax + 1].Index = Counter - 1;
                                    else
                                        TextList[TextListMax + 1].Index = Counter;
									TextList[TextListMax + 1].SubIndex = CounterSubID[CounterStep]; 
									break;
							case 1: // MODIF 2.8
									if ((id_sa_36 ==1) && (Counter == 8))
									    TextList[TextListMax + 1].Index2 = Counter - 1;
                                    else
                                        TextList[TextListMax + 1].Index2 = Counter;
									TextList[TextListMax + 1].SubIndex2 = CounterSubID[CounterStep];
									break;
						}
						
						/*SubID to check*/
						CounterStep++;
						if (CounterSubID[CounterStep] == 0xFF)
						{
							/*No more SubID to check go to next ID*/	
							CounterStep = 0;
							Counter++;
							if ((TextList[TextListMax + 1].Index2 != 0xFE) || (TextList[TextListMax + 1].Index != 0xFE))
							{
								TextListMax++;
							}
						}
						
						FrameRecv = 0;	
						BootloaderChronoState = SEND;						
					}
					else
					{
						if (RecvFrame[1] == 0x00)
						{
							/*SEND next frame, clear retry count*/	
							UpdateMTA = 0;	//Clear , when there is suces
							RetryCount = 0;
							BootloaderChronoState = SEND;	
							if (BootloaderChronoStep != WRITEFLASH)
								BootloaderChronoStep++;
					
							FrameRecv = 0;
							LastMTA = RecvFrame[7]<<24 | RecvFrame[6]<<16 | RecvFrame[5]<<8 | RecvFrame[4];
						}
						else
						{
							Percent = 101;
							sprintf(&MsgError[0],"Code -> 0x%x!",RecvFrame[1]);
						 	BootloaderChronoState = SUSPEND;	//Error	
							break;									
						}
					}
				}	
				else if (TimeoutMse > Timeout)
				{
					if (BootloaderChronoStep == SEARCHDEVICE)
					{
						/*SubID to check*/
						CounterStep++;
						if (CounterSubID[CounterStep] == 0xFF)
						{
							/*No more SubID to check go to next ID*/	
							CounterStep = 0;
							Counter++;
						}						
						
						BootloaderChronoState = SEND;						
					}
					else
					{
						/*No answer from message*/
						RetryCount++;
						if (RetryCount > 0)
						{
							Percent = 101;
							sprintf(&MsgError[0],"No Answer from Module");
						 	BootloaderChronoState = SUSPEND;	//Error
							return 1;	
						}
						BootloaderChronoState = SEND;	//ReSend
					}
				}			
			}		
					
		break;
		
		case SUSPEND:
			//Finish or Error state
		break;		
	}
	return 0;	
}
