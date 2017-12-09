/****************************************************************************************/
/* File jbusiq.c                                                                        */
/*                                                                                      */
/* Modbus functions for com iq                                                          */
/*                                                                                      */
/* Received buffer is analysed diectly (because of rolling)                             */
/* Transmitted buffer is created for each function                                      */
/*                                                                                      */
/*                                                                                      */
/*                                                                                      */
/*                                                                                      */
/* Revision History                                                                     */
/* DD.MM.YYYY OSO-UID Description                                                       */
/* 14.05.2009 RTA-MGF Initial release                                                   */
/* 28.05.2009 RTA-MGF Add mask register                                                 */
/*                    Remove JbTmrCar                                                   */
/*                    Add function 3 for ROM area                                       */
/*                    Add function 16 and 22 for FMC area                               */
/*                                                                                      */
/****************************************************************************************/

#include  "include.h"         /* Fonctions externes */

#define   JBERRADD            0x02
#define   JBERRDATA           0x03
#define   JBSLAVEBUSY         0x06

extern uchar IQFrameRx[MAX_LEN];
extern uchar IQFrameTx[MAX_LEN];
extern uchar IQLenRx;
extern uchar IQLenTx;
extern DisplayStruct Display;

/* ********** JBus Function ********** */ 

void IQJbusChrono(void)
{
	/* Scan if a request is pending */
	uint i;
	uint j;

	if (IQFrameRxJb != 0)
	{
		j = 0;
		i = IQJbRx(&IQFrameRx[0], &j, IQLenRx, MAX_LEN, &IQFrameTx[0], 0);
		if (i != 0)
		{
			IQLenTx = i;
			IQFrameTxJbOn;
		}
		IQFrameRxJbOff;
	}
}  

