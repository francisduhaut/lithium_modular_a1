/*
 * lcdcharge.h
 *
 *  Created on: 22 févr. 2013
 *      Author: duhautf
 */

#ifndef LCDCHARGE_H_
#define LCDCHARGE_H_

#include "type.h"

extern const uchar ListWord16_State[25][NBLANG][17];
extern const uchar ListWord6_Profile[24][NBLANG][7];
extern char version[8];

void LCD_Chrono (void);
void LCD_BackLightManagement (void);
void LCD_LedRefresh (void);
char LCD_Charge (void);
void LCD_ChargeInfo (void);
void LCD_LedFaultBlink (ulong bltime, uchar buzstate);
void LCD_LedGreenBlink (ulong bltime, uchar buzstate);
void LCD_LedYellowBlink (ulong bltime, uchar buzstate);
void LCD_DataCharge (void);
void LCD_DataCompEqual (void);
void LCD_DataAvail (void);
void LCD_RefreshScreenSaver (void);
void LCD_ScreenSaver (void);

#endif /* LCDCHARGE_H_ */
