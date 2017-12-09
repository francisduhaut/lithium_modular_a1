/*
 * can.c
 *
 *  Created on: 01/04/2013
 *      Author: Francis DUHAUT
 */

#include "include.h"


uint32_t api_status;
uint8_t	userapp_status = 1;
uint16_t h = 0, k = 0;
can_frame_t  can_tx_dataframe[8];
ulong TimerCanMs;

//extern volatile uint32_t g_CAN_channel;
extern int CAN_cntError;
extern uint	SlopeI;
extern uint IbatModMem[MODNB+1];
extern uchar FlagThAmbiant;
extern schar TambCompare;
extern uchar FlagSaveSerial;
extern uchar ModNumber;

ulong TimerCANRamRW;
ulong TimerCANVersion;
ulong TimerCANCounterAh;
ulong TimeoutCAN;
ulong CntCANFault;

// MODIF 2.8 : id_sa_36 = 1 broadcast message from address 36 (module 8 with V168) 
uchar id_sa_36;
extern uint8_t BootApiStep;
extern  uint PMod;

extern uchar  LcdTemp[128];
extern StructWifiConfig *TempWifi;
extern StructLanConfig *Temp;

/**
 * @brief  Main CAN Chrono
 * @param
 *
 * @retval
 */
void CAN_Chrono (void)
{
	uchar i;

	if (R_CAN_CheckErr(CH_0) != R_CAN_STATUS_ERROR_PASSIVE)
	{
		CANStateOn;
		CAN_cntError = 0;
	}
	else
	{
		if (CAN_cntError > 50)
			CANStateOff;
	}

	// MODIF R2.5
    if ((TimeoutCAN < State.TimerSec) && (StateIdle ==0))
	{
        CntCANFault++;
        if (State.Charger <= StateChgerStChg)
        {
            for (i=0; i<MODNB; i++)
		        memset(&LLCRamRO[i], 0 , sizeof(StructLLCRamRO));
            Init_CAN();
            CANTimeoutOn;
        }
        #ifndef DEMO
        else
        {   
            IdleOn;
            if (DFnetwork == 0)
            {
                for (i=0; i<MODNB; i++)
		            memset(&LLCRamRO[i], 0 , sizeof(StructLLCRamRO));
                DFnetworkOn;    // To save into memo CFC
			    State.Phase = (State.Phase & 0x0F) | StatePhDefCur;
            }
        }
        #endif
		
	}
	else
	{   // MODIF R2.2
		//CANTimeoutOff;
	}
    
    if (State.Charger <= StateChgerStChg)
    {
        if (CntCANFault != 0)
        {
            Init_CAN();
            ModAckDef();
        }
    }
	
	// MODIF P3.0
	if (StateIdle != 0)
	{
		TimerCANRamRW = State.TimerMs + TIMEOUT_RAMRW;
	}

	// MODIF 2.8 : No Stop CANRW frame in Idle mode to save power
	if ((State.TimerMs > TimerCANRamRW) && (CANState != 0) && (ManuSw == 0) && (State.TimerSec > 3L))
    //if ((State.TimerMs > TimerCANRamRW) && (CANState != 0) && (ManuSw == 0) && (State.TimerSec > 3L) && (BootApiStep == 0) && (StateIdle == OFF))
	{
		// Write RamRW data every TIMEOUT_RAMRW ms
		TimerCANRamRW = State.TimerMs + TIMEOUT_RAMRW;
		// Clear CAN data no receive broadcast message (2 sec)
		for (i=0; i < MODNB; i++)
		{
			if (LLCRamRO[i].TimerChg < State.TimerSec)
			{
				memset(&LLCRamRO[i], 0 , sizeof(StructLLCRamRO));
				LLCRamRO[i].TimerChg = State.TimerSec + TIMEOUT_LLC;
				if (CanErr(i) == 0)
					ModResetEfficiency();
				CanErrOn(i);
			}
			else
			{
				if (CanErr(i) == 0)
				{
                    // MODIF 2.8
                    PMod |= (1 << i);
                    // END
                    
					J1939_Write(i);
					if (LLCRamRO[i].Ver[0] == 0)
					{
						J1939_RequestData(i, CANVERSION); // firmware version
						J1939_RequestData(i, CANCOUNTER); // counter AH
					}
					
					if (LLCRamRO[i].CntReadId == 0)
						J1939_RequestData(i, CANEEP+1);
				}
			}
		}
	}  
}


