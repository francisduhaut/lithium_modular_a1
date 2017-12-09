/*
 * csv.c
 *
 *  Created on: 01/04/2013
 *      Author: Francis DUHAUT
 */

#include "include.h"
#include <math.h>

/* Ascii char */
#define SPACE       32
#define STAR        42
#define ZERO        48
#define TILDE       94
#define UNDERS      95        /* Underscore */
#define NUL         0
#define CR          0x0D      /* cariage return */
#define LF          0x0A      /* Line feed */
#define BS          0x5C      /* Back slash \ */


extern const uchar ListWord3[14][NBLANG][4];
extern const uchar ListWord6[45][NBLANG][7];
extern char version[8];
extern uint16_t UsbMemoPtr;
extern long abs (long i);

extern uint LoopWidthNew;
extern sint DeltaLoopWidth;
extern sint DeltaLoopWidthMax;
extern schar Tambiant;
extern sint TempCap;


/* Format definition */
#ifdef ENERSYS_EU
const uchar CsvFld = ';';               /* Field separator :    ; 0x3B */
const uchar CsvDec = ',';               /* Decimal separator :  , 0x2C */
#else
const uchar CsvFld = ',';
const uchar CsvDec = '.';
#endif
const uchar CsvDat = '/';               /* Date separator :     / 0x2F */
const uchar CsvTim = ':';               /* Time separator :     : 0x3A */

const uchar ListWord_6[11][NBLANG][7] = {
		{"      ", "      ", "      ", "      ", "      ",},	// 0 : Default
		{"DF1   ", "DF1   ", "DF1   ", "DF1   ", "DF1   ",},
		{"DF2   ", "DF2   ", "DF2   ", "DF2   ", "DF2   ",},
		{"DF3   ", "DF3   ", "DF3   ", "DF3   ", "DF3   ",},
		{"DF4   ", "DF4   ", "DF4   ", "DF4   ", "DF4   ",},
		{"DF5   ", "DF5   ", "DF5   ", "DF5   ", "DF5   ",},
		{"TH    ", "TH    ", "TH    ", "TH    ", "TH    ",},
#ifdef ENERSYS_EU        
		{"DF PMP", "DF PMP", "DF PMP", "DF PMP", "DF PMP",},
#else
        {"DF7   ", "DF7   ", "DF7   ", "DF7   ", "DF7   ",},
#endif      
		{"CHARGE", "CHARGE", "LADUNG", "CHARGE", "CHARGE",},	// 8
		{"EGAL  ", "EQUAL ", "AUSGL ", "EQUAL ", "EQUAL ",},
		{"DFC   ", "DFC   ", "DFC   ", "DFC   ", "DFC   ",},  // 10
};

const uchar ListWord7[38][NBLANG][8] = {
		{"Charge ", "Charge ", "Ladung ", "Charge ", "Carga  ",}, 	// 0 : Status
		{"Ch part", "Part ch", "Zichen ", "Part ch", "Part ch",},
		{"Ch cplt", "Full ch", "Voll ld", "Full ch", "Full ch",},
		{"Egal   ", "Equal  ", "Ausgl  ", "Equal  ", "Equal  ",},
		{"DF1    ", "DF1    ", "DF1    ", "DF1    ", "DF1    ",},
		{"DF2    ", "DF2    ", "DF2    ", "DF2    ", "DF2    ",},
		{"DF3    ", "DF3    ", "DF3    ", "DF3    ", "DF3    ",},
		{"DF4    ", "DF4    ", "DF4    ", "DF4    ", "DF4    ",},
		{"DF5    ", "DF5    ", "DF5    ", "DF5    ", "DF5    ",},
		{"TH     ", "TH     ", "TH     ", "TH     ", "TH     ",},
#ifdef ENERSYS_EU
		{"Df pomp", "Df pump", "Df pump", "Df pump", "Df pump",},
#else
        {"DF7    ", "DF7    ", "DF7    ", "DF7    ", "DF7    ",},
#endif     
		{"Cap bat", "Bat cap", "Bat kap", "Bat cap", "Bat cap",}, 	// 11 : Config 2
		{"Temp   ", "Temp   ", "Temp   ", "Temp   ", "Temp   ",},
		{"Profil ", "Profile", "Kenlini", "Profile", "Profile",},
		{"Tempo  ", "Del chg", "Del Ld ", "Del chg", "Del chg",},
		{"Ch cond", "Cond ch", "Cond Ld", "Cond ch", "Cond ch",},
		{"I flot ", "I float", "I Erhlt", "I float", "I float",},
		{"V flot ", "V float", "V Erhlt", "V float", "V float",},
		{"I egal ", "I equal", "I Ausgl", "I equal", "I equal",},
		{"Tps egl", "Time eq", "Time eq", "Time eq", "Time eq",},
		{"Del egl", "Del eql", "Del eql", "Del eql", "Del eql",},
		{"Periode", "Period ", "Periode", "Period ", "Period ",},
		{"Lngueur", "Length ", "Lange  ", "Length ", "Length ",},
		{"Section", "Section", "Quersch", "Section", "Section",},
		{"T elect", "T elect", "T elect", "T elect", "T elect",},
		{"Version", "Version", "Version", "Version", "Version",}, 	// 25 : Config 1
		{"Date   ", "Date   ", "Datum  ", "Date   ", "Date   ",},
		{"S/N    ", "S/N    ", "S/N    ", "S/N    ", "S/N    ",},
		{"S/N HTR", "S/N RTC", "S/NZeit", "S/N RTC", "S/N RTC",},
		{"Defaut ", "Default", "Fehler ", "Default", "Default",}, 	// 29
		{"Type   ", "Type   ", "Type   ", "Type   ", "Type   ",}, 	// 30
		{"RFTr Ve", "RFTr Ve", "RFTr Ve", "RFTr Ve", "RFTr Ve",}, 	// 31
		{"RFTr Ad", "RFTr Ad", "RFTr Ad", "RFTr Ad", "RFTr Ad",},   // 32
		{"Region ", "Region ", "Region ", "Region ", "Region ",},   // 33
		{"EU     ", "EU     ", "EU     ", "EU     ", "EU     ",},   // 34
		{"US     ", "US     ", "US     ", "US     ", "US     ",},   // 35
		{"DFC    ", "DFC    ", "DFC    ", "DFC    ", "DFC    ",},   // 36
		{"TH Mod ", "TH Mod ", "TH Mod ", "TH Mod ", "TH Mod ",},   // 37
};

