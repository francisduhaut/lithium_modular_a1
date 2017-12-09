#include "r_Flash_API_RX600.h"
#include "include.h"

#define	E2FlashAddr		0x00100000

extern  uchar 	MnuLevel;
extern  void LCD_RefreshScreenSaver (void);
extern  uchar FlagSaveSerial;
extern uchar  LcdTemp[128];

/* Total number of data flash blocks */
uint16_t gNumFlashBlocks = sizeof(g_flash_BlockAddresses)/4u;

/**
 * @brief  Main E2FLASH chrono
 * @param  None
 * @retval None
 */
void E2FLASH_Chrono (void)
{
	char *data;

	if (ReloadConfig != 0)
	{
		ReloadConfigOff;
		Read_Config();
        ChgData.Profile = Menu.Profile; // MODIF R2.7
		if ((State.Charger == StateChgerIdle) && (MnuLevel == 0))
		{
			CreateIdleScreen();
			screenstate = DISPLAY_IDLE;
			Display.Update = 1;
		}
	}
	else if (InitConfig != 0)
	{
		InitConfigOff;
		DefaultConfig();
		MenuWriteOn;
		ModTempConAckDef();		// MODIF 3.2 : Clear DEFCON counter

		if ((State.Charger == StateChgerIdle ) && (MnuLevel == 0))
		{
			CreateIdleScreen();
			screenstate = DISPLAY_IDLE;
			Display.Update = 1;
		}

		memset(&Status, 0 , sizeof(Status));
		Status.TypeSegment = TYPE_STATUS;
		Status.NumSegment = SEGSTATUS;
		Status.MemoPointer = SEGMEMOTOP - 1;
        #ifdef ENERSYS_EU       // MODIF R2.2
            Mains480VACOff;
        #else
            Mains480VACOn;    
        #endif        
		Write_SegFlashData(SEGSTATUS, (uchar *)&Status);

        // MODIF R2.5 :  write RTC to write new daylight saving
		State.DateW.Year = State.DateR.Year;
		State.DateW.Hr = State.DateR.Hr;
		State.DateW.Min = State.DateR.Min;
		State.DateW.Sec = 0;
		State.DateW.Date = State.DateR.Date;
		State.DateW.Month = State.DateR.Month;
        RtcWriteOn;
        // END

		if (FlagSaveSerial == OFF)
		{
			memset(&SerialNumber, 0 , sizeof(SerialNumber));
			SerialNumber.TypeSegment = TYPE_SERIAL;
			SerialNumber.NumSegment = SEGSERIAL;
			data = "NCA001001";
			strcpy ((char *)SerialNumber.SerNum, (char *)data);
			//strcpy ((char *)SerialNumber.RtcRam, (char *)data);
			Write_SegFlashData(SEGSERIAL, (uchar *)&SerialNumber);
		}
		else
			FlagSaveSerial = OFF;
	}
	else if (MenuWrite != 0)
	{
		MenuWriteOff;
		Write_SegFlashData(SEGMENU, (uchar *)&Menu);
        Write_SegFlashData(SEGAARS, (uchar *)&AarsParam);
	}
	else if (StatusWrite != 0)
	{
		StatusWriteOff;
		Write_SegFlashData(SEGSTATUS, (uchar *)&Status);
	}
	else if (MemoWrite != 0)
	{
		MemoWriteOff;
		Write_SegFlashData(Status.MemoPointer, (uchar *)&MemoToSave);
		Write_SegFlashData(SEGSTATUS, (uchar *)&Status);
	}
}


/**
 * @brief  This function initialises the MCU flash area, allowing it to be
 *         read and written to by user code.
 * @param  None
 * @retval None
 */
void Init_FlashData(void)
{
	/* Enable MCU access to the data flash area */
	R_FlashDataAreaAccess(0xFFFF, 0xFFFF);
}

/*******************************************************************************
 * Outline      : Erase_FlashData
 * Description  : This function enters a for loop, and erases all block of E2 data
 *                 flash memory
 * Argument     : none
 * Return value : none
 *******************************************************************************/
void Erase_FlashData(void)
{
	/* Declare flash API error flag */
	uint8_t ret = 0u;

	/* Declare current data flash block variable */
	uint8_t current_block;

	/* Initialise a for loop to erase each of the data flash blocks */
	for(current_block = BLOCK_DB0; current_block < gNumFlashBlocks;
			current_block++)
	{
		/* Fetch beginning address of DF block */
		uint32_t address = g_flash_BlockAddresses[current_block];

		/* Erase data flash block */
		ret |=     R_FlashErase
				(
						current_block
				);

		/* Halt here if erase was unsuccessful */
		while(R_FlashGetStatus() != FLASH_SUCCESS);

		/* Check Blank Checking */
		ret |= R_FlashDataAreaBlankCheck
				(
						address,
						BLANK_CHECK_ENTIRE_BLOCK
				);

		/* Halt here if check was unsuccessful */
		while(R_FlashGetStatus() != FLASH_SUCCESS);
	}

	/* Halt in while loop when flash API errors detected */
	while(ret);
}


