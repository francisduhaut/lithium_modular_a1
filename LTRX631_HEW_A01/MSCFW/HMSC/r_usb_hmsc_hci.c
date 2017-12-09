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
* File Name    : r_usb_hmsc_hci.c
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
//#include  "r_usb_cDefUsrPp.h"     /* System definition */
#include    "r_usb_cmsc_define.h"   /* USB  Mass Storage Class Header */
#include    "r_usb_hmsc_define.h"   /* Host Mass Storage Class Driver define */
#include    "r_usb_cDefUSBIP.h"     /* USB-FW Library Header */
#include    "r_usb_cMacSystemcall.h"/* System call macro */
#include    "r_usb_cMacPrint.h"     /* LCD/SIO disp macro */
#include    "r_usb_hmsc_extern.h"   /* Host MSC grobal define */
#include    "r_usb_cExtern.h"       /* USB-FW grobal define */
#include    "r_usb_cKernelId.h"     /* Kernel ID definition */

#include "r_usb_ansi.h"
#include "r_usb_api.h"

/******************************************************************************
Section    <Section Difinition> , "Project Sections"
******************************************************************************/
#pragma section _hmsc


/******************************************************************************
Private global variables and functions
******************************************************************************/
/* Send data transfer message */
USB_UTR_t               usb_ghmsc_TransData[USB_MAXSTRAGE];
/* Receive data transfer message */
USB_UTR_t               usb_ghmsc_ReceiveData[USB_MAXSTRAGE];
/* Control data transfer message */
USB_UTR_t               usb_ghmsc_ControlData;
/* Pipenumber / Pipectr(SQTGL) */
uint16_t                usb_ghmsc_OutPipe[USB_MAXSTRAGE][2];
/* Pipenumber / Pipectr(SQTGL) */
uint16_t                usb_ghmsc_InPipe[USB_MAXSTRAGE][2];
uint8_t                 usb_ghmsc_Data[5120];

static uint16_t         usb_shmsc_MsgNum;
uint16_t                usb_shmsc_ClearStallPipe;
USB_ER_t                usb_hmsc_ClearStall(USB_UTR_t *ptr, uint16_t Pipe, USB_CB_t complete);

void                    usb_hmsc_ControlEnd(USB_UTR_t *ptr, uint16_t sts);

uint16_t usb_hmsc_SendCbwReq(USB_UTR_t *ptr, uint16_t drvnum);
uint16_t usb_hmsc_GetDataReq(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size);
uint16_t usb_hmsc_SendDataReq(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size);
uint16_t usb_hmsc_GetCswReq(USB_UTR_t *ptr, uint16_t drvnum);

/******************************************************************************
External variables and functions
******************************************************************************/
extern uint16_t usb_shmsc_Process;
extern uint8_t      usb_ghmsc_ClassData[];

extern uint16_t usb_hmsc_StdReqCheck(uint16_t errcheck);
extern uint16_t usb_hmsc_GetStringDescriptor(USB_UTR_t *ptr, uint16_t devaddr, uint16_t index);

/******************************************************************************
Renesas Abstracted HMSC Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_SendCbw
Description     : Send CBW
Arguments       : uint16_t drvnum           : Drive Number
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_SendCbw(USB_UTR_t *ptr, uint16_t drvnum)
{
    USB_ER_t    err;
    uint16_t    msgnum;

    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);

    /* Set CBW TAG */
    usb_hmsc_CbwTagCount(msgnum);

    /* Request CBW */
    /* Device number */
    usb_ghmsc_TransData[msgnum].keyword     = msgnum;
    /* Transfer data address */
    usb_ghmsc_TransData[msgnum].tranadr     = (void*)&usb_ghmsc_Cbw[msgnum];
    /* Transfer data length */
    usb_ghmsc_TransData[msgnum].tranlen     = (uint32_t)USB_MSC_CBWLENGTH;
    /* Not control transfer */
    usb_ghmsc_TransData[msgnum].setup           = 0;
    usb_ghmsc_TransData[msgnum].segment     = USB_TRAN_END;
    /* Call Back Function Info */
    usb_ghmsc_TransData[msgnum].complete    = (USB_CB_t)&usb_hmsc_DummyFunction;

    err = usb_hmsc_Submitutr(ptr, (uint16_t)USB_DATA_NONE, &usb_ghmsc_TransData[msgnum]);

    return (err);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SendCbwReq