uint IQJbRx(uchar *r, uint *rtop, uint cur, uint size, uchar *t, uint ttop)
{
	/* Receive one character */
	/* Build frame without checksum */
	/* *r : top of received buffer */
	/* *rtop : first character valid in received buffer */
	/* cur : current character received in buffer */
	/* size : size of receive buffer (power of 2 for mask) */
	/* *t : top of transmit buffer (can be the same as received buffer) */
	/* ttop : first character to fill in transmit buffer */
	/* return : number of written characters */
	uint e;																  /* End of loop indicator */
	uint 	a;																/* Adresse de d�but, record number */
	uchar i;
	uchar s;                                /* Slave number */
	uchar c;                                /* Function code */
	uchar n;																/* Number of words */
	uint  f;                                /* File number */
	uint  g;                                /* Record length */
 
	
	/* Analyse de la trame J-Bus, d�bute � m, n pointeur de fin */
	e = 0xFFFF;                            	/* Cha�ne consid�r�e bonne au d�part */
	do						                       		/* Loop until charcaters could be analysed */
	{
		if (Size(*rtop, cur, size - 1) < 8)   /* Control of size (not greater than 8) */
		{
			e = 0;                       				/* String too short */
		}
		else
		{
			s = *(r + *rtop);                   /* Slave address */
			if ((s == 0) || (s == 1) || (s == Menu.JbEsc))  /* Slave address or 0 */
			{
				c = *(r + ((*rtop + 1) % size));  /* Function code */
				switch (c)                        /* Selection on function code */
				{
                case 0x41 : // IP address of ethernet module
                    if (CrcCalc(r, *rtop, 7, size - 1) == 0)
					{
                        for (i=0; i<4; i++)
                        {
                            IP_DHCP[i] = *(r + ((*rtop + 2 + i)% size));
                        }
                        *rtop = (*rtop + 7) % size;
                    }
                    else
                    {   // MODIF R2.7
                        *rtop = (*rtop + 1) % size;
                    }
                    break;
                        
				case 0x03 :                     /* Lecture de N words */
				case 0x04 :
				{
                    if (Display.WifiOffShow != 0)
	                {
		                Display.WifiOnShow = 1;
	                }
	                else if (Display.LanOffShow != 0)
	                {
		                Display.LanOnShow = 1;
	                }
					/* a : jbus address, n : number of words to read */
					n = *(r + ((*rtop + 5) % size));          /* Number of words to read <= 125 */
					if (n <= ((size - 6) / 2) && (0 < n) && (n <= 0x7D))
					{
						/* For faster response Crc is calculated afterwards */
						if (CrcCalc(r, *rtop, 7, size - 1) == 0)
						{
							a = (*(r + ((*rtop + 2) % size))) * (uint)256 + *(r + ((*rtop + 3) % size));
							if ((ADRJBUSFMC <= a) && ((a + n) <= (ADRJBUSFMC + SIZJBUSFMC)))
							{
								/* Reading FMC technology area */
								i = IQJbFnc3FMC(t + (ttop + 1) % size, c, a - ADRJBUSFMC, n);
								*(t + ttop) = s;                  /* Slave address afterwards */
								e = i + 1;
							}
							else if ((ADRJBUSRAMRW <= a) && ((a + n) <= (ADRJBUSRAMRW + SIZJBUSRAMRW)))
							{
								/* Reading in RAM RW area */
								i = IQJbFnc3Ram(t + (ttop + 1) % size, c, a - ADRJBUSRAMRW, n);
								*(t + ttop) = s;                  /* Slave address afterwards */
								e = i + 1;
							}
							else if ((ADRJBUSRAMRO <= a) && ((a + n) <= (ADRJBUSRAMRO + SIZJBUSRAMRO)))
							{
								/* Reading in RAM RO area */
								i = IQJbFnc3Ram(t + (ttop + 1) % size, c, a - ADRJBUSRAMRO, n);
								*(t + ttop) = s;                  /* Slave address afterwards */
								e = i + 1;
							}
							else  if ((ADRJBUSROM <= a) && ((a + n) <= (ADRJBUSROM + SIZJBUSROM)))
							{
								/* Reading in ROM area */
								i = IQJbFnc3Rom(t + (ttop + 1) % size, c, a - ADRJBUSROM, n);
								*(t + ttop) = s;                  /* Slave address afterwards */
								e = i + 1;
							}
							else if ((ADRJBUSEEP <= a) && ((a + n) < ((ulong)ADRJBUSEEP + SIZJBUSEEP)) && (n == 31))
							{         /* Reading in EEP area, segment of 62 bytes */
								i = IQJbFnc3EepSeg(t + (ttop + 1) % size, c, a - ADRJBUSEEP, n);
								*(t + ttop) = s;                  /* Slave address afterwards */
								e = i + 1;
							}
							else
							{
								/* Address error */
								i = IQJbErr(t + (ttop + 1) % size, c, JBERRADD);
								*(t + ttop) = s;                  /* Slave address afterwards */
								e = i + 1;
							}
						}
						else
						{
							*rtop = (*rtop + 1) % size;
						}
					}
					else
					{
						*rtop = (*rtop + 1) % size;
					}
				}
				break;
				case 0x05 :                     /* Write 1 words */
				case 0x06 :
				{
					if (Size(*rtop, cur, size - 1) < 8)     /* Test size */
						e = 0;                      /* Incr�ment du d�but de trame au caract�re suivant et modulo */
					else
					{
						a = (*(r + ((*rtop + 2) % size))) *256 + (*(r + ((*rtop + 3) % size)));
						g = (*(r + ((*rtop + 4) % size))) *256 + (*(r + ((*rtop + 5) % size)));
						if (CrcCalc(r, *rtop, 7, size - 1) == 0)
						{
							if ((ADRJBUSFMC <= a) && ((a + n) <= (ADRJBUSFMC + SIZJBUSFMC)))
							{
								/* Writing FMC technology area */
								i = IQJbFnc5FMC(t + (ttop + 1) % size, c, a - ADRJBUSFMC, g);
								*(t + ttop) = s;                  /* Slave address afterwards */
								e = i + 1;
							}
							else if ((ADRJBUSRAMRW <= a) && ((a + n) <= (ADRJBUSRAMRW + SIZJBUSRAMRW)))
							{
								/* Writing in RAM RW area */
								i = IQJbFnc5Ram(t + (ttop + 1) % size, c, a - ADRJBUSRAMRW, g);
								*(t + ttop) = s;                  /* Slave address afterwards */
								e = i + 1;
							}
							else
							{
								/* Address error */
								i = IQJbErr(t + (ttop + 1) % size, c, JBERRADD);
								*(t + ttop) = s;                  /* Slave address afterwards */
								e = i + 1;
							}
						}
						else
						{
							*rtop = (*rtop + 1) % size;
						}
					}
				}
				break;
				case 0x10 :                     /* Write N words */
				{
					a = (*(r + ((*rtop + 2) % size))) *256 + (*(r + ((*rtop + 3) % size)));
					n = (*(r + ((*rtop + 5) % size)));                    /* Nombre de mots � �crire */
					if (Size(*rtop, cur, size - 1) < (2 * n + 9))         /* Test taille */
						e = 0;                 			/* Incr�ment du d�but de trame au caract�re suivant et modulo */
					else
					{
						if (((*(r + ((*rtop + 4) % size))) == 0) &&         /* 00 */
								((*(r + ((*rtop + 6) % size))) == (2 * n)) &&   /* Byte = 2 * word */
								(0 < n) &&                                    /* 0 < n */
								(n <= 0x7B) &&                                /* n <= 7B */
								(n <= ((size - 10) / 2)))	        /* Control buffer size */
							/* For faster response Crc is calculated afterwards */
							if (CrcCalc(r, *rtop, (uint)(2 * n + 8), size - 1) == 0)
							{
								if ((ADRJBUSFMC <= a) && ((a + n) <= (ADRJBUSFMC + SIZJBUSFMC)))
								{
									/* Writing FMC technology area */
									i = IQJbFnc16FMC(r ,*rtop + 7, size, t + (ttop + 1) % size, a - ADRJBUSFMC, n);
									*(t + ttop) = s;                  /* Slave address afterwards */
									e = i + 1;
								}
								else if ((ADRJBUSRAMRW <= a) && ((a + n) <= (ADRJBUSRAMRW + SIZJBUSRAMRW)))
								{
									/* Writing in RAM RW area */
									i = IQJbFnc16Ram(r ,*rtop + 7, size, t + (ttop + 1) % size, a - ADRJBUSRAMRW, n);
									*(t + ttop) = s;                  /* Slave address afterwards */
									e = i + 1;
								}
								else  if ((ADRJBUSEEP <= a) && ((a + n) < ((ulong)ADRJBUSEEP + SIZJBUSEEP)) && (n == 31))
								{
									/* Writing in EEP area, segment of 62 bytes */
									//ReloadConfigOn;
									i = IQJbFnc16EepSeg(r, *rtop + 7, size, t + (ttop + 1) % size, a - ADRJBUSEEP, 31);
									*(t + ttop) = s;                  /* Slave address afterwards */
									e = i + 1;
								}
								else
								{
									/* Address error */
									i = IQJbErr(t + (ttop + 1) % size, c, JBERRADD);
									*(t + ttop) = s;                  /* Slave address afterwards */
									e = i + 1;
								}
							}
							else
							{
								*rtop = (*rtop + 1) % size;
							}
						else
						{
							*rtop = (*rtop + 1) % size;
						}
					}
				}
				break;
				case 0x14 :                     /* Read file records */
				{
					n = (*(r + *rtop + 2) % size);            /* Byte count */
					if (Size(*rtop, cur, size - 1) < (n + 5)) /* Test for size */
						e = 0;                 			/* Incr�ment du d�but de trame au caract�re suivant et modulo */
					else
					{
						f = (*(r + *rtop + 4) % (size - 1)) * 256 + (*(r + *rtop + 5) % (size - 1));
						a = (*(r + *rtop + 6) % (size - 1)) * 256 + (*(r + *rtop + 7) % (size - 1));
						g = (*(r + *rtop + 8) % (size - 1)) * 256 + (*(r + *rtop + 9) % (size - 1));
						if (((*(r + *rtop + 3) % (size - 1)) == 0x06) &&			/* Ref Type 0x06 */
								(0 < g) &&                                    /* 0 < record length */
								(g <= ((size - 10) / 2)))	  /* V�rification avec taille buffer */
							if (CrcCalc(r, *rtop, (uint)(n + 4), size - 1) == 0)
							{
								*rtop = (*rtop + 1) % size;
							}
							else
							{
								*rtop = (*rtop + 1) % size;
							}
						else
						{
							*rtop = (*rtop + 1) % size;
						}
					}
				}
				break;
				case 0x15 :                     /* Write file records */
				{
					if (Size(*rtop, cur, size - 1) < 10)      /* Test for size */
						e = 0;                 			/* Incr�ment du d�but de trame au caract�re suivant et modulo */
					else
					{
						f = (*(r + *rtop + 4) % size) * 256 + (*(r + *rtop + 5) % size);
						a = (*(r + *rtop + 6) % size) * 256 + (*(r + *rtop + 7) % size);
						g = (*(r + *rtop + 8) % size) * 256 + (*(r + *rtop + 9) % size);
						if (((*(r + ((*rtop + 3) % size))) == 0x06) &&			/* Ref Type 0x06 */
								(0 < g) &&                                    /* 0 < record length */
								(g <= ((size - 10) / 2)))	  /* V�rification avec taille buffer */
							if (CrcCalc(r, *rtop, (uint)(2 * g + 12), size - 1) == 0)
							{
								*rtop = (*rtop + 1) % size;
							}
							else
							{
								*rtop = (*rtop + 1) % size;
							}
						else
						{
							*rtop = (*rtop + 1) % size;
						}
					}
				}
				break;
				case 0x16 :                     /* Mask Write Register */
					if (Size(*rtop, cur, size - 1) < 10)      /* V�rification de la taille (pas inf�rieure � 10) */
						e = 0;                 			/* Incr�ment du d�but de trame au caract�re suivant et modulo */
					else
					{
						if (CrcCalc(r, *rtop, 9, size - 1) == 0)
						{
							a = (*(r + *rtop + 2) % size) * (uint)256 + (*(r + *rtop + 3) % size);
							f = (*(r + *rtop + 4) % size) * (uint)256 + (*(r + *rtop + 5) % size);    /* And mask */
							g = (*(r + *rtop + 6) % size) * (uint)256 + (*(r + *rtop + 7) % size);    /* Or mask */
							if ((ADRJBUSFMC <= a) && (a < (ADRJBUSFMC + SIZJBUSFMC)))
							{
								/* Writing bits in FMC technology area */
								i = IQJbFnc22FMC(t + (ttop + 1) % size, a - ADRJBUSFMC, f, g);
								*(t + ttop) = s;                  /* Slave address afterwards */
								e = i + 1;
							}
							else if ((ADRJBUSRAMRW <= a) && (a < (ADRJBUSRAMRW + SIZJBUSRAMRW)))
							{
								/* Writing bits in RAM RW area */
								i = IQJbFnc22Ram(t + (ttop + 1) % size, a - ADRJBUSRAMRW, f, g);
								*(t + ttop) = s;                  /* Slave address afterwards */
								e = i + 1;
							}
							else
							{
								/* Address error */
								i = IQJbErr(t + (ttop + 1) % size, c, JBERRADD);
								*(t + ttop) = s;                  /* Slave address afterwards */
								e = i + 1;
							}
						}
						else
						{
							*rtop = (*rtop + 1) % size;
						}
					}
					break;
				default :
				{
					*rtop = (*rtop + 1) % size;
				}
				break;
				}
			}
			else
			{
				*rtop = (*rtop + 1) % size;
			}
		}
	}
	while (e == 0xFFFF);
	/* Add checksum */
	if (e != 0)
	{
		CrcAdd(t, ttop, e - 1, size - 1);
		e = e + 2;
	}
	return e;
}


