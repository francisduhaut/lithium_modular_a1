#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <machine.h>
#include "rx_rsk_extern.h"              /* RX63NRSK Extern Header */

/* System definition */
#include "r_usb_usrconfig.h"            /* USB-H/W register set (user define) */
#include "r_usb_ctypedef.h"             /* Type define */
#include "r_usb_cdefusbip.h"            /* USB-FW Library Header */
#include "r_usb_cmacprint.h"            /* Standard IO macro */
#include "r_usb_cmacsystemcall.h"       /* System call macro */
#include "r_usb_cextern.h"              /* USB-FW global define */
#include "iodefine.h"

#include "r_usb_ckernelid.h"        	/* Kernel ID definition */

#include "include.h"

/******************************************************************************
Constant macro definitions
 ******************************************************************************/
/* Select Use IP */
#define USB_PswIntDisable               (uint32_t)(7 << 24) /* Processer Status Word - IPL(Level7) */
#define USB_PswIntSleep                 (uint32_t)(1 << 24) /* Processer Status Word - IPL(Level1) */



/******************************************************************************
Section    <Section Definition> , "Project Sections"
 ******************************************************************************/
#pragma address MDEreg=0xffffff80 /* MDE register (Single Chip Mode) */
#ifdef __BIG
const unsigned long MDEreg = 0xfffffff8; /* big */
#else   /* __BIG */
const unsigned long MDEreg = 0xffffffff; /* little */
#endif  /* __BIG */

/* Condition compilation by the difference of the devices */
#if USB_TRANS_MODE_PP == USB_TRANS_DTC_PP
#pragma section _DTCtable


/******************************************************************************
Typedef definitions
 ******************************************************************************/

/******************************************************************************
Bit Order Definition "LEFT"
 ******************************************************************************/
#pragma bit_order left

/* DTC Vector table */
typedef struct
{
	/* Condition compilation by the difference of the endian */
#if USB_CPUBYTE_PP == USB_BYTE_LITTLE_PP
	unsigned short WORD;
	union
	{
		unsigned char BYTE;
		struct
		{
			unsigned char CHNE:1;
			unsigned char CHNS:1;
			unsigned char DISEL:1;
			unsigned char DTS:1;
			unsigned char DM:2;
			unsigned char :2;
		}
		BIT;
	}
	MRB;                        /* Mode Register B */
	union
	{
		unsigned char BYTE;
		struct
		{
			unsigned char MD:2;
			unsigned char SZ:2;
			unsigned char SM:2;
			unsigned char :2;
		}
		BIT;
	}
	MRA;                        /* Mode Register A */
	unsigned long  SAR;         /* Source Address */
	unsigned long  DAR;         /* Destination Address */
	unsigned short CRB;         /* Count Register A */
	unsigned short CRA;         /* Count Register B */
#endif /* USB_CPUBYTE_PP == USB_BYTE_LITTLE_PP */

	/* Condition compilation by the difference of the endian */
#if USB_CPUBYTE_PP == USB_BYTE_BIG_PP
	union
	{
		unsigned char BYTE;
		struct
		{
			unsigned char MD:2;
			unsigned char SZ:2;
			unsigned char SM:2;
			unsigned char :2;
		}
		BIT;
	}
	MRA;                        /* Mode Register A */
	union
	{
		unsigned char BYTE;
		struct
		{
			unsigned char CHNE:1;
			unsigned char CHNS:1;
			unsigned char DISEL:1;
			unsigned char DTS:1;
			unsigned char DM:2;
			unsigned char :2;
		}
		BIT;
	}
	MRB;                        /* Mode Register B */
	unsigned short WORD;
	unsigned long  SAR;         /* Source Address */
	unsigned long  DAR;         /* Destination Address */
	unsigned short CRA;         /* Count Register A */
	unsigned short CRB;         /* Count Register B */
#endif /* USB_CPUBYTE_PP == USB_BYTE_BIG_PP */
} USB_DTC_t;

/******************************************************************************
Bit Order Definition default
 ******************************************************************************/
#pragma bit_order


typedef struct st_usb           USB_STBY_t;


/******************************************************************************
Private global variables and functions
 ******************************************************************************/
/*
 *  Must be set 0 by the Address Low 2bit. (4byte alignment)
 */
/* DTC Control Register */
USB_DTC_t   usb_dtcreg[2u];

/*
 *  Must be set 0 by the Address Low 12bit. (0x???? ?000)
 */
/* DTC VECTOR Table  */
uint32_t    usb_dtcctable[48] =
{
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR  0 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR  1 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR  2 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR  3 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR  4 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR  5 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR  6 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR  7 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR  8 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR  9 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 10 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 11 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 12 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 13 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 14 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 15 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 16 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 17 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 18 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 19 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 20 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 21 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 22 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 23 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 24 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 25 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 26 */
		(uint32_t)0x00000000,       /* DTC VECTOR 27 (SWINT) */
		(uint32_t)0x00000000,       /* DTC VECTOR 28 (CMT0) */
		(uint32_t)0x00000000,       /* DTC VECTOR 29 (CMT1) */
		(uint32_t)0x00000000,       /* DTC VECTOR 30 (CMT2) */
		(uint32_t)0x00000000,       /* DTC VECTOR 31 (CMT3) */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 32 */
		(uint32_t)&usb_dtcreg[0],   /* DTC VECTOR 33 (USB0-D0FIFO) */
		(uint32_t)0x00000000,       /* DTC VECTOR 34 (USB0-D1FIFO) */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 35 */
		(uint32_t)&usb_dtcreg[1],   /* DTC VECTOR 36  (USB1-D0FIFO) */
		(uint32_t)0x00000000,       /* DTC VECTOR 37  (USB1-D1FIFO) */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 38 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 39 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 40 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 41 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 42 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 43 */
		(uint32_t)0xFFFFFFFF,       /* DTC VECTOR 44 */
		(uint32_t)0x00000000,       /* DTC VECTOR 45 (SPRI0) */
		(uint32_t)0x00000000,       /* DTC VECTOR 46 (SPTI1) */
		(uint32_t)0xFFFFFFFF        /* DTC VECTOR 47 */
};
#endif  /* USB_TRANS_MODE_PP == USB_TRANS_DTC_PP */


/******************************************************************************
Section    <Section Definition> , "Project Sections"
 ******************************************************************************/
#pragma section _rx_rsk


/******************************************************************************
Private global variables and functions
 ******************************************************************************/
/*=== SYSTEM ================================================================*/
void        usb_cstd_TargetInit(void);
void        usb_cpu_FunctionUSB0IP(void);
void        usb_cpu_FunctionUSB1IP(void);
/*=== Interrupt =============================================================*/
void        usb_cpu_UsbintInit(void);
void        usb_cpu_DmaintInit(void);
void        usb_cpu_usb_int_hand(void);
void        usb2_cpu_usb_int_hand(void);
void        usb_cpu_d0fifo_int_hand(void);
void        usb2_cpu_d0fifo_int_hand(void);
void        usb_cpu_int_enable(USB_UTR_t *ptr);
void        usb_cpu_int_disable(USB_UTR_t *ptr);
/*=== DMA ===================================================================*/
void        usb_cpu_d0fifo2buf_start_dma(USB_UTR_t *ptr, uint32_t SourceAddr);
void        usb_cpu_buf2d0fifo_start_dma(USB_UTR_t *ptr, uint32_t DistAdr);
void        usb_cpu_d0fifo_stop_dma(USB_UTR_t *ptr);
uint16_t    usb_cpu_get_dtc_block_count(USB_UTR_t *ptr);
void        usb_cpu_d0fifo_restart_dma(USB_UTR_t *ptr);
void        usb_cpu_d0fifo_enable_dma(USB_UTR_t *ptr );
void        usb_cpu_d0fifo_disable_dma(USB_UTR_t *ptr );
/*=== Standby control =======================================================*/
void        usb_cpu_GoDeepStbyMode(USB_UTR_t *ptr);
void        usb_cpu_GoWait(void);
void        usb_cpu_GoDeepStbyMode0(USB_UTR_t *ptr);
void        usb_cpu_RegRecovEnable(USB_UTR_t *ptr);
void        usb_cpu_RegRecovDisable(USB_UTR_t *ptr);
/*=== IRQ ===================================================================*/
void        usb_cpu_IRQ2_Enable(USB_UTR_t *ptr);
void        usb_cpu_IRQ2_DisEnable(void);
void        usb_cpu_IRQ8_Enable(void);
void        usb_cpu_IRQ8_DisEnable(void);
void        usb_cpu_IRQ2Int(void);
/*=== TIMER =================================================================*/
void        usb_cpu_Delay1us(uint16_t time);
void        usb_cpu_DelayXms(uint16_t time);
/*=== SDRAM =================================================================*/
void        usb_cpu_SdramInit(void);
/*=== KEY ===================================================================*/
void        usb_cpu_KeyInit(void);
/*=== I/O ===================================================================*/
void usb_cpu_IOInit (void);
/*=== LED ===================================================================*/
void        usb_cpu_LedInit(void);
/*=== LCD ===================================================================*/
void        usb_cpu_LcdInit(void);
/*=== AD ====================================================================*/
void        usb_cpu_AdInit(void);
uint32_t    usb_cpu_AdData(void);
/*=== WDOG ==================================================================*/
void        usb_cpu_WdogInit(void);
void        usb_cpu_WdogRefresh(void);


/* Condition compilation by the difference of the devices */
#if (USB_CPU_LPW_PP == USB_LPWR_USE_PP)
uint16_t    usb_gcpu_RemoteProcess = USB_OFF;
#endif /* (USB_CPU_LPW_PP == USB_LPWR_USE_PP) */

#pragma interrupt usb_cpu_usb_int_hand (vect = VECT(USB0, USBI0))
#pragma interrupt usb2_cpu_usb_int_hand (vect = VECT(USB1, USBI1))
#pragma interrupt usb_cpu_d0fifo_int_hand (vect = VECT(USB0, D0FIFO0))
#pragma interrupt usb2_cpu_d0fifo_int_hand (vect = VECT(USB1, D0FIFO1))
#pragma interrupt usb_cpu_GoWait (vect = 11)
#pragma interrupt usb_cpu_IRQ2Int (vect = VECT(ICU, IRQ2))


/******************************************************************************
Renesas Abstracted RSK functions
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_TargetInit
Description     : Target System Initialize
Arguments       : none
Return value    : none
 ******************************************************************************/
void usb_cstd_TargetInit( void )
{
	/* Enable Interrupt */
	usb_cpu_UsbintInit();           /* Initialized USB interrupt  */
	usb_cpu_DmaintInit();           /* Initialized DMA interrupt  */
	usb_cpu_WdogInit();
	usb_cpu_LedInit();              /* Initialized LED */
	usb_cpu_KeyInit();              /* Initialized KEY */
	//usb_cpu_LcdInit();              /* Initialized LCD */
	usb_cpu_IOInit();               /* Initialized I/O */
}
/******************************************************************************
End of function usb_cstd_TargetInit
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_FunctionUSB0IP
Description     : USB0 port mode and Switch mode Initialize
Arguments       : none
Return value    : none
 ******************************************************************************/
void usb_cpu_FunctionUSB0IP(void)
{
	/* Write protect register
    b5-b0 Reserved 0
    b6 PFSWE    PFS register write enable bit
    b7 BOWI     PFSWE bit write disable bit
	 */
	MPC.PWPR.BIT.B0WI   = 0u;
	MPC.PWPR.BIT.PFSWE  = 1u;


	/* Port mode register
    b0    B0       Pm0 port mode control bit
    b1    B1       Pm1 port mode control bit
    b2    B2       Pm2 port mode control bit
    b3    B3       Pm3 port mode control bit
    b4    B4       Pm4 port mode control bit
    b5    B5       Pm5 port mode control bit
    b6    B6       Pm6 port mode control bit
    b7    B7       Pm7 port mode control bit
	 */
	PORT1.PMR.BIT.B4    = 1u;
	PORT1.PMR.BIT.B6    = 1u;

#if USB_FUNCSEL_USBIP0_PP == USB_PERI_PP
	/* Pmm port function select register
    b0-b4 PSEL     port function select bit
    b5    Reserved 0
    b6    ISEL     Interrupt select bit
    b7    Reserved 0
	 */
	MPC.P14PFS.BYTE = 0x11; /* USB0_DPUPE */
	MPC.P16PFS.BYTE = 0x11; /* USB0_VBUS */

	/* USB0 control register
    b1-b0 Reserved 0
    b2    PUPHZS   PUPHZS select bit
    b3    PDHZS    PDHZS select bit
    b4-b7 Reserved 0
	 */
	MPC.PFUSB0.BIT.PUPHZS   = 1u;
#elif USB_FUNCSEL_USBIP0_PP == USB_HOST_PP
	/* Pmm port function select register
    b0-b4 PSEL     port function select bit
    b5    Reserved 0
    b6    ISEL     Interrupt select bit
    b7    Reserved 0
	 */
	MPC.P14PFS.BYTE = 0x12; /* USB0_OVRCURA */
	MPC.P16PFS.BYTE = 0x11; /* USB0_VBUS */
	//MPC.P24PFS.BYTE = 0x13; /* USB0_VBUSEN */
	PORT2.PDR.BIT.B4 = 1;
	PORT2.PODR.BIT.B4 = 1;
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_PERI_PP */

	/* Write protect register
    b5-b0 Reserved 0
    b6 PFSWE    PFS register write enable bit
    b7 BOWI     PFSWE bit write disable bit
	 */
	MPC.PWPR.BIT.PFSWE  = 0u;
	MPC.PWPR.BIT.B0WI   = 1u;
}
/******************************************************************************
End of function usb_cpu_FunctionUSB0IP
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_FunctionUSB1IP
Description     : USB1 port mode and Switch mode Initialize
Arguments       : none
Return value    : none
 ******************************************************************************/
