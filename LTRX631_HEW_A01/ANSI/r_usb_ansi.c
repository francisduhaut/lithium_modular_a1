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
* File Name    : r_usb_ansi.c
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : ANSI IO for USB
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
*         : 4.25.2012          Function header translation. Ch. tabs to 4 spaces.
*                              SET EDITOR TO INSERT SPACES WHEN TAB PRESSED.
******************************************************************************/

/* $Id: r_usb_ansi.c 143 2012-05-07 09:16:46Z tmura $ */

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"         /* Type define */
#include "r_usb_cdefusbip.h"        /* USB-FW Library Header */
#include "r_usb_ansi.h"
#include "r_usb_control.h"
#include "r_usb_sysdef.h"
/******************************************************************************
Section    <Section Definition> , "Project Sections"
******************************************************************************/
#pragma section _ansi

/******************************************************************************
External variables and functions
******************************************************************************/
/* USB API (Host) */
extern USB_ER_t R_usb_hstd_TransferEnd(USB_UTR_t *ptr, uint16_t pipe, uint16_t status);
extern USB_ER_t R_usb_hstd_MgrChangeDeviceState(USB_UTR_t *ptr, USB_CB_INFO_t complete, uint16_t msginfo,
    uint16_t devaddr);
extern void     R_usb_hstd_DeviceInformation(USB_UTR_t *ptr, uint16_t addr, uint16_t *tbl);

/* USB API (Peripheral) */
extern USB_ER_t R_usb_pstd_TransferEnd(USB_UTR_t *ptr, uint16_t pipe, uint16_t status);
extern USB_ER_t R_usb_pstd_PcdChangeDeviceState(USB_UTR_t *ptr, uint16_t state, uint16_t port_no,
    USB_CB_INFO_t complete);
extern void     R_usb_pstd_DeviceInformation(USB_UTR_t *ptr, uint16_t *tbl);
extern void     R_usb_pstd_SetPipeStall(USB_UTR_t *ptr, uint16_t pipeno);
extern USB_ER_t R_usb_pstd_SetStall(USB_UTR_t *ptr, USB_CB_INFO_t complete, uint16_t pipe);

#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
/******************************************************************************
Private global variables and functions
******************************************************************************/
struct  usb_fninfo  usb_fnInfo[16];

/* Pointer Table for open() function to handle the class */
int16_t (*usb_classopen[])(USB_UTR_t *, struct usb_fninfo *, uint8_t, uint8_t) = 
{                                           
    usb_open_hcdc,                  /* for Host CDC(Communications Class) */
    usb_open_hcdc,                  /* for Host CDC(Data Class) */
    usb_open_hhid,                  /* for Host HID */
    usb_open_hmsc,                  /* for Host MSC */
    usb_open_hvendor,               /* for Host Vendor class(Bulk Pipe) */
    usb_open_hvendor,               /* for Host Vendor class(Interrupt Pipe) */
    usb_open_pcdc,                  /* for Peripheral CDC(Communications Class) */
    usb_open_pcdc,                  /* for Peripheral CDC(Data Class) */
    usb_open_phid,                  /* for Peripheral HID */
    usb_open_pmsc,                  /* for Peripheral MSC */
    usb_open_pvendor,               /* for Peripheral Vendor class(Bulk Pipe) */
    usb_open_pvendor                /* for Peripheral Vendor class(Interrupt Pipe) */
};

/******************************************************************************
Static variables and functions
******************************************************************************/
static  uint16_t    usb_FileNumberInfo = 0; /* Variable for FileNumber management */

#define USB_NOPIPE  ((uint8_t)0xFF)

