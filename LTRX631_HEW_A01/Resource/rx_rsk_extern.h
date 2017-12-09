/******************************************************************************
 * File Name    : rx_rsk_extern.h
 * Version      : 2.00
 * Device(s)    : Renesas SH-Series, RX-Series
 * Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
 *              : Renesas RX Standard Toolchain
 * OS           : Common to None and uITRON 4.0 Spec
 * H/W Platform : Independent
 * Description  : RX RSK Extern
 ******************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 29.07.2011 0.50    First Release
 ******************************************************************************/

/* $Id: rx_rsk_extern.h 143 2012-05-07 09:16:46Z tmura $ */

#ifndef __RX63NRSK_EXTERN_H__
#define __RX63NRSK_EXTERN_H__


/******************************************************************************
Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_usb_ctypedef.h"     /* Type define */


/******************************************************************************
External variables and functions
 ******************************************************************************/
/* SW input driver functions */

/* AD driver functions */
extern void         usb_cpu_AdInit(void);
extern uint32_t     usb_cpu_AdData(void);

/* LED driver functions */
extern void         usb_cpu_LedInit(void);
extern void         usb_cpu_LedSetBit(uint8_t bit, uint8_t data);

/* Interrupt handler for nonOS */

#pragma interrupt usb_cpu_d1fifo_int_hand   /* D1FIFO int (USB0) */ //$REA No such routine defined for 63N!
#pragma interrupt usb2_cstd_D1fifo1IntHand  /* D1FIFO int (USB1) */ //$REA No such routine defined for 62N, 630, 63N!
#pragma interrupt usb_cpu_usb_int_hand_r    /* RESUME int (USB0) */ //$REA No such routine defined for 63N!
#pragma interrupt usb2_cpu_usb_int_hand_r   /* RESUME int (USB1) */ //$REA No such routine defined for 630, 63N!

#pragma interrupt usb_cstd_DummyIntFunction /* Dummy */


#endif  /* __RX63NRSK_EXTERN_H__ */
/******************************************************************************
End  Of File
 ******************************************************************************/