Description     : Send CBW
Arguments       : uint16_t drvnum           : Drive Number
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_SendCbwReq(USB_UTR_t *ptr, uint16_t drvnum)
{
    USB_ER_t    err;
    uint16_t    msgnum;

    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);
    /* Call Back Function Info */
    usb_ghmsc_TransData[msgnum].complete    = &usb_hmsc_CheckResult;

    err = usb_hmsc_SubmitutrReq(ptr, (uint16_t)USB_DATA_NONE, &usb_ghmsc_TransData[msgnum]);
    if( err != USB_E_OK ) 
    {
        USB_PRINTF1("### Mass Storage Device Class submit error(drive:%d) !\n", drvnum);
        return USB_HMSC_SUBMIT_ERR;
    }
    return (err);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SendCbwCheck
Description     : Check send CBW 
Arguments       : uint16_t drvnum           : Drive Number
                : uint16_t hmsc_retval      : Return Value
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_SendCbwCheck(USB_UTR_t *ptr, uint16_t drvnum, uint16_t hmsc_retval)
{
    uint16_t    pipeno, msgnum;
    USB_CLSINFO_t mess;

    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);
    /* NonOS */
    /* hmsc_retval = usb_hmsc_TransWaitTmo((uint16_t)3000); */
    switch( hmsc_retval ) 
    {
    case USB_DATA_NONE:     /* Send CBW */
        pipeno = R_usb_hmsc_Information(usb_ghmsc_OutPipe[msgnum][0]);
        usb_ghmsc_OutPipe[msgnum][1] = usb_ghmsc_TransData[msgnum].pipectr;
        return USB_HMSC_OK;
        break;
    case USB_DATA_STALL:    /* Stall */
        USB_PRINTF1("*** CBW Transfer STALL(drive:%d) !\n", drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_CBW_ERR;
        usb_shmsc_StallErrSeq = USB_SEQ_0;
        mess.keyword = drvnum;

        mess.ip = ptr->ip;
        mess.ipp = ptr->ipp;

        usb_hmsc_SpecifiedPath(&mess);
        return USB_DATA_STALL;
        break;
    case USB_DATA_TMO:      /* Timeout */
        USB_PRINTF1("### CBW Transfer timeout ERROR(drive:%d) !\n", drvnum);
        pipeno  = R_usb_hmsc_Information(usb_ghmsc_OutPipe[msgnum][0]);
        R_usb_hstd_TransferEnd(ptr, pipeno, (uint16_t)USB_DATA_TMO);
        break;
    case USB_DATA_ERR:
        USB_PRINTF1("### CBW Transfer ERROR(drive:%d) !\n", drvnum);
        break;
    default:
        USB_PRINTF1("### CBW Transfer error(drive:%d) !\n", drvnum);
        break;
    }
    return USB_HMSC_CBW_ERR;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_GetData
Description     : Receive Data request
Arguments       : uint16_t drvnum           : Drive Number
                : uint8_t *buff             : Receive Data Buffer Address
                : uint32_t size             : Receive Data Size
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetData(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size)
{
    uint16_t    msgnum;

    msgnum  = usb_hmsc_SmpDrive2Msgnum(drvnum);
    /* Device number */
    usb_ghmsc_ReceiveData[msgnum].keyword   = msgnum;
    /* Transfer data address */
    usb_ghmsc_ReceiveData[msgnum].tranadr   = (void*)buff;
    /* Transfer data length */
    usb_ghmsc_ReceiveData[msgnum].tranlen   = size;
    /* Not control transfer */
    usb_ghmsc_ReceiveData[msgnum].setup     = 0;
    usb_ghmsc_ReceiveData[msgnum].segment   = USB_TRAN_END;
    /* Call Back Function Info */
    usb_ghmsc_ReceiveData[msgnum].complete  = (USB_CB_t)&usb_hmsc_DummyFunction;

    usb_hmsc_Submitutr(ptr, (uint16_t)USB_DATA_OK, &usb_ghmsc_ReceiveData[msgnum]);
    return USB_DONE;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_GetDataReq
Description     : Get Data request
Arguments       : uint16_t drvnum           : Drive Number
                : uint8_t *buff             : Not use
                : uint32_t size             : Not use
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetDataReq(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size)
{
    USB_ER_t    err;
    uint16_t    msgnum;

    msgnum  = usb_hmsc_SmpDrive2Msgnum(drvnum);

    /* Call Back Function Info */
    usb_ghmsc_ReceiveData[msgnum].complete  = &usb_hmsc_CheckResult;

    err = usb_hmsc_SubmitutrReq(ptr, (uint16_t)USB_DATA_OK, &usb_ghmsc_ReceiveData[msgnum]);
    if( err != USB_E_OK ) 
    {
        USB_PRINTF1("### Mass Storage Device Class submit error(drive:%d) !\n", drvnum);
        return USB_HMSC_SUBMIT_ERR;
    }
    return (err);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_GetDataCheck
Description     : Check Get Data 
Arguments       : uint16_t drvnum           : Drive Number
                : uint16_t hmsc_retval      : Return Value
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetDataCheck(USB_UTR_t *ptr, uint16_t drvnum, uint16_t hmsc_retval)
{
    uint16_t pipeno, msgnum;
    
    msgnum  = usb_hmsc_SmpDrive2Msgnum(drvnum);

    /* NonOS */
    /* hmsc_retval = usb_hmsc_TransWaitTmo((uint16_t)3000); */
    switch( hmsc_retval ) 
    {
    case USB_DATA_SHT:
        /* Continue */
    case USB_DATA_OK:
        pipeno = R_usb_hmsc_Information(usb_ghmsc_InPipe[msgnum][0]);
        usb_ghmsc_InPipe[msgnum][1] = usb_ghmsc_ReceiveData[msgnum].pipectr;
        return  USB_HMSC_OK;
        break;
    case USB_DATA_STALL:
        USB_PRINTF1("*** GetData Read STALL(drive:%d) !\n", drvnum);
        R_usb_hmsc_ClearStall(ptr, (uint16_t)USB_DATA_OK, msgnum, (USB_CB_t)usb_hmsc_ClearStallCheck2);
        return  USB_HMSC_STALL;
        break;
    case USB_DATA_TMO:
        USB_PRINTF1("### hmsc_Data Read timeout ERROR(drive:%d) !\n", drvnum);
        pipeno = R_usb_hmsc_Information(usb_ghmsc_InPipe[msgnum][0]);
        R_usb_hstd_TransferEnd(ptr, pipeno, (uint16_t)USB_DATA_TMO);
        break;
    case USB_DATA_ERR:
        USB_PRINTF1("### hmsc_Data Read ERROR(drive:%d) !\n", drvnum);
        break;
    case USB_DATA_OVR:
        USB_PRINTF1("### hmsc_Data receive over(drive:%d) !\n", drvnum);
        break;
    default:
        USB_PRINTF1("### hmsc_Data Read error(drive:%d) !\n", drvnum);
        break;
    }
    return USB_HMSC_DAT_RD_ERR;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SendData
Description     : Send Pipe Data
Arguments       : uint16_t drvnum           : Drive Number
                : uint8_t  *buff            : Data Info Address
                : uint32_t size             : Data Size
Return value    : uint16_t                  : Error Code(USB_DONE)
******************************************************************************/
uint16_t usb_hmsc_SendData(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size)
{
    uint16_t    msgnum;

    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);
    /* Device number */
    usb_ghmsc_TransData[msgnum].keyword     = msgnum;
    /* Transfer data address */
    usb_ghmsc_TransData[msgnum].tranadr     = (void*)buff;
    /* Transfer data length */
    usb_ghmsc_TransData[msgnum].tranlen     = size;
    /* Not control transfer */
    usb_ghmsc_TransData[msgnum].setup       = 0;
    usb_ghmsc_TransData[msgnum].segment     = USB_TRAN_END;
    /* Call Back Function Info */
    usb_ghmsc_TransData[msgnum].complete
        = (USB_CB_t)&usb_hmsc_DummyFunction;

    usb_hmsc_Submitutr(ptr, (uint16_t)USB_DATA_NONE
        , &usb_ghmsc_TransData[msgnum]);
    return USB_DONE;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SendDataReq
Description     : Send Pipe Data
Arguments       : uint16_t drvnum           : Drive Number
                : uint8_t  *buff            : Data Info Address
                : uint32_t size             : Data Size
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_SendDataReq(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size)
{
    USB_ER_t    err;
    uint16_t    msgnum;

    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);
    /* Call Back Function Info */
    usb_ghmsc_TransData[msgnum].complete    = &usb_hmsc_CheckResult;

    err = usb_hmsc_SubmitutrReq(ptr, (uint16_t)USB_DATA_NONE, &usb_ghmsc_TransData[msgnum]);
    if( err != USB_E_OK ) 
    {
        USB_PRINTF1("### Mass Storage Device Class submit error(drive:%d) !\n", drvnum);
        return USB_HMSC_SUBMIT_ERR;
    }
    return (err);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SendDataCheck
Description     : Check Send Data
Arguments       : uint16_t drvnum           : Drive Number
                : uint16_t hmsc_retval      : Return Value
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_SendDataCheck(USB_UTR_t *ptr, uint16_t drvnum, uint16_t hmsc_retval)
{
    uint16_t    pipeno, msgnum;
    
    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);

    switch( hmsc_retval ) 
    {
    case USB_DATA_NONE:
        pipeno = R_usb_hmsc_Information(usb_ghmsc_OutPipe[msgnum][0]);
        usb_ghmsc_OutPipe[msgnum][1] = usb_ghmsc_TransData[msgnum].pipectr;
        return USB_HMSC_OK;
        break;
    case USB_DATA_STALL:
        USB_PRINTF1("*** hmsc_Data Write STALL(drive:%d) !\n", drvnum);
        R_usb_hmsc_ClearStall(ptr, (uint16_t)USB_DATA_NONE, msgnum, (USB_CB_t)usb_hmsc_ClearStallCheck2);
        return USB_HMSC_STALL;
        break;
    case USB_DATA_TMO:
        USB_PRINTF1("### hmsc_Data Write timeout ERROR(drive:%d) !\n", drvnum);
        pipeno  = R_usb_hmsc_Information(usb_ghmsc_OutPipe[msgnum][0]);
        R_usb_hstd_TransferEnd(ptr, pipeno, (uint16_t)USB_DATA_TMO);
        break;
    case USB_DATA_ERR:
        USB_PRINTF1("### hmsc_Data Write ERROR(drive:%d) !\n", drvnum);
        break;
    default:
        USB_PRINTF1("### hmsc_Data Write error(drive:%d) !\n", drvnum);
        break;
    }
    return USB_HMSC_DAT_WR_ERR;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_GetCsw
Description     : Receive CSW
Arguments       : uint16_t drvnum           : Drive Number
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetCsw(USB_UTR_t *ptr, uint16_t drvnum)
{
    uint16_t    msgnum;

    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);

    /* Request */
    /* Device number */
    usb_ghmsc_ReceiveData[msgnum].keyword   = msgnum;
    /* Transfer data address */
    usb_ghmsc_ReceiveData[msgnum].tranadr   = (void*)&usb_ghmsc_Csw[msgnum];
    /* Transfer data length */
    usb_ghmsc_ReceiveData[msgnum].tranlen   = (uint32_t)USB_MSC_CSW_LENGTH;
    /* Not control transfer */
    usb_ghmsc_ReceiveData[msgnum].setup     = 0;
    usb_ghmsc_ReceiveData[msgnum].segment   = USB_TRAN_END;
    /* Call Back Function Info */
    usb_ghmsc_ReceiveData[msgnum].complete  = (USB_CB_t)&usb_hmsc_DummyFunction;

    usb_hmsc_Submitutr(ptr, (uint16_t)USB_DATA_OK, &usb_ghmsc_ReceiveData[msgnum]);
    return USB_DONE;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_GetCswReq
Description     : Request Receive CSW
Arguments       : uint16_t drvnum           : Drive Number
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetCswReq(USB_UTR_t *ptr, uint16_t drvnum)
{
    USB_ER_t    err;
    uint16_t    msgnum;

    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);

    /* Transfer data length */
    usb_ghmsc_ReceiveData[msgnum].tranlen   = (uint32_t)USB_MSC_CSW_LENGTH;
    /* Call Back Function Info */
    usb_ghmsc_ReceiveData[msgnum].complete  = &usb_hmsc_CheckResult;

    err = usb_hmsc_SubmitutrReq(ptr, (uint16_t)USB_DATA_OK, &usb_ghmsc_ReceiveData[msgnum]);
    if( err != USB_E_OK ) 
    {
        USB_PRINTF1("### Mass Storage Device Class submit error(drive:%d) !\n", drvnum);
        return USB_HMSC_SUBMIT_ERR;
    }
    return (err);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_GetCswCheck
Description     : Check Receive CSW
Arguments       : uint16_t drvnum           : Drive Number
                : uint16_t hmsc_retval      : Return Value
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetCswCheck(USB_UTR_t *ptr, uint16_t drvnum, uint16_t hmsc_retval)
{
    uint16_t        pipeno, msgnum;

    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);

    switch( hmsc_retval ) 
    {
    case USB_DATA_SHT:
        /* Continue */
    case USB_DATA_OK:
        /* CSW Check */
        pipeno = R_usb_hmsc_Information(usb_ghmsc_InPipe[msgnum][0]);
        usb_ghmsc_InPipe[msgnum][1] = usb_ghmsc_ReceiveData[msgnum].pipectr;
        return usb_hmsc_CheckCsw(ptr, drvnum);
        break;
    case USB_DATA_STALL:
        /* Stall */
        USB_PRINTF1("*** GetCSW Transfer STALL(drive:%d) !\n", drvnum);
        return USB_MSG_HMSC_DATA_STALL;
        break;
    case USB_DATA_TMO:
        /* Timeout */
        USB_PRINTF1("### usb_hmscCSW Transfer timeout ERROR(drive:%d) !\n", drvnum);
        pipeno = R_usb_hmsc_Information(usb_ghmsc_InPipe[msgnum][0]);
        R_usb_hstd_TransferEnd(ptr, pipeno, (uint16_t)USB_DATA_TMO);
        break;
    case USB_DATA_ERR:
        USB_PRINTF1("### usb_hmscCSW Transfer ERROR(drive:%d) !\n"
            , drvnum);
        break;
    case USB_DATA_OVR:
        USB_PRINTF1("### usb_hmscCSW receive over(drive:%d) !\n", drvnum);
        break;
    default:
        USB_PRINTF1("### usb_hmscCSW Transfer error(drive:%d) !\n", drvnum);
        break;
    }
    return USB_HMSC_CSW_ERR;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_GetStringInfoCheck
Description     : Check Get string descriptor infomation
Arguments       : uint16_t devaddr          : Device Address
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetStringInfoCheck(uint16_t devaddr)
{
/* Condition compilation by the difference of useful function */
 #ifdef USB_DEBUGPRINT_PP
    uint32_t    j;
    uint8_t     pdata[32];
 #endif /* USB_DEBUGPRINT_PP */

    if( usb_ghmsc_ClassData[0] < (uint8_t)(30 * 2 + 2) ) 
    {
        USB_PRINTF0("*** Serial Number short\n");
        usb_ghmsc_ClassData[0]  = (uint8_t)(usb_ghmsc_ClassData[0] / 2);
        usb_ghmsc_ClassData[0]  = (uint8_t)(usb_ghmsc_ClassData[0] - 1);
    } 
    else 
    {
        usb_ghmsc_ClassData[0]  = 30;
    }
/* Condition compilation by the difference of useful function */
 #ifdef USB_DEBUGPRINT_PP
    for( j = (uint16_t)0; j < usb_ghmsc_ClassData[0]; j++ ) 
    {
        pdata[j]    = usb_ghmsc_ClassData[j * (uint16_t)2 + (uint16_t)2];
    }
    pdata[usb_ghmsc_ClassData[0]] = 0;
    USB_PRINTF1("    Serial Number : %s\n", pdata);
    if( usb_ghmsc_ClassData[0] < (uint8_t)(12 * 2 + 2) ) 
    {
    }
 #endif /* USB_DEBUGPRINT_PP */
    return USB_DONE;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_GetMaxUnit
Description     : Get Max LUN request
Argument        : uint16_t addr             : Device Address
                : USB_CB_t complete         : CallBack Function
Return value    : USB_ER_t                  : Error Code
******************************************************************************/
USB_ER_t R_usb_hmsc_GetMaxUnit(USB_UTR_t *ptr, uint16_t addr, USB_CB_t complete)
{
    USB_ER_t err;
    static uint16_t get_max_lun_table[5] = {0xFEA1, 0x0000, 0x0000, 0x0001, 0x0000};

    /* Device address set */
    get_max_lun_table[4] = addr;

    /* Recieve MaxLUN */
    usb_ghmsc_ControlData.keyword   = USB_PIPE0;
    usb_ghmsc_ControlData.tranadr   = (void*)usb_ghmsc_Data;
    usb_ghmsc_ControlData.tranlen   = (uint32_t)1;
    usb_ghmsc_ControlData.setup     = get_max_lun_table;
    usb_ghmsc_ControlData.complete  = complete;
    usb_ghmsc_ControlData.segment   = USB_TRAN_END;

    err = usb_hmsc_SubmitutrReq(ptr, (uint16_t)USB_CTRL_END, &usb_ghmsc_ControlData);
    return err;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_GetMaxUnitCheck
Description     : Check Get Max LUN request
Argument        : uint16_t err              : Error Code
Return value    : USB_ER_t                 : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetMaxUnitCheck(USB_UTR_t *ptr, uint16_t err)
{
    if( err == (uint16_t)USB_DATA_STALL ) 
    {
        USB_PRINTF0("*** GetMaxUnit not support !\n");
        return USB_ERROR;
    } 
    else if( err == (uint16_t)USB_DATA_TMO ) 
    {
        USB_PRINTF0("*** GetMaxUnit not support(time out) !\n");
        usb_hmsc_ControlEnd(ptr, (uint16_t)USB_DATA_TMO);
        return USB_ERROR;
    } 
    else if( err != (uint16_t)USB_CTRL_END ) 
    {
        USB_PRINTF1(
            "### [GetMaxUnit] control transfer error(%d) !\n", err);
        return USB_ERROR;
    } 
    else 
    {
    }
    return (usb_ghmsc_Data[0]);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_MassStorageReset
Description     : Mass Strage Reset request
Argument        : uint16_t drvnum           : Drive Number
                : USB_CB_t complete        : Callback Funtion
Return value    : USB_ER_t                 : Error Code
******************************************************************************/
USB_ER_t R_usb_hmsc_MassStorageReset(USB_UTR_t *ptr, uint16_t drvnum, USB_CB_t complete)
{
    USB_ER_t err;
    static uint16_t mass_storage_reset_table[5] = {0xFF21, 0x0000, 0x0000, 0x0000, 0x0000};

    /* Device address set */
    mass_storage_reset_table[4] = usb_hmsc_SmpDrive2Addr(drvnum);

    /* Set MassStorageReset */
    usb_ghmsc_ControlData.keyword   = USB_PIPE0;
    usb_ghmsc_ControlData.tranadr   = (void*)usb_ghmsc_Data;
    usb_ghmsc_ControlData.tranlen   = (uint32_t)0;
    usb_ghmsc_ControlData.setup     = mass_storage_reset_table;
    usb_ghmsc_ControlData.complete  = complete;
    usb_ghmsc_ControlData.segment   = USB_TRAN_END;

    err = usb_hmsc_SubmitutrReq(ptr, (uint16_t)USB_CTRL_END, &usb_ghmsc_ControlData);
    return err;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_MassStorageResetCheck
Description     : Check Mass Strage Reset request
Argument        : uint16_t err              : Error Code
Return value    : USB_ER_t                 : Error Code
******************************************************************************/
uint16_t usb_hmsc_MassStorageResetCheck(USB_UTR_t *ptr, uint16_t err)
{
    if( err == (uint16_t)USB_DATA_STALL ) 
    {
        USB_PRINTF0("*** MassStorageReset not support !\n");
        return USB_ERROR;
    } 
    else if( err == (uint16_t)USB_DATA_TMO ) 
    {
        USB_PRINTF0("*** MassStorageReset not support(time out) !\n");
        usb_hmsc_ControlEnd(ptr, (uint16_t)USB_DATA_TMO);
        return USB_ERROR;
    } 
    else if( err != (uint16_t)USB_CTRL_END ) 
    {
        USB_PRINTF1("### [MassStorageReset] control transfer error(%d) !\n", err);
        return USB_ERROR;
    } 
    else 
    {
    }
    return USB_DONE;
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_ClearStall
Description     : Clear Stall
Arguments       : uint16_t Pipe : 
                : USB_CB_t complete : 
Return value    : uint16_t
******************************************************************************/
USB_ER_t usb_hmsc_ClearStall(USB_UTR_t *ptr, uint16_t Pipe, USB_CB_t complete)
{
    USB_ER_t    err;

    err = R_usb_hstd_ChangeDeviceState(ptr, (USB_CB_INFO_t)complete, USB_MSG_HCD_CLR_STALL, Pipe);
    return err;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_ClearStallCheck
Description     : Clear Stall Check
Arguments       : uint16_t errcheck : 
Return value    : uint16_t
******************************************************************************/
void usb_hmsc_ClearStallCheck(USB_UTR_t *ptr, uint16_t errcheck)
{
    uint16_t retval;

    retval = usb_hmsc_StdReqCheck(errcheck);
    if( retval == USB_DONE )
    {
        /* Clear STALL */
        R_usb_hstd_ChangeDeviceState(ptr, (USB_CB_INFO_t)&usb_hmsc_DummyFunction
                    , USB_MSG_HCD_CLR_STALLBIT, usb_shmsc_ClearStallPipe);

        /* SQCLR */
        R_usb_hstd_ChangeDeviceState(ptr, (USB_CB_INFO_t)&usb_hmsc_DummyFunction
                    , USB_MSG_HCD_CLRSEQBIT, usb_shmsc_ClearStallPipe);
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_ClearStallCheck2
Description     : Clear Stall Check 2
Arguments       : USB_UTR_t *mess : 
Return value    : none
******************************************************************************/
void usb_hmsc_ClearStallCheck2(USB_UTR_t *mess)
{
    uint16_t errcheck;

    errcheck = mess->status;
    usb_hmsc_ClearStallCheck(mess, errcheck);

    mess->msginfo = usb_shmsc_Process;
    usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_Submitutr
Description     : Data transfer request
Argument        : uint16_t type             : Data Transmit/Data Receive
                : USB_UTR_t *utr_table     : Information Table
Return value    : USB_ER_t                 : Error Code
******************************************************************************/
USB_ER_t usb_hmsc_Submitutr(USB_UTR_t *ptr, uint16_t type, USB_UTR_t *utr_table)
{
    uint16_t    msgnum, pipeno, offset2;

    usb_shmsc_MsgNum = utr_table->keyword;

    switch( type ) 
    {
    case USB_CTRL_END:                      /* Control transfer */
        break;
    case USB_DATA_NONE:                     /* Data transmit */
        msgnum              = utr_table->keyword;
        offset2             = usb_ghmsc_OutPipe[msgnum][0];
        pipeno              = R_usb_hmsc_Information(offset2);
        utr_table->keyword  = pipeno;
        R_usb_hstd_SetPipeInfo(&usb_ghmsc_DefEpTbl[0]
            , &usb_ghmsc_TmpEpTbl[offset2], (uint16_t)USB_EPL);
        R_usb_hstd_SetPipeRegistration(ptr, (uint16_t*)&usb_ghmsc_DefEpTbl, pipeno);
        break;
    case USB_DATA_OK:                           /* Data recieve */
        msgnum              = utr_table->keyword;
        offset2             = usb_ghmsc_InPipe[msgnum][0];
        pipeno              = R_usb_hmsc_Information(offset2);
        utr_table->keyword  = pipeno;
        R_usb_hstd_SetPipeInfo(&usb_ghmsc_DefEpTbl[0], &usb_ghmsc_TmpEpTbl[offset2], (uint16_t)USB_EPL);
        R_usb_hstd_SetPipeRegistration(ptr, (uint16_t*)&usb_ghmsc_DefEpTbl, pipeno);
        break;
    default:
        break;
    }
    return USB_E_OK;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SubmitutrReq
Description     : Data transfer request
Argument        : uint16_t type             : Data Transmit/Data Receive
                : USB_UTR_t *utr_table      : Information Table
Return value    : USB_ER_t                  : Error Code
******************************************************************************/
USB_ER_t usb_hmsc_SubmitutrReq(USB_UTR_t *ptr, uint16_t type, USB_UTR_t *utr_table)
{
    uint16_t    msgnum, pipeno;
    USB_ER_t    err;

    msgnum = usb_shmsc_MsgNum;

    utr_table->ip = ptr->ip;
    utr_table->ipp = ptr->ipp;

    switch( type ) 
    {
    case USB_CTRL_END:                      /* Control transfer */
        err = R_usb_hstd_TransferStart(utr_table);
        break;
    case USB_DATA_NONE:                     /* Data transmit */
        pipeno = utr_table->keyword;

        usb_hmsc_DoSqtgl(utr_table, pipeno, usb_ghmsc_OutPipe[msgnum][1]);
        err = R_usb_hstd_TransferStart(utr_table);
        break;
    case USB_DATA_OK:                       /* Data recieve */
        pipeno = utr_table->keyword;

        usb_hmsc_DoSqtgl(utr_table, pipeno, usb_ghmsc_InPipe[msgnum][1]);
        err = R_usb_hstd_TransferStart(utr_table);
        break;
    default:
        USB_PRINTF0("### submit error\n");
        err = USB_HMSC_SUBMIT_ERR;
        break;
    }
    return err;
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_TransResult
Description     : callback function
Arguments       : USB_UTR_t *mess           : Transfer Result Message
Return value    : none
******************************************************************************/
void usb_hmsc_TransResult(USB_UTR_t *mess, uint16_t data1, uint16_t data2)
{
    USB_ER_t        err;

    err = USB_SND_MSG(USB_HMSC_MBX, (USB_MSG_t*)mess);
    if( err != USB_E_OK ) 
    {
        USB_PRINTF1("### Mass Storage snd_msg error (%ld)\n", err);
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_TransWaitTmo
Description     : Waiting for HCD data transfer end or time out
Arguments       : uint16_t tmo              : Timeout Value
Return value    : uint16_t                  : Status
******************************************************************************/
uint16_t usb_hmsc_TransWaitTmo(uint16_t tmo)
{
    return USB_DONE;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_ControlEnd
Description     : Control end function
Argument        : uint16_t  sts             : Status
Return value    : none
******************************************************************************/
void usb_hmsc_DoSqtgl(USB_UTR_t *ptr, uint16_t Pipe, uint16_t toggle)
{
    uint16_t    dat;

    dat = (toggle & 0xFFF0) | Pipe;

    R_usb_hstd_ChangeDeviceState(ptr, (USB_CB_INFO_t)&usb_hmsc_DummyFunction, USB_MSG_HCD_SQTGLBIT, dat);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_ControlEnd
Description     : Control end function
Argument        : uint16_t  sts             : Status
Return value    : none
******************************************************************************/
void usb_hmsc_ControlEnd(USB_UTR_t *ptr, uint16_t sts)
{
    R_usb_hstd_ChangeDeviceState(ptr, (USB_CB_INFO_t)&usb_hmsc_DummyFunction, USB_MSG_HCD_CTRL_END, sts);
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/

