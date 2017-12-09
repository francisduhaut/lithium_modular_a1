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
* File Name    : r_usb_hmsc_driver.c
* Version      : 1.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB Host MSC BOT driver
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.08.2010 0.91    First Release
*         : 06.08.2010 0.92    Updated comments
*         : 29.10.2010 1.00    Mass Production Release
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
//#include "r_usb_cDefUsrPp.h"      /* System definition */
#include "r_usb_cmsc_define.h"      /* USB  Mass Storage Class Header */
#include "r_usb_cDefUSBIP.h"        /* USB-FW Library Header */
#include "r_usb_cMacSystemcall.h"   /* System call macro */
#include "r_usb_cMacPrint.h"        /* LCD/SIO disp macro */
#include "r_usb_cExtern.h"          /* USB-FW grobal define */
#include "r_usb_cKernelId.h"        /* Kernel ID definition */
#include "r_usb_usrconfig.h"        /* USB-H/W register set (user define) */
#include "r_usb_catapi_define.h"        /* Peripheral ATAPI Device extern */
#include "r_usb_hmsc_define.h"      /* Host Mass Storage Class Driver */
#include "r_usb_hmsc_extern.h"      /* Host MSC grobal define */

#include "r_usb_ansi.h"
#include "r_usb_api.h"

/******************************************************************************
Section    <Section Difinition> , "Project Sections"
******************************************************************************/
#pragma section _hmsc


/******************************************************************************
Private global variables and functions
******************************************************************************/
uint16_t            usb_ghmsc_AttSts;
/* CBW headder */
USB_MSC_CBW_t       usb_ghmsc_Cbw[USB_MAXSTRAGE];
/* CSW headder */
USB_MSC_CSW_t       usb_ghmsc_Csw[USB_MAXSTRAGE];
uint32_t            usb_ghmsc_CswDataResidue[USB_MAXSTRAGE];
/* CBW tag number */
uint32_t            usb_ghmsc_CbwTagNo[USB_MAXSTRAGE];
uint8_t             usb_ghmsc_AtapiFlag[USB_MAXSTRAGE];

uint32_t            usb_ghmsc_Secno;
uint32_t            usb_ghmsc_TransSize;
uint16_t            usb_ghmsc_Side;
uint16_t            usb_ghmsc_Seccnt;
uint8_t             *usb_ghmsc_Buff;
uint16_t            usb_shmsc_Process;
uint16_t            usb_shmsc_NoDataSeq     = USB_SEQ_0;
uint16_t            usb_shmsc_DataInSeq     = USB_SEQ_0;
uint16_t            usb_shmsc_DataOutSeq    = USB_SEQ_0;
uint16_t            usb_shmsc_StallErrSeq   = USB_SEQ_0;
uint16_t            usb_shmsc_DataStallSeq  = USB_SEQ_0;
uint16_t            usb_ghmsc_RootDrive;
uint16_t            usb_ghmsc_CswErrRoop    = USB_OFF;

extern uint16_t     usb_shmsc_StrgProcess;

void usb_hmsc_CommandResult(USB_UTR_t *ptr, uint16_t result);

/******************************************************************************
Renesas Abstracted HMSC Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_Task
Description     : USB HMSC Task
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hmsc_Task(void)
{
    USB_UTR_t       *mess;
    /* Error code */
    USB_ER_t        err;

    /* Receive message */
    err = USB_TRCV_MSG( USB_HMSC_MBX, (USB_MSG_t**)&mess, (uint16_t)0 );
    if( err != USB_E_OK )
    {
        return;
    }

    switch( mess->msginfo )
    {
    case USB_MSG_CLS_INIT:
        usb_hmsc_ClassCheck(mess, (USB_CLSINFO_t *)mess);
        break;

    case USB_MSG_HMSC_NO_DATA:
        usb_hmsc_NoDataAct((USB_CLSINFO_t *)mess);
        break;
    case USB_MSG_HMSC_DATA_IN:
        usb_hmsc_DataInAct((USB_CLSINFO_t *)mess);
        break;
    case USB_MSG_HMSC_DATA_OUT:
        usb_hmsc_DataOutAct((USB_CLSINFO_t *)mess);
        break;
    case USB_MSG_HMSC_DATA_STALL:
        usb_hmsc_DataStall((USB_UTR_t *)mess);
        break;
    case USB_MSG_HMSC_CBW_ERR:
        usb_hmsc_StallErr((USB_UTR_t *)mess);
        break;      
    case USB_MSG_HMSC_CSW_PHASE_ERR:
        usb_hmsc_StallErr((USB_UTR_t *)mess);
        break;
/* enumeration waiting of other device */
    case USB_MSG_CLS_WAIT:
        usb_hmsc_ClassWait(USB_HMSC_MBX, mess);
        break;
    default:
        break;
    }
    err = USB_REL_BLK(USB_HMSC_MPL,(USB_MH_t)mess);
    if( err != USB_E_OK )
    {
        USB_PRINTF0("### USB Strg Task rel_blk error\n");
    }
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_SetRwCbw
Description     : CBW parameter initialization for the READ10/WRITE10 command
Arguments       : uint16_t command : 
                : uint32_t secno : 
                : uint16_t seccnt : 
                : uint32_t trans_byte : 
                : uint16_t side : 
