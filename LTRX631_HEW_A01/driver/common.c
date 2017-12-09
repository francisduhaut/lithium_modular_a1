/*
 * common.c
 *
 *  Created on: 01/04/2013
 *      Author: Francis DUHAUT
 */

#include "include.h"
#include "r_flash_api_rx600_private.h"


#define MAX_POSSIBLE_APP_LENGTH     0x200000
#define LED_RESET_PERIOD            0x10000
#define LED_DELAY                   0x80000
#define SID_MASK_BIT_0              0x3F0
#define RAMRO_MASK_BIT              0 //0xFFFFFFFF

/******************************************************************************
Global variables and functions imported (externs)
 ******************************************************************************/
/* DATA */
extern const char     unlockcode[8];
/* FUNCTIONS */
void lcd_delay(uint32_t units);

/******************************************************************************
Global variables and functions exported
 ******************************************************************************/
/* DATA */
#pragma section B
uint32_t focan_state;
uint32_t nr_times_reached_busoff[2];

#pragma section

/* CAN receive send messages stored here */
can_frame_t     focan_rx_frame;
can_frame_t     focan_rx_remote;
can_frame_t     focan_tx_frame;

/* FUNCTIONS */

/******************************************************************************
Global variables and functions private to the file
 ******************************************************************************/

/*****************************************************************************
Name:           FoCanInit
Parameters:     -             
Returns:        -
Description:    Initialize CAN for Flash-o-CAN.
 *****************************************************************************/
uint32_t FoCanInit(uint32_t g_CAN_channel, uint32_t frame_id_mode)
{
	uint32_t api_status = R_CAN_OK;

	/* Init CAN. */
	api_status = R_CAN_Create(g_CAN_channel, frame_id_mode);

	/* Normal CAN bus usage. */
	api_status |= R_CAN_PortSet(g_CAN_channel, ENABLE);

	if (g_CAN_channel == CH_0)
	{
		api_status |= R_CAN_RxSet(g_CAN_channel, CANBOX_0_MONITORING, DATA_RAMRO, DATA_FRAME);
		api_status |= R_CAN_RxSet(g_CAN_channel, CANBOX_0_REMOTE_REQUEST, DATA_RAMRO, REMOTE_FRAME);
		R_CAN_RxSetMask(g_CAN_channel, CANBOX_0_MONITORING, RAMRO_MASK_BIT);
		R_CAN_RxSetMask(g_CAN_channel, CANBOX_0_REMOTE_REQUEST, RAMRO_MASK_BIT);
	}
	else if (g_CAN_channel == CH_1)
	{
		api_status |= R_CAN_RxSet(g_CAN_channel, CANBOX_1_MONITORING, DATA_RAMRO, DATA_FRAME);
		api_status |= R_CAN_RxSet(g_CAN_channel, CANBOX_1_REMOTE_REQUEST, DATA_RAMRO, REMOTE_FRAME);
		R_CAN_RxSetMask(g_CAN_channel, CANBOX_1_MONITORING, RAMRO_MASK_BIT);
		R_CAN_RxSetMask(g_CAN_channel, CANBOX_1_REMOTE_REQUEST, RAMRO_MASK_BIT);
	}
	nr_times_reached_busoff[0] = 0;
	nr_times_reached_busoff[1] = 0;

	return api_status;
}/* end FoCanInit() */

/* eof */
