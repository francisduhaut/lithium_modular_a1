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
* Copyright (C) 2010(2011) Renesas Electronics Corpration
* and Renesas Solutions Corp. All rights reserved.
******************************************************************************
* File Name    : r_tfat2_drv_if.c
* Version      : 1.10
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
*         : 01.06.2011 1.10    Version 1.10 Release
******************************************************************************/

/************************************************************************************
Includes         
************************************************************************************/
//#include "r_usb_cDefUsrPp.h"      /* System definition */
#include "r_usb_cTypedef.h"         /* Type define */
#include "r_usb_cDefUSBIP.h"        /* USB-FW Library Header */
#include "r_usb_cMacSystemcall.h"   /* System call macro */
#include "r_usb_cKernelId.h"        /* Kernel ID definition */
#include "r_usb_cmsc_define.h"      /* USB  Mass Storage Class Header */
#include "r_TinyFAT.h"              /* TFAT define */

#include "r_usb_ansi.h"
#include "r_usb_api.h"
#include "r_usb_hmsc_define.h"
#include "r_usb_hmsc_extern.h"
#include "r_usb_api.h"

/******************************************************************************
Constant macro definitions
******************************************************************************/
#define USB_ERROR           (uint16_t)(0xFFFF)


/******************************************************************************
External variables and functions
******************************************************************************/
void R_usb_hmsc_WaitLoop(void);

/*  Real Time Clock - Time set to 23 FEB 2009 15:02:20  */
uint8_t         usb_ghmsc_rtcYear = 109, usb_ghmsc_rtcMon = 2,
                usb_ghmsc_rtcMday = 23;
uint8_t         usb_ghmsc_rtcHour = 15, usb_ghmsc_rtcMin = 2,
                usb_ghmsc_rtcSec = 20;

FATFS           usb_ghmsc_FATFS;

uint16_t        usb_ghmsc_tfatSecSize = 512;

extern uint16_t usb_ghmsc_RootDevaddr;

USB_UTR_t tfat_ptr;

/******************************************************************************
Renesas Abstracted RSK functions
******************************************************************************/

/******************************************************************************
Pragma variables
******************************************************************************/
#pragma inline_asm ENABLE_IRQ

/******************************************************************************
Function Name   : ENABLE_IRQ
Description     : Enable IRQ
Arguments       : none
Return value    : none
******************************************************************************/
static void ENABLE_IRQ(void)
{
    SETPSW I
}


/******************************************************************************
Pragma variables
******************************************************************************/
#pragma inline_asm DISABLE_IRQ

/******************************************************************************
Function Name   : DISABLE_IRQ
Description     : Disable IRQ
Arguments       : none
Return value    : none
******************************************************************************/
static void DISABLE_IRQ(void)
{
    CLRPSW I
}