void J1939_Read(void)
{
	char *ptr;
	unsigned char OffsetLLCRamRo, BroadcastMessage, Pduf;

	uchar id_sa;        // Source address
	uchar id_sa_ramro;  // Source address to struct RamRO
	uchar id_da;        // Destination address

    id_sa = focan_rx_frame.id & CAN_SA_MASK;

    // MODIF 2.8 : add detection of address 36 for module 8
    if (id_sa == 32)
    {
        id_sa_36 = 0;   // clear flag
    }
    else if (id_sa == 36)
    {
        id_sa = 32;     // change address to 32
        id_sa_36 = 1;   // set flag (receive message from address 36)
    }  
    
	id_sa_ramro = (id_sa >> 2);

	//if ((Menu.ChgType == TYPE_3PHASE) && ((id_sa & 0x03) != 0))
	//	id_sa_ramro = 0xFF; // Brick answer and not a PFC

	id_da = (focan_rx_frame.id & CAN_DA_MASK) >> 8;
	OffsetLLCRamRo = (focan_rx_frame.id & CAN_OFFSET_RAMRO_MASK) >> 16;
	BroadcastMessage = (focan_rx_frame.id & CAN_BMESSAGE_MASK) >> 16;
	Pduf = (focan_rx_frame.id & CAN_PDUF_MASK) >> 16;

    // MODIF 2.8 : stop CANBUS broadcast received message in IDLE mode
	//if ((BroadcastMessage == CANRAMRO) && (OffsetLLCRamRo < 8) && (id_da == 0) && (id_sa > 0) && (id_sa_ramro <= MODNB))
    if ((BroadcastMessage == CANRAMRO) && (OffsetLLCRamRo < 8) && (id_da == 0) && (id_sa > 0) && (id_sa_ramro <= MODNB) && (StateIdle == OFF))
	{
        CANTimeoutOff;  // MODIF R2.2
		TimeoutCAN = State.TimerSec + TIMEOUT_CANBUS;
		ptr = ((char *)&(LLCRamRO[id_sa_ramro-1].UnionRamROStatus.Status)) + (OffsetLLCRamRo * 8);
		memcpy (ptr, &focan_rx_frame.data[0], focan_rx_frame.dlc);
		LLCRamRO[id_sa_ramro-1].TimerChg = State.TimerSec + TIMEOUT_LLC;
		if (LLCRamRO[id_sa_ramro-1].IBat < 0)
			LLCRamRO[id_sa_ramro-1].IBat = 0;
		if (CanErr(id_sa_ramro-1) != 0)   // MODIF P0.4
			ModResetEfficiency();
		CanErrOff(id_sa_ramro-1);
        if ((id_sa & 0x01) != 0)
        {
            if (Menu.ChgType != TYPE_1PHASE)
            {
                Menu.ChgType = TYPE_1PHASE;
                // MODIF 2.8
                AarsParam.NbModLeft = 0;
                Menu.NbModLeft = 8;
                // END
                MenuWriteOn;
            }
        }
        else
        {
            
            if (Menu.ChgType != TYPE_3PHASE)
            {
                Menu.ChgType = TYPE_3PHASE;
                MenuWriteOn;
             }
        }
	}
	// MANU MODE
	if ((BroadcastMessage == CANREMOTE) && (id_da == 0) && (id_sa == CANREMOTE))
	{
		if ((focan_rx_frame.data[0] == 0x01) && (focan_rx_frame.dlc == 1))
		{
			ManuSwOn;
			ManuRegOn;
		}
        else if ((focan_rx_frame.data[0] == 0x02) && (focan_rx_frame.dlc == 1))
        {
            ManuSwOff;
            ManuRegOff;
            FlagSaveSerial = ON;
            InitConfigOn;
            LCDMenu_Level();
        }
        else if ((focan_rx_frame.data[0] == 0x03) && (focan_rx_frame.dlc == 1))
        {
            ManuSwOff;
            ManuRegOff;
            LCDMenu_Level();
            ReloadConfigOn;
        }
		else
		{
			ManuSwOff;
			ManuRegOff;
		}
	}
	// MODULE VERSION
	if ((Pduf == CANVERSION) && (id_da == 0) && (id_sa > 0) && (id_sa_ramro <= MODNB))
	{
		LLCRamRO[id_sa_ramro-1].Ver[0] = focan_rx_frame.data[4] + focan_rx_frame.data[5] * 256;
	}

	// Counter AH
	if ((Pduf == CANCOUNTER) && (id_da == 0) && (id_sa > 0) && (id_sa_ramro <= MODNB))
	{
		LLCRamRO[id_sa_ramro-1].CntAh = focan_rx_frame.data[0] + (focan_rx_frame.data[1] << 8)
                                    		+ (focan_rx_frame.data[2] << 16) + (focan_rx_frame.data[3] << 24);
		LLCRamRO[id_sa_ramro-1].CntPower = focan_rx_frame.data[4] + (focan_rx_frame.data[5] << 8)
                                    		+ (focan_rx_frame.data[6] << 16) + (focan_rx_frame.data[7] << 24);
	}

	// ID
	if ((Pduf == (CANEEP + 1)) && (id_da == 0) && (id_sa > 0) && (id_sa_ramro <= MODNB))
	{
		LLCRamRO[id_sa_ramro-1].CntReadId++;	// MODIF 2.8
		LLCRamRO[id_sa_ramro-1].Id = focan_rx_frame.data[2];
	}

	// serial number
	if (((Pduf == CANSERIAL1) || (Pduf == CANSERIAL2)) && (id_da == 0) && (id_sa == 255))
	{
		J1939_WriteSerialNumber (Pduf);
	}
    
    // MENU CONFIG : MODIF R3.0
	if ((Pduf >= CANMENU) && (Pduf < (CANMENU + 8)) && (id_da == 0) && (id_sa == 255))
	{
		J1939_WriteMenuConfig (Pduf);
	}

    // DATE-TIME
    if ((Pduf == CANDATETIME) && (id_da == 0) && (id_sa == 255))
    {
        J1939_WriteRTC(Pduf);
    }
    
    // SETUP
    if (( (Pduf == CANSETUP_01) 
        || (Pduf == CANSETUP_02) 
        || (Pduf == CANSETUP_03)
        || (Pduf == CANSETUP_04)
        || (Pduf == CANSETUP_05)
        || (Pduf == CANSETUP_06)
        || (Pduf == CANSETUP_07)) && (id_da == 0) && (id_sa == 255))
    {
        J1939_WriteSetup(Pduf);
    }

    // LAN SETUP
    if (( (Pduf == CANLAN_01) 
        || (Pduf == CANLAN_02) 
        || (Pduf == CANLAN_03)
        || (Pduf == CANLAN_04)
        || (Pduf == CANLAN_05)
        || (Pduf == CANLAN_06)
        || (Pduf == CANLAN_07)) && (id_da == 0) && (id_sa == 255))
    {
        J1939_WriteLAN(Pduf);
    }

	// Bootloader Answer
	if (((Pduf == 0xE0) && (id_da == 0)))
	{
		/* copy to Buffer*/

		RecvFrame[0] = focan_rx_frame.data[0];
		RecvFrame[1] = focan_rx_frame.data[1];
		RecvFrame[2] = focan_rx_frame.data[2];
		RecvFrame[3] = focan_rx_frame.data[3];
		RecvFrame[4] = focan_rx_frame.data[4];
		RecvFrame[5] = focan_rx_frame.data[5];
		RecvFrame[6] = focan_rx_frame.data[6];
		RecvFrame[7] = focan_rx_frame.data[7];
		FrameRecv = 1;
	}

}

/**
 * @brief  Remot request read
 * @param
 *
 * @retval
 */
