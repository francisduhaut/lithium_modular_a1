/*******************************************************************************
* DISCLAIMER
*
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. 
*
* No other uses are authorized.
*
* This software is owned by Renesas Electronics Corp. and is protected under
* all applicable laws, including copyright laws.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
*
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE  LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************
* File Name    : r_usb_creg_dmadtc.c
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : Setting code of DMA/DTC
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
*         : 1.3.2012           Comments tranlation (REA). Changed tabs to 4 spa-
*                              ces. SET EDITOR TO INSERT SPACES WHEN TAB PRESSED.
******************************************************************************/

/* $Id: r_usb_creg_dmadtc.c 143 2012-05-07 09:16:46Z tmura $ */

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"     /* Type define */
#include "r_usb_usrconfig.h"
#include "r_usb_cdefusbip.h"        /* USB-FW Library Header */
#include "r_usb_cextern.h"          /* USB-FW global definition */
#include "iodefine.h"
#include "r_usb_reg_access.h"
#include "r_usb_cusb_bitdefine.h"

extern void     usb_cpu_d0fifo_enable_dma(USB_UTR_t *ptr );
extern void     usb_cpu_d0fifo_disable_dma(USB_UTR_t *ptr );

/******************************************************************************
Section    <Section Definition> , "Project Sections"
******************************************************************************/
#pragma section _usblib

/******************************************************************************
Function Name   : usb_cstd_Buf2D0fifoStartUsb
Description     : Setup to start DMA/DTC transfer from data buffer to D0FIFO.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void usb_cstd_Buf2D0fifoStartUsb(USB_UTR_t *ptr)
{
    uint16_t    mbw;

    /* Write (MEMORY -> FIFO) : USB register set */
    if( (usb_gcstd_Dma0Size[ptr->ip] & (uint16_t)0x0001) != 0 )
    {
        mbw = USB_MBW_8;
    }
    else
    {
        mbw = USB_MBW_16;
    }
    /* Change MBW setting */
    usb_creg_set_mbw( ptr, USB_D0DMA, mbw );

    /* DTC(D0FIFO) interrupt enable */
    usb_cpu_d0fifo_enable_dma(ptr);

    /* Set DREQ enable */
    usb_creg_set_dreqe( ptr, USB_D0DMA );
}/* eof usb_cstd_Buf2D0fifoStartUsb() */

/******************************************************************************
Function Name   : usb_cstd_D0fifo2BufStartUsb
Description     : Setup to start DMA/DTC transfer D0FIFO to buffer.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void usb_cstd_D0fifo2BufStartUsb(USB_UTR_t *ptr)
{
    /* Read (FIFO -> MEMORY) : USB register set */
    /* DMA buffer clear mode & MBW set */
    usb_creg_set_mbw( ptr, USB_D0DMA, (uint16_t)(USB_MBW_16) );
    usb_creg_clr_dclrm( ptr, USB_D0DMA );

    /* Set DREQ enable */
    usb_creg_set_dreqe( ptr, USB_D0DMA );
}/* eof usb_cstd_D0fifo2BufStartUsb */

/******************************************************************************
Function Name   : usb_cstd_D0fifoStopUsb
Description     : Setup external variables used for USB data transfer; to reg-
                : ister if you want to stop the transfer of DMA/DTC.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void usb_cstd_D0fifoStopUsb(USB_UTR_t *ptr)
{
    uint16_t    ip = ptr->ip;
    uint16_t    pipe = usb_gcstd_Dma0Pipe[ip];
    uint32_t    transfer_size = usb_gcstd_Dma0Size[ip];
    uint32_t    *request_size = &usb_gcstd_DataCnt[ip][pipe];
    uint8_t     *tran_data_ptr = (uint8_t *)&usb_gcstd_DataPtr[ip][pipe];

    usb_creg_clr_dreqe( ptr, USB_D0DMA );

    /* Direction check */
    if( usb_gcstd_Dma0Dir[ip] == USB_BUF2FIFO )
    {
        /* Buffer to FIFO */
        if( *request_size < transfer_size )
        {
            /* >yes then set BVAL */
            *tran_data_ptr += *request_size;
            *request_size = (uint32_t)0u;
            /* Disable Ready Interrupt */
            usb_creg_clr_brdyenb(ptr, pipe);
            /* Set BVAL */
            usb_creg_set_bval( ptr, USB_D0DMA );
        }
        else
        {
            *tran_data_ptr += transfer_size;
            /* Set data count to remain */
            *request_size -= transfer_size;
        }
    }
    else
    {
        /* FIFO to Buffer */
        *tran_data_ptr += transfer_size;
        /* Set data count to remain */
        if( *request_size < transfer_size )
        {
            *request_size = transfer_size - *request_size;
        }
        else
        {
            *request_size -= transfer_size;
        }
    }
}/* eof usb_cstd_D0fifoStopUsb() */

