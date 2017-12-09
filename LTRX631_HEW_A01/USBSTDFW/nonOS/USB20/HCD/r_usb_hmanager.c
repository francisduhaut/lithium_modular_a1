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
* File Name    : r_usb_hmanager.c
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB Host Control Manager
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
*         : 3.12.2012          Function header translation. Ch. tabs to 4 spaces.
*                              SET EDITOR TO INSERT SPACES WHEN TAB PRESSED.
******************************************************************************/

/* $Id: r_usb_hmanager.c 162 2012-05-21 10:20:32Z ssaek $ */

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
#include "r_usb_cusb_bitdefine.h"

/******************************************************************************
Section    <Section Definition> , "Project Sections"
******************************************************************************/
#pragma section _hcd

/******************************************************************************
Private global variables and functions
******************************************************************************/
/* Manager */
uint16_t                usb_ghstd_EnumSeq[USB_NUM_USBIP];       /* Enumeration request */
uint16_t                usb_ghstd_DeviceDescriptor[USB_DEVICESIZE / 2u];
uint16_t                usb_ghstd_ConfigurationDescriptor[USB_CONFIGSIZE / 2u];
uint16_t                usb_ghstd_SuspendPipe[USB_MAX_PIPE_NO + 1u];

uint16_t                usb_ghstd_CheckEnuResult[USB_NUM_USBIP];
uint8_t     usb_ghstd_EnuWait[USB_NUM_USBIP + USB_NUM_USBIP%2u];

void usb_hstd_MgrRelMpl(uint16_t n);

/******************************************************************************
Static variables and functions
******************************************************************************/
static uint16_t         usb_shstd_StdRequest[5];
static uint16_t         usb_shstd_DummyData;
static USB_UTR_t        usb_shstd_StdReqMsg;
#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
static USB_UTR_t        usb_shstd_ip[USB_NUM_USBIP];
#else /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */
    /* Condition compilation by the difference of the operating system */
static uint16_t         usb_shstd_RegPointer;
#endif /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */
static USB_MGRINFO_t    *usb_shstd_MgrMsg;
static uint16_t         usb_shstd_mgr_msginfo = 0;
static USB_CB_INFO_t    usb_shstd_mgr_callback;
/* Condition compilation by the difference of the operating system */
static uint16_t         usb_shstd_SuspendSeq = 0;
static uint16_t         usb_shstd_ResumeSeq = 0;
static  void usb_hstd_SuspCont(USB_UTR_t *ptr, uint16_t devaddr, uint16_t rootport);
static  void usb_hstd_ResuCont(USB_UTR_t *ptr, uint16_t devaddr, uint16_t rootport);

static  uint16_t usb_hstd_ChkDeviceClass(USB_UTR_t *ptr, USB_HCDREG_t *driver, uint16_t port);
static  void usb_hstd_EnumerationErr(uint16_t Rnum);

/******************************************************************************
Private global variables and functions
******************************************************************************/
/* Enumeration Table */
void (*usb_ghstd_EnumarationProcess[8])(USB_UTR_t *, uint16_t, uint16_t) =
{
    usb_hstd_EnumGetDescriptor,     usb_hstd_EnumSetAddress,
    usb_hstd_EnumGetDescriptor,     usb_hstd_EnumGetDescriptor,
    usb_hstd_EnumGetDescriptor,     usb_hstd_EnumGetDescriptor,
    usb_hstd_EnumSetConfiguration,  usb_hstd_EnumDummyRequest,
};

/******************************************************************************
External variables and functions
******************************************************************************/
extern  uint16_t    usb_hstd_ChkRemote(USB_UTR_t *ptr);
extern void (*usb_ghstd_EnumarationProcess[8])(USB_UTR_t *,uint16_t, uint16_t); /* Enumeration Table */

#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
//static    uint16_t    usb_shstd_StdRequest[5];
//static    USB_UTR_t   usb_shstd_StdReqMsg;

extern  struct  usb_devinfo usb_devInfo[USB_DEVICENUM*USB_IFNUM*USB_CFGNUM];
extern  uint16_t    usb_gapl_devicetpl[];


/******************************************************************************
Function Name   : usb_hstd_check_tpl
Description     : Check that the VID & PID of the connected USB Device are both 
                : registered in TPL (Target Perihperal List).
Argument        : uint16_t   vid        : Vendor ID
                : uint16_t   pid        : Product ID
Return          : uint16_t              : USB_OK/USB_NG
******************************************************************************/
static  uint16_t    usb_hstd_check_tpl(uint16_t vid, uint16_t pid)
{
    uint8_t cnt;

    for( cnt = 0; cnt < usb_gapl_devicetpl[0]; cnt++ )
    {
        if( (USB_NOVENDOR == usb_gapl_devicetpl[(cnt * 2) + 2]) || (usb_gapl_devicetpl[(cnt * 2) + 2] == vid) )
        {
            if( (USB_NOPRODUCT == usb_gapl_devicetpl[(cnt * 2) + 3]) || (usb_gapl_devicetpl[(cnt * 2) + 3] == pid) )
            {
                return USB_OK;
            }
        }
    }
    return USB_NG;
}
/******************************************************************************
End of function usb_hstd_check_tpl
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_getClassCode
Description     : Get Class Code from Configuration Descriptor of the connected 
                : USB Device.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint8_t   devno        : Device Number
                : uint8_t   if_no        : Interface Number
Return          : none
******************************************************************************/
static  void    usb_hstd_getClassCode( USB_UTR_t *ptr, uint8_t devno, uint8_t if_no )
{
    uint8_t     *p_table;
    uint16_t    offset, total_len;

    p_table = (uint8_t *)usb_ghstd_ConfigurationDescriptor;

    total_len   = ((uint16_t)*(p_table + 3)) << 8;
    total_len   |=  (uint16_t)*(p_table + 2);

    for( offset = 0; ( offset < total_len) && ( *(p_table + offset) > 0 ) ; offset += *(p_table + offset) )
    {
        if( USB_DT_INTERFACE == *(p_table + offset + 1) )
        {
            if( if_no == *(p_table + offset + 2 ) )
            {
                usb_devInfo[devno].interfaceClass = *(p_table + offset + 5);
            }
        }
    }
}
/******************************************************************************
End of function usb_hstd_getClassCode
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_getDeviceInfo
Description     : Analyze the Configuration descriptor of the connected USB 
                : device and register the device information to th structure 
                : usb_devInfo.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t   devsel        : Device Address
Return          : none
******************************************************************************/
static void usb_hstd_GetDeviceInfo( USB_UTR_t *ptr, uint16_t devsel )
{
    uint16_t    vid, pid, cnt, devno;
    uint8_t     num_if, if_no;
    uint8_t     *p_config;
    uint8_t     *p_device;

    /* Getting bNumInteface for ConfigurationDescriptor */
    p_config = (uint8_t*)&usb_ghstd_ConfigurationDescriptor;
    num_if = p_config[4];

    /* Getting VID & PID from DeviceDescriptor */
    p_device = (uint8_t*)&usb_ghstd_DeviceDescriptor;
    vid = p_device[8] | (p_device[9] << 8);
    pid = p_device[10] | (p_device[11] << 8);

    if( USB_OK == usb_hstd_check_tpl( vid, pid ) )
    {
        for( if_no = 0; if_no < num_if; if_no++ )
        {
            /* Searching to find unsetting area in usb_devInfo structure */
            for( cnt = 0, devno = 0xFFFF; (cnt < USB_MAX_DEVICENUM) && (0xFFFF == devno); cnt++ )
            {
                if( 0x00 == usb_devInfo[cnt].devadr )
                {
                    devno = cnt;
                }
            }
            usb_devInfo[devno].isTPL    = USB_OK;

            usb_devInfo[devno].devadr   = devsel >> USB_DEVADDRBIT;
            usb_shstd_ip[ptr->ip].ip    = ptr->ip;
            usb_shstd_ip[ptr->ip].ipp   = ptr->ipp;
            usb_devInfo[devno].ptr      = &usb_shstd_ip[ptr->ip];

            usb_hstd_getClassCode( ptr, devno, if_no );
        }
    }
    else
    {
        usb_devInfo[devno].devadr   = devsel;
        usb_devInfo[devno].isTPL    = (uint8_t)USB_NG;

        USB_PRINTF0("### Not support device\n");
        USB_PRINTF1(" VID=0x%04x,",vid);
        USB_PRINTF1(" PID=0x%04x\n",pid);

        return;
    }

}
/******************************************************************************
End of function usb_hstd_getDeviceInfo
******************************************************************************/

#endif  /* USB_USE_ANSIIO == USB_YES */

/******************************************************************************
Function Name   : usb_hstd_MgrRelMpl
Description     : Release a memory block.
Argument        : uint16_t n    : Error no.
Return          : none
******************************************************************************/
void usb_hstd_MgrRelMpl(uint16_t n)
{
    USB_ER_t        err;

    err = USB_REL_BLK(USB_MGR_MPL, (USB_MH_t)usb_shstd_MgrMsg);
    if( err != USB_E_OK )
    {
        USB_PRINTF1("### USB MGR rel_blk error: %d\n", n);
    }
}
/******************************************************************************
End of function usb_hstd_MgrRelMpl
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_mgr_chgdevst_cb
Description     : Call the callback function specified by the argument given to
                : the API R_usb_hstd_MgrChangeDeviceState.
Argument        : uint16_t rootport : Port no.
Return          : none
******************************************************************************/
static void usb_hstd_mgr_chgdevst_cb(USB_UTR_t *ptr, uint16_t rootport)
{
    if( usb_shstd_mgr_msginfo != 0 )
    {
        (*usb_shstd_mgr_callback)(ptr, rootport, usb_shstd_mgr_msginfo);
        usb_shstd_mgr_msginfo = 0;
    }
}

