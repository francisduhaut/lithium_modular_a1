/******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized.
* This software is owned by Renesas Electronics Corporation and is  protected
* under all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING 
* BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
* PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. 
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE  LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this
* software and to discontinue the availability of this software.
*    By using this software, you agree to the additional terms and conditions 
* found by accessing the following link: 
* http://www.renesas.com/disclaimer
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************
* File Name    : r_usb_otherclass.c
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : Other class code - when using ANSI IO.
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
*         : 4.12.2012          Function header translation. Ch. tabs to 4 spaces.
*                              SET EDITOR TO INSERT SPACES WHEN TAB PRESSED.
******************************************************************************/

/* $Id: r_usb_otherclass.c 162 2012-05-21 10:20:32Z ssaek $ */

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"     /* Type define */
#include "r_usb_cdefusbip.h"        /* USB-FW Library Header */
#include "r_usb_usrconfig.h"
#include "r_usb_class_fusrcfg.h"

#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP

#if USB_DEVICE_CLASS_PP != USB_DEVICE_CLASS_PERI_CDC
int16_t usb_open_pcdc( USB_UTR_t *p_utr, struct usb_fninfo *p_fnstr, uint8_t devadr, uint8_t speed )
{
    return 0;
}
#endif  /* USB_DEVICE_CLASS_PP != USB_DEVICE_CLASS_PERI_CDC */

#if USB_DEVICE_CLASS_PP != USB_DEVICE_CLASS_HOST_CDC
int16_t usb_open_hcdc( USB_UTR_t *p_utr, struct usb_fninfo *p_fnstr, uint8_t devadr, uint8_t speed )
{
    return 0;
}
#endif  /* USB_DEVICE_CLASS_PP != USB_DEVICE_CLASS_HOST_CDC */

#if USB_DEVICE_CLASS_PP != USB_DEVICE_CLASS_HOST_HID
int16_t usb_open_hhid( USB_UTR_t *p_utr, struct usb_fninfo *p_fnstr, uint8_t devadr, uint8_t speed )
{
    return 0;
}
#endif  /* USB_DEVICE_CLASS_PP != USB_DEVICE_CLASS_HOST_HID */

#if USB_DEVICE_CLASS_PP != USB_DEVICE_CLASS_PERI_HID
int16_t usb_open_phid( USB_UTR_t *p_utr, struct usb_fninfo *p_fnstr, uint8_t devadr, uint8_t speed )
{
    return 0;
}
#endif  /* USB_DEVICE_CLASS_PP != USB_DEVICE_CLASS_PERI_HID */

#if USB_DEVICE_CLASS_PP != USB_DEVICE_CLASS_HOST_MSC
int16_t usb_open_hmsc( USB_UTR_t *p_utr, struct usb_fninfo *p_fnstr, uint8_t devadr, uint8_t speed )
{
    return 0;
}
#endif  /* USB_DEVICE_CLASS_PP != USB_DEVICE_CLASS_HOST_MSC */

#if USB_DEVICE_CLASS_PP != USB_DEVICE_CLASS_PERI_MSC
int16_t usb_open_pmsc( USB_UTR_t *p_utr, struct usb_fninfo *p_fnstr, uint8_t devadr, uint8_t speed )
{
    return 0;
}
#endif  /* USB_DEVICE_CLASS_PP != USB_DEVICE_CLASS_PERI_MSC */

#if USB_DEVICE_CLASS_PP == USB_DEVICE_CLASS_VENDOR
#ifndef USB_HOST_MODE_PP
int16_t usb_open_hvendor( USB_UTR_t *p_utr, struct usb_fninfo *p_fnstr, uint8_t devadr, uint8_t speed )
{
    return 0;
}
#endif  /* USB_HOST_MODE_PP */

#ifndef USB_PERI_MODE_PP
int16_t usb_open_pvendor( USB_UTR_t *p_utr, struct usb_fninfo *p_fnstr, uint8_t devadr, uint8_t speed )
{
    return 0;
}
#endif  /* USB_PERI_MODE_PP */
#else   /* USB_DEVICE_CLASS_PP == USB_DEVICE_CLASS_VENDOR */
int16_t usb_open_hvendor( USB_UTR_t *p_utr, struct usb_fninfo *p_fnstr, uint8_t devadr, uint8_t speed )
{
    return 0;
}

int16_t usb_open_pvendor( USB_UTR_t *p_utr, struct usb_fninfo *p_fnstr, uint8_t devadr, uint8_t speed )
{
    return 0;
}
#endif  /* USB_DEVICE_CLASS_PP == USB_DEVICE_CLASS_VENDOR */

#ifndef USB_CLASS_REQUEST_USE_PP
USB_ER_t usb_control_class_request(void *pram)
{
    return USB_OK;
}
#endif  /* USB_CLASS_REQUEST_USE_PP */

/******************************************************************************
End of function
******************************************************************************/
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */

