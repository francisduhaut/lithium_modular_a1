/******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized.
* This software is owned by Renesas Electronics Corporation and is  protected
* under all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY,
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR  A
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE  EXPRESSLY
* DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE  LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES
* FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this
* software and to discontinue the availability of this software.
* By using this software, you agree to the additional terms and
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
*******************************************************************************
* Copyright (C) 2010(2011,2012) Renesas Electronics Corpration
* and Renesas Solutions Corp. All rights reserved.
*******************************************************************************
* File Name    : r_usb_ckernelid.h
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : ID Number Definition Header File
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
******************************************************************************/

/* $Id: r_usb_ckernelid.h 143 2012-05-07 09:16:46Z tmura $ */


#ifndef __R_USB_CKERNELID_H__
#define __R_USB_CKERNELID_H__

/*****************************************************************************
Macro definitions
******************************************************************************/
/* Scheduler use define */
#define USB_TBLCLR          0u                  /* Table clear */
#define USB_CNTCLR          0u                  /* Counter clear */
#define USB_FLGCLR          0u                  /* Flag clear */
#define USB_FLGSET          1u                  /* Flag set */
#define USB_IDCLR           0xFFu               /* Priority clear */

/* Task ID define */
#define USB_TID_0           0u                  /* Task ID 0 */
#define USB_TID_1           1u                  /* Task ID 1 */
#define USB_TID_2           2u                  /* Task ID 2 */
#define USB_TID_3           3u                  /* Task ID 3 */
#define USB_TID_4           4u                  /* Task ID 4 */
#define USB_TID_5           5u                  /* Task ID 5 */
#define USB_TID_6           6u                  /* Task ID 6 */
#define USB_TID_7           7u                  /* Task ID 7 */
#define USB_TID_8           8u                  /* Task ID 8 */
#define USB_TID_9           9u                  /* Task ID 9 */
#define USB_TID_10          10u                 /* Task ID 9 */

/* Task priority define */
#define USB_PRI_0           0u                  /* Priority 0 */
#define USB_PRI_1           1u                  /* Priority 1 */
#define USB_PRI_2           2u                  /* Priority 2 */
#define USB_PRI_3           3u                  /* Priority 3 */
#define USB_PRI_4           4u                  /* Priority 4 */
#define USB_PRI_5           5u                  /* Priority 5 */
#define USB_PRI_6           6u                  /* Priority 6 */

/* Please set with user system */
#define USB_IDMAX           11u                 /* Maximum Task ID +1 */
#define USB_PRIMAX          8u                  /* Maximum Priority number +1 */
#define USB_BLKMAX          20u                 /* Maximum block */
#define USB_TABLEMAX        USB_BLKMAX          /* Maximum priority table */


/* Peripheral Control Driver Task */
#define USB_PCD_TSK         USB_TID_0           /* Task ID */
#define USB_PCD_MBX         USB_PCD_TSK         /* Mailbox ID */
#define USB_PCD_MPL         USB_PCD_TSK         /* Memorypool ID */

/* Host Control Driver Task */
#define USB_HCD_TSK         USB_TID_1           /* Task ID */
#define USB_HCD_MBX         USB_HCD_TSK         /* Mailbox ID */
#define USB_HCD_MPL         USB_HCD_TSK         /* Memorypool ID */

/* Host Manager Task */
#define USB_MGR_TSK         USB_TID_2           /* Task ID */
#define USB_MGR_MBX         USB_MGR_TSK         /* Mailbox ID */
#define USB_MGR_MPL         USB_MGR_TSK         /* Memorypool ID */

/* Hub Task */
#define USB_HUB_TSK         USB_TID_3           /* Task ID */
#define USB_HUB_MBX         USB_HUB_TSK         /* Mailbox ID */
#define USB_HUB_MPL         USB_HUB_TSK         /* Memorypool ID */

/* Host Sample Task */
#define USB_HMSC_TSK        USB_TID_4           /* Task ID */
#define USB_HMSC_MBX        USB_HMSC_TSK        /* Mailbox ID */
#define USB_HMSC_MPL        USB_HMSC_TSK        /* Memorypool ID */

/* Host Sample Task */
#define USB_HSTRG_TSK       USB_TID_5           /* Task ID */
#define USB_HSTRG_MBX       USB_HSTRG_TSK       /* Mailbox ID */
#define USB_HSTRG_MPL       USB_HSTRG_TSK       /* Memorypool ID */

/* Host Sample Task */
#define USB_HMSCSMP_TSK     USB_TID_6           /* Task ID */
#define USB_HMSCSMP_MBX     USB_HMSCSMP_TSK     /* Mailbox ID */
#define USB_HMSCSMP_MPL     USB_HMSCSMP_TSK     /* Memorypool ID */

/* Idle Task */
#define USB_IDL_TSK         USB_TID_10          /* Task ID */
#define USB_IDL_MBX         USB_IDL_TSK         /* Mailbox ID */
#define USB_IDL_MPL         USB_IDL_TSK         /* Memorypool ID */

#endif  /* __R_USB_CKERNELID_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
