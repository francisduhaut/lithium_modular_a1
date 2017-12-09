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
* File Name    : r_usb_psignal.c
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
*         : 4.4.2012           Function header translation. Ch. tabs to 4 spaces.
*                              SET EDITOR TO INSERT SPACES WHEN TAB PRESSED.
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"         /* Type define */
#include "r_usb_usrconfig.h"        /* USB-H/W register set (user define) */
#include "r_usb_cdefusbip.h"        /* USB-FW Library Header */
#include "r_usb_cmacprint.h"        /* Standard IO macro */
#include "r_usb_cextern.h"          /* USB-FW global definition */
#include "r_usb_cusb_bitdefine.h"
#include "r_usb_reg_access.h"


/******************************************************************************
Section    <Section Definition> , "Project Sections"
******************************************************************************/
#pragma section _pcd

/******************************************************************************
External variables and functions
******************************************************************************/
extern  uint16_t    usb_gpstd_intsts0;

uint16_t usb_pstd_InitFunction(USB_UTR_t *ptr);

/******************************************************************************
Renesas Abstracted Peripheral signal control functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_DpEnable
Description     : D+ Line Pull-up Enable
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_DpEnable(USB_UTR_t *ptr)
{

    usb_preg_set_dprpu( ptr );

}
/******************************************************************************
End of function usb_pstd_DpEnable
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_DpDisable
Description     : D+ Line Pull-up Disable
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
**************************************************************************/
void usb_pstd_DpDisable(USB_UTR_t *ptr)
{

    usb_preg_clr_dprpu( ptr );

}
/******************************************************************************
End of function usb_pstd_DpDisable
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_BusReset
Description     : A USB bus reset was issued by the host. Execute relevant pro-
                : cessing.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_BusReset(USB_UTR_t *ptr)
{
    uint16_t    connect_info;

    /* Bus Reset */
    usb_pstd_BusresetFunction(ptr);

    /* Memory clear */
    usb_pstd_ClearMem();
    connect_info = usb_cstd_PortSpeed(ptr, (uint16_t)USB_PORT0);
    /* Callback */
    (*usb_gpstd_Driver.devdefault)(ptr, connect_info, (uint16_t)USB_NO_ARG);
    /* DCP configuration register  (0x5C) */
    usb_creg_write_dcpcfg( ptr, 0 );
    /* DCP maxpacket size register (0x5E) */
    usb_creg_write_dcpmxps( ptr, usb_gpstd_Driver.devicetbl[USB_DEV_MAX_PKT_SIZE]);
}
/******************************************************************************
 End of function usb_pstd_BusReset
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_InitConnect
Description     : Set up interrupts and initialize.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_InitConnect(USB_UTR_t *ptr)
{
    uint16_t        connect_info;

    /* Interrupt enable */
    usb_pstd_InterruptEnable(ptr);
    usb_cstd_SetHse(ptr, (uint16_t)USB_PORT0, usb_gcstd_HsEnable[ptr->ip]);

    connect_info = usb_pstd_InitFunction( ptr );
    
    switch( connect_info )
    {
    /* Attach host controller */
    case USB_ATTACH:
        usb_pstd_AttachProcess(ptr);
        break;
    /* Detach host controller */
    case USB_DETACH:
        usb_pstd_DetachProcess(ptr);
        break;
    default:
        break;
    }
}
/******************************************************************************
End of function usb_pstd_InitConnect
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_AttachProcess
Description     : USB attach setting.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_AttachProcess(USB_UTR_t *ptr)
{
    usb_pstd_AttachFunction( ptr );
    usb_cpu_DelayXms((uint16_t)10);
    usb_preg_set_dprpu( ptr );
}
/******************************************************************************
End of function usb_pstd_AttachProcess
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_DetachProcess
Description     : Initialize USB registers for detaching, and call the callback
                : function that completes the detach.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_DetachProcess(USB_UTR_t *ptr)
{
    uint16_t        i, conf;
    uint16_t        *tbl;

    /* Pull-up disable */
    usb_preg_clr_dprpu( ptr );
    usb_cpu_Delay1us((uint16_t)2);
    usb_creg_set_dcfm( ptr );
    usb_cpu_Delay1us((uint16_t)1);
    usb_creg_clr_dcfm( ptr );

    conf = usb_gpstd_ConfigNum;
    if( conf < (uint16_t)1 )
    {
        /* Address state */
        conf = (uint16_t)1;
    }

    tbl = usb_gpstd_Driver.pipetbl[conf - 1];
    for( i = 0; tbl[i] != USB_PDTBLEND; i += USB_EPL )
    {
        /* PID=BUF ? */
        if( usb_cstd_GetPid(ptr, tbl[i]) == USB_PID_BUF )
        {
            usb_cstd_ForcedTermination(ptr, tbl[i], (uint16_t)USB_DATA_STOP);
        }
        usb_cstd_ClrPipeCnfg(ptr, tbl[i]);
    }
    /* Callback */
    (*usb_gpstd_Driver.devdetach)(ptr, (uint16_t)USB_NO_ARG, (uint16_t)USB_NO_ARG);
    usb_cstd_StopClock(ptr);
}
/******************************************************************************
End of function usb_pstd_DetachProcess
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_SuspendProcess
Description     : Perform a USB peripheral suspend.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_SuspendProcess(USB_UTR_t *ptr)
{
    uint16_t    intsts0, buf;

    /* Resume interrupt enable */
    usb_preg_set_enb_rsme( ptr );

    intsts0 = usb_creg_read_intsts( ptr );
    buf = usb_creg_read_syssts( ptr, USB_PORT0 );
    if(((intsts0 & USB_DS_SUSP) != (uint16_t)0) && ((buf & USB_LNST) == USB_FS_JSTS))
    {
        /* Suspend */
        usb_cstd_StopClock(ptr);
        usb_pstd_SuspendFunction(ptr);
    }
    /* --- SUSPEND -> RESUME --- */
    else
    {
        /* RESM status clear */
        usb_preg_clr_sts_resm( ptr );
        /* RESM interrupt disable */
        usb_preg_clr_enb_rsme( ptr );
    }
}
/******************************************************************************
End of function usb_pstd_SuspendProcess
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/