/**
 * @brief  This functions erase a segment of 64 bytes in 2 pass of 32 bytes
 * @param  segment number
 * @retval result
 */
int Erase_SegFlashData (int SegNumber)
{
	int ret;

	ret = R_FlashEraseRange ((E2FlashAddr + (SegNumber*64)), 64);
	while(R_FlashGetStatus() != FLASH_SUCCESS);
	return ret;
}


/**
 * @brief  This functions write a segment of 64 bytes
 * @param  segment number, buffer
 * @retval result
 */
int Write_SegFlashData (int SegNumber, uchar *segment)
{
	int ret, c;

	uint8_t pad_buffer[64];
	Erase_SegFlashData(SegNumber);
	c = CrcCalc((uchar *)segment, 0, 61, 63);
	*(segment+62) = LowByte(c);
	*(segment+63) = HighByte(c);
	memcpy((char*)pad_buffer, (char*)segment, 64);

	ret =  R_FlashWrite ((E2FlashAddr + (SegNumber*64)), (uint32_t)pad_buffer, 64);
	while(R_FlashGetStatus() != FLASH_SUCCESS);
	return ret;
}

/**
 * @brief  This functions read a segment of 64 bytes in E2DataFlash
 * @param  segment number, buffer
 * @retval result
 */
uint Read_SegFlashData (int SegNumber, uchar *segment)
{
	ulong ptr = E2FlashAddr + (SegNumber*64);
	memcpy (segment, (void *)ptr, 64);
	return CrcCalc((void *)ptr, 0, 61, 63);
}


/**
 * @brief  Read charger setup in E2Flash
 * @param  None
 * @retval None
 */
