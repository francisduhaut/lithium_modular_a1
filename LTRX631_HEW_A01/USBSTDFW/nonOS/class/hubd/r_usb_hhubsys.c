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
* File Name    : r_usb_hHubsys.c
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB Host Hub system code
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
******************************************************************************/

/* $Id: r_usb_hhubsys.c 162 2012-05-21 10:20:32Z ssaek $ */

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
#define USB_HUB_QOVR            (uint16_t)0xFFFE

/******************************************************************************
External variables and functions
******************************************************************************/
extern void (*usb_ghstd_EnumarationProcess[])(USB_UTR_t *, uint16_t,uint16_t);
void        usb_hstd_DeviceDescripInfo(void);
void        usb_hstd_ConfigDescripInfo(void);
uint16_t    usb_hreg_read_devadd( USB_UTR_t *ptr, uint16_t devadr );


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
static  void        usb_hhub_InitDownPort(USB_UTR_t *ptr, uint16_t hubaddr, USB_CLSINFO_t *mess);
static  void        usb_hhub_NewConnect(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t portnum, USB_CLSINFO_t *mess);
static  uint16_t    usb_hhub_PortAttach(uint16_t hubaddr, uint16_t portnum, USB_CLSINFO_t *mess);
static  void        usb_hhub_PortReset(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t portnum, USB_CLSINFO_t *mess);
static  uint16_t    usb_hhub_ReqTransWait(void);
static  void        usb_hhub_Enumeration(USB_CLSINFO_t *mess);
static  void        usb_hhub_SubmitResult(USB_CLSINFO_t *mess, uint16_t status);
static  void        usb_hhub_Ivent(USB_CLSINFO_t *mess);
static  void        usb_hhub_SpecifiedPath(USB_CLSINFO_t *mess);
static  void        usb_hhub_SpecifiedPathWait(USB_CLSINFO_t *mess, uint16_t times);
static  void        usb_hhub_CheckResult(USB_UTR_t *mess, uint16_t, uint16_t);
static  void        usb_hhub_CheckRequest(USB_UTR_t *ptr, uint16_t result);
static  void        usb_hhub_ClearStallResult(USB_UTR_t *mess, uint16_t, uint16_t);
static  uint16_t    usb_hhub_RequestResult(uint16_t checkerr);

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

uint16_t        usb_hhub_ReqTransWait(void);

/* Condition compilation by the difference of USB function */
#if USB_NUM_USBIP == 2
uint16_t        usb_shhub_ClassSeq[USB_NUM_USBIP]       = { USB_SEQ_0, USB_SEQ_0 };
uint16_t        usb_shhub_InitSeq[USB_NUM_USBIP]        = { USB_SEQ_0, USB_SEQ_0 };
uint16_t        usb_shhub_InitPort[USB_NUM_USBIP]       = { USB_HUB_P1, USB_HUB_P1 };
uint16_t        usb_shhub_IventSeq[USB_NUM_USBIP]       = { USB_SEQ_0, USB_SEQ_0 };
uint16_t        usb_shhub_IventPort[USB_NUM_USBIP]      = { USB_HUB_P1, USB_HUB_P1 };
uint16_t        usb_shhub_AttachSeq[USB_NUM_USBIP]      = { USB_SEQ_0, USB_SEQ_0 };
uint16_t        usb_shhub_ResetSeq[USB_NUM_USBIP]       = { USB_SEQ_0, USB_SEQ_0 };
uint16_t        usb_shhub_State[USB_NUM_USBIP]          = { USB_SEQ_0, USB_SEQ_0 };
uint16_t        usb_shhub_Info[USB_NUM_USBIP]           = { USB_SEQ_0, USB_SEQ_0 };
uint16_t        usb_shhub_HubAddr[USB_NUM_USBIP]        = { USB_SEQ_0, USB_SEQ_0 };
uint16_t        usb_shhub_Process[USB_NUM_USBIP]        = { USB_SEQ_0, USB_SEQ_0 };
#else   /* USB_NUM_USBIP == 2 */
uint16_t        usb_shhub_ClassSeq[USB_NUM_USBIP]       = { USB_SEQ_0 };
uint16_t        usb_shhub_InitSeq[USB_NUM_USBIP]        = { USB_SEQ_0 };
uint16_t        usb_shhub_InitPort[USB_NUM_USBIP]       = { USB_HUB_P1 };
uint16_t        usb_shhub_IventSeq[USB_NUM_USBIP]       = { USB_SEQ_0 };
uint16_t        usb_shhub_IventPort[USB_NUM_USBIP]      = { USB_HUB_P1 };
uint16_t        usb_shhub_AttachSeq[USB_NUM_USBIP]      = { USB_SEQ_0 };
uint16_t        usb_shhub_ResetSeq[USB_NUM_USBIP]       = { USB_SEQ_0 };
uint16_t        usb_shhub_State[USB_NUM_USBIP]          = { USB_SEQ_0 };
uint16_t        usb_shhub_Info[USB_NUM_USBIP]           = { USB_SEQ_0 };
uint16_t        usb_shhub_HubAddr[USB_NUM_USBIP]        = { USB_SEQ_0 };
uint16_t        usb_shhub_Process[USB_NUM_USBIP]        = { USB_SEQ_0 };
#endif  /* USB_NUM_USBIP == 2 */

