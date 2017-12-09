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
* File Name    : r_usb_ansi_peri.c
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : ANSI IO for USB Peripheral
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
*         : 4.12.2012          Function header translation. Ch. tabs to 4 spaces.
*                              SET EDITOR TO INSERT SPACES WHEN TAB PRESSED.
******************************************************************************/

/* $Id: r_usb_ansi_peri.c 143 2012-05-07 09:16:46Z tmura $ */

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"     /* Type define */
#include "r_usb_cdefusbip.h"        /* USB-FW Library Header */
#include "r_usb_ansi.h"

/******************************************************************************
Section    <Section Definition> , "Project Sections"
******************************************************************************/
#pragma section _ansi

#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP

extern  int16_t (*usb_classopen[])(USB_UTR_t *, struct usb_fninfo *, uint8_t, uint8_t);

/******************************************************************************
Function Name   : usb_open_peri
Description     : Call the open function of the specified class, and write to the 
                : usn_fnInfo structure accordingly.
Argument        : USB_CLASS_ENUM_t class   : Class code.
                : int16_t          fn      : File number.
Return          : uint16_t                 : USB_OK (success) / USB_NG (failure).
******************************************************************************/
uint16_t    usb_open_peri( USB_CLASS_ENUM_t class, int16_t fn )
{
    uint16_t    idx, isopened;
    struct      usb_fninfo fnstr;
    USB_UTR_t   utr;

    /* Class Index set */
    idx = (uint8_t)( class - USB_CLASS_PERI_MODE);
    utr.keyword = class;

    /* Calling open function of the class specified by 1st argument(class) */
    isopened = (*usb_classopen[idx])( (USB_UTR_t *)&utr, &fnstr, 0, 0 );

    /* Checking the completion of open() function */
    if( USB_OK == isopened )
    {
        /* Setting the information is gotten by open processing to usb_fnInfo(structure) variable */
        usb_fnInfo[fn]  = fnstr;

        return USB_OK;
    }

    return USB_NG;
} /* eof usb_open_peri */

/******************************************************************************
Function Name   : usb_close_peri
Description     : Clear the usb_fninfo structure.
Argument        : int16_t    fn   : File number.
Return          : uint16_t        : USB_OK (success) / USB_NG (failure).
******************************************************************************/
uint16_t    usb_close_peri( int16_t fn )
{
    /* Clear usb_fnInfo[fn](structure) variable */
    usb_fnInfo[fn].inPipeNo = USB_NOPIPE;
    usb_fnInfo[fn].outPipeNo = USB_NOPIPE;

//  usb_shstd_classErrorInfo = ;
    return USB_OK;
} /* eof usb_close_peri */
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */

/******************************************************************************
End  Of File
******************************************************************************/
