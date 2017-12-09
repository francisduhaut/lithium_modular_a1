/*
 * lcdmenu.c
 *
 *  Created on: 25 fevrier. 2013
 *      Author: duhautf
 */

#include "include.h"

#define MNUMAXLINES 8


#define DEFAULTPASSWD  	    991
#define PASSWORD2           149
#define PASSWORD3           123

#define	OPTIONSTESTTIME     30L
#define	MENU_TIMEOUT		120L

#define SPACE       		32
#define STAR        		42
#define ZERO        		48
#define TILDE       		94
#define UNDERS      		95        /* Underscore */
#define NUL         		0
#define CR          		0x0D      /* cariage return */
#define LF          		0x0A      /* Line feed */
#define BS          		0x5C      /* Back slash \ */
#define XON         		17        /* Xon : CTRL + Q */
#define XOFF        		19        /* Xoff : CTRL + S */

/* Mask */
/* Menu permitted when bit set */
/* 0xFFFF : all is permitted */
/* menu displayed if : item mask = 1 or (bit set to 0 && item mask = 0) */
#define AE        	BitMaskAutoExit       /* 1 = auto exit */
#define I         	BitMaskInit           /* 1 = autorized in Init */
#define C         	BitMaskChg            /* 1 = autorized in charge */
#define PT        	BitMaskPasswdTmp      /* 1 = autorized without temp password */
#define P0        	BitMaskPasswdHigh     /* 1 = autorized without high password */
#define P1        	BitMaskPasswdLow      /* 1 = autorized without low password */
#define P2        	BitMaskPasswd2        /* 1 = autorized without P2 password */
#define P3        	BitMaskPasswd3        /* 1 = autorized without P3 password */
#define A0        	BitMaskArea0          /* 1 = Area 0 selected */
#define A1        	BitMaskArea1          /* 1 = Area 1 selected */
#define BP        	BitMaskBP             /* 1 = Backplane */
#define LA        	BitMaskLan            /* 1 = Ethernet RJ45 network */
#define WI        	BitMaskWLan           /* 1 = Ethernet Wifi no encrypt */
#define WA        	BitMaskWAsc           /* 1 = Ethernet Wifi Ascii encryption */
#define WH        	BitMaskWHex           /* 1 = Ethernet Wifi Hexa encryption */
#define U         	BitMaskUsb            /* 1 = Usb */
#define JB          BitMaskJbus           /* 1 = JBus network */
#define XF          BitMaskXFC            /* 1 = Force XFC profile */
#define NX          BitMaskNOXFC          /* 1 = No display XFC */
// MODIF R2.1
#define US          BitMask480VAC         /* 1 = show menu 400/480VAC */
// MODIF 2.8
#define M1          BitMask1PH            /* 1 = 1 Phase charger */

/* ********** List for different screens ********** */

#define MNUMNU      		0         /* Menu with sub menu */
#define MNULIST     		1         /* Menu with choices */
#define MNUINFO     		2         /* Info */
#define MNUVAL      		4         /* Value, no more sub menu */
#define MNUNUM      		5         /* Input numeric values without continuous update */
#define MNUBIT      		6         /* Input bit values */
#define MNUFUNLIST  		7         /* List of function list (no current choice) */
#define MNUFUNSEL   		8         /* Selection in function list (current choice) */
#define MNUFUNVAL   		9         /* List of function values (no current choice) */
#define MNUNUMUPD   		10        /* Input numeric values with continuous update */
#define MNUHEXA     		11        /* Input hexa values without continuous update */
#define MNUHEXAUPD  		12        /* Input hexa values with continuous update */
#define MNUALPHA0   		13        /* Input alphanumeric values + underscore without continuous update */
#define MNUGRAPH    		14        /* Graphe */
#define MNUSTATUS           15
#define MNUOPTION           16
#define MNUEXIT             17
#define MNUPSW              18
#define MNUBOOTAPP		    19

/* I2C LCD  in i2c.c */
extern ulong	MnuTimer;

static BYTE DisplayUpdate = 0;

uint8_t MnuValCorrection = 0;

extern uint8_t pos, prevpos;
extern uint8_t FirstEntry;
extern uint8_t EntryEvent;

extern uint8_t ReDraw;
extern uint8_t ReDraw2;

extern ulong next;

extern uint MnuListCur[6];          	/* Current value in list */
extern uint MnuListOff[6];          	/* Offset of the list (case long list) (uint for memo) */
extern uint MnuValidNb;             	/* Number of valid menus */

extern uchar MnuListLevel;
extern uchar MnuLevel;               	/* Level of current display (0 : no menu) */
extern uchar Mnu[7];                 	/* 1 more, when enter, set next level to 0 */

/* Global vars saved for each chrono */
extern uchar MnuNumVal[16];          	/* 16 variable characters */
extern uchar MnuNumIndex;           	/* Index for digit modification (0 to 15) */
/* Selected menu in list menu */
extern uint MnuNumMask;             	/* Current value of the 16 bits */
extern uint MnuBitVal;              	/* Working segment for edition */

extern uint MnuMemoVal;             	/* Value of the current displayed memo ( 0 to ...) */
extern uint	MemoPtr;					/* Pointer for read memo */

extern uchar LcdTemp[128];
extern ulong TimerMnuOption;            /* Timer for Mnu options (Pump, ev, voy) */
extern uchar FlagSaveSerial;

extern void usb_ProfileClose (void);
extern ulong VncCfgPtrSave;
extern ulong VncCfgPtrOpen;

typedef struct
{
	uchar Mnu[6];
	ulong Mask;
	uchar Text[NBLANG][17];       	        /* 16 uchar + \0 */
	uchar Type;
	uchar Val;
	void  (*f)(uint c, uchar *v);
	const IMAGE_EXTERNAL *NormalBig ;
	const IMAGE_EXTERNAL *SelectedBig;
	const IMAGE_EXTERNAL *NormalSmall ;
	const IMAGE_EXTERNAL *SelectedSmall;

} StructMnu;