/******************************************************************************
Function Name   : usb_cstd_D0fifoInt
Description     : Set end of DMA/DTC transfer. Set to restart DMA/DTC trans-
                : fer according to data size of remaining functions to be pro-
                : cessed.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void usb_cstd_D0fifoInt(USB_UTR_t *ptr)
{
    uint16_t pipe;
    uint16_t ip;
    uint16_t maxps;
    uint32_t *transfer_count;

    ip = ptr->ip;
    pipe = usb_gcstd_Dma0Pipe[ip];
    maxps = usb_gcstd_Dma0Fifo[ip];
    transfer_count = &usb_gcstd_DataCnt[ip][pipe];

    /* DMA Transfer Request Clear */
    usb_creg_clr_dreqe( ptr, USB_D0DMA );

    /* Transfer count > MAXPS */
    if ( *transfer_count > maxps )
    {
        /* DMA Transfer count update */
        *transfer_count %= maxps;
        /* Transfer continue check */
        if( *transfer_count != 0 )
        {
            /* Transfer count != MAXPS * N */
            /* Odd size data check  */
            if( (*transfer_count & 0x0001u) != 0u )
            {
                /* if count == odd */
                usb_creg_set_mbw( ptr, USB_D0DMA, USB_MBW_8 );
            }
            /* DMA Transfer size update */
            usb_gcstd_Dma0Size[ip] = *transfer_count;
            /* DMA Restart */
            usb_cpu_d0fifo_restart_dma(ptr);
            /* DMA Transfer Request Set */
            usb_creg_set_dreqe( ptr, USB_D0DMA );
        }
    }
    else if( *transfer_count == 0 )
    {
        /* More than enough Interrupt */
        return;
    }
    else
    {
        /* Transfer count < MAXPS */
        usb_creg_set_bval( ptr, USB_D0DMA );
        /* Transfer complete */
        *transfer_count = 0;
    }

    /* Transfer complete check */
    if( *transfer_count == 0 )
    {
        /* Transfer complete */
        usb_cpu_d0fifo_stop_dma(ptr);
        /* Empty Status Clear */
//      usb_creg_clr_sts_bemp( ptr, pipe );
        /* Enable Empty Interrupt */
        usb_creg_set_bempenb(ptr, pipe);
    }

}/* eof usb_cstd_D0fifoInt() */

/******************************************************************************
Function Name   : usb_cstd_SetDma
Description     : Stop DMA. If transfer direction is buffer to fifo then enable 
                : BEMP Interrupt.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void usb_cstd_SetDma(USB_UTR_t *ptr)
{
    /* Disable DREQ */
    usb_creg_clr_dreqe( ptr, USB_D0DMA );
    /* DMA stop */
    usb_cpu_d0fifo_stop_dma(ptr);

    /* Check direction */
    if( usb_gcstd_Dma0Dir[ptr->ip] == USB_BUF2FIFO )
    {
        usb_creg_clr_sts_bemp( ptr, usb_gcstd_Dma0Pipe[ptr->ip] );

        /* Enable Empty Interrupt */
        usb_creg_set_bempenb(ptr, usb_gcstd_Dma0Pipe[ptr->ip]);
        if( (usb_gcstd_Dma0Size[ptr->ip] % usb_gcstd_Dma0Fifo[ptr->ip]) != 0 )
        {
            /* DataSize % MXPS */
            /* Set BVAL */
            usb_creg_set_bval( ptr, USB_D0DMA );
        }
    }
    else
    {
        usb_gcstd_DataCnt[ptr->ip][usb_gcstd_Dma0Pipe[ptr->ip]] = 0;
    }
}/* eof usb_cstd_SetDma */