/******************************************************************************
End of function usb_hstd_mgr_chgdevst_cb
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_Enumeration
Description     : Execute enumeration on the connected USB device.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
Return          : uint16_t       : Enumeration status.
******************************************************************************/
uint16_t usb_hstd_Enumeration(USB_UTR_t *ptr)
{
#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
    uint16_t        retval;
#endif /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */
    uint16_t        md, flg;
    USB_HCDREG_t    *driver;
    uint16_t        enume_mode;     /* Enumeration mode (device state) */
    uint8_t         *descriptor_table;
    uint16_t        rootport, pipenum, devsel;

    /* Attach Detect Mode */
    enume_mode = USB_NONDEVICE;

    pipenum     = usb_shstd_MgrMsg->keyword;
    /* Agreement device address */
    devsel      = usb_cstd_GetDevsel(ptr, pipenum);
    /* Get root port number from device addr */
    rootport    = usb_hstd_GetRootport(ptr, devsel);

    /* Manager Mode Change */
    switch( usb_shstd_MgrMsg->result )
    {
    case USB_CTRL_END:
        enume_mode = USB_DEVICEENUMERATION;
        switch( usb_ghstd_EnumSeq[ptr->ip] )
        {
        /* Receive Device Descriptor */
        case 0:
            break;
        /* Set Address */
        case 1:
            descriptor_table = (uint8_t*)usb_ghstd_DeviceDescriptor;
            devsel = (uint16_t)(usb_ghstd_DeviceAddr[ptr->ip] << USB_DEVADDRBIT);
            /* Set device speed */
            usb_hstd_SetDevAddr(ptr, devsel, usb_ghstd_DeviceSpeed[ptr->ip], rootport);
            usb_ghstd_DcpRegister[ptr->ip][usb_ghstd_DeviceAddr[ptr->ip]] 
                = (uint16_t)((uint16_t)(descriptor_table[7] & USB_MAXPFIELD) | devsel);
            break;
        /* Receive Device Descriptor(18) */
        case 2:
            break;
        /* Receive Configuration Descriptor(9) */
        case 3:
            break;
        /* Receive Configuration Descriptor(xx) */
        case 4:
#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
            for( flg = 0u, md = 0; (md < usb_ghstd_DeviceNum[ptr->ip]) && (flg < usb_ghstd_DeviceNum[ptr->ip]); md++ )
            {
                driver = &usb_ghstd_DeviceDrv[ptr->ip][md];
                if( driver->devstate == USB_DETACHED ) 
                {
                    retval = usb_hstd_ChkDeviceClass(ptr, driver, rootport);
                    if( retval == USB_DONE )
                    {
                        if(driver->ifclass != USB_IFCLS_HUB)
                        {
                            enume_mode = USB_ATTACHDEVICE;
                        }
                        usb_ghstd_DeviceInfo[ptr->ip][usb_ghstd_DeviceAddr[ptr->ip]][0] = rootport; /* Root port */
                        driver->rootport    = rootport;
                        driver->devaddr     = usb_ghstd_DeviceAddr[ptr->ip];
                        flg++;  /* break; */
                        usb_hstd_GetDeviceInfo(ptr, devsel);
                    }
                }
            }
            if( flg == 0 )
            {
                usb_ghstd_EnumSeq[ptr->ip]++;
            }
            break;

#else   /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */

            /* Device enumeration function */
            switch( usb_hstd_EnumFunction1() )
            {
            /* Driver open */
            case USB_DONE:
                for( flg = 0u, md = 0; (md < usb_ghstd_DeviceNum[ptr->ip]) && (flg == 0u); md++ ) 
                {
                    driver = &usb_ghstd_DeviceDrv[ptr->ip][md];
                    if( driver->devstate == USB_DETACHED ) 
                    {
                        uint16_t retval;
                        retval = usb_hstd_ChkDeviceClass(ptr, driver, rootport);
                        usb_ghstd_CheckEnuResult[ptr->ip] = USB_DONE;
                        /* In this function, check device class of       */
                        /*              enumeration flow move to class   */
                        /* "R_usb_hstd_ReturnEnuMGR()" is used to return */
                        if( retval == USB_DONE )
                        {
                            usb_shstd_RegPointer = md;
                            flg = 1;    /* break; */
                        }
                    }
                }
                if( flg != 1 )
                {
                    usb_ghstd_EnumSeq[ptr->ip]++;
                }
                break;
            /* OTG CV test */
            case USB_OTG_DONE:
                for( flg = 0u, md = 0u; (md < usb_ghstd_DeviceNum[ptr->ip]) && (flg == 0); md++ )
                {
                    driver = &usb_ghstd_DeviceDrv[ptr->ip][md];
                    if( driver->devstate == USB_DETACHED )
                    {
                        usb_ghstd_DeviceInfo[ptr->ip][usb_ghstd_DeviceAddr[ptr->ip]][0] = rootport; /* root port */
                        driver->rootport    = rootport;
                        driver->devaddr     = usb_ghstd_DeviceAddr[ptr->ip];
                        flg = 1;    /* break; */
                    }
                }
                break;
            /* Descriptor error */
            case USB_ERROR:
                USB_PRINTF0("### Enumeration is stoped(ClassCode-ERROR)\n");
                /* Attach Detect Mode */
                enume_mode = USB_DEVICEERR;
                break;
            default:
                /* Attach Detect Mode */
                enume_mode = USB_NONDEVICE;
                break;
            }
            break;
        /* Class Check Result */
        case 5:
            switch(usb_ghstd_CheckEnuResult[ptr->ip])
            {
            case    USB_DONE:
                driver  = &usb_ghstd_DeviceDrv[ptr->ip][usb_shstd_RegPointer];
                usb_ghstd_DeviceInfo[ptr->ip][usb_ghstd_DeviceAddr[ptr->ip]][0] = rootport; /* Root port */
                driver->rootport    = rootport;
                driver->devaddr     = usb_ghstd_DeviceAddr[ptr->ip];
                break;
            case    USB_ERROR:
                enume_mode = USB_DEVICEERR;
                break;
            default:
                enume_mode = USB_NONDEVICE;
                break;
            }
            break;
        /* Set Configuration */
        case 6:
            /* Device enumeration function */
            if( usb_hstd_EnumFunction2(&enume_mode) == USB_YES )
            {
                USB_PRINTF0(" Configured Device\n");
                for( md = 0; md < usb_ghstd_DeviceNum[ptr->ip]; md++ )
                {
                    driver = &usb_ghstd_DeviceDrv[ptr->ip][md];
                    if( usb_ghstd_DeviceAddr[ptr->ip] == driver->devaddr )
                    {
                        /* Device state */
                        usb_ghstd_DeviceInfo[ptr->ip][usb_ghstd_DeviceAddr[ptr->ip]][1] = USB_CONFIGURED;
                        /* Device speed */
                        usb_ghstd_DeviceInfo[ptr->ip][usb_ghstd_DeviceAddr[ptr->ip]][4] = usb_ghstd_DeviceSpeed[ptr->ip];
                        /* Device state */
                        driver->devstate        = USB_CONFIGURED;
                        (*driver->devconfig)(ptr, usb_ghstd_DeviceAddr[ptr->ip], (uint16_t)USB_NO_ARG); /* Call Back */
                        return (USB_COMPLETEPIPESET);
                    }
                }
                enume_mode = USB_COMPLETEPIPESET;
            }
            break;
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */

        default:
            break;
        }
        usb_ghstd_EnumSeq[ptr->ip]++;
        /* Device Enumeration */
        if( enume_mode == USB_DEVICEENUMERATION )
        {
            switch( usb_ghstd_EnumSeq[ptr->ip] )
            {
            case 1:
                (*usb_ghstd_EnumarationProcess[1])(ptr, (uint16_t)USB_DEVICE_0, usb_ghstd_DeviceAddr[ptr->ip]);
                break;
            case 5:
                break;
            case 6:
                descriptor_table = (uint8_t*)usb_ghstd_ConfigurationDescriptor;
                /* Device state */
                usb_ghstd_DeviceInfo[ptr->ip][usb_ghstd_DeviceAddr[ptr->ip]][2] = descriptor_table[5];
                (*usb_ghstd_EnumarationProcess[6])(ptr, usb_ghstd_DeviceAddr[ptr->ip], (uint16_t)(descriptor_table[5]));
                break;
            default:
                (*usb_ghstd_EnumarationProcess[usb_ghstd_EnumSeq[ptr->ip]])(ptr, 
                    usb_ghstd_DeviceAddr[ptr->ip], usb_ghstd_EnumSeq[ptr->ip]);
                break;
            }
        }
        break;
    case USB_DATA_ERR:
        USB_PRINTF0("### Enumeration is stoped(SETUP or DATA-ERROR)\n");
        usb_hstd_EnumerationErr(usb_ghstd_EnumSeq[ptr->ip]);
        break;
    case USB_DATA_OVR:
        USB_PRINTF0("### Enumeration is stoped(receive data over)\n");
        usb_hstd_EnumerationErr(usb_ghstd_EnumSeq[ptr->ip]);
        break;
    case USB_DATA_STALL:
        USB_PRINTF0("### Enumeration is stoped(SETUP or DATA-STALL)\n");
        usb_hstd_EnumerationErr(usb_ghstd_EnumSeq[ptr->ip]);
        /* Device enumeration function */
        usb_hstd_EnumFunction4(&usb_ghstd_EnumSeq[ptr->ip], &enume_mode, usb_ghstd_DeviceAddr[ptr->ip]);
        break;
    default:
        USB_PRINTF0("### Enumeration is stoped(result error)\n");
        usb_hstd_EnumerationErr(usb_ghstd_EnumSeq[ptr->ip]);
        break;
    }
    return (enume_mode);
}
/******************************************************************************
End of function usb_hstd_Enumeration
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_EnumerationErr
Description     : Output error information when enumeration error occurred.
Argument        : uint16_t Rnum             : enumeration sequence
Return          : none
******************************************************************************/
void usb_hstd_EnumerationErr(uint16_t Rnum)
{
/* Condition compilation by the difference of useful function */
#ifdef USB_DEBUGPRINT_PP
    switch( Rnum ) 
    {
    case 0:     USB_PRINTF0(" Get_DeviceDescrip(8)\n");     break;
    case 1:     USB_PRINTF0(" Set_Address\n");              break;
    case 2:     USB_PRINTF0(" Get_DeviceDescrip(18)\n");    break;
    case 3:     USB_PRINTF0(" Get_ConfigDescrip(9)\n");     break;
    case 4:     USB_PRINTF0(" Get_ConfigDescrip(xx)\n");    break;
    /* Device enumeration function */
    case 5:     usb_hstd_EnumFunction5();                   break;
    case 6:     USB_PRINTF0(" Set_Configuration\n");        break;
    default:    break;
    }
#endif  /* USB_DEBUGPRINT_PP */
}
/******************************************************************************
End of function usb_hstd_EnumerationErr
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_ChkDeviceClass
Description     : Interface class search
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : USB_HCDREG_t *driver      : Class driver
                : uint16_t port             : Port no.
Return          : uint16_t                  : USB_DONE / USB_ERROR
******************************************************************************/
uint16_t usb_hstd_ChkDeviceClass(USB_UTR_t *ptr, USB_HCDREG_t *driver, uint16_t port)
{
    uint8_t     *descriptor_table;
    uint16_t    total_length1, total_length2, device_id, id_check, i;
    uint16_t    result, hub_device;
    uint16_t    *table[8];
    uint16_t    tmp4, tmp5, tmp6;

    descriptor_table = (uint8_t*)usb_ghstd_DeviceDescriptor;

    /* Device class check */
    tmp4 = descriptor_table[USB_DEV_B_DEVICE_CLASS];
    tmp5 = descriptor_table[USB_DEV_B_DEVICE_SUBCLASS];
    tmp6 = descriptor_table[USB_DEV_B_DEVICE_PROTOCOL];
    hub_device = USB_DONE;
    if( ((tmp4 == 0xFF) && (tmp5 == 0xFF)) && (tmp6 == 0xFF) )
    {
        USB_PRINTF0("*** Vendor specific device.\n\n");
    }
    else if( ((tmp4 == USB_IFCLS_HUB) && (tmp5 == 0x00)) && (tmp6 == 0x00) )
    {
        USB_PRINTF0("*** Full-Speed HUB device.\n\n");
        hub_device = USB_FSHUB;
    } 
    else if( ((tmp4 == USB_IFCLS_HUB) && (tmp5 == 0x00)) && (tmp6 == 0x01) )
    {
        USB_PRINTF0("*** High-Speed single TT device.\n\n");
        hub_device = USB_HSHUBS;
    }
    else if( ((tmp4 == USB_IFCLS_HUB) && (tmp5 == 0x00)) && (tmp6 == 0x02) )
    {
        USB_PRINTF0("*** High-Speed multiple TT device.\n\n");
        hub_device = USB_HSHUBM;
    }
    else if( ((tmp4 != 0) || (tmp5 != 0)) || (tmp6 != 0) )
    {
        USB_PRINTF0("### Device class information error!\n\n");
    }
    else
    {
    }

    if( driver->ifclass != USB_INTFCLSHET )
    {
        for( i = 0, id_check = USB_ERROR; i < driver->tpl[0]; i++ )
        {
            device_id = (uint16_t)(descriptor_table[USB_DEV_ID_VENDOR_L]
                        + ((uint16_t)descriptor_table[USB_DEV_ID_VENDOR_H] << 8));
            if( (driver->tpl[(i * 2) + 2] == USB_NOVENDOR) || (driver->tpl[(i * 2) + 2] == device_id) )
            {
                device_id = (uint16_t)(descriptor_table[USB_DEV_ID_PRODUCT_L]
                            + ((uint16_t)descriptor_table[USB_DEV_ID_PRODUCT_H] << 8));
                if( (driver->tpl[(i * 2) + 3] == USB_NOPRODUCT) || (driver->tpl[(i * 2) + 3] == device_id) )
                {
                    id_check = USB_DONE;
                }
            }
        }
        if( id_check == USB_ERROR )
        {
            USB_PRINTF0("### Not support device\n");
            device_id = (uint16_t)(descriptor_table[USB_DEV_ID_VENDOR_L]
                        + ((uint16_t)descriptor_table[USB_DEV_ID_VENDOR_H] << 8));
            USB_PRINTF1(" VID=0x%04x, ",device_id);
            device_id = (uint16_t)(descriptor_table[USB_DEV_ID_PRODUCT_L]
                        + ((uint16_t)descriptor_table[USB_DEV_ID_PRODUCT_H] << 8));
            USB_PRINTF1(" PID=0x%04x, ",device_id);
            return USB_ERROR;
        }
    }

    descriptor_table = (uint8_t*)usb_ghstd_ConfigurationDescriptor;
    total_length1 = 0;
    total_length2 = (uint16_t)(descriptor_table[USB_DEV_W_TOTAL_LENGTH_L]
                    + ((uint16_t)descriptor_table[USB_DEV_W_TOTAL_LENGTH_H] << 8));
    if( total_length2 > USB_CONFIGSIZE )
    {
        total_length2 = USB_CONFIGSIZE;
    }

    /* Search within configuration total-length */
    while( total_length1 < total_length2 )
    {               
        switch( descriptor_table[total_length1 + 1] )
        {
        /* Configuration Descriptor ? */
        case USB_DT_CONFIGURATION:
            table[1] = (uint16_t*)&descriptor_table[total_length1];
            break;
        /* Interface Descriptor ? */
        case USB_DT_INTERFACE:
            if( (driver->ifclass == (uint16_t)USB_INTFCLSHET)
                || (driver->ifclass == (uint16_t)descriptor_table[total_length1 + 5]) )
            {
                    result = USB_ERROR;
                    table[0]    = (uint16_t*)&usb_ghstd_DeviceDescriptor;
                    table[2]    = (uint16_t*)&descriptor_table[total_length1];
                    table[3]    = &result;
                    table[4]    = &hub_device;
                    table[5]    = &port;
                    table[6]    = &usb_ghstd_DeviceSpeed[ptr->ip];
                    table[7]    = &usb_ghstd_DeviceAddr[ptr->ip];
                    (*driver->classcheck)(ptr, (uint16_t**)&table);
                    /* Interface Class */
                    usb_ghstd_DeviceInfo[ptr->ip][usb_ghstd_DeviceAddr[ptr->ip]][3]
                        = descriptor_table[total_length1 + 5];
                    return result;
            }
/*          USB_PRINTF2("*** Interface class is 0x%02x (not 0x%02x)\n", 
                            descriptor_table[total_length1 + 5], driver->ifclass);*/
            break;
        default:
            break;
        }
        total_length1 += descriptor_table[total_length1];
        if( descriptor_table[total_length1] == 0 )
        {
            break;
        }
    }
    return USB_ERROR;
}
/******************************************************************************
End of function usb_hstd_ChkDeviceClass
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_NotifAtorDetach
Description     : Notify MGR (manager) task that attach or detach occurred.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t result   : Result.
                : uint16_t port     : Port no.
Return          : none
******************************************************************************/
void usb_hstd_NotifAtorDetach(USB_UTR_t *ptr, uint16_t result, uint16_t port)
{
    usb_hstd_MgrSndMbx(ptr, (uint16_t)USB_MSG_MGR_AORDETACH, port, result);
}
/******************************************************************************
End of function usb_hstd_NotifAtorDetach
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_OvcrNotifiation
Description     : Notify MGR (manager) task that overcurrent was generated
Argument        : uint16_t port     : Port no.
Return          : none
******************************************************************************/
void usb_hstd_OvcrNotifiation(USB_UTR_t *ptr, uint16_t port)
{
    usb_hstd_MgrSndMbx(ptr, (uint16_t)USB_MSG_MGR_OVERCURRENT, port, (uint16_t)0u);
}
/******************************************************************************
End of function usb_hstd_OvcrNotifiation
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_StatusResult
Description     : This is a callback as a result of calling 
                : R_usb_hstd_ChangeDeviceState. This notifies the MGR (manager) 
                : task that the change of USB Device status completed.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t port     : Port no.
                : uint16_t result   : Result.
Return          : none
******************************************************************************/
void usb_hstd_StatusResult(USB_UTR_t *ptr, uint16_t port, uint16_t result)
{
    usb_hstd_MgrSndMbx(ptr, (uint16_t)USB_MSG_MGR_STATUSRESULT, port, result);
}
/******************************************************************************
End of function usb_hstd_StatusResult
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_SubmitResult
Description     : Callback after completion of a standard request.
Argument        : uint16_t *utr_table   : Message.
Return          : none
******************************************************************************/
void usb_hstd_SubmitResult(USB_UTR_t *ptr, uint16_t data1, uint16_t data2)
{
    usb_hstd_MgrSndMbx(ptr, (uint16_t)USB_MSG_MGR_SUBMITRESULT, ptr->keyword, ptr->status);
}
/******************************************************************************
End of function usb_hstd_SubmitResult
******************************************************************************/

