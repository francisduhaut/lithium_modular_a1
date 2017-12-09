/*
 * csv.h
 *
 *  Created on: 19 mars 2013
 *      Author: duhautf
 */

#ifndef CSV_H_
#define CSV_H_

void  CsvLong(uchar c, uchar *l, const uchar *t, long v, uchar dp);
void  CsvText20(uchar c, const uchar *l, const uchar *t, const uchar *v);
void  CsvText20_10(uchar c, const uchar *l, const uchar *t, const uchar *v);
void  CsvText8(uchar c, uchar *l, const uchar *t, const uchar *v);
void  CsvDateHour(uchar c, uchar *l, uchar *t, StructDateTime dt);
void  CsvNewFileName(uchar t, uchar *n);
void  CsvConfigLine1(uchar c, uchar *l, uint *n);
void  CsvConfigLine2(uchar c, uchar *l, uint *n);
void  CsvStatusLine(uchar c, uchar *l, uint *n);
void  CsvMemoLine(uchar c, uchar *l, uint *n, StructMemo *m);
void  CsvProfileLine(uchar c, uchar *l, uint *n);
void  CsvLongHexa(uchar c, uchar *l, const uchar *t, long v, uchar dp);
void  CsvMemoE1Line(uchar c, uchar *l, uint *n, StructE1Memo *m);

#endif /* CSV_H_ */
