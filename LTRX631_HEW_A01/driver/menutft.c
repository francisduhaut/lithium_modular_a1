
/*******************************************************************************
* DISCLAIMER
*
* 
*
*******************************************************************************/
/* Copyright (C) 2013 Enersys. All rights reserved.   */
/*******************************************************************************
* File Name     : menutft.c
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

#include "include.h"

#define BACKGROUND BLACK

#define TEXTCOLOR RGBConvert(250, 250, 250)
#define SELECTCOLOR RGBConvert(255,250,30)
#define SELECTEDTEXTCOLOR RGBConvert(50, 50, 50)

#define SLIDERCOLOR RGBConvert(20, 110, 250)
#define TEXTADDCOLOR RGBConvert(100,100,200)

#define MENUFIRSTXOFFSET 40
#define MENUFIRSTYOFFSET 63
#define MENUFIRSTXOFFSETT 72
#define MENUFIRSTYOFFSETT 132

#define MENUFIRSTXSTEP 112
#define MENUFIRSTYSTEP 96

#define LINE_HEIGHT 26
#define LINE_WIDTH 200
#define LINE_OFFSET 50
#define LINE_TEXT_OFFSET 3
#define LINE_X_OFFSET 15

// MODIF 2.8 : using to display list to 8 module who can be reflash
#define LINE_HEIGHT_2 22
#define LINE_OFFSET_2 45
#define LINE_TEXT_OFFSET_2 3
#define LINE_X_OFFSET_2 15

#define SLIDER_X_OFFSET 8
#define SLIDER_WIDTH 3

/*******************************************************************************
* Outline       : 
* Description   : 
* Argument      : none
* Return value  : none
*******************************************************************************/
void DrawListBackground(void)
{
	SetColor(BLACK);
	Bar(5,44,GetMaxX()-5,GetMaxY()-5);		
}

/*******************************************************************************
* Outline       : 
* Description   : 
* Argument      : none
* Return value  : none
*******************************************************************************/
void DrawSecondEntryBackground(void)
{
	/*Clear background*/
	PutImage24BPPExt(0,0,&menu_header);
	
	SetColor(BLACK);
	Bar(0,44,GetMaxX(),GetMaxY());
}

/*******************************************************************************
* Outline       : 
* Description   : 
* Argument      : none
* Return value  : none
*******************************************************************************/

void DrawSecondEntryBackgroundList(void)
{
	/*Clear background*/
	
	SetColor(BLACK);
	Bar(480/2,44,GetMaxX(),GetMaxY());
	Bar(0,80,480/2,GetMaxY());
	
}

/*******************************************************************************
* Outline       : 
* Description   : 
* Argument      : none
* Return value  : none
*******************************************************************************/

void DrawSecondEntryBackgroundState(void)
{
	/*Clear background*/
	SetColor(BLACK);
	Bar(480/2,44,GetMaxX(),GetMaxY());
	Bar(0,80,480/2,GetMaxY());
	
	/*Bevel Background*/
	SetColor(RGBConvert(25,25,25));
	FillBevel(50,100,479-50,260,5);	
	
	/*Progres Bar background*/
	SetColor(RGBConvert(50,50,50));
	Bar(90,240,479-90,260);
	
	SetBackColor(BLACK);
	SetColor(WHITE);
	OutTextXY(90,215,"Progress:");	

}


/*******************************************************************************
* Outline       : 
* Description   : 
* Argument      : none
* Return value  : none
*******************************************************************************/
void DrawSecondEntryText(char *text)
{
	SetBackColor(RGBConvert(15,15,15));	
  SetColor(WHITE);
  SetFont((void *) &Font_Menu);
  OutTextXY(15,6,text);
}

/*******************************************************************************
* Outline       : 
* Description   : 
* Argument      : none
* Return value  : none
*******************************************************************************/
void DrawSecondEntryImage(BYTE position, BYTE prevposition,IMAGE_EXTERNAL *img)
{
}

/*******************************************************************************
* Outline       : 
* Description   : 
* Argument      : none
* Return value  : none
*******************************************************************************/
void DrawLineAdd (BYTE position, char *text)
{
    WORD ys,xs;
    xs = LINE_X_OFFSET;
    ys = LINE_OFFSET + (position * LINE_HEIGHT);
	
	  SetBackColor(BACKGROUND);
    SetColor(TEXTADDCOLOR);
    OutTextXY(xs+200,ys+LINE_TEXT_OFFSET,text);
}

/*******************************************************************************
* Outline       : 
* Description   : 
* Argument      : none
* Return value  : none
*******************************************************************************/
void DrawSlider(WORD NbOfElements,WORD position,BYTE Alignment)
{
   WORD ys,ye,xs,xe,BarHeight;
   DWORD BarHeightCorrection;
 if (Alignment == 0)
 {
     xs = SLIDER_X_OFFSET;
     xe = SLIDER_X_OFFSET + SLIDER_WIDTH;
 }
 else
 {
     xe = GetMaxX()- SLIDER_X_OFFSET;
     xs = GetMaxX()- (SLIDER_X_OFFSET + SLIDER_WIDTH);  
 }
    SetColor(BACKGROUND);
    Bar(xs,LINE_OFFSET+LINE_HEIGHT,xe,LINE_OFFSET+(LINE_HEIGHT*7));
 if (MnuValCorrection > 0) MnuValCorrection = 1;
    if (NbOfElements> (8 - MnuValCorrection))
    {
  BarHeightCorrection =  ((700*LINE_HEIGHT))/((NbOfElements - (8 - MnuValCorrection))+1);
        //BarHeight =  (7*LINE_HEIGHT)/((NbOfElements - (8 - MnuValCorrection))+1);
  
  BarHeight = BarHeightCorrection / 100;
  
        if (position > 0)
        {
            //ys = LINE_OFFSET + LINE_HEIGHT + ((position)) * BarHeight;
   ys = LINE_OFFSET + LINE_HEIGHT + ( ((((DWORD)position)) * BarHeightCorrection) / 100  );
            //ye = ys + BarHeight;
   ye = ys + BarHeight;
        }
        else
        {
   ys = LINE_OFFSET + LINE_HEIGHT;
            //ye = ys + BarHeight;
   ye = ys + BarHeight;
        }
      SetColor(SLIDERCOLOR);
       Bar(xs,ys,xe,ye);
    }
}


/*******************************************************************************
* Outline       : 
* Description   : 
* Argument      : none
* Return value  : none
*******************************************************************************/
void DrawLine (BYTE position, BYTE level, char *text,WORD Status)
{
    WORD ys,ye,xs,xe;

    xe = (GetMaxX()/2)-30;
    xs = LINE_X_OFFSET;

    ys = LINE_OFFSET + (position * LINE_HEIGHT);
    ye = ys + LINE_HEIGHT - 2;

    if ((Status & SELECTED) != 0)
    {
        SetBackColor(SELECTCOLOR);
        SetColor(SELECTCOLOR);
        Bar(xs,ys,xe,ye);	
        SetColor(SELECTEDTEXTCOLOR);
        OutTextXY(xs+15,ys+LINE_TEXT_OFFSET,text);
    }
    else if ((Status & TITLE) != 0)
    {
        SetBackColor(BACKGROUND);
        SetColor(BACKGROUND);
        Bar(xs,ys,xe,ye);
        SetColor(TEXTCOLOR);
        OutTextXY(xs,ys+LINE_TEXT_OFFSET,text);
    }
    else
    {
        SetBackColor(BACKGROUND);
        SetColor(BACKGROUND);
        Bar(xs,ys,xe,ye);
        SetColor(TEXTCOLOR);
        OutTextXY(xs+15,ys+LINE_TEXT_OFFSET,text);
    }

	  SetLineThickness(NORMAL_LINE);	
    
    if (position == 0)
    {
        SetColor(SELECTCOLOR);
        Line(xs,ye+1,xe,ye+1);
    }
    else
    {
        SetColor(SELECTCOLOR);
        Line(xs,ye+1,xe,ye+1);
    }
}

