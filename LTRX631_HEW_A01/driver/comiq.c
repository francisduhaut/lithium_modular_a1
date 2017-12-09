#include "include.h"

#define IQ_MAX_CUR    600    // 60A

// Timer - Counter
ulong TimeoutIQ;
ulong TimeCurStep;
uchar CntTimeoutIQ;
uchar CntReqIQ;
uint  IlinkIQ;
ulong TimeWIIQScan;
ulong TimeWIIQMeas;

// Rx - Tx
uchar U2BufRx;	    /* Uart6 input buffer */
uchar U2BufTx;	    /* Uart6 output buffer */
uchar U2PtrTx;
uchar U2LenTx;
uchar Value;
uchar Inc;
uchar IQFrameTx[MAX_LEN];
uchar IQFrameRx[MAX_LEN];
uchar IQLenTx;
uchar IQLenRx;
uchar CntStartOffScan;
ulong WIIQAddrTmp;
uint  VtolIQ;


// Structure 
extern StructIQData IQData;
extern StructAarsParam AarsParam;
extern uchar IQSeg[64];
extern ulong ScanAddrArray[NB_SCAN_ADDR];

uchar VncRfPtr; 

extern uint		SlopeI;
extern uint  	ItoReach;

// MODIF R2.6
uint IbatIQ;
// MODIF 2.8
uchar CntHighTpBat;

// MODIF 3.3
void Reset_SCI6(void);

/**
 * @brief  Set On/OFF RfDongle
 * @param
 *
 * @retval
 */
void RFDongle (uchar state)
{
	PORT4.PDR.BIT.B7 = 1;
	PORT4.PODR.BIT.B7 = state;
	PORT8.PDR.BIT.B4 = 1;
	PORT8.PODR.BIT.B4 = state;
}

// MODIF 3.3
void Reset_SCI6(void)
{
	RFDongle(OFF);
	
    /* Set bits TIE, RIE, RE and TE. Enable receive interrupts */
    IEN(SCI6,RXI6) = 0x1u;
	IEN(SCI6,TXI6) = 0x1u;
	/* Clear SCI6 RXI6/TXI6/TEI6 interrupt flag */
	IR(SCI6,RXI6) = 0x0u;
	IR(SCI6,TXI6) = 0x0u;
	IR(SCI6,TEI6) = 0x0u;
	
	SCI6.SCR.BYTE = 0xF0u;
	SCI6.SSR.BYTE &= 0xC7u;					/* Clear bits ORER, PER FER */
	
	RFDongle(ON);
	
	// Rx - Tx
	memset(IQFrameRx, 0, 100);
	memset(IQFrameTx, 0, 100);
	IQLenTx = 0;
	IQLenRx = 0;
	U2PtrTx = 0;
	U2LenTx = 0;
	Value = 0;
	Inc = 16;

	IQData.ScanAddrLen = 0;
	IQData.ScanAddrPtr = 0;
}

/**
 * @brief  Init SCI for IQ communciation
 * @param
 *
 * @retval
 */
void Init_COMIQ(void)
{
	uint c;

	RFDongle(OFF);
	VncRfPtr = 0;


	c = Read_SegFlashData (SEGAARS, (uchar *)&AarsParam);
	if ((AarsParam.TypeSegment != TYPE_AARS) || (AarsParam.NumSegment != SEGAARS) || (AarsParam.Crc != c))
	{
		(void)memset((void *)&AarsParam, 0, sizeof(AarsParam));
	}

	// Initialise SCI6 to 9600bds
	/* Protection off */
	SYSTEM.PRCR.WORD = 0xA503u;

	/* Cancel the SCI6 module clock stop mode */
	MSTP_SCI6 = 0u;

	/* Protection on */
	SYSTEM.PRCR.WORD = 0xA500u;

	/* Configure SCI6's TXD6 and RXD6 pins for peripheral functions - see page 661  */
	PORT0.PMR.BIT.B0 = 1;
	PORT0.PMR.BIT.B1 = 1;
	PORT0.PCR.BIT.B1 = 1;

	/* PWPR.PFSWE write protect off */
	MPC.PWPR.BYTE = 0x00u;

	/* PFS register write protect off */
	MPC.PWPR.BYTE = 0x40u;

	/* Configure SCI6's communication
	 * pins P00 = TXD6 / P01 = RXD6 on LQFP176
	 * see page 691/712 on datasheet */

	/* Set Port0's pin 0 function to TXD6 - see page 691/712 */
	MPC.P00PFS.BYTE = 0x0Au;
	/* Set Port0's pin 1 function to RXD6 - see page 691/712 */
	MPC.P01PFS.BYTE = 0x0Au;

	/* Turn on PFS register write protect */
	MPC.PWPR.BYTE = 0x80u;

	/* Cancel SCI6 module stop mode */
	SYSTEM.MSTPCRB.LONG = 0x7FFFFFFFu;

	/* Disable All bits and CKE0,1 = 00 */
	SCI6.SCR.BYTE = 0x00u;

	/* Set output port TXD6(P00) */
	PORT0.PDR.BIT.B0 = 1u;
	/* Set Receive port RXD6(P01)*/
	PORT0.PDR.BIT.B1 = 0u;

	/* disable All bits and CKE0,1 = 00 once more */
	SCI6.SCR.BYTE = 0x00u;

	/* CKS[0-1] = 00: PCLK 0 div
     1Stop 8bits non-parity     */
	SCI6.SMR.BYTE = 0x00u;

	/* SMIF = 0: Serial  */
	SCI6.SCMR.BYTE = 0xF2u;

	/* PCLK = 48MHz, Set baudrate to 9600 bps
     155 = 48MHz / (64*0.5*9600) -1 */
	SCI6.BRR = 155u;

	/* Allow a small delay */
	for (uint16_t i = 0u; i < 20u; i++ )
	{
	}

	/* Clear bits ORER, PER and FER */
	SCI6.SSR.BYTE &= 0xC7u;

	/* Set SCI6 RXI6/TXI6/TEI6 interrupt priority level to 7 */
	IPR(SCI6,RXI6) = 0x3u;
	IPR(SCI6,TXI6) = 0x3u;
	IPR(SCI6,TEI6) = 0x3u;
	/* Enable SCI6 RXI6/TXI6/TEI6 interrupts */
	IEN(SCI6,RXI6) = 0x1u;
	IEN(SCI6,TXI6) = 0x1u;
	IEN(SCI6,TEI6) = 0x0u;
	/* Clear SCI6 RXI6/TXI6/TEI6 interrupt flag */
	IR(SCI6,RXI6) = 0x0u;
	IR(SCI6,TXI6) = 0x0u;
	IR(SCI6,TEI6) = 0x0u;

	/* Set bits TIE, RIE, RE and TE. Enable receive interrupts */
	SCI6.SCR.BYTE = 0xF0u;
	SCI6.SSR.BYTE &= 0xC7u;					/* Clear bits ORER, PER FER */
	
	RFDongle(ON);

	// Rx - Tx
	memset(IQFrameRx, 0, 100);
	memset(IQFrameTx, 0, 100);
	IQLenTx = 0;
	IQLenRx = 0;
	U2PtrTx = 0;
	U2LenTx = 0;
	Value = 0;
	Inc = 16;

	// Scan
	memset(&IQData, 0 , sizeof(IQData));
	memset(ScanAddrArray, 0, NB_SCAN_ADDR);
	IQData.ScanAddrLen = 0;
	IQData.ScanAddrPtr = 0;
}

/**
 * @brief  SCI ComIQ main task
 * @param  None
 * @retval None
 */
void SCIIQ_Chrono(void)
{
	if (VncRfPtr == 0)
		IQChrono();
}

/**
 * @brief  IQInitRx
 * @param  None
 * @retval None
 */
void IQInitRx(void)
{
	SCI6.SCR.BIT.RIE = 0;                   /* Receive int disable */
	memset(IQFrameRx,0,100);
	IQLenRx = 0;
	IQRxRecvOff;
	IQRxTimeoutOff;
	IQInFrameRxOn;
	SCI6.SCR.BIT.RIE = 1;                   /* Receive int enable */
}

/**
 * @brief  IQInitTx
 * @param  None
 * @retval None
 */
void IQInitTx(uchar ReqType)
{
	switch (ReqType)
	{
	case JBUS_ANS:
		U2BufTx = 'J';
		break;
	case INT_REQ:
		U2BufTx = 'U';
		break;
	case EXT_REQ_WITHOUT_ANSWER:
		U2BufTx = 'V';
		break;
	case EXT_REQ_WITH_ANSWER:
		U2BufTx = 'W';
		break;
	}
	U2LenTx = (IQLenTx * 2);
	IQTxTimeoutOff;
	IQInFrameTxOn;
	U2PtrTx = 0;	// Pointer on 1st byte to transmit
	SCI6.SCR.BIT.TIE = 1;                   /* transmit int enable */
	SCI6.TDR = U2BufTx;
}


