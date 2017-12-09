/*******************************************************************************
* DISCLAIMER
*
* 
*
*******************************************************************************/
/* Copyright (C) 2013 Enersys. All rights reserved.   */
/*******************************************************************************
* File Name     : graphics.h
* Version       : 1.00
* Device        : R5F563NB (RX631)
* Tool-Chain    : Renesas RX Standard 1.2.0.0
* H/W Platform  : LIFETECH
* Description   : This Header file contains the Macro Definitions & prototypes  
*                  for the functions used in graphics.c
*******************************************************************************/
/*******************************************************************************
* History       : Sep. 2013  Ver. 1.00 First Release
*******************************************************************************/

/*******************************************************************************
* Macro Definitions
*******************************************************************************/
/* Multiple inclusion prevention macro */
#ifndef GRAPHICS_H
#define GRAPHICS_H

/*******************************************************************************
* User Includes (Project Level Includes)
*******************************************************************************/
/* Defines standard variable types used in this file */
#include <stdint.h>
    

/*******************************************************************************
* External Variables
*******************************************************************************/	
extern uint32_t    _color;
extern uint32_t    _bgcolor;
extern uint8_t _bevelDrawType; 
extern int16_t _lineType;
extern uint8_t _lineThickness;

/*******************************************************************************
* Macro Definitions
*******************************************************************************/
#define INPUT 0
#define OUTPUT 1
#define DATA 1
#define COMMAND 0

#define HDC_DIR PORT6.PDR.BIT.B1
#define GPU_RES_DIR PORT7.PDR.BIT.B1
#define HCS_DIR PORT6.PDR.BIT.B2

#define HDC PORT6.PODR.BIT.B1
#define GPU_RES PORT7.PODR.BIT.B1
#define HCS PORT6.PODR.BIT.B2

//#define DISPEN_DIR PORTG.PODR.BIT.B6
//#define DISPEN PORTG.PDR.BIT.B6

#define DISP_ORIENTATION		0
#define DISP_HOR_RESOLUTION		480
#define DISP_VER_RESOLUTION		272
#define DISP_DATA_WIDTH			24
#define DISP_HOR_PULSE_WIDTH    41
#define DISP_HOR_BACK_PORCH     2
#define DISP_HOR_FRONT_PORCH    2
#define DISP_VER_PULSE_WIDTH    10
#define DISP_VER_BACK_PORCH     2
#define DISP_VER_FRONT_PORCH    2
#define GFX_LCD_TYPE            GFX_LCD_TFT



#define SIN45   	46341   // sin(45) * 2^16)
#define ONEP25  	81920   // 1.25 * 2^16
	

//#define Times_New_Roman_18 Font_Tiny

#define USE_ANTIALIASED_FONTS

#define TRANSPARENTCOLOR                RGBConvert(0xFF, 0x00, 0xCC)          //Transparent Color chosen for the application


#define DisplayEnable() HCS = 0;
#define DisplayDisable()   HCS = 1;  

#define DisplaySetCommand() HDC = COMMAND
#define DisplaySetData() HDC = DATA

#define RGBConvert(red, green, blue)    (uint32_t) (((uint32_t)(red) << 16) | ((uint32_t)(green) << 8) | (uint32_t)(blue))

#define RED8(color24)   ((color24 & 0x00FF0000) >> 16)
#define GREEN8(color24) ((color24 & 0x0000FF00) >> 8)
#define BLUE8(color24)  ((color24 & 0x000000FF))

#define SetColor(color) _color = (color)

#define SetBackColor(bgcolor) _bgcolor = (bgcolor)

typedef struct
{
    uint8_t        compression;                // Compression setting 
    uint8_t        colorDepth;                 // Color depth used
    int16_t       height;                     // Image height
    int16_t       width;                      // Image width
	uint16_t	unused;
} BITMAP_HEADER;


#define BUFFER_SIZE 0x140000  //WVGA