const uchar ListWord_8[78][NBLANG][9] = {
		{" INIT   ", " INIT   ", " INIT   ", " INIT   ", " INIT   ",},	// 0 : LcdCharge
		{"ATTENTE ", " WAIT   ", "WARTEN  ", " WAIT   ", " WAIT   ",},
		{"ATTENTE ", " WAIT   ", "WARTEN  ", " WAIT   ", " WAIT   ",},
		{"IQ SCAN ", "IQ SCAN ", "IQ SCAN ", "IQ SCAN ", "IQ SCAN ",},
		{"IQ LINK ", "IQ LINK ", "IQ VERB ", "IQ LINK ", "IQ LINK ",},
		{" CHARGE ", " CHARGE ", " LADUNG ", " CHARGE ", " CHARGE ",},
		{" EGAL   ", " EQUAL  ", "AUSGLEIC", " EQUAL  ", " EQUAL  ",},
		{" COMP   ", " COMP   ", "ERHALT  ", " COMP   ", " COMP   ",},
		{" STOP   ", " STOP   ", " STOP   ", " STOP   ", " STOP   ",},
		{" DISPO  ", " AVAIL  ", "VERFUGB ", " DISPO  ", " DISPO  ",},
		{" MANU   ", " MANU   ", "MANUELL ", " MANU   ", " MANU   ",},
		{"  DF1   ", "  DF1   ", "  DF1   ", "  DF1   ", "  DF1   ",},	// 11 : LcdCharge default
		{"  DF2   ", "  DF2   ", "  DF2   ", "  DF2   ", "  DF2   ",},
		{"  DF3   ", "  DF3   ", "  DF3   ", "  DF3   ", "  DF3   ",},
		{"TEMP BAT", "BAT TEMP", "BAT TEMP", "BAT TEMP", "BAT TEMP",},
		{"  DF5   ", "  DF5   ", "  DF5   ", "  DF5   ", "  DF5   ",},
		{"  TH    ", "  TH    ", "  TH    ", "  TH    ", "  TH    ",},
		{"DF POMPE", "DF PUMP ", "FEHL PMP", "DF PUMP ", "DF PUMP ",},	// 17
		{"DEF EEP ", "DEF EEP ", "FEHL EEP", "DEF EEP ", "DEF EEP ",},	// 18 : Init default
		{"DEF MENU", "DEF MENU", "FEHL MEN", "DEF MENU", "DEF MENU",},
		{" DEFAUT ", "SERVICE ", "SERVICE ", "SERVICE ", "SERVICE ",},	// 20
		{"S/N     ", "S/N     ", "S/N     ", "S/N     ", "S/N     ",},	// 21 : Memo
		{"Capacite", "Capacity", "Kapazit ", "Capacita", "Capacita",},
		{"U batt  ", "U batt  ", "U batt  ", "U batt  ", "U batt  ",},
		{"Temp    ", "Temp    ", "Temperat", "Temp    ", "Temp    ",},
		{"Techno  ", "Techno  ", "Techno  ", "Techno  ", "Techno  ",},
		{"Alarme  ", "Warning ", "Warnung ", "Allarme ", "Allarme ",},
		{"Profil  ", "Profile ", "Kennlini", "Profilo ", "Profilo ",},
		{"Phase   ", "Phase   ", "Phase   ", "Fase    ", "Fase    ",},
		{"CFC     ", "CFC     ", "CFC     ", "CFC     ", "CFC     ",},         /* 29 */
		{"% init  ", "% init  ", "% zust  ", "% init  ", "% init  ",},
		{"U debut ", "U start ", "U anfang", "U start ", "U start ",},
		{"I ph1   ", "I ph1   ", "I ph1   ", "I ph1   ", "I ph1   ",},
		{"U gaz   ", "U gaz   ", "U gas   ", "U gaz   ", "U gaz   ",},
		{"I ph3   ", "I ph3   ", "I ph3   ", "I ph3   ", "I ph3   ",},
		{"Tps ph1 ", "Time ph1", "Zeit ph1", "Time ph1", "Time ph1",},
		{"Tps ph2 ", "Time ph2", "Zeit ph2", "Time ph2", "Time ph2",},
		{"Tps ph3 ", "Time ph3", "Zeit ph3", "Time ph3", "Time ph3",},
		{"U fin   ", "U end   ", "U ende  ", "U end   ", "U end   ",},
		{"I fin   ", "I end   ", "I ende  ", "I end   ", "I end   ",},
		{"Temp fin", "Temp end", "Temp end", "Temp end", "Temp end",},
		{"Tps chg ", "Chg Time", "Ladezeit", "Chg Time", "Chg Time",},
		{"Ah      ", "Ah      ", "Ah      ", "Ah      ", "Ah      ",},
		{"kWh     ", "kWh     ", "kWh     ", "kWh     ", "kWh     ",},
		{"Fin chg ", "Chg end ", "Ladeende", "Chg end ", "Chg end ",},
		{"Defaut  ", "Default ", "Vorgabe ", "Default ", "Default ",}, // 45
		{"Ibat    ", "Ibat    ", "Ibatt.  ", "Ibat    ", "Ibat    ",}, // 46
		{"Iconsign", "Iconsign", "Iconsign", "Iconsign", "Iconsign",},
		{"Ubat/elt", "Ubat/elt", "Ubat/elt", "Ubat/elt", "Ubat/elt",},
		{"Ubat    ", "Ubat    ", "Ubat    ", "Ubat    ", "Ubat    ",},
		{"T chgeur", "T chger ", "T lader ", "T chger ", "T chger ",},
		{"Idemande", "Irequest", "Irequest", "Irequest", "Irequest",},
		{"Udemande", "Urequest", "Urequest", "Urequest", "Urequest",},
		{"Temps Ch", "Ch Time ", "Ladezeit", "Ch Tiemp", "Ch Tiemp",},
		{"Ah Ch   ", "Ch Ah   ", "Lad Ah  ", "Ch Ah   ", "Ch Ah   ",},
		{"Etat Ch ", "Ch State", "Lad Stat", "Ch State", "Ch State",},
		{"Etat Ph ", "Ph State", "Ph State", "Ph State", "Ph State",},
		{"Ton     ", "Ton     ", "Ton     ", "Ton     ", "Ton     ",},
		{"Freq    ", "Freq    ", "Frequenz", "Freq    ", "Freq    ",}, // 58
		{"Temps   ", "Time    ", "Zeit    ", "Tiempo  ", "Tiempo  ",}, // 59
		{"JOUR    ", "DAY     ", "TAG     ", "DIA     ", "DIA     ",}, // 60
		{" LMMJVSD", " MTWTFSS", " MDMDFSS", " LMMJVSD", " LMMJVSD",}, // 61
		{"ECR. USB", "USB WRT ", "USB SCHR", "SCRI USB", "SCRI USB",}, // 62
		{"  MEMO  ", "  MEMO  ", "  MEMO  ", "  MEMO  ", "  MEMO  ",}, // 63
		{" REPROG ", "UPGRADE ", "UPGRADE ", "UPGRADE ", "UPGRADE ",},
		{"  SOFT  ", "  SOFT  ", "  SOFT  ", "  SOFT  ", "  SOFT  ",},
		{" RESEAU ", "NETWORK ", "  NET   ", "  NET   ", "  NET   ",}, // 66
		{"DELESTAG", " POWER  ", " POWER  ", " POWER  ", " POWER  ",},
		{"        ", "  CUT   ", "  CUT   ", "  CUT   ", "  CUT   ",},
		{" OUVRE  ", "  OPEN  ", "  OFFEN ", "  OPEN  ", "  OPEN  ",},
		{" ENREG  ", "  SAVE  ", "  SAVE  ", "  SAVE  ", "  SAVE  ",},
		{" CONFIG ", " CONFIG ", " KONFIG ", " CONFIG ", " CONFIG ",},
		{"RF TRANS", "RF TRANS", "RF TRANS", "RF TRANS", "RF TRANS",},
		{"Prog U  ", "Prog U  ", "Prog U  ", "Prog U  ", "Prog U  ",},
		{"Prog I  ", "Prog I  ", "Prog I  ", "Prog I  ", "Prog I  ",},
		{"  DF4   ", "  DF4   ", "  DF4   ", "  DF4   ", "  DF4   ",},
		{"NIV. EAU", " LEVEL  ", " LEVEL  ", " LEVEL  ", " LEVEL  ",},
		{" SERVICE", "  FAULT ", "  FAULT ", "  FAULT ", "  FAULT ",},
};


/**
 * @brief   Creates a field of long : 8 characters (with CscFld)
 * @param
 * c = 0 : standard line, c = 1 : first line
 * *l : line to be completed
 * *t : text for first line
 * v : int value to be printed
 * dp : number of decimal points
 * @retval None
 */
void  CsvLong(uchar c, uchar *l, const uchar *t, long v, uchar dp)
{
	int e;                        /* Integer part (signed) */
	int d;                        /* Decimal part (unsigned) */
	int i;
	int  m;                       /* Power of 10 for division */
	uchar str[8];

	if (c == 0)
	{
		m = 1;
		i = 0;
		while (i < dp)
		{
			m = m * 10;
			i ++;
		}
		e = v / m;
		d = abs(v) % m;
		/* Format definition */
		switch (dp)
		{
		case 0 :
			i = sprintf((char *)l, "%7i%1c", e, CsvFld);
			break ;
		case 1 :
			i = sprintf((char *)l, "%5i%1c%01u%1c", e, CsvDec, d, CsvFld);
			break ;
		case 2 :
			i = sprintf((char *)l, "%4i%1c%02u%1c", e, CsvDec, d, CsvFld);
			break ;
		case 3 :
			i = sprintf((char *)l, "%3i%1c%03u%1c", e, CsvDec, d, CsvFld);
			break ;
		case 4 :
			i = sprintf((char *)l, "%2i%1c%04u%1c", e, CsvDec, d, CsvFld);
			break ;
		case 5 :
			i = sprintf((char *)l, "%1u%1c%05u%1c", e, CsvDec, d, CsvFld);
			break ;
		}
	}
	else
	{
		memcpy(str, t, 7);
		i = sprintf((char *)l, "%-7.7s%1c", str, CsvFld);
	}
}

/**
 * @brief   Print 20 characters text
 * @param
 * c = 0 : standard line, c = 1 : first line
 * *l : line to be completed
 * *t : text for first line
 * *v : text to be printed
 * @retval None
 */
void  CsvText20(uchar c, const uchar *l, const uchar *t, const uchar *v)
{

	uchar str[20];

	if (c == 0)
	{
		memcpy(str, v, 19);
	}
	else
	{
		memcpy(str, t, 19);
	}
	sprintf((char *)l, "%-19.19s%1c", str, CsvFld);
}


