/*
 * lcdmenu.h
 *
 *  Created on: 25 févr. 2013
 *      Author: duhautf
 */

#ifndef LCDMENU_H_
#define LCDMENU_H_

#include "type.h"

extern uint8_t MnuValCorrection;

void DrawMenuFirstEntryTextImage (void);
void DrawMenupartialTwo (void);
void DrawMenupartialOne (void);
void DrawMenuPartialOneList (void);

void  LcdInfo(void);
void  LcdBit(void);
void  LCDMenu_Level(void);
void  LCDMenu (void);
void  LCDMenu_Liste(void);
void  LCDMenu_Fun(void);
void  LCDMenu_Input(uchar t);
void  LCDMenu_FunStatus(void);
void  LCDMenu_Info(void);
BYTE  IsPasswordMenu (void);
void LCDMenu_BootApp(void);

void  LCDMenu_Password(void);

uint  PasswdRand(void);
long  rand(void);

void  GetSetMnuList(uint a, uchar *v);
void  GetSetData(uint a, uchar *v);

void  GetSetProfil(uint a, uchar *v);
void  GetSetAutoStartOffOn(uint a, uchar *v);
void  GetSetBatCapAutoManu(uint a, uchar *v);
void  GetSetSect(uint a, uchar *v);
void  GetSetLength(uint a, uchar *v);
void  GetSetLengthM(uint a, uchar *v);
void  GetSetLengthFeet(uint a, uchar *v);
void  GetSetGetPasswd(uint a, uchar *v);
void  GetSetGetPasswd2(uint j);
void  GetSetLangage(uint a, uchar *v);
void  GetSetImax(uint a, uchar *v);
void  GetSetBatCapacity(uint a, uchar *v);
void  GetSetTempChg(uint a, uchar *v);
void  GetSetTempChgC(uint a, uchar *v);
void  GetSetTempChgF(uint a, uchar *v);
void  GetSetArea(uint a, uchar *v);
void  GetSetIequal(uint a, uchar *v);
void  GetSetEqualTime(uint a, uchar *v);
void  GetSetEqualDelay(uint a, uchar *v);
void  GetSetFactoryReset(uint a, uchar *v);
void  GetSetSetPasswd(uint a, uchar *v);
void  GetSetNbCell(uint a, uchar *v);
void  GetSetChargerType(uint a, uchar *v);
void  GetSetStatus(uint a, uchar *v);
void  GetSetMemoReset(uint a, uchar *v);
void  GetSetMemoList(uint a, uchar *v);
void  GetSetMemo(uint a, uchar *v);
void  GetSetSetTimeF(uint a, uchar *v);
void  GetSetSetTimeUS(uint a, uchar *v);
uchar CtrlDateHour(StructDateTime *dh);
void  GetSetChargeTimeType (uint a, uchar *v);
void  GetSetChargeTime(uint a, uchar *v);
void  GetSetInitDod(uint a, uchar *v);
void  GetSetFloatingOffOn(uint a, uchar *v);
void  GetSetIfloating(uint a, uchar *v);
void  GetSetVfloating(uint a, uchar *v);
void  GetSetUsbMemoWrite(uint a, uchar *v);

void  GetSetModInfoGen(uint a, uchar *v);
void  GetSetModInfo0(uint a, uchar *v);
void  GetSetModInfo1(uint a, uchar *v);
void  GetSetModInfo2(uint a, uchar *v);
void  GetSetModInfo3(uint a, uchar *v);
void  GetSetModInfo4(uint a, uchar *v);
void  GetSetModInfo5(uint a, uchar *v);

void  GetSetModInfo(uint a, uchar *v, uchar k);
void  GetSetFilPilOnOff(uint a, uchar *v);
void  GetSetUsbUpdate(uint a, uchar *v);

void  GetSetSetTime(uint a, uchar *v);
void  GetSetRefreshOffOn(uint a, uchar *v);
void  GetSetEqual1(uint a, uchar *v);
void  GetSetEqual2(uint a, uchar *v);
void  GetSetEqual3(uint a, uchar *v);
void  GetSetEqual4(uint a, uchar *v);
void  GetSetEqual5(uint a, uchar *v);
void  GetSetEqual6(uint a, uchar *v);
void  GetSetEqual7(uint a, uchar *v);

