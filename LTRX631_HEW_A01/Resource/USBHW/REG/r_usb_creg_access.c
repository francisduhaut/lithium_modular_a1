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
* File Name    : r_usb_creg_access.c
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB IP register access code
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release.
*         : 1.3.2012           Comments and descriptions (REA). Changed tabs to 
*                              4 spaces. (Do not add tabs - your editor can be
*                              set to insert spaces when you press Tab.).
******************************************************************************/

/* $Id: r_usb_creg_access.c 143 2012-05-07 09:16:46Z tmura $ */

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"     /* Type define */
#include "r_usb_usrconfig.h"
#include "r_usb_cdefusbip.h"        /* USB-FW Library Header */
#include "r_usb_cextern.h"          /* USB-FW global definition */
#include "iodefine.h"
#include "r_usb_cusb_bitdefine.h"

/******************************************************************************
Section    <Section Definition> , "Project Sections"
******************************************************************************/
#pragma section _creg

/******************************************************************************
Constant macro definitions
******************************************************************************/
#define USB_TYPE_NUM_SHIFT  14
#define USB_MXPS_NUM_SHIFT  0


/*************/
/*  SYSCFG  */
/*************/
/* System Configuration Control Register. */

/******************************************************************************
Function Name   : usb_creg_read_syscfg
Description     : Returns the specified port's SYSCFG register value.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t port   : Port number (not used $REA)
Return value    : SYSCFG content.
******************************************************************************/
uint16_t    usb_creg_read_syscfg( USB_UTR_t *ptr, uint16_t port )
{
    return ptr->ipp->SYSCFG.WORD;
}/* eof usb_creg_read_syscfg() */

/******************************************************************************
Function Name   : usb_creg_write_syscfg
Description     : Write specified value to the SYSCFG register of the given port.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t port   : Port number (only port 0 used $REA)
                : uint16_t data   : Value to write.
Return value    : none
******************************************************************************/
void    usb_creg_write_syscfg( USB_UTR_t *ptr, uint16_t port, uint16_t  data )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->SYSCFG.WORD = data;
    }
}/* eof usb_creg_write_syscfg */

/******************************************************************************
Function Name   : usb_creg_set_xtal
Description     : Not processed as the functionality is provided by R8A66597(ASSP).
Arguments       : USB_UTR_t *ptr    : USB system internal data.
                : uint16_t data     : Not used for 597ASSP silicon.
Return value    : none
******************************************************************************/
void    usb_creg_set_xtal( USB_UTR_t *ptr, uint16_t data )
{
}/* eof usb_creg_set_xtal() */

/******************************************************************************
Function Name   : usb_creg_set_xcke
Description     : Not processed as the functionality is provided by R8A66597(ASSP).
Arguments       : USB_UTR_t *ptr    : USB system internal data.
Return value    : none
******************************************************************************/
void    usb_creg_set_xcke( USB_UTR_t *ptr )
{
}/* eof usb_creg_set_xcke() */

/******************************************************************************
Function Name   : usb_creg_set_scke
Description     : Enable USB module clock.
Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel.
Return value    : -
******************************************************************************/
void    usb_creg_set_scke( USB_UTR_t *ptr )
{
    ptr->ipp->SYSCFG.WORD |= USB_SCKE;
}/* eof usb_creg_set_xcke */

/******************************************************************************
Function Name   : usb_creg_clr_scke
Description     : Disable USB module clock.
Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel.
Return value    : -
******************************************************************************/
void    usb_creg_clr_scke( USB_UTR_t *ptr )
{
    ptr->ipp->SYSCFG.WORD &= ~USB_SCKE;
}/* eof usb_creg_clr_scke() */

/******************************************************************************
Function Name   : usb_creg_set_hse
Description     : Not processed as the functionality is provided by R8A66597(ASSP).
Arguments       : USB_UTR_t *ptr  : Not used.
                : uint16_t  port  : Not used.
Return value    : none
******************************************************************************/
void    usb_creg_set_hse( USB_UTR_t *ptr, uint16_t port )
{
}/* eof usb_creg_set_hse() */

/******************************************************************************
Function Name   : usb_creg_clr_hse
Description     : Not processed as the functionality is provided by R8A66597(ASSP).
Arguments       : USB_UTR_t *ptr  : Not used.
                : uint16_t  port  : Not used.
Return value    : none
******************************************************************************/
void    usb_creg_clr_hse( USB_UTR_t *ptr, uint16_t port )
{
}/* eof usb_creg_clr_hse() */

/******************************************************************************
Function Name   : usb_creg_set_dcfm
Description     : DCFM-bit set of register SYSCFG
                : (USB Host mode is selected.)
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_creg_set_dcfm( USB_UTR_t *ptr )
{
    ptr->ipp->SYSCFG.WORD |= USB_DCFM;
}/* eof usb_creg_set_dcfm() */

/******************************************************************************
Function Name   : usb_creg_clr_dcfm
Description     : DCFM-bit clear of register SYSCFG.
                : (USB Peripheral mode is selected.)
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_creg_clr_dcfm( USB_UTR_t *ptr )
{
    ptr->ipp->SYSCFG.WORD &= ~USB_DCFM;
}/* eof usb_creg_clr_dcfm() */

/******************************************************************************
Function Name   : usb_creg_set_drpd
Description     : Set bit of the specified port's SYSCFG DRPD register.
                : (for USB Host mode; set D + / D-line PullDown.)
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_creg_set_drpd( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->SYSCFG.WORD |= USB_DRPD;
    }
}/* eof usb_creg_set_drpd() */

/******************************************************************************
Function Name   : usb_creg_clr_drpd
Description     : Clear bit of the specified port's SYSCFG DRPD register.
                : (for USB Host mode; Enable D + / D-line PullDown.)
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : Port number
Return value    : none
******************************************************************************/
void    usb_creg_clr_drpd( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->SYSCFG.WORD &= ~USB_DRPD;
    }
}/* eof usb_creg_clr_drpd() */

/******************************************************************************
Function Name   : usb_creg_set_usbe
Description     : Enable USB operation.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_creg_set_usbe( USB_UTR_t *ptr )
{
    ptr->ipp->SYSCFG.WORD |= USB_USBE;
}/* eof usb_creg_set_usbe() */

/******************************************************************************
Function Name   : usb_creg_clr_usbe
Description     : Enable USB operation.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_creg_clr_usbe( USB_UTR_t *ptr )
{
    ptr->ipp->SYSCFG.WORD &= ~USB_USBE;
}/* eof usb_creg_clr_usbe() */


/***********/
/*  SYSSTS0 */
/***********/
/* System Configuration Status Register 0 */

/******************************************************************************
Function Name   : usb_creg_read_syssts
Description     : Returns the value of the specified port's SYSSTS register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number. ($REA not used.)
Return value    : SYSSTS0 content
******************************************************************************/
uint16_t    usb_creg_read_syssts( USB_UTR_t *ptr, uint16_t port )
{
    return (uint16_t)(ptr->ipp->SYSSTS0.WORD);
}/* eof usb_creg_read_syssts() */

/******************************************************************************
Function Name   : usb_creg_write_syssts
Description     : Write to the specified port's SYSSTS register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
                : uint16_t  data  : The value to write
Return value    : none
******************************************************************************/
void        usb_creg_write_syssts( USB_UTR_t *ptr, uint16_t port, uint16_t data )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->SYSSTS0.WORD = data;
    }
}/* eof usb_creg_write_syssts() */

/************/
/*  DVSTCTR0  */
/************/
/* Device State Control Register 0 */

/******************************************************************************
Function Name   : usb_creg_read_dvstctr
Description     : Returns the specified port's DVSTCTR register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number. ($REA not used.)
Return value    : DVSTCTR0 content
******************************************************************************/
uint16_t    usb_creg_read_dvstctr( USB_UTR_t *ptr, uint16_t port )
{
    return (uint16_t)(ptr->ipp->DVSTCTR0.WORD);
}/* eof usb_creg_read_dvstctr() */

