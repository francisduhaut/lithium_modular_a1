#include "include.h"

// FFE00000   2Mbytes
// FFFDBFD0 -> With Boot loader

// FFE00000   2Mbytes
// FFFFFFD0 -> Without Boot loader.

//#define PROG_EXT_FLASH


/******************************************************************************
Section    <Section Definition> , "Project Sections"
 ******************************************************************************/

void usb_cstd_task_start( void );
void usb_cstd_IdleTaskStart(void);

/* Condition compilation by the difference of the devices */
#if (USB_CPU_LPW_PP == USB_LPWR_USE_PP)
extern uint16_t usb_gcpu_RemoteProcess;
#endif /* (USB_CPU_LPW_PP == USB_LPWR_USE_PP) */


extern void USB_Chrono (void);
extern void usb_cpu_WdogRefresh(void);

uint16_t at;
uint16_t atd;
uint16_t datt[200];

CFIstruct CFI;

uint32_t i = 0,j = 0,kz = 0,Address = 0; 
uint16_t temp;

/******************************************************************************
Function Name   : usb_cstd_IdleTaskStart
Description     : Idle Task Start process
Arguments       : none
Return value    : none
 ******************************************************************************/
void usb_cstd_IdleTaskStart(void)
{
	/* Condition compilation by the difference of the devices */
#if (USB_CPU_LPW_PP == USB_LPWR_USE_PP)
	R_usb_cstd_SetTaskPri(USB_IDL_TSK, USB_PRI_6);
	USB_SND_MSG(USB_IDL_MBX, 0);
#endif /* (USB_CPU_LPW_PP == USB_LPWR_USE_PP) */
}


/******************************************************************************
Function Name   : usb_cstd_IdleTask
Description     : Idle Task (sleep sample)
Arguments       : USB_VP_INT stacd      : task start code(not use)
Return value    : none
 ******************************************************************************/
void usb_cstd_IdleTask(USB_VP_INT stacd)
{
	/* Condition compilation by the difference of USB function */
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
	/* nothing */
#else /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */
	/* Condition compilation by the difference of the devices */
#if (USB_CPU_LPW_PP == USB_LPWR_USE_PP)
	void        usb_cpu_GoLpwrSleepMode(void);
	void        usb_cpu_GoDeepStbyMode(ptr);
	uint16_t    res[8], sts0;
	uint8_t     stby;
	USB_UTR_t utr0, *ptr0;

	USB_UTR_t   *mess;
	USB_ER_t    err;
	uint16_t    usb_idle;

	usb_idle = USB_OFF;

	ptr0 = (USB_UTR_t *)&utr0;

	ptr0->ip  = USB_PERI_USBIP_NUM;
	ptr0->ipp = usb_cstd_GetUsbIpAdr( ptr0->ip );

	/* Idle Task message receive (port0) */
	err = USB_TRCV_MSG(USB_IDL_MBX, (USB_MSG_t**)&mess, (USB_TM_t)0);
	if( (err != USB_E_OK) && (err != USB_E_TMOUT) )
	{
		USB_PRINTF1("### HSMP driver rcv_msg error %ld\n", err);
	}
	else
	{
		/* Send message to IDL_TSK */
		usb_idle = USB_ON;
		USB_SND_MSG(USB_IDL_MBX, 0);
	}

	if( usb_idle == USB_ON )
	{

		/* Condition compilation by the difference of USB function */
#if USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP
		R_usb_pstd_DeviceInformation(ptr0, (uint16_t *)res);
#endif /* USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP */
		/* Condition compilation by the difference of USB function */

		sts0 = res[0];
		if( (sts0 & USB_VBSTS) == 0 )
		{
			sts0 = USB_DETACHED;    /* Port0 detach */
		}
		else if( (sts0 & USB_DS_SPD_CNFG) == USB_DS_SPD_CNFG )
		{
			sts0 = USB_SUSPENDED;   /* Port0 suspend */
		}
		else
		{
			sts0 = USB_PORTOFF;
		}

		if( sts0 == USB_DETACHED )
		{
			/* Detach */
			usb_gcpu_RemoteProcess = USB_OFF;
			/* Get Deep Software Standby Reset Flag */
			stby = usb_cpu_GetDPSRSTF();
			if( stby == 0 )
			{
				/* Deep Standby shift */
				usb_cpu_GoLpwrDeepStby(ptr0);
			}
		}
		else if( sts0 == USB_SUSPENDED )
		{
			/* Suspend */
			usb_cpu_GoLpwrSleep(ptr0);
		}
		else
		{
			usb_gcpu_RemoteProcess = USB_OFF;
		}
	}
#else /* (USB_CPU_LPW_PP == USB_LPWR_USE_PP) */
#endif /* (USB_CPU_LPW_PP == USB_LPWR_USE_PP) */
#endif /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */
}


/******************************************************************************
Function Name   : R_usb_cstd_IdleTask
Description     : Call Idle Task (sleep sample)
Arguments       : USB_VP_INT stacd      : task start code(not use)
Return value    : none
 ******************************************************************************/
void R_usb_cstd_IdleTask(USB_VP_INT stacd)
{
	usb_cstd_IdleTask( stacd );
}


/******************************************************************************
Function Name   : main
Description     : main task process
Arguments       : USB_VP_INT stacd 
Return value    : none
 ******************************************************************************/
void main(USB_VP_INT stacd)
{

#ifdef PROG_EXT_FLASH	
	uint8_t ExternalImage = 255;
#endif  

	// USB Host init
	usb_cstd_TargetInit();

	// Charge process init
	Init_Charger();
	Init_MOD();
	// Driver init
//#ifndef DEMO
	Init_CAN();
	Init_CAN_bms();
//#endif
	Init_FlashData();
	InitGraph();
	Init_Timer();

	// Start USB
	usb_cstd_task_start();
	usb_cpu_WdogRefresh();

	InitFlash();
	Init_COMIQ();

	MaskPasswdTmpOff;
	MaskPasswdHighOn;
	MaskPasswdLowOn;
	MaskPasswd2On;
	MaskPasswd3On;

	E2FLASH_Chrono();   // MODIF A2.3 -> init uart only after read config
	Uart0Init();

#ifdef PROG_EXT_FLASH	
	UsbWriteMemoOn;
#endif  

	while(1)
	{
		SCIIQ_Chrono();
		IQJbusChrono();
		CAN_Chrono();
		//CAN_bms_Chrono();
		E2FLASH_Chrono();
		LCD_Chrono();

        #ifndef PROG_EXT_FLASH	
		TFT_Chrono();
        #endif	

		KEY_Chrono();
		RTC_Chrono();
		USB_Chrono();
        #ifndef DEMO
        MOD_Chrono();
        #endif
		CHARGE_Chrono();
		CHARGE_DefChrono();
		if (ManuReg == 0)
			Uart0Chrono();
        #ifndef DEMO
		BootloaderChrono();
        #endif

#ifdef PROG_EXT_FLASH	
		// External Image update procedure (only for DEBUG mode)
		// 1. uncomment #define PROG_EXT_FLASH
		// 2. connect USB key with external.hex
		// 3. copy external.c & external.h to Driver folder
		// 4. Build and download
		// 5. Reset & Go
		if (ExternalImage != 0)
		{
			ExternalImage = ProgramExternalImages();
		}
#endif	
		// Wdog update
		usb_cpu_WdogRefresh();
	}
}