void  CsvText20_10(uchar c, const uchar *l, const uchar *t, const uchar *v)
{

	uchar str[20];

	memset (str, 0, 19);

	if (c == 0)
	{
		memcpy(str, v, 10);
	}
	else
	{
		memcpy(str, t, 10);
	}
	sprintf((char *)l, "%-19.19s%1c", str, CsvFld);
}

/**
 * @brief   Print 8 characters text
 * @param
 * c = 0 : standard line, c = 1 : first line
 * *l : line to be completed
 * *t : text for first line
 * *v : text to be printed
 * @retval None
 */
void  CsvText8(uchar c, uchar *l, const uchar *t, const uchar *v)
{

	uchar str[8];

	if (c == 0)
	{
		memcpy(str, v, 7);
	}
	else
	{
		memcpy(str, t, 7);
	}
	sprintf((char *)l, "%-7.7s%1c", str, CsvFld);
}

/**
 * @brief   Creates a field of data (date + hour) : 20 characters (with CscFld)
 * @param
 * c = 0 : standard line, c = 1 : first line
 * *l : line to be completed
 * *dt : date/time to be printed
 * @retval None
 */
void  CsvDateHour(uchar c, uchar *l, uchar *t, StructDateTime dt)
{

	uchar str[20];
	if (dt.Sec > 59)
		dt.Sec = 0;

	if (c == 0)
	{
		if (MaskArea1 != 0) // US
			sprintf((char *)l, "%02u%1c%02u%1c%04u%1c%02u%1c%02u%1c%02u%1c", dt.Month, CsvDat, dt.Date, CsvDat, dt.Year, SPACE, dt.Hr, CsvTim, dt.Min, CsvTim, dt.Sec, CsvFld);
		else
			sprintf((char *)l, "%02u%1c%02u%1c%04u%1c%02u%1c%02u%1c%02u%1c", dt.Date, CsvDat, dt.Month, CsvDat, dt.Year, SPACE, dt.Hr, CsvTim, dt.Min, CsvTim, dt.Sec, CsvFld);
	}
	else
	{
		memcpy(str, t, 19);
		sprintf((char *)l, "%-19.19s%1c", str, CsvFld);
	}
}

void  CsvNewFileName(uchar t, uchar *n)
{
	/* Create a new file name and date */
	/* File name : Type Day Hour Min */
	/* t : type of file */
	/* 0 : profile(P) .csv */
	/* 1 : memo(M) .csv */
	/* 2 : bitmap(B) .bmp */
	/* 3 : config .mot */
	/* 4 : rc5 profile .rc5*/
	/* n : name of file */

	uint  d;            /* Number of day in year */
	switch (State.DateR.Month)
	{
	case 1:           /* Janvier */
		d = State.DateR.Date + 0;
		break;
	case 2:
		d = State.DateR.Date + 31;
		break;
	case 3:
		d = State.DateR.Date + 31 + 28;
		break;
	case 4:
		d = State.DateR.Date + 31 + 28 + 31;
		break;
	case 5:
		d = State.DateR.Date + 31 + 28 + 31 + 30;
		break;
	case 6:
		d = State.DateR.Date + 31 + 28 + 31 + 30 + 31;
		break;
	case 7:
		d = State.DateR.Date + 31 + 28 + 31 + 30 + 31 + 30;
		break;
	case 8:
		d = State.DateR.Date + 31 + 28 + 31 + 30 + 31 + 30 + 31;
		break;
	case 9:
		d = State.DateR.Date + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 30;
		break;
	case 10:
		d = State.DateR.Date + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 30 + 31;
		break;
	case 11:
		d = State.DateR.Date + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 30 + 31 + 30;
		break;
	case 12:
		d = State.DateR.Date + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 30 + 31 + 30 + 31;
		break;
	}
	switch (t)
	{
	case 0 :          /* Profile */
		sprintf((char *)n, "P%03u%02u%02u.CSV",  d, State.DateR.Hr, State.DateR.Min);
		break;
	case 1 :          /* Memo */
		sprintf((char *)n, "M%03u%02u%02u.CSV", d, State.DateR.Hr, State.DateR.Min);
		//i = sprintf(n, "%8.8sM%03u%02u%02u.CSV",  &SerialNumber.SerNum[0], d, State.DateR.Hr, State.DateR.Min);
		break;
	case 2 :          /* Bitmap */
		sprintf((char *)n, "B%03u%02u%02u.BMP",  d, State.DateR.Hr, State.DateR.Min);
		break;
	case 3 :          /* Configuration */
		//sprintf((char *)n, "C%03u%02u%02u.MOT",  d, State.DateR.Hr, State.DateR.Min);
		sprintf((char *)n, "RXIQCFG.MOT");
		break;
	case 4 :          /* Encrypted profile */
		sprintf((char *)n, "P%03u%02u%02u.RC5",  d, State.DateR.Hr, State.DateR.Min);
		break;
	// MODIF R3.3
	case 5 :          /* E1 & E2 memo */
		sprintf((char *)n, "E%03u%02u%02u.CSV",  d, State.DateR.Hr, State.DateR.Min);
		break;
	}
}

void CsvConfigLine1(uchar c, uchar *l, uint *n)
{
	/* Prints config line */
	/* c = 0 : standard line, c = 1 : first line */
	/* l : line to be completed */
	/* n : number of char */

	uint  Ilcd;
	char ChargerName[20];
	char BleVersion[5];
	char BleMac[20];

	Ilcd = Menu.Imax;
	if (Ilcd > Menu.ImaxCharger)
		Ilcd = Menu.ImaxCharger;
	if (Menu.CellSize != 255)
		sprintf(&ChargerName[0], "IQ%2.2uV%3.3uA", (Menu.NbCells*2), (Ilcd/10));
	else if (Menu.ChgType == TYPE_3PHASE)
		sprintf(&ChargerName[0], "IQ 243648V");
	else
		sprintf(&ChargerName[0], "IQ 3648V");

	CsvText20(c, l + 0, &ListWord7[30][LANG][0], (const uchar *)&ChargerName[0]);
	CsvText20(c, l + 20, &ListWord7[25][LANG][0], (const uchar *)&version[0]);
	CsvText20_10(c, l + 40, &ListWord7[27][LANG][0], (const uchar *)&SerialNumber.SerNum[0]);
	if (MaskArea1 != 0)
		CsvText20(c, l + 60, &ListWord7[33][LANG][0], &ListWord7[35][LANG][0]);
	else
		CsvText20(c, l + 60, &ListWord7[33][LANG][0], &ListWord7[34][LANG][0]);

	sprintf(&BleVersion[0], "%4.4s", &Ble.version[0]);
	sprintf(&BleMac[0], "%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X", Ble.MacAddress[0], Ble.MacAddress[1], Ble.MacAddress[2], Ble.MacAddress[3], Ble.MacAddress[4], Ble.MacAddress[5]);

	CsvText20(c, l + 80, "BLE Ver", (const uchar *)&BleVersion[0]);
	CsvText20(c, l + 100, "BLE Mac", (const uchar *)&BleMac[0]);

	/* CR LF final */
	l[120] = CR;
	l[121] = LF;
	*n = 122;
}

void  CsvConfigLine2(uchar c, uchar *l, uint *n)
{
	/* Prints config line */
	/* c = 0 : standard line, c = 1 : first line */
	/* l : line to be completed */
	/* n : numer of char */
	uchar str1[20];
	CsvLong(c, l +  0, &ListWord7[11][LANG][0], (long)Menu.BatCap, 0);
	if (MaskArea1 != 0) // US
		CsvLong(c, l +  8, &ListWord7[12][LANG][0], (slong)Menu.BatTempF, 0);
	else
		CsvLong(c, l +  8, &ListWord7[12][LANG][0], (slong)Menu.BatTemp, 0);
	memset(str1, SPACE, 20);
	memcpy(str1, &ListWord6[Menu.Profile][LANG][0], 6);
	CsvText8(c, l +  16, "Profile", str1);
	CsvLong(c, l + 24, &ListWord7[14][LANG][0], (long)Menu.ChgDelayVal, 0);
	CsvLong(c, l + 32, &ListWord7[15][LANG][0], (long)Menu.InitDOD, 0);
	CsvLong(c, l + 40, &ListWord7[16][LANG][0], (long)Menu.Ifloating, 1);
	CsvLong(c, l + 48, &ListWord7[17][LANG][0], (long)Menu.Vfloating, 3);
	CsvLong(c, l + 56, &ListWord7[18][LANG][0], (long)Menu.Iequal, 1);
	CsvLong(c, l + 64, &ListWord7[19][LANG][0], (long)Menu.EqualTime, 0);
	CsvLong(c, l + 72, &ListWord7[20][LANG][0], (long)Menu.EqualDelay, 0);
	CsvLong(c, l + 80, &ListWord7[21][LANG][0], (long)Menu.EqualPeriod, 0);
	CsvLong(c, l + 88, &ListWord7[22][LANG][0], (long)Menu.CableLength, 1);
	CsvLong(c, l + 96, &ListWord7[23][LANG][0], (long)Menu.CableSection, 0);
	CsvLong(c, l + 104, &ListWord7[24][LANG][0], (long)Menu.EVtime, 0);
	/* CR LF final */
	l[112] = CR;
	l[113] = LF;
	*n = 114;
}