/******************************************************************************
Function Name   : usb_creg_write_dvstctr
Description     : Write data to the specified port's DVSTCTR register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void        usb_creg_write_dvstctr( USB_UTR_t *ptr, uint16_t port, uint16_t data )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->DVSTCTR0.WORD = data;
    }
}/* eof usb_creg_write_dvstctr() */

/******************************************************************************
Function Name   : usb_creg_rmw_dvstctr
Description     : Read-modify-write the specified port's DVSTCTR.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : Port number
                : uint16_t  data  : The value to write.
                : uint16_t  bitptn: Bit pattern to read-modify-write.
Return value    : none
******************************************************************************/
void        usb_creg_rmw_dvstctr( USB_UTR_t *ptr, uint16_t port, uint16_t data, uint16_t bitptn )
{
    uint16_t    buf;

    if( USB_PORT0 == port )
    {
        buf = ptr->ipp->DVSTCTR0.WORD;
        buf &= ~bitptn;
        buf |= (data & bitptn);
        ptr->ipp->DVSTCTR0.WORD = buf;
    }
}/* eof usb_creg_rmw_dvstctr() */

/******************************************************************************
Function Name   : usb_creg_clr_dvstctr
Description     : Clear the bit pattern specified in argument, of the specified 
                : port's DVSTCTR register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : Port number
                : uint16_t  bitptn: Bit pattern to read-modify-write.
Return value    : none
******************************************************************************/
void        usb_creg_clr_dvstctr( USB_UTR_t *ptr, uint16_t port,uint16_t bitptn)
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->DVSTCTR0.WORD &= ~bitptn;
    }
}/* eof usb_creg_clr_dvstctr() */

/******************************************************************************
Function Name   : usb_creg_set_vbout
Description     : Set specified port's VBOUT-bit in the DVSTCTR register.
                : (To output a "High" to pin VBOUT.) 
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : Port number
Return value    : none
******************************************************************************/
void    usb_creg_set_vbout( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->DVSTCTR0.WORD |= USB_VBOUT;
    }
}/* eof usb_creg_set_vbout() */

/******************************************************************************
Function Name   : usb_creg_clr_vbout
Description     : Clear specified port's VBOUT-bit in the DVSTCTR register.
                : (To output a "Low" to pin VBOUT.)
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : Port number
Return value    : none
******************************************************************************/
void    usb_creg_clr_vbout( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->DVSTCTR0.WORD &= ~USB_VBOUT;
    }
}/* eof usb_creg_clr_vbout() */


/**************/
/*  TESTMODE  */
/**************/

/******************************************************************************
Function Name   : usb_creg_set_utst
Description     : Not processed as the functionality is provided by R8A66597(ASSP).
Arguments       : USB_UTR_t *ptr  : 
                : uint16_t  data
Return value    : none
******************************************************************************/
void    usb_creg_set_utst( USB_UTR_t *ptr, uint16_t data )
{
}/* eof usb_creg_set_utst() */


/************/
/*  PINCFG  */
/************/

/******************************************************************************
Function Name   : usb_creg_set_ldrv
Description     : Not processed as the functionality is provided by R8A66597(ASSP).
Arguments       : USB_UTR_t *ptr  : 
Return value    : none
******************************************************************************/
void    usb_creg_set_ldrv( USB_UTR_t *ptr )
{
}/* eof usb_creg_set_ldrv() */

/******************************************************************************
Function Name   : usb_creg_clr_ldrv
Description     : Not processed as the functionality is provided by R8A66597(ASSP).
Arguments       : USB_UTR_t *ptr    : USB system internal data.
Return value    : none
******************************************************************************/
void    usb_creg_clr_ldrv( USB_UTR_t *ptr )
{
}/* eof usb_creg_clr_ldrv() */

/*********************************/
/*  DMA0CFG, DMA1CFG  for 597ASSP */
/*********************************/

/******************************************************************************
Function Name   : usb_creg_write_dmacfg
Description     : Not processed as the functionality is provided by R8A66597(ASSP).
Arguments       : USB_UTR_t *ptr    : USB system internal data.
Return value    : none
******************************************************************************/
void    usb_creg_write_dmacfg( USB_UTR_t *ptr, uint16_t pipemode, uint16_t data )
{
}/* eof usb_creg_write_dmacfg() */

/***************************/
/*  CFIFO, D0FIFO, D1FIFO  */
/***************************/
/* FIFO Port Register */

/******************************************************************************
Function Name   : usb_creg_read_fifo32
Description     : Not processed as the functionality is provided by R2R.
Arguments       : USB_UTR_t *ptr    : USB system internal data.
Return value    : none
******************************************************************************/
uint32_t    usb_creg_read_fifo32( USB_UTR_t *ptr, uint16_t pipemode )
{
    /* for R2R */
    return (uint32_t)0;
}/* eof usb_creg_read_fifo32() */

/******************************************************************************
Function Name   : usb_creg_write_fifo32
Description     : Not processed as the functionality is provided by R2R.
Arguments       : USB_UTR_t *ptr    : USB system internal data.
Return value    : none
******************************************************************************/
void    usb_creg_write_fifo32( USB_UTR_t *ptr, uint16_t pipemode, uint32_t data )
{
}/* eof usb_creg_write_fifo32() */

/******************************************************************************
Function Name   : usb_creg_read_fifo16
Description     : Data is read from the specified pipemode's FIFO register, 16-bits 
                : wide, corresponding to the specified PIPEMODE.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipemode  : CUSE/D0DMA/D1DMA
Return value    : CFIFO/D0FIFO/D1FIFO content (16-bit)
******************************************************************************/
uint16_t    usb_creg_read_fifo16( USB_UTR_t *ptr, uint16_t pipemode )
{
    uint16_t    data;
    switch( pipemode )
    {
        case    USB_CUSE:
            data = ptr->ipp->CFIFO.WORD;
            break;
        case    USB_D0USE:
        case    USB_D0DMA:
            data = ptr->ipp->D0FIFO.WORD;
            break;
        case    USB_D1USE:
        case    USB_D1DMA:
            data = ptr->ipp->D1FIFO.WORD;
            break;
        default:
            while(1);
            break;
    }
    return data;
}/* eof usb_creg_read_fifo16() */

/******************************************************************************
Function Name   : usb_creg_write_fifo16
Description     : Data is written to the specified pipemode's FIFO register, 16-bits 
                : wide, corresponding to the specified PIPEMODE.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipemode  : CUSE/D0DMA/D1DMA
                : uint16_t  data      : The value to write.
Return value    : none
******************************************************************************/
void    usb_creg_write_fifo16( USB_UTR_t *ptr, uint16_t pipemode, uint16_t data )
{
    switch( pipemode )
    {
        case    USB_CUSE:
            ptr->ipp->CFIFO.WORD = data;
            break;
        case    USB_D0USE:
        case    USB_D0DMA:
            ptr->ipp->D0FIFO.WORD = data;
            break;
        case    USB_D1USE:
        case    USB_D1DMA:
            ptr->ipp->D1FIFO.WORD = data;
            break;
        default:
            while(1);
            break;
    }
}/* eof usb_creg_write_fifo16() */

/******************************************************************************
Function Name   : usb_creg_read_fifo8
Description     : Data is read from the specified pipemode's FIFO register, 8-bits 
                : wide, corresponding to the specified PIPEMODE.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipemode : CUSE/D0DMA/D1DMA.
Return value    : CFIFO/D0FIFO/D1FIFO(8-bit)
******************************************************************************/
uint8_t     usb_creg_read_fifo8( USB_UTR_t *ptr, uint16_t pipemode )
{
    uint8_t buf;
    switch( pipemode )
    {
        case    USB_CUSE:
            buf = ptr->ipp->CFIFO.BYTE.L;
            break;
        case    USB_D0USE:
        case    USB_D0DMA:
            buf = ptr->ipp->D0FIFO.BYTE.L;
            break;
        case    USB_D1USE:
        case    USB_D1DMA:
            buf = ptr->ipp->D1FIFO.BYTE.L;
            break;
        default:
            while(1);
            break;
    }
    return  buf;
}/* eof usb_creg_read_fifo8() */