/*******************************************************************************
* Outline       : 
* Description   : 
* Argument      : none
* Return value  : none
*******************************************************************************/

void DrawLine2Status (BYTE position, BYTE level, char *text,WORD Status)
{
    WORD ys,ye,xs,xe,i,offset;
    char text1[25],text2[25],c;

    for (i = 0;i < 25;i++)
    {
      c = *text;
      if ((c == 36) || (c == 0))
      {
        text1[i] = 0;
        text++;
        break;    
      }
      text1[i] = *text;
      text++;
    }

    for (i = 0;i < 25;i++)
    {
      c = *text;
      if ((c == 36) || (c == 0))
      {
        text2[i] = 0;
        text++;
        break;    
      }
      text2[i] = *text;
      text++;
    }

    offset = GetTextWidth(&text2[0],(void *) &Font_Menu);

    xe = GetMaxX() - LINE_X_OFFSET;
	
	  if ((Status & LONGTEXT) != 0) xs = (GetMaxX()/2) + 0;
	  else xs = (GetMaxX()/2) + 20;	
    
    ys = LINE_OFFSET + (position * LINE_HEIGHT);
    ye = ys + LINE_HEIGHT-2;

	  SetColor(SELECTCOLOR);
	  SetLineThickness(NORMAL_LINE);
    Line(xs,ye+1,xe,ye+1);

    if ((Status & SELECTED) != 0)
    {
        SetBackColor(SELECTCOLOR);
        SetColor(SELECTCOLOR);
        Bar(xs,ys,xe,ye);
		    SetColor(SELECTEDTEXTCOLOR);
    }
    else
    {
        SetBackColor(BACKGROUND);
        SetColor(BACKGROUND);
        Bar(xs,ys,xe,ye);
        SetColor(TEXTCOLOR);
    }

    OutTextXY(xs+15,ys+LINE_TEXT_OFFSET,&text1[0]);
    OutTextXY(GetMaxX()-16-offset,ys+LINE_TEXT_OFFSET,&text2[0]);
}


/*******************************************************************************
* Outline       : 
* Description   : 
* Argument      : none
* Return value  : none
*******************************************************************************/
void DrawLine2 (BYTE position, BYTE level, char *text,WORD Status)
{
    WORD ys,ye,xs,xe,i;

    xe = GetMaxX() - LINE_X_OFFSET;
	
	  if ((Status & LONGTEXT) != 0) xs = (GetMaxX()/2) + 0;
	  else xs = (GetMaxX()/2) + 20;	
    

    ys = LINE_OFFSET + (position * LINE_HEIGHT);
    ye = ys + LINE_HEIGHT-2;
	
	  SetBackColor(SELECTCOLOR);
    SetColor(SELECTCOLOR);
	  SetLineThickness(NORMAL_LINE);
    Line(xs,ye+1,xe,ye+1);


    if ((Status & SELECTED) != 0)
    {
      SetBackColor(SELECTCOLOR);
      SetColor(SELECTCOLOR);
      Bar(xs,ys,xe,ye);
		  SetColor(SELECTEDTEXTCOLOR);
		  if ((Status & OPTION) == 0)
		  {
        OutTextXY(xs+15,ys+LINE_TEXT_OFFSET,text);
		  }
    }
    else
    {
        SetBackColor(BACKGROUND); 
        SetColor(BACKGROUND);
        Bar(xs,ys,xe,ye);
        SetColor(TEXTCOLOR);
    }

    if ((Status & OPTION) != 0)
    {
        i = GetTextWidth("*",(void *) &Font_Menu);
        OutTextXY(xs+15,ys+LINE_TEXT_OFFSET,"*");
        OutTextXY(xs+15+i+2,ys+LINE_TEXT_OFFSET,text);
    }
    else
    {
        OutTextXY(xs+15,ys+LINE_TEXT_OFFSET,text);
    }
}

