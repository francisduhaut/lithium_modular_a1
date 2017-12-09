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
* File Name    : r_usb_pcontrolrw.c
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB Peripheral control transfer API code
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
*         : 3.30.2012          Function header translation. Ch. tabs to 4 spaces.
*                              SET EDITOR TO INSERT SPACES WHEN TAB PRESSED.
******************************************************************************/

/* $Id: r_usb_pcontrolrw.c 143 2012-05-07 09:16:46Z tmura $ */

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"         /* Type define */
#include "r_usb_usrconfig.h"        /* USB-H/W register set (user define) */
#include "r_usb_cdefusbip.h"        /* USB-FW Library Header */
#include "r_usb_cextern.h"          /* USB-FW global definition */
#include "r_usb_reg_access.h"
#include "r_usb_cusb_bitdefine.h"


/******************************************************************************
Section    <Section Definition> , "Project Sections"
******************************************************************************/
#pragma section _pcd

/******************************************************************************
Renesas Abstracted Peripheral Control RW API functions
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_ControlRead
Description     : Called by R_usb_pstd_ControlRead, see it for description.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
                : uint32_t bsize    : Read size in bytes.
                : uint8_t *table    : Start address of read data buffer.
Return value    : uint16_t          : USB_WRITESHRT/USB_WRITE_END/USB_WRITING/
                :                   : USB_FIFOERROR.
******************************************************************************/
uint16_t usb_pstd_ControlRead(USB_UTR_t *ptr, uint32_t bsize, uint8_t *table)
{
    uint16_t    end_flag;

    usb_gcstd_DataCnt[ptr->ip][USB_PIPE0] = bsize;
    usb_gcstd_DataPtr[ptr->ip][USB_PIPE0] = table;

    usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_CUSE, (uint16_t)USB_ISEL);
    
    /* Buffer clear */
    usb_creg_set_bclr( ptr, USB_CUSE );

    usb_creg_clr_sts_bemp( ptr, USB_PIPE0 );

    /* Peripheral Control sequence */
    end_flag = usb_cstd_write_data( ptr, USB_PIPE0, USB_CUSE );

    /* Peripheral control sequence */
    switch( end_flag )
    {
    /* End of data write */
    case USB_WRITESHRT:
        /* Enable not ready interrupt */
        usb_cstd_NrdyEnable(ptr, (uint16_t)USB_PIPE0);
        /* Set PID=BUF */
        usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
        break;
    /* End of data write (not null) */
    case USB_WRITEEND:
        /* Continue */
    /* Continue of data write */
    case USB_WRITING:
        /* Enable empty interrupt */
        usb_creg_set_bempenb(ptr, (uint16_t)USB_PIPE0);
        /* Enable not ready interrupt */
        usb_cstd_NrdyEnable(ptr, (uint16_t)USB_PIPE0);
        /* Set PID=BUF */
        usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
        break;
    /* FIFO access error */
    case USB_FIFOERROR:
        break;
    default:
        break;
    }
    /* End or error or continue */
    return (end_flag);
}
/******************************************************************************
End of function usb_pstd_ControlRead
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/
