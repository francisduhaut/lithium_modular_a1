/*******************************************************************************
 * UART 11 driver
 *
 *
 *
 *
 *******************************************************************************/
/* Copyright (C) 2013 Enersys. All rights reserved.   */
/*******************************************************************************
 * File Name     : Uart.c
 * Version       : 1.00
 * Device        : R5F563NB (RX631)
 * Tool-Chain    : Renesas RX Standard 1.2.0.0
 * H/W Platform  : LIFETECH
 * Description   :
 *******************************************************************************/
/*******************************************************************************
 * History       : Nov. 2013  Ver. 1.00 First Release
 *******************************************************************************/


/*******************************************************************************
 * User Includes (Project Level Includes)
 *******************************************************************************/
#include "include.h"

/*******************************************************************************
 * User Defines (Project Level Includes)
 *******************************************************************************/


/*******************************************************************************
 * Internal variables
 *******************************************************************************/
char  U0Buf[U0SIZE];          /* Uart0 input/output buffer */
uint  U0Ptr;
uint  U0Len;
uint  U0Deb;
uint  U0InitCnt;              /* Counter for initialisation (for modules) */
uint  U0InitTemp;             /* Counter for initialisation (for modules) */
ulong U0TimeOut;              /* Counter for delayed JBus transmission */

uint  UartStatus;

char  U0BufTx[U0SIZE];      /* Uart0 input/output buffer */
uint  U0PtrTx;
uint  U0LenTx;
uint  U0DebTx;

