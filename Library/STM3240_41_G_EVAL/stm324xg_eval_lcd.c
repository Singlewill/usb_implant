/**
  ******************************************************************************
  * @file    stm324xg_eval_lcd.c
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    05-March-2012
  * @brief   This file includes the LCD driver for AM-240320L8TNQW00H (LCD_ILI9320)
  *          and AM240320D5TOQW01H (LCD_ILI9325) Liquid Crystal Display Modules
  *          of STM324xG-EVAL evaluation board(MB786) RevB.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm324xg_eval_lcd.h"
#include "../Common/fonts.c"

/** @addtogroup Utilities
  * @{
  */ 

/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup STM324xG_EVAL
  * @{
  */
    
/** @defgroup STM324xG_EVAL_LCD 
  * @brief This file includes the LCD driver for AM-240320L8TNQW00H (LCD_ILI9320)
  *        and AM240320D5TOQW01H (LCD_ILI9325) Liquid Crystal Display Modules
  *        of STM324xG_EVAL board.
  * @{
  */ 

/** @defgroup STM324xG_EVAL_LCD_Private_TypesDefinitions
  * @{
  */ 


/** @defgroup STM324xG_EVAL_LCD_Private_Defines
  * @{
  */
/* Note: LCD /CS is NE3 - Bank 3 of NOR/SRAM Bank 1~4 */
#define LCD_REG              (*((volatile unsigned short *) 0x60000000)) /* RS = 0 */
#define LCD_RAM              (*((volatile unsigned short *) 0x60020000)) /* RS = 1 */

#define MAX_POLY_CORNERS   200
#define POLY_Y(Z)          ((int32_t)((Points + Z)->X))
#define POLY_X(Z)          ((int32_t)((Points + Z)->Y))

/* Private variables ---------------------------------------------------------*/
static uint8_t LCD_Code;

/* Private define ------------------------------------------------------------*/
#define  ILI9320    0  /* 0x9320 */
#define  ILI9325    1  /* 0x9325 */
#define  ILI9328    2  /* 0x9328 */
#define  ILI9331    3  /* 0x9331 */
#define  SSD1298    4  /* 0x8999 */
#define  SSD1289    5  /* 0x8989 */
#define  ST7781     6  /* 0x7783 */
#define  LGDP4531   7  /* 0x4531 */
#define  SPFD5408B  8  /* 0x5408 */
#define  R61505U    9  /* 0x1505 0x0505 */	   
#define  HX8347D    10 /* 0x0047 */
#define  HX8347A    11 /* 0x0047 */	
#define  LGDP4535   12 /* 0x4535 */  
#define  SSD2119    13 /* 3.5 LCD 0x9919 */


/**
  * @}
  */ 

/** @defgroup STM324xG_EVAL_LCD_Private_Macros
  * @{
  */
#define ABS(X)  ((X) > 0 ? (X) : -(X))     
/**
  * @}
  */ 
    
/** @defgroup STM324xG_EVAL_LCD_Private_Variables
  * @{
  */ 
static sFONT *LCD_Currentfonts;

  /* Global variables to set the written text color */
__IO uint16_t TextColor = 0x0000, BackColor = 0xFFFF;
  
/**
  * @}
  */ 


/** @defgroup STM324xG_EVAL_LCD_Private_FunctionPrototypes
  * @{
  */ 
#ifndef USE_Delay
static void delay(__IO uint32_t nCount);
#endif /* USE_Delay*/
static void PutPixel(int16_t x, int16_t y);
static void LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount, uint16_t Closed);


/**
  * @}
  */ 


/** @defgroup STM324xG_EVAL_LCD_Private_Functions
  * @{
  */ 

/**
  * @brief  DeInitializes the LCD.
  * @param  None
  * @retval None
  */
