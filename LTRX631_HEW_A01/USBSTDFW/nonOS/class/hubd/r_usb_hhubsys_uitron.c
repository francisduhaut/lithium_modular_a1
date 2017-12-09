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
* File Name    : r_usb_hHubsys_uitron.c
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB Host Hub system code for uitron
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
******************************************************************************/

/* $Id: r_usb_hhubsys_uitron.c 162 2012-05-21 10:20:32Z ssaek $ */

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"         /* Type define */
#include "r_usb_usrconfig.h"
#include "r_usb_ckernelid.h"        /* Kernel ID definition */
#include "r_usb_cdefusbip.h"        /* USB-FW Library Header */
#include "r_usb_cmacprint.h"        /* Standard IO macro */
#include "r_usb_cmacsystemcall.h"   /* uITRON system call macro */
#include "r_usb_cextern.h"          /* USB-FW global define */
#include "r_usb_cusb_bitdefine.h"
#include "r_usb_reg_access.h"
#include "r_usb_api.h"

/* Condition compilation by the difference of USB function */
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP

/******************************************************************************
Section    <Section Definition> , "Project Sections"
******************************************************************************/
#pragma section _hub


/******************************************************************************
Constant macro definitions
******************************************************************************/
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_1PORT_PP
#define USB_MAXHUB              (uint16_t)1
#endif  /* USB_1PORT_PP */
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
#define USB_MAXHUB              (uint16_t)2
#endif  /* USB_2PORT_PP */

#define USB_HUB_CLSDATASIZE     (uint16_t)512


/******************************************************************************
External variables and functions
******************************************************************************/
extern  void        (*usb_ghstd_EnumarationProcess[])(USB_UTR_t *,uint16_t,uint16_t);
extern  void        usb_hstd_DeviceDescripInfo(void);
extern  void        usb_hstd_ConfigDescripInfo(void);
extern  uint16_t    usb_hstd_GetStringDescriptor(USB_UTR_t *, uint16_t, uint16_t);
extern  uint16_t    usb_cstd_ClassTransWaitTmo(uint16_t tmo);

/* Enumeration request */
//extern uint16_t   usb_ghstd_EnumSeq[2u];
extern uint8_t  usb_ghstd_ClassData[][512];

/******************************************************************************
Static variables and functions
******************************************************************************/
static  uint16_t    usb_hhub_ChkConfig(uint16_t **table, uint16_t spec);
static  uint16_t    usb_hhub_ChkInterface(uint16_t **table, uint16_t spec);
static  uint16_t    usb_hhub_PipeInfo(USB_UTR_t *ptr, uint8_t *table, uint16_t offset
                        , uint16_t speed, uint16_t length);
static  void        usb_hhub_PortDetach(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t portnum);
static  void        usb_hhub_SelectiveDetach(USB_UTR_t *ptr, uint16_t devaddr);
static  void        usb_hhub_TransStart(USB_UTR_t *ptr, uint16_t hubaddr, uint32_t size
                        , uint8_t *table, USB_CB_t complete);
static  void        usb_hhub_TransResult(USB_UTR_t *mess, uint16_t, uint16_t);
static  uint16_t    usb_hhub_GetNewDevaddr(USB_UTR_t *ptr);
static  uint16_t    usb_hhub_GetHubaddr(USB_UTR_t *ptr, uint16_t pipenum);
static  uint16_t    usb_hhub_GetCnnDevaddr(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t portnum);
static  uint16_t    usb_hhub_ChkTblIndx1(USB_UTR_t *ptr, uint16_t hubaddr);
static  uint16_t    usb_hhub_ChkTblIndx2(USB_UTR_t *ptr, uint16_t hubaddr);
static  uint16_t    usb_hhub_PortSetFeature(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t port
                        , uint16_t command, USB_CB_t complete);
static  uint16_t    usb_hhub_PortClrFeature(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t port
                        , uint16_t command, USB_CB_t complete);
static  void        usb_hhub_InputStatus(void);
static  void        usb_hhub_InitDownPort(USB_UTR_t *ptr, uint16_t hubaddr, USB_CLSINFO_t *mess);
static  void        usb_hhub_NewConnect(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t portnum, USB_CLSINFO_t *mess);
static  uint16_t    usb_hhub_PortAttach(uint16_t hubaddr, uint16_t portnum, USB_CLSINFO_t *mess);
static  void        usb_hhub_PortReset(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t portnum, USB_CLSINFO_t *mess);
static  uint16_t    usb_hhub_ReqTransWait(void);
static  uint16_t    usb_hhub_TransWaitTmo(uint16_t tmo);

/******************************************************************************
Private global variables and functions
******************************************************************************/
/* Control transfer message */
USB_UTR_t       usb_shhub_ControlMess[USB_NUM_USBIP];
/* Data transfer message */
USB_UTR_t       usb_shhub_DataMess[USB_NUM_USBIP][USB_MAXDEVADDR + 1u];
/* HUB descriptor */
uint8_t         usb_ghhub_Descriptor[USB_NUM_USBIP][USB_CONFIGSIZE];
/* HUB status data */
uint8_t         usb_ghhub_Data[USB_NUM_USBIP][USB_MAXDEVADDR + 1u][8];
/* HUB downport status */
uint16_t        usb_shhub_DownPort[USB_NUM_USBIP][USB_MAXDEVADDR + 1u];
/* Downport remotewakeup */
uint16_t        usb_shhub_Remote[USB_NUM_USBIP][USB_MAXDEVADDR + 1u];
/* Up-hubaddr, up-hubport, portnum, pipenum */
uint16_t        usb_shhub_InfoData[USB_NUM_USBIP][USB_MAXDEVADDR + 1u][4];
uint16_t        usb_shhub_Number[USB_NUM_USBIP];
uint16_t        usb_shhub_ClassRequest[USB_NUM_USBIP][5];
USB_MGRINFO_t   *usb_shhub_Msg[USB_NUM_USBIP];

/* TPL table of Host hub */
const uint16_t usb_ghhub_TPL[USB_NUM_USBIP][4] =
{
  {
    1,                      /* Number of list */
    0,                      /* Reserved */
    USB_NOVENDOR,           /* Vendor ID  : no-check */
    USB_NOPRODUCT,          /* Product ID : no-check */
  },
#if USB_NUM_USBIP == 2
  {
    1,                      /* Number of list */
    0,                      /* Reserved */
    USB_NOVENDOR,           /* Vendor ID  : no-check */
    USB_NOPRODUCT,          /* Product ID : no-check */
  }
#endif  /* USB_NUM_USBIP == 2 */
};