#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
/******************************************************************************
Function Name   : usb_hstd_SetConfigResult
Description     : Notify MGR (manager) task that SetConfiguration for the connected USB Device is completed
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t *utr_table       : message
Return          : none
******************************************************************************/
void usb_hstd_SetConfigResult(USB_UTR_t *ptr, uint16_t *utr_table)
{
    USB_UTR_t       *table;

    table = (USB_UTR_t*)utr_table;

    usb_hstd_MgrSndMbx(ptr, (uint16_t)USB_MSG_MGR_SETCONFIGURATION_RESULT, table->keyword, table->status);
}
/******************************************************************************
End of function usb_hstd_SetConfigResult
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_GetDescriptorResult
Description     : Notify MGR (manager) task that GetDescriptor for the connected 
                : USB Device completed.
Argument        : uint16_t *utr_table       : Message.
Return          : none
******************************************************************************/
void usb_hstd_GetDescriptorResult(USB_UTR_t *ptr, uint16_t *utr_table)
{
    USB_UTR_t       *table;

    table = (USB_UTR_t*)utr_table;

    usb_hstd_MgrSndMbx(ptr, (uint16_t)USB_MSG_MGR_GETDESCRIPTOR_RESULT, table->keyword, table->status);
}
/******************************************************************************
End of function usb_hstd_GetDescriptorResult
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_EnumGetDescriptorAnsi
Description     : Send GetDescriptor to the connected USB Device. (Called only 
                : when using ANSI.)
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : IP Information. (Mode, IP no., Reg Address.)
                : uint8_t addr      : Device address.
                : uint8_t cnt_value : Descriptor Type.
Return          : none
******************************************************************************/
void usb_hstd_EnumGetDescriptorAnsi(USB_UTR_t *ptr, uint8_t addr, uint8_t cnt_value)
{
    uint8_t         *data_table;

    switch( cnt_value )
    {
    case 0:
        /* continue */
    case 1:
        /* continue */
    case 5:
        usb_shstd_StdRequest[0] = USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_STANDARD | USB_DEVICE;
        usb_shstd_StdRequest[1] = (uint16_t)USB_DEV_DESCRIPTOR;
        usb_shstd_StdRequest[2] = (uint16_t)0x0000;
        usb_shstd_StdRequest[3] = (uint16_t)0x0040;
        if( usb_shstd_StdRequest[3] > USB_DEVICESIZE )
        {
            usb_shstd_StdRequest[3] = USB_DEVICESIZE;
        }
        usb_shstd_StdReqMsg.tranadr = usb_ghstd_DeviceDescriptor;
        break;
    case 2:
        usb_shstd_StdRequest[0] = USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_STANDARD | USB_DEVICE;
        usb_shstd_StdRequest[1] = (uint16_t)USB_DEV_DESCRIPTOR;
        usb_shstd_StdRequest[2] = (uint16_t)0x0000;
        usb_shstd_StdRequest[3] = (uint16_t)0x0012;
        if( usb_shstd_StdRequest[3] > USB_DEVICESIZE )
        {
            usb_shstd_StdRequest[3] = USB_DEVICESIZE;
        }
        usb_shstd_StdReqMsg.tranadr = usb_ghstd_DeviceDescriptor;
        break;
    case 3:
        usb_shstd_StdRequest[0] = USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_STANDARD | USB_DEVICE;
        usb_shstd_StdRequest[1] = (uint16_t)USB_CONF_DESCRIPTOR;
        usb_shstd_StdRequest[2] = (uint16_t)0x0000;
        usb_shstd_StdRequest[3] = (uint16_t)0x0009;
        usb_shstd_StdReqMsg.tranadr = usb_ghstd_ConfigurationDescriptor;
        break;
    case 4:
        data_table = (uint8_t*)usb_ghstd_ConfigurationDescriptor;
        usb_shstd_StdRequest[0] = USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_STANDARD | USB_DEVICE;
        usb_shstd_StdRequest[1] = (uint16_t)USB_CONF_DESCRIPTOR;
        usb_shstd_StdRequest[2] = (uint16_t)0x0000;
        usb_shstd_StdRequest[3] = (uint16_t)(((uint16_t)data_table[3] << 8) + (uint16_t)data_table[2]);
        if( usb_shstd_StdRequest[3] > USB_CONFIGSIZE )
        {
            usb_shstd_StdRequest[3] = USB_CONFIGSIZE;
            USB_PRINTF0("***WARNING Descriptor size over !\n");
        }
        usb_shstd_StdReqMsg.tranadr = usb_ghstd_ConfigurationDescriptor;
        break;
    default:
        return;
        break;
    }
    usb_shstd_StdRequest[4] = addr;
    usb_shstd_StdReqMsg.keyword     = (uint16_t)USB_PIPE0;
    usb_shstd_StdReqMsg.tranlen     = (uint32_t)usb_shstd_StdRequest[3];
    usb_shstd_StdReqMsg.setup       = usb_shstd_StdRequest;
    usb_shstd_StdReqMsg.status      = USB_DATA_NONE;
    usb_shstd_StdReqMsg.complete    = (USB_CB_t)&usb_hstd_GetDescriptorResult;
    usb_shstd_StdReqMsg.segment     = USB_TRAN_END;

    usb_shstd_StdReqMsg.ipp         = ptr->ipp;
    usb_shstd_StdReqMsg.ip          = ptr->ip;

    usb_hstd_TransferStart(&usb_shstd_StdReqMsg);
}
/******************************************************************************
End of function usb_hstd_EnumGetDescriptorAnsi
******************************************************************************/

