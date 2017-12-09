/****************************************************************************************/
/* File bfm.c                                                                           */
/*                                                                                      */
/* Battery Fleet Management                                                             */
/* Initialisation for asynchronous mode (Modbus/Ethernet)                               */
/*                                                                                      */
/* Revision History                                                                     */
/* DD.MM.YYYY OSO-UID Description                                                       */
/* 14.05.2009 RTA-MGF initial release                                                   */
/* 28.05.2009 RTA-MGF Independant from uart1 (possible to use simultaneously)           */
/*                    Correct delayed response                                          */
/* 11.12.2009 RTA-MGF Full interrupt on transmit                                        */
/*                                                                                      */
/*                                                                                      */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/

#include  "include.h"         /* Fonctions externes */

#define BfmInc      *rtop = (*rtop + 1) % size
#define BFMTOUT     300000

ulong BfmTout = 0;



void BfmChrono(void)
{
	static int Timer1 = 0;

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
}


/************************************************************
 * INTERRUPTIONS
 ************************************************************/

uchar BfmRx(uchar *r, uint *rtop, uint cur, uint size, uint ttop)
{
	/* Receive one character */
	uchar e;																/* End of loop indicator */
	/* Analyse Bfm frame */
	e = 0;                               		/* Cha�ne consid�r�e bonne au d�part */
	do						                       		/* Boucle tant que message peut �tre tra�t� */
	{
		if (Size(*rtop, cur, size - 1) < 6)   /* V�rification de la taille (pas inf�rieure � 6) */
		{
			e = 1;                       				/* Cha�ne trop courte, sortie, seul point de sortie du while */
		}
		else
		{
			if ((*(r + *rtop) == 0) &&                              /* First 0 */
					(*(r + ((*rtop + 1) % size)) == 0) &&                 /* Second 0 */
					(*(r + ((*rtop + 2) % size)) == 0) &&                 /* Third 0 */
					(*(r + ((*rtop + 4) % size)) == Menu.JbEsc) &&        /* Slave number */
					((*(r + ((*rtop + 3) % size)) + *(r + ((*rtop + 4) % size))) == *(r + ((*rtop + 5) % size))))
			{
				e = BfmFrm(r, rtop, size, ttop);
			}
			else
			{
				BfmInc;
			}
		}
	}
	while (e == 0);
	if (e == 1)
		e = 0;
	return e;
}

uchar BfmFrm(uchar *r, uint *rtop, uint size, uint ttop)
{
	/* Analyse a valid frame, slave address is OK, case Jbus or ethernet */
	/* return : 0 if frame is not valid */
	/*          1 if frame is valid and treated */
	uchar e;																/* End of loop indicator */
	e = 0;
	switch (*(r + ((*rtop + 3) % size)))    /* Select fonction code */
	{  
	case 0x11 :                           /* No power cut */
		PowerCutOff;
		e = BfmFnc(r, ttop);
		break ;
	case 0x16 :                           /* Power cut */
		PowerCutOn;
		BfmTout = 0;
		e = BfmFnc(r, ttop);
		break ;
	case 0x17 :                           /* Easy Frame */
		BfmTout = 0;
		e = BfmEasyFrame(r, ttop);
		break ;
	default :
		BfmInc;
		break ;
	}
	return e;
}

void BfmTimer(void)
{
	BfmTout ++;
	if (BFMTOUT <= BfmTout)
		PowerCutOff;
}

/************************************************************
 * FONCTIONS
 ************************************************************/