// IQFrameTxInit
//--------------
void IQFrameTxInit(void)
{
	switch (IQData.State1)
	{
    case State1WriteJ1939Adr:
        IQFrameTx[0] = 0x20;
        IQFrameTx[1] = 0x01;
        IQFrameTx[2] = 128;
	    IQLenTx = 3;
    break;
            
	case State1RFTrAddr:
		// Reset RF Transceiver address
		IQData.RFTrAddr = 0;
		// Init FrameTx
		IQFrameTx[0] = 0x01;
		IQLenTx = 1;
		break;
	case State1RFTrVers:
		// Reset RF Transceiver version
		memset(&IQData.RFTrVers[0], 0, 4);
		// Init FrameTx
		IQFrameTx[0] = 0x04;
		IQLenTx = 1;
		break;
	case State1RFTrModAddr:
		// Init FrameTx
		IQFrameTx[0] = 0x02;
		IQFrameTx[1] = (IQData.RFTrAddr >> 16) & 0x0000000FF;
		IQFrameTx[2] = (IQData.RFTrAddr >> 8) & 0x000000FF;
		IQFrameTx[3] = IQData.RFTrAddr & 0x000000FF;
		IQFrameTx[4] = (IQData.RFTrNewAddr >> 16) & 0x0000000FF;
		IQFrameTx[5] = (IQData.RFTrNewAddr >> 8) & 0x000000FF;
		IQFrameTx[6] = IQData.RFTrNewAddr & 0x000000FF;
		IQLenTx = 7;
		break;
	case State1RFTrCom:
		// Init FrameTx
		IQFrameTx[0] = 0x09;
		IQFrameTx[1] = IQData.RFTrCom;
		IQLenTx = 2;
		IQData.RFTrCom = OFF;
		break;
	case State1RFTrSaveWIIQ:
		// Reset IQ segment
		memset(&IQSeg, 0, 64);
		// Init FrameTx
		IQFrameTx[0] = 0x0A;
		IQFrameTx[1] = (IQData.WIIQAddr >> 16) & 0x0000000FF;
		IQFrameTx[2] = (IQData.WIIQAddr >> 8) & 0x0000000FF;
		IQFrameTx[3] = IQData.WIIQAddr & 0x0000000FF;
		IQLenTx = 4;
		break;
	case State1RFTrLoadWIIQ:
		// Init FrameTx
		IQFrameTx[0] = 0xF6;
		IQFrameTx[1] = DataW.IQWIIQAddr & 0x00FF;
		IQFrameTx[2] = (DataW.IQSegAddr >> 8) & 0x00FF;
		IQFrameTx[3] = DataW.IQSegAddr & 0x00FF;
		IQLenTx = 4;
		break;
	case State1RFTrEraseFlash:
		// Init FrameTx
		IQFrameTx[0] = 0xF7;
		IQLenTx = 1;
		break;
	case State1Measure:
		// Reset IQData
		IQData.Ibat = 0;
		IQData.Vbat = 0;
		IQData.VbatCell = 0;
		IQData.Vbalance = 0;
		// Init FrameTx
		out24(IQFrameTx[2],IQFrameTx[1],IQFrameTx[0],IQData.WIIQAddr);
		out24(IQFrameTx[5],IQFrameTx[4],IQFrameTx[3],IQData.RFTrAddr);
		IQFrameTx[6] = CODE_MEASURE_REQUEST;
		IQFrameTx[7] = 0x00;
		IQFrameTx[8] = 0xFF;
		IQLenTx = 9;
		break;
	case State1CalCurOffset:
		// Init FrameTx
		out24(IQFrameTx[2],IQFrameTx[1],IQFrameTx[0],IQData.WIIQAddr);
		out24(IQFrameTx[5],IQFrameTx[4],IQFrameTx[3],IQData.RFTrAddr);
		IQFrameTx[6] = CODE_CURRENT_CALIB;
		IQFrameTx[7] = 0x03;
		IQFrameTx[8] = 0x04;
		out16(IQFrameTx[10], IQFrameTx[9], DataR.Ibat);
		IQFrameTx[11] = 0xFF;
		IQLenTx = 12;
		break;
	case State1BatInfo:
		// Reset IQData
		IQData.BatCap = 0;
		IQData.NbCells = 0;
		IQData.NbBalCells = 0;
		IQData.BatTechno = 0;
		IQData.InitSOC = 0;
		IQData.BatWarning = 0;
		memset(&IQData.BatSN[0], SPACE, 14);
		// Init FrameTx
		out24(IQFrameTx[2],IQFrameTx[1],IQFrameTx[0],IQData.WIIQAddr);
		out24(IQFrameTx[5],IQFrameTx[4],IQFrameTx[3],IQData.RFTrAddr);
		IQFrameTx[6] = CODE_BAT_INFO;
		IQFrameTx[7] = 0x00;
		IQFrameTx[8] = 0xFF;
		IQLenTx = 9;
		break;

		// RESET_FLAG_EQUAL
	case  State1ResetFlagEqual :
		out24(IQFrameTx[2],IQFrameTx[1],IQFrameTx[0],IQData.WIIQAddr);
		out24(IQFrameTx[5],IQFrameTx[4],IQFrameTx[3],IQData.RFTrAddr);
		IQFrameTx[6] = CODE_RESET_EQUAL;
		IQFrameTx[7] = 0x00;
		IQFrameTx[8] = 0xFF;
		IQLenTx = 9;
		break;

		// READ TPPL Parameters
	case State1TPPLRead :
		// Init FrameTx
		out24(IQFrameTx[2],IQFrameTx[1],IQFrameTx[0],IQData.WIIQAddr);
		out24(IQFrameTx[5],IQFrameTx[4],IQFrameTx[3],IQData.RFTrAddr);
		IQFrameTx[6] = CODE_TPPL_READ;
		IQFrameTx[7] = 0x00;
		IQFrameTx[8] = 0xFF;
		IQLenTx = 9;
		break;

		// READ EASY KIT
	case State1EasyKitRead :
		// Reset IQData
		//AarsParam.EasyKitAddr = 0x143726;
		IQData.WIIQAddr = AarsParam.EasyKitAddr; // EasyKit Addr
		IQData.BatCap = 0;
		IQData.NbCells = 0;
		IQData.NbBalCells = 0;
		IQData.BatTechno = 0;
		IQData.InitSOC = 0;
		IQData.BatWarning = 0;
		memset(&IQData.BatSN[0], SPACE, 14);
		// Init FrameTx
		out24(IQFrameTx[2],IQFrameTx[1],IQFrameTx[0],IQData.WIIQAddr);
		out24(IQFrameTx[5],IQFrameTx[4],IQFrameTx[3],IQData.RFTrAddr);
		IQFrameTx[6] = CODE_BAT_INFO;
		IQFrameTx[7] = 0x00;
		IQFrameTx[8] = 0xFF;
		IQLenTx = 9;
		break;

    case State1IQSendInChargeData :
        // Init FrameTx
		out24(IQFrameTx[2],IQFrameTx[1],IQFrameTx[0],IQData.WIIQAddr);
		out24(IQFrameTx[5],IQFrameTx[4],IQFrameTx[3],IQData.RFTrAddr);
		IQFrameTx[6] = CODE_DUAL_CHARGE_INFO;
        IQFrameTx[7] = 13;
        IQFrameTx[8] = IN_CHARGE;
        IQFrameTx[9] = 0;
		IQFrameTx[10] = 0;
		IQFrameTx[11] = 0;
		IQFrameTx[12] = 0;
        IQFrameTx[13] = 0;
		IQFrameTx[14] = 0;
        IQFrameTx[15] = 0;
		IQFrameTx[16] = 0;
        IQFrameTx[17] = 0;
		IQFrameTx[18] = 0;
        IQFrameTx[19] = 0;
		IQFrameTx[20] = 0;
		IQFrameTx[21] = 0xFF;
		IQLenTx = 22;
        break;

    case State1IQSendEndChargeData :
        // Init FrameTx
		out24(IQFrameTx[2],IQFrameTx[1],IQFrameTx[0],IQData.WIIQAddr);
		out24(IQFrameTx[5],IQFrameTx[4],IQFrameTx[3],IQData.RFTrAddr);
		IQFrameTx[6] = CODE_DUAL_CHARGE_INFO;
        IQFrameTx[7] = 13;
        IQFrameTx[8] = END_OF_CHARGE;
        IQFrameTx[9] = ((Memo.Iphase1/10) & 0xFF00) >> 8;
		IQFrameTx[10] = (Memo.Iphase1/10) & 0x00FF;
		IQFrameTx[11] = (Memo.ChgAh & 0xFF00) >> 8;
		IQFrameTx[12] = Memo.ChgAh & 0x00FF;
        IQFrameTx[13] = ((Memo.EocCurrent) & 0xFF00) >> 8;
		IQFrameTx[14] = (Memo.EocCurrent) & 0x00FF;
        IQFrameTx[15] = (Memo.ChgWh & 0xFF00) >> 8;
		IQFrameTx[16] = Memo.ChgWh & 0x00FF;
        if (Memo.TypeEoc == PARTIAL)
		    IQFrameTx[17] = 1; // Chg ended partial
		else
		    IQFrameTx[17] = 3; // Chg ended full
		IQFrameTx[18] = DataLcd.ChgSOC & 0x00FF;
        IQFrameTx[19] = (Memo.ChgTime & 0xFF00) >> 8;
		IQFrameTx[20] = Memo.ChgTime & 0x00FF;
		IQFrameTx[21] = 0xFF;
		IQLenTx = 22;
        break;

	case State1CgInfo:
		// Init FrameTx
		out24(IQFrameTx[2],IQFrameTx[1],IQFrameTx[0],IQData.WIIQAddr);
		out24(IQFrameTx[5],IQFrameTx[4],IQFrameTx[3],IQData.RFTrAddr);
		IQFrameTx[6] = CODE_CHARGER_INFO;
		IQFrameTx[7] = 11;
		switch (State.Charger)
		{
		case StateChgerChg:
			switch (State.Charge)
			{
			case StateChgInit:
			case StateChgIQScan:
			case StateChgIQLink:
			case StateChgIQEnd:
			case StateChgStartPhase1:
				IQFrameTx[8] = START_OF_CHARGE;
				IQFrameTx[16] = 0; // Chg in process
				break;
			case StateChgPhase1:
			case StateChgStartPhase2:
			case StateChgPhase2:
			case StateChgStartPhase3:
			case StateChgPhase3:
				IQFrameTx[8] = IN_CHARGE;
				IQFrameTx[16] = 0; // Chg in process
				break;
			default:
				IQFrameTx[8] = END_OF_CHARGE;
				if (Memo.TypeEoc == PARTIAL)
					IQFrameTx[16] = 1; // Chg ended partial
				else
					IQFrameTx[16] = 3; // Chg ended full
				break;
			}
			break;
			default:
				IQFrameTx[8] = END_OF_CHARGE;
				if (Memo.TypeEoc == PARTIAL)
					IQFrameTx[16] = 1; // Chg ended partial
				else
					IQFrameTx[16] = 3; // Chg ended full
				break;
		}
		IQFrameTx[9] = (Memo.ChgTime & 0xFF00) >> 8;
		IQFrameTx[10] = Memo.ChgTime & 0x00FF;
		IQFrameTx[11] = (Memo.ChgAh & 0xFF00) >> 8;
		IQFrameTx[12] = Memo.ChgAh & 0x00FF;
		IQFrameTx[13] = Memo.Profile;//ChgData.Profile; // MODIF R2.7
		IQFrameTx[14] = Memo.Phase;
		IQFrameTx[15] = Memo.Default;
		IQFrameTx[17] = (DataLcd.ChgSOC & 0xFF00) >> 8;
		IQFrameTx[18] = DataLcd.ChgSOC & 0x00FF;
		IQFrameTx[19] = 0xFF;
		IQLenTx = 20;
		break;
	}
}

