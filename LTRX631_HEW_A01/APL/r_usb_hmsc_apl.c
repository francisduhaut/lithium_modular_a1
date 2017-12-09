#include "include.h"

/* TFAT header */
#include "r_TinyFAT.h"


/*****************************************************************************
Macro definitions
******************************************************************************/
//#define USB_DEBUGPRINT_PP

/* Demonstration */
#define USB_TRANSFER_START        0u
#define USB_TRANSFER_STOP         1u

#define	USB_MEMO_TRANSFERT_START	2u
#define	USB_MEMO_TRANSFERT_STOP		3u

#define	USB_MEMO_INIT				      0x01
#define	USB_MEMO_WRITE				    0x02
#define	USB_MEMO_COMPLETE			    0x03

#define	USB_PROFILE_INIT				  0x04
#define	USB_PROFILE_WRITE				  0x05
#define	USB_PROFILE_COMPLETE			0x06

#define	USB_RFPROG_INIT				    0x07
#define	USB_RFPROG_OPEN				    0x08
#define	USB_RFPROG_READ				    0x09

/* String found in .MOT file */
const uchar S0[] = {"S0"};
const uchar S1[] = {"S1"};
const uchar S9[] = {"S9"};
const uchar S0HEADER[] = {"S0030000FC"};

/******************************************************************************
Private global variables and functions
******************************************************************************/
uint16_t usb_ghmsc_SmpAplProcess;
uint16_t usb_shmsc_DriveOpenSeq = USB_SEQ_0;
uint16_t usb_shmsc_demo_state = USB_TRANSFER_STOP;
//uint8_t usb_gBuf[600];
uint8_t usb_gBuf[1100];
FATFS   usb_gFatfs;

static  FIL fl_file;
static  FATFS  fl_fatfs;
static  uchar VncData[256];       /* R W DataR to read or write in the file */
static  ulong VncFileOffset;          /* Offset in file for read */ 
ulong   VncCfgPtrSave = 0;
ulong   VncCfgPtrOpen = 0;
extern  uchar VncRfPtr;               /* Counter for rf transceiver reprogrammation */
extern  uchar IQFrameTx[MAX_LEN];
extern  uchar U2LenTx;
extern  uchar U2PtrTx;
extern  uint UartStatus;

// MODIF R2.5
extern uchar FlagWdogReset;

#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
int16_t     usb_hmsc_fn;
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */

void    usb_hmsc_PrAplTitle(void);
void    usb_hmsc_MainTask(USB_VP_INT_t);
void    usb_hmsc_apl_init(USB_UTR_t *ptr);
void    R_usb_hmsc_AplClear(USB_UTR_t *ptr);
void    usb_shmsc_DemoStateChange(void);
void 		usb_shmsc_UsbMemoStateChange(void);
void    usb_hmsc_SmplMessageSend(USB_UTR_t *ptr, uint16_t msginfo);
void    usb_hmsc_DummyFunction(USB_UTR_t *ptr, uint16_t data1, uint16_t data2);
static  void usb_hapl_registration(USB_UTR_t *ptr);
uint32_t CheckFreeSpace(void);

void    usb_cstd_task_start( void );
void    usb_hapl_task_start( USB_UTR_t *ptr );

void 		usb_hmsc_MainAplTask(void);
void    usb_hmsc_SampleAplSpecifiedPath(USB_CLSINFO_t *mess);

void 		usb_MemoWriteInit (void);
void 		usb_MemoWriteChrono (void);
void		usb_MemoAdd (void);
void 		usb_MemoClose (void);

uchar   UsbMemoFileName[13];
uchar   UsbProfileFileName[13];
uchar   FlagWriteFile;

void 		usb_ProfileWriteInit (void);
void    usb_ProfileWriteChrono (void);
void		usb_ProfileAdd (void);
void 		usb_ProfileClose (void);

void    usb_RfPrgOpen (void);
void    usb_RfPrgChrono (void);
void    usb_RfPrgRead (void);
uchar   CrcMot(uchar *v);

void    usb_CfgChrono(void);
uint    HexUint(uchar *i);

// MODIF R3.3
void usb_E1MemoWriteInit (void);
void usb_E1MemoWriteChrono (void);
void usb_E1MemoClose (void);
void usb_E1MemoAdd (void);

extern int16_t      usb_ghmsc_SmplClassSeq;
extern int16_t      usb_ghmsc_SmplClassSeqWait;

uint16_t            usb_ghmsc_open = USB_NG;

const uint16_t usb_gapl_devicetpl[] =
{
    /* Number of tpl table */
    4,
    /* Reserved */
    0,
    /* Vendor ID  : no-check */
    USB_NOVENDOR,
    /* Product ID : no-check */
    USB_NOPRODUCT,
};

USB_UTR_t       *mess;
USB_ER_t        err;
FRESULT         res;
USB_CLSINFO_t   *mes;

uint16_t        addr;
FIL             file;
uint16_t        file_rw_cnt;
uint 			      Offset;

uint16_t		    UsbPtr;
uint16_t		    UsbMemoPtr;
uint16_t		    UsbMemoSeq;

FRESULT         res;
uint16_t        fl_file_rw_cnt;
uint8_t         v[13];
uint8_t         FileVersion;

/******************************************************************************
External variables and functions
******************************************************************************/
extern  uint16_t            usb_ghmsc_RootDevaddr;
extern  USB_UTR_t           tfat_ptr;

extern void     usb_cpu_LcdDisp(uint8_t position, uint8_t *string);
extern void     usb_cpu_LedSet(uint8_t data);
extern void     usb_cpu_LedSetBit(uint8_t bit, uint8_t data);
extern uint16_t usb_cpu_GetKeyNo(void);
static void     usb_hmsc_task_start( void );
extern void 	  usb_hmsc_hub_registration(USB_UTR_t *ptr);

/*****************************************************************************
Enumerated Types
******************************************************************************/

/* Host Std Sample application message command */
typedef enum
{
    USB_HMSC_OPEN = 1,
    USB_HMSC_DETACH,
    USB_HMSC_DEMO
}
USB_HMSC_ANSI_COMMAND;

/******************************************************************************
Renesas Host MSC Sample Code functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_task_start
Description     : Start task processing.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_task_start( void )
{
    /* Start the Idle task. */
    usb_cstd_IdleTaskStart();

    /* Start host-related USB drivers. */
    usb_hmsc_task_start();

}
/******************************************************************************
End of function usb_cstd_task_start()
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_task_start
Description     : Start task processing.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hmsc_task_start( void )
{
    /* The USB Communication Structure allocation for this MSC. */
    USB_UTR_t   utr;
    /* Pointer to the USB Communication Structure above. */
    USB_UTR_t*  ptr = &utr;
    
    /* Determine which port is host. */
    ptr->ip = USB_HOST_USBIP_NUM;

    /* If port is used, register the driver etc. */
    if (USB_NOUSE_PP != ptr->ip )
    {
        ptr->ipp = R_usb_cstd_GetUsbIpAdr( ptr->ip ); /* Get the USB IP base address. */

        tfat_ptr.ip     = ptr->ip;          /* Set up USB IP number for TFAT system. */
        tfat_ptr.ipp    = ptr->ipp;         /* Set up USB IP base address for TFAT. */

        /* Set-up tasks and drivers to use the allocated Comm. structure (for MSC) above. */
        R_usb_hstd_usbdriver_start( ptr );  /* Host driver. */

        usb_hapl_registration( ptr );       /* Host application registration. */
        usb_hmsc_hub_registration(ptr);     /* Hub registration. */
        R_usb_hmsc_driver_start( ptr );     /* Host class driver. */
        usb_hapl_task_start( ptr );         /* Host application task. */

        /* Finally, init the HW with the Comm. struct. */
        R_usb_cstd_UsbIpInit( ptr, USB_HOST_PP );
    }
}
/******************************************************************************
End of function usb_hstd_task_start()
******************************************************************************/