void LCD_DeInit(void)
{ 
  GPIO_InitTypeDef GPIO_InitStructure;

  /* BANK 3 (of NOR/SRAM Bank 1~4) is disabled */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
  
  /*!< LCD_SPI DeInit */
  FSMC_NORSRAMDeInit(FSMC_Bank1_NORSRAM1);
   
/*-- GPIO Configuration ------------------------------------------------------*/
  /* SRAM Data lines configuration */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | 
	                            GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
								 GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
	 PE.14(D11), PE.15(D12) as alternate function push pull */	
	/* GPIOE configuration */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2 | GPIO_Pin_7 | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 |
	                             GPIO_Pin_11| GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void delay_ms(uint16_t ms)    
{ 
	uint16_t i,j; 
	for( i = 0; i < ms; i++ )
	{ 
		for( j = 0; j < 1141; j++ );
	}
} 

/**
  * @brief  Initializes the LCD.
  * @param  None
  * @retval None
  */
void STM324xG_LCD_Init(void)
{ 
  __IO uint32_t lcdid = 0;
  
/* Configure the LCD Control pins --------------------------------------------*/
  LCD_CtrlLinesConfig();

/* Configure the FSMC Parallel interface -------------------------------------*/
  LCD_FSMCConfig();

  _delay_(5); /* delay 50 ms */
	delay_ms(300);
  /* Read the LCD ID */
  lcdid = LCD_ReadReg(0x00);  

  /* Check if the LCD is ILI9320 Controller */
  	if( lcdid == 0x9325 || lcdid == 0x9328 )	
	{
		LCD_Code = ILI9325;
		LCD_WriteReg(0xE5, 0x78F0); /* set SRAM internal timing */
		LCD_WriteReg(0x01, 0x0100); /* set Driver Output Control */
		LCD_WriteReg(0x02, 0x0700); /* set 1 line inversion */
		#if (DISP_ORIENTATION == 0)   /* set GRAM write direction and BGR=1 */
			LCD_WriteReg(0x03,(1<<12)|(1<<5)|(1<<4)|(0<<3));
		#elif (DISP_ORIENTATION == 90)
			LCD_WriteReg(0x03,(1<<12)|(0<<5)|(1<<4)|(1<<3));		
		#elif (DISP_ORIENTATION == 180)
			LCD_WriteReg(0x03,(1<<12)|(0<<5)|(0<<4)|(0<<3));
		#elif (DISP_ORIENTATION == 270)
			LCD_WriteReg(0x03,(1<<12)|(1<<5)|(0<<4)|(1<<3));
		#endif
		LCD_WriteReg(0x04, 0x0000); /* Resize register */
		LCD_WriteReg(0x08, 0x0207); /* set the back porch and front porch */
		LCD_WriteReg(0x09, 0x0000); /* set non-display area refresh cycle ISC[3:0] */
		LCD_WriteReg(0x0A, 0x0000); /* FMARK function */
		LCD_WriteReg(0x0C, 0x0000); /* RGB interface setting */
		LCD_WriteReg(0x0D, 0x0000); /* Frame marker Position */
		LCD_WriteReg(0x0F, 0x0000); /* RGB interface polarity */
		/*************Power On sequence ****************/
		LCD_WriteReg(0x10, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
		LCD_WriteReg(0x11, 0x0007); /* DC1[2:0], DC0[2:0], VC[2:0] */
		LCD_WriteReg(0x12, 0x0000); /* VREG1OUT voltage */
		LCD_WriteReg(0x13, 0x0000); /* VDV[4:0] for VCOM amplitude */
		LCD_WriteReg(0x07, 0x0001);
		delay_ms(200); 
		/* Dis-charge capacitor power voltage */
		LCD_WriteReg(0x10, 0x1090); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
		LCD_WriteReg(0x11, 0x0227); /* Set DC1[2:0], DC0[2:0], VC[2:0] */
		delay_ms(50); 							/* Delay 50ms */
		LCD_WriteReg(0x12, 0x001F); 
		delay_ms(50); 							/* Delay 50ms */
		LCD_WriteReg(0x13, 0x1500); /* VDV[4:0] for VCOM amplitude */
		LCD_WriteReg(0x29, 0x0027); /* 04 VCM[5:0] for VCOMH */
		LCD_WriteReg(0x2B, 0x000D); /* Set Frame Rate */
		delay_ms(50); 							/* Delay 50ms */
		LCD_WriteReg(0x20, 0x0000); /* GRAM horizontal Address */
		LCD_WriteReg(0x21, 0x0000); /* GRAM Vertical Address */
		/* ----------- Adjust the Gamma Curve ---------- */
		LCD_WriteReg(0x30, 0x0000);
		LCD_WriteReg(0x31, 0x0707);
		LCD_WriteReg(0x32, 0x0307);
		LCD_WriteReg(0x35, 0x0200);
		LCD_WriteReg(0x36, 0x0008);
		LCD_WriteReg(0x37, 0x0004);
		LCD_WriteReg(0x38, 0x0000);
		LCD_WriteReg(0x39, 0x0707);
		LCD_WriteReg(0x3C, 0x0002);
		LCD_WriteReg(0x3D, 0x1D04);
		/* ------------------ Set GRAM area --------------- */
		LCD_WriteReg(0x50, 0x0000); /* Horizontal GRAM Start Address */
		LCD_WriteReg(0x51, 0x00EF); /* Horizontal GRAM End Address */
		LCD_WriteReg(0x52, 0x0000); /* Vertical GRAM Start Address */
		LCD_WriteReg(0x53, 0x013F); /* Vertical GRAM Start Address */
		LCD_WriteReg(0x60, 0xA700); /* Gate Scan Line */
		LCD_WriteReg(0x61, 0x0001); /* NDL,VLE, REV */
		LCD_WriteReg(0x6A, 0x0000); /* set scrolling line */
		/* -------------- Partial Display Control --------- */
		LCD_WriteReg(0x80, 0x0000);
		LCD_WriteReg(0x81, 0x0000);
		LCD_WriteReg(0x82, 0x0000);
		LCD_WriteReg(0x83, 0x0000);
		LCD_WriteReg(0x84, 0x0000);
		LCD_WriteReg(0x85, 0x0000);
		/* -------------- Panel Control ------------------- */
		LCD_WriteReg(0x90, 0x0010);
		LCD_WriteReg(0x92, 0x0600);
		LCD_WriteReg(0x07, 0x0133); /* 262K color and display ON */
	}
	else if(lcdid == 0x8989)
  {
		 LCD_Code = SSD1289;
	    LCD_WriteReg(0x0000,0x0001);    delay_ms(50);   /* Enable LCD Oscillator */
	    LCD_WriteReg(0x0003,0xA8A4);    delay_ms(50);   
	    LCD_WriteReg(0x000C,0x0000);    delay_ms(50);   
	    LCD_WriteReg(0x000D,0x080C);    delay_ms(50);   
	    LCD_WriteReg(0x000E,0x2B00);    delay_ms(50);   
	    LCD_WriteReg(0x001E,0x00B0);    delay_ms(50);   
	    LCD_WriteReg(0x0001,0x2B3F);    delay_ms(50);   /* 320*240 0x2B3F */
	    LCD_WriteReg(0x0002,0x0600);    delay_ms(50);
	    LCD_WriteReg(0x0010,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0011,0x6070);    delay_ms(50);
	    LCD_WriteReg(0x0005,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0006,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0016,0xEF1C);    delay_ms(50);
	    LCD_WriteReg(0x0017,0x0003);    delay_ms(50);
	    LCD_WriteReg(0x0007,0x0133);    delay_ms(50);         
	    LCD_WriteReg(0x000B,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x000F,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0041,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0042,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0048,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0049,0x013F);    delay_ms(50);
	    LCD_WriteReg(0x004A,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x004B,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0044,0xEF00);    delay_ms(50);
	    LCD_WriteReg(0x0045,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0046,0x013F);    delay_ms(50);
	    LCD_WriteReg(0x0030,0x0707);    delay_ms(50);
	    LCD_WriteReg(0x0031,0x0204);    delay_ms(50);
	    LCD_WriteReg(0x0032,0x0204);    delay_ms(50);
	    LCD_WriteReg(0x0033,0x0502);    delay_ms(50);
	    LCD_WriteReg(0x0034,0x0507);    delay_ms(50);
	    LCD_WriteReg(0x0035,0x0204);    delay_ms(50);
	    LCD_WriteReg(0x0036,0x0204);    delay_ms(50);
	    LCD_WriteReg(0x0037,0x0502);    delay_ms(50);
	    LCD_WriteReg(0x003A,0x0302);    delay_ms(50);
	    LCD_WriteReg(0x003B,0x0302);    delay_ms(50);
	    LCD_WriteReg(0x0023,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0024,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0025,0x8000);    delay_ms(50);
	    LCD_WriteReg(0x004f,0);
	    LCD_WriteReg(0x004e,0);

		LCD_WriteReg(0x0011,0x6058);
	}
  LCD_SetFont(&LCD_DEFAULT_FONT);
}

/**
  * @brief  Sets the LCD Text and Background colors.
  * @param  _TextColor: specifies the Text Color.
  * @param  _BackColor: specifies the Background Color.
  * @retval None
  */
void LCD_SetColors(__IO uint16_t _TextColor, __IO uint16_t _BackColor)
{
  TextColor = _TextColor; 
  BackColor = _BackColor;
}

/**
  * @brief  Gets the LCD Text and Background colors.
  * @param  _TextColor: pointer to the variable that will contain the Text 
            Color.
  * @param  _BackColor: pointer to the variable that will contain the Background 
            Color.
  * @retval None
  */
void LCD_GetColors(__IO uint16_t *_TextColor, __IO uint16_t *_BackColor)
{
  *_TextColor = TextColor; *_BackColor = BackColor;
}

/**
  * @brief  Sets the Text color.
  * @param  Color: specifies the Text color code RGB(5-6-5).
  * @retval None
  */
void LCD_SetTextColor(__IO uint16_t Color)
{
  TextColor = Color;
}


/**
  * @brief  Sets the Background color.
  * @param  Color: specifies the Background color code RGB(5-6-5).
  * @retval None
  */
void LCD_SetBackColor(__IO uint16_t Color)
{
  BackColor = Color;
}

/**
  * @brief  Sets the Text Font.
  * @param  fonts: specifies the font to be used.
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts;
}

/**
  * @brief  Gets the Text Font.
  * @param  None.
  * @retval the used font.
  */
sFONT *LCD_GetFont(void)
{
  return LCD_Currentfonts;
}

/**
  * @brief  Clears the selected line.
  * @param  Line: the Line to be cleared.
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..n
  * @retval None
  */
void LCD_ClearLine(uint16_t Line)
{
  uint16_t refcolumn = LCD_PIXEL_WIDTH - 1;
  /* Send the string character by character on lCD */
  while (((refcolumn + 1)& 0xFFFF) >= LCD_Currentfonts->Width)
  {
    /* Display one character on LCD */
    LCD_DisplayChar(Line, refcolumn, ' ');
    /* Decrement the column position by 16 */
    refcolumn -= LCD_Currentfonts->Width;
  }
}

/**
  * @brief  Clears the hole LCD.
  * @param  Color: the color of the background.
  * @retval None
  */
void LCD_Clear(uint16_t Color)
{
  uint32_t index = 0;
  
  LCD_SetCursor(0x00, 0x013F); 
  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
  for(index = 0; index < 76800; index++)
  {
    LCD_RAM = Color;
  }  
}

/**
  * @brief  Sets the cursor position.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position. 
  * @retval None
  */
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
//    #if  ( DISP_ORIENTATION == 90 ) || ( DISP_ORIENTATION == 270 )
// 	
//  	uint16_t temp = Xpos;

// 			 Xpos = Ypos;
// 			 Ypos = ( MAX_X - 1 ) - temp;  

// 	#elif  ( DISP_ORIENTATION == 0 ) || ( DISP_ORIENTATION == 180 )
// 		
// 	#endif
	
// 	LCD_WriteReg(0x004e, Xpos );      
// 	LCD_WriteReg(0x004f, Ypos ); 
	
		if (LCD_Code == SSD1289)
	{
	
			 LCD_WriteReg(0x004e, Xpos);      
       LCD_WriteReg(0x004f, Ypos);  
	}
	else //9325
	{     
      Xpos=240-Xpos-1; 
      Ypos=320-Ypos-1;         		
			LCD_WriteReg(0x0020, Xpos);    
			LCD_WriteReg(0x0021, Ypos);    
			//LCD_WriteIndex(0x0022);
	}	
}

/**
  * @brief  Draws a character on LCD.
  * @param  Xpos: the Line where to display the character shape.
  * @param  Ypos: start column address.
  * @param  c: pointer to the character data.
  * @retval None
  */
void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint16_t *c)
{
  uint32_t index = 0, i = 0;
  uint16_t  Xaddress = 0;
  Xaddress = Xpos;
  
  LCD_SetCursor(Xaddress, Ypos);
  
  for(index = 0; index < LCD_Currentfonts->Height; index++)
  {
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    for(i = 0; i < LCD_Currentfonts->Width; i++)
    {
  
      if((((c[index] & ((0x80 << ((LCD_Currentfonts->Width / 12 ) * 8 ) ) >> i)) == 0x00) &&(LCD_Currentfonts->Width <= 12))||
        (((c[index] & (0x1 << i)) == 0x00)&&(LCD_Currentfonts->Width > 12 )))

      {
        LCD_WriteRAM(BackColor);
      }
      else
      {
        LCD_WriteRAM(TextColor);
      } 
    }
    Xaddress++;
    LCD_SetCursor(Xaddress, Ypos);
  }
}

