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
* File Name    : r_usb_hmsc_ddi.c
* Version      : 1.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB Host MSC BOT ddi
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.08.2010 0.91    First Release
*         : 06.08.2010 0.92    Updated comments
*         : 29.10.2010 1.00    Mass Production Release
******************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_usrconfig.h"        /* USB-H/W register set (user define) */
#include "r_usb_cmsc_define.h"      /* USB  Mass Storage Class Header */
#include "r_usb_hmsc_define.h"      /* Host Mass Storage Class Driver define */
#include "r_usb_catapi_define.h"        /* Peripheral ATAPI Device extern define */
#include "r_usb_cDefUSBIP.h"        /* USB-FW Library Header */
#include "r_usb_cMacSystemcall.h"   /* System call macro */
#include "r_usb_cMacPrint.h"        /* LCD/SIO disp macro */
#include "r_usb_cKernelId.h"        /* Kernel ID definition */
#include "r_usb_hmsc_extern.h"      /* Host MSC grobal define */
#include "r_usb_cExtern.h"          /* USB-FW grobal define */

#include "r_usb_api.h"

/******************************************************************************
Section    <Section Difinition> , "Project Sections"
******************************************************************************/
#pragma section _hmsc

/******************************************************************************
External variables and functions
******************************************************************************/
extern uint16_t         usb_shmsc_Process;

extern uint16_t usb_hmsc_GetStringDescriptor(USB_UTR_t *ptr, uint16_t devaddr, uint16_t index);
extern uint16_t usb_hmsc_GetStringDescriptor1(USB_UTR_t *ptr, uint16_t devaddr, uint16_t index, USB_CB_t complete);
extern uint16_t usb_hmsc_GetStringDescriptor2(USB_UTR_t *ptr, uint16_t devaddr, uint16_t index, USB_CB_t complete);
extern uint16_t usb_hmsc_GetStringDescriptor1Check(USB_UTR_t *ptr, uint16_t errcheck);
extern uint16_t usb_hmsc_GetStringDescriptor2Check(USB_UTR_t *ptr, uint16_t errcheck);

/******************************************************************************
Private global variables and functions
******************************************************************************/
uint8_t     *usb_ghmsc_DeviceTable;
uint8_t     *usb_ghmsc_ConfigTable;
uint8_t     *usb_ghmsc_InterfaceTable;
uint16_t    usb_ghmsc_Speed;
uint16_t    usb_ghmsc_Devaddr;
uint16_t    usb_shmsc_InitSeq   =   USB_SEQ_0;


/******************************************************************************
Private global variables and functions
******************************************************************************/
/******************************************************************************
* Endpoint Configuration Data Format
*******************************************************************************
* LINE1: Pipe Window Select Register (0x64)
*   CPU Access PIPE             : PIPE1 to MAX_PIPE_NO  : Class Driver
*   LINE2: Pipe Configuration Register (0x66)
*       Transfer Type           : USB_NONE              : Endpoint Descriptor
*       Buffer Ready interrupt  : USB_BFREOFF           : Class Driver
*       Double Buffer Mode      : USB_DBLBON            : Class Driver
*       Continuous Transmit:    : USB_CNTMDON           : Class Driver
*       Short NAK               : USB_NONE              : Endpoint Descriptor
*       Transfer Direction      : USB_NONE              : Endpoint Descriptor
*       Endpoint Number         : USB_NONE              : Endpoint Descriptor
*   LINE3: Pipe Buffer Configuration Register (0x68)
*       Buffer Size             : USB_BUF_SIZE(x)       : Class Driver
*       Buffer Top Number       : USB_BUF_NUMB(x)       : Class Driver
*   LINE4: Pipe Maxpacket Size Register (0x6A)
*       Max Packet Size         : USB_NONE              : Endpoint Descriptor
*   LINE5: Pipe Cycle Configuration Register (0x6C)
*       ISO Buffer Flush Mode   : USB_NONE              : Class Driver
*       ISO Interval Value      : USB_NONE              : Class Driver
*   LINE6: use FIFO port
*       CUSE/D1USE/D0DMA        : USB_CUSE              : Class Driver
******************************************************************************/