/* ********** Function by number ********** */

uchar IQJbErr(uchar *t, uchar c, uchar JbErrCode)
{
	/* Revoi d'un message d'erreur pour adresse incorrecte */
	/* *t : first character to add in transmit buffer */
	/* c : function code (without error) */
	/* return : number of character written in *t buffer */
	*t = c + 0x80;
	*(t + 1) = JbErrCode;         /* 2 octets lus */
	return 2;
}

uchar IQJbFnc3Ram(uchar *t, uchar c, uint a, uchar n)
{
	/* Read in RAM area */
	/* *t : first character to add in transmit buffer */
	/* c : function code */
	/* a : address of first word to read of RW area */
	/* n : number of word to read */
	/* return : number of character written in *t buffer */
	uchar i;
	/* La requ�te est bonne, construction de la trame JBus de r�ponse */
	*t = c;                       /* Code fonction */
	*(t + 1) = 2 * n;             /* Longueur */
	/* Boucle sur les adresses m�moire */
	i = 0;
	while (i < n)
	{
		*(t + 2 * i + 2) = *(uchar *)(2 * a + ADRRAMRW + 2 * i + 1);
		*(t + 2 * i + 3) = *(uchar *)(2 * a + ADRRAMRW + 2 * i);
		i = i + 1;
	}
	return (2 * n) + 2;
}

