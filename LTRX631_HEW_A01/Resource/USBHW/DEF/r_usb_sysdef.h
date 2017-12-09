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
* File Name    : r_usb_sysdef.h
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB User Definition
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
******************************************************************************/
/* This definition shows USB IP number that work as USB host mode */
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP
    #define USB_HOST_USBIP_NUM  USB_USBIP_0
#elif USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
    #define USB_HOST_USBIP_NUM  USB_USBIP_1
#else
    #define USB_HOST_USBIP_NUM  USB_NOUSE
#endif

/* This definition shows USB IP number that work as USB peripheral mode */
#if USB_FUNCSEL_USBIP0_PP == USB_PERI_PP
    #define USB_PERI_USBIP_NUM  USB_USBIP_0
#elif USB_FUNCSEL_USBIP1_PP == USB_PERI_PP
    #define USB_PERI_USBIP_NUM  USB_USBIP_1
#else
    #define USB_PERI_USBIP_NUM  USB_NOUSE
#endif

/* This definition shows whether either USB IP0 or USB IP1 is set as USB host mode */
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
    #define USB_HOST_MODE_PP
#endif

/* This definition shows whether either USB IP0 or USB IP1 is set as USB peripheral mode */
#if USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP
    #define USB_PERI_MODE_PP
#endif

/******************************************************************************
End  Of File
******************************************************************************/
