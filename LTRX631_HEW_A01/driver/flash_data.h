
#ifndef FLASH_DATA_H
#define FLASH_DATA_H

#include "type.h"

#define PROG_SIZE   256
#define BUFF_SIZE   256

void  E2FLASH_Chrono (void);
void  Init_FlashData(void);
void  Erase_FlashData(void);
int   Erase_SegFlashData (int SegNumber);
int   Write_SegFlashData (int SegNumber, uchar *segment);
uint  Read_SegFlashData (int SegNumber, uchar *segment);
void  Read_Config (void);

#endif
