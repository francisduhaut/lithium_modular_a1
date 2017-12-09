/*
 * module.h
 *
 *  Created on: 30 jully. 2013
 *      Author: duhautf
 */

#ifndef MODULE_H_
#define MODULE_H_

#include "type.h"

void Init_MOD (void);
uint ModFindIMaxModule (void);
uint ModFindIMaxModuleID (uchar ModID, uint Ver);
uint ModFindPMaxModuleID (uchar ModID);
void ModFindImax (void);
void ModCalcDef(void);
void ModCalc(void);
void ModList(void);
void ModShare(void);
void ModV(void);
void ModCheckDef(void);
uchar ModFault(void);
void ModAckDef(void);
void ModTempConAckDef(void);
void ModClearDefTemp(void);
void ModSimpleAckDef(uchar i);
void ModResetEfficiency (void);
void MOD_Chrono(void);
void ModResetCntAh(void);
void ModSort(void);
void ModCalcVbatLLC(void);
uchar ModChargerID(void);
void ModSelect3Phases(void);
void ModI(void);
void CheckDerating (void);
void ModSortLeftRight (void);

// MODIF 2.8
void ModV_8BAY_R1(void);
void ModV_8BAY_R2(void);

void CheckDeratingSinglePhase(void);
void SaveE1Memo (uchar SlotNumber);
uchar CheckInputVoltage (uint V1, uint V2, uint V3, uint OFFSET_V1, uint OFFSET_V2V3);
uchar CheckVbatModule2Module (sint *VbatModule, uchar NbrModule, uint OFFSET_V);

#endif /* MODULE_H_ */