void usb_cpu_FunctionUSB1IP(void)
{
	/* Write protect register
    b5-b0 Reserved 0
    b6 PFSWE    PFS register write enable bit
    b7 BOWI     PFSWE bit write disable bit
	 */
	MPC.PWPR.BIT.B0WI   = 0u;
	MPC.PWPR.BIT.PFSWE  = 1u;

	/* Port mode register (Enable USB1 VBUS, DPRPU)
    b0    B0       Pm0 port mode control bit
    b1    B1       Pm1 port mode control bit
    b2    B2       Pm2 port mode control bit
    b3    B3       Pm3 port mode control bit
    b4    B4       Pm4 port mode control bit
    b5    B5       Pm5 port mode control bit
    b6    B6       Pm6 port mode control bit
    b7    B7       Pm7 port mode control bit
	 */
	PORT1.PMR.BIT.B5    = 1u;
	PORT1.PMR.BIT.B7    = 1u;

	/* Pmm port function select register
    b0-b4 PSEL     port function select bit
    b5    Reserved 0
    b6    ISEL     Interrupt select bit
    b7    Reserved 0
	 */
	MPC.P15PFS.BYTE = 0x11u;    /* USB1_DPUPE */
	MPC.P17PFS.BYTE = 0x11u;    /* USB1_VBUS */

	/* USB0 control register
    b1-b0 Reserved 0
    b2    PUPHZS   PUPHZS select bit
    b3    PDHZS    PDHZS select bit
    b4-b7 Reserved 0
	 */
	MPC.PFUSB1.BIT.PUPHZS   = 1u;

	/* Write protect register
    b5-b0 Reserved 0
    b6 PFSWE    PFS register write enable bit
    b7 BOWI     PFSWE bit write disable bit
	 */
	MPC.PWPR.BIT.PFSWE  = 0u;
	MPC.PWPR.BIT.B0WI   = 1u;
}
/******************************************************************************
End of function usb_cpu_FunctionUSB1IP
 ******************************************************************************/

/******************************************************************************
Interrupt function
 ******************************************************************************/
/******************************************************************************
Function Name   : usb_cpu_UsbintInit
Description     : USB interrupt Initialize
Arguments       : void
Return value    : void
 ******************************************************************************/
void usb_cpu_UsbintInit(void)
{
#if USB_FUNCSEL_USBIP0_PP != USB_NOUSE_PP
	/* Deep standby USB monitor register
    b0      SRPC0    USB0 single end control
    b3-b1   Reserved 0
    b4      FIXPHY0  USB0 transceiver output fix
    b7-b5   Reserved 0
    b8      SRPC1    USB1 single end control
    b11-b9  Reserved 0
    b12     FIXPHY1  USB1 transceiver output fix
    b15-b13 Reserved 0
    b16     DP0      USB0 DP input
    b17     DM0      USB0 DM input
    b19-b18 Reserved 0
    b20     DOVCA0   USB0 OVRCURA input
    b21     DOVCB0   USB0 OVRCURB input
    b22     Reserved 0
    b23     DVBSTS0  USB1 VBUS input
    b24     DP1      USB1 DP input
    b25     DM1      USB1 DM input
    b27-b26 Reserved 0
    b28     DOVCA1   USB1 OVRCURA input
    b29     DOVCB1   USB1 OVRCURB input
    b30     Reserved 0
    b31     DVBSTS1  USB1 VBUS input
	 */
	USB.DPUSR0R.BIT.FIXPHY0 = 0u;   /* USB0 Transceiver Output fixed */

	/* Interrupt enable register
    b0 IEN0 Interrupt enable bit
    b1 IEN1 Interrupt enable bit
    b2 IEN2 Interrupt enable bit
    b3 IEN3 Interrupt enable bit
    b4 IEN4 Interrupt enable bit
    b5 IEN5 Interrupt enable bit
    b6 IEN6 Interrupt enable bit
    b7 IEN7 Interrupt enable bit
	 */
	ICU.IER[0x04].BIT.IEN1  = 0u;   /* D0FIFO0 disable */
	ICU.IER[0x04].BIT.IEN2  = 0u;   /* D1FIFO0 disable */
	ICU.IER[0x04].BIT.IEN3  = 1u;   /* USBI0 enable */
	ICU.IER[0x0B].BIT.IEN2  = 0u;   /* USBR0 disable */

	/* Interrupt priority register
    b3-b0 IPR      Interrupt priority
    b7-b4 Reserved 0
	 */
	ICU.IPR[33].BYTE    = 0x00; /* D0FIFO0 */
	ICU.IPR[34].BYTE    = 0x00; /* D1FIFO0 */
	ICU.IPR[35].BYTE    = 0x03; /* USBI0 */
	ICU.IPR[90].BYTE    = 0x00; /* USBR0 */

	usb_cpu_FunctionUSB0IP();   /* USB0 pin function and port mode setting. */
#endif  /* USB_FUNCSEL_USBIP0_PP != USB_NOUSE_PP */

#if USB_FUNCSEL_USBIP1_PP != USB_NOUSE_PP
	/* Deep standby USB monitor register
    b0      SRPC0    USB0 single end control
    b3-b1   Reserved 0
    b4      FIXPHY0  USB0 transceiver output fix
    b7-b5   Reserved 0
    b8      SRPC1    USB1 single end control
    b11-b9  Reserved 0
    b12     FIXPHY1  USB1 transceiver output fix
    b15-b13 Reserved 0
    b16     DP0      USB0 DP input
    b17     DM0      USB0 DM input
    b19-b18 Reserved 0
    b20     DOVCA0   USB0 OVRCURA input
    b21     DOVCB0   USB0 OVRCURB input
    b22     Reserved 0
    b23     DVBSTS0  USB1 VBUS input
    b24     DP1      USB1 DP input
    b25     DM1      USB1 DM input
    b27-b26 Reserved 0
    b28     DOVCA1   USB1 OVRCURA input
    b29     DOVCB1   USB1 OVRCURB input
    b30     Reserved 0
    b31     DVBSTS1  USB1 VBUS input
	 */
	USB.DPUSR0R.BIT.FIXPHY1 = 0u;   /* USB1 Transceiver Output fixed */

	/* Interrupt enable register
    b0 IEN0 Interrupt enable bit
    b1 IEN1 Interrupt enable bit
    b2 IEN2 Interrupt enable bit
    b3 IEN3 Interrupt enable bit
    b4 IEN4 Interrupt enable bit
    b5 IEN5 Interrupt enable bit
    b6 IEN6 Interrupt enable bit
    b7 IEN7 Interrupt enable bit
	 */
	ICU.IER[0x04].BIT.IEN4  = 0u;   /* Disable D0FIF1 interrupt */
	ICU.IER[0x04].BIT.IEN5  = 0u;   /* Disable D1FIF1 interrupt */
	ICU.IER[0x04].BIT.IEN6  = 1u;   /* Enable  USBI1  interrupt */
	ICU.IER[0x0B].BIT.IEN3  = 0u;   /* Disable Resume interrupt */

	/* Priority D0FIFO0=0(Disable)
    b3-b0 IPR      Interrupt priority
    b7-b4 Reserved 0
	 */
	ICU.IPR[36].BYTE    = 0x00; /* D0FIFO1 */
	ICU.IPR[37].BYTE    = 0x00; /* D0FIFO1 */
	ICU.IPR[38].BYTE    = 0x03; /* USBI1 */
	ICU.IPR[91].BYTE    = 0x00; /* USBR1 */

	//usb_cpu_FunctionUSB1IP();   /* USB1 port mode and Switch mode Initialize */
#endif  /* USB_FUNCSEL_USBIP1_PP != USB_NOUSE_PP */
}
/******************************************************************************
End of function usb_cpu_UsbintInit
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_DmaintInit
Description     : DMA interrupt Initialize
Arguments       : void
Return value    : void
 ******************************************************************************/
void usb_cpu_DmaintInit(void)
{
	/* Condition compilation by the difference of the devices */
#if USB_TRANS_MODE_PP == USB_TRANS_DMA_PP
#endif  /* USB_TRANS_MODE_PP == USB_TRANS_DMA_PP */
	/* Condition compilation by the difference of the devices */
#if USB_TRANS_MODE_PP == USB_TRANS_DTC_PP
	/* Protect register
    b0    PRC0     Protect bit0
    b1    PRC1     Protect bit1
    b2    Reserved 0
    b3    PRC3     Protect bit3
    b7-b4 Reserved 0
    b15-b8 PRKEY   PRC Key code bit
	 */
	SYSTEM.PRCR.WORD    = 0xA503;   /* Protect off */

	/* Module stop control register (Enable DTC module)
    b3-b0   Reserved 0
    b4      MSTPA4   8bit timer3,2 stop bit
    b5      MSTPA5   8bit timer1,0 stop bit
    b8-b6   Reserved 0
    b9      MSTPA9   Multifunction timer unit0 stop bit
    b10     MSTPA10  Programmable pulse unit1 stop bit
    b11     MSTPA11  Programmable pulse unit0 stop bit
    b12     MSTPA12  16Bit timer pulse unit1 stop bit
    b13     MSTPA13  16Bit timer pulse unit0 stop bit
    b14     MSTPA14  Compare timer unit1 stop bit
    b15     MSTPA15  Compare timer unit0 stop bit
    b16     Reserved 0
    b17     MSTPA17  12bit AD stop bit
    b18     Reserved 0
    b19     MSTPA19  DA stop bit
    b22-b20 Reserved 0
    b23     MSTPA23  10bit AD unit0 stop bit
    b24     MSTPA24  Module stop A24 set bit
    b26-b25 Reserved 0
    b27     MSTPA27  Module stop A27 set bit
    b28     MSTPA28  DMA/DTC stop bit
    b29     MSTPA29  Module stop A29 set bit
    b30     Reserved 0
    b31     ACSE     All clock stop bit
	 */
	SYSTEM.MSTPCRA.BIT.MSTPA28  = 0;

	/* Protect register
    b0    PRC0     Protect bit0
    b1    PRC1     Protect bit1
    b2    Reserved 0
    b3    PRC3     Protect bit3
    b7-b4 Reserved 0
    b15-b8 PRKEY   PRC Key code bit
	 */
	SYSTEM.PRCR.WORD    = 0xA500;   /* Protect on */

	/* DTC vector register
    b21-b0 DTCVBR Vector table address
	 */
	DTC.DTCVBR                  = (unsigned long)&usb_dtcctable;
#endif  /* USB_TRANS_MODE_PP == USB_TRANS_DMA_PP */
}
/******************************************************************************
End of function usb_cpu_DmaintInit
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_usb_int_hand
Description     : USB interrupt Handler
Arguments       : void
Return value    : void
 ******************************************************************************/
void usb_cpu_usb_int_hand(void)
{
	/* Condition compilation by the difference of USB function */
#if USB_FUNCSEL_USBIP0_PP != USB_NOUSE_PP
	usb_cstd_UsbHandler();      /* Call interrupt routine */
#endif /* USB_FUNCSEL_USBIP0_PP */
}
/******************************************************************************
End of function usb_cpu_usb_int_hand
 ******************************************************************************/

/******************************************************************************
Function Name   : usb2_cpu_usb_int_hand
Description     : USB interrupt Handler
Arguments       : void
Return value    : void
 ******************************************************************************/
void usb2_cpu_usb_int_hand(void)
{
	/* Condition compilation by the difference of USB function */
#if USB_FUNCSEL_USBIP1_PP != USB_NOUSE_PP
	usb2_cstd_UsbHandler();     /* Call interrupt routine */
#endif /* USB_FUNCSEL_USBIP1_PP */
}
/******************************************************************************
End of function usb2_cpu_usb_int_hand
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_d0fifo_int_hand
Description     : D0FIFO interrupt Handler
Arguments       : none
Return value    : none
 ******************************************************************************/
void usb_cpu_d0fifo_int_hand(void)
{
	/* Condition compilation by the difference of USB function */
#if USB_FUNCSEL_USBIP0_PP != USB_NOUSE_PP
	usb_cstd_DmaHandler();      /* Call interrupt routine */
#endif /* USB_FUNCSEL_USBIP0_PP */
}
/******************************************************************************
End of function usb_cpu_d0fifo_int_hand
 ******************************************************************************/

/******************************************************************************
Function Name   : usb2_cpu_d0fifo_int_hand
Description     : D0FIFO interrupt Handler
Arguments       : none
Return value    : none
 ******************************************************************************/
void usb2_cpu_d0fifo_int_hand(void)
{
	/* Condition compilation by the difference of USB function */
#if USB_FUNCSEL_USBIP1_PP != USB_NOUSE_PP
	usb2_cstd_DmaHandler();     /* Call interrupt routine */
#endif /* USB_FUNCSEL_USBIP1_PP */
}
/******************************************************************************
End of function usb2_cpu_d0fifo_int_hand
 ******************************************************************************/

/******************************************************************************
Private global variables and functions
 ******************************************************************************/
uint16_t usb_gcstd_D0fifo[2u] = {0,0};      /* D0fifo0 Interrupt Request enable */
uint16_t usb_gcstd_D1fifo[2u] = {0,0};      /* D1fifo0 Interrupt Request enable */

/******************************************************************************
Renesas Abstracted RSK functions
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_int_enable
Description     : USB Interrupt Enable
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : void
 ******************************************************************************/
void usb_cpu_int_enable(USB_UTR_t *ptr)
{
	if( ptr->ip == USB_USBIP_0 )
	{
		/* Interrupt enable register (USB0 USBIO enable)
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
		 */
		ICU.IER[4].BIT.IEN3     = 1;
		ICU.IER[4].BIT.IEN1     = usb_gcstd_D0fifo[ptr->ip];
		ICU.IER[4].BIT.IEN2     = usb_gcstd_D1fifo[ptr->ip];
	}

	if (ptr->ip == USB_USBIP_1)
	{
		/* Interrupt enable register (USB1 USBIO enable)
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
		 */
		ICU.IER[4].BIT.IEN6     = 1;
		ICU.IER[4].BIT.IEN4     = usb_gcstd_D0fifo[ptr->ip];
		ICU.IER[4].BIT.IEN5     = usb_gcstd_D1fifo[ptr->ip];
	}
}
/******************************************************************************
End of function usb_cpu_int_enable
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_int_disable
Description     : USB Interrupt disable
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : void
 ******************************************************************************/
void usb_cpu_int_disable(USB_UTR_t *ptr)
{
	if( ptr->ip == USB_USBIP_0 )
	{
		/* Interrupt enable register (USB0 USBIO disable)
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
		 */
		ICU.IER[4].BIT.IEN3         = 0;
		usb_gcstd_D0fifo[ptr->ip]   = ICU.IER[4].BIT.IEN1;
		ICU.IER[4].BIT.IEN1         = 0;
		usb_gcstd_D1fifo[ptr->ip]   = ICU.IER[4].BIT.IEN2;
		ICU.IER[4].BIT.IEN2         = 0;
	}

	if (ptr->ip == USB_USBIP_1)
	{
		/* Interrupt enable register (USB1 USBIO disable)
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
		 */
		ICU.IER[4].BIT.IEN6         = 0;
		usb_gcstd_D0fifo[ptr->ip]   = ICU.IER[4].BIT.IEN4;
		ICU.IER[4].BIT.IEN4         = 0;
		usb_gcstd_D1fifo[ptr->ip]   = ICU.IER[4].BIT.IEN5;
		ICU.IER[4].BIT.IEN5         = 0;
	}
}
/******************************************************************************
End of function usb_cpu_int_disable
 ******************************************************************************/

/******************************************************************************
DMA & DTC function
 ******************************************************************************/
/******************************************************************************
Function Name   : usb_cpu_d0fifo2buf_start_dma
Description     : FIFO to Buffer data read DMA start
Arguments       : USB_UTR_t *ptr        : USB internal structure. Selects USB channel. 
                : uint32_t SourceAddr   : Source address
Return value    : void
 ******************************************************************************/
void usb_cpu_d0fifo2buf_start_dma(USB_UTR_t *ptr, uint32_t SourceAddr)
{
	/* Condition compilation by the difference of the devices */
#if USB_TRANS_MODE_PP == USB_TRANS_DMA_PP
#endif /* USB_TRANS_MODE_PP == USB_TRANS_DMA_PP */

	/* Condition compilation by the difference of the devices */
#if USB_TRANS_MODE_PP == USB_TRANS_DTC_PP
	uint16_t    size;
	uint32_t    tmp;

	/* DTC Transfer enable
    b0    DTCST    DTC module start bit
    b7-b1 Reserved 0
	 */
	DTC.DTCST.BIT.DTCST         = 1;

	/* DTC control register (Transfer Information Read No Skip)
    b2-b0 Reserved 0
    b3    Reserved 0
    b4    RRS      DTC Transfer Information Read Skip enable bit
    b7-b5 Reserved 0
	 */
	DTC.DTCCR.BIT.RRS           = 0;

	/* DTC mode register A (Block Transfer Set)
    b1-b0 Reserved 0
    b3-b2 SM       source address mode bit
    b5-b4 SZ       DTC data transfer size bit
    b7-b6 MD       DTC mode bit
	 */
	usb_dtcreg[ptr->ip].MRA.BIT.MD      = 2;

	tmp                     = ((usb_gcstd_Dma0Fifo[ptr->ip] -1)/ 2) +1;

	/* DTC mode register A (Word Size)
    b1-b0 Reserved 0
    b3-b2 SM       source address mode bit
    b5-b4 SZ       DTC data transfer size bit
    b7-b6 MD       DTC mode bit
	 */
	usb_dtcreg[ptr->ip].MRA.BIT.SZ  = 1;

	/* DTC mode register A (Source Address fixed)
    b1-b0 Reserved 0
    b3-b2 SM       source address mode bit
    b5-b4 SZ       DTC data transfer size bit
    b7-b6 MD       DTC mode bit
	 */
	usb_dtcreg[ptr->ip].MRA.BIT.SM      = 0;

	/* DTC mode register B (Chain Transfer disable)
    b1-b0 Reserved 0
    b3-b2 DM       Destination address mode bit
    b4    DTS      DTC transfer mode select bit
    b5    DISEL    DTC interrupt select bit
    b6    CHNS     DTC chain transfer select bit
    b7    CHNE     DTC chain transfer enable bit
	 */
	usb_dtcreg[ptr->ip].MRB.BIT.CHNE        = 0;

	/* DTC mode register B (Select Data Transfer End Interrupt)
    b1-b0 Reserved 0
    b3-b2 DM       Destination address mode bit
    b4    DTS      DTC transfer mode select bit
    b5    DISEL    DTC interrupt select bit
    b6    CHNS     DTC chain transfer select bit
    b7    CHNE     DTC chain transfer enable bit
	 */
	usb_dtcreg[ptr->ip].MRB.BIT.DISEL   = 0;

	/* DTC mode register B (Source Side Block Area)
    b1-b0 Reserved 0
    b3-b2 DM       Destination address mode bit
    b4    DTS      DTC transfer mode select bit
    b5    DISEL    DTC interrupt select bit
    b6    CHNS     DTC chain transfer select bit
    b7    CHNE     DTC chain transfer enable bit
	 */
	usb_dtcreg[ptr->ip].MRB.BIT.DTS     = 1;

	/* DTC mode register B (Destination Address Increment)
    b1-b0 Reserved 0
    b3-b2 DM       Destination address mode bit
    b4    DTS      DTC transfer mode select bit
    b5    DISEL    DTC interrupt select bit
    b6    CHNS     DTC chain transfer select bit
    b7    CHNE     DTC chain transfer enable bit
	 */
	usb_dtcreg[ptr->ip].MRB.BIT.DM      = 2;

	/* DTC source address register (FIFO port address)
    b31-b0 SAR Destination address
	 */
	usb_dtcreg[ptr->ip].SAR             = SourceAddr;

	/* DTC source address register (Table address)
    b31-b0 SAR Source address
	 */
	usb_dtcreg[ptr->ip].DAR = (uint32_t)(usb_gcstd_DataPtr[ptr->ip][usb_gcstd_Dma0Pipe[ptr->ip]]);

	size            = (uint8_t )(tmp);
	size            += (uint16_t)(tmp << 8);

	/* DTC transfer count registerA
    b15-b0 CRA Transfer count
	 */
	usb_dtcreg[ptr->ip].CRA             = (uint16_t)(size);

	/* DTC transfer count registerB (Block count)
    b15-b0 CRB Transfer count
	 */
	usb_dtcreg[ptr->ip].CRB = (uint16_t)((usb_gcstd_DataCnt[ptr->ip][usb_gcstd_Dma0Pipe[ptr->ip]] -1)
			/ usb_gcstd_Dma0Fifo[ptr->ip]) +1;

	/* DTC address mode register (Full Address Mode)
    b0    SHORT    Short address mode bit
    b7-b1 Reserved 0
	 */
	DTC.DTCADMOD.BIT.SHORT      = 0;

	/* DTC control register (Transfer Information Read No Skip)
    b2-b0 Reserved 0
    b3    Reserved 0
    b4    RRS      DTC Transfer Information Read Skip enable bit
    b7-b5 Reserved 0
	 */
	DTC.DTCCR.BIT.RRS           = 1;

	if( ptr->ip == USB_USBIP_0 )
	{
		/* Priority D0FIFO0=0
        b3-b0 IPR      Interrupt priority
        b7-b4 Reserved 0
		 */
		ICU.IPR[33].BYTE            = 0x00;

		/* Interrupt enable register (USB0 D0FIFO enable(IEN4))
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
		 */
		ICU.IER[0x04].BIT.IEN1      = 1;

		/* DTC start enable register (USB0 D0FIFO transfer)
        b0    DTCE     DTC start enable bit
        b7-b1 Reserved 0
		 */
		ICU.DTCER[33].BIT.DTCE      = 1;
	}
	else
	{
		/* Priority D1FIFO0=0
        b3-b0 IPR      Interrupt priority
        b7-b4 Reserved 0
		 */
		ICU.IPR[36].BYTE            = 0x00;

		/* Interrupt enable register (USB1 D0FIFO enable(IEN4))
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
		 */
		ICU.IER[0x04].BIT.IEN4      = 1;

		/* DTC start enable register (USB1 D0FIFO transfer)
        b0    DTCE     DTC start enable bit
        b7-b1 Reserved 0
		 */
		ICU.DTCER[36].BIT.DTCE      = 1;
	}
#endif /* USB_TRANS_MODE_PP == USB_TRANS_DTC_PP */
}
/******************************************************************************
End of function usb_cpu_d0fifo2buf_start_dma
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_buf2d0fifo_start_dma
Description     : Buffer to FIFO data write DMA start
Arguments       : USB_UTR_t *ptr        : USB internal structure. Selects USB channel. 
                : uint32_t DistAddr     : Destination address
Return value    : void
 ******************************************************************************/
void usb_cpu_buf2d0fifo_start_dma(USB_UTR_t *ptr, uint32_t DistAdr)
{
	/* Condition compilation by the difference of the devices */
#if USB_TRANS_MODE_PP == USB_TRANS_DMA_PP
#endif /* USB_TRANS_MODE_PP == USB_TRANS_DMA_PP */

	/* Condition compilation by the difference of the devices */
#if USB_TRANS_MODE_PP == USB_TRANS_DTC_PP
	uint16_t    size;
	uint32_t    tmp;

	/* DTC Transfer enable
    b0    DTCST    DTC module start bit
    b7-b1 Reserved 0
	 */
	DTC.DTCST.BIT.DTCST = 1;

	/* DTC control register (Transfer Information Read No Skip)
    b2-b0 Reserved 0
    b3    Reserved 0
    b4    RRS      DTC Transfer Information Read Skip enable bit
    b7-b5 Reserved 0
	 */
	DTC.DTCCR.BIT.RRS = 0;

	/* DTC mode register A (Block Transfer Set)
    b1-b0 Reserved 0
    b3-b2 SM       source address mode bit
    b5-b4 SZ       DTC data transfer size bit
    b7-b6 MD       DTC mode bit
	 */
	usb_dtcreg[ptr->ip].MRA.BIT.MD = 2;
	if( (usb_gcstd_Dma0Size[ptr->ip] & 0x0001) != 0 )
	{
		/* if count == odd */
		tmp = usb_gcstd_Dma0Size[ptr->ip];

		/* DTC mode register A (Byte Size)
        b1-b0 Reserved 0
        b3-b2 SM       source address mode bit
        b5-b4 SZ       DTC data transfer size bit
        b7-b6 MD       DTC mode bit
		 */
		usb_dtcreg[ptr->ip].MRA.BIT.SZ  = 0;
	}
	else
	{
		tmp                     = usb_gcstd_Dma0Size[ptr->ip] / 2;

		/* DTC mode register A (Word Size)
        b1-b0 Reserved 0
        b3-b2 SM       source address mode bit
        b5-b4 SZ       DTC data transfer size bit
        b7-b6 MD       DTC mode bit
		 */
		usb_dtcreg[ptr->ip].MRA.BIT.SZ  = 1;
	}

	/* DTC mode register A (Source Address Increment)
    b1-b0 Reserved 0
    b3-b2 SM       source address mode bit
    b5-b4 SZ       DTC data transfer size bit
    b7-b6 MD       DTC mode bit
	 */
	usb_dtcreg[ptr->ip].MRA.BIT.SM      = 2;

	/* DTC mode register B (Chain Transfer disable)
    b1-b0 Reserved 0
    b3-b2 DM       Destination address mode bit
    b4    DTS      DTC transfer mode select bit
    b5    DISEL    DTC interrupt select bit
    b6    CHNS     DTC chain transfer select bit
    b7    CHNE     DTC chain transfer enable bit
	 */
	usb_dtcreg[ptr->ip].MRB.BIT.CHNE        = 0;

	/* DTC mode register B (Select Data Transfer End Interrupt)
    b1-b0 Reserved 0
    b3-b2 DM       Destination address mode bit
    b4    DTS      DTC transfer mode select bit
    b5    DISEL    DTC interrupt select bit
    b6    CHNS     DTC chain transfer select bit
    b7    CHNE     DTC chain transfer enable bit
	 */
	usb_dtcreg[ptr->ip].MRB.BIT.DISEL   = 0;

	/* DTC mode register B (Destination Side Block Area)
    b1-b0 Reserved 0
    b3-b2 DM       Destination address mode bit
    b4    DTS      DTC transfer mode select bit
    b5    DISEL    DTC interrupt select bit
    b6    CHNS     DTC chain transfer select bit
    b7    CHNE     DTC chain transfer enable bit
	 */
	usb_dtcreg[ptr->ip].MRB.BIT.DTS     = 0;

	/* DTC mode register B (Destination Address fixed)
    b1-b0 Reserved 0
    b3-b2 DM       Destination address mode bit
    b4    DTS      DTC transfer mode select bit
    b5    DISEL    DTC interrupt select bit
    b6    CHNS     DTC chain transfer select bit
    b7    CHNE     DTC chain transfer enable bit
	 */
	usb_dtcreg[ptr->ip].MRB.BIT.DM      = 0;

	/* DTC source address register (Table address)
    b31-b0 SAR Destination address
	 */
	usb_dtcreg[ptr->ip].SAR = (uint32_t)(usb_gcstd_DataPtr[ptr->ip][usb_gcstd_Dma0Pipe[ptr->ip]]);

	/* DTC source address register (FIFO port address)
    b31-b0 SAR Source address
	 */
	usb_dtcreg[ptr->ip].DAR             = (uint32_t)(DistAdr);
	size                        = (uint8_t )(tmp);
	size                       += (uint16_t)(tmp << 8);

	/* DTC transfer count registerA
    b15-b0 CRA Transfer count
	 */
	usb_dtcreg[ptr->ip].CRA             = (uint16_t)(size);

	/* DTC transfer count registerB (Block count)
    b15-b0 CRB Transfer count
	 */
	usb_dtcreg[ptr->ip].CRB = (uint16_t)(usb_gcstd_DataCnt[ptr->ip][usb_gcstd_Dma0Pipe[ptr->ip]]
	                                                                / usb_gcstd_Dma0Size[ptr->ip]);

	/* DTC address mode register (Full Address Mode)
    b0    SHORT    Short address mode bit
    b7-b1 Reserved 0
	 */
	DTC.DTCADMOD.BIT.SHORT      = 0;

	/* DTC control register (Transfer Information Read No Skip)
    b2-b0 Reserved 0
    b3    Reserved 0
    b4    RRS      DTC Transfer Information Read Skip enable bit
    b7-b5 Reserved 0
	 */
	DTC.DTCCR.BIT.RRS           = 1;

	if( ptr->ip == USB_USBIP_0 )
	{
		/* Priority D0FIFO0=0
        b3-b0 IPR      Interrupt priority
        b7-b4 Reserved 0
		 */
		ICU.IPR[33].BYTE            = 0x00;

		/* Interrupt enable register (USB0 D0FIFO enable(IEN4))
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
		 */
		ICU.IER[0x04].BIT.IEN1      = 1;

		/* DTC start enable register (USB0 D0FIFO transfer)
        b0    DTCE     DTC start enable bit
        b7-b1 Reserved 0
		 */
		ICU.DTCER[33].BIT.DTCE      = 1;
	}
	else
	{
		/* Priority D1FIFO0=0
        b3-b0 IPR      Interrupt priority
        b7-b4 Reserved 0
		 */
		ICU.IPR[36].BYTE            = 0x00;

		/* Interrupt enable register (USB1 D0FIFO enable(IEN4))
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
		 */
		ICU.IER[0x04].BIT.IEN4      = 1;

		/* DTC start enable register (USB1 D0FIFO transfer)
        b0    DTCE     DTC start enable bit
        b7-b1 Reserved 0
		 */
		ICU.DTCER[36].BIT.DTCE      = 1;
	}
#endif /* USB_TRANS_MODE_PP == USB_TRANS_DTC_PP */
}
/******************************************************************************
End of function usb_cpu_buf2d0fifo_start_dma
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_d0fifo_stop_dma
Description     : DMA stop
Arguments       : USB_UTR_t *ptr        : USB internal structure. Selects USB channel.
Return value    : void
 ******************************************************************************/
