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
* File Name    : r_usb_pClassVendor.c
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB Peripheral Vendor Class code
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
******************************************************************************/

/* $Id: r_usb_pclassvendor.c 143 2012-05-07 09:16:46Z tmura $ */


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"     /* Type define */
#include "r_usb_usrconfig.h"
#include "r_usb_cdefusbip.h"        /* USB-FW Library Header */
#include "r_usb_cextern.h"          /* USB-FW global define */
#include "r_usb_cmacprint.h"        /* Standard IO macro */
#include "r_usb_cusb_bitdefine.h"

/******************************************************************************
Section    <Section Definition> , "Project Sections"
******************************************************************************/
#pragma section _pcd


/******************************************************************************
Constant macro definitions
******************************************************************************/


/******************************************************************************
External variables and functions
******************************************************************************/


/******************************************************************************
Private global variables and functions
******************************************************************************/
void    usb_pstd_ClassReq0(USB_UTR_t *ptr, USB_REQUEST_t *);
void    usb_pstd_ClassReq1(USB_UTR_t *ptr, USB_REQUEST_t *);
void    usb_pstd_ClassReq2(USB_UTR_t *ptr, USB_REQUEST_t *);
void    usb_pstd_ClassReq3(USB_UTR_t *ptr, USB_REQUEST_t *);
void    usb_pstd_ClassReq4(USB_UTR_t *ptr, USB_REQUEST_t *);
void    usb_pstd_ClassReq5(USB_UTR_t *ptr, USB_REQUEST_t *);
void    usb_pstd_VendorReq0(USB_UTR_t *ptr, USB_REQUEST_t *);
void    usb_pstd_VendorReq1(USB_UTR_t *ptr, USB_REQUEST_t *);
void    usb_pstd_VendorReq2(USB_UTR_t *ptr, USB_REQUEST_t *);
void    usb_pstd_VendorReq3(USB_UTR_t *ptr, USB_REQUEST_t *);
void    usb_pstd_VendorReq4(USB_UTR_t *ptr, USB_REQUEST_t *);
void    usb_pstd_VendorReq5(USB_UTR_t *ptr, USB_REQUEST_t *);


