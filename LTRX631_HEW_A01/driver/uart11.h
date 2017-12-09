/*******************************************************************************
 * UART 11 driver
 *
 *
 *
 *
 *******************************************************************************/
/* Copyright (C) 2013 Enersys. All rights reserved.   */
/*******************************************************************************
 * File Name     : Uart.h
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

/*******************************************************************************
 * User Defines (Project Level Includes)
 *******************************************************************************/
/* ********** UART0 UART1 et JBUS ********** */
#define U0SIZE                512       /* 2 exponent mandatory */
#define JBCRC                 0xFFFF    /* Jbus start polynom */
#define UART0JBWAIT           0         /* Wait time (in ms) for Uart 0 in JBus mode */
#define UART0BFMWAIT          2         /* Wait time (in ms) for Uart 0 in BFM mode */
#define JBTMP                 4		      /* Time of non activity of JBus before frame reset (9600 bauds) */
#define BFMTMP                50        /* Time of non activity of JBus before frame reset (9600 bauds) */

#define ETHMODFRM             20        /* Number of Uart0init cnt in frame mode */

#define JBUS                  0
#define LAN                   1
#define BFM                   2

// MODIF 2.8
//#define   LANTXTIMEOUT        240000    /* 240 seconds of inactivity = reset */
#define   LANTXTIMEOUT        600000    /* 600 seconds of inactivity = reset */

#define ZERO        48
#define NUL         0
#define CR          0x0D      /* cariage return */
#define LF          0x0A      /* Line feed */
#define BS          0x5C      /* Back slash \ */
#define XON         17        /* Xon : CTRL + Q */
#define XOFF        19        /* Xoff : CTRL + S */


#define BitRs485      0x00000020
#define BitU0Car      0x00000010

#define U0Car		    (UartStatus & BitU0Car)
#define U0CarOn	    	(UartStatus |= BitU0Car)
#define U0CarOff	    (UartStatus &= ~BitU0Car)


#define RS485In	        PORT6.PODR.BIT.B3 = 0
#define RS485Out	    PORT6.PODR.BIT.B3 = 1


/*******************************************************************************
 * Internal variables
 *******************************************************************************/



/*******************************************************************************
 * Local Function Prototypes
 *******************************************************************************/
void Uart11Init(void);
void Uart0Reset(void);
void Uart0Init(void);
void Uart0Timer(void);
void Uart0Chrono(void);
void Uart0BR(uchar c);
void Uart0FrmStart(void);
void Uart0FrmEnd(void);
