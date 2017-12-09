/*
 * sci1.c
 *
 *  Created on: 1 mars 2013
 *      Author: duhautf
 */

#include "include.h"

#define	SCI1TIMEOUT	100				// ms

uchar SCI1Buf[SCI1SIZE];          	/* SCI0 input/output buffer */
uint  SCI1Ptr;
uint  SCI1Len;
uint  SCI1Deb;
ulong SCI1TimeOut;              	/* Counter for delayed JBus transmission */

/**
 * @brief  This function initialises the SCI channel
 * @param  None
 * @retval None
 */
void Init_SCI1 (void)
{
	/* Protection off */
	SYSTEM.PRCR.WORD = 0xA503u;

	/* Cancel the SCI1 module clock stop mode */
	MSTP_SCI1 = 0u;

	/* Protection on */
	SYSTEM.PRCR.WORD = 0xA500u;

	/* Configure SCI1's TXD1, RXD1 and SCK1 pins for peripheral functions - see page 661  */
	PORTF.PMR.BIT.B0 = 1;
	PORTF.PMR.BIT.B2 = 1;

	/* PWPR.PFSWE write protect off */
	MPC.PWPR.BYTE = 0x00u;

	/* PFS register write protect off */
	MPC.PWPR.BYTE = 0x40u;

	/* Configure SCI1's communication
	 * pins PF0 = TXD1 / PF2 = RXD1 on LQFP176
	 * see page 691/712 on datasheet */

	/* Set PortF's pin 0 function to TXD1 - see page 691/712 */
	MPC.PF0PFS.BYTE = 0x0Au;
	/* Set PortF's pin 2 function to RXD1 - see page 691/712 */
	MPC.PF2PFS.BYTE = 0x0Au;
	/* Set PortF's pin 1 function to SCK1 - see page 691/712 */
	//MPC.PF1PFS.BYTE = 0x0Au;

	/* Turn on PFS register write protect */
	MPC.PWPR.BYTE = 0x80u;

	/* Cancel SCI0 module stop mode */
	SYSTEM.MSTPCRB.LONG = 0x7FFFFFFFu;

	/* Disable All bits and CKE0,1 = 00 */
	SCI1.SCR.BYTE = 0x00u;

	/* Set output port TXD1(PF0) */
	PORTF.PDR.BIT.B0 = 1u;
	/* Set Receive port RXD1(PF2)*/
	PORTF.PDR.BIT.B2 = 0u;
	/* Add pull-up */
	PORTF.PCR.BIT.B0 = 1;
	PORTF.PCR.BIT.B2 = 1;

	/* disable All bits and CKE0,1 = 00 once more */
	SCI1.SCR.BYTE = 0x00u;

	/* CKS[0-1] = 00: PCLK 0 div
       1Sop 8bits non-parity     */
	SCI1.SMR.BYTE = 0x00u;

	/* SMIF = 0: Serial  */
	SCI1.SCMR.BYTE = 0xf2u;

	/* PCLK = 48MHz, Set baudrate to 115200 bps
       12 = 48MHz / (64*0.5*115200) -1 */
	SCI1.BRR = 12u;     // 115200bds
    //SCI1.BRR = 155u;    // 9600bds
    
	/* Allow a small delay */
	for (uint16_t i = 0u; i < 20u; i++ )
	{
	}

  	/* Clear bits ORER, PER and FER */
    SCI1.SSR.BYTE &= 0xC7u;

    /* Set SCI1 RXI1/TXI1/TEI1 interrupt priority level to 7 */
    IPR(SCI1,RXI1) = 0x7u;
    IPR(SCI1,TXI1) = 0x7u;
    IPR(SCI1,TEI1) = 0x7u;
    /* Enable SCI1 RXI0/TXI1/TEI1 interrupts */
    IEN(SCI1,RXI1) = 0x1u;
    IEN(SCI1,TXI1) = 0x0u;
    IEN(SCI1,TEI1) = 0x0u;
    /* Clear SCI1 RXI1/TXI1/TEI1 interrupt flag */
    IR(SCI1,RXI1) = 0x0u;
    IR(SCI1,TXI1) = 0x0u;
    IR(SCI1,TEI1) = 0x0u;

    /* Set bits TIE, RIE, RE and TE. Enable receive interrupts */
    SCI1.SCR.BYTE = 0xF0u;

    SCI1CarOff;
    SCI1Deb = 0;
    SCI1Len = 1;
    SCI1Ptr = 0;
    SCI1TimeOut = State.TimerMs + SCI1TIMEOUT;
}


