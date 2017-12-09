/******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized.
*    This software is owned by Renesas Electronics Corporation and is protected
* under all applicable laws, including copyright laws.
*    THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING 
* BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
* PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. 
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE  LIABLE
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
* File Name    : r_usb_hdriverapi.c
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB Host Control Driver API. USB Host transfer level commands.
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
*         : 3.22.2012          Function header translation. Ch. tabs to 4 spaces.
*                              SET EDITOR TO INSERT SPACES WHEN TAB PRESSED.
******************************************************************************/

/* $Id: r_usb_hdriverapi.c 162 2012-05-21 10:20:32Z ssaek $ */

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"         /* Type define */
#include "r_usb_usrconfig.h"        /* USB-H/W register set (user define) */
#include "r_usb_ckernelid.h"        /* Kernel ID definition */
#include "r_usb_cdefusbip.h"        /* USB-FW Library Header */
#include "r_usb_cmacprint.h"        /* Standard IO macro */
#include "r_usb_cmacsystemcall.h"   /* System call macro */
#include "r_usb_cextern.h"          /* USB-FW global define */
#include "r_usb_reg_access.h"

/******************************************************************************
Section    <Section Definition> , "Project Sections"
******************************************************************************/
#pragma section _hcd

/******************************************************************************
External variables and functions
******************************************************************************/
extern void     R_usb_cstd_SetTaskPri(uint8_t tasknum, uint8_t pri);

/******************************************************************************
Renesas USB Host Driver API functions
******************************************************************************/

#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
/******************************************************************************
Static variables and functions
******************************************************************************/
static  uint16_t    usb_shstd_pipeCtrInfo[USB_NUM_USBIP];

/******************************************************************************
Function Name   : R_usb_hstd_allocatePipe
Description     : Get PIPE for H/W Pipe
Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel.
                : uint16_t  type : Transfer type; BULK/INT/ISO.
Return          : uint16_t       : Obtained pipe no.
******************************************************************************/
uint16_t    R_usb_hstd_allocatePipe( USB_UTR_t *ptr, uint16_t type )
{
    uint8_t pipeno, is_used;
    uint8_t start_pipe, end_pipe;

    switch( type )
    {
        case    USB_BULK:
            start_pipe  = 1;
            end_pipe    = USB_MAXPIPE_BULK;
            break;
        case    USB_INT:
            start_pipe  = USB_MAXPIPE_BULK + 1;
            end_pipe    = USB_MAXPIPE_NUM + 1;
            break;
        case    USB_ISO:
            start_pipe  = 1;
            end_pipe    = USB_MAXPIPE_ISO;
            break;
        default:
            break;
    }
    for( pipeno = start_pipe, is_used = USB_NO; (pipeno < end_pipe) && (USB_NO == is_used); pipeno++ )
    {
        if( (usb_shstd_pipeCtrInfo[ptr->ip] & (1 << pipeno)) == 0 )
        {
            is_used = USB_YES;
        }
    }
    if( USB_YES == is_used )
    {
        pipeno--;

        usb_shstd_pipeCtrInfo[ptr->ip] |= (1 << pipeno);

        return (uint16_t)(pipeno);
    }
    return USB_NG;
}/* eof R_usb_hstd_allocatePipe() */

/******************************************************************************
Function Name   : R_usb_hstd_freePipe
Description     : Free the PIPE for H/W Pipe
Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel.
                : uint16_t  pipeno  : H/W Pipe No. to be freed
Return          : void
******************************************************************************/
void    R_usb_hstd_freePipe( USB_UTR_t *ptr, uint8_t pipeno )
{
    usb_shstd_pipeCtrInfo[ptr->ip] &= ~(1 << pipeno);
}/* eof R_usb_hstd_freePipe() */

