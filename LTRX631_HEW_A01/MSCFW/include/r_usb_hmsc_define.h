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
******************************************************************************
* Copyright (C) 2010 Renesas Electronics Corporation. All rights reserved.
******************************************************************************
* File Name    : r_usb_hmsc_define.h
* Version      : 1.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB User definition Pre-pro
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 17.03.2010 0.80    First Release
*         : 30.07.2010 0.90    Updated comments
*         : 02.08.2010 0.91    Updated comments
*         : 29.10.2010 1.00    Mass Production Release
******************************************************************************/
#ifndef __R_USB_HMSCDEFINE_H__
#define __R_USB_HMSCDEFINE_H__


/*****************************************************************************
Macro definitions
******************************************************************************/
#define USB_HMSC_DEV_DET    (uint16_t)0x00      /* detached device */
#define USB_HMSC_DEV_ATT    (uint16_t)0x01      /* attached device */
#define USB_HMSC_DEV_ENU    (uint16_t)0x02      /* Device enumeration */


#define USB_MAXDRIVE        (uint16_t)16        /* Max drive */
#define USB_MAXSTRAGE       (uint16_t)8         /* Max device */

#define USB_DRIVE           0
#define USB_MEDIADRIVE      USB_DRIVE


/******************************************************************************
Enum define
******************************************************************************/
/* ERROR CODE */
enum usb_ghmsc_Error
{
    USB_HMSC_OK         = (uint16_t)0,
    USB_HMSC_STALL          = (uint16_t)-1,
    USB_HMSC_CBW_ERR        = (uint16_t)-2,     /* CBR error */
    USB_HMSC_DAT_RD_ERR = (uint16_t)-3,     /* Data IN error */
    USB_HMSC_DAT_WR_ERR = (uint16_t)-4,     /* Data OUT error */
    USB_HMSC_CSW_ERR        = (uint16_t)-5,     /* CSW error */
    USB_HMSC_CSW_PHASE_ERR  = (uint16_t)-6,     /* Phase error */
    USB_HMSC_SUBMIT_ERR = (uint16_t)-7,     /* submit error */
};


/*****************************************************************************
Macro definitions
******************************************************************************/
#define USB_MSG_HMSC_NO_DATA                (uint16_t)0x501
#define USB_MSG_HMSC_DATA_IN                (uint16_t)0x502
#define USB_MSG_HMSC_DATA_OUT               (uint16_t)0x503
#define USB_MSG_HMSC_CSW_ERR                (uint16_t)0x504
#define USB_MSG_HMSC_CSW_PHASE_ERR          (uint16_t)0x505
#define USB_MSG_HMSC_CBW_ERR                (uint16_t)0x506
#define USB_MSG_HMSC_DATA_STALL             (uint16_t)0x507

#define USB_MSG_HMSC_STRG_DRIVE_SEARCH      (uint16_t)0x601
#define USB_MSG_HMSC_DEV_READ_SECTOR_SIZE   (uint16_t)0x602
#define USB_MSG_HMSC_DEV_READ_PARTITION     (uint16_t)0x603
#define USB_MSG_HMSC_STRG_DRIVE_OPEN        (uint16_t)0x604

#define USB_HMSC_DRIVEMOUNT                 (uint16_t)0x1000
#define USB_HMSC_FILEREAD                   (uint16_t)0x1001
#define USB_HMSC_FILEWRITE                  (uint16_t)0x1002
#define USB_HMSC_DRIVE_OPEN                 (uint16_t)0x1003
#define USB_HMSC_DRIVE_OPEN2                (uint16_t)0x1004
#define USB_HMSC_WAIT                       (uint16_t)0x1005
#define	USB_HMSC_FILEWRITEOK				(uint16_t)0x1006

#define USB_WAIT1               			(uint16_t)0x2001
#define USB_START              				(uint16_t)0x2003
#define USB_APL_CLR                 		(uint16_t)0x2002
#define	USB_WRITE_MEMO						(uint16_t)0x200A
#define	USB_WRITE_CFG						(uint16_t)0x200B
#define	USB_READ_CFG						(uint16_t)0x200C

#endif  /* __R_USB_HMSCDEFINE_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