uint16_t usb_ghmsc_DefEpTbl[] =
{
    USB_PIPE1,
    USB_NONE | USB_BFREOFF | USB_DBLBON | USB_CNTMDON | USB_NONE
    | USB_NONE | USB_NONE,
    USB_NONE,
    USB_NONE,
    USB_NONE,
    USB_CUSE,
/* Pipe end */
    USB_PDTBLEND,
};

uint16_t usb_ghmsc_TmpEpTbl[] =
{
#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
    USB_NONE,
#else   /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */
    USB_PIPE1,
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */
    /* TYPE/BFRE/DBLB/CNTMD/SHTNAK/DIR/EPNUM */
    USB_NONE | USB_BFREOFF | USB_DBLBON | USB_CNTMDON | USB_SHTNAKON
    | USB_NONE | USB_NONE,
    USB_NONE,
    USB_NONE,
    USB_NONE,
    USB_CUSE,

#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
    USB_NONE,
#else   /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */
    USB_PIPE1,
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */
    /* TYPE/BFRE/DBLB/CNTMD/SHTNAK/DIR/EPNUM */
    USB_NONE | USB_BFREOFF | USB_DBLBON | USB_CNTMDON | USB_SHTNAKON
    | USB_NONE | USB_NONE,
    USB_NONE,
    USB_NONE,
    USB_NONE,
    USB_CUSE,

#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
    USB_NONE,
#else   /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */
    USB_PIPE1,
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */
    /* TYPE/BFRE/DBLB/CNTMD/SHTNAK/DIR/EPNUM */
    USB_NONE | USB_BFREOFF | USB_DBLBON | USB_CNTMDON | USB_SHTNAKON
    | USB_NONE | USB_NONE,
    USB_NONE,
    USB_NONE,
    USB_NONE,
    USB_CUSE,

#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
    USB_NONE,
#else   /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */
    USB_PIPE1,
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */
    /* TYPE/BFRE/DBLB/CNTMD/SHTNAK/DIR/EPNUM */
    USB_NONE | USB_BFREOFF | USB_DBLBON | USB_CNTMDON | USB_SHTNAKON
    | USB_NONE | USB_NONE,
    USB_NONE,
    USB_NONE,
    USB_NONE,
    USB_CUSE,

    /* Pipe end */
    USB_PDTBLEND,
};

