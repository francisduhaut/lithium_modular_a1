/*
 * jbus.h
 *
 *  Created on: 1 mars 2013
 *      Author: duhautf
 */

#ifndef JBUS_H_
#define JBUS_H_

#include "type.h"

void  JBUS_Chrono(void);
uint  JbusRx(uchar *r, uint *rtop, uint cur, uint size, uchar *t, uint ttop);
uchar JbErr(uchar *t, uchar c, uchar JbErrCode);
uchar JbFnc3Ram(uchar *t, uchar c, uint a, uchar n);
uchar JbFnc3Rom(uchar *t, uchar c, uint a, uchar n);
uchar JbFnc3E2Seg(uchar *t, uchar c, uint a, uchar n);
uchar JbFnc5Ram(uchar *t, uchar c, uint a, uint g);
uchar JbFnc16Ram(uchar *r, uint rtop, uint size, uchar *t, uint a, uchar n);
uchar JbFnc16E2Seg(uchar *r, uint rtop, uint size, uchar *t, uint a, uchar n);
uchar JbFnc22Ram(uchar *t, uint a, uint and, uint or);
uint  Size(uint l, uint m, uint n);

#endif /* JBUS_H_ */
