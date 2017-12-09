/*******************************************************************************
 * UART - Ethernet card driver
 *
 *
 *
 *
 *******************************************************************************/
/* Copyright (C) 2013 Enersys. All rights reserved.   */
/*******************************************************************************
 * File Name     : lantx.c
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
#define BFMTOUT     300000


char SendUart0 = 0;


extern uchar NetworkVer[8];             /* Revison of network module */

extern  ulong U0TimeOut;        /* Counter for delayed transmission and character timeout (3.5 char) */
extern  uint  U0InitCnt;
extern  uint  U0InitTemp;
extern  char  U0Buf[U0SIZE];    /* Uart0 input buffer */
extern  char  U0BufTx[U0SIZE];  /* Uart0 output buffer */
extern  uint  U0PtrTx;
extern  uint  U0LenTx;
extern  uint  U0DebTx;

uchar Tranmit = 0;

extern  uint  U0Ptr;
extern  uint  U0Len;
extern  uint  U0Deb;

extern uint UartStatus;
extern DisplayStruct Display;

/*******************************************************************************
 * Local Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void LantxInit(void)
{


}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void  LantxTimer(void)
{
	static int Timer1 = 0;

	BfmTout ++;
	if (BFMTOUT <= BfmTout)
	{
		PowerCutOff;
		BfmTout = 0;
	}


	/* Framing Error occurs sometimes */

	Timer1++;
	if (Timer1 > 500)
	{
		Timer1 = 0;
		if ((SCI11.SSR.BIT.ORER == 1) || (SCI11.SSR.BIT.FER == 1) || (SCI11.SSR.BIT.PER == 1))
		{
			SCI11.SSR.BYTE &= 0xC7u;
		}	
	}
    
    // MODIF 2.8
	//if (U0TimeOut <= State.TimerMs)
    if (U0TimeOut <= State.TimerSec)
	{
		/*switch (U0InitTemp)
		{
		case 0:
			U0TimeOut = State.TimerMs + LANTXTIMEOUT;
			U0InitTemp++;
			break;

		case 1:
			RS485In;                  
			U0InitTemp++;
			break;

		default:
			U0InitTemp ++;
			if (U0InitTemp > 100)
			{
				U0TimeOut = State.TimerMs + LANTXTIMEOUT;
				U0InitTemp = 1;
				RS485Out;
			}
			break;
		}*/
        //U0TimeOut = State.TimerMs + LANTXTIMEOUT;
        U0TimeOut = State.TimerSec + LANTXTIMEOUT;
        Uart0Init();
	}
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/



