/*
 * crc.h
 *
 *  Created on: 1 mars 2013
 *      Author: duhautf
 */

#ifndef CRC_H_
#define CRC_H_

#include "type.h"

void CrcAdd(uchar *s, uint n, uint m, uint l);
uint CrcFwd(uint c, uchar s);
uint CrcCalc(uchar *s, uint n, uint m, uint l);
ulong Sqrt(ulong x);

#endif /* CRC_H_ */