/**
  * @brief  Displays one character (16dots width, 24dots height).
  * @param  Line: the Line where to display the character shape .
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..9
  * @param  Column: start column address.
  * @param  Ascii: character ascii code, must be between 0x20 and 0x7E.
  * @retval None
  */
void LCD_DisplayChar(uint16_t Line, uint16_t Column, uint8_t Ascii)
{
  Ascii -= 32;
  LCD_DrawChar(Line, Column, &LCD_Currentfonts->table[Ascii * LCD_Currentfonts->Height]);
}

/**
  * @brief  Displays a maximum of 20 char on the LCD.
  * @param  Line: the Line where to display the character shape .
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..9
  * @param  *ptr: pointer to string to display on LCD.
  * @retval None
  */
void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr)
{
  uint16_t refcolumn = LCD_PIXEL_WIDTH - 1;

  /* Send the string character by character on lCD */
  while ((*ptr != 0) & (((refcolumn + 1) & 0xFFFF) >= LCD_Currentfonts->Width))
  {
    /* Display one character on LCD */
    LCD_DisplayChar(Line, refcolumn, *ptr);
    /* Decrement the column position by 16 */
    refcolumn -= LCD_Currentfonts->Width;
    /* Point on the next character */
    ptr++;
  }
}


/**
  * @brief  Displays a line.
  * @param Xpos: specifies the X position.
  * @param Ypos: specifies the Y position.
  * @param Length: line length.
  * @param Direction: line direction.
  *   This parameter can be one of the following values: Vertical or Horizontal.
  * @retval None
  */