/******************************************************************************
Function Name   : usb_creg_write_fifo8
Description     : Data is written to the specified pipemode's FIFO register, 8-bits 
                : wide, corresponding to the specified PIPEMODE.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipdemode   : CUSE/D0DMA/D1DMA
                : uint8_t   data        : The value to write.
Return value    : none
******************************************************************************/
void        usb_creg_write_fifo8( USB_UTR_t *ptr, uint16_t pipemode, uint8_t data )
{
    switch( pipemode )
    {
        case    USB_CUSE:
            ptr->ipp->CFIFO.BYTE.L = data;
            break;
        case    USB_D0USE:
        case    USB_D0DMA:
            ptr->ipp->D0FIFO.BYTE.L = data;
            break;
        case    USB_D1USE:
        case    USB_D1DMA:
            ptr->ipp->D1FIFO.BYTE.L = data;
            break;
        default:
            while(1);
            break;
    }
}/* eof usb_creg_write_fifo8() */

/**********************************/
/* CFIFOSEL, D0FIFOSEL, D1FIFOSEL */
/**********************************/
/* FIFO Port Select Register */

/******************************************************************************
Function Name   : usb_creg_get_fifosel_adr
Description     : Returns the *address* of the FIFOSEL register corresponding to 
                : specified PIPEMODE.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipemode  : CUSE/D0DMA/D1DMA
Return value    : none
******************************************************************************/
static  void    *usb_creg_get_fifosel_adr( USB_UTR_t *ptr, uint16_t pipemode )
{
    void    *reg_p;

    switch( pipemode )
    {
        case    USB_CUSE:
            reg_p = (void *)&(ptr->ipp->CFIFOSEL);
            break;
        case    USB_D0USE:
        case    USB_D0DMA:
            reg_p = (void *)&(ptr->ipp->D0FIFOSEL);
            break;
        case    USB_D1USE:
        case    USB_D1DMA:
            reg_p = (void *)&(ptr->ipp->D1FIFOSEL);
            break;
        default:
            while(1);
            break;
    }
    return reg_p;
}/* eof usb_creg_get_fifosel_adr() */

/******************************************************************************
Function Name   : usb_creg_read_fifosel
Description     : Returns the value of the specified pipemode's FIFOSEL register.
Arguments       : USB_UTR_t *ptr       : USB internal structure. Selects USB channel.
                : uint16_t  pipemode  : CUSE/D0DMA/D1DMA
Return value    : FIFOSEL content
******************************************************************************/
uint16_t    usb_creg_read_fifosel( USB_UTR_t *ptr, uint16_t pipemode )
{
    uint16_t    *reg_p;

    reg_p = (uint16_t *)usb_creg_get_fifosel_adr( ptr, pipemode );

    return *reg_p;
}/* eof usb_creg_read_fifosel() */

/******************************************************************************
Function Name   : usb_creg_write_fifosel
Description     : Data is written to the specified pipemode's FIFOSEL register, 8-bits 
                : wide, corresponding to the specified PIPEMODE.
Arguments       : USB_UTR_t *ptr       : USB internal structure. Selects USB channel.
                : uint16_t pipemode  : CUSE/D0DMA/D1DMA
                : uint16_t data     : The value to write.
Return value    : none
******************************************************************************/
void    usb_creg_write_fifosel( USB_UTR_t *ptr, uint16_t pipemode, uint16_t data )
{
    uint16_t    *reg_p;

    reg_p = (uint16_t *)usb_creg_get_fifosel_adr( ptr, pipemode );

    *reg_p = data;
}/* eof usb_creg_write_fifosel() */

/******************************************************************************
Function Name   : usb_creg_rmw_fifosel
Description     : Data is written to the specified pipemode's FIFOSEL register 
                : (the FIFOSEL corresponding to the specified PIPEMODE), using 
                : read-modify-write.
Arguments       : USB_UTR_t *ptr       : USB internal structure. Selects USB channel.
                : uint16_t pipemode : CUSE/D0DMA/D1DMA.
                : uint16_t data     : The value to write.
                : uint16_t bitptn   : bitptn: Bit pattern to read-modify-write.
Return value    : none
******************************************************************************/
void    usb_creg_rmw_fifosel( USB_UTR_t *ptr, uint16_t pipemode, uint16_t data, uint16_t bitptn )
{
    uint16_t    buf;
    uint16_t    *reg_p;

    reg_p = (uint16_t *)usb_creg_get_fifosel_adr( ptr, pipemode );

    buf = *reg_p;
    buf &= ~bitptn;
    buf |= (data & bitptn);
    *reg_p = buf;
}/* eof usb_creg_rmw_fifosel() */

/******************************************************************************
Function Name   : usb_creg_set_dclrm
Description     : Set DCLRM-bits (FIFO buffer auto clear) of the FIFOSEL cor-
                : responding to specified PIPEMODE.
Arguments       : USB_UTR_t *ptr       : USB internal structure. Selects USB channel.
                : uint16_t pipemode : CUSE/D0DMA/D1DMA.
Return value    : none
******************************************************************************/
void    usb_creg_set_dclrm( USB_UTR_t *ptr, uint16_t pipemode )
{
    uint16_t    *reg_p;

    reg_p = (uint16_t *)usb_creg_get_fifosel_adr( ptr, pipemode );

    *reg_p |= USB_DCLRM;
}/* eof usb_creg_set_dclrm() */

/******************************************************************************
Function Name   : usb_creg_clr_dclrm
Description     : Reset DCLRM-bits (FIFO buffer not auto-cleared) of the FIFOSEL 
                : corresponding to the specified PIPEMODE.
Arguments       : USB_UTR_t *ptr       : USB internal structure. Selects USB channel.
                : uint16_t pipemode : CUSE/D0DMA/D1DMA.
Return value    : none
******************************************************************************/
void    usb_creg_clr_dclrm( USB_UTR_t *ptr, uint16_t pipemode )
{
    uint16_t    *reg_p;

    reg_p = usb_creg_get_fifosel_adr( ptr, pipemode );

    *reg_p &= ~USB_DCLRM;
}/* eof usb_creg_clr_dclrm() */

/******************************************************************************
Function Name   : usb_creg_set_dreqe
Description     : Set DREQE-bits (to output signal DxREQ_Na) of the FIFOSEL cor-
                : responding to specified PIPEMODE.
Arguments       : USB_UTR_t *ptr       : USB internal structure. Selects USB channel.
                : uint16_t pipemode : CUSE/D0DMA/D1DMA.
Return value    : none
******************************************************************************/
void    usb_creg_set_dreqe( USB_UTR_t *ptr, uint16_t pipemode )
{
    uint16_t    *reg_p;

    reg_p = usb_creg_get_fifosel_adr( ptr, pipemode );

    *reg_p |= USB_DREQE;
}/* eof usb_creg_set_dreqe() */

/******************************************************************************
Function Name   : usb_creg_clr_dreqe
Description     : Clear DREQE-bits (To prohibit the output of the signal DxREQ_N)
                : of the FIFOSEL corresponding to the specified PIPEMODE.
Arguments       : USB_UTR_t *ptr       : USB internal structure. Selects USB channel.
                : uint16_t pipemode : CUSE/D0DMA/D1DMA
Return value    : none
******************************************************************************/
void    usb_creg_clr_dreqe( USB_UTR_t *ptr, uint16_t pipemode )
{
    uint16_t    *reg_p;

    reg_p = usb_creg_get_fifosel_adr( ptr, pipemode );

    *reg_p &= ~USB_DREQE;
}/* eof usb_creg_clr_dreqe() */