void J1939_ReadRemoteRequest(void)
{
	unsigned char Pduf;

	uchar id_sa;        // Source address
	uchar id_da;        // Destination address

	id_sa = focan_rx_remote.id & CAN_SA_MASK;
	id_da = (focan_rx_remote.id & CAN_DA_MASK) >> 8;
	Pduf = (focan_rx_remote.id & CAN_PDUF_MASK) >> 16;

	// serial number
	if (((Pduf == CANSERIAL1) || (Pduf == CANSERIAL2)) && (id_da == 0) && (id_sa > 0))
	{
		J1939_ReadSerialNumber (id_sa, Pduf);
	}
    // MENU CONFIG
    if (((Pduf >= CANMENU) && (Pduf < (CANMENU+8))) && (id_da == 0) && (id_sa == 255))
    {
        J1939_ReadMenuConfig(id_sa, Pduf);
    }
    // TYPE BAY
    if ((Pduf == CANTYPEBAY) && (id_da == 0) && (id_sa == 255))
    {
        J1939_ReadTypeBay(id_sa, Pduf);
    }
    // KEYSTATE
    if ((Pduf == CANKEYSTATE) && (id_da == 0) && (id_sa == 255))
    {
        J1939_ReadKeyState(id_sa, Pduf);
    }
    // SETUP
    if (( (Pduf == CANSETUP_01) 
        || (Pduf == CANSETUP_02) 
        || (Pduf == CANSETUP_03)
        || (Pduf == CANSETUP_04)
        || (Pduf == CANSETUP_05)
        || (Pduf == CANSETUP_06)
        || (Pduf == CANSETUP_07)
        || (Pduf == CANFIRMWARE)) && (id_da == 0) && (id_sa == 255))
    {
        J1939_ReadSetup(id_sa, Pduf);
    }

    // LAN SETUP
    if (( (Pduf == CANLAN_01) 
        || (Pduf == CANLAN_02) 
        || (Pduf == CANLAN_03)
        || (Pduf == CANLAN_04)
        || (Pduf == CANLAN_05)
        || (Pduf == CANLAN_06)
        || (Pduf == CANLAN_07)) && (id_da == 0) && (id_sa == 255))
    {
        J1939_ReadLAN(id_sa, Pduf);
    }    
}

/**
 * @brief  Write RamRW data
 * @param  destination address
 *
 * @retval
 */
void J1939_Write (uchar id_da)
{
	char *ptr;
	uchar i;
	uint j;
	uchar id_sa;  // Source address
	uchar id_da_tmp;
	uchar offset;

	id_da_tmp = ((id_da + 1) << 2);
	if (Menu.ChgType == TYPE_1PHASE) // Add 1 for single phase (brick only)
		id_da_tmp += 1;

    // MODIF 2.8 : add address 36 management
    if ((id_sa_36 == 1) && (id_da_tmp == 32))
    {
        id_da_tmp = 36;
    }

	offset = 0;
	id_sa = 0;
    
    // Modif R2.1 -> Manage PowerMax with Module ID
    LLCRamRW[id_da].PMax = ModFindPMaxModuleID(LLCRamRO[id_da].Id);
    
    if (LLCRamRW[id_da].Slope < 10)     //  Slope Min = 1A/sec
        LLCRamRW[id_da].Slope = 10;

	for (i=0; i < 2; i++) // Write Frame 0 / Frame 1
	{ 
		DISABLE_IRQ();
		j = CANRAMRW + offset;  	
		can_tx_dataframe[i].id = 0x18000000 + (j << 16) + (id_da_tmp << 8) + id_sa;
		can_tx_dataframe[i].dlc = 8;
		ptr = ((char *)&(LLCRamRW[id_da].UnionStatus.Status)) + (offset * 8);
		memcpy (&can_tx_dataframe[i].data[0], ptr, can_tx_dataframe[i].dlc);
		ENABLE_IRQ();
		api_status = R_CAN_TxSet(CH_0, (CANBOX_0_RAMRW_TX + i), &can_tx_dataframe[i], DATA_FRAME); 
		if (api_status != 0)
			i = 0xFF;
		offset++;
	}
	// clear Ack fault bit
	if ((LLCRamRW[id_da].UnionStatus.Status & 0x00000001) != 0)
		LLCRamRW[id_da].UnionStatus.Status &= ~0x00000001;
    
	// clear TempCon Ack fault bit
	if ((LLCRamRW[id_da].UnionStatus.Status & 0x00000020) != 0)
		LLCRamRW[id_da].UnionStatus.Status &= ~0x00000020;
	
    // For FAN if TH ambiant
	if (FlagThAmbiant == ON) 
		LLCRamRW[id_da].UnionStatus.Status |= 0x00410000;
	else
		LLCRamRW[id_da].UnionStatus.Status &= ~0x00410000;
        
    // LMEB On/Off    
    if (FilPil != 0  )
        LLCRamRW[id_da].UnionStatus.Status |= 0x00000008;
    else
        LLCRamRW[id_da].UnionStatus.Status &= ~0x00000008;    
}

void J1939_RequestData (uchar id_da, uint Pduf)
{
	uint j;
	uchar id_sa;  // Source address
	uchar id_da_tmp;

	id_da_tmp = ((id_da + 1) << 2);
	if (Menu.ChgType == TYPE_1PHASE) // Add 1 for single phase (brick only)
		id_da_tmp += 1;

    // MODIF 2.8 : add address 36 management
    if ((id_sa_36 == 1) && (id_da_tmp == 32))
    {
        id_da_tmp = 36;
    }

	id_sa = 0;
	DISABLE_IRQ();
	j = Pduf;  	
	can_tx_dataframe[0].id = 0x18000000 + (j << 16) + (id_da_tmp << 8) + id_sa;
	can_tx_dataframe[0].dlc = 8;
	ENABLE_IRQ();
	api_status = R_CAN_TxSet(CH_0, (CANBOX_0_RAMRW_TX), &can_tx_dataframe[0], REMOTE_FRAME); 
}

/**
 * @brief  Writing serial number over CANBUS (PGN 256/512)
 * @param
 *
 * @retval
 */
void J1939_WriteSerialNumber (uchar Pduf)
{
	int i;

	if (Pduf == CANSERIAL1)
	{

		for (i=0; i<4; i++)
		{
			SerialNumber.SerNum[i] = focan_rx_frame.data[4+i];
		}
	}
	else if (Pduf == CANSERIAL2)
	{
		for (i=0; i<8; i++)
		{
			SerialNumber.SerNum[4+i] = focan_rx_frame.data[i];
		}
	}
	Erase_SegFlashData(SEGSERIAL);
	SerialNumber.TypeSegment = TYPE_SERIAL;
    SerialNumber.NumSegment = SEGSERIAL;
    Write_SegFlashData(SEGSERIAL, (uchar *)&SerialNumber);
}


