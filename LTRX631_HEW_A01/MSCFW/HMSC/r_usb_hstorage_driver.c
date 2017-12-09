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
* File Name    : r_usb_hstorage_driver.c
* Version      : 1.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB sample data declaration
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 17.03.2010 0.80    First Release
*         : 30.07.2010 0.90    Updated comments
*         : 02.08.2010 0.91    Updated comments
*         : 29.10.2010 1.00    Mass Production Release
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
//#include "r_usb_cDefUsrPp.h"      /* System definition */
#include "r_usb_cTypedef.h"         /* Type definition */
//#include "r_usb_cDefUsr.h"            /* USB-H/W register set (user define) */
#include "r_usb_usrconfig.h"
#include "r_usb_cDefUSBIP.h"        /* USB-FW Library Header */
#include "r_usb_cMacSystemcall.h"   /* System call macro */
#include "r_usb_cMacPrint.h"        /* LCD/SIO disp macro */
#include "r_usb_cExtern.h"          /* USB-FW global definition */
#include "r_usb_cKernelId.h"        /* Kernel ID definition */
#include "r_usb_catapi_define.h"        /* Peripheral ATAPI Device extern */
#include "r_usb_cmsc_define.h"      /* USB  Mass Storage Class Header */
#include "r_usb_hmsc_define.h"      /* Host Mass Storage Class Driver */
#include "r_usb_hmsc_extern.h"      /* MSC global definition */

/******************************************************************************
Section    <Section Definition> , "Project Sections"
******************************************************************************/
#pragma section _hmsc


/******************************************************************************
Constant macro definitions
******************************************************************************/
#define     USB_HMSC_CLSDATASIZE        512

/******************************************************************************
External variables and functions
******************************************************************************/
extern USB_ER_t R_usb_hstd_TransferStart(USB_UTR_t *ptr);
extern void R_usb_hstd_DeviceInformation(USB_UTR_t *ptr, uint16_t devaddr, uint16_t *tbl);

/******************************************************************************
Private global variables and functions
******************************************************************************/
uint8_t         usb_ghmsc_ClassData[USB_HMSC_CLSDATASIZE];
uint16_t        usb_ghmsc_DriveChk[USB_MAXDRIVE + 1][5];
uint8_t         usb_shmsc_DeviceReady[USB_MAXUNITNUM];
uint16_t        usb_shmsc_ClassRequest[5];
USB_UTR_t       usb_shmsc_ClassControl;

/* Yes/No, Unit Number, Partition Number, device address, EPtbl offset */
uint16_t        usb_ghmsc_StrgCount;
uint16_t        usb_ghmsc_MaxDrive;
uint32_t        usb_ghmsc_MaxLUN;

uint16_t        usb_shmsc_StrgProcess                   = USB_NONE;
uint16_t        usb_shmsc_StrgDriveSearchSeq            = USB_SEQ_0;
uint16_t        usb_shmsc_StrgDriveSearchErrCount       = USB_SEQ_0;
uint16_t        usb_shmsc_StrgDriveSearchCount          = USB_SEQ_0;

/* Read Sector */
uint16_t        usb_shmsc_DevReadSectorSizeSeq          = USB_SEQ_0;
uint16_t        usb_shmsc_DevReadSectorSizeErrCount     = USB_SEQ_0;
uint16_t        usb_shmsc_DevReadPartitionSeq           = USB_SEQ_0;

/* Drive Open */
uint16_t        usb_shmsc_StrgDriveOpenSeq              = USB_SEQ_0;
uint16_t        usb_shmsc_StrgDriveOpenCount            = USB_SEQ_0;
uint16_t        usb_shmsc_StrgDriveOpenParCount         = USB_SEQ_0;

/* Partition */
uint8_t         usb_shmsc_PartitionInfo[USB_BOOTPARTNUM];
uint32_t        usb_shmsc_PartitionLba[USB_BOOTPARTNUM + 1u];

uint16_t        usb_ghmsc_RootDevaddr;
uint16_t        usb_ghmsc_DriveOpen = USB_ERROR;

uint16_t        usb_shmsc_NewDrive;
uint16_t        usb_shmsc_LoopCont = USB_SEQ_0;
uint16_t        usb_shmsc_Unit;
uint16_t        usb_ghmsc_PartTransSize;

extern uint32_t     usb_ghmsc_TransSize;
extern uint16_t     usb_ghmsc_SmpAplProcess;

uint16_t        usb_hmsc_GetStringDesc(USB_UTR_t *ptr, uint16_t addr, uint16_t string, USB_CB_t complete);
uint16_t        usb_hmsc_CmdSubmit(USB_UTR_t *ptr, USB_CB_t complete);

/******************************************************************************
Renesas Abstracted Peripheral Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_hmsc_StrgTaskOpen
Description     : Storage task open
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_hmsc_StrgTaskOpen(USB_UTR_t *ptr)
{
    uint16_t        i;

    USB_PRINTF0("*** Install Host MS device driver ***\n");
    R_usb_hmsc_TaskOpen( ptr, 0, 0 );

    for( i = 0; i < (USB_MAXDRIVE + 1); i++ )
    {
        usb_ghmsc_DriveChk[i][0] = USB_NO;  /* Yes/No */
        usb_ghmsc_DriveChk[i][1] = 0;       /* Unit Number */
        usb_ghmsc_DriveChk[i][2] = 0;       /* Partition Number */
        usb_ghmsc_DriveChk[i][3] = 0;       /* Device address */
        usb_ghmsc_DriveChk[i][4] = 0;       /* Device number */
    }

    usb_ghmsc_MaxDrive = USB_DRIVE;
    usb_ghmsc_StrgCount = 0;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_StrgTaskClose