/**
 * @brief  IQFrameRxRecv
 * @param  None
 * @retval None
 */
uchar IQFrameRxRecv(void)
{
	uchar status;
	//uchar i;

	status = NO_OK;
	switch (IQData.State1)
	{
    case State1WriteJ1939Adr:
        status = OK;
        break;        
      
	case State1RFTrAddr:
		if (IQLenRx == 3)
		{
			in24(IQData.RFTrAddr,IQFrameRx[2],IQFrameRx[1],IQFrameRx[0]);
			IQData.RFTrNewAddr = IQData.RFTrAddr;
			if (IQData.RFTrAddr != 0)
				status = OK;
		}
		break;
	case State1RFTrVers:
		if (IQLenRx == 4)
		{
			memcpy(&IQData.RFTrVers[0], &IQFrameRx[0], 4);
			status = OK;
		}
		break;
	case State1RFTrModAddr:
		if (IQLenRx == 3)
		{
			in24(IQData.RFTrAddr,IQFrameRx[2],IQFrameRx[1],IQFrameRx[0]);
			IQData.RFTrNewAddr = IQData.RFTrAddr;
			if (IQData.RFTrAddr != 0)
				status = OK;
		}
		break;
	case State1RFTrCom:
		if (IQLenRx == 1)
		{
			IQData.RFTrCom = IQFrameRx[0];
			status = OK;
		}
		break;
	case State1RFTrSaveWIIQ:
		if (IQLenRx == 2)
			status = OK;
		break;
	case State1RFTrLoadWIIQ:
		if (IQLenRx == 67)
		{
			memcpy(&IQSeg, &IQFrameRx[3], 64);
			status = OK;
		}
		break;
	case State1RFTrEraseFlash:
		if (IQLenRx == 2)
			status = OK;
		break;
	case State1Measure: // add lenght frame control
	if ((IQFrameRx[6] == CODE_MEASURE_REQUEST) && (IQFrameRx[7] == 17) && (IQLenRx == 27))
	{
		in16(IQData.Ibat,IQFrameRx[9],IQFrameRx[8]);		  // Courant
		in16(IQData.Vbat,IQFrameRx[11],IQFrameRx[10]);		// Vbat
		in16(IQData.Vbalance,IQFrameRx[13],IQFrameRx[12]);// Vbalance
		in16(IQData.VbatCell,IQFrameRx[15],IQFrameRx[14]);// Vbat/cell
		if (((schar)IQFrameRx[22] <= -20) || ((schar)IQFrameRx[22] >= 100))
			IQData.BatTemp = Menu.BatTemp; // default value
		else
			IQData.BatTemp = (schar)IQFrameRx[22];
        // MODIF 2.8
        if (IQData.BatTemp > Menu.BatHighTemp)
            CntHighTpBat++;
        else
            CntHighTpBat = 0;
		status = OK;
	}
	break;
	case State1CalCurOffset:
		if ((IQFrameRx[6] == CODE_CURRENT_CALIB) && (IQFrameRx[7] == 4))
		{
			status = OK;
		}
		break;
	case State1BatInfo: // add frame lenght control
		if ((IQFrameRx[6] == CODE_BAT_INFO) && (IQFrameRx[7] == 24) && (IQLenRx == 34))
		{
			in16(IQData.BatCap,IQFrameRx[9],IQFrameRx[8]);// Capacity
			IQData.NbCells = IQFrameRx[10];						    // Nb_Cells
			IQData.NbBalCells = IQFrameRx[11];					    // Nb_CellsElements_Balance
			IQData.BatTechno = IQFrameRx[12] + 1;				    // Technology (+1 for compatibility)
			in16(IQData.InitSOC,IQFrameRx[14],IQFrameRx[13]);       // State of charge
			IQData.BatWarning = IQFrameRx[15];					    // Warnings (balance, low level, T�)
			memcpy(&IQData.BatSN[0], &IQFrameRx[16], 14);
			// MODIF 2.8
            if (IQData.BatCap != 0)
			{
				Memo.BatCap = IQData.BatCap; 
                status = OK;
			}
		}
		break;
	case State1CgInfo:
		if ((IQFrameRx[6] == CODE_CHARGER_INFO) && (IQFrameRx[7] == 3))
		{
			status = OK;
		}
		break;

	case State1IQSendInChargeData:  // MASTER/SLAVE DUAL CABLE
    case State1IQSendEndChargeData:
		if ((IQFrameRx[6] == CODE_DUAL_CHARGE_INFO) && (IQFrameRx[7] == 3))
		{
			status = OK;
		}
		break;

		// RESET_FLAG_EQUAL
	case State1ResetFlagEqual:
		if ((IQFrameRx[6] == CODE_RESET_EQUAL) && (IQFrameRx[7] == 3))
		{
			status = OK;
		}
		break;
		// READ TPPL parameters
	case State1TPPLRead :
		if (IQFrameRx[6] == CODE_TPPL_READ)
		{
			in16(AarsParam.AhPlus,IQFrameRx[9],IQFrameRx[8]);// AARS Ah+
			in16(AarsParam.AhNeg,IQFrameRx[11],IQFrameRx[10]);// AARS Ah-
			in16(AarsParam.OffsetVreg,IQFrameRx[13],IQFrameRx[12]);// AARS Offset Vreg
			MenuWriteOn; // Save TPPL param into AARS struct EEP
			status = OK;
		}
		break;

		// READ EASY KIT
	case State1EasyKitRead : // V3.4 : add easy kit read
		if ((IQFrameRx[6] == CODE_BAT_INFO) && (IQFrameRx[7] == 24) && (IQLenRx == 34))
		{
			in16(IQData.BatCap,IQFrameRx[9],IQFrameRx[8]);            // Capacity
			IQData.NbCells = IQFrameRx[10];						                // Nb_Cells
			IQData.BatTemp = IQFrameRx[11];
			IQData.BatTechno = IQFrameRx[12] + 1;				              // Technology (+1 for compatibility)
			in16(IQData.InitSOC,IQFrameRx[14],IQFrameRx[13]);         // State of charge
			IQData.BatWarning = IQFrameRx[15];                        // Warnings (balance, low level, T�)
			memcpy(&IQData.BatSN[0], &IQFrameRx[16], 14);
			status = OK;
			if ((IQData.BatWarning != 0xFF) && (IQData.BatWarning != 0xFE) && (EasyKitLink == 0))
			{
				StartMemo();
				EasyKitReadOff;
				EasyKitLinkOn;
				IQWIIQLinkOn;
				IQData.State1 = State1Off;
				IQData.State2 = State2TxInit;
				// Update IQ data in memo
				memcpy(&Memo.BatSN[0], &IQData.BatSN[0], 14);
				Memo.BatCap = IQData.BatCap;
				Memo.CapAutoManu = 1; // Force "CapAutoManu = 1" to display capacity
				Memo.BatTemp = IQData.BatTemp;
				Memo.BatTechno = IQData.BatTechno;
				Memo.BatWarning = IQData.BatWarning;
				if (IQData.BatTemp > 55)
					Memo.BatWarning |= 0x04; // High temp
				Memo.EocTemp = IQData.BatTemp;
				switch (IQData.BatTechno)
				{
				case TEC_PBO:
				case TEC_PZQ:
					ChgData.Profile = IONIC;
					break;
				case TEC_GEL:
					ChgData.Profile = GEL;
					break;
                case TEC_COLD:
                    ChgData.Profile = COLD;
                    break;    
#ifdef  ENERSYS_EU                    
				case TEC_WF:
					ChgData.Profile = WF200;
					break;
				case TEC_PNEU:
					ChgData.Profile = PNEU;
					break;
				case TEC_PREM:
					ChgData.Profile = IONIC;
					break;
				case TEC_PZM:
					ChgData.Profile = PZM;
					break;
                case TEC_LOWCHG:
					ChgData.Profile = LOWCHG;
					break;    
#endif                    
				case TEC_XFC:
                    // MODIF 3.3
				    ChgData.Profile = NXBLC;
					break;

				case TEC_TPPL:
                    // MODIF 3.3
                    if (ChgData.Profile != XFC)
                        ChgData.Profile = TPPL;
					break;    

				case TEC_HGB:
					ChgData.Profile = IONIC;
					break;
				case TEC_VRLA:
					ChgData.Profile = VRLA;
					break;
				case TEC_OPP:
					ChgData.Profile = OPP;
					break;
				default:
					break;
				}
				Memo.Profile = ChgData.Profile;
			}
		}
		break;
	}

	memset(IQFrameRx, 0, 100);

	return status;
}