/******************************************************************************
Function Name   : usb_GetFileNumber
Description     : Search usb_FileNumberInfo for a vacant number for management.
                : Return the file number. The index is later used for storing 
                : the return value of the open function.
Argument        : NONE
Return          : FileNumber / -1(NG)
******************************************************************************/
static  uint16_t    usb_GetFileNumber( void )
{
    uint16_t    cnt, is_found;

    /* Loop to search the Empty File Number */
    for( cnt = 0, is_found = USB_NO; (cnt < USB_MAX_FILENUMBER) && (is_found == USB_NO); cnt++ )
    {
        /* Found the Empty File Number */
        if( (usb_FileNumberInfo & (1 << cnt)) == 0 )
        {
            is_found = USB_YES;
        }
    }

    return (cnt - 1);
}
/******************************************************************************
End of function usb_GetFileNumber
******************************************************************************/

/******************************************************************************
Function Name   : usb_SetFileNumber
Description     : For management to update usb_FileNumberInfo with the specified
                : one in use.
Argument        : uint16_t fn :  FileNumber
Return          : none
******************************************************************************/
static  void    usb_SetFileNumber( uint16_t fn )
{
    /* Setting the File Number under using */
    usb_FileNumberInfo |= (1 << fn);
}
/******************************************************************************
End of function usb_SetFileNumber
******************************************************************************/

/******************************************************************************
Function Name   : usb_ReleaseFileNumber
Description     : Release specified file number.
Argument        : uint16_t   fn    : File number.
Return          : void
******************************************************************************/
static  void    usb_ReleaseFileNumber( uint16_t fn )
{
    /* Release the File Number */
    usb_FileNumberInfo &= ~(1 << fn);
}
/******************************************************************************
End of function usb_ReleaseFileNumber
******************************************************************************/

/******************************************************************************
Function Name   : usb_open
Description     : Based on the specified class code, call the corresponding 
                : class' open function.
Argument        : USB_CLASS_ENUM_t    class   : Class code
Return          : FileNumber / -1(NG)
******************************************************************************/
int16_t usb_open( USB_CLASS_ENUM_t class )
{
    uint16_t    err;
    uint16_t    fn;

    err = USB_NG;

    /* Getting the empty File Number */
    fn  = usb_GetFileNumber();

    /* Setting the File Number under using */
    usb_SetFileNumber( fn );

    /* Calling open() function that correspond to each mode after checking Host/Peripheral mode */
    if( class & USB_CLASS_PERI_MODE )
    {
#if USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP
        /* Calling open() function for Peripheral mode */
        err = usb_open_peri( class, fn );
#endif
    }
    else
    {
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
        /* Calling open() function for Host mode */
        err = usb_open_host( class, fn );
#endif
    }

    /* Checking the return from open() */
    if( USB_OK == err )
    {
//      usb_SetFileNumber( fn );
        return fn;
    }
    else
    {
        /* Release the File Number */
        usb_ReleaseFileNumber( fn );
    }
    return (int16_t)-1;
}
/******************************************************************************
End of function usb_open
******************************************************************************/

/******************************************************************************
Function Name   : usb_close
Description     : Release specified file number which was taken when open was 
                : called.
Argument        : uint16_t   fn        : FileNumber
Return          : 0(OK) / -1(NG)
******************************************************************************/
int16_t usb_close( int16_t fn )
{
    uint16_t err;

    /* Checking whether close() function is registered in usb_funInfo[](structure) variable or not */
    if( USB_NULL != usb_fnInfo[fn].close_func )
    {
        /* Calling close() function for the class */
        err = (usb_fnInfo[fn].close_func)( fn );

        /* Checking the return from close() function for the class */
        if( USB_NG == err )
        {
            return (int16_t)-1;
        }

        /* Setting the File Number under using */
        usb_ReleaseFileNumber( fn );
        return (int16_t)0;
    }
    else
    {
        /* The processing of close() for the class is NG because close() for the class is not registered */
        return (int16_t)-1;
    }
}
/******************************************************************************
End of function usb_close
******************************************************************************/