/**
* @brief  Read KeyState and send to CANBUS
* @param  Destination address, Pduf (PGN 3328)
*
* @retval
*/
void J1939_ReadKeyState (uchar id_da, uchar Pduf)
{
    int i;
    uchar id_sa;

    id_sa = 0;

    for (i=0; i<8; i++)
    {
        can_tx_dataframe[3].data[i] = 0;
    }
    DISABLE_IRQ();
    can_tx_dataframe[3].id = 0x18000000 + (Pduf << 16) + (id_da << 8) + id_sa;
    can_tx_dataframe[3].dlc = 8;
    
    if (Key.Start == 1)
        can_tx_dataframe[3].data[0] =  1;
    if (Key.Menu == 1)    
        can_tx_dataframe[3].data[1] =  1;
    if (Key.Up == 1)
        can_tx_dataframe[3].data[2] =  1;
    if (Key.Down == 1)
        can_tx_dataframe[3].data[3] =  1;
    if (Key.Right == 1)
        can_tx_dataframe[3].data[4] =  1;
    
    ENABLE_IRQ();
    TimerCANRamRW = State.TimerMs + TIMEOUT_RAMRW;  // delay RamRW writing
    api_status = R_CAN_TxSet(CH_0, (CANBOX_0_RAMRW_TX), &can_tx_dataframe[3], DATA_FRAME); 
}


/**
 * @brief  read SETUP charger over CANBUS
 * @param  Destination address, Pduf
 *
 * @retval
 */
void J1939_ReadSetup(uchar id_da, uchar Pduf)
{
    int i;
    uchar id_sa;

    id_sa = 0;
    
    // Force charger to stop charge    
    StartStopOff;

    for (i=0; i<8; i++)
    {
        can_tx_dataframe[3].data[i] = 0;
    }
    DISABLE_IRQ();
    can_tx_dataframe[3].id = 0x18000000 + (Pduf << 16) + (id_da << 8) + id_sa;
    can_tx_dataframe[3].dlc = 8;

    switch (Pduf)
    {
        case CANSETUP_01 :
            can_tx_dataframe[3].data[0] = Menu.Langage;
            can_tx_dataframe[3].data[1] = Menu.Area;
            can_tx_dataframe[3].data[2] = Menu.Profile;
            can_tx_dataframe[3].data[3] = 0;
            switch (Menu.CableSection)
            {
                case 6  : can_tx_dataframe[3].data[4] = 0; break;
                case 10 : can_tx_dataframe[3].data[4] = 1; break;    
                case 16 : can_tx_dataframe[3].data[4] = 2; break;
                case 25 : can_tx_dataframe[3].data[4] = 3; break;
                case 35 : can_tx_dataframe[3].data[4] = 4; break;
                case 50 : can_tx_dataframe[3].data[4] = 5; break;
                case 70 : can_tx_dataframe[3].data[4] = 6; break;
                case 95 : can_tx_dataframe[3].data[4] = 7; break;
            }
            if (Menu.Area ==0)
            {
                can_tx_dataframe[3].data[5] = (uchar)((Menu.CableLength & 0xFF00) >> 8);
                can_tx_dataframe[3].data[6] = (uchar)(Menu.CableLength & 0x00FF);
            }
            else
            {
                can_tx_dataframe[3].data[5] = (uchar)(((((uint)Menu.CableLength * 33) / 100) & 0xFF00) >> 8);
                can_tx_dataframe[3].data[6] = (uchar)((((uint)Menu.CableLength * 33) / 100) & 0x00FF);
            }
            if (Menu.AutoStartOffOn != 0)
                can_tx_dataframe[3].data[7] = 0;
            else
                can_tx_dataframe[3].data[7] = 1;    
        break;
        
        case CANSETUP_02 :
            can_tx_dataframe[3].data[0] = Menu.Area;
            can_tx_dataframe[3].data[1] = (uchar)((Menu.BatCap & 0xFF00) >> 8);
            can_tx_dataframe[3].data[2] = (uchar)(Menu.BatCap & 0x00FF);
            switch (Menu.Area)
            {
                case 0:
                    can_tx_dataframe[3].data[3] = Menu.BatTemp;
                break;
                
                case 1:
                    can_tx_dataframe[3].data[3] = Menu.BatTempF;
                break;
            }
            can_tx_dataframe[3].data[4] = Menu.CapAutoManu;
            can_tx_dataframe[3].data[5] = Menu.InitDOD;
            if (Menu.CellSize != 255)
                can_tx_dataframe[3].data[6] = Menu.CellSize;
            else
                can_tx_dataframe[3].data[6] = 0;
            can_tx_dataframe[3].data[7] = Menu.BatteryRest;
        break;
        
        case CANSETUP_03 :
            if (Menu.ChgSkipDelay == 1)
                can_tx_dataframe[3].data[0] = 2;
            else if ((Menu.ChgDelayHour == 1) && (Menu.ChgSkipDelay == 0))
                can_tx_dataframe[3].data[0] = 1;    
            else    
                can_tx_dataframe[3].data[0] = 0;
            can_tx_dataframe[3].data[1] = (uchar)((Menu.ChgDelayVal & 0xFF00) >> 8);
            can_tx_dataframe[3].data[2] = (uchar)(Menu.ChgDelayVal & 0x00FF);
            can_tx_dataframe[3].data[3] = Menu.FloatingOffOn;
            can_tx_dataframe[3].data[4] = (uchar)((Menu.Vfloating & 0xFF00) >> 8);
            can_tx_dataframe[3].data[5] = (uchar)(Menu.Vfloating & 0x00FF);
            can_tx_dataframe[3].data[6] = (uchar)((Menu.Ifloating & 0xFF00) >> 8);
            can_tx_dataframe[3].data[7] = (uchar)(Menu.Ifloating & 0x00FF); 
        break;
        
        case CANSETUP_04 :
            can_tx_dataframe[3].data[0] = Menu.EqualDelay;
            can_tx_dataframe[3].data[1] = Menu.EqualTime;
            can_tx_dataframe[3].data[2] = Menu.EqualPeriod;
            can_tx_dataframe[3].data[3] = (uchar)(((Menu.Iequal/10) & 0xFF00) >> 8);
            can_tx_dataframe[3].data[4] = (uchar)((Menu.Iequal/10) & 0x00FF);
            can_tx_dataframe[3].data[5] = Menu.RefreshOffOn;
            can_tx_dataframe[3].data[6] = (uchar)(((Menu.ImaxCharger/10) & 0xFF00) >> 8);
            can_tx_dataframe[3].data[7] = (uchar)((Menu.ImaxCharger/10) & 0x00FF);
        break;
        
        case CANSETUP_05 :
            can_tx_dataframe[3].data[0] = (uchar)((Menu.ChgDailyStart & 0xFF00) >> 8);
            can_tx_dataframe[3].data[1] = (uchar)(Menu.ChgDailyStart & 0x00FF);
            can_tx_dataframe[3].data[2] = (uchar)((Menu.ChgDailyEnd & 0xFF00) >> 8);
            can_tx_dataframe[3].data[3] = (uchar)(Menu.ChgDailyEnd & 0x00FF);
            if (Menu.ChgSkipDaily != 0)
                can_tx_dataframe[3].data[4] = 0;
            else
                can_tx_dataframe[3].data[4] = 1;
            can_tx_dataframe[3].data[5] = Menu.XFCOffOn;
            can_tx_dataframe[3].data[6] = Menu.Daylight;
			can_tx_dataframe[3].data[7] = Menu.PLCPulseOffOn;
        break;
        
        case CANSETUP_06 :
            can_tx_dataframe[3].data[0] = (uchar)((Menu.BlockOutStart & 0xFF00) >> 8);
            can_tx_dataframe[3].data[1] = (uchar)(Menu.BlockOutStart & 0x00FF);
            can_tx_dataframe[3].data[2] = (uchar)((Menu.BlockOutEnd & 0xFF00) >> 8);
            can_tx_dataframe[3].data[3] = (uchar)(Menu.BlockOutEnd & 0x00FF);
            can_tx_dataframe[3].data[4] =  Menu.BlockOutEnable & 0x01;
            if (BackOnOff != 0)
            {
                can_tx_dataframe[3].data[5] = (uchar)((Menu.TimeDisplaySave & 0xFF00) >> 8);
                can_tx_dataframe[3].data[6] = (uchar)(Menu.TimeDisplaySave & 0x00FF);
            }
            // MODIF 2.8
            can_tx_dataframe[3].data[7] = Menu.Theme;
        break;
        
        case CANSETUP_07 :
            switch (can_tx_dataframe[3].data[0])
            {
                case 0:
                    can_tx_dataframe[3].data[1] = (schar)Menu.BatHighTemp;
                break;
                
                case 1:
                    can_tx_dataframe[3].data[1] = (Menu.BatHighTemp * 9) / 5 + 32;
                break;
            }
            can_tx_dataframe[3].data[2] = (uchar)((Menu.EVtime & 0xFF00) >> 8);
            can_tx_dataframe[3].data[3] = (uchar)(Menu.EVtime & 0x00FF);
            if (Menu.SkipComIQ == 0)
                can_tx_dataframe[3].data[4] = 1;
            if (IdleState != 0)
                can_tx_dataframe[3].data[5] = 1;
            can_tx_dataframe[3].data[6] = Menu.IonicCoef;
            if (Menu.ChgType == TYPE_1PHASE)
            {
                if (Menu.VMains != 0)   
                    can_tx_dataframe[3].data[7] = 0;    // 120VAC
                else
                    can_tx_dataframe[3].data[7] = 1;    // 208-240VAC
            }
            else
            {
               if (Mains480VAC != 0) 
                    can_tx_dataframe[3].data[7] = 3;    // 480VAC
               else
                    can_tx_dataframe[3].data[7] = 2;    // 400VAC
            }
        break;
        
        case CANFIRMWARE:
            for (i=0; i<4; i++)
            {
              can_tx_dataframe[3].data[i] = VERSION[i];  
            }
        break;
    }
    
    ENABLE_IRQ();
    TimerCANRamRW = State.TimerMs + TIMEOUT_RAMRW;  // delay RamRW writing
    api_status = R_CAN_TxSet(CH_0, (CANBOX_0_RAMRW_TX), &can_tx_dataframe[3], DATA_FRAME); 
}