typedef struct
{
    uint8_t        fontID;                     // User assigned value
    uint8_t        extendedGlyphEntry : 1;     // Extended Glyph entry flag. When set font has extended glyph feature enabled.
    uint8_t        res1               : 1;     // Reserved for future use  (must be set to 0)
    uint8_t        bpp                : 2;     // Actual BPP = 2<sup>bpp</sup>  
                                            //   - 0 - 1 BPP
                                            //   - 1 - 2 BPP
                                            //   - 2 - 4 BPP
                                            //   - 3 - 8 BPP
    uint8_t        orientation        : 2;     // Orientation of the character glyphs (0,90,180,270 degrees)
                                            //   - 00 - Normal
                                            //   - 01 - Characters rotated 270 degrees clockwise
                                            //   - 10 - Characters rotated 180 degrees
                                            //   - 11 - Characters rotated 90 degrees clockwise
                                            // Note: Rendering DO NOT rotate the characters. The table contains rotated characters
                                            //       and will be rendered as is. 
    uint8_t        res2               : 2;     // Reserved for future use (must be set to 0).
    int16_t        firstChar;                  // Character code of first character (e.g. 32).
    int16_t        lastChar;                   // Character code of last character in font (e.g. 3006).
    int16_t        height;                     // Font characters height in pixels. 
} FONT_HEADER;


typedef struct
{
   void         *pFont;                     // pointer to the currently set font
   FONT_HEADER  fontHeader;                 // copy of the currently set font header
#ifdef USE_ANTIALIASED_FONTS
   BYTE  antiAliasType;                     // anti-alias type set
#endif   
} GFX_FONT_CURRENT;

    #define MoveTo(x, y) \
    _cursorX = x;        \
    _cursorY = y;

// Solid Line Style
    #define SOLID_LINE  0

// Dotted Line Style
    #define DOTTED_LINE 1

// Dashed Line Style
    #define DASHED_LINE 4

// Normal Line (thickness is 1 pixel)
    #define NORMAL_LINE 0

// Thick Line (thickness is 3 pixels)
    #define THICK_LINE  1
	
	#define CLIP_DISABLE                0   // Disables clipping.
#define CLIP_ENABLE                 1   // Enables clipping.

    #define ORIENT_HOR  0
    #define ORIENT_VER  1
		#define DRAWFULLBEVEL  	0xFF 
	#define DRAWTOPBEVEL  	0xF0 
	#define DRAWBOTTOMBEVEL 0x0F 
	
	
   #define ConvertColor50(color)  (GFX_COLOR)((color & (0x00FEFEFEul))>>1)
   #define ConvertColor25(color)  (GFX_COLOR)((color & (0x00FCFCFCul))>>2)
   #define ConvertColor75(color)  (GFX_COLOR)(ConvertColor50(color) + ConvertColor25(color))
   
   #define GetColor() _color	
	
	
	#define SetLineThickness(lnThickness)   _lineThickness = lnThickness;
	#define SetFontOrientation(orient)  _fontOrientation = orient;
	#define SetBevelDrawType(type)		(_bevelDrawType = type)
	#define SetLineType(lnType) _lineType = lnType;

typedef struct
{
    uint16_t type;                      // Resource type. Valid types are: 
                                            // - EXTERNAL 
                                            // - EDS_EPMP
    uint16_t         ID;                        // Memory ID, user defined value to differentiate
                                            // between graphics resources of the same type.	
                                            // When using EDS_EPMP the following ID values are
                                            // reserved and used by the Microchip display driver
                                            //   - 0 - reserved (do not use)
                                            //   - 1 - reserved for base address of EPMP CS1
                                            //   - 2 - reserved for base address of EPMP CS2
    uint32_t        address;                   // Data image address (user data, bitmap or font) 
} GFX_EXTDATA;

typedef GFX_EXTDATA IMAGE_EXTERNAL;
	
	
	#define GetY()  _cursorY
	#define GetX()  _cursorX
	
typedef struct
{
    uint8_t  type;                     // must be FLASH
    const char    *address;                 // font image address in FLASH
} FONT_FLASH;

typedef struct
{
    uint8_t        width;                      // width of the glyph
    uint8_t        offsetLSB;                  // Least Significant Byte of the glyph location offset 
    uint16_t        offsetMSB;                  // Most Significant (2) Bytes of the glyph location offset
} GLYPH_ENTRY;

typedef struct
{
    uint32_t       offset;                     // Offset Order: LSW_LSB LSW_MSB MSW_MSB MSW_MSB
    uint16_t        cursorAdvance;              // x-value by which cursor has to advance after rendering the glyph
    uint16_t        glyphWidth;                 // width of the glyph
    int16_t       xAdjust;                    // x-position is adjusted as per this signed number
    int16_t       yAdjust;                    // y-position is adjusted as per this signed number
} GLYPH_ENTRY_EXTENDED;


    #define GFX_BUFFER1 (0)
    #define GFX_BUFFER2 (1)	


// Mid colors are calculated only once while rendering each character. This is ideal for rendering text over a constant background.
    #define ANTIALIAS_OPAQUE        0

// Mid values are calculated for every necessary pixel. This feature is useful when rendering text over an image 
// or when the background is not one flat color.
    #define ANTIALIAS_TRANSLUCENT   1

    #ifdef USE_ANTIALIASED_FONTS
        #define GFX_Font_SetAntiAliasType(transparency)  _antialiastype = transparency;
    #else
        #define GFX_Font_SetAntiAliasType(transparency)
    #endif

    #ifdef USE_ANTIALIASED_FONTS
        #define GFX_Font_GetAntiAliasType()  _antialiastype
    #else
        #define GFX_Font_GetAntiAliasType()
    #endif


typedef struct 
{ 
#ifdef USE_FONT_FLASH	
    const GLYPH_ENTRY             *pChTable;
    const GLYPH_ENTRY_EXTENDED    *pChTableExtended;
#endif
    uint8_t                    bpp;
	int16_t                   chGlyphWidth;
    const uint8_t     *pChImage;
    int16_t                   xAdjust;
    int16_t                   yAdjust;
    int16_t                   xWidthAdjust;
    int16_t                   heightOvershoot;
} OUTCHAR_PARAM;

#define 	NUM_ALPHA_LEVELS 	0x20			// Specific to device
#define 	ALPHA_DELTA 		(NUM_ALPHA_LEVELS << 5) / 100

uint8_t Percentage2Alpha(uint8_t alphaPercentage);


/*********************************************************************
* Overview: Enumeration for gradient type
*********************************************************************/
typedef enum
{
    GRAD_NONE=0,                            // No Gradients to be drawn
    GRAD_DOWN,                              // gradient changes in the vertical direction
    GRAD_RIGHT,                             // gradient change in the horizontal direction
    GRAD_UP,                                // gradient changes in the vertical direction
    GRAD_LEFT,                              // gradient change in the horizontal direction
    GRAD_DOUBLE_VER,                        // two gradient transitions in the vertical direction
    GRAD_DOUBLE_HOR,                        // two gradient transitions in the horizontal direction
} GFX_GRADIENT_TYPE;

/*********************************************************************
* Overview: This structure is used to describe the gradient style.
*
*********************************************************************/
typedef struct
{
    GFX_GRADIENT_TYPE  gradientType;        // selected the gradient type 
    uint32_t              gradientStartColor;  // sets the starting color of gradient transition
    uint32_t              gradientEndColor;    // sets the ending color of gradient transition
    uint32_t              gradientLength;      // defines the length of the gradient transition in pixels
} GFX_GRADIENT_STYLE;

#define GetRed(color)       (((color) & 0xFF0000) >> 16)
#define GetGreen(color)     (((color) & 0x00FF00) >> 8)
#define GetBlue(color)      ((color) & 0x0000FF)