uchar IQJbFnc3Rom(uchar *t, uchar c, uint a, uchar n)
{
	/* Read in ROM area */
	/* *t : first character to add in transmit buffer */
	/* c : function code */
	/* a : address of first word to read of RW area */
	/* n : number of word to read */
	/* return : number of character written in *t buffer */
	uchar i;
	/* La requ�te est bonne, construction de la trame JBus de r�ponse */
	*t = c;                       /* Code fonction */
	*(t + 1) = 2 * n;             /* Longueur */
	/* Boucle sur les adresses m�moire */
	i = 0;
	while (i < n)
	{
		*(t + 2 * i + 2) = *(uchar *)(2 * a + ADRROM + 2 * i + 1);
		*(t + 2 * i + 3) = *(uchar *)(2 * a + ADRROM + 2 * i);
		i = i + 1;
	}
	return (2 * n) + 2;
}

uchar IQJbFnc3EepSeg(uchar *t, uchar c, uint a, uchar n)
{
	/* Read a segment in EEprom */
	/* *t : first character to add in transmit buffer */
	/* c : function code */
	/* a : address of first word to read of RW area */
	/* n : number of word to read */
	/* return : number of character written in *t buffer */
	uchar i;
	uchar j;
	/* Read data */

	/* Request is good, build response frame */
	*t = c;                       /* Function code */
	*(t + 1) = 2 * n;             /* Length */
	Read_SegFlashData (a / 32, t + 2);
	//k = I2CSegRead(ADREEP, a / 32, t + 2);
	/* Invert PF pf */
	i = 0;
	do
	{
		j = *(t + 2 * i + 2);
		*(t + 2 * i + 2) = *(t + 2 * i + 1 + 2);
		*(t + 2 * i + 1 + 2) = j;
		i ++;
	}
	while (i < 32);
	i = (2 * n) + 2;

	return i;
}