/**
 * @brief  IQScanCode
 * @param  None
 * @retval None
 */
uchar IQScanCode_old(void)
{
	uchar codeV;
	uchar codeI;

	if (StateConverter != 0)
	{
		codeV = 0; // No voltage range with current
		if (DataR.Ibat < 100) // range [0 : 10A]
			codeI = 1;
		else if (DataR.Ibat < 200)
			codeI = 2;
		else if (DataR.Ibat < 300)
			codeI = 3;
		else if (DataR.Ibat < 400)
			codeI = 4;
		else if (DataR.Ibat < 500)
			codeI = 5;
		else if (DataR.Ibat < 600)
			codeI = 6;
		else if (DataR.Ibat < 700)
			codeI = 7;
		else if (DataR.Ibat < 800)
			codeI = 8;
		else if (DataR.Ibat < 900)
			codeI = 9;
		else if (DataR.Ibat < 1000)
			codeI = 10;
		else if (DataR.Ibat < 1100)
			codeI = 11;
		else if (DataR.Ibat < 1200)
			codeI = 12;
		else if (DataR.Ibat < 1300)
			codeI = 13;
		else if (DataR.Ibat < 1400)
			codeI = 14;
		else
			codeI = 15;
	}
	else
	{
		codeI = 1; // range [0 : 10A]
		if (DataR.Vbat < 1500) // [0 : 15V]
			codeV = 1;
		else if (DataR.Vbat < 2900)
			codeV = 2;
		else if (DataR.Vbat < 4400)
			codeV = 3;
		else if (DataR.Vbat < 5800)
			codeV = 4;
		else if (DataR.Vbat < 8700)
			codeV = 5;
		else if (DataR.Vbat < 9600)
			codeV = 6;
		else
			codeV = 7;
	}
	return (codeV | (codeI << 3));// | 0x80); // codeV(3bits) | codeI(4bits) | Link(1bit)
}
// MODIF R2.6
uchar IQScanCode(void)
{
	uchar codeV;
	uchar codeI;
    
	if (StateConverter != 0)
	{
		codeV = 0; // No voltage range with current
		if (IbatIQ < 100) // range [0 : 10A]
			codeI = 1;
		else if (IbatIQ < 200)
			codeI = 2;
		else if (IbatIQ < 300)
			codeI = 3;
		else if (IbatIQ < 400)
			codeI = 4;
		else if (IbatIQ < 500)
			codeI = 5;
		else if (IbatIQ < 600)
			codeI = 6;
		else if (IbatIQ < 700)
			codeI = 7;
		else if (IbatIQ < 800)
			codeI = 8;
		else if (IbatIQ < 900)
			codeI = 9;
		else if (IbatIQ < 1000)
			codeI = 10;
		else if (IbatIQ < 1100)
			codeI = 11;
		else if (IbatIQ < 1200)
			codeI = 12;
		else if (IbatIQ < 1300)
			codeI = 13;
		else if (IbatIQ < 1400)
			codeI = 14;
		else
			codeI = 15;
	}
	else
	{
		codeI = 1; // range [0 : 10A]
		if (DataR.Vbat < 1500) // [0 : 15V]
			codeV = 1;
		else if (DataR.Vbat < 2900)
			codeV = 2;
		else if (DataR.Vbat < 4400)
			codeV = 3;
		else if (DataR.Vbat < 5800)
			codeV = 4;
		else if (DataR.Vbat < 8700)
			codeV = 5;
		else if (DataR.Vbat < 9600)
			codeV = 6;
		else
			codeV = 7;
	}
	return (codeV | (codeI << 3));// | 0x80); // codeV(3bits) | codeI(4bits) | Link(1bit)
}


/**
 * @brief  IQChrono
 * @param  None
 * @retval None
 */
