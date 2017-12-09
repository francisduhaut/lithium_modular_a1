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
* Copyright (C) 2010(2011,2012) Renesas Electronics Corpration
* and Renesas Solutions Corp. All rights reserved.
******************************************************************************
* File Name    : r_usb_usrconfig.h
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB User definition
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
******************************************************************************/

/* $Id: r_usb_usrconfig.h 143 2012-05-07 09:16:46Z tmura $ */

#ifndef __R_USB_USRCFG_H__
#define __R_USB_USRCFG_H__

#include    "r_usb_defvalue.h"
#include    "r_usb_fixed_config.h"

/*****************************************************************************
Macro definitions (USER DEFINE)
******************************************************************************/
/* Select using ANSI IO */
    #define USB_ANSIIO_PP           USB_ANSIIO_USE_PP
//  #define USB_ANSIIO_PP           USB_ANSIIO_NOT_USE_PP

/* Select USB mode(Host or Periphera) per each USB IP  */
    #define USB_FUNCSEL_USBIP0_PP           USB_HOST_PP     // Host Mode
//  #define USB_FUNCSEL_USBIP0_PP           USB_PERI_PP     // Peripheral Mode
//  #define USB_FUNCSEL_USBIP0_PP           USB_NOUSE_PP

//  #define USB_FUNCSEL_USBIP1_PP           USB_PERI_PP     // Peripheral Mode
    #define USB_FUNCSEL_USBIP1_PP           USB_NOUSE_PP

/* CPU byte endian select */
    #define USB_CPUBYTE_PP          USB_BYTE_LITTLE_PP
//  #define USB_CPUBYTE_PP          USB_BYTE_BIG_PP

/* Select CPU Low Power Mode                      */
    #define USB_CPU_LPW_PP          USB_LPWR_NOT_USE_PP
//  #define USB_CPU_LPW_PP          USB_LPWR_USE_PP


/*****************************************************************************
Macro definitions (Peripheral Mode)
******************************************************************************/
/* bcdUSB */
    #define USB_BCDNUM              0x0200u
/* Release Number */
    #define USB_RELEASE             0x0100u
/* DCP max packet size */
    #define USB_DCPMAXP             64u
/* Configuration number */
    #define USB_CONFIGNUM           1u
/* Max of string descriptor */
    #define USB_STRINGNUM           7u

/*****************************************************************************
Macro definitions (Host Mode)
******************************************************************************/

/* Max pipe error count */
    #define USB_PIPEERROR           1u

/* Descriptor size */
    #define USB_DEVICESIZE          20u     /* Device Descriptor size */
    #define USB_CONFIGSIZE          256u    /* Configuration Descriptor size */

/* HUB down port */
    #define USB_HUBDOWNPORT         4u      /* HUB downport (MAX15) */


#define USB_IFNUM                   1
#define USB_CFGNUM                  1
#define USB_MAX_DEVICENUM           (USB_DEVICENUM * USB_IFNUM * USB_CFGNUM)

/*******************************************************************************
Includes   <System Includes> , "Project Includes"
********************************************************************************/
#include "r_usb_sysdef.h"

#endif  /* __R_USB_CDEFUSR_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