/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void  LantxChrono(void)
{
	/* Initialisation process, last = normal process */
	if (SendUart0 != 1) return;

	SendUart0 = 0;
	/* Increment of the init counter */
	switch (U0InitCnt)
	{
	case 0 :                    /* Reset initialisation */
	case 3 :
		U0InitCnt ++;
		break;
	case 4 :
		U0InitCnt ++;
		break;
	}
	/* Actions depending on the counter */
	switch (U0InitCnt)
	{
	case 1 :
		RS485In;                  /* Reset requested */
		U0InitCnt ++;
		break ;
	case 2 :
		RS485Out;
		U0InitCnt ++;
		break ;
	case 5 :
		SCI11.SSR.BYTE &= 0xC7u;
		LantxTx0();               /* yyy for initialisation */
		break ;
	case 6 :
		LantxTx1();               /* Segment 0 */
		U0InitCnt ++;
		break;
	case 8 :
		LantxTx2();               /* VS command */
		U0InitCnt ++;
		break ;
	case 10 :
		LantxTx3();               /* Segment 8 */
		U0InitCnt ++;
		break ;
	case 12 :
		LantxTx4();               /* Segment 9 */
		U0InitCnt ++;
		break ;
	case 14 :
		LantxTx5();               /* Quit config mode */
		U0InitCnt ++;
		break ;
	case 15 :
		if (Tranmit == 0)
		{
			MaskJbusOff;
			Uart0BR(32);
			U0InitCnt ++;
		}
		break;
	}
}  
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void LantxTxStart(void)
{
	Tranmit = 1;
	U0PtrTx = 0;
	U0Deb = 0;
	U0Ptr =0;
	SCI11.TDR = U0BufTx[0];
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void LantxTx(void)
{
	if (U0PtrTx < (U0LenTx - 1))
	{
		/* Send next character */
		U0PtrTx = (U0PtrTx + 1) % U0SIZE;
		SCI11.TDR = U0BufTx[U0PtrTx];
	}
	else
	{
		if (U0LenTx != 0)
			while (SCI11.SSR.BIT.TEND != 1);           /* At end of frame wait for end of car transmission */

        Tranmit = 0;
	}
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void LantxRx(void)
{
	/* Receive one character */
	uchar e;																/* End of loop indicator */
	uchar i;
	/* Suppress CR or LF in case of text messages */
	if (U0InitCnt != 16)
	{
		if ((U0Buf[(U0Ptr + U0SIZE - 1) % U0SIZE] == CR) || (U0Buf[(U0Ptr + U0SIZE - 1) % U0SIZE] == LF))
			U0Ptr = (U0Ptr + U0SIZE - 1) % U0SIZE;
	}
	e = 0;                               		/* Cha�ne consid�r�e bonne au d�part */
	do						                       		/* Loop while message not treated */
	{
		if (Size(U0Deb, U0Ptr, U0SIZE - 1) < 2)    /* V�rification de la taille (pas inf�rieure � 8) */
		{
			e = 1;                       				/* Message too short, exit loop */
		}
		else
		{
			/* If error : retry */
			if ((U0Buf[U0Deb % U0SIZE] == '9') &&
					(U0Buf[(U0Deb + 1) % U0SIZE] == '>'))
			{
				U0InitCnt --;
				e = 1;
			}
			else
			{
				switch (U0InitCnt)
				{
				case 9 :
					if (Size(U0Deb, U0Ptr, U0SIZE - 1) < 58)
						e = 1;
					else
					{
						NetworkVer[0] = U0Buf[(U0Deb + 17) % U0SIZE];
						NetworkVer[1] = U0Buf[(U0Deb + 18) % U0SIZE];
						NetworkVer[2] = '.';
						NetworkVer[3] = U0Buf[(U0Deb + 15) % U0SIZE];
						NetworkVer[4] = U0Buf[(U0Deb + 16) % U0SIZE];
						NetworkVer[5] = HexUchar((uchar *)&U0Buf[(U0Deb + 19) % U0SIZE]);
						NetworkVer[6] = HexUchar((uchar *)&U0Buf[(U0Deb + 21) % U0SIZE]);
						NetworkVer[7] = 0;
						if ((NetworkVer[5] == 'G') && (NetworkVer[6] == 'A'))
						{
							U0InitCnt ++;               /* Case Wiport */
							Display.WifiOffShow = 1;
						}
						else
						{
							/* Other cases */
							MaskJbusOff;
							MaskLanOn;
							MaskWLanOff;
							MaskWAscOff;
							MaskWHexOff;
							U0InitCnt = 14;
							Display.LanOffShow = 1;
						}
					}
					break;
				case 5 :
					//case 7 :
				case 11 :
				case 13 :
					if ((U0Buf[U0Deb % U0SIZE] == '0') &&
							(U0Buf[(U0Deb + 1) % U0SIZE] == '>'))
					{
						U0InitCnt ++;
						e = 1;
					}
					else
						U0Deb = (U0Deb + 1) % U0SIZE;
					break;
				case 7 :
					if ((U0Buf[U0Deb % U0SIZE] == '0') &&
							(U0Buf[(U0Deb + 1) % U0SIZE] == '>'))
					{
						U0InitCnt ++;
						e = 1;
					}
					else
						U0Deb = (U0Deb + 1) % U0SIZE;
					//asm("NOP");
					break;
				case 16 :                                       /* Case standart modbus messages */
					if (Size(U0Deb, U0Ptr, U0SIZE - 1) < 8)     /* Verification de la taille (pas inferieure a 8) */
					{
						e = 1;                                  /* Chaine trop courte, sortie, seul point de sortie du while */
					}
					else
					{
						/* Response to any slave address */
						Uart0FrmStart();                        /* Save spaces for additional characters */
						i = IQJbRx((uchar *)&U0Buf[0], &U0Deb, U0Ptr, U0SIZE, (uchar *)&U0BufTx[0], 0);
						if (i != 0)
						{
							U0CarOff;				                      /* Bit message � transmettre d�sarm� */
							//              SCI11.SCR.BIT.RE = 1;                         /* Receive disable */
							U0LenTx = U0LenTx + i;
							Uart0FrmEnd();
						}
						e = 1;
					}
					break;
				default :
					e = 1;
					break;
				}
			}
		}
	}
	while (e == 0);
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void LantxTx0(void)
{
	/* Characters "yyy" for initialisation */
	U0LenTx = 0;
	do
	{
		U0BufTx[U0LenTx] = 'y';
		U0LenTx ++;
	}
	while (U0LenTx < 3);
	LantxTxStart();
}  

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void LantxTx1(void)
{
	/* Setup record 0 */
	uint l;
	StructLanConfig TempSeg;  
	Read_SegFlashData (SEGLANCONFIG, (uchar *)&TempSeg);
	/* Command */
	U0LenTx = sprintf(&U0BufTx[0], "S0\n");
	/* First line + checksum */
	l = U0LenTx;
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], ":20000010");
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "%02X%02X%02X%02X", TempSeg.EthParam[0], TempSeg.EthParam[1], TempSeg.EthParam[2], TempSeg.EthParam[3]);
	//U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "0000082D00000000");  /* 8 bits subnet */
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "0000082DFFFFFFFF");  /* 8 bits subnet, password protected */
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "%02X%02X%02X%02X", TempSeg.EthParam[0 + 8], TempSeg.EthParam[1 + 8], TempSeg.EthParam[2 + 8], TempSeg.EthParam[3 + 8]);
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "4C");              /* 8 bits, no parity, 1 stop bit */
	//U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "02");              /* 9600 bauds */
	//U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "01");              /* 19200 bauds */
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "00");              /* 38400 bauds */
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "C0");              /* 18 : Modbus config */
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "01");              /* 19 : Slave adress = 01 */
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "FF");              /* Character timeout auto (3.5 char) */
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "00");              /* Message timeout (200 ms = mini) */
	//U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "80");              /* Message timeout (1000 ms) */
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "00080000000000000000");              
	HexCrc((uchar *)&U0BufTx[l]); 
	U0LenTx = U0LenTx + 2;
	/* Second line + checksum */
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "\n");
	l = U0LenTx;
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], ":200020100000000000000000000000000000000000000000000000000000000000000000"); 
	HexCrc((uchar *)&U0BufTx[l]); 
	U0LenTx = U0LenTx + 2;
	/* Second line + checksum */
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "\n");
	l = U0LenTx;
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], ":200040100000000000000000000000000000000000000000000000000000000000000000"); 
	HexCrc((uchar *)&U0BufTx[l]); 
	U0LenTx = U0LenTx + 2;
	/* Third line + checksum */
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "\n");
	l = U0LenTx;
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], ":18006010000A00000000334434430000000000000000000000000000"); 
	//U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], ":18006010000000000000334434430000000000000000000000000000"); 
	HexCrc((uchar *)&U0BufTx[l]); 
	U0LenTx = U0LenTx + 2;
	/* End + checksum */
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "\n");
	l = U0LenTx;
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], ":00000001");
	HexCrc((uchar *)&U0BufTx[l]); 
	U0LenTx = U0LenTx + 2;
	LantxTxStart();
}