void IQChrono(void)
{
	uchar i;
	uchar AddrInTab;

	switch (State.Charger)
	{
	case StateChgerChg:
		break;
	default:
		// Check IQ scan & link stopped out of charge
		if ((IQInWIIQScan != 0) || (IQInWIIQLink != 0) || ((EasyKitRead == 0) && (AarsParam.EasyKitOnOff != 0)))
		{
			IQInWIIQScanOff;
			IQInWIIQLinkOff;
			IQData.StateIQ = StateIQOff;
			IQData.State1 = State1Off;
		}

		CntStartOffScan = 0;
		IQWIIQScanOff;
		IQWIIQLinkOff;
		break;
	}

	switch (IQData.State1)
	{
	case State1Off:
		CntTimeoutIQ = 0;
		CntReqIQ = 0;
		switch (IQData.StateIQ)
		{
		case StateIQOff:
			if (Menu.SkipComIQ == ON)
			{
				IQRFTrLinkOff;
			}
			else if (IQInRFTrLink != 0)
			{
				IQRFTrLinkOff;
				IQData.RFTrAddr = 0;
				IQData.StateIQ = StateIQRFTrAddr;
				IQData.StateIQPh = StateIQPh1;
				IQData.State1 = State1RFTrAddr;
				IQData.State2 = State2TxInit;
			}
			else if ((IQRFTrLink == 0) || (IQRxTimeout != 0))
			{
				IQInWIIQScanOff;
				IQInWIIQLinkOff;
				IQInRFTrLinkOn;
				// MODIF 3.3 : reste UARt if timeout
				Reset_SCI6();
			}
			else if (IQInRFTrModAddr != 0)
			{
				IQData.StateIQ = StateIQRFTrModAddr;
				IQData.StateIQPh = StateIQPh1;
				IQData.State1 = State1RFTrModAddr;
				IQData.State2 = State2TxInit;
			}
			else if ((IQInWIIQScan != 0) && (CntStartOffScan == 0))
			{
				// Init variables
				IQWIIQScanOff;
				CntStartOffScan = 1;
				TimeWIIQScan = State.TimerSec + TIMEOUT_WIIQSCAN; // Max time for scan
				memset(ScanAddrArray, 0, NB_SCAN_ADDR);
				IQData.ScanAddrLen = 0;
				// Set current
				Converter(ON); // Scan with current
				if (Menu.Imax > IQ_MAX_CUR) // Imax = IQ_MAX_CUR
					IlinkIQ = IQ_MAX_CUR;
				else
					IlinkIQ = Menu.Imax;

				DataW.Ireq = ((State.TimerMs & 0xFFFFFF) % (IlinkIQ - 50)) + 70; // 7A < Iscan < Imax
				if (DataW.Ireq > Menu.Imax)     // MODIF R2.2
                    DataW.Ireq = Menu.Imax;
                else if (DataW.Ireq > IQ_MAX_CUR)
                    DataW.Ireq = IQ_MAX_CUR;
                
                // MODIF R2.6
                if ((Menu.FloatingOffOn != 0) && (DataW.Ireq < (Menu.Ifloating + 70)))
                    DataW.Ireq = Menu.Ifloating + 70;
                    
                switch (ChgData.Profile)	    // Limit VreqCell by techno
				{
				case XFC:
					DataW.VreqCell = 2600;
					SlopeI = 500;  // 5A/S
					break;

				default:
					DataW.VreqCell = 2700;
					SlopeI = 500;  // 5A/S
					break;
				}
				// IQ states
				IQData.RFTrCom = OFF;
				IQData.State1 = State1StartAddrOffScan;
				IQData.State2 = State2TxInit;
			}
			else if (CntStartOffScan == 1)
			{
				CntStartOffScan = 2;
				// IQ states
				IQData.StateIQ = StateIQScan;
				IQData.StateIQPh = StateIQPh1;
				IQData.RFTrCom = OFF;
				IQData.State1 = State1RFTrCom;
				IQData.State2 = State2TxInit;
                // Modif 2.8
                TimeWIIQScan = State.TimerSec + (5 * 60L);  // 5mn Max
			}
			else if (IQInWIIQLink != 0)
			{
				IQWIIQLinkOff;
				// Init variables
				TimeWIIQMeas = State.TimerSec + TIME_WIIQMEAS; // Init time to update measure
				IQData.WIIQAddr = 0;
				IQData.ScanAddrPtr = 0;
				IQData.CntCurStep = 0;
				IQData.CheckAddrPtr = IQData.ScanAddrLen;
				// Set current
				Converter(ON);
				// IQ states
				IQData.StateIQ = StateIQLink;
				IQData.StateIQPh = StateIQPh1;
				IQData.RFTrCom = OFF;
				IQData.State1 = State1RFTrCom;
				IQData.State2 = State2TxInit;
			}
			else if (IQInWIIQMeas != 0)
			{
				IQWIIQMeasOff;          /* IQWIIQMeasOff */
				IQData.StateIQ = StateIQMeas;
				IQData.State1 = State1Measure;
				IQData.State2 = State2TxInit;
			}
			else if (IQInWIIQCgInfo != 0)
			{
				IQInWIIQCgInfoOff;
				IQData.StateIQ = StateIQCgInfo;
				IQData.State1 = State1CgInfo;
				IQData.State2 = State2TxInit;
			}
			else if (ResetFlagEqual != 0) // RESET_FLAG_EQUAL
			{
				ResetFlagEqualOff;
				IQData.StateIQ = StateIQResetFlagEqual;
				IQData.State1 = State1ResetFlagEqual;
				IQData.State2 = State2TxInit;
			}
			else if (ReadTPPL != 0)       // READ TPPL param
			{
				ReadTPPLOff;
				IQData.StateIQ = StateIQReadTPPL;
				IQData.State1 = State1TPPLRead;
				IQData.State2 = State2TxInit;
			}
			else if ((EasyKitRead != 0) && (EasyKitLink == 0))
			{
				IQData.StateIQ = StateIQReadEasyKit;
				IQData.State1 = State1EasyKitRead;
				IQData.State2 = State2TxInit;
			}
            else if (SendInChargeData != 0)
			{
                SendInChargeDataOff;
				IQData.StateIQ = StateIQSendInChargeData;
				IQData.State1 = State1IQSendInChargeData;
				IQData.State2 = State2TxInit;
			}
            else if (SendEndChargeData != 0)
			{
                SendEndChargeDataOff;
				IQData.StateIQ = StateIQSendEndChargeData;
				IQData.State1 = State1IQSendEndChargeData;
				IQData.State2 = State2TxInit;
			}
            else if (WriteJ1939Adr != 0)
            {
                WriteJ1939AdrOff;
                IQData.StateIQ = StateIQWriteJ1939Adr;
                IQData.State1 = State1WriteJ1939Adr;
                IQData.State2 = State2TxInit;
            }
			else if (IQFrameTxJb != 0)
			{
				IQInitTx(JBUS_ANS);
				IQFrameTxJbOff;
			}
			else if (LoadWIIQ != 0)
			{
				IQData.StateIQ = StateIQRFTrLoadWIIQ;
				IQData.State1 = State1RFTrLoadWIIQ;
				IQData.State2 = State2TxInit;
			}
			else if (EraseIQFlash != 0)
			{
				IQData.StateIQ = StateIQRFTrEraseFlash;
				IQData.State1 = State1RFTrEraseFlash;
				IQData.State2 = State2TxInit;
			}
			else if ((IQWIIQLink != 0) && (State.TimerSec > TimeWIIQMeas))
			{
				TimeWIIQMeas = State.TimerSec + TIME_WIIQMEAS;
				IQInWIIQMeasOn;
			}
			else if (IQData.RFTrCom == OFF)
			{
				IQData.RFTrCom = ON;
				IQData.State1 = State1RFTrCom;
				IQData.State2 = State2TxInit;
			}
			break;
			// RF Transceiver address
		case StateIQRFTrAddr:
			switch (IQData.StateIQPh)
			{
			case StateIQPh1:
				if (IQReqOk != 0)
				{
					IQData.State1 = State1RFTrVers;
					IQData.State2 = State2TxInit;
					IQData.StateIQPh = StateIQPh2;
				}
				else
				{
					IQInRFTrLinkOff;
					IQData.StateIQ = StateIQOff;
				}
				break;
			case StateIQPh2:
				if (IQReqOk != 0)
					IQRFTrLinkOn;
				IQInRFTrLinkOff;
				IQData.StateIQ = StateIQOff;
				break;
			}
			break;
			// Modify RF Transceiver address
			case StateIQRFTrModAddr:
				IQInRFTrModAddrOff;
				IQData.StateIQ = StateIQOff;
				break;
				// Load Wi-IQ from RF Transceiver
			case StateIQRFTrLoadWIIQ:
				LoadWIIQOff;
				IQData.StateIQ = StateIQOff;
				break;
				// Load Wi-IQ from RF Transceiver
			case StateIQRFTrEraseFlash:
				EraseIQFlashOff;
				IQData.StateIQ = StateIQOff;
				break;

			case StateIQScanOff :
				IQData.State1 = State1StartAddrOffScan;
				IQData.State2 = State2TxInit;
				break;

				// Wi-IQ scan
			case StateIQScan:
				switch (IQData.StateIQPh)
				{
				case StateIQPh1: // Set current
					if (IQReqOk != 0)
					{                   // IQ scan work with a fault
						// MODIF 2.8
                        //TimeWIIQScan = State.TimerSec + TIMEOUT_WIIQSCAN; // Max time for scan
						//if ((ItoReach == DataW.Ireq) && ((State.Phase == StatePh1) || (State.Phase == StatePh2)))
                        if ((ItoReach == DataW.Ireq) && ((State.Phase == StatePh1) || (State.Phase == StatePh2)) && (State.TimerSec < TimeWIIQScan))
						{
							IQData.StateIQPh = StateIQPh2;
							IQData.State1 = State1OnScan;
							IQData.State2 = State2TxInit;
						}
					}
					else
					{
						IQData.StateIQPh = StateIQPh3;
					}
					break;
				case StateIQPh2: // Scan address
					IQData.State1 = State1ReqAddrScan;
					IQData.State2 = State2TxInit;
					IQData.StateIQPh = StateIQPh3;
					break;
				case StateIQPh3: // Set Scan ON
					Converter(OFF);
					IQData.State1 = State1OnScan;
					IQData.State2 = State2TxInit;
					IQData.StateIQPh = StateIQPh4;
					break;
				case StateIQPh4: // RF Transceiver Com ON
					if (IQData.ScanAddrLen > 0) // Modif scan off V3.1
					{
						IQWIIQScanOn;
					}
					else
					{
						IQData.State1 = State1StartAddrOffScan;
						IQData.State2 = State2TxInit;
					}
					IQData.StateIQ = StateIQOff;
					IQInWIIQScanOff;
					break;
				}
				break;

				// Wi-IQ link : Measure with current
				case StateIQLink:
					switch (IQData.StateIQPh)
					{
					case StateIQPh1: // Check Address pointer
						#ifndef SAVOYE
                        if ((IQData.CheckAddrPtr == 0) || (IQData.CntCurStep > NB_CUR_STEP))
						{
							Converter(OFF);
							IQData.WIIQAddr = 0;
							IQInWIIQLinkOff;
							IQData.State1 = State1StartAddrOffScan; 
							IQData.State2 = State2TxInit;
							IQData.StateIQ = StateIQOff;
						}
                        #endif
                        
                        #ifdef SAVOYE
                        if (IQData.CheckAddrPtr == 1)
                        {
                            Converter(OFF);
                            IQData.WIIQAddr = ScanAddrArray[0];
                            // Wi-IQ measure
                            IQData.State1 = State1Measure;
                            IQData.State2 = State2TxInit;
                            IQData.StateIQPh = StateIQPh4;
                        }
                  		  else if ((IQData.CheckAddrPtr == 0) || (IQData.CntCurStep > NB_CUR_STEP))
                        {
                            Converter(OFF);
                            IQData.WIIQAddr = 0;
                            IQInWIIQLinkOff;
                            IQData.State1 = State1StartAddrOffScan; // Modif scan off V3.1
                		    IQData.State2 = State2TxInit;
                            IQData.StateIQ = StateIQOff;
                        }
                        #endif                       
						else if ((IQData.CheckAddrPtr == 1) && (IQData.CntCurStep > 2))
						{
							Converter(OFF);
							IQData.WIIQAddr = ScanAddrArray[0];
							// Wi-IQ measure
							IQData.State1 = State1Measure;
							IQData.State2 = State2TxInit;
							IQData.StateIQPh = StateIQPh4;
						}
						else // More than 1 Wi-IQ selected
						{
							IQData.ScanAddrLen = IQData.CheckAddrPtr;
							IQData.ScanAddrPtr = 0;
							IQData.CheckAddrPtr = 0;
							if (Menu.Imax > IQ_MAX_CUR) // Imax = IQ_MAX_CUR
								IlinkIQ = IQ_MAX_CUR / 2;
							else if (Menu.Imax < 100)   // Imin = 10A
								IlinkIQ = 100 / 2;
							else
								IlinkIQ = Menu.Imax / 2;
                                
							if (((IQData.CntCurStep + (uchar)IQData.RFTrAddr) % 2) == 0)
								DataW.Ireq = (State.TimerMs % IlinkIQ) + IlinkIQ;     // Imax/2 < Ilink < Imax
							else
								DataW.Ireq = (State.TimerMs % (IlinkIQ - 50)) + 50; // 5A < Ilink < Imax/2
                            
                            if (DataW.Ireq > Menu.Imax) // MODIF R2.2
                                DataW.Ireq = Menu.Imax;
                            else if (DataW.Ireq > IQ_MAX_CUR)
                                DataW.Ireq = IQ_MAX_CUR;
                            
                            // MODIF R2.6
                            if ((Menu.FloatingOffOn != 0) && (DataW.Ireq < (Menu.Ifloating + 50)))
                                DataW.Ireq = Menu.Ifloating + 50;
                            
							switch (ChgData.Profile)	// Limit VreqCell by techno
							{
							case XFC:
								DataW.VreqCell = 2600;
								SlopeI = 500;  // 5A/S
								break;

							default:
								DataW.VreqCell = 2650;
								SlopeI = 500;  // 5A/S
								break;
							}
							// Add long timeout if current step from High to low current
							// (because ItoReach == DataW.Ireq in this way)
							TimeCurStep = State.TimerSec + (( IlinkIQ * 2L * 10L) / SlopeI) + 2L;
							//TimeCurStep = State.TimerSec + 4L;
							IQData.CntCurStep++;
							IQData.StateIQPh = StateIQPh2;
							TimeWIIQScan = State.TimerSec + TIMEOUT_WIIQSCAN; // Max time for IQ Link
						}
						break;
					case StateIQPh2: // Set current
						if ((State.Phase == StatePh1) || (State.Phase == StatePh2)) // No default (DF1,DF2..)
						{
							if (State.TimerSec > TimeWIIQScan) // MODIF : Link timeout
							{
								Converter(OFF);
								IQData.WIIQAddr = 0;
								IQInWIIQLinkOff;
								IQData.State1 = State1StartAddrOffScan; // Modif scan off
								IQData.State2 = State2TxInit;
								IQData.StateIQ = StateIQOff;
							}
							else if (IQData.ScanAddrPtr < IQData.ScanAddrLen)
							{
								/*if (ItoReach != DataW.Ireq)
                  {
                    TimeCurStep = State.TimerSec + 5L;
                  }
                  else*/ if ((State.TimerSec > TimeCurStep) && (ItoReach == DataW.Ireq))
                  {
                	  IQData.WIIQAddr = ScanAddrArray[IQData.ScanAddrPtr];
                	  IQData.State1 = State1Measure;
                	  IQData.State2 = State2TxInit;
                	  IQData.StateIQPh = StateIQPh3;
                  }
							}
							else
							{
								IQData.StateIQPh = StateIQPh1;
							}
						}
						else
						{
							IQInWIIQLinkOff;
							IQData.State1 = State1StartAddrOffScan; // Modif scan off V3.1
							IQData.State2 = State2TxInit;
							IQData.StateIQ = StateIQOff;
						}
						break;
					case StateIQPh3: // Check measures I-U
						if (IQReqOk != 0)
						{
							VtolIQ = ((ulong)DataR.Vbat * VCEL_TOL_NEG) / 200L;
							if (VtolIQ > 200)                                   // Limit to 2V the VbatTol
								VtolIQ = 200;
							if( (IQData.Ibat > ((sint)IbatIQ - I_TOL)) &&       // MODIF R2.6 : use IbatIQ instead of DataR.Ibat
									(IQData.Ibat < ((sint)IbatIQ + I_TOL)) &&
									(IQData.Vbat > (DataR.Vbat - VtolIQ)) &&
									(IQData.Vbat < (DataR.Vbat + VtolIQ)) &&
									(IbatIQ > 40) ) // 4A min for the link
									{
								ScanAddrArray[IQData.CheckAddrPtr] = ScanAddrArray[IQData.ScanAddrPtr];
								IQData.CheckAddrPtr++;
									}
							else
							{
								IQReqOkOn;
							}
						}
						IQData.ScanAddrPtr++;
						IQData.StateIQPh = StateIQPh2;
						break;


					case StateIQPh4: // Wi-IQ battery info
						if (IQReqOk != 0)
						{
							IQData.State1 = State1BatInfo;
							IQData.State2 = State2TxInit;
						}
						IQData.StateIQPh = StateIQPh5;
						break;

					case StateIQPh5: // Wi-IQ charger info
						if (IQReqOk != 0)
						{
							IQData.State1 = State1CgInfo;
							IQData.State2 = State2TxInit;
						}
						IQData.StateIQPh = StateIQPh6;
						break;

					case StateIQPh6: // Adr Scan Off (MODIF FD)
						IQData.State1 = State1StartAddrOffScan;
						IQData.State2 = State2TxInit;
						IQWIIQLinkOn;
						IQData.StateIQPh = StateIQPh7;
						break;

					case StateIQPh7: // read TTPL param
						if (IQReqOk != 0)
						{
							IQData.State1 = State1TPPLRead;
							IQData.State2 = State2TxInit;
						}
						IQData.StateIQPh = StateIQPh8;
						break;

					case StateIQPh8: // Save Wi-IQ datas
						if (IQReqOk != 0)
							IQWIIQLinkOn; // WIIQ link
						IQInWIIQLinkOff;
						IQData.State1 = State1RFTrSaveWIIQ;
						IQData.State2 = State2TxInit;
						IQData.StateIQ = StateIQOff;
						break;
					}
					break;
					// Wi-IQ measure
					case StateIQMeas:
						if (IQReqOk != 0)
							IQWIIQMeasOn;
						IQInWIIQMeasOff;
						IQData.StateIQ = StateIQOff;
						break;
					default:
						IQData.StateIQ = StateIQOff;
						break;
		}
		break;
		//-------------------
		// Internal request
		//-------------------
		case State1RFTrAddr:
		case State1RFTrVers:
		case State1RFTrModAddr:
		case State1RFTrCom:
		case State1RFTrSaveWIIQ:
		case State1RFTrLoadWIIQ:
		case State1RFTrEraseFlash:
        case State1WriteJ1939Adr:
			switch (IQData.State2)
			{
			case State2TxInit:
				IQFrameTxInit();
				IQInitRx();
				IQInitTx(INT_REQ);
				IQReqOkOff;
				TimeoutIQ = State.TimerMs + TIMEOUT_IQ;
				IQData.State2 = State2TxWait;
				break;
			case State2TxWait:
				if (State.TimerMs > TimeoutIQ)
				{
					IQTxTimeoutOn;
					IQData.State1 = State1Off;
				}
				else if (IQInFrameTx == 0)
				{
					IQData.State2 = State2RxInit;
				}
				break;
			case State2RxInit:
				TimeoutIQ = State.TimerMs + TIMEOUT_IQ;
				IQData.State2 = State2RxWait;
				break;
			case State2RxWait:
				//if (State.TimerMs > TimeoutIQ)
					if ((State.TimerMs > TimeoutIQ) && (IQInFrameRx != 0))  // MODIF R0.1
					{
						IQRxTimeoutOn;
						CntTimeoutIQ++;
						if (CntTimeoutIQ < NB_TIMEOUT_IQ)
							IQData.State2 = State2TxInit;
						else
							IQData.State1 = State1Off;
					}
					else if	(IQInFrameRx == 0)
					{
						if (IQFrameRxRecv() != 0)
						{
							IQReqOkOn;
							IQData.State1 = State1Off;
						}
						else if (CntReqIQ < NB_INT_REQ)
						{
							IQData.State2 = State2TxInit;
						}
						else
						{
							IQData.State1 = State1Off;
						}
						CntReqIQ++;
					}
					break;
			}
			break;
			//-------------------
			// Scan Wi-IQ address
			//-------------------
			// Scan : all addresses ON
			case State1OnScan:
				switch (IQData.State2)
				{
				case State2TxInit:
					IQData.WIIQAddr = 0x1FFFFF;
					out24(IQFrameTx[2],IQFrameTx[1],IQFrameTx[0],IQData.WIIQAddr);
					out24(IQFrameTx[5],IQFrameTx[4],IQFrameTx[3],IQData.RFTrAddr);
					IQFrameTx[6] = CODE_SCAN_ADDRESS;
					IQFrameTx[7] = 0x02;
					IQFrameTx[8] = 0x00;
					IQFrameTx[9] = 0x01; // Scan ON
					IQFrameTx[10] = 0xFF;
					IQLenTx = 11;
					IQInitRx();
					IQInitTx(EXT_REQ_WITHOUT_ANSWER);
					TimeoutIQ = State.TimerMs + TIMEOUT_IQ;
					IQData.State2 = State2TxWait;
					break;
				case State2TxWait:
					if (State.TimerMs > TimeoutIQ)
					{
						IQTxTimeoutOn;
						IQData.State1 = State1Off;
					}
					else if (IQInFrameTx == 0)
					{
						IQData.State2 = State2RxInit;
					}
					break;
				case State2RxInit:
					TimeoutIQ = State.TimerMs + TIMEOUT_IQ;
					IQData.State2 = State2RxWait;
					break;
				case State2RxWait:
					if (State.TimerMs > TimeoutIQ)
					{
						if (CntReqIQ < NB_REQ_WITHOUT_ANSWER)
						{
							IQData.State2 = State2TxInit;
						}
						else
						{
							IQData.State1 = State1Off;
							TimeCurStep = State.TimerMs + 2000L; // Timer for "scan" set current
						}
						CntReqIQ++;
					}
					break;
				}
				break;
				// Scan : request address
				case State1ReqAddrScan:
					switch (IQData.State2)
					{
					case State2TxInit:
						IQData.WIIQAddr = 0x1FFFFF;
						out24(IQFrameTx[2],IQFrameTx[1],IQFrameTx[0],IQData.WIIQAddr);
						out24(IQFrameTx[5],IQFrameTx[4],IQFrameTx[3],IQData.RFTrAddr);
						IQFrameTx[6] = CODE_SCAN_ADDRESS;
						IQFrameTx[7] = 0x02;
						IQFrameTx[8] = 0x01;
						IQFrameTx[9] = IQScanCode();
						IQFrameTx[10] = 0xFF;
						IQLenTx = 11;
						IQInitRx();
						IQInitTx(EXT_REQ_WITH_ANSWER);
						TimeoutIQ = State.TimerMs + TIMEOUT_IQ;
						IQData.State2 = State2TxWait;
						break;
					case State2TxWait:
						if (State.TimerMs > TimeoutIQ)
						{
							IQTxTimeoutOn;
							IQData.State1 = State1Off;
						}
						else if (IQInFrameTx == 0)
						{
							IQData.State2 = State2RxInit;
						}
						break;
					case State2RxInit:
						TimeoutIQ = State.TimerMs + TIMEOUT_IQ;
						IQData.State2 = State2RxWait;
						break;
					case State2RxWait:
						if (State.TimerSec > TimeWIIQScan)
						{
							IQData.State1 = State1Off;
						}
						else if (State.TimerMs > TimeoutIQ)
						{
							IQRxTimeoutOn;
							if (CntTimeoutIQ < NB_TIMEOUT_IQ)
								IQData.State2 = State2TxInit;
							else
								IQData.State1 = State1Off;
							CntTimeoutIQ++;
						}
						else if	(IQInFrameRx == 0)
						{
							if ((IQFrameRx[6] == CODE_SCAN_ADDRESS) && (IQFrameRx[7] == 3))
							{
								in24(ScanAddrArray[IQData.ScanAddrLen], IQFrameRx[5], IQFrameRx[4], IQFrameRx[3]);
								i = 0;
								AddrInTab = NO_OK;
								while ((AddrInTab == NO_OK) && (i < IQData.ScanAddrLen))
								{
									if (ScanAddrArray[i] == ScanAddrArray[IQData.ScanAddrLen])
										AddrInTab = OK;
									i++;
								}
								IQData.WIIQAddr = ScanAddrArray[IQData.ScanAddrLen];
								if (AddrInTab == NO_OK)
									IQData.ScanAddrLen++;
								else
									ScanAddrArray[IQData.ScanAddrLen] = 0;
								if (IQData.ScanAddrLen < NB_SCAN_ADDR)
								{
									CntTimeoutIQ = 0;
									CntReqIQ = 0;
									IQData.State1 = State1AddrOffScan;
									IQData.State2 = State2TxInit;
								}
								else
								{
									IQData.State1 = State1Off;
									IQData.State2 = State2TxInit;
								}
							}
						}
						break;
					}
					break;

					// Scan off before and after Scan/Link (MODIF FD)
					case State1StartAddrOffScan :
						switch (IQData.State2)
						{
						case State2TxInit:
							WIIQAddrTmp = IQData.WIIQAddr;
							IQData.WIIQAddr = 0x1FFFFF;
							out24(IQFrameTx[2],IQFrameTx[1],IQFrameTx[0],IQData.WIIQAddr);
							out24(IQFrameTx[5],IQFrameTx[4],IQFrameTx[3],IQData.RFTrAddr);
							IQFrameTx[6] = CODE_SCAN_ADDRESS;
							IQFrameTx[7] = 0x02;
							IQFrameTx[8] = 0x00;
							IQFrameTx[9] = 0x00; // Scan OFF
							IQFrameTx[10] = 0xFF;
							IQLenTx = 11;
							IQInitRx();
							IQInitTx(EXT_REQ_WITHOUT_ANSWER);
							TimeoutIQ = State.TimerMs + TIMEOUT_IQ;
							IQData.State2 = State2TxWait;
							IQData.WIIQAddr = WIIQAddrTmp;
							break;
						case State2TxWait:
							if (State.TimerMs > TimeoutIQ)
							{
								IQTxTimeoutOn;
								CntTimeoutIQ = 0;
								CntReqIQ = 0;
								IQData.State1 = State1Off;
							}
							else if (IQInFrameTx == 0)
							{
								IQData.State2 = State2RxInit;
							}
							break;
						case State2RxInit:
							TimeoutIQ = State.TimerMs + TIMEOUT_IQ;
							IQData.State2 = State2RxWait;
							break;
						case State2RxWait:
							if (State.TimerMs > TimeoutIQ)
							{
								if (CntReqIQ < NB_REQ_WITHOUT_ANSWER)
								{
									IQData.State2 = State2TxInit;
									CntReqIQ++;
								}
								else
								{
									CntTimeoutIQ = 0;
									CntReqIQ = 0;
									IQReqOkOn;
									IQData.State1 = State1Off;
									IQData.State2 = State2TxInit;
								}
							}
							break;
						}
						break;

						// Scan : address received off
						case State1AddrOffScan:
							switch (IQData.State2)
							{
							case State2TxInit:
								//Turn scan off the WiIQ who answer -> IQData.WIIQAddr
								out24(IQFrameTx[2],IQFrameTx[1],IQFrameTx[0],IQData.WIIQAddr);
								out24(IQFrameTx[5],IQFrameTx[4],IQFrameTx[3],IQData.RFTrAddr);
								IQFrameTx[6] = CODE_SCAN_ADDRESS;
								IQFrameTx[7] = 0x02;
								IQFrameTx[8] = 0x00;
								IQFrameTx[9] = 0x00; // Scan OFF
								IQFrameTx[10] = 0xFF;
								IQLenTx = 11;
								IQInitRx();
								IQInitTx(EXT_REQ_WITHOUT_ANSWER);
								TimeoutIQ = State.TimerMs + TIMEOUT_IQ;
								IQData.State2 = State2TxWait;
								break;
							case State2TxWait:
								if (State.TimerMs > TimeoutIQ)
								{
									IQTxTimeoutOn;
									CntTimeoutIQ = 0;
									CntReqIQ = 0;
									IQData.State1 = State1Off;
								}
								else if (IQInFrameTx == 0)
								{
									IQData.State2 = State2RxInit;
								}
								break;
							case State2RxInit:
								TimeoutIQ = State.TimerMs + TIMEOUT_IQ;
								IQData.State2 = State2RxWait;
								break;
							case State2RxWait:
								if (State.TimerMs > TimeoutIQ)
								{
									if (CntReqIQ < NB_REQ_WITHOUT_ANSWER)
									{
										IQData.State2 = State2TxInit;
										CntReqIQ++;
									}
									else
									{
										CntTimeoutIQ = 0;
										CntReqIQ = 0;
										IQData.State1 = State1ReqAddrScan;
										IQData.State2 = State2TxInit;
									}
								}
								break;
							}
							break;
							//-------------------
							// External request
							//-------------------
							case State1Measure:
							case State1CalCurOffset:
							case State1BatInfo:
							case State1CgInfo:
                            // MODIF 2.8
                            case State1IQSendInChargeData:
                            case State1IQSendEndChargeData:
                            // END
							case State1ResetFlagEqual:
							case State1TPPLRead:
							case State1EasyKitRead :
								switch (IQData.State2)
								{
								case State2TxInit:
									if (IQData.State1 == State1EasyKitRead)
									{
										IQData.WIIQAddr = AarsParam.EasyKitAddr;
									}
									if ((IQData.RFTrAddr != 0) && (IQData.WIIQAddr != 0))
									{
										IQFrameTxInit();
										IQInitRx();
										IQInitTx(EXT_REQ_WITH_ANSWER);
										IQReqOkOff;
										TimeoutIQ = State.TimerMs + TIMEOUT_IQ;
										IQData.State2 = State2TxWait;
									}
									else
									{
										IQData.State1 = State1Off;
									}
									break;
								case State2TxWait:
									if (State.TimerMs > TimeoutIQ)
									{
										IQTxTimeoutOn;
										IQData.State1 = State1Off;
									}
									else if (IQInFrameTx == 0)
									{
										IQData.State2 = State2RxInit;
									}
									break;
								case State2RxInit:
									TimeoutIQ = State.TimerMs + TIMEOUT_IQ;
									IQData.State2 = State2RxWait;
									break;
								case State2RxWait:
									if (State.TimerMs > TimeoutIQ)
									{
										IQRxTimeoutOn;
										if (CntTimeoutIQ < NB_TIMEOUT_IQ)
											IQData.State2 = State2TxInit;
										else
											IQData.State1 = State1Off;
										CntTimeoutIQ++;
									}
									else if	(IQInFrameRx == 0)
									{
										if (IQFrameRxRecv() != 0)
										{
											IQReqOkOn;
											IQData.State1 = State1Off;
										}
									}
									break;
								}
								break;

								default:
									IQData.State1 = State1Off;
									break;
	}
}