void usb_cpu_d0fifo_stop_dma(USB_UTR_t *ptr)
{
	if( ptr->ip == USB_USBIP_0 )
	{
		/* Interrupt request register
        b0    IR       Interrupt status flag
        b7-b1 Reserved 0
		 */
		ICU.IR[33].BIT.IR           = 0;

		/* Priority D0FIFO0=0
        b3-b0 IPR      Interrupt priority
        b7-b4 Reserved 0
		 */
		ICU.IPR[33].BYTE            = 0x00;

		/* Interrupt enable register (USB0 D0FIFO disable(IEN4))
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
		 */
		ICU.IER[0x04].BIT.IEN1      = 0;

		/* DTC start enable register (USB0 D0FIFO transfer disable)
        b0    DTCE     DTC start enable bit
        b7-b1 Reserved 0
		 */
		ICU.DTCER[33].BIT.DTCE      = 0;
	}
	else
	{
		/* Interrupt request register
        b0    IR       Interrupt status flag
        b7-b1 Reserved 0
		 */
		ICU.IR[36].BIT.IR           = 0;

		/* Priority D1FIFO0=0
        b3-b0 IPR      Interrupt priority
        b7-b4 Reserved 0
		 */
		ICU.IPR[36].BYTE            = 0x00;

		/* Interrupt enable register (USB1 D0FIFO disable(IEN4))
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
		 */
		ICU.IER[0x04].BIT.IEN4      = 0;

		/* DTC start enable register (USB1 D0FIFO transfer disable)
        b0    DTCE     DTC start enable bit
        b7-b1 Reserved 0
		 */
		ICU.DTCER[36].BIT.DTCE      = 0;
	}
}
/******************************************************************************
End of function usb_cpu_d0fifo_stop_dma
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_d0fifo_restart_dma
Description     : DMA Restart
Arguments       : USB_UTR_t *ptr        : USB internal structure. Selects USB channel.
Return value    : none
 ******************************************************************************/
void usb_cpu_d0fifo_restart_dma( USB_UTR_t *ptr )
{
	/* Condition compilation by the difference of the devices */
#if USB_TRANS_MODE_PP == USB_TRANS_DMA_PP
#endif /* USB_TRANS_MODE_PP == USB_TRANS_DMA_PP */

	/* Condition compilation by the difference of the devices */
#if USB_TRANS_MODE_PP == USB_TRANS_DTC_PP
	uint16_t    size;
	uint32_t    tmp;

	if( (usb_gcstd_Dma0Size[ptr->ip] & 0x0001u) != 0u )
	{
		/* if count == odd */
		tmp = usb_gcstd_Dma0Size[ptr->ip];

		/* DTC mode register A (Byte Size)
        b1-b0 Reserved 0
        b3-b2 SM       source address mode bit
        b5-b4 SZ       DTC data transfer size bit
        b7-b6 MD       DTC mode bit
		 */
		usb_dtcreg[ptr->ip].MRA.BIT.SZ  = 0;
	}
	else
	{
		tmp = usb_gcstd_Dma0Size[ptr->ip] / 2;

		/* DTC mode register A (Word Size)
        b1-b0 Reserved 0
        b3-b2 SM       source address mode bit
        b5-b4 SZ       DTC data transfer size bit
        b7-b6 MD       DTC mode bit
		 */
		usb_dtcreg[ptr->ip].MRA.BIT.SZ  = 1;
	}
	size                        = (uint8_t )(tmp);
	size                       += (uint16_t)(tmp << 8);

	/* DTC transfer count registerA
    b15-b0 CRA Transfer count
	 */
	usb_dtcreg[ptr->ip].CRA             = (uint16_t)(size);

	/* DTC transfer count registerB (Block count)
    b15-b0 CRB Transfer count
	 */
	usb_dtcreg[ptr->ip].CRB             = (uint16_t)(1);

	/* DTC Transfer enable
    b0    DTCST    DTC module start bit
    b7-b1 Reserved 0
	 */
	DTC.DTCST.BIT.DTCST         = 1;

	if( ptr->ip == USB_USBIP_0 )
	{
		/* DTC start enable register (USB0 D0FIFO transfer)
        b0    DTCE     DTC start enable bit
        b7-b1 Reserved 0
		 */
		ICU.DTCER[33].BIT.DTCE      = 1;
	}
	else
	{
		/* DTC start enable register (USB1 D0FIFO transfer)
        b0    DTCE     DTC start enable bit
        b7-b1 Reserved 0
		 */
		ICU.DTCER[36].BIT.DTCE      = 1;
	}

#endif /* USB_TRANS_MODE_PP == USB_TRANS_DTC_PP */
}
/******************************************************************************
End of function usb_cpu_d0fifo_restart_dma
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_d0fifo_enable_dma
Description     : DTC(D0FIFO) interrupt enable (Interrupt priority 5 set)
Arguments       : USB_UTR_t *ptr        : USB internal structure. Selects USB channel.
Return value    : none
 ******************************************************************************/
void usb_cpu_d0fifo_enable_dma(USB_UTR_t *ptr )
{
	if( ptr->ip == USB_USBIP_0 )
	{
		/* Priority D0FIFO0=0(Disable)
        b3-b0 IPR      Interrupt priority
        b7-b4 Reserved 0
		 */
		ICU.IPR[33].BYTE    = 0x05; /* USB0 */
	}
	else
	{
		/* Priority D0FIFO0=0(Disable)
        b3-b0 IPR      Interrupt priority
        b7-b4 Reserved 0
		 */
		ICU.IPR[36].BYTE    = 0x05; /* USB1 */
	}
}
/******************************************************************************
End of function usb_cpu_d0fifo_enable_dma
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_d0fifo_disable_dma
Description     : D0FIFO interrupt disable (Interrupt priority 0 set)
Arguments       : USB_UTR_t *ptr        : USB internal structure. Selects USB channel.
Return value    : none
 ******************************************************************************/
void usb_cpu_d0fifo_disable_dma(USB_UTR_t *ptr )
{
	if( ptr->ip == USB_USBIP_0 )
	{
		/* Priority D0FIFO0=0(Disable)
        b3-b0 IPR      Interrupt priority
        b7-b4 Reserved 0
		 */
		ICU.IPR[33].BYTE    = 0x00; /* USB0 */
	}
	else
	{
		/* Priority D0FIFO0=0(Disable)
        b3-b0 IPR      Interrupt priority
        b7-b4 Reserved 0
		 */
		ICU.IPR[36].BYTE    = 0x00; /* USB1 */
	}
}
/******************************************************************************
End of function usb_cpu_d0fifo_disable_dma
 ******************************************************************************/

/******************************************************************************
SLEEP & STANDBY control function
 ******************************************************************************/
/******************************************************************************
Function Name   : usb_cpu_GoLpwrSleep
Description     : Go low power sleep mode
Arguments       : USB_UTR_t *ptr        : USB internal structure. Selects USB channel.
Return value    : none
 ******************************************************************************/
void usb_cpu_GoLpwrSleep(USB_UTR_t *ptr)
{
	if( usb_cstd_is_host_mode(ptr) == USB_YES )
	{
		/* Condition compilation by the difference of the devices */
#if USB_CPU_LPW_PP == USB_LPWR_USE_PP
		if( usb_gcpu_RemoteProcess == USB_OFF )
		{
			/* Standby control register (Software Standby disable)
            b7-b0  Reserved 0
            b12-b8 STS      Standby timer select bit
            b13    Reserved 0
            b14    OPE      Output port enable bit
            b15    SSBY     Software standby bit
			 */
			SYSTEM.SBYCR.BIT.SSBY       = 0;

			/* Goto WAIT */
			usb_cpu_GoWait();
		}
#endif /* (USB_CPU_LPW_PP == USB_LPWR_USE_PP) */
	}
	else
	{
		/* Condition compilation by the difference of the operating system */
#if USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP
		/* Condition compilation by the difference of the devices */
#if USB_CPU_LPW_PP == USB_LPWR_USE_PP
		if( usb_gcpu_RemoteProcess == USB_OFF )
		{
			if( usb_gpstd_RemoteWakeup == USB_YES )
			{
				usb_cpu_IRQ2_Enable(ptr);   /* IRQ2(RX63N-RSK_SW1) for Remote wakeup */
			}

			/* Standby control register (Software Standby disable)
            b7-b0  Reserved 0
            b12-b8 STS      Standby timer select bit
            b13    Reserved 0
            b14    OPE      Output port enable bit
            b15    SSBY     Software standby bit
			 */
			SYSTEM.SBYCR.BIT.SSBY       = 0;

			/* Goto WAIT */
			usb_cpu_GoWait();

			if( usb_gpstd_RemoteWakeup == USB_YES )
			{
				usb_gcpu_RemoteProcess = USB_ON;
			}
		}
#endif /* (USB_CPU_LPW_PP == USB_LPWR_USE_PP) */
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP */
	}
}
/******************************************************************************
End of function usb_cpu_GoLpwrSleep
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_GoLpwrDeepStby
Description     : Go Low Power Deep Standby
Arguments       : USB_UTR_t *ptr        : USB internal structure. Selects USB channel.
Return value    : none
 ******************************************************************************/
void usb_cpu_GoLpwrDeepStby(USB_UTR_t *ptr)
{
	/* DTC Transfer disable
    b0    DTCST    DTC module start bit
    b7-b1 Reserved 0
	 */
	DTC.DTCST.BIT.DTCST     = 0;

	/* Sleep mode return main clock select register
    b2-b0   RSTCKSEL Sleep mode return main clock source select bit
    b6-b3   Reserved 0
    b7      RSTCKEN  Sleep mode return main clock source change enable bit
	 */
	SYSTEM.RSTCKCR.BIT.RSTCKSEL     = 2;    /* Main clock OSC select */
	SYSTEM.RSTCKCR.BIT.RSTCKEN      = 1;    /* Source change enable */

	/* RTC control register3 (Sub clock OSC input disable)
    b0      RTCEN    Sub clock OSC input enable/disable
    b7-b1   Reserved 0
	 */
	RTC.RCR3.BIT.RTCEN              = 0;

	/* Sub clock Oscillator control register (SOSTP=Sub clock enable)
    b0      SOSTP    Sub clock Oscillator stop bit
    b7-b1   Reserved 0
	 */
	SYSTEM.SOSCCR.BIT.SOSTP         = 0;

	/* System clock control register3(CKSEL(10-8)=Sub clock)
    b7-b0   Reserved 0
    b10-b8  CKSEL    Clock source select bit
    b15-b11 Reserved 0
	 */
	SYSTEM.SCKCR3.BIT.CKSEL         = 3;

	/* Go Deep Standby Mode */
	usb_cpu_GoDeepStbyMode(ptr);

	/* Protect register (protect off)
    b0    PRC0     Protect bit0
    b1    PRC1     Protect bit1
    b2    Reserved 0
    b3    PRC3     Protect bit3
    b7-b4 Reserved 0
    b15-b8 PRKEY   PRC Key code bit
	 */
	SYSTEM.PRCR.WORD        = 0xA503;   /* protect off */
	(*(volatile unsigned char  *)0x0008C28E)    = 0;;

	/* Goto WAIT */
	usb_cpu_GoWait();
}
/******************************************************************************
End of function usb_cpu_GoLpwrDeepStby
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_GoDeepStbyMode
Description     : Go Deep Standby Mode
Arguments       : USB_UTR_t *ptr        : USB internal structure. Selects USB channel.
Return value    : none
 ******************************************************************************/