/*******************************************************************************
* Outline       : 
* Description   : 
* Argument      : none
* Return value  : none
*******************************************************************************/
void DrawLine3 (BYTE position, BYTE level, char *text,WORD Status)
{   // MODIF 2.8 : new function to display to 8 module who can be reflash
    WORD ys,ye,xs,xe,i;

    xe = GetMaxX() - LINE_X_OFFSET_2;
	
	  if ((Status & LONGTEXT) != 0) xs = (GetMaxX()/2) + 0;
	  else xs = (GetMaxX()/2) + 20;	
    

    ys = LINE_OFFSET_2 + (position * LINE_HEIGHT_2);
    ye = ys + LINE_HEIGHT_2-2;
	
	  SetBackColor(SELECTCOLOR);
    SetColor(SELECTCOLOR);
	  SetLineThickness(NORMAL_LINE);
    Line(xs,ye+1,xe,ye+1);


    if ((Status & SELECTED) != 0)
    {
      SetBackColor(SELECTCOLOR);
      SetColor(SELECTCOLOR);
      Bar(xs,ys,xe,ye);
		  SetColor(SELECTEDTEXTCOLOR);
		  if ((Status & OPTION) == 0)
		  {
        OutTextXY(xs+15,ys+LINE_TEXT_OFFSET_2,text);
		  }
    }
    else
    {
        SetBackColor(BACKGROUND); 
        SetColor(BACKGROUND);
        Bar(xs,ys,xe,ye);
        SetColor(TEXTCOLOR);
    }

    if ((Status & OPTION) != 0)
    {
        i = GetTextWidth("*",(void *) &Font_Menu);
        OutTextXY(xs+15,ys+LINE_TEXT_OFFSET_2,"*");
        OutTextXY(xs+15+i+2,ys+LINE_TEXT_OFFSET_2,text);
    }
    else
    {
        OutTextXY(xs+15,ys+LINE_TEXT_OFFSET_2,text);
    }
}

/*******************************************************************************
* Outline       : 
* Description   : 
* Argument      : none
* Return value  : none
*******************************************************************************/
void Cursor(WORD x,WORD y,GFX_COLOR Color)
{
	SetColor(SELECTCOLOR);
	SetLineThickness(THICK_LINE);
  Line(x-1,y+21,x+10,y+21);
}


/*******************************************************************************
* Outline       : 
* Description   : 
* Argument      : none
* Return value  : none
*******************************************************************************/
void Putchar(WORD x,WORD y,char *c,uint8_t Select)
{
  char text[5];
  text[0] = *c;
  text[1] = '\0';
 
	SetBackColor(BACKGROUND);
	if (Select == 0)
		SetColor(TEXTCOLOR);
	else
		SetColor(SELECTCOLOR);
    OutTextXY(x,y,text);
}



/*******************************************************************************
* Outline       : 
* Description   : 
* Argument      : none
* Return value  : none
*******************************************************************************/
void DrawPasswordPointer (uint8_t ptr)
{
	uint8_t i;
	uint16_t xs;
	
	for (i = 0;i<4;i++)
	{
		xs = 210 + ((i%4)*25);	
		
		if (i < ptr)
			PutImage24BPPExt(xs,246,&bullet_1);
		else
		{
			PutImage24BPPExt(xs,246,&bullet_2);
		}
	}	
}

/*******************************************************************************
* Outline       : 
* Description   : 
* Argument      : none
* Return value  : none
*******************************************************************************/
void DrawPasswordBackground (void)
{
	CopyPageWindow(6,GetDrawBufferAddress(),0,0,0,0,GetMaxX(),GetMaxY());
	CreateScreenHeader();
	
	PutImage24BPPExt(176,240,&title_menu_back);	
}

/*******************************************************************************
* Outline       : 
* Description   : 
* Argument      : none
* Return value  : none
*******************************************************************************/
const uint8_t TextPswTable[12][4] = {"1","2","3","4","5","6","7","8","9","0","OK","X"};