/*********************************************************************
* Overview: S1D13517 registers definitions.
*********************************************************************/	
#define REG00_PROD_CODE        0x00     // Product Code Register [READONLY]
#define REG02_READBACK         0x02     // Configuration Readback Register [READONLY]
#define REG04_PLL_DDIVIDER     0x04     // PLL D-Divider Register
#define REG06_PLL_0            0x06     // PLL Setting Register 0
#define REG08_PLL_1            0x08     // PLL Setting Register 1
#define REG0A_PLL_2            0x0A     // PLL Setting Register 2
#define REG0C_PLL_NDIVIDER     0x0C     // PLL N-Divider Register
#define REG0E_SS_CONTROL_0     0x0E     // SS Control Register 0
#define REG10_SS_CONTROL_1     0x10     // SS Control Register 1
#define REG12_CLK_SRC_SELECT   0x12     // Clock Source Select Register
#define REG14_LCD_PANEL_TYPE   0x14     // LCD Panel Type Register
#define REG16_HDISP_WIDTH      0x16     // Horizontal Display Width Register
#define REG18_HNDP_PERIOD      0x18     // Horizontal Non-Display Period Register
#define REG1A_VDISP_HEIGHT_0   0x1A     // Vertical Display Height Register 0
#define REG1C_VDISP_HEIGHT_1   0x1C     // Vertical Display Height Register 1
#define REG1E_VNDP_PERIOD      0x1E     // Vertical Non-Display Period Register
#define REG20_PHS_PULSE_WIDTH  0x20     // PHS Pulse Width (HSW) Register
#define REG22_PHS_PULSE_START  0x22     // PHS Pulse Start Position (HPS) Register
#define REG24_PVS_PULSE_WIDTH  0x24     // PVS Pulse Width (VSW) Register
#define REG26_PVS_PULSE_START  0x26     // PVS Pulse Start Position (VPS) Register
#define REG28_PCLK_POLARITY    0x28     // PCLK Polarity Register
#define REG2A_DSP_MODE         0x2A     // Display Mode Register
#define REG2C_PIP1_DSP_SA_0    0x2C     // PIP1 Display Start Address Register 0
#define REG2E_PIP1_DSP_SA_1    0x2E     // PIP1 Display Start Address Register 1
#define REG30_PIP1_DSP_SA_2    0x30     // PIP1 Display Start Address Register 2
#define REG32_PIP1_WIN_X_SP    0x32     // PIP1 Window X Start Position Register
#define REG34_PIP1_WIN_Y_SP_0  0x34     // PIP1 Window Y Start Position Register 0
#define REG36_PIP1_WIN_Y_SP_1  0x36     // PIP1 Window Y Start Position Register 1
#define REG38_PIP1_WIN_X_EP    0x38     // PIP1 Window X End Position Register
#define REG3A_PIP1_WIN_Y_EP_0  0x3A     // PIP1 Window Y End Position Register 0
#define REG3C_PIP1_WIN_Y_EP_1  0x3C     // PIP1 Window Y End Position Register 1
#define REG3E_PIP2_DSP_SA_0    0x3E     // PIP2 Display Start Address Register 0
#define REG40_PIP2_DSP_SA_1    0x40     // PIP2 Display Start Address Register 1
#define REG42_PIP2_DSP_SA_2    0x42     // PIP2 Display Start Address Register 2
#define REG44_PIP2_WIN_X_SP    0x44     // PIP2 Window X Start Position Register
#define REG46_PIP2_WIN_Y_SP_0  0x46     // PIP2 Window Y Start Position Register 0
#define REG48_PIP2_WIN_Y_SP_1  0x48     // PIP2 Window Y Start Position Register 1
#define REG4A_PIP2_WIN_X_EP    0x4A     // PIP2 Window X End Position Register
#define REG4C_PIP2_WIN_Y_EP_0  0x4C     // PIP2 Window Y End Position Register 0
#define REG4E_PIP2_WIN_Y_EP_1  0x4E     // PIP2 Window Y End Position Register 1
#define REG50_DISPLAY_CONTROL  0x50     // Display Control Register [WRITEONLY]
#define REG52_INPUT_MODE       0x52     // Input Mode Register
#define REG54_TRANSP_KEY_RED   0x54     // Transparency Key Color Red Register
#define REG56_TRANSP_KEY_GREEN 0x56     // Transparency Key Color Green Register
#define REG58_TRANSP_KEY_BLUE  0x58     // Transparency Key Color Blue Register
#define REG5A_WRITE_WIN_X_SP   0x5A     // Write Window X Start Position Register
#define REG5C_WRITE_WIN_Y_SP_0 0x5C     // Write Window Start Position Register 0
#define REG5E_WRITE_WIN_Y_SP_1 0x5E     // Write Window Start Position Register 1
#define REG60_WRITE_WIN_X_EP   0x60     // Write Window X End Position Register
#define REG62_WRITE_WIN_Y_EP_0 0x62     // Write Window Y End Position Register 0
#define REG64_WRITE_WIN_Y_EP_1 0x64     // Write Window Y End Position Register 1
#define REG66_MEM_DATA_PORT_0  0x66     // Memory Data Port Register 0 [WRITEONLY]
#define REG67_MEM_DATA_PORT_1  0x67     // Memory Data Port Register 1 [WRITEONLY]
#define REG68_POWER_SAVE       0x68     // Power Save Register
#define REG6A_N_DISP_PER_CTRS  0x6A     // Non-Display Period Control/Status Register
#define REG6C_GPO_0            0x6C     // General Purpose Output Register 0
#define REG6E_GPO_1            0x6E     // General Purpose Output Register 1
#define REG70_PWM_CONTROL      0x70     // PWM Control Register
#define REG72_PWM_HIGH_DC_0    0x72     // PWM High Duty Cycle Register 0
#define REG74_PWM_HIGH_DC_1    0x74     // PWM High Duty Cycle Register 1
#define REG76_PWM_HIGH_DC_2    0x76     // PWM High Duty Cycle Register 2
#define REG78_PWM_HIGH_DC_3    0x78     // PWM High Duty Cycle Register 3
#define REG7A_PWM_LOW_DC_0     0x7A     // PWM Low Duty Cycle Register 0
#define REG7C_PWM_LOW_DC_1     0x7C     // PWM Low Duty Cycle Register 1
#define REG7E_PWM_LOW_DC_2     0x7E     // PWM Low Duty Cycle Register 2
#define REG80_PWM_LOW_DC_3     0x80     // PWM Low Duty Cycle Register 3
#define REG82_SDRAM_CONTROL_0  0x82     // SDRAM Control Register 0
#define REG84_SDRAM_STATUS_0   0x84     // SDRAM Status Register 0 [WRITEONLY]
#define REG86_SDRAM_STATUS_1   0x86     // SDRAM Status Register 1 [READONLY]
#define REG88_SDRAM_MRS_VAL_0  0x88     // SDRAM MRS Value Register 0
#define REG8A_SDRAM_MRS_VAL_1  0x8A     // SDRAM MRS Value Register 1
#define REG8C_SDRAM_RFS_CNT_0  0x8C     // SDRAM Refresh Counter Register 0
#define REG8E_SDRAM_RFS_CNT_1  0x8E     // SDRAM Refresh Counter Register 1
#define REG90_SDRAM_BUF_SIZE   0x90     // SDRAM Write Buffer Memory Size Register
#define REG92_SDRAM_DEBUG      0x92     // SDRAM Debug Register
#define REG94_ALP_CONTROL      0x94     // Alpha-Blend Control Register [WRITEONLY]
#define REG96_ALP_STATUS       0x96     // Alpha-Blend Status Register [READONLY]
#define REG98_ALP_HR_SIZE      0x98     // Alpha-Blend Horizontal Size Register
#define REG9A_ALP_VR_SIZE_0    0x9A     // Alpha-Blend Vertical Size Register 0
#define REG9C_ALP_VR_SIZE_1    0x9C     // Alpha-Blend Vertical Size Register 1
#define REG9E_ALP_VALUE        0x9E     // Alpha-Blend Value Register
#define REGA0_ALP_IN_IMG1_SA_0 0xA0     // Alpha-Blend Input Image 1 Start Address Register 0
#define REGA2_ALP_IN_IMG1_SA_1 0xA2     // Alpha-Blend Input Image 1 Start Address Register 1
#define REGA4_ALP_IN_IMG1_SA_2 0xA4     // Alpha-Blend Input Image 1 Start Address Register 2
#define REGA6_ALP_IN_IMG2_SA_0 0xA6     // Alpha-Blend Input Image 2 Start Address Register 0
#define REGA8_ALP_IN_IMG2_SA_1 0xA8     // Alpha-Blend Input Image 2 Start Address Register 1
#define REGAA_ALP_IN_IMG2_SA_2 0xAA     // Alpha-Blend Input Image 2 Start Address Register 2
#define REGAC_ALP_OUT_IMG_SA_0 0xAC     // Alpha-Blend Output Image Start Address Register 0
#define REGAE_ALP_OUT_IMG_SA_1 0xAE     // Alpha-Blend Output Image Start Address Register 1
#define REGB0_ALP_OUT_IMG_SA_2 0xB0     // Alpha-Blend Output Image Start Address Register 2
#define REGB2_INTERRUPT_CTRL   0xB2     // Interrupt Control Register
#define REGB4_INTERRUPT_STAT   0xB4     // Interrupt Status Register [READONLY]
#define REGB6_INTERRUPT_CLEAR  0xB6     // Interrupt Clear Register [WRITEONLY]
#define REGFLAG_BASE           0xF0     // Special reserved flags beyond this point
#define REGFLAG_DELAY          0xFC     // PLL Register Programming Delay (in us)
#define REGFLAG_OFF_DELAY      0xFD     // LCD Panel Power Off Delay (in ms)
#define REGFLAG_ON_DELAY       0xFE     // LCD Panel Power On Delay (in ms)
#define REGFLAG_END_OF_TABLE   0xFF     // End of Registers Marker