void Read_Config (void)
{
	char *data;
	uint c;
	StructLanConfig *Temp;
	Temp = (StructLanConfig *)&LcdTemp[0];

	c = Read_SegFlashData (SEGMENU, (uchar *)&Menu);

	if ((Menu.Langage > NBLANG) || (Menu.Crc != c))
	{
		DefaultConfig();
		Write_SegFlashData(SEGMENU, (uchar *)&Menu);
	}
	if (Menu.Contrast == 0)
		Menu.Contrast = 37;
	if (Menu.CoefCharge == 0)
	{
		Menu.CoefCharge = 100;
		MenuWriteOn;
	}
	if (Menu.TimeDisplaySave == 0)
	{
		Menu.TimeDisplaySave = 15;
		MenuWriteOn;
	}
	if (Menu.BatHighTemp == 0)
	{
		Menu.BatHighTemp = 65;
		MenuWriteOn;
	}

	c = Read_SegFlashData (SEGSTATUS, (uchar *)&Status);
	if (Status.Crc != c)
	{
		memset(&Status, 0 , sizeof(Status));
		Status.TypeSegment = TYPE_STATUS;
		Status.NumSegment = SEGSTATUS;
		Status.MemoPointer = SEGMEMOTOP - 1;
		Write_SegFlashData(SEGSTATUS, (uchar *)&Status);
	}
	c = Read_SegFlashData (SEGSERIAL, (uchar *)&SerialNumber);
	if (SerialNumber.Crc != c)
	{
		memset(&SerialNumber, 0 , sizeof(SerialNumber));
		SerialNumber.TypeSegment = TYPE_SERIAL;
		SerialNumber.NumSegment = SEGSERIAL;
		data = "NCA001001";
		strcpy ((char *)SerialNumber.SerNum, (char *)data);
		//strcpy ((char *)SerialNumber.RtcRam, (char *)data);
		Write_SegFlashData(SEGSERIAL, (uchar *)&SerialNumber);
	}
	else
	{
		memset(&SerialNumber.RtcRam[0], 0, 10);
		Write_SegFlashData(SEGSERIAL, (uchar *)&SerialNumber);
	}

	c = Read_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
	if ((*Temp).Crc != c)
	{
		InitEthernetParam();
	}
    else    // MODIF R2.1
    {
        IP_DHCP[0] = (*Temp).EthParam[0];
        IP_DHCP[1] = (*Temp).EthParam[1];
        IP_DHCP[2] = (*Temp).EthParam[2];
        IP_DHCP[3] = (*Temp).EthParam[3];
    }
       
    c = Read_SegFlashData (SEGAARS, (uchar *)&AarsParam);
	// Modif R2.3 : add more control on AARS segment (TypeSegment and NumSegment)
	if ((AarsParam.Crc != c) || (AarsParam.TypeSegment != TYPE_AARS) || (AarsParam.NumSegment != SEGAARS))
    {
        memset(&AarsParam, 0 , sizeof(AarsParam));
        AarsParam.TypeSegment = TYPE_AARS;
		AarsParam.NumSegment = SEGAARS;
        AarsParam.EasyKitAddr = 0x00400000;
        AarsParam.ImaxLeft = 3200;
        AarsParam.ImaxRight = 3200;
        Write_SegFlashData(SEGAARS, (uchar *)&AarsParam);
    }    
    #ifndef ENERSYS_EU
    AarsParam.EasyKitAddr = 0x00400000;
    AarsParam.EasyKitOnOff = OFF;
    Write_SegFlashData(SEGAARS, (uchar *)&AarsParam);
    #endif

	// MODIF 3.3
	if ((AarsParam.EqTimePhase2 < 1 ) || (AarsParam.EqTimePhase2 > 48))
	{
		AarsParam.EqTimePhase2 = 4;
		Write_SegFlashData(SEGAARS, (uchar *)&AarsParam);
	}

	// MODIF R3.3
	c = Read_SegFlashData (SEGE1STATUS, (uchar *)&E1Status);
	if ((E1Status.Crc != c) || (E1Status.TypeSegment != TYPE_E1_STATUS) || (E1Status.NumSegment != SEGE1STATUS))
	{
		memset(&E1Status, 0 , sizeof(StructE1Status));
		E1Status.TypeSegment = TYPE_E1_STATUS;
		E1Status.NumSegment = SEGE1STATUS	;
		E1Status.EventPointer = SEGE1TOP - 1;
        Write_SegFlashData(SEGE1STATUS	, (uchar *)&E1Status);
	}

	switch (Menu.Area)
	{
	case 0 :
		MaskArea0On;
		MaskArea1Off;
		break ;
	case 1 :
		MaskArea1On;
		MaskArea0Off;
		break ;
	}
	if (Menu.TimeDisplaySave == 0)
	{
		Menu.TimeDisplaySave = 15;
		BackOff;
		MenuWriteOn;
	}

    // MODIF 2.8
    if (Menu.NbModLeft == 6)
    {
        AarsParam.NbModLeft = 0;
        Menu.NbModLeft = 8;
        MenuWriteOn;
    }

	MaskXFCOn;

    // MODIF NBCELL AUTO
    switch (Menu.CellSize)
    { // valid Cell size
    case 6:
    case 12:
    case 18:
    case 24:
    case 30:    // MODIF 2.8
    case 32:
    case 36:
    case 40:
    case 48:
        Menu.NbCells = Menu.CellSize;
        MenuWriteOn;
        break;

    case 255:
        Menu.NbCells = CalculNbCells();
        break;

    default:
        Menu.CellSize = Menu.NbCells;
        MenuWriteOn;
        break;
    }

	LCD_RefreshScreenSaver();
    
    // MODIF 2.8 : after charger update, set Nexsys ON if profile Nexsys was set
    if ( ((Menu.Profile == TPPL) || (Menu.Profile == XFC) || (Menu.Profile == NXBLC)) 
        && (Menu.XFCOffOn == 0))
    {
        Menu.XFCOffOn = 1;
        MenuWriteOn;
    }
    
    // MODIF 2.8
    if (Menu.Profile == FAST)
    {
        Menu.Profile = IONIC;
        MenuWriteOn;
    }
	
	// MODIF 3.3
#ifdef ENERSYS_EU
    if (Menu.XFCOffOn != 0)
	{
		if (Menu.Branding != BRAND_LIFE_IQ_NEXSYS)
		{
    		Menu.Branding = BRAND_LIFE_IQ_NEXSYS;
    		MenuWriteOn;
		}
	}
    else if (Menu.Branding != BRAND_LIFE_IQ)
	{
	   	Menu.Branding = BRAND_LIFE_IQ;
	   	MenuWriteOn;
	}
#endif
#ifdef ENERSYS_US
    if (Menu.XFCOffOn != 0)
	{
		if (Menu.Branding != BRAND_IMPAQ_NEXSYS)
		{	
    		Menu.Branding = BRAND_IMPAQ_NEXSYS;
    		MenuWriteOn;
		}
	}
    else if (Menu.Branding != BRAND_IMPAQ_PLUS)
	{
	   	Menu.Branding = BRAND_IMPAQ_PLUS;
	   	MenuWriteOn;
	}
#endif
#ifdef HAWKER_US
    if (Menu.XFCOffOn != 0)
	{
		if (Menu.Branding != BRAND_MODC_NEXSYS)
		{
    		Menu.Branding = BRAND_MODC_NEXSYS;
    		MenuWriteOn;
		}
	}
    else if (Menu.Branding != BRAND_MODC)
	{
	   	Menu.Branding = BRAND_MODC;
	   	MenuWriteOn;
	}
#endif   
}