uint8_t         *usb_shhub_DeviceTable[USB_NUM_USBIP];
uint8_t         *usb_shhub_ConfigTable[USB_NUM_USBIP];
uint8_t         *usb_shhub_InterfaceTable[USB_NUM_USBIP];
uint16_t        usb_shhub_Spec[USB_NUM_USBIP];
uint16_t        usb_shhub_Root[USB_NUM_USBIP];
uint16_t        usb_shhub_Speed[USB_NUM_USBIP];
uint16_t        usb_shhub_DevAddr[USB_NUM_USBIP];
uint16_t        usb_shhub_Index[USB_NUM_USBIP];

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
    /* PIPE9 Definition */
    USB_PIPE9,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(7u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
    /* PIPE8 Definition */
    USB_PIPE8,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(6u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
#endif  /* USB_IP_PIPE_9_PP */
    /* PIPE7 Definition */
    USB_PIPE7,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(5u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
    /* PIPE6 Definition */
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
    /* PIPE9 Definition */
    USB_PIPE9,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(7u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
    /* PIPE8 Definition */
    USB_PIPE8,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(6u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
#endif  /* USB_IP_PIPE_9_PP */
    /* PIPE7 Definition */
    USB_PIPE7,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(5u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
    /* PIPE6 Definition */
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
    /* PIPE9 Definition */
    USB_PIPE9,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(7u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
    /* PIPE8 Definition */
    USB_PIPE8,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(6u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
#endif  /* USB_IP_PIPE_9_PP */
    /* PIPE7 Definition */
    USB_PIPE7,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(5u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
    /* PIPE6 Definition */
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
    /* PIPE9 Definition */
    USB_PIPE9,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(7u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
    /* PIPE8 Definition */
    USB_PIPE8,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(6u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
#endif  /* USB_IP_PIPE_9_PP */
    /* PIPE7 Definition */
    USB_PIPE7,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(5u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
    /* PIPE6 Definition */
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
    case USB_HSHUBS:    /* Hi Speed Hub(Multi) */
        if( descriptor_table[4] != USB_HUB_INTNUMHSS )
        {
            USB_PRINTF0("### HUB configuration descriptor error !\n");
            return USB_ERROR;
        }
        break;
    case USB_HSHUBM:    /* Hi Speed Hub(Single) */
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
    uint8_t     *descriptor_Table;
    uint16_t    ofset;

    descriptor_Table = (uint8_t*)(table[2]);

    /* Descriptor check */
    ofset = usb_hstd_CheckDescriptor(descriptor_Table, (uint16_t)USB_DT_INTERFACE);
    if( ofset == USB_ERROR )
    {
        USB_PRINTF0("### Interface descriptor error !\n");
        return USB_ERROR;
    }

    /* Check interface class */
    if( descriptor_Table[5] != USB_IFCLS_HUB )
    {
        USB_PRINTF0("### HUB interface descriptor error !\n");
        return USB_ERROR;
    }

    /* Check interface number */
    switch( spec )
    {
    case USB_FSHUB:     /* Full Speed Hub */
        if( descriptor_Table[2] != (USB_HUB_INTNUMFS - 1u) )
        {
            USB_PRINTF0("### HUB interface descriptor error !\n");
            return USB_ERROR;
        }
        break;
    case USB_HSHUBS:    /* Hi Speed Hub(Single) */
        if( descriptor_Table[2] != (USB_HUB_INTNUMHSS - 1u) )
        {
            USB_PRINTF0("### HUB interface descriptor error !\n");
            return USB_ERROR;
        }
        break;
    case USB_HSHUBM:    /* Hi Speed Hub(Multi) */
        if( descriptor_Table[2] != (USB_HUB_INTNUMHSM - 1u) )
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
            /* Interrupt Endpoint */
            if( (table[ofdsc + 3] & USB_EP_TRNSMASK) == USB_EP_INT )
            {
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
    /* HUB downport RemoteWakeup */
    usb_shhub_Remote[ptr->ip][hubaddr]  &= (uint16_t)(~USB_BITSET(portnum));
    /* Now downport device search */
    devaddr = usb_hhub_GetCnnDevaddr(ptr, hubaddr, portnum);
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
    /* Check Connection */
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

    /* Root port */
    usb_ghstd_DeviceInfo[ptr->ip][devaddr][0] = USB_NOPORT;
    /* Device state */
    usb_ghstd_DeviceInfo[ptr->ip][devaddr][1] = USB_DETACHED;
    /* Not configured */
    usb_ghstd_DeviceInfo[ptr->ip][devaddr][2] = (uint16_t)0;
    /* Interface Class : NO class */
    usb_ghstd_DeviceInfo[ptr->ip][devaddr][3] = (uint16_t)USB_IFCLS_NOT;
    /* No connect */
    usb_ghstd_DeviceInfo[ptr->ip][devaddr][4] = (uint16_t)USB_NOCONNECT;

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
        /* Send Message failure */
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
            /* HUB address */
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
        if( (usb_shhub_InfoData[ptr->ip][i][0] == hubaddr) && (usb_shhub_InfoData[ptr->ip][i][1] == portnum) )
        {
            /* Device address */
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
                : USB_CB_t complete         : callback function
Return value    : uint16_t                  : DONE/ERROR
******************************************************************************/
uint16_t R_usb_hhub_GetHubInformation(USB_UTR_t *ptr, uint16_t hubaddr, USB_CB_t complete)
{
    uint16_t    err;

    USB_ER_t    qerr;

    /* Request */
    usb_shhub_ClassRequest[ptr->ip][0]  = USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_CLASS | USB_DEVICE;
    usb_shhub_ClassRequest[ptr->ip][1]  = USB_HUB_DESCRIPTOR;
    usb_shhub_ClassRequest[ptr->ip][2]  = 0;
    usb_shhub_ClassRequest[ptr->ip][3]  = 0x0047;
    usb_shhub_ClassRequest[ptr->ip][4]  = hubaddr;      /* Device address */

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
    qerr = R_usb_hstd_TransferStart(&usb_shhub_ControlMess[ptr->ip]);
    if(qerr == USB_E_QOVR)
    {
        return USB_HUB_QOVR;
    }

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

    USB_ER_t        qerr;

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
    qerr = R_usb_hstd_TransferStart(&usb_shhub_ControlMess[ptr->ip]);
    if(qerr == USB_E_QOVR)
    {
        return USB_HUB_QOVR;
    }

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

    USB_ER_t        qerr;

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
    qerr = R_usb_hstd_TransferStart(&usb_shhub_ControlMess[ptr->ip]);
    if(qerr == USB_E_QOVR)
    {
        return USB_HUB_QOVR;
    }

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

    USB_ER_t        qerr;

    /* Request */
    usb_shhub_ClassRequest[ptr->ip][0]  = USB_GET_STATUS| USB_DEV_TO_HOST | USB_CLASS | USB_OTHER;
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

    usb_shhub_ControlMess[ptr->ip].usr_data = (void *)ptr->keyword;     /* Device no. set */

    /* Transfer start */
    qerr = R_usb_hstd_TransferStart(&usb_shhub_ControlMess[ptr->ip]);
    if(qerr == USB_E_QOVR)
    {
        return USB_HUB_QOVR;
    }

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
Arguments       : USB_VP_INT stacd          : Start Code of Hub Task
Return value    : none
******************************************************************************/
void usb_hhub_Task(USB_VP_INT stacd)
{
    USB_UTR_t       *mess;
    USB_ER_t        err;
    uint16_t        status;
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
    uint16_t        elseport;
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
    /* Receive message */
    err = USB_TRCV_MSG(USB_HUB_MBX, (USB_MSG_t**)&mess, (USB_TM_t)0);
    if( err != USB_OK )
    {
        return;
    }

    switch( mess->msginfo )
    {
    case USB_MSG_CLS_CHECKREQUEST:
        /* USB HUB Class Enumeration */
        usb_hhub_Enumeration((USB_CLSINFO_t *) mess);
        err = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)mess);
        if( err != USB_OK )
        {
            /* Release Memoryblock failure */
            USB_PRINTF0("### USB HUB Task rel_blk error\n");
        }
        break;
    case USB_MSG_CLS_INIT:
        /* Down port initialize */
        usb_hhub_InitDownPort(mess, (uint16_t)0, (USB_CLSINFO_t *)mess);
        break;
    /* Enumeration waiting of other device */
    case USB_MSG_CLS_WAIT:
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
        elseport = 0;
        if( mess->keyword == 0 )
        {
            elseport = 1;
        }
        if( usb_ghstd_MgrMode[mess->ip][elseport] != USB_DEFAULT )
        {
            mess->msginfo = USB_MSG_MGR_AORDETACH;
            err = USB_SND_MSG( USB_MGR_MBX, (USB_MSG_t*)mess );
            if( err != USB_OK )
            {
                /* Send Message failure */
                USB_PRINTF0("### USB HUB enuwait snd_msg error\n");
            }
        }
        else
        {
            err = USB_SND_MSG(USB_HUB_MBX, (USB_MSG_t*)mess);
            if( err != USB_OK )
            {
                /* Send Message failure */
                USB_PRINTF0("### USB HUB enuwait snd_msg error\n");
            }
        }
#else   /* USB_PORTSEL_PP == USB_2PORT_PP */
        mess->msginfo = USB_MSG_MGR_AORDETACH;
        err = USB_SND_MSG( USB_MGR_MBX, (USB_MSG_t*)mess );
        if( err != USB_OK )
        {
            /* Send Message failure */
            USB_PRINTF0("### USB HUB enuwait snd_msg error\n");
        }
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
        break;
    case USB_MSG_HUB_SUBMITRESULT:
        status = (uint16_t)(mess -> status);
        usb_hhub_SubmitResult((USB_CLSINFO_t *) mess, status);
        break;

    case USB_MSG_HUB_IVENT:
        usb_hhub_Ivent((USB_CLSINFO_t *) mess);
        break;

    case USB_MSG_HUB_ATTACH:
        /* Hub Port attach */
        usb_hhub_PortAttach((uint16_t)0, (uint16_t)0, (USB_CLSINFO_t *) mess);
        break;

    case USB_MSG_HUB_RESET:
        /* Hub Reset */
        usb_hhub_PortReset(mess, (uint16_t)0, (uint16_t)0, (USB_CLSINFO_t *) mess);
        break;

    default:
        err = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)mess);
        if( err != USB_OK )
        {
            USB_PRINTF0("### USB HUB rel_blk error\n");
        }
        break;
    }
}
/******************************************************************************
End of function usb_hhub_Task
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_hhub_Task
Description     : Call HUB task
Arguments       : USB_VP_INT stacd          : Start Code of Hub Task
Return value    : none
******************************************************************************/
void R_usb_hhub_Task(USB_VP_INT stacd)
{
    usb_hhub_Task( stacd );
}
/******************************************************************************
End of function R_usb_hhub_Task
******************************************************************************/

/******************************************************************************
Function Name   : usb_hhub_ChkClass
Description     : HUB Class driver check
Arguments       : uint16_t **table          : Descriptor, etc
Return value    : none
******************************************************************************/
void usb_hhub_ChkClass(USB_UTR_t *ptr, uint16_t **table)
{
    USB_CLSINFO_t   *p_blf;
    USB_CLSINFO_t   *cp;
    USB_ER_t err;
    
    usb_shhub_DeviceTable[ptr->ip]      = (uint8_t*)(table[0]);
    usb_shhub_ConfigTable[ptr->ip]      = (uint8_t*)(table[1]);
    usb_shhub_InterfaceTable[ptr->ip]   = (uint8_t*)(table[2]);
    *table[3]                           = USB_DONE;
    usb_shhub_Spec[ptr->ip]             = *table[4];
    usb_shhub_Root[ptr->ip]             = *table[5];
    usb_shhub_Speed[ptr->ip]            = *table[6];
    usb_shhub_DevAddr[ptr->ip]          = *table[7];
    usb_shhub_Index[ptr->ip]            = usb_hhub_ChkTblIndx1(ptr, usb_shhub_DevAddr[ptr->ip]);
    
    usb_shhub_ClassSeq[ptr->ip]=0;

    /* Get mem pool blk */
    if( USB_PGET_BLK(USB_HUB_MPL,&p_blf) == USB_OK )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = USB_MSG_CLS_CHECKREQUEST;

        cp->ipp = ptr->ipp;
        cp->ip  = ptr->ip;
        
        /* Enumeration wait TaskID change */
        R_usb_hstd_EnuWait(ptr, (uint8_t)USB_HUB_TSK);
        
        /* Class check of enumeration sequence move to class function */
        err = USB_SND_MSG(USB_HUB_MBX, (USB_MSG_t*)cp);
        if( err != USB_OK )
        {
            /* Send Message failure */
            USB_PRINTF1("Host HUB snd_msg error %x\n", err);
        }
    }
    else
    {
        /* Send Message failure */
        while( 1 );
    }
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
    USB_MH_t        p_blf;
    USB_MGRINFO_t   *mp;
    uint16_t        hubaddr, index;

    err = USB_ERROR;
    hubaddr = (uint16_t)(devaddr << USB_DEVADDRBIT);
    index = usb_hhub_ChkTblIndx1(ptr, devaddr);

    if( usb_shhub_Number[ptr->ip] != USB_MAXHUB )
    {
        /* Wait 10ms */
        usb_cpu_DelayXms((uint16_t)10);
        err = USB_PGET_BLK(USB_HUB_MPL, &p_blf);
        if( err == USB_E_OK )
        {
            mp = (USB_MGRINFO_t*)p_blf;
            mp->msghead = (USB_MH_t)USB_NULL;
            mp->msginfo = USB_MSG_CLS_INIT;
            mp->keyword = devaddr;

            mp->ipp     = ptr->ipp;
            mp->ip      = ptr->ip;

            /* Send message */
            err = USB_SND_MSG(USB_HUB_MBX, (USB_MSG_t*)p_blf);
            if( err != USB_E_OK )
            {
                /* Send Message failure */
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
            /* Release memory block failure */
            USB_PRINTF1("### hHubOpen pget_blk error (%ld)\n", err);
            while( 1 );
        }
        /* Pipe number set */
        usb_shhub_InfoData[ptr->ip][devaddr][3] = usb_ghhub_TmpEPTbl[ptr->ip][index];
        /* HUB downport status */
        usb_shhub_DownPort[ptr->ip][devaddr]    = 0;
        /* Downport remotewakeup */
        usb_shhub_Remote[ptr->ip][devaddr]  = 0;
        usb_ghhub_TmpEPTbl[ptr->ip][index+3]    |= hubaddr;
        R_usb_hstd_SetPipeInfo(&usb_ghhub_DefEPTbl[ptr->ip][index],
            &usb_ghhub_TmpEPTbl[ptr->ip][index], (uint16_t)USB_EPL);

        usb_shhub_Process[ptr->ip] = USB_MSG_CLS_INIT;
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
    uint16_t        md, i;
    USB_HCDREG_t    *driver;
    uint16_t        devaddr, index;

    for( i = 1; i <= usb_shhub_InfoData[ptr->ip][hubaddr][2]; i++ )
    {
        /* Now downport device search */
        devaddr = usb_hhub_GetCnnDevaddr(ptr, hubaddr, i);
        if( devaddr != 0 )
        {
            /* HUB down port selective disconnect */
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
                }
            }
        }
    }

    usb_shhub_Number[ptr->ip]--;
    index = usb_hhub_ChkTblIndx2(ptr, hubaddr);

    /* Set pipe information */
    for(i = 1; i <= USB_MAXDEVADDR ; i++)
    {
        usb_shhub_InfoData[ptr->ip][i][0]   = 0;    /* Up-address clear */
        usb_shhub_InfoData[ptr->ip][i][1]   = 0;    /* Up-port num clear */
        usb_shhub_InfoData[ptr->ip][i][2]   = 0;    /* Port number clear */
        usb_shhub_InfoData[ptr->ip][i][3]   = 0;    /* Pipe number clear */
    }

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
    USB_ER_t        err;
    uint16_t        retval;

    hubaddr = usb_shhub_HubAddr[ptr->ip];
    retval = USB_DONE;

    /* Check Hub Process */
    if(usb_shhub_Process[ptr->ip] != USB_MSG_CLS_INIT)
    {
        err = USB_SND_MSG( USB_HUB_MBX, (USB_MSG_t*)mess );
        if( err != USB_OK )
        {
            /* Send Message failure */
            USB_PRINTF0("### HUB snd_msg error\n");
        }
    }
    else
    {
        switch(usb_shhub_InitSeq[ptr->ip])
        {
        /* HUB port power */
        case USB_SEQ_0:
            hubaddr = mess -> keyword;
            usb_shhub_HubAddr[ptr->ip] = hubaddr;

            usb_hstd_DeviceDescripInfo();
            usb_hstd_ConfigDescripInfo();
            USB_PRINTF0("\n");
            USB_PRINTF0("HHHHHHHHHHHHHHHHHHHHHHHHH\n");
            USB_PRINTF0("         USB HOST        \n");
            USB_PRINTF0("      HUB CLASS DEMO     \n");
            USB_PRINTF0("HHHHHHHHHHHHHHHHHHHHHHHHH\n");
            USB_PRINTF0("\n");
            
            /* Next Sequence */
            usb_shhub_InitSeq[ptr->ip] = USB_SEQ_1;
            usb_shhub_InitPort[ptr->ip] = USB_HUB_P1;
            /* Next Process Selector */
            usb_hhub_SpecifiedPath(mess);
            break;

        /* Request */
        case USB_SEQ_1:
            /* SetFeature request */
            retval = usb_hhub_PortSetFeature(ptr, hubaddr, usb_shhub_InitPort[ptr->ip],
                (uint16_t)USB_HUB_PORT_POWER, usb_hhub_CheckResult);
            /* Submit overlap error */
            if(retval == USB_HUB_QOVR)
            {
                usb_hhub_SpecifiedPathWait(mess, (uint16_t)10);
            }
            else
            {
                /* Next Port */
                usb_shhub_InitPort[ptr->ip]++;
                /* Next Sequence */
                usb_shhub_InitSeq[ptr->ip] = USB_SEQ_2;
            }
            break;
        /* Request Result Check */
        case USB_SEQ_2:
            retval = usb_hhub_RequestResult(mess->result);
            if( retval == USB_DONE )
            {
                if( usb_shhub_InitPort[ptr->ip] > usb_shhub_InfoData[ptr->ip][hubaddr][2] )
                {
                    /* Port Clear */
                    usb_shhub_InitPort[ptr->ip] = USB_HUB_P1;
                    /* Next Sequence */
                    usb_shhub_InitSeq[ptr->ip] = USB_SEQ_3;
                    /* Next Process Selector */
                    usb_hhub_SpecifiedPath(mess);
                }
                else
                {
                    /* Loop Sequence */
                    usb_shhub_InitSeq[ptr->ip] = USB_SEQ_1;
                    /* Next Process Selector */
                    usb_hhub_SpecifiedPath(mess);
                }
            }
            break;
        /* HUB downport initialize */
        /* Request */
        case USB_SEQ_3:
            retval = usb_hhub_PortClrFeature(ptr,hubaddr, usb_shhub_InitPort[ptr->ip],
                (uint16_t)USB_HUB_C_PORT_CONNECTION,usb_hhub_CheckResult);
            /* Submit overlap error */
            if(retval == USB_HUB_QOVR)
            {
                usb_hhub_SpecifiedPathWait(mess, (uint16_t)10);
            }
            else
            {
                /* Next Port */
                usb_shhub_InitPort[ptr->ip]++;
                /* Next Sequence */
                usb_shhub_InitSeq[ptr->ip] = USB_SEQ_4;
            }
            break;
        /* Request Result Check */
        case USB_SEQ_4:
            retval = usb_hhub_RequestResult(mess->result);
            if( retval == USB_DONE )
            {
                if( usb_shhub_InitPort[ptr->ip] > usb_shhub_InfoData[ptr->ip][hubaddr][2] )
                {
                    /* Sequence Clear */
                    usb_shhub_InitSeq[ptr->ip] = USB_SEQ_0;
                    /* Port Clear */
                    usb_shhub_InitPort[ptr->ip] = USB_HUB_P1;
                    usb_shhub_Info[ptr->ip] = USB_MSG_CLS_INIT;
                    /* Next Attach Process */
                    usb_shhub_Process[ptr->ip] = USB_MSG_HUB_IVENT;
                    /* Next Process Selector */
                    usb_hhub_SpecifiedPath(mess);
                }
                else
                {
                    /* Loop Sequence */
                    usb_shhub_InitSeq[ptr->ip] = USB_SEQ_3;
                    /* Next Process Selector */
                    usb_hhub_SpecifiedPath(mess);
                }
            }   
            break;
        default:
            retval = USB_NG;
            break;
        }
        if( (retval != USB_DONE) && (retval != USB_HUB_QOVR) ) 
        {
            /* Port Clear */
            usb_shhub_InitPort[ptr->ip] = USB_HUB_P1;
            /* Sequence Clear */
            usb_shhub_InitSeq[ptr->ip] = USB_SEQ_0;
            /* Clear */
            usb_shhub_Info[ptr->ip] = USB_NONE;
            /* Clear */
            usb_shhub_Process[ptr->ip] = USB_NONE;
        }
    
        err = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)mess);
        if( err != USB_OK )
        {
            /* Release Memoryblock failure */
            USB_PRINTF0("### USB HostHubClass rel_blk error\n");
        }
    }
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
        uint16_t        devaddr;
    
    hubaddr = usb_shhub_HubAddr[ptr->ip];
    portnum = usb_shhub_IventPort[ptr->ip];

    /* New downport device search */
    devaddr = usb_hhub_GetNewDevaddr(ptr);
    if( devaddr != 0 )
    {
        USB_PRINTF1(" Hubport connect address%d\n", devaddr);
        /* Up-hubaddr set */
        usb_shhub_InfoData[ptr->ip][devaddr][0] = hubaddr;
        /* Up-hubport set */
        usb_shhub_InfoData[ptr->ip][devaddr][1] = portnum;
        
        usb_shhub_Process[ptr->ip] = USB_MSG_HUB_ATTACH;
        /* Next Process Selector */
        usb_hhub_SpecifiedPath(mess);
        
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
    uint16_t        rootport, devaddr, retval;
    uint16_t        hpphub, hubport, buffer;
    USB_ER_t        err;
    USB_UTR_t       *ptr;

    ptr     = (USB_UTR_t *)mess;
    hubaddr = usb_shhub_HubAddr[ptr->ip];
    portnum = usb_shhub_IventPort[ptr->ip];

    if( usb_shhub_Process[ptr->ip] != USB_MSG_HUB_ATTACH )
    {
        err = USB_SND_MSG( USB_HUB_MBX, (USB_MSG_t*)mess );
        if( err != USB_OK )
        {
            /* Send Message failure */
            USB_PRINTF0("### HUB snd_msg error\n");
        }
    }
    else
    {
        switch(usb_shhub_AttachSeq[ptr->ip])
        {
        case USB_SEQ_0:
            usb_shhub_AttachSeq[ptr->ip] = USB_SEQ_1;
            usb_shhub_Process[ptr->ip] = USB_MSG_HUB_RESET;
            /* Next Process Selector */
            usb_hhub_SpecifiedPath(mess);
            break;
        case USB_SEQ_1:
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
            rootport = usb_hstd_GetRootport(ptr, 
                (uint16_t)(hubaddr << USB_DEVADDRBIT));
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
                /* Set up-port hub */
                usb_hstd_SetHubPort(ptr, (uint16_t)USB_DEVICE_0, hpphub, hubport);
                /* Set up-port hub */
                usb_hstd_SetHubPort(ptr, (uint16_t)(usb_ghstd_DeviceAddr[ptr->ip] << USB_DEVADDRBIT), hpphub, hubport);
                /* Clear Enumeration Sequence Number */
                usb_ghstd_EnumSeq[ptr->ip] = 0;
                if( usb_ghstd_DeviceSpeed[ptr->ip] != USB_NOCONNECT )
                {
                    (*usb_ghstd_EnumarationProcess[0])(ptr, (uint16_t)USB_DEVICE_0, (uint16_t)0);
                    usb_shhub_AttachSeq[ptr->ip] = USB_SEQ_2;
                    usb_hhub_SpecifiedPathWait(mess, 3u);
                }
                else
                {
                    usb_shhub_AttachSeq[ptr->ip] = USB_SEQ_3;
                    /* Next Process Selector */
                    usb_hhub_SpecifiedPath(mess);
                }
            }
            else
            {
                usb_shhub_AttachSeq[ptr->ip] = USB_SEQ_3;
                /* Next Process Selector */
                usb_hhub_SpecifiedPath(mess);
            }
            break;
            
        case USB_SEQ_2:
            /* Get Port Number */
            rootport = usb_hstd_GetRootport(ptr, (uint16_t)(hubaddr << USB_DEVADDRBIT));
            if( (usb_ghstd_MgrMode[ptr->ip][rootport] == USB_CONFIGURED)
             || (usb_ghstd_MgrMode[ptr->ip][rootport] != USB_DEFAULT) )
            {
                /* HUB downport status */
                usb_shhub_DownPort[ptr->ip][hubaddr] |= USB_BITSET(portnum);
                usb_shhub_AttachSeq[ptr->ip] = USB_SEQ_0;
                usb_shhub_Process[ptr->ip] = USB_MSG_HUB_IVENT;
                usb_hhub_SpecifiedPath(mess);
            }
            else
            {
                usb_shhub_AttachSeq[ptr->ip] = USB_SEQ_2;
                usb_hhub_SpecifiedPathWait(mess, 3u);
            }
            break;
            
        case USB_SEQ_3:
            usb_shhub_AttachSeq[ptr->ip] = USB_SEQ_4;
            usb_shhub_Process[ptr->ip] = USB_MSG_HUB_RESET;
            /* Next Process Selector */
            usb_hhub_SpecifiedPath(mess);
            break;
        case USB_SEQ_4:
            usb_hhub_RequestResult(mess->result);
            /* Hub Port Set Feature Request */
            retval = usb_hhub_PortSetFeature(ptr, hubaddr, portnum, USB_HUB_PORT_SUSPEND, usb_hhub_CheckResult);
            /* Submit overlap error */
            if(retval == USB_HUB_QOVR)
            {
                usb_hhub_SpecifiedPathWait(mess, (uint16_t)10);
            }
            else
            {
                usb_shhub_AttachSeq[ptr->ip] = USB_SEQ_5;
            }
            break;
        case USB_SEQ_5:
            usb_hhub_RequestResult(mess->result);
            usb_hhub_PortDetach(ptr, hubaddr, portnum);
            /* Up-hubaddr clear */
            usb_shhub_InfoData[ptr->ip][devaddr][0] = USB_SEQ_0;
            /* Up-hubport clear */
            usb_shhub_InfoData[ptr->ip][devaddr][1] = USB_SEQ_0;
            usb_shhub_AttachSeq[ptr->ip] = USB_SEQ_0;
            usb_shhub_Process[ptr->ip] = USB_MSG_HUB_IVENT;
            usb_hhub_SpecifiedPath(mess);
            break;
        default:
            usb_shhub_AttachSeq[ptr->ip] = USB_SEQ_0;
            usb_shhub_Process[ptr->ip] = USB_NONE;
            usb_shhub_Info[ptr->ip] = USB_NONE;
            break;
        }
        err = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)mess);
        if( err != USB_OK )
        {
            USB_PRINTF0("### USB HostHubClass rel_blk error\n");
        }
    }
    return USB_DONE;
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
    USB_ER_t        err;
    uint16_t        retval;
    
    hubaddr = usb_shhub_HubAddr[ptr->ip];
    portnum = usb_shhub_IventPort[ptr->ip];

    if( usb_shhub_Process[ptr->ip] != USB_MSG_HUB_RESET )
    {
        err = USB_SND_MSG( USB_HUB_MBX, (USB_MSG_t*)mess );
        if( err != USB_OK )
        {
            USB_PRINTF0("### HUB snd_msg error\n");
        }
    }
    else
    {
        switch( usb_shhub_ResetSeq[ptr->ip] )
        {
        case USB_SEQ_0:
            /* Hub port SetFeature */
            usb_cpu_DelayXms((uint16_t)100);
            retval = usb_hhub_PortSetFeature(ptr, hubaddr, portnum, (uint16_t)USB_HUB_PORT_RESET, usb_hhub_CheckResult);
            /* Submit overlap error */
            if(retval == USB_HUB_QOVR)
            {
                usb_hhub_SpecifiedPathWait(mess, (uint16_t)10);
            }
            else
            {
                usb_shhub_ResetSeq[ptr->ip] = USB_SEQ_1;
            }
            break;
        case USB_SEQ_1:
            usb_hhub_RequestResult(mess->result);
            usb_cpu_DelayXms((uint16_t)60);

            /* Get Status */
            retval = R_usb_hhub_GetPortInformation(ptr, hubaddr, portnum, usb_hhub_CheckResult);
            /* Submit overlap error */
            if(retval == USB_HUB_QOVR)
            {
                usb_hhub_SpecifiedPathWait(mess, (uint16_t)10);
            }
            else
            {
                usb_shhub_ResetSeq[ptr->ip] = USB_SEQ_2;
            }
            break;
        case USB_SEQ_2:
            usb_hhub_RequestResult(mess->result);
            if( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][2] & (uint8_t)0x10) != 0x10)
            {
                /* Get Status */
                usb_cpu_DelayXms((uint16_t)60);

                retval = R_usb_hhub_GetPortInformation(ptr, hubaddr, portnum, usb_hhub_CheckResult);
                /* Submit overlap error */
                if(retval == USB_HUB_QOVR)
                {
                    usb_hhub_SpecifiedPathWait(mess, (uint16_t)10);
                }
                else
                {
                    usb_shhub_ResetSeq[ptr->ip] = USB_SEQ_2;
                }
            }
            else
            {
                /* Hub port ClearFeature */
                usb_cpu_DelayXms((uint16_t)20);

                retval = usb_hhub_PortClrFeature(ptr, hubaddr, portnum,
                                (uint16_t)USB_HUB_C_PORT_RESET, usb_hhub_CheckResult);
                /* Submit overlap error */
                if(retval == USB_HUB_QOVR)
                {
                    usb_hhub_SpecifiedPathWait(mess, (uint16_t)10);
                }
                else
                {
                    usb_shhub_ResetSeq[ptr->ip] = USB_SEQ_3;
                }
            }
            break;
        case USB_SEQ_3:
            usb_hhub_RequestResult(mess->result);
            usb_shhub_ResetSeq[ptr->ip] = USB_SEQ_0;
            usb_shhub_Process[ptr->ip] = USB_MSG_HUB_ATTACH;
            usb_hhub_SpecifiedPath(mess);
            break;
        default:
            usb_shhub_ResetSeq[ptr->ip] = USB_SEQ_0;
            usb_shhub_Process[ptr->ip] = USB_NONE;
            break;
        }
        err = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)mess);
        if( err != USB_OK )
        {
            USB_PRINTF0("### USB HostHubClass rel_blk error\n");
        }
    }
}
/******************************************************************************
End of function usb_hhub_PortReset
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_ReqTransWait
Description     : Request result
Arguments       : none
Return value    : uint16_t                  : USB_DONE
******************************************************************************/
uint16_t usb_hhub_ReqTransWait(void)
{
    return USB_DONE;
}
/******************************************************************************
End of function usb_hhub_ReqTransWait
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_SubmitResult
Description     : submit result
Arguments       : USB_CLSINFO_t *mess
                : uint16_t status
Return value    : none
******************************************************************************/
void usb_hhub_SubmitResult(USB_CLSINFO_t *mess, uint16_t status)
{
    USB_ER_t        err;
    uint16_t        pipenum, hubaddr;
    USB_UTR_t       *ptr;
    
    ptr = (USB_UTR_t *)mess;
    pipenum = mess->keyword;
    hubaddr = usb_hhub_GetHubaddr(ptr, pipenum);
    usb_shhub_HubAddr[ptr->ip] = hubaddr;
    
    if( (usb_shhub_Process[ptr->ip] != USB_MSG_HUB_SUBMITRESULT) && (usb_shhub_Process[ptr->ip] != USB_NONE) )
    {
        err = USB_SND_MSG( USB_HUB_MBX, (USB_MSG_t*)mess );
        if( err != USB_OK )
        {
            /* Send Message failure */
            USB_PRINTF0("### HUB snd_msg error\n");
        }
    }
    else
    {
        usb_shhub_Process[ptr->ip] = USB_NONE;
    
        switch( status )
        {
        case USB_DATA_SHT:
                USB_PRINTF1("*** receive short(pipe %d : HUB) !\n", pipenum);
        /* Continue */
        /* Direction is in then data receive end */
        case USB_DATA_OK:
            if( (usb_ghstd_MgrMode[ptr->ip][0] == USB_DEFAULT) || (usb_ghstd_MgrMode[ptr->ip][1] == USB_DEFAULT) )
            {
                err = USB_SND_MSG( USB_HUB_MBX, (USB_MSG_t*)mess );
                if( err != USB_OK )
                {
                    USB_PRINTF0("### HUB task snd_msg error\n");
                }
            }
            else
            {
                /* HUB port connection */
                usb_shhub_Info[ptr->ip] = USB_MSG_HUB_SUBMITRESULT;
                usb_shhub_Process[ptr->ip] = USB_MSG_HUB_IVENT;
                usb_hhub_SpecifiedPath(mess);
            }
            break;
        case USB_DATA_STALL:
            USB_PRINTF0("*** Data Read error. ( STALL ) !\n");
            /* CLEAR_FEATURE */
            usb_hstd_ClearStall(ptr, pipenum, (USB_CB_t)&usb_hhub_ClearStallResult);
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
    }
}
/******************************************************************************
End of function usb_hhub_SubmitResult
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_Ivent
Description     : 
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hhub_Ivent(USB_CLSINFO_t *mess)
{
    USB_ER_t        err;
    uint16_t        hubaddr, devaddr, retval;
    USB_UTR_t       *ptr;

/* Condition compilation by the difference of useful function */
#if defined(USB_DEBUGUART_PP) || defined(USB_DEBUGPRINT_PP)
    uint16_t        pdata1, pdata2;
#endif  /* USB_DEBUGUART_PP || USB_DEBUGPRINT_PP */

    ptr = (USB_UTR_t *)mess;
    hubaddr = usb_shhub_HubAddr[ptr->ip];

    if( usb_shhub_Process[ptr->ip] != USB_MSG_HUB_IVENT )
    {
        err = USB_SND_MSG( USB_HUB_MBX, (USB_MSG_t*)mess );
        if( err != USB_OK )
        {
            USB_PRINTF0("### HUB snd_msg error\n");
        }
    }
    else
    {
        switch( usb_shhub_IventSeq[ptr->ip] )
        {
        /* Event Check */
        /* Request */
        case USB_SEQ_0:
             /* USB_MSG_HUB_SUBMITRESULT */
            if( usb_shhub_Info[ptr->ip] == USB_MSG_HUB_SUBMITRESULT )
            {
                if( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][0] & USB_BITSET(usb_shhub_IventPort[ptr->ip])) != 0 )
                {
                    USB_PRINTF1(" *** HUB port %d \t",usb_shhub_IventPort[ptr->ip]);
                    /* GetStatus request */
                    retval = R_usb_hhub_GetPortInformation(ptr, hubaddr, usb_shhub_IventPort[ptr->ip],
                                                    usb_hhub_CheckResult);
                    /* Submit overlap error */
                    if(retval == USB_HUB_QOVR)
                    {
                        usb_hhub_SpecifiedPathWait(mess, (uint16_t)10);
                    }
                    else
                    {
                        usb_shhub_IventSeq[ptr->ip] = USB_SEQ_1;
                    }
                }
                else
                {
                    /* Port check end */
                    if( usb_shhub_IventPort[ptr->ip] >= usb_shhub_InfoData[ptr->ip][hubaddr][2] )
                    {
                        usb_hhub_TransStart(ptr, hubaddr, (uint32_t)1, &usb_ghhub_Data[ptr->ip][hubaddr][0],
                                            &usb_hhub_TransResult);
                        /* Port Clear */
                        usb_shhub_IventPort[ptr->ip] = USB_HUB_P1;
                        /* Sequence Clear */
                        usb_shhub_IventSeq[ptr->ip] = USB_SEQ_0;
                        usb_shhub_Process[ptr->ip] = USB_NONE;
                        usb_shhub_Info[ptr->ip] = USB_NONE;
                    }
                    else
                    {
                        /* Next port check */
                        usb_shhub_IventPort[ptr->ip]++;
                        usb_shhub_IventSeq[ptr->ip] = USB_SEQ_0;
                        usb_hhub_SpecifiedPath(mess);
                    }
                }
            }
            else
            {
                /* USB_MSG_HUB_INIT */
                USB_PRINTF2(" *** address %d downport %d \t", hubaddr, usb_shhub_IventPort[ptr->ip]);
                retval = R_usb_hhub_GetPortInformation(ptr, hubaddr, usb_shhub_IventPort[ptr->ip],
                                                        usb_hhub_CheckResult);
                /* Submit overlap error */
                if(retval == USB_HUB_QOVR)
                {
                    usb_hhub_SpecifiedPathWait(mess, (uint16_t)10);
                }
                else
                {
                    usb_shhub_IventSeq[ptr->ip] = USB_SEQ_3;
                }
            }
            break;
        /* Request Result Check & Request */
        case USB_SEQ_1:
            usb_hhub_RequestResult(mess->result);
/* Condition compilation by the difference of useful function */
#if defined(USB_DEBUGUART_PP) || defined(USB_DEBUGPRINT_PP)
            pdata1 = (uint16_t)(((uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][1]) << 8) 
                    + usb_ghhub_Data[ptr->ip][hubaddr][0]);
            pdata2 = (uint16_t)(((uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][3]) << 8)
                    + usb_ghhub_Data[ptr->ip][hubaddr][2]);
            USB_PRINTF2(" [status/change] : 0x%04x, 0x%04x\n", pdata1, pdata2);
#endif  /* USB_DEBUGUART_PP || USB_DEBUGPRINT_PP */
            /* Next Sequence */
            usb_shhub_IventSeq[ptr->ip]=USB_SEQ_2;
            if( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][2] & (uint8_t)0x01) != 0 )
            {
                /* C_PORT_CONNECTION */
                retval = usb_hhub_PortClrFeature(ptr, hubaddr,
                    usb_shhub_IventPort[ptr->ip], (uint16_t)USB_HUB_C_PORT_CONNECTION, usb_hhub_CheckResult);
                /* Submit overlap error */
                if(retval == USB_HUB_QOVR)
                {
                    usb_shhub_IventSeq[ptr->ip] = USB_SEQ_1;
                    usb_hhub_SpecifiedPathWait(mess, (uint16_t)10);
                }
                else
                {
                    /* Attach Sequence */
                    usb_shhub_IventSeq[ptr->ip]=USB_SEQ_3;
                }
            }
            else
            {
                /* Now downport device search */
                devaddr = usb_hhub_GetCnnDevaddr(ptr, hubaddr, usb_shhub_IventPort[ptr->ip]);
                if( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][0] & (uint8_t)0x02) != 0 )
                {
                    /* C_PORT_ENABLE */
                    USB_PRINTF1(" Hubport error address%d\n",devaddr);
                    retval = usb_hhub_PortClrFeature(ptr, hubaddr, usb_shhub_IventPort[ptr->ip],
                                                    (uint16_t)USB_HUB_C_PORT_ENABLE, usb_hhub_CheckResult);
                    /* Submit overlap error */
                    if(retval == USB_HUB_QOVR)
                    {
                        usb_shhub_IventSeq[ptr->ip]=USB_SEQ_1;
                        usb_hhub_SpecifiedPathWait(mess, (uint16_t)10);
                    }
                }
                else if( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][0] & (uint8_t)0x04) != 0 )
                {
                    /* C_PORT_SUSPEND */
                    USB_PRINTF1(" Hubport suspend(resume complete) address%d\n", devaddr);
                    retval = usb_hhub_PortClrFeature(ptr, hubaddr, usb_shhub_IventPort[ptr->ip],
                                                    (uint16_t)USB_HUB_C_PORT_SUSPEND, usb_hhub_CheckResult);
                    /* Submit overlap error */
                    if(retval == USB_HUB_QOVR)
                    {
                        usb_shhub_IventSeq[ptr->ip] = USB_SEQ_1;
                        usb_hhub_SpecifiedPathWait(mess, (uint16_t)10);
                    }
                }
                else if( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][0] & (uint8_t)0x08) != 0 )
                {
                    /* C_PORT_OVER_CURRENT */
                    USB_PRINTF1(" Hubport over current address%d\n", devaddr);
                    /* ClearFeature request */
                    retval = usb_hhub_PortClrFeature(ptr, hubaddr, usb_shhub_IventPort[ptr->ip],
                                                    (uint16_t)USB_HUB_C_PORT_OVER_CURRENT, usb_hhub_CheckResult);
                    /* Submit overlap error */
                    if(retval == USB_HUB_QOVR)
                    {
                        usb_shhub_IventSeq[ptr->ip]=USB_SEQ_1;
                        usb_hhub_SpecifiedPathWait(mess, (uint16_t)10);
                    }
                }
                else if( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][0] & (uint8_t)0x10) != 0 )
                {
                    /* C_PORT_RESET */
                    USB_PRINTF1(" Hubport reset(reset complete) address%d\n", devaddr);
                    /* ClearFeature request */
                    retval = usb_hhub_PortClrFeature(ptr, hubaddr, usb_shhub_IventPort[ptr->ip],
                                                    (uint16_t)USB_HUB_C_PORT_RESET, usb_hhub_CheckResult);
                    /* Submit overlap error */
                    if(retval == USB_HUB_QOVR)
                    {
                        usb_shhub_IventSeq[ptr->ip]=USB_SEQ_1;
                        usb_hhub_SpecifiedPathWait(mess, (uint16_t)10);
                    }
                }
                else
                {
                    if( usb_shhub_IventPort[ptr->ip] >= usb_shhub_InfoData[ptr->ip][hubaddr][2] )
                    {
                        /* Port check end */
                        usb_hhub_TransStart(ptr, hubaddr, (uint32_t)1, &usb_ghhub_Data[ptr->ip][hubaddr][0],
                                            &usb_hhub_TransResult);
                        /* Port Clear */
                        usb_shhub_IventPort[ptr->ip] = USB_HUB_P1;
                        /* Sequence Clear */
                        usb_shhub_IventSeq[ptr->ip] = USB_SEQ_0;
                        usb_shhub_Process[ptr->ip] = USB_NONE;
                        usb_shhub_Info[ptr->ip] = USB_NONE;
                    }
                    else
                    {
                        /* Next port check */
                        usb_shhub_IventPort[ptr->ip]++;
                        usb_shhub_IventSeq[ptr->ip] = USB_SEQ_0;
                        usb_hhub_SpecifiedPath(mess);
                    }
                }
            }
            break;
            
        /* Request Result Check */
        case USB_SEQ_2:
            usb_hhub_RequestResult(mess-> result);
            if( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][0] & (uint8_t)0x04) != 0 )
            {
                /* C_PORT_SUSPEND */
                /* HUB downport status */
                usb_shhub_Remote[ptr->ip][hubaddr] |= USB_BITSET(usb_shhub_IventPort[ptr->ip]);
            }
            if( usb_shhub_IventPort[ptr->ip] >= usb_shhub_InfoData[ptr->ip][hubaddr][2] )
            {
                /* Port check end */
                /* Next command check  */
                usb_hhub_TransStart(ptr, hubaddr, (uint32_t)1 , &usb_ghhub_Data[ptr->ip][hubaddr][0],
                                    &usb_hhub_TransResult);
                usb_shhub_IventPort[ptr->ip] = USB_HUB_P1;
                usb_shhub_IventSeq[ptr->ip] = USB_SEQ_0;
                usb_shhub_Process[ptr->ip] = USB_NONE;
                usb_shhub_Info[ptr->ip] = USB_NONE;
            }
            else
            {
                /* Next port check */
                usb_shhub_IventSeq[ptr->ip] = USB_SEQ_0;
                usb_shhub_IventPort[ptr->ip] ++;
                usb_hhub_SpecifiedPath(mess);
            }
            break;
        /* Attach Sequence */
        case USB_SEQ_3:
            usb_hhub_RequestResult(mess->result);
            if( usb_shhub_Info[ptr->ip] == USB_MSG_CLS_INIT )
            {
/* Condition compilation by the difference of useful function */
#if defined(USB_DEBUGUART_PP) || defined(USB_DEBUGPRINT_PP)
                pdata1 = (uint16_t)(((uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][1]) << 8)
                        + usb_ghhub_Data[ptr->ip][hubaddr][0]);
                pdata2 = (uint16_t)(((uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][3]) << 8)
                        + usb_ghhub_Data[ptr->ip][hubaddr][2]);
                USB_PRINTF2(" [status/change] : 0x%04x, 0x%04x\n", pdata1, pdata2);