void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction)
{
  uint32_t i = 0;
  
  LCD_SetCursor(Xpos, Ypos);
  if(Direction == LCD_DIR_HORIZONTAL)
  {
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    for(i = 0; i < Length; i++)
    {
      LCD_WriteRAM(TextColor);
    }
  }
  else
  {
    for(i = 0; i < Length; i++)
    {
      LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
      LCD_WriteRAM(TextColor);
      Xpos++;
      LCD_SetCursor(Xpos, Ypos);
    }
  }
}

/**
  * @brief  Displays a rectangle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Height: display rectangle height.
  * @param  Width: display rectangle width.
  * @retval None
  */
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width)
{
  LCD_DrawLine(Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);
  LCD_DrawLine((Xpos + Height), Ypos, Width, LCD_DIR_HORIZONTAL);
  
  LCD_DrawLine(Xpos, Ypos, Height, LCD_DIR_VERTICAL);
  LCD_DrawLine(Xpos, (Ypos - Width + 1), Height, LCD_DIR_VERTICAL);
}

/**
  * @brief  Displays a circle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Radius
  * @retval None
  */
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t  D;/* Decision Variable */ 
  uint32_t  CurX;/* Current X Value */
  uint32_t  CurY;/* Current Y Value */ 
  
  D = 3 - (Radius << 1);
  CurX = 0;
  CurY = Radius;
  
  while (CurX <= CurY)
  {
    LCD_SetCursor(Xpos + CurX, Ypos + CurY);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos + CurX, Ypos - CurY);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos - CurX, Ypos + CurY);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos - CurX, Ypos - CurY);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos + CurY, Ypos + CurX);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos + CurY, Ypos - CurX);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos - CurY, Ypos + CurX);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos - CurY, Ypos - CurX);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    if (D < 0)
    { 
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
}

