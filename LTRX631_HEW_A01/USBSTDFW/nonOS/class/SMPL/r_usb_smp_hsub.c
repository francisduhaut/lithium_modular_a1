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
* File Name    : r_usb_smp_hsub.c
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB Host sample code
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
******************************************************************************/

/* $Id: r_usb_smp_hsub.c 143 2012-05-07 09:16:46Z tmura $ */


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"     /* Type define */
#include "r_usb_usrconfig.h"
#include "r_usb_cdefusbip.h"        /* USB-FW Library Header */
#include "r_usb_cmacprint.h"        /* Standard IO macro */
#include "r_usb_cmacsystemcall.h"   /* uITRON system call macro */
#include "r_usb_cextern.h"          /* USB-FW global define */
#include "r_usb_api.h"


/******************************************************************************
Section    <Section Definition> , "Project Sections"
******************************************************************************/
#pragma section _hcd


/******************************************************************************
Constant macro definitions
******************************************************************************/
#define CLSDATASIZE     512


/******************************************************************************
External variables and functions
******************************************************************************/
void            usb_hstd_DeviceDescripInfo(void);
void            usb_hstd_ConfigDescripInfo(void);
void            usb_hstd_EndpDescripInfo(uint8_t *table);
uint16_t        usb_hstd_CheckDescriptor(uint8_t *table, uint16_t spec);

uint16_t        usb_hstd_SetInterface(USB_UTR_t *ptr, uint16_t addr, uint16_t altnum, uint16_t intnum,
                                        USB_CB_t complete);
uint16_t        usb_hstd_GetStatus(USB_UTR_t *ptr, uint16_t addr, uint16_t epnum, USB_CB_t complete);
uint16_t        usb_hstd_SetFeature(USB_UTR_t *ptr, uint16_t addr, uint16_t epnum, USB_CB_t completeC);
uint16_t        usb_hstd_GetConfigDesc(USB_UTR_t *ptr, uint16_t addr, uint16_t length, USB_CB_t complete);
uint16_t        usb_hstd_GetStringDesc(USB_UTR_t *ptr, uint16_t addr, uint16_t string, USB_CB_t complete);
uint16_t        usb_hstd_StdReqCheck(uint16_t errcheck);
uint16_t        usb_hstd_GetStringDescriptor1(USB_UTR_t *ptr, uint16_t devaddr, uint16_t index, USB_CB_t complete);
uint16_t        usb_hstd_GetStringDescriptor1Check(uint16_t errcheck);
uint16_t        usb_hstd_GetStringDescriptor2(USB_UTR_t *ptr, uint16_t devaddr, uint16_t index, USB_CB_t complete);
uint16_t        usb_hstd_GetStringDescriptor2Check(uint16_t errcheck);
uint16_t        usb_hstd_CmdSubmit(USB_UTR_t *ptr, USB_CB_t complete);

uint16_t        usb_hstd_ChkStatus08(USB_UTR_t *ptr);
uint16_t        usb_hstd_ChkStatus16(USB_UTR_t *ptr);
uint16_t        usb_hstd_ChkRemote(USB_UTR_t *ptr);
uint16_t        usb_cstd_ClassTransWaitTmo(uint16_t tmo);



/******************************************************************************
Private global variables and functions
******************************************************************************/
uint8_t         usb_ghstd_ClassData[USB_NUM_USBIP][CLSDATASIZE];
USB_UTR_t       usb_ghstd_ClassControl[USB_NUM_USBIP];
uint16_t        usb_ghstd_ClassRequest[5];