void LantxTx2(void)
{
	U0LenTx = sprintf(&U0BufTx[0], "VS\n");
	LantxTxStart();
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void LantxTx3(void)
{
	/* Setup record 8 : */
	uint l;  
	uchar i;
	uchar *c;
	uchar TempSeg[64];
	StructLanConfig *Temp0;
	StructWifiConfig *Temp1;
	Temp0 = (StructLanConfig *)&TempSeg[0];
	Temp1 = (StructWifiConfig *)&TempSeg[0];

	/* Read LANCONFIG segment */
	Read_SegFlashData (SEGLANCONFIG, (uchar *)&TempSeg);

	/* Command : S8 */
	U0LenTx = sprintf(&U0BufTx[0], "S8\n");
	/* First line + checksum : 0 to 31 */
	l = U0LenTx;
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], ":03000010");
	//U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "7FB041");       
	//U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "6DA241"); 
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "7DA0");
	/* Masks for display */
	MaskJbusOff;
	MaskLanOff;
	MaskWLanOff;
	MaskWAscOff;
	MaskWHexOff;
	/* Encryption type */
	switch ((*Temp0).Security)
	{
	case 0 :
		U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "00");      /* No */
		MaskWLanOn;
		break ;
	case 1 :
		U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "21");      /* WEP 64 */
		MaskWHexOn;
		break ;
	case 2 :
		U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "41");      /* WEP 128 */
		MaskWHexOn;
		break ;
	case 3 :
		U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "66");      /* WPA */
		MaskWAscOn;
		break ;
	case 4 :
		U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "67");      /* WPA2/PSK : text key */
		MaskWAscOn;
		break ;
	}
	/* Checksum */
	HexCrc((uchar *)&U0BufTx[l]); 
	U0LenTx = U0LenTx + 2;
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "\n");
	/* SSID : 32 bytes */
	l = U0LenTx;
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], ":20000310");
	/* Left justify and remove spaces */ 
	c = &(*Temp0).SSID[0];
	c = RTrim(LTrim(c, 32), 32);    
	/* SSID */
	i = 0;
	do
	{
		U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "%02X", (*Temp0).SSID[i]);
		i ++;
	}
	while (i < 32);
	HexCrc((uchar *)&U0BufTx[l]); 
	U0LenTx = U0LenTx + 2;
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "\n");
	/* Key type : 1 byte */
	l = U0LenTx;
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], ":03004510");
	/* Group encryption */
	switch ((*Temp0).Security)
	{
	case 0 :
		U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "00");
		break ;
	case 1 :
	case 2 :
		U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "02");      /* WEP */
		break ;
	case 3 :
	case 4 :
		U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "03");      /* TKIP */
		break ;
	}
	/* Key length */
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "00");  
	/* Key type */
	switch ((*Temp0).Security)
	{
	case 0 :
	case 1 :
	case 2 :
		U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "00");      /* Hexa */
		break ;
	case 3 :
	case 4 :
		U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "01");      /* Text */
		break ;
	}
	HexCrc((uchar *)&U0BufTx[l]); 
	U0LenTx = U0LenTx + 2;
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "\n");
	/* End + checksum */
	l = U0LenTx;
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], ":00000001");
	HexCrc((uchar *)&U0BufTx[l]); 
	U0LenTx = U0LenTx + 2;
	LantxTxStart();
}  

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void LantxTx4(void)
{
	/* Setup record 9 */
	uint l; 
	uint i;
	uchar s;                          /* Security */
	uchar TempSeg[64];
	StructLanConfig *Temp0;
	StructWifiConfig *Temp1;
	Temp0 = (StructLanConfig *)&TempSeg[0];
	Temp1 = (StructWifiConfig *)&TempSeg[0];

	/* Read infos */
	Read_SegFlashData (SEGLANCONFIG, (uchar *)&TempSeg);
	s = (*Temp0).Security;  
	Read_SegFlashData (SEGWIFI, (uchar *)&TempSeg);


	/* Command */
	//U0LenTx = sprintf(&U0BufTx[0], "\nS9\n");
	U0LenTx = sprintf(&U0BufTx[0], "S9\n");
	/* First line + checksum : 7 to 38 */
	l = U0LenTx;
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], ":20000710");
	/* Encryption type */
	switch (s)
	{
	case 0 :
		/* No encryption key */
		i = 0;
		do
		{
			U0BufTx[U0LenTx] = ZERO;
			U0LenTx ++;
			U0BufTx[U0LenTx] = ZERO;
			U0LenTx ++;
			i ++;
		}
		while (i < 32);
	case 1 :
	case 2 :
		/* Encryption key for WEP */
		i = 0;
		do
		{
			/* Case hexa : wep, no wlan */
			U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "%02X",(*Temp1).WepKey[i]);
			i ++;
		}
		while (i < 26);
		U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "000000000000");
		break ;
	case 3 :
	case 4 :
		/* Encryption key for WPA : 32 char */
		i = 0;
		do
		{
			U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "%02X", (*Temp1).WLanKey[i]);
			i ++;
		}
		while (i < 32);
		break ;
	}
	HexCrc((uchar *)&U0BufTx[l]); 
	U0LenTx = U0LenTx + 2;
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], "\n");
	/* End + checksum */
	l = U0LenTx;
	U0LenTx = U0LenTx + sprintf(&U0BufTx[U0LenTx], ":00000001");
	HexCrc((uchar *)&U0BufTx[l]); 
	U0LenTx = U0LenTx + 2;
	LantxTxStart();
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void LantxTx5(void)
{
	/* Quit mode diagnostics */
	U0LenTx = sprintf(&U0BufTx[0], "QU\n");
	LantxTxStart();
} 

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void HexCrc(uchar *t)
{
	/* Calculate the Hex Crc of the string beginning with : */
	/* Crc is added at the end (2 char) */
	/* *t : string to calculate */
	/* *(t + 1) and *(t + 2) indicate length  */
	uchar c;  /* Crc */
	uint n;   /* Number of bytes */
	uchar i;  /* Byte counter */
	uint v;   /* byte value */

	sscanf((const char *)t + 1, "%2x", &n);
	c = 0;
	i = 0;
	do
	{
		sscanf((const char *)t + (2 * i) + 1, "%2x", &v);
		c = c + v;
		i = i + 1;
	}
	while (i < (n + 4));
	c = ~c + 1;
	sprintf((char *)t + 2 * n + 9, "%02X", c);
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void  LantxFrmStart(void)
{
	/* Beginning of frame : nothing for Jbus */
	U0LenTx = 0;
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/

void  LantxFrmEnd(void)
{
	/* End of JbBus frame */
	LantxTxStart();							/* Lancement de temporisation d'�mission */
	U0TimeOut = State.TimerMs + LANTXTIMEOUT;
} 

#if (1)

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uchar *LTrim(uchar *c, uchar l)
{
	/* Remove left NUL or SPACE */
	/* l : number of char */
	uchar i;
	uchar j;
	/* Search how many char to shift */
	i = 0;
	while ((i < l) && ((*(c + i) == NUL) || (*(c + i) == SPACE)))
		i ++;
	/* Shift char */
	j = 0;
	while (j < (l - i))
	{
		*(c + j) = *(c + i + j);
		j ++;
	}
	/* Fill remaining with NUL */
	while (j < l)
	{
		*(c + j) = NUL;
		j ++;
	}
	return c;
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/

uchar *RTrim(uchar *c, uchar l)
{
	/* Replace right NUL or SPACE by NUL */
	/* l : number of char */
	uchar i;
	i = l;
	while ((i != 0) && ((*(c + i - 1) == NUL) || (*(c + i - 1) == SPACE)))
	{
		*(c + i - 1) = NUL;
		i --;
	}
	return c;
}

#endif

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/

#if (0)
/* Control SSID string : remove leading spaces */
i = 31;
e = 0;
do
{
	if ((*Temp0).SSID[i] == SPACE)
		(*Temp0).SSID[i] = 0;
	else
		e = 1;
	i --;
}
while ((i != 0) && (e == 0));

#endif



/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uchar HexUchar(uchar *i)
{
	/* Convert Hex to uchar */
	/* *i : adress of the high order char */
	uint j;
	sscanf((const char *)i, "%2x", &j);
	return (uchar)j;
}