/******************************************************************************
Function Name   : USB_Chrono
Description     : Non-OS task switch loop.
Argument        : none
Return          : none
******************************************************************************/
void USB_Chrono (void)
{
    // MODIF R2.5 : enable wdog refresh by timer_cmt
    FlagWdogReset = 1;
    
    /* Scheduler */
    R_usb_cstd_Scheduler();

    /* Check for any task processing requests flags. */
    if( USB_FLGSET == R_usb_cstd_CheckSchedule() )
    {
        R_usb_hstd_HcdTask((USB_VP_INT)0);          /* HCD Task */
        R_usb_hstd_MgrTask((USB_VP_INT)0);          /* MGR Task */
        R_usb_hhub_Task((USB_VP_INT)0);             /* HUB Task */

        usb_hmsc_Task();                        /* HMSC Task */
        usb_hmsc_StrgDriveTask();               /* HSTRG Task */
        usb_hmsc_MainAplTask();					/* HMSC Main USB Task  */
    }
    else
    {
        /* Idle task - background "sleep". */
        R_usb_cstd_IdleTask(0);
    }
    
    // MODIF R2.5 : disable wdog refresh by timer_cmt
    FlagWdogReset = 0;
}
/******************************************************************************
End of function usb_apl_task_switch()
******************************************************************************/

/******************************************************************************
Function Name   : usb_hapl_task_start
Description     : Start up host USB application task.
Arguments       : USB_UTR_t *ptr        : The app's USB Comm. Structure.
Return value    : none
******************************************************************************/
void    usb_hapl_task_start( USB_UTR_t *ptr )
{
    /* Set task priority of HMSC sample application. */
    R_usb_cstd_SetTaskPri(USB_HMSCSMP_TSK, USB_PRI_4);

    /* Clear application using fresh USB Comm. Structure. */
    usb_hmsc_apl_init(ptr);
}
/******************************************************************************
End of function usb_hapl_task_start()
******************************************************************************/

