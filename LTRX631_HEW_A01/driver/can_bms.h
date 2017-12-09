/*
 * can_bms.h
 *
 *  Created on: 17/10/2017
 *      Author: duhautf
 */
 
#ifndef CAN_BMS_H_
#define CAN_BMS_H_

#define	TIME_TX_BMS			100
#define TIME_RX_BMS			2000
#define	BMS_MAX_TX_FRAME	3
#define	BMS_TX_ACT_FRAME	0
#define	BMS_TX_STS_FRAME	1

#define	BMS_TX_STS			0x610
#define	BMS_TX_ACT			0x611
#define	BMS_CHRGR_CTL		0x618
#define	BMS_SOC_DATA		0x302
#define	BMS_BATTP_DATA		0x306
#define	BMS_BATP_DATA		0x308
#define	BMS_BATU_DATA		0x309

void Init_CAN_bms (void);
void CAN_bms_Chrono (void);
void CAN_bms_TX_ACT (void);
void CAN_bms_TX_STS (void);

void CAN1_TXM1_ISR(void);
void CAN1_RXM1_ISR(void);

#endif /* CAN_BMS_H_ */