/******************************************************************************
Function Name   : R_usb_hstd_EnumGetDescriptor
Description     : Send "GetDescriptor" command to the connected USB Device.
Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel.
                : uint8_t   addr : The address of the USB Device to which 
                                   to send the "GetDescriptor".
                : uint8_t   cnt_value : Code for required Descriptor; device/
                                      : /configuration/etc...
                : USB_CB_t  complete  : Pointer to callback function to call 
                                      : when done.
Return          : USB_ER_t  error code: USB_E_OK etc
******************************************************************************/
USB_ER_t    R_usb_hstd_EnumGetDescriptor(USB_UTR_t *ptr, uint8_t addr, uint8_t cnt_value, USB_CB_t complete )
{
    USB_MH_t            p_blf;
    USB_ER_t            err, err2;
    USB_HCDINFO_t       *hp;

    /* Get mem pool blk */
    err = USB_PGET_BLK(USB_MGR_MPL, &p_blf);
    if( err == USB_E_OK )
    {
//      USB_PRINTF2("*** devaddr%d : msginfo=%d ***\n", devaddr, msginfo);
        hp  = (USB_HCDINFO_t*)p_blf;
        hp->msghead     = (USB_MH_t)USB_NULL;
        hp->msginfo     = USB_MSG_MGR_GETDESCRIPTOR;
        hp->keyword     = (uint16_t)addr << 8 | cnt_value;
        hp->complete    = complete;

        hp->ipp         = ptr->ipp;
        hp->ip          = ptr->ip;

        /* Send message */
        err = USB_SND_MSG(USB_MGR_MBX, (USB_MSG_t*)p_blf);
        if( err != USB_E_OK )
        {
            USB_PRINTF1("### MgrEnumSetConfiguration snd_msg error (%ld)\n", err);
            err2 = USB_REL_BLK(USB_MGR_MPL, (USB_MH_t)p_blf);
            if( err2 != USB_E_OK ) 
            {
                USB_PRINTF1("### MgrEnumSetConfiguration rel_blk error (%ld)\n", err2);
            }
        }
    }
    else
    {
        USB_PRINTF1("### hMgrChangeDeviceState pget_blk error (%ld)\n", err);
    }
    return err;
}/* eof R_usb_hstd_EnumGetDescriptor() */

/******************************************************************************
Function Name   : R_usb_hstd_MgrEnumSetConfiguration
Description     : Send "SetConfiguration" command to the connected USB Device.
Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel.
                : uint8_t   devqdr       : The address of the USB Device to which 
                                           to send the "SetConfigration".
                : uint8_t   config_value : Configuration no.
                : USB_CB_t  complete     : Pointer to the callback function.
Return          : USB_ER_t  error code   : USB_E_OK etc.
******************************************************************************/
USB_ER_t R_usb_hstd_MgrEnumSetConfiguration( USB_UTR_t *ptr, uint8_t devadr, uint8_t config_val, USB_CB_t complete )
{
    USB_MH_t            p_blf;
    USB_ER_t            err, err2;
    USB_HCDINFO_t       *hp;

    /* Get mem pool blk */
    err = USB_PGET_BLK(USB_MGR_MPL, &p_blf);
    if( err == USB_E_OK )
    {
//      USB_PRINTF2("*** devaddr%d : msginfo=%d ***\n", devaddr, msginfo);
        hp  = (USB_HCDINFO_t*)p_blf;
        hp->msghead     = (USB_MH_t)USB_NULL;
        hp->msginfo     = USB_MSG_MGR_SETCONFIGURATION;
        hp->keyword     = (uint16_t)devadr << 8 | config_val;
        hp->complete    = complete;

        hp->ipp         = ptr->ipp;
        hp->ip          = ptr->ip;

        /* Send message */
        err = USB_SND_MSG(USB_MGR_MBX, (USB_MSG_t*)p_blf);
        if( err != USB_E_OK )
        {
            USB_PRINTF1("### MgrEnumSetConfiguration snd_msg error (%ld)\n", err);
            err2 = USB_REL_BLK(USB_MGR_MPL, (USB_MH_t)p_blf);
            if( err2 != USB_E_OK ) 
            {
                USB_PRINTF1("### MgrEnumSetConfiguration rel_blk error (%ld)\n", err2);
            }
        }
    }
    else
    {
        USB_PRINTF1("### hMgrChangeDeviceState pget_blk error (%ld)\n", err);
    }
    return err;
}/* eof R_usb_hstd_MgrEnumSetConfiguration() */
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */

/******************************************************************************
Function Name   : R_usb_hstd_TransferStart
Description     : Request HCD to transfer data. HCD will transfer the data 
                  based on the transfer information stored in ptr.
Arguments       : USB_UTR_t *ptr        : USB system internal structure.
Return          : USB_ER_t  error code  : USB_E_OK/USB_E_QOVR/USB_E_ERROR.
******************************************************************************/
USB_ER_t R_usb_hstd_TransferStart(USB_UTR_t *ptr)
{
    USB_ER_t        err;

    err = usb_hstd_TransferStart( ptr );

    return err;
}/* eof R_usb_hstd_TransferStart() */

/******************************************************************************
Function Name   : R_usb_hstd_SetPipeRegistration
Description     : Set pipe configuration of USB H/W. Set the content of the 
                : specified pipe information table (2nd argument).
Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel.
                : uint16_t *table       : DEF_EP table pointer.
                : uint16_t pipe         : pipe number.
Return          : USB_ER_t error code   : USB_E_OK etc.
******************************************************************************/
USB_ER_t R_usb_hstd_SetPipeRegistration(USB_UTR_t *ptr, uint16_t *table, uint16_t pipe)
{
    usb_hstd_SetPipeRegister(ptr, pipe, table);

    return USB_E_OK;
}/* eof R_usb_hstd_SetPipeRegistration() */

/******************************************************************************
Function Name   : R_usb_hstd_TransferEnd
Description     : Request HCD to force termination of data transfer.
Arguments       : USB_UTR_t *ptr : USB system internal structure. 
                : uint16_t *table       : DEF_EP table pointer
                : uint16_t pipe         : pipe number
Return          : USB_ER_t error code   : USB_E_OK etc
******************************************************************************/
USB_ER_t R_usb_hstd_TransferEnd(USB_UTR_t *ptr, uint16_t pipe, uint16_t status)
{
    uint16_t        msg;
    USB_ER_t        err;

    if( usb_gcstd_Pipe[ptr->ip][pipe] == USB_NULL )
    {
        USB_PRINTF1("### R_usb_hstd_TransferEnd overlaps %d\n", pipe);
        return USB_E_QOVR;
    }

    if( status == USB_DATA_TMO )
    {
        msg = USB_MSG_HCD_TRANSEND1;
    }
    else
    {
        msg = USB_MSG_HCD_TRANSEND2;
    }

    err = usb_hstd_HcdSndMbx(ptr, msg, pipe, (uint16_t*)0, &usb_cstd_DummyFunction);
    return err;
}/* eof R_usb_hstd_TransferEnd() */

/******************************************************************************
Function Name   : R_usb_hstd_ChangeDeviceState
Description     : Change the state of the connected USB Device.
Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel.
                : USB_CB_INFO_t complete    : Callback function.
                : uint16_t msginfo          : Request type.
                : uint16_t member           : Rootport/pipe number.
Return          : USB_ER_t error code       : USB_E_OK etc.
******************************************************************************/
USB_ER_t R_usb_hstd_ChangeDeviceState(USB_UTR_t *ptr, USB_CB_t complete, uint16_t msginfo, uint16_t member)
{
    USB_ER_t        err;

    err = usb_hstd_ChangeDeviceState(ptr, complete, msginfo, member);

    return err;
}/* eof R_usb_hstd_ChangeDeviceState() */