/**
  * @brief  Displays a mono-color picture.
  * @param  Pict: pointer to the picture array.
  * @retval None
  */
void LCD_DrawMonoPict(const uint32_t *Pict)
{
  uint32_t index = 0, i = 0;
  LCD_SetCursor(0, (LCD_PIXEL_WIDTH - 1)); 
  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
  for(index = 0; index < 2400; index++)
  {
    for(i = 0; i < 32; i++)
    {
      if((Pict[index] & (1 << i)) == 0x00)
      {
        LCD_WriteRAM(BackColor);
      }
      else
      {
        LCD_WriteRAM(TextColor);
      }
    }
  }
}

/**
  * @brief  Displays a bitmap picture loaded in the internal Flash.
  * @param  BmpAddress: Bmp picture address in the internal Flash.
  * @retval None
  */
void LCD_WriteBMP(uint32_t BmpAddress)
{
  uint32_t index = 0, size = 0;
  /* Read bitmap size */
  size = *(__IO uint16_t *) (BmpAddress + 2);
  size |= (*(__IO uint16_t *) (BmpAddress + 4)) << 16;
  /* Get bitmap data address offset */
  index = *(__IO uint16_t *) (BmpAddress + 10);
  index |= (*(__IO uint16_t *) (BmpAddress + 12)) << 16;
  size = (size - index)/2;
  BmpAddress += index;
  /* Set GRAM write direction and BGR = 1 */
  /* I/D=00 (Horizontal : decrement, Vertical : decrement) */
  /* AM=1 (address is updated in vertical writing direction) */
  //LCD_WriteReg(LCD_REG_3, 0x1008);
 
  LCD_WriteRAM_Prepare();
 
  for(index = 0; index < size; index++)
  {
    LCD_WriteRAM(*(__IO uint16_t *)BmpAddress);
    BmpAddress += 2;
  }
 
  /* Set GRAM write direction and BGR = 1 */
  /* I/D = 01 (Horizontal : increment, Vertical : decrement) */
  /* AM = 1 (address is updated in vertical writing direction) */
  //LCD_WriteReg(LCD_REG_3, 0x1018);
}

