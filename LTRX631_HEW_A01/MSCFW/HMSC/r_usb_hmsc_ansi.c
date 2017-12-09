/******************************************************************************
* DISCLAIMER
*    This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized.
*    This software is owned by Renesas Electronics Corporation and is protected
* under all applicable laws, including copyright laws.
*    THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING 
* BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
* PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. 
*    TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
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
* File Name    : r_usb_hmsc_ansi.c
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB Host Control Manager
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
*         : 05.17.2012 2.00    Comments tranlation (REA). Changed tabs to 4 spa-
*                              ces. SET EDITOR TO INSERT SPACES WHEN TAB PRESSED.
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_cmsc_define.h"      /* USB Mass Storage Class header */
#include "r_usb_cDefUSBIP.h"        /* USB-FW Library Header */
#include "r_usb_cMacSystemcall.h"   /* System call macro */
#include "r_usb_cMacPrint.h"        /* LCD/SIO disp macro */
#include "r_usb_cExtern.h"          /* USB-FW grobal define */
#include "r_usb_cKernelId.h"        /* Kernel ID definition */
#include "r_usb_usrconfig.h"        /* USB-H/W register set (user define) */
#include "r_usb_catapi_define.h"    /* Peripheral ATAPI Device extern */

#include "r_usb_hmsc_define.h"      /* Host Mass Storage Class Driver */
#include "r_usb_hmsc_extern.h"      /* Host MSC grobal define */
#include "r_usb_ansi.h"
#include "r_usb_api.h"

/******************************************************************************
Section    <Section Definition> , "Project Sections"
******************************************************************************/
#pragma section _hmsc

#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
/******************************************************************************
External variables and functions
******************************************************************************/
extern uint16_t  usb_ghstd_ConfigurationDescriptor[];
extern uint16_t  usb_ghstd_DeviceDescriptor[];
extern uint8_t   *usb_ghmsc_DeviceTable;

uint16_t usb_cstd_AnsiProcessWaitTmo( uint16_t );
void     usb_cstd_AnsiCallBack( USB_UTR_t *ptr, uint16_t, uint16_t );
int      usb_hmsc_SmplEnumeration(USB_UTR_t *ptr, struct usb_fninfo *p_fnstr, uint8_t devadr, uint8_t speed);

/* ANSI-C IO enumeration initialization sequence. */
int16_t  usb_ghmsc_SmplClassSeq;
uint8_t  *usb_hmsc_SmplDeviceTable;
uint8_t  *usb_hmsc_SmplConfigTable;
uint8_t  *usb_hmsc_SmplInterfaceTable;
uint16_t usb_hmsc_SmplDevaddr;
uint16_t usb_hmsc_SmplSpeed;
/* Wait for the ANSI flag enumeration initialization sequence. */
int16_t  usb_ghmsc_SmplClassSeqWait;