/**
 * @brief  SCI6 receive interrupt handler, called when a character is
 *         received. -> see vect.h for interrupt vector definition
 * @param  None
 * @retval None
 */
void Excep_SCI6_RXI6(void)
{
	if (VncRfPtr == 0)
	{
		U2BufRx = SCI6.RDR; // Read current byte
		/* Case standard Rf transceiver */
		if (U2BufRx == 'Y')
		{
			IQRxRecvOn;
			// Init variables
			Value = 0;
			Inc = 16;
			IQLenRx = 0;
		}
		else if (IQRxRecv != 0)
		{
			if ((U2BufRx == 'Z') || (U2BufRx == 'J'))
			{
				IQRxRecvOff;
				if (U2BufRx == 'Z')
					IQInFrameRxOff;
				else
					IQFrameRxJbOn; // JBus frame
			}
			else
			{
				if ((U2BufRx == 'O') || (U2BufRx == 'K'))
				{
					Value = U2BufRx;
					Inc = 0;
				}
				else if ((U2BufRx >= '0') && (U2BufRx <= '9'))
				{
					Value += (U2BufRx - 0x30) * Inc;
					Inc /= 16;
				}
				else if ((U2BufRx >= 'A') && (U2BufRx <= 'F'))
				{
					Value += (U2BufRx - 0x41 + 10) * Inc;
					Inc /= 16;
				}
				if (Inc == 0)
				{
					IQFrameRx[IQLenRx] = Value;
					Value = 0;
					Inc = 16;
					IQLenRx++;
				}
			}
		}
	}
	else
	{
		/* Case reprogrammation of Rf transceiver */
		U2BufRx = SCI6.RDR; // Read current byte
		switch (U2BufRx)
		{
		case XON :
			if (U2XonXoff != 0)
			{
				U2Xon;
				if ((U2PtrTx != 0) && (U2PtrTx < (U2LenTx - 1)))
				{
					U2PtrTx = (U2PtrTx + 1) % MAX_LEN;
					SCI6.TDR = IQFrameTx[U2PtrTx];
				}
			}
			break ;
		case XOFF :
			U2Xoff;
			break ;
		}
	}
}


