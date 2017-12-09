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
* File Name    : r_usb_smp_csub.c
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB Host and Peripheral common sample code
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
******************************************************************************/


/* $Id: r_usb_smp_csub.c 162 2012-05-21 10:20:32Z ssaek $ */


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"     /* Type define */
#include "r_usb_usrconfig.h"
#include "r_usb_ckernelid.h"        /* Kernel ID definition */
#include "r_usb_cdefusbip.h"        /* USB-FW Library Header */
#include "r_usb_cmacprint.h"        /* Standard IO macro */
#include "r_usb_cmacsystemcall.h"   /* uITRON system call macro */
#include "r_usb_cextern.h"          /* USB-FW global define */


/******************************************************************************
Section    <Section Definition> , "Project Sections"
******************************************************************************/
#pragma section _usblib


/******************************************************************************
Constant macro definitions
******************************************************************************/


/******************************************************************************
External variables and functions
******************************************************************************/
void            usb_cstd_ClassTransResult(USB_UTR_t *mess, uint16_t data1, uint16_t data2);
uint16_t        usb_cstd_ClassTransWaitTmo(uint16_t tmo);
void            usb_cstd_ClassProcessResult(USB_UTR_t *ptr, uint16_t data,uint16_t Dummy);
uint16_t        usb_cstd_ClassProcessWaitTmo(uint16_t tmo);
void            usb_cstd_DummyFunction(USB_UTR_t *ptr, uint16_t data1, uint16_t data2);


/******************************************************************************
Private global variables and functions
******************************************************************************/


/******************************************************************************
Renesas Abstracted common Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_ClassTransResult
Description     : send message
Arguments       : USB_UTR_t *mess    : USB system internal message.
Return value    : none
******************************************************************************/
void usb_cstd_ClassTransResult(USB_UTR_t *mess, uint16_t data1, uint16_t data2)
{
}
/******************************************************************************
End of function usb_cstd_ClassTransResult
******************************************************************************/


/******************************************************************************
Function Name   : usb_cstd_ClassTransWaitTmo
Description     : receive message
Arguments       : uint16_t tmo              : time out
Return value    : uint16_t                  : status
******************************************************************************/
uint16_t usb_cstd_ClassTransWaitTmo(uint16_t tmo)
{
    return USB_E_OK;
}
/******************************************************************************
End of function usb_cstd_ClassTransWaitTmo
******************************************************************************/


/******************************************************************************
Function Name   : usb_cstd_ClassProcessResult
Description     : callback function
Arguments       : uint16_t data             : status
                : uint16_t dummy            : not use
Return value    : none
******************************************************************************/
void usb_cstd_ClassProcessResult(USB_UTR_t *ptr, uint16_t data, uint16_t dummy)
{
}
/******************************************************************************
End of function usb_cstd_ClassProcessResult
******************************************************************************/


/******************************************************************************
Function Name   : usb_cstd_ClassProcessWaitTmo
Description     : receive message
Arguments       : uint16_t tmo              : time out
Return value    : uint16_t                  : status
******************************************************************************/
uint16_t usb_cstd_ClassProcessWaitTmo(uint16_t tmo)
{
    return USB_E_OK;
}
/******************************************************************************
End of function usb_cstd_ClassProcessWaitTmo
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/
