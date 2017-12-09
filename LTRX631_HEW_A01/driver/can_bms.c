/*
 * can_bms.c
 *
 *  Created on: 19/10/2017
 *      Author: Francis DUHAUT
 */

#include "include.h"

can_frame_t can1_tx_dataframe[BMS_MAX_TX_FRAME];
can_frame_t can1_rx_remote;

uint32_t can1_api_status;
uint32_t can1_timer_tx_bms;

uint32_t TimerBmsRX;
uint16_t ChrgrDcUMem = 0;

/**
 * @brief  CAN bms initialization
 * @param
 *
 * @retval
 */
void Init_CAN_bms (void)
{
	can1_api_status = FoCanInit(CH_1, STD_ID_MODE);
	can1_timer_tx_bms = TIME_TX_BMS;
	TimerBmsRX = TIME_RX_BMS;
}


/**
 * @brief  CAN bms chrono task
 * @param
 *
 * @retval
 */
void CAN_bms_Chrono (void)
{
	if (can1_timer_tx_bms == 0)
	{
		can1_timer_tx_bms = TIME_TX_BMS;
		CAN_bms_TX_ACT();
		CAN_bms_TX_STS(); 
	}
	if (TimerBmsRX == 0)
	{
		memset(&Bms, 0 , sizeof(Bms));
		BmsStateOff;
		if (ChgData.Profile == LITH)
			StartStopOff;
	}
}

/**
 * @brief  CAN bms TX ACT data
 * @param
 *
 * @retval
 */
void CAN_bms_TX_ACT (void) 
{
	uint8_t i;
	
    for (i=0; i<8; i++)
    {
        can1_tx_dataframe[BMS_TX_ACT_FRAME].data[i] = 0;
    }
	can1_tx_dataframe[BMS_TX_ACT_FRAME].id = BMS_TX_ACT;
	can1_tx_dataframe[BMS_TX_ACT_FRAME].dlc = 8;
	can1_tx_dataframe[BMS_TX_ACT_FRAME].data[4] = (uchar)(((DataLcd.Vbat/10) & 0xFF00) >> 8);
	can1_tx_dataframe[BMS_TX_ACT_FRAME].data[5] = (uchar)((DataLcd.Vbat/10) & 0x00FF);
	can1_tx_dataframe[BMS_TX_ACT_FRAME].data[6] = (uchar)(((DataLcd.Ibat/10) & 0xFF00) >> 8);
	can1_tx_dataframe[BMS_TX_ACT_FRAME].data[7] = (uchar)((DataLcd.Ibat/10) & 0x00FF);
	can1_api_status = R_CAN_TxSet(CH_1, (CANBOX_1_RAMRW_TX), &can1_tx_dataframe[BMS_TX_ACT_FRAME], DATA_FRAME);
}

/**
 * @brief  CAN bms TX STS data
 * @param
 *
 * @retval
 */
void CAN_bms_TX_STS (void) 
{
	uint8_t i;
	
    for (i=0; i<8; i++)
    {
        can1_tx_dataframe[BMS_TX_STS_FRAME].data[i] = 0;
    }
	can1_tx_dataframe[BMS_TX_STS_FRAME].id = BMS_TX_STS;
	can1_tx_dataframe[BMS_TX_STS_FRAME].dlc = 8;
	can1_api_status = R_CAN_TxSet(CH_1, (CANBOX_1_RAMRW_TX), &can1_tx_dataframe[BMS_TX_STS_FRAME], DATA_FRAME);
}

/**
 * @brief  CAN bms read message
 * @param
 *
 * @retval
 */
void CAN_bms_read (can_frame_t* frame_p)
{
	ulong i;
	switch (frame_p->id)
	{
	case BMS_CHRGR_CTL :
		// Clear timeout and update BMS status
		TimerBmsRX = TIME_RX_BMS;
		BmsStateOn;

		// For Lithium profile
		if (Menu.Profile != LITH)
		{
			ChgData.Profile = LITH;
			Menu.Profile = LITH;
			MenuWriteOn;
		}
		Bms.ChrgrEna = frame_p->data[0] >> 7;
		if (Bms.ChrgrEna != 0)
			StartStopOn;
		else
			StartStopOff;
		Bms.ChrgrAcMax = (frame_p->data[1] << 8) + frame_p->data[2];
		Bms.ChrgrDcU = ((frame_p->data[3] << 8) + frame_p->data[4])*10;
		Bms.ChrgrDcI = ((frame_p->data[5] << 8) + frame_p->data[6]);
		if (Bms.ChrgrDcU != ChrgrDcUMem)
		{
			ChrgrDcUMem = Bms.ChrgrDcU;
			ModResetEfficiency();
		}


		// control value
		//if (Bms.ChrgrDcU > (Menu.NbCells * 420))
		//	Bms.ChrgrDcU = 0;
		if (Bms.ChrgrDcI > Menu.Imax)
			Bms.ChrgrDcI = Menu.Imax;
		break;

	case BMS_SOC_DATA :
		i = ((frame_p->data[0] + ((frame_p->data[1] & 0x0003) << 8)) * 125) / 1000;
		Bms.Soc = (uint)i;
		i = (((frame_p->data[1] >> 2) + ((frame_p->data[2] & 0x000F) << 8)) * 125) / 1000;
		Bms.Soh = (uint)i;
		break;

	case BMS_BATTP_DATA :
		Bms.Temp = ((sint)((frame_p->data[2] >> 4) + ((frame_p->data[3] & 0x003F) << 4)) * 250) / 1000;
		break;

	case BMS_BATP_DATA :
		Bms.Pbat = (sint)(((frame_p->data[0] + ((frame_p->data[1] & 0x1F) << 8)) * 125) / 10);	// 121 = 1,21kW
		Bms.Ibat = (sint)(((frame_p->data[2] + ((frame_p->data[3] & 0x1F) << 8)) * 500) / 100);	// 525 = 52,5A
		break;

	case BMS_BATU_DATA :
		Bms.U_Pack = (frame_p->data[0] + ((frame_p->data[1] & 0x0F) << 8)) * 25;	// 0,01V
		break;

	default :
		break;
	}
}

/**
 * @brief  CAN bms TX ISR
 * @param
 *
 * @retval
 */
#pragma interrupt CAN1_TXM1_ISR(vect=VECT_CAN1_TXM1, enable)
void CAN1_TXM1_ISR(void)
{
	nop();
}


/**
 * @brief  CAN bms RX ISR
 * @param
 *
 * @retval
 */
#pragma interrupt CAN1_RXM1_ISR(vect=VECT_CAN1_RXM1, enable)
void CAN1_RXM1_ISR(void)
{
	/* Use CAN API. */
	uint32_t api_status = R_CAN_OK;

	/* Start process of CANBOX_MONITORING. */
	api_status = R_CAN_RxPoll(CH_1, CANBOX_1_MONITORING);
	if (api_status == R_CAN_OK)
	{
		/* Something in CANBOX_MONITORING. Get it. */
		api_status = R_CAN_RxRead(CH_1, CANBOX_1_MONITORING, &can1_rx_remote);
		if (!api_status)
		{
			CAN_bms_read (&can1_rx_remote);
		}
	}
	/* end process of CANBOX_MONITORING. */
}