/* Host hub endpoint table definition */
uint16_t usb_ghhub_DefEPTbl[USB_NUM_USBIP][6 * 5] =
{
  {
/* Condition compilation by the difference of IP */
#if USB_IP_PIPE_PP == USB_IP_PIPE_9_PP
    USB_PIPE9,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(7u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
    USB_PIPE8,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(6u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
#endif  /* USB_IP_PIPE_9_PP */

    USB_PIPE7,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(5u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
    USB_PIPE6,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(4u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,

/* Pipe end */
    USB_PDTBLEND,
  },
#if USB_NUM_USBIP == 2
  {
/* Condition compilation by the difference of IP */
#if USB_IP_PIPE_PP == USB_IP_PIPE_9_PP
    USB_PIPE9,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(7u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
    USB_PIPE8,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(6u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
#endif  /* USB_IP_PIPE_9_PP */

    USB_PIPE7,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(5u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
    USB_PIPE6,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(4u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,

/* Pipe end */
    USB_PDTBLEND,
  }
#endif  /* USB_NUM_USBIP == 2 */
};

/* Host hub temporary endpoint table definition */
uint16_t usb_ghhub_TmpEPTbl[USB_NUM_USBIP][6 * 5] =
{
  {
/* Condition compilation by the difference of IP */
#if USB_IP_PIPE_PP == USB_IP_PIPE_9_PP
    USB_PIPE9,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(7u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
    USB_PIPE8,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(6u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
#endif  /* USB_IP_PIPE_9_PP */

    USB_PIPE7,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(5u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
    USB_PIPE6,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(4u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,

/* Pipe end */
    USB_PDTBLEND,
  },
#if USB_NUM_USBIP == 2
  {
/* Condition compilation by the difference of IP */
#if USB_IP_PIPE_PP == USB_IP_PIPE_9_PP
    USB_PIPE9,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(7u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
    USB_PIPE8,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(6u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
#endif  /* USB_IP_PIPE_9_PP */

    USB_PIPE7,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(5u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
    USB_PIPE6,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(4u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,

/* Pipe end */
    USB_PDTBLEND,
  }
#endif  /*  USB_NUM_USBIP == 2 */
};


/******************************************************************************
Renesas Abstracted Hub Driver API functions
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_hhub_Registration
Description     : HUB driver
Arguments       : USB_HCDREG_t *callback
Return value    : none
******************************************************************************/
void R_usb_hhub_Registration(USB_UTR_t *ptr, USB_HCDREG_t *callback)
{
    USB_HCDREG_t    driver;

    /* Driver registration */
    if( callback == USB_NULL )
    {
        /* Interface Class */
        driver.ifclass      = (uint16_t)USB_IFCLS_HUB;
        /* Target peripheral list */
        driver.tpl          = (uint16_t*)&usb_ghhub_TPL[ptr->ip];
        /* Pipe Define Table address */
        driver.pipetbl      = (uint16_t*)&usb_ghhub_DefEPTbl[ptr->ip];
        /* Driver init */
        driver.classinit    = &usb_hhub_Initial;
        /* Driver check */
        driver.classcheck   = &usb_hhub_ChkClass;
        /* Device configured */
        driver.devconfig    = (USB_CB_INFO_t)&R_usb_hhub_Open;
        /* Device detach */
        driver.devdetach    = (USB_CB_INFO_t)&R_usb_hhub_Close;
        /* Device suspend */
        driver.devsuspend   = &usb_cstd_DummyFunction;
        /* Device resume */
        driver.devresume    = &usb_cstd_DummyFunction;
        /* Device over current */
        driver.overcurrent  = &usb_cstd_DummyFunction;
        R_usb_hstd_DriverRegistration(ptr, (USB_HCDREG_t *)&driver);
    }
    else
    {
        /* Interface Class */
        driver.ifclass      = (uint16_t)USB_IFCLS_HUB;
        /* Target peripheral list */
        driver.tpl          = callback->tpl;
        /* Pipe Define Table address */
        driver.pipetbl      = callback->pipetbl;
        /* Driver init */
        driver.classinit    = &usb_hhub_Initial;
        /* Driver check */
        driver.classcheck   = &usb_hhub_ChkClass;
        /* Device configured */
        driver.devconfig    = (USB_CB_INFO_t)&R_usb_hhub_Open;
        /* Device detach */
        driver.devdetach    = (USB_CB_INFO_t)&R_usb_hhub_Close;
        /* Device suspend */
        driver.devsuspend   = &usb_cstd_DummyFunction;
        /* Device resume */
        driver.devresume    = &usb_cstd_DummyFunction;
        /* Device over current */
        driver.overcurrent  = &usb_cstd_DummyFunction;
        R_usb_hstd_DriverRegistration(ptr, (USB_HCDREG_t *)&driver);
    }
}
/******************************************************************************
End of function R_usb_hhub_Registration
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_Release
Description     : HUB driver release
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hhub_Release(USB_UTR_t *ptr)
{
    uint16_t    i;

    for( i = 0; i < USB_MAXHUB; i++ )
    {
        /* Hub driver release */
        R_usb_hstd_DriverRelease(ptr, (uint8_t)USB_IFCLS_HUB);
    }
}
/******************************************************************************
End of function usb_hhub_Release
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_Initial
Description     : Global memory initialized
Arguments       : uint16_t data1 : necessary for regist the callback
                : uint16_t data2 : necessary for regist the callback
Return value    : none
******************************************************************************/
void usb_hhub_Initial(USB_UTR_t *ptr, uint16_t data1, uint16_t data2)
{
    uint16_t    i;

    for( i = 0u; i < (USB_MAXDEVADDR + 1u); i++ )
    {
        usb_shhub_InfoData[ptr->ip][i][0] = 0;
        usb_shhub_InfoData[ptr->ip][i][1] = 0;
        usb_shhub_InfoData[ptr->ip][i][2] = 0;
        usb_shhub_InfoData[ptr->ip][i][3] = 0;
    }
    usb_shhub_Number[ptr->ip] = 0;
}
/******************************************************************************
End of function usb_hhub_Initial
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_ChkConfig
Description     : Configuration Descriptor check
Arguments       : uint16_t **table          : Configuration Descriptor
                : uint16_t spec             : HUB specification
Return value    : uint16_t                  : DONE/ERROR
******************************************************************************/
uint16_t usb_hhub_ChkConfig(uint16_t **table, uint16_t spec)
{
    uint8_t     *descriptor_table;
    uint16_t    ofset;

    descriptor_table = (uint8_t*)(table[1]);

    /* Descriptor check */
    ofset = usb_hstd_CheckDescriptor(descriptor_table, (uint16_t)USB_DT_CONFIGURATION);
    if( ofset == USB_ERROR )
    {
        USB_PRINTF0("### Configuration descriptor error !\n");
        return USB_ERROR;
    }

    /* Check interface number */
    switch( spec )
    {
    case USB_FSHUB:     /* Full Speed Hub */
        if( descriptor_table[4] != USB_HUB_INTNUMFS )
        {
            USB_PRINTF0("### HUB configuration descriptor error !\n");
            return USB_ERROR;
        }
        break;
    case USB_HSHUBS:    /* Hi Speed Hub(Single) */
        if( descriptor_table[4] != USB_HUB_INTNUMHSS )
        {
            USB_PRINTF0("### HUB configuration descriptor error !\n");
            return USB_ERROR;
        }
        break;
    case USB_HSHUBM:    /* Hi Speed Hub(Multi) */
        if( descriptor_table[4] != USB_HUB_INTNUMHSM )
        {
            USB_PRINTF0("### HUB configuration descriptor error !\n");
            return USB_ERROR;
        }
        break;
    default:
        return USB_ERROR;
        break;
    }
    return USB_DONE;
}
/******************************************************************************
End of function usb_hhub_ChkConfig
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_ChkInterface
Description     : Interface Descriptor check
Arguments       : uint16_t **table          : Interface Descriptor
                : uint16_t spec             : HUB specification
Return value    : uint16_t                  : DONE/ERROR
******************************************************************************/
uint16_t usb_hhub_ChkInterface(uint16_t **table, uint16_t spec)
{
    uint8_t     *descriptor_table;
    uint16_t    ofset;

    descriptor_table = (uint8_t*)(table[2]);

    /* Descriptor check */
    ofset = usb_hstd_CheckDescriptor(descriptor_table, (uint16_t)USB_DT_INTERFACE);
    if( ofset == USB_ERROR )
    {
        USB_PRINTF0("### Interface descriptor error !\n");
        return USB_ERROR;
    }

    /* Check interface class */
    if( descriptor_table[5] != USB_IFCLS_HUB )
    {
        USB_PRINTF0("### HUB interface descriptor error !\n");
        return USB_ERROR;
    }

    /* Check interface number */
    switch( spec )
    {
    case USB_FSHUB:     /* Full Speed Hub */
        if( descriptor_table[2] != (USB_HUB_INTNUMFS - 1u) )
        {
            USB_PRINTF0("### HUB interface descriptor error !\n");
            return USB_ERROR;
        }
        break;
    case USB_HSHUBS:    /* Hi Speed Hub(Single) */
        if( descriptor_table[2] != (USB_HUB_INTNUMHSS - 1u) )
        {
            USB_PRINTF0("### HUB interface descriptor error !\n");
            return USB_ERROR;
        }
        break;
    case USB_HSHUBM:    /* Hi Speed Hub(Multi) */
        if( descriptor_table[2] != (USB_HUB_INTNUMHSM - 1u) )
        {
            USB_PRINTF0("### HUB interface descriptor error !\n");
            return USB_ERROR;
        }
        break;
    default:
        return USB_ERROR;
        break;
    }
    return USB_DONE;
}
/******************************************************************************
End of function usb_hhub_ChkInterface
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_PipeInfo
Description     : Set pipe information
Arguments       : uint8_t  *table           : Interface Descriptor
                : uint16_t offset           : DEF_EP table offset
                : uint16_t speed            : device speed
                : uint16_t length           : Interface Descriptor length
Return value    : uint16_t                  : DONE/ERROR
******************************************************************************/
uint16_t usb_hhub_PipeInfo(USB_UTR_t *ptr, uint8_t *table, uint16_t offset, uint16_t speed, uint16_t length)
{
    uint16_t        ofdsc;
    uint16_t        retval;

    /* Check Descriptor */
    if( table[1] != USB_DT_INTERFACE )
    {
        /* Configuration Descriptor */
        USB_PRINTF0("### Interface descriptor error (HUB).\n");
        return USB_ERROR;
    }

    /* Check Endpoint Descriptor */
    ofdsc = table[0];
    while( ofdsc < (length - table[0]) )
    {
        /* Search within Interface */
        switch( table[ofdsc + 1] )
        {
        /* Device Descriptor */
        case USB_DT_DEVICE:
        /* Configuration Descriptor */
        case USB_DT_CONFIGURATION:
        /* String Descriptor */
        case USB_DT_STRING:
        /* Interface Descriptor */
        case USB_DT_INTERFACE:
            USB_PRINTF0("### Endpoint Descriptor error(HUB).\n");
            return USB_ERROR;
            break;
        /* Endpoint Descriptor */
        case USB_DT_ENDPOINT:
            if( (table[ofdsc + 3] & USB_EP_TRNSMASK) == USB_EP_INT )
            {
                /* Interrupt Endpoint */
                retval = R_usb_hstd_ChkPipeInfo(speed, &usb_ghhub_TmpEPTbl[ptr->ip][offset], &table[ofdsc]);
                if( retval == USB_DIR_H_IN )
                {
                    return USB_DONE;
                }
                else
                {
                    USB_PRINTF0("### Endpoint Descriptor error(HUB).\n");
                }
            }
            ofdsc += table[ofdsc];
            break;
        /* Device Qualifier Descriptor */
        case USB_DT_DEVICE_QUALIFIER:
        /* Other Speed Configuration */
        case USB_DT_OTHER_SPEED_CONF:
        /* Interface Power Descriptor */
        case USB_DT_INTERFACE_POWER:
            USB_PRINTF0("### Endpoint Descriptor error(HUB).\n");
            return USB_ERROR;
            break;
        /* Another Descriptor */
        default:
            ofdsc += table[ofdsc];
            break;
        }
    }
    return USB_ERROR;
}
/******************************************************************************
End of function usb_hhub_PipeInfo
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_PortDetach
Description     : HUB down port disconnect
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t portnum          : down port number
Return value    : none
******************************************************************************/
void usb_hhub_PortDetach(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t portnum)
{
    uint16_t        md;
    USB_HCDREG_t    *driver;
    /* Device number --> DeviceAddress */
    uint16_t        devaddr;

    /* HUB downport status */
    usb_shhub_DownPort[ptr->ip][hubaddr]    &= (uint16_t)(~USB_BITSET(portnum));
    /* HUB downport status */
    usb_shhub_Remote[ptr->ip][hubaddr]  &= (uint16_t)(~USB_BITSET(portnum));
    /* Now downport device search */
    devaddr = usb_hhub_GetCnnDevaddr( ptr, hubaddr, portnum);
    for( md = 0; md < usb_ghstd_DeviceNum[ptr->ip]; md++ )
    {
        driver = &usb_ghstd_DeviceDrv[ptr->ip][md];
        if( devaddr == driver->devaddr )
        {
            (*driver->devdetach)(ptr, driver->devaddr, (uint16_t)USB_NO_ARG);

            /* Root port */
            usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][0] = USB_NOPORT;
            /* Device state */
            usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][1] = USB_DETACHED;
            /* Not configured */
            usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][2] = (uint16_t)0;
            /* Interface Class : NO class */
            usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][3] = (uint16_t)USB_IFCLS_NOT;
            /* No connect */
            usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][4] = (uint16_t)USB_NOCONNECT;

            /* Root port */
            driver->rootport    = USB_NOPORT;
            /* Device address */
            driver->devaddr     = USB_NODEVICE;
            /* Device state */
            driver->devstate    = USB_DETACHED;
        }
    }
    /* Selective detach */
    usb_hhub_SelectiveDetach(ptr, devaddr);
}
/******************************************************************************
End of function usb_hhub_PortDetach
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_SelectiveDetach
Description     : HUB down port Selective disconnect
Arguments       : uint16_t devaddr          : device address
Return value    : none
******************************************************************************/
void usb_hhub_SelectiveDetach(USB_UTR_t *ptr, uint16_t devaddr)
{
    uint16_t    addr, i;

    addr = (uint16_t)(devaddr << USB_DEVADDRBIT);
    if( usb_hstd_GetDevSpeed(ptr, addr) != USB_NOCONNECT )
    {
        for( i = USB_MIN_PIPE_NO; i <= USB_MAX_PIPE_NO; i++ )
        {
            /* Not control transfer */
            if( usb_cstd_GetDeviceAddress(ptr, i) == addr )
            {
                /* Agreement device address */
                if( usb_cstd_GetPid(ptr, i) == USB_PID_BUF )
                {
                    /* PID=BUF ? */
                    R_usb_hstd_TransferEnd(ptr, i, (uint16_t)USB_DATA_STOP);
                }
                usb_cstd_ClrPipeCnfg(ptr, i);
            }
        }
        usb_hstd_SetDevAddr(ptr, addr, USB_DONE, USB_DONE);
        usb_hstd_SetHubPort(ptr, addr, USB_DONE, USB_DONE);
        USB_PRINTF1("*** Device address %d clear.\n", devaddr);
    }
}
/******************************************************************************
End of function usb_hhub_SelectiveDetach
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_TransStart
Description     : HUB sys data transfer / control transfer
Arguments       : uint16_t hubaddr          : hub address
                : uint32_t size             : Data Transfer size
                : uint8_t  *table           : Receive Data area
                : USB_CB_t complete         : Callback function
Return value    : none
******************************************************************************/
void usb_hhub_TransStart(USB_UTR_t *ptr, uint16_t hubaddr, uint32_t size, uint8_t *table, USB_CB_t complete)
{
    /* Transfer structure setting */
    usb_shhub_DataMess[ptr->ip][hubaddr].keyword    = usb_shhub_InfoData[ptr->ip][hubaddr][3];
    usb_shhub_DataMess[ptr->ip][hubaddr].tranadr    = table;
    usb_shhub_DataMess[ptr->ip][hubaddr].tranlen    = size;
    usb_shhub_DataMess[ptr->ip][hubaddr].setup      = 0;
    usb_shhub_DataMess[ptr->ip][hubaddr].status     = USB_DATA_WAIT;
    usb_shhub_DataMess[ptr->ip][hubaddr].complete   = complete;
    usb_shhub_DataMess[ptr->ip][hubaddr].segment    = USB_TRAN_END;

    usb_shhub_DataMess[ptr->ip][hubaddr].ipp        = ptr->ipp;
    usb_shhub_DataMess[ptr->ip][hubaddr].ip         = ptr->ip;

    /* Transfer start */
    R_usb_hstd_TransferStart(&usb_shhub_DataMess[ptr->ip][hubaddr]);
}
/******************************************************************************
End of function usb_hhub_TransStart
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_TransResult
Description     : Transfer result
Arguments       : USB_UTR_t *mess    : USB system internal message.
Return value    : none
******************************************************************************/
void usb_hhub_TransResult(USB_UTR_t *mess, uint16_t data1, uint16_t data2)
{
    USB_ER_t        err;

    mess->msginfo = USB_MSG_HUB_SUBMITRESULT;
    /* Send message */
    err = USB_SND_MSG(USB_HUB_MBX, (USB_MSG_t*)mess);
    if( err != USB_E_OK )
    {
        /* Send message failure */
        USB_PRINTF1("### hub_TransResult snd_msg error (%ld)\n", err);
    }
}
/******************************************************************************
End of function usb_hhub_TransResult
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_GetNewDevaddr
Description     : Get the new device address
                : when connection of a device detected in the down port of HUB
Arguments       : none
Return value    : uint16_t                  : New device address
******************************************************************************/
uint16_t usb_hhub_GetNewDevaddr(USB_UTR_t *ptr)
{
    uint16_t i;

    /* Search new device */
    for( i = (USB_HUBDPADDR); i < (USB_MAXDEVADDR + 1u); i++ )
    {
        if( usb_shhub_InfoData[ptr->ip][i][0] == 0 )
        {
            /* New device address */
            return i;
        }
    }
    return 0;
}
/******************************************************************************
End of function usb_hhub_GetNewDevaddr
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_GetHubaddr
Description     : Get the HUB address
                : from the pipe number for HUB notification
Arguments       : uint16_t pipenum          : pipe number
Return value    : uint16_t                  : HUB address
******************************************************************************/
uint16_t usb_hhub_GetHubaddr(USB_UTR_t *ptr, uint16_t pipenum)
{
    uint16_t    i;

    for( i = 1; i < (USB_MAXDEVADDR + 1u); i++ )
    {
        if( usb_shhub_InfoData[ptr->ip][i][3] == pipenum )
        {
            /* Return HUB address */
            return i;
        }
    }
    return 0;
}
/******************************************************************************
End of function usb_hhub_GetHubaddr
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_GetCnnDevaddr
Description     : Get the connected device address
                : from the HUB address and the down port number of a HUB
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t portnum          : down port number
Return value    : uint16_t                  : Connected device address
******************************************************************************/
uint16_t usb_hhub_GetCnnDevaddr(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t portnum)
{
    uint16_t    i;

    for( i = (USB_HUBDPADDR); i < (USB_MAXDEVADDR + 1u); i++ )
    {
        if( (usb_shhub_InfoData[ptr->ip][i][0] == hubaddr) &&
            (usb_shhub_InfoData[ptr->ip][i][1] == portnum) )
        {
            /* Return Device address */
            return i;
        }
    }
    return 0;
}
/******************************************************************************
End of function usb_hhub_GetCnnDevaddr
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_ChkTblIndx1
Description     : table index search
Arguments       : uint16_t hubaddr          : hub address
Return value    : uint16_t                  : table index
******************************************************************************/
uint16_t usb_hhub_ChkTblIndx1(USB_UTR_t *ptr, uint16_t hubaddr)
{
    uint16_t    pipecheck[USB_MAX_PIPE_NO];
    uint16_t    i;

    for( i = 0; i < USB_MAX_PIPE_NO; i++ )
    {
        /* Check table clear */
        pipecheck[i] = 0;
    }

    for( i = 0; i < (USB_MAXDEVADDR + 1u); i++ )
    {
        /* Used pipe number set */
        if( usb_shhub_InfoData[ptr->ip][i][3] != 0 )
        {
            pipecheck[usb_shhub_InfoData[ptr->ip][i][3] - 1] = 1;
        }
    }

    for( i = USB_MAX_PIPE_NO; i != 0; i-- )
    {
        if( pipecheck[i - 1] == 0 )
        {
            return (uint16_t)((USB_MAX_PIPE_NO - i) * USB_EPL);
        }
    }
    return (USB_ERROR);
}
/******************************************************************************
End of function usb_hhub_ChkTblIndx1
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_ChkTblIndx2
Description     : table index search
Arguments       : uint16_t hubaddr          : hub address
Return value    : uint16_t                  : table index
******************************************************************************/
uint16_t usb_hhub_ChkTblIndx2(USB_UTR_t *ptr, uint16_t hubaddr)
{
/* Search table index */
/* Condition compilation by the difference of IP */
#if USB_IP_PIPE_PP == USB_IP_PIPE_7_PP
    switch( usb_shhub_InfoData[ptr->ip][hubaddr][3] )
    {
    case USB_PIPE7: return (uint16_t)(0u * USB_EPL);    break;
    case USB_PIPE6: return (uint16_t)(1u * USB_EPL);    break;
    default:        break;
    }
#endif  /* USB_IP_PIPE_7_PP */

/* Search table index */
/* Condition compilation by the difference of IP */
#if USB_IP_PIPE_PP == USB_IP_PIPE_9_PP
    switch( usb_shhub_InfoData[ptr->ip][hubaddr][3] )
    {
    case USB_PIPE9: return (uint16_t)(0u * USB_EPL);    break;
    case USB_PIPE8: return (uint16_t)(1u * USB_EPL);    break;
    case USB_PIPE7: return (uint16_t)(2u * USB_EPL);    break;
    case USB_PIPE6: return (uint16_t)(3u * USB_EPL);    break;
    default:        break;
    }
#endif  /* USB_IP_PIPE_9_PP */

    return (USB_ERROR);
}
/******************************************************************************
End of function usb_hhub_ChkTblIndx2
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_ChkTblIndx3
Description     : table index search
Arguments       : uint16_t pipenum          : pipe number
Return value    : uint16_t                  : table index
******************************************************************************/
uint16_t usb_hhub_ChkTblIndx3(uint16_t pipenum)
{
/* Search table index */
/* Condition compilation by the difference of IP */
#if USB_IP_PIPE_PP == USB_IP_PIPE_7_PP
    switch( pipenum )
    {
    case USB_PIPE7: return (uint16_t)(0u * USB_EPL);    break;
    case USB_PIPE6: return (uint16_t)(1u * USB_EPL);    break;
    default:        break;
    }
#endif  /* USB_IP_PIPE_7_PP */

/* Search table index */
/* Condition compilation by the difference of IP */
#if USB_IP_PIPE_PP == USB_IP_PIPE_9_PP
    switch( pipenum )
    {
    case USB_PIPE9: return (uint16_t)(0u * USB_EPL);    break;
    case USB_PIPE8: return (uint16_t)(1u * USB_EPL);    break;
    case USB_PIPE7: return (uint16_t)(2u * USB_EPL);    break;
    case USB_PIPE6: return (uint16_t)(3u * USB_EPL);    break;
    default:        break;
    }
#endif  /* USB_IP_PIPE_9_PP */

    return (USB_ERROR);
}
/******************************************************************************
End of function usb_hhub_ChkTblIndx3
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hhub_GetHubInformation
Description     : Read HUB-Descriptor
Arguments       : uint16_t hubaddr          : hub address
                : USB_CB_t complete
Return value    : uint16_t                  : DONE/ERROR
******************************************************************************/
uint16_t R_usb_hhub_GetHubInformation(USB_UTR_t *ptr, uint16_t hubaddr, USB_CB_t complete)
{
    uint16_t    err;

    /* Request */
    usb_shhub_ClassRequest[ptr->ip][0]  = USB_GET_DESCRIPTOR | USB_DEV_TO_HOST |USB_CLASS | USB_DEVICE;
    usb_shhub_ClassRequest[ptr->ip][1]  = USB_HUB_DESCRIPTOR;
    usb_shhub_ClassRequest[ptr->ip][2]  = 0;
    usb_shhub_ClassRequest[ptr->ip][3]  = 0x0047;
    usb_shhub_ClassRequest[ptr->ip][4]  = hubaddr;  /* Device address */

    /* HUB Descriptor */
    usb_shhub_ControlMess[ptr->ip].keyword  = USB_PIPE0;
    usb_shhub_ControlMess[ptr->ip].tranadr  = (void*)&usb_ghhub_Descriptor[ptr->ip][0];
    usb_shhub_ControlMess[ptr->ip].tranlen  = (uint32_t)usb_shhub_ClassRequest[ptr->ip][3];
    usb_shhub_ControlMess[ptr->ip].setup    = usb_shhub_ClassRequest[ptr->ip];
    usb_shhub_ControlMess[ptr->ip].segment  = USB_TRAN_END;
    usb_shhub_ControlMess[ptr->ip].complete = complete;

    usb_shhub_ControlMess[ptr->ip].ipp      = ptr->ipp;
    usb_shhub_ControlMess[ptr->ip].ip       = ptr->ip;

    /* Transfer start */
    R_usb_hstd_TransferStart(&usb_shhub_ControlMess[ptr->ip]);

    err = usb_hhub_ReqTransWait();
    return err;
}
/******************************************************************************
End of function R_usb_hhub_GetHubInformation
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_PortSetFeature
Description     : SetFeature request
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t port             : down port number
                : uint16_t command          : request command
Return value    : uint16_t                  : DONE/ERROR
******************************************************************************/
uint16_t usb_hhub_PortSetFeature(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t port, uint16_t command, USB_CB_t complete)
{
    uint16_t        err;

    /* Request */
    usb_shhub_ClassRequest[ptr->ip][0]  = USB_SET_FEATURE | USB_HOST_TO_DEV | USB_CLASS | USB_OTHER;
    usb_shhub_ClassRequest[ptr->ip][1]  = command;
    usb_shhub_ClassRequest[ptr->ip][2]  = port;         /* Port number */
    usb_shhub_ClassRequest[ptr->ip][3]  = 0;
    usb_shhub_ClassRequest[ptr->ip][4]  = hubaddr;      /* Device address */

    /* Port SetFeature */
    usb_shhub_ControlMess[ptr->ip].keyword  = USB_PIPE0;
    usb_shhub_ControlMess[ptr->ip].tranadr  = (void*)&usb_ghhub_Data[ptr->ip][hubaddr][0];
    usb_shhub_ControlMess[ptr->ip].tranlen  = (uint32_t)usb_shhub_ClassRequest[ptr->ip][3];
    usb_shhub_ControlMess[ptr->ip].setup    = usb_shhub_ClassRequest[ptr->ip];
    usb_shhub_ControlMess[ptr->ip].segment  = USB_TRAN_END;
    usb_shhub_ControlMess[ptr->ip].complete = complete;

    usb_shhub_ControlMess[ptr->ip].ipp      = ptr->ipp;
    usb_shhub_ControlMess[ptr->ip].ip       = ptr->ip;

    /* Transfer start */
    R_usb_hstd_TransferStart(&usb_shhub_ControlMess[ptr->ip]);

    err = usb_hhub_ReqTransWait();
    return err;
}
/******************************************************************************
End of function usb_hhub_PortSetFeature
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_PortClrFeature
Description     : ClearFeature request
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t port             : down port number
                : uint16_t command          : request command
Return value    : uint16_t                  : DONE/ERROR
******************************************************************************/
uint16_t usb_hhub_PortClrFeature(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t port, uint16_t command, USB_CB_t complete)
{
    uint16_t        err;

    /* Request */
    usb_shhub_ClassRequest[ptr->ip][0]  = USB_CLEAR_FEATURE | USB_HOST_TO_DEV | USB_CLASS | USB_OTHER;
    usb_shhub_ClassRequest[ptr->ip][1]  = command;
    usb_shhub_ClassRequest[ptr->ip][2]  = port;         /* Port number */
    usb_shhub_ClassRequest[ptr->ip][3]  = 0;
    usb_shhub_ClassRequest[ptr->ip][4]  = hubaddr;      /* Device address */

    /* Port ClearFeature */
    usb_shhub_ControlMess[ptr->ip].keyword  = USB_PIPE0;
    usb_shhub_ControlMess[ptr->ip].tranadr  = (void*)&usb_ghhub_Data[ptr->ip][hubaddr][0];
    usb_shhub_ControlMess[ptr->ip].tranlen  = (uint32_t)usb_shhub_ClassRequest[ptr->ip][3];
    usb_shhub_ControlMess[ptr->ip].setup    = usb_shhub_ClassRequest[ptr->ip];
    usb_shhub_ControlMess[ptr->ip].segment  = USB_TRAN_END;
    usb_shhub_ControlMess[ptr->ip].complete = complete;

    usb_shhub_ControlMess[ptr->ip].ipp      = ptr->ipp;
    usb_shhub_ControlMess[ptr->ip].ip       = ptr->ip;

    /* Transfer start */
    R_usb_hstd_TransferStart(&usb_shhub_ControlMess[ptr->ip]);

    err = usb_hhub_ReqTransWait();
    return err;
}
/******************************************************************************
End of function usb_hhub_PortClrFeature
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hhub_GetPortInformation
Description     : GetStatus request
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t port             : down port number
                : USB_CB_t complete
Return value    : uint16_t                  : DONE/ERROR
******************************************************************************/
uint16_t R_usb_hhub_GetPortInformation(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t port, USB_CB_t complete)
{
    uint16_t        err;

    /* Request */
    usb_shhub_ClassRequest[ptr->ip][0]  = USB_GET_STATUS | USB_DEV_TO_HOST | USB_CLASS | USB_OTHER;
    usb_shhub_ClassRequest[ptr->ip][1]  = 0;
    usb_shhub_ClassRequest[ptr->ip][2]  = port;     /* Port number */
    usb_shhub_ClassRequest[ptr->ip][3]  = 4;
    usb_shhub_ClassRequest[ptr->ip][4]  = hubaddr;  /* Device address */

    /* Port GetStatus */
    usb_shhub_ControlMess[ptr->ip].keyword  = USB_PIPE0;
    usb_shhub_ControlMess[ptr->ip].tranadr  = (void*)&usb_ghhub_Data[ptr->ip][hubaddr][0];
    usb_shhub_ControlMess[ptr->ip].tranlen  = (uint32_t)usb_shhub_ClassRequest[ptr->ip][3];
    usb_shhub_ControlMess[ptr->ip].setup    = usb_shhub_ClassRequest[ptr->ip];
    usb_shhub_ControlMess[ptr->ip].segment  = USB_TRAN_END;
    usb_shhub_ControlMess[ptr->ip].complete = complete;

    usb_shhub_ControlMess[ptr->ip].ipp      = ptr->ipp;
    usb_shhub_ControlMess[ptr->ip].ip       = ptr->ip;

    /* Transfer start */
    R_usb_hstd_TransferStart(&usb_shhub_ControlMess[ptr->ip]);

    err = usb_hhub_ReqTransWait();
    return err;
}
/******************************************************************************
End of function R_usb_hhub_GetPortInformation
******************************************************************************/


/******************************************************************************
Renesas Abstracted Hub Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_hhub_Task
Description     : HUB task
Arguments       : USB_VP_INT stacd
Return value    : none
******************************************************************************/
void usb_hhub_Task(USB_VP_INT stacd)
{
    usb_cpu_DelayXms((uint16_t)200);
    /* Device Descriptor */
    usb_hstd_DeviceDescripInfo();
    /* Config Descriptor */
    usb_hstd_ConfigDescripInfo();
    USB_PRINTF0("\n");
    USB_PRINTF0("HHHHHHHHHHHHHHHHHHHHHHHHH\n");
    USB_PRINTF0("    USB HOST USB_597IP   \n");
    USB_PRINTF0("      HUB CLASS DEMO     \n");
    USB_PRINTF0("HHHHHHHHHHHHHHHHHHHHHHHHH\n");
    USB_PRINTF0("\n");

    usb_hhub_InputStatus();
}
/******************************************************************************
End of function usb_hhub_Task
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_ChkClass
Description     : HUB Class driver check
Arguments       : uint16_t **table          : Descriptor, etc
Return value    : none
******************************************************************************/
void usb_hhub_ChkClass(USB_UTR_t *ptr, uint16_t **table)
{
    uint8_t     *device_tbl, *config_tbl, *interface_tbl;
    uint16_t    *result;
    uint16_t    ofset, hub_spec, speed, devaddr, index;
    uint16_t    err;
/* Work area debug  */
/* Condition compilation by the difference of useful function */
#ifdef USB_DEBUGPRINT_PP
    uint32_t    j;
    uint8_t     pdata[32];
#endif  /* USB_DEBUGPRINT_PP */

    device_tbl      = (uint8_t*)(table[0]);
    config_tbl      = (uint8_t*)(table[1]);
    interface_tbl   = (uint8_t*)(table[2]);
    result          = table[3];
    hub_spec        = *table[4];
    speed           = *table[6];
    devaddr         = *table[7];
    index           = usb_hhub_ChkTblIndx1( ptr, devaddr);
    *result         = USB_DONE;

    /* Configuration Descriptor check */
    ofset = usb_hhub_ChkConfig(table, hub_spec);
    if( ofset == USB_ERROR )
    {
        USB_PRINTF0("### Configuration descriptor error !\n");
        *result = USB_ERROR;
        return;
    }

    /* Interface Descriptor check */
    ofset = usb_hhub_ChkInterface(table, hub_spec);
    if( ofset == USB_ERROR )
    {
        USB_PRINTF0("### Interface descriptor error !\n");
        *result = USB_ERROR;
        return;
    }

    /* String descriptor check */
//  err = usb_hstd_GetStringDescriptor(devaddr, (uint16_t)device_tbl[15]);
    err = usb_hstd_GetStringDescriptor(ptr, devaddr, (uint16_t)device_tbl[15]);
    if( err == USB_DONE )
    {
        if( usb_ghstd_ClassData[ptr->ip][0] < (uint8_t)(32 * 2 + 2) )
        {
            usb_ghstd_ClassData[ptr->ip][0] = (uint8_t)(usb_ghstd_ClassData[ptr->ip][0] / 2);
            usb_ghstd_ClassData[ptr->ip][0] = (uint8_t)(usb_ghstd_ClassData[ptr->ip][0] - 1);
        }
        else
        {
            usb_ghstd_ClassData[ptr->ip][0] = (uint8_t)32;
        }
/* LCD for debug */
/* Condition compilation by the difference of useful function */
#ifdef USB_DEBUGPRINT_PP
        for( j = (uint16_t)0; j < usb_ghstd_ClassData[ptr->ip][0]; j++ )
        {
            pdata[j] = usb_ghstd_ClassData[ptr->ip][j * (uint16_t)2 + (uint16_t)2];
        }
        pdata[usb_ghstd_ClassData[ptr->ip][0]] = 0;
        USB_PRINTF1("    Product name : %s\n", pdata);
#endif  /* USB_DEBUGPRINT_PP */
    }
    else
    {
        USB_PRINTF0("*** Product name error\n");
    }

    /* Get HUB descriptor */
    err = R_usb_hhub_GetHubInformation( ptr, devaddr, usb_cstd_ClassTransResult);
    if( err == USB_DONE )
    {
        /* Descriptor check */
        ofset = usb_hstd_CheckDescriptor(usb_ghhub_Descriptor[ptr->ip], (uint16_t)USB_DT_HUBDESCRIPTOR);
        if( ofset == USB_ERROR )
        {
            USB_PRINTF0("### HUB descriptor error !\n");
            *result = USB_ERROR;
            return;
        }
        if( usb_ghhub_Descriptor[ptr->ip][2] > USB_HUBDOWNPORT )
        {
            USB_PRINTF0("### HUB Port number over\n");
            *result = USB_ERROR;
            return;
        }
        USB_PRINTF1("    Attached %d port HUB\n", usb_ghhub_Descriptor[ptr->ip][2]);
    }
    else
    {
        USB_PRINTF0("### HUB Descriptor over\n");
        *result = USB_ERROR;
        return;
    }

    /* Pipe Information table set */
    switch( hub_spec )
    {
    /* FS Hub */
    case USB_FSHUB:
        if( speed == USB_FSCONNECT )
        {
            ofset = usb_hhub_PipeInfo( ptr, interface_tbl, index, speed, (uint16_t)config_tbl[2]);
            if( ofset == USB_ERROR )
            {
                USB_PRINTF0("### Device information error(HUB) !\n");
                *result = USB_ERROR;
                return;
            }
        }
        else
        {
            USB_PRINTF0("### HUB Descriptor speed error\n");
            *result = USB_ERROR;
        }
        break;
    /* HS Hub */
    case USB_HSHUBS:    /* Single */
        if( speed == USB_HSCONNECT )
        {
            ofset = usb_hhub_PipeInfo( ptr, interface_tbl, index, speed, (uint16_t)config_tbl[2]);
            if( ofset == USB_ERROR )
            {
                USB_PRINTF0("### Device information error(HUB) !\n");
                *result = USB_ERROR;
                return;
            }
        }
        else
        {
            USB_PRINTF0("### HUB Descriptor speed error\n");
            *result = USB_ERROR;
        }
        break;
    case USB_HSHUBM:    /* Multi */
        if( speed == USB_HSCONNECT )
        {
            ofset = usb_hhub_PipeInfo( ptr, interface_tbl, index, speed, (uint16_t)config_tbl[2]);
            if( ofset == USB_ERROR )
            {
                USB_PRINTF0("### Device information error(HUB) !\n");
                *result = USB_ERROR;
                return;
            }
            ofset = usb_hhub_PipeInfo( ptr, interface_tbl, index, speed, (uint16_t)config_tbl[2]);
            if( ofset == USB_ERROR )
            {
                USB_PRINTF0("### Device information error(HUB) !\n");
                *result = USB_ERROR;
                return;
            }
        }
        else
        {
            USB_PRINTF0("### HUB Descriptor speed error\n");
            *result = USB_ERROR;
        }
        break;
    default:
        *result = USB_ERROR;
        return;
        break;
    }
    /* Port number set */
    usb_shhub_InfoData[ptr->ip][devaddr][2] = usb_ghhub_Descriptor[ptr->ip][2];
    return;
}
/******************************************************************************
End of function usb_hhub_ChkClass
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hhub_Open
Description     : HUB sys open
Arguments       : uint16_t devaddr          : device address
                : uint16_t data2            : Not use
Return value    : USB_ER_t                  : Error Info
******************************************************************************/
void R_usb_hhub_Open(USB_UTR_t *ptr, uint16_t devaddr, uint16_t data2)
{
    USB_ER_t        err, err2;
    USB_RTST_t      refinfo;
    USB_MH_t        p_blf;
    USB_MGRINFO_t   *mp;
    uint16_t        hubaddr, index;

/* Info area create */
/* Condition compilation by the difference of the device's operating system */
    err = USB_ERROR;
    hubaddr = (uint16_t)(devaddr << USB_DEVADDRBIT);
    index = usb_hhub_ChkTblIndx1( ptr, devaddr);

    /* Hub number 0 */
    if( usb_shhub_Number[ptr->ip] == (uint16_t)0 )
    {

/* Condition compilation by the difference of the device's operating system */
        /* Check task status */
        err = USB_REF_TST(USB_HUB_TSK, &refinfo);
        if( err != USB_E_OK )
        {
            /* Is HCD task opened ? */
            USB_PRINTF1("REF_TSK USB_HUB_TSK Error (%ld)\n", err);
            while( 1 )
            {
                /* Do nothing */
            }
        }
        if( refinfo.tskstat != USB_TTS_DMT )
        {
            USB_PRINTF1("Already started USB_HUB_TSK (%ld)\n", refinfo.tskstat);
            /* >Yes no process */
        }

        USB_PRINTF0("*** Install HUB driver ***\n");

        if( refinfo.tskstat == USB_TTS_DMT )
        {
            /* Start HUB driver task */
            err = USB_ACT_TSK(USB_HUB_TSK);
            if( err != USB_E_OK )
            {
                USB_PRINTF1("ACT_TSK USB_HUB_TSK Error (%ld)\n", err);
                while( 1 )
                {
                    /* Do nothing */
                }
            }
        }


    }

    /* Hub number !0 */
    if( usb_shhub_Number[ptr->ip] != USB_MAXHUB )
    {
        usb_cpu_DelayXms((uint16_t)10);
        err = USB_PGET_BLK(USB_HUB_MPL, &p_blf);
        if( err == USB_E_OK )
        {
            mp = (USB_MGRINFO_t*)p_blf;
            mp->msghead = (USB_MH_t)USB_NULL;
            mp->msginfo = USB_MSG_HUB_START;
            mp->keyword = devaddr;

            mp->ipp     = ptr->ipp;
            mp->ip      = ptr->ip;

            /* Send message */
            err = USB_SND_MSG(USB_HUB_MBX, (USB_MSG_t*)p_blf);
            if( err != USB_E_OK )
            {
                USB_PRINTF1("### hHubOpen snd_msg error (%ld)\n", err);
                err2 = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)p_blf);
                if( err2 != USB_E_OK )
                {
                    USB_PRINTF1("### hHubOpen rel_blk error (%ld)\n", err2);
                }
            }
        }
        else
        {
            USB_PRINTF1("### hHubOpen pget_blk error (%ld)\n", err);
            while( 1 );
        }
        /* Pipe number set */
        usb_shhub_InfoData[ptr->ip][devaddr][3] = usb_ghhub_TmpEPTbl[ptr->ip][index];
        /* HUB downport status */
        usb_shhub_DownPort[ptr->ip][devaddr] = 0;
        /* Downport remotewakeup */
        usb_shhub_Remote[ptr->ip][devaddr] = 0;
        usb_ghhub_TmpEPTbl[ptr->ip][index+3] |= hubaddr;
        R_usb_hstd_SetPipeInfo(&usb_ghhub_DefEPTbl[ptr->ip][index], &usb_ghhub_TmpEPTbl[ptr->ip][index],
                                (uint16_t)USB_EPL);
        R_usb_hstd_SetPipeRegistration(ptr, (uint16_t*)&usb_ghhub_DefEPTbl[ptr->ip],
                                usb_ghhub_DefEPTbl[ptr->ip][index]);
        usb_shhub_Number[ptr->ip]++;
    }
}
/******************************************************************************
End of function R_usb_hhub_Open
******************************************************************************/

#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
extern  struct  usb_devinfo usb_devInfo[USB_DEVICENUM*USB_IFNUM*USB_CFGNUM];
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */

/******************************************************************************
Function Name   : R_usb_hhub_Close
Description     : HUB sys close
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t data2            : Not use
Return value    : USB_ER_t                  : Error Info
******************************************************************************/
void R_usb_hhub_Close(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t data2)
{
    USB_ER_t        err;
    uint16_t        md, i;
    USB_HCDREG_t    *driver;
    uint16_t        devaddr, index;

    err = USB_ERROR;
    for( i = 1; i <= usb_shhub_InfoData[ptr->ip][hubaddr][2]; i++ )
    {
        /* Now downport device search */
        devaddr = usb_hhub_GetCnnDevaddr( ptr, hubaddr, i);
        if( devaddr != 0 )
        {
            usb_hhub_SelectiveDetach(ptr, devaddr);
            for( md = 0; md < usb_ghstd_DeviceNum[ptr->ip]; md++ )
            {
                driver = &usb_ghstd_DeviceDrv[ptr->ip][md];
                if( devaddr == driver->devaddr )
                {
                    (*driver->devdetach)(ptr, driver->devaddr, (uint16_t)USB_NO_ARG);
                    /* Root port */
                    driver->rootport    = USB_NOPORT;
                    /* Device devaddress */
                    driver->devaddr     = USB_NODEVICE;
                    /* Device state */
                    driver->devstate    = USB_DETACHED;
                    /* Up-address clear */
                    usb_shhub_InfoData[ptr->ip][devaddr][0] = 0;
                    /* Up-port num clear */
                    usb_shhub_InfoData[ptr->ip][devaddr][1] = 0;
                    /* Port number clear */
                    usb_shhub_InfoData[ptr->ip][devaddr][2] = 0;
                    /* Pipe number clear */
                    usb_shhub_InfoData[ptr->ip][devaddr][3] = 0;
                }
            }
        }
    }

    usb_shhub_Number[ptr->ip]--;
    index = usb_hhub_ChkTblIndx2( ptr, hubaddr);

    /* Set pipe information */
    usb_shhub_InfoData[ptr->ip][hubaddr][0] = 0;    /* Up-address clear */
    usb_shhub_InfoData[ptr->ip][hubaddr][1] = 0;    /* Up-port num clear */
    usb_shhub_InfoData[ptr->ip][hubaddr][2] = 0;    /* Port number clear */
    usb_shhub_InfoData[ptr->ip][hubaddr][3] = 0;    /* Pipe number clear */
    usb_shhub_DownPort[ptr->ip][hubaddr]    = 0;
    usb_shhub_Remote[ptr->ip][hubaddr]      = 0;
    usb_ghhub_DefEPTbl[ptr->ip][index + 1]  = USB_NONE | USB_BFREOFF | USB_DBLBOFF
                                             | USB_CNTMDOFF | USB_NONE | USB_NONE | USB_NONE;
    usb_ghhub_DefEPTbl[ptr->ip][index + 3]  = USB_NONE;
    usb_ghhub_DefEPTbl[ptr->ip][index + 4]  = USB_NONE;
    usb_ghhub_TmpEPTbl[ptr->ip][index + 1]  = USB_NONE | USB_BFREOFF | USB_DBLBOFF
                                             | USB_CNTMDOFF | USB_NONE | USB_NONE | USB_NONE;
    usb_ghhub_TmpEPTbl[ptr->ip][index + 3]  = USB_NONE;
    usb_ghhub_TmpEPTbl[ptr->ip][index + 4]  = USB_NONE;

#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
    for( i = 0; i < USB_MAX_DEVICENUM; i++ )
    {
        if( hubaddr == usb_devInfo[i].devadr )
        {
            usb_devInfo[i].devadr = USB_NO;             /* Clear the member for the device address in usb_devInfo[] */
            usb_devInfo[i].isActDev = USB_NO;           /* Clear the member for the device state */
            usb_devInfo[i].interfaceClass = USB_NO;     /* Clear the member for the interface class */
        }
    }
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */

    if( (usb_shhub_Number[0] == 0) && (usb_shhub_Number[1] == 0) )
    {
        USB_PRINTF0("*** Release HUB driver ***\n");

        /* Stop HUB sys Task */
        usb_cpu_DelayXms((uint16_t)10);

        /* Stop HUB driver task */
        err = USB_TER_TSK(USB_HUB_TSK);
        if( err != USB_E_OK )
        {
            USB_PRINTF1("TER_TSK USB_HUB Error (%ld)\n", err);
            while( 1 )
            {
            }
        }
    }
}
/******************************************************************************
End of function R_usb_hhub_Close
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_InputStatus
Description     : HUB input status
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hhub_InputStatus(void)
{
    USB_UTR_t       *mess;
    USB_HCDREG_t    *driver;
    USB_ER_t        err;
    uint16_t        hubaddr, pipenum, index, devaddr, portnum;
    uint16_t        md, i, result;
    USB_CB_INFO_t   complete;
/* Condition compilation by the difference of useful function */
#ifdef USB_DEBUGPRINT_PP
    uint16_t        pdata1, pdata2;
#endif  /* USB_DEBUGPRINT_PP */
    USB_UTR_t       *ptr;

    while( 1 )
    {
        do
        {
            /* Receive message */
            err = USB_TRCV_MSG(USB_HUB_MBX, (USB_MSG_t**)&mess, (USB_TM_t)10000);
            if ( (err != USB_E_OK) && (err != USB_E_TMOUT) )
            {
                USB_PRINTF1("### hub_InputStatus rcv_msg error (%ld)\n", err);
            }
        } while( err != USB_E_OK );

        ptr         = mess;
        usb_shhub_Msg[ptr->ip]  = (USB_MGRINFO_t*)mess;
        hubaddr     = usb_shhub_Msg[ptr->ip]->keyword;
        devaddr     = usb_shhub_Msg[ptr->ip]->keyword;
        pipenum     = usb_shhub_Msg[ptr->ip]->keyword;

        switch( usb_shhub_Msg[ptr->ip]->msginfo )
        {
        case USB_MSG_HUB_START:
            /* Hub downport initialize */
            usb_hhub_InitDownPort(mess, hubaddr, (USB_CLSINFO_t*)USB_NULL);
            err = USB_REL_BLK(USB_HUB_MPL, (USB_MH_t)usb_shhub_Msg[ptr->ip]);
            if( err != USB_E_OK )
            {
                USB_PRINTF0("### HUB Class rel_blk error !\n");
            }
            break;
        case USB_MSG_HUB_SUBMITRESULT:
            /* HUB address search */
            hubaddr = usb_hhub_GetHubaddr( ptr, pipenum);
            switch( mess->status )
            {
            case USB_DATA_SHT:
                /* Continue */
            case USB_DATA_OK:   /* Direction is in then data receive end */
                /* HUB port connection */
                if( (usb_ghstd_MgrMode[mess->ip][0] == USB_DEFAULT)
                 || (usb_ghstd_MgrMode[mess->ip][1] == USB_DEFAULT) )
                {
                    err = USB_SND_MSG( USB_HUB_MBX, (USB_MSG_t*)usb_shhub_Msg[ptr->ip] );
                    if( err != USB_E_OK )
                    {
                        USB_PRINTF1("### hub_InputStatus snd_msg error (%ld)\n", err);
                        usb_cpu_DelayXms((uint16_t)10);
                    }
                }
                else
                {
                    for( i = 1; i <= usb_shhub_InfoData[ptr->ip][hubaddr][2]; i++ )
                    {
                        if( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][0] & USB_BITSET(i)) != 0 )
                        {
                            USB_PRINTF1(" *** HUB port %d \t", i);
                            usb_cpu_DelayXms((uint16_t)20);
                            R_usb_hhub_GetPortInformation( ptr, hubaddr, i, usb_cstd_ClassTransResult);
/* Condition compilation by the difference of useful function */
#ifdef USB_DEBUGPRINT_PP
                            pdata1 = (uint16_t)(((uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][1]) << 8) 
                                    + usb_ghhub_Data[ptr->ip][hubaddr][0]);
                            pdata2 = (uint16_t)(((uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][3]) << 8)
                                    + usb_ghhub_Data[ptr->ip][hubaddr][2]);
                            USB_PRINTF2(" [status/change] : 0x%04x, 0x%04x\n", pdata1, pdata2);
#endif  /* USB_DEBUGPRINT_PP */
                            if( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][2] & (uint8_t)0x01) != 0 )
                            {
                                /* C_PORT_CONNECTION */
                                usb_hhub_PortClrFeature( ptr, hubaddr, i, (uint16_t)USB_HUB_C_PORT_CONNECTION,
                                                        usb_cstd_ClassTransResult);
                                if( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][0] & (uint8_t)0x01) != 0 )
                                {
                                    /* PORT_CONNECTION */
                                    if( (uint16_t)(usb_shhub_DownPort[ptr->ip][hubaddr] & USB_BITSET(i)) == 0 )
                                    {
                                        usb_hhub_NewConnect(mess, hubaddr, i, (USB_CLSINFO_t*)USB_NULL);
                                    }
                                }
                                else
                                {
                                    /* Now downport device search */
                                    devaddr = usb_hhub_GetCnnDevaddr( ptr, hubaddr, i);
                                    if( devaddr != 0 )
                                    {
                                        usb_hhub_PortDetach(mess, hubaddr, i);
                                        USB_PRINTF1(" Hubport disconnect address%d\n", devaddr);
                                        /* Up-address clear */
                                        usb_shhub_InfoData[ptr->ip][devaddr][0] = 0;
                                        /* Up-port num clear */
                                        usb_shhub_InfoData[ptr->ip][devaddr][1] = 0;
                                        /* Port number clear */
                                        usb_shhub_InfoData[ptr->ip][devaddr][2] = 0;
                                        /* Pipe number clear */
                                        usb_shhub_InfoData[ptr->ip][devaddr][3] = 0;
                                    }
                                }
                            }
                            else
                            {
                                /* Now downport device search */
                                devaddr = usb_hhub_GetCnnDevaddr( ptr, hubaddr, i);
                                if( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][2] & (uint8_t)0x02) != 0 )
                                {
                                    /* C_PORT_ENABLE */
                                    USB_PRINTF1(" Hubport error address%d\n",devaddr);
                                    usb_hhub_PortClrFeature( ptr, hubaddr, i, (uint16_t)USB_HUB_C_PORT_ENABLE,
                                                            usb_cstd_ClassTransResult);
                                }
                                else if( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][2] & (uint8_t)0x04) != 0 )
                                {
                                    /* C_PORT_SUSPEND */
                                    USB_PRINTF1("Hubport suspend(resume complete)addr%d\n", devaddr);
                                    /* Change HUB down port device status */
                                    R_usb_hstd_MgrChangeDeviceState(ptr, (USB_CB_t)&usb_cstd_DummyFunction,
                                                                    USB_DO_GLOBAL_RESUME, devaddr);
                                }
                                else if( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][2] & (uint8_t)0x08) != 0 )
                                {
                                    /* C_PORT_OVER_CURRENT */
                                    USB_PRINTF1(" Hubport over current address%d\n", devaddr);
                                    usb_hhub_PortClrFeature( ptr, hubaddr, i, (uint16_t)USB_HUB_C_PORT_OVER_CURRENT,
                                                            usb_cstd_ClassTransResult);
                                }
                                else if( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][2] & (uint8_t)0x10) != 0 )
                                {
                                    /* C_PORT_RESET */
                                    USB_PRINTF1("Hubport reset(reset complete)address%d\n", devaddr);
                                    usb_hhub_PortClrFeature( ptr, hubaddr, i, (uint16_t)USB_HUB_C_PORT_RESET,
                                                            usb_cstd_ClassTransResult);
                                }
                                else
                                {
                                    /* Nothing */
                                }
                            }
                        }
                    }
                    /* Next command check */
                    usb_hhub_TransStart( ptr, hubaddr, (uint32_t)1, &usb_ghhub_Data[ptr->ip][hubaddr][0],
                                        &usb_hhub_TransResult);
                }
                break;
            case USB_DATA_STALL:
                USB_PRINTF0("*** Data Read error. ( STALL ) !\n");
                /* CLEAR_FEATURE */
                usb_hstd_ClearStall(mess, pipenum);
                break;
            case USB_DATA_OVR:
                USB_PRINTF0("### receiver over. !\n");
                break;
            case USB_DATA_STOP:
                USB_PRINTF0("### receiver stop. !\n");
                break;
            default:
                USB_PRINTF0("### HUB Class Data Read error !\n");
                break;
            }
            break;
        default:
            hubaddr = usb_shhub_InfoData[ptr->ip][devaddr][0];
            portnum = usb_shhub_InfoData[ptr->ip][devaddr][1];
            complete = ((USB_HCDINFO_t*)usb_shhub_Msg[ptr->ip])->complete;
            if( hubaddr == 0 )
            {
                USB_PRINTF0("### error address \n");
            }
            else
            {
                switch( usb_shhub_Msg[ptr->ip]->msginfo )
                {
                case USB_MSG_HCD_DETACH:
                case USB_MSG_HCD_DETACH_MGR:
                    USB_PRINTF3(" HUBaddr%d,port%d detach (address%d)\n", hubaddr, portnum, devaddr);
                    /* Clear Feature */
                    usb_hhub_PortClrFeature( ptr, hubaddr, portnum, (uint16_t)USB_HUB_PORT_ENABLE,
                                            usb_cstd_ClassTransResult);
                    /* HUB down port disconnect */
                    usb_hhub_PortDetach(mess, hubaddr, portnum);
                    (*complete)(mess, (uint16_t)USB_NO_ARG,(uint16_t)USB_NO_ARG);
                    break;
                case USB_MSG_HCD_VBON:
                case USB_MSG_HCD_ATTACH:
                    USB_PRINTF3(" HUBaddr%d,port%d attach (address%d)\n", hubaddr, portnum, devaddr);
                    /* Get Status */
                    R_usb_hhub_GetPortInformation( ptr, hubaddr, portnum, usb_cstd_ClassTransResult);
/* Condition compilation by the difference of useful function */
#ifdef USB_DEBUGPRINT_PP
                    pdata1 = (uint16_t)(((uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][1]) << 8)
                            + usb_ghhub_Data[ptr->ip][hubaddr][0]);
                    pdata2 = (uint16_t)(((uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][3]) << 8)
                            + usb_ghhub_Data[ptr->ip][hubaddr][2]);
                    USB_PRINTF2(" [status/change] : 0x%04x, 0x%04x\n", pdata1, pdata2);
