/*
 * bfm.h
 *
 *  Created on: 26 févr. 2013
 *      Author: duhautf
 */

#ifndef BFM_H_
#define BFM_H_

void BfmChrono(void);
uchar BfmRx(uchar *r, uint *top, uint cur, uint size, uint ttop);
uchar BfmFrm(uchar *r, uint *rtop, uint size, uint ttop);
void BfmTimer(void);
void BfmInit(void);
void BfmReset(void);
uchar BfmFnc(uchar *r, uint ttop);
uchar BfmEasyFrame(uchar *r, uint ttop);

#endif /* BFM_H_ */