Return value    : none
******************************************************************************/
void usb_hmsc_SetRwCbw(uint16_t command, uint32_t secno, uint16_t seccnt,
     uint32_t trans_byte, uint16_t side)
{
    uint16_t    msgnum;

    /* Data IN */
    msgnum = usb_hmsc_SmpDrive2Msgnum(side);

    /* CBW parameter set */
    usb_ghmsc_Cbw[msgnum].dCBWTag = usb_ghmsc_CbwTagNo[msgnum];
    usb_ghmsc_Cbw[msgnum].dCBWDTL_Lo = (uint8_t)trans_byte;
    usb_ghmsc_Cbw[msgnum].dCBWDTL_ML = (uint8_t)(trans_byte >> 8);
    usb_ghmsc_Cbw[msgnum].dCBWDTL_MH = (uint8_t)(trans_byte >> 16);
    usb_ghmsc_Cbw[msgnum].dCBWDTL_Hi = (uint8_t)(trans_byte >> 24);
    usb_ghmsc_Cbw[msgnum].bmCBWFlags.CBWdir = 0;
    usb_ghmsc_Cbw[msgnum].bmCBWFlags.reserved7 = 0;
    usb_ghmsc_Cbw[msgnum].bCBWLUN.bCBWLUN = usb_hmsc_SmpDrive2Unit(side);
    usb_ghmsc_Cbw[msgnum].bCBWLUN.reserved4 = 0;
    usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 0;
    usb_ghmsc_Cbw[msgnum].bCBWCBLength.reserved3 = 0;

    /* ATAPI_COMMAND */
    usb_ghmsc_Cbw[msgnum].CBWCB[0] = (uint8_t)command;
    /* LUN */
    usb_ghmsc_Cbw[msgnum].CBWCB[1] = 0x00;
    /* sector address */
    usb_ghmsc_Cbw[msgnum].CBWCB[2] = (uint8_t)(secno >> 24);
    usb_ghmsc_Cbw[msgnum].CBWCB[3] = (uint8_t)(secno >> 16);
    usb_ghmsc_Cbw[msgnum].CBWCB[4] = (uint8_t)(secno >> 8);
    usb_ghmsc_Cbw[msgnum].CBWCB[5] = (uint8_t)secno;
    /* Reserved */
    usb_ghmsc_Cbw[msgnum].CBWCB[6] = 0x00;
    /* Sector length */
    usb_ghmsc_Cbw[msgnum].CBWCB[7] = (uint8_t)(seccnt >> 8);
    /* Block address */
    usb_ghmsc_Cbw[msgnum].CBWCB[8] = (uint8_t)seccnt;
    /* Control data */
    usb_ghmsc_Cbw[msgnum].CBWCB[9] = (uint8_t)0x00;

    /* ATAPI command check */
    switch( command )
    {
    case USB_ATAPI_TEST_UNIT_READY:
    case USB_ATAPI_REQUEST_SENSE:
    case USB_ATAPI_INQUIRY:
    case USB_ATAPI_MODE_SELECT6:
    case USB_ATAPI_MODE_SENSE6:
    case USB_ATAPI_START_STOP_UNIT:
    case USB_ATAPI_PREVENT_ALLOW:
    case USB_ATAPI_READ_FORMAT_CAPACITY:
    case USB_ATAPI_READ_CAPACITY:
        USB_PRINTF0("### Non-mounted command demand generating !\n");
        break;
    /* Initialized READ CBW TAG */
    case USB_ATAPI_READ10:
        usb_ghmsc_Cbw[msgnum].bmCBWFlags.CBWdir = 1;
        /* 10bytes */
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 10;
        break;
    /* Initialized WRITE CBW TAG */
    case USB_ATAPI_WRITE10:
        usb_ghmsc_Cbw[msgnum].bmCBWFlags.CBWdir = 0;
        /* 10bytes */
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 10;
        break;
    case USB_ATAPI_SEEK:
    case USB_ATAPI_WRITE_AND_VERIFY:
    case USB_ATAPI_VERIFY10:
    case USB_ATAPI_MODE_SELECT10:
    case USB_ATAPI_MODE_SENSE10:
    default:
        USB_PRINTF0("### Non-mounted command demand generating !\n");
        break;
    }

    if( usb_ghmsc_AtapiFlag[msgnum] == USB_ATAPI )
    {
        /* 12bytes */
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength
        = USB_MSC_CBWCB_LENGTH;
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SetElsCbw
Description     : CBW parameter initialization for other commands
Arguments       : uint8_t *data : 
                : uint32_t trans_byte : 
                : uint16_t side : 
Return value    : none
******************************************************************************/
void usb_hmsc_SetElsCbw(uint8_t *data, uint32_t trans_byte, uint16_t side)
{
    uint8_t     i;
    uint16_t    msgnum;

    /* Data IN */
    msgnum = usb_hmsc_SmpDrive2Msgnum(side);

    /* CBW parameter set */
    usb_ghmsc_Cbw[msgnum].dCBWTag = usb_ghmsc_CbwTagNo[msgnum];
    usb_ghmsc_Cbw[msgnum].dCBWDTL_Lo = (uint8_t)trans_byte;
    usb_ghmsc_Cbw[msgnum].dCBWDTL_ML = (uint8_t)(trans_byte >> 8);
    usb_ghmsc_Cbw[msgnum].dCBWDTL_MH = (uint8_t)(trans_byte >> 16);
    usb_ghmsc_Cbw[msgnum].dCBWDTL_Hi = (uint8_t)(trans_byte >> 24);
    /* Receive */
    usb_ghmsc_Cbw[msgnum].bmCBWFlags.CBWdir = 0;
    usb_ghmsc_Cbw[msgnum].bmCBWFlags.reserved7 = 0;
    usb_ghmsc_Cbw[msgnum].bCBWLUN.bCBWLUN = usb_hmsc_SmpDrive2Unit(side);
    usb_ghmsc_Cbw[msgnum].bCBWLUN.reserved4 = 0;
    usb_ghmsc_Cbw[msgnum].bCBWCBLength.reserved3 = 0;

    for( i = 0; i < 12; i++ )
    {
        usb_ghmsc_Cbw[msgnum].CBWCB[i] = data[i];
    }

    /* ATAPI command check */
    switch( data[0] )
    {
    /* No data */
    case USB_ATAPI_TEST_UNIT_READY:
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 6;
        break;
    /* Receive */
    case USB_ATAPI_REQUEST_SENSE:
        usb_ghmsc_Cbw[msgnum].bmCBWFlags.CBWdir = 1;
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 6;
        break;
    /* Send */
    case USB_ATAPI_FORMAT_UNIT:
        USB_PRINTF0("### Non-mounted command demand generating !\n");
        break;
    /* Receive */
    case USB_ATAPI_INQUIRY:
        usb_ghmsc_Cbw[msgnum].bmCBWFlags.CBWdir = 1;
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 6;
        break;
    case USB_ATAPI_MODE_SELECT6:
    case USB_ATAPI_MODE_SENSE6:
        break;
    /* No data */
    case USB_ATAPI_START_STOP_UNIT:
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 6;
        break;
    /* No data */
    case USB_ATAPI_PREVENT_ALLOW:
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 6;
        break;
    /* Receive */
    case USB_ATAPI_READ_FORMAT_CAPACITY:
        usb_ghmsc_Cbw[msgnum].bmCBWFlags.CBWdir = 1;
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 10;
        break;
    /* Receive */
    case USB_ATAPI_READ_CAPACITY:
        usb_ghmsc_Cbw[msgnum].bmCBWFlags.CBWdir = 1;
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 10;
        break;
    case USB_ATAPI_READ10:
    case USB_ATAPI_WRITE10:
        USB_PRINTF0("### Non-mounted command demand generating !\n");
        break;
    case USB_ATAPI_SEEK:
    case USB_ATAPI_WRITE_AND_VERIFY:
    case USB_ATAPI_VERIFY10:
        USB_PRINTF0("### Non-mounted command demand generating !\n");
        break;
    /* Send */
    case USB_ATAPI_MODE_SELECT10:
        USB_PRINTF0("### Non-mounted command demand generating !\n");
        break;
    /* Receive */
    case USB_ATAPI_MODE_SENSE10:
        usb_ghmsc_Cbw[msgnum].bmCBWFlags.CBWdir = 1;
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 10;
        break;
    default:
        USB_PRINTF0("### Non-mounted command demand generating !\n");
        break;
    }

    if( usb_ghmsc_AtapiFlag[msgnum] == USB_ATAPI )
    {
        /* 12bytes */
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength
        = USB_MSC_CBWCB_LENGTH;
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_CbwTagCount
Description     : Updates tag information
Arguments       : uint16_t msgnum : 
Return value    : none
******************************************************************************/
void usb_hmsc_CbwTagCount(uint16_t msgnum)
{
    usb_ghmsc_CbwTagNo[msgnum]++;
    if( usb_ghmsc_CbwTagNo[msgnum] == (uint16_t)0 )
    {
        usb_ghmsc_CbwTagNo[msgnum] = (uint16_t)1;
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_CheckCsw
Description     : CSW check
Arguments       : uint16_t drvnum : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_CheckCsw(USB_UTR_t *ptr, uint16_t drvnum)
{
    uint16_t    msgnum;
    USB_CLSINFO_t *mess;
    
    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);

    /* CSW Check */
    usb_ghmsc_CswDataResidue[msgnum]    =
                 (uint32_t)(usb_ghmsc_Csw[msgnum].dCSWDataResidue_Lo)
                | (uint32_t)(usb_ghmsc_Csw[msgnum].dCSWDataResidue_ML) << 8
                | (uint32_t)(usb_ghmsc_Csw[msgnum].dCSWDataResidue_MH) << 16
                | (uint32_t)(usb_ghmsc_Csw[msgnum].dCSWDataResidue_Hi) << 24;

    if( usb_ghmsc_Csw[msgnum].dCSWSignature != USB_MSC_CSW_SIGNATURE )
    {
        USB_PRINTF2("### CSW signature error 0x%08x:SIGN=0x%08x.\n",
             usb_ghmsc_Csw[msgnum].dCSWSignature, USB_MSC_CSW_SIGNATURE);
        return USB_HMSC_CSW_ERR;
    }
    if( usb_ghmsc_Csw[msgnum].dCSWTag != usb_ghmsc_Cbw[msgnum].dCBWTag )
    {
        USB_PRINTF2("### CSW Tag error 0x%08x:CBWTAG=0x%08x.\n",
             usb_ghmsc_Csw[msgnum].dCSWTag, usb_ghmsc_Cbw[msgnum].dCBWTag);
        return USB_HMSC_CSW_ERR;
    }
    switch( usb_ghmsc_Csw[msgnum].bCSWStatus )
    {
    case USB_MSC_CSW_OK:
        return USB_HMSC_OK;
        break;
    case USB_MSC_CSW_NG:
        return USB_HMSC_CSW_ERR;
        break;
    case USB_MSC_CSW_PHASE_ERR:
        usb_shmsc_Process = USB_MSG_HMSC_CSW_PHASE_ERR;
        usb_shmsc_StallErrSeq = USB_SEQ_0;
        mess->keyword = drvnum;
        usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess );
        return USB_HMSC_CSW_PHASE_ERR;
        break;
    default:
        break;
    }
    USB_PRINTF1("### CSW status error 0x%2x.\n", usb_ghmsc_Csw[msgnum].bCSWStatus);
    return USB_HMSC_CSW_ERR;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_SetDevSts
Description     : Sets HMSCD operation state
Arguments       : uint16_t data : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_SetDevSts(uint16_t data)
{
    usb_ghmsc_AttSts = data;
    return USB_DONE;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_GetDevSts
Description     : Responds to HMSCD operation state
Arguments       : none
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_GetDevSts(void)
{
    return(usb_ghmsc_AttSts);
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_hmsc_Information
Description     : EP Table Information
Arguments       : uint16_t pipe_offset : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_Information(uint16_t pipe_offset)
{
    return(usb_ghmsc_TmpEpTbl[pipe_offset]);
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_SmpBotDescriptor
Description     : BOT Descriptor
Arguments       : uint8_t *table : 
                : uint16_t msgnum : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_SmpBotDescriptor(uint8_t *table, uint16_t msgnum)
{
    /* Check Descriptor */
    switch( table[1] )
    {
    /* Device Descriptor */
    case USB_DT_DEVICE:
        USB_PRINTF0("### Not Interface descriptor (Device Desc).\n");
        return USB_ERROR;
        break;
    /* Configuration Descriptor */
    case USB_DT_CONFIGURATION:
        USB_PRINTF0("### Not Interface descriptor (Config Desc).\n");
        return USB_ERROR;
        break;
    /* String Descriptor */
    case USB_DT_STRING:
        USB_PRINTF0("### Not Interface descriptor (String Desc).\n");
        return USB_ERROR;
        break;
    /* Interface Descriptor ? */
    case USB_DT_INTERFACE:
        /* Check Interface Descriptor (deviceclass) */
        if( table[5] != USB_IFCLS_MAS )
        {
            USB_PRINTF1("### Interface deviceclass is %x , not support.\n", table[5]);
            return USB_ERROR;
        }
        /* Check Interface Descriptor (subclass) */
        if( table[6] == USB_ATAPI )
        {
            USB_PRINTF0("    Interface subclass  : SFF-8070i\n");
            /* ATAPI Command */
            usb_ghmsc_AtapiFlag[msgnum] = USB_ATAPI;
        }
        else if (table[6] == USB_SCSI)
        {
            USB_PRINTF0(
                "Interface subclass  : SCSI transparent command set\n");
            /* SCSI Command */
            usb_ghmsc_AtapiFlag[msgnum] = USB_SCSI;
        }
        else if (table[6] == USB_ATAPI_MMC5)
        {
            USB_PRINTF0("   Interface subclass   : ATAPI command set\n");
            /* ATAPI Command */
            usb_ghmsc_AtapiFlag[msgnum] = USB_ATAPI_MMC5;
        }
        else
        {
            USB_PRINTF1("### Interface subclass is %x , not support.\n", table[6]);
            /* Unknown Command */
            usb_ghmsc_AtapiFlag[msgnum] = USB_NONE;
            return USB_ERROR;
        }
        /* Check Interface Descriptor (protocol) */
        if( table[7] == USB_BOTP )
        {
            USB_PRINTF0("   Interface protocol   : BOT \n");
        }
        else
        {
            USB_PRINTF1("### Interface protocol is %x , not support.\n", table[7]);
            return USB_ERROR;
        }
        /* Check Endpoint number */
        if( table[4] < USB_TOTALEP )
        {
            USB_PRINTF1("### Endpoint number is %x , less than 2.\n", table[4]);
            return USB_ERROR;
        }
        return USB_DONE;
        break;
    /* Endpoint Descriptor */
    case USB_DT_ENDPOINT:
        USB_PRINTF0("### Not Interface descrip (Endpoint Desc).\n");
        return USB_ERROR;
        break;
    /* Device Qualifier Descriptor */
    case USB_DT_DEVICE_QUALIFIER:
        USB_PRINTF0("### Not Interface descrip (Dev Qualifier Desc).\n");
        return USB_ERROR;
        break;
    /* Other Speed Configuration Descriptor */
    case USB_DT_OTHER_SPEED_CONF:
        USB_PRINTF0("### Not Interface descrip (Other Speed Config Desc).\n");
        return USB_ERROR;
        break;
    /* Interface Power Descriptor */
    case USB_DT_INTERFACE_POWER:
        USB_PRINTF0("### Not Interface descrip (Interface Power Desc).\n");
        return USB_ERROR;
        break;
    /* Not Descriptor */
    default:
        USB_PRINTF0("### Not Interface descrip ( Not Standard Desc ).\n");
        break;
    }
    return USB_ERROR;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SmpPipeInfo
Description     : Pipe Information
Arguments       : uint8_t *table : 
                : uint16_t msgnum : 
                : uint16_t speed : 
                : uint16_t length : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_SmpPipeInfo(uint8_t *table, uint16_t msgnum, uint16_t speed, uint16_t length)
{
    uint16_t        dirflag = 0;
    uint16_t        ofdsc, offset;
    uint16_t        retval;

    /* Check Descriptor */
    if( table[1] != USB_DT_INTERFACE )
    {
        /* Configuration Descriptor */
        USB_PRINTF0("### Not Interface descriptor.\n");
        return USB_ERROR;
    }

    offset = (uint16_t)(2u * USB_EPL * msgnum);

    /* Check Endpoint Descriptor */
    ofdsc = table[0];
    /* Pipe initial */
    usb_ghmsc_OutPipe[msgnum][0]    = USB_NOPORT;
    usb_ghmsc_InPipe[msgnum][0]     = USB_NOPORT;
    /* Toggle clear */
    usb_ghmsc_OutPipe[msgnum][1]    = 0;
    usb_ghmsc_InPipe[msgnum][1]     = 0;
    while( ofdsc < (length - table[0]) )
    {
        /* Search within Interface */
        switch( table[ofdsc + 1] )
        {
        /* Device Descriptor ? */
        case USB_DT_DEVICE:
        /* Configuration Descriptor ? */
        case USB_DT_CONFIGURATION:
        /* String Descriptor ? */
        case USB_DT_STRING:
        /* Interface Descriptor ? */
        case USB_DT_INTERFACE:
            USB_PRINTF0("### Endpoint Descriptor error.\n");
            return USB_ERROR;
            break;
        /* Endpoint Descriptor */
        case USB_DT_ENDPOINT:
            /* Bulk Endpoint */
            if( (table[ofdsc + 3] & USB_EP_TRNSMASK) == USB_EP_BULK )
            {
                switch( dirflag )
                {
                case 0:
                    retval = R_usb_hstd_ChkPipeInfo(speed, &usb_ghmsc_TmpEpTbl[offset], &table[ofdsc]);
                    if( retval == USB_DIR_H_OUT )
                    {
                        usb_ghmsc_OutPipe[msgnum][0] = offset;
                    }
                    if( retval == USB_DIR_H_IN )
                    {
                        usb_ghmsc_InPipe[msgnum][0] = offset;
                    }
                    dirflag++;
                    break;
                case 1:
                    retval = R_usb_hstd_ChkPipeInfo(speed, &usb_ghmsc_TmpEpTbl[offset + USB_EPL], &table[ofdsc]);
                    if( retval == USB_DIR_H_OUT )
                    {
                        usb_ghmsc_OutPipe[msgnum][0] = (uint16_t)(offset + USB_EPL);
                    }
                    if( retval == USB_DIR_H_IN )
                    {
                        usb_ghmsc_InPipe[msgnum][0] = (uint16_t)(offset + USB_EPL);
                    }
                    if( (usb_ghmsc_InPipe[msgnum][0] != USB_NOPORT) && (usb_ghmsc_OutPipe[msgnum][0] != USB_NOPORT) )
                    {
                        return USB_DONE;
                    }
                    USB_PRINTF0("### Endpoint Descriptor error.\n");
                    break;
                default:
                    break;
                }
            }
            ofdsc += table[ofdsc];
            break;
        /* Device Qualifier Descriptor */
        case USB_DT_DEVICE_QUALIFIER:
        /* Other Speed Configuration Descriptor */
        case USB_DT_OTHER_SPEED_CONF:
        /* Interface Power Descriptor */
        case USB_DT_INTERFACE_POWER:
            USB_PRINTF0("### Endpoint Descriptor error.\n");
            return USB_ERROR;
            break;
        /* Antoher Descriptor */
        default:
            ofdsc += table[ofdsc];
            break;
        }
    }
    return USB_ERROR;
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_NoData
Description     : HMSC No data
Arguments       : uint16_t drvnum : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_NoData(USB_UTR_t *ptr, uint16_t drvnum)
{
    USB_CLSINFO_t mess;

    mess.ip = ptr->ip;
    mess.ipp = ptr->ipp;

    mess.keyword = drvnum;
    usb_shmsc_Process = USB_MSG_HMSC_NO_DATA;
    usb_hmsc_SpecifiedPath(&mess);
    usb_shmsc_NoDataSeq = USB_SEQ_0;

    return USB_DONE;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_NoDataAct
Description     : No Data Request
Arguments       : USB_CLSINFO_t *mess : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_NoDataAct(USB_CLSINFO_t *mess)
{
    uint16_t    hmsc_retval, result;
    uint16_t    drvnum;
    
    drvnum = usb_ghmsc_RootDrive;
    result = mess -> result;
    
    switch( usb_shmsc_NoDataSeq )
    {
    case USB_SEQ_0:
        /* CBW */
        drvnum = mess -> keyword;
        usb_ghmsc_RootDrive = drvnum;
        hmsc_retval = usb_hmsc_SendCbw(mess, drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_NO_DATA;
        usb_shmsc_NoDataSeq++;
        usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
        break;
    case USB_SEQ_1:
        hmsc_retval = usb_hmsc_SendCbwReq(mess, drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_NO_DATA;
        usb_shmsc_NoDataSeq++;
        break;
    case USB_SEQ_2:
        hmsc_retval = usb_hmsc_SendCbwCheck(mess, drvnum, result);
        if( hmsc_retval == USB_DATA_STALL )
        {
            usb_shmsc_NoDataSeq = USB_SEQ_0;
        }
        else if( hmsc_retval != USB_HMSC_OK )
        {
            USB_PRINTF1("### NoData : SendCbw error(drive:%d) \n", drvnum);
            usb_shmsc_NoDataSeq = USB_SEQ_0;
            usb_hmsc_CommandResult(mess,hmsc_retval);
        }
        else
        {
            /* CSW */
            hmsc_retval = usb_hmsc_GetCsw(mess, drvnum);
            
            usb_shmsc_Process = USB_MSG_HMSC_NO_DATA;
            usb_shmsc_NoDataSeq++;
            usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
        }
        break;
    case USB_SEQ_3:
        hmsc_retval = usb_hmsc_GetCswReq(mess, drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_NO_DATA;
        usb_shmsc_NoDataSeq++;
        break;
    case USB_SEQ_4:
        hmsc_retval = usb_hmsc_GetCswCheck(mess, drvnum, result);

        switch( hmsc_retval )
        {
        case USB_HMSC_OK:
            if(usb_ghmsc_CswErrRoop == USB_ON)
            {
                usb_ghmsc_CswErrRoop = USB_OFF;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            usb_hmsc_CommandResult(mess, hmsc_retval);
            break;
        case USB_HMSC_CSW_ERR:
            USB_PRINTF1("*** NoData : CSW-NG(drive:%d) \n", drvnum);
            usb_ghmsc_CswErrRoop = USB_ON;
            drvnum = usb_ghmsc_RootDrive;
            R_usb_hmsc_RequestSense(mess, drvnum, (uint8_t *)usb_ghmsc_Data);
            break;
        case USB_MSG_HMSC_DATA_STALL:
            USB_PRINTF1("*** NoData : CSW-STALL(drive:%d) \n", drvnum);
            usb_shmsc_Process = USB_MSG_HMSC_DATA_STALL;
            mess->keyword = drvnum;
            usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
            break;
        default:
            if(usb_ghmsc_CswErrRoop == USB_ON)
            {
                usb_ghmsc_CswErrRoop  =USB_OFF;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            USB_PRINTF1("### NoData : GetCSW error(drive:%d) \n", drvnum);
            usb_hmsc_CommandResult(mess, hmsc_retval);
            break;
        }
        usb_shmsc_NoDataSeq = USB_SEQ_0;
        break;
    default:
        usb_hmsc_CommandResult(mess, hmsc_retval);
        usb_shmsc_NoDataSeq = USB_SEQ_0;
        break;
    }
    return (hmsc_retval);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_DataIn
Description     : HMSC Data In
Arguments       : uint16_t drvnum : 
                : uint8_t *buff : 
                : uint32_t size : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_DataIn(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size)
{
    USB_CLSINFO_t mess;

    mess.ip = ptr->ip;
    mess.ipp = ptr->ipp;

    mess.keyword = drvnum;
    usb_ghmsc_Buff = buff;
    usb_ghmsc_TransSize = size;

    usb_shmsc_Process = USB_MSG_HMSC_DATA_IN;
    usb_hmsc_SpecifiedPath(&mess);
    usb_shmsc_DataInSeq = USB_SEQ_0;

    return USB_DONE;
}

/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_DataInAct
Description     : Receive Data request
Arguments       : USB_CLSINFO_t *mess : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_DataInAct(USB_CLSINFO_t *mess)
{
    uint16_t    hmsc_retval, result;
    uint16_t    drvnum;
    uint32_t    size;
    uint8_t     *buff;

    drvnum = usb_ghmsc_RootDrive;
    buff = usb_ghmsc_Buff;
    size = usb_ghmsc_TransSize;
    result = mess -> result;

    switch( usb_shmsc_DataInSeq )
    {
    case USB_SEQ_0:
        /* CBW */
        drvnum = mess -> keyword;
        usb_ghmsc_RootDrive = drvnum;
        hmsc_retval = usb_hmsc_SendCbw(mess, drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_DATA_IN;
        usb_shmsc_DataInSeq++;
        usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
        break;
    case USB_SEQ_1:
        hmsc_retval = usb_hmsc_SendCbwReq(mess, drvnum);
        usb_shmsc_DataInSeq++;
        break;
    case USB_SEQ_2:
        hmsc_retval = usb_hmsc_SendCbwCheck(mess, drvnum, result);
        if( hmsc_retval == USB_DATA_STALL )
        {
            usb_shmsc_DataInSeq = USB_SEQ_0;
        }
        else if( hmsc_retval != USB_HMSC_OK )
        {
            USB_PRINTF1("### DataIN : SendCBW error(drive:%d) \n", drvnum);
            usb_shmsc_DataInSeq = USB_SEQ_0;
            usb_hmsc_CommandResult(mess, hmsc_retval);
        }
        else
        {
/* Data */
            hmsc_retval = usb_hmsc_GetData(mess, drvnum, buff, size);
            usb_shmsc_Process = USB_MSG_HMSC_DATA_IN;
            usb_shmsc_DataInSeq++;
            usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
        }
        break;
    case USB_SEQ_3:
        hmsc_retval = usb_hmsc_GetDataReq(mess, drvnum, buff, size);
        usb_shmsc_Process = USB_MSG_HMSC_DATA_IN;
        usb_shmsc_DataInSeq++;
        break;
    case USB_SEQ_4:
        hmsc_retval = usb_hmsc_GetDataCheck(mess, drvnum, result);
        if( hmsc_retval == USB_HMSC_STALL )
        {
            usb_shmsc_Process = USB_MSG_HMSC_DATA_IN;
            usb_shmsc_DataInSeq++;
        }
        else if( hmsc_retval != USB_HMSC_OK )
        {
            USB_PRINTF1("### DataIN : GetData error(drive:%d) \n", drvnum);
            usb_hmsc_CommandResult(mess, hmsc_retval);
            usb_shmsc_DataInSeq = USB_SEQ_0;
        }
        else
        {
            /* CSW */
            hmsc_retval = usb_hmsc_GetCsw(mess, drvnum);

            usb_shmsc_Process = USB_MSG_HMSC_DATA_IN;
            usb_shmsc_DataInSeq++;
            usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
        }
        break;
    case USB_SEQ_5:
        hmsc_retval = usb_hmsc_GetCswReq(mess, drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_DATA_IN;
        usb_shmsc_DataInSeq++;
        break;
    case USB_SEQ_6:
        hmsc_retval = usb_hmsc_GetCswCheck(mess, drvnum, result);
        switch( hmsc_retval )
        {
        case USB_HMSC_OK:
            if(usb_ghmsc_CswErrRoop == USB_ON)
            {
                usb_ghmsc_CswErrRoop = USB_OFF;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            usb_hmsc_CommandResult(mess, hmsc_retval);
            break;
        case USB_HMSC_CSW_ERR:
            USB_PRINTF1("*** DataIN : CSW-NG(drive:%d) \n", drvnum);
            usb_ghmsc_CswErrRoop = USB_ON;
            drvnum = usb_ghmsc_RootDrive;
            R_usb_hmsc_RequestSense(mess, drvnum, (uint8_t*)&usb_ghmsc_Data);
            break;
        case USB_MSG_HMSC_DATA_STALL:
            USB_PRINTF1("*** DataIN : CSW-STALL(drive:%d) \n", drvnum);
            usb_shmsc_Process = USB_MSG_HMSC_DATA_STALL;
            mess->keyword = drvnum;
            usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
            break;
        default:
            if(usb_ghmsc_CswErrRoop == USB_ON)
            {
                usb_ghmsc_CswErrRoop = USB_OFF;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            USB_PRINTF1("### DataIN : GetCSW error(drive:%d) \n", drvnum);
            usb_hmsc_CommandResult(mess, hmsc_retval);
            break;
        }
        
        usb_shmsc_DataInSeq = USB_SEQ_0;
        break;
    default:
        usb_hmsc_CommandResult(mess, hmsc_retval);
        usb_shmsc_DataInSeq = USB_SEQ_0;
        break;
    }
    /* Data read error */
    return (hmsc_retval);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_DataOut
Description     : HMSC Data Out
Arguments       : uint16_t drvnum : 
                : uint8_t *buff : 
                : uint32_t size : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_DataOut(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size)
{
    USB_CLSINFO_t mess;

    mess.ip = ptr->ip;
    mess.ipp = ptr->ipp;

    mess.keyword = drvnum;
    usb_ghmsc_Buff = buff;
    usb_ghmsc_TransSize = size;
    usb_shmsc_Process = USB_MSG_HMSC_DATA_OUT;
    usb_hmsc_SpecifiedPath(&mess);
    usb_shmsc_DataOutSeq = usb_shmsc_DataOutSeq;

    return USB_DONE;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_DataOutAct
Description     : Send Data request
Arguments       : USB_CLSINFO_t *mess : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_DataOutAct(USB_CLSINFO_t *mess)
{
    uint16_t    hmsc_retval, result;
    uint16_t    drvnum;
    uint8_t     *buff;
    uint32_t    size;

    drvnum = usb_ghmsc_RootDrive;
    buff = usb_ghmsc_Buff;
    size = usb_ghmsc_TransSize;
    result = mess -> result;
    
    switch( usb_shmsc_DataOutSeq )
    {
    case USB_SEQ_0:
        /* CBW */
        drvnum = mess -> keyword;
        usb_ghmsc_RootDrive = drvnum;
        hmsc_retval = usb_hmsc_SendCbw(mess, drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_DATA_OUT;
        usb_shmsc_DataOutSeq++;
        usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
        break;
    case USB_SEQ_1:
        hmsc_retval = usb_hmsc_SendCbwReq(mess, drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_DATA_OUT;
        usb_shmsc_DataOutSeq++;
        break;
    case USB_SEQ_2:
        hmsc_retval = usb_hmsc_SendCbwCheck(mess, drvnum, result);
        if( hmsc_retval == USB_DATA_STALL )
        {
            usb_shmsc_DataOutSeq = USB_SEQ_0;
        }
        else if( hmsc_retval != USB_HMSC_OK )
        {
            USB_PRINTF1("### DataOUT : SendCBW error(drive:%d) \n", drvnum);
            usb_shmsc_DataOutSeq = USB_SEQ_0;
            usb_hmsc_CommandResult(mess, hmsc_retval);
        }
        else
        {
            /* Data */
            usb_hmsc_SendData(mess, drvnum, buff, size);
            usb_shmsc_Process = USB_MSG_HMSC_DATA_OUT;
            usb_shmsc_DataOutSeq++;
            usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
        }
        break;
    case USB_SEQ_3:
        usb_hmsc_SendDataReq(mess, drvnum, buff, size);
        usb_shmsc_Process = USB_MSG_HMSC_DATA_OUT;
        usb_shmsc_DataOutSeq++;
        break;
    case USB_SEQ_4:
        hmsc_retval = usb_hmsc_SendDataCheck(mess, drvnum, result);
        if( hmsc_retval == USB_HMSC_STALL )
        {
            usb_shmsc_Process = USB_MSG_HMSC_DATA_OUT;
            usb_shmsc_DataOutSeq++;
        }
        else if( hmsc_retval != USB_HMSC_OK )
        {
            USB_PRINTF1("### DataOUT : SendData error(drive:%d) \n", drvnum);
            usb_hmsc_CommandResult(mess, hmsc_retval);
            usb_shmsc_DataOutSeq = USB_SEQ_0;
        }
        else
        {
            /* CSW */
            hmsc_retval = usb_hmsc_GetCsw(mess, drvnum);
            usb_shmsc_Process = USB_MSG_HMSC_DATA_OUT;
            usb_shmsc_DataOutSeq++;
            usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
        }
        break;
    case USB_SEQ_5:
        hmsc_retval = usb_hmsc_GetCswReq(mess, drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_DATA_OUT;
        usb_shmsc_DataOutSeq++;
        break;
    case USB_SEQ_6:
        hmsc_retval = usb_hmsc_GetCswCheck(mess, drvnum,result);
        switch( hmsc_retval )
        {
        case USB_HMSC_OK:
            if( usb_ghmsc_CswErrRoop == USB_ON )
            {
                usb_ghmsc_CswErrRoop = USB_OFF;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            usb_hmsc_CommandResult(mess, hmsc_retval);
            break;
        case USB_HMSC_CSW_ERR:
            USB_PRINTF1("*** DataOUT : CSW-NG(drive:%d) \n", drvnum);
            usb_ghmsc_CswErrRoop = USB_ON;
            drvnum = usb_ghmsc_RootDrive;
            R_usb_hmsc_RequestSense(mess, drvnum, (uint8_t*)usb_ghmsc_Data);
            break;
        case USB_MSG_HMSC_DATA_STALL:
            USB_PRINTF1("*** DataOUT : CSW-STALL(drive:%d) \n", drvnum);
            usb_shmsc_Process = USB_MSG_HMSC_DATA_STALL;
            mess->keyword = drvnum;
            usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
            break;
        default:
            if( usb_ghmsc_CswErrRoop == USB_ON )
            {
                usb_ghmsc_CswErrRoop = USB_OFF;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            USB_PRINTF1("### DataOUT : GetCSW error(drive:%d) \n", drvnum);
            usb_hmsc_CommandResult(mess, hmsc_retval);
            break;
        }

        usb_shmsc_DataOutSeq = USB_SEQ_0;
        break;
    default:
        usb_shmsc_DataOutSeq = USB_SEQ_0;
        usb_hmsc_CommandResult(mess, hmsc_retval);
        break;
    }
    /* Data read error */
    return (hmsc_retval);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SpecifiedPath
Description     : Next Process Selector
Arguments       : USB_CLSINFO_t *mess : 
Return value    : none
******************************************************************************/
void usb_hmsc_SpecifiedPath(USB_CLSINFO_t *mess)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *ptr;

    /* Get mem pool blk */
    if( USB_PGET_BLK(USB_HMSC_MPL, &p_blf) == USB_OK )
    {
        ptr = (USB_CLSINFO_t*)p_blf;
        ptr->msginfo    = usb_shmsc_Process;
        ptr->keyword    = mess->keyword;
        ptr->result     = mess->result;

        ptr->ip         = mess->ip;
        ptr->ipp        = mess->ipp;

        /* Send message */
        err = USB_SND_MSG(USB_HMSC_MBX, (USB_MSG_t*)p_blf);
        if( err != USB_E_OK )
        {
            err = USB_REL_BLK(USB_HMSC_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### SpecifiedPass function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### SpecifiedPass function pget_blk error\n");
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_CheckResult
Description     : Hub class check result
Arguments       : USB_UTR_t *mess : 
Return value    : none
******************************************************************************/
void usb_hmsc_CheckResult(USB_UTR_t *mess, uint16_t data1, uint16_t data2)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *ptr;

    ptr = mess;

    if( mess->status == USB_DATA_STALL )
    {
        ptr->msginfo    = usb_shmsc_Process;
    }
    
    /* Get mem pool blk */
    if( USB_PGET_BLK(USB_HMSC_MPL, &p_blf) == USB_OK )
    {
        ptr = (USB_CLSINFO_t*)p_blf;
        ptr->msginfo    = usb_shmsc_Process;
        ptr->keyword    = mess->keyword;
        ptr->result     = mess->status;

        ptr->ip         = mess->ip;
        ptr->ipp        = mess->ipp;

        /* Send message */
        err = USB_SND_MSG( USB_HMSC_MBX, (USB_MSG_t*)p_blf );
        if( err != USB_E_OK )
        {
            err = USB_REL_BLK(USB_HMSC_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### CheckResult function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### CheckResult function pget_blk error\n");
    }
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_StallErr
Description     : HMSC Stall Error
Arguments       : USB_UTR_t *ptr : 
Return value    : none
******************************************************************************/
void usb_hmsc_StallErr(USB_UTR_t *ptr)
{
    uint16_t    drvnum, msgnum, result;
    uint16_t    devaddr,hub_addr,hub_port_no;
    USB_ER_t    err,err2;
    USB_MH_t    p_blf;
    USB_UTR_t   *cp;

    drvnum = usb_ghmsc_RootDrive;
    result = ptr->status;
    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);

    switch( usb_shmsc_StallErrSeq )
    {
    case USB_SEQ_0:
        drvnum = ptr->keyword;
        /* Device address set */
        devaddr = usb_hmsc_SmpDrive2Addr(drvnum);

        /* Get connected Hub address from Device address. */
        hub_addr = R_usb_hhub_get_hub_addr(ptr, devaddr);

        /* Get connected Hub port number from Device address. */
        hub_port_no = R_usb_hhub_get_hub_port_no(ptr, devaddr);

        /* Transfer Get Port Status when get Hub address and connected Hub port no. */
        if( (hub_addr != USB_ERROR) && (hub_port_no != USB_ERROR) )
        {
            /* Get Port Status(GET_STATUS) */
            R_usb_hhub_GetPortInformation(ptr, hub_addr, hub_port_no, (USB_CB_t)usb_hmsc_CheckResult );
            usb_shmsc_StallErrSeq = USB_SEQ_4;
        }
        else
        {
            usb_ghmsc_RootDrive = drvnum;

            err = R_usb_hmsc_MassStorageReset(ptr, drvnum, (USB_CB_t)usb_hmsc_CheckResult);
            /* Control Transfer overlaps */
            if( err == USB_E_QOVR )
            {
                /* Resend message */
                err = USB_PGET_BLK(USB_HMSC_MPL, &p_blf);
                if( err == USB_E_OK )
                {
                    cp = (USB_UTR_t*)p_blf;
                    cp->msginfo = ptr -> msginfo;
                    cp->keyword = ptr -> keyword;
                    cp->status  = ptr -> status;

                    cp->ip      = ptr->ip;
                    cp->ipp     = ptr->ipp;

                    /* Send message */
                    err = USB_SND_MSG(USB_HMSC_MBX, (USB_MSG_t*)p_blf);
                    if( err != USB_E_OK )
                    {
                        USB_PRINTF1("### StallErr snd_msg error (%ld)\n", err);
                        err2 = USB_REL_BLK(USB_HMSC_MPL, (USB_MH_t)p_blf);
                        if( err2 != USB_E_OK )
                        {
                            USB_PRINTF1("### StallErr rel_blk error (%ld)\n", err2);
                        }
                    }
                }
                else
                {
                    USB_PRINTF1("### StallErr pget_blk error (%ld)\n", err);
                }
            }
            else
            {
                /* Control Transfer not overlaps */
                usb_shmsc_StallErrSeq++;
            }
        }
        break;

    case USB_SEQ_1:
        usb_hmsc_MassStorageResetCheck(ptr, result);
        R_usb_hmsc_ClearStall(ptr, (uint16_t)USB_DATA_NONE, msgnum,
             usb_hmsc_CheckResult);
        usb_shmsc_StallErrSeq++;
        break;
    case USB_SEQ_2:
        usb_hmsc_ClearStallCheck(ptr, result);
        R_usb_hmsc_ClearStall(ptr, (uint16_t)USB_DATA_OK, msgnum,
             usb_hmsc_CheckResult);
        usb_shmsc_StallErrSeq++;
        break;
    case USB_SEQ_3:
        usb_hmsc_ClearStallCheck(ptr, result);
        if( ptr->msginfo == USB_MSG_HMSC_CSW_PHASE_ERR )
        {
            result = USB_HMSC_CSW_PHASE_ERR;
        }
        else
        {
            result = USB_HMSC_CBW_ERR;
        }
        usb_hmsc_CommandResult(ptr, result);
        usb_shmsc_StallErrSeq = USB_SEQ_0;
        break;

    case USB_SEQ_4:
        drvnum = (uint16_t)ptr->usr_data;                               /* Device no. set */
        /* Device address set */
        devaddr = usb_hmsc_SmpDrive2Addr(drvnum);

        /* Get connected Hub address from Device address. */
        hub_addr = R_usb_hhub_get_hub_addr(ptr, devaddr);

        /* Check device connect status for after transfer complete GET_STATUS(Get Port Status) */
        if( R_usb_hhub_chk_connect_status(ptr, hub_addr) == USB_DEV_NO_CONNECT )
        {
            if( ptr->msginfo == USB_MSG_HMSC_CSW_PHASE_ERR )
            {
                result = USB_HMSC_CSW_PHASE_ERR;
            }
            else
            {
                result = USB_HMSC_CBW_ERR;
            }
            usb_hmsc_CommandResult(ptr, result);
            usb_shmsc_StallErrSeq = USB_SEQ_0;
        }
        else
        {
            usb_ghmsc_RootDrive = drvnum;

            err = R_usb_hmsc_MassStorageReset(ptr, drvnum, (USB_CB_t)usb_hmsc_CheckResult);
            /* Control Transfer overlaps */
            if( err == USB_E_QOVR )
            {
                /* Resend message */
                err = USB_PGET_BLK(USB_HMSC_MPL, &p_blf);
                if( err == USB_E_OK )
                {
                    cp = (USB_UTR_t*)p_blf;
                    cp->msginfo = ptr -> msginfo;
                    cp->keyword = ptr -> keyword;
                    cp->status  = ptr -> status;

                    cp->ip      = ptr->ip;
                    cp->ipp     = ptr->ipp;

                    /* Send message */
                    err = USB_SND_MSG(USB_HMSC_MBX, (USB_MSG_t*)p_blf);
                    if( err != USB_E_OK )
                    {
                        USB_PRINTF1("### StallErr snd_msg error (%ld)\n", err);
                        err2 = USB_REL_BLK(USB_HMSC_MPL, (USB_MH_t)p_blf);
                        if( err2 != USB_E_OK )
                        {
                            USB_PRINTF1("### StallErr rel_blk error (%ld)\n", err2);
                        }
                    }
                }
                else
                {
                    USB_PRINTF1("### StallErr pget_blk error (%ld)\n", err);
                }
            }
            else
            {
                /* Control Transfer not overlaps */
                usb_shmsc_StallErrSeq = USB_SEQ_1;
            }
        }
        break;

    default:
        if( ptr->msginfo == USB_MSG_HMSC_CSW_PHASE_ERR )
        {
            result = USB_HMSC_CSW_PHASE_ERR;
        }
        else
        {
            result = USB_HMSC_CBW_ERR;
        }
        usb_hmsc_CommandResult(ptr, result);
        usb_shmsc_StallErrSeq = USB_SEQ_0;
        break;
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_DataStall
Description     : HMSC Data Stall
Arguments       : USB_UTR_t *mess : 
Return value    : none
******************************************************************************/
void usb_hmsc_DataStall(USB_UTR_t *mess)
{
    uint16_t result, status, drvnum, msgnum, hmsc_retval;
    USB_CLSINFO_t *ptr;

    drvnum = usb_ghmsc_RootDrive;
    ptr = (USB_CLSINFO_t*)mess;
    result = ptr->result;
    status = mess->status;

    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);

    switch( usb_shmsc_DataStallSeq )
    {
    case USB_SEQ_0:
        drvnum = mess->keyword;
        usb_ghmsc_RootDrive = drvnum;

        R_usb_hmsc_ClearStall(mess, (uint16_t)USB_DATA_OK, msgnum, usb_hmsc_CheckResult);
        usb_shmsc_Process = USB_MSG_HMSC_DATA_STALL;
        usb_shmsc_DataStallSeq++;
        break;
    case USB_SEQ_1:
        usb_hmsc_ClearStallCheck(mess, status);
        usb_hmsc_GetCsw(mess, drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_DATA_STALL;
        usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
        usb_shmsc_DataStallSeq++;
        break;
    case USB_SEQ_2:
        usb_hmsc_GetCswReq(mess, drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_DATA_STALL;
        usb_shmsc_DataStallSeq++;
        break;
    case USB_SEQ_3:
        hmsc_retval = usb_hmsc_GetCswCheck(mess, drvnum,result);
        switch( hmsc_retval )
        {
        case USB_HMSC_OK:
            if( usb_ghmsc_CswErrRoop == USB_ON )
            {
                usb_ghmsc_CswErrRoop = USB_OFF;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            usb_hmsc_CommandResult(mess, hmsc_retval);
            break;
        case USB_HMSC_CSW_ERR:
            USB_PRINTF1("*** DataOUT : CSW-NG(drive:%d) \n", drvnum);
            usb_ghmsc_CswErrRoop = USB_ON;
            drvnum = usb_ghmsc_RootDrive;
            R_usb_hmsc_RequestSense(mess, drvnum, (uint8_t*)usb_ghmsc_Data);
            break;
        case USB_MSG_HMSC_DATA_STALL:
            usb_shmsc_Process = USB_MSG_HMSC_CSW_PHASE_ERR;
            mess->keyword = drvnum;
            usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
            USB_PRINTF1("*** DataOUT : Phase error(drive:%d) \n", drvnum);
            break;
        default:
            if( usb_ghmsc_CswErrRoop == USB_ON )
            {
                usb_ghmsc_CswErrRoop = USB_OFF;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            USB_PRINTF1("### DataOUT : GetCSW error(drive:%d) \n", drvnum);
            usb_hmsc_CommandResult(mess, hmsc_retval);
            break;
        }
        usb_shmsc_DataStallSeq = USB_SEQ_0;
        break;
    default:
        usb_hmsc_CommandResult(mess,USB_HMSC_CSW_ERR);
        usb_shmsc_DataStallSeq = USB_SEQ_0;
        break;
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_CommandResult
Description     : Hub class check result
Arguments       : uint16_t result     : Result
Return value    : none
******************************************************************************/
void usb_hmsc_CommandResult(USB_UTR_t *ptr, uint16_t result)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *cp;
    
    /* Get mem pool blk */
    if( USB_PGET_BLK(USB_HSTRG_MPL,&p_blf) == USB_OK )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = usb_shmsc_StrgProcess;
        cp->result  = result;

        cp->ip      = ptr->ip;
        cp->ipp     = ptr->ipp;

        /* Send message */
        err = USB_SND_MSG( USB_HSTRG_MBX, (USB_MSG_t*)p_blf );
        if( err != USB_E_OK )
        {
            err = USB_REL_BLK(USB_HSTRG_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### CheckResult function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### CheckResult function pget_blk error\n");
    }
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/