/******************************************************************************
* Function Name : R_tfat_disk_initialize
* Include       : r_tfat_drv_if.h
* Declaration   : DSTATUS R_tfat_disk_initialize ()
* Description   : This function initializes the memory medium
*               :    for file operations
* Arguments     : none
* Return value  : Status of the memory medium
******************************************************************************/
DSTATUS R_tfat_disk_initialize(void)
{
    /*  Please put the code for disk_initalize driver interface function
         over here  */
    /*  Please refer the application note for details   */  
    /*  Please put the code for memory driver initialization
        , if any, over here */

    /* File system mount ID */
    usb_ghmsc_FATFS.id          = 0;
    /* Number of root directory entries */
    usb_ghmsc_FATFS.n_rootdir   = 0;
    /* Current sector appearing in the win[] */
    usb_ghmsc_FATFS.winsect     = 0;
    /* FAT start sector */
    usb_ghmsc_FATFS.fatbase     = 0;
    /* Root directory start sector */
    usb_ghmsc_FATFS.dirbase     = 0;
    /* Data start sector */
    usb_ghmsc_FATFS.database    = 0;
    /* Sectors per fat */
    usb_ghmsc_FATFS.sects_fat   = 0;
    /* Maximum cluster# + 1 */
    usb_ghmsc_FATFS.max_clust   = 0;
#if !_TFAT_FS_READONLY
    /* Last allocated cluster */
    usb_ghmsc_FATFS.last_clust  = 0;
    /* Number of free clusters */
    usb_ghmsc_FATFS.free_clust  = 0;
#if _TFAT_USE_FSINFO
    /* fsinfo sector */
    usb_ghmsc_FATFS.fsi_sector  = 0;
    /* fsinfo dirty flag (1:must be written back) */
    usb_ghmsc_FATFS.fsi_flag    = 0;
    usb_ghmsc_FATFS.pad1        = 0;    
#endif
#endif
    /* FAT sub type */
    usb_ghmsc_FATFS.fs_type     = 0;
    /* Number of sectors per cluster */
    usb_ghmsc_FATFS.csize       = 0;
    /* Number of FAT copies */
    usb_ghmsc_FATFS.n_fats      = 0;
    /* win[] dirty flag (1:must be written back) */
    usb_ghmsc_FATFS.winflag     = 0;
    /* Disk access window for Directory/FAT/File */
    usb_ghmsc_FATFS.win[0]      = 0;

    return  TFAT_RES_OK;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
* Function Name : R_tfat_disk_read
* Include       : r_tfat_drv_if.h
* Declaration   : DRESULT R_tfat_disk_read (uint8_t Drive, uint8_t* Buffer
*               :   , uint32_t SectorNumber, uint8_t SectorCount)
* Description   : This function reads data from the specified location
*               :    of the memory medium
* Arguments     : uint8_t  drive        : Physical drive number
*               : uint8_t* buffer       : Pointer to the read data buffer
*               : uint32_t sector_number : uint32_t SectorNumber
*               : uint8_t sector_count   : Number of sectors to read 
* Return value  : Result of function execution
******************************************************************************/
DRESULT R_tfat_disk_read (
    uint8_t drive,         /* Physical drive number             */    
    uint8_t* buffer,       /* Pointer to the read data buffer   */
    uint32_t sector_number, /* Start sector number              */
    uint8_t sector_count    /* Number of sectors to read        */
)
{
    /*  Please put the code for R_tfat_disk_read driver interface
        function over here  */
    /*  Please refer the application note for details                           */
    uint16_t        res[10], sts0;
    USB_UTR_t       *mess;
    USB_CLSINFO_t   *mes;
    uint16_t        mess_err;

    uint16_t        err;
    uint32_t        tran_byte;

    /* set transfer length */
    tran_byte = (uint32_t)sector_count * usb_ghmsc_tfatSecSize;

    /* read function */
    err = R_usb_hmsc_StrgReadSector(&tfat_ptr, (uint16_t)drive, buffer, sector_number, (uint16_t)sector_count,
        tran_byte);
    do
    {
        R_usb_hmsc_WaitLoop();
        R_usb_hstd_DeviceInformation(&tfat_ptr, usb_ghmsc_RootDevaddr, (uint16_t *)res);
        sts0 = res[1];

        mess_err = R_USB_TRCV_MSG(USB_HSTRG_MBX, (USB_MSG_t**)&mess, (uint16_t)0);
    }
    while( (mess_err != USB_OK) && (sts0 != USB_DETACHED) );

    if( mess_err == USB_OK )
    {
        mes = (USB_CLSINFO_t *)mess;
        err = mes->result;
        R_USB_REL_BLK(USB_HSTRG_MPL,(USB_MH_t)mess);
    }
    else
    {
        err = USB_ERROR;
    }

    if( err != USB_OK )
    {
        return TFAT_RES_ERROR;
    }
    return TFAT_RES_OK;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_WaitLoop
Description     : Hmsc wait loop function
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_hmsc_WaitLoop(void)
{
    if( R_usb_cstd_CheckSchedule() == USB_FLGSET )
    {
        R_usb_hstd_HcdTask((USB_VP_INT)0);
        R_usb_hstd_MgrTask((USB_VP_INT)0);
        R_usb_hhub_Task((USB_VP_INT)0);
        usb_hmsc_Task();
    }
    R_usb_cstd_Scheduler();
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
* Function Name : R_tfat_disk_write
* Include       : r_tfat_drv_if.h
* Declaration   : DRESULT R_tfat_disk_write (uint8_t drive
*               :   , const uint8_t* buffer, uint32_t sector_number
*               :   , uint8_t sector_count)
* Description   : This function writes data to a specified location
*               :    of the memory medium
* Arguments     : uint8_t Drive : Physical drive number
*               : const uint8_t* buffer       : Pointer to the write data
*               : uint32_t       sector_number : Sector number to write
*               : uint8_t        sector_count  : Number of sectors to write
* Return value  : Result of function execution
******************************************************************************/
DRESULT R_tfat_disk_write (
    uint8_t drive,          /* Physical drive number            */
    const uint8_t* buffer,  /* Pointer to the write data        */
    uint32_t sector_number,  /* Sector number to write          */
    uint8_t sector_count     /* Number of sectors to write      */
)
{
    /*  Please put the code for R_tfat_disk_write driver interface
        function over here  */
    /*  Please refer the application note for details                           */  
    uint16_t        res[10], sts0;
    USB_UTR_t       *mess;
    USB_CLSINFO_t   *mes;

    uint16_t        err;
    uint32_t        tran_byte;

    /* set transfer length */
    tran_byte = (uint32_t)sector_count * usb_ghmsc_tfatSecSize;

    /* write function */
    err = R_usb_hmsc_StrgWriteSector(&tfat_ptr, (uint16_t)drive, (uint8_t *)buffer
            , sector_number, (uint16_t)sector_count, tran_byte);

    do
    {
        R_usb_hmsc_WaitLoop();
        R_usb_hstd_DeviceInformation(&tfat_ptr, usb_ghmsc_RootDevaddr, (uint16_t *)res);
        sts0 = res[1];
    }
    while( (R_USB_TRCV_MSG(USB_HSTRG_MBX, (USB_MSG_t**)&mess, (uint16_t)0 )!= USB_OK) && (sts0!= USB_DETACHED) );

    mes = (USB_CLSINFO_t *)mess;
    err = mes->result;
    R_USB_REL_BLK(USB_HSTRG_MPL,(USB_MH_t)mess);

    if( err != USB_OK )
    {
        return TFAT_RES_ERROR;
    }
    return TFAT_RES_OK;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
* Function Name : R_tfat_disk_ioctl
* Include       : r_tfat_drv_if.h
* Declaration   : DRESULT R_tfat_disk_ioctl (uint8_t drive
*               :   , uint8_t command, void* buffer)
* Description   : This function is used to execute memory operations
*               :    other than read\write
* Arguments     : uint8_t drive   : Drive number
*               : uint8_t command : Control command code
*               : void*   buffer  : Data transfer buffer
* Return value  : Result of function execution
******************************************************************************/
DRESULT R_tfat_disk_ioctl (
    uint8_t drive,          /* Drive number             */
    uint8_t command,        /* Control command code     */
    void* buffer            /* Data transfer buffer     */
){

    /*  Please put the code for R_tfat_disk_ioctl driver interface
         function over here */
    /*  Please refer the application note for details                           */  
    return TFAT_RES_OK;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
* Function Name : R_tfat_disk_status
* Include       : r_tfat_drv_if.h
* Declaration   : DSTATUS R_tfat_disk_status (uint8_t drive)
* Description   : This function is used to retrieve the current status
*               :    of the disk
* Arguments     : uint8_t drive : Physical drive number
* Return value  : Status of the disk
******************************************************************************/
DSTATUS R_tfat_disk_status (
    uint8_t drive           /* Physical drive number    */
)
{

    /*  Please put the code for R_tfat_disk_status driver interface
         function over here */
    /*  Please refer the application note for details                           */  
    return TFAT_RES_OK;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
* Function Name : R_tfat_get_fattime
* Include       : r_tfat_sample_software.h
* Declaration   : void R_tfat_get_fattime(void)
* Description   : This function returns the current date and time
* Arguments     : none
* Return value  : uint32_t
* NOTE          : Please DO NOT modify this function. This function
*               :    is used by the FAT
                :   library to get the current date and time during
*               :    file manipulations.
******************************************************************************/
uint32_t R_tfat_get_fattime (void)
{
    uint32_t tmr;

    /* Disable interrupts   */
    DISABLE_IRQ();

    /* Convert the time to store in FAT entry   */
    tmr =   (((uint32_t)usb_ghmsc_rtcYear - 80) << 25);
    tmr |=  ((uint32_t)usb_ghmsc_rtcMon << 21);
    tmr |=  ((uint32_t)usb_ghmsc_rtcMday << 16);
    tmr |=  ((uint32_t)usb_ghmsc_rtcHour << 11);
    tmr |=  ((uint32_t)usb_ghmsc_rtcMin << 5);
    tmr |=  ((uint32_t)usb_ghmsc_rtcSec >> 1);

    /* Enable interrupts    */
    ENABLE_IRQ();

    return tmr;
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/
