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
* File Name    : r_usb_preg_access.c
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB Peripheral signal control code
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
*         : 19.03.2012         Comments translation. Changed tabs to 4 spa-
*                              ces. SET EDITOR TO INSERT SPACES WHEN TAB PRESSED.
******************************************************************************/


/* $Id: r_usb_preg_access.c 114 2012-04-09 06:18:09Z tmura $ */

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
#pragma section _preg

/************/
/*  SYSCFG  */
/************/

/******************************************************************************
Function Name   : usb_preg_set_dprpu
Description     : Set DPRPU-bit SYSCFG register.
                : (Enable D+Line pullup when PeripheralController function is selected) 
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void    usb_preg_set_dprpu( USB_UTR_t *ptr )
{
    ptr->ipp->SYSCFG.WORD |= USB_DPRPU;
} /* eof usb_preg_set_dprpu() */

/******************************************************************************
Function Name   : usb_preg_clr_dprpu
Description     : Clear DPRPU-bit of the SYSCFG register.
                : (Disable D+Line pullup when PeripheralController function is 
                : selected.)
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void    usb_preg_clr_dprpu( USB_UTR_t *ptr )
{
    ptr->ipp->SYSCFG.WORD &= ~USB_DPRPU;
} /* eof usb_preg_clr_dprpu() */

/************/
/*  SYSSTS0 */
/************/

/**************/
/*  DVSTCTR0  */
/**************/
/******************************************************************************
Function Name   : usb_preg_set_wkup
Description     : Set WKUP-bit DVSTCTR register.
                : (Output Remote wakeup signal when PeripheralController function is selected)
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void    usb_preg_set_wkup( USB_UTR_t *ptr )
{
    ptr->ipp->DVSTCTR0.WORD |= USB_WKUP;
} /* eof usb_preg_set_wkup() */

/**************/
/*  TESTMODE  */
/**************/

/************/
/*  PINCFG  */
/************/

/**********************************/
/*  DMA0CFG, DMA1CFG  for 597ASSP */
/**********************************/

/***************************/
/*  CFIFO, D0FIFO, D1FIFO  */
/***************************/

/**********************************/
/* CFIFOSEL, D0FIFOSEL, D1FIFOSEL */
/**********************************/

/**********************************/
/* CFIFOCTR, D0FIFOCTR, D1FIFOCTR */
/**********************************/

/*************/
/*  INTENB0  */
/*************/
/******************************************************************************
Function Name   : usb_preg_set_enb_rsme
Description     : Enable interrupt from RESUME
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void    usb_preg_set_enb_rsme( USB_UTR_t *ptr )
{
    ptr->ipp->INTENB0.WORD |= USB_RSME;
} /* eof usb_preg_set_enb_rsme() */

/******************************************************************************
Function Name   : usb_preg_clr_enb_rsme
Description     : Disable interrupt from RESUME
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void    usb_preg_clr_enb_rsme( USB_UTR_t *ptr )
{
    ptr->ipp->INTENB0.WORD &= ~USB_RSME;
} /* eof usb_preg_set_enb_rsme() */

/*************/
/*  INTENB1  */
/*************/

/*************/
/*  BRDYENB  */
/*************/

/*************/
/*  NRDYENB  */
/*************/

/*************/
/*  BEMPENB  */
/*************/

/************/
/*  SOFCFG   */
/************/

/************/
/*  INTSTS0  */
/************/
/******************************************************************************
Function Name   : usb_preg_clr_sts_resm
Description     : Clear interrupt status of RESUME.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void    usb_preg_clr_sts_resm( USB_UTR_t *ptr )
{
    ptr->ipp->INTSTS0.WORD = (uint16_t)~USB_RESM;
} /* eof usb_preg_clr_sts_resm() */

/******************************************************************************
Function Name   : usb_preg_clr_sts_dvst
Description     : Clear Device State Transition interrupt flag. 
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void    usb_preg_clr_sts_dvst( USB_UTR_t *ptr )
{
    ptr->ipp->INTSTS0.WORD = (uint16_t)~USB_DVST;
} /* eof usb_preg_clr_sts_dvst() */

/******************************************************************************
Function Name   : usb_preg_clr_sts_ctrt
Description     : Clear Control Transfer Stage Transition interrupt flag.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void    usb_preg_clr_sts_ctrt( USB_UTR_t *ptr )
{
    ptr->ipp->INTSTS0.WORD = (uint16_t)~USB_CTRT;
}/* eof usb_preg_clr_sts_dvst() */

/******************************************************************************
Function Name   : usb_preg_clr_sts_valid
Description     : Clear the Setup Packet Reception interrupt flag.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void    usb_preg_clr_sts_valid( USB_UTR_t *ptr )
{
    ptr->ipp->INTSTS0.WORD = (uint16_t)~USB_VALID;
}/* eof usb_preg_clr_sts_valid() */

/************/
/*  INTSTS1  */
/************/

/************/
/* BRDYSTS  */
/************/

/************/
/* NRDYSTS  */
/************/

/************/
/* BEMPSTS  */
/************/

/************/
/* FRMNUM   */
/************/

/************/
/* USBADDR  */
/************/

/************/
/* USBREQ   */
/************/

/************/
/* USBVAL   */
/************/

/************/
/* USBINDX  */
/************/

/************/
/* USBLENG  */
/************/

/************/
/* DCPCFG   */
/************/

/************/
/* DCPMAXP  */
/************/

/************/
/* DCPCTR   */
/************/
/******************************************************************************
Function Name   : usb_preg_clr_sts_valid
Description     : Enable termination of control transfer status stage.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void    usb_preg_set_ccpl( USB_UTR_t *ptr )
{
    ptr->ipp->DCPCTR.WORD |= USB_CCPL;
}/* eof usb_preg_set_ccpl() */

/************/
/* PIPESEL  */
/************/

/************/
/* PIPECFG  */
/************/

/************/
/* PIPEBUF  */
/************/

/************/
/* PIPEMAXP */
/************/

/************/
/* PIPEPERI */
/************/

/********************/
/* DCPCTR, PIPEnCTR */
/********************/

/************/
/* PIPEnTRE */
/************/

/************/
/* PIPEnTRN */
/************/

/************/
/* DEVADDn  */
/************/

/******************************************************************************
End of file
******************************************************************************/

