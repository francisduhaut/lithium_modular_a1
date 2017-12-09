/*
 * rtc.h
 *
 *  Created on: 22 févr. 2013
 *      Author: duhautf
 */

#ifndef RTC_H
#define RTC_H

#include "type.h"

#define EUROPE 1
#define USCANADA 2
#define AUSTRALIA 3

uint32_t CalculateHours (StructDateTime *dt);

void RTC_Chrono (void);
void Init_RTC (void);
void Init_RTC_S1(void);
void Init_RTC_S2(void);
void Write_RTC (uint32_t ct);

uchar BcdUchar(uchar c);
uchar UcharBcd(uchar c);
uchar DayOfWeek(StructDateTime *dt);
void CalcNextDayLight(uint32_t ct);

#endif