/******************************************************************************
Function Name   : usb_creg_set_mbw
Description     : Set MBW-bits (CFIFO Port Access Bit Width) of the FIFOSEL cor-
                : responding to the specified PIPEMODE, to select 8 or 16-bit 
                : wide FIFO port access.
Arguments       : USB_UTR_t *ptr       : USB internal structure. Selects USB channel.
                : uint16_t pipemode : CUSE/D0DMA/D1DMA.
                : uint16_t data     : Defined value of 8 (data = 0) or 16 bit 
                : (data = 1) access mode.
Return value    : none
******************************************************************************/
void    usb_creg_set_mbw( USB_UTR_t *ptr, uint16_t pipemode, uint16_t data )
{
    uint16_t    *reg_p;

    reg_p = usb_creg_get_fifosel_adr( ptr, pipemode );

    if( data != 0 )
    {
        *reg_p |= USB_MBW;
    }
    else
    {
        *reg_p &= ~USB_MBW;
    }
}/* eof usb_creg_set_mbw() */

/******************************************************************************
Function Name   : usb_creg_set_bigend
Description     : Set BIGEND-bit of the FIFOSEL corresponding to the specified 
                : PIPEMODE to select big or little endian of CFIFO.
                : mode of the CFIFO.
Arguments       : USB_UTR_t *ptr       : USB internal structure. Selects USB channel.
                : uint16_t pipemode : CUSE/D0DMA/D1DMA.
                : uint16_t data     : Defined value of big/little endian.
Return value    : none
******************************************************************************/
void    usb_creg_set_bigend( USB_UTR_t *ptr, uint16_t pipemode, uint16_t data )
{
    uint16_t    *reg_p;

    reg_p = usb_creg_get_fifosel_adr( ptr, pipemode );

    if( data != 0 )
    {
        *reg_p |= USB_BIGEND;
    }
    else
    {
        *reg_p &= ~USB_BIGEND;
    }
}/* eof usb_creg_set_bigend() */
/******************************************************************************
Function Name   : usb_creg_set_curpipe
Description     : Set pipe to the number given; in the FIFOSEL corresponding 
                : to specified PIPEMODE.
Arguments       : USB_UTR_t *ptr       : USB internal structure. Selects USB channel.
                : uint16_t pipemode : CUSE/D0DMA/D1DMA.
                : uint16_t pipeno   : Pipe number.
Return value    : none
******************************************************************************/
void    usb_creg_set_curpipe( USB_UTR_t *ptr, uint16_t  pipemode, uint16_t  pipeno )
{
    uint16_t    *reg_p;
    uint16_t    reg;

    reg_p = usb_creg_get_fifosel_adr( ptr, pipemode );
    reg = *reg_p;

    reg &= ~USB_CURPIPE;
    reg |= pipeno;
    
    *reg_p = reg;
}/* eof usb_creg_set_curpipe() */

/**********************************/
/* CFIFOCTR, D0FIFOCTR, D1FIFOCTR */
/**********************************/
/* FIFO control Registers */

/******************************************************************************
Function Name   : usb_creg_get_fifoctr_adr
Description     : Returns the *address* of the FIFOCTR register corresponding to 
                : specified PIPEMODE.
                : (FIFO Port Control Register.)
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t pipemode : CUSE/D0DMA/D1DMA.
Return value    : none
******************************************************************************/
static  void    *usb_creg_get_fifoctr_adr( USB_UTR_t *ptr, uint16_t pipemode )
{
    void    *reg_p;

    switch( pipemode )
    {
        case    USB_CUSE:
            reg_p = (void *)&(ptr->ipp->CFIFOCTR);
            break;
        case    USB_D0USE:
        case    USB_D0DMA:
            reg_p = (void *)&(ptr->ipp->D0FIFOCTR);
            break;
        case    USB_D1USE:
        case    USB_D1DMA:
            reg_p = (void *)&(ptr->ipp->D1FIFOCTR);
            break;
        default:
            while(1);
            break;
    }
    return reg_p;
}/* eof usb_creg_get_fifoctr_adr() */

/******************************************************************************
Function Name   : usb_creg_read_fifoctr
Description     : Returns the value of the FIFOCTR register corresponding to 
                : specified PIPEMODE.
Arguments       : USB_UTR_t *ptr       : USB internal structure. Selects USB channel.
                : uint16_t pipemode : CUSE/D0DMA/D1DMA.
Return value    : FIFOCTR content
******************************************************************************/
uint16_t    usb_creg_read_fifoctr( USB_UTR_t *ptr, uint16_t pipemode )
{
    uint16_t    *reg_p;

    reg_p = (uint16_t *)usb_creg_get_fifoctr_adr( ptr, pipemode );

    return *reg_p;
}/* eof usb_creg_read_fifoctr() */

/******************************************************************************
Function Name   : usb_creg_set_bval
Description     : Set BVAL (Buffer Memory Valid Flag) to the number given; in 
                : the FIFOCTR corresponding to the specified PIPEMODE.
Arguments       : USB_UTR_t *ptr       : USB internal structure. Selects USB channel.
                : uint16_t pipemode : CUSE/D0DMA/D1DMA.
Return value    : none
******************************************************************************/
void    usb_creg_set_bval( USB_UTR_t *ptr, uint16_t pipemode )
{
    uint16_t *reg_p;

    reg_p = (uint16_t *)usb_creg_get_fifoctr_adr( ptr, pipemode );

    *reg_p |= USB_BVAL;
}/* eof usb_creg_set_bval() */

/******************************************************************************
Function Name   : usb_creg_set_bclr
Description     : Set BCLR (CPU Buffer Clear) to the number given; in the 
                : FIFOCTR corresponding to the specified PIPEMODE.
Arguments       : USB_UTR_t *ptr       : USB internal structure. Selects USB channel.
                : uint16_t pipemode : CUSE/D0DMA/D1DMA.
Return value    : none
******************************************************************************/
void    usb_creg_set_bclr( USB_UTR_t *ptr, uint16_t pipemode )
{
    uint16_t *reg_p;

    reg_p = (uint16_t *)usb_creg_get_fifoctr_adr( ptr, pipemode );

    *reg_p = USB_BCLR;
}/* eof usb_creg_set_bclr() */


/*************/
/*  INTENB0  */
/*************/
/* Interrupt Enable Register 0 */

/******************************************************************************
Function Name   : usb_creg_read_intenb
Description     : Returns INTENB0 register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : INTENB0 content
******************************************************************************/
uint16_t    usb_creg_read_intenb( USB_UTR_t *ptr )
{
    return ptr->ipp->INTENB0.WORD;
}/* eof usb_creg_read_intenb() */

/******************************************************************************
Function Name   : usb_creg_write_intenb
Description     : Data is written to INTENB register, 
                : enabling/disabling the various USB interrupts.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void    usb_creg_write_intenb( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->INTENB0.WORD = data;
}/* eof usb_creg_write_intenb() */

/******************************************************************************
Function Name   : usb_creg_set_intenb
Description     : Bit(s) to be set in INTENB register, 
                : enabling the respective USB interrupt(s).
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data  : Bit pattern: Respective interrupts with '1' 
                                  : will be enabled.
Return value    : none
******************************************************************************/
void    usb_creg_set_intenb( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->INTENB0.WORD |= data;
}/* eof usb_creg_set_intenb() */

/******************************************************************************
Function Name   : usb_creg_clr_enb_vbse
Description     : Clear the VBE-bit of INTENB register,
                : to prohibit VBUS interrupts.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_creg_clr_enb_vbse( USB_UTR_t *ptr )
{
    ptr->ipp->INTENB0.WORD &= ~USB_VBSE;
}/* eof usb_creg_clr_enb_vbse() */

/******************************************************************************
Description     : Clear the SOFE-bit of INTENB register,
                : to prohibit Frame Number Update interrupts.
Function Name   : usb_creg_clr_enb_sofe
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_creg_clr_enb_sofe( USB_UTR_t *ptr )
{
    ptr->ipp->INTENB0.WORD &= ~USB_SOFE;
}/* eof usb_creg_clr_enb_sofe() */


/*************/
/*  INTENB1  */
/*************/