void  GetSetIdleOffOn(uint a, uchar *v);
void  GetSetProfileEqual(uint a, uchar *v);
void  GetSetVMains(uint a, uchar *v);
void  GetSetVMains480(uint a, uchar *v);    // MODIF R2.1
void  GetSetBatteryRest(uint a, uchar *v);

void  GetSetContrast(uint a, uchar *v);
void  GetSetBackOnOff(uint a, uchar *v);
void  GetSetScreenSaverTime(uint a, uchar *v);
void  GetSetPeriod(uint a, uchar *v);
void  GetSetValveDelay(uint a, uchar *v);
void  GetSetTestOptions(uint a, uchar *v);
void  GetSetComIQOffOn(uint a, uchar *v);
void  GetSetModRFTrAddr(uint a, uchar *v);
void  GetSetSerialNumber(uint a, uchar *v);
void  GetSetTempHighC(uint a, uchar *v);
void  GetSetTempHighF(uint a, uchar *v);

void  GetSetProtocol(uint a, uchar *v);
void  GetSetBaudRate(uint a, uchar *v);
void  GetSetJbAdr(uint a, uchar *v);
void  GetSetEth(uint a, uchar *v, uchar c);
void  GetSetEthIP(uint a, uchar *v);
void  GetSetEthDNS(uint a, uchar *v);
void  GetSetEthGW(uint a, uchar *v);
void  GetSetEthSM(uint a, uchar *v);
void  GetSetEthA10100(uint a, uchar *v);

void  GetSetUsbProfileWrite(uint a, uchar *v);
void  GetSetUsbProfileTime(uint a, uchar *v);
void  GetSetUsbPrgRf(uint a, uchar *v);

void  GetSetDailyChargeTimeType (uint a, uchar *v);
void  GetSetDailyChargeStartTime(uint a, uchar *v);
void  GetSetDailyChargeEndTime(uint a, uchar *v);
void  GetSetIonicCoef(uint a, uchar *v);

void  GetSetCF(uint a, uchar *v);
void  GetSetCFOnOff(uint a, uchar *v);
void  GetSetExit(uint a, uchar *v);
void  GetSetCECOnOff(uint a, uchar *v);
void  GetSetGelIphase3(uint a, uchar *v);
void  GetSetLoopOffOn(uint a, uchar *v);

void  GetSetEthSSID1(uint a, uchar *v);
void  GetSetEthSSID2(uint a, uchar *v);
void  GetSetEthSSID(uint a, uchar *v, uchar c);
void  GetSetEthSecurity(uint a, uchar *v);
void  GetSetEthPassPh1(uint a, uchar *v);
void  GetSetEthPassPh2(uint a, uchar *v);
void  GetSetEthPassPh(uint a, uchar *v, uchar c);

void  GetSetBlockOutChargeType (uint a, uchar *v);
void  GetSetBlockOutStartTime(uint a, uchar *v);
void  GetSetBlockOutEndTime(uint a, uchar *v);

void  GetSetXFCOffOn(uint a, uchar *v);

void  GetSetUsb2Config(uint a, uchar *v);
void  GetSetConfig2Usb(uint a, uchar *v);

void  GetSetBootApp (uint a, uchar *v);
void  GetSetDayLight(uint a, uchar *v);

void  GetSetEasyKitAdr(uint a, uchar *v);
void  GetSetEasyKitOnOff(uint a, uchar *v);
void  GetSetPLCPulseOffOn(uint a, uchar *v);
void  GetSetNbMod(uint a, uchar *v);

void  GetSetTheme(uint a, uchar *v);
void  GetSetReboot(uint a, uchar *v);

void GetSetClearTempCon (uint a, uchar *v);

void  GetSetEqualTimePh1(uint a, uchar *v);
void  GetSetEqualTimePh2(uint a, uchar *v);


#endif /* LCDMENU_H_ */