/******************************************************************************
* Macro Definitions
******************************************************************************/
#define GetMaxX()   (DISP_HOR_RESOLUTION - 1)
#define GetMaxY()   (DISP_VER_RESOLUTION - 1)

/*****************************************************************************
* LCD commands - use LCD_write function to write these commands to the LCD. 
*****************************************************************************/

#ifndef BLACK
			#define BLACK           RGBConvert(0, 0, 0)
		#endif	
		#ifndef BRIGHTBLUE
			#define BRIGHTBLUE      RGBConvert(0, 0, 255)
		#endif	
		#ifndef BRIGHTGREEN
			#define BRIGHTGREEN     RGBConvert(0, 255, 0)
		#endif	
		#ifndef BRIGHTCYAN
			#define BRIGHTCYAN      RGBConvert(0, 255, 255)
		#endif	
		#ifndef BRIGHTRED
			#define BRIGHTRED       RGBConvert(255, 0, 0)
		#endif	
		#ifndef BRIGHTMAGENTA
			#define BRIGHTMAGENTA   RGBConvert(255, 0, 255)
		#endif	
		#ifndef YELLOW
			#define YELLOW          RGBConvert(255, 255, 128)
		#endif	
		#ifndef BRIGHTYELLOW
			#define BRIGHTYELLOW    RGBConvert(255, 255, 0)
		#endif	
		#ifndef LIGHTYELLOW
			#define LIGHTYELLOW     RGBConvert(255, 255, 150)
		#endif	
		#ifndef GOLD
			#define GOLD            RGBConvert(255, 215, 0)
		#endif
		#ifndef BLUE
			#define BLUE            RGBConvert(0, 0, 128)
		#endif	
		#ifndef GREEN
			#define GREEN           RGBConvert(0, 128, 0)
		#endif	
		#ifndef CYAN
			#define CYAN            RGBConvert(0, 128, 128)
		#endif	
		#ifndef RED
			#define RED             RGBConvert(128, 0, 0)
		#endif	
		#ifndef MAGENTA
			#define MAGENTA         RGBConvert(128, 0, 128)
		#endif	
		#ifndef BROWN
			#define BROWN           RGBConvert(255, 128, 0)
		#endif	
		#ifndef LIGHTGRAY
			#define LIGHTGRAY       RGBConvert(128, 128, 128)
		#endif	
		#ifndef DARKGRAY
			#define DARKGRAY        RGBConvert(64, 64, 64)
		#endif	
		#ifndef LIGHTBLUE
			#define LIGHTBLUE       RGBConvert(128, 128, 255)
		#endif	
		#ifndef LIGHTGREEN
			#define LIGHTGREEN      RGBConvert(128, 255, 128)
		#endif	
		#ifndef LIGHTCYAN
			#define LIGHTCYAN       RGBConvert(128, 255, 255)
		#endif	
		#ifndef LIGHTRED
			#define LIGHTRED        RGBConvert(255, 128, 128)
		#endif	
		#ifndef LIGHTMAGENTA
			#define LIGHTMAGENTA    RGBConvert(255, 128, 255)
		#endif	
		#ifndef WHITE
			#define WHITE           RGBConvert(255, 255, 255)
		#endif	
		#ifndef SADDLEBROWN
			#define SADDLEBROWN 	RGBConvert(139, 69, 19)
		#endif	
		#ifndef SIENNA
			#define SIENNA      	RGBConvert(160, 82, 45)
		#endif	
		#ifndef PERU
			#define PERU        	RGBConvert(205, 133, 63)
		#endif	
		#ifndef BURLYWOOD
			#define BURLYWOOD  	 	RGBConvert(222, 184, 135)
		#endif	
		#ifndef WHEAT
			#define WHEAT       	RGBConvert(245, 245, 220)
		#endif	
		#ifndef TAN
			#define TAN         	RGBConvert(210, 180, 140)
		#endif	
		#ifndef ORANGE
			#define ORANGE         	RGBConvert(255, 187, 76)
		#endif	
		#ifndef DARKORANGE
			#define DARKORANGE      RGBConvert(255, 140, 0)
		#endif	
		#ifndef LIGHTORANGE
			#define LIGHTORANGE     RGBConvert(255, 200, 0)
		#endif		
		#ifndef GRAY242
			#define GRAY242      	RGBConvert(242, 242, 242)    
		#endif	
		#ifndef GRAY229
			#define GRAY229      	RGBConvert(229, 229, 229)    
		#endif	
		#ifndef GRAY224
			#define GRAY224         RGBConvert(224, 224, 224)
		#endif	
		#ifndef GRAY204
			#define GRAY204      	RGBConvert(204, 204, 204)    
		#endif	
		#ifndef GRAY192
			#define GRAY192         RGBConvert(192, 192, 192)
		#endif	
		#ifndef GRAY160
			#define GRAY160         RGBConvert(160, 160, 160)
		#endif	
		#ifndef GRAY128
			#define GRAY128         RGBConvert(128, 128, 128)
		#endif	
		#ifndef GRAY096
			#define GRAY096          RGBConvert(96, 96, 96)
		#endif	
		#ifndef GRAY032
			#define GRAY032          RGBConvert(32, 32, 32)
		#endif 
		#ifndef GRAY010
			#define GRAY010          RGBConvert(10, 10, 10)
		#endif
		
        //DOM-IGNORE-BEGIN
        //Following color definitions are deprecated and should not be used in new projects.
		#ifndef GRAY95
			#define GRAY95      	RGBConvert(242, 242, 242)
		#endif	
		#ifndef GRAY90
			#define GRAY90      	RGBConvert(229, 229, 229)
		#endif		
		#ifndef GRAY0
			#define GRAY0           RGBConvert(224, 224, 224)
		#endif	
		#ifndef GRAY80
			#define GRAY80      	RGBConvert(204, 204, 204)
		#endif	
		#ifndef GRAY1
			#define GRAY1           RGBConvert(192, 192, 192)
		#endif	
		#ifndef GRAY2
			#define GRAY2           RGBConvert(160, 160, 160)
		#endif	
		#ifndef GRAY3
			#define GRAY3           RGBConvert(128, 128, 128)
		#endif	
		#ifndef GRAY4
			#define GRAY4           RGBConvert(96, 96, 96)
		#endif	
		#ifndef GRAY5
			#define GRAY5           RGBConvert(64, 64, 64)
		#endif	
		#ifndef GRAY6
			#define GRAY6           RGBConvert(32, 32, 32)
		#endif