void  CsvStatusLine(uchar c, uchar *l, uint *n)
{
	/* Prints status line */
	/* c = 0 : standard line, c = 1 : first line */
	/* l : line to be completed */
	/* n : numer of char */
	CsvLong(c, l +  0, &ListWord7[0][LANG][0], (long)Status.NbCharge, 0);
	CsvLong(c, l +  8, &ListWord7[1][LANG][0], (long)Status.NbPartialCharge, 0);
	CsvLong(c, l + 16, &ListWord7[2][LANG][0], (long)(Status.NbCharge - Status.NbPartialCharge), 0);
	CsvLong(c, l + 24, &ListWord7[3][LANG][0], (long)Status.NbEqual, 0);
	CsvLong(c, l + 32, &ListWord7[4][LANG][0], (long)(Status.NbConverterDef), 0);
	CsvLong(c, l + 40, &ListWord7[5][LANG][0], (long)Status.NbFuseDef, 0);
	CsvLong(c, l + 48, &ListWord7[6][LANG][0], (long)Status.NbBadBatDef, 0);
	CsvLong(c, l + 56, &ListWord7[9][LANG][0], (long)Status.NbTempDef, 0);
#ifdef  ENERSYS_EU
	CsvLong(c, l + 64, &ListWord7[10][LANG][0], (long)Status.NbPumpDef, 0);
#else
    CsvLong(c, l + 64, &ListWord7[10][LANG][0], (long)Status.NbdIdTDef, 0);
#endif   
	CsvLong(c, l + 72, &ListWord7[7][LANG][0], (long)Status.NbOverdisDef, 0);
#ifdef  ENERSYS_EU    
	CsvLong(c, l + 80, &ListWord7[8][LANG][0], (long)(Status.NbdIdTDef + Status.NbdVdTDef + Status.NbTimeSecuDef + Status.NbOtherBatDef), 0);
#else
    CsvLong(c, l + 80, &ListWord7[8][LANG][0], (long)(Status.NbdVdTDef + Status.NbTimeSecuDef + Status.NbOtherBatDef), 0);
#endif
	CsvLong(c, l + 88, &ListWord7[36][LANG][0], (long)(Status.NbNetworkDef), 0);
	CsvLong(c, l + 96, &ListWord7[37][LANG][0], (long)(Status.NbThModule), 0);	// MODIF R3.1
    /* CR LF final */
    l[104] = CR;
    l[105] = LF;
    *n = 106;
}

void CsvMemoLine(uchar c, uchar *l, uint *n, StructMemo *m)
{
	/* Prints a Memo line in VncData  */
	/* c = 0 : standard line, c = 1 : first line */
	/* l : line to be completed */
	/* n : number of char */
	/* *m : memo to record */
	uchar df;
	uchar str1[20];
	StructDateTime dt;
	memset(l, SPACE, 240);

	if (Status.NbRstPointer == 0)
	{
		CsvLong(c, l +   0, "Nr     ", Status.MemoPointer - (long)UsbMemoPtr + 1, 0);
	}
	else
	{
		CsvLong(c, l +   0, "Nr     ", SEGMEMONB + SEGMEMOTOP - (long)UsbMemoPtr, 0);
	}
	memset(str1, SPACE, 20);
	memcpy(str1, (uchar *)((*m).BatSN), 14);
	CsvText20(c, l + 8, &ListWord_8[21][LANG][0] /* "SN     " */, str1);
	if ((*m).CapAutoManu != 0)
		CsvLong(c, l +  28, &ListWord_8[22][LANG][0] /* "Cap    " */, (long)(*m).BatCap, 0);
	else
		CsvText8(c, l +  28, &ListWord_8[22][LANG][0] /* "Cap    " */, "      ");
        
	if (((*m).Default & BitDFbadBat) != 0) // MODIF R2.7
        CsvText8(c, l + 36, &ListWord_8[23][LANG][0] /* "U batt " */, "  ----");
    else
        CsvLong(c, l +  36, &ListWord_8[23][LANG][0] /* "U batt " */, (long)(*m).BatVolt, 0);
	
    if (MaskArea1 != 0) // US
		CsvLong(c, l +  44, &ListWord_8[24][LANG][0] /* "Temp   " */, ((long)(*m).BatTemp* 9) / 5 + 32, 0);
	else
		CsvLong(c, l +  44, &ListWord_8[24][LANG][0] /* "Temp   " */, (long)(*m).BatTemp, 0);
	CsvText8(c, l +  52, &ListWord_8[25][LANG][0]/* "Techno " */, (uchar*)&ListWord6[(*m).BatTechno + TEC_POS][LANG][0]);
	CsvText8(c, l +  60, &ListWord_8[27][LANG][0]/* "Profile" */, (uchar*)&ListWord6[(*m).Profile][LANG][0]);
	CsvLong(c, l +  68, &ListWord_8[30][LANG][0] /* "InitSoC" */, (long)(*m).InitSOC, 0);
    
	if (((*m).Default & BitDFbadBat) != 0) // MODIF R2.7
        CsvText8(c, l + 76, &ListWord_8[31][LANG][0] /* "U start" */, "  ----");
    else
        CsvLong(c, l +  76, &ListWord_8[31][LANG][0] /* "U start" */, (long)((*m).SocVoltage / 10), 2);
	
    CsvLong(c, l +  84, &ListWord_8[38][LANG][0] /* "U end  " */, (long)((*m).EocVoltage / 10), 2);
	CsvLong(c, l +  92, &ListWord_8[39][LANG][0] /* "I end  " */, (long)(*m).EocCurrent, 0);
	if (MaskArea1 != 0) // US
		CsvLong(c, l + 100, &ListWord_8[40][LANG][0] /* "Tp end " */, ((long)(*m).EocTemp* 9) / 5 + 32, 0);
	else
		CsvLong(c, l + 100, &ListWord_8[40][LANG][0] /* "Tp end " */, (long)(*m).EocTemp, 0);
	CsvLong(c, l + 108, &ListWord_8[41][LANG][0] /* "ChgTime" */, (long)(*m).ChgTime, 0);
	CsvLong(c, l + 116, &ListWord_8[42][LANG][0] /* "Ah     " */, (long)(*m).ChgAh, 0);
	CsvLong(c, l + 124, &ListWord_8[43][LANG][0] /* "Wh     " */, (long)(*m).ChgWh, 2);
	CsvText8(c, l +  132, &ListWord_8[44][LANG][0]/* "Chg end" */, &ListWord7[(*m).TypeEoc + 1][LANG][0]);
	if( (((*m).Default & BitDFconverter) != 0) ||
			(((*m).Default & BitDFotherCur) != 0) )
		df = 1;
	else if (((*m).Default & BitDFfuse) != 0)
		df = 2;
	else if (((*m).Default & BitDFbadBat) != 0)
		df = 3;
	else if (((*m).Default & BitDFtemp) != 0)
		df = 6;
	else if (((*m).Default & BitDFpump) != 0)
		df = 7;
#ifdef ENERSYS_EU        
	else if( (((*m).Default & BitDFdIdT) != 0) /*|| (((*m).Default & BitDFdVdT) != 0)*/ ||
			(((*m).Default & BitDFtimeSecu) != 0) || (((*m).Default & BitDFotherBat) != 0) || (((*m).Default & BitDFDisDF5) != 0) )
		df = 5;
#else
    else if( ((*m).Default & BitDFdIdT) != 0)
		df = 7;
	else if ( (((*m).Default & BitDFtimeSecu) != 0) || (((*m).Default & BitDFotherBat) != 0) || (((*m).Default & BitDFDisDF5) != 0))
		df = 5;
#endif
	else if (((*m).Profile == EQUAL) && (((*m).Default & BitDFdVdT) != 0) )      
		df = 5;
	else if (((*m).Default & BitDFoverdis) != 0)
		df = 4;
	else if (((*m).Default & BitDFnetwork) != 0)
		df = 10;
	else
		df = 0;
	CsvText8(c, l + 140, &ListWord_8[45][LANG][0], &ListWord_6[df][LANG][0]);
	// Soc Date
	dt.Year = (*m).SocDate.Year;
	dt.Year += 2000;
	dt.Month = (*m).SocDate.Month;
	dt.Date = (*m).SocDate.Date;
	dt.Hr = (*m).SocDate.Hr;
	dt.Min = (*m).SocDate.Min;
	CsvDateHour(c, l + 148, (uchar *)&ListWord3[0][LANG][0], dt);
	// EocDate
	dt.Year = (*m).EocDate.Year;
	dt.Year += 2000;
	dt.Month = (*m).EocDate.Month;
	dt.Date = (*m).EocDate.Date;
	dt.Hr = (*m).EocDate.Hr;
	dt.Min = (*m).EocDate.Min;
	CsvDateHour(c, l + 168, (uchar *)&ListWord3[1][LANG][0], dt);
	/* End of charge code */
	CsvLong(c, l + 188, &ListWord_8[29][LANG][0], (long)(*m).Default, 0);
    if (((*m).CFC & BitBatDisconnect) != 0)
    {
        CsvText8(c, l + 196, "Bat Disc", "YES    ");
       	/* CR LF final */
    	l[204] = CR;
    	l[205] = LF;
    	*n = 206;                     // Limited to 240 char

    }
    else
    {
    	/* CR LF final */
    	//l[196] = CR;
    	//l[197] = LF;
    	//*n = 198;                     // Limited to 240 char
        CsvText8(c, l + 196, "Bat Disc", "NO     ");
       	/* CR LF final */
    	l[204] = CR;
    	l[205] = LF;
    	*n = 206;                     // Limited to 240 char
    }
}