void usb_cpu_GoDeepStbyMode(USB_UTR_t *ptr)
{
	/***************************************/
	/*  Can not use DTC in Standby Mode !! */
	/***************************************/
	/* Standby control register (Software Standby Shift)
    b7-b0  Reserved 0
    b12-b8 STS      Standby timer select bit
    b13    Reserved 0
    b14    OPE      Output port enable bit
    b15    SSBY     Software standby bit
	 */
	SYSTEM.SBYCR.BIT.SSBY       = 1;

	/* Deep Standby control register (Deep Standby Shift)
    b0-b1 DEEPCUT  RAM off bit
    b5-b2 Reserved 0
    b6    IOKEEP   IO port keep bit
    b7    DPSBY    Deep standby bit
	 */
	SYSTEM.DPSBYCR.BIT.DPSBY    = 1;

	/* Deep Standby control register (Internal RAM0, USB Power supply on)
    b0-b1 DEEPCUT  RAM off bit
    b5-b2 Reserved 0
    b6    IOKEEP   IO port keep bit
    b7    DPSBY    Deep standby bit
	 */
	SYSTEM.DPSBYCR.BIT.DEEPCUT  = 0;

	/* Standby control register (Bus Signal Output status keep)
    b7-b0  Reserved 0
    b12-b8 STS      Standby timer select bit
    b13    Reserved 0
    b14    OPE      Output port enable bit
    b15    SSBY     Software standby bit
	 */
	SYSTEM.SBYCR.BIT.OPE        = 1;

	/* Deep Standby control register (IO status keep by the return)
    b0-b1 DEEPCUT  RAM off bit
    b5-b2 Reserved 0
    b6    IOKEEP   IO port keep bit
    b7    DPSBY    Deep standby bit
	 */
	SYSTEM.DPSBYCR.BIT.IOKEEP   = 1;

	/* Deep standby interrupt edge register (Down Edge Clear)
    b0    DLVD1EG  LVD1 edge select bit
    b1    DLVD2EG  LVD2 edge select bit
    b3-2  Reserved 0
    b4    DNMIEG   NMI edge select bit
    b5    DRIICDEG SDA2-DS edge select bit
    b6    DRIICCEG SCA2-DS edge select bit
    b7    Reserved 0
	 */
	SYSTEM.DPSIEGR2.BYTE            = 0;

	/* LDV,DRT,NMI,RIIC disable & USB enable */
	/* Deep standby interrupt enable register (Cancel by USB enable)
    b0 DLVD1IE  LVD1 deep standby cancel enable bit
    b1 DLVD2IE  LVD2 deep standby cancel enable bit
    b2 DRTCIIE  RTC deep standby cancel enable bit
    b3 DRTCAIE  RTC periodic interrupt deep standby cancel enable bit
    b4 DNMIE    NMI deep standby cancel enable bit
    b5 DRIICDIE SDA2-DS deep standby cancel enable bit
    b6 DRIICCIE SCL2-DS deep standby cancel enable bit
    b7 DUSBIE   USB suspend/resume deep standby cancel enable bit
	 */
	SYSTEM.DPSIER2.BYTE         = 0x80;

	/* IRQ0-7 interrupt clear */
	/* Deep standby interrupt flag register2 (IRQ0-7 Request clear)
    b0 DIRQ0F  IRQ0 deep standby cancel flag
    b1 DIRQ1F  IRQ1 deep standby cancel flag
    b2 DIRQ2F  IRQ2 deep standby cancel flag
    b3 DIRQ3F  IRQ3 deep standby cancel flag
    b4 DIRQ4F  IRQ4 deep standby cancel flag
    b5 DIRQ5F  IRQ5 deep standby cancel flag
    b6 DIRQ6F  IRQ6 deep standby cancel flag
    b7 DIRQ7F  IRQ7 deep standby cancel flag
	 */
	SYSTEM.DPSIFR0.BYTE         = 0;

	/* IRQ8-15 deep standby cancel request clear */
	/* Deep standby interrupt flag register2 (IRQ8-15 Request clear)
    b0 DIRQ8F   IRQ8 deep standby cancel flag
    b1 DIRQ9F   IRQ9 deep standby cancel flag
    b2 DIRQ10F  IRQ10 deep standby cancel flag
    b3 DIRQ11F  IRQ11 deep standby cancel flag
    b4 DIRQ12F  IRQ12 deep standby cancel flag
    b5 DIRQ13F  IRQ13 deep standby cancel flag
    b6 DIRQ14F  IRQ14 deep standby cancel flag
    b7 DIRQ15F  IRQ15 deep standby cancel flag
	 */
	SYSTEM.DPSIFR1.BYTE         = 0;

	/* LDV,DRT,NMI,RIIC & USB interrupt clear */
	/* Deep standby interrupt flag register2 (Cancel by USB Request clear)
    b0 DLVD1IF LVD1 deep standby cancel flag
    b1 DLVD2IF LVD2 deep standby cancel flag
    b2 DRTCIIF RTC Cycle interrupt deep standby cancel flag
    b3 DRTCAIF RTC Alarm interrupt deep standby cancel flag
    b4 DNMIF   NMI deep standby cancel flag
    b5 DRIICDIF  SDA2-DS deep standby cancel flag
    b6 DRIICCIF  SCL2-DS deep standby cancel flag
    b7 DUSBIF  USB suspend/resume deep standby cancel flag
	 */
	SYSTEM.DPSIFR2.BYTE         = 0;

	/* CAN interrupt clear */
	/* Deep standby interrupt flag register3 (CRX1-DS Request clear)
    b0 DCANIF CRX1-DS deep standby cancel flag
    b7-b1     Reserved 0
	 */
	SYSTEM.DPSIFR3.BYTE         = 0;

	/* Oscillator stop detect control register (disable)
    b0     OSTDIE   Oscillator stop detect interrupt enable
    b1-b6  Reserved 0
    b7     OSTDE    Oscillator stop detect enable
	 */
	SYSTEM.OSTDCR.BYTE          = 0x00;

	/* Go Deep Standby Mode 0 */
	usb_cpu_GoDeepStbyMode0(ptr);
}
/******************************************************************************
End of function usb_cpu_GoDeepStbyMode
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_GoDeepStbyMode0
Description     : Go Deep Standby Mode 0
Arguments       : USB_UTR_t *ptr        : USB internal structure. Selects USB channel.
Return value    : none
 ******************************************************************************/
void usb_cpu_GoDeepStbyMode0(USB_UTR_t *ptr)
{
	/* Deep standby USB monitor register (Single end Receiver Control)
    b0      SRPC0    USB0 single end control
    b3-b1   Reserved 0
    b4      FIXPHY0  USB0 transceiver output fix
    b7-b5   Reserved 0
    b8      SRPC1    USB1 single end control
    b11-b9  Reserved 0
    b12     FIXPHY1  USB1 transceiver output fix
    b15-b13 Reserved 0
    b16     DP0      USB0 DP input
    b17     DM0      USB0 DM input
    b19-b18 Reserved 0
    b20     DOVCA0   USB0 OVRCURA input
    b21     DOVCB0   USB0 OVRCURB input
    b22     Reserved 0
    b23     DVBSTS0  USB1 VBUS input
    b24     DP1      USB1 DP input
    b25     DM1      USB1 DM input
    b27-b26 Reserved 0
    b28     DOVCA1   USB1 OVRCURA input
    b29     DOVCB1   USB1 OVRCURB input
    b30     Reserved 0
    b31     DVBSTS1  USB1 VBUS input
	 */
	USB.DPUSR0R.BIT.SRPC0       = 1;    /* Single end Receiver Control */
	USB.DPUSR0R.BIT.FIXPHY0     = 1;    /* Transceiver Output fixed */

	if ( usb_cstd_is_host_mode(ptr) == USB_NO )
	{
		/* Deep standby USB interrupt register(VBUS clear)
        b0      DPINTE0   USB0 DP interrupt enable clear bit
        b1      DMINTE0   USB0 DM interrupt enable clear bit
        b3-b2   Reserved  0
        b4      DOVRCRAE0 USB0 OVRCURA interrupt enable clear bit
        b5      DOVRCRBE0 USB0 OVRCURB interrupt enable clear bit
        b6      Reserved  0
        b7      DVBSE0    USB0 VBUS interrupt enable clear bit
        b8      DPINTE1   USB1 DP interrupt enable clear bit
        b9      DMINTE1   USB1 DM interrupt enable clear bit
        b11-b10 Reserved  0
        b12     DOVRCRAE1 USB1 OVRCURA interrupt enable clear bit
        b13     OVRCRBE1  USB1 OVRCURB interrupt enable clear bit
        b14     Reserved  0
        b15     DVBSE1    USB1 VBUS interrupt enable clear bit
		 */
		USB.DPUSR1R.BIT.DVBSE0      = 0;    /* VBUS clear */
		USB.DPUSR1R.BIT.DVBSE0      = 1;    /* VBUS set */
	}

	if ( usb_cstd_is_host_mode(ptr) == USB_YES )
	{
		/* Deep standby USB interrupt register(OVCR clear)
        b0      DPINTE0   USB0 DP interrupt enable clear bit
        b1      DMINTE0   USB0 DM interrupt enable clear bit
        b3-b2   Reserved  0
        b4      DOVRCRAE0 USB0 OVRCURA interrupt enable clear bit
        b5      DOVRCRBE0 USB0 OVRCURB interrupt enable clear bit
        b6      Reserved  0
        b7      DVBSE0    USB0 VBUS interrupt enable clear bit
        b8      DPINTE1   USB1 DP interrupt enable clear bit
        b9      DMINTE1   USB1 DM interrupt enable clear bit
        b11-b10 Reserved  0
        b12     DOVRCRAE1 USB1 OVRCURA interrupt enable clear bit
        b13     OVRCRBE1  USB1 OVRCURB interrupt enable clear bit
        b14     Reserved  0
        b15     DVBSE1    USB1 VBUS interrupt enable clear bit
		 */
		USB.DPUSR1R.BIT.DOVRCRAE0   = 0;    /* OVCR clear */
		USB.DPUSR1R.BIT.DOVRCRBE0   = 0;    /* OVCR clear */
		USB.DPUSR1R.BIT.DOVRCRAE0   = 1;    /* OVCR set */
		USB.DPUSR1R.BIT.DOVRCRBE0   = 1;    /* OVCR set */
	}

	/* Deep standby USB interrupt register(DP clear)
    b0      DPINTE0   USB0 DP interrupt enable clear bit
    b1      DMINTE0   USB0 DM interrupt enable clear bit
    b3-b2   Reserved  0
    b4      DOVRCRAE0 USB0 OVRCURA interrupt enable clear bit
    b5      DOVRCRBE0 USB0 OVRCURB interrupt enable clear bit
    b6      Reserved  0
    b7      DVBSE0    USB0 VBUS interrupt enable clear bit
    b8      DPINTE1   USB1 DP interrupt enable clear bit
    b9      DMINTE1   USB1 DM interrupt enable clear bit
    b11-b10 Reserved  0
    b12     DOVRCRAE1 USB1 OVRCURA interrupt enable clear bit
    b13     OVRCRBE1  USB1 OVRCURB interrupt enable clear bit
    b14     Reserved  0
    b15     DVBSE1    USB1 VBUS interrupt enable clear bit
	 */
	USB.DPUSR1R.BIT.DPINTE0     = 0;    /* DP clear */
	USB.DPUSR1R.BIT.DPINTE0     = 1;    /* DP set */
}
/******************************************************************************
End of function usb_cpu_GoDeepStbyMode0
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_RegRecovEnable
Description     : Register Recover Enable
Arguments       : USB_UTR_t *ptr        : USB internal structure. Selects USB channel.
Return value    : none
 ******************************************************************************/
void usb_cpu_RegRecovEnable(USB_UTR_t *ptr)
{
	if( ptr->ip == USB_USBIP_0 )
	{
		/* Device state change register
        b14-b0 Reserved 0
        b15    DVCHG    Device state change bit
		 */
		USB0.DVCHGR.BIT.DVCHG = 1;
	}
	else
	{
		/* Device state change register
        b14-b0 Reserved 0
        b15    DVCHG    Device state change bit
		 */
		USB1.DVCHGR.BIT.DVCHG = 1;
	}
}
/******************************************************************************
End of function usb_cpu_RegRecovEnable
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_RegRecovDisable
Description     : Register Recover Disable
Arguments       : USB_UTR_t *ptr        : USB internal structure. Selects USB channel.
Return value    : none
 ******************************************************************************/
void usb_cpu_RegRecovDisable(USB_UTR_t *ptr)
{
	if( ptr->ip == USB_USBIP_0 )
	{
		/* Device state change register
        b14-b0 Reserved 0
        b15    DVCHG    Device state change bit
		 */
		USB0.DVCHGR.BIT.DVCHG = 0;
	}
	else
	{
		/* Device state change register
        b14-b0 Reserved 0
        b15    DVCHG    Device state change bit
		 */
		USB1.DVCHGR.BIT.DVCHG = 0;
	}
}
/******************************************************************************
End of function usb_cpu_RegRecovDisable
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_GoWait
Description     : Go Wait
Arguments       : none
Return value    : none
 ******************************************************************************/
void usb_cpu_GoWait(void)
{
	int_exception(11);              /* Goto usb_cpu_GoWait_Int() */
}
/******************************************************************************
End of function usb_cpu_GoWait
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_GoWait_Int
Description     : Go Wait Interrupt
Arguments       : none
Return value    : none
 ******************************************************************************/
void usb_cpu_GoWait_Int(void)
{
	wait();                         /* Wait */
}
/******************************************************************************
End of function usb_cpu_GoWait_Int
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_GetDPSRSTF
Description     : Get Deep Software Standby Reset Flag
Arguments       : none
Return value    : Deep Software Standby Reset Flag
 ******************************************************************************/
uint8_t usb_cpu_GetDPSRSTF(void)
{
	/* Reset status register (Deep software standby reset check)
    b0    POFR     Power on reset Detection flag
    b1    LVD0F    Voltage Watch 0 reset detection flag
    b2    LVD1F    Voltage Watch 1 reset detection flag
    b3    LVD2F    Voltage Watch 2 reset detection flag
    b6-b4 Reserved 0
    b7    DPSRSTF  Deep software standby reset flag
	 */
	return (uint8_t)SYSTEM.RSTSR0.BIT.DPSRSTF;
}
/******************************************************************************
End of function usb_cpu_GetDPSRSTF
 ******************************************************************************/

/******************************************************************************
IRQ function
 ******************************************************************************/
/******************************************************************************
Function Name   : usb_cpu_IRQ2_Enable
Description     : IRQ8 Interrupt Enable
Arguments       : USB_UTR_t *ptr        : USB internal structure. Selects USB channel.
Return value    : none
 ******************************************************************************/