/******************************************************************************
Function Name   : R_usb_hstd_MgrOpen
Description     : Initialize global variable that contains registration status 
                : of HDCD.
                : For RTOS version, start Manager(MGR) task
Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel.
Return          : none
******************************************************************************/
USB_ER_t R_usb_hstd_MgrOpen(USB_UTR_t *ptr)
{
    USB_ER_t        err=USB_DONE;
    USB_HCDREG_t    *driver;
    uint16_t        i;

    /* Manager Mode */
    usb_ghstd_MgrMode[ptr->ip][0]   = USB_DETACHED;
    usb_ghstd_MgrMode[ptr->ip][1]   = USB_DETACHED;
    usb_ghstd_DeviceSpeed[ptr->ip]  = USB_NOCONNECT;
    /* Device address */
    usb_ghstd_DeviceAddr[ptr->ip]   = USB_DEVICE_0;
    /* Device driver number */
    usb_ghstd_DeviceNum[ptr->ip]        = 0;
    for( i = USB_PIPE0; i <= USB_MAX_PIPE_NO; i++ )
    {
        usb_ghstd_SuspendPipe[i] = USB_DONE;
    }

    for( i = 0; i < (USB_MAXDEVADDR + 1u); i++ )
    {
        driver  = &usb_ghstd_DeviceDrv[ptr->ip][i];
        /* Root port */
        driver->rootport            = USB_NOPORT;
        /* Device address */
        driver->devaddr             = USB_NODEVICE;
        /* Device state */
        driver->devstate            = USB_DETACHED;
        /* Interface Class : NO class */
        driver->ifclass             = (uint16_t)USB_IFCLS_NOT;
        /* Root port */
        usb_ghstd_DeviceInfo[ptr->ip][i][0] = USB_NOPORT;
        /* Device state */
        usb_ghstd_DeviceInfo[ptr->ip][i][1] = USB_DETACHED;
        /* Not configured */
        usb_ghstd_DeviceInfo[ptr->ip][i][2] = (uint16_t)0;
        /* Interface Class : NO class */
        usb_ghstd_DeviceInfo[ptr->ip][i][3] = (uint16_t)USB_IFCLS_NOT;
        /* No connect */
        usb_ghstd_DeviceInfo[ptr->ip][i][4] = (uint16_t)USB_NOCONNECT;
    }

    USB_PRINTF0("*** Install USB-MGR ***\n");

    R_usb_cstd_SetTaskPri(USB_MGR_TSK,  USB_PRI_2);

    return err;
}/* eof R_usb_hstd_MgrOpen() */

/******************************************************************************
Function Name   : R_usb_hstd_MgrClose
Description     : For RTOS version, stop Manager(MGR) task.
Argument        : none
Return          : none
******************************************************************************/
USB_ER_t R_usb_hstd_MgrClose(void)
{
    USB_ER_t        err=USB_DONE;

    USB_PRINTF0("*** Release USB-MGR ***\n");

    return err;
}/* eof R_usb_hstd_MgrClose() */

/******************************************************************************
Function Name   : R_usb_hstd_DriverRegistration
Description     : The HDCD information registered in the class driver structure 
                : is registered in HCD.
Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel.
                : USB_HCDREG_t *callback    : class driver structure
Return          : none
******************************************************************************/
void R_usb_hstd_DriverRegistration(USB_UTR_t *ptr, USB_HCDREG_t *callback)
{
    USB_HCDREG_t    *driver;

    if( usb_ghstd_DeviceNum[ptr->ip] <= USB_MAXDEVADDR )
    {
        driver = &usb_ghstd_DeviceDrv[ptr->ip][usb_ghstd_DeviceNum[ptr->ip]];
        /* Root port */
        driver->rootport    = USB_NOPORT;
        /* Device address */
        driver->devaddr     = USB_NODEVICE;
        /* Device state */
        driver->devstate    = USB_DETACHED;
        /* Interface Class */
        driver->ifclass     = callback->ifclass;
        /* Target peripheral list */
        driver->tpl         = callback->tpl;
        /* Pipe Define Table address */
        driver->pipetbl     = callback->pipetbl;
        /* Driver init */
        driver->classinit   = callback->classinit;
        /* Driver check */
        driver->classcheck  = callback->classcheck;
        /* Device configured */
        driver->devconfig   = callback->devconfig;
        /* Device detach */
        driver->devdetach   = callback->devdetach;
        /* Device suspend */
        driver->devsuspend  = callback->devsuspend;
        /* Device resume */
        driver->devresume   = callback->devresume;
        /* Device over current */
        driver->overcurrent = callback->overcurrent;
        /* Initialized device driver */
        (*driver->classinit)(ptr, (uint16_t)USB_NO_ARG, (uint16_t)USB_NO_ARG);  

        usb_ghstd_DeviceNum[ptr->ip]++;
        USB_PRINTF1("*** Registration driver 0x%02x\n",driver->ifclass);
    }
    else
    {
        USB_PRINTF0("### Registration device driver over\n");
    }
}/* eof R_usb_hstd_DriverRegistration() */