/******************************************************************************
Function Name   : usb_cstd_Buf2fifoStartDma
Description     : Start transfer using DMA/DTC. If transfer size is 0, write 
                : more data to buffer.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t pipe   : Pipe nr.
                : uint16_t useport: FIFO select
Return value    : none
******************************************************************************/
void    usb_cstd_Buf2fifoStartDma( USB_UTR_t *ptr, uint16_t pipe, uint16_t useport )
{
    /* Transfer size check */
    if( usb_gcstd_Dma0Size[ptr->ip] != 0 )
    {
        /* 16bit access */
        if ( (usb_gcstd_Dma0Size[ptr->ip] & 0x0001u) == 0u )
        {
            /* DMA access Buffer to FIFO start */
            usb_cpu_buf2d0fifo_start_dma(ptr, usb_cstd_GetD0fifo16Adr(ptr));
        }
        else
        {
            /* 8bit access */
            /* DMA access Buffer to FIFO start */
            usb_cpu_buf2d0fifo_start_dma(ptr, usb_cstd_GetD0fifo8Adr(ptr));
        }
        /* Changes the FIFO port by the pipe. */
        usb_cstd_chg_curpipe(ptr, pipe, useport, USB_NO);
        /* Enable Not Ready Interrupt */
        usb_cstd_NrdyEnable(ptr, pipe);
        /* CPU access Buffer to FIFO start */
        usb_cstd_Buf2D0fifoStartUsb(ptr);
    }
    else
    {
        /* Buffer to FIFO data write */
        usb_cstd_Buf2Fifo(ptr, pipe, useport);
    }
}/* eof usb_cstd_Buf2fifoStartDma() */

/******************************************************************************
Function Name   : usb_cstd_Fifo2BufStartDma
Description     : Start transfer using DMA/DTC. If transfer size is 0, clear DMA. 
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t pipe   : Pipe nr.
                : uint16_t useport: FIFO select
                : uint32_t length
Return value    : none
******************************************************************************/
void    usb_cstd_Fifo2BufStartDma( USB_UTR_t *ptr, uint16_t pipe, uint16_t useport, uint32_t length )
{
    uint16_t        mxps;

    /* Data size check */
    if( usb_gcstd_Dma0Size[ptr->ip] != 0u )
    {
        /* 16bit access */
        if( (usb_gcstd_Dma0Size[ptr->ip] & 0x0001u) == 0u )
        {
            /* D0FIFO to Buffer DMA read start */
            usb_cpu_d0fifo2buf_start_dma(ptr, usb_cstd_GetD0fifo16Adr(ptr));
        }
        else
        {
            /* 8bit access */
            /* D0FIFO to Buffer DMA read start */
            usb_cpu_d0fifo2buf_start_dma(ptr, usb_cstd_GetD0fifo8Adr(ptr));
        }

        /* Changes the FIFO port by the pipe. */
        usb_cstd_chg_curpipe(ptr, pipe, useport, USB_NO);
        /* Max Packet Size */
        mxps = usb_cstd_GetMaxPacketSize(ptr, pipe);
        if( length != (uint32_t)0u )
        {
            /* Data length check */
            if( (length % mxps) == (uint32_t)0u )
            {
                /* Set Transaction counter */
                usb_cstd_SetTransactionCounter(ptr, pipe, (uint16_t)(length / mxps));
            }
            else
            {
                /* Set Transaction counter */
                usb_cstd_SetTransactionCounter(ptr, pipe, (uint16_t)((length / mxps) + (uint32_t)1u));
            }
        }
        /* Set BUF */
        usb_cstd_SetBuf(ptr, pipe);
        /* Enable Ready Interrupt */
        usb_creg_set_brdyenb(ptr, pipe);
        /* Enable Not Ready Interrupt */
        usb_cstd_NrdyEnable(ptr, pipe);
        usb_cstd_D0fifo2BufStartUsb(ptr);
    }
    else
    {
        /* Changes the FIFO port by the pipe. */
        usb_cstd_chg_curpipe(ptr, pipe, useport, USB_NO);
        /* DMA buffer clear mode set */
        usb_creg_set_dclrm( ptr, USB_D0DMA );
        /* Set BUF */
        usb_cstd_SetBuf(ptr, pipe);
        /* Enable Ready Interrupt */
        usb_creg_set_brdyenb(ptr, pipe);
        /* Enable Not Ready Interrupt */
        usb_cstd_NrdyEnable(ptr, pipe);
    }
}/* eof usb_cstd_Fifo2BufStartDma() */

/******************************************************************************
End of file
******************************************************************************/