/**
 * @brief  SCI6 transmit interrupt handler, called when a character is
 *         send. -> see vect.h for interrupt vector definition
 * @param  None
 * @retval None
 */
void Excep_SCI6_TXI6(void)
{
	if (VncRfPtr == 0)
	{
		if (U2PtrTx < U2LenTx)
		{
			if ((U2PtrTx % 2) == 0)
			{
				if ((IQFrameTx[U2PtrTx/2] / 16) < 10)
					U2BufTx = (IQFrameTx[U2PtrTx/2] / 16) + 0x30;
				else
					U2BufTx = (IQFrameTx[U2PtrTx/2] / 16) - 10 + 0x41;
			}
			else
			{
				if ((IQFrameTx[U2PtrTx/2] % 16) < 10)
					U2BufTx = (IQFrameTx[U2PtrTx/2] % 16) + 0x30;
				else
					U2BufTx = (IQFrameTx[U2PtrTx/2] % 16) - 10 + 0x41;
			}
			SCI6.TDR = U2BufTx; // Send next character
		}
		else if (U2PtrTx == U2LenTx)
		{
			U2BufTx = 'X';	// Last character
			SCI6.TDR = U2BufTx;
		}
		else
		{
			U2LenTx = 0;
			U2PtrTx = 0;
			IQInFrameTxOff;
		}
		U2PtrTx++;
	}
	else
	{
		if (U2XonXoff == 0)
		{
			if (U2PtrTx < (U2LenTx - 1))
			{
				/* Send next character */
				U2PtrTx = (U2PtrTx + 1) % MAX_LEN;
				SCI6.TDR = IQFrameTx[U2PtrTx];
			}
			else
				if (4 <= VncRfPtr)
					VncRfPtr = 0;
		}
	}
}



/**
 * @brief  SCI6 complete transmit interrupt handler, called when last  character is
 *         send. -> see vect.h for interrupt vector definition
 * @param  None
 * @retval None
 */
void Excep_SCI6_TEI6(void)
{

}