/******************************************************************************
Function Name   : usb_read
Description     : Receive data for corresponding class.
Argument        : int16_t    fn        : File number
                : void       *buf      : Start address of buffer to store data.
                : uint32_t   length    : Read data length.
Return          : none
******************************************************************************/
int32_t usb_read( int16_t fn, void *buf, uint32_t   length )
{
    int32_t read_len = -1;

    /* Checking whether the class that correspond to the File Number is opened or not */
    if( USB_NO == usb_fnInfo[fn].isOpened )
    {
        /* Return -1(NG) if the class is not opened */
        return -1;
    }

    /*  Checking whether read() function that correspond to the FileNumber is registered 
        in usb_fnInfo[](structure) variable or not */
    if( USB_NULL != usb_fnInfo[fn].class_read )
    {
        /* Calling read() function for the class */
        read_len = usb_fnInfo[fn].class_read( fn, buf, length );
    }
    return read_len;
}
/******************************************************************************
End of function usb_read
******************************************************************************/

/******************************************************************************
Function Name   : usb_write
Description     : Send data for corresponding class.
Argument        : uint16_t   fn        : File number
                : void       *buf      : Top address of buffer to store data.
                : uint32_t   length    : Write data length.
Return          : none
******************************************************************************/
int32_t usb_write( int16_t fn, void *buf, uint32_t  length )
{
    int32_t     write_len = -1;

    /* Checking whether the class that correspond to the File Number is opened or not */
    if( USB_NO == usb_fnInfo[fn].isOpened )
    {
        /* Return -1(NG) if the class is not opened */
        return (int16_t)-1;
    }

    /*  Checking whether write() function that correspond to the FileNumber is registered 
        in usb_fnInfo[](structure) variable or not */
    if( USB_NULL != usb_fnInfo[fn].class_write )
    {
        /* Calling write() function for the class */
        write_len = usb_fnInfo[fn].class_write( fn, buf, length );
    }
    return write_len;
}
/******************************************************************************
End of function usb_write
******************************************************************************/