void  CsvLongHexa(uchar c, uchar *l, const uchar *t, long v, uchar dp)
{
	int e;                        /* Integer part (signed) */
	int d;                        /* Decimal part (unsigned) */
	int i;
	int  m;                       /* Power of 10 for division */
	uchar str[8];

	if (c == 0)
	{
		m = 1;
		i = 0;
		while (i < dp)
		{
			m = m * 10;
			i ++;
		}
		e = v / m;
		d = abs(v) % m;
		/* Format definition */
		switch (dp)
		{
		case 0 :
			i = sprintf((char *)l, "%08X%1c", e, CsvFld);
			break ;
		case 1 :
			i = sprintf((char *)l, "%5i%1c%01u%1c", e, CsvDec, d, CsvFld);
			break ;
		case 2 :
			i = sprintf((char *)l, "%4i%1c%02u%1c", e, CsvDec, d, CsvFld);
			break ;
		case 3 :
			i = sprintf((char *)l, "%3i%1c%03u%1c", e, CsvDec, d, CsvFld);
			break ;
		case 4 :
			i = sprintf((char *)l, "%2i%1c%04u%1c", e, CsvDec, d, CsvFld);
			break ;
		case 5 :
			i = sprintf((char *)l, "%1u%1c%05u%1c", e, CsvDec, d, CsvFld);
			break ;
		}
	}
	else
	{
		memcpy(str, t, 8);
		i = sprintf((char *)l, "%-8.8s%1c", str, CsvFld);
	}
}


void  CsvProfileLine2(uchar c, uchar *l, uint *n)
{
	/* Prints a profile line */
	/* c = 0 : standard line, c = 1 : first line */
	/* l : line to be completed */
	/* n : numer of char */
	CsvDateHour(c, l, "Time   ", State.DateR);
	CsvLong(c, l + 20,  "Ibat   ", (long)DataR.Ibat, 1);
	CsvLong(c, l + 28,  "Vbat/c ", (long)DataR.VbatCell, 3);
	CsvLong(c, l + 36,  "Vbat   ", (long)DataR.Vbat, 2);
	CsvLong(c, l + 44,  "Vchger ", (long)DataR.Vcharger, 2);
	CsvLong(c, l + 52,  "Vfuse  ", (long)DataR.Vfuse, 2);
	CsvLong(c, l + 60,  "Power  ", (long)DataR.Power, 0);
	CsvLong(c, l + 68,  "Ah     ", (long)DataR.Ah, 0);
	CsvLong(c, l + 76,  "IQBatTp", (long)IQData.BatTemp, 0);

	CsvLong(c, l + 84,  "IReq M1", (long)LLCRamRW[0].IReq, 1);
	CsvLong(c, l + 92,  "Ibat M1", (long)LLCRamRO[0].IBat, 1);
	CsvLong(c, l + 100, "VReq M1", (long)LLCRamRW[0].VReq, 2);
	CsvLong(c, l + 108, "Vbat M1", (long)LLCRamRO[0].VBat, 2);
	CsvLongHexa(c, l + 116, "Def  M1 ", (long)LLCRamRO[0].UnionRamRODef.Def, 0); // size = 9 car
	CsvLongHexa(c, l + 125, "Stat M1 ", (long)LLCRamRO[0].UnionRamROStatus.Status, 0);

	CsvLong(c, l + 134, "IReq M2", (long)LLCRamRW[1].IReq, 1);
	CsvLong(c, l + 142, "Ibat M2", (long)LLCRamRO[1].IBat, 1);
	CsvLong(c, l + 150, "VReq M2", (long)LLCRamRW[1].VReq, 2);
	CsvLong(c, l + 158, "Vbat M2", (long)LLCRamRO[1].VBat, 2);
	CsvLongHexa(c, l + 166, "Def  M2 ", (long)LLCRamRO[1].UnionRamRODef.Def, 0);
	CsvLongHexa(c, l + 175, "Stat M2 ", (long)LLCRamRO[1].UnionRamROStatus.Status, 0);

	CsvLong(c, l + 184, "IReq M3", (long)LLCRamRW[2].IReq, 1);
	CsvLong(c, l + 192, "Ibat M3", (long)LLCRamRO[2].IBat, 1);
	CsvLong(c, l + 200, "VReq M3", (long)LLCRamRW[2].VReq, 2);
	CsvLong(c, l + 208, "Vbat M3", (long)LLCRamRO[2].VBat, 2);
	CsvLongHexa(c, l + 216, "Def  M3 ", (long)LLCRamRO[2].UnionRamRODef.Def, 0);
	CsvLongHexa(c, l + 225, "Stat M3 ", (long)LLCRamRO[2].UnionRamROStatus.Status, 0);
    
    CsvLong(c, l + 234, "IReq M4", (long)LLCRamRW[3].IReq, 1);
	CsvLong(c, l + 242, "Ibat M4", (long)LLCRamRO[3].IBat, 1);
	CsvLong(c, l + 250, "VReq M4", (long)LLCRamRW[3].VReq, 2);
	CsvLong(c, l + 258, "Vbat M4", (long)LLCRamRO[3].VBat, 2);
	CsvLongHexa(c, l + 266, "Def  M4 ", (long)LLCRamRO[3].UnionRamRODef.Def, 0);
	CsvLongHexa(c, l + 275, "Stat M4 ", (long)LLCRamRO[3].UnionRamROStatus.Status, 0);

    CsvLong(c, l + 284, "IReq M5", (long)LLCRamRW[4].IReq, 1);
	CsvLong(c, l + 292, "Ibat M5", (long)LLCRamRO[4].IBat, 1);
	CsvLong(c, l + 300, "VReq M5", (long)LLCRamRW[4].VReq, 2);
	CsvLong(c, l + 308, "Vbat M5", (long)LLCRamRO[4].VBat, 2);
	CsvLongHexa(c, l + 316, "Def  M5 ", (long)LLCRamRO[4].UnionRamRODef.Def, 0);
	CsvLongHexa(c, l + 325, "Stat M5 ", (long)LLCRamRO[4].UnionRamROStatus.Status, 0);

    CsvLong(c, l + 334, "IReq M6", (long)LLCRamRW[5].IReq, 1);
	CsvLong(c, l + 342, "Ibat M6", (long)LLCRamRO[5].IBat, 1);
	CsvLong(c, l + 350, "VReq M6", (long)LLCRamRW[5].VReq, 2);
	CsvLong(c, l + 358, "Vbat M6", (long)LLCRamRO[5].VBat, 2);
	CsvLongHexa(c, l + 366, "Def  M6 ", (long)LLCRamRO[5].UnionRamRODef.Def, 0);
	CsvLongHexa(c, l + 375, "Stat M6 ", (long)LLCRamRO[5].UnionRamROStatus.Status, 0);

	CsvLong(c, l + 384,  "Vloop2 ", (long)DataR.Vloop2, 0);
	CsvLong(c, l + 392,  "Vloop1 ", (long)DataR.Vloop1, 0);
	CsvLong(c, l + 400, "Del LW ", (long)DeltaLoopWidth, 0);
	CsvLong(c, l + 408, "Del LWM", (long)DeltaLoopWidthMax, 0);
	CsvLong(c, l + 416, "LoopWdt", (long)LoopWidthNew, 0); 
	CsvLong(c, l + 424, "Tamb   ", (long)Tambiant, 0); 
    
    CsvLong(c, l + 432, "VLoopL ", (long)DataW.VLoopLow, 0);
    CsvLong(c, l + 440, "VLoopH ", (long)DataW.VLoopHigh, 0);
    CsvLong(c, l + 448, "TempCap", (long)TempCap, 0);
    
	*n = 456;
	/* CR LF final */
	l[*n] = CR;
	*n = *n + 1;
	l[*n] = LF;
	*n = *n + 1;
}