/******************************************************************************
Function Name   : usb_open_hmsc
Description     : Host MSC class open function. Request host MSC enumeration.
Arguments       : USB_UTR_t *ptr    : The host's USB Comm. Structure for this 
                                    : "devno". (IP-nr, mode, IP-register addr.
                                    : start. etc.
                : struct usb_fninfo *p_fnstr    : File structure pointer.
                : uint8_t devadr                : Device address.
                : uint8_t speed                 : Device connected speed.
Return          : Process result                : NG (-1), OK (0).
******************************************************************************/
int16_t usb_open_hmsc( USB_UTR_t *ptr, struct usb_fninfo *p_fnstr, uint8_t devadr, uint8_t speed )
{
    int16_t        ret = -1;

    /* Open status check */
    if (usb_ghmsc_SmplClassSeq == -1 )
    {
        /* Device Status Enumeration set */
        R_usb_hmsc_SetDevSts((uint16_t)USB_HMSC_DEV_ENU);
        /* Get Device Descriptor */
        R_usb_hstd_EnumGetDescriptor( ptr, devadr, 2, usb_cstd_AnsiCallBack);
        /* Enumeration sequence number update. */
        usb_ghmsc_SmplClassSeq = 0;
        /* Pending state enumeration sequence. */
        usb_ghmsc_SmplClassSeqWait = 1;
    }
    else
    {
        /* Check if not waiting for enumeration sequence. */
        if (usb_ghmsc_SmplClassSeqWait == 0 )
        {
            /* Class Enumeration */
            ret = usb_hmsc_SmplEnumeration( ptr, p_fnstr, devadr, speed );
        }
    }

    return ret;
}
/******************************************************************************
End of function usb_open_hmsc
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_AnsiCallBack
Description     : Callback upon completion of data transmission from USB periph-
                : eral. (Or if you cancel the task waiting for completion of data 
                : transmission and reception). $REA correct?
Arguments       : USB_UTR_t *ptr        : USB Comm. Structure (IP nr, mode, IP reg.
                                        : address etc.
                : uint16_t unused       : Not used
                : uint16_t status       : Transfer result status
Return          : none
******************************************************************************/
void usb_cstd_AnsiCallBack(USB_UTR_t *ptr, uint16_t unused, uint16_t status )
{
    /* Release waiting in the enumeration sequence. */
    usb_ghmsc_SmplClassSeqWait = 0;
}
/******************************************************************************
End of function usb_cstd_AnsiCallBack
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_SmplEnumeration
Description     : Class Enumeration
Arguments       : USB_UTR_t *ptr        : USB Comm. Structure (IP nr, mode, IP reg.
                                        : address etc.
                : usb_fninfo *p_fnstr   : ANSI IO open file object info for the 
                                        : class' peripheral.
                : uint8_t devadr        : device address
                : uint16_t speed        : device speed
Return          : none
******************************************************************************/
int usb_hmsc_SmplEnumeration(USB_UTR_t *ptr, struct usb_fninfo *p_fnstr, uint8_t devadr, uint8_t speed)
{
    uint8_t     string;
    int         ret = -1;
    uint8_t     *p_desc, *p_iftable;
    uint16_t    desc_len, inpipe, outpipe;

    /* Manager mode change */
    switch (usb_ghmsc_SmplClassSeq)
    {
        case USB_SEQ_0:
            /* Get Configuration Descriptor(Short 9Byte) */
            R_usb_hstd_EnumGetDescriptor( ptr, devadr, 3, usb_cstd_AnsiCallBack);
        break;

        case USB_SEQ_1:
            /* Get Configuration Descriptor(Long) */
            R_usb_hstd_EnumGetDescriptor( ptr, devadr, 4, usb_cstd_AnsiCallBack);
        break;
        
        case USB_SEQ_2:
            /* String descriptor */
            usb_hstd_GetStringDescriptor1(ptr, devadr, (uint16_t)15, (USB_CB_t)&usb_cstd_AnsiCallBack);
        break;

        case USB_SEQ_3:
            /* Device descriptor address. */
            p_desc = (uint8_t *)usb_ghstd_DeviceDescriptor;  /* $REA: Why is p_desc needed? Just use usb_ghstd_DeviceDescriptor. */
            string = p_desc[15];  /* iSerial Nr. */
            /* Get string descriptor. (Serial nr.) */
            usb_hstd_GetStringDescriptor2(ptr, devadr, (uint16_t)string, (USB_CB_t)&usb_cstd_AnsiCallBack);
        break;

        case USB_SEQ_4:
            /* Configuration descriptor address.   $REA: p_desc not needed? */
            p_desc = (uint8_t *)usb_ghstd_ConfigurationDescriptor;

            /* Get length of the entire configuration descriptor. */
            desc_len = ((uint16_t)*(p_desc + 3)) << 8;
            desc_len += (uint16_t)*(p_desc + 2);

            /* Allocate a pipe. */
            inpipe  = R_usb_hstd_allocatePipe( ptr, USB_BULK );
            outpipe = inpipe;
            if ((USB_NG == inpipe ) || (USB_NG == outpipe) )
            {
                // Can not allocate HW Pipe
    //            usb_shstd_classErrorInfo = ;
                return (int16_t)-1;
            }

            /* Register this peripheral's control pipe for receiving. $REA Control pipe for device, right? */
            p_fnstr->inPipeNo   = inpipe;
            /* Register this peripheral's control pipe for transmitting. */
            p_fnstr->outPipeNo  = outpipe;

            /* Set pipe for for corresponding entry of host endpoint information table 
            (IN pipe). */
            if (USB_NG == usb_setEpTbl( inpipe, usb_ghmsc_TmpEpTbl ))
            {
    //        usb_shstd_classErrorInfo = ;
                return (int16_t)-1;
            }

            /* Set pipe for for corresponding entry of host endpoint information table 
            (OUT pipe). */
            if (USB_NG == usb_setEpTbl( outpipe, usb_ghmsc_TmpEpTbl ))
            {
                return (int16_t)-1;
            }

            /* Search Interface Descriptor. */
            p_iftable = usb_search_ifDesc(USB_IFCLS_MAS);

            /* Get root port number from device addr */
    //        port    = usb_hstd_GetRootport(ptr, devsel);

    //        desc_len = desc_len - (p_iftable - p_desc);

            /* Use endpoint descriptor received from peripheral devices to set up 
            pipe information table. */
            if (USB_ERROR == usb_hmsc_SmpPipeInfo( p_iftable, usb_ghmsc_StrgCount, speed, desc_len) )
            {
    //            usb_shstd_classErrorInfo = ;
                return -1;
            }
            p_fnstr->isOpened        = USB_YES;

            /* Register this peripheral's control pipe for receiving. $REA already done above. */
            p_fnstr->inPipeNo        = inpipe;
            /* Register this peripheral's control pipe for transmitting. $REA already done above. */
            p_fnstr->outPipeNo        = outpipe;

    //        p_fnstr->class_read = &usb_hstd_read_data_bulk;
    //        p_fnstr->class_write = &usb_hstd_write_data_bulk;
        
            /* Register callback function for processing Close. */
            p_fnstr->close_func = &usb_close_host;

            // Request to send SetConfiguration to Manager Task.
            R_usb_hstd_MgrEnumSetConfiguration( ptr, devadr, *(p_desc + 5), usb_cstd_AnsiCallBack);
        break;

        case USB_SEQ_5:
            ret = USB_OK;
        break;

        default:
        break;
    }

    /* Branch or handle completed enumeration or not. */
    if (ret == USB_OK )
    {
        /* If the enumeration process is completed, init enumeration sequence number. */
        usb_ghmsc_SmplClassSeq = -1;
    }
    else
    {
        /* If enumeration process to continue, update enumeration sequence number. */
        usb_ghmsc_SmplClassSeq++;
        /* Pending state enumeration sequence. */
        usb_ghmsc_SmplClassSeqWait = 1;
    }
    return ret;
}
/******************************************************************************
End of function usb_hmsc_SmplEnumeration
******************************************************************************/
#endif /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */

/******************************************************************************
End  Of File
******************************************************************************/