/**
 * @brief  Writing RTC over CANBUS (PGN 3072)
 * @param
 *
 * @retval
 */
void J1939_WriteRTC (uchar Pduf)
{
	State.DateW.Year = focan_rx_frame.data[0] * 256 + focan_rx_frame.data[1];
    State.DateW.Month = focan_rx_frame.data[2];
    State.DateW.Date = focan_rx_frame.data[3];
    State.DateW.Hr = focan_rx_frame.data[4];
    State.DateW.Min = focan_rx_frame.data[5];
    State.DateW.Sec = focan_rx_frame.data[6];
    RtcWriteOn;
}


/**
 * @brief  Writing SETUP over CANBUS
 * @param
 *
 * @retval
 */
void J1939_WriteSetup (uchar Pduf)
{
    uchar format;
    
    switch (Pduf)
    {
        case CANSETUP_01 :
            Menu.Langage = focan_rx_frame.data[0];
            Menu.Area = focan_rx_frame.data[1];
            Menu.Profile = focan_rx_frame.data[2];
        
            switch (focan_rx_frame.data[3]*256 + focan_rx_frame.data[4])
            {
                case 0: Menu.CableSection = 6; break;
                case 1: Menu.CableSection = 10; break;    
                case 2: Menu.CableSection = 16; break;
                case 3: Menu.CableSection = 25; break;
                case 4: Menu.CableSection = 35; break;
                case 5: Menu.CableSection = 50; break;
                case 6: Menu.CableSection = 70; break;
                case 7: Menu.CableSection = 95; break;
            }
        
            if (Menu.Area == 0)
                Menu.CableLength = focan_rx_frame.data[5]*256 + focan_rx_frame.data[6];
            else
                Menu.CableLength = (((uint)100 * (focan_rx_frame.data[5]*256 + focan_rx_frame.data[6])) + 50) / 33;
        
            if ((focan_rx_frame.data[7] & 0x01) != 0)
                Menu.AutoStartOffOn =  0;
            else
                Menu.AutoStartOffOn =  1;
        break;
        
        case CANSETUP_02 :
            format = focan_rx_frame.data[0];
            Menu.BatCap = focan_rx_frame.data[1]*256 + focan_rx_frame.data[2];
            if (format ==0)
            {
                Menu.BatTemp = (schar)focan_rx_frame.data[3];
                Menu.BatTempF = ((schar)focan_rx_frame.data[3] * 9) / 5 + 32;
            }
            else
            {
                Menu.BatTemp = (((((schar)focan_rx_frame.data[3] - 32) * 50) + 50) / 90);
                Menu.BatTempF = focan_rx_frame.data[3];
            }
            Menu.CapAutoManu = focan_rx_frame.data[4] & 0x01;
            Menu.InitDOD = focan_rx_frame.data[5];
            if (focan_rx_frame.data[6] != 0)
                Menu.CellSize = focan_rx_frame.data[6];
            else
                Menu.CellSize = 255;
            Menu.BatteryRest = focan_rx_frame.data[7];
        break;
        
        case CANSETUP_03 :
            switch (focan_rx_frame.data[0])
    		{
        		case 0:
        			Menu.ChgDelayHour = 0;
        			Menu.ChgSkipDelay = 0;
        			break;
        		case 1:
        			Menu.ChgDelayHour = 1;
        			Menu.ChgSkipDelay = 0;
        			break;
        		case 2:
        			Menu.ChgSkipDelay = 1;
        			break;
    		}
            Menu.ChgDelayVal = focan_rx_frame.data[1]*256 + focan_rx_frame.data[2];
            Menu.FloatingOffOn = focan_rx_frame.data[3] & 0x01; 
            Menu.Vfloating = focan_rx_frame.data[4]*256 + focan_rx_frame.data[5];
            Menu.Ifloating = focan_rx_frame.data[6]*256 + focan_rx_frame.data[7];
        break;
        
        case CANSETUP_04 :
            Menu.EqualDelay = focan_rx_frame.data[0];
            Menu.EqualTime = focan_rx_frame.data[1];
            Menu.EqualPeriod = focan_rx_frame.data[2];
            Menu.Iequal = (focan_rx_frame.data[3]*256 + focan_rx_frame.data[4])*10;
            Menu.RefreshOffOn = focan_rx_frame.data[5] & 0x01;
            Menu.ImaxCharger = (focan_rx_frame.data[6]*256 + focan_rx_frame.data[7])*10;
        break;
        
        case CANSETUP_05 :
            Menu.ChgDailyStart = focan_rx_frame.data[0]*256 + focan_rx_frame.data[1];
            Menu.ChgDailyEnd = focan_rx_frame.data[2]*256 + focan_rx_frame.data[3];
            if ((focan_rx_frame.data[4] & 0x01) != 0)
                Menu.ChgSkipDaily = 0;
            else
                Menu.ChgSkipDaily = 1;
            Menu.XFCOffOn = focan_rx_frame.data[5] & 0x01;
            Menu.Daylight =  focan_rx_frame.data[6];
			Menu.PLCPulseOffOn = focan_rx_frame.data[7];
        break;
        
        case CANSETUP_06 :
            Menu.BlockOutStart = focan_rx_frame.data[0]*256 + focan_rx_frame.data[1];
            Menu.BlockOutEnd = focan_rx_frame.data[2]*256 + focan_rx_frame.data[3];
            if ((focan_rx_frame.data[4] & 0x01) != 0)
                Menu.BlockOutEnable = 1;
            else
                Menu.BlockOutEnable = 0;
            if ((focan_rx_frame.data[5]*256 + focan_rx_frame.data[6]) != 0)
            {
                Menu.TimeDisplaySave = focan_rx_frame.data[5]*256 + focan_rx_frame.data[6];
                Menu.Status |= BitBackOnOff;
            }
            else
            {
                Menu.Status &= ~BitBackOnOff;
            }
            // MODIF 2.8
            Menu.Theme = focan_rx_frame.data[7];
        break;
        
        case CANSETUP_07 :
            format = focan_rx_frame.data[0];
            if (format ==0)
            {
                Menu.BatHighTemp= focan_rx_frame.data[1];
            }
            else
            {
                Menu.BatHighTemp = ((((focan_rx_frame.data[1] - 32) * 50) + 50) / 90);
            }
            Menu.EVtime = focan_rx_frame.data[2]*256 + focan_rx_frame.data[3];
            if ((focan_rx_frame.data[4] & 0x01) != 0)
                Menu.SkipComIQ = 0;
            else
                Menu.SkipComIQ = 1;
            if ((focan_rx_frame.data[5] & 0x01) != 0)    
            {
                IdleStateOn;
			    IdleOn;
            }
            else
            {
                IdleStateOff;
			    IdleOff;
            }
            Menu.IonicCoef = focan_rx_frame.data[6];
            switch (focan_rx_frame.data[7])
            {
                case 0: // 120VAC
                    Menu.VMains = 1;
                break;
                 
                case 1: // 208-240VAC
                    Menu.VMains = 0;
                break;
                
                case 2: // 400VAC
                    Mains480VACOff;
                break;
                
                case 3: // 480VAC
                    Mains480VACOn;
                break;
            }
        break;
    }
    MenuWriteOn;
}