/******************************************************************************
* Function Prototypes
******************************************************************************/
void DriverInterfaceInit(void);
void DisplayBrightness(uint8_t level);
void ResetDevice(void);
void SetBacklight(uint8_t State);
void InitGraph(void);						
uint16_t AlphaBlendWindow(uint32_t foregroundArea, int16_t foregroundLeft, int16_t foregroundTop,
                      uint32_t backgroundArea, int16_t backgroundLeft, int16_t backgroundTop,
					  uint32_t destinationArea, int16_t destinationLeft, int16_t destinationTop,		            
					  uint16_t  width, uint16_t height,  	
					  uint8_t  alphaPercentage);
					  
void CopyPageWindow( uint8_t srcPage, uint8_t dstPage,       
                        uint16_t srcX, uint16_t srcY, uint16_t dstX, uint16_t dstY, 
                        uint16_t width, uint16_t height);					  
uint8_t CreatePIPLayer(uint16_t x,uint16_t y,uint16_t width,uint16_t height);
uint8_t PIPState(uint8_t State);
uint8_t PIPDSA(uint16_t x,uint16_t y,uint8_t Page);
void UpdateDisplayNow(void);
void PutPixel(int16_t x, int16_t y);
uint16_t Bar(int16_t left, int16_t top, int16_t right, int16_t bottom);
void ClearDevice(void);
void PutImage24BPPExt(int16_t left, int16_t top, const IMAGE_EXTERNAL *image);
uint32_t GetDrawBufferAddress(void);
void SetActivePage(uint32_t page);
void SetVisualPage(uint32_t page);
void SetTestPattern(void);
uint32_t GFXGetPageXYAddress(int16_t pageNumber, uint16_t x, uint16_t y);
uint32_t GFXGetPageOriginAddress(int16_t pageNumber);
void SetClipRgn(int16_t left, int16_t top, int16_t right, int16_t bottom);
void SetClip(uint8_t control);
uint16_t Line(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
void InitGraph(void);
void SetFont(void *pFont);
void OutCharGetInfoFlash (char ch, OUTCHAR_PARAM *pParam);
uint16_t OutCharRender(char ch, OUTCHAR_PARAM *pParam);
uint16_t OutChar(char ch);
uint16_t  OutText(char *textString);
uint16_t  OutTextXY(int16_t x, int16_t y, char *textString);
int16_t GetTextWidthFlash(char* textString, void* pFont);
int16_t GetTextWidth(char *textString, void *pFont);
int16_t GetTextHeight(void *pFont);
uint16_t BarGradient(int16_t left, int16_t top, int16_t right, int16_t bottom, uint32_t color1, uint32_t color2, uint32_t length, uint8_t direction);
uint8_t Bevel(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t rad);
uint8_t FillBevel(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t rad);
void PutImage24BPPExtPartial(int16_t left, int16_t top,uint8_t Percent ,uint8_t Max,const IMAGE_EXTERNAL *image);





/* End of multiple inclusion prevention macro */
#endif