/******************************************************************************
Function Name   : R_usb_hstd_DriverRelease
Description     : Release the Device Class Driver.
Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel.
                : uint8_t   devclass    : Interface class
Return          : none
******************************************************************************/
void R_usb_hstd_DriverRelease(USB_UTR_t *ptr, uint8_t devclass)
{
    USB_HCDREG_t    *driver;
    uint16_t        i, flg;

    if( devclass == USB_IFCLS_NOT )
    {
        /* Device driver number */
        usb_ghstd_DeviceNum[ptr->ip] = 0;
    }
    else
    {
        for( flg = 0u, i = 0u
            ; (i < (USB_MAXDEVADDR + 1u)) && (flg == 0u); i++ )
        {
            driver = &usb_ghstd_DeviceDrv[ptr->ip][i];
            if( driver->ifclass == devclass )
            {
                /* Root port */
                driver->rootport    = USB_NOPORT;
                /* Device address */
                driver->devaddr     = USB_NODEVICE;
                /* Device state */
                driver->devstate    = USB_DETACHED;
                /* Interface Class : NO class */
                driver->ifclass     = (uint16_t)USB_IFCLS_NOT;

                usb_ghstd_DeviceNum[ptr->ip]--;
                USB_PRINTF1("*** Release class %d driver ***\n",devclass);
                flg = 1u;           /* break; */
            }
        }
    }
}/* eof R_usb_hstd_DriverRelease() */