/******************************************************************************
Renesas Abstracted HMSC Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_hmsc_TaskOpen
Description     : Open Mass storage class driver
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_hmsc_TaskOpen(USB_UTR_t *ptr, uint16_t data1, uint16_t data2)
{
    USB_PRINTF0("*** Install Host MSCD device driver ***\n");
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_TaskClose
Description     : Close Mass storage class driver
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_hmsc_TaskClose(USB_UTR_t *ptr)
{
    USB_PRINTF0("*** Release Host MS device driver ***\n");
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_Initialized
Description     : initialized USB_HMSC_TSK
Arguments       : uint16_t data1 : 
                : uint16_t data2 : 
Return value    : none
******************************************************************************/
void R_usb_hmsc_Initialized(USB_UTR_t *ptr, uint16_t data1, uint16_t data2)
{
    uint16_t    i;

    /* 0x00 */
    usb_ghmsc_AttSts = USB_HMSC_DEV_DET;

    for( i = 0; i < USB_MAXSTRAGE; i++ )
    {
        usb_ghmsc_CbwTagNo[i] = (uint16_t)1;

        usb_ghmsc_Cbw[i].dCBWSignature = USB_MSC_CBW_SIGNATURE;
        usb_ghmsc_Cbw[i].dCBWTag = usb_ghmsc_CbwTagNo[i];
        usb_ghmsc_Cbw[i].dCBWDTL_Lo = 0;
        usb_ghmsc_Cbw[i].dCBWDTL_ML = 0;
        usb_ghmsc_Cbw[i].dCBWDTL_MH = 0;
        usb_ghmsc_Cbw[i].dCBWDTL_Hi = 0;
        usb_ghmsc_Cbw[i].bmCBWFlags.CBWdir = 0;
        usb_ghmsc_Cbw[i].bmCBWFlags.reserved7 = 0;
        usb_ghmsc_Cbw[i].bCBWLUN.bCBWLUN = 0;
        usb_ghmsc_Cbw[i].bCBWLUN.reserved4 = 0;
        usb_ghmsc_Cbw[i].bCBWCBLength.bCBWCBLength = 0;
        usb_ghmsc_Cbw[i].bCBWCBLength.reserved3 = 0;

        usb_ghmsc_Cbw[i].CBWCB[0]  = 0;
        usb_ghmsc_Cbw[i].CBWCB[1]  = 0;
        usb_ghmsc_Cbw[i].CBWCB[2]  = 0;
        usb_ghmsc_Cbw[i].CBWCB[3]  = 0;
        usb_ghmsc_Cbw[i].CBWCB[4]  = 0;
        usb_ghmsc_Cbw[i].CBWCB[5]  = 0;
        usb_ghmsc_Cbw[i].CBWCB[6]  = 0;
        usb_ghmsc_Cbw[i].CBWCB[7]  = 0;
        usb_ghmsc_Cbw[i].CBWCB[8]  = 0;
        usb_ghmsc_Cbw[i].CBWCB[9]  = 0;
        usb_ghmsc_Cbw[i].CBWCB[10] = 0;
        usb_ghmsc_Cbw[i].CBWCB[11] = 0;
        usb_ghmsc_Cbw[i].CBWCB[12] = 0;
        usb_ghmsc_Cbw[i].CBWCB[13] = 0;
        usb_ghmsc_Cbw[i].CBWCB[14] = 0;
        usb_ghmsc_Cbw[i].CBWCB[15] = 0;
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_ClassCheck
Description     : check connected device
Arguments       : uint16_t **table : 
Return value    : none
******************************************************************************/
void R_usb_hmsc_ClassCheck(USB_UTR_t *ptr, uint16_t **table)
{
#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
    uint16_t    *result;

    result          = table[3];
    *result         = USB_DONE;
#else

    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *cp;

    usb_ghmsc_DeviceTable       = (uint8_t*)(table[0]);
    usb_ghmsc_ConfigTable       = (uint8_t*)(table[1]);
    usb_ghmsc_InterfaceTable    = (uint8_t*)(table[2]);
    usb_ghmsc_Speed             = *table[6];
    usb_ghmsc_Devaddr           = *table[7];
    *table[3]                   = USB_DONE;

    /* Get mem pool blk */
    if( USB_PGET_BLK(USB_HMSC_MPL,&p_blf) == USB_OK )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = USB_MSG_CLS_INIT;
        usb_shmsc_InitSeq = USB_SEQ_0;

        cp->ip = ptr->ip;
        cp->ipp = ptr->ipp;

        /* Send message */
        err = USB_SND_MSG( USB_HMSC_MBX, (USB_MSG_t*)p_blf );
        if( err != USB_E_OK )
        {
            err = USB_REL_BLK(USB_HMSC_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### ClassCheck function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### ClassCheck function pget_blk error\n");
    }   
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_ClassCheck
Description     : check class
Arguments       : USB_CLSINFO_t *mess : message
Return value    : none
******************************************************************************/
void usb_hmsc_ClassCheck(USB_UTR_t *ptr, USB_CLSINFO_t *mess)
{
    uint16_t ofset, result;

    result = USB_DONE;
    switch( usb_shmsc_InitSeq )
    {
    case USB_SEQ_0:
        /* Check device count */
        if( usb_ghmsc_StrgCount == USB_MAXSTRAGE )
        {
            USB_PRINTF0("### max device count over(HMSC) !\n");
            result = USB_ERROR;
        }

        /* Check drive count */
        if( usb_ghmsc_MaxDrive >= USB_MAXDRIVE )
        {
            USB_PRINTF1("    max drive over %d(HMSC) .\n",
                usb_ghmsc_MaxDrive);
            result = USB_ERROR;
        }

        /* Descriptor check */
        ofset = usb_hmsc_SmpBotDescriptor(usb_ghmsc_InterfaceTable, usb_ghmsc_StrgCount);
        if( ofset == USB_ERROR )
        {
            USB_PRINTF0("### Descriptor search error(HMSC) !\n");
            result = USB_ERROR;
        }

        /* Serial number check */
        if( result != USB_ERROR )
        {
            /* no string device (STALL) */
            if( usb_ghmsc_DeviceTable[14] == 0
                && usb_ghmsc_DeviceTable[15] == 0
                && usb_ghmsc_DeviceTable[16] == 0 ) {

                ofset = usb_hmsc_SmpPipeInfo(usb_ghmsc_InterfaceTable
                            , usb_ghmsc_StrgCount, usb_ghmsc_Speed
                            , (uint16_t)usb_ghmsc_ConfigTable[2]);
                if( ofset == USB_ERROR ) {
                    USB_PRINTF0("### Device information error !\n");
                }
                R_usb_hstd_ReturnEnuMGR(ptr, ofset);    /* return to MGR */
                usb_shmsc_InitSeq = USB_SEQ_0;
                return;
            }

            usb_shmsc_Process = USB_MSG_CLS_INIT;
            ofset = usb_hmsc_GetStringDescriptor1(ptr, usb_ghmsc_Devaddr,
                     (uint16_t)usb_ghmsc_DeviceTable[16],
                     (USB_CB_t)usb_hmsc_CheckResult );
            usb_shmsc_InitSeq++;
        }
        break;
    case USB_SEQ_1:
        ofset = usb_hmsc_GetStringDescriptor1Check(ptr, mess->result);
        if( ofset == USB_ERROR )
        {
            result = USB_ERROR;
        }
        else
        {
            ofset = usb_hmsc_GetStringDescriptor2(ptr, usb_ghmsc_Devaddr,
                     (uint16_t)usb_ghmsc_DeviceTable[15],
                     (USB_CB_t)usb_hmsc_CheckResult );
            usb_shmsc_InitSeq++;
        }
        break;
    case USB_SEQ_2:
        /* Serial number check */
        ofset = usb_hmsc_GetStringDescriptor2Check(ptr, mess->result);
        if( ofset == USB_ERROR )
        {
            result = USB_ERROR;
        }
        
        ofset = usb_hmsc_GetStringInfoCheck(usb_ghmsc_Devaddr);
        if( ofset == USB_ERROR )
        {
            USB_PRINTF0("*** This device is No Serial Number\n");
            result = USB_ERROR;
        }
        
        if( result != USB_ERROR )
        {
            /* Pipe Information table set */
            ofset = usb_hmsc_SmpPipeInfo(usb_ghmsc_InterfaceTable,
                     usb_ghmsc_StrgCount, usb_ghmsc_Speed,
                     (uint16_t)usb_ghmsc_ConfigTable[2]);
            if( ofset == USB_ERROR )
            {
                USB_PRINTF0("### Device information error !\n");
            }
            /* Return to MGR */
            R_usb_hstd_ReturnEnuMGR(ptr, ofset);
            usb_shmsc_InitSeq = USB_SEQ_0;
        }
        break;
    default:
        result = USB_ERROR;
        break;
    }
    
    if( result == USB_ERROR )
    {
        usb_shmsc_InitSeq = USB_SEQ_0;
        /* Return to MGR */
        R_usb_hstd_ReturnEnuMGR(ptr, USB_ERROR);
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Renesas Abstracted Mass Storage Device Class command
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_hmsc_Read10
Description     : Read10
Arguments       : uint16_t side : 
                : uint8_t *buff : 
                : uint32_t secno : 
                : uint16_t seccnt : 
                : uint32_t trans_byte : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_Read10(USB_UTR_t *ptr, uint16_t side, uint8_t *buff, uint32_t secno,
     uint16_t seccnt, uint32_t trans_byte)
{
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(side);
    if( unit == USB_ERROR )
    {
        USB_PRINTF2("### unit error(Read10:side=%d,unit=%d)\n", side, unit);
        return (USB_ERROR);
    }

    /* set CBW parameter */
    usb_hmsc_SetRwCbw((uint16_t)USB_ATAPI_READ10, secno, seccnt, trans_byte, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_DataIn(ptr, side, buff, trans_byte);
    return (hmsc_retval);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_Write10
Description     : Write10
Arguments       : uint16_t side : 
                : uint8_t *buff : 
                : uint32_t secno : 
                : uint16_t seccnt : 
                : uint32_t trans_byte : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_Write10(USB_UTR_t *ptr, uint16_t side, uint8_t *buff, uint32_t secno,
     uint16_t seccnt, uint32_t trans_byte)
{
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(side);
    if( unit == USB_ERROR )
    {
        USB_PRINTF2("### unit error(Write10:side=%d,unit=%d)\n", side, unit);
        return (USB_ERROR);
    }

    /* set CBW parameter */
    usb_hmsc_SetRwCbw((uint16_t)USB_ATAPI_WRITE10, secno, seccnt, trans_byte, side);

    /* Data OUT */
    hmsc_retval = usb_hmsc_DataOut(ptr, side, buff, trans_byte);
    return (hmsc_retval);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_PreventAllow
Description     : PreventAllow
Arguments       : uint16_t side : 
                : uint8_t *buff : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_PreventAllow(USB_UTR_t *ptr, uint16_t side, uint8_t *buff)
{
    uint8_t     data[12];
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(side);
    if( unit == USB_ERROR )
    {
        USB_PRINTF2("### unit error(PreventAllow:side=%d,unit=%d)\n", side, unit);
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_PREVENT_ALLOW;
    /* Reserved */
    data[4] = buff[0];

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw((uint8_t*)&data, (uint32_t)0, side);

    /* No Data */
    hmsc_retval = usb_hmsc_NoData(ptr, side);
    return (hmsc_retval);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_TestUnit
Description     : TestUnit
Arguments       : uint16_t side : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_TestUnit(USB_UTR_t *ptr, uint16_t side)
{
    uint8_t     data[12];
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(side);
    if( unit == USB_ERROR )
    {
        USB_PRINTF2("### unit error(TestUnit:side=%d,unit=%d)\n", side, unit);
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_TEST_UNIT_READY;

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw((uint8_t*)&data, (uint32_t)0, side);

    /* No Data */
    hmsc_retval = usb_hmsc_NoData(ptr, side);
    return (hmsc_retval);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_RequestSense
Description     : RequestSense
Arguments       : uint16_t side : 
                : uint8_t *buff : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_RequestSense(USB_UTR_t *ptr, uint16_t side, uint8_t *buff)
{
    uint8_t     data[12];
    uint8_t     length = 18;
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(side);
    if( unit == USB_ERROR )
    {
        USB_PRINTF2("### unit error(RequestSense:side=%d,unit=%d)\n", side, unit);
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_REQUEST_SENSE;
    /* Allocation length */
    data[4] = length;

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw((uint8_t*)&data, (uint32_t)length, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_DataIn(ptr, side, buff, (uint32_t)length);
    return (hmsc_retval);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_Inquiry
Description     : Inquiry
Arguments       : uint16_t side : 
                : uint8_t *buff : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_Inquiry(USB_UTR_t *ptr, uint16_t side, uint8_t *buff)
{
    uint8_t     data[12];
    uint8_t     length = 36;
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(side);
    if( unit == USB_ERROR )
    {
        USB_PRINTF2("### unit error(Inquiry:side=%d,unit=%d)\n", side, unit);
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_INQUIRY;
    /* Allocation length */
    data[4] = length;

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw((uint8_t*)&data, (uint32_t)length, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_DataIn(ptr, side, buff, (uint32_t)length);
    return (hmsc_retval);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_ReadCapacity
Description     : ReadCapacity
Arguments       : uint16_t side : 
                : uint8_t *buff : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_ReadCapacity(USB_UTR_t *ptr, uint16_t side, uint8_t *buff)
{
    uint8_t     data[12];
    uint8_t     length = 8;
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(side);
    if( unit == USB_ERROR )
    {
        USB_PRINTF2("### unit error(ReadCapacity:side=%d,unit=%d)\n", side, unit);
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_READ_CAPACITY;

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw((uint8_t*)&data, (uint32_t)length, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_DataIn(ptr, side, buff, (uint32_t)length);
    return (hmsc_retval);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_ReadFormatCapacity
Description     : ReadFormatCapacity
Arguments       : uint16_t side : 
                : uint8_t *buff : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_ReadFormatCapacity(USB_UTR_t *ptr, uint16_t side, uint8_t *buff)
{
    uint8_t     data[12];
    uint8_t     length = 0x20;
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(side);
    if( unit == USB_ERROR )
    {
        USB_PRINTF2("### unit error(read_format:side=%d,unit=%d)\n", side, unit);
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_READ_FORMAT_CAPACITY;
    /* Allocation length */
    data[8] = length;

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw((uint8_t*)&data, (uint32_t)length, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_DataIn(ptr, side, buff, (uint32_t)length);
    return (hmsc_retval);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_ModeSense10
Description     : ModeSense10
Arguments       : uint16_t side : 
                : uint8_t *buff : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_ModeSense10(USB_UTR_t *ptr, uint16_t side, uint8_t *buff)
{
    uint8_t     data[12];
    uint8_t     length = 26;
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(side);
    if( unit == USB_ERROR )
    {
        USB_PRINTF2("### unit error(ModeSense10:side=%d,unit=%d)\n", side, unit);
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_MODE_SENSE10;
    /* Set LUN / DBD=1 */
    data[1] = 0x08;
    /* Allocation length */
    data[7] = 0x00;
    /* Allocation length */
    data[8] = 0x02;

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw((uint8_t*)&data, (uint32_t)length, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_DataIn(ptr, side, buff, (uint32_t)length);
    return (hmsc_retval);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_ModeSelect6
Description     : ModeSelect6
Arguments       : uint16_t side : 
                : uint8_t *buff : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_ModeSelect6(USB_UTR_t *ptr, uint16_t side, uint8_t *buff)
{
    uint8_t     data[12];
    uint8_t     length = 18;
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(side);
    if( unit == USB_ERROR )
    {
        USB_PRINTF2("### unit error(mode_sense6:side=%d,unit=%d)\n", side, unit);
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_MODE_SELECT6;
    /* SP=0 */
    data[1] = 0x10;
    /* Parameter list length ??? */
    data[4] = 0x18;

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw((uint8_t*)&data, (uint32_t)length, side);

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)24, buff);

    /* Data set */
    buff[3] = 0x08;
    buff[10] = 0x02;
    buff[12] = 0x08;
    buff[13] = 0x0A;

    /* Data OUT */
    hmsc_retval = usb_hmsc_DataOut(ptr, side, buff, (uint32_t)length);
    return (hmsc_retval);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_DriveSpeed
Description     : DriveSpeed
Arguments       : uint16_t side : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_DriveSpeed(USB_UTR_t *ptr, uint16_t side)
{
    uint16_t    tbl[10];

    R_usb_hstd_DeviceInformation(ptr, side, (uint16_t *)tbl);
    return (tbl[3]);            /* Speed */
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_ClrData
Description     : data clear
Arguments       : uint16_t len : 
                : uint8_t *buf : 
Return value    : none
******************************************************************************/
void usb_hmsc_ClrData(uint16_t len, uint8_t *buf)
{
    uint16_t    i;

    for( i = 0; i < len; ++i )
    {
        *buf++ = 0x00;
    }
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
External variables and functions
******************************************************************************/
extern uint16_t usb_ghmsc_ConnectCont;

/******************************************************************************
Function Name   : R_usb_hmsc_Release
Description     : Release Mass Strage Class driver
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_hmsc_Release(USB_UTR_t *ptr)
{
    R_usb_hstd_DriverRelease(ptr, (uint8_t)USB_IFCLS_MAS);
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/