/* MNUVAL must follow MNULIST */
const StructMnu MnuDef[] = {
		{{0, 0, 0, 0, 0, 0}, AE | I | C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Menu principal  ", "Main menu       ", "Hauptmenu       ", "Menu principale ", "Menu principale ", MNUMNU, 0, 		&GetSetMnuList,0,0,0,0},
		{{0, 1, 0, 0, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Histo.", "Logs", "Verlauf", "Storico", "Histo.", MNUMNU, 0, 	&GetSetMnuList,&logs_icon,&logs_icon_select,&icon_logs,0},
		{{0, 1, 1, 0, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Memo			", "Memo			", "Memo			", "Memo			", "Memo			", MNUFUNLIST, 0, 	&GetSetMemoList,0,0,0,0,},
		{{0, 1, 1, 1, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Memo            ", "Memo            ", "Speicher        ", "Memo            ", "Memo            ", MNUFUNVAL, 0, 	&GetSetMemo,0,0,0,0},
		{{0, 1, 2, 0, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Status			", "Status			", "Status			", "Satoto			", "Estados			", MNUSTATUS, 0, 	&GetSetStatus,0,0,0,0},
		{{0, 2, 0, 0, 0, 0}, AE | I | C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Config.", "Settings", "Konfig. ", "Configur.", "Configur.",MNUMNU, 0, 		  &GetSetMnuList,&settings_icon,&settings_icon_select,&icon_settings,0},
		{{0, 2, 1, 0, 0, 0}, AE | I |     PT | M1 |  US | NX | XF | P3 | P2 |  P0      | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Charge          ", "Charge          ", "Ladung          ", "Carica          ", "Carga           ", MNUMNU, 0, 		  &GetSetMnuList,0,0,0,0},
		{{0, 2, 1, 1, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0      | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Profil          ", "Profile         ", "Kennlinie       ", "Profilo         ", "Perfil          ", MNULIST, 0, 		&GetSetMnuList,0,0,0,0},

#ifdef  ENERSYS_US
		{{0, 2, 1, 1, 1, 0}, AE |         PT | M1 |  US | NX      | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "IONIC           ", "IONIC           ", "IEM             ", "IONIC           ", "IONIC           ", MNUVAL, IONIC, 	&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 2, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "GELBLC          ", "GELBLC          ", "GELBLC          ", "GELBLC          ", "GELBLC          ", MNUVAL, GEL, 		&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 3, 0}, AE |         PT | M1 |  US | NX      | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "VRLA            ", "VRLA            ", "VRLA            ", "VRLA            ", "VRLA            ", MNUVAL, VRLA,		&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 4, 0}, AE |         PT | M1 |  US | NX      | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "OPP             ", "OPP             ", "OPP             ", "OPP             ", "OPP             ", MNUVAL, OPP, 		&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 5, 0}, AE |         PT | M1 |  US | NX      | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "FROID           ", "COLD            ", "COLD            ", "COLD            ", "COLD            ", MNUVAL, COLD, 	  	&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 6, 0}, AE |         PT | M1 |  US |      XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "NXSTND          ", "NXSTND          ", "NXSTND          ", "NXSTND          ", "NXSTND          ", MNUVAL, XFC, 		&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 7, 0}, AE |         PT | M1 |  US |      XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "NXFAST          ", "NXFAST          ", "NXFAST          ", "NXFAST          ", "NXFAST          ", MNUVAL, TPPL,		&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 8, 0}, AE |         PT | M1 |  US |      XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "NXBLOC          ", "NXBLOC          ", "NXBLOC          ", "NXBLOC          ", "NXBLOC          ", MNUVAL, NXBLC,		&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 9, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "LITHIUM         ", "LITHIUM         ", "LITHIUM         ", "LITHIUM         ", "LITHIUM         ", MNUVAL, LITH,		&GetSetProfil,0,0,0,0},
#endif

#ifdef  HAWKER_US
		{{0, 2, 1, 1, 1, 0}, AE |         PT | M1 |  US | NX      | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "IONIC           ", "IONIC           ", "IONIC           ", "IONIC           ", "IONIC           ", MNUVAL, IONIC, 	&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 2, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "EVLTN           ", "EVLTN           ", "EVLTN           ", "EVLTN           ", "EVLTN           ", MNUVAL, GEL, 		&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 3, 0}, AE |         PT | M1 |  US | NX      | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "ENVLNK          ", "ENVLNK          ", "ENVLNK          ", "ENVLK           ", "ENVLK           ", MNUVAL, VRLA,		&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 4, 0}, AE |         PT | M1 |  US | NX      | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "OPP             ", "OPP             ", "OPP             ", "OPP             ", "OPP             ", MNUVAL, OPP, 		&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 5, 0}, AE |         PT | M1 |  US | NX      | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "FROID           ", "COLD            ", "KALT            ", "FREDDO          ", "FRIO            ", MNUVAL, COLD, 	  	&GetSetProfil,0,0,0,0},
        {{0, 2, 1, 1, 6, 0}, AE |         PT | M1 |  US |      XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "FXSTND          ", "FXSTND          ", "FXSTND          ", "FXSTND          ", "FXSTND          ", MNUVAL, XFC, 		&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 7, 0}, AE |         PT | M1 |  US |      XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "FXFAST          ", "FXFAST          ", "FXFAST          ", "FXFAST          ", "FXFAST          ", MNUVAL, TPPL,		&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 8, 0}, AE |         PT | M1 |  US |      XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "FXBLOC          ", "FXBLOC          ", "FXBLOC          ", "FXBLOC          ", "FXBLOC          ", MNUVAL, NXBLC,		&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 9, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "LITHIUM         ", "LITHIUM         ", "LITHIUM         ", "LITHIUM         ", "LITHIUM         ", MNUVAL, LITH,		&GetSetProfil,0,0,0,0},
#endif

#ifdef  ENERSYS_EU
		{{0, 2, 1, 1, 1, 0}, AE |         PT | M1 |  US | NX      | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "HDUTY           ", "HDUTY           ", "HDUTY           ", "HDUTY           ", "HDUTY           ", MNUVAL, IONIC, 	&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 2, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "GEL             ", "GEL             ", "GEL             ", "GEL             ", "GEL             ", MNUVAL, GEL, 		&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 3, 0}, AE |         PT | M1 |  US | NX      | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "RGT             ", "AGM             ", "AGM             ", "AGM             ", "AGM             ", MNUVAL, RGT, 		&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 4, 0}, AE |         PT | M1 |  US | NX      | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "STDWL           ", "STDWL           ", "STDWL           ", "STDWL           ", "STDWL           ", MNUVAL, PZM, 		&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 5, 0}, AE |         PT | M1 |  US | NX      | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "OPP             ", "OPP             ", "OPP             ", "OPP             ", "OPP             ", MNUVAL, OPP, 		&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 6, 0}, AE |         PT | M1 |  US | NX      | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "PNEU            ", "PNEU            ", "PNEU            ", "PNEU            ", "PNEU            ", MNUVAL, PNEU, 	    &GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 7, 0}, AE |         PT | M1 |  US |      XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "NXSTND          ", "NXSTND          ", "NXSTND          ", "NXSTND          ", "NXSTND          ", MNUVAL, XFC, 		&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 8, 0}, AE |         PT | M1 |  US |      XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "NXFAST          ", "NXFAST          ", "NXFAST          ", "NXFAST          ", "NXFAST          ", MNUVAL, TPPL,		&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1, 9, 0}, AE |         PT | M1 |  US |      XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "NXBLOC          ", "NXBLOC          ", "NXBLOC          ", "NXBLOC          ", "NXBLOC          ", MNUVAL, NXBLC,		&GetSetProfil,0,0,0,0},
        {{0, 2, 1, 1,10, 0}, AE |         PT | M1 |  US | NX      | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "WL20            ", "WL20            ", "WL20            ", "WL20            ", "WL20            ", MNUVAL, WF200, 	&GetSetProfil,0,0,0,0},
        {{0, 2, 1, 1,11, 0}, AE |         PT | M1 |  US | NX      | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "LOWCHG          ", "LOWCHG          ", "LOWCHG          ", "LOWCHG          ", "LOWCHG          ", MNUVAL, LOWCHG,	&GetSetProfil,0,0,0,0},
		{{0, 2, 1, 1,12, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "LITHIUM         ", "LITHIUM         ", "LITHIUM         ", "LITHIUM         ", "LITHIUM         ", MNUVAL, LITH,		&GetSetProfil,0,0,0,0},
#endif

		{{0, 2, 1, 2, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "AutoStart       ", "AutoStart       ", "AutoStart       ", "AutoStart       ", "Inicio automat. ", MNULIST, 0, 	&GetSetMnuList,0,0,0,0},
		{{0, 2, 1, 2, 1, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "On              ", "On              ", "Ein             ", "On              ", "On              ", MNUVAL, 1, 	&GetSetAutoStartOffOn,0,0,0,0},
		{{0, 2, 1, 2, 2, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Off             ", "Off             ", "Aus             ", "Off             ", "Off             ", MNUVAL, 0, 	&GetSetAutoStartOffOn,0,0,0,0},
		{{0, 2, 1, 3, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Depart differe  ", "Charge delay    ", "Ladeverzogerung ", "Partenza ritard ", "Retardo carga   ", MNUMNU, 0, 	&GetSetMnuList,0,0,0,0},
		{{0, 2, 1, 3, 1, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Type            ", "Type            ", "Typ             ", "Tipo            ", "Typo            ", MNULIST, 0, 	&GetSetMnuList,0,0,0,0},
		{{0, 2, 1, 3, 1, 1}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Retard          ", "Delay           ", "Ladeverzogerung ", "Ritardata       ", "Retardo         ", MNUVAL, 0, 	&GetSetChargeTimeType,0,0,0,0},
		{{0, 2, 1, 3, 1, 2}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Heure fixe      ", "Time of day     ", "Zeit des Tages  ", "Ora             ", "Hora del dia    ", MNUVAL, 1, 	&GetSetChargeTimeType,0,0,0,0},
		{{0, 2, 1, 3, 1, 3}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Off             ", "Off             ", "Aus             ", "Off             ", "Off             ", MNUVAL, 2, 	&GetSetChargeTimeType,0,0,0,0},
		{{0, 2, 1, 3, 2, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Val heure/delais", "Value hour/delay", "Wert Zeit/Verzog", "Valore/h rit    ", "Hora / retraso  ", MNUNUM, 0, 	&GetSetChargeTime,0,0,0,0},

		//#ifndef  ENERSYS_EU
		{{0, 2, 1, 4, 0, 0}, AE |         PT | M1 |  US | NX |      P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Daily Charge    ", "Daily Charge    ", "Taglicheladung  ", "Carga Diaria    ", "Carga Diaria    ", MNUMNU,  0,   	&GetSetMnuList,0,0,0,0},
		{{0, 2, 1, 4, 1, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "ON - OFF        ", "ON - OFF        ", "EIN - AUS       ", "Carga Diaria E/A", "ON - OFF        ", MNULIST, 0,    &GetSetMnuList,0,0,0,0},
		{{0, 2, 1, 4, 1, 1}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "On              ", "On              ", "Ein             ", "Encender        ", "On              ", MNUVAL,  0,    &GetSetDailyChargeTimeType,0,0,0,0},
		{{0, 2, 1, 4, 1, 2}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Off             ", "Off             ", "Aus             ", "Apagar          ", "Off             ", MNUVAL,  1,    &GetSetDailyChargeTimeType,0,0,0,0},
		{{0, 2, 1, 4, 2, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Daily Chg Start ", "Daily Chg Start ", "Lad. Start      ", "Carga Dia Inicar", "Diaria Chg Start", MNUNUM,  0,    &GetSetDailyChargeStartTime,0,0,0,0},
		{{0, 2, 1, 4, 3, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Daily Chg End   ", "Daily Chg End   ", "Lad. End        ", "Carga Dia Final ", "Diaria Chg End  ", MNUNUM,  0,    &GetSetDailyChargeEndTime,0,0,0,0},
		//#endif

		{{0, 2, 1, 5, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Block Out Charge", "Block Out Charge", "Ladeabbruch     ", "Block Out Charge", "Bloqueo Carga   ", MNUMNU,  0,	&GetSetMnuList,0,0,0,0},
		{{0, 2, 1, 5, 1, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "ON - OFF        ", "ON - OFF        ", "EIN - AUS       ", "ON - OFF        ", "ON - OFF        ", MNULIST, 0,    &GetSetMnuList,0,0,0,0},
		{{0, 2, 1, 5, 1, 1}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "On              ", "On              ", "Ein             ", "On              ", "On              ", MNUVAL,  1,    &GetSetBlockOutChargeType,0,0,0,0},
		{{0, 2, 1, 5, 1, 2}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Off             ", "Off             ", "Aus             ", "Off             ", "Off             ", MNUVAL,  0,    &GetSetBlockOutChargeType,0,0,0,0},
		{{0, 2, 1, 5, 2, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Block Out Start ", "Block Out Start ", "Ladeabbr. Start ", "Block Out Start ", "Block Out Start ", MNUNUM,  0,    &GetSetBlockOutStartTime,0,0,0,0},
		{{0, 2, 1, 5, 3, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Block Out End   ", "Block Out End   ", "Ladeabbr. End   ", "Block Out End   ", "Block Out End   ", MNUNUM,  0,    &GetSetBlockOutEndTime,0,0,0,0},

		{{0, 2, 1, 6, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Floating-Load   ", "Floating-Load   ", "Ladeer-Load     ", "Flotante-Load   ", "Flotacion-Load  ", MNUMNU, 0, 	&GetSetMnuList,0,0,0,0},
		{{0, 2, 1, 6, 1, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Fl.-Load On/Off ", "Fl.-Load On/Off ", "Fl.-Load Ein/Aus", "Fl.-Load On/Off ", "Fl.-Load On/Off ", MNULIST, 0, 	&GetSetMnuList,0,0,0,0},
		{{0, 2, 1, 6, 1, 1}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "On              ", "On              ", "Ein             ", "On              ", "On              ", MNUVAL, ON, 	&GetSetFloatingOffOn,0,0,0,0},
		{{0, 2, 1, 6, 1, 2}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Off             ", "Off             ", "Aus             ", "Off             ", "Off             ", MNUVAL, OFF, 	&GetSetFloatingOffOn,0,0,0,0},
		{{0, 2, 1, 6, 2, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Courant         ", "Current         ", "Strom           ", "Corrente        ", "Corriente       ", MNUNUM, 0, 	&GetSetIfloating,0,0,0,0},
		{{0, 2, 1, 6, 3, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Tension         ", "Voltage         ", "Spannung        ", "Tensione        ", "Tension         ", MNUNUM, 0, 	&GetSetVfloating,0,0,0,0},

		{{0, 2, 1, 7, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Chg conditionnel", "Condition charge", "Bedingungslad.  ", "Carica condizion", "Carga condicion.", MNUNUM, 0, 	&GetSetInitDod,0,0,0,0},
		{{0, 2, 1, 8, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Repos Batterie  ", "Battery Rest    ", "Batterie Rest   ", "Riposo Batteria ", "Reposo Bateria  ", MNUNUM, 0,		&GetSetBatteryRest,0,0,0,0},


		//  {{0, 2, 1, 7, 5, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |       P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "CEC             ", "CEC             ", "CEC             ", "CEC             ", MNULIST, 0,      &GetSetMnuList,0,0,0,0},
		//  {{0, 2, 1, 7, 5, 1}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |       P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "On              ", "On              ", "On              ", "Encender        ", MNUVAL,  1,      &GetSetCECOnOff,0,0,0,0},
		//  {{0, 2, 1, 7, 5, 2}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |       P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Off             ", "Off             ", "Off             ", "Apagar          ", MNUVAL,  0,      &GetSetCECOnOff,0,0,0,0},
		{{0, 2, 1, 9, 0, 0}, AE |         PT | M1 |  US | NX |      P3 |       P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "COLD Pulse      ", "COLD Pulse      ", "KALT Puls       ", "COLD Pulse      ", "COLD Pulse      ", MNULIST, 0, 	&GetSetMnuList,0,0,0,0},
		{{0, 2, 1, 9, 1, 0}, AE |         PT | M1 |  US | NX | XF | P3 |       P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "On              ", "On              ", "Ein             ", "Encender        ", "On              ", MNUVAL, ON, 	&GetSetLoopOffOn,0,0,0,0},
		{{0, 2, 1, 9, 2, 0}, AE |         PT | M1 |  US | NX | XF | P3 |       P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Off             ", "Off             ", "Aus             ", "Apagar          ", "Off             ", MNUVAL, OFF, 	&GetSetLoopOffOn,0,0,0,0},
#ifdef  ENERSYS_US
		{{0, 2, 1,10, 0, 0}, AE |         PT | M1 |  US | NX |      P3 |       P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "VRLA Iphase3    ", "VRLA Iphase3    ", "VRLA Iphase3    ", "VRLA Iphase3    ", "VRLA Iphase3    ", MNUNUM, 0,     &GetSetGelIphase3,0,0,0,0},
#endif
#ifdef  HAWKER_US
		{{0, 2, 1,10, 0, 0}, AE |         PT | M1 |  US | NX |      P3 |       P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "ENVLNK Iphase 3 ", "ENVLNK Iphase 3 ", "ENVLNK Iphase 3 ", "ENVLNK Iphase 3 ", "ENVLNK Iphase 3 ", MNUNUM, 0,     &GetSetGelIphase3,0,0,0,0},
#endif

		{{0, 2, 1,11, 0, 0}, AE |         PT | M1 |  US | NX |      P3 |       P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "CF Flooded      ", "CF Flooded      ", "CF Flooded      ", "CF Flooded      ", "CF Flooded      ", MNUNUM, 0,     &GetSetIonicCoef,0,0,0,0},

		{{0, 2, 1,12, 0, 0}, AE |         PT | M1 |  US | NX |      P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Refresh On/Off  ", "Refresh On/Off  ", "Refresh Ein/Aus ", "Refresh On/Off  ", "Refresco On/Off ", MNULIST, 0,  	&GetSetMnuList,0,0,0,0},
		{{0, 2, 1,12, 1, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "On              ", "On              ", "Ein             ", "On              ", "On              ", MNUVAL, ON,    &GetSetRefreshOffOn,0,0,0,0},
		{{0, 2, 1,12, 2, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Off             ", "Off             ", "Aus             ", "Off             ", "Off             ", MNUVAL, OFF,   &GetSetRefreshOffOn,0,0,0,0},

        // MODIF R2.2
		{{0, 2, 1,13, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "I max           ", "I max           ", "Max. Strom      ", "I max           ", "I max           ", MNUNUM, 0, 	&GetSetImax,0,0,0,0},

        // MODIF 2.8
#ifndef  HAWKER_US
        // PASSWORD 0991 
        {{0, 2, 1,14, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |       P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "NexSys  On/Off  ", "NexSys  On/Off  ", "NexSys  On/Off  ", "NexSys  On/Off  ", "NexSys  On/Off  ", MNULIST, 0,  	&GetSetMnuList,0,0,0,0},
		{{0, 2, 1,14, 1, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |       P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "On              ", "On              ", "Ein             ", "On              ", "On              ", MNUVAL, ON,    &GetSetXFCOffOn,0,0,0,0},
		{{0, 2, 1,14, 2, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |       P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Off             ", "Off             ", "Aus             ", "Off             ", "Off             ", MNUVAL, OFF,   &GetSetXFCOffOn,0,0,0,0},
#endif  
#ifdef  HAWKER_US
        // PASSWORD 0991
        {{0, 2, 1,14, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 |            P0 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Flex    On/Off  ", "Flex  On/Off    ", "Flex  On/Off    ", "Flex  On/Off    ", "Flex  On/Off    ", MNULIST, 0,  	&GetSetMnuList,0,0,0,0},
		{{0, 2, 1,14, 1, 0}, AE |         PT | M1 |  US | NX | XF | P3 |            P0 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "On              ", "On              ", "Ein             ", "On              ", "On              ", MNUVAL, ON,    &GetSetXFCOffOn,0,0,0,0},
		{{0, 2, 1,14, 2, 0}, AE |         PT | M1 |  US | NX | XF | P3 |            P0 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Off             ", "Off             ", "Aus             ", "Off             ", "Off             ", MNUVAL, OFF,   &GetSetXFCOffOn,0,0,0,0},
#endif
        // END
		{{0, 2, 2, 0, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Batterie        ", "Battery         ", "Batterie        ", "Batteria        ", "Bateria         ", MNUMNU,  0, 	&GetSetMnuList,0,0,0,0},
		{{0, 2, 2, 1, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Nb Elements     ", "Nb Cells        ", "Zellenzahl      ", "Num celle       ", "Nb Elementos    ", MNULIST, 0, 	&GetSetMnuList,0,0,0,0},
		{{0, 2, 2, 1, 1, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 |      JB | LA | WI | WA | WH | U | 0x001F, "06              ", "06              ", "06              ", "06              ", "06              ", MNUVAL,  6,    &GetSetNbCell,0,0,0,0},
		{{0, 2, 2, 1, 2, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "12              ", "12              ", "12              ", "12              ", "12              ", MNUVAL, 12, 	&GetSetNbCell,0,0,0,0},
		{{0, 2, 2, 1, 3, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "18              ", "18              ", "18              ", "18              ", "18              ", MNUVAL, 18, 	&GetSetNbCell,0,0,0,0},
		{{0, 2, 2, 1, 4, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "24              ", "24              ", "24              ", "24              ", "24              ", MNUVAL, 24, 	&GetSetNbCell,0,0,0,0},
		{{0, 2, 2, 1, 5, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "30              ", "30              ", "30              ", "30              ", "30              ", MNUVAL, 30, 	&GetSetNbCell,0,0,0,0},
        {{0, 2, 2, 1, 6, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "32              ", "32              ", "32              ", "32              ", "32              ", MNUVAL, 32, 	&GetSetNbCell,0,0,0,0},
        {{0, 2, 2, 1, 7, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "36              ", "36              ", "36              ", "36              ", "36              ", MNUVAL, 36, 	&GetSetNbCell,0,0,0,0},
		{{0, 2, 2, 1, 8, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "40              ", "40              ", "40              ", "40              ", "40              ", MNUVAL, 40, 	&GetSetNbCell,0,0,0,0},
#ifdef SAVOYE
		{{0, 2, 2, 1, 9, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "48              ", "48              ", "48              ", "48              ", "48              ", MNUVAL, 48, 	&GetSetNbCell,0,0,0,0},
#endif
		{{0, 2, 2, 1,10, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "AUTO            ", "AUTO            ", "AUTO            ", "AUTO            ", "AUTO            ", MNUVAL,255,	&GetSetNbCell,0,0,0,0},
		{{0, 2, 2, 1,11, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "06/LITH 21,6V   ", "06/LITH 21,6V   ", "06/LITH 21,6V   ", "06/LITH 21,6V   ", "06/LITH 21,6V   ", MNUVAL,6,		&GetSetNbCell,0,0,0,0},
		{{0, 2, 2, 1,12, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "07/LITH 25,2V   ", "07/LITH 25,2V   ", "07/LITH 25,2V   ", "07/LITH 25,2V   ", "07/LITH 25,2V   ", MNUVAL,7,		&GetSetNbCell,0,0,0,0},
		{{0, 2, 2, 1,13, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "10/LITH 36,0V   ", "10/LITH 36,0V   ", "10/LITH 36,0V   ", "10/LITH 36,0V   ", "10/LITH 36,0V   ", MNUVAL,10,		&GetSetNbCell,0,0,0,0},
		{{0, 2, 2, 1,14, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "14/LITH 50,4V   ", "14/LITH 50,4V   ", "14/LITH 50,4V   ", "14/LITH 50,4V   ", "14/LITH 50,4V   ", MNUVAL,14,		&GetSetNbCell,0,0,0,0},
		{{0, 2, 2, 1,15, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "20/LITH 72,0V   ", "20/LITH 72,0V   ", "20/LITH 72,0V   ", "20/LITH 72,0V   ", "20/LITH 72,0V   ", MNUVAL,20,		&GetSetNbCell,0,0,0,0},
		{{0, 2, 2, 1,16, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "24/LITH 86,4V   ", "24/LITH 86,4V   ", "24/LITH 86,4V   ", "24/LITH 86,4V   ", "24/LITH 86,4V   ", MNUVAL,24,		&GetSetNbCell,0,0,0,0},

		{{0, 2, 2, 2, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Cap Manu Auto   ", "Cap Manu Auto   ", "Kap Manu Auto   ", "Cap Manu Auto   ", "Cap Manu Auto   ", MNULIST, 0, 	&GetSetMnuList,0,0,0,0},
		{{0, 2, 2, 2, 1, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Auto            ", "Auto            ", "Automatisch     ", "Auto            ", "Auto            ", MNUVAL, 0, 	&GetSetBatCapAutoManu,0,0,0,0},
		{{0, 2, 2, 2, 2, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Manu            ", "Manu            ", "Manuell         ", "Manu            ", "Manu            ", MNUVAL, 1, 	&GetSetBatCapAutoManu,0,0,0,0},
		{{0, 2, 2, 3, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Capacite        ", "Capacity        ", "Kapazitat       ", "Capacita        ", "Capacidad       ", MNUNUM, 0, 	&GetSetBatCapacity,0,0,0,0},
		{{0, 2, 2, 4, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Temp batterie   ", "Battery temp    ", "Batterietemp.   ", "Temp batteria   ", "Temp bateria    ", MNUNUM, 0, 	&GetSetTempChg,0,0,0,0},
		{{0, 2, 2, 5, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 |      BP | JB | LA | WI | WA | WH | U | 0x001F, "Temp haute      ", "High temp       ", "Hochtemp.       ", "Temperatura alta", "Altas temp      ", MNUNUM, 0,     &GetSetTempHighC,0,0,0,0},
		{{0, 2, 2, 6, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |           A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Temp haute      ", "High temp       ", "Hochtemp.       ", "Temperatura alta", "Altas temp      ", MNUNUM, 0,     &GetSetTempHighF,0,0,0,0},

//		{{0, 2, 2, 7,  0, 0},AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Fil Pilote      ", "LM/EB           ", "Pilotkontakt    ", "LM/EB           ", "LM/EB           ", MNULIST, 0,      &GetSetMnuList,0,0,0,0},
//		{{0, 2, 2, 7,  1, 0},AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "On              ", "On              ", "Ein             ", "On              ", "On              ", MNUVAL, ON,      &GetSetFilPilOnOff,0,0,0,0},
//		{{0, 2, 2, 7,  2, 0},AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Off             ", "Off             ", "Aus             ", "Off             ", "Off             ", MNUVAL, OFF,     &GetSetFilPilOnOff,0,0,0,0},

		{{0, 2, 3, 0, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Cable           ", "Cable           ", "Kabel           ", "Cavo            ", "Cable           ", MNUMNU, 0,		&GetSetMnuList,0,0,0,0},
		{{0, 2, 3, 1, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Longueur        ", "Length          ", "Lange           ", "Lunghezza       ", "Longitud        ", MNUNUM, 0, 	&GetSetLength,0,0,0,0},
		{{0, 2, 3, 2, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Section         ", "Section         ", "Querschnitt     ", "Sezione         ", "Seccion         ", MNULIST, 0, 	&GetSetMnuList,0,0,0,0},
		{{0, 2, 3, 2, 1, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "06 mm2 - 10 AWG ", "06 mm2 - 10 AWG ", "06 mm2 - 10 AWG ", "06 mm2 - 10 AWG ", "06 mm2 - 10 AWG ", MNUVAL, 6, 	&GetSetSect,0,0,0,0},
		{{0, 2, 3, 2, 2, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "10 mm2 - 08 AWG ", "10 mm2 - 08 AWG ", "10 mm2 - 08 AWG ", "10 mm2 - 08 AWG ", "10 mm2 - 08 AWG ", MNUVAL, 10, 	&GetSetSect,0,0,0,0},
		{{0, 2, 3, 2, 3, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "16 mm2 - 06 AWG ", "16 mm2 - 06 AWG ", "16 mm2 - 06 AWG ", "16 mm2 - 06 AWG ", "16 mm2 - 06 AWG ", MNUVAL, 16, 	&GetSetSect,0,0,0,0},
		{{0, 2, 3, 2, 4, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "25 mm2 - 04 AWG ", "25 mm2 - 04 AWG ", "25 mm2 - 04 AWG ", "25 mm2 - 04 AWG ", "25 mm2 - 04 AWG ", MNUVAL, 25, 	&GetSetSect,0,0,0,0},
		{{0, 2, 3, 2, 5, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "35 mm2 - 02 AWG ", "35 mm2 - 02 AWG ", "35 mm2 - 02 AWG ", "35 mm2 - 02 AWG ", "35 mm2 - 02 AWG ", MNUVAL, 35, 	&GetSetSect,0,0,0,0},
		{{0, 2, 3, 2, 6, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "50 mm2 - 1/0AWG ", "50 mm2 - 1/0AWG ", "50 mm2 - 1/0AWG ", "50 mm2 - 1/0AWG ", "50 mm2 - 1/0AWG ", MNUVAL, 50, 	&GetSetSect,0,0,0,0},
		{{0, 2, 3, 2, 7, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "70 mm2 - 2/0AWG ", "70 mm2 - 2/0AWG ", "70 mm2 - 2/0AWG ", "70 mm2 - 2/0AWG ", "70 mm2 - 2/0AWG ", MNUVAL, 70, 	&GetSetSect,0,0,0,0},
        {{0, 2, 3, 2, 8, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "95 mm2 - 3/0AWG ", "95 mm2 - 3/0AWG ", "95 mm2 - 3/0AWG ", "95 mm2 - 3/0AWG ", "95 mm2 - 3/0AWG ", MNUVAL, 95, 	&GetSetSect,0,0,0,0},

#ifdef DEF8BAY                 
        // PASSWORD 0991 : only on 3 phases (no mask M1)
		{{0, 2, 3, 3, 0, 0}, AE |         PT |        US | NX | XF | P3 | P2 |      P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Type de Sortie  ", "Output Type     ", "Output Type     ", "Output Type     ", "Output Type     ", MNULIST, 0,   	&GetSetMnuList,0,0,0,0},
		{{0, 2, 3, 3, 1, 0}, AE |         PT |        US | NX | XF | P3 | P2 |      P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "1 sortie        ", "1 output        ", "1 output        ", "1 output        ", "1 output        ", MNUVAL, OUTPUT_1_CABLE,     &GetSetNbMod,0,0,0,0},
        {{0, 2, 3, 3, 2, 0}, AE |         PT |        US | NX | XF | P3 | P2 |      P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "2 sorties 6 Mod ", "2 outputs 6 Mod ", "2 outputs 6 Mod ", "2 outputs 6 Mod ", "2 outputs 6 Mod ", MNUVAL, OUTPUT_2_CABLE_6M,  &GetSetNbMod,0,0,0,0},        
        {{0, 2, 3, 3, 3, 0}, AE |         PT |        US | NX | XF | P3 | P2 |      P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "2 sorties 8 Mod ", "2 outputs 8 Mod ", "2 outputs 8 Mod ", "2 outputs 8 Mod ", "2 outputs 8 Mod ", MNUVAL, OUTPUT_2_CABLE_8M,  &GetSetNbMod,0,0,0,0},        
#endif

		{{0, 2, 4, 0, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Egalisation     ", "Equalization    ", "Ausgleichslad.  ", "Equalizzazione  ", "Igualacion      ", MNUMNU, 0, 	&GetSetMnuList,0,0,0,0},
		{{0, 2, 4, 1, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Courant         ", "Current         ", "Strom           ", "Corrente        ", "Corriente       ", MNUNUM, 0, 	&GetSetIequal,0,0,0,0},
#ifdef  ENERSYS_US
		{{0, 2, 4, 2, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Temps Ph1       ", "Ph1 duration    ", "Zeit Ph1        ", "Tempo  Ph1      ", "Tiempo  Ph1     ", MNUNUM, 0, 	&GetSetEqualTimePh1,0,0,0,0},
		{{0, 2, 4, 3, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Temps Ph2       ", "Ph2 duration    ", "Zeit Ph2        ", "Tempo  Ph2      ", "Tiempo  Ph2     ", MNUNUM, 0, 	&GetSetEqualTimePh2,0,0,0,0},
#else
		{{0, 2, 4, 2, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Temps           ", "Time            ", "Zeit            ", "Tempo           ", "Tiempo          ", MNUNUM, 0, 	&GetSetEqualTime,0,0,0,0},
#endif
		{{0, 2, 4, 4, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Depart differe  ", "Delay           ", "Verzogerung     ", "Ora del giorno  ", "Retraso         ", MNUNUM, 0,		&GetSetEqualDelay,0,0,0,0},
		{{0, 2, 4, 5, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Periodicite     ", "Frequency       ", "Frequenz        ", "Frequenza       ", "Frecuencia      ", MNUBIT, 0,     &GetSetPeriod,0,0,0,0},

		{{0, 2, 6, 0, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Idle  On/Off    ", "Idle  On/Off    ", "Idle  Ein/Aus   ", "Idle  On/Off    ", "Idle  On/Off    ", MNULIST, 0,    &GetSetMnuList,0,0,0,0},
		{{0, 2, 6, 1, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "On              ", "On              ", "Ein             ", "On              ", "On              ", MNUVAL, ON,    &GetSetIdleOffOn,0,0,0,0},
		{{0, 2, 6, 2, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Off             ", "Off             ", "Aus             ", "Off             ", "Off             ", MNUVAL, OFF,   &GetSetIdleOffOn,0,0,0,0},
		
#ifdef  ENERSYS_EU
        // PASSWORD 0991 : only in 3 phases (no mask BP)
        {{0, 2, 7, 0, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |       P1 | A0 | A1 |      JB | LA | WI | WA | WH | U | 0x001F, "Alimentation    ", "Power Supply    ", "Netzspannung    ", "Alimentazione   ", "Fuente de Alim. ", MNULIST,  0, 	&GetSetMnuList,0,0,0,0},
        {{0, 2, 7, 1, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |       P1 | A0 | A1 |      JB | LA | WI | WA | WH | U | 0x001F, "230VAC          ", "230VAC          ", "230VAC          ", "230VAC          ", "230VAC          ", MNUVAL,   0, 	&GetSetVMains,0,0,0,0},		
        {{0, 2, 7, 2, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |       P1 | A0 | A1 |      JB | LA | WI | WA | WH | U | 0x001F, "120VAC          ", "120VAC          ", "120VAC          ", "120VAC          ", "120VAC          ", MNUVAL,   1, 	&GetSetVMains,0,0,0,0},
#else
        // PASSWORD 0991  : only in 3 phases (no mask BP)
        {{0, 2, 7, 0, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |       P1 | A0 | A1 |      JB | LA | WI | WA | WH | U | 0x001F, "Alimentation    ", "Power Supply    ", "Netzspannung    ", "Alimentazione   ", "Fuente de Alim. ", MNULIST,  0, 	&GetSetMnuList,0,0,0,0},
		{{0, 2, 7, 1, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |       P1 | A0 | A1 |      JB | LA | WI | WA | WH | U | 0x001F, "208/240VAC      ", "208/240VAC      ", "208/240VAC      ", "208/240VAC      ", "208/240VAC      ", MNUVAL,   0, 	&GetSetVMains,0,0,0,0},
        {{0, 2, 7, 2, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |       P1 | A0 | A1 |      JB | LA | WI | WA | WH | U | 0x001F, "120VAC          ", "120VAC          ", "120VAC          ", "120VAC          ", "120VAC          ", MNUVAL,   1, 	&GetSetVMains,0,0,0,0},
#endif
        
        // MODIF R2.1
        {{0, 2, 8, 0, 0, 0}, AE |         PT | M1 |       NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Alimentation    ", "Power Supply    ", "Netzspannung    ", "Alimentazione   ", "Fuente de Alim. ", MNULIST,  0, 	&GetSetMnuList,0,0,0,0},
        {{0, 2, 8, 1, 0, 0}, AE |         PT | M1 |       NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "400VAC          ", "400VAC          ", "400VAC          ", "400VAC          ", "400VAC          ", MNUVAL,   OFF, &GetSetVMains480,0,0,0,0},
        {{0, 2, 8, 2, 0, 0}, AE |         PT | M1 |       NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "480VAC          ", "480VAC          ", "480VAC          ", "480VAC          ", "480VAC          ", MNUVAL,   ON, 	&GetSetVMains480,0,0,0,0},

		{{0, 2, 9, 0, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Option          ", "Option          ", "Option          ", "Opzioni         ", "Opciones        ", MNUMNU, 0,    	&GetSetMnuList,0,0,0,0},
		{{0, 2, 9, 1, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Test options    ", "Options test    ", "Test Optionen   ", "Test opzioni    ", "Test            ", MNUOPTION, 0,  &GetSetTestOptions,0,0,0,0},
		{{0, 2, 9, 2, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Temps Electro.  ", "Electoval. Time ", "Dauer Magnet.   ", "Tempo elettrov  ", "Elect. Tiempo   ", MNUNUM, 0,     &GetSetValveDelay,0,0,0,0},
        {{0, 2, 9, 3, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "PLC Pulse On/Off", "PLC Pulse On/Off", "PLC Pulse On/Off", "PLC Pulse On/Off", "PLC Pulse On/Off", MNULIST, 0,    &GetSetMnuList,0,0,0,0},
        {{0, 2, 9, 3, 1, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "On              ", "On              ", "Ein             ", "On              ", "On              ", MNUVAL, 1,     &GetSetPLCPulseOffOn,0,0,0,0},
		{{0, 2, 9, 3, 2, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Off             ", "Off             ", "Aus             ", "Off             ", "Off             ", MNUVAL, 0,     &GetSetPLCPulseOffOn,0,0,0,0},
        
        
#ifndef HAWKER_US
		{{0, 2,10, 0, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Com IQ On/Off   ", "Com IQ On/Off   ", "IQ Komm. Ein/Aus", "Com IQ On/Off   ", "Com IQ On/Off   ", MNULIST, 0,   	&GetSetMnuList,0,0,0,0},
#else
        {{0, 2,10, 0, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "RFI On/Off      ", "RFI On/Off      ", "RFI On/Off      ", "RFI On/Off      ", "RFI On/Off      ", MNULIST, 0,    &GetSetMnuList,0,0,0,0},
#endif		
        {{0, 2,10, 1, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "On              ", "On              ", "Ein             ", "On              ", "On              ", MNUVAL, ON,   	&GetSetComIQOffOn,0,0,0,0},
		{{0, 2,10, 2, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Off             ", "Off             ", "Aus             ", "Off             ", "Off             ", MNUVAL, OFF,   &GetSetComIQOffOn,0,0,0,0},

		{{0, 2,11, 0, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |       P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Mod Adr Trans RF", "Mod RF Trans Add", "Mod RF Trans Add", "Mod RF Trans Add", "Mod RF Trans Add", MNUHEXA, 0,    &GetSetModRFTrAddr,0,0,0,0},

#ifdef ENERSYS_EU
		{{0, 2,12, 0, 0, 0}, AE |         PT | M1 |  US | NX | XF |      P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Raz Memo/Status ", "Rst Memo/Status ", "Speicher-Reset  ", "Rst Memo/Stato  ", "Rst Memo/Estados", MNULIST, 0, 	&GetSetMnuList,0,0,0,0},
		{{0, 2,12, 1, 0, 0}, AE |         PT | M1 |  US | NX | XF |      P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Oui             ", "Yes             ", "Ja              ", "Si              ", "Si              ", MNUVAL, ON, 	&GetSetMemoReset,0,0,0,0},
#else
		{{0, 2,12, 0, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Raz Memo/Status ", "Rst Memo/Status ", "Speicher-Reset  ", "Rst Memo/Stato  ", "Rst Memo/Estados", MNULIST, 0, 	&GetSetMnuList,0,0,0,0},
		{{0, 2,12, 1, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Oui             ", "Yes             ", "Ja              ", "Si              ", "Si              ", MNUVAL, ON, 	&GetSetMemoReset,0,0,0,0},
#endif

		{{0, 2,13, 0, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |       P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x0000, "Reset Usine     ", "Factory Reset   ", "Werkseinst.Reset", "Factory Reset   ", "Reset fabrica   ", MNULIST, 0, 	&GetSetMnuList,0,0,0,0},
		{{0, 2,13, 1, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |       P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x0000, "Oui             ", "Yes             ", "Ja              ", "Si              ", "Si              ", MNUVAL, ON, 	&GetSetFactoryReset,0,0,0,0},


		{{0, 2,14, 0, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Reseau          ", "Network         ", "Netzwerk        ", "Rete            ", "Network         ", MNUMNU, 0,       &GetSetMnuList,0,0,0,0},
		//{{0, 2,14, 1, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "CANJ1939 Adresse", "CANJ1939 Address", "CANJ1939 Address", "CANJ1939 Address", MNUNUM, 0,      &GetSetJ1939Adr,0,0,0,0},
		{{0, 2,14, 2, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Protocole       ", "Protocol        ", "Protokoll       ", "Protocollo      ", "Protocolo       ", MNULIST, 0,      &GetSetMnuList,0,0,0,0},
		{{0, 2,14, 2, 1, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Jbus            ", "Jbus            ", "Jbus            ", "Jbus            ", "Jbus            ", MNUVAL, JBUS,    &GetSetProtocol,0,0,0,0},
		{{0, 2,14, 2, 2, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Ethernet        ", "LAN             ", "LAN             ", "LAN             ", "LAN             ", MNUVAL, LAN,     &GetSetProtocol,0,0,0,0},
		{{0, 2,14, 2, 4, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "BFM             ", "BFM             ", "BFM             ", "BFM             ", "BFM             ", MNUVAL, BFM,     &GetSetProtocol,0,0,0,0},
		{{0, 2,14, 3, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB                     | U | 0x001F, "Vitesse         ", "Baud rate       ", "Baud rate       ", "Baud rate       ", "Baud rate       ", MNULIST, 0,      &GetSetMnuList,0,0,0,0},
		{{0, 2,14, 3, 1, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB                     | U | 0x001F, "1200            ", "1200            ", "1200            ", "1200            ", "1200            ", MNUVAL, 1,       &GetSetBaudRate,0,0,0,0},
		{{0, 2,14, 3, 2, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB                     | U | 0x001F, "9600            ", "9600            ", "9600            ", "9600            ", "9600            ", MNUVAL, 8,       &GetSetBaudRate,0,0,0,0},
		{{0, 2,14, 3, 3, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB                     | U | 0x001F, "14400           ", "14400           ", "14400           ", "14400           ", "14400           ", MNUVAL, 12,      &GetSetBaudRate,0,0,0,0},
		{{0, 2,14, 3, 4, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB                     | U | 0x001F, "19200           ", "19200           ", "19200           ", "19200           ", "19200           ", MNUVAL, 16,      &GetSetBaudRate,0,0,0,0},
		{{0, 2,14, 3, 5, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB                     | U | 0x001F, "38400           ", "38400           ", "38400           ", "38400           ", "38400           ", MNUVAL, 32,      &GetSetBaudRate,0,0,0,0},
		{{0, 2,14, 3, 6, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB                     | U | 0x001F, "115200          ", "115200          ", "115200          ", "115200          ", "115200          ", MNUVAL, 96,      &GetSetBaudRate,0,0,0,0},
		{{0, 2,14, 4, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB                     | U | 0x001F, "Adresse Jbus    ", "JBus-Address    ", "JBus-Adresse    ", "Indrizzo Jbus   ", "JBus-Address    ", MNUNUM, 0,       &GetSetJbAdr,0,0,0,0},
		{{0, 2,14, 5, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP |      LA | WI | WA | WH | U | 0x001F, "Ethernet        ", "Ethernet        ", "Ethernet        ", "Ethernet        ", "Ethernet        ", MNUMNU, 0,       &GetSetMnuList,0,0,0,0},
		{{0, 2,14, 5, 1, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP |      LA | WI | WA | WH | U | 0x001F, "Adresse IP      ", "IP Address      ", "IP-Adresse      ", "Indrizzo IP     ", "Direccion IP    ", MNUNUM, 0,       &GetSetEthIP,0,0,0,0},
		{{0, 2,14, 5, 2, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP |      LA | WI | WA | WH | U | 0x001F, "DNS             ", "DNS             ", "DNS             ", "DNS             ", "DNS             ", MNUNUM, 0,       &GetSetEthDNS,0,0,0,0},
		{{0, 2,14, 5, 3, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP |      LA | WI | WA | WH | U | 0x001F, "Gateway         ", "Gateway         ", "Gateway         ", "Gateway         ", "Gateway         ", MNUNUM, 0,       &GetSetEthGW,0,0,0,0},
		{{0, 2,14, 5, 4, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP |      LA | WI | WA | WH | U | 0x001F, "Subnet mask     ", "Subnet mask     ", "Subnet mask     ", "Subnet mask     ", "Subnet mask     ", MNUNUM, 0,       &GetSetEthSM,0,0,0,0},
		//{{0, 2,14, 5, 5, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP |      LA | WI | WA | WH | U | 0x001F, "Vitesse         ", "Speed           ", "Geschwindigkeit ", "Velocita        ", "Velocidad       ", MNULIST, 0,      &GetSetMnuList,0,0,0,0},
		//{{0, 2,14, 5, 5, 1}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP |      LA | WI | WA | WH | U | 0x001F, "Auto            ", "Auto            ", "Automatisch     ", "Auto            ", "Auto            ", MNUVAL, 0,       &GetSetEthA10100,0,0,0,0},
		//{{0, 2,14, 5, 5, 2}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP |      LA | WI | WA | WH | U | 0x001F, "10 Mb/s         ", "10 Mb/s         ", "10 Mb/s         ", "10 Mb/s         ", "10 Mb/s         ", MNUVAL, 1,       &GetSetEthA10100,0,0,0,0},
		//{{0, 2,14, 5, 5, 3}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP |      LA | WI | WA | WH | U | 0x001F, "100 Mb/s        ", "100 Mb/s        ", "100 Mb/s        ", "100 Mb/s        ", "100 Mb/s        ", MNUVAL, 2,       &GetSetEthA10100,0,0,0,0},

		{{0, 2,14, 6, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP |           WI | WA | WH | U | 0x001F, "WIFI            ", "WIFI            ", "WIFI            ", "WIFI            ", "WIFI            ", MNUMNU, 0,       &GetSetMnuList},
		{{0, 2,14, 6, 1, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP |           WI | WA | WH | U | 0x001F, "SSID1           ", "SSID1           ", "SSID1           ", "SSID1           ", "SSID1           ", MNUALPHA0, 0,    &GetSetEthSSID1},
		{{0, 2,14, 6, 2, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP |           WI | WA | WH | U | 0x001F, "SSID2           ", "SSID2           ", "SSID2           ", "SSID2           ", "SSID2           ", MNUALPHA0, 0,    &GetSetEthSSID2},
		{{0, 2,14, 6, 3, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP |           WI | WA | WH | U | 0x001F, "Securite        ", "Security        ", "Sicherheit      ", "Encryption      ", "Encryption      ", MNULIST, 0,      &GetSetMnuList},
		{{0, 2,14, 6, 3, 1}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP |           WI | WA | WH | U | 0x001F, "No              ", "No              ", "No              ", "No              ", "No              ", MNUVAL, 0,       &GetSetEthSecurity},
		//  {{0, 2,14, 6, 3, 2}, AE |          PT | M1 |  US | NX | XF | P3 | P2 |            A0 | A1               | WI | WA | WH | U | 0x001F, "WEP 64          ", "WEP 64          ", "WEP 64          ", "WEP 64          ", MNUVAL, 1,      &GetSetEthSecurity},
		//  {{0, 2,14, 6, 3, 3}, AE |          PT | M1 |  US | NX | XF | P3 | P2 |            A0 | A1               | WI | WA | WH | U | 0x001F, "WEP 128         ", "WEP 128         ", "WEP 128         ", "WEP 128         ", MNUVAL, 2,      &GetSetEthSecurity},
		{{0, 2,14, 6, 3, 4}, AE |          PT | M1 |  US | NX | XF | P3 | P2 |  P0 |     A0 | A1 | BP |           WI | WA | WH | U | 0x001F, "WPA-PSK         ", "WPA-PSK         ", "WPA-PSK         ", "WPA-PSK         ", "WPA-PSK         ", MNUVAL, 3,       &GetSetEthSecurity},
		{{0, 2,14, 6, 3, 5}, AE |          PT | M1 |  US | NX | XF | P3 | P2 |  P0 |     A0 | A1 | BP |           WI | WA | WH | U | 0x001F, "WPA2-802.1x     ", "WPA2-802.1x     ", "WPA2-802.1x     ", "WPA2-802.1x     ", "WPA2-802.1x     ", MNUVAL, 4,       &GetSetEthSecurity},
		{{0, 2,14, 6, 4, 0}, AE |          PT | M1 |  US | NX | XF | P3 | P2 |  P0 |     A0 | A1 | BP |                WA      | U | 0x001F, "Pass phrase 1   ", "Pass phrase 1   ", "Pass phrase 1   ", "Pass phrase 1   ", "Pass phrase 1   ", MNUALPHA0, 0,    &GetSetEthPassPh1},
		{{0, 2,14, 6, 5, 0}, AE |          PT | M1 |  US | NX | XF | P3 | P2 |  P0 |     A0 | A1 | BP |                WA      | U | 0x001F, "Pass phrase 2   ", "Pass phrase 2   ", "Pass phrase 2   ", "Pass phrase 2   ", "Pass phrase 2   ", MNUALPHA0, 0,    &GetSetEthPassPh2},
		//  {{0, 2,14, 6, 6, 0}, AE |      C | PT | M1 |  US | NX | XF | P3 | P2 |            A0 | A1 | BP |                    | WH | U | 0x001F, "WEPKEY1         ", "WEPKEY1         ", "WEPKEY1         ", "WEPKEY1         ", MNUHEXA, 0,     &GetSetEthWEP1},
		//  {{0, 2,14, 6, 7, 0}, AE |      C | PT | M1 |  US | NX | XF | P3 | P2 |            A0 | A1 | BP |                    | WH | U | 0x001F, "WEPKEY2         ", "WEPKEY2         ", "WEPKEY2         ", "WEPKEY2         ", MNUHEXA, 0,     &GetSetEthWEP2},

#ifdef  ENERSYS_EU
        {{0, 2, 15, 0, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "EasyKit Param   ", "EasyKit Param   ", "EasyKit Param   ", "EasyKit Param   ", "EasyKit Param   ", MNUMNU, 0,      &GetSetMnuList,0,0,0,0},
        {{0, 2, 15, 1, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Adresse         ", "Address         ", "Adresse         ", "Address         ", "Address         ", MNUHEXA, 0,     &GetSetEasyKitAdr,0,0,0,0},
        {{0, 2, 15, 2, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "EasyKit On/Off  ", "EasyKit On/Off  ", "EasyKit Ein/Aus ", "EasyKit On/Off  ", "EasyKit On/Off  ", MNULIST, 0,     &GetSetMnuList,0,0,0,0},
        {{0, 2, 15, 2, 1, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "On              ", "On              ", "Ein             ", "On              ", "On              ", MNUVAL, ON,     &GetSetEasyKitOnOff,0,0,0,0},
        {{0, 2, 15, 2, 2, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Off             ", "Off             ", "Aus             ", "Off             ", "Off             ", MNUVAL, OFF,    &GetSetEasyKitOnOff,0,0,0,0},
#endif

		{{0, 2, 16, 0, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Parametres", "Parameters", "Parameter", "Parametri",  "Parametros", MNUMNU, 0, 	  &GetSetMnuList,0,0,0,0},
		{{0, 2, 16, 1, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Date / Heure    ", "Date / Hour     ", "Datum / Zeit    ", "Data / Ora      ", "Fecha / Ora     ", MNUNUM, 0, 	&GetSetSetTime,0,0,0,0},

		{{0, 2, 16, 2, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Numero de serie ", "Serial Number   ", "Seriennummer    ", "Matricola       ", "Numero serie    ", MNUALPHA0, 0,	&GetSetSerialNumber},

		{{0, 2, 16, 3, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Langue          ", "Language        ", "Sprache         ", "Lingua          ", "Idioma          ", MNULIST, 0, 	&GetSetMnuList,0,0,0,0},
		{{0, 2, 16, 3, 1, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Francais        ", "Francais        ", "Francais        ", "Francais        ", "Francais        ", MNUVAL, 0, 	&GetSetLangage,0,0,0,0},
		{{0, 2, 16, 3, 2, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "English         ", "English         ", "English         ", "English         ", "English         ", MNUVAL, 1, 	&GetSetLangage,0,0,0,0},
		{{0, 2, 16, 3, 3, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Deutsch         ", "Deutsch         ", "Deutsch         ", "Deutsch         ", "Deutsch         ", MNUVAL, 2, 	&GetSetLangage,0,0,0,0},
		{{0, 2, 16, 3, 4, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Italiano        ", "Italiano        ", "Italiano        ", "Italiano        ", "Italiano        ", MNUVAL, 3, 	&GetSetLangage,0,0,0,0},
        {{0, 2, 16, 3, 5, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Spanish         ", "Espanol         ", "Espanol         ", "Espanol         ", "Espanol         ", MNUVAL, 4, 	&GetSetLangage,0,0,0,0},

		{{0, 2, 16, 4, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Region          ", "Region          ", "Region          ", "Reione          ", "Region          ", MNULIST, 0,  	&GetSetMnuList,0,0,0,0},
		{{0, 2, 16, 4, 1, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Europe          ", "Europe          ", "Europa          ", "Europa          ", "Europa          ", MNUVAL, 0,    &GetSetArea,0,0,0,0},
		{{0, 2, 16, 4, 2, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "US              ", "US              ", "US              ", "US              ", "US              ", MNUVAL, 1,    &GetSetArea,0,0,0,0},

		{{0, 2, 16, 5, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Afficheur       ", "Display         ", "Anzeige         ", "Display         ", "Pantalla        ", MNUMNU, 0, 	&GetSetMnuList,0,0,0,0},
		//{{0, 2, 20, 1, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Contraste       ", "Contrast        ", "Kontrast        ", "Contrasto       ",  MNUNUM, 0,      &GetSetContrast,0,0,0,0},
		{{0, 2, 16, 5, 1, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Mise en veille  ", "Screen Saver    ", "Bildsch.schoner ", "Screen Saver    ", "Protector Panta.", MNULIST, 0,  	&GetSetMnuList,0,0,0,0},
		{{0, 2, 16, 5, 1, 1}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "On              ", "On              ", "Ein             ", "On              ", "On              ", MNUVAL, ON,   &GetSetBackOnOff,0,0,0,0},
		{{0, 2, 16, 5, 1, 2}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Off             ", "Off             ", "Aus             ", "Off             ", "Off             ", MNUVAL, OFF,  &GetSetBackOnOff,0,0,0,0},
		{{0, 2, 16, 5, 2, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Delai           ", "Delay           ", "Verzogerung     ", "Delay           ", "Retardo         ", MNUNUM, 0,	&GetSetScreenSaverTime,0,0,0,0},
        //{{0, 2, 16, 5, 3, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Calib. Touch    ", "Calib. Touch    ", "Calib. Touch    ", "Calib. Touch    ",  MNULIST, 0,    &GetSetMnuList,0,0,0,0},
        //{{0, 2, 16, 5, 3, 1}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Reset           ", "Reset           ", "Reset           ", "Reset           ",  MNUVAL, ON,    &GetSetCalibTouch,0,0,0,0},
#ifndef  ENERSYS_EU
        {{0, 2, 16, 5, 3, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Themes          ", "Themes          ", "Themes          ", "Themes          ", "Themes          ", MNULIST, 0,     &GetSetMnuList,0,0,0,0},
		{{0, 2, 16, 5, 3, 1}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Theme A         ", "Theme A         ", "Theme A         ", "Theme A         ", "Theme A         ", MNUVAL, THEMEA, &GetSetTheme,0,0,0,0},
		{{0, 2, 16, 5, 3, 2}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Theme B         ", "Theme B         ", "Theme B         ", "Theme B         ", "Theme B         ", MNUVAL, THEMEB, &GetSetTheme,0,0,0,0},
#endif

#ifndef HAWKER_US
		{{0, 2, 16, 6, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "DayLight        ", "DayLight        ", "DayLight        ", "DayLight        ", "DayLight        ", MNULIST, 0,     &GetSetMnuList,0,0,0,0},
#else
		{{0, 2, 16, 6, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "DayLight Saving ", "DayLight Saving ", "DayLight Saving ", "DayLight Saving ", "DayLight Saving ", MNULIST, 0,     &GetSetMnuList,0,0,0,0},
#endif
		{{0, 2, 16, 6, 1, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Off             ", "Off             ", "Aus             ", "Off             ", "Off             ", MNUVAL, OFF,        &GetSetDayLight,0,0,0,0},
		{{0, 2, 16, 6, 2, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Europe          ", "Europe          ", "Europe          ", "Europe          ", "Europe          ", MNUVAL, EUROPE,     &GetSetDayLight,0,0,0,0},
		{{0, 2, 16, 6, 3, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "US - Canada     ", "US - Canada     ", "US - Canada     ", "US - Canada     ", "US - Canada     ", MNUVAL, USCANADA,   &GetSetDayLight,0,0,0,0},
		{{0, 2, 16, 6, 4, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Australia       ", "Australia       ", "Australia       ", "Australia       ", "Australia       ", MNUVAL, AUSTRALIA,  &GetSetDayLight,0,0,0,0},

		{{0, 2, 16, 7, 0, 0}, AE |         PT | M1 |  US | NX | XF |      P2 |       P0 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Reboot          ", "Reboot          ", "Reboot          ", "Reboot          ", "Reboot          ", MNULIST, 0, 	    &GetSetMnuList,0,0,0,0},
		{{0, 2, 16, 7, 1, 0}, AE |         PT | M1 |  US | NX | XF |      P2 |       P0 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Oui             ", "Yes             ", "Ja              ", "Si              ", "Si              ", MNUVAL, ON, 	    &GetSetReboot,0,0,0,0},

		{{0, 3, 0, 0, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "USB",           "USB",              "USB",              "USB",              "USB",                  MNUMNU, 0,      &GetSetMnuList,&usb_icon, &usb_icon_select,&icon_USB,0},
#ifdef ENERSYS_EU		
		{{0, 3, 1, 0, 0, 0}, AE |         PT | M1 |  US | NX | XF |      P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Enreg memo      ", "Record memo     ", "Aufzeichnung    ", "Record Memo     ",  "Registro Memo.  ", MNULIST, 0, 	&GetSetMnuList,0,0,0,0},
        {{0, 3, 1, 1, 0, 0}, AE |         PT | M1 |  US | NX | XF |      P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Oui             ", "Yes             ", "Ja              ", "Si              ",  "Si              ", MNUVAL, ON, 	&GetSetUsbMemoWrite,0,0,0,0},
#else
		{{0, 3, 1, 0, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0      | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Enreg memo      ", "Record memo     ", "Aufzeichnung    ", "Record Memo     ",  "Registro Memo.  ", MNULIST, 0, 	&GetSetMnuList,0,0,0,0},
        {{0, 3, 1, 1, 0, 0}, AE |         PT | M1 |  US | NX | XF | P3 | P2 |  P0      | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Oui             ", "Yes             ", "Ja              ", "Si              ",  "Si              ", MNUVAL, ON, 	&GetSetUsbMemoWrite,0,0,0,0},
#endif
		{{0, 3, 2, 0, 0, 0}, AE |     C | PT | M1 |  US | NX | XF |      P2 |       P0 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Enreg profil    ", "Record profile  ", "Kennlinienaufz. ", "Record profili  ",  "Registro muest. ", MNULIST, 0,  	&GetSetMnuList,0,0,0,0},
		{{0, 3, 2, 1, 0, 0}, AE |     C | PT | M1 |  US | NX | XF |      P2 |       P0 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "On              ", "On              ", "Ein             ", "On              ",  "On              ", MNUVAL, ON,   &GetSetUsbProfileWrite,0,0,0,0},
		{{0, 3, 2, 2, 0, 0}, AE |     C | PT | M1 |  US | NX | XF |      P2 |       P0 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Off             ", "Off             ", "Aus             ", "Off             ",  "Off             ", MNUVAL, OFF,  &GetSetUsbProfileWrite,0,0,0,0},
		{{0, 3, 3, 0, 0, 0}, AE |     C | PT | M1 |  US | NX | XF |      P2 |       P0 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Echantil profil ", "Sampling profile", "Messrate        ", "Campionamento   ",  "Perfil muestra  ", MNUNUM, 2,    &GetSetUsbProfileTime,0,0,0,0},
		{{0, 3, 4, 0, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Reprog Soft     ", "Update Software ", "Software Update ", "Update Software ",  "Actual. Software", MNULIST, 0,	&GetSetMnuList,0,0,0,0},
#ifndef HAWKER_US
		{{0, 3, 4, 1, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Press. OK 10sec ", "Press OK 10sec  ", "OK Klicken 10sek", "Press OK 10sec  ",  "Press OK 10sec  ", MNUVAL, ON, 	&GetSetUsbUpdate,0,0,0,0},
#else
		{{0, 3, 4, 1, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Press Start/Stop", "Press Start/Stop", "Start/Stop Klick", "Press Start/Stop",  "Press Start/Stop", MNUVAL, ON, 	&GetSetUsbUpdate,0,0,0,0},
#endif
		{{0, 3, 5, 0, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0      | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Sauvegarder     ", "Save Setting    ", "Konfig.speichern", "Save Setting    ",  "Guardar config. ", MNULIST, 0, 	&GetSetMnuList,0,0,0,0},
		{{0, 3, 5, 1, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0      | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Oui             ", "Yes             ", "Ja              ", "Si              ",  "Si              ", MNUVAL, ON, 	&GetSetUsb2Config,0,0,0,0},
		{{0, 3, 6, 0, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0      | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Restaurer       ", "Restore Setting ", "Konf.wiederhers.", "Restore Setting ",  "Restaurar config", MNULIST, 0, 	&GetSetMnuList,0,0,0,0},
		{{0, 3, 6, 1, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0      | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Oui             ", "Yes             ", "Ja              ", "Si              ",  "Si              ", MNUVAL, ON, 	&GetSetConfig2Usb,0,0,0,0},

		//{{0, 3, 7, 0, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |       P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Reprog Rf transc", "Update Rf transc", "Update Rf transc", "Update Rf transc",  MNULIST, 0, 	  &GetSetMnuList,0,0,0,0},
		//{{0, 3, 7, 1, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |       P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "On              ", "On              ", "Ein             ", "On              ",  MNUVAL, ON, 	  &GetSetUsbPrgRf,0,0,0,0},

		{{0, 3, 8, 0, 0, 0},              PT | M1 |  US | NX | XF | P3 | P2 |       P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Reprog MODULE   ", "Update MODULE	  ", "MODULE Update	  ", "Update MODULE	  ",  "Actual. Modulo", MNUBOOTAPP, 0, 	 &GetSetBootApp,0,0,0,0},

		{{0, 4, 0, 0, 0, 0}, AE | I | C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Mot de passe",     "Password",          "Passwort",        "Password",           "Contrasena",      MNUPSW, 0, 	    &GetSetGetPasswd,&password_icon,&password_icon_select,&icon_password,0},

		{{0, 5, 0, 0, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Chargeur", "Charger", "Ladegerat", "Charger",  "Cargador", MNUMNU, 0,     &GetSetMnuList,&charger_icon,&charger_icon_select,&icon_charger,0},
		{{0, 5, 1, 0, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Informations    ", "Informations    ", "Information     ", "Informations    ", "Informacion     ", MNUINFO, 0,	&GetSetModInfoGen,0,0,0,0},
		{{0, 5, 2, 0, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Modules         ", "Modules         ", "Modules         ", "Modules         ", "Modules         ", MNUFUNLIST, 0, &GetSetModInfoGen,0,0,0,0},
		{{0, 5, 2, 1, 0, 0}, AE |     C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Modules Info    ", "Modules Info    ", "Modules Info    ", "Modules Info    ", "Modulos Info    ", MNUFUNVAL, 0,  &GetSetModInfo0,0,0,0,0},
		{{0, 5, 3, 0, 0, 0}, AE |     C | PT | M1 |  US | NX | XF |      P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Clear TH-LOCK   ", "Clear TH-LOCK   ", "Clear TH-LOCK   ", "Clear TH-LOCK   ", "Clear TH-LOCK   ", MNULIST, 0, 	&GetSetMnuList,0,0,0,0},
		{{0, 5, 3, 1, 0, 0}, AE |     C | PT | M1 |  US | NX | XF |      P2 |  P0 |      A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Oui             ", "Yes             ", "Ja              ", "Si              ", "Si              ", MNUVAL, ON,	&GetSetClearTempCon,0,0,0,0},
		{{0, 6, 0, 0, 0, 0}, AE | I | C | PT | M1 |  US | NX | XF | P3 | P2 |  P0 | P1 | A0 | A1 | BP | JB | LA | WI | WA | WH | U | 0x001F, "Sortie", "Exit", "Ausgang", "Uscita", "Retorno", MNUEXIT, 0, &GetSetExit,&return_icon,&return_icon_select, &icon_exit,0},
};


const uchar MnuNb = (uint)sizeof(MnuDef) / sizeof(StructMnu);         /* Number of menu */

extern const uchar ListWord16_State[25][NBLANG][17];

const uchar ListWord3[14][NBLANG][4] = {
		{"DdC", "SoC", "SoC", "SoC", "SoC",},   // 2 : Date Memo
		{"DBa", "DBa", "DBa", "DBa", "DBa",},
		{"jan", "jan", "Jan", "jan", "jan",}, // 4 : Month
		{"fev", "feb", "Feb", "feb", "feb",},
		{"mar", "mar", "Mar", "mar", "mar",},
		{"avr", "apr", "Apr", "apr", "apr",},
		{"mai", "may", "Mai", "may", "may",},
		{"jui", "jun", "Jun", "jun", "jun",},
		{"jul", "jul", "Jul", "jul", "jul",},
		{"aou", "aug", "Aug", "aug", "aug",},
		{"sep", "sep", "Sep", "sep", "sep",},
		{"oct", "oct", "Okt", "oct", "oct",},
		{"nov", "nov", "Nov", "nov", "nov",},
		{"dec", "dec", "Dez", "dec", "dec",},
};


const uchar ListWord8[24][NBLANG][9] = {
		{"Capacite", "Capacity", "Kapazit ", "Capacita", "Capacida",},   // 0
		{"U batt  ", "U batt  ", "U batt  ", "U batt  ", "U batt  ",},
		{"Temp    ", "Temp    ", "Temperat", "Temp    ", "Temp    ",},
		{"Techno  ", "Techno  ", "Techno  ", "Techno  ", "Techno  ",},
		{"Alarme  ", "Warning ", "Warnung ", "Allarme ", "Alarma  ",},
		{"Profil  ", "Profile ", "Kennlini", "Profilo ", "Perfil  ",},
		{"Phase   ", "Phase   ", "Phase   ", "Fase    ", "Phase   ",},
		{"CFC     ", "CFC     ", "CFC     ", "CFC     ", "CFC     ",},
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
		{"Tps chg ", "Chg Time", "Ladezeit", "Chg Time", "Chg Time",},
		{"Ah      ", "Ah      ", "Ah      ", "Ah      ", "Ah      ",},
		{"kWh     ", "kWh     ", "kWh     ", "kWh     ", "kWh     ",},
		{"Fin chg ", "Chg end ", "Ladeende", "Chg end ", "Chg end ",},
		{"Defaut  ", "Default ", "Fehler  ", "Default ", "Default ",},
		{"S/N     ", "S/N     ", "S/N     ", "S/N     ", "S/N     ",},	  // 23 : Memo
};


const uchar ListWord9[13][NBLANG][10] = {
		{"DF1","DF1","DF1","DF1","DF1",},
		{"DF2","DF2","DF2","DF2","DF2",},
		{"DF3","DF3","DF3","DF3","DF3",},
		{"DF4","DF4","DF4","DF4","DF4",},
		{"DF5","DF5","DF5","DF5","DF5",},
		{"TH","TH","TH","TH","TH",},
		{"CHARGE","CHARGE","LADUNG","CHARGE","CHARGE",},
		{"COMPLETE","COMPLETE","VOLL.","COMPLETE","COMPLETE",},
		{"INCOMPL.","PARTIAL","UNVOLL.","PARTIAL","PARTIAL",},
		{"DFC","DFC","DFC","DFC","DFC",},                   //9
		{"NBRAH","CNTAH","CNTAH","CNTAH","CNTAH",},
		{"DF7","DF7","DF7","DF7","DF7",},                   // 11
		{"TH MOD","TH MOD","TH MOD","TH MOD","TH MOD",},                   // 12
};


const uchar ListWord6[45][NBLANG][7] = {
		{"","","","",},//0PROFILE
#ifdef  ENERSYS_EU 
		{"HDUTY","HDUTY","HDUTY","HDUTY","HDUTY",},
#else
		{"IONIC","IONIC","IEM","IONIC","IONIC",},
#endif
#ifdef  ENERSYS_EU 
		{"GEL","GEL","GEL","GEL","GEL",},
#endif
#ifdef  ENERSYS_US 
		{"GELBLC","GELBLC","GELBLC","GELBLC","GELBLC",},
#endif
#ifdef  HAWKER_US 
		{"EVLTN","EVLTN","EVLTN","EVLTN","EVLTN",},
#endif
		{"FROID","COLD","KALT","COLD","COLD",},
		{"WL20","WL20","WL20","WL20","WL20",},
		{"PNEU","PNEU","EU","PNEU","PNEU",},
		{"EGAL","EQUAL","AUSGL","EQUAL","EQUAL",},
		{"OPP","OPP","OPP","OPP","OPP",},
#ifndef HAWKER_US     
		{"VRLA","VRLA","VRLA","VRLA","VRLA",},
#else
		{"ENVLNK","ENVLNK","ENVLNK","ENVLNK","ENVLNK",},
#endif    
		{"RGT","AGM","AGM","AGM","AGM",},
		{"","","","",},
//#ifndef HAWKER_US        
#ifdef ENERSYS_EU
		{"OPPIUI", "OPPIUI", "OPPIUI", "OPPIUI","OPPIUI",},  // 11
#else
        {"CMP CH", "CMP CH", "CMP CH", "CMP CH","CMP CH",},  // 11
#endif
		{"PREM","PREM","PREM","PREM","PREM",},
		{"TEST","TEST","TEST","TEST","TEST",},
		{"","","","","",},
		{"","","","","",},
#ifdef  HAWKER_US    // MODIF 3.3
		{"FXSTND","FXSTND","FXSTND","FXSTND","FXSTND",},
#else
		{"NXSTND","NXSTND","NXSTND","NXSTND","NXSTND",},
#endif   
		{"","","","","",},
		{"","","","","",},
		{"STDWL","STDWL","STDWL","STDWL","STDWL",},
#ifdef  HAWKER_US    // MODIF 3.3
		{"FXFAST","FXFAST","FXFAST","FXFAST","FXFAST",},
#else
		{"NXFAST","NXFAST","NXFAST","NXFAST","NXFAST",},
#endif
		{"LOWCHG","LOWCHG","LOWCHG","LOWCHG","LOWCHG",},
#ifdef  HAWKER_US    // MODIF 3.3
		{"FXBLOC","FXBLOC","FXBLOC","FXBLOC","FXBLOC",},
#else
		{"NXBLOC","NXBLOC","NXBLOC","NXBLOC","NXBLOC",},
#endif
		{"LITH  ","LITH  ","LITH  ","LITH  ","LITH  ",},
		{"","","","",}, //23 TECHNO -> update "#define TEC_POS     23" in comiq.h if you add techno
#ifdef  ENERSYS_EU 
		{"GEL","GEL","GEL","GEL","GEL",},
#endif
#ifdef  ENERSYS_US 
		{"GELBLC","GELBLC","GELBLC","GELBLC","GELBLC",},
#endif
#ifdef  HAWKER_US 
		{"EVLTN","EVLTN","EVLTN","EVLTN","EVLTN",},
#endif
#ifdef  ENERSYS_EU 
		{"PBO","PBO","PBO","PBO","PBO",},
#else
		{"FLOOD.","FLOOD.","FLOOD.","FLOOD.","FLOOD.",},
#endif
		{"PNEU","PNEU","EU","PNEU","PNEU",},
		{"WL20","WL20","WL20","WL20","WL20",},
		{"PREM","PREM","PREM","PREM","PREM",},
		{"FASTEU","FASTEU","FASTEU","FASTEU","FASTEU",},
		{"FASTUS","FASTUS","FASTUS","FASTUS","FASTUS",},
		{"STDWL","STDWL","STDWL","STDWL","STDWL",},
#ifndef  HAWKER_US
		{"NexSys","NexSys","NexSys","NexSys","NexSys",},
#else
		{"FLEX  ","FLEX  ","FLEX  ","FLEX  ","FLEX  ",},
#endif   
		{"HGB","HGB","HGB","HGB","HGB",},
		{"","","","","",},
		{"","","","","",},
#ifndef HAWKER_US     
		{"VRLA","VRLA","VRLA","VRLA","VRLA",},
#else
		{"ENVLNK","ENVLNK","ENVLNK","ENVLNK","ENVLNK",},
#endif    
		{"OPP","OPP","OPP","OPP","OPP",},
#ifndef  HAWKER_US
		{"NXFAST","NXFAST","NXFAST","NXFAST","NXFAST",},
#else
		{"FXFAST","FXFAST","FXFAST","FXFAST","FXFAST",},
#endif
		{"PZQ","PZQ","PZQ","PZQ","PZQ",},
        {"COLD","COLD","COLD","COLD","COLD",},
        {"LOWCHG","LOWCHG","LOWCHG","LOWCHG","LOWCHG",},
#ifndef  HAWKER_US
        {"NXBLOC","NXBLOC","NXBLOC","NXBLOC","NXBLOC",},
#else
		{"FXBLOC","FXBLOC","FXBLOC","FXBLOC","FXBLOC",},
#endif
		{"LITH  ","LITH  ","LITH  ","LITH  ","LITH  ",},
};


const uchar ListWord16[2][NBLANG][20] = {
		{"JOUR", "DAY", "DAY", "DAY","DAY",},     // 0
		{"Auto", "Auto", "Auto", "Auto","Auto",},  // 1
};

const uchar ListWord16_2[2][NBLANG][20] = {
		{"LMMJVSD", "MTWTFSS", "MTWTFSS", "LMMGVSD","MTWTFSS",},  // 0
		{"LMMJVSD", "MTWTFSS", "MTWTFSS", "LMMGVSD","MTWTFSS",},  // 1
};


/* Table of reversed bit of char */
const uchar UcharRev[256] = {
		0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
		0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
		0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
		0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
		0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
		0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
		0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
		0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
		0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
		0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
		0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
		0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
		0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
		0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
		0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
		0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF,
};



/**
 * @brief	Return to top level menu
 * @param  None
 * @retval None
 */
void LCDMenu_Level(void)
{
	State.CursorOn = 0; // disable cursor
	MnuLevel = 0;
	Mnu[0] = 0;
	Mnu[1] = 0;
	Mnu[2] = 0;
	Mnu[3] = 0;
	Mnu[4] = 0;
	Mnu[5] = 0;
}


/**
 * @brief	Menu management
 * @param  None
 * @retval None
 */
void LCDMenu (void)
{
	uint i;
	uint e;

	if ((Key.MenuLong != 0) && (MnuLevel == 0))
	{
		MnuTimer = State.TimerSec + MENU_TIMEOUT;
		MnuLevel = 1;
		MnuListCur[MnuLevel] = 1; /* First choice in menu */
		MnuListOff[MnuLevel] = 0; /* No offset in previous menu */
		Mnu[1] = 0;
		Mnu[2] = 0;
		Mnu[3] = 0;
		Mnu[4] = 0;
		Mnu[5] = 0;
		pos = 0;
		prevpos = 0;
		PIPState(0);
		DrawMenuFirstEntryBackground();
		DrawMenuFirstEntryTextImage();
		UpdateDisplayNow();
		FirstEntry = 1;
		ReDraw = 1;
		ReDraw2 = 1;
		Key.Menu = 0;
		Key.MenuLong = 0;
		Key.RightLong = 0;
	}

	if (MnuLevel > 0)
	{
		/* Searh for current menu */
		e = 0;
		i = MnuNb;
		while ((0 < i) && (e == 0))
		{
			i --;
			if ((Mnu[1] == MnuDef[i].Mnu[1]) && (Mnu[2] == MnuDef[i].Mnu[2]) &&
					(Mnu[3] == MnuDef[i].Mnu[3]) && (Mnu[4] == MnuDef[i].Mnu[4]) &&
					(Mnu[5] == MnuDef[i].Mnu[5]))
			{
				e = 1;
			}
		}

		switch (MnuDef[i].Type)
		{
		case  MNUMNU :
		case  MNULIST :
			LCDMenu_Liste();
			break;
		case  MNUOPTION :
			LCDMenu_Info();
			break;
		case  MNUINFO :
			LcdInfo();
			break;
		case  MNUNUM :
		case  MNUNUMUPD :
		case  MNUHEXA :
		case  MNUHEXAUPD :
		case  MNUALPHA0 :
			LCDMenu_Input(MnuDef[i].Type);
			break;
		case  MNUBIT:
			LcdBit();
			break;
		case  MNUFUNLIST:
		case  MNUFUNVAL:
		case  MNUFUNSEL:
			LCDMenu_Fun();
			break;
		case MNUSTATUS:
			LCDMenu_FunStatus();
			break;
		case MNUEXIT:
			LCDMenu_Level();
			KEY_Reset();
			Key.MenuReset = 1;
			break;
		case MNUPSW:
			LCDMenu_Password();
			break;
		case MNUBOOTAPP:
			LCDMenu_BootApp();
			break;
		}

	}

	if ((MnuLevel != 0) && (MnuTimer < State.TimerSec))
	{
		MaskPasswdTmpOn;            /* Set password protection */
		MaskPasswdHighOn;           /* Set password protection */
		MaskPasswdLowOn;            /* Set password protection */
		MaskPasswd2On;
		MaskPasswd3On;
		if ((MnuDef[i].Mask & AE) == AE)
			LCDMenu_Level();        /* Returns to top menu level */
		KEY_Reset ();
	}
	else
	{
		if (MnuTimer < State.TimerSec)
		{
			MaskPasswdTmpOn;            /* Set password protection */
			MaskPasswdHighOn;           /* Set password protection */
			MaskPasswdLowOn;            /* Set password protection */
			MaskPasswd2On;
			MaskPasswd3On;
			LCDMenu_Level();
		}
	}
	Key.MenuLong = 0;
}


#define Key0Short Key.StartShort
#define Key1Short Key.RightShort
#define Key2Short Key.DownShort
#define Key3Short Key.MenuShort
#define Key4Short Key.UpShort

#define Key0Long Key.StartLong
#define Key1Long Key.RightLong
#define Key2Long Key.DownLong
#define Key3Long Key.MenuLong
#define Key4Long Key.UpLong


/**
 * @brief	Display menu info
 * @param  None
 * @retval None
 */
void  LCDMenu_Info(void)
{
	/* Print information creeen */
	/* Possible to write 10 lines */
	uchar v[20];               /* 5 lines * 20 characters */
	uchar i;                      /* Counter on menus */

	/* Test if key left pressed (return to previous menu) */
	if (MnuLevel != 0)            /* Set this level to 0 if key left is pressed */
		if (Key.MenuShort != 0)
		{
			/* Loop on all menus */
			i = 0;                    /* Index on menu */
			while (i < MnuNb)
			{
				/* Print title m(l) = 0 */
				if ((Mnu[1] == MnuDef[i].Mnu[1]) && (Mnu[2] == MnuDef[i].Mnu[2]) &&
						(Mnu[3] == MnuDef[i].Mnu[3]) && (Mnu[4] == MnuDef[i].Mnu[4]) &&
						(Mnu[5] == MnuDef[i].Mnu[5]) && (MnuDef[i].Type == MNUOPTION))
				{
					/* Write value for action */
					(*MnuDef[i].f)(0, &v[0]);
				}
				i ++;
			}
			KEY_Reset();
			Mnu[MnuLevel] = 0;
			MnuLevel --;
			Mnu[MnuLevel] = 0;

			if (MnuLevel == 1)
			{
				DrawMenuFirstEntryBackground();
				DrawMenuFirstEntryTextImage();
				UpdateDisplayNow();
			}
			else
				DrawMenupartialTwo();
		}
		else
		{
			/* Loop on all menus */
			i = 0;                      /* Index on menu */
			while (i < MnuNb)
			{
				/* Print title m(l) = 0 */
				if ((Mnu[1] == MnuDef[i].Mnu[1]) && (Mnu[2] == MnuDef[i].Mnu[2]) &&
						(Mnu[3] == MnuDef[i].Mnu[3]) && (Mnu[4] == MnuDef[i].Mnu[4]) &&
						(Mnu[5] == MnuDef[i].Mnu[5]) && (MnuDef[i].Type == MNUOPTION))
				{
					/* Clears display */
					DrawListBackground();
					DrawLine(0,0,(char*)&(MnuDef[i].Text[LANG][0]),0x0000);
					/* Read new value */
					(*MnuDef[i].f)(1, &v[0]);
					DrawLine(1,0,(char*)&v[0],0x0000);
					UpdateDisplayNow();
				}
				i ++;
			}
		}
}


/**
 * @brief	Display selected menu
 * @param  None
 * @retval None
 */
void  LcdInfo(void)
{
	//Put Charger settings summary
	char text[50];
	uint8_t i;

	/* If key OK or exit key pressed */
	if ((Key.MenuShort != 0) || (Key.StartShort != 0))
	{
		Key.MenuShort = 0;
		MnuTimer = State.TimerSec + MENU_TIMEOUT;
		Mnu[MnuLevel] = 0;
		MnuLevel --;
		Mnu[MnuLevel] = 0;

		KEY_Reset();
		if (MnuLevel == 1)
		{
			DrawMenuFirstEntryBackground();
			DrawMenuFirstEntryTextImage();
			UpdateDisplayNow();
		}
		else
			DrawMenupartialTwo();
	}

	if (DisplayUpdate == 1)
	{
		i = sprintf(&text[0], "Profile: ");
		strcpy(&text[i],(char *)&ListWord6[Menu.Profile][LANG][0]);
		SetColor(WHITE);
		OutTextXY(20,75,text);
		if (Menu.Area == 0)
            sprintf(&text[0], "T' : %+2.2d'C", (sint)Menu.BatTemp);
            
		else
			sprintf(&text[0], "T' : %+2.2d'F", ((sint)Menu.BatTemp * 9) / 5 + 32);

		SetColor(WHITE);
		OutTextXY(20,100,text);

		if (Menu.CapAutoManu == 0)
		{
			sprintf(&text[0], "Capacity: Auto");
			SetColor(WHITE);
			OutTextXY(20,125,text);
		}
		else
		{
			sprintf(&text[0], "Capacity: Manual , %u Ah", Menu.BatCap);
			SetColor(WHITE);
			OutTextXY(20,125,text);
		}

		sprintf(&text[0], "Max Current: %u A", Menu.ImaxCharger/10);
		SetColor(WHITE);
		OutTextXY(20,150,text);

		if (Menu.FloatingOffOn == 0)
		{
			sprintf(&text[0], "Floating: Off");
		}
		else
		{   // MODIF R2.7 : fix display bug with Vfloat and current
			//sprintf(&text[0], "Floating: On , %u V %u A", Menu.Vfloating, Menu.Ifloating);
            sprintf(&text[0], "Floating: On , %u mV %u A", Menu.Vfloating, Menu.Ifloating/10);
		}

		SetColor(WHITE);
		OutTextXY(20,175,text);

		if (Menu.Area == 0)
			sprintf(&text[0], "Cable: %u.%u m, %u mm2", Menu.CableLength/10, Menu.CableLength%10, Menu.CableSection);
		else
			sprintf(&text[0], "Cable: %u ft", (uint)Menu.CableLength * 33 / 100);
		SetColor(WHITE);
		OutTextXY(20,200,text);

		sprintf(&text[0], "Equal: %u h, %u A", Menu.EqualTime, (Menu.Iequal & 0x7FFF)/10);
		SetColor(WHITE);
		OutTextXY(20,225,text);

		sprintf(&text[0], "Equal: %u h, %u A", Menu.EqualTime, Menu.Iequal/10);
		SetColor(WHITE);
		OutTextXY(20,225,text);

		sprintf(&text[0], "Delay Charge: %u h %u m", (Menu.ChgDelayVal / 60) % 24, Menu.ChgDelayVal % 60);
		SetColor(WHITE);
		OutTextXY(220,75,text);

		if (Menu.AutoStartOffOn != 0)
		{
			sprintf(&text[0], "Autostart: Off");
		}
		else
		{
			sprintf(&text[0], "Autostart: On");
		}

		SetColor(WHITE);
		OutTextXY(220,100,text);

		if ((Ble.MacAddress[0] != 0) && (Ble.MacAddress[1] != 0) && (Ble.MacAddress[2] != 0))
		{
			sprintf (&text[0], "BLE Mac: %2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X", Ble.MacAddress[0], Ble.MacAddress[1], Ble.MacAddress[2], Ble.MacAddress[3], Ble.MacAddress[4], Ble.MacAddress[5]);
			SetColor(WHITE);
			OutTextXY(220,150,text);
			sprintf(&text[0], "BLE Ver: %4.4s", &Ble.version[0]);
			SetColor(WHITE);
			OutTextXY(220,175,text);
		}
		else
		{
			sprintf(&text[0], "BLE Device OFF");
			SetColor(WHITE);
			OutTextXY(220,150,text);
		}

		DisplayUpdate = 0;
		UpdateDisplayNow();
	}
}

uint8_t PswPtr;
const uint8_t PswTable[16] = {1,2,3,4,5,6,7,8,9,0,12,15};
uint8_t PswSelected[4];
uint8_t PswSelectPtr;


void LCDMenu_Password(void)
{
	uint16_t temp;
	//BYTE e;                      /* Exit flag */
	//e = 0;
	//TimerMnuOption = 0;           /* Reset timer for options display */
	if (EntryEvent == 1)
	{
		DrawPasswordBackground();
		PswSelected[0] = 0;
		PswSelected[1] = 0;
		PswSelected[2] = 0;
		PswSelected[3] = 0;
		PswSelectPtr = 0;
		PswPtr = 0;
		DrawPasswordIcons(0);
		DrawPasswordPointer(0);
		EntryEvent = 0;
		DisplayUpdate = 1;
	}


	if (MnuLevel != 0)            /* Set this level to 0 if key left is pressed */
	{
		if (Key.StartShort != 0)             /* If key OK pressed, exit */
		{
			Key.StartShort = 0;
			if (PswTable[PswPtr] == 12)
			{
				temp = 	PswSelected[0] * 1000 +
						PswSelected[1] * 100 +
						PswSelected[2] * 10 +
						PswSelected[3];
				GetSetGetPasswd2(temp);
				/* Force to exit */
				Key.StartLong = 1;
			}
			else if (PswTable[PswPtr] == 15) Key.StartLong = 1;
			else
			{
				if (PswSelectPtr < 4)
				{
					PswSelected[PswSelectPtr] = PswTable[PswPtr];
					PswSelectPtr++;
					//if (PswSelectPtr != 4)
					DrawPasswordPointer(PswSelectPtr);
					DisplayUpdate = 1;

					/* Temporary for Francis */
					if (PswSelectPtr == 4)
					{
						temp = 	PswSelected[0] * 1000 +
								PswSelected[1] * 100 +
								PswSelected[2] * 10 +
								PswSelected[3];
						GetSetGetPasswd2(temp);
						/* Force to exit */
						Key.StartLong = 1;
					}
					/*End of Temporary for Francis */
				}
			}
		}

		/* If key OK or exit key pressed */
		if (/*(Key.StartShort != 0) || */(Key.StartLong != 0))
		{
			//Key.StartShort = 0;
			Key.StartLong = 0;
			MnuTimer = State.TimerSec + MENU_TIMEOUT;
			Mnu[MnuLevel] = 0;
			MnuLevel --;
			Mnu[MnuLevel] = 0;
			//MnuLevuLevel] = 0;
			/**/
			DrawMenuFirstEntryBackground();DrawMenuFirstEntryTextImage();
			UpdateDisplayNow();
			//DisplayUpdate = 1;
		}
		else
		{

			/* Test if key up pressed */
			if (Key.UpShort != 0)
			{
				Key.UpShort = 0;
				MnuTimer = State.TimerSec + MENU_TIMEOUT;

				if ((int8_t)PswPtr - 4 >= 0)
				{
					PswPtr -= 4 ;
					DrawPasswordIcons(PswPtr);
					DisplayUpdate = 1;
				}
			}

			/* Test if key Left pressed */
			if (Key.MenuShort != 0)
			{
				Key.MenuShort = 0;
				MnuTimer = State.TimerSec + MENU_TIMEOUT;

				if ((int8_t)PswPtr != 0)
				{
					PswPtr -- ;
					DrawPasswordIcons(PswPtr);
					DisplayUpdate = 1;
				}

			}

			/* Test if key Down pressed */
			if (Key.DownShort != 0)
			{
				Key.DownShort = 0;
				MnuTimer = State.TimerSec + MENU_TIMEOUT;
				if (PswPtr + 4 < 12)
				{
					PswPtr += 4 ;
					DrawPasswordIcons(PswPtr);
					DisplayUpdate = 1;
				}
			}

			/* Test if key Right pressed */
			if (Key.RightShort != 0)
			{
				Key.RightShort = 0;
				MnuTimer = State.TimerSec + MENU_TIMEOUT;
				if (PswPtr < 12)
				{
					PswPtr ++;
					DrawPasswordIcons(PswPtr);
					DisplayUpdate = 1;
				}
			}
		}
	}

	if (Display.UpdateDateTime == 1)
	{
		UpdateDateTime();
		Display.UpdateDateTime = 0;
		DisplayUpdate = 1;
	}

	if (DisplayUpdate == 1)
	{
		DisplayUpdate = 0;
		UpdateDisplayNow();
	}
}

void LCDMenu_BootApp (void)
{

	if ((Key.MenuLong != 0) && (Key.UpLong != 0))
	{
		Key.MenuLong = 0;
		Key.UpLong = 0;
		MnuTimer = State.TimerSec + MENU_TIMEOUT;
		Mnu[MnuLevel] = 0;
		MnuLevel --;
		Mnu[MnuLevel] = 0;
		/**/
		ResetBootloaderApi();
		StopBootloaderChrono();

		if (MnuLevel == 1) { DrawMenuFirstEntryBackground();DrawMenuFirstEntryTextImage(); UpdateDisplayNow();}
		else DrawMenupartialTwo();

		return;
	}

	if ((Key.MenuShort != 0) && (InBootUpdate == 0))
	{
		Key.MenuShort = 0;
		MnuTimer = State.TimerSec + MENU_TIMEOUT;
		Mnu[MnuLevel] = 0;
		MnuLevel --;
		Mnu[MnuLevel] = 0;
		/**/
		ResetBootloaderApi();
		StopBootloaderChrono();

		if (MnuLevel == 1) { DrawMenuFirstEntryBackground();DrawMenuFirstEntryTextImage(); UpdateDisplayNow();}
		else DrawMenupartialTwo();

		return;
	}

	/**/

	DisplayUpdate = BootloaderApi();

	/**/

	if (DisplayUpdate == 1)
	{
		DisplayUpdate = 0;
		UpdateDisplayNow();
	}
}



void LcdBit(void)
{
	/* Screen for selection of bit values */
	/* t : indicates if continuous valid or not */
	/* Definition of call function */
	/* 2 bytes : mask of cursor index */
	/* 2 bytes : value of each bit */
	/* 16 bytes : letters of the choice */
	char c[21];                  /* 20 char + terminating null string */
	BYTE e;
	BYTE i;
	BYTE j;
	BYTE k;

	if (MnuLevel != 0)            /* Set this level to 0 if key left is pressed */
	{
		/* Loop on all menus to find the correct */
		e = 0;
		i = 0;                      /* Index on menu */
		while ((i < MnuNb) && (e == 0))
		{
			/* Print title m(l) = 0 */
			if ((Mnu[1] == MnuDef[i].Mnu[1]) && (Mnu[2] == MnuDef[i].Mnu[2]) &&
					(Mnu[3] == MnuDef[i].Mnu[3]) && (Mnu[4] == MnuDef[i].Mnu[4]) &&
					(Mnu[5] == MnuDef[i].Mnu[5]) && (MnuDef[i].Type == MNUBIT))
			{
				e = 1;
			}
			else
				i ++;
		}

		if (Key.StartShort != 0)             /* If key OK pressed, exit */
		{
			/* Stores MnuNumVal in c[] vor validation */
			c[2] = HighByte(MnuBitVal);
			c[3] = LowByte(MnuBitVal);
			/* Write new value */
			(*MnuDef[i].f)(0, (uchar *)&c[0]);
		}

		/* If key OK or exit key pressed */
		if ((Key.StartShort != 0) || (Key.StartLong != 0))
		{
			Key.StartShort = 0;
			Key.StartLong = 0;
			MnuTimer = State.TimerSec + MENU_TIMEOUT;
			Mnu[MnuLevel] = 0;
			MnuLevel --;
			Mnu[MnuLevel] = 0;
			/**/
			if (MnuLevel == 1) { DrawMenuFirstEntryBackground();DrawMenuFirstEntryTextImage(); UpdateDisplayNow();}
			else DrawMenupartialTwo();
		}
		else
		{
			if (MnuNumIndex == 16)    /* Forces to reload new values */
			{
				/* Read value */
				(*MnuDef[i].f)(1, (uchar *)&c[0]);
				MnuNumMask = c[0] * 256 + c[1];
				MnuBitVal = c[2] * 256 + c[3];    /* Value of bits */
				if (MnuNumMask != 0)
					MnuNumIndex = 0;
				j = 0;
				while (j < 16)              /* Stores c[] in MnuNumVal[] */
				{                         /* Invert string */
					MnuNumVal[j] = c[j + 4];
					j ++;
				}
			}

			/* Key left and right first */
			if (!(((WORD)1 << MnuNumIndex) & MnuNumMask))
			{
				j = 0;
				do
					j ++;
				while ((!(((WORD)1 << (MnuNumIndex + j)) & MnuNumMask)) && ((MnuNumIndex + j) < 16));
				if ((MnuNumIndex + j) < 16)
					MnuNumIndex += j;
			}

			if (Key.MenuShort != 0)           /* If key left is pressed, shift left */
			{
				Key.MenuShort = 0;
				MnuTimer = State.TimerSec + MENU_TIMEOUT;
				j = MnuNumIndex;
				do
					//j = j + 1;            /* Without rolling */
					j = (j + 1) % 16;     /* With rolling */
				while ((!(((WORD)1 << j) & MnuNumMask)) && (j != MnuNumIndex));
				MnuNumIndex = j;
				DisplayUpdate = 1;
			}

			if (Key.RightShort != 0)           /* If key right pressed, shift right */
			{
				Key.RightShort = 0;
				MnuTimer = State.TimerSec + MENU_TIMEOUT;
				j = MnuNumIndex;
				do
					if (j == 0)
						j = 15;
					else
						j = j - 1;
				while ((!(((WORD)1 << j) & MnuNumMask)) && (j != MnuNumIndex));
				MnuNumIndex = j;
				DisplayUpdate = 1;
			}

			/* Plus and minus invert the value */
			if ((Key.UpShort != 0) || (Key.DownShort != 0))
			{
				Key.UpShort = 0;
				Key.DownShort = 0;
				MnuTimer = State.TimerSec + MENU_TIMEOUT;
				MnuBitVal ^= (1 << MnuNumIndex);
				DisplayUpdate = 1;
			}

			/* Stores MnuNumVal[] in c[] */
			c[2] = HighByte(MnuBitVal);
			c[3] = LowByte(MnuBitVal);
			/* Stores MnuNumVal[] in c[] */
			j = 0;
			while (j < 16)
			{
				c[j + 4] = MnuNumVal[j];
				j ++;
			}

			/* Control new value */
			(*MnuDef[i].f)(2, (uchar *)&c[0]);
			/* Print MnuNumVal[] at the beginning of the line */

			if (DisplayUpdate == 1)
			{
				DrawDataInputBackground();
				DrawLine2(0,0,(char *)&ListWord16[0][LANG][0],0);
				for(k = 0;k < MNUMAXLINES;k++)
				{
					Putchar(180+(k+9)*18,83,(char *)&ListWord16_2[0][LANG][k],0);
				}
				/*Put a line?*/
						j = 0;
						do
						{
							if (j == (15 - MnuNumIndex))
							{
								if (MnuBitVal & (1 << (15 - j)))
								{
									c[0] = 36;
									Cursor(180+j*18,110,(RGBConvert(180, 180, 180)));
									Putchar(180+j*18,110,&c[0],0);
								}
								else
								{
									c[0] = 32;
									Cursor(180+j*18,110,(RGBConvert(180, 180, 180)));
									Putchar(180+j*18,110,&c[0],1);
								}
							}
							else if (MnuBitVal & (1 << (15 - j)))
							{
								c[0] = 36;
								Putchar(180+j*18,110,&c[0],0);
							}
							else
							{
								c[0] = 32;
								Putchar(180+j*18,110,&c[0],0);
							}
							j ++;
						}
						while (j < 16);

						DisplayUpdate = 0;
						UpdateDisplayNow();
			}
		}
	}
}




void DrawMenuFirstEntryTextImage (void)
{
	BYTE i;
	BYTE j;                      /* Counter on items */

	/* Counts the number of VALID items */
	j = 0;
	i = 0;                      /* Index on menu */
	MnuValidNb = 0;             /* Counter of menus */
	while (i < MnuNb)
	{
		if 	((j+1 == MnuDef[i].Mnu[1]) && (0 == MnuDef[i].Mnu[2]) &&
				(0 == MnuDef[i].Mnu[3]) && (0 == MnuDef[i].Mnu[4]) &&
				(0 == MnuDef[i].Mnu[5]) /*&& ((MnuDef[i].Mask & State.Mask) == State.Mask)*/)
		{

			j++;
			if (((MnuDef[i].Mask & State.Mask) == State.Mask))
			{
				MnuValidNb ++;

				if (MnuValidNb == MnuListCur[1])
				{
					DrawMenuFirstEnrtyPartial(MnuValidNb-1,255,(IMAGE_EXTERNAL *)MnuDef[i].SelectedBig);
					DrawMenuFirstEntryPartialSelected((IMAGE_EXTERNAL *)MnuDef[i].NormalSmall);
					DrawMenuFirstEnrtyText(MnuValidNb-1,(char*)&(MnuDef[i].Text[LANG][0]),1);
				}
				else
				{
					DrawMenuFirstEnrtyPartial(MnuValidNb-1,255,(IMAGE_EXTERNAL *)MnuDef[i].NormalBig);
					DrawMenuFirstEnrtyText(MnuValidNb-1,(char *)&(MnuDef[i].Text[LANG][0]),0);
				}
				if (MnuValidNb == MnuListCur[1])
					MnuListLevel = MnuDef[i].Mnu[1];
			}
		}
		i ++;
	}
	//UpdateDisplayNow();
}



BYTE IsPasswordMenu (void)
{
	BYTE i;
	BYTE j;                      /* Counter on items */

	/* Counts the number of VALID items */
	j = 0;
	i = 0;                      /* Index on menu */
	MnuValidNb = 0;             /* Counter of menus */
	while (i < MnuNb)
	{
		if 	((j+1 == MnuDef[i].Mnu[1]) && (0 == MnuDef[i].Mnu[2]) &&
				(0 == MnuDef[i].Mnu[3]) && (0 == MnuDef[i].Mnu[4]) &&
				(0 == MnuDef[i].Mnu[5]) /*&& ((MnuDef[i].Mask & State.Mask) == State.Mask)*/)
		{
			j++;
			if ((MnuDef[i].Mask & State.Mask) == State.Mask)
			{
				MnuValidNb ++;

				if (MnuValidNb == MnuListCur[1])
				{
					if (MnuDef[i].Type == MNUPSW)
					{
						return 1;
						break;
					}
					//DrawSecondEntryText((char*)&(MnuDef[i].Text[LANG][0]));
					//DrawSecondEntryImage(MnuValidNb-1,255,(IMAGE_EXTERNAL *)MnuDef[i].SelectedSmall);
				}
				else
				{
					//DrawSecondEntryImage(255,MnuValidNb-1,(IMAGE_EXTERNAL *)MnuDef[i].NormalSmall);
				}
				if (MnuValidNb == MnuListCur[1])
					MnuListLevel = MnuDef[i].Mnu[1];
			}
		}
		i ++;
	}
	return 0;
}



void DrawMenupartialOne (void)
{
	BYTE i;
	BYTE j;                      /* Counter on items */

	/* Counts the number of VALID items */
	j = 0;
	i = 0;                      /* Index on menu */
	MnuValidNb = 0;             /* Counter of menus */
	while (i < MnuNb)
	{
		if 	((j+1 == MnuDef[i].Mnu[1]) && (0 == MnuDef[i].Mnu[2]) &&
				(0 == MnuDef[i].Mnu[3]) && (0 == MnuDef[i].Mnu[4]) &&
				(0 == MnuDef[i].Mnu[5]) /*&& ((MnuDef[i].Mask & State.Mask) == State.Mask)*/)
		{
			j++;
			if ((MnuDef[i].Mask & State.Mask) == State.Mask)
			{
				MnuValidNb ++;

				if (MnuValidNb == MnuListCur[1])
				{
					//if (MnuDef[i].Type != MNUPASSWORD)
					DrawSecondEntryText((char*)&(MnuDef[i].Text[LANG][0]));
					//DrawSecondEntryImage(MnuValidNb-1,255,(IMAGE_EXTERNAL *)MnuDef[i].SelectedSmall);
				}
				else
				{
					//DrawSecondEntryImage(255,MnuValidNb-1,(IMAGE_EXTERNAL *)MnuDef[i].NormalSmall);
				}
				if (MnuValidNb == MnuListCur[1])
					MnuListLevel = MnuDef[i].Mnu[1];
			}
		}
		i ++;
	}
}

void DrawMenuPartialOneList(void)
{
	BYTE i;
	BYTE j;
	BYTE t;

	/* Clear Prev Image */
	DrawListBackground();

	j = 0;
	i = 1;
	t = 0;
	while (i < MnuNb)
	{
		if ((MnuDef[i].Mnu[1] == MnuListLevel) &&
				(MnuDef[i].Mnu[2] == j) &&
				(MnuDef[i].Mnu[3] == 0) &&
				(MnuDef[i].Mnu[4] == 0) &&
				(MnuDef[i].Mnu[5] == 0))
		{
			j++;
			if ((MnuDef[i].Mask & State.Mask) == State.Mask)
			{
				/* Title */
				if (t == 0)
				{
					DrawLine(t,0,(char*)&(MnuDef[i].Text[LANG][0]),0x0002);
					t++;
				}
				else
				{
					if ((MnuDef[i].Mask & State.Mask) == State.Mask)
						/* Option */
						{
						DrawLine(t,0,(char*)&(MnuDef[i].Text[LANG][0]),0x0000);
						t++;
						}
				}

				if (t > 6)
				{
					break;
				}
			}
		}
		i++;
	}
	UpdateDisplayNow();
}


void DrawMenupartialTwo (void)
{

	/* Mnu[] indicates top of list or, at end, the choice */
	BYTE i;
	BYTE j;                      /* Counter on items */
	BYTE m[6];
	BYTE v;
	BYTE mSave;
	BYTE textbuff[30];
	BYTE z,k,p,q;
	BYTE RLSlider = 0;


	WORD Status;

	/* Loop on all menus */
	m[1] = Mnu[1];
	m[2] = Mnu[2];
	m[3] = Mnu[3];
	m[4] = Mnu[4];
	m[5] = Mnu[5];

	m[MnuLevel] = 0;
	j = 0;                      /* No item counted */
	i = 0;                      /* Index on menu */
	while (i < MnuNb)
	{
		/* Search and print title */
		if 	((m[1] == MnuDef[i].Mnu[1]) && (m[2] == MnuDef[i].Mnu[2]) &&
				(m[3] == MnuDef[i].Mnu[3]) && (m[4] == MnuDef[i].Mnu[4]) &&
				(m[5] == MnuDef[i].Mnu[5]) && (m[MnuLevel] == 0))
		{
			/* Clears display */
			DrawListBackground();
			p = 1;
			if (MnuLevel > 2)
			{
				//j++;
				p = 0;
				DrawLine(0,MnuLevel+1, (char*)&(MnuDef[i].Text[LANG][0]),0x0002);
			}
			m[MnuLevel] = 1;
		}
		else
		{ /* Print menu items */
			if (m[MnuLevel] <= MnuDef[i].Mnu[MnuLevel])
			{
				mSave = m[MnuLevel];
				m[MnuLevel] = MnuDef[i].Mnu[MnuLevel];
				if 	((m[1] == MnuDef[i].Mnu[1]) && (m[2] == MnuDef[i].Mnu[2]) &&
						(m[3] == MnuDef[i].Mnu[3]) && (m[4] == MnuDef[i].Mnu[4]) &&
						(m[5] == MnuDef[i].Mnu[5]) && ((MnuDef[i].Mask & State.Mask) == State.Mask))
				{
					/* Search */
					j ++;

					if (MnuDef[i].Type == MNUVAL) {q = 1; MnuValCorrection = 2;} else {q = 1; MnuValCorrection = 0;}

					if ((MnuListOff[MnuLevel] < j) && (j <= (MnuListOff[MnuLevel] + (MNUMAXLINES - q) + p)))
					{

						/* Save current level of selected item */
						if (MnuListCur[MnuLevel] == 0)      /* MnuListCur has not been fixed : fix to 1 */
							MnuListCur[MnuLevel] = 1;

						if (MnuDef[i].Type == MNUVAL)
							(*MnuDef[i].f)(1, &v);  /* Get value of menu */

						Status = 0;
						/* Case of selected (current) menu */
						if ((v == MnuDef[i].Val) && (MnuDef[i].Type == MNUVAL))
						{
							Status = 0x1000;

							if (MnuListCur[MnuLevel] == 0)
							{
								MnuListCur[MnuLevel] = j;
							}
						}

						if (MnuDef[i].Type == MNUVAL)
						{
							if (j != MnuListCur[MnuLevel])
							{
								DrawLine2((j - MnuListOff[MnuLevel])-p,MnuLevel+1, (char*)&(MnuDef[i].Text[LANG][0]),0x0000 | Status);
								RLSlider = 1;
							}
							else
							{
								DrawLine2((j - MnuListOff[MnuLevel])-p,MnuLevel+1, (char*)&(MnuDef[i].Text[LANG][0]),0x0001 | Status);
								RLSlider = 1;
							}
						}
						else
						{
							/* Case of active (in charger) menu */
							if (j != MnuListCur[MnuLevel])
							{
								DrawLine((j - MnuListOff[MnuLevel])-p,MnuLevel+1, (char*)&(MnuDef[i].Text[LANG][0]),0x0000);

								if ((MnuDef[i].Type == MNULIST))
								{
									z = i+1;

									for (k = 0;k<10;k++)
									{
										if ((0 == MnuDef[z].Mnu[MnuLevel+1]))
										{
											/*No more value in the list*/
											break;
										}
										else
										{
											(*MnuDef[z].f)(1, &v);
											if (v == MnuDef[z].Val)
											{
												/*Find the value*/
												DrawLineAdd((j - MnuListOff[MnuLevel])-p,(char*)&(MnuDef[z].Text[LANG][0]));
												break;
											}
											/*Try next*/
										}
										z++;
									}
								}
								else if ((MnuDef[i].Type == MNUNUM) || (MnuDef[i].Type == MNUALPHA0))
								{

									(*MnuDef[i].f)(1, (uchar *)&textbuff);  /* Get value of menu */
									DrawLineAdd((j - MnuListOff[MnuLevel])-p,(char *)&textbuff[2]);
								}

							}
							else
							{
								DrawLine((j - MnuListOff[MnuLevel])-p,MnuLevel+1, (char*)&(MnuDef[i].Text[LANG][0]),0x0001);
								/*Prelist selections, values*/
								if ((MnuDef[i].Type == MNULIST))
								{
									z = i+1;

									for (k = 0;k<10;k++)
									{
										if ((0 == MnuDef[z].Mnu[MnuLevel+1]))
										{
											/*No more value in the list*/
											break;
										}
										else
										{
											(*MnuDef[z].f)(1, &v);
											if (v == MnuDef[z].Val)
											{
												/*Find the value*/
												DrawLineAdd((j - MnuListOff[MnuLevel])-p,(char*)&(MnuDef[z].Text[LANG][0]));
												break;
											}
											/*Try next*/
										}
										z++;
									}
								}
								else if ((MnuDef[i].Type == MNUNUM) || (MnuDef[i].Type == MNUALPHA0))
								{
									(*MnuDef[i].f)(1, (uchar *)&textbuff);  /* Get value of menu */
									DrawLineAdd((j - MnuListOff[MnuLevel])-p,(char*)&textbuff[2]);
								}
							}
						}
						if (j == MnuListCur[MnuLevel])
							MnuListLevel = MnuDef[i].Mnu[MnuLevel];
					}
					m[MnuLevel] ++;
				}
				else
					m[MnuLevel] = mSave;
			}
		}
		i ++;
	}

	/* Draw Slider */
	if (MnuLevel>2)
	{
		if (MnuListOff[MnuLevel] != 0)
			DrawSlider(MnuValidNb,MnuListOff[MnuLevel]-1,RLSlider);
		else
			DrawSlider(MnuValidNb,MnuListOff[MnuLevel],RLSlider);
	}
	else
	{
		DrawSlider(MnuValidNb,MnuListOff[MnuLevel],RLSlider);
	}

	UpdateDisplayNow();
}

void SaveCurrentSelection (void)
{
	BYTE i;
	BYTE j;                      /* Counter on items */
	BYTE m[6];

	BYTE mSave;

	/* Loop on all menus */
	m[1] = Mnu[1];
	m[2] = Mnu[2];
	m[3] = Mnu[3];
	m[4] = Mnu[4];
	m[5] = Mnu[5];

	m[MnuLevel] = 0;
	j = 0;                      /* No item counted */
	i = 0;                      /* Index on menu */
	while (i < MnuNb)
	{
		/* Search and print title */
		if 	((m[1] == MnuDef[i].Mnu[1]) && (m[2] == MnuDef[i].Mnu[2]) &&
				(m[3] == MnuDef[i].Mnu[3]) && (m[4] == MnuDef[i].Mnu[4]) &&
				(m[5] == MnuDef[i].Mnu[5]) && (m[MnuLevel] == 0))
		{

		}
		else
		{        /* Print menu items */
			if (m[MnuLevel] <= MnuDef[i].Mnu[MnuLevel])
			{
				mSave = m[MnuLevel];
				m[MnuLevel] = MnuDef[i].Mnu[MnuLevel];
				if 	((m[1] == MnuDef[i].Mnu[1]) && (m[2] == MnuDef[i].Mnu[2]) &&
						(m[3] == MnuDef[i].Mnu[3]) && (m[4] == MnuDef[i].Mnu[4]) &&
						(m[5] == MnuDef[i].Mnu[5]) && ((MnuDef[i].Mask & State.Mask) == State.Mask))
				{
					/* Search */
					j ++;
					if ((MnuListOff[MnuLevel] < j) && (j <= (MnuListOff[MnuLevel] + MNUMAXLINES)))
					{

						if (MnuDef[i].Type == MNUVAL)
							if ((MnuListCur[MnuLevel] == j) && (EntryEvent != 1))
							{
								/* Write new value */
								(*MnuDef[i].f)(0, (BYTE *)&MnuDef[i].Val);
							}
					}
					m[MnuLevel] ++;
				}
				else
					m[MnuLevel] = mSave;
			}
		}
		i ++;
	}
}

void CalcValidMnuNumber (void)
{
	BYTE i;
	BYTE m[6];
	BYTE mSave;

	/* Set value of searched item */
	m[1] = Mnu[1];
	m[2] = Mnu[2];
	m[3] = Mnu[3];
	m[4] = Mnu[4];
	m[5] = Mnu[5];
	m[MnuLevel] = 0;
	/* Counts the number of VALID items */
	i = 0;                      /* Index on menu */
	MnuValidNb = 0;             /* Counter of menus */
	while (i < MnuNb)
	{
		if (m[MnuLevel] < MnuDef[i].Mnu[MnuLevel])
		{                       /* A menu of higher order found */
			mSave = m[MnuLevel];
			m[MnuLevel] = MnuDef[i].Mnu[MnuLevel];
			if ((m[1] == MnuDef[i].Mnu[1]) && (m[2] == MnuDef[i].Mnu[2]) &&
					(m[3] == MnuDef[i].Mnu[3]) && (m[4] == MnuDef[i].Mnu[4]) &&
					(m[5] == MnuDef[i].Mnu[5]) && ((MnuDef[i].Mask & State.Mask) == State.Mask))
			{
				MnuValidNb ++;
			}
			else
				m[MnuLevel] = mSave;
		}
		i ++;
	}
}

BYTE CheckOkPress (void)
{
	BYTE i,e = 0;
	BYTE m[6];

	/* Set value of searched item */
	m[1] = Mnu[1];
	m[2] = Mnu[2];
	m[3] = Mnu[3];
	m[4] = Mnu[4];
	m[5] = Mnu[5];

	/* Search current menu type  */
	i = 0;
	while ((i < MnuNb) && (e == 0))
	{
		if 	((m[1] == MnuDef[i].Mnu[1]) && (m[2] == MnuDef[i].Mnu[2]) &&
				(m[3] == MnuDef[i].Mnu[3]) && (m[4] == MnuDef[i].Mnu[4]) &&
				(m[5] == MnuDef[i].Mnu[5]))
		{
			if (MnuDef[i].Type != MNULIST)
				/* Test if key OK is pressed to enter menu */
				if (Key.StartShort != 0)
				{
					Mnu[MnuLevel] = MnuListLevel;
					MnuLevel ++;
					MnuListCur[MnuLevel] = 1;     /* Force to reload choice in next menu */
					MnuListOff[MnuLevel] = 0;     /* No offset in new menu */
					MnuNumIndex = 16;             /* Indicates to load new value */
					Mnu[MnuLevel] = 0;
					e = 1;
					EntryEvent = 1;                   /* Can exit and reload MnuChrono */
				}
		}
		i ++;
	}
	return e;
}

void LCDMenu_Liste (void)
{
	BYTE e;                      /* Exit flag */
	e = 0;
	TimerMnuOption = 0;           /* Reset timer for options display */


	/* Test if key left pressed (return to previous menu) */
	if (MnuLevel != 0)
	{
		/*Exit from menu hold Right key long*/
		if (MnuLevel == 1)
		{
			if ((Key.MenuLong != 0))	/*Prevent Exit from menu, when enter*/
			{
				//Mnu[MnuLevel] = 0;
				//MnuLevel --;
				//Mnu[MnuLevel] = 0;
				//Key.MenuLong = 0;
				//Key.MenuShort = 0;
				LCDMenu_Level();
				KEY_Reset();
				Key.MenuReset = 1;
				//MnuTimer = 0;
			}
		}
		else
		{

			if ((Key.MenuLong != 0))	/*Prevent Exit from menu, when enter*/
			{
				LCDMenu_Level();
				KEY_Reset();
				Key.MenuReset = 1;
				//MnuTimer = 0;
			}

			/*Return to previous menu, push and release LEFT button*/
			if (Key.MenuShort != 0)
			{
				Key.MenuShort = 0;
				Mnu[MnuLevel] = 0;
				MnuLevel --;
				Mnu[MnuLevel] = 0;
				if (MnuLevel != 0)
				{
					if (MnuLevel == 1)
					{
						DrawMenuFirstEntryBackground();
						DrawMenuFirstEntryTextImage();
						UpdateDisplayNow();
						//
						//				  DrawMenupartialOne();
						//				  DrawMenuPartialOneList();
					}
					else
					{
						CalcValidMnuNumber();
						DrawMenupartialTwo();
					}
				}
				KEY_Reset();
			}
		}
	}

	/* A menu is printed */

	if (MnuLevel != 0)
	{
		if (MnuLevel == 1)
		{
			if (FirstEntry == 1)
			{
				e = CheckOkPress();
				/* Test if key OK pressed to valid selected menu */

				if (Key.StartShort != 0)
				{
					KEY_Reset();
					Key.MenuLong = 0;
					FirstEntry = 1;
					MnuTimer = State.TimerSec + MENU_TIMEOUT;
					Key.StartShort = 0;
					DisplayUpdate = 1;

					if (IsPasswordMenu() != 1)
					{
						DrawSecondEntryBackground();
						//UpdateDisplayNow();
						DrawMenupartialOne();
						//UpdateDisplayNow();
						CalcValidMnuNumber();
						DrawMenupartialTwo();
					}
				}

				if (e == 0)
				{


					if (Display.UpdateDateTime == 1)
					{
						UpdateDateTime();
						Display.UpdateDateTime = 0;
						DisplayUpdate = 1;
					}

					/* Test if key up pressed */


					if (Key.UpShort != 0)
					{
						Key.UpShort = 0;
						MnuTimer = State.TimerSec + MENU_TIMEOUT;

						if (1 < MnuListCur[MnuLevel])
						{
							MnuListCur[MnuLevel] --;
							DrawMenuFirstEntryTextImage();
							DisplayUpdate = 1;
						}
					}

					/* Test if key up pressed */
					if (Key.MenuShort != 0)
					{
						Key.MenuShort = 0;

						MnuTimer = State.TimerSec + MENU_TIMEOUT;

						if (1 < MnuListCur[MnuLevel])
						{
							if (((int8_t)MnuListCur[MnuLevel]-3)> 0)
							{
								MnuListCur[MnuLevel] -= 3;
								DrawMenuFirstEntryTextImage();
								DisplayUpdate = 1;
							}
						}
					}

					/* Test if key RIGHT pressed */
					if (Key.DownShort != 0)
					{
						Key.DownShort = 0;

						MnuTimer = State.TimerSec + MENU_TIMEOUT;
						if (MnuListCur[MnuLevel] < MnuValidNb)
						{
							MnuListCur[MnuLevel] ++;
							DrawMenuFirstEntryTextImage();
							DisplayUpdate = 1;
						}
					}

					/* Test if key rigth pressed */
					if (Key.RightShort != 0)
					{
						Key.RightShort = 0;

						MnuTimer = State.TimerSec + MENU_TIMEOUT;
						if (MnuListCur[MnuLevel] < MnuValidNb)
						{
							if ((MnuListCur[MnuLevel]+3) <= MnuValidNb)
							{
								MnuListCur[MnuLevel] += 3;
								DrawMenuFirstEntryTextImage();
								DisplayUpdate = 1;
							}
						}
					}


					if (DisplayUpdate == 1)
					{
						DisplayUpdate = 0;
						UpdateDisplayNow();
					}


				}




			}
			else
			{
				e = CheckOkPress();

				/* Test if key OK pressed to valid selected menu */
				if (Key.StartShort != 0)
				{
					KEY_Reset();
					MnuTimer = State.TimerSec + MENU_TIMEOUT;
					Key.StartShort = 0;
					DisplayUpdate = 1;
					CalcValidMnuNumber();
					DrawMenupartialTwo();
				}

				if (e == 0)
				{

					/* Test if key LEFT pressed */
					if (Key.MenuShort != 0)
					{
						Key.MenuShort = 0;
						MnuTimer = State.TimerSec + MENU_TIMEOUT;

						if (1 < MnuListCur[MnuLevel])
						{
							MnuListCur[MnuLevel] --;
							DrawMenupartialOne();
							DrawMenuPartialOneList();
						}
					}

					/* Test if key RIGHT pressed */
					if (Key.RightShort != 0)
					{
						Key.RightShort = 0;
						MnuTimer = State.TimerSec + MENU_TIMEOUT;
						if (MnuListCur[MnuLevel] < MnuValidNb)
						{
							MnuListCur[MnuLevel] ++;
							DrawMenupartialOne();
							DrawMenuPartialOneList();
						}
					}
				}
			}
		}
		else
		{
			/* Test if key OK pressed to valid selected menu */
			e = CheckOkPress();

			/* Test if key OK pressed to valid selected menu */
			if (Key.StartShort != 0)
			{
				KEY_Reset();
				Key.StartShort = 0;
				MnuTimer = State.TimerSec + MENU_TIMEOUT;
				DisplayUpdate = 1;
				if (EntryEvent == 0)
				{
					SaveCurrentSelection();		/*Save only when neede*/
				}
				EntryEvent = 0;
				CalcValidMnuNumber();		/*Te get normal slider*/
				DrawMenupartialTwo();
			}

			if (e == 0)
			{
				CalcValidMnuNumber();

				/* Test if key up pressed */
				if (Key.UpShort != 0)
				{
					Key.UpShort = 0;
					EntryEvent = 0;
					MnuTimer = State.TimerSec + MENU_TIMEOUT;
					if (1 < MnuListCur[MnuLevel])
					{
						MnuListCur[MnuLevel] --;

						if (MnuLevel > 2)
						{
							if ((MnuListOff[MnuLevel] != 0) && (MnuListCur[MnuLevel] == (MnuListOff[MnuLevel] + 1)))
								MnuListOff[MnuLevel] --;
						}
						else
						{
							if ((MnuListOff[MnuLevel] != 0) && (MnuListCur[MnuLevel] == (MnuListOff[MnuLevel] + 1)))
								MnuListOff[MnuLevel] --;
						}
						DrawMenupartialTwo();
					}
				}

				/* Test if key down pressed */
				if (Key.DownShort != 0)
				{
					Key.DownShort = 0;
					EntryEvent = 0;
					MnuTimer = State.TimerSec + MENU_TIMEOUT;
					if (MnuListCur[MnuLevel] < MnuValidNb)
					{
						MnuListCur[MnuLevel] ++;

						if (MnuLevel > 2)
						{
							if (((MnuListOff[MnuLevel] + MNUMAXLINES-1 - MnuValCorrection) != MnuValidNb) && ((MnuListOff[MnuLevel] + MNUMAXLINES-1 /* - MnuValCorrection*/) <= MnuListCur[MnuLevel]))
								MnuListOff[MnuLevel] ++;
						}
						else
						{
							if (((MnuListOff[MnuLevel] + MNUMAXLINES - MnuValCorrection) != MnuValidNb) && ((MnuListOff[MnuLevel] + MNUMAXLINES /* - MnuValCorrection*/) <= MnuListCur[MnuLevel]))
								MnuListOff[MnuLevel] ++;
						}
						DrawMenupartialTwo();
					}
				}
			}
		}
	}
}



/**
 * @brief	Display function list
 * @param  None
 * @retval None
 */
void LCDMenu_Fun(void)
{
	/* Display a function list box */
	/* Mnu[] indicates top of list or, at end, the choice */
	/* MnuMemoVal : current choice */
	/* v[0], v[1] : number of choices */
	BYTE e;                      /* Exit flag */
	BYTE i;
	BYTE j;                      /* Counter on items */
	BYTE m[6];
	BYTE v[32];                  /* 1 WORD + 20 char + terminating null string */

	TimerMnuOption = 0;           /* Reset timer for options display */

	/* Test if key left pressed (return to previous menu) */
	if (MnuLevel != 0)              /* Set this level to 0 if key left is pressed */
		if (Key.MenuShort != 0)
		{
			Key.MenuShort = 0;
			MnuTimer = State.TimerSec + MENU_TIMEOUT;
			Mnu[MnuLevel] = 0;
			MnuLevel --;
			Mnu[MnuLevel] = 0;
			/**/

			CalcValidMnuNumber();
			if (MnuLevel == 1)
			{
				DrawMenuFirstEntryBackground();
				DrawMenuFirstEntryTextImage();
				UpdateDisplayNow();
			}
			else
				DrawMenupartialTwo();
		}
		else
		{
			/* Set value of searched item */
			m[1] = Mnu[1];
			m[2] = Mnu[2];
			m[3] = Mnu[3];
			m[4] = Mnu[4];
			m[5] = Mnu[5];
			/* Search current menu type  */
			e = 0;
			i = 0;
			while ((i < MnuNb) && (e == 0))
			{
				if ((m[1] == MnuDef[i].Mnu[1]) && (m[2] == MnuDef[i].Mnu[2]) &&
						(m[3] == MnuDef[i].Mnu[3]) && (m[4] == MnuDef[i].Mnu[4]) &&
						(m[5] == MnuDef[i].Mnu[5]))
				{
					if (MnuDef[i].Type == MNUFUNLIST)
						/* Test if key right is pressed and MnulistCur != 0 to enter menu */
						if ((Key.StartShort != 0) && (MnuListCur[MnuLevel] != 0) && (MnuValidNb != 0))
						{
							/* Save current memo number ( 0 to ...) */
							MnuMemoVal = MnuListCur[MnuLevel] - 1;
							/* Enter next menu */
							Key.StartShort = 0;
							MnuTimer = State.TimerSec + MENU_TIMEOUT;
							Mnu[MnuLevel] = 1; //MnuListLevel;
							MnuLevel ++;
							MnuListCur[MnuLevel] = 0;     /* Force to reload choice in next menu */
							MnuListOff[MnuLevel] = 0;     /* No offset in new menu */
							Mnu[MnuLevel] = 0;
							e = 1;              /* Can exit and reload MnuChrono */
						}
				}
				i ++;
			}

			if (e == 0)
			{
				/* Set value of searched item */
				m[1] = Mnu[1];
				m[2] = Mnu[2];
				m[3] = Mnu[3];
				m[4] = Mnu[4];
				m[5] = Mnu[5];
				m[MnuLevel] = 0;
				/* Loop on menus */
				m[MnuLevel] = 0;
				e = 0;
				j = 0;                      /* No item counted */
				i = 0;                      /* Index on menu */
				while ((i < MnuNb) && (e == 0))
				{
					/* Search and print title */
					if ((m[1] == MnuDef[i].Mnu[1]) && (m[2] == MnuDef[i].Mnu[2]) &&
							(m[3] == MnuDef[i].Mnu[3]) && (m[4] == MnuDef[i].Mnu[4]) &&
							(m[5] == MnuDef[i].Mnu[5]) && (m[MnuLevel] == 0))
					{

						DrawDataInputBackground();
						m[MnuLevel] = 1;
						e = 1;
					}
					i ++;
				}

				/* Read number of items */
				(*MnuDef[i - 1].f)(MnuListOff[MnuLevel] + j, &v[0]);
				MnuValidNb = ((WORD)v[0] << 8) + v[1];
				/* Test if short key up pressed */
				if (Key.UpShort != 0)
				{
					Key.UpShort = 0;
					MnuTimer = State.TimerSec + MENU_TIMEOUT;
					if (1 < MnuListCur[MnuLevel])
					{
						MnuListCur[MnuLevel] --;
						if ((MnuListOff[MnuLevel] != 0) && (MnuListCur[MnuLevel] == (MnuListOff[MnuLevel] + 1)))
							MnuListOff[MnuLevel] --;
					}
				}

				/* Test if short key down pressed */
				if (Key.DownShort != 0)
				{
					Key.DownShort = 0;
					MnuTimer = State.TimerSec + MENU_TIMEOUT;
					if (MnuListCur[MnuLevel] < MnuValidNb)
					{
						MnuListCur[MnuLevel] ++;
						if (((MnuListOff[MnuLevel] + MNUMAXLINES) != MnuValidNb) && ((MnuListOff[MnuLevel] + MNUMAXLINES) <= MnuListCur[MnuLevel]))
							MnuListOff[MnuLevel] ++;
					}
				}

				/* Test if long key up pressed */
				if (Key.UpLong != 0)
				{
					Key.UpLong = 0;
					MnuTimer = State.TimerSec + MENU_TIMEOUT;
					MnuListCur[MnuLevel] = 0;
					MnuListOff[MnuLevel] = 0;
				}

				/* Test if Long key down pressed */
				if (Key.DownLong != 0)
				{
					Key.DownLong = 0;
					MnuTimer = State.TimerSec + MENU_TIMEOUT;
					MnuListCur[MnuLevel] = MnuValidNb;
					MnuListOff[MnuLevel] = MnuValidNb - MNUMAXLINES;
				}

				/* Loop on functions */
				if (MnuListCur[MnuLevel] == 0)
					MnuListCur[MnuLevel] = 1;
				j = 0;                      /* Index on menu */
				while ((j < MNUMAXLINES) && (j < MnuValidNb))
				{
					/* Read value */
					(*MnuDef[i - 1].f)(MnuListOff[MnuLevel] + j, &v[0]);
					j ++;
					/* Case of selected (current) menu */
					if (j != (MnuListCur[MnuLevel] - MnuListOff[MnuLevel]))
					{
						DrawLine2Status(j-1,0,(char*)&v[2],0x0000| LONGTEXT);
					}
					else if (MnuValidNb != 0)
					{
						DrawLine2Status(j-1,0,(char*)&v[2],0x0001 | LONGTEXT);
					}
					else
					{
						DrawLine2Status(j-1,0,(char*)&v[2],0x0000 | LONGTEXT);
					}
				}

				/* Display up arrow at end of line*/
				DrawSlider(MnuValidNb,MnuListOff[MnuLevel],1);
			}
		}
	UpdateDisplayNow();
}



/**
 * @brief	Display function list
 * @param  None
 * @retval None
 */
void LCDMenu_FunStatus(void)
{
	/* Display a function list box */
	/* Mnu[] indicates top of list or, at end, the choice */
	/* MnuMemoVal : current choice */
	/* v[0], v[1] : number of choices */
	BYTE e;                      /* Exit flag */
	BYTE i;
	BYTE j;                      /* Counter on items */
	BYTE m[6];
	BYTE v[23];                  /* 1 WORD + 20 char + terminating null string */

	/* Test if key left pressed (return to previous menu) */
	if (MnuLevel != 0)              /* Set this level to 0 if key left is pressed */
		if (Key.MenuShort != 0)
		{
			Key.MenuShort = 0;
			MnuTimer = State.TimerSec + MENU_TIMEOUT;
			Mnu[MnuLevel] = 0;
			MnuLevel --;
			Mnu[MnuLevel] = 0;
			/**/
			CalcValidMnuNumber();
			if (MnuLevel == 1) {DrawMenuFirstEntryBackground(); DrawMenuFirstEntryTextImage();UpdateDisplayNow();}
			else	DrawMenupartialTwo();
		}
		else
		{
			/* Set value of searched item */
			m[1] = Mnu[1];
			m[2] = Mnu[2];
			m[3] = Mnu[3];
			m[4] = Mnu[4];
			m[5] = Mnu[5];
			/* Search current menu type  */
			e = 0;
			i = 0;
			while ((i < MnuNb) && (e == 0))
			{
				if ((m[1] == MnuDef[i].Mnu[1]) && (m[2] == MnuDef[i].Mnu[2]) &&
						(m[3] == MnuDef[i].Mnu[3]) && (m[4] == MnuDef[i].Mnu[4]) &&
						(m[5] == MnuDef[i].Mnu[5]))
				{
					if (MnuDef[i].Type == MNUFUNLIST)
						/* Test if key right is pressed and MnulistCur != 0 to enter menu */
						if ((Key.StartShort != 0) && (MnuListCur[MnuLevel] != 0) && (MnuValidNb != 0))
						{
							/* Save current memo number ( 0 to ...) */
							MnuMemoVal = MnuListCur[MnuLevel] - 1;
							/* Enter next menu */
							Key.StartShort = 0;
							MnuTimer = State.TimerSec + MENU_TIMEOUT;
							Mnu[MnuLevel] = 1; //MnuListLevel;
							MnuLevel ++;
							MnuListCur[MnuLevel] = 0;     /* Force to reload choice in next menu */
							MnuListOff[MnuLevel] = 0;     /* No offset in new menu */
							Mnu[MnuLevel] = 0;
							e = 1;              /* Can exit and reload MnuChrono */
						}
				}
				i ++;
			}

			if (e == 0)
			{
				/* Set value of searched item */
				m[1] = Mnu[1];
				m[2] = Mnu[2];
				m[3] = Mnu[3];
				m[4] = Mnu[4];
				m[5] = Mnu[5];
				m[MnuLevel] = 0;
				/* Loop on menus */
				m[MnuLevel] = 0;
				e = 0;
				j = 0;                      /* No item counted */
				i = 0;                      /* Index on menu */
				while ((i < MnuNb) && (e == 0))
				{
					/* Search and print title */
					if ((m[1] == MnuDef[i].Mnu[1]) && (m[2] == MnuDef[i].Mnu[2]) &&
							(m[3] == MnuDef[i].Mnu[3]) && (m[4] == MnuDef[i].Mnu[4]) &&
							(m[5] == MnuDef[i].Mnu[5]) && (m[MnuLevel] == 0))
					{

						DrawDataInputBackground();
						m[MnuLevel] = 1;
						e = 1;
					}
					i ++;
				}

				/* Read number of items */
				(*MnuDef[i - 1].f)(MnuListOff[MnuLevel] + j, &v[0]);
				MnuValidNb = ((WORD)v[0] << 8) + v[1];
				/* Test if short key up pressed */
				if (Key.UpShort != 0)
				{
					Key.UpShort = 0;
					MnuTimer = State.TimerSec + MENU_TIMEOUT;
					if (1 < MnuListCur[MnuLevel])
					{
						MnuListCur[MnuLevel] --;
						if ((MnuListOff[MnuLevel] != 0) && (MnuListCur[MnuLevel] == (MnuListOff[MnuLevel] + 1)))
							MnuListOff[MnuLevel] --;
					}
				}

				/* Test if short key down pressed */
				if (Key.DownShort != 0)
				{
					Key.DownShort = 0;
					MnuTimer = State.TimerSec + MENU_TIMEOUT;
					if (MnuListCur[MnuLevel] < MnuValidNb)
					{
						MnuListCur[MnuLevel] ++;
						if (((MnuListOff[MnuLevel] + MNUMAXLINES) != MnuValidNb) && ((MnuListOff[MnuLevel] + MNUMAXLINES) <= MnuListCur[MnuLevel]))
							MnuListOff[MnuLevel] ++;
					}
				}

				/* Test if long key up pressed */
				if (Key.UpLong != 0)
				{
					Key.UpLong = 0;
					MnuTimer = State.TimerSec + MENU_TIMEOUT;
					MnuListCur[MnuLevel] = 0;
					MnuListOff[MnuLevel] = 0;
				}

				/* Test if Long key down pressed */
				if (Key.DownLong != 0)
				{
					Key.DownLong = 0;
					MnuTimer = State.TimerSec + MENU_TIMEOUT;
					MnuListCur[MnuLevel] = MnuValidNb;
					MnuListOff[MnuLevel] = MnuValidNb - MNUMAXLINES;
				}

				/* Loop on functions */
				if (MnuListCur[MnuLevel] == 0)
					MnuListCur[MnuLevel] = 1;
				j = 0;                      /* Index on menu */
				while ((j < MNUMAXLINES) && (j < MnuValidNb))
				{
					/* Read value */
					(*MnuDef[i - 1].f)(MnuListOff[MnuLevel] + j, &v[0]);
					j ++;
					/* Case of selected (current) menu */
					if (j != (MnuListCur[MnuLevel] - MnuListOff[MnuLevel]))
					{
						DrawLine2Status(j-1,0,(char*)&v[2],0x0000| LONGTEXT);
					}
					else if (MnuValidNb != 0)
					{
						DrawLine2Status(j-1,0,(char*)&v[2],0x0001 | LONGTEXT);
					}
					else
					{
						DrawLine2Status(j-1,0,(char*)&v[2],0x0000 | LONGTEXT);
					}
				}

				/* Display up arrow at end of line*/
				DrawSlider(MnuValidNb,MnuListOff[MnuLevel],1);
			}
		}
	UpdateDisplayNow();

}




/**
 * @brief	Print decimal numeric, hexadecimal, alphanumeric, input screen
 * @param  None
 * @retval None
 */
void  LCDMenu_Input(uchar t)
{
	/* Print decimal numeric, hexadecimal, alphanumeric, input screen */
	/* t : indicates if continuous valid or not */
	/* Definition of call function */
	/* 2 bytes : mask of cursor index */
	/* 16 bytes : letters of the choice */
	BYTE v[21];                  /* 20 char + terminating null string */
	BYTE e;
	BYTE i;
	BYTE j;
	BYTE MnuNumOldVal;           /* Old value in case of error */


	if (MnuLevel != 0)            /* Set this level to 0 if key left is pressed */
	{
		/* Loop on all menus to find the correct */
		e = 0;
		i = 0;                      /* Index on menu */
		while ((i < MnuNb) && (e == 0))
		{
			/* Print title m(l) = 0 */
			if ((Mnu[1] == MnuDef[i].Mnu[1]) && (Mnu[2] == MnuDef[i].Mnu[2]) &&
					(Mnu[3] == MnuDef[i].Mnu[3]) && (Mnu[4] == MnuDef[i].Mnu[4]) &&
					(Mnu[5] == MnuDef[i].Mnu[5]) && (
							(MnuDef[i].Type == MNUNUM) || (MnuDef[i].Type == MNUNUMUPD) ||
							(MnuDef[i].Type == MNUHEXA) || (MnuDef[i].Type == MNUHEXAUPD) ||
							(MnuDef[i].Type == MNUALPHA0)))
			{
				e = 1;
			}
			else
				i ++;
		}

		if (Key.StartShort != 0)             /* If key OK pressed, exit */
		{
			/* Stores c[] in MnuNumVal[], invert string */
			j = 0;
			while (j < 16)
			{
				v[17 - j] = MnuNumVal[j];
				j ++;
			}
			v[18] = SPACE;            /* Add space char to stop sscanf */
			/* Write new value */
			(*MnuDef[i].f)(0, &v[0]);
		}

		/* If key OK or exit key pressed */
		if ((Key.StartShort != 0) || (Key.StartLong != 0))
		{
			Key.StartShort = 0;
			Key.StartLong = 0;
			MnuTimer = State.TimerSec + MENU_TIMEOUT;
			Mnu[MnuLevel] = 0;
			MnuLevel --;
			Mnu[MnuLevel] = 0;
			/**/
			if (MnuLevel == 1) {DrawMenuFirstEntryBackground(); DrawMenuFirstEntryTextImage();UpdateDisplayNow();}
			else DrawMenupartialTwo();
		}
		else
		{
			if (MnuNumIndex == 16)        /* Forces to reload new values */
			{
				/* Read value */
				(*MnuDef[i].f)(1, &v[0]);
				MnuNumMask = v[0] * 256 + v[1];
				if (MnuNumMask != 0)
					MnuNumIndex = 0;
				j = 0;
				while (j < 16)              /* Stores c[] in MnuNumVal[] */
				{                         /* Invert string */
					MnuNumVal[j] = v[17 - j];
					j ++;
				}
			}

			/* Key left and right first */
			/* Find a editable item */
			if (!(((WORD)1 << MnuNumIndex) & MnuNumMask))
			{
				j = MnuNumIndex;
				do
					j = (j + 1);
				while ((!(((WORD)1 << j) & MnuNumMask)) && (j < 16));
				if (j < 16)
					MnuNumIndex = j;
			}

			if ((Key.MenuShort != 0) && (MnuNumMask != 0))    /* If key left is pressed, shift left */
			{
				Key.MenuShort = 0;
				MnuTimer = State.TimerSec + MENU_TIMEOUT;
				j = MnuNumIndex;
				do
					j = (j + 1) % 16;     /* With rolling */
				while ((!(((WORD)1 << j) & MnuNumMask)) && (j != MnuNumIndex));
				MnuNumIndex = j;
				DisplayUpdate = 1;
			}

			if ((Key.RightShort != 0) && (MnuNumMask != 0))    /* If key right pressed, shift right */
			{
				Key.RightShort = 0;
				MnuTimer = State.TimerSec + MENU_TIMEOUT;
				j = MnuNumIndex;
				do
					if (j == 0)
						j = 15;
					else
						j = j - 1;
				while ((!(((WORD)1 << j) & MnuNumMask)) && (j != MnuNumIndex));
				MnuNumIndex = j;
				DisplayUpdate = 1;
			}

			/* Saves old value */
			MnuNumOldVal = MnuNumVal[MnuNumIndex];
			/* Plus and minus on the copy*/
			if (Key.UpShort != 0)           /* If key up pressed, increment value */
			{
				Key.UpShort = 0;
				MnuTimer = State.TimerSec + MENU_TIMEOUT;
				if (MnuNumVal[MnuNumIndex] == 43)           /* Case + sign : become - sign */
					MnuNumVal[MnuNumIndex] = 45;
				else if (MnuNumVal[MnuNumIndex] == 45)         /* Case - sign : become + sign */
					MnuNumVal[MnuNumIndex] = 43;
				else
				{
					switch (t)
					{
					case MNUNUM :
					case MNUNUMUPD :
						do
							MnuNumVal[MnuNumIndex] ++;
						while (isdigit(MnuNumVal[MnuNumIndex]) == 0);
						break ;
					case MNUHEXA :
					case MNUHEXAUPD :
						do
							MnuNumVal[MnuNumIndex] ++;
						while ((isxdigit(MnuNumVal[MnuNumIndex]) == 0) ||
								((isdigit(MnuNumVal[MnuNumIndex]) == 0) && (isupper(MnuNumVal[MnuNumIndex]) == 0)));
						break ;
					case MNUALPHA0 :
						do
							MnuNumVal[MnuNumIndex] ++;
						while ((isalnum(MnuNumVal[MnuNumIndex]) == 0) &&
								(MnuNumVal[MnuNumIndex] != SPACE) &&
								(MnuNumVal[MnuNumIndex] != UNDERS));
						break ;
					}
				}
				DisplayUpdate = 1;
			}

			if (Key.DownShort != 0)           /* If key down pressed, decrement value */
			{
				Key.DownShort = 0;
				MnuTimer = State.TimerSec + MENU_TIMEOUT;
				if (MnuNumVal[MnuNumIndex] == 43)           /* Case + sign : become - sign */
					MnuNumVal[MnuNumIndex] = 45;
				else if (MnuNumVal[MnuNumIndex] == 45)         /* Case - sign : become + sign */
					MnuNumVal[MnuNumIndex] = 43;
				else
				{
					switch (t)
					{
					case MNUNUM :
					case MNUNUMUPD :
						do
							MnuNumVal[MnuNumIndex] --;
						while (isdigit(MnuNumVal[MnuNumIndex]) == 0);
						break ;
					case MNUHEXA :
					case MNUHEXAUPD :
						do
							MnuNumVal[MnuNumIndex] --;
						while ((isxdigit(MnuNumVal[MnuNumIndex]) == 0) ||
								((isdigit(MnuNumVal[MnuNumIndex]) == 0) && (isupper(MnuNumVal[MnuNumIndex]) == 0)));
						break ;
					case MNUALPHA0 :
						do
							MnuNumVal[MnuNumIndex] --;
						while ((isalnum(MnuNumVal[MnuNumIndex]) == 0) &&
								(MnuNumVal[MnuNumIndex] != SPACE) &&
								(MnuNumVal[MnuNumIndex] != UNDERS));
						break ;
					}
				}
				DisplayUpdate = 1;
			}

			/* Stores MnuNumVal[] in v[] , invert string */
			j = 0;
			while (j < 16)
			{
				v[17 - j] = MnuNumVal[j];
				j ++;
			}

			v[18] = SPACE;          /* Add space char to stop sscanf */
			/* Control new value */
			(*MnuDef[i].f)(2, &v[0]);
			/* Restore new old value if needed */
			if ((v[0] == 0) && (v[1] == 0))
			{
				MnuNumVal[MnuNumIndex] = MnuNumOldVal;
				v[17 - MnuNumIndex] = MnuNumVal[MnuNumIndex];
			}
			if ((t == MNUNUMUPD) || (t == MNUHEXAUPD))
			{
				/* Stores MnuNumVal[] in v[] , invert string */
				j = 0;
				while (j < 16)              /* Stores c[] in MnuNumVal[] */
				{                        /* Invert string */
					MnuNumVal[j] = v[17 - j];
					j ++;
				}
			}

			if (DisplayUpdate == 1)
			{
				DrawDataInputBackground();

				/* Print MnuNumVal[] at the beginning of the line */
				j = 0;
				while (j < 16)
				{
					if (j == (15 - MnuNumIndex))
					{
						Putchar(250+j*12,110,(char*)&MnuNumVal[15 - j],1);
					}
					else
						Putchar(250+j*12,110,(char*)&MnuNumVal[15 - j],0);
					j ++;
				}
				DisplayUpdate = 0;
				UpdateDisplayNow();
			}
		}
	}
}


/**
 * @brief	Generates a new password
 * @param  None
 * @retval None
 */
uint PasswdRand(void)
{
	return (uint)(rand() % 10000);          /* Calculates new password */
}

long rand( void )
{
	next = next * 1103515245L + 12345;
	return (long)((next / 65536L) % 32768U);
}


/**
 * @brief
 * @param  None
 * @retval None
 */
void  GetSetData(uint a, uchar *v)
{

}


/**
 * @brief
 * @param  None
 * @retval None
 */
void  GetSetMnuList(uint a, uchar *v)
{
	/* For lists set mask bits */
}


/**
 * @brief	Set or gets profile value
 * @param  a = 0 : get value, 0 = 1 set value
 * @param	v  profile value
 * @retval None
 */
void  GetSetProfil(uint a, uchar *v)
{
	if (a == 0)
	{
		Menu.Profile = *v;
		MenuWriteOn;
	}
	else
	{
		*v = Menu.Profile;
	}
}


/**
 * @brief	Set AutoStart On/Off mode
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetAutoStartOffOn(uint a, uchar *v)
{
	if (a == 0)
	{
		if (*v == OFF)
			Menu.AutoStartOffOn = ON;
		else
			Menu.AutoStartOffOn = OFF;
		MenuWriteOn;
		if (StartStop != 0)
			StartStopOff;
	}
	else
	{
		if (Menu.AutoStartOffOn == OFF)
			*v = ON;
		else
			*v = OFF;
	}
}

/**
 * @brief	Set battery capacity auto or manu
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetBatCapAutoManu(uint a, uchar *v)
{
	if (a == 0)
	{
		if (*v == ON)
			Menu.CapAutoManu = ON;
		else
			Menu.CapAutoManu = OFF;
		MenuWriteOn;
	}
	else
	{
		if (Menu.CapAutoManu != 0)
			*v = ON;
		else
			*v = OFF;
	}
}

/**
 * @brief	Set or get section of cable
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetSect(uint a, uchar *v)
{
	if (a == 0)
	{
		Menu.CableSection = *v;
		MenuWriteOn;
	}
	else
	{
		*v = Menu.CableSection;
	}
}



/**
 * @brief	Enter the current password
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetGetPasswd2(uint j)
{
	if (j == DEFAULTPASSWD)
	{
		MnuListCur[1] = 1;					/* Force to reload selection to zero */
		MaskPasswdHighOff;
		MaskPasswdLowOff;
		MaskPasswd2Off;
		MaskPasswd3Off;
	}
	else if (j == Menu.Passwd)     		/* User password (99) */
	{
		MnuListCur[1] = 1;			/* Force to reload selection to zero */
		MaskPasswdHighOn;
		MaskPasswdLowOff;
		MaskPasswd2On;
		MaskPasswd3On;
	}
	else if (j == PASSWORD2)
	{
		MnuListCur[1] = 1;
		MaskPasswdHighOn;
		MaskPasswdLowOff;
		MaskPasswd2Off;
		MaskPasswd3On;
	}
	else if (j == PASSWORD3)
	{
		MnuListCur[1] = 1;
		MaskPasswdHighOn;
		MaskPasswdLowOff;
		MaskPasswd2On;
		MaskPasswd3Off;
	}
}

/**
 * @brief	Enter the current password
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetGetPasswd(uint a, uchar *v)
{
	uint  j;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%4u", &j);
	}
	if (a == 1)
	{

		j = 0;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (10000 < j)
	{
		j = 9999;
		*v = 0;
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xF0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}

	/* Write value : no write value in memory */
	if (a == 0)
	{
		if (j == DEFAULTPASSWD)
		{
			MnuListCur[1] = 1;					/* Force to reload selection to zero */
			//MaskPasswdTmpOff;
			MaskPasswdHighOff;
			MaskPasswdLowOff;
			MaskPasswd2Off;
			MaskPasswd3Off;
		}
		else
		{
			//			if (j == PasswdTmp)                 /* Temporary password */
			//			{
			//				MnuListCur[1] = 1;				/* Force to reload selection to zero */
			//				PasswdTmp = PasswdRand();       /* Temporary password */
			//				//MaskPasswdTmpOff;
			//				MaskPasswdHighOn;
			//				MaskPasswdLowOff;
			//			}
			/*			else*/ if (j == Menu.Passwd)     		/* User password (99) */
			{
				MnuListCur[1] = 1;			/* Force to reload selection to zero */
				//MaskPasswdTmpOn;
				MaskPasswdHighOn;
				MaskPasswdLowOff;
				MaskPasswd2On;
				MaskPasswd3On;
			}
			else if (j == PASSWORD2)
			{
				MaskPasswdHighOn;
				MaskPasswdLowOff;
				MaskPasswd2Off;
				MaskPasswd3On;
			}
			else if (j == PASSWORD3)
			{
				MaskPasswdHighOn;
				MaskPasswdLowOff;
				MaskPasswd2On;
				MaskPasswd3Off;
			}

		}
	}

	if (a == 1)
	{
		sprintf((char *)v + 2,"%4.4u", j);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

/**
 * @brief	Set the langage
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetLangage(uint a, uchar *v)
{
	if (a == 0)
	{
		LANG = *v;
		Menu.Langage = LANG;
		MenuWriteOn;
	}
	else
	{
		*v = LANG;
	}
}


/**
 * @brief	Set or get length of cable
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetLength(uint a, uchar *v)
{
	if (Menu.Area == 0)
		GetSetLengthM(a, v);
	else
		GetSetLengthFeet(a, v);
}

/**
 * @brief	Set or get length of cable in meters, 1m to 10 m step 0.1 m
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetLengthM(uint a, uchar *v)
{
	uint  j;
	uint  e, d;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%2u", &e);
		sscanf((const char *)v + 5, "%1u", &d);
		j = e * 10 + d;
	}
	if (a == 1)
	{
		j = Menu.CableLength;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (j < 10)
	{
		j = 10;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else  if (150 < j)
	{
		j = 150;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xD0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		Menu.CableLength = j;
		MenuWriteOn;
	}
	if (a == 1)
	{
		e = j / 10;                 /* Integer part */
		d = j % 10;                 /* Decimal part */
		sprintf((char *)v + 2,"%02u.%01u m", e, d);   /* 16 char */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

/**
 * @brief	Set or get length of cable in feet, 1m to 10 m step 0.1 m
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetLengthFeet(uint a, uchar *v)
{
	uint  j;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%2u", &j);
	}
	if (a == 1)
	{
		j = ((uint)Menu.CableLength * 33) / 100;
		if (j == 3)
			j = 4;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (j < 4)
	{
		j = 4;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else  if (49 < j)
	{
		j = 49;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xC0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		Menu.CableLength = (((uint)100 * j) + 50) / 33;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%02u ft", j);   /* 16 char */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

/**
 * @brief	Set or get the max charger current
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetImax(uint a, uchar *v)
{

	uint  j;
    
    // MODIF 2.8
    uint  ImaxTmp;
    if ((AarsParam.NbModLeft == OUTPUT_2_CABLE_6M) || (AarsParam.NbModLeft == OUTPUT_2_CABLE_8M))
        ImaxTmp = 6400;
    else
        ImaxTmp = 3200;
    // END
        
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%3u", &j);
		j = j * 10;
	}
	if (a == 1)
	{
		j = Menu.ImaxCharger;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value  */
	// MODIF 2.8
    //if (3200 < j)
    if (ImaxTmp < j)
	{   // MODIF 2.8
		//j = 3200;
        j = ImaxTmp;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else if (100 > j)
	{
		j = 100;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else
	{                           /* No error mask for editing */
		*(v + 0) = 0xE0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		if (j < 100)    // MODIF R2.2 Imax min = 10A
			j = 100;
		Menu.ImaxCharger = j;
		/* Control value for Ifloating */
		if (Menu.ImaxCharger < Menu.Ifloating)
			Menu.Ifloating = Menu.ImaxCharger;
		/* Control value for IFloating */
		if ((Menu.Imax / 2 ) < Menu.Iequal)
			Menu.Iequal = Menu.ImaxCharger / 2;
		MenuWriteOn;
	}
	if (a == 1)
	{
		j = j / 10;
		sprintf((char *)v + 2,"%3.3u A", j);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}


/**
 * @brief	Set or get the battery capacity
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetBatCapacity(uint a, uchar *v)
{

	uint  j;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%4u", &j);
	}
	if (a == 1)
	{
		j = Menu.BatCap;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (j < 25)
	{
		j = 25;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else  if (3000 < j)
	{
		j = 3000;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xF0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		Menu.BatCap = j;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%4.4u Ah", j);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

/**
 * @brief	Set or get temperature of charger
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetTempChg(uint a, uchar *v)
{
	if (Menu.Area == 0)
		GetSetTempChgC(a, v);
	else
		GetSetTempChgF(a, v);
}


/**
 * @brief	Set or get temperature of charger EU
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetTempChgC(uint a, uchar *v)
{

	sint  j;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%3d", &j);
	}
	if (a == 1)
	{
		j = Menu.BatTemp;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (j < -15)
	{
		j = -15;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else if (45 < j)
	{
		j = 45;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xE0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		Menu.BatTemp = j;
		Menu.BatTempF = (j * 9) / 5 + 32;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%+2.2d'C", j);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}


/**
 * @brief	Set or get temperature of charger US
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetTempChgF(uint a, uchar *v)
{

	sint  j;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%3d", &j);
	}
	if (a == 1)
	{
		j = (sint)Menu.BatTempF;
		//j = ((sint)Menu.BatTemp * 9) / 5 + 32;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (j < 5)
	{
		j = 5;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else if (113 < j)
	{
		j = 113;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xE0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		Menu.BatTempF = j;
		Menu.BatTemp = (((j - 32) * 50) + 50) / 90;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%3.3d'F", j);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

/**
 * @brief	Set the area
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetArea(uint a, uchar *v)
{
	if (a == 0)
	{
		Menu.Area = *v;
		switch (Menu.Area)
		{
		case 0 :
			MaskArea0On;
			MaskArea1Off;
			break ;
		case 1 :
			MaskArea1On;
			MaskArea0Off;
			break ;
		}
		MenuWriteOn;
	}
	else
	{
		*v = Menu.Area;
	}
}

/**
 * @brief	Set the Equal current
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetIequal(uint a, uchar *v)
{

	uint  j;
	uint  IequalMax;

	//IequalMax = (Menu.Imax * 2)/ 3;
    IequalMax = ((Menu.Imax * 2)/ 3) + 50;

	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%3u", &j);
		j = j * 10;
	}
	if (a == 1)
	{
		j = Menu.Iequal;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (j < 20)
	{
		j = 20;
		*v = 0;
		*(v + 1) = 0;
	}
	else if (IequalMax < j) 
	{
		j = IequalMax;
		*v = 0;
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xE0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		Menu.Iequal = j;
		MenuWriteOn;
	}
	if (a == 1)
	{
		/* %% to print % sign */
		j = j / 10;
		sprintf((char *)v + 2,"%3.3u A", j);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

/**
 * @brief	Set or get the equalisation time
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetEqualTime(uint a, uchar *v)
{

	uint  h;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%2u", &h);
	}
	if (a == 1)
	{
		h = Menu.EqualTime;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (h < 1)
	{
		h = 1;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else if (48 < h)
	{
		h = 48;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xC0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		Menu.EqualTime = h;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%2.2uh00", h);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

/**
 * @brief	Set or get the equalisation time
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetEqualTimePh1(uint a, uchar *v)
{

	uint  h;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%2u", &h);
	}
	if (a == 1)
	{
		h = Menu.EqualTime;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (h < 1)
	{
		h = 1;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else if (6 < h)
	{
		h = 6;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xC0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		Menu.EqualTime = h;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%2.2uh00", h);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

/**
 * @brief	Set or get the equalisation time
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetEqualTimePh2(uint a, uchar *v)
{

	uint  h;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%2u", &h);
	}
	if (a == 1)
	{
		h = AarsParam.EqTimePhase2;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (h < 1)
	{
		h = 1;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else if (48 < h)
	{
		h = 48;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xC0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		AarsParam.EqTimePhase2 = h;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%2.2uh00", h);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}


/**
 * @brief	Set or get the time/delay for equalisation
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetEqualDelay(uint a, uchar *v)
{

	uint  h;
	/* Select case depending on MSB, MSB = 0 : hour, MSB = 1 : delay */
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%2u", &h);
	}
	if (a == 1)
	{
		h = Menu.EqualDelay;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (23 < h)
	{
		h = 23;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xC0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		Menu.EqualDelay = h;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%2.2uh00", h);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

/**
 * @brief	Factory reset of Menu parameters
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetFactoryReset(uint a, uchar *v)
{
	if (a == 0)
	{
		if (*v == ON)
		{
			InitConfigOn;
			LCDMenu_Level();
			FlagSaveSerial = ON;
		}
	}
	else
		*v = ON;
}


/**
 * @brief	Set a new password
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetSetPasswd(uint a, uchar *v)
{

	uint  j;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%4u", &j);
	}
	if (a == 1)
	{
		j = Menu.Passwd;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (10000 < j)
	{
		j = 9999;
		*v = 0;
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xF0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		Menu.Passwd = j;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%4.4u", j);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

/**
 * @brief	Set charge nb cells
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void GetSetNbCell(uint a, uchar *v)
{
	if (a == 0)
	{
		//Menu.NbCells = *v;
		Menu.CellSize = *v;     // MODIF NBCELL AUTO
		MenuWriteOn;
	}
	else
	{
		//*v = Menu.NbCells;
		*v = Menu.CellSize;     // MODIF NBCELL AUTO
	}
}

/**
 * @brief	Set charger Type
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void GetSetChargerType(uint a, uchar *v)
{
	if (a == 0)
	{
		Menu.ChgType = *v;
		MenuWriteOn;
	}
	else
	{
		*v = Menu.ChgType;
	}
}

/**
 * @brief	Get status value
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetStatus(uint a, uchar *v)
{
	/* Number of choices */
	*v = 0;

	/*if (MaskPasswdHigh != 0 && MaskPasswdLow != 0)
		*(v + 1) = 11;
	else*/
		*(v + 1) = 13;    
    
	/* Values */
	switch (a)
	{
	case 0 :
		sprintf((char *)v + 2, "%s$%u", &ListWord9[6][LANG][0], (Status.NbCharge) % 10000);
		break;
	case 1 :
		sprintf((char *)v + 2, "%s$%u", &ListWord9[7][LANG][0], (Status.NbCharge - Status.NbPartialCharge) % 10000);
		break;
	case 2 :
		sprintf((char *)v + 2, "%s$%u", &ListWord9[8][LANG][0], (Status.NbPartialCharge) % 100000);
		break;
	case 3 :
		sprintf((char *)v + 2, "%s$%u", &ListWord9[0][LANG][0], (Status.NbConverterDef) % 100000);
		break;
	case 4 :
		sprintf((char *)v + 2, "%s$%u", &ListWord9[1][LANG][0], Status.NbFuseDef % 100000);
		break;
	case 5 :
		sprintf((char *)v + 2, "%s$%u", &ListWord9[2][LANG][0], Status.NbBadBatDef % 100000);
		break;
	case 6 :
		sprintf((char *)v + 2, "%s$%u", &ListWord9[3][LANG][0], Status.NbOverdisDef % 100000);
		break;
	case 7 :
	#ifdef ENERSYS_EU
		sprintf((char *)v + 2, "%s$%u", &ListWord9[4][LANG][0], (Status.NbdIdTDef + Status.NbdVdTDef + Status.NbTimeSecuDef + Status.NbOtherBatDef) % 1000);
	#else
		sprintf((char *)v + 2, "%s$%u", &ListWord9[4][LANG][0], (Status.NbdVdTDef + Status.NbTimeSecuDef + Status.NbOtherBatDef) % 1000);
	#endif		
		break;
#ifdef ENERSYS_EU 
    case 9 :
        sprintf((char *)v + 2, "%s$%u", &ListWord9[5][LANG][0], Status.NbTempDef % 100000);
        break;
	case 8 :
		sprintf((char *)v + 2, "%s$%u", &ListWord9[11][LANG][0], Status.NbPumpDef % 100000);
		break;
	case 10 :
		sprintf((char *)v + 2, "%s$%u", &ListWord9[9][LANG][0], Status.NbNetworkDef % 100000);
		break;
	case 11 :
			sprintf((char *)v + 2, "%s$%u", &ListWord9[10][LANG][0], Status.NbTotalAh);
		break;            
#else
	case 9 :
		sprintf((char *)v + 2, "%s$%u", &ListWord9[5][LANG][0], Status.NbTempDef % 100000);
		break;
	case 8 :
		sprintf((char *)v + 2, "%s$%u", &ListWord9[11][LANG][0], Status.NbdIdTDef % 100000);
		break;	
	case 10 :
		sprintf((char *)v + 2, "%s$%u", &ListWord9[9][LANG][0], Status.NbNetworkDef % 100000);
		break;

	case 11 :
			sprintf((char *)v + 2, "%s$%u", &ListWord9[10][LANG][0], Status.NbTotalAh);
		break;
#endif
	case 12 :
			sprintf((char *)v + 2, "%s$%u", &ListWord9[12][LANG][0], Status.NbThModule);
		break;

	}
}

/**
 * @brief	 Reset memos in EEPROM
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetMemoReset(uint a, uchar *v)
{
	ulong NbTotalAh;

    // MODIF R2.5
	uint  Year; 
	uint  Month;
	uint  Date;
	uint  Hr;
	uint  Min;
	uint  Sec;
    // END
    
	if (a == 0)
	{
		if (*v == ON)
		{
			NbTotalAh = Status.NbTotalAh;
            // MODIF R2.5 : save daylight data before to clear Status segment
            Year = Status.NextDaylight.Year;
            Month = Status.NextDaylight.Month;
            Date = Status.NextDaylight.Date;
            Hr = Status.NextDaylight.Hr;
            Min = Status.NextDaylight.Min;
            Sec = Status.NextDaylight.Sec;
            // END
			memset(&Status, 0, sizeof(StructStatus));
			// MODIF 3.0
			Status.TypeSegment = TYPE_STATUS;
    		Status.NumSegment = SEGSTATUS;
            // END
			Status.NbRstPointer = 0;
			Status.MemoPointer = SEGMEMOTOP - 1;
			Status.NbTotalAh = NbTotalAh;
            // MODIF R2.5
            Status.NextDaylight.Year = Year;
            Status.NextDaylight.Month = Month;
            Status.NextDaylight.Date = Date;
            Status.NextDaylight.Hr = Hr;
            Status.NextDaylight.Min = Min;
            Status.NextDaylight.Sec = Sec;
            // END
			StatusWriteOn;

			// MODIF R3.3
			memset(&E1Status, 0 , sizeof(StructE1Status));
			E1Status.TypeSegment = TYPE_E1_STATUS;
			E1Status.NumSegment = SEGE1STATUS;
			E1Status.EventPointer = SEGE1TOP - 1;
			Write_SegFlashData(SEGE1STATUS	, (uchar *)&E1Status);

			LCDMenu_Level();
		}
	}
	else
	{
		if ((Status.NbRstPointer == 0) && (Status.MemoPointer == (SEGMEMOTOP - 1)))
			*v = ON;
		else
			*v = OFF;
	}
}

/**
 * @brief	Get Memo list
 * @param  a : number of the memo (0 = oldest one) to read
 * @retval None
 */
void  GetSetMemoList(uint a, uchar *v)
{
	if (Status.MemoPointer  <= (SEGMEMOTOP-1))
	{
		*v = 0;
		*(v + 1) = 0;
		return;
	}

	StructDateTime dt;
	uint  i;
	/* Number of choices */
	if (Status.NbRstPointer == 0)           /* No reset */
	{
		i = Status.MemoPointer - SEGMEMOTOP + 1;  /* MEMO */
		*v = HighByte(i);
		*(v + 1) = LowByte(i);
		/* a : 0 to Status.MemoPointer - SEGMEMOTOP */
		i = Status.MemoPointer - a;
	}
	else
	{
		/* Limited to 999 for 3 digits display */
		i = (SEGMEMONB % 1000);                  /* MEMO */
		*v = HighByte(i);
		*(v + 1) = LowByte(i);
		if (a <= (Status.MemoPointer - SEGMEMOTOP))
			i = Status.MemoPointer - a;
		else
			i = (Status.MemoPointer + SEGMEMONB) - a;
	}

	if ((MemoPtr != i) && (Status.MemoPointer  > (SEGMEMOTOP-1)))
	{
		MemoPtr = i;
		Read_SegFlashData (MemoPtr, (uchar *)&MemoTemp);
	}

	if (Status.MemoPointer  > (SEGMEMOTOP-1))
	{
		dt.Year = MemoTemp.SocDate.Year;
		//dt.Year += 2000;
		dt.Month = MemoTemp.SocDate.Month;
		dt.Date = MemoTemp.SocDate.Date;
		dt.Hr = MemoTemp.SocDate.Hr;
		dt.Min = MemoTemp.SocDate.Min;
		if (dt.Month == 0 || dt.Date == 0)
		{
			sprintf((char *)v + 2, "%3u  --/--/----", a + 1);
		}
		else
		{
			if (MaskArea1 != 0) // US
				sprintf((char *)v + 2, "%u %02u%1c%02u%1c%02u%1c%02u%1c%02u", a + 1, dt.Month, '/', dt.Date, '/', dt.Year, SPACE, dt.Hr, 'h', dt.Min);
			else
				sprintf((char *)v + 2, "%u %02u%1c%02u%1c%02u%1c%02u%1c%02u", a + 1, dt.Date, '/', dt.Month, '/', dt.Year, SPACE, dt.Hr, 'h', dt.Min);
		}

	}
	else
	{
		i = 0;                  /* MEMO */
		*v = HighByte(i);
		*(v + 1) = LowByte(i);
		sprintf((char *)v, "%20.20s", &ListWord16_State[11][LANG][0]);
	}
}


/**
 * @brief	Get status of a selected memo
 * @param  a : number of the memo
 * @retval None
 */
void  GetSetMemo(uint a, uchar *v)
{
	uint i;
	uint df;

	if (Status.MemoPointer  <= (SEGMEMOTOP-1))
	{
		*v = 0;
		*(v + 1) = 0;
		return;
	}

	if (Status.NbRstPointer == 0)           /* No reset */
		i = Status.MemoPointer - MnuMemoVal; // MEMO - 1;
	else if (MnuMemoVal < (Status.MemoPointer - SEGMEMOTOP))
		i = Status.MemoPointer - MnuMemoVal;
	else
		i = (Status.MemoPointer + SEGMEMONB) - MnuMemoVal - 1;

	Read_SegFlashData (i, (uchar *)&MemoTemp);

	memset(v, SPACE, 22);
	/* Number of choices */
	*v = 0;
	*(v + 1) = 20;

	/* Values */
	switch (a)
	{
	case 0:
		sprintf((char *)v + 2, "%s$%14.14s", &ListWord8[23][LANG][0], &MemoTemp.BatSN[0]);
		break;
	case 1 :
		if (MemoTemp.BatTechno < TEC_NB)
			sprintf((char *)v + 2, "%s$%s", &ListWord8[3][LANG][0], &ListWord6[MemoTemp.BatTechno + TEC_POS][LANG][0]);
		else
			sprintf((char *)v + 2, "%s$%s", &ListWord8[3][LANG][0], &ListWord6[0 + TEC_POS][LANG][0]);
		break;
	case 2 :
		if ((MemoTemp.CapAutoManu != 0) || ((State.Mask & P0) == 0))
			sprintf((char *)v + 2, "%s$%uAh", &ListWord8[0][LANG][0], MemoTemp.BatCap);
		else
			sprintf((char *)v + 2, "%s", &ListWord8[0][LANG][0]);
		break;

	case 3 :
		i = sprintf((char *)v + 2, "%s$0x%2.2X", &ListWord8[4][LANG][0], MemoTemp.BatWarning);
		/*if ((m.BatWarning & 0x04) == 0x04) // High temp
          i = sprintf(v + 2 + 11, "%1c  ", 124 + SPACE);
        else
          i = sprintf(v + 2 + 11, "   ");
        if (((m.BatWarning & 0x80) == 0x80) || ((m.BatWarning & 0x40) == 0x40)) // Balance
          i = sprintf(v + 2 + 14, "%1c%1c ", 120 + SPACE, 121 + SPACE);
        else
          i = sprintf(v + 2 + 14, "   ");
        if ((m.BatWarning & 0x20) == 0x20) // Low level
          i = sprintf(v + 2 + 17, "%1c%1c ", 122 + SPACE, 123 + SPACE);
        else
          i = sprintf(v + 2 + 17, "   ");*/
		break;

	case 4 :
		sprintf((char *)v + 2, "%s$%s", &ListWord8[5][LANG][0], &ListWord6[MemoTemp.Profile][LANG][0]);
		break;

	case 5 :
		sprintf((char *)v + 2, "%s$%uV", &ListWord8[1][LANG][0], MemoTemp.BatVolt);
		break;
	case 6 :
		switch (Menu.Area)
		{
		case 0 :
			sprintf((char *)v + 2, "%s$%+2.2d'C", &ListWord8[2][LANG][0], MemoTemp.BatTemp);
			//v[15] = DEGRES;
			break;
		case 1 :
			sprintf((char *)v + 2, "%s$%+2.2d'F", &ListWord8[2][LANG][0], ((sint)MemoTemp.BatTemp * 9) / 5 + 32);
			//v[15] = DEGRES;
			break;
		}
		break;
		case 7 :
			sprintf((char *)v + 2, "%s$%u%%", &ListWord8[8][LANG][0], (MemoTemp.InitSOC)%100);
			break;
		case 8 :
			sprintf((char *)v + 2, "%s$%1u.%2.2uV", &ListWord8[9][LANG][0], (MemoTemp.SocVoltage/1000)%10, (MemoTemp.SocVoltage%1000)/10);
			break;
		case 9 :
			sprintf((char *)v + 2, "%s$%1u.%2.2uV", &ListWord8[16][LANG][0], (MemoTemp.EocVoltage/1000)%10, (MemoTemp.EocVoltage%1000)/10);
			break;
		case 10 :
			sprintf((char *)v + 2, "%s$%uA", &ListWord8[17][LANG][0], MemoTemp.EocCurrent%1000);
			break;
		case 11:
			sprintf((char *)v + 2, "%s$%2uh%2.2umn", &ListWord8[18][LANG][0], (MemoTemp.ChgTime/60)%100, MemoTemp.ChgTime%60);
			break;
		case 12 :
			sprintf((char *)v + 2, "%s$%4uAh", &ListWord8[19][LANG][0], MemoTemp.ChgAh);
			break;
		case 13 : // SOC date
			if (MaskArea1 != 0) // US
				sprintf((char *)v + 2, "%s$%02u%1c%02u%1c%04u", &ListWord3[0][LANG][0], MemoTemp.SocDate.Month, '/', MemoTemp.SocDate.Date, '/', (MemoTemp.SocDate.Year+2000));
			else
				sprintf((char *)v + 2, "%s$%02u%1c%02u%1c%04u", &ListWord3[0][LANG][0], MemoTemp.SocDate.Date, '/', MemoTemp.SocDate.Month, '/', (MemoTemp.SocDate.Year+2000));
			break;
		case 14 :
			sprintf((char *)v + 2, "%s$%02u%1c%02u", &ListWord3[0][LANG][0], MemoTemp.SocDate.Hr, 'h', MemoTemp.SocDate.Min);
			break;
		case 15 :
			if (MaskArea1 != 0) // US
				sprintf((char *)v + 2, "%s$%02u%1c%02u%1c%04u", &ListWord3[1][LANG][0], MemoTemp.EocDate.Month, '/', MemoTemp.EocDate.Date, '/', (MemoTemp.EocDate.Year+2000));
			else
				sprintf((char *)v + 2, "%s$%02u%1c%02u%1c%04u", &ListWord3[1][LANG][0], MemoTemp.EocDate.Date, '/', MemoTemp.EocDate.Month, '/', (MemoTemp.EocDate.Year+2000));
			break;
		case 16 :
			sprintf((char *)v + 2, "%s$%02u%1c%02u", &ListWord3[1][LANG][0], MemoTemp.EocDate.Hr, 'h', MemoTemp.EocDate.Min);
			break;
		case 17 :
			if (MemoTemp.TypeEoc == 1) // complete charge
			{ // Complete
				sprintf((char *)v + 2, "Status$Complete");
			}
			else
			{ // Partial
				sprintf((char *)v + 2, "Status$Partial");
			}
			break;
		case 18 : 
            if( (MemoTemp.Default & BitDFconverter) != 0)
				df = 1;
			if( (MemoTemp.Default & BitDFnetwork) != 0 )
				df = 10; // DFC
			else if ((MemoTemp.Default & BitDFfuse) != 0)
				df = 2;
			else if ((MemoTemp.Default & BitDFbadBat) != 0)
				df = 3;
			else if ((MemoTemp.Default & BitDFtemp) != 0)
				df = 6;   // TH
			else if ((MemoTemp.Default & BitDFpump) != 0)
				df = 12;  // DF7
        #ifdef ENERSYS_EU        
        	else if( ((MemoTemp.Default & BitDFdIdT) != 0)||
        			((MemoTemp.Default & BitDFtimeSecu) != 0) || ((MemoTemp.Default & BitDFotherBat) != 0) )
        		df = 5;
        #else
            else if( (MemoTemp.Default & BitDFdIdT) != 0)
        		df = 12; // DF7
        	else if ( ((MemoTemp.Default & BitDFtimeSecu) != 0) || ((MemoTemp.Default & BitDFotherBat) != 0) )
        		df = 5;
        #endif
			else if ((MemoTemp.Profile == EQUAL) && ((MemoTemp.Default & BitDFdVdT) != 0) )      
				df = 5;		
        	else if ((MemoTemp.Default & BitDFoverdis) != 0)
        		df = 4;
        	else
        		df = 0;
                
            if (df > 0)
				sprintf((char *)v + 2, "Default$%6s", &ListWord9[df-1][LANG][0]);
			else
				sprintf((char *)v + 2, "Default$ ");
			break;

		/*	if( (MemoTemp.Default & BitDFconverter) != 0)
				df = 1;
			if( (MemoTemp.Default & BitDFnetwork) != 0 )
				df = 10; // DFC
			else if ((MemoTemp.Default & BitDFfuse) != 0)
				df = 2;
			else if ((MemoTemp.Default & BitDFbadBat) != 0)
				df = 3;
			else if ((MemoTemp.Default & BitDFtemp) != 0)
				df = 6;   // TH
				else if ((MemoTemp.Default & BitDFpump) != 0)
					df = 12;  // DF7
				else if( ((MemoTemp.Default & BitDFdIdT) != 0) ||
						((MemoTemp.Default & BitDFtimeSecu) != 0) || ((MemoTemp.Default & BitDFotherBat) != 0) )
					df = 5;   // DF5
				else if ((MemoTemp.Default & BitDFoverdis) != 0)
					df = 4;   // DF4
				else
					df = 0;
			if (df > 0)
				sprintf((char *)v + 2, "Default$%6s", &ListWord9[df-1][LANG][0]);
			else
				sprintf((char *)v + 2, "Default$ ");
			break;*/

		case 19 : // CFC
			sprintf((char *)v + 2, "CFC$%4.4X", MemoTemp.Default );
			break;
	}

	if ((State.Mask & P0) == 0)
	{
		*(v + 1) = 28;
		/* Values */
		switch (a)
		{
		case 20 :
			sprintf((char *)v + 2, "Warning$%2.2X", MemoTemp.BatWarning);
			break;
		case 21 :
			sprintf((char *)v + 2, "Phase$%3.3u", MemoTemp.Phase);
			break;
		case 22 :
			sprintf((char *)v + 2, "Iphase1$%3.3uA", (MemoTemp.Iphase1/10)%1000);
			break;
		case 23 :
			sprintf((char *)v + 2, "VgazCell$%1u.%2.2uV", (MemoTemp.VgazCell/1000)%10, (MemoTemp.VgazCell%1000)/10);
			break;
		case 24 :
			sprintf((char *)v + 2, "Iphase3$%3.3uA", (MemoTemp.Iphase3/10)%1000);
			break;
		case 25 :
			sprintf((char *)v + 2, "Tphase1$%2.2uh%2.2u", (MemoTemp.TimePhase1/60)%100, MemoTemp.TimePhase1%60);
			break;
		case 26 :
			sprintf((char *)v + 2, "Tphase2$%2.2uh%2.2u", (MemoTemp.TimePhase2/60)%100, MemoTemp.TimePhase2%60);
			break;
		case 27 :
			sprintf((char *)v + 2, "Tphase3$%2.2uh%2.2u", (MemoTemp.TimePhase3/60)%100, MemoTemp.TimePhase3%60);
			break;
		}
	}
}

/**
 * @brief	Set time (for adjusting) EU/US format
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 *
 * @retval None
 */
void  GetSetSetTime(uint a, uchar *v)
{
	if (State.DateR.Date == 0)
	{
		State.DateR.Sec = 0;
		State.DateR.Date = 1;
		State.DateR.Month = 1;
		State.DateR.Year = 2017;
	}

	if (Menu.Area == 0)
		GetSetSetTimeF(a, v);
	else
		GetSetSetTimeUS(a, v);
}

/**
 * @brief	Set time (for adjusting) EU format
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 *         16 char : "dd/mm/yy hh:mm
 * @retval None
 */
void  GetSetSetTimeF(uint a, uchar *v)
{
	uint data[6];
	
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%2u", &data); //&NewDate);
		State.DateW.Date = data[0]; 
		sscanf((const char *)v + 5, "%2u", &data);
		State.DateW.Month = data[0];		
		sscanf((const char *)v + 8, "%4u", &data);
		State.DateW.Year = data[0];
		sscanf((const char *)v + 13, "%4u", &data);
		State.DateW.Hr = data[0];
		sscanf((const char *)v + 16, "%2u", &data);
		State.DateW.Min = data[0];
	}
	
	if (a == 1)                   /* Get value */
	{
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (CtrlDateHour(&State.DateW) != 0)
	{
		*v = 0;
		*(v + 1) = 0;
	}
	else
	{
		*v = 0xDB;
		*(v + 1) = 0xDB;
	}
	/* Write value */
	if (a == 0)
	{
		RtcWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%02u/%02u/%04u %02uh%02um", State.DateR.Date, State.DateR.Month, State.DateR.Year, State.DateR.Hr, State.DateR.Min);
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

/**
 * @brief	Set time (for adjusting) US format
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 *         16 char : "mm/dd/yy hh:mm
 * @retval None
 */
void  GetSetSetTimeUS(uint a, uchar *v)
{
	uint data[6];

	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%2u", &data);
		State.DateW.Month = data[0];
		sscanf((const char *)v + 5, "%2u", &data);
		State.DateW.Date = data[0];
		sscanf((const char *)v + 8, "%4u", &data);
		State.DateW.Year = data[0];
		sscanf((const char *)v + 13, "%2u", &data);
		State.DateW.Hr = data[0];
		sscanf((const char *)v + 16, "%2u", &data);
		State.DateW.Min = data[0];
		State.DateW.Sec = 0;
	}
	if (a == 1)
	{
		memset(v, SPACE, 20);
	}
	/* Control value */
	if (CtrlDateHour(&State.DateW) != 0)
	{
		*v = 0;
		*(v + 1) = 0;
	}
	else
	{
		*v = 0xDB;
		*(v + 1) = 0xDB;
	}
	/* Write value */
	if (a == 0)
	{
		RtcWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%02u/%02u/%04u %02uh%02um", State.DateR.Month, State.DateR.Date, State.DateR.Year, State.DateR.Hr, State.DateR.Min);
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}


/**
 * @brief	Control and correct dt
 * @param  StructDateTime
 * @retval Return 0 if ok, 1 if error
 */
uchar CtrlDateHour(StructDateTime *dh)
{
	/* Control and correct dt */
	/* Return 0 if ok, 1 if error */
	uchar i;
	i = 0;
	if ((*dh).Date < 1)
	{
		(*dh).Date = 1;
		i ++;
	}
	if (31 < (*dh).Date)
	{
		(*dh).Date = 31;
		i ++;
	}
	if ((*dh).Month < 1)
	{
		(*dh).Month = 1;
		i ++;
	}
	if (12 < (*dh).Month)
	{
		(*dh).Month = 12;
		i ++;
	}
	if ((*dh).Year < 2000)
	{
		(*dh).Year = 2000;
		i ++;
	}
	if (2099 < (*dh).Year)
	{
		(*dh).Year = 2099;
		i ++;
	}
	if (23 < (*dh).Hr)
	{
		(*dh).Hr = 23;
		i ++;
	}
	if (59 < (*dh).Min)
	{
		(*dh).Min = 59;
		i ++;
	}
	return  0;
}


/**
 * @brief	Set type of delay for charge (time or delay)
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval
 */
void  GetSetChargeTimeType (uint a, uchar *v)
{
	if (a == 0)
	{
		switch (*v)
		{
		case 0:
			Menu.ChgDelayHour = 0;
			Menu.ChgSkipDelay = 0;
			break;
		case 1:
			Menu.ChgDelayHour = 1;
			Menu.ChgSkipDelay = 0;
			break;
		case 2:
			Menu.ChgSkipDelay = 1;
			break;
		}
		MenuWriteOn;
	}
	else
	{
		if (Menu.ChgSkipDelay != 0)
			*v = 2;
		else if (Menu.ChgDelayHour != 0)
			*v = 1;
		else
			*v = 0;
	}
}


/**
 * @brief	Set or get the time/delay for charge
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval
 */
void  GetSetChargeTime(uint a, uchar *v)
{

	uint  h;                      /* Hours */
	uint  m;                      /* Minute */
	/* Select case depending on MSB, MSB = 0 : hour, MSB = 1 : delay */
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%2u", &h);
		sscanf((const char *)v + 5, "%2u", &m);
	}
	if (a == 1)
	{
		h = (Menu.ChgDelayVal /60) % 24 ;
		m = Menu.ChgDelayVal % 60;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (23 < h)
	{
		h = 23;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else  if (59 < m)
	{
		m = 59;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xD8;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		Menu.ChgDelayVal = 60 * h + m;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%2.2uh%2.2u", h, m);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

/**
 * @brief	Set or get the conditional charge %
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval
 */
void  GetSetInitDod(uint a, uchar *v)
{

	uint  j;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%2u", &j);
	}
	if (a == 1)
	{
		j = Menu.InitDOD;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (70 < j)
	{
		j = 70;
		*v = 0;
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xC0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		Menu.InitDOD = j;
		MenuWriteOn;
	}
	if (a == 1)
	{
		/* %% to print % sign */
		sprintf((char *)v + 2,"%2.2u %%", j);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

/**
 * @brief	Set floating On or Off
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval
 */
void  GetSetFloatingOffOn(uint a, uchar *v)
{
	if (a == 0)
	{
		if (*v == OFF)
			Menu.FloatingOffOn = OFF;
		else
			Menu.FloatingOffOn = ON;
		MenuWriteOn;
	}
	else
	{
		if (Menu.FloatingOffOn == OFF)
			*v = OFF;
		else
			*v = ON;
	}
}


/**
 * @brief	Set or get the floating current
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval
 */
void  GetSetIfloating(uint a, uchar *v)
{

	uint  j;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%3u", &j);
		j = j * 10;
	}
	if (a == 1)
	{
		j = Menu.Ifloating;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (j < 20)
	{
		j = 20;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else  if (200 < j)
	{
		j = 200;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xE0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		Menu.Ifloating = j;
		MenuWriteOn;
	}
	if (a == 1)
	{
		j = j / 10;
		sprintf((char *)v + 2,"%3.3u A", j);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}


/**
 * @brief	Set or get the floating voltage
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval
 */
void  GetSetVfloating(uint a, uchar *v)
{

	uint  j;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%4u", &j);
	}
	if (a == 1)
	{
		j = Menu.Vfloating;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (j < 2100)
	{
		j = 2100;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else  if (2400 < j)
	{
		j = 2400;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xF0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		Menu.Vfloating = j;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%4.4u mV", j);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

void  GetSetUsbMemoWrite(uint a, uchar *v)
{
	/* Set memo transfer to Usb disk  */
	/* a = 0 : set value, a = 1 : get value, a = 2 : control value */
	/* DataR.VncProfilePtr = 0 : Off, Other : On */
	if (a == 0)
	{
		if (*v == ON)
		{
			if (UsbDisk != 0)
			{
				UsbWriteMemoOn;
				LCDMenu_Level();                    /* Returns to top menu level */
			}
		}
	}
	else
		*v = OFF;
}

void  GetSetUsb2Config(uint a, uchar *v)
{
	/* a = 0 : set value, a = 1 : get value, a = 2 : control value */
	/* DataR.VncProfilePtr = 0 : Off, Other : On */
	if (a == 0)
	{
		if (*v == ON)
		{
			if (UsbDisk != 0)
			{
				VncCfgPtrSave = 1;
				LCDMenu_Level();                    /* Returns to top menu level */
			}
		}
	}
	else
		*v = OFF;
}

void GetSetClearTempCon (uint a, uchar *v)
{
	/* Clear Bit TempCon on all module
	/* a = 0 : set value, a = 1 : get value, a = 2 : control value */
	if (a == 0)
	{
		if (*v == ON)
		{
			ModTempConAckDef();
			LCDMenu_Level();                    /* Returns to top menu level */
		} 
	}
	else
		*v = OFF;
}

void  GetSetConfig2Usb(uint a, uchar *v)
{
	/* Set memo transfer to Usb disk  */
	/* a = 0 : set value, a = 1 : get value, a = 2 : control value */
	if (a == 0)
	{
		if (*v == ON)
		{
			if (UsbDisk != 0)
			{
				VncCfgPtrOpen = 1;
				LCDMenu_Level();                    /* Returns to top menu level */
			}
		}
	}
	else
		*v = OFF;
}

void  GetSetUsbUpdate(uint a, uchar *v)
{
	//if (PORTB.PIDR.BIT.B3 == 0) // Key Start pressed -> no watchdog refresh
	RestartOn;
	*v = OFF;
}



void  GetSetReboot(uint a, uchar *v)
{
	//if (PORTB.PIDR.BIT.B3 == 0) // Key Start pressed -> no watchdog refresh
	//RestartOn;
	//*v = OFF;
	if (a == 0)
	{
		while(1){};
	}
	else
	{
		*v = ON;
	}
}


void GetSetModInfoGen(uint a, uchar *v)
{

	uint i;
	uint ITotal;

	/* Number of choices */
	*v = 0;
	*(v + 1) = 9;
	/* Values */
	switch (a)
	{
	case 0 :
		if (State.Charger < StateChgerChg)
			ITotal = 0;
		else
			ITotal = DataR.Ibat;
		sprintf((char *)v + 2, "IBat =$ %3.3u.%1.1uA",ITotal / 10, ITotal % 10);
		break;
	case 1 :
	case 2 :
	case 3 :
	case 4 :
	case 5 :
	case 6 :
	case 7 :
	case 8 :

		i = a - 1;
		if (CanErr(i) != 0)
		{
			sprintf((char *)v + 2, "MOD%1.1u $...........", a);
		}
		else if (LLCErr(i) != 0)
		{
			if (ModDefTempCon(i) != 0)
				sprintf((char *)v + 2, "MOD%1.1u $ TH-LOCK", a);
			else if ((ModDefTempStop(i) != 0) || (LLC[i].CntDefTemp > MAX_MOD_DEF_TEMP))
				sprintf((char *)v + 2, "MOD%1.1u $ TH", a);
			else if (ModVLmfb(i) != 0)
				sprintf((char *)v + 2, "MOD%1.1u $ VLMFB", a);
			else if (ModVBatMax(i) != 0)
				sprintf((char *)v + 2, "MOD%1.1u $ VBATMAX", a);
			else if (ModVFuse(i) != 0)
				sprintf((char *)v + 2, "MOD%1.1u $ FUSE-OUT", a);
			else if (ModPwmMax(i) != 0)
				sprintf((char *)v + 2, "MOD%1.1u $ PWM-MAX", a);
			else if (ModDefCTN(i) != 0)
				sprintf((char *)v + 2, "MOD%1.1u $ DEF-CTN", a);			
			else if (ModOtherDef(i) != 0)
				sprintf((char *)v + 2, "MOD%1.1u $ ERROR  ", a);
            else if (LLCWarn(i) != 0)       // MODIF R2.7 : VLMFB
                sprintf((char *)v + 2, "MOD%1.1u $ VBAT   ", a);    
            else // MODIF R2.6
				sprintf((char *)v + 2, "MOD%1.1u $ UNKNOW", a);    
		}
		else
		{
            if (LLCRamRO[i].VBat > 0)
			    sprintf((char *)v + 2, "MOD%1.1u $%3.3uA %2.2u.%1.1uV", a, LLCRamRO[i].IBat / 10, LLCRamRO[i].VBat / 100, (LLCRamRO[i].VBat % 100)/10);
            else
                sprintf((char *)v + 2, "MOD%1.1u $%3.3uA %2.2u.%1.1uV", a, LLCRamRO[i].IBat / 10, 0, 0);
		}
		break ;
	}
}


void GetSetModInfo0(uint a, uchar *v)
{
	if (MnuMemoVal != 0)
		GetSetModInfo(a, v, (MnuMemoVal - 1));
}

void GetSetModInfo1(uint a, uchar *v)
{
	GetSetModInfo(a, v, 1);
}

void GetSetModInfo2(uint a, uchar *v)
{
	GetSetModInfo(a, v, 2);
}

void GetSetModInfo3(uint a, uchar *v)
{
	GetSetModInfo(a, v, 3);
}

void GetSetModInfo4(uint a, uchar *v)
{
	GetSetModInfo(a, v, 4);
}

void GetSetModInfo5(uint a, uchar *v)
{
	GetSetModInfo(a, v, 5);
}



void GetSetModInfo(uint a, uchar *v, uchar k)
{
	uint j;

	/* Number of choices */
	*v = 0;
	*(v + 1) = 19;
	/* Values */
	switch (a)
	{
    case 0:           /* Firmware version */
        sprintf((char *)v + 2, "Version-ID$%3.3u-%3.3u", LLCRamRO[k].Ver[0], LLCRamRO[k].Id);
        //sprintf((char *)v + 2, "ID$%3.3u", LLCRamRO[k].Id);
        break;      
	case 1 :          /* uint  VCellReq */
		sprintf((char *)v + 2, "VReq$%3u.%2.2uV", LLCRamRW[k].VReq / 100, LLCRamRW[k].VReq % 100);
		break ;
	case 2 :          /* uint  IReq */
		sprintf((char *)v + 2, "IReq$%3u.%1.1uA", LLCRamRW[k].IReq / 10, LLCRamRW[k].IReq % 10);
		break ;
	case 3 :          /* uint  VFuse (bat) */
		if (LLCRamRO[k].VFuse >= 0)
            sprintf((char *)v + 2, "VFuse$%3u.%2.2uV", LLCRamRO[k].VFuse / 100, LLCRamRO[k].VFuse % 100);
        else
            sprintf((char *)v + 2, "VFuse$%3u.%2.2uV", 0, 0);    
		break ;
	case 4 :          /* uint  VFilPilot */
		if (LLCRamRO[k].VLmfb >= 0)
            sprintf((char *)v + 2, "VLmfb$%3u.%2.2uV", LLCRamRO[k].VLmfb / 100, LLCRamRO[k].VLmfb % 100);
        else
            sprintf((char *)v + 2, "VLmfb$%3u.%2.2uV", 0, 0);
		break ;
	case 5 :          /* uint  IBat */
		sprintf((char *)v + 2, "IShunt$%3u.%1.1uA", LLCRamRO[k].IBat / 10, LLCRamRO[k].IBat % 10);
		break ;
	case 6 :          /* uint  VBat */
        if (LLCRamRO[k].VBat >= 0)
		    sprintf((char *)v + 2, "VBat$%3u.%2.2uV", LLCRamRO[k].VBat / 100, LLCRamRO[k].VBat % 100);
        else
            sprintf((char *)v + 2, "VBat$%3u.%2.2uV", 0, 0);    
		break ;
	case 7 :          /* ulong  ICons */
		j = HighWord(LLCRamRO[k].ISlope);
		sprintf((char *)v + 2, "ICons$%3u.%1.1uA", j / 10, j % 10);
		break ;
	case 8 :          /* uint  Power */
		sprintf((char *)v + 2, "Power$%5uW", LLCRamRO[k].Power);
		break ;
	case 9 :          /* ulong TimerSec */
		sprintf((char *)v + 2, "Time$%8.8lus" , LLCRamRO[k].TimerChg);
		break ;
	case 10 :          /* ulong Status RamRO */
		sprintf((char *)v + 2, "StaRO$x%8.8lX", LLCRamRO[k].UnionRamROStatus);
		break ;
	case 11 :          /* ulong Status RamRO */
		sprintf((char *)v + 2, "DefRO$x%8.8lX", LLCRamRO[k].UnionRamRODef);
		break ;
	case 12 :          /* ulong Status RamRW */
		sprintf((char *)v + 2, "StaRW$x%8.8lX", LLCRamRW[k].UnionStatus);
		break ;
	//case 13 :          /* Tp ambiant */
	//	sprintf((char *)v + 2, "Ambiant$%3d 'C", (schar)((LLCRamRO[k].Temp[0] >> 8) - 64), DEGRES);
	//	break ;
	//case 14 :          /* Tp CPU */
	//	sprintf((char *)v + 2, "CPU$%3d 'C", (schar)((LLCRamRO[k].Temp[0] & 0x00FF) - 64), DEGRES);
	//	break ;
	case 13 :          /* Tp +Vbat */
		sprintf((char *)v + 2, "T +VBAT$%3d 'C", (schar)((LLCRamRO[k].Temp[1] & 0x00FF) - 64), DEGRES);
		break ;
	case 14 :          /* Tp -Vbat */
		sprintf((char *)v + 2, "T -VBAT$%3d 'C", (schar)((LLCRamRO[k].Temp[1] >> 8) - 64), DEGRES);
		break ;
	case 15 :          /* Tp MOS LLC */
        if (Menu.ChgType == TYPE_1PHASE)
		    sprintf((char *)v + 2, "MOS LLC$%3d 'C", (schar)((LLCRamRO[k].Temp[2] >> 8) - 64), DEGRES);
        else
            sprintf((char *)v + 2, "MOSG   $%3d 'C", (schar)((LLCRamRO[k].Temp[2] >> 8) - 64), DEGRES);
		break ;
	case 16 :          /* Tp Transfo */
        if (Menu.ChgType == TYPE_1PHASE)
		    sprintf((char *)v + 2, "TRANSFO$%3d 'C", (schar)((LLCRamRO[k].Temp[2] & 0x00FF) - 64), DEGRES);
        else
            sprintf((char *)v + 2, "MOSD   $%3d 'C", (schar)((LLCRamRO[k].Temp[2] & 0x00FF) - 64), DEGRES);
		break ;
	case 17 :          /* Tp PFC */
        if (Menu.ChgType == TYPE_1PHASE)
		    sprintf((char *)v + 2, "PFC$%3d 'C", (schar)((LLCRamRO[k].Temp[3] >> 8) - 64), DEGRES);
        else
            sprintf((char *)v + 2, "PDE$%3d 'C", (schar)((LLCRamRO[k].Temp[3] >> 8) - 64), DEGRES);
		break ;
	case 18 :          /* Tp Flyback */
        if (Menu.ChgType == TYPE_1PHASE)
		    sprintf((char *)v + 2, "FlyBack$%3d 'C", (schar)((LLCRamRO[k].Temp[3] & 0x00FF) - 64), DEGRES);
        else
            sprintf((char *)v + 2, "DS     $%3d 'C", (schar)((LLCRamRO[k].Temp[3] & 0x00FF) - 64), DEGRES);
		break ;
	}
}

void GetSetFilPilOnOff(uint a, uchar *v)
{
	/* a = 0 : set value, a = 1 : get value, a = 2 : control value */
	if (a == 0)
	{
		if (*v == ON)
			FilPilOn;
		else
			FilPilOff;
		MenuWriteOn;
	}
	else
	{
		if (FilPil == 0)
			*v = OFF;
		else
			*v = ON;
	}
}

void  GetSetRefreshOffOn(uint a, uchar *v)
{
	/* Set Refresh On or Off  */
	/* a = 0 : set value, a = 1 : get value, a = 2 : control value */
	if (a == 0)
	{
		if (*v == OFF)
			Menu.RefreshOffOn = OFF;
		else
			Menu.RefreshOffOn = ON;
		MenuWriteOn;
	}
	else
	{
		if (Menu.RefreshOffOn == OFF)
			*v = OFF;
		else
			*v = ON;
	}
}

void  GetSetXFCOffOn(uint a, uchar *v)
{
	/* Set XFC profile On or Off  */
	/* a = 0 : set value, a = 1 : get value, a = 2 : control value */
    if (a == 0)
    {
        if (*v == OFF)
        {
            Menu.XFCOffOn = OFF;
        }
        else
        {
            Menu.XFCOffOn = ON;
        }
        if (Menu.XFCOffOn == 0)
        {
            Menu.Profile = IONIC;
        }
		
		// MODIF 3.3
	#ifdef ENERSYS_EU
	    if (Menu.XFCOffOn != 0)
		{
	    	Menu.Branding = BRAND_LIFE_IQ_NEXSYS;
		}
	    else
		{
		   	Menu.Branding = BRAND_LIFE_IQ;
		}
	#endif
	#ifdef ENERSYS_US
	    if (Menu.XFCOffOn != 0)
		{
	    	Menu.Branding = BRAND_IMPAQ_NEXSYS;
		}
	    else
		{
		   	Menu.Branding = BRAND_IMPAQ_PLUS;
		}
	#endif
	#ifdef HAWKER_US
	    if (Menu.XFCOffOn != 0)
		{
	    	Menu.Branding = BRAND_MODC_NEXSYS;
		}
	    else
		{
		   	Menu.Branding = BRAND_MODC;
		}
	#endif   
		
        MenuWriteOn;
        LCDMenu_Level();
    }
	else
	{
		if (Menu.XFCOffOn == OFF)
			*v = OFF;
		else
			*v = ON;
	}
}


void GetSetIdleOffOn(uint a, uchar *v)
{
	/* Set Idle On or Off  */
	/* a = 0 : set value, a = 1 : get value, a = 2 : control value */
	if (a == 0)
	{
		if (*v == OFF)
		{
			IdleStateOff;
			IdleOff;
		}
		else
		{
			IdleStateOn;
			IdleOn;
		}
		MenuWriteOn;
	}
	else
	{
		if (IdleState != 0)
			*v = ON;
		else
			*v = OFF;
	}
}

/**
 * @brief	Set or gets Profile auto equal On/Off
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @param	v  profile value
 * @retval None
 */
void GetSetProfileEqual(uint a, uchar *v)
{
	if (a == 0)
	{
		if (*v == OFF)
			ProfileEqualOff;
		else
			ProfileEqualOn;
		MenuWriteOn;
	}
	else
	{
		if (ProfileEqual != 0)
			*v = ON;
		else
			*v = OFF;
	}
}

/**
 * @brief	Set or gets VMains
 * @param  a = 0 : get value, 0 = 1 set value
 * @param	v  profile value
 * @retval None
 */
void  GetSetVMains(uint a, uchar *v)
{
	if (a == 0)
	{
		Menu.VMains = *v;
		MenuWriteOn;
	}
	else
	{
		*v = Menu.VMains;
	}
}

/**
 * @brief	Set or gets VMains 400/480VAC 
 * @param  a = 0 : get value, a = 1 set value
 * @param
 * @retval None
 */
void  GetSetVMains480(uint a, uchar *v)
{   // MODIF R2.1
	if (a == 0)
	{
        if (*v == OFF)
		    Mains480VACOff;
        else
            Mains480VACOn;    
		MenuWriteOn;
	}
	else
	{
		if (Mains480VAC == 0)
            *v = ON;
        else
            *v = OFF;
	}
}


/**
 * @brief	Set or get the battery rest after charge
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetBatteryRest(uint a, uchar *v)
{

	uint  h;
	/* Select case depending on MSB, MSB = 0 : hour, MSB = 1 : delay */
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%2u", &h);
	}
	if (a == 1)
	{
		h = Menu.BatteryRest;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (23 < h)
	{
		h = 23;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xC0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		Menu.BatteryRest = h;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%2.2uh", h);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

void  GetSetContrast(uint a, uchar *v)
{
	/* Set or get Contrast of lcd display */
	/* a = 0 : get value, a = 1 : set value, a = 2 : control value */

	uint  j;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%2u", &j);
	}
	if (a == 1)
	{
		j = Menu.Contrast;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (j < 20)
	{
		j = 20;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else  if (60 < j)
	{
		j = 60;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xC0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
		Menu.Contrast = j;
	}
	/* Write value */
	if (a == 0)
	{
		Menu.Contrast = j;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%2.2u", j);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

/**
 * @brief	Set Screen saver ON or OFF
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void GetSetBackOnOff(uint a, uchar *v)
{
	if (a == 0)
	{
		if (*v == ON)
			BackOff;
		else
			BackOn;
		LCD_RefreshScreenSaver();
		MenuWriteOn;
	}
	else
	{
		if (BackOnOff != 0)
			*v = ON;
		else
			*v = OFF;
	}
}


/**
 * @brief	Set or get the Screen Saver time for charge
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval
 */
void  GetSetScreenSaverTime(uint a, uchar *v)
{

	uint  h;                      /* Hours */
	uint  m;                      /* Minute */
	/* Select case depending on MSB, MSB = 0 : hour, MSB = 1 : delay */
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%2u", &h);
		sscanf((const char *)v + 5, "%2u", &m);
	}
	if (a == 1)
	{
		h = (Menu.TimeDisplaySave /60) % 24 ;
		m = Menu.TimeDisplaySave % 60;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (1 < h)
	{
		h = 1;
		*v = 0;
		*(v + 1) = 0;
	}
	else  if (59 < m)
	{
		m = 59;
		*v = 0;
		*(v + 1) = 0;
	}
	else  if ((1 > m) && (h == 0))
	{
		m = 1;
		*v = 0;
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xD8;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		Menu.TimeDisplaySave = 60 * h + m;
		LCD_RefreshScreenSaver();
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%2.2uh%2.2u", h, m);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}


/**
 * @brief	 Set or get periodicity of equalization
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval
 */
void  GetSetPeriod(uint a, uchar *v)
{
	uint  j;
	/* Read value */
	//sprintf(&line1[0], "%16.16s", &ListWord16[0][LANG][0]);

	if ((a == 0) || (a == 2))
	{
		j = *(v + 2) * 256  + *(v + 3);
	}
	if (a == 1)
	{
		j = UcharRev[Menu.EqualPeriod] / 2;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	{                 /* No error mask for editing */
		*(v + 0) = 0x00;              /* PF of mask */
		*(v + 1) = 0x7F;              /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		Menu.EqualPeriod = UcharRev[j] / 2;
		MenuWriteOn;
	}
	if (a == 1)
	{
		*(v + 2) = 0x00;                                /* PF of value */
		*(v + 3) = UcharRev[Menu.EqualPeriod] / 2;      /* pf of value */
		sprintf((char *)v + 4," ");   /* 16 char */
		*(v + 20) = SPACE;          /* Suppress final \0 */
	}
}


/**
 * @brief	Set or get the Electrovalve time
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetValveDelay(uint a, uchar *v)
{
	/* Delay for valve */
	/* a = 0 : set value, a = 1 : get value, a = 2 : control value */
	uint  j;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%3u", &j);
		Menu.EVtime = j;
	}
	if (a == 1)                   /* Get value */
	{
		j = Menu.EVtime;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	/*if (j < 30)
  {
  j = 30;
	 *v = 0;
	 *(v + 1) = 0;
  }
  else */
	if (900 < j)
	{
		j = 900;
		*v = 0;
		*(v + 1) = 0;
	}
	else
	{                           /* No error mask for editing */
		*v = 0xE0;                  /* Mask for editing */
		*(v + 1) = 0x00;
	}
	/* Write value */
	if (a == 0)
	{
		Menu.EVtime = j;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%3.3u s", Menu.EVtime);
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

/**
 * @brief	Set or get options test
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetTestOptions(uint a, uchar *v)
{
	/* Test of the pump */
	/* a = 0 : set value, a = 1 get value */
	if (a == 0)
	{
		UsePumpOff;                 /* Stop the pump */
		ManuOptionOff;              /* Stop options */
		StateElectOff;
		StateGreenOff;
		StateYellowOff;
	}
	else
	{
		if (TimerMnuOption == 0)
		{
			TimerMnuOption = State.TimerSec + OPTIONSTESTTIME;
			UsePumpOn;
			ManuOptionOn;
			StateElectOn;
		}
		if (TimerMnuOption < State.TimerSec)
		{
			UsePumpOff;                 /* Stop the pump */
			ManuOptionOff;              /* Stop options */
			StateElectOff;
			StateGreenOff;
			StateYellowOff;
		}
		else
		{
			/* 1s green, 2s red */
			if ((State.TimerSec & 1) == 0)
			{
				StateGreenOn;
				StateRedOff;
                StateYellowOff;
				OUT_FAULT = 0;
			}
			else
			{
				StateGreenOff;
                StateRedOn;
				OUT_FAULT = 1;
				if (750 < (State.TimerMs % 1000))
				{
					StateYellowOff;
				}
				else if (500 < (State.TimerMs % 1000))
				{
					StateYellowOn;
				}
				else if (250 < (State.TimerMs % 1000))
				{
					StateYellowOff;
				}
				else
				{
					StateYellowOn;
				}
			}
		}
		/* Display of fault */
		memset(v, SPACE, 20);
		if (UsePump != 0)
		{
			if (PumpDef != 0)
				sprintf((char *)v + 2, "DF PUMP  %2u s",(uint)(TimerMnuOption - State.TimerSec));
			else
				sprintf((char *)v + 2, "%2u s", (uint)(TimerMnuOption - State.TimerSec));
		}
		else
			sprintf((char *)v + 2, "OFF");
		*(v + 18) = SPACE;
	}
}


/**
 * @brief	Set Com IQ On or Off
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetComIQOffOn(uint a, uchar *v)
{
	if (a == 0)
	{
		if (*v == OFF)
		{
			Menu.SkipComIQ = ON;
		}
		else
		{
			Menu.SkipComIQ = OFF;
		}
		MenuWriteOn;
	}
	else
	{
		if (Menu.SkipComIQ == OFF)
			*v = ON;
		else
			*v = OFF;
	}
}

/**
 * @brief	Get/Set RF Transceiver address
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetModRFTrAddr(uint a, uchar *v)
{
	ulong  j;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%8lx", &j);
	}
	if (a == 1)
	{
		j = IQData.RFTrNewAddr;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (0x003FFFFF < j)
	{
		j = 0x00300000;
		*v = 0x00;
		*(v + 1) = 0;
	}
	else if (j < 0x00200000)
	{
		j = 0x00200000;
		*v = 0x00;
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0x3F;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		IQData.RFTrNewAddr = j;
		IQInRFTrModAddrOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"0x%6.6lX", j);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

/**
 * @brief	Set or get the SERIAL NUMBER
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetSerialNumber(uint a, uchar *v)
{
	/* Set SerialNumber (12 car ascii) */
	/* a = 0 : set value, a = 1 : get value, a = 2 : control value */
	uint i;
	StructSerialNumber *Temp;
	Temp = (StructSerialNumber *)&LcdTemp[0];
	/* Read value */
	if (a == 0)
	{
		if ((*(v + 2)) == SPACE)
			memset(&(*Temp).SerNum[0], SPACE, 10);
		else
		{
			memset(&(*Temp).SerNum[0], 0, 10);
			sscanf((const char *)v + 2, "%10s", &(*Temp).SerNum[0]);
		}
		Write_SegFlashData(SEGSERIAL, (uchar *)Temp);
		Read_SegFlashData (SEGSERIAL, (uchar *)&SerialNumber);
	}
	/* Control value */
	memset(v, 0, 18);
	// Set bit to 1 into PF and Pf mask
	*(v + 0) = 0xFF;            /* PF of mask - 8 digits */
	*(v + 1) = 0x80;            /* pf of mask - 2 digits */
	if (a == 1)
	{
		memset(&(*Temp).SerNum[0], SPACE, 10);
		Read_SegFlashData (SEGSERIAL, (uchar *)Temp);
		memcpy(v + 2, &(*Temp).SerNum[0], 10);

		/* Replace nul with space */
		i = 2;
		do
		{
			if (*(v + i) == 0x00)
				*(v + i) = SPACE;
			i ++;
		}
		while (i < 18);
		*(v + 18) = SPACE;                 /* Suppress final \0 */
	}
}


/**
 * @brief	Set or get Max temperature of battery
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetTempHighC(uint a, uchar *v)
{
	sint  j;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%3d", &j);
	}
	if (a == 1)
	{
		j = Menu.BatHighTemp;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (j < 40)
	{
		j = 40;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else if (70 < j)
	{
		j = 70;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xE0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		Menu.BatHighTemp = j;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%+2.2d'C", j);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

/**
 * @brief	Set or get Max temperature of battery
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetTempHighF(uint a, uchar *v)
{
	sint  j;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%3d", &j);
	}
	if (a == 1)
	{
		j = ((sint)Menu.BatHighTemp * 9) / 5 + 32;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (j < 104)
	{
		j = 104;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else if (158 < j)
	{
		j = 158;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xE0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		Menu.BatHighTemp = (((j - 32) * 50) + 50) / 90;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%3.3d'F", j);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}


void  GetSetProtocol(uint a, uchar *v)
{
	/* Set or get section of cable */
	/* a = 0 : get value, 0 = 1 set value */
	if (a == 0)
	{
		Menu.Network = *v;
		Uart0Init();
		MenuWriteOn;
	}
	else
	{
		*v = Menu.Network;
	}
}

void GetSetBaudRate(uint a, uchar *v)
{
	/* Set or get network speed */
	if (a == 0)
	{
		Menu.JbSpeed = *v;
		MenuWriteOn;
		Uart0Init();
	}
	else
	{
		*v = Menu.JbSpeed;
	}
}

void  GetSetJbAdr(uint a, uchar *v)
{
	/* Set or get Jbus address */
	/* a = 0 : get value, a = 1 : set value, a = 2 : control value */
	uint  j;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%3u", &j);
	}
	if (a == 1)
	{
		j = Menu.JbEsc;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (j < 1)
	{
		j = 1;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else  if (255 < j)
	{
		j = 255;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xE0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		Menu.JbEsc = j;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%3.3u", j);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}


StructLanConfig *Temp;
StructLanConfig *Temp2;

void  GetSetEth(uint a, uchar *v, uchar c)
{
	/* Set or get Ethernet address */
	/* a = 0 : get value, a = 1 : set value, a = 2 : control value */
	/* c = 0 : IP, c = 1 : DNS, c = 2 : GW, c = 3 : SM */
	uchar k;
	uchar e;
	uint crc;
	uint  j[5];


	Temp = (StructLanConfig *)&LcdTemp[0];
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		k = 0;
		do
		{
			sscanf((const char *)v + 2 + 4 * k, "%3u", &j[k]);
			k ++;
		}
		while (k < 4);
	}
	if (a == 1)
	{
		crc = Read_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
		if (crc != (*Temp).Crc)
		{
			InitEthernetParam ();
			Read_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
			Read_SegFlashData (SEGLANCONFIG, (uchar *)Temp2);
		}
		else
		{
			Read_SegFlashData (SEGLANCONFIG, (uchar *)Temp2);
		}
		k = 0;
		do
		{
			j[k] = (*Temp).EthParam[k + 4 * c];
			k ++;
		}
		while (k < 4);
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	e = 0;
	k = 0;
	do
	{
		if (j[k] < 0)
		{
			j[k] = 0;
			e  = 1;
		}
		else
			if (255 < j[k])
			{
				j[k] = 255;
				e = 1;
			}
		k ++;
	}
	while (k < 4);
	if (e == 1)
	{
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0xEE;            /* PF of mask */
		*(v + 1) = 0xEE;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		k = 0;
		do
		{
			(*Temp2).EthParam[k + 4 * c] = j[k];
			k ++;
		}
		while (k < 4);
		Write_SegFlashData (SEGLANCONFIG, (uchar *)Temp2);
		Uart0Init();
        // MODIF R2.1
        IP_DHCP[0] = (*Temp).EthParam[0];
        IP_DHCP[1] = (*Temp).EthParam[1];
        IP_DHCP[2] = (*Temp).EthParam[2];
        IP_DHCP[3] = (*Temp).EthParam[3];
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%3.3u.%3.3u.%3.3u.%3.3u", j[0], j[1], j[2], j[3]);  /* Place 16 car */
		//*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

void  GetSetEthIP(uint a, uchar *v)
{
	GetSetEth(a, v, 0);
}

void  GetSetEthDNS(uint a, uchar *v)
{
	GetSetEth(a, v, 1);
}

void  GetSetEthGW(uint a, uchar *v)
{
	GetSetEth(a, v, 2);
}

void  GetSetEthSM(uint a, uchar *v)
{
	GetSetEth(a, v, 3);
}

void  GetSetEthA10100(uint a, uchar *v)
{
	/* Set or get section of cable */
	/* a = 0 : get value, 0 = 1 set value */
	//StructLanConfig *Temp;
	Temp = (StructLanConfig *)&LcdTemp[0];
	if (a == 0)
	{
		(*Temp).A10100 = *v;
		Write_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
		Uart0Init();
	}
	else
	{
		Read_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
		//(*Temp).A10100 = 0;
		*v = (*Temp).A10100;
	}
}


/**
 * @brief	Set profile recording On or Off
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetUsbProfileWrite(uint a, uchar *v)
{

	if (a == 0)
	{
		if (*v == ON)
		{
			if (UsbWriteProfile == 0)
				UsbWriteProfileOn;
		}
		else
		{
			usb_ProfileClose();
		}
	}
	else
	{
		if (UsbWriteProfile == 0)
			*v = OFF;
		else
			*v = ON;
	}
}

void  GetSetUsbProfileTime(uint a, uchar *v)
{
	/* Set time betweeb profile records  */
	/* a = 0 : set value, a = 1 : get value, a = 2 : control value */
	uint  j;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%3u", &j);
	}
	if (a == 1)
	{
		j = Menu.VncProfileTime;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (j < 2)
	{
		j = 2;
		*v = 0;
		*(v + 1) = 0;
	}
	else
		if (600 < j)
		{
			j = 600;
			*v = 0;
			*(v + 1) = 0;
		}
		else
		{                         /* No error mask for editing */
			*(v + 0) = 0xE0;          /* PF of mask */
			*(v + 1) = 0x00;          /* pf of mask */
		}
	/* Write value */
	if (a == 0)
	{
		Menu.VncProfileTime = j;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%3.3u s", j);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

void GetSetUsbPrgRf(uint a, uchar *v)
{
	/* Set memo transfer to Usb disk  */
	/* a = 0 : set value, a = 1 : get value, a = 2 : control value */
	/* DataR.VncProfilePtr = 0 : Off, Other : On */
	if (a == 0)
	{
		if (*v == ON)
		{
			if (UsbDisk != 0)
			{
				UsbRfProgOn;
				LCDMenu_Level();                    /* Returns to top menu level */
			}
		}
	}
	else
		*v = OFF;
}

void GetSetBlockOutChargeType (uint a, uchar *v)
{
	if (a == 0)
	{
		switch (*v)
		{
		case 0:
			Menu.BlockOutEnable = 0;
			break;
		case 1:
			Menu.BlockOutEnable = 1;
			break;
		}
		MenuWriteOn;
	}
	else
	{
		if (Menu.BlockOutEnable != 0)
		{
			*v = 1;
		}
		else
		{
			*v = 0;
		}
	}
}

void GetSetBlockOutStartTime(uint a, uchar *v)
{
	uint  h;
	uint  m;
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%2u", &h);
		sscanf((const char *)v + 5, "%2u", &m);
	}
	if (a == 1)
	{
		h = (Menu.BlockOutStart /60) % 24;
		m = Menu.BlockOutStart % 60;
		memset(v, SPACE, 20);
	}
	if (23 < h)
	{
		h = 23;
		*v = 0;
		*(v + 1) = 0;
	}
	else  if (59 < m)
	{
		m = 59;
		*v = 0;
		*(v + 1) = 0;
	}
	else
	{
		*(v + 0) = 0xD8;
		*(v + 1) = 0x00;
	}
	if (a == 0)
	{
		Menu.BlockOutStart = 60 * h + m;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%2.2uh%2.2u", h, m);
		*(v + 18) = SPACE;
	}
}

void GetSetBlockOutEndTime(uint a, uchar *v)
{
	uint  h;
	uint  m;
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%2u", &h);
		sscanf((const char *)v + 5, "%2u", &m);
	}
	if (a == 1)
	{
		h = (Menu.BlockOutEnd /60) % 24;
		m = Menu.BlockOutEnd % 60;
		memset(v, SPACE, 20);
	}
	if (23 < h)
	{
		h = 23;
		*v = 0;
		*(v + 1) = 0;
	}
	else  if (59 < m)
	{
		m = 59;
		*v = 0;
		*(v + 1) = 0;
	}
	else
	{
		*(v + 0) = 0xd8;
		*(v + 1) = 0x00;
	}
	if (a == 0)
	{
		Menu.BlockOutEnd = 60 * h + m;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%2.2uh%2.2u", h, m);
		*(v + 18) = SPACE;
	}
}


void GetSetDailyChargeTimeType (uint a, uchar *v)
{
	if (a == 0)
	{
		switch (*v)
		{
		case 0:
			Menu.ChgDailyHour = 1; // Always 'Time of day'
			Menu.ChgSkipDaily = 0; // Daily Charge - ON
			break;
		case 1:
			Menu.ChgDailyHour = 1; // Always 'Time of day'
			Menu.ChgSkipDaily = 1; // Daily Charge - OFF
			break;
		}
		MenuWriteOn;
	}
	else
	{
		if (Menu.ChgSkipDaily != 0)
		{
			*v = 1;
		}
		else
		{
			*v = 0;
		}
	}
}

void GetSetDailyChargeStartTime(uint a, uchar *v)
{
	uint  h;
	uint  m;
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%2u", &h);
		sscanf((const char *)v + 5, "%2u", &m);
	}
	if (a == 1)
	{
		h = (Menu.ChgDailyStart /60) % 24;
		m = Menu.ChgDailyStart % 60;
		memset(v, SPACE, 20);
	}
	if (23 < h)
	{
		h = 23;
		*v = 0;
		*(v + 1) = 0;
	}
	else  if (59 < m)
	{
		m = 59;
		*v = 0;
		*(v + 1) = 0;
	}
	else
	{
		*(v + 0) = 0xD8;
		*(v + 1) = 0x00;
	}
	if (a == 0)
	{
		Menu.ChgDailyHour = 1; // Always 'Time of day'
		Menu.ChgDailyStart = 60 * h + m;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%2.2uh%2.2u", h, m);
		*(v + 18) = SPACE;
	}
}

void GetSetDailyChargeEndTime(uint a, uchar *v)
{
	uint  h;
	uint  m;
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%2u", &h);
		sscanf((const char *)v + 5, "%2u", &m);
	}
	if (a == 1)
	{
		h = (Menu.ChgDailyEnd /60) % 24;
		m = Menu.ChgDailyEnd % 60;
		memset(v, SPACE, 20);
	}
	if (23 < h)
	{
		h = 23;
		*v = 0;
		*(v + 1) = 0;
	}
	else  if (59 < m)
	{
		m = 59;
		*v = 0;
		*(v + 1) = 0;
	}
	else
	{
		*(v + 0) = 0xd8;
		*(v + 1) = 0x00;
	}
	if (a == 0)
	{
		Menu.ChgDailyHour = 1; // Always 'Time of day'
		Menu.ChgDailyEnd = 60 * h + m;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%2.2uh%2.2u", h, m);
		*(v + 18) = SPACE;
	}
}
/* End new code for 1B-List #19 jmf 05/09/2012 */

#ifdef	CALIFORNIA
#define	MAXIONICCOEFFICIENT		9
#else
#define	MAXIONICCOEFFICIENT		20
#endif

void GetSetIonicCoef(uint a, uchar *v)  /* jmf 10/29/2012 New function for IONIC Charge Coefficient */
{
	uint  j;
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%2u", &j);
	}
	if (a == 1)
	{
		j = Menu.IonicCoef;
		memset(v, SPACE, 20);
	}
	if (j < 1)
	{
		j = 1;
		*v = 0;
		*(v + 1) = 0;
	}
	else if (MAXIONICCOEFFICIENT < j)	/* was 20 wcc3 11/15/2013 CALIFORNIA */
	{
		j = MAXIONICCOEFFICIENT;		/* was 20 wcc3 11/15/2013 CALIFORNIA */
		*v = 0;
		*(v + 1) = 0;
	}
	else
	{
		*(v + 0) = 0xC0;
		*(v + 1) = 0x00;
	}
	if (a == 0)
	{
		Menu.IonicCoef = j;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%2.2u %%", j);
		*(v + 18) = SPACE;
	}
}


/**
 * @brief	Set or get the charge Factor
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetCF(uint a, uchar *v)
{

	uint  j;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%3u", &j);
	}
	if (a == 1)
	{
		j = Menu.CoefCharge;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value : no low limit, only limited when writing */
	/* Be careful to reach and leave value 100 */
	if (120 < j)
	{
		j = 200;
		*v = 0;           /* Error */
		*(v + 1) = 0;
	}
	else
	{                             /* No error mask for editing */
		*(v + 0) = 0xE0;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		if (j < 50)
			j = 50;
		Menu.CoefCharge = j;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%3.3u", j);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

/**
 * @brief	Set or get the charge Factor On/Off
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void GetSetCFOnOff(uint a, uchar *v)
{
	/* Set Idle On or Off  */
	/* a = 0 : set value, a = 1 : get value, a = 2 : control value */
	if (a == 0)
	{
		if (*v == OFF)
		{
			Menu.CoefOnOff = 0;
		}
		else
		{
			Menu.CoefOnOff = 1;
		}
		MenuWriteOn;
	}
	else
	{
		if (Menu.CoefOnOff != 0)
			*v = ON;
		else
			*v = OFF;
	}
}

/**
 * @brief	Set or get the charge Factor On/Off
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void GetSetExit(uint a, uchar *v)
{
}


/**
 * @brief	Set or get the charge Factor On/Off
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void GetSetCECOnOff(uint a, uchar *v)
{
	/* Set Idle On or Off  */
	/* a = 0 : set value, a = 1 : get value, a = 2 : control value */
	if (a == 0)
	{
		if (*v == OFF)
		{
			Menu.CECOnOff = 0;
		}
		else
		{
			Menu.CECOnOff = 1;
		}
		MenuWriteOn;
	}
	else
	{
		if (Menu.CECOnOff != 0)
			*v = ON;
		else
			*v = OFF;
	}
}

void GetSetGelIphase3(uint a, uchar *v)
{
	uint  j;
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%2u", &j);
	}
	if (a == 1)
	{
		j = Menu.VRLAIphase3;
		memset(v, SPACE, 20);
	}
	if (j < 1)
	{
		j = 1;
		*v = 0;
		*(v + 1) = 0;
	}
	else if (5 < j)
	{
		j = 5;
		*v = 0;
		*(v + 1) = 0;
	}
	else
	{
		*(v + 0) = 0xC0;
		*(v + 1) = 0x00;
	}
	if (a == 0)
	{
		Menu.VRLAIphase3 = j;
		MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"%2.2u %%", j);
		*(v + 18) = SPACE;
	}
}

/**
 * @brief	Set Loop On or Off in COLD profile
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval
 */
void  GetSetLoopOffOn(uint a, uchar *v)
{
	if (a == 0)
	{
		if (*v == OFF)
			Menu.LoopOnOff = OFF;
		else
			Menu.LoopOnOff = ON;
		MenuWriteOn;
	}
	else
	{
		if (Menu.LoopOnOff == OFF)
			*v = OFF;
		else
			*v = ON;
	}
}

/**
 * @brief	Get set SSID1
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval
 */
void  GetSetEthSSID1(uint a, uchar *v)
{
	/* Set SSID1 */
	GetSetEthSSID(a, v, 0);
}

/**
 * @brief	Get set SSID2
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval
 */
void  GetSetEthSSID2(uint a, uchar *v)
{
	/* Set SSID2 */
	GetSetEthSSID(a, v, 1);
}

/**
 * @brief	Get set SSID
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval
 */
void  GetSetEthSSID(uint a, uchar *v, uchar c)
{
	/* Set SSID (32 car ascii) */
	/* a = 0 : set value, a = 1 : get value, a = 2 : control value */
	uint i;
	//StructLanConfig *Temp;
	Temp = (StructLanConfig *)&LcdTemp[0];
	/* Read value */
	if (a == 0)
	{
		/* Read value */
		if (c == 0)
		{
			if ((*(v + 2)) == SPACE)
				memset(&(*Temp).SSID[0], SPACE, 16);
			else
			{
				memset(&(*Temp).SSID[0], 0, 16);
				sscanf((const char *)v + 2, "%16s", &(*Temp).SSID[0]);
			}
		}
		else
		{
			if ((*(v + 2)) == SPACE)
				memset(&(*Temp).SSID[16], SPACE, 16);
			else
			{
				memset(&(*Temp).SSID[16], 0, 16);
				sscanf((const char *)v + 2, "%16s", &(*Temp).SSID[16]);
			}
		}
		Write_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
		Uart0Init();
	}
	/* Control value */
	*(v + 0) = 0xFF;            /* PF of mask */
	*(v + 1) = 0xFF;            /* pf of mask */
	if (a == 1)
	{
		i = Read_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
		if (i != (*Temp).Crc)
		{
			memset(&(*Temp).SSID[0], SPACE, 32);
		}
		if (c == 0)
			memcpy(v + 2, &(*Temp).SSID[0], 16);
		else
			memcpy(v + 2, &(*Temp).SSID[16], 16);
		/* Replace nul with space */
		i = 2;
		do
		{
			if (*(v + i) == 0x00)
				*(v + i) = SPACE;
			i ++;
		}
		while (i < 18);
		*(v + 18) = SPACE;                 /* Suppress final \0 */
	}
}

void GetSetEthSecurity(uint a, uchar *v)
{
	uint i;
	/* Set or get section of cable */
	/* a = 0 : get value, 0 = 1 set value */
	//StructLanConfig *Temp;
	Temp = (StructLanConfig *)&LcdTemp[0];
	if (a == 0)
	{
		(*Temp).Security = *v;
		Write_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
		Uart0Init();
	}
	else
	{
		i = Read_SegFlashData (SEGLANCONFIG, (uchar *)Temp);
		if (i != (*Temp).Crc)
			(*Temp).Security = 0;
		*v = (*Temp).Security;
	}
}


void  GetSetEthPassPh1(uint a, uchar *v)
{
	/* Set passphrase1 */
	GetSetEthPassPh(a, v, 0);
}

void  GetSetEthPassPh2(uint a, uchar *v)
{
	/* Set passphrase2 */
	GetSetEthPassPh(a, v, 1);
}


StructWifiConfig *TempWifi;

void  GetSetEthPassPh(uint a, uchar *v, uchar c)
{
	/* Set WEP (26 car hexa) */
	/* a = 0 : set value, a = 1 : get value, a = 2 : control value */
	uint i;
	//StructWifiConfig *TempWifi;
	TempWifi = (StructWifiConfig *)&LcdTemp[0];
	/* Read value */
	if (a == 0)
	{
		if (c == 0)
		{
			if ((*(v + 2)) == SPACE)
				memset(&(*TempWifi).WLanKey[0], SPACE, 16);
			else
			{
				memset(&(*TempWifi).WLanKey[0], 0, 16);
				sscanf((const char *)v + 2, "%16s", &(*TempWifi).WLanKey[0]);
			}
		}
		else
		{
			if ((*(v + 2)) == SPACE)
				memset(&(*TempWifi).WLanKey[16], SPACE, 16);
			else
			{
				memset(&(*TempWifi).WLanKey[16], 0, 16);
				sscanf((const char *)v + 2, "%16s", &(*TempWifi).WLanKey[16]);
			}
		}
		Write_SegFlashData (SEGWIFI, (uchar *)TempWifi);
		Uart0Init();
	}
	/* Control value */
	memset(v, 0, 18);
	*(v + 0) = 0xFF;            /* PF of mask */
	*(v + 1) = 0xFF;            /* pf of mask */
	if (a == 1)
	{
		i = Read_SegFlashData (SEGWIFI, (uchar *)TempWifi);
		if (i != (*TempWifi).Crc)
		{
			memset(&(*TempWifi).WLanKey[0], 0, 26);
		}
		if (c == 0)
			memcpy(v + 2, &(*TempWifi).WLanKey[0], 16);
		else
			memcpy(v + 2, &(*TempWifi).WLanKey[16], 16);
		/* Replace nul with space at display */
		i = 2;
		do
		{
			if (*(v + i) == 0x00)
				*(v + i) = SPACE;
			i ++;
		}
		while (i < 18);
		*(v + 18) = SPACE;                 /* Suppress final \0 */
	}
}

void GetSetBootApp (uint a, uchar *v)
{


}

void  GetSetDayLight(uint a, uchar *v)
{
	if (a == 0)
	{
		Menu.Daylight = *v;
		MenuWriteOn;
		CalcNextDayLight(0);
	}
	else
	{
		*v = Menu.Daylight;
	}
}


void  GetSetEasyKitAdr(uint a, uchar *v)
{
	ulong  j;
	/* Read value */
	if ((a == 0) || (a == 2))
	{
		sscanf((const char *)v + 2, "%8lx", &j);
	}
	if (a == 1)
	{
		j = AarsParam.EasyKitAddr;
		memset(v, SPACE, 20);       /* Clear v */
	}
	/* Control value */
	if (0x004FFFFF < j)
	{
		j = 0x00400000;
		*v = 0x00;
		*(v + 1) = 0;
	}
	else if (j < 0x00300000)
	{
		j = 0x00400000;
		*v = 0x00;
		*(v + 1) = 0;
	}
	else
	{                 /* No error mask for editing */
		*(v + 0) = 0x3F;            /* PF of mask */
		*(v + 1) = 0x00;            /* pf of mask */
	}
	/* Write value */
	if (a == 0)
	{
		AarsParam.EasyKitAddr = j;
        MenuWriteOn;
	}
	if (a == 1)
	{
		sprintf((char *)v + 2,"0x%6.6lX", j);  /* Place 16 car */
		*(v + 18) = SPACE;          /* Suppress final \0 */
	}
}

void  GetSetEasyKitOnOff(uint a, uchar *v)
{
    /* Set EasyKit On or Off  */
    /* a = 0 : set value, a = 1 : get value, a = 2 : control value */
    if (a == 0)
      {
      if (*v == OFF)
    	  AarsParam.EasyKitOnOff = OFF;
      else
    	  AarsParam.EasyKitOnOff = ON;
      MenuWriteOn;
      }
    else
      {
      if (AarsParam.EasyKitOnOff == OFF)
        *v = OFF;
      else
        *v = ON;
      }
}

/**
 * @brief   Set or get PLC pulse
 * @param   a = 0 : get value, 0 = 1 set value
 * @param	v  
 * @retval  None
 */
void  GetSetPLCPulseOffOn(uint a, uchar *v)
{
	if (a == 0)
	{
		if (*v == OFF)
			Menu.PLCPulseOffOn = ON;
		else
        {
			Menu.PLCPulseOffOn = OFF;
            Menu.AutoStartOffOn = 0;
        }
		MenuWriteOn;
	}
	else
	{
		if (Menu.PLCPulseOffOn == OFF)
			*v = ON;
		else
			*v = OFF;
	}
}

/**
 * @brief	Set or gets AarsParam.NbModLeft
 * @param  a = 0 : get value, 0 = 1 set value
 * @param	v  AarsParam.NbModLeft
 * @retval None
 */
void  GetSetNbMod(uint a, uchar *v)
{   // MODIF 2.8 : new function
	if (a == 0)
	{
		AarsParam.NbModLeft = *v;
        if ((Menu.ImaxCharger > 3200) && (AarsParam.NbModLeft != OUTPUT_2_CABLE_6M) && (AarsParam.NbModLeft != OUTPUT_2_CABLE_8M))
            Menu.ImaxCharger = 3200;    // Limit to 320A if not 2 output charger
		MenuWriteOn;
	}
	else
	{
		*v = AarsParam.NbModLeft;
	}
}

/**
 * @brief	Get/Set the Theme
 * @param  a = 0 : set value, a = 1 : get value, a = 2 : control value
 * @retval None
 */
void  GetSetTheme(uint a, uchar *v)
{
	if (a == 0)
	{
		Menu.Theme = *v;
		MenuWriteOn;
        LCDMenu_Level();
	}
	else
	{
		*v = Menu.Theme;
	}
}