/******************************************************************************
Renesas Abstracted Host sample functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_DeviceDescripInfo
Description     : device descriptor info
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_DeviceDescripInfo(void)
{
/* Condition compilation by the difference of useful function */
#ifdef USB_DEBUGPRINT_PP
    uint8_t     *p;

    p = usb_hstd_DevDescriptor();

    /* For DEMO */
    USB_PRINTF0("Device descriptor fields\n");
    USB_PRINTF2("  bcdUSB         : %02x.%02x\n",  p[0x03], p[0x02]);
    USB_PRINTF1("  bDeviceClass   : 0x%02x\n",     p[0x04]);
    USB_PRINTF1("  bDeviceSubClass: 0x%02x\n",     p[0x05]);
    USB_PRINTF1("  bProtocolCode  : 0x%02x\n",     p[0x06]);
    USB_PRINTF1("  bMaxPacletSize : 0x%02x\n",     p[0x07]);
    USB_PRINTF2("  idVendor       : 0x%02x%02x\n", p[0x09], p[0x08]);
    USB_PRINTF2("  idProduct      : 0x%02x%02x\n", p[0x0b], p[0x0a]);
    USB_PRINTF2("  bcdDevice      : 0x%02x%02x\n", p[0x0d], p[0x0c]);
    USB_PRINTF1("  iSerialNumber  : 0x%02x\n",     p[0x10]);
    USB_PRINTF1("  bNumConfig     : 0x%02x\n",     p[0x11]);
    USB_PRINTF0("\n");
#endif  /* USB_DEBUGPRINT_PP */
}
/******************************************************************************
End of function usb_hstd_DeviceDescripInfo
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_ConfigDescripInfo
Description     : configuration descriptor info
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_ConfigDescripInfo(void)
{
/* Condition compilation by the difference of useful function */
#ifdef USB_DEBUGPRINT_PP
    uint8_t     *p;
    uint16_t    len;

    p = usb_hstd_ConDescriptor();

    len = 0;
    while( len < (p[2]) )
    {
        /* Search within Configuration descriptor */
        switch( p[len + 1] )
        {
        /* Configuration Descriptor ? */
        case 0x02:
            USB_PRINTF0("Configuration descriptor fields\n");
            USB_PRINTF1("  Configuration Value  : 0x%02x\n", p[0x05]);
            USB_PRINTF1("  Number of Interface  : 0x%02x\n", p[0x04]);
            break;
        /* Interface Descriptor ? */
        case 0x04:
            USB_PRINTF0("\nInterface descriptor fields\n");
            switch( p[len + 5] )
            {
            /* Class check */
            /* Audio Class */
            case 1: 
                USB_PRINTF0("  This device has Audio Class.\n");
                break;
            /* CDC-Control Class */
            case 2:
                USB_PRINTF0("  This device has CDC-Control Class.\n");
                break;
            /* HID Class */
            case 3:
                USB_PRINTF0("  This device has HID Class.\n");
                break;
            /* Physical Class */
            case 5:
                USB_PRINTF0("  This device has Physical Class.\n");
                break;
            /* Image Class */
            case 6:
                USB_PRINTF0("  This device has Image Class.\n");
                break;
            /* Printer Class */
            case 7:
                USB_PRINTF0("  This device has Printer Class.\n");
                break;
            /* Mass Storage Class */
            case 8:
                USB_PRINTF0("  I/F class    : Mass Storage\n");
                switch( p[len + 6] )
                {
                case 0x05:
                    USB_PRINTF0("  I/F subclass : SFF-8070i\n");
                    break;
                case 0x06:
                    USB_PRINTF0("  I/F subclass : SCSI command\n");
                    break;
                default:
                    USB_PRINTF0("### I/F subclass not support.\n");
                }
                if( p[len + 7] == 0x50 )
                {
                    /* Check Interface Descriptor (protocol) */
                    USB_PRINTF0("  I/F protocol : BOT\n");
                }
                else
                {
                    USB_PRINTF0("### I/F protocol not support.\n");
                }
                break;
            /* HUB Class */
            case 9:
                USB_PRINTF0("  This device has HUB Class.\n");
                break;
            /* CDC-Data Class */
            case 10:
                USB_PRINTF0("  This device has CDC-Data Class.\n");
                break;
            /* Chip/Smart Card Class */
            case 11:
                USB_PRINTF0("  This device has Chip/Smart Class.\n");
                break;
            /* Content-Security Class */
            case 13:
                USB_PRINTF0("  This device has Content-Security Class.\n");
                break;
            /* Video Class */
            case 14:
                USB_PRINTF0("  This device has Video Class.\n");
                break;
            /* Vendor Specific Class */
            case 255:
                USB_PRINTF0("  I/F class : Vendor Specific\n");
                break;
            /* Reserved */
            case 0:
                USB_PRINTF0("  I/F class : class error\n");
                break;
            default:
                USB_PRINTF0("  This device has not USB Device Class.\n");
                break;
            }
            break;
        /* Endpoint Descriptor */
        case 0x05:
            usb_hstd_EndpDescripInfo(&p[len]);
            break;
        default:
            break;
        }
        len += p[len];
    }
