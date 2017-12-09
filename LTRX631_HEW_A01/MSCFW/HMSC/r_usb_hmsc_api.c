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
* Copyright (C) 2010 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************
* File Name    : r_usb_hmsc_api.c
* Version      : 1.00
* Device(s)    : Renesas
* Tool-Chain   : Renesas
* OS           : Common to None and uITRON
* H/W Platform : Independent
* Description  : USB Peripheral Sample Code
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 17.03.2010 0.80    First Release
*         : 30.07.2010 0.90    Updated comments
*         : 02.08.2010 0.91    Updated comments
*         : 29.10.2010 1.00    Mass Production Release
******************************************************************************/

/* $Id$ */

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_cTypedef.h"         /* Type define */
#include "r_usb_usrconfig.h"
#include "r_usb_cDefUSBIP.h"        /* USB-FW Library Header */
#include "r_usb_cMacPrint.h"        /* Standard IO macro */
#include "r_usb_cKernelId.h"        /* Kernel ID definition */
#include "r_usb_cMacSystemcall.h"   /* uITRON system call macro */
#include "r_usb_ansi.h"
#include "r_usb_cmsc_define.h"      /* USB  Mass Storage Class Header */
#include "r_usb_cusb_bitdefine.h"
#include "r_usb_catapi_define.h"        /* Peripheral ATAPI Device extern */
#include "r_usb_hmsc_define.h"      /* Host Mass Storage Class Driver */
#include "r_usb_hmsc_extern.h"      /* Host MSC grobal define */
#include "r_usb_cExtern.h"          /* USB-FW grobal define */

#include "r_usb_api.h"

/******************************************************************************
Section    <Section Difinition> , "Project Sections"
******************************************************************************/
#pragma section _hmsc

/******************************************************************************
Private global variables and functions
******************************************************************************/
void usb_hmsc_hub_registration(USB_UTR_t *ptr);

/******************************************************************************
External variables and functions
******************************************************************************/
extern  const   uint16_t    usb_ghhub_TPL[];
extern  uint16_t            usb_ghhub_DefEPTbl[];

extern  USB_UTR_t           tfat_ptr;

extern  void    usb_hapl_task_start( USB_UTR_t *ptr );
extern  void    usb_hapl_registration(USB_UTR_t *ptr);

/******************************************************************************
Renesas Host MSC Sample Code functions
******************************************************************************/

USB_REGADR_t    R_usb_cstd_GetUsbIpAdr( uint16_t ipno );

/******************************************************************************
Function Name   : usb_hmsc_hub_registration
Description     : Hub Data Registration
Argument        : none
Return          : none
******************************************************************************/
void usb_hmsc_hub_registration(USB_UTR_t *ptr)
{
    USB_HCDREG_t    driver;

        R_usb_cstd_SetTaskPri(USB_HUB_TSK,  USB_PRI_3);

    /* Driver registration */
    driver.ifclass      = (uint16_t)USB_IFCLS_HUB;          /* Interface Class : HUB */
    driver.tpl          = (uint16_t*)&usb_ghhub_TPL;        /* Target peripheral list */
    driver.pipetbl      = (uint16_t*)&usb_ghhub_DefEPTbl;   /* Pipe Define Table address */
    driver.classinit    = &usb_hhub_Initial;                /* Driver init */
    driver.classcheck   = &usb_hhub_ChkClass;               /* Driver check */
    driver.devconfig    = (USB_CB_INFO_t)&R_usb_hhub_Open;  /* Device configuered */
    driver.devdetach    = (USB_CB_INFO_t)&R_usb_hhub_Close; /* Device detach */
    driver.devsuspend   = &usb_hmsc_DummyFunction;          /* Device suspend */
    driver.devresume    = &usb_hmsc_DummyFunction;          /* Device resume */
    driver.overcurrent  = &usb_hmsc_DummyFunction;          /* Device over current */

    R_usb_hhub_Registration(ptr, &driver);
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_hmsc_driver_start
Description     : USB Host Initialize process
Argument        : none
Return          : none
******************************************************************************/
void R_usb_hmsc_driver_start( USB_UTR_t *ptr )
{
    R_usb_cstd_SetTaskPri(USB_HMSC_TSK, USB_PRI_3);
    R_usb_cstd_SetTaskPri(USB_HSTRG_TSK, USB_PRI_3);
}
/******************************************************************************
End of function R_usb_hvendor_driver_start()
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_hmsc_ClearStall
Description     : Clear Stall
Argument        : uint16_t type             : Data transmit/Data Receive
                : uint16_t msgnum           : Message Number
                : USB_CB_t complete         : Callback Function
Return value    : none
******************************************************************************/
void R_usb_hmsc_ClearStall(USB_UTR_t *ptr, uint16_t type, uint16_t msgnum, USB_CB_t complete)
{
    uint16_t        pipeno;

    switch( type ) 
    {
    case USB_DATA_NONE:                         /* Data transmit */
        pipeno = R_usb_hmsc_Information(usb_ghmsc_OutPipe[msgnum][0]);
        usb_ghmsc_OutPipe[msgnum][1] = 0;
        usb_hmsc_ClearStall(ptr, pipeno, complete);
        break;
    case USB_DATA_OK:                           /* Data recieve */
        pipeno = R_usb_hmsc_Information(usb_ghmsc_InPipe[msgnum][0]);
        usb_ghmsc_InPipe[msgnum][1] = 0;
        usb_hmsc_ClearStall(ptr, pipeno, complete);
        break;
    default:
        USB_PRINTF0("### stall error\n");
        break;
    }
}
/******************************************************************************
End of function
******************************************************************************/
