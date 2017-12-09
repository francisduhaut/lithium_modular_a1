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
* File Name    : r_usb_ansi_host.c
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : ANSI IO for USB Host
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
*         : 4.12.2012          Function header translation. Ch. tabs to 4 spaces.
*                              SET EDITOR TO INSERT SPACES WHEN TAB PRESSED.
******************************************************************************/

/* $Id: r_usb_ansi_host.c 162 2012-05-21 10:20:32Z ssaek $ */

/******************************************************************************
Includes   <System Includes>, "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"         /* Type define */
#include "r_usb_usrconfig.h"        /* USB-H/W register set (user define) */
#include "r_usb_ckernelid.h"        /* Kernel ID definition */
#include "r_usb_cdefusbip.h"        /* USB-FW Library Header */
#include "r_usb_cmacprint.h"        /* Standard IO macro */
#include "r_usb_cmacsystemcall.h"   /* System call macro */
#include "r_usb_cextern.h"          /* USB-FW global define */
#include "r_usb_ansi.h"
#include "r_usb_api.h"
#include "r_usb_class_usrcfg.h"
#include "r_usb_class_fusrcfg.h"

/******************************************************************************
Section    <Section Definition> , "Project Sections"
******************************************************************************/
#pragma section _ansi

#if    USB_ANSIIO_PP == USB_ANSIIO_USE_PP
/******************************************************************************
Private global variables and functions
******************************************************************************/
/* An enumerated USB device's descriptors will be stored in this structure. */
struct usb_devinfo  usb_devInfo[USB_DEVICENUM*USB_IFNUM*USB_CFGNUM];

/******************************************************************************
External variables and functions
******************************************************************************/
extern int16_t  (*usb_classopen[])(USB_UTR_t *, struct usb_fninfo *, uint8_t, uint8_t);

/******************************************************************************
Static variables and functions
******************************************************************************/
static int16_t  usb_search_DevInfo( USB_CLASS_ENUM_t class );
static uint8_t  usb_Get_Ifclass( USB_CLASS_ENUM_t class );

/* the allocated block number for each PIPE (for R8A66597) */
static const uint8_t          usb_chstd_bufNmb[] = {8, 24, 40, 56, 72, 88};
static struct usb_fninfo    usb_host_fnstr[2u];

/******************************************************************************
Function Name   : usb_open_host
Description     : Call open function of the class specified by the first argument
                : and set the information to the usn_fnInfo structure.
Argument        : USB_CLASS_ENUM_t class       : Class code
                : uint16_t fn                  : File number
Return          : File number                  : -1(NG)
******************************************************************************/
uint16_t    usb_open_host( USB_CLASS_ENUM_t class, int16_t fn )
{
    uint8_t        speed, devadr;
    uint8_t        isopened;
    int16_t        devno, idx;
    USB_UTR_t    *ptr;

    /* Return the device number of the connected USB device based on the 1st arugument(class)*/
    devno    = usb_search_DevInfo( class );
    if( -1 == devno )
    {
        return USB_NG;
    }
    /* The member of usb_devInfo structure is set when USB device is attached */
    ptr        = usb_devInfo[devno].ptr;
    devadr    = usb_devInfo[devno].devadr;  /* Getting the device address */
    speed    = usb_devInfo[devno].speed;    /* Getting the device speed */

    /* Class Index set */
    idx    = (uint16_t)( class );
    ptr->keyword = class;

    /* Calling open function of the class specified by 1st argument(class) */
    isopened = (*usb_classopen[idx])( ptr, &usb_host_fnstr[ptr->ip], devadr, speed );

    /* Checking the completion of open() function */
    if( USB_OK == isopened )
    {
        usb_devInfo[devno].isActDev = USB_YES;  /* Device enable setting */

        /* Setting the information is gotten by open processing to usb_fnInfo(structure) variable */
        usb_fnInfo[fn]            = usb_host_fnstr[ptr->ip];
        usb_fnInfo[fn].devno    = devno;     /* Setting the device number */
        usb_fnInfo[fn].usb_ip   = ptr->ip;   /* Setting USB IP number */
        usb_fnInfo[fn].usb_ipp  = ptr->ipp;  /* Setting USB IP base address */

        return USB_OK;
    }

    return USB_NG;
} /* eof usb_open_host */