void usb_cpu_IRQ2_Enable(USB_UTR_t *ptr)
{
	/* Condition compilation by the difference of USB function */
#if USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP
	if( usb_cstd_is_host_mode(ptr) == USB_NO )
	{
		if( usb_gpstd_RemoteWakeup == USB_YES )
		{
			/* Interrupt enable register (IEN0 disable))
            b0 IEN0 Interrupt enable bit
            b1 IEN1 Interrupt enable bit
            b2 IEN2 Interrupt enable bit
            b3 IEN3 Interrupt enable bit
            b4 IEN4 Interrupt enable bit
            b5 IEN5 Interrupt enable bit
            b6 IEN6 Interrupt enable bit
            b7 IEN7 Interrupt enable bit
			 */
			ICU.IER[8].BIT.IEN2     = 0u;

			/* IRQ control register
            b1-b0 Reserved 0
            b3-b2 IRQMD    IRQ detect bit
            b7-b4 Reserved 0
			 */
			ICU.IRQCR[2].BIT.IRQMD  = 1u;   /* Down Edge Set */

			/* Priority Resume0=B
            b3-b0 IPR      Interrupt priority
            b7-b4 Reserved 0
			 */
			ICU.IPR[66].BYTE        = 0x0B;

			/* Interrupt request register
            b0    IR       Interrupt status flag
            b7-b1 Reserved 0
			 */
			ICU.IR[66].BIT.IR       = 0u;

			/* Interrupt enable register (IEN0 enable)
            b0 IEN0 Interrupt enable bit
            b1 IEN1 Interrupt enable bit
            b2 IEN2 Interrupt enable bit
            b3 IEN3 Interrupt enable bit
            b4 IEN4 Interrupt enable bit
            b5 IEN5 Interrupt enable bit
            b6 IEN6 Interrupt enable bit
            b7 IEN7 Interrupt enable bit
			 */
			ICU.IER[8].BIT.IEN2     = 1u;
		}
	}
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP */

	if( usb_cstd_is_host_mode(ptr) == USB_YES )
	{
		/* Interrupt enable register (IEN0 disable))
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
		 */
		ICU.IER[8].BIT.IEN2     = 0u;

		/* IRQ control register
        b1-b0 Reserved 0
        b3-b2 IRQMD    IRQ detect bit
        b7-b4 Reserved 0
		 */
		ICU.IRQCR[2].BIT.IRQMD  = 1u;   /* Down Edge Set */

		/* Priority Resume0=B
        b3-b0 IPR      Interrupt priority
        b7-b4 Reserved 0
		 */
		ICU.IPR[66].BYTE        = 0x0B;

		/* Interrupt request register
        b0    IR       Interrupt status flag
        b7-b1 Reserved 0
		 */
		ICU.IR[66].BIT.IR       = 0u;

		/* Interrupt enable register (IEN0 enable)
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
		 */
		ICU.IER[8].BIT.IEN2     = 1u;
	}
}
/******************************************************************************
End of function usb_cpu_IRQ2_Enable
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_IRQ2_DisEnable
Description     : IRQ8 Interrupt Disable
Arguments       : none
Return value    : none
 ******************************************************************************/
void usb_cpu_IRQ2_DisEnable(void)
{
	/* Interrupt request register
    b0    IR       Interrupt status flag
    b7-b1 Reserved 0
	 */
	ICU.IR[66].BIT.IR           = 0u;

	/* Interrupt enable register (IEN0 disable)
    b0 IEN0 Interrupt enable bit
    b1 IEN1 Interrupt enable bit
    b2 IEN2 Interrupt enable bit
    b3 IEN3 Interrupt enable bit
    b4 IEN4 Interrupt enable bit
    b5 IEN5 Interrupt enable bit
    b6 IEN6 Interrupt enable bit
    b7 IEN7 Interrupt enable bit
	 */
	ICU.IER[8].BIT.IEN2         = 0u;

	/* Priority Resume0=0(Disable)
    b3-b0 IPR      Interrupt priority
    b7-b4 Reserved 0
	 */
	ICU.IPR[66].BYTE            = 0x00;
}
/******************************************************************************
End of function usb_cpu_IRQ2_DisEnable
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_IRQ8_Enable
Description     : IRQ9 Interrupt Enable
Arguments       : none
Return value    : none
 ******************************************************************************/
void usb_cpu_IRQ8_Enable(void)
{
	/* Interrupt enable register (IEN4 disable)
    b0 IEN0 Interrupt enable bit
    b1 IEN1 Interrupt enable bit
    b2 IEN2 Interrupt enable bit
    b3 IEN3 Interrupt enable bit
    b4 IEN4 Interrupt enable bit
    b5 IEN5 Interrupt enable bit
    b6 IEN6 Interrupt enable bit
    b7 IEN7 Interrupt enable bit
	 */
	ICU.IER[9].BIT.IEN0     = 0;        /* Interrupt disable */

	/* IRQ control register
    b1-b0 Reserved 0
    b3-b2 IRQMD    IRQ detect bit
    b7-b4 Reserved 0
	 */
	ICU.IRQCR[9].BIT.IRQMD  = 1;        /* Down Edge Set */

	/* Priority Resume0=B
    b3-b0 IPR      Interrupt priority
    b7-b4 Reserved 0
	 */
	ICU.IPR[72].BYTE        = 0x0B;     /* Priority Resume0=B */

	/* Interrupt request register
    b0    IR       Interrupt status flag
    b7-b1 Reserved 0
	 */
	ICU.IR[72].BIT.IR       = 0;

	/* Interrupt enable register (IEN4 enable)
    b0 IEN0 Interrupt enable bit
    b1 IEN1 Interrupt enable bit
    b2 IEN2 Interrupt enable bit
    b3 IEN3 Interrupt enable bit
    b4 IEN4 Interrupt enable bit
    b5 IEN5 Interrupt enable bit
    b6 IEN6 Interrupt enable bit
    b7 IEN7 Interrupt enable bit
	 */
	ICU.IER[9].BIT.IEN0     = 1;
}
/******************************************************************************
End of function usb_cpu_IRQ8_Enable
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_IRQ12_Disable
Description     : IRQ9 Interrupt Disable
Arguments       : none
Return value    : none
 ******************************************************************************/
void usb_cpu_IRQ12_Disable(void)
{
	/* Interrupt request register
    b0    IR       Interrupt status flag
    b7-b1 Reserved 0
	 */
	ICU.IR[72].BIT.IR       = 0;

	/* Interrupt enable register
    b0 IEN0 Interrupt enable bit
    b1 IEN1 Interrupt enable bit
    b2 IEN2 Interrupt enable bit
    b3 IEN3 Interrupt enable bit
    b4 IEN4 Interrupt enable bit
    b5 IEN5 Interrupt enable bit
    b6 IEN6 Interrupt enable bit
    b7 IEN7 Interrupt enable bit
	 */
	ICU.IER[9].BIT.IEN0     = 0;

	/* Priority Resume0=0
    b3-b0 IPR      Interrupt priority
    b7-b4 Reserved 0
	 */
	ICU.IPR[72].BYTE        = 0x00;
}
/******************************************************************************
End of function usb_cpu_IRQ12_Disable
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_IRQ2Int
Description     : IRQ2 Interrupt process
Arguments       : none
Return value    : none
 ******************************************************************************/
void usb_cpu_IRQ2Int(void)
{
	/* Condition compilation by the difference of USB function */
#if USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP

	/* Condition compilation by the difference of the devices */
#if (USB_CPU_LPW_PP == USB_LPWR_USE_PP)
	USB_UTR_t utr;
	USB_UTR_t *ptr;

	ptr = (USB_UTR_t *)&utr;
	ptr->ip = USB_PERI_USBIP_NUM;
	ptr->ipp = usb_cstd_GetUsbIpAdr( ptr->ip );

	R_usb_pstd_PcdChangeDeviceState(ptr, USB_DO_REMOTEWAKEUP, (uint16_t)0, (USB_CB_INFO_t)usb_cstd_DummyFunction);
	usb_cpu_IRQ2_DisEnable();
#endif /* (USB_CPU_LPW_PP == USB_LPWR_USE_PP) */

#endif  /* USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP */
}
/******************************************************************************
End of function usb_cpu_IRQ2Int
 ******************************************************************************/

/******************************************************************************
End of function usb_cpu_IRQ8Int
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_D1fifoIntHand
Description     : D1FIFO interrupt Handler
Arguments       : none
Return value    : none
 ******************************************************************************/
void usb_cstd_D1fifoIntHand(void)
{
	/* Please add the processing for the system. */
}
/******************************************************************************
End of function usb_cstd_D1fifoIntHand
 ******************************************************************************/

/******************************************************************************
Function Name   : usb2_cstd_D1fifoIntHand
Description     : D1FIFO interrupt Handler
Arguments       : none
Return value    : none
 ******************************************************************************/
void usb2_cstd_D1fifoIntHand(void)
{
	/* Please add the processing for the system. */
}
/******************************************************************************
End of function usb2_cstd_D1fifoIntHand
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_UsbIntHandR
Description     : RESUME interrupt Handler
Arguments       : none
Return value    : none
 ******************************************************************************/
void usb_cstd_UsbIntHandR(void)
{
	/* Please add the processing for the system. */
}
/******************************************************************************
End of function usb_cstd_UsbIntHandR
 ******************************************************************************/

/******************************************************************************
Function Name   : usb2_cstd_UsbIntHandR
Description     : RESUME interrupt Handler
Arguments       : none
Return value    : none
 ******************************************************************************/
void usb2_cstd_UsbIntHandR(void)
{
	/* Please add the processing for the system. */
}
/******************************************************************************
End of function usb2_cstd_UsbIntHandR
 ******************************************************************************/




/******************************************************************************
TIMER function
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_Delay1us
Description     : 1us Delay timer
Arguments       : uint16_t  time        ; Delay time(*1us)
Return value    : none
Note            : Please change for your MCU
 ******************************************************************************/
void usb_cpu_Delay1us(uint16_t time)
{
	volatile register uint16_t  i;

	/* Wait 1us (Please change for your MCU) */
	for( i = 0; i < (5 * time); ++i )
	{
	};
}
/******************************************************************************
End of function usb_cpu_Delay1us
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_DelayXms
Description     : xms Delay timer
Arguments       : uint16_t  time        ; Delay time(*1ms)
Return value    : void
Note            : Please change for your MCU
 ******************************************************************************/
void usb_cpu_DelayXms(uint16_t time)
{
	/* Wait xms (Please change for your MCU) */
	volatile register uint32_t  i;

	/* Wait 1ms */
	for( i = 0; i < (6500 * time); ++i )
	{
	};
	/* When "ICLK=48MHz" is set, this code is waiting for 1ms.
          Please change this code with CPU Clock mode. */
}
/******************************************************************************
End of function usb_cpu_DelayXms
 ******************************************************************************/

/******************************************************************************
KEY function
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_KeyInit
Description     : Input key port initialize
Arguments       : none
Return value    : none
 ******************************************************************************/
void usb_cpu_KeyInit(void)
{
	PORT0.PDR.BIT.B2 = 0;
	PORT0.PDR.BIT.B3 = 0;
	PORT0.PDR.BIT.B5 = 0;
	PORTF.PDR.BIT.B5 = 0;
	PORTJ.PDR.BIT.B5 = 0;
}
/******************************************************************************
End of function usb_cpu_KeyInit
 ******************************************************************************/

void usb_cpu_IOInit (void)
{
	PORT1.PDR.BIT.B2 = 0;   // IN  : PRES-BAT2
	PORT1.PCR.BIT.B2 = 1;   //       Pull-up

	PORTG.PDR.BIT.B4 = 1;   // OUT : CMD_POMPE

	PORT5.PDR.BIT.B7 = 1;   // OUT : IDLE

	PORT8.PDR.BIT.B5 = 0;   // IN  : PclkSelect
	PORT8.PCR.BIT.B5 = 1;   //       Pull-up

	PORT9.PDR.BIT.B6 = 0;   // IN  : DEF_POMPE
	PORT9.PCR.BIT.B6 = 1;   //       Pull-up

	PORTG.PDR.BIT.B2 = 1;   // OUT : DEF
	PORT6.PDR.BIT.B5 = 1;   // OUT : DISPO
	PORT6.PDR.BIT.B6 = 1;   // OUT : CHARGE
	PORTG.PDR.BIT.B3 = 1;   // OUT : CMD_EV
	PORT5.PDR.BIT.B6 = 0;   // IN  : PRES-BAT1
	PORT5.PCR.BIT.B6 = 1;   //       Pull-up

	PORTG.PDR.BIT.B0 = 0;   // IN  : EQUAL
	PORTG.PCR.BIT.B0 = 1;   //       Pull-up

	PORTG.PDR.BIT.B1 = 1;   // OUT  : DE

	PORT9.PDR.BIT.B7 = 0;   // IN  : M/A
	PORT9.PCR.BIT.B7 = 1;   //       Pull-up

	PORT1.PDR.BIT.B0 = 0;   // IN : PM1 with pull-up
	PORT1.PCR.BIT.B0 = 1;
	PORT9.PDR.BIT.B5 = 0;   // IN : PM2 with pull-up
	PORT9.PCR.BIT.B5 = 1;
	PORT9.PDR.BIT.B3 = 0;   // IN : PM3 with pull-up
	PORT9.PCR.BIT.B3 = 1;
	PORT9.PDR.BIT.B1 = 0;   // IN : PM4 with pull-up
	PORT9.PCR.BIT.B1 = 1;
	PORT1.PDR.BIT.B1 = 0;   // IN : PM5 with pull-up
	PORT1.PCR.BIT.B1 = 1;
	PORT9.PDR.BIT.B4 = 0;   // IN : PM6 with pull-up
	PORT9.PCR.BIT.B4 = 1;
	PORT9.PDR.BIT.B2 = 0;   // IN : PM7 with pull-up
	PORT9.PCR.BIT.B2 = 1;
	PORT9.PDR.BIT.B0 = 0;   // IN : PM8 with pull-up
	PORT9.PCR.BIT.B0 = 1;

	ElectroVOff;
	PumpOff;
	IdleOn;
}



/******************************************************************************
LED function
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_LedInit
Description     : LED Initialize
Arguments       : none
Return value    : none
 ******************************************************************************/