Description     : Storage task close
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_hmsc_StrgTaskClose(USB_UTR_t *ptr)
{
    /* Task close */
    R_usb_hmsc_TaskClose(ptr);
    USB_PRINTF0("*** Release Host MS device driver ***\n");
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_StrgDriveTask
Description     : Storage drive task
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hmsc_StrgDriveTask(void)
{
    USB_UTR_t       *mess;
    USB_ER_t        err;                    /* Error code */
    
    /* receive message */
    err = USB_TRCV_MSG( USB_HSTRG_MBX, (USB_MSG_t**)&mess, (uint16_t)0 );
    if( err != USB_E_OK )
    {
        return;
    }

    switch( mess->msginfo )
    {
    case USB_MSG_CLS_INIT:
        break;
    /* enumeration waiting of other device */
    case USB_MSG_CLS_WAIT:
        usb_hmsc_ClassWait(USB_HSTRG_MBX, mess);
        break;
    case USB_MSG_HMSC_STRG_DRIVE_SEARCH:
        /* Drive search */
        usb_hmsc_StrgDriveSearchAct((USB_CLSINFO_t *)mess);
        break;
    case USB_MSG_HMSC_DEV_READ_SECTOR_SIZE:
        /* Read sector */
        usb_hmsc_SmpDevReadSectorSizeAct((USB_CLSINFO_t *)mess);
        break;
    case USB_MSG_HMSC_DEV_READ_PARTITION:
        /* Read partition */
        usb_hmsc_SmpDevReadPartitionAct((USB_CLSINFO_t *)mess);
        break;
    case USB_MSG_HMSC_STRG_DRIVE_OPEN:
        /* Drive open */
        usb_hmsc_SmpStrgDriveOpenAct((USB_CLSINFO_t *)mess);
        break;
    default:
        break;
    }
    err = USB_REL_BLK(USB_HSTRG_MPL,(USB_MH_t)mess);
    if( err != USB_E_OK )
    {
        USB_PRINTF0("### USB Strg Task rel_blk error\n");
    }
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_SmpDevNextDriveSearch
Description     : Next drive search
Arguments       : none
Return value    : uint16_t             : 
******************************************************************************/
uint16_t usb_hmsc_SmpDevNextDriveSearch(void)
{
    uint16_t i;

    for( i = 0; i < USB_MAXDRIVE; i++ )
    {
        if( usb_ghmsc_DriveChk[i][0] == USB_NO )
        {
            return i;
        }
    }
    return (uint16_t)0;
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_hmsc_StrgDriveSearch
Description     : Searches drive SndMsg
Arguments       : uint16_t addr        : Address
Return value    : uint16_t             : Status
******************************************************************************/
uint16_t R_usb_hmsc_StrgDriveSearch(USB_UTR_t *ptr, uint16_t addr)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *cp;

    usb_shmsc_StrgDriveSearchSeq = USB_SEQ_0;

    /* Get mem pool blk */
    if( USB_PGET_BLK(USB_HSTRG_MPL,&p_blf) == USB_OK )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        cp->keyword = addr;

        cp->ip      = ptr->ip;
        cp->ipp     = ptr->ipp;

        /* Send message */
        err = USB_SND_MSG( USB_HSTRG_MBX, (USB_MSG_t*)p_blf );
        if( err != USB_E_OK )
        {
            err = USB_REL_BLK(USB_HSTRG_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### StrgDriveSearch function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### StrgDriveSearch function pget_blk error\n");
    }
    return (err);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_StrgDriveSearchAct
Description     : Storage drive search
Arguments       : USB_CLSINFO_t *mess   : Message
Return value    : none
******************************************************************************/
void usb_hmsc_StrgDriveSearchAct(USB_CLSINFO_t *mess)
{
    uint32_t    j, result;
    uint16_t    resultk;
/* Condition compilation by the difference of quality control */
 #ifdef USB_DEBUGPRINT_PP
    uint32_t    i;
    uint8_t     pdata[32];
 #endif /* USB_DEBUGPRINT_PP */
    uint16_t    offset, new_count, addr;
    USB_ER_t err,err2;
    USB_MH_t p_blf;
    USB_CLSINFO_t *cp;

    switch( usb_shmsc_StrgDriveSearchSeq )
    {
    case USB_SEQ_0:
        USB_PRINTF0("\n*** Drive search !\n");
        /* Unit number set */
        addr = mess -> keyword;
        usb_ghmsc_RootDevaddr = addr;

        usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        err = R_usb_hmsc_GetMaxUnit(mess, addr, (USB_CB_t)usb_hmsc_StrgCheckResult);
        if( err == USB_E_QOVR )
        {
            /* Resend message */
            err = USB_PGET_BLK(USB_HSTRG_MPL, &p_blf);
            if( err == USB_E_OK )
            {
                cp = (USB_CLSINFO_t*)p_blf;
                cp->msginfo = mess -> msginfo;
                cp->keyword = mess -> keyword;
                cp->result  = mess -> result;

                cp->ip      = mess->ip;
                cp->ipp     = mess->ipp;

                /* Send message */
                err = USB_SND_MSG(USB_HSTRG_MBX, (USB_MSG_t*)p_blf);
                if( err != USB_E_OK )
                {
                    USB_PRINTF1("### hmsc_StrgDriveSearch snd_msg error (%ld)\n", err);
                    err2 = USB_REL_BLK(USB_HSTRG_MPL, (USB_MH_t)p_blf);
                    if( err2 != USB_E_OK )
                    {
                        USB_PRINTF1("### hmsc_StrgDriveSearch rel_blk error (%ld)\n", err2);
                    }
                }
            }
            else
            {
                USB_PRINTF1("### hmsc_StrgDriveSearch pget_blk error (%ld)\n", err);
            }
        }
        else
        {
            usb_shmsc_StrgDriveSearchSeq++;
        }
        break;
    case USB_SEQ_1:
        addr = usb_ghmsc_RootDevaddr;
        /* Get MAX_LUN */
        usb_ghmsc_MaxLUN =  usb_hmsc_GetMaxUnitCheck(mess, mess->result);
        if( usb_ghmsc_MaxLUN == USB_ERROR )
        {
            usb_ghmsc_MaxLUN = (uint16_t)0;
            USB_PRINTF1("*** Unit information error, set unit number %d !\n", usb_ghmsc_MaxLUN);
        }
        else if( usb_ghmsc_MaxLUN > (uint32_t)USB_MAXUNITNUM )
        {
            USB_PRINTF2("*** Max Unit number(%d) is error, set unit number %d !\n", usb_ghmsc_MaxLUN, USB_MAXUNITNUM);
            usb_ghmsc_MaxLUN = USB_MAXUNITNUM - 1u;
        }
        else
        {
            USB_PRINTF1(" Unit number is %d\n", usb_ghmsc_MaxLUN);
        }

        /* Set pipe information */
        offset  = (uint16_t)(2u * USB_EPL*usb_ghmsc_StrgCount);
        usb_ghmsc_TmpEpTbl[offset + 3u] |= (uint16_t)(addr << USB_DEVADDRBIT);
        usb_ghmsc_TmpEpTbl[(offset + 3u) + USB_EPL] |= (uint16_t)(addr << USB_DEVADDRBIT);

        /* Check connection */
        USB_PRINTF0("\nPlease wait device ready\n");
        usb_cpu_DelayXms(100);
        /* Drive yes */
        usb_ghmsc_DriveChk[USB_MAXDRIVE][0] = USB_YES;
        /* Device address */
        usb_ghmsc_DriveChk[USB_MAXDRIVE][3] = addr;
        /* Device number */
        usb_ghmsc_DriveChk[USB_MAXDRIVE][4] = usb_ghmsc_StrgCount;

        usb_shmsc_StrgDriveSearchSeq++;
        usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        usb_hmsc_StrgSpecifiedPath((USB_CLSINFO_t *)mess);
        break;
    case USB_SEQ_2:
        addr = usb_ghmsc_RootDevaddr;
        /* Unit Number */
        usb_ghmsc_DriveChk[USB_MAXDRIVE][1] = (uint16_t)usb_shmsc_StrgDriveSearchCount;
        /* Inquiry */
        resultk = R_usb_hmsc_Inquiry(mess, USB_MAXDRIVE, (uint8_t*)&usb_ghmsc_Data);
        usb_shmsc_DeviceReady[usb_shmsc_StrgDriveSearchCount] = USB_PDT_UNKNOWN;
        
        usb_shmsc_StrgDriveSearchSeq++;
        usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        break;
    case USB_SEQ_3:
        addr = usb_ghmsc_RootDevaddr;
        resultk = mess -> result;
        if( resultk == USB_HMSC_OK )
        {
            usb_shmsc_DeviceReady[usb_shmsc_StrgDriveSearchCount] = usb_ghmsc_Data[0];
/* Condition compilation by the difference of quality control */
 #ifdef USB_DEBUGPRINT_PP
            /* Unit number */
            for( i = (uint32_t)0; i < (uint32_t)8; i++ )
            {
                pdata[i] = usb_ghmsc_Data[i + (uint32_t)8];
            }
            USB_PRINTF1("\n    Unit number %d .\n", usb_shmsc_StrgDriveSearchCount);
            pdata[8] = 0;
            USB_PRINTF1("    Vender Identification : %s\n", pdata);
            /* Product Identification */
            for( i = (uint32_t)0; i < (uint32_t)16; i++ )
            {
                pdata[i] = usb_ghmsc_Data[i + (uint32_t)16];
            }
            pdata[16] = 0;
            USB_PRINTF1("    Product Identification : %s\n", pdata);
 #endif /* USB_DEBUGPRINT_PP*/
            usb_shmsc_StrgDriveSearchErrCount = USB_SEQ_0;
            usb_shmsc_StrgDriveSearchCount++;
            usb_shmsc_StrgDriveSearchSeq = USB_SEQ_2;

            if( usb_shmsc_StrgDriveSearchCount > usb_ghmsc_MaxLUN )
            {
                usb_shmsc_StrgDriveSearchCount = USB_SEQ_0;
                usb_shmsc_StrgDriveSearchSeq = USB_SEQ_4;
            }

        }
        else if( resultk == USB_HMSC_CSW_ERR )
        {
            /* Inquiry error */
            USB_PRINTF1("### inquiry error ( %d times )\n", (usb_shmsc_StrgDriveSearchErrCount + 1));
            usb_shmsc_StrgDriveSearchErrCount++;
            usb_shmsc_StrgDriveSearchSeq = USB_SEQ_2;
            if( usb_shmsc_StrgDriveSearchErrCount >= 3 )
            {
                usb_shmsc_StrgDriveSearchErrCount = USB_SEQ_0;
                usb_shmsc_StrgDriveSearchCount++;
                if( usb_shmsc_StrgDriveSearchCount > usb_ghmsc_MaxLUN )
                {
                    usb_shmsc_StrgDriveSearchCount = USB_SEQ_0;
                    usb_shmsc_StrgDriveSearchSeq = USB_SEQ_4;
                }
            }
        }
        else
        {
            USB_PRINTF0("### inquiry error\n");

            usb_shmsc_StrgDriveSearchErrCount = USB_SEQ_0;
            usb_shmsc_StrgDriveSearchCount++;
            usb_shmsc_StrgDriveSearchSeq = USB_SEQ_2;

            if( usb_shmsc_StrgDriveSearchCount > usb_ghmsc_MaxLUN )
            {
                usb_shmsc_StrgDriveSearchCount = USB_SEQ_0;
                usb_shmsc_StrgDriveSearchSeq = USB_SEQ_4;
            }
        }
        usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        usb_hmsc_StrgSpecifiedPath((USB_CLSINFO_t *)mess);
        break;

    case USB_SEQ_4:
        /* Read Format Capacity */
        R_usb_hmsc_ReadFormatCapacity(mess, USB_MAXDRIVE
            , (uint8_t*)&usb_ghmsc_Data);
        usb_shmsc_StrgDriveSearchSeq++;
        usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        break;

    case USB_SEQ_5:
        /* Read Capacity */
        R_usb_hmsc_ReadCapacity(mess, USB_MAXDRIVE, (uint8_t*)&usb_ghmsc_Data);
        usb_shmsc_StrgDriveSearchSeq++;
        usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        break;

    case USB_SEQ_6:
        resultk = mess -> result;
        if( resultk != USB_HMSC_OK )
        {
            /* TestUnitReady */
            R_usb_hmsc_TestUnit(mess, USB_MAXDRIVE);
            usb_shmsc_StrgDriveSearchSeq++;
            usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        }
        else
        {
            /* Pass TestUnitReady  */
            usb_shmsc_StrgDriveSearchSeq= USB_SEQ_8;
            usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
            usb_hmsc_StrgSpecifiedPath((USB_CLSINFO_t *)mess);
        };
        break;

    case USB_SEQ_7:
        resultk = mess -> result;
        if( resultk != USB_HMSC_OK )
        {
            /* TestUnitReady */
            R_usb_hmsc_TestUnit(mess, USB_MAXDRIVE);
            usb_shmsc_StrgDriveSearchSeq == USB_SEQ_7;
            usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        }
        else
        {
            /* Read Capacity */
            R_usb_hmsc_ReadCapacity(mess, USB_MAXDRIVE, (uint8_t*)&usb_ghmsc_Data);
            usb_shmsc_StrgDriveSearchSeq++;
            usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        };
        break;

    case USB_SEQ_8:
        addr = usb_ghmsc_RootDevaddr;
        /* Read & set partition information */
        USB_PRINTF0("\nPartition information\n");
        result = 0;
        for( j = (uint32_t)0; j <= usb_ghmsc_MaxLUN; j++ )
        {
            /* Set sector size & block address */
            switch( usb_shmsc_DeviceReady[j] )
            {
            case USB_PDT_DIRECT:
                USB_PRINTF1("    Unit %d is direct access device.\n", j);
                /* Unit Number */
                usb_ghmsc_DriveChk[USB_MAXDRIVE][1] = (uint16_t)j;
                offset = usb_hmsc_SmpDevReadPartition(mess, (uint16_t)j, (uint32_t)512);
                result++;                                   
                break;
            case USB_PDT_SEQUENTIAL:
                /* Not support: Sequential device */
                USB_PRINTF1("### Unit %d sequential device.(not support)\n", j);
                break;
            case USB_PDT_WRITEONCE:
                /* Not support: Write once device */
                USB_PRINTF1("### Unit %d write once device.(not support)\n", j);
                break;
            case USB_PDT_CDROM:
                /* Not support: CD-ROM device */
                USB_PRINTF1("### Unit %d CD-ROM device.(not support)\n", j);
                break;
            case USB_PDT_OPTICAL:
                /* Not support: Optivasl device */
                USB_PRINTF1("### Unit %d optivasl device.(not support)\n", j);
                break;
            case USB_PDT_UNKNOWN:
                /* Not support: Unknown device */
                USB_PRINTF1("### Unit %d unknown device.(not support)\n", j);
                break;
            default:
                /* Not support: Not direct access device */
                USB_PRINTF1("### Unit %d is not direct access device.(not support)\n", j);
                break;
            }
        }
        usb_shmsc_StrgDriveSearchSeq++;
        if( result == 0 )
        {
            usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
            usb_hmsc_StrgSpecifiedPath((USB_CLSINFO_t *)mess);

        }   
        break;

    case USB_SEQ_9:
        addr = usb_ghmsc_RootDevaddr;
        new_count = USB_ERROR;
        for( j = (uint32_t)0; j <= usb_ghmsc_MaxLUN; j++ )
        {
            /* Set sector size & block address */
            if( usb_shmsc_DeviceReady[j] == USB_PDT_DIRECT )
            {
                new_count = mess->result;
            }
        }
        if( new_count == USB_DONE )
        {
            usb_ghmsc_StrgCount++;
        }
        usb_hmsc_StrgCommandResult((USB_CLSINFO_t *)mess);

        usb_shmsc_StrgDriveSearchSeq = USB_SEQ_0;
        usb_shmsc_StrgProcess = USB_NONE;
        break;
    default:
        usb_shmsc_StrgProcess = USB_NONE;
        usb_shmsc_StrgDriveSearchSeq = USB_SEQ_0;
        usb_shmsc_StrgDriveSearchCount = USB_SEQ_0;
        usb_shmsc_StrgDriveSearchErrCount = USB_SEQ_0;
        break;
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SmpDevReadSectorSize
Description     : Searches drive SndMsg
Arguments       : uint16_t size        : Size
Return value    : uint32_t             : Sector size
******************************************************************************/
uint32_t usb_hmsc_SmpDevReadSectorSize(USB_UTR_t *ptr, uint16_t size)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *cp;

    usb_shmsc_DevReadSectorSizeSeq = USB_SEQ_0;

    /* Get mem pool blk */
    if( USB_PGET_BLK(USB_HSTRG_MPL,&p_blf) == USB_OK )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = USB_MSG_HMSC_DEV_READ_SECTOR_SIZE;
        cp->keyword = size;

        cp->ip      = ptr->ip;
        cp->ipp     = ptr->ipp;

        /* Send message */
        err = USB_SND_MSG( USB_HSTRG_MBX, (USB_MSG_t*)p_blf );
        if( err != USB_E_OK )
        {
            err = USB_REL_BLK(USB_HSTRG_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### DevReadSectorSize function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### DevReadSectorSize function pget_blk error\n");
    }
    return(err);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SmpDevReadSectorSizeAct
Description     : Drive read sector size
Arguments       : USB_CLSINFO_t *mess  : Message
Return value    : none
******************************************************************************/
void usb_hmsc_SmpDevReadSectorSizeAct(USB_CLSINFO_t *mess)
{
    uint32_t    trans_byte = (uint32_t)0;
    uint16_t    side;

    side = mess -> keyword;
    switch( usb_shmsc_DevReadSectorSizeSeq )
    {
    case USB_SEQ_0:
        /* Read capacity */
        R_usb_hmsc_ReadCapacity(mess, side, (uint8_t*)&usb_ghmsc_Data);

        usb_shmsc_StrgProcess = USB_MSG_HMSC_DEV_READ_SECTOR_SIZE;
        usb_shmsc_DevReadSectorSizeSeq++;
        break;
    case USB_SEQ_1:
        usb_shmsc_DevReadSectorSizeSeq = USB_SEQ_0;
        if( mess -> result == USB_HMSC_OK )
        {
            trans_byte  =  (uint32_t)usb_ghmsc_Data[7];
            trans_byte  |= ((uint32_t)(usb_ghmsc_Data[6]) << 8);
            trans_byte  |= ((uint32_t)(usb_ghmsc_Data[5]) << 16);
            trans_byte  |= ((uint32_t)(usb_ghmsc_Data[4]) << 24);
            if( trans_byte != (uint32_t)0x0200 )
            {
                /* Sector size error */
                USB_PRINTF0("### Sector size error !\n");
                trans_byte = (uint32_t)0;
            }
            else
            {
                /* FSI Sector initialize */
                usb_hmsc_SmpFsiSectorInitialized(side, (uint32_t)0, (uint16_t)trans_byte);
            }
            usb_shmsc_DevReadSectorSizeErrCount = USB_SEQ_0;
            usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_OPEN;
        }
        else
        {
            /* Sector size error */
            USB_PRINTF0("*** Sector size error !\n");
            trans_byte =  (uint32_t)0x200;
            /* FSI sector initialize */
            usb_hmsc_SmpFsiSectorInitialized(side, (uint32_t)0, (uint16_t)trans_byte);
            usb_shmsc_StrgProcess = USB_MSG_HMSC_DEV_READ_SECTOR_SIZE;
            usb_shmsc_DevReadSectorSizeErrCount++;
            if( usb_shmsc_DevReadSectorSizeErrCount >= 3 )
            {
                usb_shmsc_DevReadSectorSizeErrCount = USB_SEQ_0;
                usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_OPEN;
            }
        }
        usb_ghmsc_TransSize = trans_byte;
        usb_hmsc_StrgSpecifiedPath((USB_CLSINFO_t *)mess);
        break;
    default:
        usb_shmsc_StrgProcess = USB_NONE;
        usb_shmsc_DevReadSectorSizeSeq = USB_SEQ_0;
        usb_shmsc_DevReadSectorSizeErrCount = USB_SEQ_0;
        break;
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SmpDevReadPartition
Description     : Searches drive SndMsg
Arguments       : uint16_t unit        : Unit
                : uint32_t trans_byte  : Trans byte
Return value    : uint16_t
******************************************************************************/
uint16_t usb_hmsc_SmpDevReadPartition(USB_UTR_t *ptr, uint16_t unit, uint32_t trans_byte)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *cp;

    usb_shmsc_DevReadPartitionSeq = USB_SEQ_0;

    /* Get mem pool blk */
    if( USB_PGET_BLK(USB_HSTRG_MPL, &p_blf) == USB_OK )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = USB_MSG_HMSC_DEV_READ_PARTITION;
        cp->keyword = unit;
        usb_ghmsc_TransSize = trans_byte;

        cp->ip      = ptr->ip;
        cp->ipp     = ptr->ipp;

        /* Send message */
        err = USB_SND_MSG( USB_HSTRG_MBX, (USB_MSG_t*)p_blf );
        if( err != USB_E_OK )
        {
            err = USB_REL_BLK(USB_HSTRG_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### DevReadSectorSize function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### DevReadSectorSize function pget_blk error\n");
    }   
    return (USB_DONE);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SmpDevReadPartitionAct
Description     : Drive read partition
Arguments       : USB_CLSINFO_t *mess : Message
Return value    : uint16_t            : [USB_DONE/USB_ERROR]
******************************************************************************/
uint16_t usb_hmsc_SmpDevReadPartitionAct(USB_CLSINFO_t *mess)
{
    uint32_t    i;
    uint16_t    result;
    uint16_t    new_drive, parcount = 0;
    uint16_t    unit;
    uint8_t     partition_info[USB_BOOTPARTNUM];
    uint32_t    partition_lba[USB_BOOTPARTNUM + 1u];
    uint32_t    trans_byte;

    trans_byte = usb_ghmsc_TransSize;
    new_drive = usb_shmsc_NewDrive;

    switch( usb_shmsc_DevReadPartitionSeq )
    {
    case USB_SEQ_0:
        if( usb_shmsc_LoopCont == USB_SEQ_0 )
        {
            usb_shmsc_Unit = mess->keyword;
            usb_ghmsc_PartTransSize = trans_byte;
        }
        else
        {
            trans_byte = usb_ghmsc_PartTransSize;
        }
        partition_lba[0]                = (uint32_t)0;
        partition_lba[USB_BOOTPARTNUM]  = (uint32_t)0;
    
        /* set drive number */
        new_drive = usb_hmsc_SmpDevNextDriveSearch();
        usb_shmsc_NewDrive = new_drive;

        /* Read10 */
        result = R_usb_hmsc_Read10(
                    mess, USB_MAXDRIVE, (uint8_t*)&usb_ghmsc_Data, partition_lba[0], (uint16_t)1, trans_byte);
        usb_shmsc_DevReadPartitionSeq++;
        usb_shmsc_StrgProcess = USB_MSG_HMSC_DEV_READ_PARTITION;
        break;
    case USB_SEQ_1:
        unit = usb_shmsc_Unit;
        usb_shmsc_DevReadPartitionSeq = USB_SEQ_0;

        if( mess -> result == USB_HMSC_OK )
        {
            /* Check boot record */
            result = usb_hmsc_SmpDevCheckBootRecord((uint8_t*)&usb_ghmsc_Data,
                        (uint32_t*)&partition_lba, (uint8_t*)&partition_info, (uint16_t)0 );
            /* Display partition information */
            if( result != (uint16_t)USB_BOOT_ERROR )
            {
                result = USB_DONE;

                for( i = (uint32_t)0; i < (uint32_t)USB_BOOTPARTNUM; i++ )
                {
                    switch( partition_info[i] )
                    {
                    case USB_PT_FAT12:
                    case USB_PT_FAT16:
                    case USB_PT_FAT32:
                        USB_PRINTF2("    Partition %d open. SIDE %d !\n", i, new_drive);
                        /* Drive yes */
                        usb_ghmsc_DriveChk[new_drive][0] = USB_YES;
                        /* Unit Number */
                        usb_ghmsc_DriveChk[new_drive][1] = unit;
                        /* Partition Number */
                        usb_ghmsc_DriveChk[new_drive][2] = parcount;
                        /* Device address */
                        usb_ghmsc_DriveChk[new_drive][3] = usb_ghmsc_DriveChk[USB_MAXDRIVE][3];
                        /* Endpoint table offset */
                        usb_ghmsc_DriveChk[new_drive][4] = usb_ghmsc_DriveChk[USB_MAXDRIVE][4];
                        usb_ghmsc_MaxDrive++;
                        if( usb_ghmsc_MaxDrive == USB_MAXDRIVE )
                        {
                            USB_PRINTF1("    Max drive over %d .\n", usb_ghmsc_MaxDrive);
                            i = (uint32_t)USB_BOOTPARTNUM;
                        }
                        else
                        {
                            /* Next drive search */
                            new_drive = usb_hmsc_SmpDevNextDriveSearch();
                            usb_shmsc_NewDrive = new_drive;
                        }
                        parcount++;
                        break;
                    case USB_PT_EPRT:
                        USB_PRINTF1("    Extended partition %d. !\n", i);
                        if( partition_lba[USB_BOOTPARTNUM] == (uint32_t)0 )
                        {
                            /* Master Boot */
                            partition_lba[USB_BOOTPARTNUM] = partition_lba[i];
                            /* Next EMBR sector */
                            partition_lba[0] = partition_lba[i];
                        }
                        else
                        {
                            /* Next EBMR sector */
                            partition_lba[0] = partition_lba[i] + partition_lba[USB_BOOTPARTNUM];
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
            else
            {
                /* Drive read error */
                USB_PRINTF2("### %d drive read error  ( %d times ).\n", new_drive, 0);
                result = USB_ERROR;
            }
        }
        else
        {
            /* Drive read error */
            USB_PRINTF2("### %d drive read error  ( %d times ).\n", new_drive, 0);
            usb_shmsc_LoopCont++;
            result = (uint16_t)USB_EMBR_ADDR;
            if( usb_shmsc_LoopCont == (uint32_t)10 )
            {
                result = USB_ERROR;
                usb_shmsc_LoopCont = USB_SEQ_0;
            }
        }

        if( result != (uint16_t)USB_EMBR_ADDR )
        {
            usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
            mess->result = result;
            usb_shmsc_LoopCont = USB_SEQ_0;
        }
        usb_hmsc_StrgSpecifiedPath((USB_CLSINFO_t *)mess);
        break;
    default:
        usb_shmsc_DevReadPartitionSeq = USB_SEQ_0;
        mess->result = USB_ERROR;
        usb_hmsc_StrgSpecifiedPath((USB_CLSINFO_t *)mess);
        break;
    }
    return 0;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_StrgDriveOpen
Description     : Mounts the drive
Arguments       : uint16_t side        : Side
Return value    : uint16_t             : Status
******************************************************************************/
USB_ER_t R_usb_hmsc_StrgDriveOpen(USB_UTR_t *ptr, uint16_t side )
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *cp;

    usb_shmsc_StrgDriveOpenSeq = USB_SEQ_0;

    /* Get mem pool blk */
    if( USB_PGET_BLK(USB_HSTRG_MPL, &p_blf) == USB_OK )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = USB_MSG_HMSC_STRG_DRIVE_OPEN;
        cp->keyword = side;

        cp->ip      = ptr->ip;
        cp->ipp     = ptr->ipp;

        /* Send message */
        err = USB_SND_MSG( USB_HSTRG_MBX, (USB_MSG_t*)p_blf );
        if( err != USB_E_OK )
        {
            USB_REL_BLK(USB_HSTRG_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### DevReadSectorSize function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### DevReadSectorSize function pget_blk error\n");
    }
    return err;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SmpStrgDriveOpenAct
Description     : Mounts the drive
Arguments       : USB_CLSINFO_t *mess  : Message
Return value    : none
******************************************************************************/
void usb_hmsc_SmpStrgDriveOpenAct( USB_CLSINFO_t *mess )
{
    uint16_t    result, unit, partition;
    uint32_t    trans_byte;
    uint16_t    side;
    uint16_t    endflg, retval;

    endflg = USB_OFF;
    retval = USB_DONE;
    switch( usb_shmsc_StrgDriveOpenSeq )
    {
    case USB_SEQ_0:
        side = mess -> keyword;
        /* Check drive numberk */
        if( side > usb_hmsc_SmpTotalDrive() )
        {
            USB_PRINTF1("### side over(StrgDriveOpen:side=%d)\n", side);
            endflg = USB_ON;
            retval = USB_ERROR;
        }
        /* Device Status */
        if( R_usb_hmsc_GetDevSts() != USB_HMSC_DEV_ATT )
        {
            USB_PRINTF1("### device det(StrgDriveOpen:side=%d)\n", side);
            endflg = USB_ON;
            retval = USB_ERROR;
        }
        unit        = usb_hmsc_SmpDrive2Unit(side);
        partition   = usb_hmsc_SmpDrive2Part(side);
        if( (unit == USB_ERROR) || (partition == USB_ERROR) )
        {
            USB_PRINTF2("### unit error(StrgDriveOpen:side=%d,unit=%d)\n", side, unit);
            endflg = USB_ON;
            retval = USB_ERROR;
        }
        USB_PRINTF1("\n*** drive %d open !\n", side);
        /* Set secor_size & block_address */
        usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_OPEN;
        usb_shmsc_StrgDriveOpenSeq++;
        usb_hmsc_SmpDevReadSectorSize(mess, side);
        
        usb_shmsc_PartitionLba[0]               = (uint32_t)0;
        usb_shmsc_PartitionLba[USB_BOOTPARTNUM] = (uint32_t)0;
        break;
    case USB_SEQ_1:
        side = mess -> keyword;
        trans_byte = usb_ghmsc_TransSize;

        /* Partition sector */
        usb_hmsc_SmpFsiSectorInitialized(side, usb_shmsc_PartitionLba[0], (uint16_t)trans_byte);
        usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_OPEN;
        usb_shmsc_StrgDriveOpenSeq++;

        /* Read10 */
        R_usb_hmsc_Read10(mess, side, (uint8_t*)&usb_ghmsc_Data, usb_shmsc_PartitionLba[0], (uint16_t)1, trans_byte);
        break;
    case USB_SEQ_2:
        side = mess -> keyword;

        /* Check boot record */
        result = usb_hmsc_SmpDevCheckBootRecord(
                    (uint8_t*)&usb_ghmsc_Data, (uint32_t*)&usb_shmsc_PartitionLba,
                    (uint8_t*)&usb_shmsc_PartitionInfo, (uint16_t)1);

        usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_OPEN;
        usb_shmsc_StrgDriveOpenSeq++;
        usb_hmsc_StrgSpecifiedPath((USB_CLSINFO_t *)mess);
        break;
    case USB_SEQ_3:
        side = mess -> keyword;
        result = mess -> result;
        partition   = usb_hmsc_SmpDrive2Part(side);
        trans_byte = usb_ghmsc_TransSize;
        
        /* Drive open */
        switch( usb_shmsc_PartitionInfo[usb_shmsc_StrgDriveOpenCount] )
        {
        case USB_PT_FAT12:
        case USB_PT_FAT16:
        case USB_PT_FAT32:
            if( partition == usb_shmsc_StrgDriveOpenParCount )
            {
                switch( result )
                {
                case USB_PBR_ADDR:
                    USB_PRINTF0("*** PBR !\n");
                    /* Offset sector read */
                    usb_shmsc_PartitionLba[0] = usb_hmsc_SmpFsiOffsetSectorRead(side);
                    /* Initial FFS */
                    result = usb_hmsc_SmpFsiFileSystemInitialized(
                                side, (uint8_t*)&usb_ghmsc_Data, usb_shmsc_PartitionLba[0]);
                    endflg = USB_ON;
                    retval = USB_DONE;
                    break;
                case USB_MBR_ADDR:
                case USB_EMBR_ADDR:
                    if( usb_shmsc_PartitionLba[usb_shmsc_StrgDriveOpenCount] == (uint16_t)0 )
                    {
                        /* Culc LBA from CHS */
                        USB_PRINTF0("### It is necessary to calculate CHS to LBA !\n");
                        endflg = USB_ON;
                        retval = USB_ERROR;
                    }
                    else
                    {
                        USB_PRINTF0("*** MBR !\n");
                        /* FSI offset sector read */
                        usb_shmsc_PartitionLba[usb_shmsc_StrgDriveOpenCount] += usb_hmsc_SmpFsiOffsetSectorRead(side);

                        usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_OPEN;
                        usb_shmsc_StrgDriveOpenSeq++;
                        /* Read10 */
                        R_usb_hmsc_Read10(mess, side, (uint8_t*)&usb_ghmsc_Data,
                            usb_shmsc_PartitionLba[usb_shmsc_StrgDriveOpenCount],(uint16_t)1, trans_byte);
                    }
                    break;
                default:
                    USB_PRINTF0("### Boot sector error !\n");
                    endflg = USB_ON;
                    retval = USB_ERROR;
                    break;
                }
            }
            else
            {
                usb_shmsc_StrgDriveOpenParCount++;
                usb_shmsc_StrgDriveOpenCount++;
                if( usb_shmsc_StrgDriveOpenCount >= USB_BOOTPARTNUM )
                {
                    usb_shmsc_StrgDriveOpenCount = USB_SEQ_0;
                    usb_shmsc_StrgDriveOpenSeq == USB_SEQ_1;
                }
                else
                {
                    usb_shmsc_StrgDriveOpenSeq == USB_SEQ_3;
                }
                usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_OPEN;
                usb_hmsc_StrgSpecifiedPath((USB_CLSINFO_t *)mess);
            }
            break;
        case USB_PT_EPRT:
            if( usb_shmsc_PartitionLba[USB_BOOTPARTNUM] == (uint16_t)0 )
            {
                /* Memory Master Boot */
                usb_shmsc_PartitionLba[USB_BOOTPARTNUM] = usb_shmsc_PartitionLba[usb_shmsc_StrgDriveOpenCount];
                /* Next EMBR sector */
                usb_shmsc_PartitionLba[0] = usb_shmsc_PartitionLba[usb_shmsc_StrgDriveOpenCount];
            }
            else
            {
                /* Next EBMR sector */
                usb_shmsc_PartitionLba[0] = usb_shmsc_PartitionLba[usb_shmsc_StrgDriveOpenCount] +
                                            usb_shmsc_PartitionLba[USB_BOOTPARTNUM];
            }
            usb_shmsc_StrgDriveOpenCount++;
            if( usb_shmsc_StrgDriveOpenCount >= USB_BOOTPARTNUM )
            {
                usb_shmsc_StrgDriveOpenCount = USB_SEQ_0;
                usb_shmsc_StrgDriveOpenSeq == USB_SEQ_1;
            }
            else
            {
                usb_shmsc_StrgDriveOpenSeq == USB_SEQ_3;
            }
            usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_OPEN;
            usb_hmsc_StrgSpecifiedPath((USB_CLSINFO_t *)mess);
            break;
        default:
            usb_shmsc_StrgDriveOpenCount++;
            if( usb_shmsc_StrgDriveOpenCount >= USB_BOOTPARTNUM )
            {
                usb_shmsc_StrgDriveOpenCount = USB_SEQ_0;
                usb_shmsc_StrgDriveOpenSeq == USB_SEQ_1;
            }
            else
            {
                usb_shmsc_StrgDriveOpenSeq == USB_SEQ_3;
            }
            usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_OPEN;
            usb_hmsc_StrgSpecifiedPath((USB_CLSINFO_t *)mess);
            break;
        }
        break;
    case USB_SEQ_4:
        side = mess -> keyword;
        /* Initial FFS */
        result  = usb_hmsc_SmpFsiFileSystemInitialized(side,
                    (uint8_t*)&usb_ghmsc_Data, usb_shmsc_PartitionLba[usb_shmsc_StrgDriveOpenCount]);
        endflg = USB_ON;
        retval = USB_DONE;
        break;
    default:
        usb_shmsc_StrgProcess = USB_NONE;
        usb_shmsc_StrgDriveOpenSeq = USB_SEQ_0;
        usb_shmsc_StrgDriveOpenCount = USB_SEQ_0;
        usb_shmsc_StrgDriveOpenParCount = USB_SEQ_0;
        usb_ghmsc_DriveOpen = USB_ERROR;
        break;
    }
    
    if( endflg == USB_ON )
    {
        usb_shmsc_StrgProcess = USB_NONE;
        usb_shmsc_StrgDriveOpenSeq = USB_SEQ_0;
        usb_shmsc_StrgDriveOpenCount = USB_SEQ_0;
        usb_shmsc_StrgDriveOpenParCount = USB_SEQ_0;
        usb_ghmsc_DriveOpen = retval;
        usb_hmsc_StrgCommandResult((USB_CLSINFO_t *)mess);
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_GetStringDescriptor1
Description     : Get String descriptor
Arguments       : uint16_t devaddr          : device address
                : uint16_t index            : descriptor index
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hmsc_GetStringDescriptor1(USB_UTR_t *ptr, uint16_t devaddr, uint16_t index, USB_CB_t complete)
{
    usb_hmsc_GetStringDesc(ptr, devaddr, (uint16_t)0, complete);

    return  USB_DONE;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_GetStringDescriptor2
Description     : Get String descriptor
Arguments       : uint16_t devaddr          : device address
                : uint16_t index            : descriptor index
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hmsc_GetStringDescriptor2(USB_UTR_t *ptr, uint16_t devaddr, uint16_t index, USB_CB_t complete)
{
    usb_hmsc_GetStringDesc(ptr, devaddr, index, complete);

    return  USB_DONE;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_StrgSpecifiedPath
Description     : Next Process Selector
Arguments       : USB_CLSINFO_t *mess  : Message
Return value    : none
******************************************************************************/
void usb_hmsc_StrgSpecifiedPath(USB_CLSINFO_t *mess)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *cp;

    /* Get mem pool blk */
    if( USB_PGET_BLK(USB_HSTRG_MPL,&p_blf) == USB_OK )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = usb_shmsc_StrgProcess;
        cp->keyword = mess->keyword;
        cp->result  = mess->result;

        cp->ip      = mess->ip;
        cp->ipp     = mess->ipp;

        /* Send message */
        err = USB_SND_MSG( USB_HSTRG_MBX, (USB_MSG_t*)p_blf );
        if( err != USB_E_OK )
        {
            err = USB_REL_BLK(USB_HSTRG_MPL,(USB_MH_t)p_blf);
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
Function Name   : usb_hmsc_StrgCheckResult
Description     : Hub class check result
Arguments       : USB_UTR_t *mess    : Message
Return value    : none
******************************************************************************/
void usb_hmsc_StrgCheckResult(USB_UTR_t *mess)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *cp;
    
    /* Get mem pool blk */
    if( USB_PGET_BLK(USB_HSTRG_MPL,&p_blf) == USB_OK )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = usb_shmsc_StrgProcess;
        cp->keyword = mess->keyword;
        cp->result      = mess->status;

        cp->ip      = mess->ip;
        cp->ipp     = mess->ipp;

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
Function Name   : usb_hmsc_StrgSpecifiedPath
Description     : Next Process Selector
Argument        : USB_CLSINFO_t *mess
Return          : none
******************************************************************************/
void usb_hmsc_StrgCommandResult(USB_CLSINFO_t *mess)
{
    USB_MH_t            p_blf;
    USB_ER_t            Err;
    USB_CLSINFO_t       *cp;

    /* Get mem pool blk */
    if( USB_PGET_BLK(USB_HMSCSMP_MPL,&p_blf) == USB_OK )  {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = usb_ghmsc_SmpAplProcess;
        cp->keyword = mess->keyword;
        cp->result      = mess->result;

        cp->ip      = mess->ip;
        cp->ipp     = mess->ipp;

        /* Send message */
        Err = USB_SND_MSG( USB_HMSCSMP_MBX, (USB_MSG_t*)p_blf );

        if(Err != USB_OK)
        {
            Err = USB_REL_BLK(USB_HMSCSMP_MPL,(USB_MH_t)p_blf);
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
Function Name   : usb_hmsc_GetStringDesc
Description     : Set GetDescriptor
Arguments       : uint16_t addr            : device address
                : uint16_t string          : descriptor index
                : USB_CB_t complete       : callback function
Return value    : uint16_t                 : error info
******************************************************************************/
uint16_t usb_hmsc_GetStringDesc(USB_UTR_t *ptr, uint16_t addr, uint16_t string
            , USB_CB_t complete)
{
    uint16_t        i;

    if( string == 0 )
    {
        usb_shmsc_ClassRequest[2] = (uint16_t)0x0000;
        usb_shmsc_ClassRequest[3] = (uint16_t)0x0004;
    }
    else
    {
        /* Set LanguageID */
        usb_shmsc_ClassRequest[2] = (uint16_t)(usb_ghmsc_ClassData[2]);
        usb_shmsc_ClassRequest[2] |= (uint16_t)((uint16_t)(usb_ghmsc_ClassData[3]) << 8);
        usb_shmsc_ClassRequest[3] = (uint16_t)USB_HMSC_CLSDATASIZE;
    }
    usb_shmsc_ClassRequest[0]   = USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_STANDARD | USB_DEVICE;
    usb_shmsc_ClassRequest[1]   = (uint16_t)(USB_STRING_DESCRIPTOR + string);
    usb_shmsc_ClassRequest[4]   = addr;

    for( i = 0; i < usb_shmsc_ClassRequest[3]; i++ )
    {
        usb_ghmsc_ClassData[i]  = (uint8_t)0xFF;
    }

    return usb_hmsc_CmdSubmit(ptr, complete);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_CmdSubmit
Description     : command submit
Arguments       : USB_CB_t complete         : callback info
Return value    : uint16_t                  : USB_DONE
******************************************************************************/
uint16_t usb_hmsc_CmdSubmit(USB_UTR_t *ptr, USB_CB_t complete)
{
    usb_shmsc_ClassControl.tranadr  = (void *)usb_ghmsc_ClassData;
    usb_shmsc_ClassControl.complete = complete;
    usb_shmsc_ClassControl.tranlen  = (uint32_t)usb_shmsc_ClassRequest[3];
    usb_shmsc_ClassControl.keyword  = USB_PIPE0;
    usb_shmsc_ClassControl.setup    = usb_shmsc_ClassRequest;
    usb_shmsc_ClassControl.segment  = USB_TRAN_END;

    usb_shmsc_ClassControl.ip   = ptr->ip;
    usb_shmsc_ClassControl.ipp  = ptr->ipp;

    R_usb_hstd_TransferStart(&usb_shmsc_ClassControl);
    
    return USB_DONE;
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_SmpDevCheckBootRecord
Description     : Device check boot record
Arguments       : uint8_t  *data      : Data
                : uint32_t *par_lba   : Par LBA
                : uint8_t  *par_info   : Par info
                : uint16_t flag       : Flag
Return value    : uint16_t            : Error code [USB_DONE/USB_ERROR]
******************************************************************************/
uint16_t usb_hmsc_SmpDevCheckBootRecord(uint8_t *data, uint32_t *par_lba, uint8_t *par_info, uint16_t flag)
{
    USB_MBR_t       *mbr_data;
    USB_PBR_t       *pbr_data;
    USB_FAT1216_t   *fat1216;
    USB_PTBL_t      *partition;
    uint16_t        fat_sector, i, embr_flag;
    uint32_t        total_sector32, dummy;

    mbr_data = (USB_MBR_t*)data;
    pbr_data = (USB_PBR_t*)data;

    /* BOOT Recorder ? */
    dummy   = (uint32_t)(pbr_data->Signature[0]);
    dummy   |= (uint32_t)((uint16_t)(pbr_data->Signature[1]) << 8);
    if( dummy != (uint32_t)USB_BOOTRECORD_SIG )
    {
        par_info[0] = USB_PT_NONE;
        USB_PRINTF1("    USB_BOOTRECORD_SIG error 0x%04x\n", dummy);
        return USB_BOOT_ERROR;
    }

    embr_flag = USB_PT_NONE;

    /* MBR check (Partition n) */
    for( i = 0; i < USB_BOOTPARTNUM; i++ )
    {
        partition   = (USB_PTBL_t*)&(mbr_data->PartitionTable[i * 16]);
        par_info[i] = USB_PT_NONE;
        par_lba[i]  = (uint32_t)(partition->StartSectorNum[0]);
        par_lba[i]  |= (uint32_t)(partition->StartSectorNum[1]) << 8;
        par_lba[i]  |= (uint32_t)(partition->StartSectorNum[2]) << 16;
        par_lba[i]  |= (uint32_t)(partition->StartSectorNum[3]) << 24;
        switch( partition->PartitionType )
        {
        case USB_PT_NONE:
            break;
        case USB_PT_EPRTA:
        case USB_PT_EPRTB:
            embr_flag   = USB_EMBR_ADDR;
            par_info[i] = USB_PT_EPRT;
            break;
        case USB_PT_FAT12A:
            if( embr_flag == USB_PT_NONE )
            {
                embr_flag = USB_MBR_ADDR;
            }
            par_info[i] = USB_PT_FAT12;
            break;
        case USB_PT_FAT16A:
        case USB_PT_FAT16B:
        case USB_PT_FAT16X:
            if( embr_flag == USB_PT_NONE )
            {
                embr_flag = USB_MBR_ADDR;
            }
            par_info[i] = USB_PT_FAT16;
            break;
        case USB_PT_FAT32A:
        case USB_PT_FAT32X:
            if( embr_flag == USB_PT_NONE )
            {
                embr_flag = USB_MBR_ADDR;
            }
            par_info[i] = USB_PT_FAT32;
            break;
        default:
            if( flag != 0 )
            {
                USB_PRINTF1("    Partition type not support 0x%02x\n", partition->PartitionType);
            }
            break;
        }
    }

    switch( embr_flag )
    {
    case USB_MBR_ADDR:
    case USB_EMBR_ADDR:
        return embr_flag;
        break;
    default:
        break;
    }

    /* PBR check */
    fat1216     = (USB_FAT1216_t*)&(pbr_data->FATSigData);

    fat_sector      =  (uint16_t)(pbr_data->FATSector[0]);
    fat_sector      |= (uint16_t)((uint16_t)(pbr_data->FATSector[1]) << 8);
    total_sector32  =  (uint32_t)(pbr_data->TotalSector1[0]);
    total_sector32  |= ((uint32_t)(pbr_data->TotalSector1[1]) << 8);
    total_sector32  |= ((uint32_t)(pbr_data->TotalSector1[2]) << 16);
    total_sector32  |= ((uint32_t)(pbr_data->TotalSector1[3]) << 24);

    if( ((pbr_data->JMPcode == USB_JMPCODE1) &&
        (pbr_data->NOPcode == USB_NOPCODE)) ||
        (pbr_data->JMPcode == USB_JMPCODE2) )
    {
        if( fat_sector == 0 )
        {
            if( total_sector32 != (uint32_t)0 )
            {
                par_info[0] = USB_PT_FAT32; /* FAT32 spec */
            }
        }
        else
        {
            if( (fat1216->FileSystemType[3] == 0x31) && (fat1216->FileSystemType[4] == 0x32) )
            {
                par_info[0] = USB_PT_FAT12; /* FAT12 spec */
            }
            else if( (fat1216->FileSystemType[3] == 0x31) && (fat1216->FileSystemType[4] == 0x36) )
            {
                par_info[0] = USB_PT_FAT16; /* FAT16 spec */
            }
            else
            {
            }
        }
    }

    if( par_info[0] == USB_PT_NONE )
    {
        USB_PRINTF0("    Partition error\n");
        return USB_BOOT_ERROR;
    }
    else
    {
        return USB_PBR_ADDR;
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SmpDevCheckBootRecord
Description     : Device check boot record
Arguments       : uint16_t addr       : Address
Return value    : none
******************************************************************************/
void usb_hmsc_SmpFsiDriveClear(USB_UTR_t *ptr, uint16_t addr)
{
    uint16_t        i, offset, msgnum;

    for( i = 0; i < USB_MAXDRIVE; i++ )
    {
        if( usb_ghmsc_DriveChk[i][3] == addr )
        {
            msgnum = usb_hmsc_SmpDrive2Msgnum(i);
            offset = (uint16_t)(2u * USB_EPL * msgnum);

#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
            usb_ghmsc_TmpEpTbl[offset + 0]  = USB_NONE;
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */

            usb_ghmsc_TmpEpTbl[offset + 1]
                = USB_NONE | USB_BFREOFF | USB_DBLBON | USB_CNTMDON | USB_NONE | USB_NONE | USB_NONE;
            usb_ghmsc_TmpEpTbl[offset + 3]  = USB_NONE;
            usb_ghmsc_TmpEpTbl[offset + 4]  = USB_NONE;
            offset += USB_EPL;

#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
            usb_ghmsc_TmpEpTbl[offset + 0]  = USB_NONE;
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */

            usb_ghmsc_TmpEpTbl[offset + 1]  
                = USB_NONE | USB_BFREOFF | USB_DBLBON | USB_CNTMDON | USB_NONE | USB_NONE | USB_NONE;
            usb_ghmsc_TmpEpTbl[offset + 3]  = USB_NONE;
            usb_ghmsc_TmpEpTbl[offset + 4]  = USB_NONE;

            usb_ghmsc_DriveChk[i][0] = USB_NO;  /* Yes/No */
            usb_ghmsc_DriveChk[i][1] = 0;       /* Unit Number */
            usb_ghmsc_DriveChk[i][2] = 0;       /* Partition Number */
            usb_ghmsc_DriveChk[i][3] = 0;       /* Device address */
            usb_ghmsc_DriveChk[i][4] = 0;       /* Device number */
            usb_ghmsc_MaxDrive--;
        }
    }

    if( usb_ghmsc_StrgCount != 0 )
    {
        usb_ghmsc_StrgCount--;
    }

    usb_ghmsc_OutPipe[msgnum][0]    = USB_NOPORT;   /* Pipe initial */
    usb_ghmsc_OutPipe[msgnum][1]    = 0;            /* Toggle clear */
    usb_ghmsc_InPipe[msgnum][0]     = USB_NOPORT;
    usb_ghmsc_InPipe[msgnum][1]     = 0;

    if( usb_ghmsc_StrgCount == 0 )
    {
        R_usb_hmsc_TaskClose(ptr);
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_StrgDriveClose
Description     : Releases drive
Arguments       : uint16_t side    : Side
Return value    : uint16_t         : [DONE/ERROR]
******************************************************************************/
uint16_t R_usb_hmsc_StrgDriveClose( uint16_t side )
{
    /* Check drive numberk */
    if( side > usb_hmsc_SmpTotalDrive() )
    {
        USB_PRINTF1("### side over(R_usb_hmsc_StrgDriveClose:side=%d)\n", side);
        return (USB_ERROR);
    }

    if( (USB_MEDIADRIVE != USB_DRIVE) && (side == USB_MEDIADRIVE) )
    {   /* Memory device */
        usb_hmsc_SmpFsiSectorInitialized(side, (uint32_t)0, (uint16_t)0);
        return (USB_DONE);
    }
    else
    {   /* USB device */
        /* Device Status */
        if( R_usb_hmsc_GetDevSts() != USB_HMSC_DEV_ATT )
        {
            USB_PRINTF1("### device det(R_usb_hmsc_StrgDriveClose:side=%d)\n", side);
            return (USB_ERROR);
        }
        usb_hmsc_SmpFsiSectorInitialized(side, (uint32_t)0, (uint16_t)0);
        return (USB_DONE);
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_StrgReadSector
Description     : Releases drive
Arguments       : uint16_t side        : Side
                : uint8_t  *buff       : Buffer address
                : uint32_t secno       : Sector number
                : uint16_t seccnt      : Sector count
                : uint32_t trans_byte  : Trans byte
Return value    : uint16_t             : [DONE/ERROR]
******************************************************************************/
uint16_t R_usb_hmsc_StrgReadSector(USB_UTR_t *ptr, uint16_t side, uint8_t *buff
    , uint32_t secno, uint16_t seccnt, uint32_t trans_byte )
{
    uint16_t    result;

    /* Check drive numberk */
    if( side > usb_hmsc_SmpTotalDrive() )
    {
        USB_PRINTF1("### side over(R_usb_hmsc_StrgReadSector:side=%d)\n", side);
        return (USB_ERROR);
    }

    /* Device Status */
    if( R_usb_hmsc_GetDevSts() != USB_HMSC_DEV_ATT )
    {
        USB_PRINTF1("### device det(R_usb_hmsc_StrgReadSector:side=%d)\n", side);
        return (USB_ERROR);
    }
    result = R_usb_hmsc_Read10(ptr, side, buff, secno, seccnt, trans_byte);

    return (result);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_StrgWriteSector
Description     : Writes sector information
Arguments       : uint16_t side        : Side
                : uint8_t  *buff       : Buffer address
                : uint32_t secno       : Sector number
                : uint16_t seccnt      : Sector count
                : uint32_t trans_byte  : Trans byte
Return value    : uint16_t             : [DONE/ERROR]
******************************************************************************/
uint16_t R_usb_hmsc_StrgWriteSector(USB_UTR_t *ptr, uint16_t side, uint8_t *buff
    , uint32_t secno, uint16_t seccnt, uint32_t trans_byte )
{
    uint16_t    result;

    /* Check drive numberk */
    if( side > usb_hmsc_SmpTotalDrive() )
    {
        USB_PRINTF1("### side over(R_usb_hmsc_StrgWriteSector:side=%d)\n", side);
        return (USB_ERROR);
    }

    /* Device Status */
    if( R_usb_hmsc_GetDevSts() != USB_HMSC_DEV_ATT )
    {
        USB_PRINTF1("### device det(R_usb_hmsc_StrgWriteSector:side=%d)\n", side);
        return (USB_ERROR);
    }
    result = R_usb_hmsc_Write10(ptr, side, buff, secno, seccnt, trans_byte);
    return(result);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_StrgUserCommand
Description     : USB Mass Storage Command
Arguments       : uint16_t side        : Side
                : uint16_t command     : Command
                : uint8_t  *buff       : Buffer address
Return value    : uint16_t             : [DONE/ERROR]
******************************************************************************/
uint16_t R_usb_hmsc_StrgUserCommand(USB_UTR_t *ptr, uint16_t side, uint16_t command
    , uint8_t *buff)
{
    uint16_t    result;

    /* Check drive number */
    if( side > usb_hmsc_SmpTotalDrive() )
    {
        USB_PRINTF1("### side over(StrgUserCommand:side=%d)\n", side);
        return USB_ERROR;
    }

    if( (USB_MEDIADRIVE != USB_DRIVE) && (side == USB_MEDIADRIVE) )
    {   /* Memory device */
        USB_PRINTF1("### not support command(StrgUserCommand:side=%d)\n", side);
        return (USB_ERROR);
    }
    else
    {   /* USB device */
        /* Device Status */
        if( R_usb_hmsc_GetDevSts() != USB_HMSC_DEV_ATT )
        {
            USB_PRINTF1("### device det(StrgUserCommand:side=%d)\n", side);
            return (USB_ERROR);
        }

        switch( command )
        {
        case USB_ATAPI_TEST_UNIT_READY:
            /* Test unit */
            result = R_usb_hmsc_TestUnit(ptr, side);
            break;
        case USB_ATAPI_REQUEST_SENSE:
            /*Request sense */
            result = R_usb_hmsc_RequestSense(ptr, side, buff);
            break;
        case USB_ATAPI_FORMAT_UNIT:
            return (USB_ERROR);
            break;
        case USB_ATAPI_INQUIRY:
            /* Inquiry */
            result = R_usb_hmsc_Inquiry(ptr, side, buff);
            break;
        case USB_ATAPI_MODE_SELECT6:
            /* Mode select6 */
            result = R_usb_hmsc_ModeSelect6(ptr, side, buff);
            break;
        case USB_ATAPI_MODE_SENSE6:
            return (USB_ERROR);
            break;
        case USB_ATAPI_START_STOP_UNIT:
            return (USB_ERROR);
            break;
        case USB_ATAPI_PREVENT_ALLOW:
            /* Prevent allow */
            result = R_usb_hmsc_PreventAllow(ptr, side, buff);
            break;
        case USB_ATAPI_READ_FORMAT_CAPACITY:
            /* Read format capacity */
            result = R_usb_hmsc_ReadFormatCapacity(ptr, side, buff);
            break;
        case USB_ATAPI_READ_CAPACITY:
            /* Read capacity */
            result = R_usb_hmsc_ReadCapacity(ptr, side, buff);
            break;
        case USB_ATAPI_READ10:
        case USB_ATAPI_WRITE10:
            return (USB_ERROR);
            break;
        case USB_ATAPI_SEEK:
        case USB_ATAPI_WRITE_AND_VERIFY:
        case USB_ATAPI_VERIFY10:
            return (USB_ERROR);
            break;
        case USB_ATAPI_MODE_SELECT10:
            return (USB_ERROR);
            break;
        case USB_ATAPI_MODE_SENSE10:
            /* Mode sense10 */
            result = R_usb_hmsc_ModeSense10(ptr, side, buff);
            break;
        default:
            return (USB_ERROR);
            break;
        }
        if( result == USB_HMSC_OK )
        {
            return (USB_DONE);
        }
        else
        {
            return (USB_ERROR);
        }
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SmpTotalDrive
Description     : Total drive information
Arguments       : none
Return value    : uint16_t             : Max drive
******************************************************************************/
uint16_t usb_hmsc_SmpTotalDrive(void)
{
    return usb_ghmsc_MaxDrive;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SmpDrive2Unit
Description     : Total drive information
Arguments       : uint16_t side        : Side
Return value    : uint16_t             : Unit number
******************************************************************************/
uint16_t usb_hmsc_SmpDrive2Unit(uint16_t side)
{
    if( usb_ghmsc_DriveChk[side][0] != USB_YES )
    {
        USB_PRINTF3("### Drive %d is not opened.   Unit=%d, Partition=%d !\n",
                side, usb_ghmsc_DriveChk[side][1], usb_ghmsc_DriveChk[side][2]);
        return USB_ERROR;
    }
    return (usb_ghmsc_DriveChk[side][1]);           /* Unit Number */
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SmpDrive2Part
Description     : Retrieves partition number
Arguments       : uint16_t side        : Side
Return value    : uint16_t             : Partition number
******************************************************************************/
uint16_t usb_hmsc_SmpDrive2Part(uint16_t side)
{
    if( usb_ghmsc_DriveChk[side][0] != USB_YES )
    {
        USB_PRINTF3("### Drive %d is not opened.   Unit=%d, Partition=%d !\n"
            , side, usb_ghmsc_DriveChk[side][1], usb_ghmsc_DriveChk[side][2]);
        return USB_ERROR;
    }
    return (usb_ghmsc_DriveChk[side][2]);   /* Parttition Number */
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SmpDrive2Addr
Description     : Retrieves device address
Arguments       : uint16_t side        : Side
Return value    : uint16_t             : Device address
******************************************************************************/
uint16_t usb_hmsc_SmpDrive2Addr(uint16_t side)
{
    return (usb_ghmsc_DriveChk[side][3]);   /* Device Address */
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SmpCheckAddr
Description     : Retrieves device address
Arguments       : uint16_t side        : Side
Return value    : uint16_t             : Device address
******************************************************************************/
uint16_t usb_hmsc_SmpCheckAddr(uint16_t side)
{
    if( usb_ghmsc_DriveChk[side][0] != USB_YES )
    {
        return USB_ERROR;
    }
    return (usb_ghmsc_DriveChk[side][3]);   /* Device Address */
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SmpDrive2Msgnum
Description     : Checks drive number
Arguments       : uint16_t side        : Side
Return value    : uint16_t             : Drive address
******************************************************************************/
uint16_t usb_hmsc_SmpDrive2Msgnum(uint16_t side)
{
    return (usb_ghmsc_DriveChk[side][4]);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_ClassWait
Description     : HMSC Class Wait
Arguments       : USB_UTR_t *mess : 
Return value    : none
******************************************************************************/
void usb_hmsc_ClassWait(USB_ID_t id, USB_UTR_t *mess)
{
    USB_ER_t        err;                    /* Error code */
    uint16_t        mode, tbl[10];

    R_usb_hstd_DeviceInformation(mess, 0, (uint16_t *)&tbl);

    if( mess->keyword == 0 )
    {
        mode = tbl[9];      /* PORT1 */
    }
    else
    {
        mode = tbl[8];      /* PORT0 */
    }
    if( mode != USB_DEFAULT )
    {
        mess->msginfo = USB_MSG_MGR_AORDETACH;
        err = USB_SND_MSG(USB_MGR_MBX, (USB_MSG_t*)mess);
        if( err != USB_E_OK )
        {
            USB_PRINTF0("### USB Strg enuwait snd_msg error\n");
        }
    }
    else
    {
        err = USB_SND_MSG(id, (USB_MSG_t*)mess);
        if( err != USB_E_OK )
        {
            USB_PRINTF0("### USB Strg enuwait snd_msg error\n");
        }
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_StdReqCheck
Description     : Sample Standard Request Check
Arguments       : uint16_t errcheck        : error
Return value    : uint16_t                 : error info
******************************************************************************/
uint16_t usb_hmsc_StdReqCheck(uint16_t errcheck)
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
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_GetStringDescriptor1Check
Description     : Get String descriptor Check
Arguments       : uint16_t errcheck         : errcheck
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hmsc_GetStringDescriptor1Check(USB_UTR_t *ptr, uint16_t errcheck)
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
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_GetStringDescriptor2Check
Description     : Get String descriptor Check
Arguments       : uint16_t errcheck         : errcheck
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hmsc_GetStringDescriptor2Check(USB_UTR_t *ptr, uint16_t errcheck)
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
End of function
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/