/******************************************************************************
Function Name   : usb_close_host
Description     : Release the specified control pipe that was allocated for a 
                : now disconnect function, and clear the usb_fninfo structure.
Argument        : uint16_t    fn : File number
Return          : uint16_t       : USB_OK (success) / USB_NG (failure).
******************************************************************************/
uint16_t    usb_close_host( int16_t fn )
{
    uint8_t        devno;
    USB_UTR_t    *ptr;
    uint16_t    dev_adr;
    uint16_t    i;

    /* Checking whether usb_fnInfo[fn] is enable or not */
    if( USB_NO == usb_fnInfo[fn].isOpened )
    {
        // usb_shstd_classErrorInfo = ;
        /* Setting NG as the return value because usb_fnInfo[fn] is disable */
        return USB_NG;
    }
    usb_fnInfo[fn].isOpened = USB_NO;/* Clear usb_fnInfo[fn](structure) variable */
    devno = usb_fnInfo[fn].devno;    /* Getting the device number */
    ptr   = usb_devInfo[devno].ptr;  /* Getting the pointer that USB IP information that correspond to 
                                      the device number is stored */
    /* Release the HW pipe */
    /* Check pipe number for receiving (read) */
    if( USB_NOPIPE != usb_fnInfo[fn].inPipeNo )
    {
        /* Release the pipe */
        R_usb_hstd_freePipe( ptr, usb_fnInfo[fn].inPipeNo );
        usb_fnInfo[fn].inPipeNo = USB_NOPIPE;
    }

    /* Check pipe number for sending (write) */
    if( USB_NOPIPE != usb_fnInfo[fn].outPipeNo )
    {
        /* Release the pipe */
        R_usb_hstd_freePipe( ptr, usb_fnInfo[fn].outPipeNo );
        usb_fnInfo[fn].outPipeNo = USB_NOPIPE;
    }

    dev_adr = usb_devInfo[devno].devadr;        /* Get Clear device address  */
    for( i = 0; i < ( USB_DEVICENUM * USB_IFNUM * USB_CFGNUM ); i++ )
    {
        if( dev_adr == usb_devInfo[i].devadr )      /* Device address check */
        {
            usb_devInfo[i].devadr = USB_NO;         /* Clear the member for the device address in usb_devInfo[] */
            usb_devInfo[i].isActDev = USB_NO;       /* Clear the member for the device state */
            usb_devInfo[i].interfaceClass = USB_NO; /* Clear the member for the interface class */
        }
    }

    // Normal End
    // usb_shstd_classErrorInfo = ;
    return USB_OK;
} /* eof usb_close_host */

/******************************************************************************
Function Name   : usb_search_DevInfo
Description     : Check whether the specified class corresponds to the connected
                : USB function's class or not.
Argument        : USB_CLASS_ENUM_t class       : Class code
Return          : Device number
******************************************************************************/
static    int16_t    usb_search_DevInfo( USB_CLASS_ENUM_t class )
{
    uint8_t    devno;
    uint8_t    ifclass;

    /* Getting USB Device class code */
    ifclass = usb_Get_Ifclass( class );

    /* Checking whether the specified device class is registered in usb_devInfo[]
    (structure) or not. */
    for( devno = 0; devno < USB_MAX_DEVICENUM; devno++ )
    {
        /* Checking whether the device is registered in TPL(Target Peripheral List) */
        if( USB_OK == usb_devInfo[devno].isTPL )
        {
            /* Checking whether the specified class is registered in usb_devInfo[]
            (structure) or not */
            if( usb_devInfo[devno].interfaceClass == ifclass )
            {
                /* Checking whether the USB device is active or not */
                if( USB_NO == usb_devInfo[devno].isActDev )
                {
                    /* If the device is not active yet, Setting the device number to 
                    the return value */
                    return devno;
                }
            }
        }
    }
    return -1;
} /* eof usb_search_DevInfo */

