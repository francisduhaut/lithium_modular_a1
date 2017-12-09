/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : common.h
 * Version      : 1.02
 * Description  : Common functions for CANloader and UserApp. These are functions that can
potentially differentiate if user so desires and in that case moved to CANloader 
and UserApp respectively. For now, to only have to change code in one place, they 
are the same.
 ************************************************************************************************************************
 * History : DD.MM.YYYY Ver. Description
 *         : 12.07.2011 1.00 First Release.
 *         : 10.5.10         FoCAN for RX/62N.
 *         : 6.2.2012        For RX63N
 ***********************************************************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H

/******************************************************************************
Includes     <System Includes> , "Project Includes"
 ******************************************************************************/
/* CAN library definition */
#include "r_can_api.h"

/* CAN channel numbers */                  
enum CAN_channel_num {	CH_0 = 0,
	CH_1 = 1,
	CH_2 = 2
};

/******************************************************************************
Macro definitions
 ******************************************************************************/
/* Define message box for control message responses */
#define		CANBOX_0_MONITORING	    	1
#define   	CANBOX_0_REMOTE_REQUEST 	2
#define		CANBOX_0_RAMRW_TX       	3

#define		CANBOX_1_MONITORING	    	10
#define   	CANBOX_1_REMOTE_REQUEST 	11
#define		CANBOX_1_RAMRW_TX       	12

/* This flag is used for setting/clearing the I bit in the PSW */
#define I_FLAG	(0x00010000)    

/******************************************************************************
Global variables and functions imported (externs)
 ******************************************************************************/
/* DATA */
extern uint32_t     focan_state;
/* FUNCTIONS */
#pragma inline_asm  Change_PSW_PM_to_UserMode
extern void         Change_PSW_PM_to_UserMode(void);

#pragma inline_asm  Change_PSW_PM_to_SupervisorMode
extern void         Change_PSW_PM_to_SupervisorMode(void);

/******************************************************************************
Constant definitions
 *****************************************************************************/
/******************************************************************************
Global variables and functions exported
 ******************************************************************************/
/* DATA */
extern can_frame_t      focan_rx_frame;    /* Received control and data messages stored here */
extern can_frame_t      focan_rx_remote;   /* Received control and data messages stored here */
extern can_frame_t      focan_tx_frame;    /* Control confirm msgs */

/* FUNCTIONS */
extern uint32_t     FoCanInit(uint32_t g_CAN_channel, uint32_t frame_id_mode);

#endif //_COMMON_H
/* eof */

