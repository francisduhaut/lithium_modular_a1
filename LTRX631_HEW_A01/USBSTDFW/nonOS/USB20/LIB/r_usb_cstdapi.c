/******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized.
*    This software is owned by Renesas Electronics Corporation and is protected
* under all applicable laws, including copyright laws.
*    THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING 
* BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
* PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. 
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE  LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*    Renesas reserves the right, without notice, to make changes to this
* software and to discontinue the availability of this software.
*    By using this software, you agree to the additional terms and conditions 
* found by accessing the following link: 
* http://www.renesas.com/disclaimer
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************
* File Name    : r_usb_cstdapi.c
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : A USB Host and Peripheral common low level API.
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
*         : 3.27.2012          Comments tranlation (REA). Changed tabs to 4 spa-
*                              ces. SET EDITOR TO INSERT SPACES WHEN TAB PRESSED.
******************************************************************************/

/* $Id: r_usb_cstdapi.c 162 2012-05-21 10:20:32Z ssaek $ */

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"         /* Type define */
#include "r_usb_usrconfig.h"        /* USB-H/W register set (user define) */
#include "r_usb_cdefusbip.h"        /* USB-FW Library Header */
#include "r_usb_cmacprint.h"        /* Standard IO macro */
#include "r_usb_cmacsystemcall.h"   /* System call macro */
#include "r_usb_cextern.h"          /* USB-FW global define */
#include "r_usb_reg_access.h"       /* USB register access function */
#include "r_usb_cusb_bitdefine.h"

#include "r_usb_api.h"

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
Renesas Abstracted common Signal functions
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_cstd_GetUsbIpAdr  
Description     : Return USB register base address of specified USB IP.
Arguments       : uint16_t  ipno        : USB IP No. that requires the base 
                                          address value 
Return value    : USB_REGADR_t          : Address value
******************************************************************************/
USB_REGADR_t    R_usb_cstd_GetUsbIpAdr( uint16_t ipno )
{
    return (USB_REGADR_t)usb_cstd_GetUsbIpAdr( ipno );
}/* eof R_usb_cstd_GetUsbIpAdr */
 
/******************************************************************************
Function Name   : R_usb_cstd_UsbIpInit
Description     : Initialize the USB IP.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t  usb_mode    : USB mode (Host/Peripheral).
Return value    : none
******************************************************************************/
void R_usb_cstd_UsbIpInit( USB_UTR_t *ptr, uint16_t usb_mode )
{
    usb_cstd_WaitUsbip( ptr );      /* Wait USB-H/W access enable */
    usb_cstd_AsspConfig( ptr );     /* Set ASSP pin_config */
    usb_cstd_InitialClock( ptr );   /* Start clock */
	R_usb_cstd_ClearHwFunction( ptr );	/* nitinalize USB register (Host/Peripheral common) */
    usb_cstd_Pinconfig( ptr );      /* Set pin_config */

    usb_cstd_set_usbip_mode( ptr, usb_mode );
}/* eof R_usb_cstd_UsbIpInit */
 
/******************************************************************************
Function Name   : R_usb_cstd_ClearHwFunction
Description     : Initinalize USB register (Host/Peripheral common)
Arguments       : USB_UTR_t *ptr : USB system internal structure.
Return value    : none
******************************************************************************/
void R_usb_cstd_ClearHwFunction(USB_UTR_t *ptr)
{
    usb_cstd_SelfClock(ptr);

    usb_cstd_SetNak(ptr, USB_PIPE0);

    usb_cstd_ClearIntEnb( ptr );

    /* Interrupt Disable(BRDY,NRDY,USB_BEMP) */
    usb_creg_write_brdyenb( ptr, 0 );
    usb_creg_write_nrdyenb( ptr, 0 );
    usb_creg_write_bempenb( ptr, 0 );

    /* Interrupt status clear */
    usb_cstd_ClearIntSts( ptr );

    /* Interrupt status clear(USB_BRDY,NRDY,USB_BEMP) */
    usb_creg_write_brdysts( ptr, 0 );
    usb_creg_write_nrdysts( ptr, 0 );
    usb_creg_write_bempsts( ptr, 0 );

    /* D+/D- control line set */
    usb_cstd_ClearDline( ptr );

    usb_creg_clr_hse( ptr, USB_PORT0 );
    usb_creg_clr_hse( ptr, USB_PORT1 );

    /* Function controller select */
    usb_creg_clr_dcfm( ptr ); 
    usb_cstd_SwReset(ptr);

}/* eof R_usb_cstd_ClearHwFunction */

/******************************************************************************
Function Name   : R_usb_cstd_SetRegDvstctr0
Description     : Setting the value(2nd argument) to DVSTCTR0 register
Arguments       : USB_UTR_t *ptr       ; USB internal structure. Selects USB channel.
                : uint16_t   val        : setting value
Return value    : none
******************************************************************************/
void    R_usb_cstd_SetRegDvstctr0( USB_UTR_t *ptr, uint16_t val )
{

    usb_creg_write_dvstctr( ptr, USB_PORT0, val );

}
/******************************************************************************
End of function R_usb_cstd_SetRegDvstctr0
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_cstd_SetRegPipeCtr
Description     : Setting the value(3rd argument) to PIPExCTR register
Arguments       : USB_UTR_t *ptr      ; USB internal structure. Selects USB channel.
                : uint16_t   pipeno    ; Pipe No. 
                : uint16_t   val       ; setting value
Return value    : none
******************************************************************************/
void    R_usb_cstd_SetRegPipeCtr( USB_UTR_t *ptr, uint16_t pipeno, uint16_t val )
{

    usb_creg_write_pipectr( ptr, pipeno, val );

}
/******************************************************************************
End of function R_usb_cstd_SetRegPipeCtr
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/