#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */

/******************************************************************************
Function Name   : usb_hstd_TransferEndResult
Description     : Notify the MGR (manager) task that force-termination of data 
                : transfer completed.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t Result   : Transfer result
                : uint16_t Pipe             : Pipe No
Return          : none
******************************************************************************/
void usb_hstd_TransferEndResult(USB_UTR_t *ptr, uint16_t result, uint16_t pipe)
{
    usb_hstd_MgrSndMbx(ptr, (uint16_t)USB_MSG_MGR_TRANSENDRESULT, pipe, result);
}
/******************************************************************************
End of function usb_hstd_TransferEndResult
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_EnumGetDescriptor
Description     : Send GetDescriptor to the connected USB device.
Argument        : uint16_t addr             : Device Address
                : uint16_t cnt_value        : Enumeration sequence
Return          : none
******************************************************************************/
void usb_hstd_EnumGetDescriptor(USB_UTR_t *ptr, uint16_t addr, uint16_t cnt_value)
{
    uint8_t         *data_table;

    switch( cnt_value )
    {
    case 0:
        /* continue */
    case 1:
        /* continue */
    case 5:
        usb_shstd_StdRequest[0] = USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_STANDARD | USB_DEVICE;
        usb_shstd_StdRequest[1] = (uint16_t)USB_DEV_DESCRIPTOR;
        usb_shstd_StdRequest[2] = (uint16_t)0x0000;
        usb_shstd_StdRequest[3] = (uint16_t)0x0040;
        if( usb_shstd_StdRequest[3] > USB_DEVICESIZE )
        {
            usb_shstd_StdRequest[3] = USB_DEVICESIZE;
        }
        usb_shstd_StdReqMsg.tranadr = usb_ghstd_DeviceDescriptor;
        break;
    case 2:
        usb_shstd_StdRequest[0] = USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_STANDARD | USB_DEVICE;
        usb_shstd_StdRequest[1] = (uint16_t)USB_DEV_DESCRIPTOR;
        usb_shstd_StdRequest[2] = (uint16_t)0x0000;
        usb_shstd_StdRequest[3] = (uint16_t)0x0012;
        if( usb_shstd_StdRequest[3] > USB_DEVICESIZE )
        {
            usb_shstd_StdRequest[3] = USB_DEVICESIZE;
        }
        usb_shstd_StdReqMsg.tranadr = usb_ghstd_DeviceDescriptor;
        break;
    case 3:
        usb_shstd_StdRequest[0] = USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_STANDARD | USB_DEVICE;
        usb_shstd_StdRequest[1] = (uint16_t)USB_CONF_DESCRIPTOR;
        usb_shstd_StdRequest[2] = (uint16_t)0x0000;
        usb_shstd_StdRequest[3] = (uint16_t)0x0009;
        usb_shstd_StdReqMsg.tranadr = usb_ghstd_ConfigurationDescriptor;
        break;
    case 4:
        data_table = (uint8_t*)usb_ghstd_ConfigurationDescriptor;
        usb_shstd_StdRequest[0] = USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_STANDARD | USB_DEVICE;
        usb_shstd_StdRequest[1] = (uint16_t)USB_CONF_DESCRIPTOR;
        usb_shstd_StdRequest[2] = (uint16_t)0x0000;
        usb_shstd_StdRequest[3] = (uint16_t)(((uint16_t)data_table[3] << 8) + (uint16_t)data_table[2]);
        if( usb_shstd_StdRequest[3] > USB_CONFIGSIZE )
        {
            usb_shstd_StdRequest[3] = USB_CONFIGSIZE;
            USB_PRINTF0("***WARNING Descriptor size over !\n");
        }
        usb_shstd_StdReqMsg.tranadr = usb_ghstd_ConfigurationDescriptor;
        break;
    default:
        return;
        break;
    }
    usb_shstd_StdRequest[4] = addr;
    usb_shstd_StdReqMsg.keyword     = (uint16_t)USB_PIPE0;
    usb_shstd_StdReqMsg.tranlen     = (uint32_t)usb_shstd_StdRequest[3];
    usb_shstd_StdReqMsg.setup       = usb_shstd_StdRequest;
    usb_shstd_StdReqMsg.status      = USB_DATA_NONE;
    usb_shstd_StdReqMsg.complete    = (USB_CB_t)&usb_hstd_SubmitResult;
    usb_shstd_StdReqMsg.segment     = USB_TRAN_END;

    usb_shstd_StdReqMsg.ipp         = ptr->ipp;
    usb_shstd_StdReqMsg.ip          = ptr->ip;

    usb_hstd_TransferStart(&usb_shstd_StdReqMsg);
}
/******************************************************************************
End of function usb_hstd_EnumGetDescriptor
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_EnumSetAddress
Description     : Send SetAddress to the connected USB device.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t addr     : Device Address.
                : uint16_t setaddr  : New address.
Return          : none
******************************************************************************/
void usb_hstd_EnumSetAddress(USB_UTR_t *ptr, uint16_t addr, uint16_t setaddr)
{
    usb_shstd_StdRequest[0] = USB_SET_ADDRESS | USB_HOST_TO_DEV | USB_STANDARD | USB_DEVICE;
    usb_shstd_StdRequest[1] = setaddr;
    usb_shstd_StdRequest[2] = (uint16_t)0x0000;
    usb_shstd_StdRequest[3] = (uint16_t)0x0000;
    usb_shstd_StdRequest[4] = addr;
    usb_shstd_StdReqMsg.keyword     = (uint16_t)USB_PIPE0;
    usb_shstd_StdReqMsg.tranadr     = (void *)&usb_shstd_DummyData;
    usb_shstd_StdReqMsg.tranlen     = (uint32_t)usb_shstd_StdRequest[3];
    usb_shstd_StdReqMsg.setup       = usb_shstd_StdRequest;
    usb_shstd_StdReqMsg.status      = USB_DATA_NONE;
    usb_shstd_StdReqMsg.complete    = (USB_CB_t)&usb_hstd_SubmitResult;
    usb_shstd_StdReqMsg.segment     = USB_TRAN_END;

    usb_shstd_StdReqMsg.ipp         = ptr->ipp;
    usb_shstd_StdReqMsg.ip          = ptr->ip;

    usb_hstd_TransferStart(&usb_shstd_StdReqMsg);
}
/******************************************************************************
End of function usb_hstd_EnumSetAddress
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_EnumSetConfiguration
Description     : Send SetConfiguration to the connected USB device.
Argument        : uint16_t addr     : Device Address.
                : uint16_t confnum  : Configuration number.
Return          : none
******************************************************************************/
void usb_hstd_EnumSetConfiguration(USB_UTR_t *ptr, uint16_t addr, uint16_t confnum)
{
    usb_shstd_StdRequest[0] = USB_SET_CONFIGURATION | USB_HOST_TO_DEV | USB_STANDARD | USB_DEVICE;
    usb_shstd_StdRequest[1] = confnum;
    usb_shstd_StdRequest[2] = (uint16_t)0x0000;
    usb_shstd_StdRequest[3] = (uint16_t)0x0000;
    usb_shstd_StdRequest[4] = addr;
    usb_shstd_StdReqMsg.keyword     = (uint16_t)USB_PIPE0;
    usb_shstd_StdReqMsg.tranadr     = (void *)&usb_shstd_DummyData;
    usb_shstd_StdReqMsg.tranlen     = (uint32_t)usb_shstd_StdRequest[3];
    usb_shstd_StdReqMsg.setup       = usb_shstd_StdRequest;
    usb_shstd_StdReqMsg.status      = USB_DATA_NONE;
#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
    usb_shstd_StdReqMsg.complete    = (USB_CB_t)&usb_hstd_SetConfigResult;
#else   /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */
    usb_shstd_StdReqMsg.complete    = (USB_CB_t)&usb_hstd_SubmitResult;
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */
    usb_shstd_StdReqMsg.segment     = USB_TRAN_END;

    usb_shstd_StdReqMsg.ipp         = ptr->ipp;
    usb_shstd_StdReqMsg.ip          = ptr->ip;

    usb_hstd_TransferStart(&usb_shstd_StdReqMsg);
}
/******************************************************************************
End of function usb_hstd_EnumSetConfiguration
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_EnumDummyRequest
Description     : Dummy function.
Argument        : uint16_t addr             : Device Address
                : uint16_t cnt_value        : Enumeration Sequence
Return          : none
******************************************************************************/
void usb_hstd_EnumDummyRequest(USB_UTR_t *ptr, uint16_t addr, uint16_t cnt_value)
{
}
/******************************************************************************
End of function usb_hstd_EnumDummyRequest
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_MgrSuspend
Description     : Suspend request
Argument        : uint16_t info     : Info for release of memory block.
Return          : none
******************************************************************************/
void usb_hstd_MgrSuspend(USB_UTR_t *ptr, uint16_t info)
{
    uint16_t        rootport, devaddr, devsel;
    uint16_t        j;

    devaddr     = usb_shstd_MgrMsg->keyword;
    devsel      = (uint16_t)(devaddr << USB_DEVADDRBIT);
    /* Get root port number from device addr */
    rootport    = usb_hstd_GetRootport(ptr, devsel);

    if( usb_hstd_ChkDevAddr(ptr, devsel, rootport) != USB_NOCONNECT )
    {
        /* PIPE suspend */
        for( j = USB_MIN_PIPE_NO; j <= USB_MAX_PIPE_NO; j++ )
        {
            /* Agreement device address */
            if( usb_cstd_GetDevsel(ptr, j) == devsel )
            {
                /* PID=BUF ? */
                if( usb_cstd_GetPid(ptr, j) == USB_PID_BUF )
                {
                    usb_cstd_SetNak(ptr, j);
                    usb_ghstd_SuspendPipe[j] = USB_SUSPENDED;
                }
            }
        }
        usb_shstd_SuspendSeq=0;
        usb_hstd_SuspCont(ptr, (uint16_t)devaddr, (uint16_t)rootport);
        usb_ghstd_MgrMode[ptr->ip][rootport] = USB_SUSPENDED_PROCESS;
    }
    usb_hstd_MgrRelMpl(info);
}
/******************************************************************************
End of function usb_hstd_MgrSuspend
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_DeviceStateControl
Description     : Setup a call to the function usb_hstd_ChangeDeviceState to re-
                : quest the connected USB Device to change status.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t devaddr      : Device address.
                : uint16_t msginfo      : Request type.
Return          : none
******************************************************************************/
void usb_hstd_DeviceStateControl(USB_UTR_t *ptr, uint16_t devaddr, uint16_t msginfo)
{
    switch( devaddr )
    {
    case 0:
        USB_PRINTF0("### usbd_message device address error\n");
        break;
    case USB_DEVICEADDR:
        usb_hstd_ChangeDeviceState(ptr, &usb_hstd_StatusResult, msginfo, (uint16_t)USB_PORT0);
        break;
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
    case (USB_DEVICEADDR + 1u):
        usb_hstd_ChangeDeviceState(ptr, &usb_hstd_StatusResult, msginfo, (uint16_t)USB_PORT1);
        break;
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
    default:
        if( USB_HUBDPADDR <= devaddr )
        {
        }
        break;
    }
}
/******************************************************************************
End of function usb_hstd_DeviceStateControl
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_DeviceStateControl2
Description     : Setup a call to the function usb_hstd_ChangeDeviceState to re-
                : quest the connected USB Device to change status.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : USB_CB_INFO_t complete    : Callback function Pointer
                : uint16_t devaddr          : Device address
                : uint16_t msginfo          : Request type for HCD
                : uint16_t mgr_msginfo      : Request type for MGR
Return          : none
******************************************************************************/
void usb_hstd_DeviceStateControl2(  USB_UTR_t *ptr, USB_CB_INFO_t complete, 
                                    uint16_t devaddr, uint16_t msginfo,
                                    uint16_t mgr_msginfo)
{
    usb_shstd_mgr_callback = complete;
    usb_shstd_mgr_msginfo = mgr_msginfo;
    usb_hstd_DeviceStateControl(ptr, devaddr, msginfo);
}
/******************************************************************************
End of function usb_hstd_DeviceStateControl2
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_MgrReset
Description     : Request HCD (Host Control Driver) to do a USB bus reset.
Argument        : uint16_t devaddr      : Device address.
Return          : none
******************************************************************************/
void usb_hstd_MgrReset(USB_UTR_t *ptr, uint16_t addr)
{
    usb_hstd_DeviceStateControl(ptr, addr, (uint16_t)USB_MSG_HCD_USBRESET);
    if( addr == USB_DEVICEADDR )
    {
        usb_ghstd_MgrMode[ptr->ip][USB_PORT0] = USB_DEFAULT;
    }
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
    else if( addr == (USB_DEVICEADDR + 1u) )
    {
        usb_ghstd_MgrMode[ptr->ip][USB_PORT1] = USB_DEFAULT;
    }
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
    else
    {
    }
}
/******************************************************************************
End of function usb_hstd_MgrReset
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_MgrResume
Description     : Request HCD (Host Control Device) to send RESUME signal.
Argument        : uint16_t info : Information.
Return          : none
******************************************************************************/
void usb_hstd_MgrResume(USB_UTR_t *ptr, uint16_t info)
{
    uint16_t        rootport, devaddr, devsel;

    devaddr     = usb_shstd_MgrMsg->keyword;
    devsel      = (uint16_t)(devaddr << USB_DEVADDRBIT);
    /* Get root port number from device addr */
    rootport    = usb_hstd_GetRootport(ptr, devsel);
    if( usb_hstd_ChkDevAddr(ptr, devsel, rootport) != USB_NOCONNECT )
    {
        /* Device resume */
        usb_hstd_DeviceStateControl(ptr, devaddr, usb_shstd_MgrMsg->msginfo);
        usb_ghstd_MgrMode[ptr->ip][rootport] = USB_RESUME_PROCESS;
        usb_shstd_ResumeSeq = 0;
    }
    usb_hstd_MgrRelMpl(info);
}
/******************************************************************************
End of function usb_hstd_MgrResume
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_MgrTask
Description     : The host manager (MGR) task.
Argument        : USB_VP_INT stacd          : Task Start Code
Return          : none
******************************************************************************/
void usb_hstd_MgrTask(USB_VP_INT stacd)
{
    USB_UTR_t       *mess, *ptr;
    USB_ER_t        err;
    USB_HCDREG_t    *driver;
    USB_HCDINFO_t   *hp;
    uint16_t        rootport, devaddr, devsel, pipenum, msginfo;
    uint16_t        md;
    uint16_t        enume_mode; /* Enumeration mode (device state) */
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
    uint16_t        elseport;
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */

    /* Receive message */
    err = USB_TRCV_MSG(USB_MGR_MBX, (USB_MSG_t**)&mess, (USB_TM_t)10000);
    if ( (err != USB_E_OK) && (err != USB_E_TMOUT) )
    {
        return;
    }
    else
    {
        usb_shstd_MgrMsg    = (USB_MGRINFO_t*)mess;
        rootport    = usb_shstd_MgrMsg->keyword;
        devaddr     = usb_shstd_MgrMsg->keyword;
        pipenum     = usb_shstd_MgrMsg->keyword;
        devsel      = (uint16_t)(devaddr << USB_DEVADDRBIT);
        hp          = (USB_HCDINFO_t*)mess;
        ptr         = mess;

        /* Detach is all device */
        msginfo = usb_shstd_MgrMsg->msginfo;
        switch( usb_shstd_MgrMsg->msginfo )
        {
        /* USB-bus control (change device state) */
        case USB_MSG_MGR_STATUSRESULT:
            switch( usb_ghstd_MgrMode[ptr->ip][rootport] )
            {
            /* End of reset signal */
            case USB_DEFAULT:
                usb_ghstd_DeviceSpeed[ptr->ip] = usb_shstd_MgrMsg->result;
                /* Set device speed */
                usb_hstd_SetDevAddr(ptr, (uint16_t)USB_DEVICE_0, usb_ghstd_DeviceSpeed[ptr->ip], rootport);
                usb_ghstd_DcpRegister[ptr->ip][0] = (uint16_t)(USB_DEFPACKET + USB_DEVICE_0);
                usb_ghstd_EnumSeq[ptr->ip] = 0;
                switch( usb_ghstd_DeviceSpeed[ptr->ip] )
                {
                case USB_HSCONNECT: /* Hi Speed Device Connect */
                    USB_PRINTF0(" Hi-Speed Device\n");
                    (*usb_ghstd_EnumarationProcess[0])(ptr, (uint16_t)USB_DEVICE_0, (uint16_t)0);
                    break;
                case USB_FSCONNECT: /* Full Speed Device Connect */
                    USB_PRINTF0(" Full-Speed Device\n");
                    (*usb_ghstd_EnumarationProcess[0])(ptr, (uint16_t)USB_DEVICE_0, (uint16_t)0);
                    break;
                case USB_LSCONNECT: /* Low Speed Device Connect */
                    USB_PRINTF0(" Low-Speed Device\n");
                    usb_hstd_LsConnectFunction(ptr);
                    break;
                default:
                    USB_PRINTF0(" Device/Detached\n");
                    usb_ghstd_MgrMode[ptr->ip][rootport] = USB_DETACHED;
                    break;
                }
                break;
            /* End of resume signal */
            case USB_CONFIGURED:
                /* This Resume Sorce is moved to usb_hResuCont() by nonOS */
                break;
            /* Start of suspended state */
            case USB_SUSPENDED:
                for( md = 0; md < usb_ghstd_DeviceNum[ptr->ip]; md++ )
                {
                    driver = &usb_ghstd_DeviceDrv[ptr->ip][md];
                    if( driver->devaddr == (rootport + USB_DEVICEADDR) )
                    {
                        (*driver->devsuspend)(ptr, (uint16_t)USB_NO_ARG, (uint16_t)USB_NO_ARG);

                        if( usb_shstd_mgr_msginfo == USB_DO_GLOBAL_SUSPEND )
                        {
                            usb_hstd_mgr_chgdevst_cb(ptr, rootport);
                        }
                        /* Device state */
                        usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][1] = USB_SUSPENDED;
                        /* Device state */
                        driver->devstate = USB_SUSPENDED;
                    }
                }
                break;
            /* Continue of resume signal */
            case    USB_RESUME_PROCESS:
                /* Resume Sequence Number is 0 */
                usb_hstd_ResuCont(ptr, (uint16_t)(USB_DEVICEADDR + rootport), (uint16_t)rootport);
                break;

            case    USB_DETACHED:
                switch( usb_shstd_mgr_msginfo )
                {
                case    USB_GO_POWEREDSTATE:
                case    USB_PORT_DISABLE:
                    usb_hstd_mgr_chgdevst_cb(ptr, rootport);
                    break;
                default:
                    break;
                }
                break;

            default:
                break;
            }
            usb_hstd_MgrRelMpl(msginfo);
            break;

#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
        case    USB_MSG_MGR_SETCONFIGURATION:
            devaddr     = (uint8_t)(ptr->keyword >> 8);
            devsel      = (uint16_t)(devaddr << USB_DEVADDRBIT);
            rootport    = usb_hstd_GetRootport( ptr, devsel );
            if( usb_ghstd_MgrMode[ptr->ip][rootport] == USB_DEFAULT )
            {
                uint16_t confnum;
                confnum = (uint8_t)(ptr->keyword & 0xff);

                usb_ghstd_DeviceInfo[ptr->ip][devaddr][2] = *((uint8_t*)usb_ghstd_ConfigurationDescriptor + 5);
                usb_shstd_mgr_callback = ptr->complete;

                usb_hstd_EnumSetConfiguration( ptr, devaddr, confnum );

            }
            usb_hstd_MgrRelMpl(msginfo);
            break;

        case    USB_MSG_MGR_SETCONFIGURATION_RESULT:
            devaddr     = usb_ghstd_DeviceAddr[ptr->ip];
            devsel      = devaddr << USB_DEVADDRBIT;
            rootport    = usb_hstd_GetRootport( ptr, devsel );

            if( ptr->status == USB_DATA_STALL )
            {
                usb_ghstd_DeviceInfo[ptr->ip][devaddr][1] = USB_DETACHED;       /* Device state */
                usb_ghstd_MgrMode[ptr->ip][rootport] = USB_DETACHED;
            }
            else
            {
                usb_ghstd_DeviceInfo[ptr->ip][devaddr][1] = USB_CONFIGURED; /* Device state */
                usb_ghstd_MgrMode[ptr->ip][rootport] = USB_CONFIGURED;
            }
            usb_ghstd_DeviceInfo[ptr->ip][devaddr][4] = usb_ghstd_DeviceSpeed[ptr->ip]; /* Device speed */


            if( USB_NULL != usb_shstd_mgr_callback )
            {
                (*usb_shstd_mgr_callback)(ptr, (uint16_t)USB_NO_ARG, ptr->status);
                usb_shstd_mgr_callback = USB_NULL;
            }
            for( md = 0; md < usb_ghstd_DeviceNum[ptr->ip]; md++ )
            {
                driver = &usb_ghstd_DeviceDrv[ptr->ip][md];
                if( usb_ghstd_DeviceAddr[ptr->ip] == driver->devaddr )
                {
                    driver->devstate = USB_CONFIGURED;
                    /* Call Back */
                    (*driver->devconfig)(ptr, usb_ghstd_DeviceAddr[ptr->ip], (uint16_t)USB_NO_ARG);
                }
            }

            usb_hstd_MgrRelMpl(msginfo);
            break;

        case USB_MSG_MGR_GETDESCRIPTOR:
            devaddr     = (uint8_t)(ptr->keyword >> 8);
            usb_shstd_mgr_callback = ptr->complete;
            /* confnum      = (uint8_t)(ptr->keyword & 0x00ff); */
            usb_hstd_EnumGetDescriptorAnsi( ptr, devaddr, (uint8_t)(ptr->keyword & 0x00ff) );
            usb_hstd_MgrRelMpl(msginfo);
            break;

        case USB_MSG_MGR_GETDESCRIPTOR_RESULT:
            if( USB_NULL != usb_shstd_mgr_callback )
            {
                (*usb_shstd_mgr_callback)(ptr, (uint16_t)USB_NO_ARG, ptr->status);
                usb_shstd_mgr_callback = USB_NULL;
            }
            usb_hstd_MgrRelMpl(msginfo);
            break;
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */

        case USB_MSG_MGR_SUBMITRESULT:
            /* Agreement device address */
            devsel      = usb_cstd_GetDevsel(ptr, pipenum);
            /* Get root port number from device addr */
            rootport    = usb_hstd_GetRootport(ptr, devsel);
            switch(usb_ghstd_MgrMode[ptr->ip][rootport])
            {
            /* Resume */
            case    USB_RESUME_PROCESS:
                /* Resume Sequence Number is 1 to 2 */
                usb_hstd_ResuCont(ptr, (uint16_t)(devsel >> USB_DEVADDRBIT), (uint16_t)rootport);
                break;
            /* Suspend */
            case    USB_SUSPENDED_PROCESS:
                usb_hstd_SuspCont(ptr, (uint16_t)(devsel >> USB_DEVADDRBIT), (uint16_t)rootport);
                break;
            /* Enumeration */
            case    USB_DEFAULT:
                enume_mode = usb_hstd_Enumeration(ptr);
                switch( enume_mode )
                {
                /* Detach Mode */
                case USB_NONDEVICE:
                    USB_PRINTF1("### Enumeration error (address%d)\n", usb_ghstd_DeviceAddr[ptr->ip]);
                    usb_ghstd_MgrMode[ptr->ip][rootport] = USB_DETACHED;

                    if( ( usb_shstd_mgr_msginfo == USB_DO_RESET_AND_ENUMERATION )
                        || ( usb_shstd_mgr_msginfo == USB_PORT_ENABLE ) )
                    {
                        usb_hstd_mgr_chgdevst_cb(ptr, rootport);
                    }
                    break;
                /* Detach Mode */
                case USB_DEVICEERR:
                    USB_PRINTF1("### Not support device (address%d)\n", usb_ghstd_DeviceAddr[ptr->ip]);
                    usb_ghstd_MgrMode[ptr->ip][rootport] = USB_DETACHED;

                    if( ( usb_shstd_mgr_msginfo == USB_DO_RESET_AND_ENUMERATION )
                        || ( usb_shstd_mgr_msginfo == USB_PORT_ENABLE ) )
                    {
                        usb_hstd_mgr_chgdevst_cb(ptr, rootport);
                    }
                    break;
                case USB_COMPLETEPIPESET:
                    usb_ghstd_MgrMode[ptr->ip][rootport] = USB_CONFIGURED;

                    if( ( usb_shstd_mgr_msginfo == USB_DO_RESET_AND_ENUMERATION )
                        || ( usb_shstd_mgr_msginfo == USB_PORT_ENABLE ) )
                    {
                        usb_hstd_mgr_chgdevst_cb(ptr, rootport);
                    }
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
            usb_hstd_MgrRelMpl(msginfo);
            break;
        case USB_MSG_MGR_AORDETACH:
            switch( usb_shstd_MgrMsg->result )
            {
            case USB_DETACH:
                USB_PRINTF1(" [Detach Device port%d] \n", rootport);
                usb_ghstd_MgrMode[ptr->ip][rootport] = USB_DETACHED;
                usb_ghstd_DeviceSpeed[ptr->ip] = USB_NOCONNECT;

                for( md = 0; md < usb_ghstd_DeviceNum[ptr->ip]; md++ )
                {
                    driver = &usb_ghstd_DeviceDrv[ptr->ip][md];
                    if( driver->devaddr == (rootport + USB_DEVICEADDR) )
                    {
                        (*driver->devdetach)(ptr, driver->devaddr, (uint16_t)USB_NO_ARG);
                        /* Root port */
                        usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][0] = USB_NOPORT;
                        /* Device state */
                        usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][1] = USB_DETACHED;
                        /* Not configured */
                        usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][2] = (uint16_t)0;
                        /* Interface Class : NO class */
                        usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][3] = (uint16_t)USB_IFCLS_NOT;
                        /* No connect */
                        usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][4] = (uint16_t)USB_NOCONNECT;
                        
                        /* Root port */
                        driver->rootport    = USB_NOPORT;
                        /* Device address */
                        driver->devaddr     = USB_NODEVICE;
                        /* Device state */
                        driver->devstate    = USB_DETACHED;
                    }
                }
                usb_hstd_MgrRelMpl(msginfo);
                break;
            case USB_ATTACHL:
                /* continue */
            case USB_ATTACHF:
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
                elseport = 0;
                if( rootport == 0 )
                {
                    elseport = 1;
                }
                if( usb_ghstd_MgrMode[ptr->ip][elseport] == USB_DEFAULT )
                {
                    /* 2ms wait */
                    usb_cpu_DelayXms((uint16_t)2);
/* Enumeration wait setting--------------- */
                    if(usb_ghstd_EnuWait[ptr->ip] != (uint8_t)USB_MGR_TSK)
                    {
                        usb_shstd_MgrMsg->msginfo = USB_MSG_CLS_WAIT;
                        usb_shstd_MgrMsg->keyword = rootport;
                        err = USB_SND_MSG( usb_ghstd_EnuWait[ptr->ip], (USB_MSG_t*)usb_shstd_MgrMsg );
                    }
                    else
                    {
                        err = USB_SND_MSG(USB_MGR_MBX, (USB_MSG_t*)usb_shstd_MgrMsg);
                    }
/* Enumeration wait setting end------------ */
                    if( err != USB_E_OK )
                    {
                        USB_PRINTF1("### hMgrTask snd_msg error (%ld)\n", err);
                    }
                }
                else if( usb_ghstd_MgrMode[ptr->ip][rootport] == USB_DETACHED )
                {
                    /* enumeration wait setting */
                    usb_ghstd_EnuWait[ptr->ip] = (uint8_t)USB_MGR_TSK;
                    usb_ghstd_DeviceAddr[ptr->ip] = (uint16_t)(rootport + USB_DEVICEADDR);
                    if( USB_MAXDEVADDR < usb_ghstd_DeviceAddr[ptr->ip] )
                    {
                        /* For port1 */
                        USB_PRINTF0("Device address error\n");
                    }
                    else
                    {
                        usb_ghstd_MgrMode[ptr->ip][rootport] = USB_DEFAULT;
                        usb_hstd_AttachFunction();
                        usb_hstd_MgrReset(ptr, usb_ghstd_DeviceAddr[ptr->ip]);
                    }
                    usb_hstd_MgrRelMpl(msginfo);
                }