#endif  /* USB_DEBUGUART_PP || USB_DEBUGPRINT_PP */
            }
            if( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][0] & (uint8_t)0x01) != 0 )
            {
                /* PORT_CONNECTION */
                if( usb_shhub_Info[ptr->ip] == USB_MSG_HUB_SUBMITRESULT )
                {
                    if( (uint16_t)(usb_shhub_DownPort[ptr->ip][hubaddr] 
                        & USB_BITSET(usb_shhub_IventPort[ptr->ip])) == 0 )
                    {
                        /* Next Attach sequence */
                        usb_shhub_IventSeq[ptr->ip] = USB_SEQ_4;
                        usb_hhub_NewConnect(mess, (uint16_t)0, (uint16_t)0, mess);
                    }
                    else
                    {
                        /* Not PORT_CONNECTION */
                        if( usb_shhub_IventPort[ptr->ip] >= usb_shhub_InfoData[ptr->ip][hubaddr][2] )
                        {
                            /* Port check end */
                            usb_hhub_TransStart(ptr, hubaddr, (uint32_t)1, &usb_ghhub_Data[ptr->ip][hubaddr][0],
                                                &usb_hhub_TransResult);
                            /* Port Clear */
                            usb_shhub_IventPort[ptr->ip] = USB_HUB_P1;
                            /* Sequence Clear */
                            usb_shhub_IventSeq[ptr->ip] = USB_SEQ_0;
                            usb_shhub_Process[ptr->ip] = USB_NONE;
                            usb_shhub_Info[ptr->ip] = USB_NONE;
                        }
                        else
                        {
                            /* Next port check */
                            /* Return to Event Check phase */
                            usb_shhub_IventSeq[ptr->ip] = USB_SEQ_0;
                            usb_shhub_IventPort[ptr->ip] ++;
                            usb_hhub_SpecifiedPath(mess);
                        }
                    }
                }
                else
                {
                    /* Next Attach sequence */
                    usb_shhub_IventSeq[ptr->ip] = USB_SEQ_4;
                    usb_hhub_NewConnect(mess, (uint16_t)0, (uint16_t)0, mess);
                }
            }
            else
            {
                if( usb_shhub_Info[ptr->ip] == USB_MSG_HUB_SUBMITRESULT )
                {
                    /* Now downport device search */
                    devaddr = usb_hhub_GetCnnDevaddr(ptr, hubaddr, usb_shhub_IventPort[ptr->ip]);
                    if( devaddr != 0 )
                    {
                        usb_hhub_PortDetach(ptr, hubaddr, usb_shhub_IventPort[ptr->ip]);
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
                
                if( usb_shhub_IventPort[ptr->ip] >= usb_shhub_InfoData[ptr->ip][hubaddr][2] )
                {
                    /* Port check end */
                    usb_hhub_TransStart(ptr, hubaddr, (uint32_t)1, &usb_ghhub_Data[ptr->ip][hubaddr][0],
                                        &usb_hhub_TransResult);
                    /* Port Clear */
                    usb_shhub_IventPort[ptr->ip] = USB_HUB_P1;
                    /* Sequence Clear */
                    usb_shhub_IventSeq[ptr->ip] = USB_SEQ_0;
                    usb_shhub_Process[ptr->ip] = USB_NONE;
                    usb_shhub_Info[ptr->ip] = USB_NONE;
                }
                else
                {
                    /* Next port check */
                    usb_shhub_IventSeq[ptr->ip] = USB_SEQ_0;
                    usb_shhub_IventPort[ptr->ip] ++;
                    usb_hhub_SpecifiedPath(mess);
                }
            }
            break;
            
        /* Attach */
        case USB_SEQ_4:
            if( usb_shhub_IventPort[ptr->ip] >= usb_shhub_InfoData[ptr->ip][hubaddr][2] )
            {   /* Port check end */
                usb_hhub_TransStart(ptr, hubaddr, (uint32_t)1, &usb_ghhub_Data[ptr->ip][hubaddr][0],
                                    &usb_hhub_TransResult);
                /* Port Clear */
                usb_shhub_IventPort[ptr->ip] = USB_HUB_P1;
                /* Sequence Clear */
                usb_shhub_IventSeq[ptr->ip] = USB_SEQ_0;
                usb_shhub_Process[ptr->ip] = USB_NONE;
                usb_shhub_Info[ptr->ip] = USB_NONE;
            }
            else
            {
                /* Next port check */
                /* Sequence Clear */
                usb_shhub_IventSeq[ptr->ip] = USB_SEQ_0;
                usb_shhub_IventPort[ptr->ip] ++;
                usb_hhub_SpecifiedPath(mess);
            }
            break;
            
        default:
            /* Port Clear */
            usb_shhub_IventPort[ptr->ip] = USB_HUB_P1;
            /* Sequence Clear */
            usb_shhub_IventSeq[ptr->ip] = USB_SEQ_0;
            usb_shhub_Process[ptr->ip] = USB_NONE;
            usb_shhub_Info[ptr->ip] = USB_NONE;
            break;
        }
        err = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)mess);
        if( err != USB_OK )
        {
            USB_PRINTF0("### USB HostHubClass rel_blk error\n");
        }
    }
}
/******************************************************************************
End of function usb_hhub_Ivent
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_Enumeration
Description     : USB HUB Class Enumeration
Arguments       : USB_CLSINFO_t *mess
Return value    : none
******************************************************************************/
void usb_hhub_Enumeration(USB_CLSINFO_t *ptr)
{
/* Condition compilation by the difference of useful function */
#if defined(USB_DEBUGUART_PP) || defined(USB_DEBUGPRINT_PP)
    uint8_t     pdata[32];
    uint16_t    j;
#endif  /* USB_DEBUGUART_PP || USB_DEBUGPRINT_PP */
    uint16_t    checkerr,retval;
    uint8_t     string;
    uint16_t    *table[8];

    checkerr = ptr->result;
    table[0] = (uint16_t*)usb_shhub_DeviceTable[ptr->ip];
    table[1] = (uint16_t*)usb_shhub_ConfigTable[ptr->ip];
    table[2] = (uint16_t*)usb_shhub_InterfaceTable[ptr->ip];


    /* Manager Mode Change */
    switch( usb_shhub_ClassSeq[ptr->ip] )
    {
    case USB_SEQ_0:
        checkerr = USB_DONE;
        /* Descriptor check */
        retval = usb_hhub_ChkConfig((uint16_t**)&table, (uint16_t)usb_shhub_Spec[ptr->ip]);
        if( retval == USB_ERROR )
        {
            USB_PRINTF0("### Configuration descriptor error !\n");
            checkerr = USB_ERROR;
            break;
        }
        /* Interface Descriptor check */
        retval = usb_hhub_ChkInterface((uint16_t**)&table, (uint16_t)usb_shhub_Spec[ptr->ip]);
        if( retval == USB_ERROR )
        {
            USB_PRINTF0("### Interface descriptor error !\n");
            checkerr = USB_ERROR;
            break;
        }

        /* String descriptor */
        usb_shhub_Process[ptr->ip] = USB_MSG_CLS_CHECKREQUEST;
        usb_hstd_GetStringDescriptor1(ptr, usb_shhub_DevAddr[ptr->ip], (uint16_t)15, (USB_CB_t)&usb_hhub_CheckResult);
        break;

    case USB_SEQ_1:
        /* String descriptor check */
        retval = usb_hstd_GetStringDescriptor1Check(checkerr);
        if( retval == USB_DONE )
        {
            string = usb_shhub_DeviceTable[ptr->ip][15];
            /* String descriptor */
            usb_shhub_Process[ptr->ip] = USB_MSG_CLS_CHECKREQUEST;
            usb_hstd_GetStringDescriptor2(ptr, usb_shhub_DevAddr[ptr->ip],
                                            (uint16_t)string, (USB_CB_t)&usb_hhub_CheckResult);
        }
        else
        {
            /* If USB_ERROR, Go case 3 (checkerr==USB_ERROR) */
            usb_shhub_ClassSeq[ptr->ip] = USB_SEQ_2;
            /* Next sequence */
            usb_hhub_CheckRequest(ptr, USB_ERROR);
            checkerr = USB_DONE;
        }
        break;
        
    case USB_SEQ_2:
        /* String descriptor check */
        retval = usb_hstd_GetStringDescriptor2Check(checkerr);
        if( retval == USB_DONE )
        {
            /* Next sequence */
            usb_hhub_CheckRequest(ptr, checkerr);
        }
        break;
    
    case USB_SEQ_3:
        /* String descriptor check */
        if( checkerr == USB_DONE )
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
/* Condition compilation by the difference of useful function */
#if defined(USB_DEBUGUART_PP) || defined(USB_DEBUGPRINT_PP)
            for( j = (uint16_t)0; j < usb_ghstd_ClassData[ptr->ip][0]; j++ )
            {
                pdata[j] = usb_ghstd_ClassData[ptr->ip][j * (uint16_t)2 + (uint16_t)2];
            }
            pdata[usb_ghstd_ClassData[ptr->ip][0]] = 0;
            USB_PRINTF1("    Product name : %s\n", pdata);
#endif  /* USB_DEBUGUART_PP || USB_DEBUGPRINT_PP */
        }
        else
        {
            USB_PRINTF0("*** Product name error\n");
            checkerr = USB_DONE;
        }
        
        /* Get HUB descriptor */
        usb_shhub_Process[ptr->ip] = USB_MSG_CLS_CHECKREQUEST;
        checkerr = R_usb_hhub_GetHubInformation(ptr, usb_shhub_DevAddr[ptr->ip], usb_hhub_CheckResult);
        /* Submit overlap error */
        if(checkerr == USB_HUB_QOVR)
        {
            usb_hhub_SpecifiedPathWait(ptr, (uint16_t)10);
        }
        break;
        
    case USB_SEQ_4:
        /* Get HUB descriptor Check */
        retval = usb_hhub_RequestResult(checkerr);
        if( retval == USB_DONE )
        {
            usb_hhub_CheckRequest(ptr, checkerr);   /* Next sequence */
        }
        break;
        
    case USB_SEQ_5:
        /* Get HUB descriptor Check */
        if( checkerr == USB_DONE )
        {
            retval = usb_hstd_CheckDescriptor(usb_ghhub_Descriptor[ptr->ip], (uint16_t)USB_DT_HUBDESCRIPTOR);
            if( retval == USB_ERROR )
            {
                USB_PRINTF0("### HUB descriptor error !\n");
                checkerr = USB_ERROR;
            }
            else if( usb_ghhub_Descriptor[ptr->ip][2] > USB_HUBDOWNPORT )
            {
                USB_PRINTF0("### HUB Port number over\n");
                checkerr = USB_ERROR;
            }
            else
            {
                USB_PRINTF1("    Attached %d port HUB\n", usb_ghhub_Descriptor[ptr->ip][2]);
            }
        }
        else
        {
            USB_PRINTF0("### HUB Descriptor over\n");
            checkerr = USB_ERROR;
        }

        /* Pipe Information table set */
        switch( usb_shhub_Spec[ptr->ip] )
        {
        case USB_FSHUB:     /* Full Speed Hub */
            if( usb_shhub_Speed[ptr->ip] == USB_FSCONNECT )
            {
                retval = usb_hhub_PipeInfo( ptr, (uint8_t *)usb_shhub_InterfaceTable[ptr->ip],
                                            usb_shhub_Index[ptr->ip], usb_shhub_Speed[ptr->ip],
                                            (uint16_t)usb_shhub_ConfigTable[ptr->ip][2]);
                if( retval == USB_ERROR )
                {
                    USB_PRINTF0("### Device information error(HUB) !\n");
                    checkerr = USB_ERROR;
                }
            }
            else
            {
                USB_PRINTF0("### HUB Descriptor speed error\n");
                checkerr = USB_ERROR;
            }
            break;
        case USB_HSHUBS:    /* Hi Speed Hub(Single) */
            if( usb_shhub_Speed[ptr->ip] == USB_HSCONNECT )
            {
                retval = usb_hhub_PipeInfo( ptr, (uint8_t *)usb_shhub_InterfaceTable[ptr->ip],
                                            usb_shhub_Index[ptr->ip], usb_shhub_Speed[ptr->ip],
                                            (uint16_t)usb_shhub_ConfigTable[ptr->ip][2]);
                if( retval == USB_ERROR )
                {
                    USB_PRINTF0("### Device information error(HUB) !\n");
                    checkerr = USB_ERROR;
                }
            }
            else
            {
                USB_PRINTF0("### HUB Descriptor speed error\n");
                checkerr = USB_ERROR;
            }
            break;
        case USB_HSHUBM:    /* Hi Speed Hub(Multi) */
            if( usb_shhub_Speed[ptr->ip] == USB_HSCONNECT )
            {
                /* Set pipe information */
                retval = usb_hhub_PipeInfo( ptr, (uint8_t *)usb_shhub_InterfaceTable[ptr->ip],
                                            usb_shhub_Index[ptr->ip], usb_shhub_Speed[ptr->ip],
                                            (uint16_t)usb_shhub_ConfigTable[ptr->ip][2]);
                if( retval == USB_ERROR )
                {
                    USB_PRINTF0("### Device information error(HUB) !\n");
                    checkerr = USB_ERROR;
                }
                /* Set pipe information */
                retval = usb_hhub_PipeInfo( ptr, (uint8_t *)usb_shhub_InterfaceTable[ptr->ip],
                                            usb_shhub_Index[ptr->ip], usb_shhub_Speed[ptr->ip],
                                            (uint16_t)usb_shhub_ConfigTable[ptr->ip][2]);
                if( retval == USB_ERROR )
                {
                    USB_PRINTF0("### Device information error(HUB) !\n");
                    checkerr= USB_ERROR;
                }
            }
            else
            {
                USB_PRINTF0("### HUB Descriptor speed error\n");
                checkerr = USB_ERROR;
            }
            break;
        default:
            checkerr = USB_ERROR;
            break;
        }
        /* Port number set */
        usb_shhub_InfoData[ptr->ip][usb_shhub_DevAddr[ptr->ip]][2] = usb_ghhub_Descriptor[ptr->ip][2];
        usb_shhub_Process[ptr->ip] = USB_NONE;
        /* Return to MGR */
        R_usb_hstd_ReturnEnuMGR((USB_UTR_t *)ptr, checkerr);
        break;
    }

    switch( checkerr )
    {
    /* Driver open */
    case USB_DONE:
        usb_shhub_ClassSeq[ptr->ip]++;
        break;
    /* Submit overlap error */
    case USB_HUB_QOVR:
        break;
    /* Descriptor error */
    case USB_ERROR:
        USB_PRINTF0("### Enumeration is stoped(ClassCode-ERROR)\n");
        usb_shhub_Process[ptr->ip] = USB_NONE;
        /* Return to MGR */
        R_usb_hstd_ReturnEnuMGR((USB_UTR_t *)ptr, USB_ERROR);
        break;
    default:
        usb_shhub_Process[ptr->ip] = USB_NONE;
        /* Return to MGR */
        R_usb_hstd_ReturnEnuMGR((USB_UTR_t *)ptr, USB_ERROR);
        break;
    }
}
/******************************************************************************
End of function usb_hhub_Enumeration
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_SpecifiedPath
Description     : Next Process Selector
Arguments       : USB_CLSINFO_t *mess
                : uint16_t status
Return value    : none
******************************************************************************/
void usb_hhub_SpecifiedPath(USB_CLSINFO_t *ptr)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *cp;

    /* Get mem pool blk */
    if( USB_PGET_BLK(USB_HUB_MPL, &p_blf) == USB_OK )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = usb_shhub_Process[ptr->ip];
        cp->keyword = ptr->keyword;
        cp->result  = ptr->result;

        cp->ipp = ptr->ipp;
        cp->ip  = ptr->ip;
        
        /* Send message */
        err = USB_SND_MSG( USB_HUB_MBX, (USB_MSG_t*)p_blf );
        if( err != USB_OK )
        {
            /* Send message failure */
            err = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### SpecifiedPass function snd_msg error\n");
        }
    }
    else
    {
        /* Get memoryblock failure */
        USB_PRINTF0("### SpecifiedPass function pget_blk error\n");
        while( 1 );
    }
}
/******************************************************************************
End of function usb_hhub_SpecifiedPath
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_SpecifiedPathWait
Description     : Next Process Selector
Arguments       : USB_CLSINFO_t *mess
                : uint16_t times
Return value    : none
******************************************************************************/
void usb_hhub_SpecifiedPathWait(USB_CLSINFO_t *ptr, uint16_t times)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *hp;

    /* Get mem pool blk */
    if( USB_PGET_BLK(USB_HUB_MPL, &p_blf) == USB_OK )
    {
        hp = (USB_CLSINFO_t*)p_blf;
        hp->msginfo = usb_shhub_Process[ptr->ip];
        hp->keyword = ptr->keyword;
        hp->result  = ptr->result;

        hp->ipp = ptr->ipp;
        hp->ip  = ptr->ip;

        /* Wait message */
        err = USB_WAI_MSG( USB_HUB_MBX, (USB_MSG_t*)p_blf, times );
        if( err != USB_OK )
        {
            /* Wait message failure */
            err = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### SpecifiedPassWait function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### SpecifiedPassWait function pget_blk error\n");
        while( 1 );
    }
}
/******************************************************************************
End of function usb_hhub_SpecifiedPathWait
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_CheckResult
Description     : Hub class check result
Arguments       : USB_UTR_t *mess    : USB system internal message.
Return value    : none
******************************************************************************/
void usb_hhub_CheckResult(USB_UTR_t *ptr, uint16_t data1, uint16_t data2)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *cp;
    
    /* Get mem pool blk */
    if( USB_PGET_BLK(USB_HUB_MPL, &p_blf) == USB_OK )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = usb_shhub_Process[ptr->ip];
        cp->keyword = ptr->keyword;
        cp->result  = ptr->status;

        cp->ipp = ptr->ipp;
        cp->ip  = ptr->ip;

        /* Send message */
        err = USB_SND_MSG( USB_HUB_MBX, (USB_MSG_t*)p_blf );
        if( err != USB_OK )
        {
            /* Send message failure */
            err = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### CheckResult function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### CheckResult function pget_blk error\n");
        while( 1 );
    }
}
/******************************************************************************
End of function usb_hhub_CheckResult
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_CheckRequest
Description     : Class check request
Arguments       : uint16_t result
Return value    : none
******************************************************************************/
void usb_hhub_CheckRequest(USB_UTR_t *ptr, uint16_t result)
{
    USB_MH_t        p_blf;
    USB_ER_t        err;
    USB_CLSINFO_t   *cp;
    
    /* Get mem pool blk */
    if( USB_PGET_BLK(USB_HUB_MPL, &p_blf) == USB_OK )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = USB_MSG_CLS_CHECKREQUEST;
        cp->result  = result;

        cp->ipp = ptr->ipp;
        cp->ip  = ptr->ip;

        /* Send message */
        err = USB_SND_MSG( USB_HUB_MBX, (USB_MSG_t*)p_blf );
        if( err != USB_OK )
        {
            /* Send message failure */
            err = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### CheckRequest function snd_msg error\n");
        }
    }
    else
    {
        /* Get memoryblock failure */
        USB_PRINTF0("### CheckRequest function pget_blk error\n");
        while( 1 );
    }
    
}
/******************************************************************************
End of function usb_hhub_CheckRequest
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_ClearStallResult
Description     : Clear Stall Result
Arguments       : USB_UTR_t *mess    : USB system internal message.
Return value    : none
******************************************************************************/
void usb_hhub_ClearStallResult(USB_UTR_t *mess, uint16_t data1, uint16_t data2)
{
}
/******************************************************************************
End of function usb_hhub_ClearStallResult
******************************************************************************/