/******************************************************************************
Function Name   : R_usb_hstd_ChkPipeInfo
Description     : Analyze descriptor information of the connected USB Device, 
                  and reflect it in the pipe information table.
Argument        : uint16_t speed            : device speed
                : uint16_t *ep_tbl          : DEF_EP table pointer
                : uint8_t  *descriptor      : Endpoint Descriptor table
Return          : uint16_t                  : DIR_H_IN / DIR_H_OUT / USB_ERROR
******************************************************************************/
uint16_t R_usb_hstd_ChkPipeInfo(uint16_t speed, uint16_t *ep_tbl, uint8_t *descriptor)
{
    uint16_t        pipe_cfg, pipe_maxp, pipe_peri;
    uint16_t        retval, work1, work2;

    /* Check Endpoint descriptor */
    /* Descriptor Type */
    if( descriptor[1] == USB_DT_ENDPOINT )
    {
        switch( (uint16_t)(descriptor[3] & USB_EP_TRNSMASK) )
        {
        /* Control Endpoint */
        case USB_EP_CNTRL:
            USB_PRINTF0("###Control pipe is not support.\n");
            return USB_ERROR;
            break;
        /* Isochronous Endpoint */
        case USB_EP_ISO:
            if( (ep_tbl[0] != USB_PIPE1) && (ep_tbl[0] != USB_PIPE2) ) 
            {
                USB_PRINTF0("###Iso pipe is 1 or 2.\n");
                return USB_ERROR;
            }
            USB_PRINTF0(" ISOCH ");
            pipe_cfg = USB_ISO;
            break;
        /* Bulk Endpoint */
        case USB_EP_BULK:
            if( (ep_tbl[0] < USB_PIPE1) || (ep_tbl[0] > USB_PIPE5) )
            {
                USB_PRINTF0("###Bulk pipe is 1 to 5.\n");
                return USB_ERROR;
            }
            /* USB_PRINTF0(" BULK "); */
            pipe_cfg = USB_BULK;
            break;
        /* Interrupt Endpoint */
        case USB_EP_INT:
            if( (ep_tbl[0] < USB_PIPE6) || (ep_tbl[0] > USB_MAX_PIPE_NO) )
            {
                USB_PRINTF0("###Int pipe is 6 to 9.\n");
                return USB_ERROR;
            }
            /* USB_PRINTF0(" INT "); */
            pipe_cfg = USB_INT;
            break;
        default:
            break;
        }

        /* Set pipe configuration table */
        if( (descriptor[2] & USB_EP_DIRMASK) == USB_EP_IN )
        {
            /* IN(receive) */

            if( (descriptor[3] & USB_EP_TRNSMASK) != USB_EP_ISO )
            {
                /* Compulsory SHTNAK */
                pipe_cfg |= USB_SHTNAKON | USB_DIR_H_IN;
            }
            else
            {
                pipe_cfg |= USB_DIR_H_IN;
            }
            switch( ep_tbl[5] )
            {
            case USB_CUSE:
                /* continue */
            case USB_D0USE:
                /* continue */
            case USB_D1USE:
                pipe_cfg |= (uint16_t)(ep_tbl[1] & (USB_DBLBFIELD|USB_CNTMDFIELD));
                break;
            case USB_D0DMA:
                /* continue */
            case USB_D1DMA:
                pipe_cfg |= (uint16_t)((uint16_t)(ep_tbl[1] & (USB_DBLBFIELD|USB_CNTMDFIELD)) | USB_BFREON);
                break;
            default:
                break;
            }
            retval = USB_DIR_H_IN;
        }
        else 
        {
            /* OUT(send) */
            pipe_cfg |= (uint16_t)((uint16_t)(ep_tbl[1] & (USB_DBLBFIELD|USB_CNTMDFIELD)) | USB_DIR_H_OUT);
            retval  = USB_DIR_H_OUT;
        }

        /* Endpoint number set */
        pipe_cfg |= (uint16_t)(descriptor[2] & USB_EP_NUMMASK);

        /* Max packet size set */
        pipe_maxp   = (uint16_t)(descriptor[4] | (uint16_t)((uint16_t)descriptor[5] << 8u));
        /* Buffer flash */
        pipe_peri   = (uint16_t)(ep_tbl[4] & (~USB_IITVFIELD));
        if( descriptor[6] != 0 )
        {
            /* FS/LS interrupt */
            if( ((uint16_t)(pipe_cfg & USB_TYPFIELD) == USB_INT) && (speed != USB_HSCONNECT) )
            {
                work1 = descriptor[6];
                work2 = 0;
                for(  ; work1 != 0; work2++ )
                {
                    work1 = (uint16_t)(work1 >> 1);
                }
                if( work2 != 0 )
                {
                    /* Interval time */
                    pipe_peri |= (uint16_t)(work2 - 1);
                }
            }
            else
            {
                if( descriptor[6] <= 8 )
                {
                    /* Interval time */
                    pipe_peri |= (uint16_t)((descriptor[6] - 1u) & (USB_IITVFIELD));
                }
                else
                {
                    /* Interval time */
                    pipe_peri |= (uint16_t)(0x00FFu) & (USB_IITVFIELD);
                }
            }
        }
        ep_tbl[1] = pipe_cfg;
        ep_tbl[3] = pipe_maxp;
        ep_tbl[4] = pipe_peri;
        /* USB_PRINTF4(" pipe%d configuration %04X %04X %04X\n", ep_tbl[0], ep_tbl[1], ep_tbl[3], ep_tbl[4]); */
    }
    else
    {
        USB_PRINTF0("###Endpoint Descriptor error.\n");
        return USB_ERROR;
    }
    return (retval);
}/* eof R_usb_hstd_ChkPipeInfo */

