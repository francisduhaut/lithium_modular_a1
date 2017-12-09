/*******************************************************************************
 * DISCLAIMER
 *
 *
 *
 *
 *******************************************************************************/
/* Copyright (C) 2013 Enersys. All rights reserved.   */
/*******************************************************************************
 * File Name     : graphics.c
 * Version       : 1.00
 * Device        : R5F563NB (RX631)
 * Tool-Chain    : Renesas RX Standard 1.2.0.0
 * H/W Platform  : LIFETECH
 * Description   : TFT Module utility functions.
 *                  Written for S1D13517 TFT controller.
 *******************************************************************************/
/*******************************************************************************
 * History       : Sep. 2013  Ver. 1.00 First Release
 *******************************************************************************/

/*******************************************************************************
 * User Includes (Project Level Includes)
 *******************************************************************************/

/* Defines RX63N port registers */
#include "iodefine.h"
#include "include.h"

//#define TIANMA
//#define ROCKTEC

/*******************************************************************************
 * Internal variables
 *******************************************************************************/

// Clipping region control
int16_t       _clipRgn;

// Clipping region borders
int16_t       _clipLeft;
int16_t       _clipTop;
int16_t       _clipRight;
int16_t       _clipBottom;

uint32_t    pixelWindow[8] =
{
		TRANSPARENTCOLOR,
		TRANSPARENTCOLOR,
		TRANSPARENTCOLOR,
		TRANSPARENTCOLOR,
		TRANSPARENTCOLOR,
		TRANSPARENTCOLOR,
		TRANSPARENTCOLOR,
		TRANSPARENTCOLOR
};

uint32_t    _color;

uint32_t    _bgcolor;

/*Primitives variables*/

// Current line type
int16_t   _lineType;

// Current line thickness
uint8_t    _lineThickness;

// Font orientation
uint8_t    _fontOrientation;

// Current cursor x-coordinates
int16_t   _cursorX;

// Current cursor y-coordinates
int16_t   _cursorY;

// information on currently set font
GFX_FONT_CURRENT currentFont;

// bevel drawing type (0 = full bevel, 0xF0 - top bevel only, 0x0F - bottom bevel only
uint8_t _bevelDrawType;

volatile uint32_t  _drawbuffer;

#ifdef USE_ANTIALIASED_FONTS

BYTE    _antialiastype;

#endif

/*******************************************************************************
 * Local Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void WritePixel(uint32_t color)
{
	uint16_t * dram = (uint16_t *)0x06000000ul;

	*dram = (uint16_t)(color >> 16);
	Delay_us(0);
	*dram = (uint16_t)(color);
	//Delay_us(1);
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void SetReg (uint16_t index, uint8_t value)
{
	uint16_t * dram = (uint16_t *)0x06000000ul;

	HCS = 0;

	HDC = COMMAND;

	*dram = (uint16_t)(index);

	HDC = DATA;

	*dram = (uint16_t)(value);
	Delay_us(1u);

	HCS = 1;
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint8_t GetReg(uint8_t index)
{
	uint16_t * dram = (uint16_t *)0x06000000ul;
	uint8_t data;

	HCS = 0;
	HDC = COMMAND;

	*dram = (uint16_t)(index);

	HDC = DATA;

	data = *dram;
	Delay_us(1u);
	HCS = 1;
	return data;
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void DeviceWrite(uint16_t data)
{
	uint16_t * dram = (uint16_t *)0x06000000ul;
	*dram = (uint16_t)(data);
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void DriverInterfaceInit(void)
{

	BSC.CS2MOD.WORD = 0x0301;
	BSC.CS2WCR1.BIT.CSRWAIT = 7;
	BSC.CS2WCR1.BIT.CSWWAIT = 1;
	BSC.CS2WCR1.BIT.CSPRWAIT = 7;
	BSC.CS2WCR1.BIT.CSPWWAIT = 1;
	BSC.CS2WCR2.BIT.CSON = 0;
	BSC.CS2WCR2.BIT.WDON = 0;
	BSC.CS2WCR2.BIT.WRON = 1;
	BSC.CS2WCR2.BIT.AWAIT = 7;
	BSC.CS2WCR2.BIT.WDOFF = 4;
	BSC.CS2WCR2.BIT.CSWOFF = 4;
	BSC.CS2WCR2.BIT.CSROFF = 7;
	BSC.CS2CR.WORD = 0x0001;
	BSC.CS2REC.WORD = 0;
	BSC.CSRECEN.WORD = 0x0000;
	BSC.BEREN.BYTE = 0x03;
	BSC.BUSPRI.WORD = 0x0000;

	//MPC.PFCSE.BYTE = 0x08;
	MPC.PFCSE.BYTE = 0x00;
	MPC.PFCSS0.BYTE = 0x00;
	MPC.PFAOE0.BYTE = 0xFF;
	MPC.PFAOE1.BYTE = 0x3F;
	MPC.PFBCR0.BYTE = 0x11;
	MPC.PFBCR1.BYTE = 0x00;

	//set input pins:
	//none all output

	SYSTEM.PRCR.WORD = 0xA502;

	SYSTEM.SYSCR0.WORD = 0x5A03;

	while (SYSTEM.SYSCR0.BIT.EXBE != 1);

	SYSTEM.SCKCR.BIT.PSTOP1 = 0;


}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void DisplayBrightness(uint8_t level)
{
	uint16_t High,Low;

	if(level >= 100)
	{
		SetReg(REG70_PWM_CONTROL,0x85);      	  //Turn on Backlight
	}
	else if (level == 0)
	{
		SetReg(REG70_PWM_CONTROL,0x84);       	//Turn off Backlight
	}
	else
	{
		High = ((uint32_t)3000 * (uint32_t)level) /(uint32_t)100;
		Low = 3000 - High;

		SetReg(REG72_PWM_HIGH_DC_0,(uint8_t)High);
		SetReg(REG74_PWM_HIGH_DC_1,(uint8_t)(High>>8));
		SetReg(REG7A_PWM_LOW_DC_0,(uint8_t)Low);
		SetReg(REG7C_PWM_LOW_DC_1,(uint8_t)(Low>>8));
		SetReg(REG70_PWM_CONTROL,0x86);   		//Turn on Backlight PWM
	}
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void ResetDevice(void)
{
	HDC_DIR = 1; 		// Output
	GPU_RES_DIR = 1;
	HCS_DIR = 1;
	HCS = 1;

	/*Inicialize GPU interface*/
	DriverInterfaceInit();

	/*Hard Reset GPU*/
	GPU_RES = 0;
	Delay_us(500u);
	GPU_RES = 1;
	Delay_us(100000u);

	/*Inicialize GPU registers*/
	SetReg(REG2A_DSP_MODE,0x00);

	SetReg(REG68_POWER_SAVE,0x00);
	SetReg(REG04_PLL_DDIVIDER,0x17);  	//PLLCLK = 1MHz (=24MHz/24=CLKI/PLL_D)
	SetReg(REG06_PLL_0,0x29);		 	      //VCO = 142-180MHz
	SetReg(REG08_PLL_1,0x01);  			    //PLLCLK is divided by 2
	SetReg(REG0A_PLL_2,0x08);  			    //REG0A must be progremmed with value 08h

	SetReg(REG0C_PLL_NDIVIDER,0x59);	  //Target Frequency = 90MHz; NN = 180; CLKI = 24MHz; PLL_D = 24:1
	SetReg(REG12_CLK_SRC_SELECT,0x02);

	SetReg(REG04_PLL_DDIVIDER,0x97);	  //Enable PLL
	SetReg(REG0E_SS_CONTROL_0,0x8F);  	//Spread Spectrum modulation enabled with default spread of +/-0.25ns
	Delay_us(100000u);

	SetReg(REG14_LCD_PANEL_TYPE,0x02);  //8:8:8 color format PP Panel

	SetReg(REG16_HDISP_WIDTH,(DISP_HOR_RESOLUTION>>3)-1);
	SetReg(REG18_HNDP_PERIOD,((DISP_HOR_PULSE_WIDTH+DISP_HOR_FRONT_PORCH+DISP_HOR_BACK_PORCH)>>1)-1);
	SetReg(REG1A_VDISP_HEIGHT_0,(uint8_t)DISP_VER_RESOLUTION-1);
	SetReg(REG1C_VDISP_HEIGHT_1,(DISP_VER_RESOLUTION-1)>>8);
	SetReg(REG1E_VNDP_PERIOD,((DISP_VER_PULSE_WIDTH+DISP_VER_FRONT_PORCH+DISP_VER_BACK_PORCH)>>1)-1);
	SetReg(REG20_PHS_PULSE_WIDTH,DISP_HOR_PULSE_WIDTH -1);
	SetReg(REG22_PHS_PULSE_START,DISP_HOR_FRONT_PORCH);
	SetReg(REG24_PVS_PULSE_WIDTH,DISP_VER_PULSE_WIDTH -1);
	SetReg(REG26_PVS_PULSE_START,DISP_VER_FRONT_PORCH);


