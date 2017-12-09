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
******************************************************************************
* Copyright (C) 2010(2011,2012) Renesas Electronics Corpration
* and Renesas Solutions Corp. All rights reserved.
******************************************************************************
* File Name    : r_usb_api.h
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB API header
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
******************************************************************************/

/* $Id: r_usb_api.h 162 2012-05-21 10:20:32Z ssaek $ */

#ifndef __R_USB_API_H__
#define __R_USB_API_H__

#include "r_usb_ctypedef.h"

/*****************************************************************************
Public Functions (API)
******************************************************************************/

/* ANSI IO */
int16_t open( int8_t *, uint16_t, uint16_t );
int16_t close( int16_t );
int32_t write( int16_t, uint8_t *, int32_t );
int32_t read( int16_t, uint8_t *, int32_t );
int16_t control( int16_t, uint16_t, void * );

/* USB API (Host) */
uint16_t    R_usb_hstd_allocatePipe(USB_UTR_t *ptr, uint16_t type);
void        R_usb_hstd_freePipe(USB_UTR_t *ptr, uint8_t pipeno);
USB_ER_t    R_usb_hstd_EnumGetDescriptor(USB_UTR_t *ptr, uint8_t addr, uint8_t cnt_value, USB_CB_t complete);
USB_ER_t    R_usb_hstd_MgrEnumSetConfiguration(USB_UTR_t *ptr, uint8_t devadr, uint8_t config_val, USB_CB_t complete);
USB_ER_t    R_usb_hstd_TransferStart(USB_UTR_t *utr_table);
USB_ER_t    R_usb_hstd_SetPipeRegistration(USB_UTR_t *ptr, uint16_t *table, uint16_t pipe);
USB_ER_t    R_usb_hstd_TransferEnd(USB_UTR_t *ptr, uint16_t pipe, uint16_t status);
USB_ER_t    R_usb_hstd_ChangeDeviceState(USB_UTR_t *ptr, USB_CB_INFO_t complete, uint16_t msginfo, uint16_t rootport);
USB_ER_t    R_usb_hstd_MgrOpen(USB_UTR_t *ptr);
USB_ER_t    R_usb_hstd_MgrClose(void);
void        R_usb_hstd_DriverRegistration(USB_UTR_t *ptr, USB_HCDREG_t *callback);
void        R_usb_hstd_DriverRelease(USB_UTR_t *ptr, uint8_t devclass);
uint16_t    R_usb_hstd_ChkPipeInfo(uint16_t speed, uint16_t *EpTbl, uint8_t *Descriptor);
void        R_usb_hstd_SetPipeInfo(uint16_t *EpTbl, uint16_t *TmpTbl, uint16_t length);
void        R_usb_hstd_DeviceInformation(USB_UTR_t *ptr, uint16_t addr, uint16_t *tbl);
USB_ER_t    R_usb_hstd_MgrChangeDeviceState(USB_UTR_t *ptr, USB_CB_t complete, uint16_t msginfo, uint16_t devaddr);
void        R_usb_hstd_ReturnEnuMGR(USB_UTR_t *ptr, uint16_t cls_result);
void        R_usb_hstd_EnuWait(USB_UTR_t *ptr, uint8_t taskID);
uint16_t    R_usb_hstd_DetachControl(uint16_t port);
void        R_usb_hstd_usbdriver_start( USB_UTR_t *ptr );

USB_ER_t    R_usb_hstd_HcdOpen(USB_UTR_t *ptr);
USB_ER_t    R_usb_hstd_HcdClose(void);