/**
  * @brief  Displays a full rectangle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Height: rectangle height.
  * @param  Width: rectangle width.
  * @retval None
  */
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  LCD_SetTextColor(TextColor);

  LCD_DrawLine(Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);
  LCD_DrawLine((Xpos + Height), Ypos, Width, LCD_DIR_HORIZONTAL);
  
  LCD_DrawLine(Xpos, Ypos, Height, LCD_DIR_VERTICAL);
  LCD_DrawLine(Xpos, (Ypos - Width + 1), Height, LCD_DIR_VERTICAL);

  Width -= 2;
  Height--;
  Ypos--;

  LCD_SetTextColor(BackColor);

  while(Height--)
  {
    LCD_DrawLine(++Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);    
  }

  LCD_SetTextColor(TextColor);
}

/**
  * @brief  Displays a full circle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Radius
  * @retval None
  */
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t  D;    /* Decision Variable */ 
  uint32_t  CurX;/* Current X Value */
  uint32_t  CurY;/* Current Y Value */ 
  
  D = 3 - (Radius << 1);

  CurX = 0;
  CurY = Radius;
  
  LCD_SetTextColor(BackColor);

  while (CurX <= CurY)
  {
    if(CurY > 0) 
    {
      LCD_DrawLine(Xpos - CurX, Ypos + CurY, 2*CurY, LCD_DIR_HORIZONTAL);
      LCD_DrawLine(Xpos + CurX, Ypos + CurY, 2*CurY, LCD_DIR_HORIZONTAL);
    }

    if(CurX > 0) 
    {
      LCD_DrawLine(Xpos - CurY, Ypos + CurX, 2*CurX, LCD_DIR_HORIZONTAL);
      LCD_DrawLine(Xpos + CurY, Ypos + CurX, 2*CurX, LCD_DIR_HORIZONTAL);
    }
    if (D < 0)
    { 
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }

  LCD_SetTextColor(TextColor);
  LCD_DrawCircle(Xpos, Ypos, Radius);
}

/**
  * @brief  Displays an uni-line (between two points).
  * @param  x1: specifies the point 1 x position.
  * @param  y1: specifies the point 1 y position.
  * @param  x2: specifies the point 2 x position.
  * @param  y2: specifies the point 2 y position.
  * @retval None
  */
void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
  curpixel = 0;
  
  deltax = ABS(x2 - x1);        /* The difference between the x's */
  deltay = ABS(y2 - y1);        /* The difference between the y's */
  x = x1;                       /* Start x off at the first pixel */
  y = y1;                       /* Start y off at the first pixel */
  
  if (x2 >= x1)                 /* The x-values are increasing */
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* The x-values are decreasing */
  {
    xinc1 = -1;
    xinc2 = -1;
  }
  
  if (y2 >= y1)                 /* The y-values are increasing */
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* The y-values are decreasing */
  {
    yinc1 = -1;
    yinc2 = -1;
  }
  
  if (deltax >= deltay)         /* There is at least one x-value for every y-value */
  {
    xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
    yinc2 = 0;                  /* Don't change the y for every iteration */
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;         /* There are more x-values than y-values */
  }
  else                          /* There is at least one y-value for every x-value */
  {
    xinc2 = 0;                  /* Don't change the x for every iteration */
    yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;         /* There are more y-values than x-values */
  }
  
  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    PutPixel(x, y);             /* Draw the current pixel */
    num += numadd;              /* Increase the numerator by the top of the fraction */
    if (num >= den)             /* Check if numerator >= denominator */
    {
      num -= den;               /* Calculate the new numerator value */
      x += xinc1;               /* Change the x as appropriate */
      y += yinc1;               /* Change the y as appropriate */
    }
    x += xinc2;                 /* Change the x as appropriate */
    y += yinc2;                 /* Change the y as appropriate */
  }
}