void DrawPasswordIcons(uint8_t ptr)
{
	char text[5];
	uint8_t i;
	uint16_t xs,ys;
	uint16_t offset;
	
	for (i=0;i<12;i++)
	{
		xs = 155 + i%4 * 45;  	
		ys = 80 + i/4 * 45;		
		
		if (i == ptr)
			PutImage24BPPExt(xs,ys,&password_select);
		else
			PutImage24BPPExt(xs,ys,&password);
			
    	strcpy((char *)&text[0],(char *)&TextPswTable[i][0]);
    	
		SetBackColor(RGBConvert(128,128,128));
		SetColor(BLACK);
			
		offset = GetTextWidth(&text[0],(void *) &Font_Menu)/2;
    OutTextXY(xs + 23 - offset,ys + 13,&text[0]);			
	}
}


/*******************************************************************************
* Outline       : 
* Description   : 
* Argument      : none
* Return value  : none
*******************************************************************************/
void DrawMenuFirstEnrtyText(uint8_t pos, char *ptr, uint8_t color)
{
 uint16_t xs,ys,i,j,xe;
 if (pos != 255)
 {
  SetFont((void *) &Font_SvBold);
  j = 17;
  i = GetTextWidth(ptr,(void *) &Font_SvBold);
  
  xs = (40 + (pos/3 * 392)) - (i/2);
  ys = 84 + (pos%3) * 60; 
  
  xs-=2;
  xe = xs+(i+4); 
  
  if (xe > 480) xe = 479;
  if (xs > 480) xs = 0;    
    
  if (color == 0) 
  {
   SetColor(RGBConvert(0, 0, 0));
   //Bar(xs,ys,xe,ys+(j));
   CopyPageWindow(SCREEN_WAIT_MENU,GetDrawBufferAddress(),xs,ys,xs,ys,(i+10),20);
   SetBackColor(RGBConvert(0, 0, 0));
   SetColor(WHITE);
  }
  else 
  {
   SetColor(BLACK);   
   Bar(185,246,300,264);
   
   SetBackColor(RGBConvert(0, 0, 0));
   SetColor(WHITE);   
   OutTextXY(240-(i/2),250,ptr);
   
   SetColor(WHITE);
   Bar(xs,ys,xe,ys+(j));
   SetBackColor(WHITE);
   SetColor(BLACK);   
  }
   OutTextXY(xs+2,ys+2,ptr);
   //UpdateDisplayNow();	
 }
}

void DrawMenuFirstEntryPartialSelected (IMAGE_EXTERNAL *img)
{
	/*To get correct printing with menuschearch*/
	SetColor(WHITE);
	if (img != 0)
		PutImage24BPPExt(GetMaxX()/2-64,GetMaxY()/2-64,img);
	else	
		Bar(GetMaxX()/2-64,GetMaxY()/2-64,GetMaxX()/2-64+128,GetMaxY()/2-64+128);
}

/*******************************************************************************
* Outline       : 
* Description   : 
* Argument      : none
* Return value  : none
*******************************************************************************/
void DrawMenuFirstEnrtyPartial(uint8_t pos,uint8_t prevpos,IMAGE_EXTERNAL *img)
{
	uint16_t xs,ys,xe,ye;
	
	/*To get correct printing with menuschearch*/
	if (pos != 255)
	{
		
		xs = 20 + (pos/3 * 390);
		ys = 45 + (pos%3) * 60;			

		xe = xs+40;
		ye = ys+40;
	
		SetColor(WHITE);
		if (img != 0)
		{
			PutImage24BPPExt(xs,ys,img);
		}
		else	
			Bar(xs,ys,xe,ye);
	}
	
	if (prevpos != 255)
	{
		xs = 20 + (pos/3 * 390);
		ys = 45 + (pos%3) * 60;	
		
		xe = xs+40;
		ye = ys+40;
	
		SetColor(RGBConvert(64, 64, 64));	
		if (img != 0)
		{
			PutImage24BPPExt(xs,ys,img);
		}
		else	
			Bar(xs,ys,xe,ye);		
	}
}

/*******************************************************************************
* Outline       : 
* Description   : 
* Argument      : none
* Return value  : none
*******************************************************************************/
void DrawMenuFirstEntryBackground (void)
{
    /* Print Menu text */
	CopyPageWindow(6,GetDrawBufferAddress(),0,0,0,0,GetMaxX(),GetMaxY());
	
	CreateScreenHeader();
	 
	PutImage24BPPExt(165,240,&title_menu_back);	
}

