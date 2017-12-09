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
* File Name    : r_usb_cstdfunction.c
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB Host and Peripheral common low level functions.
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
*         : 3.27.2012          Comments tranlation (REA). Changed tabs to 4 spa-
*                              ces. SET EDITOR TO INSERT SPACES WHEN TAB PRESSED.
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"         /* Type define */
#include "r_usb_usrconfig.h"        /* USB-H/W register set (user define) */
#include "r_usb_cdefusbip.h"        /* USB-FW Library Header */
#include "r_usb_cmacprint.h"        /* Standard IO macro */
#include "r_usb_cmacsystemcall.h"   /* System call macro */
#include "r_usb_cextern.h"          /* USB-FW global define */
#include "r_usb_reg_access.h"

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


/******************************************************************************
Private global variables and functions
******************************************************************************/


/******************************************************************************
Renesas Abstracted common standard function functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_set_usbip_mode_sub
Description     : USB init depending on mode (host peripharal). 
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
                : uint16_t function         : HOST/PERI
Return value    : none
******************************************************************************/
void usb_cstd_set_usbip_mode_sub(USB_UTR_t *ptr, uint16_t function)
{

#if USB_PORTSEL_PP == USB_2PORT_PP
    uint16_t   else_connect_inf;
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */

    if (function == (uint16_t)USB_PERI)
    {
#if USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP
        /* Interrupt Enable */
        usb_cstd_BerneEnable(ptr);
        usb_pstd_InitConnect(ptr);
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP */
    }
    else
    {
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
        usb_creg_set_dcfm( ptr );
        usb_creg_set_drpd( ptr, USB_PORT0 );
#if USB_PORTSEL_PP == USB_2PORT_PP
        usb_creg_set_drpd( ptr, USB_PORT1 );
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
        /* Interrupt Enable */
        usb_cstd_BerneEnable(ptr);
        /* Wait 10us */
        usb_cpu_Delay1us((uint16_t)10);
#if USB_PORTSEL_PP == USB_2PORT_PP
        else_connect_inf = usb_hstd_InitConnect(ptr, USB_PORT0, USB_DETACHED );
        usb_hstd_InitConnect(ptr, USB_PORT1, else_connect_inf);
#else   /* USB_PORTSEL_PP == USB_2PORT_PP */
        usb_hstd_InitConnect(ptr, USB_PORT0, USB_DETACHED );
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */
    }
}
/******************************************************************************
End of function usb_cstd_set_usbip_mode_sub
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/