/******************************************************************************
Function Name   : usb_Get_Ifclass
Description     : Convert from specified class code to USB spec class code.
Argument        : USB_CLASS_ENUM_t class    : Class code used for open().
Return          : USB Interface class code
******************************************************************************/
static    uint8_t    usb_Get_Ifclass( USB_CLASS_ENUM_t class )
{
    uint8_t    code;

    /* Checking the class code */
    switch( class )
    {
        case USB_CLASS_HCDCD:
        case USB_CLASS_PCDCD:
#if USB_DEVICE_CLASS_PP == USB_DEVICE_CLASS_HOST_CDC
            code = USB_HCDC_IFCLS_CDCD;     /* CDC-Data Class */
#else    /* USB_DEVICE_CLASS_PP == USB_DEVICE_CLASS_HOST_CDC */
            code = USB_IFCLS_CDCD;      /* CDC-Data Class */
#endif    /* USB_DEVICE_CLASS_PP == USB_DEVICE_CLASS_HOST_CDC */
            break;
        case USB_CLASS_HCDCC:
        case USB_CLASS_PCDCC:
#if USB_DEVICE_CLASS_PP == USB_DEVICE_CLASS_HOST_CDC
            code = USB_HCDC_IFCLS_CDCC;     /* CDC-Control Class */
#else    /* USB_DEVICE_CLASS_PP == USB_DEVICE_CLASS_HOST_CDC */
            code = USB_IFCLS_CDCC;      /* CDC-Control Class */
#endif    /* USB_DEVICE_CLASS_PP == USB_DEVICE_CLASS_HOST_CDC */
            break;
        case USB_CLASS_HHID:
        case USB_CLASS_PHID:
            code = USB_IFCLS_HID;       /* HID Class */
            break;
        case USB_CLASS_HMSC:
        case USB_CLASS_PMSC:
            code = USB_IFCLS_MAS;       /* Mass Storage Class */
            break;
        case USB_CLASS_HSTD_BULK:
        case USB_CLASS_PSTD_BULK:
        case USB_CLASS_HSTD_INT:
        case USB_CLASS_PSTD_INT:
            code = USB_IFCLS_VEN;       /* Vendor-Specific Class */
        default:
            break;
    }
    return code;
} /* eof usb_Get_Ifclass */

/******************************************************************************
Function Name   : usb_search_ifDesc
Description     : Search for the start address of the Interface descriptor in the 
                : Configuration descriptor of the connected USB Device.
Argument        : uint8_t class            : Class code.
Return          : Top address of host's copy of the interface descriptor.
******************************************************************************/
uint8_t    *usb_search_ifDesc( uint8_t class )
{
    uint8_t        *p_table;
    uint16_t    total_len, offset;

    /* Getting the start address of Configuration Descriptor array */
    p_table = (uint8_t *)usb_ghstd_ConfigurationDescriptor;

    /* Getting the length of Configuration Descriptor  */
    total_len    =    ((uint16_t)*(p_table + 3)) << 8;
    total_len    |=    (uint16_t)*(p_table + 2);

    /* Searching "Interface Descriptor" in Configuration Descriptor */
    for( offset = 0; offset < total_len; offset += *(p_table + offset) )
    {
        /* Checking whether the content of ths pointer is "Interface Descriptor" or not */
        if( USB_DT_INTERFACE == *(p_table + offset + 1) )
        {
            /*  Checking whether the class in Interface Descriptor is the same 
                as the specified class(argument) or not */
            if( class == *(p_table + offset + 5) )
            {
                /* Return the start address of Interface Descriptor in Configuration Descriptor */
                return (p_table + offset);
            }
        }
    }
    return 0;
} /* eof usb_search_ifDesc */

/******************************************************************************
Function Name   : usb_setEpTbl
Description     : Set specified pipe number to the corresponding address of the
                : endpoint information table.
Argument        : uint8_t    pipe     : HW pipe number.
                : uint8_t    *p_eptbl : Top address of endpoint table.
Return          : uint16_t            : USB_OK (success) / USB_NG (failure).
******************************************************************************/
uint16_t    usb_setEpTbl( uint8_t pipe, uint16_t *p_eptbl )
{
    uint8_t    cnt;
    uint8_t    is_set;

    /* Searching the empty pipe in EndPoint information table */
    for( cnt = 0, is_set = USB_NO; (USB_PDTBLEND != *(p_eptbl + cnt))&&(is_set == USB_NO) ; cnt += USB_EPL )
    {
        /* Checking whether the pipe is empty or not */
        if( USB_NONE == *(p_eptbl + cnt) )
        {
            /*  the empty pipe detection */
            
            is_set = USB_YES;

            /* Setting the detection pipe number to EndPoint information table */
            *(p_eptbl + cnt)         = pipe;

            /* Setting the buffer size & the block number of FIFO buffer  */
            *(p_eptbl + cnt + 2)    = USB_BUF_SIZE(512) | usb_chstd_bufNmb[pipe-1];
        }
    }

    /* Checking whether the empty pipe is detected  */
    if( USB_NO == is_set )
    {
        return USB_NG;
    }

    return USB_OK;
} /* eof usb_setEpTbl */
#endif    /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */

/******************************************************************************
End  Of File
******************************************************************************/