/*******************************************************************************
* Outline       : 
* Description   : 
* Argument      : none
* Return value  : none
*******************************************************************************/
void DrawDataInputBackground (void)
{
	SetColor(BACKGROUND);
	Bar(GetMaxX()/2,80,GetMaxX()-5,GetMaxY()-5);	
}


uint8_t BootApiStep = 0;
uint8_t BootApiState = SEND;
FATFS  fl_fatfs;
// MODIF 2.8 : increase buffer size to manage 8 modules
structTextList TextList[20];
structUpdateList UpdateList[40];
uint8_t TextListCurrent = 0;
uint8_t TextListOffset = 0;
uint8_t TextListMax = 0;

uint32_t 		ap_status;
can_frame_t     can_dataframe[8];
uint32_t TimeoutMse = 0;
uint8_t FrameRecv = 0;
uint8_t RecvFrame[8];
//extern volatile uint32_t g_CAN_channel;
uint8_t AllStep = 0;
uint8_t ListGenerated = 0;

uint8_t InBootUpdate = 0;

AppHeaderStruct BootFileHeader;
AppHeaderStruct BootFileHeader_PRC;

/* When exit this variables need to be cleared */

void ResetBootloaderApi(void)
{
	InBootUpdate = 0;
	BootApiStep = 0;
	BootApiState = SEND;	
}


void ExitMsg(char* ptr)
{
	SetBackColor(BLACK);
	SetColor(WHITE);
	OutTextXY(50,80,ptr);					
}


void DrawDisplayList(void)
{
	uint8_t i;
	
	DrawSecondEntryBackgroundList();
	
	for(i=0;i<TextListMax;i++)
	{   // MODIF 2.8 : use new DrawLine3 function
		if ((TextListOffset + i) == TextListCurrent)
			//DrawLine2(TextListOffset + i,0,&TextList[TextListOffset + i].Text[0],0x0001);		
            DrawLine3(TextListOffset + i,0,&TextList[TextListOffset + i].Text[0],0x0001);		
		else
			//DrawLine2(TextListOffset + i,0,&TextList[TextListOffset + i].Text[0],0x0000);
            DrawLine3(TextListOffset + i,0,&TextList[TextListOffset + i].Text[0],0x0000);
	}
}