#else   /* USB_PORTSEL_PP == USB_2PORT_PP */
                if( usb_ghstd_MgrMode[ptr->ip][rootport] == USB_DETACHED )
                {
                    /* enumeration wait setting */
                    usb_ghstd_EnuWait[ptr->ip] = (uint8_t)USB_MGR_TSK;
                    usb_ghstd_DeviceAddr[ptr->ip] = (uint16_t)(rootport + USB_DEVICEADDR);
                    if( USB_MAXDEVADDR < usb_ghstd_DeviceAddr[ptr->ip] )
                    {
                        /* For port1 */
                        USB_PRINTF0("Device address error\n");
                    }
                    else
                    {
                        usb_ghstd_MgrMode[ptr->ip][rootport] = USB_DEFAULT;
                        usb_hstd_AttachFunction();
                        usb_hstd_MgrReset(ptr, usb_ghstd_DeviceAddr[ptr->ip]);
                    }
                    usb_hstd_MgrRelMpl(msginfo);
                }
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
                else
                {
                    /* enumeration wait setting */
                    usb_ghstd_EnuWait[ptr->ip] = (uint8_t)USB_MGR_TSK;  
                    usb_hstd_MgrRelMpl(msginfo);
                }
                break;
            default:
                usb_hstd_MgrRelMpl(msginfo);
                break;
            }
            break;
        case USB_MSG_MGR_OVERCURRENT:
            USB_PRINTF0(" Please detach device \n ");
            USB_PRINTF1("VBUS off port%d\n", rootport);
            usb_hstd_VbusControl(ptr, rootport, (uint16_t)USB_VBOFF);
            usb_ghstd_MgrMode[ptr->ip][rootport] = USB_DEFAULT;
            for( md = 0; md < usb_ghstd_DeviceNum[ptr->ip]; md++ )
            {
                driver = &usb_ghstd_DeviceDrv[ptr->ip][md];
                if( driver->rootport == rootport )
                {
                    (*driver->overcurrent)(ptr, rootport, (uint16_t)USB_NO_ARG);
                    /* Root port */
                    usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][0] = USB_NOPORT;
                    /* Device state */
                    usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][1] = USB_DETACHED;   
                    /* Not configured */
                    usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][2] = (uint16_t)0;    
                    /* Interface Class : NO class */
                    usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][3] = (uint16_t)USB_IFCLS_NOT;
                    /* No connect */
                    usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][4] = (uint16_t)USB_NOCONNECT;
                    
                    /* Root port */
                    driver->rootport    = USB_NOPORT;
                    /* Device address */
                    driver->devaddr     = USB_NODEVICE;
                    /* Device state */
                    driver->devstate    = USB_DETACHED;
                }
            }
            usb_hstd_MgrRelMpl(msginfo);
            break;

        /* USB_MSG_HCD_DETACH */
        case USB_GO_POWEREDSTATE:
            ptr->msginfo = USB_MSG_HCD_DETACH_MGR;

            if( devaddr == USB_DEVICEADDR )
            {
                usb_ghstd_MgrMode[ptr->ip][USB_PORT0] = USB_DETACHED;
            }
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
            else if( devaddr == (USB_DEVICEADDR + 1u) )
            {
                usb_ghstd_MgrMode[ptr->ip][USB_PORT1] = USB_DETACHED;
            }
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
            else
            {
            }

            usb_hstd_DeviceStateControl2(ptr, hp->complete, devaddr, ptr->msginfo, msginfo);
            usb_hstd_MgrRelMpl(msginfo);
            break;

        /* USB_MSG_HCD_ATTACH */
        case USB_DO_RESET_AND_ENUMERATION:
            ptr->msginfo = USB_MSG_HCD_ATTACH_MGR;

            if( devaddr == USB_DEVICEADDR )
            {
                usb_ghstd_MgrMode[ptr->ip][USB_PORT0] = USB_DETACHED;
            }
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
            else if( devaddr == (USB_DEVICEADDR + 1u) )
            {
                usb_ghstd_MgrMode[ptr->ip][USB_PORT1] = USB_DETACHED;
            }
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
            else
            {
            }

            usb_hstd_DeviceStateControl2(ptr, hp->complete, devaddr, ptr->msginfo, msginfo);
            usb_hstd_MgrRelMpl(msginfo);
            break;

        /* USB_MSG_HCD_VBON */
        case USB_PORT_ENABLE:
            ptr->msginfo = USB_MSG_HCD_VBON;
            if( devaddr == USB_DEVICEADDR )
            {
                usb_ghstd_MgrMode[ptr->ip][USB_PORT0] = USB_DETACHED;
            }
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
            else if( devaddr == (USB_DEVICEADDR + 1u) )
            {
                usb_ghstd_MgrMode[ptr->ip][USB_PORT1] = USB_DETACHED;
            }
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
            else
            {
            }
            usb_hstd_DeviceStateControl2(ptr, hp->complete, devaddr, ptr->msginfo, msginfo);
            usb_hstd_MgrRelMpl(msginfo);
            break;

        /* USB_MSG_HCD_VBOFF */
        case USB_PORT_DISABLE:
            ptr->msginfo = USB_MSG_HCD_VBOFF;
            if( devaddr == USB_DEVICEADDR )
            {
                usb_ghstd_MgrMode[ptr->ip][USB_PORT0] = USB_DETACHED;
            }
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
            else if( devaddr == (USB_DEVICEADDR + 1u) )
            {
                usb_ghstd_MgrMode[ptr->ip][USB_PORT1] = USB_DETACHED;
            }
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
            else
            {
            }
            usb_hstd_DeviceStateControl2(ptr, hp->complete, devaddr, ptr->msginfo, msginfo);
            usb_hstd_MgrRelMpl(msginfo);
            break;

        /* USB_MSG_HCD_SUSPEND */
        case USB_DO_GLOBAL_SUSPEND:
            ptr->msginfo = USB_MSG_HCD_REMOTE;
            usb_shstd_mgr_callback = hp->complete;
            usb_shstd_mgr_msginfo = msginfo;
            usb_hstd_MgrSuspend(ptr, msginfo);
            break;

        /* USB_MSG_HCD_SUSPEND */
        case USB_DO_SELECTIVE_SUSPEND:
            ptr->msginfo = USB_MSG_HCD_REMOTE;
            usb_hstd_MgrSuspend(ptr, msginfo);
            usb_hstd_DeviceStateControl2(ptr, hp->complete, devaddr, ptr->msginfo, msginfo);
            break;

        /* USB_MSG_HCD_RESUME */
        case USB_DO_GLOBAL_RESUME:
            ptr->msginfo = USB_MSG_HCD_RESUME;
            usb_shstd_mgr_callback = hp->complete;
            usb_shstd_mgr_msginfo = msginfo;
            usb_hstd_MgrResume(ptr, msginfo);
            break;

        /* USB_MSG_HCD_RESUME */
        case USB_MSG_HCD_RESUME:
            usb_shstd_mgr_msginfo = msginfo;
            usb_hstd_MgrResume(ptr, msginfo);
            break;

        /* USB_MSG_HCD_RESUME */
        case USB_DO_SELECTIVE_RESUME:
            ptr->msginfo = USB_MSG_HCD_RESUME;
            usb_hstd_MgrResume(ptr, msginfo);
            usb_hstd_DeviceStateControl2(ptr, hp->complete, devaddr, ptr->msginfo, msginfo);
            break;

        default:
            usb_hstd_MgrRelMpl(msginfo);
            break;
        }
    }
}
/******************************************************************************
End of function usb_hstd_MgrTask
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_SuspCont
Description     : Suspend the connected USB Device (Function for nonOS)
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t devaddr          : Device Address
                : uint16_t rootport         : Port no.
Return          : none
******************************************************************************/
void usb_hstd_SuspCont(USB_UTR_t *ptr, uint16_t devaddr, uint16_t rootport)
{
    uint16_t        checkerr;
    
    checkerr    = usb_shstd_MgrMsg->result;

    switch(usb_shstd_SuspendSeq)
    {
    case    0:
        usb_hstd_GetConfigDesc(ptr, devaddr, (uint16_t)0x09, (USB_CB_t)&usb_hstd_SubmitResult);
        usb_shstd_SuspendSeq++;
        break;
    case    1:
        if (usb_hstd_StdReqCheck(checkerr) == USB_DONE)
        {
            if( usb_hstd_ChkRemote(ptr) == USB_YES )
            {
                usb_hstd_SetFeature(ptr, devaddr, (uint16_t)0xFF, (USB_CB_t)&usb_hstd_SubmitResult);
                usb_shstd_SuspendSeq++;
            }
            else
            {
                USB_PRINTF0("### Remote wakeup disable\n");
                usb_hstd_DeviceStateControl(ptr, devaddr, (uint16_t)USB_MSG_HCD_REMOTE);
                usb_ghstd_MgrMode[ptr->ip][rootport]    = USB_SUSPENDED;
            }
        }
        break;
    case    2:
        if(usb_hstd_StdReqCheck(checkerr) == USB_DONE)
        {
            usb_hstd_DeviceStateControl(ptr, devaddr, (uint16_t)USB_MSG_HCD_REMOTE);
            usb_ghstd_MgrMode[ptr->ip][rootport]    = USB_SUSPENDED;
        }
        break;
    default:
        break;
    }
}
/******************************************************************************
End of function usb_hstd_SuspCont
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_ResuCont
Description     : Resume the connected USB Device (Function for nonOS)
Argument        : uint16_t devaddr          : Device Address
                : uint16_t rootport         : Port no.
Return          : none
******************************************************************************/
void usb_hstd_ResuCont(USB_UTR_t *ptr, uint16_t devaddr, uint16_t rootport)
{
    uint16_t            devsel;
    uint16_t            j,md;
    USB_HCDREG_t        *driver;
    uint16_t            checkerr;

    devsel      = (uint16_t)(devaddr << USB_DEVADDRBIT);
    checkerr    = usb_shstd_MgrMsg->result;
    
    switch(usb_shstd_ResumeSeq)
    {
    case    0:
        usb_hstd_GetConfigDesc(ptr, devaddr, (uint16_t)0x09, (USB_CB_t)&usb_hstd_SubmitResult);
        usb_shstd_ResumeSeq++;
        break;
    case    1:
        if(usb_hstd_StdReqCheck(checkerr) == USB_DONE)
        {
            if( usb_hstd_ChkRemote(ptr) == USB_YES )
            {
                usb_hstd_ClearFeature(ptr, devaddr, (uint16_t)0xFF, (USB_CB_t)&usb_hstd_SubmitResult);
                usb_shstd_ResumeSeq++;
            }
            else
            {
                usb_ghstd_MgrMode[ptr->ip][rootport] = USB_CONFIGURED;
            }
        }
        break;
    case    2:
        if(usb_hstd_StdReqCheck(checkerr) == USB_DONE)
        {
            usb_ghstd_MgrMode[ptr->ip][rootport] = USB_CONFIGURED;
        }
        break;
    default:
        break;
    }
    
    if( usb_ghstd_MgrMode[ptr->ip][rootport] == USB_CONFIGURED )
    {
        /* PIPE resume */
        for( j = USB_MIN_PIPE_NO; j <= USB_MAX_PIPE_NO; j++ )
        {
            /* Agreement device address */
            if(usb_cstd_GetDeviceAddress(ptr, j) == devsel)
            {
                if(usb_ghstd_SuspendPipe[j] == USB_SUSPENDED)
                {
                    usb_cstd_SetBuf(ptr, j);
                    usb_ghstd_SuspendPipe[j] = USB_DONE;
                }
            }
        }

        for(md = 0; md < usb_ghstd_DeviceNum[ptr->ip]; md++)
        {
            driver  = &usb_ghstd_DeviceDrv[ptr->ip][md];
            if(driver->devaddr == (rootport+USB_DEVICEADDR))
            {
                (*driver->devresume)(ptr, (uint16_t)USB_NO_ARG, (uint16_t)USB_NO_ARG);
                /* Device state */
                usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][1] = USB_CONFIGURED;

                if( usb_shstd_mgr_msginfo == USB_DO_GLOBAL_RESUME )
                {
                    usb_hstd_mgr_chgdevst_cb(ptr, rootport);
                }

                /* Device state */
                driver->devstate = USB_CONFIGURED;
            }
        }
    }
}
/******************************************************************************
End of function usb_hstd_ResuCont
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/
