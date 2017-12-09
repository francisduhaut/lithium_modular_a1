/* Multiple inclusion prevention macro */
#ifndef I2C_H
#define I2C_H


#define SCL__OUT    {PORTG.PDR.BIT.B6 = 1;}
#define SCL__0      {PORTG.PODR.BIT.B6 = 0;}
#define SCL__1      {PORTG.PODR.BIT.B6 = 1;}

#define SDA__IN     {PORTG.PDR.BIT.B7 = 0; PORTG.PCR.BIT.B7 = 1;} // input with pull-up
#define SDA__OUT    {PORTG.PDR.BIT.B7 = 1;}
#define SDA__0      {PORTG.PODR.BIT.B7 = 0;}
#define SDA__1      {PORTG.PODR.BIT.B7 = 1;}
#define SDA_        PORTG.PIDR.BIT.B7


#define ADRPCA8574  0x40        /* PCA8574A port expander */

void  I2CRemStart(void);
void  I2CRemStop(void);
void  I2CRemBitWrite(uchar a);
uchar I2CRemBitRead(void);
uchar I2CRemByteWrite(uchar a);
uchar I2CRemByteRead(uchar a);
uchar I2CRemAckTest(uchar s);
uchar PCA8574Read(uchar s);
uchar PCA8574Write(uchar s, uchar v);
void  Init_I2C(void);
void  I2C_Chrono(void);

#endif