/**
 * @brief  Read serial number and send to CANBUS
 * @param  Destination address, Pduf (PGN 256/512)
 *
 * @retval
 */
void J1939_ReadSerialNumber (uchar id_da, uchar Pduf)
{
	int i;
	uchar id_sa;

	id_sa = 0;

	for (i=0; i<8; i++)
	{
		can_tx_dataframe[3].data[i] = 0;
	}
	DISABLE_IRQ();
	can_tx_dataframe[3].id = 0x18000000 + (Pduf << 16) + (id_da << 8) + id_sa;
	can_tx_dataframe[3].dlc = 8;

	if (Pduf == CANSERIAL1)
	{
		for (i=0; i<4; i++)
		{
			can_tx_dataframe[3].data[4+i] = SerialNumber.SerNum[i];
		}
	}
	else if (Pduf == CANSERIAL2)
	{
		for (i=0; i<8; i++)
		{
			can_tx_dataframe[3].data[i] =  SerialNumber.SerNum[4+i];
		}
	}

	ENABLE_IRQ();
	TimerCANRamRW = State.TimerMs + TIMEOUT_RAMRW;  // delay RamRW writing
	api_status = R_CAN_TxSet(CH_0, (CANBOX_0_RAMRW_TX), &can_tx_dataframe[3], DATA_FRAME); 
}