#endif  /* USB_DEBUGPRINT_PP */
                    if( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][0] & (uint8_t)0x02) != 0 )
                    {
                        /* PORT_ENABLE */
                        USB_PRINTF3("*** HUBaddr%d,port%d already attach (address%d)\n", hubaddr, portnum, devaddr);
                    }
                    else
                    {
                        pipenum = usb_shhub_InfoData[ptr->ip][hubaddr][3];
                        index = usb_hhub_ChkTblIndx3(pipenum);
                        R_usb_hstd_TransferEnd(mess, usb_ghhub_DefEPTbl[ptr->ip][index], (uint16_t)USB_DATA_STOP);
                        /* Stop interrupt pipe */
                        usb_hhub_TransWaitTmo((uint16_t)1000);
                        /* Set Feature */
                        usb_hhub_PortSetFeature( ptr, hubaddr, portnum, (uint16_t)USB_HUB_PORT_POWER,
                                                usb_cstd_ClassTransResult);
                        result = usb_hhub_PortAttach(hubaddr, portnum, (USB_CLSINFO_t*)USB_NULL);
                        if( result != USB_CONFIGURED )
                        {
                            /* Up-hubaddr clear */
                            usb_shhub_InfoData[ptr->ip][devaddr][0] = 0;
                            /* Up-hubport clear */
                            usb_shhub_InfoData[ptr->ip][devaddr][1] = 0;
                            usb_hhub_PortReset( ptr, hubaddr, portnum, (USB_CLSINFO_t*)USB_NULL);
                            usb_hhub_PortSetFeature( ptr, hubaddr, portnum, (uint16_t)USB_HUB_PORT_SUSPEND,
                                                    usb_cstd_ClassTransResult);
                            usb_hhub_PortDetach(mess, hubaddr, portnum);
                        }
                        /* Next command check */
                        usb_hhub_TransStart( ptr, hubaddr, (uint32_t)1, &usb_ghhub_Data[ptr->ip][hubaddr][0],
                                            &usb_hhub_TransResult);
                    }
                    (*complete)(mess, (uint16_t)USB_NO_ARG,(uint16_t)USB_NO_ARG);
                    break;
                case USB_MSG_HCD_ATTACH_MGR:
                    pipenum = usb_shhub_InfoData[ptr->ip][hubaddr][3];
                    index = usb_hhub_ChkTblIndx3(pipenum);
                    R_usb_hstd_TransferEnd(mess, usb_ghhub_DefEPTbl[ptr->ip][index], (uint16_t)USB_DATA_STOP);
                    /* Stop interrupt pipe */
                    usb_hhub_TransWaitTmo((uint16_t)1000);
                    /* Set Feature */
                    usb_hhub_PortSetFeature( ptr, hubaddr, portnum, (uint16_t)USB_HUB_PORT_POWER,
                                            usb_cstd_ClassTransResult);
                    result = usb_hhub_PortAttach(hubaddr, portnum, (USB_CLSINFO_t*)USB_NULL);
                    if( result != USB_CONFIGURED )
                    {
                        /* Up-hubaddr clear */
                        usb_shhub_InfoData[ptr->ip][devaddr][0] = 0;
                        /* Up-hubport clear */
                        usb_shhub_InfoData[ptr->ip][devaddr][1] = 0;
                        usb_hhub_PortReset( ptr, hubaddr, portnum, (USB_CLSINFO_t*)USB_NULL);
                        usb_hhub_PortSetFeature( ptr, hubaddr, portnum, (uint16_t)USB_HUB_PORT_SUSPEND,
                                                usb_cstd_ClassTransResult);
                        usb_hhub_PortDetach(mess, hubaddr, portnum);
                    }
                    /* Next command check */
                    usb_hhub_TransStart( ptr, hubaddr, (uint32_t)1, &usb_ghhub_Data[ptr->ip][hubaddr][0],
                                        &usb_hhub_TransResult);

                    (*complete)(mess, (uint16_t)USB_NO_ARG, (uint16_t)USB_NO_ARG);
                    break;
                case USB_MSG_HCD_USBRESET:
                    USB_PRINTF3(" HUBaddr%d,port%d UsbReset (address%d)\n", hubaddr, portnum, devaddr);
                    /* Set Feature */
                    usb_hhub_PortSetFeature( ptr, hubaddr, portnum, (uint16_t)USB_HUB_PORT_RESET,
                                            usb_cstd_ClassTransResult);

                    (*complete)(mess, (uint16_t)USB_NO_ARG, (uint16_t)USB_NO_ARG);
                    break;
                case USB_MSG_HCD_REMOTE:
                case USB_MSG_HCD_SUSPEND:
                    USB_PRINTF3(" HUBaddr%d,port%d suspend (address%d)\n", hubaddr, portnum, devaddr);
                    /* Set Feature */
                    usb_hhub_PortSetFeature( ptr, hubaddr, portnum, (uint16_t)USB_HUB_PORT_SUSPEND,
                                            usb_cstd_ClassTransResult);
                    for( md = 0; md < usb_ghstd_DeviceNum[mess->ip]; md++ )
                    {
                        driver = &usb_ghstd_DeviceDrv[mess->ip][md];
                        if( devaddr == driver->devaddr )
                        {
                            (*driver->devsuspend)(mess, (uint16_t)USB_NO_ARG, (uint16_t)USB_NO_ARG);
                        }
                    }
                    if( complete )
                    {
                        (*complete)(mess, (uint16_t)USB_NO_ARG, (uint16_t)USB_NO_ARG);
                    }
                    break;
                case USB_MSG_HCD_RESUME:
                    USB_PRINTF3(" HUBaddr%d,port%d resume (address%d)\n", hubaddr, portnum, devaddr);
                    pipenum = usb_shhub_InfoData[ptr->ip][hubaddr][3];
                    index = usb_hhub_ChkTblIndx3(pipenum);
                    R_usb_hstd_TransferEnd(mess, usb_ghhub_DefEPTbl[ptr->ip][index], (uint16_t)USB_DATA_STOP);
                    /* Stop interrupt pipe */
                    usb_hhub_TransWaitTmo((uint16_t)1000);
                    /* Clear Feature */
                    usb_hhub_PortClrFeature( ptr, hubaddr, portnum, (uint16_t)USB_HUB_PORT_SUSPEND,
                                            usb_cstd_ClassTransResult);
                    if( (uint16_t)(usb_shhub_DownPort[ptr->ip][hubaddr] & USB_BITSET(portnum)) != 0 )
                    {
                        do
                        {
                            usb_cpu_DelayXms((uint16_t)20);
                            R_usb_hhub_GetPortInformation( ptr, hubaddr, portnum, usb_cstd_ClassTransResult);
                        }
                        while( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][2] & (uint8_t)0x04) != 0x04 );
                    }
                    /* HUB downport status */
                    usb_shhub_Remote[ptr->ip][hubaddr] &= (uint16_t)(~USB_BITSET(portnum));
                    /* Clear Feature */
                    usb_hhub_PortClrFeature( ptr, hubaddr, portnum, (uint16_t)USB_HUB_C_PORT_SUSPEND,
                                            usb_cstd_ClassTransResult);
                    for( md = 0; md < usb_ghstd_DeviceNum[mess->ip]; md++ )
                    {
                        driver = &usb_ghstd_DeviceDrv[mess->ip][md];
                        if( devaddr == driver->devaddr )
                        {
                            (*driver->devresume)(mess, (uint16_t)USB_NO_ARG, (uint16_t)USB_NO_ARG);
                        }
                    }
                    usb_hhub_TransStart( ptr, hubaddr, (uint32_t)1, &usb_ghhub_Data[ptr->ip][hubaddr][0],
                                        &usb_hhub_TransResult);
                    if( complete )
                    {
                        (*complete)(mess, (uint16_t)USB_NO_ARG, (uint16_t)USB_NO_ARG);
                    }
                    break;
                case USB_MSG_HCD_VBOFF:
                    USB_PRINTF3(" HUBaddr%d,port%d VBoff (address%d)\n", hubaddr, portnum, devaddr);
                    usb_hhub_PortClrFeature( ptr, hubaddr, portnum, (uint16_t)USB_HUB_PORT_ENABLE,
                                            usb_cstd_ClassTransResult);
                    (*complete)(mess, (uint16_t)USB_NO_ARG, (uint16_t)USB_NO_ARG);
                    break;
                default:
                    break;
                }
            }
            err = USB_REL_BLK(USB_HUB_MPL, (USB_MH_t)usb_shhub_Msg[ptr->ip]);
            if( err != USB_E_OK )
            {
                USB_PRINTF0("### HUB Class rel_blk error !\n");
            }
            break;
        }
    }
}
/******************************************************************************
End of function usb_hhub_InputStatus
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_InitDownPort
Description     : Down port initialized
Arguments       : uint16_t hubaddr          : hub address
                : USB_CLSINFO_t *mess
Return value    : none
******************************************************************************/
void usb_hhub_InitDownPort(USB_UTR_t *ptr, uint16_t hubaddr, USB_CLSINFO_t *mess)
{
    uint16_t        i;
/* Condition compilation by the difference of useful function */
#ifdef USB_DEBUGPRINT_PP
    uint16_t        pdata1, pdata2;
#endif  /* USB_DEBUGPRINT_PP */

    /* HUB port power */
    for( i = 1; i <= usb_shhub_InfoData[ptr->ip][hubaddr][2]; i++ )
    {
        usb_hhub_PortSetFeature( ptr, hubaddr, i, (uint16_t)USB_HUB_PORT_POWER, usb_cstd_ClassTransResult);
    }
    /* HUB downport initialize */
    for( i = 1; i <= usb_shhub_InfoData[ptr->ip][hubaddr][2]; i++ )
    {
        usb_hhub_PortClrFeature( ptr, hubaddr, i, (uint16_t)USB_HUB_C_PORT_CONNECTION, usb_cstd_ClassTransResult);
    }

    usb_cpu_DelayXms((uint16_t)20);

    /* Check already connected device */
    for( i = 1; i <= usb_shhub_InfoData[ptr->ip][hubaddr][2]; i++ )
    {
        USB_PRINTF2(" *** address %d downport %d \t", hubaddr, i);
        R_usb_hhub_GetPortInformation( ptr, hubaddr, i, usb_cstd_ClassTransResult);
/* Condition compilation by the difference of useful function */
#ifdef USB_DEBUGPRINT_PP
        pdata1 = (uint16_t)(((uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][1]) << 8)
                + usb_ghhub_Data[ptr->ip][hubaddr][0]);
        pdata2 = (uint16_t)(((uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][3]) << 8)
                + usb_ghhub_Data[ptr->ip][hubaddr][2]);
        USB_PRINTF2(" [status/change] : 0x%04x, 0x%04x\n", pdata1, pdata2);