/******************************************************************************
Function Name   : usb_hapl_registration
Description     : Callback registration.
Arguments       : USB_UTR_t *ptr        : The app's USB Comm. Structure.
Return value    : none
******************************************************************************/
void usb_hapl_registration(USB_UTR_t *ptr)
{
    USB_HCDREG_t    driver;

    /* Driver registration */
    driver.ifclass      = (uint16_t)USB_IFCLS_MAS;      /* Use Interface class for MSC. */
    driver.tpl          = (uint16_t*)&usb_gapl_devicetpl;   /* Target peripheral list. */
    driver.pipetbl      = (uint16_t*)&usb_ghmsc_DefEpTbl;   /* Pipe def. table address. */
    driver.classinit    = &R_usb_hmsc_Initialized;      /* Driver init. */
    driver.classcheck   = &R_usb_hmsc_ClassCheck;       /* Driver check. */
    driver.devconfig    = &R_usb_hmsc_DriveOpen;        /* Callback when device is configured. */
    driver.devdetach    = &R_usb_hmsc_DriveClose;       /* Callback when device is detached. */
    driver.devsuspend   = &usb_hmsc_DummyFunction;      /* Callback when device is suspended. */
    driver.devresume    = &usb_hmsc_DummyFunction;      /* Callback when device is resumed. */

    /* Host MSC class driver registration. */
    R_usb_hstd_DriverRegistration(ptr, &driver);
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_PrAplTitle
Description     : Output Title
Argument        : none
Return          : none
******************************************************************************/
void usb_hmsc_PrAplTitle(void)
{
    /* Print title */
    USB_PRINTF0("\n");
    USB_PRINTF0("+++++++++++++++++++++++++++++++++++++\n");
    USB_PRINTF0("+  HOST MASS STRAGE CLASS FW SAMPLE +\n");
    USB_PRINTF0("+                                   +\n");
    USB_PRINTF0("+     RENESAS ELECTRONICS CORP.     +\n");
    USB_PRINTF0("+     RENESAS SOLUTIONS  CORP.      +\n");
    USB_PRINTF0("+++++++++++++++++++++++++++++++++++++\n");
    USB_PRINTF1("       DATE [%s] \n", __DATE__);
    USB_PRINTF1("       TIME [%s] \n", __TIME__);
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_apl_init
Description     : Clear application.
Arguments       : USB_UTR_t *ptr        : The app's USB Comm. Structure.
Return          : none
******************************************************************************/
void usb_hmsc_apl_init(USB_UTR_t *ptr)
{
    /* Demo stop */
    usb_shmsc_demo_state = USB_TRANSFER_STOP;
#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
    /* Clear ANSI-C IO file number. */
    usb_hmsc_fn = -1;
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */

    /* Init this application's drive open sequence state. */
    usb_shmsc_DriveOpenSeq = USB_SEQ_0;
    usb_ghmsc_SmpAplProcess = USB_APL_CLR;
#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
    /* ANSI-C IO enumeration initialization sequence. */
    usb_ghmsc_SmplClassSeq = -1;
    /* Wait for the ANSI flag enumeration initialization sequence. */
    usb_ghmsc_SmplClassSeqWait = 0;
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */
    
    /* Open the demo application's message transmission task. */
    usb_hmsc_SmplMessageSend(ptr, USB_HMSC_OPEN);
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_hmsc_AplClear
Description     : Application clear
Arguments       : USB_UTR_t *ptr        : The app's USB Comm. Structure.
Return          : none
******************************************************************************/
void R_usb_hmsc_AplClear(USB_UTR_t *ptr)
{
    /* Demo stop */
    usb_shmsc_demo_state = USB_TRANSFER_STOP;

    /* Init this application's drive open sequence state. */
    usb_shmsc_DriveOpenSeq = USB_SEQ_0;
    usb_ghmsc_SmpAplProcess = USB_APL_CLR;
#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
    /* Host MSC file number check. */
    if( usb_hmsc_fn != -1 )
    {
        /* End of HMSC communication. */
        close(usb_hmsc_fn);
        usb_hmsc_fn = -1;
    }
    usb_ghmsc_open = USB_NG;
    
    /* ANSI-IO enumeration initialization sequence. */
    usb_ghmsc_SmplClassSeq = -1;
    
    /* Wait for the ANSI flag enumeration initialization sequence. */
    usb_ghmsc_SmplClassSeqWait = 0;

    /* Send detach notification to demo application tasks. */
    usb_hmsc_SmplMessageSend(ptr, USB_HMSC_DETACH);
#else  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */
    usb_hmsc_SampleAplSpecifiedPath(ptr);
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_shmsc_UsbMemoStateChange
Description     : Change demo state.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_shmsc_UsbMemoStateChange(void)
{
    /* Application demo state. */
    if(usb_shmsc_demo_state == USB_MEMO_TRANSFERT_START)
    {
        usb_shmsc_demo_state = USB_MEMO_TRANSFERT_STOP;
    }
    else if(usb_shmsc_demo_state == USB_MEMO_TRANSFERT_STOP)
    {
        usb_shmsc_demo_state = USB_MEMO_TRANSFERT_START;
    }
    else
    {
    }
}

/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_DummyFunction
Description     : dummy function
Arguments       : USB_UTR_t *ptr    : The app's USB Comm. Structure.
                : uint16_t data1            : not use
                : uint16_t data2            : not use
Return value    : none
******************************************************************************/
void usb_hmsc_DummyFunction(USB_UTR_t *ptr, uint16_t data1, uint16_t data2)
{
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_hmsc_SmplMessageSend
Description     : Sample class init
Arguments       : USB_UTR_t *ptr        : The app's USB Comm. Structure.
                : uint16_t   msginfo         : Message Information Kind
Return          : none
******************************************************************************/
void usb_hmsc_SmplMessageSend( USB_UTR_t *ptr, uint16_t msginfo )
{
    USB_MH_t        p_blf;
    USB_ER_t        err;
    USB_CLSINFO_t   *cp;
    
    /* Get mem pool blk */
    if( USB_PGET_BLK(USB_HMSCSMP_MPL, &p_blf) == USB_OK )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        
        cp->msginfo = msginfo;
        cp->ipp = ptr->ipp;
        cp->ip  = ptr->ip;
        cp->keyword = ptr->keyword;
        cp->result  = ptr->result;
        
        /* Send message */
        err = USB_SND_MSG( USB_HMSCSMP_MBX, (USB_MSG_t*)cp );
        if( err != USB_E_OK )
        {
            /* Error. Release message memory from pool. */
            err = USB_REL_BLK(USB_HMSCSMP_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### usb_hmsc_SmplMessageSend function snd_msg error\n");
        }
    }
    else
    {
        /* error */
        USB_PRINTF0("### usb_hmsc_SmplMessageSend function pget_blk error\n");
        while( 1 );
    }
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_SampleAplSpecifiedPath
Description     : Next Process Selector
Argument        : USB_CLSINFO_t *mess
Return          : none
******************************************************************************/
void usb_hmsc_SampleAplSpecifiedPath(USB_CLSINFO_t *mess)
{
    USB_MH_t            p_blf;
    USB_ER_t            Err;
    USB_CLSINFO_t       *ptr;

    /* Get mem pool blk */
    if( USB_PGET_BLK(USB_HMSCSMP_MPL, &p_blf) == USB_OK )
    {
        ptr = (USB_CLSINFO_t*)p_blf;
        ptr->msginfo    = usb_ghmsc_SmpAplProcess;
        ptr->keyword    = mess->keyword;
        ptr->result     = mess->result;
        
        /* Send message */
        Err = USB_SND_MSG(USB_HMSCSMP_MBX, (USB_MSG_t*)p_blf);
        if( Err != USB_E_OK )
        {
            Err = USB_REL_BLK(USB_HMSCSMP_MPL, (USB_MH_t)p_blf);
            USB_PRINTF0("### SpecifiedPass function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### SpecifiedPass function pget_blk error\n");
    }
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_MainAplTask
Description     : Main usb application

Argument        : none
Return          : none
******************************************************************************/
void usb_hmsc_MainAplTask(void)
{

#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
    USB_UTR_t       *ptr;
#else  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */
    uint16_t        file_err = USB_OK;
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */

    /* Check for incoming application messages. */
    err = USB_TRCV_MSG(USB_HMSCSMP_MBX, (USB_MSG_t**)&mess, (USB_TM_t)5000);
#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
    if(err == USB_OK && usb_hmsc_fn == -1)
    {
        /* Open the case processing application processing has not been completed. */
        ptr = mess;

        if(mess->msginfo != USB_HMSC_OPEN && mess->msginfo != USB_HMSC_DETACH)
        {
            mess->msginfo = USB_HMSC_DETACH;
        }

        /* Switch action depending on message. */
        switch(mess->msginfo)
        {
        case USB_HMSC_OPEN:
            /* Host MSC open. */
            usb_hmsc_fn = open((int8_t *)USB_CLASS_HMSC, 0, 0);
            /* Host MSC file number check. */
            if(usb_hmsc_fn == -1)
            {
                /* If host MSC open is not complete, send a message requesting to re-open. */
                usb_hmsc_SmplMessageSend(ptr, USB_HMSC_OPEN);
            }
            else
            {
                UsbDiskOn;
            }
            break;

        case USB_HMSC_DETACH:
            UsbDiskOff;
            UsbWriteMemoOff;
            UsbE1MemoOff;			// MODIF R3.3
            UsbWriteProfileOff;
            UsbRfProgOff;
            usb_hmsc_SmplMessageSend(ptr, USB_HMSC_OPEN);
            break;

        default:
            UsbDiskOff;
            break;
        }
        /* Release message memory from pool. */
        err = USB_REL_BLK(USB_HMSCSMP_MPL,(USB_MH_t)mess);
        if( err != USB_E_OK )
        {
            USB_PRINTF0("### USB Hsmp Task rel_blk error\n");
        }
    }
    else if(err == USB_OK && usb_ghmsc_open == USB_OK)
    {
        /* Open application process when processing has been completed. */
#else   /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */
        if( err != USB_OK )
        {
            return;
        }
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */


        /* Switch action depending on message. */
        mes = (USB_CLSINFO_t *)mess;
        switch( mes->msginfo )
        {
        case USB_HMSC_DRIVE_OPEN:
            /* Message next time we come back here is.. */
            /* == 0 -> demo start */
            usb_ghmsc_SmpAplProcess = USB_START;
            usb_hmsc_SampleAplSpecifiedPath(mes);
            break;


        case USB_START:
            /* Switch on application drive open sequence state. */
            switch( usb_shmsc_DriveOpenSeq )
            {
            case USB_SEQ_0:
                /* Increment application drive open sequence. */
                usb_shmsc_DriveOpenSeq++;

                /* Set device address. */
                addr = mes->keyword;

                /* Storage drive search. */
                R_usb_hmsc_StrgDriveSearch(mess, addr);
                break;

            case USB_SEQ_1:
                /* Check storage drive search result. */
                if( mes->result != USB_DONE )
                {
                    UsbDiskOff;
                    UsbWriteMemoOff;
                    UsbE1MemoOff;			// MODIF R3.3
                    UsbWriteProfileOff;
                    UsbRfProgOff;
                }
                else
                {
                    /* Device status setting = attached. */
                    R_usb_hmsc_SetDevSts((uint16_t)USB_HMSC_DEV_ATT);

                    /* Application initialization sequence start. */
                    /* Increment application drive open sequence. */
                    usb_shmsc_DriveOpenSeq = USB_SEQ_0;

                    /* Send message to myself: Start demo key input wait. */
                    usb_ghmsc_SmpAplProcess = USB_WAIT1;
                    usb_hmsc_SampleAplSpecifiedPath(mes);
                    // USB DRIVE READY
                    UsbDiskOn;
                }
                break;
            default:
                /* Reset application drive open sequence. */
                usb_shmsc_DriveOpenSeq = USB_SEQ_0;
                break;
            }
            break;

        case USB_WAIT1:
            
            if (UsbE1Memo != 0)		// MODIF R3.3
            {
            	UsbPtr = USB_MEMO_INIT;
            	/* Message next time we come back here is Mount device. */
            	usb_ghmsc_SmpAplProcess = USB_HMSC_DRIVEMOUNT;
            	usb_hmsc_SampleAplSpecifiedPath(mes);
            }
            else if (UsbWriteMemo != 0)
            {
                UsbPtr = USB_MEMO_INIT;
                /* Message next time we come back here is Mount device. */
                usb_ghmsc_SmpAplProcess = USB_HMSC_DRIVEMOUNT;
                usb_hmsc_SampleAplSpecifiedPath(mes);
            }
            else if (UsbWriteProfile != 0)
            {
                UsbPtr = USB_PROFILE_INIT;
                /* Message next time we come back here is Mount device. */
                usb_ghmsc_SmpAplProcess = USB_HMSC_DRIVEMOUNT;
                usb_hmsc_SampleAplSpecifiedPath(mes);
            }
            else if (UsbRfProg != 0)
            {
                UsbPtr = USB_RFPROG_INIT;
                /* Message next time we come back here is Mount device. */
                usb_ghmsc_SmpAplProcess = USB_HMSC_DRIVEMOUNT;
                usb_hmsc_SampleAplSpecifiedPath(mes);
            }
            else if ((VncCfgPtrSave != 0) || (VncCfgPtrOpen != 0))
            {
                /* Message next time we come back here is Mount device. */
                usb_ghmsc_SmpAplProcess = USB_HMSC_DRIVEMOUNT;
                usb_hmsc_SampleAplSpecifiedPath(mes);
            }
            else
            {
                usb_hmsc_SampleAplSpecifiedPath(mes);
            }
            break;

        case USB_HMSC_DRIVEMOUNT:
            /* File system media work area memory mount. */
            res = R_tfat_f_mount(0, &usb_gFatfs);
            if( res != TFAT_FR_OK )
            {
                USB_PRINTF1("R_tfat_f_mount error: %d\n", res);
            }
            /* Message next time we come back here is.. */
            usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITE;
            usb_hmsc_SampleAplSpecifiedPath(mes);
            break;

        case USB_HMSC_FILEWRITE:
        	if (UsbE1Memo != 0)		// MODIF R3.3
        	usb_E1MemoWriteChrono();
        	else if (UsbWriteMemo != 0)
            usb_MemoWriteChrono();
            else if (UsbWriteProfile != 0)
            usb_ProfileWriteChrono();  
            else if (UsbRfProg != 0)
            usb_RfPrgChrono();  
            else if ((VncCfgPtrSave != 0) || (VncCfgPtrOpen != 0))
            usb_CfgChrono();   
            break;

        case USB_HMSC_FILEWRITEOK:
            if( usb_ghmsc_open == USB_OK )
            {
                if ((VncCfgPtrSave != 0) || (VncCfgPtrOpen != 0))
                usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITE;
                else if (UsbE1Memo != 0)		// MODIF R3.3
                {
                	UsbPtr = USB_MEMO_INIT;
                	usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITE;
                }
                else if (UsbWriteMemo != 0)
                {
                    UsbPtr = USB_MEMO_INIT;
                    usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITE;
                }
                else if (UsbWriteProfile != 0)
                {
                    UsbPtr = USB_PROFILE_INIT;
                    usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITE;
                }
                else if (UsbRfProg != 0)
                {
                    UsbPtr = USB_RFPROG_INIT;
                    usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITE;
                }

#if USB_ANSIIO_PP != USB_ANSIIO_USE_PP
                if( file_err != USB_NG )
                {
#endif  /* USB_ANSIIO_PP != USB_ANSIIO_USE_PP */
                    usb_hmsc_SampleAplSpecifiedPath(mes);
#if USB_ANSIIO_PP != USB_ANSIIO_USE_PP
                }
#endif  /* USB_ANSIIO_PP != USB_ANSIIO_USE_PP */
            }
            break;

        case USB_APL_CLR:
            UsbDiskOff;
            break;

        default:
            break;
        }
        /* Release message memory from pool. */
        err = USB_REL_BLK(USB_HMSCSMP_MPL,(USB_MH_t)mess);
        if( err != USB_E_OK )
        {
            USB_PRINTF0("### USB Hsmp Task rel_blk error\n");
        }
#if USB_ANSIIO_PP == USB_ANSIIO_USE_PP
    }
    else
    {
        return;
    }
#endif  /* USB_ANSIIO_PP == USB_ANSIIO_USE_PP */
}
/******************************************************************************
End of function
******************************************************************************/



void usb_MemoWriteChrono (void)
{

    switch (UsbPtr)
    {
    case USB_MEMO_INIT :
        usb_MemoWriteInit();
        break;

    case USB_MEMO_WRITE :
        usb_MemoAdd();
        break;

    case USB_MEMO_COMPLETE :
        usb_MemoClose();
        UsbWriteMemoOff;
        UsbPtr = 0;
        if (MaskPasswdHigh == 0)
        {
        	UsbE1MemoOn;		// MODIF R3.3
        }
        break;

        default :
        UsbPtr = 0;
        break;
    }

    if (UsbPtr != 0)
    usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITE;
    else
    usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITEOK;
    usb_hmsc_SampleAplSpecifiedPath(mes);

}


void usb_MemoWriteInit (void)
{
    //DIR dir;
    usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITE;
    CsvNewFileName(1, &UsbMemoFileName[0]);         /* Creates a new file name */

    //res = R_tfat_f_mkdir("LIFETECH");
    //res = R_tfat_f_opendir(&dir, "LIFETECH");
    /* Create a file object. */
    res = R_tfat_f_open(&file, &UsbMemoFileName[0], (TFAT_FA_CREATE_ALWAYS | TFAT_FA_WRITE));
    if( res != TFAT_FR_OK )
    {
        USB_PRINTF1("R_tfat_f_open error: %d\n", res);
        UsbPtr = 0;
        UsbWriteMemoOff;
    }
    else
    {
        UsbPtr = USB_MEMO_WRITE;
        UsbMemoPtr = Status.MemoPointer;
        UsbMemoSeq = 0;
    }

}


void usb_MemoClose (void)
{
    FILINFO filinfo;

    filinfo.fdate = ((State.DateR.Year - 1980) << 9) + (State.DateR.Month << 5) + State.DateR.Date;
    filinfo.ftime = (State.DateR.Hr << 11) + (State.DateR.Min << 5) + (State.DateR.Sec / 2);
    /* Close the file object. */
    res = R_tfat_f_close(&file);
    R_tfat_f_utime (&UsbMemoFileName[0], &filinfo);
    if( res != TFAT_FR_OK )
    {
        USB_PRINTF1("R_tfat_f_close error: %d\n", res);
    }

    /* Message next time we come back here is.. */
    usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITEOK;
    usb_hmsc_SampleAplSpecifiedPath(mes);
}

void usb_MemoAdd (void)
{
    uint i;

    uint Bytes2Writes;
    StructMemo MemoUsb;

    switch (UsbMemoSeq)
    {
    case 0 :
        if  (CheckFreeSpace() > 1000) // check if 1Mbytes freespace
        {
            UsbMemoSeq++;
            CsvConfigLine1(1, &usb_gBuf[0], &Bytes2Writes);	// header
            res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
        }
        else
        {
            UsbPtr = 0;
            UsbWriteMemoOff;
        }
        break;

    case 1 :
        UsbMemoSeq++;
        CsvConfigLine1(0, &usb_gBuf[0], &Bytes2Writes);
        res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
        break;

    case 2 :
        UsbMemoSeq++;
        CsvConfigLine2(1, &usb_gBuf[0], &Bytes2Writes);	// header
        res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
        break;

    case 3 :
        UsbMemoSeq++;
        CsvConfigLine2(0, &usb_gBuf[0], &Bytes2Writes);
        res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
        break;

    case 4 :
        UsbMemoSeq++;
        CsvStatusLine(1, &usb_gBuf[0], &Bytes2Writes);	// header
        res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
        break;

    case 5 :
        UsbMemoSeq++;
        CsvStatusLine(0, &usb_gBuf[0], &Bytes2Writes);
        res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
        break;

    case 6 :
        UsbMemoSeq++;
        UsbMemoPtr = SEGMEMOTOP;
        CsvMemoLine(1, &usb_gBuf[0], &Bytes2Writes, &MemoUsb);
        res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
        break;

    case 7 :
        if (Status.NbRstPointer == 0)
        {
            
            if (Status.MemoPointer < UsbMemoPtr)
            {
                UsbMemoPtr = 0;                             /* End of writing */
                UsbPtr = USB_MEMO_COMPLETE;
            }
            else
            {
                Read_SegFlashData (UsbMemoPtr, (uchar *)&MemoUsb);
                CsvMemoLine(0, &usb_gBuf[0], &Bytes2Writes, &MemoUsb);
                res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
                UsbMemoPtr++;
            }
        }
        else
        {
            i = (((Status.MemoPointer - SEGMEMOTOP) + (UsbMemoPtr - SEGMEMOTOP) + 1) % SEGMEMONB) + SEGMEMOTOP;
            Read_SegFlashData (i, (uchar *)&MemoUsb);
            CsvMemoLine(0, &usb_gBuf[0], &Bytes2Writes, &MemoUsb);
            res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
            UsbMemoPtr++;
            if ((SEGMEMOTOP + SEGMEMONB) <= UsbMemoPtr)
            {
                UsbMemoPtr = 0;                             /* End of writing */
                UsbPtr = USB_MEMO_COMPLETE;
            }
        }
        break;
    }

    if (res != TFAT_FR_OK)
    {
        UsbPtr = 0;
        UsbWriteMemoOff;
    }
}



void usb_ProfileWriteInit (void)
{
    FILINFO filinfo;
    
    usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITE;
    CsvNewFileName(0, &UsbProfileFileName[0]);         /* Creates a new file name */

    /* Create a file object. */
    res = R_tfat_f_open(&file, &UsbProfileFileName[0], (TFAT_FA_CREATE_ALWAYS | TFAT_FA_WRITE));
    if( res != TFAT_FR_OK )
    {
        USB_PRINTF1("R_tfat_f_open error: %d\n", res);
    }
    filinfo.fdate = ((State.DateR.Year - 1980) << 9) + (State.DateR.Month << 5) + State.DateR.Date;
    filinfo.ftime = (State.DateR.Hr << 11) + (State.DateR.Min << 5) + (State.DateR.Sec / 2);
    R_tfat_f_utime (&UsbProfileFileName[0], &filinfo);
    
    UsbMemoSeq = 0;
}

void usb_ProfileAdd (void)
{
    uint Bytes2Writes;
    FILINFO filinfo;

    if  (CheckFreeSpace() < 500)  // Chek if minimum of 500Kbytes freespace
    {
      UsbWriteProfileOff;
      return;
    }

    switch (UsbMemoSeq)
    {
    case 0 :
        UsbMemoSeq++;
        CsvConfigLine1(1, &usb_gBuf[0], &Bytes2Writes);	// header
        res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
        break;

    case 1 :
        UsbMemoSeq++;
        CsvConfigLine1(0, &usb_gBuf[0], &Bytes2Writes);
        res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
        break;

    case 2 :
        UsbMemoSeq++;
        CsvConfigLine2(1, &usb_gBuf[0], &Bytes2Writes);	// header
        res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
        break;

    case 3 :
        UsbMemoSeq++;
        CsvConfigLine2(0, &usb_gBuf[0], &Bytes2Writes);
        res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
        break;

    case 4 :
        UsbMemoSeq++;
        CsvStatusLine(1, &usb_gBuf[0], &Bytes2Writes);	// header
        res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
        break;

    case 5 :
        UsbMemoSeq++;
        CsvStatusLine(0, &usb_gBuf[0], &Bytes2Writes);
        res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
        break;

    case 6 :
        UsbMemoSeq++;
        UsbMemoPtr = SEGMEMOTOP;
        CsvProfileLine(1, &usb_gBuf[0], &Bytes2Writes);
        res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
        res = R_tfat_f_sync(&file);
        FlagWriteFile = 1;
        break;

        default :
        
        if (Menu.VncProfileTime == 0)
        {  
            Menu.VncProfileTime = 10;
            MenuWriteOn;
        }
        if ((State.TimerSec % (ulong)Menu.VncProfileTime) == 0)
        {
            if (FlagWriteFile == 1)
            {
                CsvProfileLine(0, &usb_gBuf[0], &Bytes2Writes);
                res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
                res = R_tfat_f_sync(&file);
                filinfo.fdate = ((State.DateR.Year - 1980) << 9) + (State.DateR.Month << 5) + State.DateR.Date;
                filinfo.ftime = (State.DateR.Hr << 11) + (State.DateR.Min << 5) + (State.DateR.Sec / 2);
                R_tfat_f_utime (&UsbProfileFileName[0], &filinfo);
                FlagWriteFile = 0; // Do one time per sec
            }
        }
        else
        FlagWriteFile = 1;
        break;
    }

}


void usb_ProfileClose (void)
{
    FILINFO filinfo;

    if (UsbWriteProfile != 0)
    {
        filinfo.fdate = ((State.DateR.Year - 1980) << 9) + (State.DateR.Month << 5) + State.DateR.Date;
        filinfo.ftime = (State.DateR.Hr << 11) + (State.DateR.Min << 5) + (State.DateR.Sec / 2);
        /* Close the file object. */
        res = R_tfat_f_close(&file);
        R_tfat_f_utime (&UsbProfileFileName[0], &filinfo);
        if( res != TFAT_FR_OK )
        {
            USB_PRINTF1("R_tfat_f_close error: %d\n", res);
        }

        /* Message next time we come back here is.. */
        usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITEOK;
        usb_hmsc_SampleAplSpecifiedPath(mes);  
    }
    UsbWriteProfileOff;
}


void usb_ProfileWriteChrono (void)
{

    switch (UsbPtr)
    {
    case USB_PROFILE_INIT :
        usb_ProfileWriteInit();
        UsbPtr = USB_PROFILE_WRITE;
        break;

    case USB_PROFILE_WRITE :
        usb_ProfileAdd();
        break;

        default :
        UsbPtr = 0;
        break;
    }

    if (UsbPtr != 0)
    usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITE;
    else
    usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITEOK;
    usb_hmsc_SampleAplSpecifiedPath(mes);
}


void usb_RfPrgOpen (void)
{

    /* **** Mount the file system **** */
    res = R_tfat_f_mount(0, &fl_fatfs);
    if( res == TFAT_FR_OK )
    {
        /* Open the file to be read */        
        sprintf ((char *)&v[0], "IQRFPRG.S19");
        res = R_tfat_f_open(&fl_file, v, TFAT_FA_OPEN_EXISTING | TFAT_FA_READ);
    }
    if( res != TFAT_FR_OK )
    UsbPtr = 0;
    else
    {
        UsbPtr = USB_RFPROG_READ;
        VncRfPtr = 1;
    }
}

void usb_RfPrgRead (void)
{
    uchar *d;
    uint i;

    /* Read data first for non changing read data */
    switch (VncRfPtr)
    {
    case 0 :
        break ;  
    case 1 :
        VncRdfOff;              /* Mandatory for launching first read */      
        VncFileOffset = 0;      /* Beginning of the file */
        U2Xon;                  /* U1 transceive allowed */
        VncRfPtr ++;
        break ;  
    case 2 :
        /* Data are stored in VncData[256], number is */
        if (VncRdf != 0)        /* Test for available data */   
        {
            /* Search for S0 */
            d = (uchar *)strstr((const char *)&VncData[0], (const char *)&S0[0]);
            if (d != 0)           /* S0 sequence detected */
            {
                if (U2XonXoff == 0) 
                {
                    memcpy(&IQFrameTx[0], &S0HEADER[0], 10);
                    U2LenTx = 10;       
                    IQFrameTx[U2LenTx] = CR;
                    U2LenTx ++;
                    IQFrameTx[U2LenTx] = LF;
                    U2LenTx ++;
                    U2PtrTx = 0;  
                    SCI6.TDR = IQFrameTx[0]; 
                    /* Calculation of new offset in file for next reading */
                    i = (uint)(d - &VncData[0]);
                    VncFileOffset = VncFileOffset + i + 2 * HexUchar(d + 2) + 4;
                    VncRfPtr ++;      /* S0 treated, go to S1 or S2 */
                    VncRdfOff;        /* Mandatory for launching first read */      
                }
            }
            else
            {
                UsbPtr = 0;  
                VncRfPtr = 0;
                VncRdfOff;          /* Mandatory if launch function again */      
            }
        }
        if ((VncRdf == 0) && (VncRfPtr != 0) && (U0Car == 0) && (U2XonXoff == 0)) 
        {
            /* No data, launch command, buffer empty, Xon */  
            R_tfat_f_lseek(&fl_file, 0); 
            R_tfat_f_read(&fl_file, VncData, 240, &fl_file_rw_cnt);
            VncRdfOn;
        }
        break ;                      
    case 3 :       
        /* Data are stored in VncData[256], number is */
        if (VncRdf != 0)        /* Test for available data */   
        {
            d = (uchar *)strstr((const char *)&VncData[0], (const char *)&S1[0]);
            if (d != 0)                                                 /* S1 sequence detected */
            {
                if (CrcMot(d) == 0xFF)                                    /* Crc OK */
                {
                    if ((U2XonXoff == 0) && ((U2LenTx - 1) <= U2PtrTx))
                    {
                        memcpy(&IQFrameTx[0], d, 2 * HexUchar(d + 2) + 4);
                        U2LenTx = 2 * HexUchar(d + 2) + 4;
                        IQFrameTx[U2LenTx] = CR;
                        U2LenTx ++;
                        IQFrameTx[U2LenTx] = LF;
                        U2LenTx ++;
                        U2PtrTx = 0;
                        SCI6.TDR = IQFrameTx[0]; 
                        /* Calculation of new offset in file for next reading */
                        i = (uint)(d - &VncData[0]);
                        VncFileOffset = VncFileOffset + i + 2 * HexUchar(d + 2) + 4;
                        VncRdfOff;                /* Mandatory */
                    }
                }
            }
            else
            {              
                d = (uchar *)strstr((const char *)&VncData[0], (const char *)&S9[0]);
                if (d != 0)                 /* S9 sequence detected */
                {
                    if (CrcMot(d) == 0xFF)    /* Crc OK */
                    {
                        if ((U2XonXoff == 0) && ((U2LenTx - 1) <= U2PtrTx))
                        {
                            memcpy(&IQFrameTx[0], d, 2 * HexUchar(d + 2) + 4);
                            U2LenTx = 2 * HexUchar(d + 2) + 4;
                            IQFrameTx[U2LenTx] = CR;
                            U2LenTx ++;
                            IQFrameTx[U2LenTx] = LF;
                            U2LenTx ++;
                            U2PtrTx = 0;
                            SCI6.TDR = IQFrameTx[0]; 
                            VncRdfOff;              /* Mandatory */
                            VncRfPtr ++;
                        }
                    } 
                }
                else
                {          
                    VncFileOffset = VncFileOffset + 240;
                }
            }
        }      
        if ((VncRdf == 0) && (0 < VncRfPtr) && (VncRfPtr <= 3) && (U0Car == 0) && (U2XonXoff == 0)) 
        {
            /* No data, launch command, buffer empty, Xon */   
            R_tfat_f_lseek(&fl_file, VncFileOffset);
            R_tfat_f_read(&fl_file, VncData, 240, &fl_file_rw_cnt);
            VncRdfOn;
        }
        break ;
    }
}


void  usb_RfPrgChrono (void)
{  
    switch (UsbPtr)
    {
    case USB_RFPROG_INIT :
        UsbPtr = USB_RFPROG_OPEN;
        break;

    case USB_RFPROG_OPEN :
        usb_RfPrgOpen();
        break;
        
    case USB_RFPROG_READ :
        usb_RfPrgRead();
        break;
        
        default :
        UsbPtr = 0;
        UsbRfProgOff;
        break;
    }

    if (UsbPtr != 0)
    usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITE;
    else
    usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITEOK;
    usb_hmsc_SampleAplSpecifiedPath(mes);

}

uchar CrcMot(uchar *v)
{
    /* Calculate and add crc of v (v = 'S0030000' + crc) */
    /* Return 0 if crc is OK */
    uchar i;
    uchar c;
    c = 0;
    i = 0;
    do
    {
        c = c + HexUchar(v + 2 * i + 2);  
        i ++;
    }
    while (i < HexUchar(v + 2));    
    sprintf((char *)v + 2 + 2 * i, "%2.2hX", 0xFF - c);
    c = c + HexUchar(v + 2 * i + 2);  
    return c;    
}

uint32_t CheckFreeSpace(void)
{
    uint32_t clust, /*tot_disk_space,*/ free_disk_space;
    FATFS   *usbfs;
    FRESULT res;

    res = R_tfat_f_getfree ("", &clust, &usbfs);
    /*tot_disk_space = (usbfs->max_clust - 2) * usbfs->csize / 2;*/
    if (res == TFAT_FR_OK)
    {
        free_disk_space = clust * usbfs->csize;
        return free_disk_space;
    }
    else
    return 0;

}

uint HexUint(uchar *i)
{
    /* Convert Hex to uint */
    /* *i : adress of the low order char */
    uint j;
    sscanf((const char *)i, "%4x", &j);
    return j;
}

void usb_CfgChrono(void)
{
    uchar c[256];
    uchar n;                                
    uint i;
    uchar j;
    uchar *d;
    uchar TempSeg[64];
    StructMemo MemoTemp;                    /* Temp memo for EEprom to USB transfer */
    FILINFO filinfo;

    /* Save config */
    switch (VncCfgPtrSave)
    {
    case 0 :
        break ;  
    case 1 :
        CsvNewFileName(3, &UsbProfileFileName[0]);         /* Creates a new file name */
        /* Create a file object. */
        res = R_tfat_f_open(&file, &UsbProfileFileName[0], (TFAT_FA_CREATE_ALWAYS | TFAT_FA_WRITE));
        if( res != TFAT_FR_OK )
        {
            VncCfgPtrSave = 0;
            break;
        }
        /* Fist line of file */
        i = sprintf((char *)&c[0], "S0030000");
        n = CrcMot(&c[0]);
        c[i + 2] = CR; 
        c[i + 3] = LF; 
        res = R_tfat_f_write(&file, c, (i + 4), &file_rw_cnt);
        if( res != TFAT_FR_OK )
        {
            VncCfgPtrSave = 0;
            break;
        }
        VncCfgPtrSave ++;
        res = R_tfat_f_sync(&file);
        filinfo.fdate = ((State.DateR.Year - 1980) << 9) + (State.DateR.Month << 5) + State.DateR.Date;
        filinfo.ftime = (State.DateR.Hr << 11) + (State.DateR.Min << 5) + (State.DateR.Sec / 2);
        R_tfat_f_utime (&UsbProfileFileName[0], &filinfo);
        usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITE;
        usb_hmsc_SampleAplSpecifiedPath(mes);
        
        break ;
        default :       /* Data lines : 62 bytes per segment */
        if ((VncCfgPtrSave - 2) < SEGCONFIGTOP)
        {
            Read_SegFlashData (VncCfgPtrSave - 2, (uchar *)&MemoTemp);
            
            /* First line of data : 32 bytes */
            i = sprintf((uchar *)&c[0], "S123%4.4X", (VncCfgPtrSave - 2) * 64);
            n = 0;
            do
            {
                i = i + sprintf((uchar *)&c[i], "%2.2X", *((uchar *)&MemoTemp + n)); 
                n ++;
            }
            while (n < 32);
            j = CrcMot(&c[0]);
            i = i + 2;
            c[i ++] = CR; 
            c[i ++] = LF;
            /* Second line of data : 32 bytes*/
            j = i;
            i = i + sprintf((uchar *)&c[i], "S123%4.4X", (VncCfgPtrSave - 2) * 64 + 32);
            do
            {
                i = i + sprintf((uchar *)&c[i], "%2.2X", *((uchar *)&MemoTemp + n)); 
                n ++;
            }
            while (n < 64);
            j = CrcMot(&c[j]);
            i = i + 2;
            c[i ++] = CR; 
            c[i ++] = LF; 
            res = R_tfat_f_write(&file, c, i, &file_rw_cnt);
            if( res != TFAT_FR_OK )
            {
                VncCfgPtrSave = 0;
                break;
            }
            VncCfgPtrSave ++;
            res = R_tfat_f_sync(&file);
            filinfo.fdate = ((State.DateR.Year - 1980) << 9) + (State.DateR.Month << 5) + State.DateR.Date;
            filinfo.ftime = (State.DateR.Hr << 11) + (State.DateR.Min << 5) + (State.DateR.Sec / 2);
            R_tfat_f_utime (&UsbProfileFileName[0], &filinfo);
            usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITE;
            usb_hmsc_SampleAplSpecifiedPath(mes);
        }    
        else
        {                       /* End of file indicator */
            i = sprintf(&c[0], "S9030000");
            n = CrcMot(&c[0]);
            res = R_tfat_f_write(&file, c, (i + 2), &file_rw_cnt);
            VncCfgPtrSave = 0;
            res = R_tfat_f_sync(&file);
            filinfo.fdate = ((State.DateR.Year - 1980) << 9) + (State.DateR.Month << 5) + State.DateR.Date;
            filinfo.ftime = (State.DateR.Hr << 11) + (State.DateR.Min << 5) + (State.DateR.Sec / 2);
            R_tfat_f_utime(&UsbProfileFileName[0], &filinfo);
            usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITEOK;
            usb_hmsc_SampleAplSpecifiedPath(mes);
        } 
        break;  
    }

    switch (VncCfgPtrOpen)
    {
    case 0 :
        break ;  
    case 1 :
        VncRdfOff;              /* Mandatory for launching first read */      
        VncFileOffset = 0;      /* Beginning of the file */
        VncCfgPtrOpen ++;
        usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITE;
        usb_hmsc_SampleAplSpecifiedPath(mes);
        break ;  
    case 2 :
        /* Data are stored in VncData[256], number is */
        if (VncRdf != 0)        /* Test for available data */   
        {
            /* Search for S0 */
            d = (uchar *)strstr(&VncData[0], &S0[0]);
            if (d != 0)           /* S0 sequence detected */
            {
                if (CrcMot(d) == 0xFF)
                {
                    VncFileOffset = VncFileOffset +  2 * HexUchar(d + 2) + 5;
                    VncCfgPtrOpen ++;
                    VncRdfOff;        /* Mandatory for launching first read */      
                }
            }
            else
            {
                VncCfgPtrOpen = 0;
                VncRdfOff;          /* Mandatory if launch function again */      
            }
        }
        if ((VncRdf == 0) && (VncCfgPtrOpen != 0))                   /* No data, launch command */   
        {
            /* Test for Vncseq and Vnccmd inside */
            /* Open the file to be read */        
            sprintf ((char *)&v[0], "RXIQCFG.MOT");
            res = R_tfat_f_open(&fl_file, v, TFAT_FA_OPEN_EXISTING | TFAT_FA_READ);
            if( res != TFAT_FR_OK )
            {
                VncCfgPtrOpen = 0;
                break;
            }
            R_tfat_f_lseek(&fl_file, VncFileOffset);
            R_tfat_f_read(&fl_file, VncData, 240, &fl_file_rw_cnt);
            VncRdfOn;
            //j = VncRead(&VncDataFileName[0], VncFileOffset, 192);
        } 
        usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITE;
        usb_hmsc_SampleAplSpecifiedPath(mes); 
        break ;                      
    case 3 :       
        /* Data are stored in VncData[256], number is */
        if (VncRdf != 0)        /* Test for available data */   
        {
            d = (uchar *)strstr(&VncData[0], &S1[0]);
            if (d != 0)                                                 /* S1 sequence detected */
            {
                if (CrcMot(d) == 0xFF)                                    /* Crc OK */
                {
                    if (((HexUint(d + 4) % 32) == 0) && /* Adress OK */
                            ((HexUint(d + 4) / 64) != SEGSERIAL) &&               /* Segment number <> SEGSERIAL */
                            ((HexUint(d + 4) / 64) != SEGWDDATAR) &&              /* Segment number <> SEGWDDATAR */
                            ((HexUint(d + 4) / 64) != SEGWDDATAW) &&              /* Segment number <> SEGWDDATAW */
                            ((HexUint(d + 4) / 64) != SEGWDSTATE) &&              /* Segment number <> SEGWDSTATE */
                            ((HexUint(d + 4) / 64) != SEGSTATUS) &&               /* Segment number <> SEGSTATUS */
                            ((HexUint(d + 4) / 64) != SEGWDCHGDATA))              /* Segment number <> SEGWDCHGDATA */
                    {
                        Read_SegFlashData (HexUint(d + 4) / 64, (uchar *)&TempSeg[0]);
                        //if (I2CSegRead(ADREEP, HexUint(d + 4) / 64, &TempSeg[0]) != 0)
                        //  memset(TempSeg, 0, 64);
                        if ((HexUchar(d + 6) % 64) == 0) 
                        {
                            j = 0;
                            do
                            {
                                TempSeg[j] = HexUchar(d + 2 * j + 8);  
                                j ++;
                            }                  
                            while (j < 32);
                        }
                        else
                        {
                            j = 0;
                            do
                            {
                                TempSeg[j + 32] = HexUchar(d + 2 * j + 8);  
                                j ++;
                            }
                            while (j < 32);
                        }
                        Write_SegFlashData(HexUint(d + 4) / 64, &TempSeg[0]);
                        //j = I2CSegWrite(ADREEP, HexUint(d + 4) / 64, &TempSeg[0]);                            
                    }
                    VncFileOffset = VncFileOffset + 2 * HexUchar(d + 2) + 6;
                    VncRdfOff;                /* Mandatory */
                }
            }
            else
            {              
                d = (uchar *)strstr(&VncData[0], &S9[0]);
                if (d != 0)                 /* S9 sequence detected */
                {
                    if (CrcMot(d) == 0xFF)
                    {
                        ReloadConfigOn;         /* Force to reload new config */                
                        VncRdfOff;              /* Mandatory */
                        VncCfgPtrOpen = 0;
                        usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITEOK;
                        usb_hmsc_SampleAplSpecifiedPath(mes);
                    }
                }
                else
                {          
                    VncFileOffset = VncFileOffset + 192;
                    /*if (VncSize <= VncFileOffset)
            {
                VncCfgPtrOpen = 0;  
                VncRdfOff;          
            } */
                }
            }
        }      
        if ((VncRdf == 0) && (VncCfgPtrOpen != 0))                   /* No data, launch command */   
        {
            /* Test for Vncseq and Vnccmd inside */
            R_tfat_f_lseek(&fl_file, VncFileOffset);
            R_tfat_f_read(&fl_file, VncData, 240, &fl_file_rw_cnt);
            VncRdfOn;
            usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITE;
            usb_hmsc_SampleAplSpecifiedPath(mes);
            //j = VncRead(&VncDataFileName[0], VncFileOffset, 192);
        }
        break ;
    }
}

void usb_E1MemoWriteChrono (void)	// MODIF R3.3
{

    switch (UsbPtr)
    {
    case USB_MEMO_INIT :
        usb_E1MemoWriteInit();
        break;

    case USB_MEMO_WRITE :
        usb_E1MemoAdd();
        break;

    case USB_MEMO_COMPLETE :
        usb_E1MemoClose();
        UsbE1MemoOff;
        UsbPtr = 0;
        break;

        default :
        UsbPtr = 0;
        break;
    }

    if (UsbPtr != 0)
    usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITE;
    else
    usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITEOK;
    usb_hmsc_SampleAplSpecifiedPath(mes);

}


void usb_E1MemoWriteInit (void)	// MODIF R3.3
{
    usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITE;
    CsvNewFileName(5, &UsbMemoFileName[0]);         /* Creates a new file name */

    res = R_tfat_f_open(&file, &UsbMemoFileName[0], (TFAT_FA_CREATE_ALWAYS | TFAT_FA_WRITE));
    if( res != TFAT_FR_OK )
    {
        USB_PRINTF1("R_tfat_f_open error: %d\n", res);
        UsbPtr = 0;
        UsbE1MemoOff;
    }
    else
    {
        UsbPtr = USB_MEMO_WRITE;
        UsbMemoPtr = E1Status.EventPointer;
        UsbMemoSeq = 0;
    }

}


void usb_E1MemoClose (void)	// MODIF R3.3
{
    FILINFO filinfo;

    filinfo.fdate = ((State.DateR.Year - 1980) << 9) + (State.DateR.Month << 5) + State.DateR.Date;
    filinfo.ftime = (State.DateR.Hr << 11) + (State.DateR.Min << 5) + (State.DateR.Sec / 2);
    /* Close the file object. */
    res = R_tfat_f_close(&file);
    R_tfat_f_utime (&UsbMemoFileName[0], &filinfo);
    if( res != TFAT_FR_OK )
    {
        USB_PRINTF1("R_tfat_f_close error: %d\n", res);
    }

    /* Message next time we come back here is.. */
    usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITEOK;
    usb_hmsc_SampleAplSpecifiedPath(mes);
}

void usb_E1MemoAdd (void)	// MODIF R3.3
{
    uint i;

    uint Bytes2Writes;
    StructE1Memo MemoUsb;

    switch (UsbMemoSeq)
    {
    case 0 :
        if  (CheckFreeSpace() > 1000) // check if 1Mbytes freespace
        {
            UsbMemoSeq++;
            CsvConfigLine1(1, &usb_gBuf[0], &Bytes2Writes);	// header
            res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
        }
        else
        {
            UsbPtr = 0;
            UsbE1MemoOff;
        }
        break;

    case 1 :
        UsbMemoSeq++;
        CsvConfigLine1(0, &usb_gBuf[0], &Bytes2Writes);
        res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
        break;

    case 2 :
        UsbMemoSeq++;
        CsvConfigLine2(1, &usb_gBuf[0], &Bytes2Writes);	// header
        res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
        break;

    case 3 :
        UsbMemoSeq++;
        CsvConfigLine2(0, &usb_gBuf[0], &Bytes2Writes);
        res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
        break;

    case 4 :
        UsbMemoSeq++;
        CsvStatusLine(1, &usb_gBuf[0], &Bytes2Writes);	// header
        res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
        break;

    case 5 :
        UsbMemoSeq++;
        CsvStatusLine(0, &usb_gBuf[0], &Bytes2Writes);
        res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
        break;

    case 6 :
        UsbMemoSeq++;
        UsbMemoPtr = SEGE1TOP;
        CsvMemoE1Line(1, &usb_gBuf[0], &Bytes2Writes, &MemoUsb);
        res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
        break;

    case 7 :
        if (E1Status.NbRstPointer == 0)
        {

            if (E1Status.EventPointer < UsbMemoPtr)
            {
                UsbMemoPtr = 0;                             /* End of writing */
                UsbPtr = USB_MEMO_COMPLETE;
            }
            else
            {
                Read_SegFlashData (UsbMemoPtr, (uchar *)&MemoUsb);
                CsvMemoE1Line(0, &usb_gBuf[0], &Bytes2Writes, &MemoUsb);
                res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
                UsbMemoPtr++;
            }
        }
        else
        {
            i = (((E1Status.EventPointer - SEGE1TOP) + (UsbMemoPtr - SEGE1TOP) + 1) % SEGE1NB) + SEGE1TOP;
            Read_SegFlashData (i, (uchar *)&MemoUsb);
            CsvMemoE1Line(0, &usb_gBuf[0], &Bytes2Writes, &MemoUsb);
            res = R_tfat_f_write(&file, usb_gBuf, Bytes2Writes, &file_rw_cnt);
            UsbMemoPtr++;
            if ((SEGE1TOP + SEGE1NB) <= UsbMemoPtr)
            {
                UsbMemoPtr = 0;                             /* End of writing */
                UsbPtr = USB_MEMO_COMPLETE;
            }
        }
        break;
    }

    if (res != TFAT_FR_OK)
    {
        UsbPtr = 0;
        UsbE1MemoOff;
    }
}

/******************************************************************************
End  Of File
******************************************************************************/
