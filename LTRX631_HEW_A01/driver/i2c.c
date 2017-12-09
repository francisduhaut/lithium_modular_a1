/*
 * i2c.c
 *
 *  Created on: 01/04/2013
 *      Author: Francis DUHAUT
 */

#include "include.h"

uchar Pca8574;              

/**
 * @brief  Start I2C
 * @param  None
 * @retval None
 */
void I2CRemStart(void)
{
	SDA__IN;
	Delay_us(6);
	SCL__1;
	SDA__0;
	SDA__OUT;
	Delay_us(6);
	SCL__0;
}

/**
 * @brief  Stop I2C
 * @param  None
 * @retval None
 */
void I2CRemStop(void)
{
	Delay_us(6);
	SCL__1;
	Delay_us(6);
	SDA__IN;
}

/**
 * @brief  Write BIT to I2C
 * @param  bit value to write
 * @retval None
 */
void I2CRemBitWrite(uchar a)
{
	if (a != 0)
	{
		SDA__IN;
	}
	else
	{
		SDA__0;
		SDA__OUT;
	}
	Delay_us(4);
	SCL__1;
	Delay_us(4);
	SCL__0;
	Delay_us(2);
	/*if (a != 0)
  {  
    SDA__0;
    SDA__OUT;
  }*/
}

/**
 * @brief  Read BIT to I2C
 * @param  None
 * @retval bit value
 */
uchar I2CRemBitRead(void)
{
	uchar i;
	SDA__IN;
	Delay_us(4);
	SCL__1;
	Delay_us(4);
	if (SDA_!= 0)
		i = 1;
	else
		i = 0;
	SCL__0;
	return i;
}

/**
 * @brief  Write BYTE to I2C
 * @param  byte to write
 * @retval 0 if ACK by slave, 1 if no ACK
 */
uchar I2CRemByteWrite(uchar a)
{
	uchar i;
	i = 8;
	do
	{
		I2CRemBitWrite(a & 0x80);
		a <<= 1;
	}
	while (--i);
	/* Test ACK */
	return I2CRemBitRead();
}

/**
 * @brief  Read BYTE to I2C
 * @param  a = 0 : ACK ; a = 1 NOACK
 * @retval BYTE
 */
uchar I2CRemByteRead(uchar a)
{
	uchar i;
	uchar j;
	j = 0;
	i = 7;
	do
	{
		j += I2CRemBitRead() * (1 << i);
	}
	while (i--);
	/* Read ACK */
	I2CRemBitWrite(a & 1);
	return j;
}

/**
 * @brief  Test acknoledge of device
 * @param  full address of device (7 bits)
 * @retval 0 = acknoledge, 1 = non acknoledge
 */
uchar I2CRemAckTest(uchar s)
{
	uchar i;
	I2CRemStart();
	i = I2CRemByteWrite(s);
	I2CRemStop();
	return  i;
}


/**
 * @brief  Read value from PCA8574 standard and A
 * @param  I2C adress (mask with 0x7E for 0111 A2 A1 A0 R/W)
 * @retval BYTE
 */
uchar PCA8574Read(uchar s)
{   
	uchar i;
	I2CRemStart();                                    /* Start */
	i = I2CRemByteWrite(s + 1);                       /* Dev sel avec R/W = 1 */
	i = I2CRemByteRead(1);                            /* Read no ACK */
	I2CRemStop();
	return i;
}

/**
 * @brief  Write value to PCA8574 standard and A
            Must write all ports, input ports are set to 1
 * @param  s : I2C adress (mask with 0x7E for 0111 A2 A1 A0 R/W)
            v : value 
 * @retval 0 = ACK by slave, 0 = no ACK
 */
uchar PCA8574Write(uchar s, uchar v)
{   
	uchar i;
	I2CRemStart();                                    /* Start */
	i = I2CRemByteWrite(s);                           /* Dev sel avec R/W = 0 */
	i = I2CRemByteWrite(v);                           /* Value */
	I2CRemStop();
	return i;
}


/**
 * @brief  Initialization of I2C bus
            Detect PCA8574
 * @param  None
 * @retval None
 */
void Init_I2C(void)
{

	/* Initialisation du port I2C */
	SDA__IN;
	SCL__OUT;
	SCL__1;

	Pca8574 = 0xFF;

	if (I2CRemAckTest(ADRPCA8574) == 0)
	{
		PCA8574DefOff;
		MaskBPOff;
	}
	else
	{
		PCA8574DefOn;
		MaskBPOn;
	}
}

/**
 * @brief  Read PCA8574 in chrono task
 * @param  None
 * @retval None
 */
void  I2C_Chrono(void)
{
	if ((State.TimerMs % 100) == 0)
	{
		if (PCA8574Def == 0)
		{
			if (State.TimerSec < 1)
				Pca8574 = PCA8574Read(ADRPCA8574);
		}
		else
		{
			Init_I2C();
		}
	}

}