/*************/
/*  BRDYENB  */
/*************/
/* BRDY Interrupt Enable Register */

/******************************************************************************
Function Name   : usb_creg_read_brdyenb
Description     : Returns BRDYENB register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : BRDYENB content
******************************************************************************/
uint16_t    usb_creg_read_brdyenb( USB_UTR_t *ptr )
{
    return ptr->ipp->BRDYENB.WORD;
}/* eof usb_creg_read_brdyenb() */

/******************************************************************************
Function Name   : usb_creg_write_brdyenb
Description     : Data is written to BRDYENB register, 
                : enabling/disabling each respective pipe's BRDY interrupt. 
                : (The BRDY interrupt indicates that a FIFO port is accessible.)
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data        : The value to write.
Return value    : none
******************************************************************************/
void    usb_creg_write_brdyenb( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->BRDYENB.WORD = data;
}/* eof usb_creg_write_brdyenb() */

/******************************************************************************
Function Name   : usb_creg_set_brdyenb
Description     : A bit is set in the specified pipe's BRDYENB, enabling the 
                : respective pipe BRDY interrupt(s).
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
Return value    : none
******************************************************************************/
void    usb_creg_set_brdyenb( USB_UTR_t *ptr, uint16_t  pipeno )
{
    ptr->ipp->BRDYENB.WORD |= (1 << pipeno);
}/* eof usb_creg_set_brdyenb() */

/******************************************************************************
Function Name   : usb_creg_clr_brdyenb
Description     : Clear the PIPExBRDYE-bit of the specified pipe to prohibit 
                : BRDY interrupts of that pipe.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
Return value    : none
******************************************************************************/
void    usb_creg_clr_brdyenb( USB_UTR_t *ptr, uint16_t  pipeno )
{
    ptr->ipp->BRDYENB.WORD &= ~(1 << pipeno);
}/* eof usb_creg_clr_brdyenb() */


/*************/
/*  NRDYENB  */
/*************/
/* NRDY (not ready) Interrupt Enable Register */

/******************************************************************************
Function Name   : usb_creg_read_nrdyenb
Description     : Returns NRDYENB register content. 
                : (The NRDY interrupt indicates that more time is needed before 
                : continuing data communication.)
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : NRDYENB content
******************************************************************************/
uint16_t    usb_creg_read_nrdyenb( USB_UTR_t *ptr )
{
    return ptr->ipp->NRDYENB.WORD;
}/* eof usb_creg_read_nrdyenb() */

/******************************************************************************
Function Name   : usb_creg_write_nrdyenb
Description     : Data is written to NRDYENB register, 
                : enabling/disabling each respective pipe's NRDY interrupt
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void    usb_creg_write_nrdyenb( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->NRDYENB.WORD = data;
}/* eof usb_creg_write_nrdyenb() */

/******************************************************************************
Function Name   : usb_creg_set_nrdyenb
Description     : A bit is set in the specified pipe's NRDYENB, enabling the 
                : respective pipe NRDY interrupt(s).
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
Return value    : none
******************************************************************************/
void    usb_creg_set_nrdyenb( USB_UTR_t *ptr, uint16_t pipeno )
{
    ptr->ipp->NRDYENB.WORD |= (1 << pipeno);
}/* eof usb_creg_set_nrdyenb() */

/******************************************************************************
Function Name   : usb_creg_clr_nrdyenb
Description     : Clear the PIPExNRDYE-bit of the specified pipe to prohibit 
                : NRDY interrupts of that pipe.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
Return value    : none
******************************************************************************/
void    usb_creg_clr_nrdyenb(USB_UTR_t *ptr, uint16_t pipeno )
{
    ptr->ipp->NRDYENB.WORD &= ~(1 << pipeno);
}/* eof usb_creg_clr_nrdyenb() */


/*************/
/*  BEMPENB  */
/*************/
/* BEMP (buffer empty) Interrupt Enable Register */

/******************************************************************************
Function Name   : usb_creg_read_bempenb
Description     : Returns BEMPENB register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : BEMPENB content
******************************************************************************/
uint16_t    usb_creg_read_bempenb( USB_UTR_t *ptr )
{
    return ptr->ipp->BEMPENB.WORD;
}/* eof usb_creg_read_bempenb() */

/******************************************************************************
Function Name   : usb_creg_write_bempenb
Description     : Data is written to BEMPENB register, 
                : enabling/disabling each respective pipe's BEMP interrupt. 
                : (The BEMP interrupt indicates that the USB buffer is empty, 
                : and so the FIFO can now be written to.)
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void    usb_creg_write_bempenb( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->BEMPENB.WORD = data;
}/* eof usb_creg_write_bempenb() */

/******************************************************************************
Function Name   : usb_creg_set_bempenb
Description     : A bit is set in the specified pipe's BEMPENB enabling the 
                : respective pipe's BEMP interrupt(s).
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
Return value    : none
******************************************************************************/
void    usb_creg_set_bempenb( USB_UTR_t *ptr, uint16_t pipeno )
{
    ptr->ipp->BEMPENB.WORD |= (1 << pipeno);
}/* eof usb_creg_set_bempenb() */

/******************************************************************************
Function Name   : usb_creg_clr_bempenb
Description     : Clear the PIPExBEMPE-bit of the specified pipe to prohibit 
                : BEMP interrupts of that pipe.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
Return value    : none
******************************************************************************/
void    usb_creg_clr_bempenb( USB_UTR_t *ptr, uint16_t pipeno )
{
    ptr->ipp->BEMPENB.WORD &= ~(1 << pipeno);
}/* eof usb_creg_clr_bempenb() */


/*************/
/*  SOFCFG   */
/*************/
/* SOF (start of frame) Output Configuration Register */

/******************************************************************************
Function Name   : usb_creg_read_sofcfg
Description     : Returns SOFCFG register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : SOFCFG content
******************************************************************************/
uint16_t    usb_creg_read_sofcfg( USB_UTR_t *ptr )
{
    return ptr->ipp->SOFCFG.WORD;
}/* eof usb_creg_read_sofcfg() */


/*************/
/*  INTSTS0  */
/*************/
/* Interrupt Status Register 0 */

/******************************************************************************
Function Name   : usb_creg_read_intsts
Description     : Returns INTSTS0 register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : INTSTS0 content
******************************************************************************/
uint16_t    usb_creg_read_intsts( USB_UTR_t *ptr )
{
    return ptr->ipp->INTSTS0.WORD;
}/* eof usb_creg_read_intsts() */

/******************************************************************************
Function Name   : usb_creg_write_intsts
Description     : Data is written to INTSTS0 register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void    usb_creg_write_intsts( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->INTSTS0.WORD = data;
}/* eof usb_creg_write_intsts() */

/******************************************************************************
Function Name   : usb_creg_clr_sts_vbint
Description     : Clear the the VBINT status bit to clear its the VBUS inter-
                : rupt status.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_creg_clr_sts_vbint( USB_UTR_t *ptr )
{
    ptr->ipp->INTSTS0.WORD = (uint16_t)~USB_VBINT;
}/* eof usb_creg_clr_sts_vbint() */

/******************************************************************************
Function Name   : usb_creg_clr_sts_sofr
Description     : Clear the SOFR-bit (Frame Number Refresh Interrupt Status) of 
                : the clear SOF interrupt status.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_creg_clr_sts_sofr( USB_UTR_t *ptr )
{
    ptr->ipp->INTSTS0.WORD = (uint16_t)~USB_SOFR;
}/* eof usb_creg_clr_sts_sofr() */


/*************/
/*  INTSTS1  */
/*************/
/* Interrupt Status Register 1 */


/*************/
/* BRDYSTS  */
/*************/
/* BRDY (buffer ready) Interrupt Status Register */

/******************************************************************************
Function Name   : usb_creg_read_brdysts
Description     : Returns BRDYSTS register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : BRDYSTS content
******************************************************************************/
uint16_t    usb_creg_read_brdysts( USB_UTR_t *ptr )
{
    return ptr->ipp->BRDYSTS.WORD;
}/* eof usb_creg_read_brdysts() */