void usb_cpu_LedInit(void)
{
	PORT0.PDR.BIT.B7    = 1;            /* LED CHARGE		*/
	PORT2.PDR.BIT.B2    = 1;            /* LED AVAIL 		*/
	PORT4.PDR.BIT.B6    = 1;            /* LED FAULT		*/

	//PORT4.PDR.BIT.B0    = 1;            /* BUZZER			  */
	//PORT4.PODR.BIT.B0   = 0;            /* BUZZER	 OFF  */

	// LED OFF
	LED_CHARGE = LED_OFF;
	LED_AVAIL = LED_OFF;
	LED_FAULT = LED_OFF;
}
/******************************************************************************
End of function usb_cpu_LedInit
 ******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_McuInitialize
Description     : MCU Initialize
Arguments       : none
Return value    : none
 ******************************************************************************/
void usb_cpu_McuInitialize(void)
{
	uint32_t    i;

	/* Protect register (protect off)
    b0    PRC0     Protect bit0
    b1    PRC1     Protect bit1
    b2    Reserved 0
    b3    PRC3     Protect bit3
    b7-b4 Reserved 0
    b15-b8 PRKEY   PRC Key code bit
	 */
	SYSTEM.PRCR.WORD                = 0xA503;

	/* Reset status register (Deep software standby reset check)
    b0    POFR     Power on reset Detection flag
    b1    LVD0F    Voltage Watch 0 reset detection flag
    b2    LVD1F    Voltage Watch 1 reset detection flag
    b3    LVD2F    Voltage Watch 2 reset detection flag
    b6-b4 Reserved 0
    b7    DPSRSTF  Deep software standby reset flag
	 */
	if( SYSTEM.RSTSR0.BIT.DPSRSTF == 1 )
	{
		/* Deep standby interrupt flag register2
        b0 DLVD1IF LVD1 deep standby cancel flag
        b1 DLVD2IF LVD2 deep standby cancel flag
        b2 DRTCIIF RTC Cycle interrupt deep standby cancel flag
        b3 DRTCAIF RTC Alarm interrupt deep standby cancel flag
        b4 DNMIF   NMI deep standby cancel flag
        b5 DRIICDIF  SDA2-DS deep standby cancel flag
        b6 DRIICCIF  SCL2-DS deep standby cancel flag
        b7 DUSBIF  USB suspend/resume deep standby cancel flag
		 */
		if( SYSTEM.DPSIFR2.BIT.DUSBIF == 1 )
		{
			SYSTEM.DPSIFR2.BIT.DUSBIF   = 0;    /*  Clear USB Request */
		}

		/* Port mode register (Xin-Xout) and start main clock */
		/* Port mode register
        b0 B0 Pn0 pin mode control bit
        b1 B1 Pn1 pin mode control bit
        b2 B2 Pn2 pin mode control bit
        b3 B3 Pn3 pin mode control bit
        b4 B4 Pn4 pin mode control bit
        b5 B5 Pn5 pin mode control bit
        b6 B6 Pn6 pin mode control bit
        b7 B7 Pn7 pin mode control bit
		 */
		PORT3.PMR.BIT.B6        = 1;
		/* Port mode register
        b0 B0 Pn0 pin mode control bit
        b1 B1 Pn1 pin mode control bit
        b2 B2 Pn2 pin mode control bit
        b3 B3 Pn3 pin mode control bit
        b4 B4 Pn4 pin mode control bit
        b5 B5 Pn5 pin mode control bit
        b6 B6 Pn6 pin mode control bit
        b7 B7 Pn7 pin mode control bit
		 */
		PORT3.PMR.BIT.B7        = 1;

		/* Main clock Oscillator control register
        b4-b0   MOSTP     Main clock oscillator stop bit
        b7-b5   Reserved 0
		 */
		SYSTEM.MOSCCR.BYTE      = 0x00; /* Main clock oscillator is operated */

		/* Main clock Oscillator wait control register
        b4-b0   MSTS     Main clock Oscillator wait time bit
        b7-b5   Reserved 0
		 */
		SYSTEM.MOSCWTCR.BYTE    = 0x0D; /* 131072 state */

		/* Sub clock Oscillator control register
        b0      SOSTP    Sub clock Oscillator stop bit
        b7-b1   Reserved 0
		 */
		SYSTEM.SOSCCR.BYTE      = 0x01; /* Sub clock Oscillator is stopped */

		/* Start PLL Controler */
		/* PLL control register
        b1-b0   PLIDIV   PLL input division ratio select bit
        b7-b2   Reserved 0
        b13-b8  STC      frequency synthesizer set bit
        b15-b14 Reserved 0
		 */
		SYSTEM.PLLCR.WORD       = 0x0F00;   /* PLIDIV = 12MHz(/1), STC = 192MHz(*16) */

		/* PLL control register2
        b0      PLLEN    PLL stop control bit
        b7-b1   Reserved 0
		 */
		SYSTEM.PLLCR2.BYTE      = 0x00; /* PLL enable */

		/* PLL wait control register
        b4-b0   PSTS     PLL wait time set bit
        b7-b5   Reserved 0
		 */
		SYSTEM.PLLWTCR.BYTE     = 0x0F; /* 4194304cycle(Default) */

		for(i = 0;i< 200;i++)   /* wait over 4ms ? */
		{
		}

		/* System clock control register
        b7-b0   Reserved 0
        b11-b8  PCK      PCLK
        b15-b12 Reserved 0
        b19-b16 BCK      BCLK
        b22-b20 Reserved 0
        b23     PSTOP1   BCLK control bit
        b27-b24 ICK      System clock (ICLK) select bit
        b31-b28 FCK      Flash IC Clock(FCLK) select bit
		 */
		SYSTEM.SCKCR.LONG       = 0x21042222;   /* ICK(96MHz)=PLL/2,BCK(24MHz)=PLL/8,FCK,PCK(48MHz)=PLL/4 */

		/* System clock control register3
        b7-b0   Reserved 0
        b10-b8  CKSEL    Clock source select bit
        b15-b11 Reserved 0
		 */
		SYSTEM.SCKCR3.WORD      = 0x0400;       /* PLL */

		/* System clock control register2
        b3-b0   IEBCK    IEBUS clock(IECLK) select bit
        b7-b4   UCK      USB clock(UCLK) select bit
        b15-b8  Reserved 0
		 */
		SYSTEM.SCKCR2.BIT.UCK   = 3;    /* USB clock : 48MHz */

		/* Module stop control register (Disable ACSE)
        b3-b0   Reserved 0
        b4      MSTPA4   8bit timer3,2 stop bit
        b5      MSTPA5   8bit timer1,0 stop bit
        b8-b6   Reserved 0
        b9      MSTPA9   Multifunction timer unit0 stop bit
        b10     MSTPA10  Programmable pulse unit1 stop bit
        b11     MSTPA11  Programmable pulse unit0 stop bit
        b12     MSTPA12  16Bit timer pulse unit1 stop bit
        b13     MSTPA13  16Bit timer pulse unit0 stop bit
        b14     MSTPA14  Compare timer unit1 stop bit
        b15     MSTPA15  Compare timer unit0 stop bit
        b16     Reserved 0
        b17     MSTPA17  12bit AD stop bit
        b18     Reserved 0
        b19     MSTPA19  DA stop bit
        b22-b20 Reserved 0
        b23     MSTPA23  10bit AD unit0 stop bit
        b24     MSTPA24  Module stop A24 set bit
        b26-b25 Reserved 0
        b27     MSTPA27  Module stop A27 set bit
        b28     MSTPA28  DMA/DTC stop bit
        b29     MSTPA29  Module stop A29 set bit
        b30     Reserved 0
        b31     ACSE     All clock stop bit
		 */
		SYSTEM.MSTPCRA.LONG     = 0x7FFFFFFF;

#if USB_FUNCSEL_USBIP0_PP != USB_NOUSE_PP
		/* Module stop control register
        b0     MSTPB0   CAN stop bit
        b14-b1 Reserved 0
        b15    MSTPB15  Ether DMA stop bit
        b16    MSTPB16  Serial peri1 stop bit
        b17    MSTPB17  Serial peri0 stop bit
        b18    MSTPB18  USB port1 stop bit
        b19    MSTPB19  USB port0 stop bit
        b20    MSTPB20  I2C interface1 stop bit
        b21    MSTPB21  I2C interface0 stop bit
        b22    Reserved 0
        b23    MSTPB23  CRC stop bit
        b24    Reserved 0
        b25    MSTPB25  Serial interface6 stop bit
        b26    MSTPB26  Serial interface5 stop bit
        b27    Reserved 0
        b28    MSTPB28  Serial interface3 stop bit
        b29    MSTPB29  Serial interface2 stop bit
        b30    MSTPB30  Serial interface1 stop bit
        b31    MSTPB31  Serial interface0 stop bit
		 */
		SYSTEM.MSTPCRB.BIT.MSTPB19  = 0u;   /*  Enable USB0 module */
#endif  /* USB_FUNCSEL_USBIP0_PP != USB_NOUSE_PP */

#if USB_FUNCSEL_USBIP1_PP != USB_NOUSE_PP
		/* Module stop control register
        b0     MSTPB0   CAN stop bit
        b14-b1 Reserved 0
        b15    MSTPB15  Ether DMA stop bit
        b16    MSTPB16  Serial peri1 stop bit
        b17    MSTPB17  Serial peri0 stop bit
        b18    MSTPB18  USB port1 stop bit
        b19    MSTPB19  USB port0 stop bit
        b20    MSTPB20  I2C interface1 stop bit
        b21    MSTPB21  I2C interface0 stop bit
        b22    Reserved 0
        b23    MSTPB23  CRC stop bit
        b24    Reserved 0
        b25    MSTPB25  Serial interface6 stop bit
        b26    MSTPB26  Serial interface5 stop bit
        b27    Reserved 0
        b28    MSTPB28  Serial interface3 stop bit
        b29    MSTPB29  Serial interface2 stop bit
        b30    MSTPB30  Serial interface1 stop bit
        b31    MSTPB31  Serial interface0 stop bit
		 */
		SYSTEM.MSTPCRB.BIT.MSTPB18 = 0u;    /* Enable USB1 module */
#endif /* USB_FUNCSEL_USBIP1_PP != USB_NOUSE_PP */

		/* Port mode register (BCLK) */
		/* System control register
        b0     ROME     Internal ROM Enable bit
        b1     EXBE     External Bus Enable bit
        b7-b2  Reserved 0
        b15-b8 KEY[7:0] SYSCR0 Key code
		 */
		SYSTEM.SYSCR0.WORD      = 0x5A03;   /* External Bus Enable */

		/* Port mode register
        b0 B0 Pn0 pin mode control bit
        b1 B1 Pn1 pin mode control bit
        b2 B2 Pn2 pin mode control bit
        b3 B3 Pn3 pin mode control bit
        b4 B4 Pn4 pin mode control bit
        b5 B5 Pn5 pin mode control bit
        b6 B6 Pn6 pin mode control bit
        b7 B7 Pn7 pin mode control bit
		 */
		PORT5.PMR.BIT.B3            = 1;

		/* External Bus clock control register
        b0     BCLKDIV  BCLK Pin output select bit
        b7-b1  Reserved 0
		 */
		SYSTEM.BCKCR.BIT.BCLKDIV    = 1;    /* BCLK * 1/2 */

		/* System clock control register
        b7-b0   Reserved 0
        b11-b8  PCK      PCLK
        b15-b12 Reserved 0
        b19-b16 BCK      BCLK
        b22-b20 Reserved 0
        b23     PSTOP1   BCLK control bit
        b27-b24 ICK      System clock (ICLK) select bit
        b31-b28 FCK      Flash IC Clock(FCLK) select bit
		 */
		SYSTEM.SCKCR.BIT.PSTOP1     = 0;    /* BCLK Pin output enable */

		/* Deep Standby control register
        b0-b1 DEEPCUT  RAM off bit
        b5-b2 Reserved 0
        b6    IOKEEP   IO port keep bit
        b7    DPSBY    Deep standby bit
		 */
		SYSTEM.DPSBYCR.BIT.IOKEEP   = 0;    /* IO status keep disable */

		/* Reset status register
        b0    POFR     Power on reset Detection flag
        b1    LVD0F    Voltage Watch 0 reset detection flag
        b2    LVD1F    Voltage Watch 1 reset detection flag
        b3    LVD2F    Voltage Watch 2 reset detection flag
        b6-b4 Reserved 0
        b7    DPSRSTF  Deep software standby reset flag
		 */
		SYSTEM.RSTSR0.BIT.DPSRSTF   = 0;    /* Deep software standby reset */
	}
	else
	{
		/* Port mode register (Xin-Xout) and start main clock */
		/* Port mode register
        b0 B0 Pn0 pin mode control bit
        b1 B1 Pn1 pin mode control bit
        b2 B2 Pn2 pin mode control bit
        b3 B3 Pn3 pin mode control bit
        b4 B4 Pn4 pin mode control bit
        b5 B5 Pn5 pin mode control bit
        b6 B6 Pn6 pin mode control bit
        b7 B7 Pn7 pin mode control bit
		 */
		PORT3.PMR.BIT.B6        = 1;

		/* Port mode register
        b0 B0 Pn0 pin mode control bit
        b1 B1 Pn1 pin mode control bit
        b2 B2 Pn2 pin mode control bit
        b3 B3 Pn3 pin mode control bit
        b4 B4 Pn4 pin mode control bit
        b5 B5 Pn5 pin mode control bit
        b6 B6 Pn6 pin mode control bit
        b7 B7 Pn7 pin mode control bit
		 */
		PORT3.PMR.BIT.B7        = 1;

		/* Main clock Oscillator wait control register
        b4-b0   MSTS     Main clock Oscillator wait time bit
        b7-b5   Reserved 0
		 */
		SYSTEM.MOSCWTCR.BYTE    = 0x0D; /* 131072 state */

		/* Start PLL Controler */
		/* PLL control register ( PLIDIV = 12MHz(/1), STC = 192MHz(*16) )
        b1-b0   PLIDIV   PLL input division ratio select bit
        b7-b2   Reserved 0
        b13-b8  STC      frequency synthesizer set bit
        b15-b14 Reserved 0
		 */
		SYSTEM.PLLCR.WORD       = 0x0F00;

		/* Main clock Oscillator control register
        b4-b0   MOSTP     Main clock Oscillator stop bit
        b7-b5   Reserved 0
		 */
		SYSTEM.MOSCCR.BYTE      = 0x00; /* EXTAL ON */

		/* PLL control register2
        b0      PLLEN    PLL stop control bit
        b7-b1   Reserved 0
		 */
		SYSTEM.PLLCR2.BYTE      = 0x00; /* PLL ON */

		/* PLL wait control register(Default:4194304cycle)
        b4-b0   PSTS     PLL wait time set bit
        b7-b5   Reserved 0
		 */
		SYSTEM.PLLWTCR.BYTE     = 0x0F;

		/* wait over 11ms */
		for(i = 0; i< 0x14A; i++)
		{
		}

		/* System clock control register(ICK=PLL/2,BCK,FCK,PCK=PLL/4)
        b7-b0   Reserved 0
        b11-b8  PCK      PCLK
        b15-b12 Reserved 0
        b19-b16 BCK      BCLK
        b22-b20 Reserved 0
        b23     PSTOP1   BCLK control bit
        b27-b24 ICK      System clock (ICLK) select bit
        b31-b28 FCK      Flash IC Clock(FCLK) select bit
		 */
		//= 0x21822222;
		SYSTEM.SCKCR.LONG       = 0x21422222;

		/* System clock control register3(CKSEL(10-8)=PLL)
        b7-b0   Reserved 0
        b10-b8  CKSEL    Clock source select bit
        b15-b11 Reserved 0
		 */
		SYSTEM.SCKCR3.WORD      = 0x0400;

		/* System clock control register2(UCK(7-4)=48MHz(/4))
        b3-b0   IEBCK    IEBUS clock(IECLK) select bit
        b7-b4   UCK      USB clock(UCLK) select bit
        b15-b8  Reserved 0
		 */
		SYSTEM.SCKCR2.BIT.UCK   = 3;

		/* Module stop control register (Disable ACSE)
        b3-b0   Reserved 0
        b4      MSTPA4   8bit timer3,2 stop bit
        b5      MSTPA5   8bit timer1,0 stop bit
        b8-b6   Reserved 0
        b9      MSTPA9   Multifunction timer unit0 stop bit
        b10     MSTPA10  Programmable pulse unit1 stop bit
        b11     MSTPA11  Programmable pulse unit0 stop bit
        b12     MSTPA12  16Bit timer pulse unit1 stop bit
        b13     MSTPA13  16Bit timer pulse unit0 stop bit
        b14     MSTPA14  Compare timer unit1 stop bit
        b15     MSTPA15  Compare timer unit0 stop bit
        b16     Reserved 0
        b17     MSTPA17  12bit AD stop bit
        b18     Reserved 0
        b19     MSTPA19  DA stop bit
        b22-b20 Reserved 0
        b23     MSTPA23  10bit AD unit0 stop bit
        b24     MSTPA24  Module stop A24 set bit
        b26-b25 Reserved 0
        b27     MSTPA27  Module stop A27 set bit
        b28     MSTPA28  DMA/DTC stop bit
        b29     MSTPA29  Module stop A29 set bit
        b30     Reserved 0
        b31     ACSE     All clock stop bit
		 */
		SYSTEM.MSTPCRA.LONG     = 0x7FFFFFFF;


#if USB_FUNCSEL_USBIP0_PP != USB_NOUSE_PP
		/* Module stop control register
        b0     MSTPB0   CAN stop bit
        b14-b1 Reserved 0
        b15    MSTPB15  Ether DMA stop bit
        b16    MSTPB16  Serial peri1 stop bit
        b17    MSTPB17  Serial peri0 stop bit
        b18    MSTPB18  USB port1 stop bit
        b19    MSTPB19  USB port0 stop bit
        b20    MSTPB20  I2C interface1 stop bit
        b21    MSTPB21  I2C interface0 stop bit
        b22    Reserved 0
        b23    MSTPB23  CRC stop bit
        b24    Reserved 0
        b25    MSTPB25  Serial interface6 stop bit
        b26    MSTPB26  Serial interface5 stop bit
        b27    Reserved 0
        b28    MSTPB28  Serial interface3 stop bit
        b29    MSTPB29  Serial interface2 stop bit
        b30    MSTPB30  Serial interface1 stop bit
        b31    MSTPB31  Serial interface0 stop bit
		 */
		SYSTEM.MSTPCRB.BIT.MSTPB19  = 0u;   /*  Enable USB0 module */
#endif  /* USB_FUNCSEL_USBIP0_PP != USB_NOUSE_PP */

#if USB_FUNCSEL_USBIP1_PP != USB_NOUSE_PP
		/* Module stop control register
        b0     MSTPB0   CAN stop bit
        b14-b1 Reserved 0
        b15    MSTPB15  Ether DMA stop bit
        b16    MSTPB16  Serial peri1 stop bit
        b17    MSTPB17  Serial peri0 stop bit
        b18    MSTPB18  USB port1 stop bit
        b19    MSTPB19  USB port0 stop bit
        b20    MSTPB20  I2C interface1 stop bit
        b21    MSTPB21  I2C interface0 stop bit
        b22    Reserved 0
        b23    MSTPB23  CRC stop bit
        b24    Reserved 0
        b25    MSTPB25  Serial interface6 stop bit
        b26    MSTPB26  Serial interface5 stop bit
        b27    Reserved 0
        b28    MSTPB28  Serial interface3 stop bit
        b29    MSTPB29  Serial interface2 stop bit
        b30    MSTPB30  Serial interface1 stop bit
        b31    MSTPB31  Serial interface0 stop bit
		 */
		SYSTEM.MSTPCRB.BIT.MSTPB18 = 0u;    /* Enable USB1 module */
#endif /* USB_FUNCSEL_USBIP1_PP != USB_NOUSE_PP */

		/* Port mode register (BCLK) */
		/* System control register (External Bus Enable)
        b0     ROME     Internal ROM Enable bit
        b1     EXBE     External Bus Enable bit
        b7-b2  Reserved 0
        b15-b8 KEY[7:0] SYSCR0 Key code
		 */
		SYSTEM.SYSCR0.WORD              = 0x5A03;

		/* Port mode register
        b0 B0 Pn0 pin mode control bit
        b1 B1 Pn1 pin mode control bit
        b2 B2 Pn2 pin mode control bit
        b3 B3 Pn3 pin mode control bit
        b4 B4 Pn4 pin mode control bit
        b5 B5 Pn5 pin mode control bit
        b6 B6 Pn6 pin mode control bit
        b7 B7 Pn7 pin mode control bit
		 */
		PORT5.PMR.BIT.B3            = 1;

		/* External Bus clock control register (BCLK * 1/2)
        b0     BCLKDIV  BCLK Pin output select bit
        b7-b1  Reserved 0
		 */
		SYSTEM.BCKCR.BIT.BCLKDIV    = 1;

		/* System clock control register(BCLK Pin output enable)
        b7-b0   Reserved 0
        b11-b8  PCK      PCLK
        b15-b12 Reserved 0
        b19-b16 BCK      BCLK
        b22-b20 Reserved 0
        b23     PSTOP1   BCLK control bit
        b27-b24 ICK      System clock (ICLK) select bit
        b31-b28 FCK      Flash IC Clock(FCLK) select bit
		 */
		SYSTEM.SCKCR.BIT.PSTOP1     = 0;
	}
}
/******************************************************************************
End of function usb_cpu_McuInitialize
 ******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_LcdInit
Description     : LCD initialize
Arguments       : none
Return value    : none
 ******************************************************************************/