/**
* @brief  Read serial number and send to CANBUS
* @param  Destination address, Pduf (PGN 256/512)
*
* @retval
*/
void J1939_ReadTypeBay (uchar id_da, uchar Pduf)
{
    int i;
    uchar id_sa;

    id_sa = 0;

    for (i=0; i<8; i++)
    {
        can_tx_dataframe[3].data[i] = 0;
    }
    DISABLE_IRQ();
    can_tx_dataframe[3].id = 0x18000000 + (Pduf << 16) + (id_da << 8) + id_sa;
    can_tx_dataframe[3].dlc = 8;
    
    can_tx_dataframe[3].data[0] =  ModNumber;   // Number of module
    #ifdef ENERSYS_EU
    can_tx_dataframe[3].data[1] = 0x11;         // LIFE-IQ
    #endif
    #ifdef ENERSYS_US
    can_tx_dataframe[3].data[1] = 0x12;         // IMPAQ+
    #endif
    switch (ModNumber)
    {
        #ifdef ENERSYS_EU
        case 1:
        case 2:
            can_tx_dataframe[3].data[2] = 'G';
        break;
        case 3:
        case 4:
            can_tx_dataframe[3].data[2] = 'H';
        break;
        case 5:
        case 6:
            can_tx_dataframe[3].data[2] = 'I';
        break;
        #endif
        #ifdef ENERSYS_US
        case 1:
        case 2:
            can_tx_dataframe[3].data[2] = 'X';
        break;
        case 3:
        case 4:
            can_tx_dataframe[3].data[2] = 'Y';
        break;
        case 5:
        case 6:
            can_tx_dataframe[3].data[2] = 'Z';
        break;
        #endif       
    }
    
    #ifdef ENERSYS_EU
    can_tx_dataframe[3].data[3] = 'E';
    #else
    can_tx_dataframe[3].data[3] = 'U';
    #endif
    
    ENABLE_IRQ();
    TimerCANRamRW = State.TimerMs + TIMEOUT_RAMRW;  // delay RamRW writing
    api_status = R_CAN_TxSet(CH_0, (CANBOX_0_RAMRW_TX), &can_tx_dataframe[3], DATA_FRAME); 
}


/**
 * @brief  Writing Menu config over CANBUS
 * @param
 *
 * @retval
 */
void J1939_WriteMenuConfig (uchar Pduf)
{
    uchar offset;
    char *ptr;
    
    offset = Pduf - CANMENU;  
    ptr = (char *)&(Menu) + (offset * 8);
    
    memcpy (ptr, &(focan_rx_frame.data[0]), focan_rx_frame.dlc);
    MenuWriteOn;
}

/**
 * @brief  Read Menu config and send to CANBUS
 * @param  Destination address, Pduf
 *
 * @retval
 */
void J1939_ReadMenuConfig (uchar id_da, uchar Pduf)
{
	int i;
	uchar id_sa;
    uchar offset;
    char *ptr;
    
    offset = Pduf - CANMENU;
	id_sa = 0;

	for (i=0; i<8; i++)
	{
		can_tx_dataframe[3].data[i] = 0;
	}
	DISABLE_IRQ();
	can_tx_dataframe[3].id = 0x18000000 + (Pduf << 16) + (id_da << 8) + id_sa;
	can_tx_dataframe[3].dlc = 8;

	ptr = (char *)&(Menu) + (offset * 8);
	memcpy (&can_tx_dataframe[3].data[0], ptr, can_tx_dataframe[3].dlc);

	ENABLE_IRQ();
	TimerCANRamRW = State.TimerMs + TIMEOUT_RAMRW;  // delay RamRW writing
	api_status = R_CAN_TxSet(CH_0, (CANBOX_0_RAMRW_TX), &can_tx_dataframe[3], DATA_FRAME); 
}


/**
 * @brief  CAN initialization
 * @param
 *
 * @retval
 */
void Init_CAN (void)
{
	/* Init CAN and FoCAN slots. */
	api_status = FoCanInit(CH_0, EXT_ID_MODE);
	TimerCANRamRW = 0;
	//TimeoutCAN = 0;
    TimeoutCAN = State.TimerSec + TIMEOUT_CANBUS; // MODIF R2.2
    CntCANFault = 0;
    // MODIF 2.8 : add address 36 management (module 8)
    id_sa_36 = 0;
}

/**
 * @brief  Timer CAN Reset
 * @param
 *
 * @retval
 */
void Reset_IdleTimerCAN (void)
{
	TimeoutCAN = State.TimerSec + TIME_CHECK_IDLE;
}


/**
 * @brief  read LAN setup charger over CANBUS
 * @param  Destination address, Pduf
 *
 * @retval
 */
void J1939_ReadLAN(uchar id_da, uchar Pduf)
{
    int i;
    uchar id_sa;
	
    Temp = (StructLanConfig *)&LcdTemp[0];
    TempWifi = (StructWifiConfig *)&LcdTemp[0];
    
    id_sa = 0;
    
    // Force charger to stop charge    
    StartStopOff;

    for (i=0; i<8; i++)
    {
        can_tx_dataframe[3].data[i] = 0;
    }
    DISABLE_IRQ();
    can_tx_dataframe[3].id = 0x18000000 + (Pduf << 16) + (id_da << 8) + id_sa;
    can_tx_dataframe[3].dlc = 8;

    switch (Pduf)
    {
        case CANLAN_01 :
        Read_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
        for (i=0; i<8; i++)
        {
            can_tx_dataframe[3].data[i] = (*Temp).EthParam[i];
        }
        break;
        
        case CANLAN_02 :
        Read_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
        for (i=0; i<8; i++)
        {
            can_tx_dataframe[3].data[i] = (*Temp).EthParam[i+8];
        }
        break;
        
        case CANLAN_03 :
        Read_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
        if (Menu.Network == LAN)                            // LAN enable
            can_tx_dataframe[3].data[0] = 1;
        can_tx_dataframe[3].data[1] = (*Temp).A10100;       // Speed: 0=Auto / 1=10M / 2=100M
        can_tx_dataframe[3].data[2] = (*Temp).Security;     // Security: 0=None 3=WPA-PSK
        break;
        
        case CANLAN_04 :
        Read_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
        for (i=0; i<8; i++)
        {
            can_tx_dataframe[3].data[i] = (*Temp).SSID[i];
        }
        break;
        
        case CANLAN_05 :
        Read_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
        for (i=0; i<8; i++)
        {
            can_tx_dataframe[3].data[i] = (*Temp).SSID[i+8];
        }
        break;
        
        case CANLAN_06 :
        Read_SegFlashData (SEGWIFI, (uchar *)TempWifi);
        for (i=0; i<8; i++)
        {
            can_tx_dataframe[3].data[i] = (*TempWifi).WLanKey[i];
        }
        break;
        
        case CANLAN_07 :
        Read_SegFlashData (SEGWIFI, (uchar *)TempWifi);
        for (i=0; i<8; i++)
        {
            can_tx_dataframe[3].data[i] = (*TempWifi).WLanKey[i+8];
        }
        break;
    }
    
    ENABLE_IRQ();
    TimerCANRamRW = State.TimerMs + TIMEOUT_RAMRW;  // delay RamRW writing
    api_status = R_CAN_TxSet(CH_0, (CANBOX_0_RAMRW_TX), &can_tx_dataframe[3], DATA_FRAME);         
}