/**
  * @brief  Displays an poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_PolyLine(pPoint Points, uint16_t PointCount)
{
  int16_t X = 0, Y = 0;

  if(PointCount < 2)
  {
    return;
  }

  while(--PointCount)
  {
    X = Points->X;
    Y = Points->Y;
    Points++;
    LCD_DrawUniLine(X, Y, Points->X, Points->Y);
  }
}

/**
  * @brief  Displays an relative poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @param  Closed: specifies if the draw is closed or not.
  *           1: closed, 0 : not closed.
  * @retval None
  */
static void LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount, uint16_t Closed)
{
  int16_t X = 0, Y = 0;
  pPoint First = Points;

  if(PointCount < 2)
  {
    return;
  }  
  X = Points->X;
  Y = Points->Y;
  while(--PointCount)
  {
    Points++;
    LCD_DrawUniLine(X, Y, X + Points->X, Y + Points->Y);
    X = X + Points->X;
    Y = Y + Points->Y;
  }
  if(Closed)
  {
    LCD_DrawUniLine(First->X, First->Y, X, Y);
  }  
}

/**
  * @brief  Displays a closed poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_ClosedPolyLine(pPoint Points, uint16_t PointCount)
{
  LCD_PolyLine(Points, PointCount);
  LCD_DrawUniLine(Points->X, Points->Y, (Points+PointCount-1)->X, (Points+PointCount-1)->Y);
}

/**
  * @brief  Displays a relative poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_PolyLineRelative(pPoint Points, uint16_t PointCount)
{
  LCD_PolyLineRelativeClosed(Points, PointCount, 0);
}

/**
  * @brief  Displays a closed relative poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_ClosedPolyLineRelative(pPoint Points, uint16_t PointCount)
{
  LCD_PolyLineRelativeClosed(Points, PointCount, 1);
}


/**
  * @brief  Displays a  full poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_FillPolyLine(pPoint Points, uint16_t PointCount)
{
  /*  public-domain code by Darel Rex Finley, 2007 */
  uint16_t  nodes = 0, nodeX[MAX_POLY_CORNERS], pixelX = 0, pixelY = 0, i = 0,
  j = 0, swap = 0;
  uint16_t  IMAGE_LEFT = 0, IMAGE_RIGHT = 0, IMAGE_TOP = 0, IMAGE_BOTTOM = 0;

  IMAGE_LEFT = IMAGE_RIGHT = Points->X;
  IMAGE_TOP= IMAGE_BOTTOM = Points->Y;

  for(i = 1; i < PointCount; i++)
  {
    pixelX = POLY_X(i);
    if(pixelX < IMAGE_LEFT)
    {
      IMAGE_LEFT = pixelX;
    }
    if(pixelX > IMAGE_RIGHT)
    {
      IMAGE_RIGHT = pixelX;
    }
    
    pixelY = POLY_Y(i);
    if(pixelY < IMAGE_TOP)
    { 
      IMAGE_TOP = pixelY;
    }
    if(pixelY > IMAGE_BOTTOM)
    {
      IMAGE_BOTTOM = pixelY;
    }
  }
  
  LCD_SetTextColor(BackColor);  

  /*  Loop through the rows of the image. */
  for (pixelY = IMAGE_TOP; pixelY < IMAGE_BOTTOM; pixelY++) 
  {  
    /* Build a list of nodes. */
    nodes = 0; j = PointCount-1;

    for (i = 0; i < PointCount; i++) 
    {
      if (((POLY_Y(i)<(double) pixelY) && (POLY_Y(j)>=(double) pixelY)) || \
          ((POLY_Y(j)<(double) pixelY) && (POLY_Y(i)>=(double) pixelY)))
      {
        nodeX[nodes++]=(int) (POLY_X(i)+((pixelY-POLY_Y(i))*(POLY_X(j)-POLY_X(i)))/(POLY_Y(j)-POLY_Y(i))); 
      }
      j = i; 
    }
  
    /* Sort the nodes, via a simple "Bubble" sort. */
    i = 0;
    while (i < nodes-1) 
    {
      if (nodeX[i]>nodeX[i+1]) 
      {
        swap = nodeX[i]; 
        nodeX[i] = nodeX[i+1]; 
        nodeX[i+1] = swap; 
        if(i)
        {
          i--; 
        }
      }
      else 
      {
        i++;
      }
    }
  
    /*  Fill the pixels between node pairs. */
    for (i = 0; i < nodes; i+=2) 
    {
      if(nodeX[i] >= IMAGE_RIGHT) 
      {
        break;
      }
      if(nodeX[i+1] > IMAGE_LEFT) 
      {
        if (nodeX[i] < IMAGE_LEFT)
        {
          nodeX[i]=IMAGE_LEFT;
        }
        if(nodeX[i+1] > IMAGE_RIGHT)
        {
          nodeX[i+1] = IMAGE_RIGHT;
        }
        LCD_SetTextColor(BackColor);
        LCD_DrawLine(pixelY, nodeX[i+1], nodeX[i+1] - nodeX[i], LCD_DIR_HORIZONTAL);
        LCD_SetTextColor(TextColor);
        PutPixel(pixelY, nodeX[i+1]);
        PutPixel(pixelY, nodeX[i]);
        /* for (j=nodeX[i]; j<nodeX[i+1]; j++) PutPixel(j,pixelY); */
      }
    }
  } 

  /* draw the edges */
  LCD_SetTextColor(TextColor);
}