uchar IQJbFnc3FMC(uchar *t, uchar c, uint a, uchar n)
{
	/* Read in FMC area */
	/* *t : first character to add in transmit buffer */
	/* c : function code */
	/* a : address of first word to read in FMC area */
	/* n : number of word to read */
	/* return : number of character written in *t buffer */
	uchar i;
	uint j;
	/* La requ�te est bonne, construction de la trame JBus de r�ponse */
	*t = c;                       /* Code fonction */
	*(t + 1) = 2 * n;             /* Longueur */
	/* Boucle sur les adresses m�moire */
	i = 0;
	while (i < n)
	{
		j = FMCRead(a + i);
		*(t + 2 * i + 2) = HighByte(j);
		*(t + 2 * i + 3) = LowByte(j);
		i = i + 1;
	}
	return (2 * n) + 2;
}

uchar IQJbFnc5FMC(uchar *t, uchar c, uint a, uint g)
{
	/* Write in FMC area */
	/* *t : first character to add in transmit buffer */
	/* a : address of first word to write of RW area */
	/* g : value of word */
	/* return : number of character written in *t buffer */
	uint i;
	uint j;
	/* Write value */
	j = FMCWrite(a, g);
	/* Test if request valid or not */
	if (j == 0)
	{
		*t = c;                       /* Code fonction */
		i = a + ADRJBUSFMC;
		*(t + 1) = HighByte(i);       /* Address high */
		*(t + 2) = LowByte(i);        /* Address low */
		*(t + 3) = HighByte(g);       /* Address high */
		*(t + 4) = LowByte(g);        /* Address low */
		i = 5;
	}
	else
	{
		/* Address error */
		i = IQJbErr(t , c, JBERRADD);
	}
	return i;
}

uchar IQJbFnc5Ram(uchar *t, uchar c, uint a, uint g)
{
	/* Write 1 word in ram area */
	/* *t : first character to add in transmit buffer */
	/* a : address of first word to write of RW area */
	/* g : value of word */
	/* return : number of character written in *t buffer */
	uint i;
	uint j;
	/* Write in FMC area */
	*(uint *)(2 * a + ADRRAMRW ) = g;
	/* Test if request valid or not */
	if (j == 0)
	{
		*t = c;                       /* Code fonction */
		i = a + ADRJBUSRAMRW;
		*(t + 1) = HighByte(i);       /* Address high */
		*(t + 2) = LowByte(i);        /* Address low */
		*(t + 3) = HighByte(g);       /* Address high */
		*(t + 4) = LowByte(g);        /* Address low */
		i = 5;
	}
	else
	{
		/* Address error */
		i = IQJbErr(t , c, JBERRADD);
	}
	return i;
}

uchar IQJbFnc16Ram(uchar *r, uint rtop, uint size, uchar *t, uint a, uchar n)
{
	/* Write in RAM area */
	/* *r : first character of data */
	/* size : size of received buffer */
	/* *t : first character to add in transmit buffer */
	/* a : address of first word to write of RW area */
	/* n : number of word to write */
	/* return : number of character written in *t buffer */
	uchar i;
	uint j;
	/* Boucle sur les adresses m�moire */
	i = 0;
	while (i < n)
	{
		*(uint *)(2 * a + ADRRAMRW + 2 * i) = 256 * *(r + ((rtop + 2 * i) % size)) + *(r + ((rtop + 2 * i + 1) % size));
		i = i + 1;
	}
	/* La requ�te est bonne, construction de la trame JBus de r�ponse */
	*t = 0x10;                    /* Code fonction */
	j = a + ADRJBUSRAMRW;
	*(t + 1) = HighByte(j);       /* Address high */
	*(t + 2) = LowByte(j);        /* Address low */
	*(t + 3) = 0;                 /* 0 */
	*(t + 4) = n;                 /* Length */
	return 5;
}

uchar IQJbFnc16EepSeg(uchar *r, uint rtop, uint size, uchar *t, uint a, uchar n)
{
	/* Write a segment in EEPROM */
	/* *r : top of received buffer */
	/* rtop : first character valid in received buffer */
	/* size : size of receive buffer (power of 2 for mask) */
	/* *t : first character to add in transmit buffer */
	/* a : address of first word to write in EEPROM */
	/* n : number of word to write */
	/* return : number of character written in *t buffer */
	uchar i;
	uint j;

	/* Invert PF pf */
	i = 0;
	do
	{
		j = *(r + ((rtop + 2 * i) % size));
		*(r + ((rtop + 2 * i) % size)) = *(r + ((rtop + 2 * i + 1) % size));
		*(r + ((rtop + 2 * i + 1) % size)) = j;
		i ++;
	}
	while (i < 32);
	Write_SegFlashData(a / 32, r + rtop);

	i = 0;
	if (i == 0)
	{
		/* Request is OK, response frame */
		*t = 0x10;                    /* Function code */
		j = a + ADRJBUSEEP;
		*(t + 1) = HighByte(j);       /* Address high */
		*(t + 2) = LowByte(j);        /* Address low */
		*(t + 3) = 0;                 /* 0 */
		*(t + 4) = n;                 /* Length */
		i = 5;
	}
	else
	{
		/* Slave busy error */
		i = IQJbErr(t , 0x10, JBSLAVEBUSY);
	}

	return i;
} 