uchar BfmFnc(uchar *r, uint ttop)
{
	/* e : slave address */
	/* *r : buffer to fill */
	/* *top : first address */
	/* return : number of written bytes */
	uint i;
	uint j;
	j = ttop;
	i = ttop;                               /* Saves place of first byte */
	*(r + i) = Menu.JbEsc;
	i ++;
	/* Power */
	if (PowerCut == 0)                      /* Power cut */
	{
		*(r + i) = HighByte(DataR.Power);
		i ++;
		*(r + i) = LowByte(DataR.Power);
		i ++;
	}
	else
	{
		*(r + i) = 0;                  /* 00 */
		i ++;
		*(r + i) = 0;                  /* 00 */
		i ++;
	}
	/* Status */
	switch (State.Charger)
	{
	case StateChgerIdle :
		*(r + i) = 0;                /* Wait */
		i ++;
		break ;
	case StateChgerChg :
		switch (State.Phase & 0xF0)
		{
		case  StatePhDefCur:
			//*(r + i) = 0x10;         /* DF1 */
			*(r + i) = 0x30;         /* DF1 */
			i ++;
			break;
		case  StatePhDefFuse:
			//*(r + i) = 0x20;         /* DF2 */
			*(r + i) = 0x50;         /* DF2 */
			i ++;
			break;
		case  StatePhDefTemp:
			//*(r + i) = 0x60;         /* DF6 */
			*(r + i) = 0xD0;         /* DF6 */
			i ++;
			break;
		case  StatePhDefBadBat:
			//*(r + i) = 0x30;         /* DF3 */
			*(r + i) = 0x70;         /* DF3 */
			i ++;
			break;
		default:
			switch (State.Charge)
			{
			case StateChgStAvail:
			case StateChgAvail:
			case StateChgStartComp:
			case StateChgComp:
				*(r + i) = 0x04;         /* Dispo */
				i ++;
				break ;
			case StateChgStartEqual:
			case StateChgEqual:
				*(r + i) = 0x08;         /* Egalisation */
				i ++;
				break ;
			default :
				*(r + i) = 0x02;         /* Charge */
				i ++;
				break ;
			}
			break ;
		}
		break ;
		default :
			*(r + i) = 0x00;             /* Attente */
			i ++;
			break ;
	}
	/* Nb cells */
	*(r + i) = Menu.NbCells;
	i ++;
	/* Ah  + Easy/WiIQ Link*/
	*(r + i) = HighByte(DataR.Ah) | 0xC0;
	if (IQWIIQLink != 0)
		*(r + i) |= 0x20;
	i ++;
	*(r + i) = LowByte(DataR.Ah);
	i ++;
	/* Checksum */
	*(r + i) = 0;
	do
	{
		*(r + i) += *(r + j);
		j ++;
	}
	while (j < i);
	return (i - ttop + 1);
}  


uchar BfmEasyFrame(uchar *r, uint ttop)
{
	/* e : slave address */
	/* *r : buffer to fill */
	/* *top : first address */
	/* return : number of written bytes */
	uint i;
	uint j;
	uint k;
	j = ttop;
	i = ttop;                           /* Saves place of first byte */
	*(r + i) = Menu.JbEsc;              // charger address
	i ++;
	*(r + i) = IQData.BatTemp;          // T�C
	i ++;
	*(r + i) = IQData.BatTechno - 1;    // techno
	i ++;
	*(r + i) = IQData.NbCells;          // nb cells
	i ++;

	*(r + i) = 0;                       // nb cycles
	i ++;
	*(r + i) = 0;
	i ++;
	*(r + i) = HighByte(IQData.BatCap);  // Capacity
	i ++;
	*(r + i) = LowByte(IQData.BatCap);
	i ++;
	*(r + i) = IQData.BatWarning;       // Warning
	if ((IQData.BatWarning & BatWarningBal1) != 0) // Balance Low
		*(r + i) |= BatWarningBal1;
	i ++;

	for (k=0; k<14; k++)
	{
		*(r + i) = IQData.BatSN[k];      // Serial Number
		i ++;
	}
	*(r + i) = 0;                       // 0x00 (for complete serial number to size 16)
	i ++;
	*(r + i) = 0;                       // 0x00 (for complete serial number to size 16)
	i ++;
	*(r + i) = 0;                       // 0x00 (for complete serial number to size 16)
	i ++;

	/* Checksum */
	*(r + i) = 0;
	do
	{
		*(r + i) += *(r + j);
		j ++;
	}
	while (j < i);
	return (i - ttop + 1);
}  

