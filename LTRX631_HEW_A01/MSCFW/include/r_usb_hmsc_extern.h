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
* Copyright (C) 2010 Renesas Electronics Corporation. All rights reserved.
******************************************************************************
* File Name    : r_usb_hmsc_extern.h
* Version      : 1.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB common uItron header
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 17.03.2010 0.80    First Release
*         : 30.07.2010 0.90    Updated comments
*         : 02.08.2010 0.91    Updated comments
*         : 29.10.2010 1.00    Mass Production Release
******************************************************************************/
#ifndef __R_USB_HMSCEXTERN_H__
#define __R_USB_HMSCEXTERN_H__

#include "r_usb_cmsc_define.h"      /* USB  Mass Storage Class Header */

/*****************************************************************************
Public Valiables
******************************************************************************/
/* Condition compilation by the difference of quality control */
#ifdef  USB_MISRA_CHECK_PP
extern uint32_t         data_top[];
extern uint32_t         fat_sector[];
extern uint32_t         fat_top[];
extern uint32_t         fsinfo_sector[];
extern uint32_t         offset_sector[];
extern uint32_t         rootdir_top[];
extern uint32_t         total_sector[];
extern uint8_t          cluster_size[];
extern uint8_t          dfs_mediatype[];
extern uint8_t          fat_count[];
extern uint8_t          fat_type[];
extern uint16_t         sector_size[];
#endif /* USB_MISRA_CHECK_PP */

extern uint16_t         usb_ghmsc_AttSts;
extern uint16_t         usb_ghmsc_DefEpTbl[];
extern uint16_t         usb_ghmsc_DriveChk[][5];
extern uint16_t         usb_ghmsc_InPipe[][2];
extern uint16_t         usb_ghmsc_MaxDrive;
extern uint16_t         usb_ghmsc_OutPipe[][2];
extern uint16_t         usb_ghmsc_Seccnt;
extern uint16_t         usb_ghmsc_Side;
extern uint16_t         usb_ghmsc_StrgCount;
extern uint16_t         usb_ghmsc_SuspendPipe[];
extern uint16_t         usb_ghmsc_TmpEpTbl[];
extern uint32_t         usb_ghmsc_CbwTagNo[];
extern uint32_t         usb_ghmsc_CswDataResidue[];
extern uint32_t         usb_ghmsc_MaxLun;
extern uint32_t         usb_ghmsc_Secno;
extern uint32_t         usb_ghmsc_TransSize;
extern uint8_t          *usb_ghmsc_Buff;
extern uint8_t          usb_ghmsc_AtapiFlag[];
extern uint8_t          usb_ghmsc_Data[];
extern uint8_t          usb_ghmsc_ClassData[];
extern USB_MSC_CBW_t    usb_ghmsc_Cbw[];
extern USB_MSC_CSW_t    usb_ghmsc_Csw[];
extern USB_UTR_t        usb_ghmsc_ControlData;
extern USB_UTR_t        usb_ghmsc_ReceiveData[];
extern USB_UTR_t        usb_ghmsc_TransData[];

extern uint16_t         usb_shmsc_NoDataSeq;
extern uint16_t         usb_shmsc_DataInSeq;
extern uint16_t         usb_shmsc_DataOutSeq;
extern uint16_t         usb_shmsc_StallErrSeq;
extern uint16_t         usb_shmsc_DataStallSeq;


/*****************************************************************************
Public Functions
******************************************************************************/