/******************************************************************************
Function Name   : R_usb_hstd_SetPipeInfo
Description     : Copy information of pipe information table from source 
                  (2nd argument) to destination (1st argument)
Argument        : uint16_t *ep_tbl          : DEF_EP table pointer(destination)
                : uint16_t *tmp_tbl         : DEF_EP table pointer(source)
                : uint16_t length           : Table length
Return          : none
******************************************************************************/
void R_usb_hstd_SetPipeInfo(uint16_t *ep_tbl, uint16_t *tmp_tbl, uint16_t length)
{
    uint16_t    i;

//USB_PRINTF3("!!! %lx %lx %d\n", ep_tbl, TmpTbl, length);

    for( i = 0; i < length; i++ )
    {
        ep_tbl[i] = tmp_tbl[i];
    }
}
/* eofR_usb_hstd_SetPipeInfo
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_hstd_DeviceInformation
Description     : Get the status of the connected USB Device
Arguments       : USB_UTR_t *ptr : USB system internal structure. 
                : uint16_t devaddr     : Device address
                : uint16_t *tbl        : Table Pointer
Return          : None
******************************************************************************/
void R_usb_hstd_DeviceInformation(USB_UTR_t *ptr, uint16_t devaddr, uint16_t *tbl)
{
    uint16_t    i, port, *p;

    if( devaddr == 0 )
    {
        for( p = tbl, i = 0; i < 8; ++i )
        {
            *p++ = USB_NOPORT;
        }

        port = usb_ghstd_DeviceInfo[ptr->ip][devaddr][0];
        if( port != USB_NOPORT )
        {
            tbl[0] = port;
            tbl[1] = usb_ghstd_MgrMode[ptr->ip][port];
            tbl[4] = usb_ghstd_DeviceInfo[ptr->ip][devaddr][4];
        }
        else
        {
            tbl[0] = port;
            tbl[1] = usb_ghstd_MgrMode[ptr->ip][0];
            tbl[4] = usb_ghstd_DeviceInfo[ptr->ip][devaddr][4];
        }
    }
    else
    {
        for( i = 0; i < 8; ++i )
        {
            tbl[i] = usb_ghstd_DeviceInfo[ptr->ip][devaddr][i];
        }
    }
    tbl[8] = usb_ghstd_MgrMode[ptr->ip][0];
    tbl[9] = usb_ghstd_MgrMode[ptr->ip][1];
}
/*
 eofR_usb_hstd_DeviceInformation
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_hstd_ReturnEnuMGR
Description     : Continuous enumeration is requested to MGR task (API for nonOS)
Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel.
                : uint16_t cls_result   : class check result
Return          : none
******************************************************************************/
void R_usb_hstd_ReturnEnuMGR(USB_UTR_t *ptr, uint16_t cls_result)
{
    usb_ghstd_CheckEnuResult[ptr->ip] = cls_result;
    usb_hstd_MgrSndMbx(ptr, USB_MSG_MGR_SUBMITRESULT, USB_PIPE0, USB_CTRL_END);
}
/*
 eofR_usb_hstd_ReturnEnuMGR
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_hstd_EnuWait
Description     : Request to change enumeration priority(API for nonOS)
Argument        : USB_UTR_t *ptr        : Pointer to a structure that includes
                                          the USB IP No. setting to change 
                : uint16_t taskID       : Enumeration wait Task ID
Return          : none
******************************************************************************/
void R_usb_hstd_EnuWait(USB_UTR_t *ptr, uint8_t taskID)
{
    usb_ghstd_EnuWait[ptr->ip] = taskID;
}
/*
 eofR_usb_hstd_EnuWait
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_hstd_MgrChangeDeviceState
Description     : Request to change the status of the connected USB Device
Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel.
                : USB_CB_INFO_t complete     : callback function pointer
                : uint16_t msginfo           : Message Info
                : uint16_t devaddr           : Device address
Return          : USB_ER_t                   : USB_E_OK etc
******************************************************************************/
USB_ER_t R_usb_hstd_MgrChangeDeviceState(USB_UTR_t *ptr, USB_CB_t complete, uint16_t msginfo, uint16_t devaddr)
{
    USB_MH_t            p_blf;
    USB_ER_t            err, err2;
    USB_HCDINFO_t       *hp;

    /* Get mem pool blk */
    err = USB_PGET_BLK(USB_MGR_MPL, &p_blf);
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
        err = USB_SND_MSG(USB_MGR_MBX, (USB_MSG_t*)p_blf);
        if( err != USB_E_OK )
        {
            USB_PRINTF1("### hMgrChangeDeviceState snd_msg error (%ld)\n", err);
            err2 = USB_REL_BLK(USB_MGR_MPL, (USB_MH_t)p_blf);
            if( err2 != USB_E_OK ) 
            {
                USB_PRINTF1("### hMgrChangeDeviceState rel_blk error (%ld)\n", err2);
            }
        }
    }
    else
    {
        USB_PRINTF1("### hMgrChangeDeviceState pget_blk error (%ld)\n", err);
    }
    return err;
}
/* eof R_usb_hstd_MgrChangeDeviceState() */