/******************************************************************************
Function Name   : usb_control
Description     : Process specified (2nd argument) CTRL code.
Argument        : uint16_t   fn        : File number.
                : CTRLCODE   code      : Cotrol code.
                : void       *data     : Pointer of data.
Return          : int16_t   OK(0)/NG(-1)
******************************************************************************/
int16_t usb_control( int16_t fn, USB_CTRLCODE_t code, void *data )
{
    int16_t                 ret_code = 0;
    USB_UTR_t               utr;
    USB_UTR_t               *ptr;
    USB_CTL_PARAMETER_t     *parameter;
    USB_FNINFO_t            *p_fn_info;

    /* Setting USB IP number and USB IP base address that correspond to the FileNumber */
    p_fn_info = &usb_fnInfo[fn];
    ptr = (USB_UTR_t*)&utr;
    utr.ip = p_fn_info->usb_ip;
    utr.ipp = p_fn_info->usb_ipp;

    /* Convert the type of the input parameter for control */
    parameter = (USB_CTL_PARAMETER_t*)data;

    /* Checking the control code */
    switch( code )
    {
        case    USB_CTL_USBIP_NUM:
            /* Store the number of USB IP */
            *(uint16_t *)data = p_fn_info->usb_ip;
        break;

        case    USB_CTL_RD_NOTIFY_SET:
            /* Setting the call-back function is called when read() function completes */
            p_fn_info->read_complete = (USB_CB_t)data;
        break;

        case    USB_CTL_WR_NOTIFY_SET:
            /* Setting the call-back function is called when write() function completes */
            p_fn_info->write_complete = (USB_CB_t)data;
        break;

        case    USB_CTL_RD_LENGTH_GET:
            /* Getting the received data length by read() function */
            *(int32_t*)data = p_fn_info->read_length;
        break;

        case    USB_CTL_WR_LENGTH_GET:
            /* Getting the sent data length by write() function */
            *(int32_t*)data = p_fn_info->write_length;
        break;

        case    USB_CTL_GET_OPEN_STATE:
            /* Getting the open state */
            *(int16_t*)data = p_fn_info->open_state;
        break;

        case    USB_CTL_GET_RD_STATE:
            /* Getting the read state */
            *(int16_t*)data = p_fn_info->read_state;
        break;

        case    USB_CTL_GET_WR_STATE:
            /* Getting the write state */
            *(int16_t*)data = p_fn_info->write_state;
        break;

#ifdef  USB_HOST_MODE_PP
        case    USB_CTL_H_RD_TRANSFER_END:
            /* Forcibly termination processing for read() function */
            ret_code = R_usb_hstd_TransferEnd(ptr, p_fn_info->inPipeNo,
                parameter->transfer_end.status);
        break;

        case    USB_CTL_H_WR_TRANSFER_END:
            /* Forcibly termination processing for write() function */
            ret_code = R_usb_hstd_TransferEnd(ptr, p_fn_info->outPipeNo,
                parameter->transfer_end.status);
        break;

        case    USB_CTL_H_CHG_DEVICE_STATE:
            /* Changing the USB device state */
            ret_code = R_usb_hstd_MgrChangeDeviceState(ptr, parameter->device_state.complete,
                parameter->device_state.msginfo, parameter->device_state.devaddr);

        break;

        case    USB_CTL_H_GET_DEVICE_INFO:
            /* Getting the USB device information */
            R_usb_hstd_DeviceInformation(ptr, usb_devInfo[p_fn_info->devno].devadr,
                parameter->device_information.tbl);
        break;
#endif  /* USB_HOST_MODE_PP */


#ifdef  USB_PERI_MODE_PP
        case    USB_CTL_P_RD_TRANSFER_END:
            /* Forcibly termination processing for read() function */
            ret_code = R_usb_pstd_TransferEnd(ptr, p_fn_info->outPipeNo,
                parameter->transfer_end.status);
        break;

        case    USB_CTL_P_WR_TRANSFER_END:
            /* Forcibly termination processing for write() function */
            ret_code = R_usb_pstd_TransferEnd(ptr, p_fn_info->inPipeNo,
                parameter->transfer_end.status);
        break;

        case    USB_CTL_P_CHG_DEVICE_STATE:
            /* Changing the USB device state */
            ret_code = R_usb_pstd_PcdChangeDeviceState(ptr, parameter->device_state.msginfo,
                (uint16_t)0, parameter->device_state.complete);
        break;

        case    USB_CTL_P_GET_DEVICE_INFO:
            /* Getting the USB device information */
            R_usb_pstd_DeviceInformation(ptr, parameter->device_information.tbl);
        break;

        case    USB_CTL_P_RD_SET_STALL:
            /* Setting the pipe stall for read() processing */
            if( parameter->setstall.complete == NULL )
            {
                R_usb_pstd_SetPipeStall(ptr, p_fn_info->outPipeNo);
            }
            else
            {
                R_usb_pstd_SetStall(ptr, parameter->setstall.complete, p_fn_info->outPipeNo);
            }
        break;

        case    USB_CTL_P_WR_SET_STALL:
            /* Setting the pipe stall for write() processing */
            if( parameter->setstall.complete == NULL )
            {
                /* when the call-back function is not set */
                R_usb_pstd_SetPipeStall(ptr, p_fn_info->inPipeNo);
            }
            else
            {
                /* when the call-back function is set */
                R_usb_pstd_SetStall(ptr, parameter->setstall.complete, p_fn_info->inPipeNo);
            }
        break;
#endif  /* USB_PERI_MODE_PP */

        case    USB_CTL_CLASS_REQUEST:
            ret_code = usb_control_class_request(data);
        break;

        default:
            /* Setting the error(-1) because the control code does not exist */
            ret_code = -1;                  /* Cotrol Code error */
        break;
    }
    return ret_code;
}
/******************************************************************************
End of function usb_control
******************************************************************************/
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */

/******************************************************************************
End  Of File
******************************************************************************/