/**
  * @brief  Writes to the selected LCD register.
  * @param  LCD_Reg: address of the selected register.
  * @param  LCD_RegValue: value to write to the selected register.
  * @retval None
  */
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
  /* Write 16-bit Index, then Write Reg */
  LCD_REG = LCD_Reg;
  /* Write 16-bit Reg */
  LCD_RAM = LCD_RegValue;
}

/**
  * @brief  Reads the selected LCD Register.
  * @param  LCD_Reg: address of the selected register.
  * @retval LCD Register Value.
  */
uint16_t LCD_ReadReg(uint8_t LCD_Reg)
{
	/* Write 16-bit Index (then Read Reg) */
	LCD_REG = LCD_Reg;

	/* Read 16-bit Reg */
	return LCD_RAM;
}

/**
  * @brief  Prepare to write to the LCD RAM.
  * @param  None
  * @retval None
  */
void LCD_WriteRAM_Prepare(void)
{
  LCD_REG = 0x0022;
}

/**
  * @brief  Writes to the LCD RAM.
  * @param  RGB_Code: the pixel color in RGB mode (5-6-5).
  * @retval None
  */
void LCD_WriteRAM(uint16_t RGB_Code)
{
  /* Write 16-bit GRAM Reg */
  LCD_RAM = RGB_Code;
}

/**
  * @brief  Reads the LCD RAM.
  * @param  None
  * @retval LCD RAM Value.
  */
uint16_t LCD_ReadRAM(void)
{
  /* Read 16-bit Reg */
  return LCD_RAM;
}

/**
  * @brief  Configures LCD Control lines (FSMC Pins) in alternate function mode.
  * @param  None
  * @retval None
  */
void LCD_CtrlLinesConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	
	/* Enable GPIOs clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE , ENABLE);
	
	/* Enable FSMC clock */
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE); 

	/*-- GPIOs Configuration ------------------------------------------------------*/
	/*
	+-------------------+--------------------+------------------+------------------+
	+                       SRAM pins assignment                                   +

	+-------------------+--------------------+
	*/
	/* GPIOD configuration */
		/* Set PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
	 PD.10(D15), PD.11(A16), PD.14(D0), PD.15(D1) as alternate function push pull */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | 
	                            GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
								 GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
	 PE.14(D11), PE.15(D12) as alternate function push pull */	
	/* GPIOE configuration */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2 | GPIO_Pin_7 | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 |
	                             GPIO_Pin_11| GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOD,GPIO_Pin_7);
}

/**
  * @brief  Configures the Parallel interface (FSMC) for LCD(Parallel mode)
  * @param  None
  * @retval None
  */
void LCD_FSMCConfig(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAMTimingInitStructure;
	
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;
//	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 


	/* FSMC写速度设置 */
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 15;//1;   /* 地址建立时间  */
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 0;	   
	FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 15;//1;	   /* 数据建立时间  */
	FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	/* FSMC 访问模式 */
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;	  
	
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
	
	/* Enable FSMC Bank4_SRAM Bank */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  
}

/**
  * @brief  Displays a pixel.
  * @param  x: pixel x.
  * @param  y: pixel y.  
  * @retval None
  */
static void PutPixel(int16_t x, int16_t y)
{ 
  if(x < 0 || x > 239 || y < 0 || y > 319)
  {
    return;  
  }
  LCD_DrawLine(x, y, 1, LCD_DIR_HORIZONTAL);
}


#ifndef USE_Delay
/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
static void delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0; 
  for(index = (100000 * nCount); index != 0; index--)
  {
  }
}
#endif /* USE_Delay*/
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/**
  * @}
  */  

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