#endif  /* USB_DEBUGPRINT_PP */
}
/******************************************************************************
End of function usb_hstd_ConfigDescripInfo
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_EndpDescripInfo
Description     : endpoint descriptor info
Arguments       : uint8_t *tbl              : table (indata).
Return value    : none
******************************************************************************/
void usb_hstd_EndpDescripInfo(uint8_t *tbl)
{
/* Condition compilation by the difference of useful function */
#ifdef USB_DEBUGPRINT_PP
    uint16_t        epnum, pipe_mxp;

    switch( (uint8_t)(tbl[3] & USB_EP_TRNSMASK) )
    {
    /* Isochronous Endpoint */
    case USB_EP_ISO:
        USB_PRINTF0("  ISOCHRONOUS");
        break;
    /* Bulk Endpoint */
    case USB_EP_BULK:
        USB_PRINTF0("  BULK");
        break;
    /* Interrupt Endpoint */
    case USB_EP_INT:
        USB_PRINTF0("  INTERRUPT");
        break;
    /* Control Endpoint */
    default:
        USB_PRINTF0("### Control pipe is not support.\n");
        break;
    }

    if( (uint8_t)(tbl[2] & USB_EP_DIRMASK) == USB_EP_IN )
    {
        /* Endpoint address set */
        USB_PRINTF0(" IN  endpoint\n");
    }
    else
    {
        USB_PRINTF0(" OUT endpoint\n");
    }
    /* Endpoint number set */
    epnum       = (uint16_t)((uint16_t)tbl[2] & USB_EP_NUMMASK);
    /* Max packet size set */
    pipe_mxp = (uint16_t)(tbl[4] | (uint16_t)((uint16_t)(tbl[5]) << 8));
    USB_PRINTF2("   Number is %d. MaxPacket is %d. \n", epnum, pipe_mxp);
    switch( (uint16_t)(tbl[3] & USB_EP_TRNSMASK) )
    {
    /* Isochronous Endpoint */
    case 0x01u:
    /* Interrupt Endpoint */
    case 0x03u:
        USB_PRINTF1("    interval time is %d\n", tbl[6]);
        break;
    default:
        break;
    }
#endif  /* USB_DEBUGPRINT_PP */
}
/******************************************************************************
End of function usb_hstd_EndpDescripInfo
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_CheckDescriptor
Description     : check descriptor
Arguments       : uint8_t *table            : table (indata).
                : uint16_t spec             : spec.
Return value    : none
******************************************************************************/
uint16_t usb_hstd_CheckDescriptor(uint8_t *table, uint16_t spec)
{
/* Condition compilation by the difference of useful function */
#ifdef USB_DEBUGPRINT_PP
    /* Check Descriptor */
    if( table[1] == spec )
    {
        switch( table[1] )
        {
        /* Device Descriptor */
        case USB_DT_DEVICE:
            USB_PRINTF0("  Device Descriptor.\n");
            break;
        /* Configuration Descriptor */
        case USB_DT_CONFIGURATION:
            USB_PRINTF0("  Configuration Descriptor.\n");
            break;
        /* String Descriptor */
        case USB_DT_STRING:
            USB_PRINTF0("  String Descriptor.\n");
            break;
        /* Interface Descriptor */
        case USB_DT_INTERFACE:
            USB_PRINTF0("  Interface Descriptor.\n");
            break;
        /* Endpoint Descriptor */
        case USB_DT_ENDPOINT:
            USB_PRINTF0("  Endpoint Descriptor.\n");
            break;
        /* Device Qualifier Descriptor */
        case USB_DT_DEVICE_QUALIFIER:
            USB_PRINTF0("  Device Qualifier Descriptor.\n");
            break;
        /* Other Speed Configuration Descriptor */
        case USB_DT_OTHER_SPEED_CONF:
            USB_PRINTF0("  Other Speed Configuration Descriptor.\n");
            break;
        /* Interface Power Descriptor */
        case USB_DT_INTERFACE_POWER:
            USB_PRINTF0("  Interface Power Descriptor.\n");
            break;
        /* HUB Descriptor */
        case USB_DT_HUBDESCRIPTOR:
            USB_PRINTF0("  HUB Descriptor.\n");
            break;
        /* Not Descriptor */
        default:
            USB_PRINTF0("### Descriptor error (Not Standard Descriptor).\n");
            return USB_ERROR;
            break;
        }
        return USB_DONE;
    }
    else
    {
        switch( table[1] )
        {
        /* Device Descriptor */
        case USB_DT_DEVICE:
            USB_PRINTF0("### Descriptor error ( Device Descriptor ).\n");
            break;
        /* Configuration Descriptor */
        case USB_DT_CONFIGURATION:
            USB_PRINTF0("### Descriptor error ( Configuration Descriptor ).\n");
            break;
        /* String Descriptor */
        case USB_DT_STRING:
            USB_PRINTF0("### Descriptor error ( String Descriptor ).\n");
            break;
        /* Interface Descriptor ? */
        case USB_DT_INTERFACE:
            USB_PRINTF0("### Descriptor error ( Interface Descriptor ).\n");
            break;
        /* Endpoint Descriptor */
        case USB_DT_ENDPOINT:
            USB_PRINTF0("### Descriptor error ( Endpoint Descriptor ).\n");
            break;
        /* Device Qualifier Descriptor */
        case USB_DT_DEVICE_QUALIFIER:
            USB_PRINTF0("### Descriptor error ( Device Qualifier Descriptor ).\n");
            break;
        /* Other Speed Configuration Descriptor */
        case USB_DT_OTHER_SPEED_CONF:
            USB_PRINTF0("### Descriptor error ( Other Speed Configuration Descriptor ).\n");
            break;
        /* Interface Power Descriptor */
        case USB_DT_INTERFACE_POWER:
            USB_PRINTF0("### Descriptor error ( Interface Power Descriptor ).\n");
            break;
        /* Not Descriptor */
        default:
            USB_PRINTF0("### Descriptor error ( Not Standard Descriptor ).\n");
            break;
        }
        return USB_ERROR;
    }
#else   /* Not USB_DEBUGPRINT_PP */
        return USB_DONE;
#endif  /* USB_DEBUGPRINT_PP */
}
/******************************************************************************
End of function usb_hstd_CheckDescriptor
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_SetInterface
Description     : Set SetInterface
Arguments       : uint16_t addr            : device address
                : uint16_t altnum          : descriptor index
                : uint16_t intfnum         : interface number
                : USB_CB_t complete        : callback function
Return value    : uint16_t                 : error info
******************************************************************************/
uint16_t usb_hstd_SetInterface(USB_UTR_t *ptr, uint16_t addr, uint16_t altnum, uint16_t intnum, USB_CB_t complete)
{
    usb_ghstd_ClassRequest[0]   = USB_SET_INTERFACE | USB_HOST_TO_DEV | USB_STANDARD | USB_INTERFACE;
    usb_ghstd_ClassRequest[1]   = altnum;   /* Alternate number */
    usb_ghstd_ClassRequest[2]   = intnum;   /* Interface number */
    usb_ghstd_ClassRequest[3]   = (uint16_t)0x0000;
    usb_ghstd_ClassRequest[4]   = addr;

    return usb_hstd_CmdSubmit(ptr, complete);
}
/******************************************************************************
End of function usb_hstd_SetInterface
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_GetStatus
Description     : Set GetStatus
Arguments       : uint16_t addr            : device address
                : uint16_t epnum           : endpoint number
                : USB_CB_t complete        : callback function
Return value    : uint16_t                 : error info
******************************************************************************/
uint16_t usb_hstd_GetStatus(USB_UTR_t *ptr, uint16_t addr, uint16_t epnum, USB_CB_t complete)
{
    uint16_t        i;

    if( epnum == 0xFF )
    {
        /* GetStatus(Device) */
        usb_ghstd_ClassRequest[0]   = USB_GET_STATUS | USB_DEV_TO_HOST | USB_STANDARD | USB_DEVICE;
        usb_ghstd_ClassRequest[2]   = (uint16_t)0x0000;
    }
    else
    {
        /* GetStatus(endpoint) */
        usb_ghstd_ClassRequest[0]   = USB_GET_STATUS | USB_DEV_TO_HOST | USB_STANDARD | USB_ENDPOINT;
        usb_ghstd_ClassRequest[2]   = epnum;
    }
    usb_ghstd_ClassRequest[1]   = (uint16_t)0x0000;
    usb_ghstd_ClassRequest[3]   = (uint16_t)0x0002;
    usb_ghstd_ClassRequest[4]   = addr;

    for( i = 0; i < usb_ghstd_ClassRequest[3]; i++ )
    {
        usb_ghstd_ClassData[ptr->ip][i] = (uint8_t)0xFF;
    }

    return usb_hstd_CmdSubmit(ptr, complete);
}
/******************************************************************************
End of function usb_hstd_GetStatus
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_SetFeature
Description     : Set SetFeature
Arguments       : uint16_t addr            : device address
                : uint16_t epnum           : endpoint number
                : USB_CB_t complete        : callback function
Return value    : uint16_t                 : error info
******************************************************************************/
uint16_t usb_hstd_SetFeature(USB_UTR_t *ptr, uint16_t addr, uint16_t epnum, USB_CB_t complete)
{
    if( epnum == 0xFF )
    {
        /* SetFeature(Device) */
        usb_ghstd_ClassRequest[0]   = USB_SET_FEATURE | USB_HOST_TO_DEV | USB_STANDARD | USB_DEVICE;
        usb_ghstd_ClassRequest[1]   = USB_DEV_REMOTE_WAKEUP;
        usb_ghstd_ClassRequest[2]   = (uint16_t)0x0000;
    }
    else
    {
        /* SetFeature(endpoint) */
        usb_ghstd_ClassRequest[0]   = USB_SET_FEATURE | USB_HOST_TO_DEV | USB_STANDARD | USB_ENDPOINT;
        usb_ghstd_ClassRequest[1]   = USB_ENDPOINT_HALT;
        usb_ghstd_ClassRequest[2]   = epnum;
    }
    usb_ghstd_ClassRequest[3]   = (uint16_t)0x0000;
    usb_ghstd_ClassRequest[4]   = addr;

    return usb_hstd_CmdSubmit(ptr, complete);
}
/******************************************************************************
End of function usb_hstd_SetFeature
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_GetConfigDesc
Description     : Set GetConfigurationDescriptor
Arguments       : uint16_t addr            : device address
                : uint16_t length          : descriptor length
                : USB_CB_t complete        : callback function
Return value    : uint16_t                 : error info
******************************************************************************/
uint16_t usb_hstd_GetConfigDesc(USB_UTR_t *ptr, uint16_t addr, uint16_t length, USB_CB_t complete)
{
    uint16_t        i;

    /* Get Configuration Descriptor */
    usb_ghstd_ClassRequest[0]   = USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_STANDARD | USB_DEVICE;
    usb_ghstd_ClassRequest[1]   = USB_CONF_DESCRIPTOR;
    usb_ghstd_ClassRequest[2]   = (uint16_t)0x0000;
    usb_ghstd_ClassRequest[3]   = length;
    if( usb_ghstd_ClassRequest[3] > CLSDATASIZE )
    {
        usb_ghstd_ClassRequest[3] = (uint16_t)CLSDATASIZE;
        USB_PRINTF0("***WARNING Descriptor size over !\n");
    }
    usb_ghstd_ClassRequest[4]   = addr;

    for( i = 0; i < usb_ghstd_ClassRequest[3]; i++ )
    {
        usb_ghstd_ClassData[ptr->ip][i] = (uint8_t)0xFF;
    }

    return usb_hstd_CmdSubmit(ptr, complete);
}
/******************************************************************************
End of function usb_hstd_GetConfigDesc
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_GetStringDesc
Description     : Set GetDescriptor
Arguments       : uint16_t addr            : device address
                : uint16_t string          : descriptor index
                : USB_CB_t complete        : callback function
Return value    : uint16_t                 : error info
******************************************************************************/
uint16_t usb_hstd_GetStringDesc(USB_UTR_t *ptr, uint16_t addr, uint16_t string, USB_CB_t complete)
{
    uint16_t        i;

    if( string == 0 )
    {
        usb_ghstd_ClassRequest[2] = (uint16_t)0x0000;
        usb_ghstd_ClassRequest[3] = (uint16_t)0x0004;
    }
    else
    {
        /* Set LanguageID */
        usb_ghstd_ClassRequest[2] = (uint16_t)(usb_ghstd_ClassData[ptr->ip][2]);
        usb_ghstd_ClassRequest[2] |= (uint16_t)((uint16_t)(usb_ghstd_ClassData[ptr->ip][3]) << 8);
        usb_ghstd_ClassRequest[3] = (uint16_t)CLSDATASIZE;
    }
    usb_ghstd_ClassRequest[0]   = USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_STANDARD | USB_DEVICE;
    usb_ghstd_ClassRequest[1]   = (uint16_t)(USB_STRING_DESCRIPTOR + string);
    usb_ghstd_ClassRequest[4]   = addr;

    for( i = 0; i < usb_ghstd_ClassRequest[3]; i++ )
    {
        usb_ghstd_ClassData[ptr->ip][i] = (uint8_t)0xFF;
    }

    return usb_hstd_CmdSubmit(ptr, complete);
}
/******************************************************************************
End of function usb_hstd_GetStringDesc
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_StdReqCheck
Description     : Sample Standard Request Check
Arguments       : uint16_t errcheck        : error
Return value    : uint16_t                 : error info
******************************************************************************/
uint16_t usb_hstd_StdReqCheck(uint16_t errcheck)
{
    if( errcheck == USB_DATA_TMO )
    {
        USB_PRINTF0("*** Standard Request Timeout error !\n");
        return  USB_ERROR;
    }
    else if( errcheck == USB_DATA_STALL )
    {
        USB_PRINTF0("*** Standard Request STALL !\n");
        return  USB_ERROR;
    }
    else if( errcheck != USB_CTRL_END )
    {
        USB_PRINTF0("*** Standard Request error !\n");
        return  USB_ERROR;
    }
    else
    {
    }
    return  USB_DONE;
}
/******************************************************************************
End of function usb_hstd_StdReqCheck
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_GetStringDescriptor1
Description     : Get String descriptor
Arguments       : uint16_t devaddr          : device address
                : uint16_t index            : descriptor index
                : USB_CB_t complete         : callback
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hstd_GetStringDescriptor1(USB_UTR_t *ptr, uint16_t devaddr, uint16_t index, USB_CB_t complete)
{
    usb_hstd_GetStringDesc(ptr, devaddr, (uint16_t)0, complete);

    return  USB_DONE;
}
/******************************************************************************
End of function usb_hstd_GetStringDescriptor1
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_GetStringDescriptor2
Description     : Get String descriptor
Arguments       : uint16_t devaddr          : device address
                : uint16_t index            : descriptor index
                : USB_CB_t complete         : callback
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hstd_GetStringDescriptor2(USB_UTR_t *ptr, uint16_t devaddr, uint16_t index, USB_CB_t complete)
{
    usb_hstd_GetStringDesc(ptr, devaddr, index, complete);

    return  USB_DONE;
}
/******************************************************************************
End of function usb_hstd_GetStringDescriptor2
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_GetStringDescriptor1Check
Description     : Get String descriptor Check
Arguments       : uint16_t errcheck         : errcheck
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hstd_GetStringDescriptor1Check(uint16_t errcheck)
{
    if( errcheck == (USB_ER_t)USB_DATA_STALL )
    {
        USB_PRINTF0("*** LanguageID  not support !\n");
        return USB_ERROR;
    }
    else if( errcheck != (USB_ER_t)USB_CTRL_END )
    {
        USB_PRINTF0("*** LanguageID  not support !\n");
        return USB_ERROR;
    }
    else
    {
    }

    return  USB_DONE;
}
/******************************************************************************
End of function usb_hstd_GetStringDescriptor1Check
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_GetStringDescriptor2Check
Description     : Get String descriptor Check
Arguments       : uint16_t errcheck         : errcheck
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hstd_GetStringDescriptor2Check(uint16_t errcheck)
{
    if( errcheck == (USB_ER_t)USB_DATA_STALL )
    {
        USB_PRINTF0("*** SerialNumber not support !\n");
        return USB_ERROR;
    }
    else if( errcheck != (USB_ER_t)USB_CTRL_END )
    {
        USB_PRINTF0("*** SerialNumber not support !\n");
        return USB_ERROR;
    }
    else
    {
    }

    return USB_DONE;
}
/******************************************************************************
End of function usb_hstd_GetStringDescriptor2Check
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_ChkStatus08
Description     : check status
Arguments       : none
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hstd_ChkStatus08(USB_UTR_t *ptr)
{
    return (uint16_t)usb_ghstd_ClassData[ptr->ip][0];
}
/******************************************************************************
End of function usb_hstd_ChkStatus08
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_ChkStatus16
Description     : check status
Arguments       : none
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hstd_ChkStatus16(USB_UTR_t *ptr)
{
    return (uint16_t)(((uint16_t)(usb_ghstd_ClassData[ptr->ip][0]) << 8) + usb_ghstd_ClassData[ptr->ip][1]);
}
/******************************************************************************
End of function usb_hstd_ChkStatus16
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_ChkRemote
Description     : check remote
Arguments       : none
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hstd_ChkRemote(USB_UTR_t *ptr)
{
    if( (usb_ghstd_ClassData[ptr->ip][7] & USB_CF_RWUPON) != (uint8_t)0 )
    {
        return USB_YES;
    }
    return USB_NO;
}
/******************************************************************************
End of function usb_hstd_ChkRemote
******************************************************************************/