uchar IQJbFnc16FMC(uchar *r, uint rtop, uint size, uchar *t, uint a, uchar n)
{
	/* Write in FMC area */
	/* *r : first character of data */
	/* size : size of received buffer */
	/* *t : first character to add in transmit buffer */
	/* a : address of first word to write of RW area */
	/* n : number of word to write */
	/* return : number of character written in *t buffer */
	uchar i;
	uint j;
	/* Boucle sur les adresses m�moire */
	j = 0;
	i = 0;
	while (i < n)
	{
		j = j + FMCWrite(a + i, 256 * *(r + ((rtop + 2 * i) % size)) + *(r + ((rtop + 2 * i + 1) % size)));
		i = i + 1;
	}
	/* Test if request valid or not */
	if (j == 0)
	{
		*t = 0x10;                    /* Code fonction */
		j = a + ADRJBUSFMC;
		*(t + 1) = HighByte(j);       /* Address high */
		*(t + 2) = LowByte(j);        /* Address low */
		*(t + 3) = 0;                 /* 0 */
		*(t + 4) = n;                 /* Length */
		i = 5;
	}
	else
	{
		/* Address error */
		i = IQJbErr(t , 0x10, JBERRADD);
	}
	return i;
}

uchar IQJbFnc22Ram(uchar *t, uint a, uint and, uint or) 
{
	/* Mask write bits in RAM */
	/* t = buffer */
	/* a = adress */
	/* or = 0 : and set the 1 bits  */
	/* and = 0 : result = or mask */
	uchar i;
	/* Operation */
	
	*(uint *)(2 * a + ADRRAMRW) = ((*(uint *)(2 * a + ADRRAMRW)) & and) | (or & ~and);
	/* Response */
	*t = 0x16;
	i = a + ADRJBUSRAMRW;
	*(t + 1) = HighByte(i);
	*(t + 2) = LowByte(i);
	*(t + 3) = HighByte(and);
	*(t + 4) = LowByte(and);
	*(t + 5) = HighByte(or);
	*(t + 6) = LowByte(or);
	
	// MODIF 3.3 : powercut
	if (or == 0x0040)
		BfmTout = 0;
	
	return 7;
} 

uchar IQJbFnc22FMC(uchar *t, uint a, uint and, uint or) 
{
	/* Mask write bits in FMC area */
	/* t = buffer */
	/* a = adress */
	/* or = 0 : and set the 1 bits  */
	/* and = 0 : result = or mask */
	uchar i;
	uint j;
	/* Operation */
	j = FMCRead(a);
	j = (j & and) | (or & ~and);
	i = FMCWrite(a, j);
	if (i == 0)
	{
		/* Response */
		*t = 0x16;
		*(t + 1) = HighByte(a + ADRJBUSFMC);
		*(t + 2) = LowByte(a + ADRJBUSFMC);
		*(t + 3) = HighByte(and);
		*(t + 4) = LowByte(and);
		*(t + 5) = HighByte(or);
		*(t + 6) = LowByte(or);
		i = 7;
	}
	else
	{
		/* Address error */
		i = IQJbErr(t , 0x16, JBERRADD);
	}
	return i;
}