/**
 * @brief  Reset de la réception JBus suite à un temps d'attente trop long ou message parti
 * @param  None
 * @retval None
 */
void Reset_SCI1(void)
{
	SCI1CarOff;                             /* No pending characters to be transmitted */
    SCI1Deb = 0;
    SCI1Len = 1;                           	/* Avoid loop at power up */
    SCI1Ptr = 0;                            /* Replace le pointeur a 0 pour une réception */
	
    /* Set bits TIE, RIE, RE and TE. Enable receive interrupts */
    IEN(SCI1,RXI1) = 0x1u;
	SCI1.SCR.BIT.RIE = 1;                   /* Receive int enable */
	SCI1.SCR.BIT.TEIE = 0;
	SCI1.SCR.BIT.TIE = 0;
	SCI1.SCR.BIT.TE = 1;                   	/* Transmit enable */
	SCI1.SCR.BIT.RE = 1;                   	/* Receive enable */
	SCI1.SSR.BYTE &= 0xC7u;					/* Clear bits ORER, PER FER */
	
    SCI1TimeOut = State.TimerMs + SCI1TIMEOUT;
}

/**
 * @brief  Executed every ms
 * @param  None
 * @retval None
 */
void Timer_SCI1 (void)
{
	if (SCI1TimeOut <= State.TimerMs)
	{
		Reset_SCI1();
	}
}


/**
 * @brief  SCI1 receive interrupt handler, called when a character is
 *         received. -> see vect.h for interrupt vector definition
 * @param  None
 * @retval None
 */
void Excep_SCI1_RXI1(void)
{
	/* Store the received character */
    SCI1Buf[SCI1Ptr] = SCI1.RDR;            /* Read current byte */
    SCI1Ptr = (SCI1Ptr + 1) % SCI1SIZE;
    SCI1CarOff;								/* No pending character to be send */
    SCI1TimeOut = (State.TimerMs + SCI1TIMEOUT);  /* Reset counter for  sci0 reset */
    SCI1Len = JbusRx(&SCI1Buf[0], &SCI1Deb, SCI1Ptr, SCI1SIZE, &SCI1Buf[0], 0);
    if (SCI1Len != 0)
    {
        SCI1CarOn;
        //SCI1.SCR.BIT.RIE = 0;		// disable RX int
        SCI1Ptr = 0;
		IEN(SCI1,TXI1) = 0x1u;
        SCI1.SCR.BIT.TIE = 1; 		// enable TX int
        SCI1.TDR = SCI1Buf[0];
        SCI1TimeOut = State.TimerMs + SCI1TIMEOUT;
    }
}

/**
 * @brief  SCI1 transmit interrupt handler, called when a character is
 *         send. -> see vect.h for interrupt vector definition
 * @param  None
 * @retval None
 */
void Excep_SCI1_TXI1(void)
{
	 if ((SCI1Ptr < (SCI1Len - 1)) && (SCI1Len != 0))
    {
        /* Send next character */
        SCI1Ptr = SCI1Ptr + 1;
        SCI1.TDR = SCI1Buf[SCI1Ptr];
        SCI1TimeOut = State.TimerMs + SCI1TIMEOUT;
    }
    else
    {
        SCI1Ptr = SCI1Ptr + 1;
		IEN(SCI1,TEI1) = 0x1u;
        SCI1.SCR.BIT.TEIE = 1;				  /* Interrupt on transmission completed */
    }
}

/**
 * @brief  SCI1 complete transmit interrupt handler, called when last  character is
 *         send. -> see vect.h for interrupt vector definition
 * @param  None
 * @retval None
 */
void Excep_SCI1_TEI1(void)
{
	if (SCI1Ptr >= SCI1Len)
    {
        SCI1CarOff;                           /* No more byte to transmit */
        Reset_SCI1();
    }
}