/**
 * @brief  Writing LAN setup over CANBUS
 * @param
 *
 * @retval
 */
void J1939_WriteLAN (uchar Pduf)
{
    int i;
    switch (Pduf)
    {
        case CANLAN_01 :    // IP address + DNS
        Read_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
        for (i=0; i<8; i++)
        {
            (*Temp).EthParam[i] = focan_rx_frame.data[i];
        }
        Write_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
        break;
        
        case CANLAN_02 :    // Gateway + subnet-mask
        Read_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
        for (i=0; i<8; i++)
        {
            (*Temp).EthParam[i+8] = focan_rx_frame.data[i];
        }
        Write_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
        break;
        
        case CANLAN_03 :
        Read_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
        if (focan_rx_frame.data[0] == 1)    // Network On-Off
            Menu.Network = 1;
        switch (focan_rx_frame.data[1])     // Speed    
        {
            case 0:
            case 1:
            case 2:
                (*Temp).A10100 = focan_rx_frame.data[1];
            break;
        }
        switch (focan_rx_frame.data[2])     // Security   
        {
            case 0:
            case 3:
                (*Temp).Security = focan_rx_frame.data[2];
            break;
        }
        Write_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
        MenuWriteOn;
        break;
        
        case CANLAN_04 :    // SSID (8 caract)
        Read_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
        for (i=0; i<8; i++)
        {
            (*Temp).SSID[i] = focan_rx_frame.data[i];
        }
        Write_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
        break;
        
        case CANLAN_05 :    // SSID (next 8 caract)
        Read_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
        for (i=0; i<8; i++)
        {
            (*Temp).SSID[i+8] = focan_rx_frame.data[i];
        }
        Write_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
        break;
        
        case CANLAN_06 :    // Passphrase (8 caract)
        Read_SegFlashData (SEGWIFI, (uchar *)TempWifi);
        for (i=0; i<8; i++)
        {
            (*TempWifi).WLanKey[i] = focan_rx_frame.data[i];
        }
        Write_SegFlashData (SEGWIFI, (uchar *)TempWifi);
        break;
        
        case CANLAN_07 :    // Passphrase (next 8 caract)
        Read_SegFlashData (SEGWIFI, (uchar *)TempWifi);
         for (i=0; i<8; i++)
        {
            (*TempWifi).WLanKey[i+8] = focan_rx_frame.data[i];
        }
        Write_SegFlashData (SEGWIFI, (uchar *)TempWifi);
        break;
    }
}

/**
 * @brief  CAN transmit interrupt routine
 * @param
 *
 * @retval
 */
#pragma interrupt CAN0_TXM0_ISR(vect=VECT_CAN0_TXM0, enable)
void CAN0_TXM0_ISR(void)
{
	nop();
}


/**
 * @brief  CAN receive interrupt routine
 * @param
 *
 * @retval
 */
#pragma interrupt CAN0_RXM0_ISR(vect=VECT_CAN0_RXM0, enable)
void CAN0_RXM0_ISR(void)
{
	uint32_t api_status = R_CAN_OK;


	api_status = R_CAN_RxPoll(CH_0, CANBOX_0_MONITORING);
	if (api_status == R_CAN_OK)
	{
		api_status = R_CAN_RxRead(CH_0, CANBOX_0_MONITORING, &focan_rx_frame);
		if (!api_status)
		{
			J1939_Read();
		}
	}

	api_status = R_CAN_RxPoll(CH_0, CANBOX_0_REMOTE_REQUEST);
	if (api_status == R_CAN_OK)
	{
		api_status = R_CAN_RxRead(CH_0, CANBOX_0_REMOTE_REQUEST, &focan_rx_remote);
		if (!api_status)
		{
			J1939_ReadRemoteRequest();
		}
	}
}


/**
 * @brief  CAN Group Error interrupt
 * @param
 *			Check which CAN channel is source of interrupt
 * @retval
 */
#pragma interrupt	CAN_ERS_ISR(vect=VECT_ICU_GROUPE0, enable)
void CAN_ERS_ISR(void)
{
	/* Error interrupt can have multiple sources. Check interrupt flags to id source. */
	if (IS(CAN0, ERS0))
	{
		/*TODO: additional error handling/cause identification */
		CLR(CAN0, ERS0) = 1;	/* clear interrupts */
	}
	if (IS(CAN1, ERS1))
	{
		/*TODO: additional error handling/cause identification */
		CLR(CAN1, ERS1) = 1;	/* clear interrupts */
	}
	if (IS(CAN2, ERS2))
	{
		/*TODO: additional error handling/cause identification */
	}
	nop();
}


#pragma inline_asm ENABLE_IRQ

/******************************************************************************
Function Name   : ENABLE_IRQ
Description     : Enable IRQ
Arguments       : none
Return value    : none
 ******************************************************************************/
static void ENABLE_IRQ(void)
{
	SETPSW I
}


/******************************************************************************
Pragma variables
 ******************************************************************************/
#pragma inline_asm DISABLE_IRQ

/******************************************************************************
Function Name   : DISABLE_IRQ
Description     : Disable IRQ
Arguments       : none
Return value    : none
 ******************************************************************************/
static void DISABLE_IRQ(void)
{
	CLRPSW I
}