void  CsvProfileLine(uchar c, uchar *l, uint *n)
{
    uint pos;
    
	/* Prints a profile line */
	/* c = 0 : standard line, c = 1 : first line */
	/* l : line to be completed */
	/* n : numer of char */
	CsvDateHour(c, l, "Time   ", State.DateR);
	CsvLong(c, l + 20,  "Ibat   ", (long)DataR.Ibat, 1);
	CsvLong(c, l + 28,  "Vbat/c ", (long)DataR.VbatCell, 3);
	CsvLong(c, l + 36,  "Vbat   ", (long)DataR.Vbat, 2);
	CsvLong(c, l + 44,  "Vchger ", (long)DataR.Vcharger, 2);
	CsvLong(c, l + 52,  "Vfuse  ", (long)DataR.Vfuse, 2);
	CsvLong(c, l + 60,  "Power  ", (long)DataR.Power, 0);
	CsvLong(c, l + 68,  "Ah     ", (long)DataR.Ah, 0);
	CsvLong(c, l + 76,  "IQBatTp", (long)IQData.BatTemp, 0);

	CsvLong(c, l + 84,  "IReq M1", (long)LLCRamRW[0].IReq, 1);
	CsvLong(c, l + 92,  "Ibat M1", (long)LLCRamRO[0].IBat, 1);
	CsvLong(c, l + 100, "VReq M1", (long)LLCRamRW[0].VReq, 2);
	CsvLong(c, l + 108, "Vbat M1", (long)LLCRamRO[0].VBat, 2);
	CsvLongHexa(c, l + 116, "Def  M1", (long)LLCRamRO[0].UnionRamRODef.Def, 0); // size = 9 car
	CsvLongHexa(c, l + 125, "Stat M1", (long)LLCRamRO[0].UnionRamROStatus.Status, 0);

	CsvLong(c, l + 134, "IReq M2", (long)LLCRamRW[1].IReq, 1);
	CsvLong(c, l + 142, "Ibat M2", (long)LLCRamRO[1].IBat, 1);
	CsvLong(c, l + 150, "VReq M2", (long)LLCRamRW[1].VReq, 2);
	CsvLong(c, l + 158, "Vbat M2", (long)LLCRamRO[1].VBat, 2);
	CsvLongHexa(c, l + 166, "Def  M2", (long)LLCRamRO[1].UnionRamRODef.Def, 0);
	CsvLongHexa(c, l + 175, "Stat M2", (long)LLCRamRO[1].UnionRamROStatus.Status, 0);

	CsvLong(c, l + 184, "IReq M3", (long)LLCRamRW[2].IReq, 1);
	CsvLong(c, l + 192, "Ibat M3", (long)LLCRamRO[2].IBat, 1);
	CsvLong(c, l + 200, "VReq M3", (long)LLCRamRW[2].VReq, 2);
	CsvLong(c, l + 208, "Vbat M3", (long)LLCRamRO[2].VBat, 2);
	CsvLongHexa(c, l + 216, "Def  M3", (long)LLCRamRO[2].UnionRamRODef.Def, 0);
	CsvLongHexa(c, l + 225, "Stat M3", (long)LLCRamRO[2].UnionRamROStatus.Status, 0);
    
    CsvLong(c, l + 234, "IReq M4", (long)LLCRamRW[3].IReq, 1);
	CsvLong(c, l + 242, "Ibat M4", (long)LLCRamRO[3].IBat, 1);
	CsvLong(c, l + 250, "VReq M4", (long)LLCRamRW[3].VReq, 2);
	CsvLong(c, l + 258, "Vbat M4", (long)LLCRamRO[3].VBat, 2);
	CsvLongHexa(c, l + 266, "Def  M4", (long)LLCRamRO[3].UnionRamRODef.Def, 0);
	CsvLongHexa(c, l + 275, "Stat M4", (long)LLCRamRO[3].UnionRamROStatus.Status, 0);

    CsvLong(c, l + 284, "IReq M5", (long)LLCRamRW[4].IReq, 1);
	CsvLong(c, l + 292, "Ibat M5", (long)LLCRamRO[4].IBat, 1);
	CsvLong(c, l + 300, "VReq M5", (long)LLCRamRW[4].VReq, 2);
	CsvLong(c, l + 308, "Vbat M5", (long)LLCRamRO[4].VBat, 2);
	CsvLongHexa(c, l + 316, "Def  M5", (long)LLCRamRO[4].UnionRamRODef.Def, 0);
	CsvLongHexa(c, l + 325, "Stat M5", (long)LLCRamRO[4].UnionRamROStatus.Status, 0);

    CsvLong(c, l + 334, "IReq M6", (long)LLCRamRW[5].IReq, 1);
	CsvLong(c, l + 342, "Ibat M6", (long)LLCRamRO[5].IBat, 1);
	CsvLong(c, l + 350, "VReq M6", (long)LLCRamRW[5].VReq, 2);
	CsvLong(c, l + 358, "Vbat M6", (long)LLCRamRO[5].VBat, 2);
	CsvLongHexa(c, l + 366, "Def  M6", (long)LLCRamRO[5].UnionRamRODef.Def, 0);
	CsvLongHexa(c, l + 375, "Stat M6", (long)LLCRamRO[5].UnionRamROStatus.Status, 0);
    
    CsvLong(c, l + 384, "IReq M7", (long)LLCRamRW[6].IReq, 1);
	CsvLong(c, l + 392, "Ibat M7", (long)LLCRamRO[6].IBat, 1);
	CsvLong(c, l + 400, "VReq M7", (long)LLCRamRW[6].VReq, 2);
	CsvLong(c, l + 408, "Vbat M7", (long)LLCRamRO[6].VBat, 2);
	CsvLongHexa(c, l + 416, "Def  M7", (long)LLCRamRO[6].UnionRamRODef.Def, 0);
	CsvLongHexa(c, l + 425, "Stat M7", (long)LLCRamRO[6].UnionRamROStatus.Status, 0);

    CsvLong(c, l + 434, "IReq M8", (long)LLCRamRW[7].IReq, 1);
	CsvLong(c, l + 442, "Ibat M8", (long)LLCRamRO[7].IBat, 1);
	CsvLong(c, l + 450, "VReq M8", (long)LLCRamRW[7].VReq, 2);
	CsvLong(c, l + 458, "Vbat M8", (long)LLCRamRO[7].VBat, 2);
	CsvLongHexa(c, l + 466, "Def  M8", (long)LLCRamRO[7].UnionRamRODef.Def, 0);
	CsvLongHexa(c, l + 475, "Stat M8", (long)LLCRamRO[7].UnionRamROStatus.Status, 0);
    
    pos = 484 + 8;
    
    if (Menu.ChgType == TYPE_1PHASE)
    {
        //CsvLong(c, l + pos,  "M1 Amb", (long)((LLCRamRO[0].Temp[0] >> 8) - 64), 0); pos += 8;
    	CsvLong(c, l + pos,  "M1+Vbat", (long)((LLCRamRO[0].Temp[1] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M1-Vbat", (long)((LLCRamRO[0].Temp[1] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M1  MOS", (long)((LLCRamRO[0].Temp[2] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M1  TRF", (long)((LLCRamRO[0].Temp[2] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M1  FLY", (long)((LLCRamRO[0].Temp[3] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M1  PFC", (long)((LLCRamRO[0].Temp[3] >> 8) - 64), 0); pos += 8;
    
        //CsvLong(c, l + pos,  "M2 Amb", (long)((LLCRamRO[1].Temp[0] >> 8) - 64), 0); pos += 8;
    	CsvLong(c, l + pos,  "M2+Vbat", (long)((LLCRamRO[1].Temp[1] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M2-Vbat", (long)((LLCRamRO[1].Temp[1] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M2  MOS", (long)((LLCRamRO[1].Temp[2] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M2  TRF", (long)((LLCRamRO[1].Temp[2] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M2  FLY", (long)((LLCRamRO[1].Temp[3] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M2  PFC", (long)((LLCRamRO[1].Temp[3] >> 8) - 64), 0); pos += 8;
    
        //CsvLong(c, l + pos,  "M3 Amb", (long)((LLCRamRO[2].Temp[0] >> 8) - 64), 0); pos += 8;
    	CsvLong(c, l + pos,  "M3+Vbat", (long)((LLCRamRO[2].Temp[1] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M3-Vbat", (long)((LLCRamRO[2].Temp[1] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M3  MOS", (long)((LLCRamRO[2].Temp[2] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M3  TRF", (long)((LLCRamRO[2].Temp[2] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M3  FLY", (long)((LLCRamRO[2].Temp[3] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M3  PFC", (long)((LLCRamRO[2].Temp[3] >> 8) - 64), 0); pos += 8;
    
        //CsvLong(c, l + pos,  "M4 Amb", (long)((LLCRamRO[3].Temp[0] >> 8) - 64), 0); pos += 8;
    	CsvLong(c, l + pos,  "M4+Vbat", (long)((LLCRamRO[3].Temp[1] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M4-Vbat", (long)((LLCRamRO[3].Temp[1] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M4  MOS", (long)((LLCRamRO[3].Temp[2] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M4  TRF", (long)((LLCRamRO[3].Temp[2] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M4  FLY", (long)((LLCRamRO[3].Temp[3] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M4  PFC", (long)((LLCRamRO[3].Temp[3] >> 8) - 64), 0); pos += 8;
    
        //CsvLong(c, l + pos,  "M5 Amb", (long)((LLCRamRO[4].Temp[0] >> 8) - 64), 0); pos += 8;
    	CsvLong(c, l + pos,  "M5+Vbat", (long)((LLCRamRO[4].Temp[1] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M5-Vbat", (long)((LLCRamRO[4].Temp[1] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M5  MOS", (long)((LLCRamRO[4].Temp[2] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M5  TRF", (long)((LLCRamRO[4].Temp[2] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M5  FLY", (long)((LLCRamRO[4].Temp[3] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M5  PFC", (long)((LLCRamRO[4].Temp[3] >> 8) - 64), 0); pos += 8;
    
        //CsvLong(c, l + pos,  "M6 Amb", (long)((LLCRamRO[5].Temp[0] >> 8) - 64), 0); pos += 8;
    	CsvLong(c, l + pos,  "M6+Vbat", (long)((LLCRamRO[5].Temp[1] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M6-Vbat", (long)((LLCRamRO[5].Temp[1] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M6  MOS", (long)((LLCRamRO[5].Temp[2] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M6  TRF", (long)((LLCRamRO[5].Temp[2] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M6  FLY", (long)((LLCRamRO[5].Temp[3] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M6  PFC", (long)((LLCRamRO[5].Temp[3] >> 8) - 64), 0); pos += 8;
        
        //CsvLong(c, l + pos,  "M7 Amb", (long)((LLCRamRO[6].Temp[0] >> 8) - 64), 0); pos += 8;
    	CsvLong(c, l + pos,  "M7+Vbat", (long)((LLCRamRO[6].Temp[1] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M7-Vbat", (long)((LLCRamRO[6].Temp[1] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M7  MOS", (long)((LLCRamRO[6].Temp[2] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M7  TRF", (long)((LLCRamRO[6].Temp[2] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M7  FLY", (long)((LLCRamRO[6].Temp[3] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M7  PFC", (long)((LLCRamRO[6].Temp[3] >> 8) - 64), 0); pos += 8;
        
        //CsvLong(c, l + pos,  "M8 Amb", (long)((LLCRamRO[7].Temp[0] >> 8) - 64), 0); pos += 8;
    	CsvLong(c, l + pos,  "M8+Vbat", (long)((LLCRamRO[7].Temp[1] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M8-Vbat", (long)((LLCRamRO[7].Temp[1] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M8  MOS", (long)((LLCRamRO[7].Temp[2] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M8  TRF", (long)((LLCRamRO[7].Temp[2] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M8  FLY", (long)((LLCRamRO[7].Temp[3] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M8  PFC", (long)((LLCRamRO[7].Temp[3] >> 8) - 64), 0); pos += 8;
    }
    else
    {
        //CsvLong(c, l + pos,  "M1 Amb", (long)((LLCRamRO[0].Temp[0] >> 8) - 64), 0); pos += 8;
    	CsvLong(c, l + pos,  "M1+Vbat", (long)((LLCRamRO[0].Temp[1] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M1-Vbat", (long)((LLCRamRO[0].Temp[1] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M1 MOSG", (long)((LLCRamRO[0].Temp[2] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M1 MOSD", (long)((LLCRamRO[0].Temp[2] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M1 DS  ", (long)((LLCRamRO[0].Temp[3] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M1 PDE ", (long)((LLCRamRO[0].Temp[3] >> 8) - 64), 0); pos += 8;
    
        //CsvLong(c, l + pos,  "M2 Amb", (long)((LLCRamRO[1].Temp[0] >> 8) - 64), 0); pos += 8;
    	CsvLong(c, l + pos,  "M2+Vbat", (long)((LLCRamRO[1].Temp[1] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M2-Vbat", (long)((LLCRamRO[1].Temp[1] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M2 MOSG", (long)((LLCRamRO[1].Temp[2] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M2 MOSD", (long)((LLCRamRO[1].Temp[2] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M2 DS  ", (long)((LLCRamRO[1].Temp[3] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M2 PDE ", (long)((LLCRamRO[1].Temp[3] >> 8) - 64), 0); pos += 8;
    
        //CsvLong(c, l + pos,  "M3 Amb", (long)((LLCRamRO[2].Temp[0] >> 8) - 64), 0); pos += 8;
    	CsvLong(c, l + pos,  "M3+Vbat", (long)((LLCRamRO[2].Temp[1] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M3-Vbat", (long)((LLCRamRO[2].Temp[1] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M3 MOSG", (long)((LLCRamRO[2].Temp[2] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M3 MOSD", (long)((LLCRamRO[2].Temp[2] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M3 DS  ", (long)((LLCRamRO[2].Temp[3] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M3 PDE ", (long)((LLCRamRO[2].Temp[3] >> 8) - 64), 0); pos += 8;
    
        //CsvLong(c, l + pos,  "M4 Amb", (long)((LLCRamRO[3].Temp[0] >> 8) - 64), 0); pos += 8;
    	CsvLong(c, l + pos,  "M4+Vbat", (long)((LLCRamRO[3].Temp[1] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M4-Vbat", (long)((LLCRamRO[3].Temp[1] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M4 MOSG", (long)((LLCRamRO[3].Temp[2] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M4 MOSD", (long)((LLCRamRO[3].Temp[2] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M4 DS  ", (long)((LLCRamRO[3].Temp[3] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M4 PDE ", (long)((LLCRamRO[3].Temp[3] >> 8) - 64), 0); pos += 8;
    
        //CsvLong(c, l + pos,  "M5 Amb", (long)((LLCRamRO[4].Temp[0] >> 8) - 64), 0); pos += 8;
    	CsvLong(c, l + pos,  "M5+Vbat", (long)((LLCRamRO[4].Temp[1] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M5-Vbat", (long)((LLCRamRO[4].Temp[1] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M5 MOSG", (long)((LLCRamRO[4].Temp[2] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M5 MOSD", (long)((LLCRamRO[4].Temp[2] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M5 DS  ", (long)((LLCRamRO[4].Temp[3] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M5 PDE ", (long)((LLCRamRO[4].Temp[3] >> 8) - 64), 0); pos += 8;
    
        //CsvLong(c, l + pos,  "M6 Amb", (long)((LLCRamRO[5].Temp[0] >> 8) - 64), 0); pos += 8;
    	CsvLong(c, l + pos,  "M6+Vbat", (long)((LLCRamRO[5].Temp[1] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M6-Vbat", (long)((LLCRamRO[5].Temp[1] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M6 MOSG", (long)((LLCRamRO[5].Temp[2] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M6 MOSD", (long)((LLCRamRO[5].Temp[2] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M6 DS  ", (long)((LLCRamRO[5].Temp[3] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M6 PDE ", (long)((LLCRamRO[5].Temp[3] >> 8) - 64), 0); pos += 8;
        
        //CsvLong(c, l + pos,  "M7 Amb", (long)((LLCRamRO[6].Temp[0] >> 8) - 64), 0); pos += 8;
    	CsvLong(c, l + pos,  "M7+Vbat", (long)((LLCRamRO[6].Temp[1] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M7-Vbat", (long)((LLCRamRO[6].Temp[1] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M7 MOSG", (long)((LLCRamRO[6].Temp[2] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M7 MOSD", (long)((LLCRamRO[6].Temp[2] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M7 DS  ", (long)((LLCRamRO[6].Temp[3] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M7 PDE ", (long)((LLCRamRO[6].Temp[3] >> 8) - 64), 0); pos += 8;
        
        //CsvLong(c, l + pos,  "M8 Amb", (long)((LLCRamRO[7].Temp[0] >> 8) - 64), 0); pos += 8;
    	CsvLong(c, l + pos,  "M8+Vbat", (long)((LLCRamRO[7].Temp[1] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M8-Vbat", (long)((LLCRamRO[7].Temp[1] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M8 MOSG", (long)((LLCRamRO[7].Temp[2] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M8 MOSD", (long)((LLCRamRO[7].Temp[2] >> 8) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M8 DS  ", (long)((LLCRamRO[7].Temp[3] & 0x00FF) - 64), 0); pos += 8;
        CsvLong(c, l + pos,  "M8 PDE ", (long)((LLCRamRO[7].Temp[3] >> 8) - 64), 0); pos += 8;
    }
	
	CsvLong(c, l + pos,  "Vloop2 ", (long)DataR.Vloop2, 0); pos += 8;
	CsvLong(c, l + pos,  "Vloop1 ", (long)DataR.Vloop1, 0); pos += 8;
	CsvLong(c, l + pos, "Del LW ", (long)DeltaLoopWidth, 0); pos += 8;
	CsvLong(c, l + pos, "Del LWM", (long)DeltaLoopWidthMax, 0); pos += 8;
	CsvLong(c, l + pos, "LoopWdt", (long)LoopWidthNew, 0);  pos += 8;
	CsvLong(c, l + pos, "Tamb   ", (long)Tambiant, 0); pos += 8;
    
    CsvLong(c, l + pos, "VLoopL ", (long)DataW.VLoopLow, 0); pos += 8;
    CsvLong(c, l + pos, "VLoopH ", (long)DataW.VLoopHigh, 0); pos += 8;
    CsvLong(c, l + pos, "TempCap", (long)TempCap, 0); pos += 8;
    
    /*CsvLong(c, l + pos,  "ADCID 1", (long)LLCRamRO[0].VMains, 0); pos += 8;  
    CsvLong(c, l + pos,  "ADCID 2", (long)LLCRamRO[1].VMains, 0); pos += 8;  
    CsvLong(c, l + pos,  "ADCID 3", (long)LLCRamRO[2].VMains, 0); pos += 8;  
    CsvLong(c, l + pos,  "ADCID 4", (long)LLCRamRO[3].VMains, 0); pos += 8;  
    CsvLong(c, l + pos,  "ADCID 5", (long)LLCRamRO[4].VMains, 0); pos += 8;  
    CsvLong(c, l + pos,  "ADCID 6", (long)LLCRamRO[5].VMains, 0); pos += 8;  
    CsvLong(c, l + pos,  "ADCID 7", (long)LLCRamRO[6].VMains, 0); pos += 8;  
    CsvLong(c, l + pos,  "ADCID 8", (long)LLCRamRO[7].VMains, 0); pos += 8;*/
        
	*n = pos;
	/* CR LF final */
	l[*n] = CR;
	*n = *n + 1;
	l[*n] = LF;
	*n = *n + 1;
}



void CsvMemoE1Line(uchar c, uchar *l, uint *n, StructE1Memo *m)
{	// MODIF R3.3
	/* Prints a Memo line in USB  */
	/* c = 0 : standard line, c = 1 : first line */
	/* l : line to be completed */
	/* n : number of char */
	/* *m : memo to record */
	uint pos = 0;

	StructDateTime dt;
	memset(l, SPACE, 240);

	if (E1Status.NbRstPointer == 0)
	{
		CsvLong(c, l +   pos, "Nr     ", E1Status.EventPointer - (long)UsbMemoPtr + 1, 0); pos += 8;
	}
	else
	{
		CsvLong(c, l +   pos, "Nr     ", SEGE1NB + SEGE1TOP - (long)UsbMemoPtr, 0); pos += 8;
	}
	// Memo Date
	dt.Year = (*m).EventDate.Year;
	dt.Year += 2000;
	dt.Month = (*m).EventDate.Month;
	dt.Date = (*m).EventDate.Date;
	dt.Hr = (*m).EventDate.Hr;
	dt.Min = (*m).EventDate.Min;
	CsvDateHour(c, l + pos, "Date", dt); pos += 20;
	CsvLong(c, l + pos, "Module ", (long)(*m).SlotNumber, 0); pos += 8;
	CsvLongHexa(c, l + pos, "ID      ", (long)(*m).Id, 0); pos += 9;
	CsvLongHexa(c, l + pos, "RO-DEF  ", (long)(*m).RamRoDef, 0); pos += 9;
	CsvLongHexa(c, l + pos, "RO-STAT ", (long)(*m).RamRoStatus, 0); pos += 9;
	CsvLong(c, l + pos, "Version", (long)(*m).Ver[0], 0); pos += 8;
	CsvLong(c, l + pos, "TimerSe", (long)(*m).TimerSec, 0); pos += 8;

	if (Menu.ChgType == TYPE_1PHASE)
	{
		CsvLong(c, l + pos,  "TP+Vbat", (long)(((*m).Temp[1] & 0x00FF) - 64), 0); pos += 8;
		CsvLong(c, l + pos,  "TP-Vbat", (long)(((*m).Temp[1] >> 8) - 64), 0); pos += 8;
	    CsvLong(c, l + pos,  "TP  MOS", (long)(((*m).Temp[2] & 0x00FF) - 64), 0); pos += 8;
	    CsvLong(c, l + pos,  "TP  TRF", (long)(((*m).Temp[2] >> 8) - 64), 0); pos += 8;
	    CsvLong(c, l + pos,  "TP  FLY", (long)(((*m).Temp[3] & 0x00FF) - 64), 0); pos += 8;
	    CsvLong(c, l + pos,  "TP  PFC", (long)(((*m).Temp[3] >> 8) - 64), 0); pos += 8;
	}
	else
	{
	   	CsvLong(c, l + pos,  "TP+Vbat", (long)(((*m).Temp[1] & 0x00FF) - 64), 0); pos += 8;
	   	CsvLong(c, l + pos,  "TP-Vbat", (long)(((*m).Temp[1] >> 8) - 64), 0); pos += 8;
	    CsvLong(c, l + pos,  "TP MOSG", (long)(((*m).Temp[2] & 0x00FF) - 64), 0); pos += 8;
	    CsvLong(c, l + pos,  "TP MOSD", (long)(((*m).Temp[2] >> 8) - 64), 0); pos += 8;
	    CsvLong(c, l + pos,  "TP DS  ", (long)(((*m).Temp[3] & 0x00FF) - 64), 0); pos += 8;
	    CsvLong(c, l + pos,  "TP PDE ", (long)(((*m).Temp[3] >> 8) - 64), 0); pos += 8;
	}

	CsvLong(c, l + pos, "VLmfb  ", (long)(*m).VLmfb, 2); pos += 8;
	CsvLong(c, l + pos, "VFuse  ", (long)(*m).VFuse, 2); pos += 8;
	CsvLong(c, l + pos, "VBat-mo", (long)(*m).VBat, 2); pos += 8;
	CsvLong(c, l + pos, "IBat-mo", (long)(*m).IBat, 1); pos += 8;
	CsvLong(c, l + pos, "VReq   ", (long)(*m).VReq, 2); pos += 8;
	CsvLong(c, l + pos, "IReq   ", (long)(*m).IReq, 1); pos += 8;
	CsvLong(c, l + pos, "Slope  ", (long)(*m).Slope, 0); pos += 8;
	CsvLong(c, l + pos, "P-Max  ", (long)(*m).PowerMax, 0); pos += 8;
	CsvLong(c, l + pos, "VBat-ch", (long)(*m).VbatCharger, 2); pos += 8;
	CsvLong(c, l + pos, "IBat-ch", (long)(*m).IbatCharger, 1); pos += 8;
	CsvLongHexa(c, l + pos, "Charger ", (long)(*m).StateCharger, 0); pos += 9;
	CsvLongHexa(c, l + pos, "Charge  ", (long)(*m).StateCharge, 0); pos += 9;
	CsvLongHexa(c, l + pos, "Phase   ", (long)(*m).StatePhase, 0); pos += 9;

	*n = pos;
	/* CR LF final */
	l[*n] = CR;
	*n = *n + 1;
	l[*n] = LF;
	*n = *n + 1;
}
