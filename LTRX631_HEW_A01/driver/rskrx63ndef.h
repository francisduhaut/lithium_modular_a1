/*******************************************************************************
 * File Name     : rskrx63ndef.h
 * Version       : 1.00
 * Device        : R5F563NB
 * Tool-Chain    : Renesas RX Standard 1.2.0.0
 * H/W Platform  : RSK+RX63N
 * Description   : Defines macros relating to the RX63N user LEDs and switches
 *******************************************************************************/
/*******************************************************************************
 * History       : 13 Aug. 2012 Ver. 1.00 First Release
 *******************************************************************************/

/*******************************************************************************
 * Macro Definitions
 *******************************************************************************/
/* Multiple inclusion prevention macro */
#ifndef RSKRX63NDEF_H
#define RSKRX63NDEF_H

/*******************************************************************************
 * User Includes (Project Level Includes)
 *******************************************************************************/
/* Defines RX63N port registers */
#include "iodefine.h"

/* General Values */
#define LED_ON              (0)
#define LED_OFF             (1)
#define SET_BIT_HIGH        (1)
#define SET_BIT_LOW         (0)
#define SET_BYTE_HIGH       (0xFF)
#define SET_BYTE_LOW        (0x00)
#define OUTPUT_PIN          (1)
#define INPUT_PIN           (0)

/* LED ouptut pin settings */
#define LED_CHARGE       		PORT0.PODR.BIT.B7
#define LED_AVAIL        		PORT2.PODR.BIT.B2
#define LED_FAULT        		PORT4.PODR.BIT.B6
//#define	BUZZER					    PORT4.PODR.BIT.B0


//#define BuzzerOff           {if (Key.KeySound == 0) BUZZER = 0;}
//#define BuzzerOn            {if (Key.KeySound == 0) BUZZER = 1;}

/* LCD BACKLIGHT */
//#define	LCD_BACKLIGHT    		PORTA.PODR.BIT.B6

/* I/O */
#define PClkSelect          (PORT8.PIDR.BIT.B5)         
#define PumpInput           (PORT9.PIDR.BIT.B6)

#define PresBat1Input       (PORT5.PIDR.BIT.B6)
#define PresBat2Input       (PORT1.PIDR.BIT.B2)

#define ElectroVOn          {PORTG.PODR.BIT.B3 = 1;}
#define ElectroVOff         {PORTG.PODR.BIT.B3 = 0;}

#define PumpOn              {PORTG.PODR.BIT.B4 = 1; StatePumpOn;} 
#define PumpOff             {PORTG.PODR.BIT.B4 = 0; StatePumpOff;}

// MODIF 2.8 : add ExtOn in Idle mode to save power
#define ExtOff              {PORT6.PDR.BIT.B0 = 1; PORT6.PODR.BIT.B0 = 1;}
#define ExtOn               {PORT6.PDR.BIT.B0 = 1; PORT6.PODR.BIT.B0 = 0;}


//#define IdleOn              {PORT5.PDR.BIT.B7 = 1; PORT5.PODR.BIT.B7 = 0; StateIdleOn; Reset_IdleTimerCAN();} 
//#define IdleOff             {PORT5.PDR.BIT.B7 = 1; PORT5.PODR.BIT.B7 = 1; StateIdleOff;}
// MODIF 2.8 : add ExtOn in Idle mode to save power
#define IdleOn              {PORT5.PDR.BIT.B7 = 1; PORT5.PODR.BIT.B7 = 0; StateIdleOn; Reset_IdleTimerCAN(); ExtOff;} 
#define IdleOff             {PORT5.PDR.BIT.B7 = 1; PORT5.PODR.BIT.B7 = 1; StateIdleOff; ExtOn;}


#define OUT_FAULT           PORTG.PODR.BIT.B2
#define OUT_AVAIL           PORT6.PODR.BIT.B5
#define OUT_CHARGE          PORT6.PODR.BIT.B6

#define OUT_DE          	PORTG.PODR.BIT.B1

#define PM1                 PORT1.PIDR.BIT.B0
#define PM2                 PORT9.PIDR.BIT.B5
#define PM3                 PORT9.PIDR.BIT.B3
#define PM4                 PORT9.PIDR.BIT.B1
#define PM5                 PORT1.PIDR.BIT.B1
#define PM6                 PORT9.PIDR.BIT.B4
#define PM7                 PORT9.PIDR.BIT.B2
#define PM8                 PORT9.PIDR.BIT.B0

#define OUT_EXT             PORT6.PODR.BIT.B0       // MODIF R2.2             

/* End of multiple inclusion prevention macro */
#endif
