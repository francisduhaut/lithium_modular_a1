void IQJbusChrono(void);

uint IQJbRx(uchar *r, uint *top, uint cur, uint size, uchar *t, uint ttop);

uchar IQJbFnc3Ram(uchar *t, uchar c, uint a, uchar n);

uchar IQJbFnc3Rom(uchar *t, uchar c, uint a, uchar n);

uchar IQJbFnc3EepSeg(uchar *t, uchar c, uint a, uchar n);

uchar IQJbFnc3FMC(uchar *t, uchar c, uint a, uchar n);

uchar IQJbFnc5FMC(uchar *t, uchar c, uint a, uint g);

uchar IQJbFnc5Ram(uchar *t, uchar c, uint a, uint g);

uchar IQJbFnc16Ram(uchar *r, uint rtop, uint size, uchar *t, uint a, uchar n);

uchar IQJbFnc16EepSeg(uchar *r, uint rtop, uint size, uchar *t, uint a, uchar n);

uchar IQJbFnc16FMC(uchar *r, uint rtop, uint size, uchar *t, uint a, uchar n);

uchar IQJbErr(uchar *t, uchar c, uchar JbErrCode);

uchar IQJbFnc22Ram(uchar *t,uint a, uint and, uint or);

uchar IQJbFnc22FMC(uchar *t, uint a, uint and, uint or);

uint FMCRead(uint a);

uchar FMCWrite(uint a, uint d);