/* ********** FMC read and write functions ********** */
uint FMCRead(uint a)
{
	/* Read a parameter in FMC area */
	/* a : parameter to read */
	/* return : value */
	uint i;
	sint j;
	i = 0;
	if (a == 0)                   /* R W */
	{
		i |= (StartStop != 0) ? 0x01 : 0x00;    /* Enable charge */
		i |= (RequestEqual != 0) ? 0x02 : 0x00; /* Enable equalization */
		i |= (LoadWIIQ != 0) ? 0x04 : 0x00; 
	}
    else if (a == 1)                   /* R */
    {
        if (ChgData.Profile == TPPL)
            i = 0;
        else
            i = 1;    
    }
    else if (a == 2)                   /* R */
    {
        i = (sint)Menu.BatTemp;
    }
	else if (a == 5)                   /* R */
	{
		i |= ((State.Charger == StateChgerChg) & (State.Charge != StateChgAvail)) ? 0x01 : 0x00; /* Charger in use */
		i |= ((State.Charger == StateChgerChg) & (State.Charge == StateChgEqual)) ? 0x02 : 0x00; /* Equalizing */
		if ((State.Charger == StateChgerChg) & (State.Charge != StateChgAvail) & (ChgData.Profile == EQUAL))
			i |= 0x02; /* Modif V3.4 : Manual Equalizing */
		i |= (Memo.Default & MaskWarnDF) ? 0x04 : 0x00; /* Charger warning */
		i |= (Memo.Default & MaskCritDF) ? 0x08 : 0x00; /* Charger fault */
		i |= (State.Def & BitDefBattery) ? 0x10 : 0x00; /* Battery connected */
		if (IQWIIQLink != 0)
			i |= 0x20;
	}
	else if ((10 <= a) && (a <= 19))   /* R */
	{                           /* Host message 1 */
		in16(i, Host.HostMsgText0[2 * (a - 10)], Host.HostMsgText0[(2 * (a - 10)) + 1]);
	}
	else if ((20 <= a) && (a <= 29))   /* R */
	{                           /* Host message 2 */
		in16(i, Host.HostMsgText1[2 * (a - 20)], Host.HostMsgText1[(2 * (a - 20)) + 1]);
	}
	else if (a == 110)                 /* R */
	{                           /* Fault code */
		if( ((Memo.Default & BitDFconverter) != 0) || ((Memo.Default & BitDFnetwork) != 0) || ((Memo.Default & BitDFotherCur) != 0) )
			i = 1;
		else if ((Memo.Default & BitDFfuse) != 0)
			i = 2;
		else if ((Memo.Default & BitDFbadBat) != 0)
			i = 3;
		else if ((Memo.Default & BitDFtemp) != 0)
			i = 6;
		else if ((Memo.Default & BitDFpump) != 0)
			i = 7;
		else if( ((Memo.Default & BitDFdIdT) != 0) /*|| ((Memo.Default & BitDFdVdT) != 0)*/ ||
				((Memo.Default & BitDFtimeSecu) != 0) || ((Memo.Default & BitDFotherBat) != 0) )
			i = 5;
		else if ((Memo.Default & BitDFoverdis) != 0)
			i = 4;
		else
			i = 0;
	}
	else if (a == 111)                 /* R */
	{                           /* State of charge */
		i = DataLcd.ChgSOC;
	}
	else if (a == 112)                 /* R */
	{                           /* Equal time remaining */
		if (ChgData.TimeMaxPhase1 > ChgData.TimerSecCharge)
			i = (ChgData.TimeMaxPhase1 - ChgData.TimerSecCharge) / 60;
		else
			i = 0;
	}
	else if (a == 113)                 /* R */
	{                           /* Battery temperature (signed)*/
		if (IQWIIQLink != 0)
			j = (sint)IQData.BatTemp;
		else
			j = (sint)Menu.BatTemp;
		return j;
	}
	else if (a == 114)                 /* R */
	{                           /* output current */
		i = DataLcd.Ibat;
	}
	else if (a == 115)                 /* R */
	{                           /* output voltage */
		i = DataLcd.Vbat;
	}
	else if (a == 116)                 /* R */
	{                           /* Battery V/cell */
		i = DataLcd.VbatCell;
	}
	else if (a == 117)                 /* R */
	{                           /* charger output voltage */
		i = DataLcd.Vcharger;
	}
	else if (a == 118)                 /* R */
	{                           /* charger output power */
		i = DataLcd.Power;
	}
	else if (a == 119)                 /* R */
	{                           /* chargetime */
		i = DataLcd.ProfileTime;
	}
	else if (a == 120)                 /* R */
	{                           /* charge Ah */
		i = DataLcd.ProfileAh;
	}
	else if (a == 121)                 /* R */
	{                           /* Delay before start charge */
		i = DataLcd.DelayStChg;
	}
	else if (a == 122)                 /* R */
	{                           /* remaining charge time */
		i = DataLcd.ChgRestTime;
	}
	else if (a == 123)                 /* R */
	{                           /* charger status */
		i = DataLcd.StateCharger;
	}
	else if (a == 124)                 /* R */
	{                           /* charge status */
		i = DataLcd.StateCharge;
	}
	else if (a == 125)                 /* R */
	{                           /* charge profile status */
		i = DataLcd.StatePhase;
	}
	else if ((126 <= a) && (a <= 139))   /* R */
	{                           /* Battery serial */
		in16(i, IQData.BatSN[2 * (a - 126)], IQData.BatSN[(2 * (a - 126)) + 1]);
	}
	else if (a == 140)                 /* R */
	{                           /* Battery capacity */
		if (IQWIIQLink != 0)
			i = IQData.BatCap;
		else
			i = Menu.BatCap;
	}
	else if (a == 141)                 /* R */
	{                           /* Battery Nb Cell */
		if (IQWIIQLink != 0)
			i = IQData.NbCells;
		else
			i = Menu.NbCells;
	}
	else if (a == 142)                 /* R */
	{                           /* Battery Technology */
		i = IQData.BatTechno;
	}
	else if (a == 143)                 /* R */
	{                           /* Battery warning */
		i = IQData.BatWarning;
	}
	else if (a == 144)                 /* R */
	{                           /* WiIQ number */
		i = DataW.IQWIIQAddr;
	}
	else if (a == 145)                 /* R */
	{                           /* WiIQ seg */
		i = DataW.IQSegAddr;
	}
	else if ((150 <= a) && (a <= 159))   /* R */
	{                           /* charger serial number */
		in16(i, SerialNumber.SerNum[2 * (a - 150)], SerialNumber.SerNum[(2 * (a - 150)) + 1]);
	}
	else if ((160 <= a) && (a <= 169))   /* R */
	{                          /* charger type */
		in16(i, ChargerConfig.ChargerName[2 * (a - 160)], ChargerConfig.ChargerName[(2 * (a - 160)) + 1]);
	}
	return i;
}

