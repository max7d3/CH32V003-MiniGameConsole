#ifndef _ST7735_H_
#define _ST7735_H_

/*
 *	ST7735 driver for CH32V003
 *	V 0.0.1
 */


#define LCD_WIDTH 160
#define LCD_HEIGHT 80
#define LCD_DEFAULT_ROTATION 3

#define INVON
//#define INVOFF

#define OFFSET_1_26
//#define OFFSET_0_24

#define COLOR_ORDER_BGR
//#define COLOR_ORDER_RGB

#define LCD_CS_HIGH GPIOC->BSHR |= GPIO_BSHR_BS2
#define LCD_CS_LOW GPIOC->BSHR |= GPIO_BSHR_BR2

#define LCD_DC_HIGH GPIOC->BSHR |= GPIO_BSHR_BS4
#define LCD_DC_LOW GPIOC->BSHR |= GPIO_BSHR_BR4

#define LCD_RS_HIGH GPIOC->BSHR |= GPIO_BSHR_BS3
#define LCD_RS_LOW GPIOC->BSHR |= GPIO_BSHR_BR3

#define ST7735_XSTART_H     0x00
#define ST7735_XSTART_L     0x1A
#define ST7735_YSTART_H     0x00
#define ST7735_YSTART_L     0x01
#define ST7735_XEND_H       0x00
#define ST7735_XEND_L       0x69
#define ST7735_YEND_H       0x00
#define ST7735_YEND_L       0xA0
#define ST7735_16BIT        0x05

#define ST7735_NOP 	  		0x00
#define ST7735_SWRESET 		0x01
#define ST7735_RDDID  		0x04
#define ST7735_RDDST  		0x09

#define ST7735_SLPIN  		0x10
#define ST7735_SLPOUT  		0x11
#define ST7735_PTLON   		0x12
#define ST7735_NORON   		0x13

#define ST7735_INVOFF 		0x20
#define ST7735_INVON  		0x21
#define ST7735_DISPOFF 		0x28
#define ST7735_DISPON 		0x29
#define ST7735_CASET 		0x2A
#define ST7735_RASET 		0x2B
#define ST7735_RAMWR 		0x2C
#define ST7735_RAMRD 		0x2E
#define ST7735_VSCRDEF 		0x33
#define ST7735_VSCSAD 		0x37

#define ST7735_COLMOD 		0x3A
#define ST7735_MADCTL 		0x36

#define ST7735_FRMCTR1 		0xB1
#define ST7735_FRMCTR2 		0xB2
#define ST7735_FRMCTR3 		0xB3
#define ST7735_INVCTR 		0xB4
#define ST7735_DISSET5 		0xB6

#define ST7735_PWCTR1 		0xC0
#define ST7735_PWCTR2 		0xC1
#define ST7735_PWCTR3 		0xC2
#define ST7735_PWCTR4 		0xC3
#define ST7735_PWCTR5 		0xC4
#define ST7735_VMCTR1 		0xC5

#define ST7735_RDID1 		0xDA
#define ST7735_RDID2 		0xDB
#define ST7735_RDID3 		0xDC
#define ST7735_RDID4 		0xDD

#define ST7735_PWCTR6 		0xFC

#define ST7735_GMCTRP1 		0xE0
#define ST7735_GMCTRN1 		0xE1

#define ST7735_BLACK 0x0000
#define ST7735_WHITE 0xFFFF
#define ST7735_RED 0xF800
#define ST7735_GREEN 0x07E0
#define ST7735_BLUE 0x001F
#define ST7735_CYAN 0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW 0xFFE0
#define ST7735_ORANGE 0xFC00

typedef struct
{
    uint8_t command;
    const uint8_t *args;
    uint8_t arg_count;

}lcd_command_t;

void LCD_Init(void);
void LCD_SetRotation(uint8_t rotation);
void LCD_SetWindow(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h);
void LCD_WriteRawData(uint16_t *data, uint16_t size);
void LCD_WriteRawDataDMA(uint16_t *data, uint16_t size);
void LCD_DMA_Wait(void);
void LCD_OptimizedFill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color);

#endif /* _ST7735_H_ */
