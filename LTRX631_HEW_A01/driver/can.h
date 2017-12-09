/*
 * can.h
 *
 *  Created on: 26 févr. 2013
 *      Author: duhautf
 */

#ifndef CAN_H_
#define CAN_H_

#include "type.h"

#define TIMEOUT_LLC       4     // sec
#define TIMEOUT_CANBUS    2     // sec
#define TIMEOUT_RAMRW     100   // ms


/* CAN pduf */
#define CANEEP      0                       /* EepCfg PDUF 0 */
#define CANSERIAL1  1
#define CANSERIAL2  2
#define CANMENU     3                       /* 8 Frame -> 3 to 10 */

#define CANTYPEBAY  11                      /* PGN 2816 : Detect type of cabinet */
#define CANDATETIME 12                      /* PGN 3072 : Update date-time */
#define CANKEYSTATE 13                      /* PGN 3328 : Key status */

#define CANSETUP_01 14                      
#define CANSETUP_02 15                      
#define CANSETUP_03 16                      
#define CANSETUP_04 17                      
#define CANSETUP_05 18                      
#define CANSETUP_06 19                      
#define CANSETUP_07 20

#define CANLAN_01   21                      
#define CANLAN_02   22                      
#define CANLAN_03   23                      
#define CANLAN_04   24                      
#define CANLAN_05   25                      
#define CANLAN_06   26                      
#define CANLAN_07   27

#define CANFIRMWARE 28


#define CANCOUNTER  65                      /* Global counter in EEP*/
#define CANVERSION  52                      /* Module Version */
#define CANRAMRO    240                     /* RamRO PDUF 240  */
#define CANRAMRW    16               		/* RamRW PDUF 16 to 31 */
#define CANREMOTE   128                     /* PGN 32768 : Remote Frame */
#define CANCLM      234                     /* CAN CLAIM : request */
#define CANCLMED    238                     /* CAN CLAIM : answer */


/* CAN MASK */
#define CAN_SA_MASK             0x000000FF  /* Source address mask */
#define CAN_DA_MASK             0x0000FF00  /* Destination address mask */
#define CAN_OFFSET_RAMRO_MASK   0x000F0000  /* Offset RAMRo mask */
#define CAN_BMESSAGE_MASK       0x00F00000  /* Broadcast message mask */
#define CAN_PDUF_MASK           0x00FF0000  /* Claim mask*/


void CAN_Chrono (void);
void Init_CAN (void);
void J1939_Read(void);
void J1939_ReadRemoteRequest(void);
void J1939_Write (uchar id_da);
void J1939_RequestData (uchar id_da, uint Pduf);
void J1939_ReadSerialNumber (uchar id_da, uchar Pduf);
void J1939_ReadTypeBay (uchar id_da, uchar Pduf);
void J1939_ReadMenuConfig (uchar id_da, uchar Pduf);
void J1939_WriteSerialNumber (uchar Pduf);
void J1939_WriteMenuConfig (uchar Pduf);
void Reset_IdleTimerCAN (void);
void J1939_WriteRTC (uchar Pduf);
void J1939_WriteSetup(uchar Pduf);
void J1939_ReadKeyState (uchar id_da, uchar Pduf);
void J1939_ReadSetup(uchar id_da, uchar Pduf);

void J1939_ReadLAN(uchar id_da, uchar Pduf);
void J1939_WriteLAN (uchar Pduf);

void CAN1_TXM1_ISR(void);
void CAN1_RXM1_ISR(void);
void CAN_ERS_ISR(void);

static void ENABLE_IRQ(void);
static void DISABLE_IRQ(void);


#endif /* CAN_H_ */
