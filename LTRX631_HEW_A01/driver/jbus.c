/*
 * jbus.c
 *
 *  Created on: 1 mars 2013
 *      Author: duhautf
 */

#include "include.h"

#define   JBERRADD            0x02
#define   JBERRDATA           0x03
#define   JBSLAVEBUSY         0x06


/**
 * @brief  Receive one character
 * @param  *r : top of received buffer
 * 		*rtop : first character valid in received buffer
 * 		cur : current character received in buffer
 * 		size : size of receive buffer (power of 2 for mask)
 * 		*t : top of transmit buffer (can be the same as received buffer)
 * 		ttop : first character to fill in transmit buffer
 *
 * @retval number of written characters
 */
uint JbusRx(uchar *r, uint *rtop, uint cur, uint size, uchar *t, uint ttop)
{
	uint e;    					/* End of loop indicator */
	uint a;						/* Adresse de début, record number */
	uchar i;
	uchar s;                    /* Slave number */
	uchar c;                    /* Function code */
	uchar n;					/* Number of words */
	uint f;                     /* File number */
	uint g;                  	/* Record length */

	/* Analyse de la trame J-Bus, débute à m, n pointeur de fin */
	e = 0xFFFF;                            	/* Chaîne considérée bonne au départ */
	do						                       		/* Loop until charcaters could be analysed */
	{
		if (Size(*rtop, cur, size - 1) < 8)   /* Control of size (not greater than 8) */
		{
			e = 0;                       				/* String too short */
		}
		else
		{
			s = *(r + *rtop);                   /* Slave address */
			if ((s == 0) || (s == 1))  /* Slave address or 0 */
			{
				c = *(r + ((*rtop + 1) % size));  /* Function code */
				switch (c)                        /* Selection on function code */
				{
				case 0x03 :                     /* Lecture de N words */
				case 0x04 :
				{
					/* a : jbus address, n : number of words to read */
					n = *(r + ((*rtop + 5) % size));          /* Number of words to read <= 125 */
					if (n <= ((size - 6) / 2) && (0 < n) && (n <= 0x7D))
					{
						/* For faster response Crc is calculated afterwards */
						if (CrcCalc(r, *rtop, 7, size - 1) == 0)
						{
							a = (*(r + ((*rtop + 2) % size))) * (uint)256 + *(r + ((*rtop + 3) % size));
							if ((ADRJBUSRAMRW <= a) && ((a + n) <= (ADRJBUSRAMRW + SIZJBUSRAMRW)))
							{
								/* Reading in RAM RW area */
								i = JbFnc3Ram(t + (ttop + 1) % size, c, a - ADRJBUSRAMRW, n);
								*(t + ttop) = s;                  /* Slave address afterwards */
								e = i + 1;
							}
							else if ((ADRJBUSEEP <= a) && ((a + n) < ((ulong)ADRJBUSEEP + SIZJBUSEEP)) /*&& (n == 31)*/)
							{         /* Reading in EEP area, segment of 62 bytes */
								i = JbFnc3E2Seg(t + (ttop + 1) % size, c, a - ADRJBUSEEP, n);
								*(t + ttop) = s;                  /* Slave address afterwards */
								e = i + 1;
							}
							else  if ((ADRJBUSROM <= a) && ((a + n) <= (ADRJBUSROM + SIZJBUSROM)))
							{
								/* Reading in ROM area */
								i = JbFnc3Rom(t + (ttop + 1) % size, c, a - ADRJBUSROM, n);
								*(t + ttop) = s;                  /* Slave address afterwards */
								e = i + 1;
							}
							else
							{
								/* Address error */
								i = JbErr(t + (ttop + 1) % size, c, JBERRADD);
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
						e = 0;                      /* Incrément du début de trame au caractère suivant et modulo */
					else
					{
						a = (*(r + ((*rtop + 2) % size))) *256 + (*(r + ((*rtop + 3) % size)));
						g = (*(r + ((*rtop + 4) % size))) *256 + (*(r + ((*rtop + 5) % size)));
						if (CrcCalc(r, *rtop, 7, size - 1) == 0)
						{
							if ((ADRJBUSRAMRW <= a) && ((a + n) <= (ADRJBUSRAMRW + SIZJBUSRAMRW)))
							{
								/* Writing in RAM RW area */
								i = JbFnc5Ram(t + (ttop + 1) % size, c, a - ADRJBUSRAMRW, g);
								*(t + ttop) = s;                  /* Slave address afterwards */
								e = i + 1;
							}
							else
							{
								/* Address error */
								i = JbErr(t + (ttop + 1) % size, c, JBERRADD);
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
					n = (*(r + ((*rtop + 5) % size)));                    /* Nombre de mots à écrire */
					if (Size(*rtop, cur, size - 1) < (2 * n + 9))         /* Test taille */
						e = 0;                 			/* Incrément du début de trame au caractère suivant et modulo */
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
								if ((ADRJBUSRAMRW <= a) && ((a + n) <= (ADRJBUSRAMRW + SIZJBUSRAMRW)))
								{
									/* Writing in RAM RW area */
									i = JbFnc16Ram(r ,*rtop + 7, size, t + (ttop + 1) % size, a - ADRJBUSRAMRW, n);
									*(t + ttop) = s;                  /* Slave address afterwards */
									e = i + 1;
								}
								else  if ((ADRJBUSEEP <= a) && ((a + n) < ((ulong)ADRJBUSEEP + SIZJBUSEEP)) && (n == 31))
								{
									/* Writing in EEP area, segment of 62 bytes */
									i = JbFnc16E2Seg(r, *rtop + 7, size, t + (ttop + 1) % size, a - ADRJBUSEEP, 31);
									*(t + ttop) = s;                  /* Slave address afterwards */
									e = i + 1;
								}
								else
								{
									/* Address error */
									i = JbErr(t + (ttop + 1) % size, c, JBERRADD);
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
						e = 0;                 			/* Incrément du début de trame au caractère suivant et modulo */
					else
					{
						f = (*(r + *rtop + 4) % (size - 1)) * 256 + (*(r + *rtop + 5) % (size - 1));
						a = (*(r + *rtop + 6) % (size - 1)) * 256 + (*(r + *rtop + 7) % (size - 1));
						g = (*(r + *rtop + 8) % (size - 1)) * 256 + (*(r + *rtop + 9) % (size - 1));
						if (((*(r + *rtop + 3) % (size - 1)) == 0x06) &&			/* Ref Type 0x06 */
								(0 < g) &&                    /* 0 < record length */
								(g <= ((size - 10) / 2)))	  /* Vérification avec taille buffer */
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
						e = 0;                 			/* Incrément du début de trame au caractère suivant et modulo */
					else
					{
						f = (*(r + *rtop + 4) % size) * 256 + (*(r + *rtop + 5) % size);
						a = (*(r + *rtop + 6) % size) * 256 + (*(r + *rtop + 7) % size);
						g = (*(r + *rtop + 8) % size) * 256 + (*(r + *rtop + 9) % size);
						if (((*(r + ((*rtop + 3) % size))) == 0x06) &&			/* Ref Type 0x06 */
								(0 < g) &&                                    /* 0 < record length */
								(g <= ((size - 10) / 2)))	  /* Vérification avec taille buffer */
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
					if (Size(*rtop, cur, size - 1) < 10)      /* Vérification de la taille (pas inférieure à 10) */
						e = 0;                 			/* Incrément du début de trame au caractère suivant et modulo */
					else
					{
						if (CrcCalc(r, *rtop, 9, size - 1) == 0)
						{
							a = (*(r + *rtop + 2) % size) * (uint)256 + (*(r + *rtop + 3) % size);
							f = (*(r + *rtop + 4) % size) * (uint)256 + (*(r + *rtop + 5) % size);    /* And mask */
							g = (*(r + *rtop + 6) % size) * (uint)256 + (*(r + *rtop + 7) % size);    /* Or mask */
							if ((ADRJBUSRAMRW <= a) && (a < (ADRJBUSRAMRW + SIZJBUSRAMRW)))
							{
								/* Writing bits in RAM RW area */
								i = JbFnc22Ram(t + (ttop + 1) % size, a - ADRJBUSRAMRW, f, g);
								*(t + ttop) = s;                  /* Slave address afterwards */
								e = i + 1;
							}
							else
							{
								/* Address error */
								i = JbErr(t + (ttop + 1) % size, c, JBERRADD);
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


/**
 * @brief  Renvoi d'un message d'erreur pour adresse incorrecte
 * @param  *t : first character to add in transmit buffer
 * 		c : function code (without error)
 * @retval None
 */
uchar JbErr(uchar *t, uchar c, uchar JbErrCode)
{
	/* Renvoi d'un message d'erreur pour adresse incorrecte */
	/* *t : first character to add in transmit buffer */
	/* c : function code (without error) */
	/* return : number of character written in *t buffer */
	*t = c + 0x80;
	*(t + 1) = JbErrCode;         /* 2 octets lus */
	return 2;
}

/**
 * @brief  Read in RAM area
 * @param  *t : first character to add in transmit buffer
 * 		c : function code (without error)
 * 		a : address of first word to read of RW area
 * 		n : number of word to read
 *
 * @retval number of character written in *t buffer
 */
uchar JbFnc3Ram(uchar *t, uchar c, uint a, uchar n)
{
	uchar i;
	/* La requête est bonne, construction de la trame JBus de réponse */
	*t = c;                       /* Code fonction */
	*(t + 1) = 2 * n;             /* Longueur */
	/* Boucle sur les adresses mémoire */
	i = 0;
	while (i < n)
	{
		*(t + 2 * i + 2) = *(uchar *)(2 * a + ADRRAMRW + 2 * i + 1);
		*(t + 2 * i + 3) = *(uchar *)(2 * a + ADRRAMRW + 2 * i);
		i = i + 1;
	}
	return (2 * n) + 2;
}

uchar JbFnc3Rom(uchar *t, uchar c, uint a, uchar n)
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


/**
 * @brief  Read a segment in E2DataFlash
 * @param  *t : first character to add in transmit buffer
 * 		c : function code (without error)
 * 		a : address of first word to read of RW area
 * 		n : number of word to read
 *
 * @retval number of character written in *t buffer
 */
uchar JbFnc3E2Seg(uchar *t, uchar c, uint a, uchar n)
{
	uchar i;
	uchar j;
	/* Read data */

	/* Request is good, build response frame */
	*t = c;                       /* Function code */
	*(t + 1) = 2 * n;             /* Length */
	Read_SegFlashData(a / 32, t + 2);
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

/**
 * @brief  Write 1 word in ram area
 * @param  *t : first character to add in transmit buffer
 * 		c : function code (without error)
 * 		a : address of first word to write of RW area
 * 		g : value of word
 *
 * @retval number of character written in *t buffer
 */
uchar JbFnc5Ram(uchar *t, uchar c, uint a, uint g)
{
	uint i;
	uint j;
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
		i = JbErr(t , c, JBERRADD);
	}
	return i;
}

/**
 * @brief  Write 1 word in ram area
 * @param  *r : first character of data
 * 		size : size of received buffer
 * 		*t : first character to add in transmit buffer
 * 		a : address of first word to write of RW area
 * 		n : number of word to write
 *
 * @retval number of character written in *t buffer
 */
uchar JbFnc16Ram(uchar *r, uint rtop, uint size, uchar *t, uint a, uchar n)
{
	uchar i;
	uint j;
	/* Boucle sur les adresses mémoire */
	i = 0;
	while (i < n)
	{
		*(uint *)(2 * a + ADRRAMRW + 2 * i) = 256 * *(r + ((rtop + 2 * i) % size)) + *(r + ((rtop + 2 * i + 1) % size));
		i = i + 1;
	}
	/* La requête est bonne, construction de la trame JBus de réponse */
	*t = 0x10;                    /* Code fonction */
	j = a + ADRJBUSRAMRW;
	*(t + 1) = HighByte(j);       /* Address high */
	*(t + 2) = LowByte(j);        /* Address low */
	*(t + 3) = 0;                 /* 0 */
	*(t + 4) = n;                 /* Length */
	return 5;
}

/**
 * @brief  Write a segment in EEPROM
 * @param  *r : top of received buffer
 * 		size : size of receive buffer (power of 2 for mask)
 * 		*t : first character to add in transmit buffer
 * 		a : address of first word to write in EEPROM
 * 		n : number of word to write
 *
 * @retval number of character written in *t buffer
 */
uchar JbFnc16E2Seg(uchar *r, uint rtop, uint size, uchar *t, uint a, uchar n)
{
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
	i = Write_SegFlashData(a / 32, r + rtop);
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
		i = JbErr(t , 0x10, JBSLAVEBUSY);
	}

	return i;
}

/**
 * @brief  Mask write bits in RAM
 * @param  t = buffer
 * 		a = adress
 * 		or = 0 : and set the 1 bits
 * 		a : address of word to modify
 * 		and = 0 : result = or mask
 *
 * @retval number of character written in *t buffer
 */
uchar JbFnc22Ram(uchar *t, uint a, uint and, uint or)
{
	uchar i;
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


/**
 * @brief  Calculates number of characters
 * @param  l : index of first char
 * 		m : index of first free space
 * 		n : size of buffer - 1
 *
 * @retval number of character written in *t buffer
 */
uint  Size(uint l, uint m, uint n)
{
	if (l <= m)
		return (uint)(m - l);
	else
		return (uint)(((uint)(m + n + 1) - l));
}