/******************************************************************************
Renesas Abstracted Peripheral Vendor Class Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_UsrCtrlTransFunction
Description     : Class request (idle or setup stage)
Arguments       : USB_REQUEST_t *ptr        : request
                : uint16_t stginfo           : stage info
Return value    : none
******************************************************************************/
void usb_pstd_UsrCtrlTransFunction(USB_UTR_t *ptr, USB_REQUEST_t *req, uint16_t stginfo )
{
    if( req->ReqTypeType == USB_CLASS )
    {
        switch( stginfo )
        {
        /* Idle or setup stage */
        case USB_CS_IDST:   usb_pstd_ClassReq0(ptr, req);   break;
        /* Control read data stage */
        case USB_CS_RDDS:   usb_pstd_ClassReq1(ptr, req);   break;
        /* Control write data stage */
        case USB_CS_WRDS:   usb_pstd_ClassReq2(ptr, req);   break;
        /* Control write nodata status stage */
        case USB_CS_WRND:   usb_pstd_ClassReq3(ptr, req);   break;
        /* Control read status stage */
        case USB_CS_RDSS:   usb_pstd_ClassReq4(ptr, req);   break;
        /* Control write status stage */
        case USB_CS_WRSS:   usb_pstd_ClassReq5(ptr, req);   break;

        /* Control sequence error */
        case USB_CS_SQER:
            R_usb_pstd_ControlEnd(ptr, (uint16_t)USB_DATA_ERR);
            break;
        /* Illegal */
        default:
            R_usb_pstd_ControlEnd(ptr, (uint16_t)USB_DATA_ERR);
            break;
        }
    }
    else if( req->ReqTypeType == USB_VENDOR )
    {
        switch( stginfo )
        {
        /* Idle or setup stage */
        case USB_CS_IDST:   usb_pstd_VendorReq0(ptr, req);  break;
        /* Control read data stage */
        case USB_CS_RDDS:   usb_pstd_VendorReq1(ptr, req);  break;
        /* Control write data stage */
        case USB_CS_WRDS:   usb_pstd_VendorReq2(ptr, req);  break;
        /* Control write nodata status stage */
        case USB_CS_WRND:   usb_pstd_VendorReq3(ptr, req);  break;
        /* Control read status stage */
        case USB_CS_RDSS:   usb_pstd_VendorReq4(ptr, req);  break;
        /* Control write status stage */
        case USB_CS_WRSS:   usb_pstd_VendorReq5(ptr, req);  break;

        /* Control sequence error */
        case USB_CS_SQER:
            R_usb_pstd_ControlEnd(ptr, (uint16_t)USB_DATA_ERR);
            break;
        /* Illegal */
        default:
            R_usb_pstd_ControlEnd(ptr, (uint16_t)USB_DATA_ERR);
            break;
        }
    }
    else
    {
        USB_PRINTF2("usb_pstd_UsrCtrlTransFunction Err: %x %x\n", req->ReqTypeType, stginfo );
        R_usb_pstd_PcdChangeDeviceState(ptr, USB_DO_STALL, USB_PIPE0, (USB_CB_INFO_t)usb_cstd_DummyFunction );
    }
}
/******************************************************************************
End of function usb_pstd_UsrCtrlTransFunction
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_ClassReq0
Description     : Class request (idle or setup stage)
Arguments       : USB_REQUEST_t *req        : request
Return value    : none
******************************************************************************/
void usb_pstd_ClassReq0(USB_UTR_t *ptr, USB_REQUEST_t *req)
{
}
/******************************************************************************
End of function usb_pstd_ClassReq0
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_ClassReq1
Description     : Class request (control read data stage)
Arguments       : USB_REQUEST_t *req        : request
Return value    : none
******************************************************************************/
void usb_pstd_ClassReq1(USB_UTR_t *ptr, USB_REQUEST_t *req)
{
}
/******************************************************************************
End of function usb_pstd_ClassReq1
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_ClassReq2
Description     : Class Request (control write data stage)
Arguments       : USB_REQUEST_t *req        : request
Return value    : none
******************************************************************************/
void usb_pstd_ClassReq2(USB_UTR_t *ptr, USB_REQUEST_t *req)
{
}
/******************************************************************************
End of function usb_pstd_ClassReq2
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_ClassReq3
Description     : Class request (control write nodata status stage)
Arguments       : USB_REQUEST_t *req        : request
Return value    : none
******************************************************************************/
void usb_pstd_ClassReq3(USB_UTR_t *ptr, USB_REQUEST_t *req)
{
}
/******************************************************************************
End of function usb_pstd_ClassReq3
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_ClassReq4
Description     : Class request (control read status stage)
Arguments       : USB_REQUEST_t *req        : request
Return value    : none
******************************************************************************/
void usb_pstd_ClassReq4(USB_UTR_t *ptr, USB_REQUEST_t *req)
{
}
/******************************************************************************
End of function usb_pstd_ClassReq4
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_ClassReq5
Description     : Class request (control write status stage)
Arguments       : USB_REQUEST_t *req        : request
Return value    : none
******************************************************************************/
void usb_pstd_ClassReq5(USB_UTR_t *ptr, USB_REQUEST_t *req)
{
}
/******************************************************************************
End of function usb_pstd_ClassReq5
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_VendorReq0
Description     : Vendor request (idle or setup stage)
Arguments       : USB_REQUEST_t *req        : request
Return value    : none
******************************************************************************/
void usb_pstd_VendorReq0(USB_UTR_t *ptr, USB_REQUEST_t *req)
{
    R_usb_pstd_PcdChangeDeviceState(ptr, USB_DO_STALL, USB_PIPE0, (USB_CB_INFO_t)usb_cstd_DummyFunction );
}
/******************************************************************************
End of function usb_pstd_VendorReq0
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_VendorReq1
Description     : Vendor request (control read data stage)
Arguments       : USB_REQUEST_t *req        : request
Return value    : none
******************************************************************************/
void usb_pstd_VendorReq1(USB_UTR_t *ptr, USB_REQUEST_t *req)
{
    R_usb_pstd_PcdChangeDeviceState(ptr, USB_DO_STALL, USB_PIPE0, (USB_CB_INFO_t)usb_cstd_DummyFunction );
}
/******************************************************************************
End of function usb_pstd_VendorReq1
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_VendorReq2
Description     : Vendor request (control write data stage)
Arguments       : USB_REQUEST_t *req        : request
Return value    : none
******************************************************************************/
void usb_pstd_VendorReq2(USB_UTR_t *ptr, USB_REQUEST_t *req)
{
    R_usb_pstd_PcdChangeDeviceState(ptr, USB_DO_STALL, USB_PIPE0, (USB_CB_INFO_t)usb_cstd_DummyFunction );
}
/******************************************************************************
End of function usb_pstd_VendorReq2
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_VendorReq3
Description     : Vendor request (control write nodata status stage)
Arguments       : USB_REQUEST_t *req        : request
Return value    : none
******************************************************************************/
void usb_pstd_VendorReq3(USB_UTR_t *ptr, USB_REQUEST_t *req)
{
    R_usb_pstd_PcdChangeDeviceState(ptr, USB_DO_STALL, USB_PIPE0, (USB_CB_INFO_t)usb_cstd_DummyFunction );
}
/******************************************************************************
End of function usb_pstd_VendorReq3
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_VendorReq4
Description     : Vendor request (control read status stage)
Arguments       : USB_REQUEST_t *req        : request
Return value    : none
******************************************************************************/
void usb_pstd_VendorReq4(USB_UTR_t *ptr, USB_REQUEST_t *req)
{
    R_usb_pstd_PcdChangeDeviceState(ptr, USB_DO_STALL, USB_PIPE0, (USB_CB_INFO_t)usb_cstd_DummyFunction );
}
/******************************************************************************
End of function usb_pstd_VendorReq4
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_VendorReq5
Description     : Vendor request (control write status stage)
Arguments       : USB_REQUEST_t *req        : request
Return value    : none
******************************************************************************/
void usb_pstd_VendorReq5(USB_UTR_t *ptr, USB_REQUEST_t *req)
{
    R_usb_pstd_PcdChangeDeviceState(ptr, USB_DO_STALL, USB_PIPE0, (USB_CB_INFO_t)usb_cstd_DummyFunction );
}
/******************************************************************************
End of function usb_pstd_VendorReq5
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/