/******************************************************************************
Function Name   : usb_hhub_RequestResult
Description     : Hub Request Result Check
Arguments       : uint16_t errcheck         : hub result
Return value    : uint16_t                  : USB_DONE/USB_ERROR
******************************************************************************/
uint16_t usb_hhub_RequestResult(uint16_t errcheck)
{
    if( errcheck == USB_DATA_TMO )
    {
        USB_PRINTF0("*** HUB Request Timeout error !\n");
        return USB_ERROR;
    }
    else if( errcheck == USB_DATA_STALL )
    {
        USB_PRINTF0("*** HUB Request STALL !\n");
        return USB_ERROR;
    }
    else if( errcheck != USB_CTRL_END )
    {
        USB_PRINTF0("*** HUB Request error !\n");
        return USB_ERROR;
    }
    else
    {
    }
    return USB_DONE;
}
/******************************************************************************
End of function usb_hhub_RequestResult
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_hhub_get_hub_addr
Description     : Get connected Hub address from device address.
Argument        : USB_UTR_t *ptr    : IP info (mode, IP no, reg. address).
                : uint16_t devadr           : Device address
Return          : 1-10:Hub address / USB_ERROR:Device address error or Hub no connect.
******************************************************************************/
uint16_t R_usb_hhub_get_hub_addr(USB_UTR_t *ptr, uint16_t devadr)
{
    uint16_t ret;

    /* Device address check */
    if( ( USB_MAXDEVADDR < devadr ) || ( 0 == devadr ) )
    {
        ret = USB_ERROR;                /* Device address error */
    }
    else
    {
        /* Set Hub address */
        ret = usb_shhub_InfoData[ptr->ip][devadr][0];
        /* Hub address check */
        if( ( USB_MAXDEVADDR < ret ) || ( 0 == ret ) )
        {
            ret = USB_ERROR;            /* Hub address error */
        }
    }

    return ret;
} /* eof R_usb_hhub_get_hub_addr */