uint16_t    usb_hmsc_CheckCsw(USB_UTR_t *ptr, uint16_t drvnum);
uint16_t    usb_hmsc_DataIn(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size);
uint16_t    usb_hmsc_DataOut(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size);
uint16_t    usb_hmsc_GetCsw(USB_UTR_t *ptr, uint16_t drvnum);
uint16_t    usb_hmsc_GetData(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size);
uint16_t    usb_hmsc_GetMaxUnitCheck(USB_UTR_t *ptr, uint16_t Err);
uint16_t    usb_hmsc_NoData(USB_UTR_t *ptr, uint16_t drvnum);
uint16_t    usb_hmsc_ProcessWaitTmo(uint16_t tmo);
uint16_t    usb_hmsc_SendCbw(USB_UTR_t *ptr, uint16_t drvnum);
uint16_t    usb_hmsc_SendData(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size);
uint16_t    usb_hmsc_SmpBotDescriptor(uint8_t *Table, uint16_t msgnum);
uint16_t    usb_hmsc_SmpCheckAddr(uint16_t side);
uint16_t    usb_hmsc_SmpDevCheckBootRecord(uint8_t *Data, uint32_t *ParLBA, uint8_t *ParInfo, uint16_t flag);
uint16_t    usb_hmsc_SmpDevNextDriveSearch(void);
uint16_t    usb_hmsc_SmpDevReadPartition(USB_UTR_t *ptr, uint16_t unit, uint32_t trans_byte);
uint32_t    usb_hmsc_SmpDevReadSectorSize(USB_UTR_t *ptr, uint16_t side);
uint16_t    usb_hmsc_SmpDrive2Addr(uint16_t side);
uint16_t    usb_hmsc_SmpDrive2Msgnum(uint16_t side);
uint16_t    usb_hmsc_SmpDrive2Part(uint16_t side);
uint16_t    usb_hmsc_SmpDrive2Unit(uint16_t side);
uint16_t    usb_hmsc_SmpFsiFileSystemInitialized(uint16_t side, uint8_t *Data, uint32_t Offset);
uint32_t    usb_hmsc_SmpFsiOffsetSectorRead(uint16_t side);
uint16_t    usb_hmsc_SmpPipeInfo(uint8_t *table, uint16_t msgnum, uint16_t speed, uint16_t length);
uint16_t    usb_hmsc_SmpTotalDrive(void);
USB_ER_t    usb_hmsc_SubmitCtrEnd(USB_UTR_t *utr_table);
USB_ER_t    usb_hmsc_Submitutr(USB_UTR_t *ptr, uint16_t type, USB_UTR_t *utr_table);
USB_ER_t    usb_hmsc_SubmitutrReq(USB_UTR_t *ptr, uint16_t type, USB_UTR_t *utr_table);
uint16_t    usb_hmsc_TransWaitTmo(uint16_t tmo);
void        usb_hmsc_CbwTagCount(uint16_t msgnum);
void        usb_hmsc_ClassWait(USB_ID_t id, USB_UTR_t *mess);
void        usb_hmsc_ClrData(uint16_t len, uint8_t *buf);
void        usb_hmsc_DoSqtgl(USB_UTR_t *ptr, uint16_t Pipe, uint16_t toggle);
void        usb_hmsc_DummyFunction(USB_UTR_t *ptr, uint16_t data1, uint16_t data2);
void        usb_hmsc_MainTask(VP_INT);
void        usb_hmsc_ProcessResult(USB_UTR_t *ptr, uint16_t data, uint16_t dummy);
void        usb_hmsc_SetElsCbw(uint8_t *data, uint32_t trans_byte, uint16_t side);
void        usb_hmsc_SetRwCbw(uint16_t command, uint32_t secno, uint16_t seccnt, uint32_t trans_byte, uint16_t side);
void        usb_hmsc_SmpFsiDriveClear(USB_UTR_t *ptr, uint16_t addr);
void        usb_hmsc_SmpFsiSectorInitialized(uint16_t side, uint32_t offset, uint16_t size);
void        usb_hmsc_StrgCheckResult(USB_UTR_t *mess);
void        usb_hmsc_StrgCommandResult(USB_CLSINFO_t *mess);
void        usb_hmsc_StrgDriveTask(void);
void        usb_hmsc_StrgSpecifiedPath(USB_CLSINFO_t *mess);
void        usb_hmsc_TransResult(USB_UTR_t *mess, uint16_t data1, uint16_t data2);
uint16_t    usb_hmsc_GetStringInfo(USB_UTR_t *ptr, uint16_t devaddr, uint8_t *table);
void        usb_hmsc_SmplRegistration(USB_UTR_t *ptr);

