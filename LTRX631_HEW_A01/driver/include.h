/*
 * include.h
 *
 *  Created on: 22 feb. 2013
 *      Author: duhautf
 */

#include <machine.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "hwsetup.h"
#include "rskrx63ndef.h"
#include "iodefine.h"
#include "type.h"
#include "vect.h"


#include "charge.h"
#include "lcdcharge.h"
#include "lcdmenu.h"
#include "module.h"
#include "profile_equal.h"
#include "profile_ionic.h"
#include "profile_pzm.h"
#include "profile_test.h"
#include "profile_xfc.h"
#include "profile_gel.h"
#include "profile_rgt.h"
#ifndef ENERSYS_EU  // MODIF 2.8
	#include "profile_cold.h"
	#include "profile_cold_loop.h"
#else
	#include "profile_cold_eu.h"
	#include "profile_cold_loop_eu.h"
#endif

#include "profile_pneu.h"
#include "profile_tppl.h"
#include "profile_wf200.h"
#ifndef ENERSYS_EU  // MODIF 2.8
    #include "profile_opp_us.h"
#else
    #include "profile_opp.h"
#endif
    
#include "profile_vrla.h"
#include "profile_desul.h"
#include "profile_oppiui.h"
#include "profile_lwchg.h"
#include "profile_bms.h"

#include "can.h"
#include "can_bms.h"
#include "comiq.h"
#include "crc.h"
#include "csv.h"
#include "flash_data.h"
#include "jbus.h"
#include "tempo.h"
#include "rtc.h"
#include "key.h"
#include "pwm.h"
#include "timer_cmt.h"
#include "sci1.h"
#include "jbusiq.h"
#include "bfm.h"
#include "i2c.h"

#include "graphics.h"
#include "font.h"
#include "menutft.h"
#include "screen.h"
#include "screen_themeb.h"
#include "sst_flash.h"
#include "ext_mem_programmer.h"
#include "external.h"

#include "lantx.h"
#include "uart11.h"

#include "canbootloader.h"

// USB HOST - MASS STORAGE
#include "r_usb_cTypedef.h"         /* Type define */
#include "r_usb_usrconfig.h"        /* USB-H/W register set (user define) */
#include "r_usb_cDefUSBIP.h"        /* USB-FW Library Header */
#include "r_usb_cMacSystemcall.h"   /* uITRON system call macro */
#include "r_usb_cMacPrint.h"        /* Standard IO macro */
#include "r_usb_cKernelId.h"        /* Kernel ID definition */
#include "r_usb_cExtern.h"          /* USB-FW grobal define */
#include "r_usb_hmsc_define.h"      /* HMSC define */
#include "r_TinyFAT.h"              /* TFAT header */
#include "r_usb_ansi.h"
#include "r_usb_api.h"
#include "r_usb_hmsc_extern.h"

// CAN API
#include "r_can_api.h"
#include "config_r_can_rapi.h"
#include "common.h"
#include "focan.h"


extern StructDataR DataR;
extern StructDataW DataW;
extern StructState State;
extern StructChgData ChgData;
extern StructChargerConfig ChargerConfig;
extern StructMenu Menu;
extern StructStatus Status;
extern StructSerialNumber SerialNumber;
extern StructHost Host;
extern StructIQData IQData;
extern StructMemo Memo;
extern StructMemo MemoToSave;
extern StructMemo MemoTemp;
extern StructDataLcd DataLcd;
extern StructLLCRamRO LLCRamRO[MODNB+1];
extern StructLLCRamRW LLCRamRW[MODNB+1];
extern StructLLC LLC[MODNB+1];

extern StructE1Status E1Status;		/* MODIF R3.3 */
extern StructE1Memo E1Memo;			/* MODIF R3.3 */

// MODIF BLE
extern StructDataBle DataBle;
extern StructBle Ble;
extern StructLva Lva;

// MODIF BMS
extern StrcutBMS Bms;

// COMM IQ IMPLEMENTATION
extern StructIQData IQData;
extern StructAarsParam AarsParam;
extern uchar FlagIQEqual;
extern uchar IP_DHCP[4];
extern uchar CntHighTpBat;

// MODIF 2.8
extern ulong TimeCheckRcable;
extern uchar FlagVRegPhase1;

extern ulong TimerBleLedBlink;
extern uchar FlagEqPh2;

extern ulong TimeMaxEqual;

// MODIF 3.3
extern ulong BfmTout;
extern uint32_t can1_timer_tx_bms;
extern uint32_t TimerBmsRX;