#ifdef ROCKTEC
	SetReg(REG28_PCLK_POLARITY,0x00);       //ROCKTEC POLARITY
#endif

#ifdef TIANMA
	SetReg(REG28_PCLK_POLARITY,0x80);       //TIANMA POLARITY
#endif

	if (PClkSelect != 0)
		SetReg(REG28_PCLK_POLARITY,0x80);       //TIANMA POLARITY
	else
		SetReg(REG28_PCLK_POLARITY,0x00);       //ROCKTEC POLARITY

	SetReg(REG82_SDRAM_CONTROL_0,0x03);		    //SDRAM memory size = 128MBits
	SetReg(REG8C_SDRAM_RFS_CNT_0,0xFF);		    //SDRAM Refresh Rate =
	SetReg(REG8E_SDRAM_RFS_CNT_1,0x03);
	SetReg(REG90_SDRAM_BUF_SIZE,0x50);		    //SDRAM Write Buffer Memory Size = 1.75MHz (max)
	SetReg(REG68_POWER_SAVE,0xE8);			      //Reset modules
	SetReg(REG68_POWER_SAVE,0x00);
	SetReg(REG68_POWER_SAVE,0x01);			      //Enable SDCLK
	SetReg(REG84_SDRAM_STATUS_0,0x86);		    // Enable SDRAM, SDRAM auto refresh and powersave

	SetReg(REG52_INPUT_MODE,0x08);        	  //Transparency On = 0x08
	SetReg(REG54_TRANSP_KEY_RED, RED8(TRANSPARENTCOLOR));
	SetReg(REG56_TRANSP_KEY_GREEN, GREEN8(TRANSPARENTCOLOR));
	SetReg(REG58_TRANSP_KEY_BLUE, BLUE8(TRANSPARENTCOLOR));

	SetReg(REG60_WRITE_WIN_X_EP,(GetMaxX() >> 2));     // Write Window X End Position Register
	SetReg(REG62_WRITE_WIN_Y_EP_0,(GetMaxY() >> 2));   // Write Window Y End Position Register 0
	SetReg(REG64_WRITE_WIN_Y_EP_1,(uint8_t)GetMaxY());    // Write Window Y End Position Register 1

	DisplayBrightness(0);                   //Sets Backlight Brightness Level - PWM
	SetReg(REG2A_DSP_MODE,0x01);   			    //Enable display interface
	SetReg(REG50_DISPLAY_CONTROL,0x80);   	//Update registers 0x2A-0x4E
	Delay_us(500000u);             				  //Delay of 500ms for SDRAM to stabilize

	//DISPEN = 1;
	SetReg(REG6E_GPO_1,0x02);   			      //GPO All High
	SetReg(REGB2_INTERRUPT_CTRL,0x01);			//Enable Alpha Blend Interrupt

	_drawbuffer = GFX_BUFFER1;
	SetActivePage(_drawbuffer);
	SetVisualPage(_drawbuffer);
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void SetBacklight(uint8_t State)
{
	uint8_t temp;

	temp = GetReg(REG6E_GPO_1);

	if (State == 0)
	{
		SetReg(REG6E_GPO_1,temp & 0x0E);   			//GPO low
		//SetReg(REG6E_GPO_1,0x00);   			//GPO low
	}
	else
	{
		SetReg(REG6E_GPO_1,temp | 0x01);   			//GPO High
	}
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint8_t Percentage2Alpha(uint8_t alphaPercentage)
{
	uint32_t percent = (uint32_t)(alphaPercentage);
	percent *= ALPHA_DELTA;
	percent >>= 5;

	return (uint8_t)percent;
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint16_t AlphaBlendWindow(uint32_t foregroundArea, int16_t foregroundLeft, int16_t foregroundTop,
		uint32_t backgroundArea, int16_t backgroundLeft, int16_t backgroundTop,
		uint32_t destinationArea, int16_t destinationLeft, int16_t destinationTop,
		uint16_t  width, uint16_t height,
		uint8_t  alphaPercentage)
{

	UINT32_VAL foregroundWindowAddr,backgroundWindowAddr,destinationWindowAddr;

	foregroundWindowAddr.Val = GFXGetPageXYAddress(foregroundArea, foregroundLeft, foregroundTop);
	backgroundWindowAddr.Val = GFXGetPageXYAddress(backgroundArea, backgroundLeft, backgroundTop);
	destinationWindowAddr.Val = GFXGetPageXYAddress(destinationArea, destinationLeft, destinationTop);

	SetReg(REG98_ALP_HR_SIZE,((width-1)>>3));
	SetReg(REG9A_ALP_VR_SIZE_0,height);
	SetReg(REG9C_ALP_VR_SIZE_1,(height>>8));

	foregroundWindowAddr.Val  -=(foregroundWindowAddr.Val %8);
	backgroundWindowAddr.Val  -=(backgroundWindowAddr.Val %8);
	destinationWindowAddr.Val -=(destinationWindowAddr.Val %8);

	SetReg(REGA0_ALP_IN_IMG1_SA_0,foregroundWindowAddr.v[0]);
	SetReg(REGA2_ALP_IN_IMG1_SA_1,foregroundWindowAddr.v[1]);
	SetReg(REGA4_ALP_IN_IMG1_SA_2,foregroundWindowAddr.v[2]);
	SetReg(REGA6_ALP_IN_IMG2_SA_0,backgroundWindowAddr.v[0]);
	SetReg(REGA8_ALP_IN_IMG2_SA_1,backgroundWindowAddr.v[1]);
	SetReg(REGAA_ALP_IN_IMG2_SA_2,backgroundWindowAddr.v[2]);
	SetReg(REGAC_ALP_OUT_IMG_SA_0,destinationWindowAddr.v[0]);
	SetReg(REGAE_ALP_OUT_IMG_SA_1,destinationWindowAddr.v[1]);
	SetReg(REGB0_ALP_OUT_IMG_SA_2,destinationWindowAddr.v[2]);

	SetReg(REG9E_ALP_VALUE,0x80 | Percentage2Alpha(alphaPercentage));

	SetReg(REG94_ALP_CONTROL,0x01); 		    //Enable Alpha Blend
	SetReg(REG94_ALP_CONTROL,0x00);             // Added From DataSheet

	while(!GetReg((REGB4_INTERRUPT_STAT)));
	SetReg(REGB6_INTERRUPT_CLEAR,0x01);         //Send Clear Interrupt Command
	SetReg(REGB6_INTERRUPT_CLEAR,0x00);			//Bring value back to 0

	return 1;
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void CopyPageWindow( uint8_t srcPage, uint8_t dstPage,
		uint16_t srcX, uint16_t srcY, uint16_t dstX, uint16_t dstY,
		uint16_t width, uint16_t height)
{
	while(!AlphaBlendWindow(srcPage,srcX,srcY, srcPage,srcX,srcY, dstPage,dstX,dstY, width, height,
			100));
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint8_t CreatePIPLayer(uint16_t x,uint16_t y,uint16_t width,uint16_t height)
{
	SetReg(REG32_PIP1_WIN_X_SP, x/4);
	SetReg(REG38_PIP1_WIN_X_EP, (x+width)/4);
	SetReg(REG34_PIP1_WIN_Y_SP_0, y >> 2);
	SetReg(REG36_PIP1_WIN_Y_SP_1, y);
	SetReg(REG3A_PIP1_WIN_Y_EP_0, (y+height) >> 2);
	SetReg(REG3C_PIP1_WIN_Y_EP_1, (y+height) );
	SetReg(REG50_DISPLAY_CONTROL,0x80);
	return 0;
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint8_t PIPState(uint8_t State)
{
	uint8_t Temp;

	Temp = GetReg(REG2A_DSP_MODE);
	Temp &= 0xF0;

	if (State > 0)
	{
		SetReg(REG2A_DSP_MODE,(Temp | 0x05));                           //Turn on both PIP1
	}
	else
	{
		SetReg(REG2A_DSP_MODE,(Temp | 0x01));                           //Turn off both PIP1
	}
	SetReg(REG50_DISPLAY_CONTROL,0x80);
	return 0;
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint8_t PIPDSA(uint16_t x,uint16_t y,uint8_t Page)
{
	UINT32_VAL PipStartAddress;

	PipStartAddress.Val = GFXGetPageXYAddress(Page,x,y);

	SetReg(REG2C_PIP1_DSP_SA_0,PipStartAddress.v[0]);
	SetReg(REG2E_PIP1_DSP_SA_1,PipStartAddress.v[1]);
	SetReg(REG30_PIP1_DSP_SA_2,PipStartAddress.v[2]);
	SetReg(REG50_DISPLAY_CONTROL,0x80);
	return 0;
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void UpdateDisplayNow(void)
{

	uint32_t SourceBuffer, DestBuffer;

	if(_drawbuffer == GFX_BUFFER1)
	{
		SourceBuffer = GFX_BUFFER1;
		DestBuffer   = GFX_BUFFER2;
	}
	else
	{
		SourceBuffer = GFX_BUFFER2;
		DestBuffer   = GFX_BUFFER1;
	}


	_drawbuffer = DestBuffer;
	SetActivePage(_drawbuffer);
	SetVisualPage(SourceBuffer);
	/* Copy actual content to drawbuffer */
	CopyPageWindow(SourceBuffer,_drawbuffer,0,0,0,0,GetMaxX(),GetMaxY());
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
GFX_COLOR GetPixel(int16_t x, int16_t y)
{
	return (_bgcolor);
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void PutPixel(int16_t x, int16_t y)
{
	if(_clipRgn)
	{
		if(x < _clipLeft)
			return;
		if(x > _clipRight)
			return;
		if(y < _clipTop)
			return;
		if(y > _clipBottom)
			return;
	}

	static  uint8_t xc=0;

#if (DISP_ORIENTATION == 90)

	uint16_t templ = x;

	x = y;
	y = (DISP_VER_RESOLUTION-1) - templ;

#endif

#if (DISP_ORIENTATION == 270)

	uint16_t templ = x;

	x = (DISP_HOR_RESOLUTION-1)- y;
	y = templ;

#endif

#if (DISP_ORIENTATION == 180)

	x = (DISP_HOR_RESOLUTION-1)- x;
	y = (DISP_VER_RESOLUTION-1)- y;

#endif

	xc = x & 7;

	DisplayEnable();

	DisplaySetCommand();
	DeviceWrite(REG5A_WRITE_WIN_X_SP);

	DisplaySetData();

	DeviceWrite((uint16_t)(x >> 2));    //Setup the start window size/position
	DeviceWrite((uint16_t)(y >> 2));
	DeviceWrite((uint16_t)y);

	DisplaySetCommand();
	DeviceWrite(REG66_MEM_DATA_PORT_0);
	DisplaySetData();

	pixelWindow[xc] = _color;

	WritePixel(pixelWindow[0]);    //Output the pixelWindow
	WritePixel(pixelWindow[1]);    //Output the pixelWindow
	WritePixel(pixelWindow[2]);    //Output the pixelWindow
	WritePixel(pixelWindow[3]);    //Output the pixelWindow
	WritePixel(pixelWindow[4]);    //Output the pixelWindow
	WritePixel(pixelWindow[5]);    //Output the pixelWindow
	WritePixel(pixelWindow[6]);    //Output the pixelWindow
	WritePixel(pixelWindow[7]);    //Output the pixelWindow

	pixelWindow[xc] = TRANSPARENTCOLOR;

	DisplayDisable();
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint16_t Bar(int16_t left, int16_t top, int16_t right, int16_t bottom)
{

	uint32_t xdelta,ydelta;
	uint32_t loffset,roffset;

	if(left > right)
		return(1);        //Added to make sure function call is accurate

	if(bottom < top)
		return(1);        //Added to make sure function call is accurate

	if(_clipRgn)
	{
		left   = (left   < _clipLeft)   ? _clipLeft   : ((left   > _clipRight)  ? _clipRight  : left);
		right  = (right  > _clipRight)  ? _clipRight  : ((right  < _clipLeft)   ? _clipLeft   : right);
		top    = (top    < _clipTop)    ? _clipTop    : ((top    > _clipBottom) ? _clipBottom : top);
		bottom = (bottom > _clipBottom) ? _clipBottom : ((bottom < _clipTop)    ? _clipTop    : bottom);
	}

	if (right>479) right = 479;
	if (bottom>271) bottom = 271;


#if (DISP_ORIENTATION == 90)       //Added for rotation capabilities
	uint32_t templ,tempr;
	templ = left; tempr = right;

	left = top;
	right = bottom;

	top = (DISP_VER_RESOLUTION-1) - tempr;
	bottom = (DISP_VER_RESOLUTION-1) - templ;
#endif

#if (DISP_ORIENTATION == 270)         //Added for rotation capabilities
	uint32_t templ,tempr;
	templ = left; tempr = right;

	left = (DISP_HOR_RESOLUTION-1) - bottom;
	right = (DISP_HOR_RESOLUTION-1) - top;

	top = templ;
	bottom = tempr;
#endif

#if (DISP_ORIENTATION == 180)         //Added for rotation capabilities
	uint32_t templ,tempr;
	templ = left; tempr = top;

	left = (DISP_HOR_RESOLUTION-1) - right;
	right = (DISP_HOR_RESOLUTION-1) - templ;

	top = (DISP_VER_RESOLUTION-1) - bottom;
	bottom = (DISP_VER_RESOLUTION-1) - tempr;
#endif

	if(left>0)
		loffset = left & 7;
	else
		loffset = 0;

	roffset = right & 7;
	roffset = 8 - roffset;

	DisplayEnable();
	DisplaySetCommand();
	DeviceWrite(REG5A_WRITE_WIN_X_SP);

	DisplaySetData();

	DeviceWrite((uint16_t)(left >> 2));
	DeviceWrite((uint16_t)(top >> 2));
	DeviceWrite((uint16_t)top);
	DeviceWrite((uint16_t)(((right+roffset)-8) >> 2));
	DeviceWrite((uint16_t)(bottom >> 2));
	DeviceWrite((uint16_t)bottom);

	uint16_t j,i;

	xdelta = (right - left) + loffset;
	roffset += xdelta;
	ydelta = bottom - top;

	for(i=0;i <= ydelta; i++)
	{
		j = 0;
		while(j < loffset)
		{
			WritePixel(TRANSPARENTCOLOR);
			//WritePixel(RGBConvert(254,1,120));
			j++;
		}

		while(j <= xdelta)
		{
			WritePixel(_color);
			j++;
		}

		while(j < roffset)
		{
			WritePixel(TRANSPARENTCOLOR);
			//WritePixel(RGBConvert(254,1,120));
			j++;
		}
	}

	//Change Write Window Settings back to max
	SetReg(REG60_WRITE_WIN_X_EP,(GetMaxX() >> 2));     // Write Window X End Position Register
	SetReg(REG62_WRITE_WIN_Y_EP_0,(GetMaxY() >> 2));   // Write Window Y End Position Register 0
	SetReg(REG64_WRITE_WIN_Y_EP_1,(uint8_t)GetMaxY());    // Write Window Y End Position Register 1

	DisplayDisable();     // disable S1D13517

	return (1);
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void ClearDevice(void)
{
	DisplayEnable();
	DisplaySetCommand();
	DeviceWrite(REG5A_WRITE_WIN_X_SP);

	DisplaySetData();
	DeviceWrite(0x00);                  // X Start Position
	DeviceWrite(0x00);                  // Y Start Position
	DeviceWrite(0x00);                  // Y Start Position
	DeviceWrite((DISP_HOR_RESOLUTION-1) >>2);     // X End Position
	DeviceWrite((DISP_VER_RESOLUTION-1) >>2);     // Y End Position
	DeviceWrite((uint8_t)(DISP_VER_RESOLUTION-1));         // Y End Position

	uint32_t i;

	for(i = 0; i < ((uint32_t)DISP_VER_RESOLUTION * (uint32_t)DISP_HOR_RESOLUTION); i++)
		WritePixel(_color);

	DisplayDisable();

}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
/*void PutImage24BPP(int16_t left, int16_t top, uint8_t *bitmap, uint8_t stretch)
{
    register uint32_t  memOffset;
    BITMAP_HEADER   bmp;
    uint32_t            lineBuffer[(GetMaxX() + 1)];
    uint32_t            *pData;
    uint16_t            uint8_tWidth;


	uint16_t                	sizeX, sizeY;
	register uint16_t       	x, y;
	uint32_t                	temp;
	register uint8_t       	stretchX, stretchY;
	uint16_t 					loffset,roffset;

    // Get bitmap header
    memcpy(&bmp,bitmap,sizeof(BITMAP_HEADER));

    // Set offset to the image data
    memOffset = bitmap + sizeof(BITMAP_HEADER);

//    if(pPartialImageData->width != 0)
//    {
//         memOffset += (pPartialImageData->xoffset + pPartialImageData->yoffset*(bmp.width))<<1;
//         sizeY = pPartialImageData->height;
//         sizeX = pPartialImageData->width;
//    }
//    else
    {
        // Get size
        sizeX = bmp.width;
        sizeY = bmp.height;
    }

    uint8_tWidth = sizeX << 2;

//	loffset = left & 0x07;
//	roffset = (left + (sizeX*stretch)) & 0x07;
//	roffset = 8 - roffset;
	loffset = 0;
    roffset = 0;


	DisplayEnable();     // enable S1D13517
	DisplaySetCommand(); // set RS line to low for command
	DeviceWrite(REG5A_WRITE_WIN_X_SP);
	DisplaySetData();    // set RS line to high for data
	DeviceWrite((left>>3)<<1);
	DeviceWrite(top>>2);
	DeviceWrite(top);
	DeviceWrite(((left+(sizeX*stretch)+roffset-8)>>3)<<1);
	DeviceWrite((top+(sizeY*stretch))>>2);
	DeviceWrite((top+(sizeY*stretch)));
	DisplaySetCommand(); // set RS line to low for command
	DeviceWrite(0x66);
	DisplaySetData();    // set RS line to high for data



    for(y = 0; y < sizeY; y++)
    {
        // Get line
		memcpy(&lineBuffer[0],memOffset,uint8_tWidth);
        //ReadWordArray(memOffset, &lineBuffer[0],uint8_tWidth);
        memOffset += uint8_tWidth;

        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
	    pData = lineBuffer;

            for(x = 0; x < (sizeX + loffset + roffset); x++)
            {
//                if((x<=loffset) || (x > (sizeX+loffset)))
//                {
//                    WritePixel(TRANSPARENTCOLOR);
//                }
//                else
                {

                    // Read pixels from flash
                    temp = *pData++;

                    // Set color
                    SetColor(temp);

                    // Write pixel to screen
                    for(stretchX = 0; stretchX < stretch; stretchX++)
                    {
                        WritePixel(_color);
                    }
                }
            }
        }
    }

    DisplayDisable();
}*/
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void PutImage24BPPExt(int16_t left, int16_t top, const IMAGE_EXTERNAL *image)
{
	static uint32_t bitmap;
	static uint32_t  memOffset;
	static BITMAP_HEADER   bmp;
	static uint32_t            lineBuffer[(GetMaxX() + 1)];
	uint32_t            *pData;
	uint16_t            Width;


	uint16_t                	sizeX, sizeY;
	register uint16_t       	x, y;
	uint32_t                	temp;
	uint16_t 					loffset,roffset;

	bitmap = image->address >> 1;

	// Get bitmap header
	ReadWordArray(bitmap,(uint16_t*)&bmp, sizeof(BITMAP_HEADER)/2);

	// Set offset to the image data
	memOffset = bitmap + 3;

	// Get size
	sizeX = bmp.width;
	sizeY = bmp.height;

	Width = ((sizeX)<<1);
	//Width++;
	//Width++;
	//Width = (sizeX);
	/*
	if(left>0)
        loffset = left & 7;
	else
        loffset = 0;
	 */



	loffset = left & 0x07;
	roffset = (left + (sizeX)) & 0x07;
	roffset = 8 - roffset;

	if (sizeX>= GetMaxX()) roffset = 0;


	//	loffset = left & 0x07;
	//    roffset = (left + sizeX) & 7;
	//	roffset = 8 - roffset;

	//	if (sizeX>= GetMaxX()) roffset = 0;
	//	if (sizeY>= GetMaxY()) loffset = 0;


	DisplayEnable();     // enable S1D13517
	DisplaySetCommand(); // set RS line to low for command
	DeviceWrite(REG5A_WRITE_WIN_X_SP);
	DisplaySetData();    // set RS line to high for data
	DeviceWrite((left>>3)<<1);
	DeviceWrite(top>>2);
	DeviceWrite(top);
	DeviceWrite(((left+(sizeX)+roffset-8)>>3)<<1);
	DeviceWrite((top+(sizeY))>>2);
	DeviceWrite((top+(sizeY)));
	DisplaySetCommand(); // set RS line to low for command
	DeviceWrite(0x66);
	DisplaySetData();    // set RS line to high for data
	DisplayDisable();


	for(y = 0; y < sizeY; y++)
	{
		// Get line
		ReadDWordArray(memOffset, &lineBuffer[0],Width);
		memOffset += Width;

		DisplayEnable();
		pData = &lineBuffer[0];

		for(x = 0; x < (sizeX + loffset + roffset); x++)
		{


			//if((x<loffset) || (x > (sizeX+loffset)))
			if((x<loffset) || (x >= (sizeX+loffset)))
			{
				//WritePixel(RGBConvert(254,01,128));
				WritePixel(TRANSPARENTCOLOR);
			}
			else
			{
				// Read pixels from flash
				temp = *pData++;

				// Set color
				SetColor(temp);
				WritePixel(_color);
			}
		}
		DisplayDisable();
	}
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void PutImage24BPPExtPartial(int16_t left, int16_t top,uint8_t Percent ,uint8_t Max,const IMAGE_EXTERNAL *image)
{
	static uint32_t bitmap;
	static uint32_t  memOffset;
	static BITMAP_HEADER   bmp;
	static uint32_t lineBuffer[(GetMaxX() + 1)];

	uint32_t *pData;
	uint16_t Width,lines = 0;

	uint16_t  sizeX, sizeY;
	register uint16_t   x, y;
	uint32_t  temp;
	uint16_t  loffset,roffset;

	if (Percent == 0) return;
	if (Percent > Max) Percent = Max;

	bitmap = image->address >> 1;

	// Get bitmap header
	ReadWordArray(bitmap,(uint16_t*)&bmp, sizeof(BITMAP_HEADER)/2);

	// Set offset to the image data
	memOffset = bitmap + 3;

	// Get size
	sizeX = bmp.width;
	sizeY = bmp.height;

	Width = ((sizeX)<<1);

	memOffset += (sizeY-1) * Width;

	loffset = left & 0x07;

	roffset = (left + sizeX) & 7;
	roffset = 8 - roffset;

	if (sizeX>= GetMaxX()) roffset = 0;
	if (sizeY>= GetMaxY()) loffset = 0;

	for(y = 0; y < sizeY; y++)
	{
		DisplayEnable();     // enable S1D13517
		DisplaySetCommand(); // set RS line to low for command
		DeviceWrite(REG5A_WRITE_WIN_X_SP);
		DisplaySetData();    // set RS line to high for data
		DeviceWrite((left>>3)<<1);
		DeviceWrite((top + (sizeY-y))>>2);
		DeviceWrite(top + (sizeY-y));
		DeviceWrite(((left+(sizeX)+roffset-8)>>3)<<1);
		DeviceWrite((top+(sizeY))>>2);
		DeviceWrite((top+(sizeY)));
		DisplaySetCommand(); // set RS line to low for command
		DeviceWrite(0x66);
		DisplaySetData();    // set RS line to high for data
		DisplayDisable();

		// Get line
		ReadDWordArray(memOffset, &lineBuffer[0],Width);
		memOffset -= Width;

		DisplayEnable();
		pData = &lineBuffer[0];

		for(x = 0; x < (sizeX + loffset + roffset); x++)
		{
			if((x<loffset) || (x >= (sizeX+loffset)))
			{
				WritePixel(TRANSPARENTCOLOR);
			}
			else
			{
				// Read pixels from flash
				temp = *pData++;

				// Set color
				SetColor(temp);
				WritePixel(_color);
			}
		}
		DisplayDisable();
		lines++;
		if (lines == Percent) return;
	}
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint32_t GetDrawBufferAddress(void)
{
	return _drawbuffer;
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void SetActivePage(uint32_t page)
{
	//_GFXActivePage = page;
	SetReg(REG52_INPUT_MODE, 0x08 | (page<<4));
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void SetVisualPage(uint32_t page)
{
	uint8_t Temp;
	Temp = GetReg(REG2A_DSP_MODE);
	Temp &= 0x0F;
	SetReg(REG2A_DSP_MODE, Temp | (0x01 | (page<<4)));
	//SetReg(REG2A_DSP_MODE, 0x0F);
	SetReg(REG50_DISPLAY_CONTROL,0x80);
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void SetTestPattern(void)
{
	SetReg(REG2A_DSP_MODE, 0x0F);
	SetReg(REG50_DISPLAY_CONTROL,0x80);
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint32_t GFXGetPageOriginAddress(int16_t pageNumber)
{
	return(BUFFER_SIZE * (uint32_t)pageNumber);
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint32_t GFXGetPageXYAddress(int16_t pageNumber, uint16_t x, uint16_t y)
{
	uint32_t address;

#if (DISP_ORIENTATION == 90)       //Added for rotation capabilities
	Duint16_t tempx;
	tempx = x;

	x=y;
	y = (uint32_t)(DISP_VER_RESOLUTION-1) - tempx;
	if(tempx ==0) y=0;
#endif

	address = (uint32_t)y * (uint32_t)DISP_HOR_RESOLUTION;
	address += (uint32_t)x;
	address *= 3;
	address += GFXGetPageOriginAddress(pageNumber);

	return address;
}


/*====================Primitives======================*/




/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void SetClipRgn(int16_t left, int16_t top, int16_t right, int16_t bottom)
{
	_clipLeft=left;
	_clipTop=top;
	_clipRight=right;
	_clipBottom=bottom;

}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void SetClip(uint8_t control)
{
	_clipRgn=control;
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint16_t Line(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	int16_t   deltaX, deltaY;
	int16_t   error, stepErrorLT, stepErrorGE;
	int16_t   stepX, stepY;
	int16_t   steep;
	int16_t   temp;
	int16_t   style, type;


	// Move cursor
	MoveTo(x2, y2);

	if(x1 == x2)
	{
		if(y1 > y2)
		{
			temp = y1;
			y1 = y2;
			y2 = temp;
		}

		style = 0;
		type = 1;
		for(temp = y1; temp < y2 + 1; temp++)
		{
			if((++style) == _lineType)
			{
				type ^= 1;
				style = 0;
			}

			if(type)
			{
				PutPixel(x1, temp);
				if(_lineThickness)
				{
					PutPixel(x1 + 1, temp);
					PutPixel(x1 - 1, temp);
				}
			}
		}

		return (1);
	}

	if(y1 == y2)
	{
		if(x1 > x2)
		{
			temp = x1;
			x1 = x2;
			x2 = temp;
		}

		style = 0;
		type = 1;
		for(temp = x1; temp < x2 + 1; temp++)
		{
			if((++style) == _lineType)
			{
				type ^= 1;
				style = 0;
			}

			if(type)
			{
				PutPixel(temp, y1);
				if(_lineThickness)
				{
					PutPixel(temp, y1 + 1);
					PutPixel(temp, y1 - 1);
				}
			}
		}

		return (1);
	}

	stepX = 0;
	deltaX = x2 - x1;
	if(deltaX < 0)
	{
		deltaX = -deltaX;
		--stepX;
	}
	else
	{
		++stepX;
	}

	stepY = 0;
	deltaY = y2 - y1;
	if(deltaY < 0)
	{
		deltaY = -deltaY;
		--stepY;
	}
	else
	{
		++stepY;
	}

	steep = 0;
	if(deltaX < deltaY)
	{
		++steep;
		temp = deltaX;
		deltaX = deltaY;
		deltaY = temp;
		temp = x1;
		x1 = y1;
		y1 = temp;
		temp = stepX;
		stepX = stepY;
		stepY = temp;
		PutPixel(y1, x1);
	}
	else
	{
		PutPixel(x1, y1);
	}

	// If the current error greater or equal zero
	stepErrorGE = deltaX << 1;

	// If the current error less than zero
	stepErrorLT = deltaY << 1;

	// Error for the first pixel
	error = stepErrorLT - deltaX;

	style = 0;
	type = 1;

	while(--deltaX >= 0)
	{
		if(error >= 0)
		{
			y1 += stepY;
			error -= stepErrorGE;
		}

		x1 += stepX;
		error += stepErrorLT;

		if((++style) == _lineType)
		{
			type ^= 1;
			style = 0;
		}

		if(type)
		{
			if(steep)
			{
				PutPixel(y1, x1);
				if(_lineThickness)
				{
					PutPixel(y1 + 1, x1);
					PutPixel(y1 - 1, x1);
				}
			}
			else
			{
				PutPixel(x1, y1);
				if(_lineThickness)
				{
					PutPixel(x1, y1 + 1);
					PutPixel(x1, y1 - 1);
				}
			}
		}
	}   // end of while

		return (1);
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void InitGraph(void)
{

	// Current line type
	SetLineType(SOLID_LINE);

	// Current line thickness
	SetLineThickness(THICK_LINE);

	// Current cursor coordinates to 0,0
	MoveTo(0, 0);

	// Reset device
	ResetDevice();

	// Set color to GRAY
	SetColor(RGBConvert(13,13,13));

	// Clear screen
	ClearDevice();

	// Disable clipping
	SetClip(CLIP_DISABLE);

	// Set font orientation
	SetFontOrientation(ORIENT_HOR);

	// set Bevel drawing
	SetBevelDrawType(DRAWFULLBEVEL);

	// Clear update bits
	UpdateStateInit();

}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void SetFontFlash(void *pFont)
{
	const uint8_t *src;
	uint8_t *dest;
	int i;

	src = (const uint8_t *)((FONT_FLASH *)pFont)->address;
	dest = (uint8_t *)&(currentFont.fontHeader);
	for(i = 0; i < sizeof(FONT_HEADER); i++)
	{
		*dest = *src;
		src++;
		dest++;
	}
	currentFont.pFont = pFont;
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void SetFont(void *pFont)
{
	SetFontFlash(pFont);
}


/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
#ifdef USE_ANTIALIASED_FONTS

static GFX_COLOR   _fgcolor100;
static GFX_COLOR   _fgcolor25;
static GFX_COLOR   _fgcolor75;
static GFX_COLOR   _bgcolor100;
static GFX_COLOR   _bgcolor25;
static GFX_COLOR   _bgcolor75;

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
static void calculateColors(void)
{
	GFX_COLOR   _fgcolor50;
	GFX_COLOR   _bgcolor50;

	_fgcolor50  = ConvertColor50(_fgcolor100);
	_fgcolor25  = ConvertColor25(_fgcolor100);
	_fgcolor75  = _fgcolor50   +  _fgcolor25;

	_bgcolor50  = ConvertColor50(_bgcolor100);
	_bgcolor25  = ConvertColor25(_bgcolor100);
	_bgcolor75  = _bgcolor50   +  _bgcolor25;

	_fgcolor25 += _bgcolor75;
	_fgcolor75 += _bgcolor25;
}

#endif //#ifdef USE_ANTIALIASED_FONTS


/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
void OutCharGetInfoFlash (char ch, OUTCHAR_PARAM *pParam)
{
	const GLYPH_ENTRY_EXTENDED    *pChTableExtended;
	const GLYPH_ENTRY             *pChTable;

	// set color depth of font,
	// based on 2^bpp where bpp is the color depth setting in the FONT_HEADER
	pParam->bpp = 1 << currentFont.fontHeader.bpp;

	if(currentFont.fontHeader.extendedGlyphEntry)
	{
		pChTableExtended = (const GLYPH_ENTRY_EXTENDED *) (((FONT_FLASH *)currentFont.pFont)->address + sizeof(FONT_HEADER)) + ((uint8_t)ch - (uint8_t)currentFont.fontHeader.firstChar);
		pParam->pChImage = (const uint8_t *) (((FONT_FLASH *)currentFont.pFont)->address + pChTableExtended->offset);
		pParam->chGlyphWidth = pChTableExtended->glyphWidth;
		pParam->xWidthAdjust = pChTableExtended->glyphWidth - pChTableExtended->cursorAdvance;
		pParam->xAdjust = pChTableExtended->xAdjust;
		pParam->yAdjust = pChTableExtended->yAdjust;

		if(pParam->yAdjust > 0)
		{
			pParam->heightOvershoot = pParam->yAdjust;
		}
	}
	else
	{
		pChTable = (const GLYPH_ENTRY *) (((FONT_FLASH *)currentFont.pFont)->address + sizeof(FONT_HEADER)) + ((uint8_t)ch - (uint8_t)currentFont.fontHeader.firstChar);
		pParam->pChImage = (const uint8_t *) (((FONT_FLASH *)currentFont.pFont)->address + ((uint32_t)(pChTable->offsetMSB) << 8) + pChTable->offsetLSB);
		pParam->chGlyphWidth = pChTable->width;
	}
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint16_t OutCharRender(char ch, OUTCHAR_PARAM *pParam)
{
	uint8_t        temp = 0;
	uint8_t        mask;
	uint8_t        restoremask;
	int16_t       xCnt, yCnt, x = 0, y;

#ifdef USE_ANTIALIASED_FONTS
	uint8_t        val, shift;
	GFX_COLOR   bgcolor;
#endif

#ifdef USE_ANTIALIASED_FONTS
	if(pParam->bpp == 1)
	{
		restoremask = 0x01;
	}
	else
	{
		if(pParam->bpp == 2)
		{
			restoremask = 0x03;
		}
		else
		{
			return 255; // BPP > 2 are not yet supported
		}

		bgcolor = GetPixel(GetX(), GetY() + (currentFont.fontHeader.height >> 1));

		if((_fgcolor100 != GetColor()) || (_bgcolor100 != bgcolor))
		{
			_fgcolor100 = GetColor();
			_bgcolor100 = bgcolor;
			calculateColors();
		}
	}
#else
	restoremask = 0x01;
#endif

	if(_fontOrientation == ORIENT_HOR)
	{
		y = GetY() + pParam->yAdjust;
		for(yCnt = 0; yCnt < currentFont.fontHeader.height + pParam->heightOvershoot; yCnt++)
		{
			x = GetX() + pParam->xAdjust;
			mask = 0;

#ifdef USE_ANTIALIASED_FONTS
			shift = 0;
#endif
			for(xCnt = 0; xCnt < pParam->chGlyphWidth; xCnt++)
			{
				if(mask == 0)
				{
					temp = *(pParam->pChImage)++;
					mask = restoremask;

#ifdef USE_ANTIALIASED_FONTS
shift = 0;
#endif
				}

#ifdef USE_ANTIALIASED_FONTS
				if(pParam->bpp == 1)
				{
					if(temp & mask)
					{
						PutPixel(x, y);
					}
				}
				else
				{
					val = (temp & mask) >> shift;
					if(val)
					{
						if(GFX_Font_GetAntiAliasType() == ANTIALIAS_TRANSLUCENT)
						{
							bgcolor = GetPixel(x, y);
							if(_bgcolor100 != bgcolor)
							{
								_bgcolor100 = bgcolor;
								calculateColors();
							}
						}

						switch(val)
						{
						case 1:     SetColor(_fgcolor25);
						break;

						case 2:     SetColor(_fgcolor75);
						break;

						case 3:     SetColor(_fgcolor100);
						}

						PutPixel(x, y);
					}
				}

				mask  <<=  pParam->bpp;
				shift  +=  pParam->bpp;
#else
				if(temp & mask)
				{
					PutPixel(x, y);
				}

				mask <<= 1;
#endif
				x++;
			}
			y++;
		}

		// move cursor
		_cursorX = x - pParam->xAdjust - pParam->xWidthAdjust;
	}
	else    // If extended glyph is used, then vertical alignment may not be rendered properly and hence users must position the texts properly
	{
		y = GetX() + pParam->xAdjust;
		for(yCnt = 0; yCnt < currentFont.fontHeader.height + pParam->heightOvershoot; yCnt++)
		{
			x = GetY() + pParam->yAdjust;
			mask = 0;

#ifdef USE_ANTIALIASED_FONTS
			shift = 0;
#endif

			for(xCnt = 0; xCnt < pParam->chGlyphWidth; xCnt++)
			{
				if(mask == 0)
				{
					temp = *(pParam->pChImage)++;
					mask = restoremask;

#ifdef USE_ANTIALIASED_FONTS
shift = 0;
#endif
				}

#ifdef USE_ANTIALIASED_FONTS
				if(pParam->bpp == 1)
				{
					if(temp & mask)
					{
						PutPixel(y, x);
					}
				}
				else
				{
					val = (temp & mask) >> shift;
					if(val)
					{
						if(GFX_Font_GetAntiAliasType() == ANTIALIAS_TRANSLUCENT)
						{
							bgcolor = GetPixel(x, y);
							if(_bgcolor100 != bgcolor)
							{
								_bgcolor100 = bgcolor;
								calculateColors();
							}
						}
						switch(val)
						{
						case 1: SetColor(_fgcolor25);
						break;

						case 2: SetColor(_fgcolor75);
						break;

						case 3: SetColor(_fgcolor100);
						}
						PutPixel(y, x);
					}
				}

				mask  <<=  pParam->bpp;
				shift  +=  pParam->bpp;
#else
				if(temp & mask)
				{
					PutPixel(y, x);
				}
				mask  <<=  1;
#endif
				x--;
			}
			y++;
		}

		// move cursor
		_cursorY = x - pParam->xAdjust;
	}

	// restore color
#ifdef USE_ANTIALIASED_FONTS
	if(pParam->bpp > 1)
	{
		SetColor(_fgcolor100);
	}
#endif
	return (1);

}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint16_t OutChar(char ch)
{
	static OUTCHAR_PARAM OutCharParam;

	OutCharParam.xAdjust = 0;
	OutCharParam.yAdjust = 0;
	OutCharParam.xWidthAdjust = 0;
	OutCharParam.heightOvershoot = 0;

	// check for error conditions (a return value of 0xFFFF means error)
	if((uint8_t)ch < (uint8_t)currentFont.fontHeader.firstChar)
		return (255);
	if((uint8_t)ch > (uint8_t)currentFont.fontHeader.lastChar)
		return (255);

#ifndef USE_ANTIALIASED_FONTS
	if(currentFont.fontHeader.bpp > 1)
		return (-1);
#endif

	//    while(IsDeviceBusy() != 0) Nop();

	OutCharGetInfoFlash(ch, &OutCharParam);

	return (OutCharRender(ch, &OutCharParam));
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint16_t  OutText(char *textString)
{
	char   ch;
	while((int8_t)15 < (int8_t)(ch = *textString++))
		while(OutChar(ch) == 0);
	return (1);
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint16_t  OutTextXY(int16_t x, int16_t y, char *textString)
{
	MoveTo(x, y);
	OutText(textString);
	return (1);
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
int16_t GetTextWidthFlash(char* textString, void* pFont)
{

	const GLYPH_ENTRY *pChTable = 0;
	const GLYPH_ENTRY_EXTENDED *pChTableExtended = 0;
	const FONT_HEADER *pHeader;

	int16_t       textWidth;
	uint8_t       ch;
	uint8_t       fontFirstChar;
	uint8_t       fontLastChar;

	pHeader = (const FONT_HEADER *) ((FONT_FLASH *)pFont)->address;
	fontFirstChar = pHeader->firstChar;
	fontLastChar = pHeader->lastChar;
	if(pHeader->extendedGlyphEntry)
	{
		pChTableExtended = (const GLYPH_ENTRY_EXTENDED *) (pHeader + 1);
	}
	else
	{
		pChTable = (const GLYPH_ENTRY *) (pHeader + 1);
	}
	textWidth = 0;
	while((int8_t)15 < (int8_t)(ch = *textString++))
	{
		if((int8_t)ch < (int8_t)fontFirstChar)
			continue;
		if((int8_t)ch > (int8_t)fontLastChar)
			continue;
		if(pHeader->extendedGlyphEntry)
		{
			textWidth += (pChTableExtended + ((int8_t)ch - (int8_t)fontFirstChar))->cursorAdvance;
		}
		else
		{
			textWidth += (pChTable + ((int8_t)ch - (int8_t)fontFirstChar))->width;
		}
	}

	return (textWidth);
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
int16_t GetTextWidth(char *textString, void *pFont)
{
	return GetTextWidthFlash(textString, pFont);
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
int16_t GetTextHeight(void *pFont)
{

	// if the current set font is the same just return with
	// the already set value in currentFont
	if (pFont == currentFont.pFont)
		return currentFont.fontHeader.height;
	else
	{
		return ((const FONT_HEADER *) ((FONT_FLASH *)pFont)->address)->height;
	}

}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint16_t BarGradient(int16_t left, int16_t top, int16_t right, int16_t bottom, uint32_t color1, uint32_t color2, uint32_t length, uint8_t direction)
{
	uint16_t startRed, startBlue, startGreen;
	uint16_t endRed, endBlue, endGreen;

	int32_t rdiff=0,gdiff=0,bdiff=0;
	int16_t i,steps;

	SetColor(color2);

	// if length is 100, why waste the bar call?
	switch(direction)
	{
	case GRAD_UP:
		length = length * (bottom - top);
		length /= 100;
		steps = length;
		while(!Bar(left,top,right,bottom-steps));
		break;

	case GRAD_DOWN:
		length = length * (bottom - top);
		length /= 100;
		steps = length;
		while(!Bar(left,top+steps,right,bottom));
		break;

	case GRAD_RIGHT:
		length = length * (right - left);
		length /= 100;
		steps = length;
		while(!Bar(left+steps,top,right,bottom));
		break;

	case GRAD_LEFT:
		length = length * (right - left);
		length /= 100;
		steps = length;
		while(!Bar(left,top,right-steps,bottom));
		break;

	case GRAD_DOUBLE_VER:
		steps = (right - left) >> 1;
		break;

	case GRAD_DOUBLE_HOR:
		steps = (bottom - top) >> 1;
		break;

	default:
		return 1;
	}

	startRed    = GetRed(color1);
	startGreen  = GetGreen(color1);
	startBlue   = GetBlue(color1);

	endRed      = GetRed(color2);
	endGreen    = GetGreen(color2);
	endBlue     = GetBlue(color2);

	///////////////////////////////////

	//Find the step size for the red portion//
	rdiff = ((long)endRed - (long)startRed) << 8;
	rdiff /= steps;

	//Find the step size for the green portion//
	gdiff = ((long)endGreen - (long)startGreen) << 8;
	gdiff /= steps;

	//Find the step size for the blue portion//
	bdiff = ((long)endBlue - (long)startBlue) << 8;
	bdiff /= steps;

	short barSize = 1;
	color1 = RGBConvert(startRed, startGreen, startBlue);

	// PERCENTAGE BASED CODE
	for(i=0; i < steps; i++)
	{
		//Calculate the starting RGB values
		endRed      = startRed + ((rdiff*i) >> 8);
		endGreen    = startGreen + ((gdiff*i) >> 8);
		endBlue     = startBlue + ((bdiff*i) >> 8);

		color2 = RGBConvert(endRed, endGreen, endBlue);

		if(color2 == color1)
		{
			barSize++;
			continue;
		}

		SetColor(color2);
		color1 = color2;

		switch(direction)          //This switch statement draws the gradient depending on direction chosen
		{
		case GRAD_DOWN:
			while(!Bar(left, top, right, top + barSize));
			top += barSize;
			break;

		case GRAD_UP:
			while(!Bar(left,bottom - barSize,right,bottom));
			bottom -= barSize;
			break;

		case GRAD_RIGHT:
			while(!Bar(left, top, left + barSize, bottom));
			left += barSize;
			break;

		case GRAD_LEFT:
			while(!Bar(right - barSize, top, right, bottom));
			right -= barSize;
			break;

		case GRAD_DOUBLE_VER:
			while(!Bar(right - barSize, top, right, bottom));
			right -= barSize;
			while(!Bar(left, top, left + barSize, bottom));
			left += barSize;
			break;

		case GRAD_DOUBLE_HOR:
			while(!Bar(left, bottom - barSize, right, bottom));
			bottom -= barSize;
			while(!Bar(left, top, right, top + barSize));
			top += barSize;
			break;

		default:
			break;
		}

		barSize = 1;
	}

	if(barSize > 1)
	{

		SetColor(RGBConvert(endRed, endGreen, endBlue));

		switch(direction)          //This switch statement draws the gradient depending on direction chosen
		{
		case GRAD_DOWN:
			while(!Bar(left, top, right, top + barSize));
			break;

		case GRAD_UP:
			while(!Bar(left,bottom - barSize,right,bottom));
			break;

		case GRAD_RIGHT:
			while(!Bar(left, top, left + barSize, bottom));
			break;

		case GRAD_LEFT:
			while(!Bar(right - barSize, top, right, bottom));
			break;

		case GRAD_DOUBLE_VER:
			while(!Bar(right - barSize, top, right, bottom));
			while(!Bar(left, top, left + barSize, bottom));
			break;

		case GRAD_DOUBLE_HOR:
			while(!Bar(left, bottom - barSize, right, bottom));
			while(!Bar(left, top, right, top + barSize));
			break;

		default:
			break;
		}

	}
	return 1;
}

/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint8_t Bevel(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t rad)
{
	int16_t       style, type, xLimit, xPos, yPos, error;
	UINT32_VAL   temp;

	//while(IsDeviceBusy() != 0)

	temp.Val = SIN45 * rad;
	xLimit = temp.w[1] + 1;
	temp.Val = (uint32_t) (ONEP25 - ((long long) rad << 16));
	error = (int16_t) (temp.w[1]);
	yPos = rad;

	style = 0;
	type = 1;

	if(rad)
	{
		for(xPos = 0; xPos <= xLimit; xPos++)
		{
			if((++style) == _lineType)
			{
				type ^= 1;
				style = 0;
			}

			if(type)
			{
				PutPixel(x2 + xPos, y1 - yPos);         // 1st quadrant
				PutPixel(x2 + yPos, y1 - xPos);
				PutPixel(x2 + xPos, y2 + yPos);         // 2nd quadrant
				PutPixel(x2 + yPos, y2 + xPos);
				PutPixel(x1 - xPos, y2 + yPos);         // 3rd quadrant
				PutPixel(x1 - yPos, y2 + xPos);
				PutPixel(x1 - yPos, y1 - xPos);         // 4th quadrant
				PutPixel(x1 - xPos, y1 - yPos);

				if(_lineThickness)
				{
					PutPixel(x2 + xPos, y1 - yPos - 1); // 1st quadrant
					PutPixel(x2 + xPos, y1 - yPos + 1);
					PutPixel(x2 + yPos + 1, y1 - xPos);
					PutPixel(x2 + yPos - 1, y1 - xPos);
					PutPixel(x2 + xPos, y2 + yPos - 1); // 2nd quadrant
					PutPixel(x2 + xPos, y2 + yPos + 1);
					PutPixel(x2 + yPos + 1, y2 + xPos);
					PutPixel(x2 + yPos - 1, y2 + xPos);
					PutPixel(x1 - xPos, y2 + yPos - 1); // 3rd quadrant
					PutPixel(x1 - xPos, y2 + yPos + 1);
					PutPixel(x1 - yPos + 1, y2 + xPos);
					PutPixel(x1 - yPos - 1, y2 + xPos);
					PutPixel(x1 - yPos + 1, y1 - xPos); // 4th quadrant
					PutPixel(x1 - yPos - 1, y1 - xPos);
					PutPixel(x1 - xPos, y1 - yPos + 1);
					PutPixel(x1 - xPos, y1 - yPos - 1);
				}
			}

			if(error > 0)
			{
				yPos--;
				error += 5 + ((xPos - yPos) << 1);
			}
			else
				error += 3 + (xPos << 1);
		}
	}
	// Must use lines here since this can also be used to draw focus of round buttons
	if(x2 - x1)
	{
		while(!Line(x1, y1 - rad, x2, y1 - rad));

		// draw top
	}

	if(y2 - y1)
	{
		while(!Line(x1 - rad, y1, x1 - rad, y2));

		// draw left
	}

	if((x2 - x1) || (y2 - y1))
	{
		while(!Line(x2 + rad, y1, x2 + rad, y2));

		// draw right
		while(!Line(x1, y2 + rad, x2, y2 + rad));

		// draw bottom
	}

	return (1);
}


/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
uint8_t FillBevel(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t rad)
{

	int16_t       yLimit, xPos, yPos, err;
	int16_t       xCur, yCur, yNew;
	int16_t       last54Y, last18Y;
	UINT32_VAL   temp;


	// this covers filled rectangle.
	if (_bevelDrawType == DRAWFULLBEVEL)
		Bar(x1 - rad, y1, x2 + rad, y2);
	else if (_bevelDrawType == DRAWTOPBEVEL)
		Bar(x1 - rad, y1, x2 + rad, y1+((y2-y1)>>1));
	else
		Bar(x1 - rad, y1+((y2-y1)>>1), x2 + rad, y2);

	// note that octants here is defined as:
	// from yPos=-radius, xPos=0 in the clockwise direction octant 1 to 8 are labeled
	// assumes an origin at 0,0. Quadrants are defined in the same manner
	if(rad)
	{
		temp.Val = SIN45 * rad;
		yLimit = temp.w[1];
		temp.Val = (uint32_t) (ONEP25 - ((long long) rad << 16));
		err = (int16_t) (temp.w[1]);
		xPos = rad;
		yPos = 0;

		// yCur starts at 1 so the center line is not drawn and will be drawn separately
		// this is to avoid rendering twice on the same line
		yCur = 1;
		xCur = xPos;
		yNew = yPos;

		// note initial values are important
		last54Y = GetMaxY();
		last18Y = 0;

		while(yPos <= yLimit)
		{
			// Drawing of the rounded panel is done only when there is a change in the
			// x direction. Bars are drawn to be efficient.
			// detect changes in the x position. Every change will mean a bar will be drawn
			// to cover the previous area. yNew records the last position of y before the
			// change in x position.
			yNew = yPos;

			if(err > 0)
			{
				xPos--;
				err += 5 + ((yPos - xPos) << 1);
			}
			else
				err += 3 + (yPos << 1);
			yPos++;

			if(xCur != xPos)
			{
				if (_bevelDrawType & DRAWBOTTOMBEVEL)
				{
					// 6th octant to 3rd octant
					// check first if there will be an overlap
					if (y2 + yNew > last54Y)
						Bar(x1 - xCur, y2 + yCur, x2 + xCur, last54Y - 1);
					else
						Bar(x1 - xCur, y2 + yCur, x2 + xCur, y2 + yNew);

					// 5th octant to 4th octant
					// check: if overlapping then no need to proceed
					if ((y2 + xCur) > (y2 + yNew))
					{
						Bar(x1 - yNew, y2 + xCur, x2 + yNew, y2 + xCur);
						last54Y = (y2 + xCur);

					}

				}

				if (_bevelDrawType & DRAWTOPBEVEL)
				{
					// 7th octant to 2nd octant
					// check: if overlapping then no need to proceed
					if (y1 - yNew < last18Y)
						Bar(x1 - xCur, last18Y + 1, x2 + xCur, y1 - yCur);
					else
						Bar(x1 - xCur, y1 - yNew, x2 + xCur, y1 - yCur);

					// 8th octant to 1st octant
					// check first if there will be an overlap
					if ((y1 - xCur) < (y1 - yNew))
					{
						Bar(x1 - yNew, y1 - xCur, x2 + yNew, y1 - xCur);
						last18Y = y1 - xCur;
					}

				}
				// update current values
				xCur = xPos;
				yCur = yPos;
			}
		}

		if ((y1 - yNew) > (y1 - xPos))
		{
			// there is a missing line, draw that line
			if (_bevelDrawType & DRAWTOPBEVEL)
				Bar(x1 - yNew, y1 - xPos, x2 + yNew, y1 - xPos);
		}
		if ((y2 + yNew) < (y2 + xPos))
		{
			// there is a missing line, draw that line
			if (_bevelDrawType & DRAWBOTTOMBEVEL)
				Bar(x1 - yNew, y2 + xPos, x2 + yNew, y2 + xPos);
		}
	}

	return (1);
}
/*******************************************************************************
 * Outline       :
 * Description   :
 * Argument      : none
 * Return value  : none
 *******************************************************************************/