uchar NetworkVer[8];          /* Revison of network module */
/*******************************************************************************
 * Local Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void Excep_SCI11_RXI11(void)
{
	uint i;

	U0Buf[U0Ptr] = SCI11.RDR;                    /* Read current byte */
	U0Ptr = (U0Ptr + 1) % U0SIZE;
	switch (Menu.Network)
	{
	case JBUS :
	{
		U0CarOff;												    /* No pending character to be send */
		U0TimeOut = (State.TimerMs + JBTMP);/* Repousse le compteur pour reset Jbus */
		Uart0FrmStart();                    /* Save spaces for additional characters */
		i = IQJbRx((uchar *)&U0Buf[0], &U0Deb, U0Ptr, U0SIZE, (uchar *)&U0Buf[0], U0Len);
		if (i != 0)
		{
			U0CarOff;				                  /* Bit message � transmettre d�sarm� */
			SCI11.SCR.BIT.RE = 1;                     /* Receive disable */
			U0Len = U0Len + i;
			Uart0FrmEnd();
		}
	}
	break ;
	case LAN :
		LantxRx();
		//EthG3Rx();
		break ;
	case BFM :
		U0CarOff;												    /* No pending character to be send */
		U0TimeOut = (State.TimerMs + BFMTMP);/* Repousse le compteur pour reset Jbus */
		Uart0FrmStart();                    /* Save spaces for additional characters */
		i = BfmRx((uchar *)&U0Buf[0], &U0Deb, U0Ptr, U0SIZE, U0Len);
		if (i != 0)
		{
			U0CarOff;				                  /* Bit message � transmettre d�sarm� */
			SCI11.SCR.BIT.RE = 0;                      /* Receive disable */
			OUT_DE = 1;
			U0Len = U0Len + i;
			Uart0FrmEnd();
		}
		break ;
	}

}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void Excep_SCI11_TXI11(void)
{
	switch (Menu.Network)
	{
	case JBUS :
		if ((U0Ptr < (U0Len - 1)) && (U0Len != 0))
		{
			/* Send next character */
			U0Ptr = U0Ptr + 1;
			SCI11.TDR = U0Buf[U0Ptr];
			U0TimeOut = (State.TimerMs + 3);
		}
		else
		{
			U0Ptr = U0Ptr + 1;
			if (U0Ptr <= U0Len)
			{
				//ucon |= 0x01;                       /* Interrupt on transmission completed */
				U0CarOff;                           /* No more byte to transmit */
			}
			else
			{
				Uart0Reset();
				RS485In;
			}
		}
		break ;
        
	case LAN :
		LantxTx();
		break ;
        
	case BFM :
		if ((U0Ptr < (U0Len-1)) && (U0Len != 0))
		{
			/* Send next character */
			U0Ptr = U0Ptr + 1;
			SCI11.TDR = U0Buf[U0Ptr];
			U0TimeOut = (State.TimerMs + 3);
		}
		else
		{

			U0Ptr = U0Ptr + 1;
			if (U0Ptr <= U0Len)
			{
				U0CarOff;                           /* No more byte to transmit */
			}
			else
			{
				Uart0Reset();
				RS485In;
			}
		}
		break ;
	}
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void Excep_SCI11_TEI11(void)
{
	Uart0Reset(); 
}



/* ********** INITIALISATION ********** */


void Uart11Init(void)
{
	// Initialise SCI11 to 9600bds
	/* Protection off */
	SYSTEM.PRCR.WORD = 0xA503u;

	/* Cancel the SCI11 module clock stop mode */
	MSTP_SCI11 = 0u;

	/* Protection on */
	SYSTEM.PRCR.WORD = 0xA500u;

	/* Configure SCI11's TXD11 and RXD11 pins for peripheral functions - see page 661  */
	PORT7.PMR.BIT.B6 = 1;
	PORT7.PMR.BIT.B7 = 1;
	//PORT0.PCR.BIT.B1 = 1;

	/* PWPR.PFSWE write protect off */
	MPC.PWPR.BYTE = 0x00u;

	/* PFS register write protect off */
	MPC.PWPR.BYTE = 0x40u;

	/* Configure SCI11's communication
	 * pins P77 = TXD11 / P76 = RXD11 on LQFP176
	 * see page 691/712 on datasheet */

	/* Set Port0's pin 0 function to TXD11 - see page 691/712 */
	MPC.P76PFS.BYTE = 0x0Au;
	/* Set Port0's pin 1 function to RXD11 - see page 691/712 */
	MPC.P77PFS.BYTE = 0x0Au;

	/* Turn on PFS register write protect */
	MPC.PWPR.BYTE = 0x80u;

	/* Cancel SCI11 module stop mode */
	SYSTEM.MSTPCRC.LONG = 0xFEFF0000u;

	/* Disable All bits and CKE0,1 = 00 */
	SCI11.SCR.BYTE = 0x00u;

	/* Set output port TXD11(P00) */
	PORT7.PDR.BIT.B7 = 1u;
	/* Set Receive port RXD11(P01)*/
	PORT7.PDR.BIT.B6 = 0u;

	PORT7.PCR.BIT.B7 = 1;
	PORT7.PCR.BIT.B6 = 1;

	/* disable All bits and CKE0,1 = 00 once more */
	SCI11.SCR.BYTE = 0x00u;

	/* CKS[0-1] = 00: PCLK 0 div
     1Stop 8bits non-parity     */
	SCI11.SMR.BYTE = 0x00u;
	//SCI11.SMR.BIT.STOP = 1;

	/* SMIF = 0: Serial  */
	SCI11.SCMR.BYTE = 0xF2u;

	/* PCLK = 48MHz, Set baudrate to 9600 bps
     155 = 48MHz / (64*0.5*9600) -1 */
	SCI11.BRR = 155u;

	/* Allow a small delay */
	for (uint16_t i = 0u; i < 20u; i++ )
	{
	}

	PORT6.PDR.BIT.B3 = 1u;
	//PORT6.PDR.BIT.B3 = 1u;

	/* Clear bits ORER, PER and FER */
	SCI11.SSR.BYTE &= 0xC7u;

	/* Set SCI11 RXI11/TXI11/TEI11 interrupt priority level to 7 */
	IPR(SCI11,RXI11) = 0x3u;
	IPR(SCI11,TXI11) = 0x3u;
	IPR(SCI11,TEI11) = 0x3u;
	/* Enable SCI11 RXI11/TXI11/TEI11 interrupts */
	IEN(SCI11,RXI11) = 0x1u;
	IEN(SCI11,TXI11) = 0x1u;
	IEN(SCI11,TEI11) = 0x0u;
	/* Clear SCI11 RXI11/TXI11/TEI11 interrupt flag */
	IR(SCI11,RXI11) = 0x0u;
	IR(SCI11,TXI11) = 0x0u;
	IR(SCI11,TEI11) = 0x0u;

	/* Set bits TIE, RIE, RE and TE. Enable receive interrupts */
	SCI11.SCR.BYTE = 0xF0u;
}


/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void Uart0Reset(void)
{
	/* Reset de la r�ception JBus suite � un temps d'attente trop long ou message parti */
	U0CarOff;                               /* No pending characters to be transmitted */
	U0Deb = 0;
	U0Len = 0;
	U0Ptr = 0;                            	
	SCI11.SCR.BIT.TE = 1;                   /* Transmit disable */
	SCI11.SCR.BIT.RE = 1;                   /* Receive enable */
	OUT_DE = 0;
	U0TimeOut = 0;
}  

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void Uart0Init(void) 
{
	/* Reset all the Uart0 initialisation process */
	U0InitCnt = 0;
	U0InitTemp = 0;
	switch (Menu.Network)
	{
	case JBUS :
	{
        Display.BFMOffShow = 0;
        Display.LanOffShow = 0;
        Display.LanOnShow = 0;
        Display.WifiOffShow = 0;
        Display.WifiOnShow = 0;
		/* Network name */
		sprintf((char *)&NetworkVer[0], "JBUS   ");
		/* Initialisation J-Bus */
		Uart11Init();
		Uart0BR(Menu.JbSpeed);              /* Set baud rate */

		/* Mask */
		MaskJbusOn;
		MaskLanOff;
		MaskWLanOff;
		MaskWAscOff;
		MaskWHexOff;
		Uart0Reset();                       /* Pour placer le CmdDirOff avant CmdDirInit */
		RS485In;
	}
	break ;
	case LAN :
        Display.BFMOffShow = 0;
		/* Network name */
		sprintf((char *)&NetworkVer[0], "LAN    ");
		Uart11Init();
		//EthG3Init();
		break ;
	case BFM :
		/* Network name */
        Display.BFMOffShow = 1;
        Display.LanOffShow = 0;
        Display.LanOnShow = 0;
        Display.WifiOffShow = 0;
        Display.WifiOnShow = 0;
		sprintf((char *)&NetworkVer[0], "BFM    ");
		Uart11Init();
		Uart0BR(12);                  /* Set baud rate : 14400 bauds */

		/* Mask */
		MaskJbusOn;
		MaskLanOff;
		MaskWLanOff;
		MaskWAscOff;
		MaskWHexOff;
		RS485In;
		Uart0Reset();
		break ;
	}
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void Uart0Timer(void)
{
	/* Executed every ms */
	switch (Menu.Network)
	{
	case JBUS :
		/* Actions waiting for timer */
		if (U0Car == 0)
		{
			/* No pending character to be transmitted */
			if (U0TimeOut <= State.TimerMs)
			{
				Uart0Reset();
				RS485In;
			}
		}
		else
		{
			/* Pending frame to be transmitted */
			if (U0TimeOut <= State.TimerMs)
			{
				RS485Out;
				U0Ptr = 0; 
				SCI11.SCR.BIT.RE = 1;           /* Receive disable */
				SCI11.SCR.BIT.TE = 1;           /* Transmit enable */
				SCI11.TDR = U0Buf[0];
				U0TimeOut = (State.TimerMs + 100);
				U0CarOff;
			}
		}
		break ;
        
	case LAN :
		LantxTimer();
		break ;
        
	case BFM :
		BfmTimer();
		if (U0Car == 0)
		{
			/* No pending character to be transmitted */
			if (U0TimeOut <= State.TimerMs)
			{
				Uart0Reset();
				RS485In;
			}
		}
		else
		{
			/* Pending frame to be transmitted */
			if (U0TimeOut <= State.TimerMs)
			{
				RS485Out;
				U0Ptr = 0;                          /* Pointeur sur le caract�re � transmettre */
				SCI11.SCR.BIT.RE = 0;                       /* Receive disable */
				SCI11.SCR.BIT.TE = 1;                         /* Transmit enable */
				SCI11.TDR = U0Buf[0];
				U0TimeOut = (State.TimerMs + 3);
				U0CarOff;
			}
		}
		break ;
	}
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void Uart0Chrono(void)
{

	/* Executed in chrono */
	switch (Menu.Network)
	{
	case JBUS :
		/* Launch initialisation of Uart0 */
		switch (U0InitCnt)
		{
		if (U0InitCnt == 0)
			U0InitCnt = 1;
		}
		break ;
		case LAN :
			LantxChrono();
			break ;
		case BFM :
			BfmChrono();
			break ;
	}
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void Uart0BR(uchar c)
{
	/* Set Uart0 baud rate */
	/* c : baud rate, c * 1200 */  
	/* PCLK = 48MHz, Set baudrate to 9600 bps
     155 = 48MHz / (64*0.5*9600) -1 */

	SCI11.SCR.BYTE = 0;

	SCI11.BRR = (48000000 / (32 * (c * 1200))) -1;

	/* Clear SCI6 RXI6/TXI6/TEI6 interrupt flag */
	IR(SCI11,RXI11) = 0x0u;
	IR(SCI11,TXI11) = 0x0u;
	IR(SCI11,TEI11) = 0x0u;

	/* Set bits TIE, RIE, RE and TE. Enable receive interrupts */
	SCI11.SCR.BYTE = 0xF0u;
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void Uart0FrmStart(void)
{
	/* Set Start of frame */
	/* In Ethernet mode, U0Ptr is fixed when the frame is recognized OK */
	/* Other informations are placed afterwards */
	switch (Menu.Network)
	{
	case JBUS :
		U0Len = 0;                /* Beginning of frame : nothing for Jbus */
	case LAN :
		LantxFrmStart();
		break ;
	case BFM :
		U0Len = 0;
		break ;
	}
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void Uart0FrmEnd(void)
{
	/* Set End of frame */
	switch (Menu.Network)
	{
	case JBUS :
		/* End of Modbus frame, delayed frame launching */
		U0CarOn;                                  /* Pending characters to be transmitted */
		if (UART0JBWAIT == 0)
		{
			RS485Out;
			U0Ptr = 0;                              /* Pointeur sur le caract�re � transmettre */
			SCI11.SCR.BIT.TE = 1;                             /* Transmit enable */
			SCI11.TDR = U0Buf[0];
			U0TimeOut = (State.TimerMs + 3);
		}
		else
		{
			U0TimeOut = (State.TimerMs + UART0JBWAIT);
		}
		break;
	case LAN :
		LantxFrmEnd();
		break ;
	case BFM :
		U0CarOn;                                  /* Pending characters to be transmitted */
		if (UART0BFMWAIT == 0)
		{
			RS485Out;
			U0Ptr = 0;                              /* Pointeur sur le caract�re � transmettre */
			SCI11.SCR.BIT.TE = 1;                           /* Transmit enable */
			SCI11.TDR = U0Buf[0];
			U0TimeOut = State.TimerMs + 3;
		}
		else
		{
			U0TimeOut = State.TimerMs + UART0BFMWAIT;
		}
		break ;
	}
}