uchar FMCWrite(uint a, uint d)
{
	/* Write a data in FMC area */
	/* return : 1 error, 0 OK */
	uchar r;
	if (a == 0)                   /* R W */
	{
		if ((d & 0x0001) != 0)  /* Enable charge */
			StartStopOn;
		else
			StartStopOff;
		if ((d & 0x0002) != 0)  /* Enable equalisation */
		{

#ifndef ENERSYS_US
			if (StartStop == 0)
			{
				StartStopOn;
				ManuEqualOff;
			}
#endif

#ifdef ENERSYS_US
			if ((RequestEqual == 0) && (ManuEqual == 0))
			{
				RequestEqualOn;
				if ((State.Charger == StateChgerChg) && (State.Charge == StateChgAvail))
					ChgData.TimeEqual = ChgData.TimerSecCharge;
			}
#endif
		}
		else
#ifdef ENERSYS_US
		{
			if ((RequestEqual != 0) && (State.Charger == StateChgerChg) && (State.Charge == StateChgAvail))
			{
				ChgData.TimeEqual = 0xFFFFFFFF;
			}
			RequestEqualOff;
		}
#else
		{
			ManuEqualOff;
		}
#endif

		if ((d & 0x0004) != 0)  /* Load WiIQ seg */
			LoadWIIQOn;
		else
			LoadWIIQOff;

		r = 0;
	}
    else if (a == 1)            // profile : 16 = XFC profile
    {
        r = 0;
        if (d == 16)
        {
            Menu.Profile = TPPL;
            MenuWriteOn;
            E2FLASH_Chrono();   // Force segment write
            ReloadConfigOn;     // Relaod charger setting 
        }
        else
            r = 1;
    }
    else if (a == 2)            // Temperature profile
    {
        r = 0;
        if ((d >= 0) && (d < 46))
        {
            Menu.BatTemp = d;
            Menu.BatTempF = (d * 9) / 5 + 32;
            MenuWriteOn;
            E2FLASH_Chrono();   // Force segment write
            ReloadConfigOn;     // Reload charger setting 
        }
        else
            r = 1;
    }
	else if ((10 <= a) && (a <= 19))
	{
		Host.HostMsgText0[2 * (a - 10)] = LowByte(d);
		Host.HostMsgText0[(2 * (a - 10)) + 1] = HighByte(d);
		if ((Host.HostMsgText0[0] == '\0') || (Host.HostMsgText0[0] == SPACE))
			HostMsg0Off;
		else
			HostMsg0On;
		r = 0;
	}
	else if ((20 <= a) && (a <= 29))
	{
		Host.HostMsgText1[2 * (a - 20)] = LowByte(d);
		Host.HostMsgText1[(2 * (a - 20)) + 1] = HighByte(d);
		if ((Host.HostMsgText1[0] == '\0') || (Host.HostMsgText1[0] == SPACE))
			HostMsg1Off;
		else
			HostMsg1On;
		r = 0;
	}
    else if ((150 <= a) && (a <= 159)) 
    {
        SerialNumber.SerNum[2 * (a - 150)] = LowByte(d);
        SerialNumber.SerNum[2 * (a - 150) + 1] = HighByte(d);
		Write_SegFlashData (SEGSERIAL, (uchar *)&SerialNumber);
        r = 0;
    }
	else if (a == 144)
	{
		DataW.IQWIIQAddr = d;
		r = 0;
	}
	else if (a == 145)
	{
		DataW.IQSegAddr = d;
		r = 0;
	}
	else
	{
		/* Address is not writeable */
		r = 1;
	}
	return r;
}  
