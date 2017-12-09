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
* File Name    : r_usb_pStdFunction.c
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB Peripheral standard function code
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
*         : 4.4.2012           Function header translation. Ch. tabs to 4 spaces.
*                              SET EDITOR TO INSERT SPACES WHEN TAB PRESSED.
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"         /* Type define */
#include "r_usb_usrconfig.h"        /* USB-H/W register set (user define) */
#include "r_usb_cdefusbip.h"        /* USB-FW Library Header */
#include "r_usb_cmacprint.h"        /* Standard IO macro */
#include "r_usb_cextern.h"          /* USB-FW global definition */
#include "r_usb_cusb_bitdefine.h"
#include "r_usb_reg_access.h"

/******************************************************************************
External variables and functions
******************************************************************************/
void    usb_cstd_SetStallPipe0( USB_UTR_t *ptr );


/******************************************************************************
Section    <Section Definition> , "Project Sections"
******************************************************************************/
#pragma section _pcd

/******************************************************************************
Renesas Abstracted Peripheral standard function functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_SetFeatureFunction
Description     : Process a SET_FEATURE request.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_SetFeatureFunction(USB_UTR_t *ptr)
{
    /* Request error */
    usb_cstd_SetStallPipe0(ptr);
}
/******************************************************************************
End of function usb_pstd_SetFeatureFunction
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_DtchFunction
Description     : Detach function. See usb_pstd_DetachProcess.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_DtchFunction(void)
{
}
/******************************************************************************
End of function usb_pstd_DtchFunction
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_IntHandFunction
Description     : Interrupt handler function.   $REA Not used? Remove?
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
                : uint16_t ists1
Return value    : none
******************************************************************************/
void usb_pstd_IntHandFunction(USB_UTR_t *ptr, uint16_t ists1)
{
}
/******************************************************************************
End of function usb_pstd_IntHandFunction
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_ChkVbsts
Description     : Return the VBUS status.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return          : uint16_t connection status(ATTACH/DETACH)
******************************************************************************/
uint16_t usb_pstd_ChkVbsts(USB_UTR_t *ptr)
{
    uint16_t    buf1, buf2, buf3;
    uint16_t    connect_info;

    /* VBUS chattering cut */
    do
    {
        buf1 = usb_creg_read_intsts( ptr );
        usb_cpu_Delay1us((uint16_t)10);
        buf2 = usb_creg_read_intsts( ptr );
        usb_cpu_Delay1us((uint16_t)10);
        buf3 = usb_creg_read_intsts( ptr );
    }
    while( ((buf1 & USB_VBSTS) != (buf2 & USB_VBSTS))
          || ((buf2 & USB_VBSTS) != (buf3 & USB_VBSTS)) );

    /* VBUS status judge */
    if( (buf1 & USB_VBSTS) != (uint16_t)0 )
    {
        /* Attach */
        connect_info = USB_ATTACH;
    }
    else
    {
        /* Detach */
        connect_info = USB_DETACH;
    }
    return connect_info;
}
/******************************************************************************
End of function usb_pstd_ChkVbsts
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_AttachFunction
Description     : Processing for attach detect.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_AttachFunction(USB_UTR_t *ptr)
{
    /* Delay about 10ms */
    usb_cpu_DelayXms((uint16_t)10);
}
/******************************************************************************
End of function usb_pstd_AttachFunction
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_DetachFunction
Description     : Processing for detach detect.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_DetachFunction(USB_UTR_t *ptr)
{
}
/******************************************************************************
End of function usb_pstd_DetachFunction
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_BusresetFunction
Description     : Processing for USB bus reset detection.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_BusresetFunction(USB_UTR_t *ptr)
{
}
/******************************************************************************
End of function usb_pstd_BusresetFunction
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_SuspendFunction
Description     : Processing for suspend signal detection.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_SuspendFunction(USB_UTR_t *ptr)
{
}
/******************************************************************************
End of function usb_pstd_SuspendFunction
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_InitFunction
Description     : Call function that checks VBUS status.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
uint16_t usb_pstd_InitFunction(USB_UTR_t *ptr)
{
    /* Wait USBC_VBSTS */
    return usb_pstd_ChkVbsts( ptr );
}
/******************************************************************************
End of function usb_pstd_InitFunction
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/