uint8_t BootloaderApi (void)
{
	uint8_t Update = 0;
	uint16_t i;
 	FILINFO Fileinfo;
	static char text[25];
	static uint8_t ChronoStatus = 0;
	
	/*Api Function List:*/
	/*Check USB*/
	/*Check Files*/
	/*Create List*/
	/*Display & Select List*/
	/*Check Revision*/
	/*Upload (Read / Decode / Send)*/
	/*Force Exit in*/
	
	switch (BootApiState)
	{
		case SEND:
			switch (BootApiStep)
			{
				case 0:
					
					/* Clear Display */
					DrawSecondEntryBackgroundList();
				
					if (!(UsbDisk != 0))
					{	
						SetBackColor(BLACK);
						SetColor(BRIGHTRED);
						i = GetTextWidth("No USB FlashDrive!",(void *) &Font_Menu);
	  					OutTextXY((GetMaxX()/2)-(i/2),GetMaxY()/2+30,"No USB FlashDrive!");						
						Update = 1;	
						BootApiState = SUSPEND;
						break;
					}
					else if (R_tfat_f_mount(0, &fl_fatfs) != TFAT_FR_OK)
					{
						SetBackColor(BLACK);
						SetColor(BRIGHTRED);
						i = GetTextWidth("USB Error!",(void *) &Font_Menu);
	  					OutTextXY((GetMaxX()/2)-(i/2),GetMaxY()/2+30,"USB Error!");				
						Update = 1;	
						BootApiState = SUSPEND;								
						break;
					}
					else if ((Menu.ChgType == 0) && (R_tfat_f_stat("LLC631.hex",&Fileinfo) != TFAT_FR_OK)) 
					{
						SetBackColor(BLACK);
						SetColor(BRIGHTRED);
						i = GetTextWidth("File Not Found!",(void *) &Font_Menu);
	  					OutTextXY((GetMaxX()/2)-(i/2),GetMaxY()/2+30,"File Not Found!");					
						Update = 1;	
						BootApiState = SUSPEND;						
						break;
					}
					else if ((Menu.ChgType != 0) &&  ( (R_tfat_f_stat("PRC631.hex",&Fileinfo) != TFAT_FR_OK) && (R_tfat_f_stat("PFC631.hex",&Fileinfo) != TFAT_FR_OK)) ) 
					{
						SetBackColor(BLACK);
						SetColor(BRIGHTRED);
						i = GetTextWidth("File Not Found!",(void *) &Font_Menu);
	  					OutTextXY((GetMaxX()/2)-(i/2),GetMaxY()/2+30,"File Not Found!");					
						Update = 1;	
						BootApiState = SUSPEND;						
						break;
					}					
					
						InBootUpdate = 1;
						
						/* GetFileStatus (search device if only valid files are present)*/
						i = GetFileHeaderInformation(&BootFileHeader,&BootFileHeader_PRC);
						
						/* Create list */
						TextListMax = 0;
						StartBootloaderChronoSearch(i);
						BootApiStep = 1;
						ListGenerated = 0;
					
							
				break;
				
				case 1:				
				
					if (ListGenerated > 0)
					{
						InBootUpdate = 0;
						
						if (ListGenerated == 255)
						{
							SetBackColor(BLACK);
							SetColor(BRIGHTRED);
							i = GetTextWidth("No Module Found!",(void *) &Font_Menu);
	  						OutTextXY((GetMaxX()/2)-(i/2),GetMaxY()/2+30,"No Module Found!");												
							Update = 1;	
							BootApiState = SUSPEND;	
							//break;		
						}
						else
						{
							/*Display list*/
							DrawDisplayList();
							Update = 1;	
							AllStep = 0;
							BootApiState = LIST;						
						}
					}

				break;
				
				case 2:
				
					/* which address want to update */
//DEBUG					
					if (UpdateList[AllStep].CanID == 0xFFFF)
					{
							SetBackColor(BLACK);
							SetColor(BRIGHTRED);
							i = GetTextWidth("Updater List Error!",(void *) &Font_Menu);
	  						OutTextXY((GetMaxX()/2)-(i/2),GetMaxY()/2+30,"Updater List Error");												
							Update = 1;	
							BootApiState = SUSPEND;	
							break;						
					}
//DEBUG END					
					if (StartBootloaderChrono(UpdateList[AllStep].CanID,UpdateList[AllStep].SubCanID) == 255)
					{
						SetBackColor(BLACK);
						SetColor(BRIGHTRED);
						i = GetTextWidth("Requested file missing!",(void *) &Font_Menu);
  						OutTextXY((GetMaxX()/2)-(i/2),GetMaxY()/2+30,"Requested file missing!");												
						Update = 1;	
						BootApiState = SUSPEND;	
						break;
					}

					if ((UpdateList[AllStep].SubCanID == 0) || (UpdateList[AllStep].SubCanID == 1))
						sprintf(&text[0],"MOD %d : -> R%d.%d",UpdateList[AllStep].CanID + 1,BootFileHeader.AppVersion/100,BootFileHeader.AppVersion%100);	
					else
						sprintf(&text[0],"MOD %d : -> R%d.%d",UpdateList[AllStep].CanID + 1,BootFileHeader_PRC.AppVersion/100,BootFileHeader_PRC.AppVersion%100);	
					
					InBootUpdate = 1;
					
					DrawSecondEntryBackgroundState();
					
					SetBackColor(BLACK);
					SetColor(WHITE);
					i = GetTextWidth(&text[0],(void *) &Font_Menu);
	  				OutTextXY((GetMaxX()/2)-(i/2),110,&text[0]);					
					Update = 1;
					BootApiStep = 3;
					
					
				break;
				
				
				case 3:
					if (ChronoStatus != GetBootloaderChronoStatus())
					{
						ChronoStatus = GetBootloaderChronoStatus();
						
						if (ChronoStatus < 101)
						{
							//Updatetext
							SetColor(RGBConvert(25,25,25));
							Bar(200,215,280,235);
							SetBackColor(BLACK);
							SetColor(WHITE);
							sprintf(&text[0],"%d %%",ChronoStatus);
							i = GetTextWidth(&text[0],(void *) &Font_Menu);
		  					OutTextXY((GetMaxX()/2)-(i/2),215,&text[0]);	
						
							//Update BAR
							SetColor(RGBConvert(255,255,0));
							Bar(90,240,90+ChronoStatus*3,260);
							Update = 1;	
						}
						if (ChronoStatus > 99)
						{	
							if (ChronoStatus == 101)
							{
								SetBackColor(BLACK);
								SetColor(BRIGHTRED);
								
								sprintf(&text[0],"Error -> %s",&MsgError[0]);
								//sprintf(&text[i]," -> R%d.%d",BootFileHeader.AppVersion/100,BootFileHeader.AppVersion%100);
								
								i = GetTextWidth(&text[0],(void *) &Font_Menu);
	  							OutTextXY((GetMaxX()/2)-(i/2),GetMaxY()/2+30,&text[0]);					
								Update = 1;	
								Key.StartShort = 0;
								Key.MenuShort = 0;									
								BootApiStep = 4;
								break;									
							}
							
							/*Go to next can address*/
							AllStep++;	
							
							if (UpdateList[AllStep].CanID == 0xFFFF)
							{
								/*No more module to update*/
								SetBackColor(BLACK);
								SetColor(RGBConvert(0,255,0));
								i = GetTextWidth("ALL FINISHED!",(void *) &Font_Menu);
  								OutTextXY((GetMaxX()/2)-(i/2),GetMaxY()/2+30,"ALL FINISHED!");					
								Update = 1;	
								
								Key.StartShort = 0;
								Key.MenuShort = 0;									
								BootApiStep = 4;
								/* Update Finished */
								/* Write text finished or start next update after all finished generate list again */									
							}
							else
							{
								/*Go to module to update*/
								BootApiStep = 2;
								
							}
						}
					}
					break;
					
				case 4:
					
					/* If ok key pressed */
					if ((Key.StartShort != 0) || (Key.MenuShort != 0))
					{
						Key.StartShort = 0;
						Key.MenuShort = 0;
						
						/*Restart Process*/
						BootApiStep = 0;
						InBootUpdate = 0;
					}							
									
				break;
					
			}
			
		break;
		
		case LIST:		
			/* Update Display List */
		
			/* Test if key Down pressed */
	    	if (Key.DownShort != 0)
	    	{
	      		Key.DownShort = 0;
					
	        	if (TextListCurrent < (TextListMax-1))
	          	{
	            	TextListCurrent++;
			
	            	if ((TextListOffset + 9 != TextListMax) && (TextListOffset + 9 <= TextListCurrent))
					{
						TextListOffset++;
					}
					Update = 1;	
					DrawDisplayList();			  				
				}	
			}
				
	      	/* Test if key up pressed */
			if (Key.UpShort != 0)
			{
	      		Key.UpShort = 0;				  				

          		if (TextListCurrent != 0)
          		{
           			TextListCurrent --;

            		if ((TextListOffset != 0) && (TextListCurrent == (TextListOffset + 1)))
              			TextListOffset --;
			
					Update = 1;
					DrawDisplayList();
				}
			}
	  
	  
			if (Key.StartShort != 0)             /* If key OK pressed, exit */
			{
				/* Generate Update List */
				if (TextList[TextListCurrent].All == 0xFF)
				{
					GenerateUpdateList(0xFF);	
				}
				else
				{
					GenerateUpdateList(TextListCurrent);	
				}
				
				AllStep = 0;
				Key.StartShort = 0;
				BootApiStep = 2;
				BootApiState = SEND;
			}			

		
		break;
		
		case SUSPEND:

		
		break;
		
		
	}
	
	return Update;	
}