void usb_cpu_LcdInit(void)
{
	//PORTA.PDR.BIT.B6    = 1;            /* LCD BACKLIGHT	*/
	//LCD_BACKLIGHT = ON;
	/* Data register
    b0 B0 Pn0 output data bit
    b1 B1 Pn1 output data bit
    b2 B2 Pn2 output data bit
    b3 B3 Pn3 output data bit
    b4 B4 Pn4 output data bit
    b5 B5 Pn5 output data bit
    b6 B6 Pn6 output data bit
    b7 B7 Pn7 output data bit
	 */
	PORTE.PODR.BIT.B4   = 0;
	PORTE.PODR.BIT.B5   = 0;
	PORTE.PODR.BYTE = 0;

	/* Data direction register
    b0 B0 Pn0 in/out select bit
    b1 B1 Pn1 in/out select bit
    b2 B2 Pn2 in/out select bit
    b3 B3 Pn3 in/out select bit
    b4 B4 Pn4 in/out select bit
    b5 B5 Pn5 in/out select bit
    b6 B6 Pn6 in/out select bit
    b7 B7 Pn7 in/out select bit
	 */
	PORTE.PDR.BIT.B4    = 1u;           /* Out LCD-E */
	PORTE.PDR.BIT.B5    = 1u;           /* Out LCD-RS */
	PORTE.PDR.BYTE      |= 0x0F;        /* Out LCD-DB4/DB7 */
}
/******************************************************************************
End of function usb_cpu_LcdInit
 ******************************************************************************/

/******************************************************************************
A/D functions
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_AdInit
Description     : 12bit A/D initialize
Arguments       : none
Return value    : none
 ******************************************************************************/
void usb_cpu_AdInit(void)
{
	/* Protect register
    b0    PRC0     Protect bit0
    b1    PRC1     Protect bit1
    b2    Reserved 0
    b3    PRC3     Protect bit3
    b7-b4 Reserved 0
    b15-b8 PRKEY   PRC Key code bit
	 */
	SYSTEM.PRCR.WORD    = 0xA503;   /* Protect off */

	/* Enable S12AD module(MSTPA17)
    b3-b0   Reserved 0
    b4      MSTPA4   8bit timer3,2 stop bit
    b5      MSTPA5   8bit timer1,0 stop bit
    b7-b6   Reserved 0
    b8      MSTPA8   Multifunction timer unit1 stop bit
    b9      MSTPA9   Multifunction timer unit0 stop bit
    b10     MSTPA10  Programable pulse unit1 stop bit
    b11     MSTPA11  Programable pulse unit0 stop bit
    b13-12  Reserved 0
    b14     MSTPA14  Compare timer unit1 stop bit
    b15     MSTPA15  Compare timer unit0 stop bit
    b16     Reserved 0
    b17     MSTPA17  12bit AD stop bit
    b18     Reserved 0
    b19     MSTPA19  DA stop bit
    b21-b20 Reserved 0
    b22     MSTPA22  10bit AD unit1 stop bit
    b23     MSTPA23  10bit AD unit0 stop bit
    b26-b24 Reserved 0
    b27     Reserved 0
    b28     MSTPA28  DMA/DTC stop bit
    b29     MSTPA29  EXDMA stop bit
    b30     Reserved 0
    b31     ACSE     All clock stop bit
	 */
	SYSTEM.MSTPCRA.BIT.MSTPA17  = 0;

	/* Protect register
    b0    PRC0     Protect bit0
    b1    PRC1     Protect bit1
    b2    Reserved 0
    b3    PRC3     Protect bit3
    b7-b4 Reserved 0
    b15-b8 PRKEY   PRC Key code bit
	 */
	SYSTEM.PRCR.WORD    = 0xA500;   /* Protect on */

	/* Data direction register (P40/AN0 in)
    b0 B0 Pn0 in/out select bit
    b1 B1 Pn1 in/out select bit
    b2 B2 Pn2 in/out select bit
    b3 B3 Pn3 in/out select bit
    b4 B4 Pn4 in/out select bit
    b5 B5 Pn5 in/out select bit
    b6 B6 Pn6 in/out select bit
    b7 B7 Pn7 in/out select bit
	 */
	PORT4.PDR.BIT.B0    = 0;

	/* A/D channel select register (AN0 select)
    b7-b0  ANS      A/D conversion channel select bit
    b15-b8 Reserved 0
	 */
	S12AD.ADANS0.BIT.ANS0 = 0x01;
}
/******************************************************************************
End of function usb_cpu_AdInit
 ******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_AdData
Description     : Read A/D data
Arguments       : none
Return value    : uint32_t              : AD data
 ******************************************************************************/
uint32_t usb_cpu_AdData(void)
{
	uint16_t    dat;

	/* A/D control register (A/D start)
    b0    EXTRG    Trigger select bit
    b1    TRGE     Trigger enable bit
    b3-b2 CKS      Clock select bit
    b4    ADIE     A/D scan interrupt enable bit
    b5    Reserved 0
    b6    ADCS     Scan conversion mode select bit
    b7    ADST     A/D start bit
	 */
	S12AD.ADCSR.BIT.ADST = 1;

	/* A/D control register (Wait A/D start bit clear)
    b0    EXTRG    Trigger select bit
    b1    TRGE     Trigger enable bit
    b3-b2 CKS      Clock select bit
    b4    ADIE     A/D scan interrupt enable bit
    b5    Reserved 0
    b6    ADCS     Scan conversion mode select bit
    b7    ADST     A/D start bit
	 */
	while( S12AD.ADCSR.BIT.ADST )
	{
		/* Do nothing */
	}

	/* A/D data register (AN0)
    b11-b0           12bit A/D data
    b15-b12 Reserved 0
	 */
	dat = S12AD.ADDR0;
	return (uint32_t)dat;
}
/******************************************************************************
End of function usb_cpu_AdData
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_get_dtc_block_count
Description     : 
Arguments       : 
Return value    : 
 ******************************************************************************/
uint16_t usb_cpu_get_dtc_block_count(USB_UTR_t *ptr)
{
	return usb_dtcreg[ptr->ip].CRB;
}
/******************************************************************************
End of function
 ******************************************************************************/



/******************************************************************************
WDOG functions
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_WdogInit
Description     : init Io for wdog
Arguments       : 
Return value    : 
 ******************************************************************************/
void usb_cpu_WdogInit(void)
{
	PORT2.PDR.BIT.B0 = 1;
}

/******************************************************************************
End of function
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_WdogRefresh
Description     : refresh Wdog pin
Arguments       : 
Return value    : 
 ******************************************************************************/
void usb_cpu_WdogRefresh(void)
{
	if (StateWDog == 0)
		PORT2.PODR.BIT.B0 ^= 1;
}

/******************************************************************************
End of function
 ******************************************************************************/

/******************************************************************************
End  Of File
 ******************************************************************************/