/******************************************************************************
Function Name   : R_usb_hstd_HcdOpen
Description     : Start HCD(Host Control Driver) task
Argument        : USB_UTR_t *ptr      : Pointer to a structure that includes USB IP No. setting 
Return          : USB_ER_t            : USB_E_OK etc
******************************************************************************/
USB_ER_t R_usb_hstd_HcdOpen(USB_UTR_t *ptr)
{
    uint16_t    i, j;
    USB_ER_t    err=USB_DONE;

    if( USB_MAXDEVADDR < USB_DEVICEADDR )
    {
        USB_PRINTF0("Device address error\n");
        /* >yes no process */
        return USB_ERROR;
    }

    /* Global Init */
    /* Control transfer stage management */
    usb_ghstd_Ctsq[ptr->ip] = USB_IDLEST;

    usb_ghstd_RemortPort[0] = USB_DEFAULT;
    usb_ghstd_RemortPort[1] = USB_DEFAULT;

    for( j = 0; i < USB_NUM_USBIP; ++j )
    {
        for( i = USB_PIPE0; i <= USB_MAX_PIPE_NO; i++ )
        {
            usb_gcstd_Pipe[j][i] = (USB_UTR_t*)USB_NULL;
        }
    }

    USB_PRINTF0("*** Install USB-HCD ***\n");

    R_usb_cstd_SetTaskPri(USB_HCD_TSK,  USB_PRI_1);

    return err;
}/* eof R_usb_hstd_HcdOpen */

/******************************************************************************
Function Name   : R_usb_hstd_HcdClose
Description     : Stop HCD(Host Control Driver) task.
Argument        : none
Return          : USB_ER_t                   : USB_E_OK etc
******************************************************************************/
USB_ER_t R_usb_hstd_HcdClose(void)
{
    USB_ER_t        err=USB_DONE;

    USB_PRINTF0("*** Release USB-HCD ***\n");

    return err;
}/* eof R_usb_hstd_HcdClose() */

/******************************************************************************
Function Name   : R_usb_hstd_HcdTask
Description     : Call HCD(Host Control Driver) task (API for nonOS)
Argument        : none
Return          : none
******************************************************************************/
void R_usb_hstd_HcdTask(USB_VP_INT_t stcd)
{
    usb_hstd_HcdTask(stcd);
}/* eof R_usb_hstd_HcdTask() */

/******************************************************************************
Function Name   : R_usb_hstd_MgrTask
Description     : Call MGR(Manager) task (API for nonOS)
Argument        : none
Return          : none
******************************************************************************/
void R_usb_hstd_MgrTask(USB_VP_INT_t stcd)
{
    usb_hstd_MgrTask(stcd);
}/* eof R_usb_hstd_MgrTask() */

/******************************************************************************
Function Name   : R_usb_hstd_usbdriver_start
Description     : Host USB Low Level Driver Task Start
Argument        : none
Return          : none
******************************************************************************/
void R_usb_hstd_usbdriver_start( USB_UTR_t *ptr )
{
    /* Task Start Processing */
    R_usb_hstd_MgrOpen(ptr);            /* Manager open */
    R_usb_hstd_HcdOpen(ptr);            /* Hcd open */
}
/******************************************************************************
End of function R_usb_hstd_usbdriver_start()
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/