USB_ER_t    usb_hmsc_ClearStall(USB_UTR_t *ptr, uint16_t Pipe, USB_CB_t complete);
uint16_t    usb_hmsc_DataInAct(USB_CLSINFO_t *mess);
uint16_t    usb_hmsc_DataOutAct(USB_CLSINFO_t *mess);
uint16_t    usb_hmsc_GetCswCheck(USB_UTR_t *ptr, uint16_t drvnum, uint16_t hmsc_retval);
uint16_t    usb_hmsc_GetCswReq(USB_UTR_t *ptr, uint16_t drvnum);
uint16_t    usb_hmsc_GetDataCheck(USB_UTR_t *ptr, uint16_t drvnum, uint16_t hmsc_retval);
uint16_t    usb_hmsc_GetDataReq(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size);
uint16_t    usb_hmsc_GetStringInfoCheck(uint16_t devaddr);
uint16_t    usb_hmsc_MassStorageResetCheck(USB_UTR_t *ptr, uint16_t Err);
uint16_t    usb_hmsc_NoDataAct(USB_CLSINFO_t *mess);
uint16_t    usb_hmsc_SendCbwCheck(USB_UTR_t *ptr, uint16_t drvnum, uint16_t hmsc_retval);
uint16_t    usb_hmsc_SendCbwReq(USB_UTR_t *ptr, uint16_t drvnum);
uint16_t    usb_hmsc_SendDataCheck(USB_UTR_t *ptr, uint16_t drvnum, uint16_t hmsc_retval);
uint16_t    usb_hmsc_SendDataReq(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size);
uint16_t    usb_hmsc_SmpDevReadPartitionAct(USB_CLSINFO_t *mess);
void        usb_hmsc_CheckResult(USB_UTR_t *mess, uint16_t, uint16_t);
void        usb_hmsc_ClassCheck(USB_UTR_t *ptr, USB_CLSINFO_t *mess);
void        usb_hmsc_ClearStallCheck(USB_UTR_t *ptr, uint16_t errcheck);
void        usb_hmsc_ClearStallCheck2(USB_UTR_t *mess);
void        usb_hmsc_CommandResult(USB_UTR_t *ptr, uint16_t result);

void        usb_hmsc_DataStall(USB_UTR_t *mess);
void        usb_hmsc_SmpDevReadSectorSizeAct(USB_CLSINFO_t *mess);
void        usb_hmsc_SmpStrgDriveOpenAct( USB_CLSINFO_t *mess );
void        usb_hmsc_SpecifiedPath(USB_CLSINFO_t *mess);
void        usb_hmsc_StallErr(USB_UTR_t *mess);
void        usb_hmsc_StrgDriveSearchAct(USB_CLSINFO_t *mess);
void        usb_hmsc_Task(void);


int         dev_close(int side);
int         dev_open(int side);
int         read_sector(int side, unsigned char *buff, unsigned long secno, long seccnt);
int         write_sector(int side, unsigned char *buff, unsigned long secno, long seccnt);

/* r_usb_HMSC_api.c */
void        usb_hmsc_UsbHostDriverSetting(USB_UTR_t *ptr);
void        usb_hmsc_HubRegistAll(USB_UTR_t *ptr);

/*****************************************************************************
Public Functions (API)
******************************************************************************/