/* USB API (Peripheral) */
uint16_t    R_usb_pstd_ControlRead(USB_UTR_t *ptr, uint32_t Bsize, uint8_t *Table);
void        R_usb_pstd_ControlWrite(USB_UTR_t *ptr, uint32_t Bsize, uint8_t *Table);
void        R_usb_pstd_ControlEnd(USB_UTR_t *ptr, uint16_t status);
USB_ER_t    R_usb_pstd_PcdOpen(USB_UTR_t *ptr);
USB_ER_t    R_usb_pstd_PcdClose(USB_UTR_t *ptr);
USB_ER_t    R_usb_pstd_TransferStart(USB_UTR_t *ptr);
USB_ER_t    R_usb_pstd_TransferEnd(USB_UTR_t *ptr, uint16_t pipe, uint16_t status);
USB_ER_t    R_usb_pstd_PcdChangeDeviceState(USB_UTR_t *ptr, uint16_t state, uint16_t port_no, USB_CB_INFO_t complete);
void        R_usb_pstd_DeviceInformation(USB_UTR_t *ptr, uint16_t *tbl);
void        R_usb_pstd_DriverRegistration(USB_UTR_t *ptr, USB_PCDREG_t *callback);
void        R_usb_pstd_DriverRelease(void);
void        R_usb_pstd_SetPipeRegister(USB_UTR_t *ptr, uint16_t PipeNo, uint16_t *tbl);
void        R_usb_pstd_SetPipeStall(USB_UTR_t *ptr, uint16_t pipeno);
void        R_usb_pstd_usbdriver_start( USB_UTR_t *ptr );

/* USB API (Other) */
void        R_usb_cstd_ClearHwFunction(USB_UTR_t *ptr);
void        R_usb_cstd_UsbIpInit( USB_UTR_t *ptr, uint16_t usb_mode );
uint8_t     R_usb_cstd_CheckSchedule(void);
void        R_usb_ScheInit( void );
USB_REGADR_t    R_usb_cstd_GetUsbIpAdr( uint16_t ipno );
void        R_usb_cstd_UsbIpInit( USB_UTR_t *ptr, uint16_t usb_mode );
void        R_usb_cstd_SetRegDvstctr0( USB_UTR_t *ptr, uint16_t val );
void        R_usb_cstd_SetRegPipeCtr( USB_UTR_t *ptr, uint16_t pipeno, uint16_t val );



void        R_usb_hhub_Open(USB_UTR_t *ptr, uint16_t devaddr, uint16_t data2);
void        R_usb_hhub_Close(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t data2);
void        R_usb_hhub_Registration(USB_UTR_t *ptr, USB_HCDREG_t *callback);
USB_ER_t    R_usb_hhub_ChangeDeviceState(USB_UTR_t *ptr, USB_CB_t complete, uint16_t msginfo, uint16_t devaddr);
uint16_t    R_usb_hhub_GetHubInformation(USB_UTR_t *ptr, uint16_t hubaddr, USB_CB_t complete);
uint16_t    R_usb_hhub_GetPortInformation(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t port, USB_CB_t complete);

uint16_t    R_usb_hhub_get_hub_addr(USB_UTR_t *ptr, uint16_t devadr);
uint16_t    R_usb_hhub_get_hub_port_no(USB_UTR_t *ptr, uint16_t devadr);
uint16_t    R_usb_hhub_chk_connect_status(USB_UTR_t *ptr, uint16_t hub_adr);

void        R_usb_pstd_PcdTask(USB_VP_INT_t);
void        R_usb_hhub_Task(USB_VP_INT_t);
void        R_usb_hstd_MgrTask(USB_VP_INT_t);
void        R_usb_hstd_HcdTask(USB_VP_INT_t);
void        R_usb_cstd_IdleTask(USB_VP_INT stacd);
void        R_usb_hstd_HubRegistAll(USB_UTR_t *ptr);

/* for NonOS Scheduler */
USB_ER_t    R_usb_cstd_RecMsg( uint8_t id, USB_MSG_t** mess, USB_TM_t tm );
USB_ER_t    R_usb_cstd_SndMsg( uint8_t id, USB_MSG_t* mess );
USB_ER_t    R_usb_cstd_iSndMsg( uint8_t id, USB_MSG_t* mess );
USB_ER_t    R_usb_cstd_PgetBlk( uint8_t id, USB_UTR_t** blk );
USB_ER_t    R_usb_cstd_RelBlk( uint8_t id,  USB_UTR_t* blk );
void        R_usb_cstd_Scheduler(void);
void        R_usb_cstd_SetTaskPri(uint8_t tasknum, uint8_t pri);


#endif  /* __R_USB_API_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