/* Condition compilation by the difference of IP */
/******************************************************************************
Function Name   : usb_hstd_CmdSubmit
Description     : command submit
Arguments       : USB_CB_t complete         : callback info
Return value    : uint16_t                  : USB_DONE
******************************************************************************/
uint16_t usb_hstd_CmdSubmit(USB_UTR_t *ptr, USB_CB_t complete)
{
    usb_ghstd_ClassControl[ptr->ip].tranadr = (void*)usb_ghstd_ClassData[ptr->ip];
    usb_ghstd_ClassControl[ptr->ip].complete    = complete;
    usb_ghstd_ClassControl[ptr->ip].tranlen = (uint32_t)usb_ghstd_ClassRequest[3];
    usb_ghstd_ClassControl[ptr->ip].keyword = USB_PIPE0;
    usb_ghstd_ClassControl[ptr->ip].setup   = usb_ghstd_ClassRequest;
    usb_ghstd_ClassControl[ptr->ip].segment = USB_TRAN_END;

    usb_ghstd_ClassControl[ptr->ip].ip = ptr->ip;
    usb_ghstd_ClassControl[ptr->ip].ipp = ptr->ipp;

    R_usb_hstd_TransferStart(&usb_ghstd_ClassControl[ptr->ip]);
    
    return USB_DONE;
}
/******************************************************************************
End of function usb_hstd_CmdSubmit
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/