/******************************************************************************
Function Name   : usb_creg_write_brdysts
Description     : Data is written to BRDYSTS register, to set the BRDY interrupt status.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void        usb_creg_write_brdysts( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->BRDYSTS.WORD = data;
}/* eof usb_creg_write_brdysts() */

/******************************************************************************
Function Name   : usb_creg_clr_sts_brdy
Description     : Clear the PIPExBRDY status bit of the specified pipe to clear 
                : its BRDY interrupt status.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
Return value    : none
******************************************************************************/
void    usb_creg_clr_sts_brdy( USB_UTR_t *ptr, uint16_t pipeno )
{
    ptr->ipp->BRDYSTS.WORD = (uint16_t)~(1 << pipeno);
}/* eof usb_creg_clr_sts_brdy() */

/************/
/* NRDYSTS  */
/************/
/* NRDY (not ready) Interrupt Status Register */
/******************************************************************************
Function Name   : usb_creg_read_brdysts
Description     : Returns NRDYSTS register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : NRDYSTS content
******************************************************************************/
uint16_t    usb_creg_read_nrdysts( USB_UTR_t *ptr )
{
    return (uint16_t)ptr->ipp->NRDYSTS.WORD;
}/* eof usb_creg_read_brdysts() */

/******************************************************************************
Function Name   : usb_creg_write_nrdysts
Description     : Data is written to NRDYSTS register, to
                : set the NRDY interrupt status.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void        usb_creg_write_nrdysts( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->NRDYSTS.WORD = data;
}/* eof usb_creg_write_nrdysts() */

/******************************************************************************
Function Name   : usb_creg_clr_sts_nrdy
Description     : Clear the PIPExNRDY status bit of the specified pipe to clear 
                : its NRDY interrupt status.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
Return value    : none
******************************************************************************/
void    usb_creg_clr_sts_nrdy( USB_UTR_t *ptr, uint16_t pipeno )
{
    ptr->ipp->NRDYSTS.WORD = (uint16_t)~(1 << pipeno);
}/* eof usb_creg_clr_sts_nrdy() */


/************/
/* BEMPSTS  */
/************/
/* BEMP Interrupt Status Register */

/******************************************************************************
Function Name   : usb_creg_read_bempsts
Description     : Returns BEMPSTS register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : BEMPSTS content
******************************************************************************/
uint16_t    usb_creg_read_bempsts( USB_UTR_t *ptr )
{
    return (uint16_t)ptr->ipp->BEMPSTS.WORD;
}/* eof usb_creg_read_bempsts() */

/******************************************************************************
Function Name   : usb_creg_write_bempsts
Description     : Data is written to BEMPSTS register, to set the BEMP interrupt status.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data        : The value to write.
Return value    : none
******************************************************************************/
void    usb_creg_write_bempsts( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->BEMPSTS.WORD = data;
}/* eof usb_creg_write_bempsts() */

/******************************************************************************
Function Name   : usb_creg_clr_sts_bemp
Description     : Clear the PIPExBEMP status bit of the specified pipe to clear 
                : its BEMP interrupt status.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
Return value    : none
******************************************************************************/
void    usb_creg_clr_sts_bemp( USB_UTR_t *ptr, uint16_t pipeno )
{
    ptr->ipp->BEMPSTS.WORD = (uint16_t)~(1 << pipeno);
}/* eof usb_creg_clr_sts_bemp() */


/***********/
/* FRMNUM   */
/***********/
/* Frame Number Register */

/******************************************************************************
Function Name   : usb_creg_read_frmnum
Description     : Returns FRMNUM register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : FRMNUM content
******************************************************************************/
uint16_t    usb_creg_read_frmnum( USB_UTR_t *ptr )
{
    return (uint16_t)ptr->ipp->FRMNUM.WORD;
}/* eof usb_creg_read_frmnum() */


/************/
/* USBADDR  */
/************/
/* USB Address Register */

/******************************************************************************
Function Name   : usb_creg_read_usbaddr
Description     : Returns USBADDR register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : USBADDR content
******************************************************************************/
uint16_t    usb_creg_read_usbaddr( USB_UTR_t *ptr )
{
    return (uint16_t)ptr->ipp->USBADDR.WORD;
}/* eof usb_creg_read_usbaddr() */

/******************************************************************************
Function Name   : usb_creg_set_stsrecov
Description     : STSRECOV-bits are set in USBADDR register
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data  : Value to be set.
Return value    : none
******************************************************************************/
void    usb_creg_set_stsrecov( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->USBADDR.BIT.STSRECOV = data;
}/* eof usb_creg_set_stsrecov() */


/***********/
/* USBREQ   */
/***********/
/* USB Request Type Register (bRequest and bmRequestType) */

/******************************************************************************
Function Name   : usb_creg_read_usbreq
Description     : Returns USBREQ register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : USBREQ content
******************************************************************************/
uint16_t    usb_creg_read_usbreq( USB_UTR_t *ptr )
{
    return (uint16_t)ptr->ipp->USBREQ.WORD;
}/* eof usb_creg_read_usbreq() */


/************/
/* USBVAL   */
/************/
/* USB Request Value Register (wValue) */

/******************************************************************************
Function Name   : usb_creg_read_usbval
Description     : Returns USBVAL register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : USBVAL content
******************************************************************************/
uint16_t    usb_creg_read_usbval( USB_UTR_t *ptr )
{
    return (uint16_t)ptr->ipp->USBVAL;
}/* eof usb_creg_read_usbval() */


/************/
/* USBINDX  */
/************/
/* USB Request Index Register (wIndex) */

/******************************************************************************
Function Name   : usb_creg_read_usbindx
Description     : Returns USBINDX register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : USBINDX content
******************************************************************************/
uint16_t    usb_creg_read_usbindx( USB_UTR_t *ptr )
{
    return (uint16_t)ptr->ipp->USBINDX;
}/* eof usb_creg_read_usbindx() */


/************/
/* USBLENG  */
/************/
/* USB Request Length Register (wLength) */

/******************************************************************************
Function Name   : usb_creg_read_usbleng
Description     : Returns USBLENG register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : USBLENG content
******************************************************************************/
uint16_t    usb_creg_read_usbleng( USB_UTR_t *ptr )
{
    return (uint16_t)ptr->ipp->USBLENG;
}/* eof usb_creg_read_usbleng() */


/***********/
/* DCPCFG   */
/***********/
/* DCP Configuration Register */

/******************************************************************************
Function Name   : usb_creg_read_dcpcfg
Description     : Returns DCPCFG register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : DCPCFG content
******************************************************************************/
uint16_t    usb_creg_read_dcpcfg( USB_UTR_t *ptr )
{
    return (uint16_t)ptr->ipp->DCPCFG.WORD;
}/* eof usb_creg_read_dcpcfg() */

/******************************************************************************
Function Name   : usb_creg_write_dcpcfg
Description     : Specified data is written to DCPCFG register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data
Return value    : none
******************************************************************************/
void    usb_creg_write_dcpcfg( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->DCPCFG.WORD = data;
}/* eof usb_creg_write_dcpcfg()*/

/******************************************************************************
Function Name   : usb_creg_set_dcpshtnak
Description     : SHTNAK-bit in the DCPCFG register is set.
                : = Pipe disabled at end of transfer.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_creg_set_dcpshtnak( USB_UTR_t *ptr )
{
    ptr->ipp->DCPCFG.WORD |= USB_SHTNAKON;
}/* eof usb_creg_set_dcpshtnak() */


/************/
/* DCPMAXP  */
/************/
/* DCP Maximum Packet Size Register */

/******************************************************************************
Function Name   : usb_creg_read_dcpmaxp
Description     : Returns DCPMAXP register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : DCPMAXP content
******************************************************************************/
uint16_t    usb_creg_read_dcpmaxp( USB_UTR_t *ptr )
{
    return (uint16_t)ptr->ipp->DCPMAXP.WORD;
}/* eof usb_creg_read_dcpmaxp() */