/******************************************************************************
Function Name   : R_usb_hhub_get_hub_port_no
Description     : Get Hub connected port no. from device address.
Argument        : USB_UTR_t *ptr    : IP info (mode, IP no, reg. address).
                : uint16_t devadr           : Device address
Return          : 1-4:Hub port no. / USB_ERROR:Device address error or Hub no connect.
******************************************************************************/
uint16_t R_usb_hhub_get_hub_port_no(USB_UTR_t *ptr, uint16_t devadr)
{
    uint16_t ret;

    /* Device address check */
    if( ( USB_MAXDEVADDR < devadr ) || ( 0 == devadr ) )
    {
        ret = USB_ERROR;                /* Device address error */
    }
    else
    {
        /* Set Hub port no */
        ret = usb_shhub_InfoData[ptr->ip][devadr][1];
        /* Hub port no check */
        if( ( USB_HUBDOWNPORT < ret ) || ( 0 == ret ) )
        {
            ret = USB_ERROR;            /* Hub port no error */
        }
    }

    return ret;
} /* eof R_usb_hhub_get_hub_port_no */

/******************************************************************************
Function Name   : R_usb_hhub_chk_connect_status
Description     : Device connect check of after GET_STATUS(Get Port Status) complete.
Argument        : USB_UTR_t *ptr    : IP info (mode, IP no, reg. address).
                : uint16_t hub_adr          : Hub address
Return          : USB_DEV_NO_CONNECT:No device is present.
                : USB_DEV_CONNECTED:A device is present on this port.
                : USB_ERROR: Hub address error.
******************************************************************************/
uint16_t R_usb_hhub_chk_connect_status(USB_UTR_t *ptr, uint16_t hub_adr)
{
    uint16_t ret;

    /* Hub address check */
    if( ( USB_MAXDEVADDR < hub_adr ) || ( 0 == hub_adr ) )
    {
        ret = USB_ERROR;                /* Hub address error */
    }
    else
    {
        /* Port Status : Current Connect Status */
        if( usb_ghhub_Data[ptr->ip][hub_adr][0] & USB_BIT0 )
        {
            ret = USB_DEV_CONNECTED;    /* A device is present on this port. */
        }
        else
        {
            ret = USB_DEV_NO_CONNECT;   /* No device is present. */
        }
    }

    return ret;
} /* eof R_usb_hhub_chk_connect_status */

#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */

/******************************************************************************
End  Of File
******************************************************************************/
