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
* File Name    : r_usb_fixed_config.h
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB Fixed Configuration
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
******************************************************************************/

/* $Id: r_usb_fixed_config.h 143 2012-05-07 09:16:46Z tmura $ */

#ifndef __R_USB_FIXEDCFG_RX_H__
#define __R_USB_FIXEDCFG_RX_H__

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* !!!!! WARNING--You can not edit this file. !!!!!*/
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

/*****************************************************************************
Macro definitions (597IP)
******************************************************************************/
    /* The number of USBIP */
    #define USB_NUM_USBIP           2

    /* Clock mode */
    #define USB_USBIP_LPW_PP        USB_CLK_NOT_STOP_PP

    /* Sleep mode */
    #define USB_LPWRSEL_PP          USB_LPSM_DISABLE_PP

    /* SPEED mode */
    #define USB_HSESEL              USB_HS_DISABLE

    /* Select target chip define */
    #define USB_TARGET_CHIP_PP      USB_RX600_PP

    /* Select IP mode PIPE MAX */
    #define USB_IP_PIPE_PP          USB_IP_PIPE_9_PP

    /* Select IP mode DEVADD MAX */
    #define USB_IP_DEVADD_PP        USB_IP_DEVADD_5_PP

    /* Select PIPEBUF fix or variable */
    #define USB_PIPEBUF_MODE_PP     USB_PIPEBUF_FIX_PP

    /* Select Transfer Speed */
    #define USB_SPEEDSEL_PP         USB_FS_PP

    /* Default MBW */
    #define USB_BUSSIZE_PP          USB_BUSSIZE_16_PP

    /* Select PORT */
    #define USB_PORTSEL_PP          USB_1PORT_PP    /* 1port in 1IP */

    /* Start Pipe No */
    #define USB_MIN_PIPE_NO         USB_PIPE1

    /* Data Trans mode */
    #define USB_TRANS_MODE_PP       USB_TRANS_DTC_PP

    /* USB Device address define */
    #define USB_DEVICEADDR          1u      /* PORT0 USB Address (1 to 10) */

    /* HUB Address */
    #define USB_HUBDPADDR           (uint16_t)(USB_DEVICEADDR + 1u)

#define USB_MAX_FILENUMBER  16
#define USB_DEVICENUM       10

#endif
/******************************************************************************
End  Of File
******************************************************************************/