/******************************************************************************
Function Name   : usb_creg_write_dcpmxps
Description     : Specified data is written to DCPMAXP register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void    usb_creg_write_dcpmxps( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->DCPMAXP.WORD = data;
}/* eof usb_creg_write_dcpmxps() */


/************/
/* DCPCTR   */
/************/
/* DCP Control Register */

/******************************************************************************
Function Name   : usb_creg_read_dcpctr
Description     : Returns DCPCTR register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : DCPCTR content
******************************************************************************/
uint16_t    usb_creg_read_dcpctr( USB_UTR_t *ptr )
{
    return (uint16_t)ptr->ipp->DCPCTR.WORD;
}/* eof usb_creg_read_dcpctr() */


/************/
/* PIPESEL  */
/************/
/* Pipe Window Select Register */
/******************************************************************************
Function Name   : usb_creg_read_pipesel
Description     : Returns PIPESEL register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : PIPESEL content
******************************************************************************/
uint16_t    usb_creg_read_pipesel( USB_UTR_t *ptr )
{
    return (uint16_t)ptr->ipp->PIPESEL.WORD;
}/* eof usb_creg_read_pipesel() */

/******************************************************************************
Function Name   : usb_creg_write_pipesel
Description     : Specified data is written to PIPESEL register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void    usb_creg_write_pipesel( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->PIPESEL.WORD = data;
}/* eof usb_creg_write_pipesel() */


/************/
/* PIPECFG  */
/************/
/* Pipe Configuration Register */

/******************************************************************************
Function Name   : usb_creg_read_pipecfg
Description     : Returns PIPECFG register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : PIPECFG content
******************************************************************************/
uint16_t    usb_creg_read_pipecfg( USB_UTR_t *ptr )
{
    return (uint16_t)ptr->ipp->PIPECFG.WORD;
}/* eof usb_creg_read_pipecfg() */

/******************************************************************************
Function Name   : usb_creg_write_pipecfg
Description     : Specified data is written to PIPECFG register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void    usb_creg_write_pipecfg( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->PIPECFG.WORD = data;
}/* eof usb_creg_write_pipecfg() */

/******************************************************************************
Function Name   : usb_creg_set_type
Description     : Specified Transfer Type is set in PIPECFG register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data  : BULK/INT/ISO
Return value    : none
******************************************************************************/
void    usb_creg_set_type( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->PIPECFG.WORD &= (uint16_t)~USB_TYPE;
    ptr->ipp->PIPECFG.WORD |= (data << USB_TYPE_NUM_SHIFT);
}/* eof usb_creg_set_type() */


/************/
/* PIPEBUF  */
/************/
/* - */

/******************************************************************************
Function Name   : usb_creg_write_pipebuf
Description     : Not processed as the functionality is provided by R8A66597(ASSP).
Arguments       : USB_UTR_t *ptr    : Not used.
                : uint16_t  data    : Not used.
Return value    : none
******************************************************************************/
void    usb_creg_write_pipebuf( USB_UTR_t *ptr, uint16_t data )
{
}/* eof usb_creg_write_pipebuf() */


/************/
/* PIPEMAXP */
/************/
/* Pipe Maximum Packet Size Register */

/******************************************************************************
Function Name   : usb_creg_read_pipemaxp
Description     : Returns PIPEMAXP register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : PIPEMAXP content
******************************************************************************/
uint16_t    usb_creg_read_pipemaxp( USB_UTR_t *ptr )
{
    return (uint16_t)ptr->ipp->PIPEMAXP.WORD;
}/* eof usb_creg_read_pipemaxp() */

/******************************************************************************
Function Name   : usb_creg_write_pipemaxp
Description     : Specified data is written to PIPEMAXP register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void    usb_creg_write_pipemaxp( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->PIPEMAXP.WORD = data;
}/* eof usb_creg_write_pipemaxp() */

/******************************************************************************
Function Name   : usb_creg_set_mxps
Description     : The specified MXPS-bits, Maximum Packet Size, in PIPEMAXP register is set.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data  : Max packet size value.
Return value    : none
******************************************************************************/
void    usb_creg_set_mxps( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->PIPEMAXP.WORD &= (uint16_t)~USB_MXPS;
    ptr->ipp->PIPEMAXP.WORD |= (data << USB_MXPS_NUM_SHIFT);
}/* eof usb_creg_set_mxps() */


/************/
/* PIPEPERI */
/************/
/* Pipe Cycle Control Register */

/******************************************************************************
Function Name   : usb_creg_read_pipeperi
Description     : Returns PIPEPERI register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : PIPEPERI content
******************************************************************************/
uint16_t usb_creg_read_pipeperi( USB_UTR_t *ptr )
{
    return (uint16_t)ptr->ipp->PIPEPERI.WORD;
}/* eof usb_creg_read_pipeperi() */

/******************************************************************************
Function Name   : usb_creg_write_pipeperi
Description     : Specified data is written to PIPEPERI register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void    usb_creg_write_pipeperi( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->PIPEPERI.WORD = data;
}/* eof usb_creg_write_pipeperi() */


/********************/
/* DCPCTR, PIPEnCTR */
/********************/
/* PIPEn Control Registers */

/******************************************************************************
Function Name   : usb_creg_read_pipectr
Description     : Returns DCPCTR or the specified pipe's PIPECTR register content.
                : The Pipe Control Register returned is determined by the speci-
                : fied pipe number.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
Return value    : PIPExCTR content
******************************************************************************/
uint16_t    usb_creg_read_pipectr( USB_UTR_t *ptr, uint16_t pipeno )
{
    uint16_t    *reg_p;

    if( USB_PIPE0 == pipeno )
    {
        reg_p = (uint16_t *)&(ptr->ipp->DCPCTR);
    }
    else
    {
        reg_p = (uint16_t *)&(ptr->ipp->PIPE1CTR) + (pipeno - 1);
    }

    return *reg_p;
}/* eof usb_creg_read_pipectr() */

/******************************************************************************
Function Name   : usb_creg_write_pipectr
Description     : Specified data is written to the specified pipe's PIPEPERI register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void    usb_creg_write_pipectr( USB_UTR_t *ptr, uint16_t pipeno, uint16_t data )
{
    uint16_t    *reg_p;

    if( USB_PIPE0 == pipeno )
    {
        reg_p = (uint16_t *)&(ptr->ipp->DCPCTR);
    }
    else
    {
        reg_p = (uint16_t *)&(ptr->ipp->PIPE1CTR) + (pipeno - 1);
    }
    *reg_p = data;
} /* eof usb_creg_write_pipectr() */

/******************************************************************************
Function Name   : usb_creg_set_csclr
Description     : Not processed as the functionality is provided by R8A66597(ASSP).
Arguments       : USB_UTR_t *ptr    : USB system internal data.
                : uint16_t  pipeno  : Pipe number.
Return value    : none
******************************************************************************/
void    usb_creg_set_csclr( USB_UTR_t *ptr, uint16_t pipeno )
{
}/* eof usb_creg_set_csclr() */

/******************************************************************************
Function Name   : usb_creg_set_aclrm
Description     : The ACLRM-bit (Auto Buffer Clear Mode) is set in the specified 
                : pipe's control register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
Return value    : none
******************************************************************************/
void    usb_creg_set_aclrm( USB_UTR_t *ptr, uint16_t pipeno )
{
    uint16_t    *reg_p;

    reg_p = (uint16_t *)&(ptr->ipp->PIPE1CTR) + (pipeno - 1);

    *reg_p |= USB_ACLRM;
}/* eof usb_creg_set_aclrm() */

