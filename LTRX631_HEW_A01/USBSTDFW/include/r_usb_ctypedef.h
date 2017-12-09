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
* File Name    : r_usb_ctypedef.h
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : Type Definition Header File
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
******************************************************************************/

/* $Id: r_usb_ctypedef.h 143 2012-05-07 09:16:46Z tmura $ */

#ifndef __R_USB_CTYPEDEF_H__
#define __R_USB_CTYPEDEF_H__


/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdint.h>
#include "r_usb_usrconfig.h"
#include "r_usb_citron.h"


/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct st_usb       USB_STNBYINT_t;

typedef void (*USB_CB_CHECK_t)(struct USB_UTR *, uint16_t**);
typedef void (*USB_CB_t)(struct USB_UTR *, uint16_t, uint16_t);
typedef void (*USB_CB_INFO_t)(struct USB_UTR *, uint16_t, uint16_t);
typedef void (*USB_CB_MSG_t)(uint16_t, uint8_t*);
typedef void (*USB_CB_REQ_t)(void *);

#if USB_TARGET_CHIP_PP == USB_ASSP_PP
typedef struct  USB_REGISTER*   USB_REGADR_t;
#else
typedef volatile struct st_usb0 __evenaccess*   USB_REGADR_t;
#endif

typedef struct
{
    uint16_t        ReqType;            /* Request type */
    uint16_t        ReqTypeType;        /* Request type TYPE */
    uint16_t        ReqTypeRecip;       /* Request type RECIPIENT */
    uint16_t        ReqRequest;         /* Request */
    uint16_t        ReqValue;           /* Value */
    uint16_t        ReqIndex;           /* Index */
    uint16_t        ReqLength;          /* Length */
} USB_REQUEST_t;

typedef struct USB_HCDREG
{
    uint16_t        rootport;       /* Root port */
    uint16_t        devaddr;        /* Device address */
    uint16_t        devstate;       /* Device state */
    uint16_t        ifclass;        /* Interface Class */
    uint16_t        *tpl;           /* Target peripheral list 
                                        (Vendor ID, Product ID) */
    uint16_t        *pipetbl;       /* Pipe Define Table address */
    USB_CB_INFO_t   classinit;      /* Driver init */
    USB_CB_CHECK_t  classcheck;     /* Driver check */
    USB_CB_INFO_t   devconfig;      /* Device configured */
    USB_CB_INFO_t   devdetach;      /* Device detach */
    USB_CB_INFO_t   devsuspend;     /* Device suspend */
    USB_CB_INFO_t   devresume;      /* Device resume */
    USB_CB_INFO_t   overcurrent;    /* Device over current */
} USB_HCDREG_t;

typedef struct USB_UTR
{
    USB_MH_t        msghead;        /* Message header (for SH-solution) */
    uint16_t        msginfo;        /* Massage Info for F/W */
    uint16_t        keyword;        /* Rootport / Device address / Pipe number */
    USB_REGADR_t    ipp;            /* IP Address(USB0orUSB1)*/
    uint16_t        ip;             /* IP number(0or1) */
    uint16_t        result;         /* Result */
    USB_CB_t        complete;       /* Call Back Function Info */
    void            *tranadr;       /* Transfer data Start address */
    uint32_t        tranlen;        /* Transfer data length */
    uint16_t        *setup;         /* Setup packet(for control only) */
    uint16_t        status;         /* Status */
    uint16_t        pipectr;        /* Pipe control register */
    uint8_t         errcnt;         /* Error count */
    uint8_t         segment;        /* Last flag */
    int16_t         fn;             /* File Number */
    void            *usr_data;      
} USB_UTR_t;

/* Class request processing function type. */
typedef void (*USB_CB_TRN_t)(USB_UTR_t *, USB_REQUEST_t*, uint16_t ctsq);

typedef struct USB_PCDREG
{
    uint16_t        **pipetbl;      /* Pipe Define Table address */
    uint8_t         *devicetbl;     /* Device descriptor Table address */
    uint8_t         *qualitbl;      /* Qualifier descriptor Table address */
    uint8_t         **configtbl;    /* Configuration descriptor
                                        Table address */
    uint8_t         **othertbl;     /* Other configuration descriptor
                                        Table address */
    uint8_t         **stringtbl;    /* String descriptor Table address */
    USB_CB_INFO_t   classinit;      /* Driver init */
    USB_CB_INFO_t   devdefault;     /* Device default */
    USB_CB_INFO_t   devconfig;      /* Device configured */
    USB_CB_INFO_t   devdetach;      /* Device detach */
    USB_CB_INFO_t   devsuspend;     /* Device suspend */
    USB_CB_INFO_t   devresume;      /* Device resume */
    USB_CB_INFO_t   interface;      /* Interface changed */
    USB_CB_TRN_t    ctrltrans;      /* Control Transfer */
} USB_PCDREG_t;

typedef struct USB_UTR      USB_HCDINFO_t;
typedef struct USB_UTR      USB_MGRINFO_t;
typedef struct USB_UTR      USB_PCDINFO_t;
typedef struct USB_UTR      USB_CLSINFO_t;

typedef struct usb_fninfo {
    uint8_t         isOpened;
    uint8_t         usb_ip;
    USB_REGADR_t    usb_ipp;            /* IP Address(USB0orUSB1)*/
    uint8_t         devno;
    uint8_t         inPipeNo;
    uint8_t         outPipeNo;
    int32_t         (*class_read)( int16_t, void *, uint32_t );
    int32_t         (*class_write)( int16_t , void *, uint32_t );
    uint16_t        (*close_func)( int16_t );

    USB_CB_t        read_complete;      /* Call Back Function Info */
    USB_CB_t        write_complete;     /* Call Back Function Info */
    int32_t         read_length;        /* Transfer data length */
    int32_t         write_length;       /* Transfer data length */
//  uint16_t        *class_control();
    int16_t         open_state;         /* Open status */
    int16_t         read_state;         /* read status */
    int16_t         write_state;        /* write status */
} USB_FNINFO_t;

struct usb_devinfo 
{
    uint8_t         devadr;
    uint8_t         speed;
    uint8_t         isTPL;
    uint8_t         interfaceClass;
    uint8_t         isActDev;
    USB_UTR_t       *ptr;
};

typedef struct usb_open_parametor
{
    USB_FNINFO_t *p_fnstr;
    uint8_t devadr;
    uint8_t speed;
    uint16_t class;
} USB_OPEN_PARM_t;

/*****************************************************************************
Macro definitions
******************************************************************************/
#define USB_NONE            (uint16_t)(0)
#define USB_YES             (uint16_t)(1)
#define USB_NO              (uint16_t)(0)
#define USB_DONE            (uint16_t)(0)
#define USB_ERROR           (uint16_t)(0xFFFF)
#define USB_OK              (uint16_t)(0)
#define USB_NG              (uint16_t)(0xFFFF)
#define USB_ON              (uint16_t)(1)
#define USB_OFF             (uint16_t)(0)
#define USB_OTG_DONE        (uint16_t)(2)

#endif  /* __R_USB_CTYPEDEF_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