#endif  /* USB_DEBUGPRINT_PP */
        if( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][0] & (uint8_t)0x01) != 0 )
        {
            /* PORT_CONNECTION */
            usb_hhub_NewConnect(ptr, hubaddr, i, (USB_CLSINFO_t*)USB_NULL);
        }
    }
    usb_hhub_TransStart( ptr, hubaddr, (uint32_t)1, &usb_ghhub_Data[ptr->ip][hubaddr][0], &usb_hhub_TransResult);
}
/******************************************************************************
End of function usb_hhub_InitDownPort
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_NewConnect
Description     : new connect
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t portnum          : down port number
                : USB_CLSINFO_t *mess
Return value    : none
******************************************************************************/
void usb_hhub_NewConnect(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t portnum, USB_CLSINFO_t *mess)
{
    uint16_t        devaddr, result;

    /* New downport device search */
    devaddr = usb_hhub_GetNewDevaddr(ptr);
    if( devaddr != 0 )
    {
        USB_PRINTF1(" Hubport connect address%d\n", devaddr);
        /* Up-hubaddr set */
        usb_shhub_InfoData[ptr->ip][devaddr][0] = hubaddr;
        /* Up-hubport set */
        usb_shhub_InfoData[ptr->ip][devaddr][1] = portnum;
        result = usb_hhub_PortAttach(hubaddr, portnum, (USB_CLSINFO_t*)ptr);
        if( result != USB_CONFIGURED )
        {
            usb_hhub_PortReset( ptr, hubaddr, portnum, (USB_CLSINFO_t*)NULL);
            usb_hhub_PortSetFeature( ptr, hubaddr, portnum, (uint16_t)USB_HUB_PORT_SUSPEND, usb_cstd_ClassTransResult);
            usb_hhub_PortDetach(ptr, hubaddr, portnum);
            /* Up-hubaddr clear */
            usb_shhub_InfoData[ptr->ip][devaddr][0] = 0;
            /* Up-hubport clear */
            usb_shhub_InfoData[ptr->ip][devaddr][1] = 0;
        }
    }
    else
    {
        USB_PRINTF0("### device count over !\n");
    }
}
/******************************************************************************
End of function usb_hhub_NewConnect
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_PortAttach
Description     : port attach
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t portnum          : down port number
                : USB_CLSINFO_t *mess
Return value    : uint16_t                  : Manager Mode
******************************************************************************/
uint16_t usb_hhub_PortAttach(uint16_t hubaddr, uint16_t portnum, USB_CLSINFO_t *mess)
{
    uint16_t        rootport, devaddr;
    uint16_t        hpphub, hubport, buffer;
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
    uint16_t        elseport;
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
    USB_UTR_t       *ptr;

    ptr = (USB_UTR_t *)mess;

    rootport = usb_hstd_GetRootport(ptr, (uint16_t)(hubaddr << USB_DEVADDRBIT));
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
    elseport = 0;
    if( rootport == 0 )
    {
        elseport = 1;
    }
    do
    {
        usb_cpu_DelayXms((uint16_t)10);
    }
    while( usb_ghstd_MgrMode[ptr->ip][elseport] == USB_DEFAULT );
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */

    /* USB reset */
    usb_hhub_PortReset(ptr, hubaddr, portnum, (USB_CLSINFO_t*)USB_NULL);

    /* Device Enumeration */
    switch( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][1] & (uint8_t)0x06) )
    {
    case 0x00:
        usb_ghstd_DeviceSpeed[ptr->ip] = USB_FSCONNECT;
        USB_PRINTF0(" Full-Speed Device\n");
        break;
    case 0x02:
        usb_ghstd_DeviceSpeed[ptr->ip] = USB_LSCONNECT;
        USB_PRINTF0(" Low-Speed Device\n");
        break;
    case 0x04:
        usb_ghstd_DeviceSpeed[ptr->ip] = USB_HSCONNECT;
        USB_PRINTF0(" Hi-Speed Device\n");
        break;
    default:
        usb_ghstd_DeviceSpeed[ptr->ip] = USB_NOCONNECT;
        USB_PRINTF0(" Detach Detached\n");
        break;
    }
    /* Now downport device search */
    devaddr = usb_hhub_GetCnnDevaddr(ptr, hubaddr, portnum);
    usb_ghstd_DeviceAddr[ptr->ip] = devaddr;
    devaddr = (uint16_t)(devaddr << USB_DEVADDRBIT);
    usb_ghstd_MgrMode[ptr->ip][rootport] = USB_DEFAULT;
    if( devaddr != 0 )
    {
        usb_hstd_SetHubPort(ptr, devaddr, (uint16_t)(hubaddr << 11), (uint16_t)(portnum << 8));
        /* Get DEVADDR register */
        buffer = usb_hreg_read_devadd( ptr, devaddr );
        do
        {
            hpphub  = (uint16_t)(buffer & USB_UPPHUB);
            hubport = (uint16_t)(buffer & USB_HUBPORT);
            devaddr = (uint16_t)(hpphub << 1);
            /* Get DEVADDR register */
            buffer = usb_hreg_read_devadd( ptr, devaddr );
        }
        while( ((buffer & USB_USBSPD) != USB_HSCONNECT) && (devaddr != USB_DEVICE_0) );
        usb_hstd_SetDevAddr(ptr, (uint16_t)USB_DEVICE_0, usb_ghstd_DeviceSpeed[ptr->ip], rootport);
        usb_hstd_SetHubPort(ptr, (uint16_t)USB_DEVICE_0, hpphub,hubport);
        usb_hstd_SetHubPort(ptr, (uint16_t)(usb_ghstd_DeviceAddr[ptr->ip] << USB_DEVADDRBIT), hpphub, hubport);
        usb_ghstd_EnumSeq[ptr->ip] = 0;
        if( usb_ghstd_DeviceSpeed[ptr->ip] != USB_NOCONNECT )
        {
            (*usb_ghstd_EnumarationProcess[0])(ptr, (uint16_t)USB_DEVICE_0, (uint16_t)0);
            while( 1 )
            {
                usb_cpu_DelayXms((uint16_t)10);
                if( (usb_ghstd_MgrMode[ptr->ip][rootport] == USB_CONFIGURED)
                 || (usb_ghstd_MgrMode[ptr->ip][rootport] != USB_DEFAULT) )
                {
                    /* HUB downport status */
                    usb_shhub_DownPort[ptr->ip][hubaddr] |= USB_BITSET(portnum);
                    return (usb_ghstd_MgrMode[ptr->ip][rootport]);
                }
            }
        }
    }
    else
    {
        USB_PRINTF0("### device count over !\n");
        return (usb_ghstd_MgrMode[ptr->ip][rootport]);
    }
    return USB_DETACHED;
}
/******************************************************************************
End of function usb_hhub_PortAttach
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_PortReset
Description     : HUB down port USB-reset request
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t portnum          : down port number
                : USB_CLSINFO_t *mess
Return value    : none
******************************************************************************/
void usb_hhub_PortReset(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t portnum, USB_CLSINFO_t *mess)
{
    usb_cpu_DelayXms((uint16_t)100);
    /* Hub port SetFeature */
    usb_hhub_PortSetFeature( ptr, hubaddr, portnum, (uint16_t)USB_HUB_PORT_RESET, usb_cstd_ClassTransResult);
    usb_cpu_DelayXms((uint16_t)50);
    do
    {
        usb_cpu_DelayXms((uint16_t)10);
        /* GetStatus request */
        R_usb_hhub_GetPortInformation(ptr, hubaddr, portnum, usb_cstd_ClassTransResult);
    } while( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][2] & (uint8_t)0x10) != 0x10 );

    usb_cpu_DelayXms((uint16_t)20);
    /* Hub port ClearFeature */
    usb_hhub_PortClrFeature(ptr, hubaddr, portnum, (uint16_t)USB_HUB_C_PORT_RESET, usb_cstd_ClassTransResult);
}
/******************************************************************************
End of function usb_hhub_PortReset
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_ReqTransWait
Description     : Request result
Arguments       : none
Return value    : uint16_t                  : DONE/ERROR
******************************************************************************/
uint16_t usb_hhub_ReqTransWait(void)
{
    USB_ER_t    err;

    err = usb_cstd_ClassTransWaitTmo((uint16_t)3000);
    if( err == USB_DATA_TMO )
    {
        /* Timeout */
        USB_PRINTF0("*** usb_hhub_ReqTransWait Timeout error !\n");
        return USB_ERROR;
    }
    else if( err == USB_DATA_STALL )
    {
        /* STALL */
        USB_PRINTF0("*** usb_hhub_ReqTransWait STALL !\n");
        return USB_ERROR;
    }
    else if( err != USB_CTRL_END )
    {
        USB_PRINTF0("*** usb_hhub_ReqTransWait error !\n");
        return USB_ERROR;
    }
    else
    {
    }
    return USB_DONE;
}
/******************************************************************************
End of function usb_hhub_ReqTransWait
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_ProcessResult
Description     : Process result
Arguments       : uint16_t data                 : status
                : uint16_t Dummy                : Not use
Return value    : none
******************************************************************************/
void usb_hhub_ProcessResult(USB_UTR_t *ptr, uint16_t data, uint16_t Dummy)
{
    USB_MH_t        p_blf;
    USB_ER_t        err, err2;
    USB_UTR_t       *up;

    /* Get mem pool blk */
    err = USB_PGET_BLK(USB_HUB_MPL, &p_blf);
    if( err == USB_E_OK )
    {
        up = (USB_UTR_t*)p_blf;
        up->msghead = (USB_MH_t)USB_NULL;
        up->msginfo = (uint16_t)0u;
        up->keyword = (uint16_t)0u;
        up->status  = data;

        up->ipp = ptr->ipp;
        up->ip  = ptr->ip;

        /* Send message */
        err = USB_SND_MSG(USB_HUB_MBX, (USB_MSG_t*)p_blf);
        if( err != USB_E_OK )
        {
            USB_PRINTF1("### hub_ProcessResult snd_msg error (%ld)\n", err);
            err2 = USB_REL_BLK(USB_HUB_MPL, (USB_MH_t)p_blf);
            if( err2 != USB_E_OK )
            {
                USB_PRINTF1("### hub_ProcessResult rel_blk error (%ld)\n", err2);
            }
        }
    }
    else
    {
        USB_PRINTF1("### hub_ProcessResult pget_blk error (%ld)\n", err);
        while( 1 );
    }
}
/******************************************************************************
End of function usb_hhub_ProcessResult
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_TransWaitTmo
Description     : receive message
Arguments       : uint16_t tmo                  : time out
Return value    : uint16_t                      : status
******************************************************************************/
uint16_t usb_hhub_TransWaitTmo(uint16_t tmo)
{
    USB_UTR_t       *mess;
    USB_ER_t        err;

    /* Receive message */
    err = USB_TRCV_MSG(USB_HUB_MBX, (USB_MSG_t**)&mess, (USB_TM_t)tmo);
    if( err == USB_E_TMOUT )
    {
        return (USB_DATA_TMO);
    }
    if( err != USB_E_OK )
    {
        USB_PRINTF1("### hub_TransWaitTmo trcv_msg error (%ld)\n", err);
        return (USB_DATA_ERR);
    }
    return (mess->status);
}
/******************************************************************************
End of function usb_hhub_TransWaitTmo
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_ProcessWaitTmo
Description     : receive message
Arguments       : uint16_t tmo                  : time out
Return value    : uint16_t                      : status
******************************************************************************/
uint16_t usb_hhub_ProcessWaitTmo(uint16_t tmo)
{
    USB_UTR_t       *mess;
    USB_ER_t        err;
    uint16_t        status;

    /* Receive message */
    err = USB_TRCV_MSG(USB_HUB_MBX, (USB_MSG_t**)&mess, (USB_TM_t)tmo);
    if( err == USB_E_TMOUT )
    {
        return (USB_ERROR);
    }
    if( err != USB_E_OK )
    {
        USB_PRINTF1("### hub_ProcessWaitTmo tcv_msg error (%ld)\n", err);
        return (USB_ERROR);
    }
    status = mess->status;
    /* Release block */
    err = USB_REL_BLK(USB_HUB_MPL, (USB_MH_t)mess);
    if( err != USB_E_OK )
    {
        USB_PRINTF2("### HUB Class Process (%x) rel_blk error (%ld)\n", status, err);
        return (USB_ERROR);
    }
    return (status);
}
/******************************************************************************
 End of function usb_hhub_ProcessWaitTmo
 ******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_ChangeState
Description     : Change State
Arguments       : uint16_t devaddr              : device address
                : uint16_t msginfo              : device state
                : USB_CB_INFO_t complete       : callback function
Return value    : USB_ER_t                     : error info
******************************************************************************/
USB_ER_t usb_hhub_ChangeState(USB_UTR_t *ptr, uint16_t devaddr, uint16_t msginfo, USB_CB_INFO_t complete)
{
    USB_MH_t            p_blf;
    USB_ER_t            err, err2;
    USB_HCDINFO_t       *hp;

    /* Get mem pool blk */
    err = USB_PGET_BLK(USB_HUB_MPL, &p_blf);
    if( err == USB_E_OK )
    {
        USB_PRINTF2("*** devaddr%d : msginfo=%d ***\n", devaddr, msginfo);
        hp  = (USB_HCDINFO_t*)p_blf;
        hp->msghead     = (USB_MH_t)USB_NULL;
        hp->msginfo     = msginfo;
        hp->keyword     = devaddr;
        hp->complete    = complete;

        hp->ipp         = ptr->ipp;
        hp->ip          = ptr->ip;

        /* Send message */
        err = USB_SND_MSG(USB_HUB_MBX, (USB_MSG_t*)p_blf);
        if( err != USB_E_OK )
        {
            USB_PRINTF1("### hub_ChangeState snd_msg error (%ld)\n", err);
            err2 = USB_REL_BLK(USB_HUB_MPL, (USB_MH_t)p_blf);
            if( err2 != USB_E_OK )
            {
                USB_PRINTF1("### hub_ChangeState rel_blk error (%ld)\n", err2);
            }
        }
    }
    else
    {
        USB_PRINTF1("### hub_ChangeState pget_blk error (%ld)\n", err);
        while( 1 );
    }
    return err;
}
/******************************************************************************
End of function usb_hhub_ChangeState
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_ChangeState
Description     : Change HUB down port device status
Arguments       : USB_CB_INFO_t complete        : 
                : uint16_t msginfo              : device state
                : uint16_t devaddr              : device address
Return value    : USB_ER_t                      : error info
******************************************************************************/
USB_ER_t R_usb_hhub_ChangeDeviceState(USB_UTR_t *ptr, USB_CB_INFO_t complete, uint16_t msginfo, uint16_t devaddr)
{
    USB_MH_t            p_blf;
    USB_ER_t            err, err2;
    USB_HCDINFO_t       *hp;

    /* Get mem pool blk */
    err = USB_PGET_BLK(USB_HUB_MPL, &p_blf);
    if( err == USB_E_OK )
    {
        USB_PRINTF2("*** devaddr%d : msginfo=%d ***\n", devaddr, msginfo);
        hp  = (USB_HCDINFO_t*)p_blf;
        hp->msghead     = (USB_MH_t)USB_NULL;
        hp->msginfo     = msginfo;
        hp->keyword     = devaddr;
        hp->complete    = complete;

        hp->ipp         = ptr->ipp;
        hp->ip          = ptr->ip;

        /* Send message */
        err = USB_SND_MSG(USB_HUB_MBX, (USB_MSG_t*)p_blf);
        if( err != USB_E_OK )
        {
            USB_PRINTF1("### hHubChangeDeviceState snd_msg error (%ld)\n", err);
            err2 = USB_REL_BLK(USB_HUB_MPL, (USB_MH_t)p_blf);
            if( err2 != USB_E_OK )
            {
                USB_PRINTF1("### hHubChangeDeviceState rel_blk error (%ld)\n", err2);
            }
        }
    }
    else
    {
        USB_PRINTF1("### hHubChangeDeviceState pget_blk error (%ld)\n", err);
        while( 1 );
    }
    return err;
}
/******************************************************************************
End of function usb_hhub_ChangeState
******************************************************************************/
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */

/******************************************************************************
End  Of File
******************************************************************************/