/******************************************************************************
Function Name   : usb_creg_clr_aclrm
Description     : Clear the ACLRM bit in the specified pipe's control register
                : to disable Auto Buffer Clear Mode.
                : its BEMP interrupt status.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
Return value    : none
******************************************************************************/
void    usb_creg_clr_aclrm( USB_UTR_t *ptr, uint16_t pipeno )
{
    uint16_t    *reg_p;

    reg_p = (uint16_t *)&(ptr->ipp->PIPE1CTR) + (pipeno - 1);

    *reg_p &= ~USB_ACLRM;
}/* eof usb_creg_clr_aclrm() */

/******************************************************************************
Function Name   : usb_creg_set_sqclr
Description     : The SQCLR-bit (Toggle Bit Clear) is set in the specified pipe's 
                : control register. Setting SQSET to 1 makes DATA0 the expected 
                : data in the pipe's next transfer.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
Return value    : none
******************************************************************************/
void    usb_creg_set_sqclr( USB_UTR_t *ptr, uint16_t pipeno )
{
    if( pipeno == USB_PIPE0 )
    {
        ptr->ipp->DCPCTR.WORD |= USB_SQCLR;
    }
    else
    {
        uint16_t    *reg_p;

        reg_p = ((uint16_t *)&(ptr->ipp->PIPE1CTR) + (pipeno - 1));
        *reg_p |= USB_SQCLR;
    }
}/* eof usb_creg_set_sqclr() */

/******************************************************************************
Function Name   : usb_creg_set_sqset
Description     : The SQSET-bit (Toggle Bit Set) is set in the specified pipe's 
                : control register. Setting SQSET to 1 makes DATA1 the expected 
                : data in the next transfer.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
Return value    : none
******************************************************************************/
void    usb_creg_set_sqset( USB_UTR_t *ptr, uint16_t pipeno )
{
    if( pipeno == USB_PIPE0 )
    {
        ptr->ipp->DCPCTR.WORD |= USB_SQSET;
    }
    else
    {
        uint16_t    *reg_p;

        reg_p = ((uint16_t *)&(ptr->ipp->PIPE1CTR) + (pipeno - 1));
        *reg_p |= USB_SQSET;
    }
}/* eof usb_creg_set_sqset() */

/******************************************************************************
Function Name   : usb_creg_clr_sqset
Description     : The SQSET-bit (Toggle Bit Set) is cleared in the specified 
                : pipe's control register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
Return value    : none
******************************************************************************/
void    usb_creg_clr_sqset( USB_UTR_t *ptr, uint16_t pipeno )
{
    if( pipeno == USB_PIPE0 )
    {
        ptr->ipp->DCPCTR.WORD &= ~USB_SQSET;
    }
    else
    {
        uint16_t *reg_p;

        reg_p = ((uint16_t *)&(ptr->ipp->PIPE1CTR) + (pipeno - 1));
        *reg_p &= ~USB_SQSET;
    }
}/* eof usb_creg_clr_sqset() */

/******************************************************************************
Function Name   : usb_creg_set_pid
Description     : Set the specified PID of the specified pipe's DCPCTR/PIPECTR 
                : register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
                : uint16_t  data  : NAK/BUF/STALL.
Return value    : none
******************************************************************************/
void    usb_creg_set_pid( USB_UTR_t *ptr, uint16_t pipeno, uint16_t data )
{
    uint16_t *reg_p;

    if( pipeno == USB_PIPE0 )
    {
        reg_p = ((uint16_t *)&(ptr->ipp->DCPCTR));
    }
    else
    {
        reg_p = ((uint16_t *)&(ptr->ipp->PIPE1CTR) + (pipeno - 1));
    }
    *reg_p &= ~USB_PID;
    *reg_p |= data;
}/* eof usb_creg_set_pid() */

/******************************************************************************
Function Name   : usb_creg_clr_pid
Description     : Clear the specified PID-bits of the specified pipe's DCPCTR/
                : PIPECTR register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
                : uint16_t  data  : NAK/BUF/STALL - to be cleared.
Return value    : none
******************************************************************************/
void    usb_creg_clr_pid( USB_UTR_t *ptr, uint16_t pipeno, uint16_t data )
{
    uint16_t *reg_p;

    if( pipeno == USB_PIPE0 )
    {
        reg_p = ((uint16_t *)&(ptr->ipp->DCPCTR));
    }
    else
    {
        reg_p = ((uint16_t *)&(ptr->ipp->PIPE1CTR) + (pipeno - 1));
    }
    *reg_p &= ~data;
}/* eof usb_creg_clr_pid() */


/************/
/* PIPEnTRE */
/************/
/* PIPEn Transaction Counter Enable Registers */

/******************************************************************************
Function Name   : usb_creg_read_pipetre
Description     : Returns the specified pipe's PIPETRE register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
Return value    : PIPETRE content
******************************************************************************/
uint16_t    usb_creg_read_pipetre( USB_UTR_t *ptr, uint16_t pipeno )
{
    uint16_t    *reg_p;

    reg_p = (uint16_t *)&(ptr->ipp->PIPE1TRE) + (pipeno - 1) * 2;
    
    return *reg_p;
}/* eof usb_creg_read_pipetre() */

/******************************************************************************
Function Name   : usb_creg_set_trenb
Description     : The TRENB-bit (Transaction Counter Enable) is set in the speci-
                : fied pipe's control register, to enable the counter.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
Return value    : none
******************************************************************************/
void    usb_creg_set_trenb( USB_UTR_t *ptr, uint16_t pipeno )
{
    uint16_t    *reg_p;

    reg_p = (uint16_t *)&(ptr->ipp->PIPE1TRE) + (pipeno - 1) * 2;

    *reg_p |= USB_TRENB;
}/* eof usb_creg_set_trenb() */

/******************************************************************************
Function Name   : usb_creg_clr_trenb
Description     : The TRENB-bit (Transaction Counter Enable) is cleared in the 
                : specified pipe's control register, to disable the counter.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
Return value    : none
******************************************************************************/
void    usb_creg_clr_trenb( USB_UTR_t *ptr, uint16_t pipeno )
{
    uint16_t    *reg_p;

    reg_p = (uint16_t *)&(ptr->ipp->PIPE1TRE) + (pipeno - 1) * 2;

    *reg_p &= ~USB_TRENB;
}/* eof usb_creg_clr_trenb() */

/******************************************************************************
Function Name   : usb_creg_set_trclr
Description     : The TRENB-bit (Transaction Counter Clear) is set in the speci-
                : fied pipe's control register to clear the current counter 
                : value.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
Return value    : none
******************************************************************************/
void    usb_creg_set_trclr( USB_UTR_t *ptr, uint16_t pipeno )
{
    uint16_t    *reg_p;

    reg_p = (uint16_t *)&(ptr->ipp->PIPE1TRE) + (pipeno - 1) * 2;

    *reg_p |= USB_TRCLR;
}/* eof usb_creg_set_trclr() */


/************/
/* PIPEnTRN */
/************/
/* PIPEn Transaction Counter Registers */

/******************************************************************************
Function Name   : usb_creg_read_pipetrn
Description     : Returns the specified pipe's PIPETRN register content.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
                : uint16_t  pipeno: Pipe number.
Return value    : PIPETRN content
******************************************************************************/
uint16_t    usb_creg_read_pipetrn( USB_UTR_t *ptr, uint16_t pipeno )
{
    uint16_t    *reg_p;

    reg_p = (uint16_t *)&(ptr->ipp->PIPE1TRN) + ((pipeno - 1) * 2);

    return *reg_p;
}/* eof usb_creg_read_pipetrn() */

/******************************************************************************
Function Name   : usb_creg_write_pipetrn
Description     : Specified data is written to the specified pipe's PIPETRN reg-
                : ister.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  pipeno: Pipe number.
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void    usb_creg_write_pipetrn( USB_UTR_t *ptr, uint16_t pipeno, uint16_t data )
{
    uint16_t    *reg_p;

    reg_p = (uint16_t *)&(ptr->ipp->PIPE1TRN) + ((pipeno - 1) * 2);

    *reg_p = data;
}/* eof usb_creg_write_pipetrn */

/************/
/* DEVADDn  */
/************/

/******************************************************************************
End of file
******************************************************************************/