uint16_t    R_usb_hmsc_DriveSpeed(USB_UTR_t *ptr, uint16_t side);
uint16_t    R_usb_hmsc_GetDevSts(void);
uint16_t    R_usb_hmsc_Information(uint16_t PipeOffset);
uint16_t    R_usb_hmsc_Inquiry(USB_UTR_t *ptr, uint16_t side, uint8_t *buff);
uint16_t    R_usb_hmsc_ModeSelect6(USB_UTR_t *ptr, uint16_t side, uint8_t *buff);
uint16_t    R_usb_hmsc_ModeSense10(USB_UTR_t *ptr, uint16_t side, uint8_t *buff);
uint16_t    R_usb_hmsc_PreventAllow(USB_UTR_t *ptr, uint16_t side, uint8_t *buff);
uint16_t    R_usb_hmsc_Read10(USB_UTR_t *ptr, uint16_t side, uint8_t *buff,
                uint32_t secno, uint16_t seccnt, uint32_t trans_byte);
uint16_t    R_usb_hmsc_ReadCapacity(USB_UTR_t *ptr, uint16_t side, uint8_t *buff);
uint16_t    R_usb_hmsc_ReadFormatCapacity(USB_UTR_t *ptr, uint16_t side, uint8_t *buff);
uint16_t    R_usb_hmsc_RequestSense(USB_UTR_t *ptr, uint16_t side, uint8_t *buff);
uint16_t    R_usb_hmsc_SetDevSts(uint16_t data);
uint16_t    R_usb_hmsc_StrgDriveClose(uint16_t side);
USB_ER_t    R_usb_hmsc_StrgDriveOpen(USB_UTR_t *ptr, uint16_t side);
uint16_t    R_usb_hmsc_StrgDriveSearch(USB_UTR_t *ptr, uint16_t addr);
uint16_t    R_usb_hmsc_StrgReadSector(USB_UTR_t *ptr, uint16_t side, uint8_t *buff
                , uint32_t secno, uint16_t seccnt, uint32_t trans_byte);
uint16_t    R_usb_hmsc_StrgUserCommand(USB_UTR_t *ptr, uint16_t side, uint16_t command, uint8_t *buff);
uint16_t    R_usb_hmsc_StrgWriteSector(USB_UTR_t *ptr, uint16_t side, uint8_t *buff
                , uint32_t secno, uint16_t seccnt, uint32_t trans_byte);
uint16_t    R_usb_hmsc_TestUnit(USB_UTR_t *ptr, uint16_t side);
uint16_t    R_usb_hmsc_Write10(USB_UTR_t *ptr, uint16_t side, uint8_t *buff
                , uint32_t secno, uint16_t seccnt, uint32_t trans_byte);
void        R_usb_hmsc_ClassCheck(USB_UTR_t *ptr, uint16_t **table);
void        R_usb_hmsc_DriveClose(USB_UTR_t *ptr, uint16_t addr, uint16_t data2);
void        R_usb_hmsc_DriveOpen(USB_UTR_t *ptr, uint16_t addr, uint16_t data2);
void        R_usb_hmsc_Initialized(USB_UTR_t *ptr, uint16_t data1, uint16_t data2);
void        R_usb_hmsc_Release(USB_UTR_t *ptr);
void        R_usb_hmsc_StrgTaskClose(USB_UTR_t *ptr);
void        R_usb_hmsc_StrgTaskOpen(USB_UTR_t *ptr);
void        R_usb_hmsc_TaskClose(USB_UTR_t *ptr);
void        R_usb_hmsc_TaskOpen(USB_UTR_t *ptr, uint16_t data1, uint16_t data2);
void        R_usb_hmsc_driver_start( USB_UTR_t *ptr );

void        R_usb_hmsc_ClearStall(USB_UTR_t *ptr, uint16_t type, uint16_t msgnum, USB_CB_t complete);
USB_ER_t    R_usb_hmsc_MassStorageReset(USB_UTR_t *ptr, uint16_t drvnum, USB_CB_t complete);
USB_ER_t    R_usb_hmsc_GetMaxUnit(USB_UTR_t *ptr, uint16_t addr, USB_CB_t complete);

#endif  /* __R_USB_HMSCEXTERN_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
